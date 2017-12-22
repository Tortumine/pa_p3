//TODO: clean code

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "Comic.h"
#include "SeamCarving.h"
#include "PNM.h"

int extras(const PNMImage** images,size_t i, size_t j, size_t comicWidth, size_t comicBorder,int** Memo);
unsigned long long cost(const PNMImage** images,size_t i, size_t j, size_t comicWidth, size_t comicBorder,int** Memo);
unsigned long long c(const PNMImage **images, size_t i, size_t j, size_t comicWidth, size_t comicBorder, int **memo, size_t **cuts,size_t *nb_cuts);
void setBackgroudColor(PNMImage *image,int R,int G,int B);


/*** ------------------------------------------------------------------------- *
 * Compute the optimal positions of the images on the page.
 *
 * The returned array must later be deleted by calling free().
 *
 * PARAMETERS
 * images       An array of images
 * nbImages     The number of images
 * comicWidth   The width of a page
 * comicBorder  The border around images
 *
 * RETURN
 * positions    An array of size nbImages, such that positions[i] corresponds
 *              to the row (starting from 0) of images[i] in the optimal
 *              configuration.
 * NULL         if an error occured
 * ------------------------------------------------------------------------- */
size_t* wrapImages(const PNMImage** images, size_t nbImages, size_t comicWidth,size_t comicBorder)
{
    size_t i,j,nb_cuts;
    size_t* positions;
    int tmp;
    size_t cellsOnLine=0;

    PNMImage* output;

    //Memoization table
    int *memo[nbImages];
    for (i=0; i<nbImages; i++)
        memo[i] = (int *)malloc(nbImages * sizeof(int));
    for (i = 0; i <  nbImages; i++)
        for (j = 0; j < nbImages; j++)
            memo[i][j] = -13;

    //cuts table
    size_t *cuts[nbImages];
    for (i=0; i<nbImages; i++)
        cuts[i] = (size_t *)malloc(2 * sizeof(size_t));
    nb_cuts=0;


    c(images, 0, nbImages - 1, comicWidth, comicBorder, memo, cuts, &nb_cuts);
    
    //convert cuts table into the desired output format (cf statement)
    positions=malloc(nbImages* sizeof(size_t));
    int k =0;
    for(i=0;i<nb_cuts;i++)
    {
        for(j=cuts[i][0];j<=cuts[i][1];j++)
        {
            positions[k]=i;
            k++;
        }
    }

    //images fusion
    for(i=0;i<nb_cuts;i++)
    {
        tmp = extras(images,cuts[i][0],cuts[i][1],comicWidth,comicBorder,memo );
        if(tmp !=0)//if the difference between de desired length and selected pic is 0 => do nothing
        {
            cellsOnLine=cuts[i][1]-cuts[i][0]+1;
            if(tmp>0)   //if diff is > 0 => reduce image 
            {
                for(j=cuts[i][0];j<=cuts[i][1];j++)//reduce all images on the line equally
                {
                    output=reduceImageWidth(images[j], tmp/cellsOnLine);
                    images[j]=output;
                }
                if(tmp%cellsOnLine !=0)//extra pixel length goes to the firs cell on the row
                {
                    output=reduceImageWidth(images[cuts[i][0]], tmp%cellsOnLine);
                    images[cuts[i][0]]=output;
                }
            }
            else        //if diff is < 0 => increase
            {
                for(j=cuts[i][0];j<=cuts[i][1];j++)//reduce all images on the line equally
                {
                    output=increaseImageWidth(images[j], abs(tmp)/cellsOnLine);
                    images[j]=output;
                }
                if(abs(tmp)%cellsOnLine !=0)//extra pixel length goes to the firs cell on the row
                {
                    output=increaseImageWidth(images[cuts[i][0]], abs(tmp)%cellsOnLine);
                    images[cuts[i][0]]=output;
                }
            }
        }
    }
    //Free used mem
    for (i=0; i<nbImages; i++)
        free(memo[i]);
    //Return
    return positions;
}

/*** ------------------------------------------------------------------------- *
 * Pack images into a single comic image.
 *
 * The returned image must later be deleted by calling freePNM().
 *
 * PARAMETERS
 * images       An array of images
 * nbImages     The number of images
 * comicWidth   The width of a page
 * comicBorder  The border around images
 *
 * RETURN
 * image        The comic image
 * NULL         if an error occured
 * ------------------------------------------------------------------------- */
PNMImage* packComic(const PNMImage** images, size_t nbImages, size_t comicWidth, size_t comicBorder)
{
    size_t i,j,k;
    size_t w,h;
    size_t y_offset=comicBorder+images[0]->height;
    size_t x=comicBorder,y=0;

    size_t tmpF=0,tmpI=0;


    //get the optimal disposition and modify images to fit this disposition
    size_t* disposition = wrapImages(images, nbImages, comicWidth,comicBorder);

    w=comicWidth;
    h=((images[0]->height)*(disposition[nbImages-1]+1))+(2*comicBorder)+((disposition[nbImages-1])*comicBorder);


    //initialize the destination image
    PNMImage* final = createPNM(w,h);
    setBackgroudColor(final,255,255,255);


    //go through image and print them to the destination image
    for(k=0;k<nbImages;k++)
    {
        y=disposition[k]*y_offset+comicBorder;

        for(i=0;i<images[k]->height;i++)
        {
            //for each point of the images[k]
            for(j=0;j<images[k]->width;j++)
            {
                //calculate position in the final image
                tmpF=(x+j)+((y+i)*final->width);
                tmpI=j+i*images[k]->width;
                //print value to the final image
                //only if the calculated position is on the bounds
                // this if in necessary for tests (SIGABRT risk) if
                //something goes wring during image modification
                if(tmpF<=w*h)
                {
                    //copy each pixel RGB
                    final->data[tmpF].red=images[k]->data[tmpI].red;
                    final->data[tmpF].green=images[k]->data[tmpI].green;
                    final->data[tmpF].blue=images[k]->data[tmpI].blue;
                }
            }
        }

        //get the image position
        if(disposition[k]!=disposition[k+1]) x=comicBorder;
        else x+=images[k]->width+comicBorder;
    }
    //return final image
    return final;
}
/***
 * This function calculate the diffrence in pixels between the set of images and the desired length
 * Ti also saves the result in the memo table to it can be accessed in a constant time in the future
 * 
 * @param images 
 * @param i 
 * @param j 
 * @param comicWidth 
 * @param comicBorder 
 * @param Memo 
 * @return pixel difference
 */
int extras(const PNMImage** images,size_t i, size_t j, size_t comicWidth, size_t comicBorder,int** Memo)
{
    long int tmp;
    //check if value is already exist in the table
    // the -13 value us used by default, if the actual result is -13, 
    //  the function will be called recursively (only once)
    if(Memo[i][j] != -13) return Memo[i][j];
    else
    {
        //if not saved but only one cell
        if(i==j)
        {
            //unique image formula
            tmp = images[i]->width - comicWidth + comicBorder*2;
        }
        //if not saved bus many cells
        else
        {
            //extra = (first_cell + comic_Border + extra(next_cells))- desired_comic_Width
            tmp = images[i]->width + comicBorder + extras(images,i+1,j,comicWidth,comicBorder,Memo);
        }
        //save in the array
        Memo[i][j]= (int) tmp;
        return (int) tmp;
    }
}
/***
 * This function calculate the cost a set of images bases on the extra
 * because of the n³ the cost can be really big
 * to be sure to avoid an overflow unsigned long long are used
 * 
 * @param images 
 * @param i 
 * @param j 
 * @param comicWidth 
 * @param comicBorder 
 * @param Memo 
 * @return 
 */
unsigned long long cost(const PNMImage** images,size_t i, size_t j, size_t comicWidth, size_t comicBorder,int** Memo)
{
    unsigned long long tmp = abs(extras(images,i,j,comicWidth,comicBorder,Memo));
    tmp = tmp*tmp*tmp;
    return tmp;
}

/***
 * This function calculate the global cost and also crate the best images arrangement
 * 
 * @param images
 * @param i
 * @param j
 * @param comicWidth
 * @param comicBorder
 * @param memo
 * @param cuts          array for saving cuts (where the algorithm cut the line)
 *                      formatted ad double array [n][2]
 *                      the first row contains the first cell index on the line, the second contains the last
 * @param nb_cuts
 *
 * @return cost ( disposition is saved in the cuts pointer array)
 */
unsigned long long c(const PNMImage **images, size_t i, size_t j, size_t comicWidth, size_t comicBorder, int **memo, size_t **cuts,
       size_t *nb_cuts)
{
    unsigned long long tmpa,tmpb;
    //if only one element
    if(i>=j)
    {
        cuts[*nb_cuts-1][1]=(size_t) j;
        return cost(images,j, j,comicWidth,comicBorder,memo);
    }
    //if more than one
    else
    {
        int cond = 1;
        size_t k = i;
        while(cond)
        {
            //compare cost for (i,k) and (i,k+1)
            tmpa=cost(images,i,k,comicWidth,comicBorder,memo);
            tmpb=cost(images,i,k+1,comicWidth,comicBorder,memo);
            if(tmpa>tmpb)//if cost(i,k+1) is lower, we can add a cell to the line
            {
                k++;
            }
            else//if current disposition of the line is optimal
            {
                //save to cuts table
                cuts[*nb_cuts][0]= (size_t) i;
                cuts[*nb_cuts][1]= (size_t) k;
                *nb_cuts=*nb_cuts + 1;
                
                //call for rest of images
                tmpa+=c(images, k + 1, j, comicWidth, comicBorder, memo, cuts, nb_cuts);
                cond =0;
            }
        }
        //return global cost
        return tmpa;
    }
}

/***
 * This function paint the entire image in the desired color
 *
 * @param image
 * @param R
 * @param G
 * @param B
 */
void setBackgroudColor(PNMImage *image,int R,int G,int B)
{
    size_t i;
    if(image){

        for(i=0;i<image->width*image->height;i++){
            image->data[i].red=(unsigned char)R;
            image->data[i].green=(unsigned char)G;
            image->data[i].blue=(unsigned char)B;
        }
    }
}