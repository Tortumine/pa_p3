//
// Created by tortumine on 19/12/17.
//
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "PNM.h"

/*--------------------------------------------------------------------
* Computes the energy of a PNM image pixel by using the image gradient
* Used for seam carving technique
*
* PARAMETERS
* image     Pointer to a PNM image
* i         position of the pixel in the array
* 
* RETURN
* energy    the energy of the pixel
*----------------------------------------------------------------------*/
size_t pixelEnergy(const PNMImage* image, size_t i);

/*--------------------------------------------------------------------
* Generates an array containing the minimal energy seam energy for each pixel of a PNM image
*
* PARAMETERS
* image     Pointer to a PNM image
* energies  Pointer to an array containing minimal enrgy seam for each pixel
* moves     Pointer to an array containing for every pixel direction to the previous pixel in the seam
*           Three values possible : -1=left, 0=center, 1=right
* RETURN
* energies    the array containing the seams energies
*----------------------------------------------------------------------*/
void seams(const PNMImage* image, size_t* energies, int* moves);

/*--------------------------------------------------------------------
* Select the seam with least energy in the array and saves indexes relevant pixels in selectedSeam
*
* PARAMETERS
* heigth        Heigth of the image
* width        width of the image
* energies     Pointer to an array containing minimal enrgy seam for each pixel
* moves     Pointer to an array containing for every pixel direction to the previous pixel in the seam
* selectedSeam Array containing indexes of pixels involved in the selected seam. For row 0 to row n

*----------------------------------------------------------------------*/
void selectSeam(size_t heigth,size_t width, size_t* energies, int* moves, size_t* selectedSeam);

size_t pixelEnergy(const PNMImage* image, size_t i)
{
    size_t energy = 0;
    size_t right,left,bottom,up;

    //positions of neighbour pixels
    if(i%image->width > 0)
        left=i-1;
    else
        left=i;
    if(i%image->width < image->width-1)
        right=i+1;
    else
        right=i;
    if(i/(image->width) > 0)
        up=i-image->width;
    else
        up=i;
    if(i/image->width < image->height-1)
        bottom=i+image->width;
    else
        bottom=i;
    //red
    energy += (size_t)(abs(image->data[left].red - image->data[right].red)/2 
                + abs(image->data[up].red - image->data[bottom].red)/2);
    //green 
    energy += (size_t)(abs(image->data[left].green - image->data[right].green)/2 
                + abs(image->data[up].green - image->data[bottom].green)/2);
    //blue
    energy += (size_t)(abs(image->data[left].blue - image->data[right].blue)/2 
                + abs(image->data[up].blue - image->data[bottom].blue)/2);
    return energy;
}

void seams(const PNMImage* image, size_t* energies, int* moves)
{
    size_t left, center, right;
    for(size_t i=0; i < image->height;i++)
        for(size_t j=0;j<image->width;j++)
        {
            //case when we are in the first row
            if(i==0)
            {
                energies[i*image->width + j]=pixelEnergy(image,i*image->width + j);
                moves[i*image->width + j]=0;
            }
            else
            {
                //selection of the seam to follow
                if(j>0)
                    left = energies[(i-1)*image->width + j-1];
                else
                    left= 42949;

                center = energies[(i-1)*image->width + j];

                if(j<image->width-1)
                    right = energies[(i-1)*image->width + j+1];
                else
                    right = 42949;

                //int tmp = (left < center) ? left : center;
                int tmp;
                if(left < center)
                     tmp=left;
                else
                     tmp=right;
                int min;
                if(tmp < right)
                     min=tmp;
                else
                     min=right;
                //int min = (tmp < right) ? tmp : right;

                //fill arrays "moves" and "energies" on base of selected seam
                //from (i,j) gives the move to go to (i-1,?)
                if(min == left)
                    moves[i*image->width + j] = -1;
                else if(min == center)
                    moves[i*image->width + j] = 0;
                else
                    moves[i*image->width + j] = 1;

                energies[i*image->width + j] = min + pixelEnergy(image,i*image->width + j);
            }
        }
}

void selectSeam(size_t heigth, size_t width, size_t* energies, int* moves, size_t* selectedSeam)
{
    size_t start=0;//offset of starting pixel, at the bottom row of the image
    size_t k=heigth-1;//index in the slectedSeam array
    for(int j=0;j < width;j++)
    {
        if(energies[(heigth-1)*width + j] < energies[(heigth-1)*width + start])
            start=j;
    }
    selectedSeam[k]=start;
    k--;
    for(int i=heigth-2;i>=0; i--)
    {
        if(moves[(i+1)*width + start] == -1)
            start --;
        else if(moves[(i+1)*width + start] == 1)
            start ++;
        selectedSeam[k]=start;
        k--;
    }
}
PNMImage* reduceImageWidth(const PNMImage* image, size_t k)
{
    size_t* energies;
    int* moves;
    size_t* selectedSeam;
    PNMImage* tmp;
    energies = malloc(image->height*image->width*sizeof(size_t));
    if (energies==NULL)
        printf("Erreur d'allocation pour energies\n");

    moves = malloc(image->height*image->width*sizeof(int));
    if (moves==NULL)
        printf("Erreur d'allocation pour moves\n");

    selectedSeam = malloc(image->height*sizeof(size_t));
    if (selectedSeam==NULL)
        printf("Erreur d'allocation pour selectedSeam\n");

    PNMImage* new = createPNM(image->width, image->height);

    for(size_t i=0; i < image->height;i++)
        for(size_t j=0;j<image->width;j++)
            new->data[i*image->width+j] = image->data[i*image->width+j];
    for(size_t l=0; l<k; l++)
    {
        seams(new, energies, moves);
        selectSeam(new->height, new->width, energies, moves, selectedSeam);
        tmp = createPNM(new->width-1, new->height);
        size_t jP;
        for(size_t i=0; i<new->height;i++)
        {
            jP=0;
            for(size_t j=0; j<tmp->width;j++)
            {
                if(j == selectedSeam[i])
                    jP++;
                tmp->data[i*tmp->width + j] = new->data[i*new->width + jP];
                jP++;   
            }
        }  
        freePNM(new);
        new = tmp;
    }
    free(energies);
    free(moves);
    free(selectedSeam);

    return new;
}


PNMImage* increaseImageWidth(const PNMImage* image, size_t k)
{
    size_t* energies;
    int* moves;
    size_t* selectedSeam;
    PNMImage* tmp;
    energies = malloc(image->height*image->width*sizeof(size_t));
    if (energies==NULL)
        printf("Erreur d'allocation pour energies\n");

    moves = malloc(image->height*image->width*sizeof(int));
    if (moves==NULL)
        printf("Erreur d'allocation pour moves\n");

    selectedSeam = malloc(image->height*sizeof(size_t));
    if (selectedSeam==NULL)
        printf("Erreur d'allocation pour selectedSeam\n");

    PNMImage* new = createPNM(image->width, image->height);

    for(int i=0; i<k; i++)
    {
        seams(new, energies, moves);
        selectSeam(new->height, new->width, energies, moves, selectedSeam);
        
        tmp = createPNM(new->width, new->height);
    }
    return new;
}