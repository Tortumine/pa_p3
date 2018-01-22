//
// Sadzot Antoine (s173130) 
// Mazurchyk Aliaksei (s174957)
//
#include <stdlib.h>
#include <stddef.h>
#include "PNM.h"
#include "SeamCarving.h"

/*--------------------------------------------------------------------
* Computes the energy of a PNM image pixel by using the image gradient
* Used for seam carving technique
*
* PARAMETERS
* image     Pointer to a PNM image
* i         position of the pixel in the array containing the image
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
* moves     Pointer to an array containing, for every pixel, the direction to the previous pixel in the seam
*           Three values possible : -1=left, 0=center, 1=right
*----------------------------------------------------------------------*/
void seams(const PNMImage* image, size_t* energies, int* moves);
/*--------------------------------------------------------------------
* Generates an array containing the minimal energy seam energy for each pixel of a PNM image
*
* PARAMETERS
* image     Pointer to a PNM image
* energies  Pointer to an array containing minimal enrgy seam for each pixel
* moves     Pointer to an array containing, for every pixel, the direction to the previous pixel in the seam
*           Three values possible : -1=left, 0=center, 1=right
* selectedSeam Array containing the préviously created seams
* l         number of previously created seams
* kP        width of array selectedSeams
*----------------------------------------------------------------------*/
void seamsIncrease(const PNMImage* image, size_t* energies, int* moves, size_t* selectedSeam, size_t l, size_t kP);
/*--------------------------------------------------------------------
* Select the seam with least energy in the array and saves the indexes of relevant pixels in selectedSeam
*
* PARAMETERS
* heigth       Heigth of the image
* width        width of the image
* energies     Pointer to an array containing minimal energy seam for each pixel
* moves        Pointer to an array containing, for every pixel, thr direction to the previous pixel in the seam
* selectedSeam Array containing indexes of pixels involved in the selected seam. From row 0 to row n
*----------------------------------------------------------------------*/
void selectSeam(size_t heigth,size_t width, size_t* energies, int* moves, size_t* selectedSeam);
/*--------------------------------------------------------------------
* Select the seam with least energy in the array and saves the indexes of relevant pixels in selectedSeam
*
* PARAMETERS
* heigth       Heigth of the image
* width        width of the image
* energies     Pointer to an array containing minimal energy seam for each pixel
* moves        Pointer to an array containing, for every pixel, thr direction to the previous pixel in the seam
* selectedSeam Array containing indexes of pixels involved in the selected seam. From row 0 to row n
* l            Number of seams previously created
* jP           width of selectedSeam 
*----------------------------------------------------------------------*/
void selectSeamIncrease(size_t heigth,size_t width, size_t* energies, int* moves, size_t* selectedSeam, size_t l, size_t kP);
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
    
    //Computing and adding the energy for the the three colors
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
    //compute the seam with the least energy for every pixel
    for(size_t i=0; i<image->height;i++)
    {
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
                //selection of the seam to follow by compairing the three potential candidates
                //If we are out of bounds, the variables are set to a value higher than any possible
                //seam energy. On base of the height of the image 
                if(j>0)
                    left = energies[(i-1)*image->width + j-1];
                else
                    left= 429490;

                center = energies[(i-1)*image->width + j];

                if(j<image->width-1)
                    right = energies[(i-1)*image->width + j+1];
                else
                    right = 429490;

                //More compact with ternary operator?
                size_t tmp;
                if(left < center)
                     tmp=left;
                else
                     tmp=center;
                size_t min;
                if(tmp < right)
                     min=tmp;
                else
                     min=right;

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
        
}

void selectSeam(size_t heigth, size_t width, size_t* energies, int* moves, size_t* selectedSeam)
{
    size_t start=0;//offset of the pixel in the seam, for the current image row
    size_t k=heigth-1;//index in the slectedSeam array
    //select the seam on base of the last row of the image
    for(size_t j=0;j < width;j++)
    {
        if(energies[(heigth-1)*width + j] < energies[(heigth-1)*width + start])
            start=j;
    }
    selectedSeam[k]=start;
    k--;
    //for every row, starting from the bottom, add in selectedSeam the offset of the pixel
    //in the selected seam
    for(int i=(int)(heigth-2);i>=0; i--)
    {
        if(moves[(i+1)*width + start] == -1)
            start --;
        else if(moves[(i+1)*width + start] == 1)
            start ++;
        selectedSeam[k]=start;
        k--;
    }
}

void selectSeamIncrease(size_t heigth,size_t width, size_t* energies, int* moves, size_t* selectedSeam, size_t l, size_t kP)
{
    size_t start=0;//offset of the pixel in the seam, for the current image row
    size_t k=heigth-1;//index in the slectedSeam array
    //select the seam on base of the last row of the image
    for(size_t j=0;j<width;j++)
    {
        if(energies[(heigth-1)*width + j] < energies[(heigth-1)*width + start])
            start=j;
    }
    
    selectedSeam[k*kP + l]=start;
    k--;
    //for every row, starting from the bottom, add in selectedSeam the offset of the pixel
    //in the selected seam
    for(int i=(int)(heigth-2);i>=0; i--)
    {
        if(moves[(i+1)*width + start] == -1)
            start --;
        else if(moves[(i+1)*width + start] == 1)
            start ++;
        selectedSeam[k*kP + l]=start;
        k--;
    }
}

PNMImage* reduceImageWidth(const PNMImage* image, size_t k)
{
    size_t* energies;
    int* moves;
    size_t* selectedSeam;
    PNMImage* tmp;
    //array containing the position of the pixels visited by the selected seam for every row of the image
    selectedSeam = malloc(image->height*sizeof(size_t));
    //Current image
    PNMImage* new = createPNM(image->width, image->height);
    for(size_t i=0; i < image->height;i++)
        for(size_t j=0;j<image->width;j++)
            new->data[i*image->width+j] = image->data[i*image->width+j];
    //loop corresponding to the number of pixels in width to reduce
    for(size_t l=0; l<k; l++)
    {
        //array containing the vertical energy seam for every pixel
        energies = malloc(image->height*image->width*sizeof(size_t));
        //array containing the moves to follow any seam 
        moves = malloc(image->height*image->width*sizeof(int));

        seams(new, energies, moves);
        selectSeam(new->height, new->width, energies, moves, selectedSeam);
        //temporary image of width new->width-1
        tmp = createPNM(new->width-1, new->height);
        size_t jP;
        //create the new image of width -1.
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
        free(energies);
        free(moves);
        new = tmp;//the tmp image becomes the new current image
    }  
    free(selectedSeam);
    return new;
}

void seamsIncrease(const PNMImage* image, size_t* energies, int* moves, size_t* selectedSeam, size_t l, size_t kP)
{
    size_t left, center, right;
    //size_t BiggestEnergy = 762*(image->height+1);
    size_t BiggestEnergy = 762*image->height;
    //compute the seam with the least energy for every pixel
    for(size_t i=0; i<image->height;i++)
    {
        for(size_t j=0;j<image->width;j++)
        {
            //case when we are in the first row
            if(i==0)
            {
                energies[i*image->width + j]=pixelEnergy(image,i*image->width + j);     
                for(size_t o=0;o<l;o++)
                {
                    if(j == selectedSeam[o])
                    {
                        energies[j]+=BiggestEnergy;
                    }    
                }
                moves[i*image->width + j]=0;
                
            }
            else
            {
                //we verify if the pixel visited is part of a seam
                //If so, the pixel is increased by an enormous value
                for(size_t o=0;o<l;o++)
                {
                    if(j>0 && (j-1) == selectedSeam[(i-1)*kP+o])
                        energies[(i-1)*image->width + j-1]+=BiggestEnergy;
                    if(j == selectedSeam[(i-1)*kP+o]) 
                        energies[(i-1)*image->width + j]+=BiggestEnergy;
                    if(j<(image->width-1) && (j+1) == selectedSeam[(i-1)*kP+o])
                        energies[(i-1)*image->width + j+1]+=BiggestEnergy;
                }              
                //selection of the seam to follow by compairing the three potential candidates
                //If we are out of bounds, the variables are set to a value higher than any possible
                //seam energy. On base of the height of the image 762 is the max pixel energy one could compute
                if(j>0)
                    left = energies[(i-1)*image->width + j-1];
                else
                    left= BiggestEnergy*BiggestEnergy;

                center = energies[(i-1)*image->width + j];

                if(j<image->width-1)
                    right = energies[(i-1)*image->width + j+1];
                else
                    right = BiggestEnergy*BiggestEnergy;

                //More compact with ternary operator?
                size_t tmp;
                if(left < center)
                    tmp=left;
                else
                    tmp=center;
                size_t min;
                if(tmp < right)
                    min=tmp;
                else
                    min=right;

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
        
}

PNMImage* increaseImageWidth(const PNMImage* image, size_t k)
{
    size_t* energies;
    int* moves;
    size_t* selectedSeam;
    PNMImage* tmp;
    PNMImage* definitive;//image qui sera renvoyée
    
    //Current image
    PNMImage* new = createPNM(image->width, image->height);
    for(size_t i=0; i < image->height;i++)
        for(size_t j=0;j<image->width;j++)
            new->data[i*image->width+j] = image->data[i*image->width+j];
    //if k<20% of width we do that in one iteration
    size_t kP = image->width/5;
    if (k<=kP)
        kP=k;
    //loop repeated a number of times = k/kP
    for(int o=(int)k;o>0;o-=(int)kP)
    {
        definitive= createPNM(new->width, new->height);
        for(size_t i=0;i<new->height;i++)
            for(size_t j=0;j<new->width;j++)
                definitive->data[i*definitive->width+j] = new->data[i*new->width+j];
        //array containing the vertical energy seam for every pixel
            energies = malloc(definitive->height*definitive->width*sizeof(size_t));
            //array containing the moves to follow any seam
            moves = malloc(definitive->height*definitive->width*sizeof(int));

        if(o < (int)kP)
            kP=o;

        //array containing the position of the pixels visited by the selected seam for every row of the image
            selectedSeam = malloc(definitive->height*kP*sizeof(size_t));

        //loop corresponding to the number of pixels in width to increase max 20% of original picture
        size_t l;
        for(l=0; l<kP; l++)
        {
            seamsIncrease(definitive, energies, moves, selectedSeam, l, kP);
            selectSeamIncrease(definitive->height, definitive->width, energies, moves, selectedSeam,l,kP);         
        }
        tmp = createPNM(new->width+l, new->height);
        size_t jP;
        //create the new image of width +1
        for(size_t i=0; i<new->height;i++)
        {
            jP=0;
            for(size_t j=0; j<new->width;j++)
            {
                tmp->data[i*tmp->width + jP] = new->data[i*new->width + j];
                //duplicate the pixels in the seam
                for(size_t o=0;o<l;o++)
                {
                    if(j == selectedSeam[i*kP + o])
                    {
                        jP++;
                        if(j<new->width-1)
                        {
                            tmp->data[i*tmp->width + jP].red = 
                                (new->data[i*new->width + j].red + new->data[i*new->width + j+1].red)/2;
                            tmp->data[i*tmp->width + jP].green = 
                                (new->data[i*new->width + j].green + new->data[i*new->width + j+1].green)/2;
                            tmp->data[i*tmp->width + jP].blue = 
                                (new->data[i*new->width + j].blue + new->data[i*new->width + j+1].blue)/2;
                        }
                        else
                        {
                            tmp->data[i*tmp->width + jP] = new->data[i*new->width + j];
                        }
                    }
                }
                jP++;   
            }
        } 
        freePNM(new);
        new = tmp;//the tmp image becomes the new current image
        free(energies);
        free(moves);
        free(selectedSeam);
    }
    return new;
}