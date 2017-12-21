//
// Created by tortumine on 19/12/17.
//

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "Comic.h"

int extras(const PNMImage** images,int i, int j, size_t comicWidth, size_t comicBorder,int** Memo);
int cost(const PNMImage** images,int i, int j, size_t comicWidth, size_t comicBorder,int** Memo);
int c(const PNMImage **images, size_t comicWidth, size_t comicBorder, int **memo, size_t **cuts, int nb_cuts);
void Primary(const PNMImage** images,int i, int j, size_t comicWidth, size_t comicBorder,int** memo, size_t ** cuts,size_t * nb_cuts);
void setBackgroudColor(PNMImage *image,int R,int G,int B);


/* ------------------------------------------------------------------------- *
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


    Primary(images,0,nbImages-1,comicWidth,comicBorder,memo,cuts,&nb_cuts);

    // convertion de la disposition en format demandé (cf specification)
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

    //
    for(i=0;i<nbImages;i++)
    {
        printf("\t%ld",i+1);
        if(positions[i]!=positions[i+1])printf("\n");
    }
    fflush(stdout);

    //Free used mem
    for (i=0; i<nbImages; i++)
        free(memo[i]);
    /***
     * TODO: put secondary optimisation here
     */

    //Return
    return positions;
}

/* ------------------------------------------------------------------------- *
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

    size_t tmp=0;


    //calcul optimal de 'une disposition
    size_t* disposition = wrapImages(images, nbImages, comicWidth,comicBorder);

    w=comicWidth;
    h=((images[0]->height)*disposition[nbImages-1])+(2*comicBorder)+((disposition[nbImages-1]-1)*comicBorder);

    PNMImage* final = createPNM(w,h);
    setBackgroudColor(final,255,255,255);

    for(k=0;k<nbImages;k++)
    {
        y=disposition[k]*y_offset+comicBorder;

        printf("\t %ld:%ld",x,y);
        for(i=0;i<images[k]->height;i++)
        {
            //for each point of the images[k]
            for(j=0;j<images[k]->width;j++)
            {
                //calculate position in the final image
                tmp=(x+j)+((y+i)*final->width);

                //print value to the final image
                //only if the calculated position is on the bounds
                // this if in necessary for tests (SIGABRT risk)
                if(tmp<=w*h)
                {
                    final->data[tmp].red=0;
                    final->data[tmp].green=0;
                    final->data[tmp].blue=0;
                }
            }
        }

        //Calcul des coordonnées des cases
        if(disposition[k]!=disposition[k+1]) x=comicBorder;
        else x+=images[k]->width+comicBorder;
    }

    return final;
}


int extras(const PNMImage** images,int i, int j, size_t comicWidth, size_t comicBorder,int** Memo)
{
    int tmp;
    //si la valeur est sauvegardée
    if(Memo[i][j] != -13) return Memo[i][j];
    else
    {
        //si non sauvegardée mais une seule case
        if(i==j)
        {
            //calcul extra case unique
            tmp = images[i]->width - comicWidth + comicBorder*2;
        }
        //si non sauvegardée mais plusieurs cases
        else
        {
            //calcul (case actuelle + espace + extra(cases suivante))-largeur souhaitée
            tmp = images[i]->width + comicBorder + extras(images,i+1,j,comicWidth,comicBorder,Memo);
        }
        Memo[i][j]=tmp;
        return tmp;
    }
}
int cost(const PNMImage** images,int i, int j, size_t comicWidth, size_t comicBorder,int** Memo)
{
    int tmp = extras(images,i,j,comicWidth,comicBorder,Memo);
    tmp = tmp*tmp*tmp;
    tmp = abs(tmp);
    return tmp;
}
int c(const PNMImage **images, size_t comicWidth, size_t comicBorder, int **memo, size_t **cuts, int nb_cuts)
{
    int k,tmp=0;
    for(k=0;k<=nb_cuts;k++)
    {
        tmp+=cost(images,cuts[k][0],cuts[k][1],comicWidth,comicBorder,memo);
    }
    return tmp;
}

/***
 * Fonction de découpe primaire, agence les cases de façon gloutonne donnant priorité au premières cases.
 * On essaye d'approcjher le plus possible la largeur pour la première ligne, puis on coupe et on continue pour la seconde, etc
 *
 * @param images
 * @param i
 * @param j
 * @param comicWidth
 * @param comicBorder
 * @param memo
 * @param cuts
 * @param nb_cuts
 *
 * Les données utiles sont dans cuts et nb_cuts
 */
void Primary(const PNMImage** images,int i, int j, size_t comicWidth, size_t comicBorder,int** memo, size_t ** cuts,size_t * nb_cuts)
{
    if(i>=j)
    {
        cuts[*nb_cuts-1][1]=j;
        return;
    }
    else
    {
        int cond = 1;
        int k = i;
        while(cond)
        {
            //comparaison des couts
            int tmpa=cost(images,i,k,comicWidth,comicBorder,memo);
            int tmpb=cost(images,i,k+1,comicWidth,comicBorder,memo);
            if(tmpa>tmpb)//si on peut encore ajouter une case
            {
                k++;
            }
            else//si la configuration actuelle est optimale
            {
                cuts[*nb_cuts][0]=i;
                cuts[*nb_cuts][1]=k;
                *nb_cuts=*nb_cuts + 1;
                Primary(images,k+1,j,comicWidth,comicBorder,memo,cuts,nb_cuts);
                cond =0;
            }
        }
        return;
    }
}

/***
 * This function pain the entire image in the desired color
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