//
// Created by tortumine on 19/12/17.
//

//TODO: translate all comments
//TODO: uniforming comments style "/***" preferred
//TODO: clean code
//TODO: do something with c return value


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

    for(i=0;i<nb_cuts;i++)
    {
        tmp = extras(images,cuts[i][0],cuts[i][1],comicWidth,comicBorder,memo );
        if(tmp !=0)// bonne taille on ne fait rien pour cette ligne
        {
            cellsOnLine=cuts[i][1]-cuts[i][0]+1;
            if(tmp>0)   //trop grand -> on réduit
            {
                for(j=cuts[i][0];j<=cuts[i][1];j++)
                {
                    output=reduceImageWidth(images[j], tmp/cellsOnLine);
                    images[j]=output;
                }
                if(tmp%cellsOnLine !=0)
                {
                    output=reduceImageWidth(images[cuts[i][0]], tmp%cellsOnLine);
                    images[cuts[i][0]]=output;
                }
            }
            else        //trop petit -> on agrandit
            {
                for(j=cuts[i][0];j<=cuts[i][1];j++)
                {
                    output=increaseImageWidth(images[j], abs(tmp)/cellsOnLine);
                    images[j]=output;
                }
                if(abs(tmp)%cellsOnLine !=0)
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

    size_t tmpF=0,tmpI=0;


    //calcul optimal de 'une disposition
    size_t* disposition = wrapImages(images, nbImages, comicWidth,comicBorder);

    w=comicWidth;
    h=((images[0]->height)*(disposition[nbImages-1]+1))+(2*comicBorder)+((disposition[nbImages-1])*comicBorder);


    //initialization de l'image de destination
    PNMImage* final = createPNM(w,h);
    setBackgroudColor(final,255,255,255);


    //parcours des images et leur écriture dans l'image de destination
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
                tmpF=(x+j)+((y+i)*final->width);
                tmpI=j+i*images[k]->width;
                //print value to the final image
                //only if the calculated position is on the bounds
                // this if in necessary for tests (SIGABRT risk)
                if(tmpF<=w*h)
                {
                    final->data[tmpF].red=images[k]->data[tmpI].red;
                    final->data[tmpF].green=images[k]->data[tmpI].green;
                    final->data[tmpF].blue=images[k]->data[tmpI].blue;
                }
            }
        }

        //Calcul des coordonnées des cases
        if(disposition[k]!=disposition[k+1]) x=comicBorder;
        else x+=images[k]->width+comicBorder;
    }

    return final;
}
int extras(const PNMImage** images,size_t i, size_t j, size_t comicWidth, size_t comicBorder,int** Memo)
{
    long int tmp;
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
        Memo[i][j]= (int) tmp;
        return (int) tmp;
    }
}
unsigned long long cost(const PNMImage** images,size_t i, size_t j, size_t comicWidth, size_t comicBorder,int** Memo)
{
    unsigned long long tmp = abs(extras(images,i,j,comicWidth,comicBorder,Memo));
    tmp = tmp*tmp*tmp;
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
unsigned long long c(const PNMImage **images, size_t i, size_t j, size_t comicWidth, size_t comicBorder, int **memo, size_t **cuts,
       size_t *nb_cuts)
{
    unsigned long long tmpa,tmpb;
    if(i>=j)
    {
        cuts[*nb_cuts-1][1]=(size_t) j;
        return cost(images,j, j,comicWidth,comicBorder,memo);
    }
    else
    {
        int cond = 1;
        size_t k = i;
        while(cond)
        {
            //comparaison des couts
            tmpa=cost(images,i,k,comicWidth,comicBorder,memo);
            tmpb=cost(images,i,k+1,comicWidth,comicBorder,memo);
            if(tmpa>tmpb)//si on peut encore ajouter une case
            {
                k++;
            }
            else//si la configuration actuelle est optimale
            {
                cuts[*nb_cuts][0]= (size_t) i;
                cuts[*nb_cuts][1]= (size_t) k;
                *nb_cuts=*nb_cuts + 1;
                tmpa+=c(images, k + 1, j, comicWidth, comicBorder, memo, cuts, nb_cuts);
                cond =0;
            }
        }
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