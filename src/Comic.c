//
// Created by tortumine on 19/12/17.
//

#include <stddef.h>
#include <math.h>
#include "PNM.h"

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

}

/* ------------------------------------------------------------------------- *
 * Compute the optimal positions of the images on the page.
 *
 * Computes the difference between the wanted comicWidth and
 * the with obtained bt cells i to j.
 *
 * PARAMETERS
 * images       An array of images
 * i            First image to consider
 * j            Last image to consider
 * comicWidth   The width of a page
 * comicBorder  The border around images
 *
 * RETURN
 * extras       The difference between desired length and the obtained one
 * NULL         if an error occurred
 * ------------------------------------------------------------------------- */
int extras(const PNMImage** images,int i, int j, size_t comicWidth, size_t comicBorder)
{

}

/* ------------------------------------------------------------------------- *
 *
 * Computes the cost of placing the cells i to j on the same row.
 *
 * PARAMETERS
 * images       An array of images
 * i            First image to consider
 * j            Last image to consider
 * comicWidth   The width of a page
 * comicBorder  The border around images
 *
 * RETURN
 * extras       The cost of the line
 * NULL         if an error occurred
 * ------------------------------------------------------------------------- */
int cost(const PNMImage** images,int i, int j, size_t comicWidth, size_t comicBorder)
{
    int tmp = extras(images,i,j,comicWidth,comicBorder);
    if(tmp==NULL)return NULL;
    return pow(tmp,3);
}
