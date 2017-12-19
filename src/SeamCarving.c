//
// Created by tortumine on 19/12/17.
//

#include <stddef.h>
#include "PNM.h"
/* ------------------------------------------------------------------------- *
 * Reduce the width of a PNM image to `image->width-k` using the seam carving
 * algorithm.
 *
 * The PNM image must later be deleted by calling freePNM().
 *
 * PARAMETERS
 * image        Pointer to a PNM image
 * k            The number of pixels to be removed (along the width axis)
 *
 * RETURN
 * image        Pointer to a new PNM image
 * NULL         if an error occured
 * ------------------------------------------------------------------------- */
PNMImage* reduceImageWidth(const PNMImage* image, size_t k)
{

}

/* ------------------------------------------------------------------------- *
 * Increase the width of a PNM image to `image->width+k` using the seam
 * carving algorithm.
 *
 * The PNM image must later be deleted by calling freePNM().
 *
 * PARAMETERS
 * image        Pointer to a PNM image
 * k            The number of pixels to be added (along the width axis)
 *
 * RETURN
 * image        Pointer to a new PNM image
 * NULL         if an error occured
 * ------------------------------------------------------------------------- */
PNMImage* increaseImageWidth(const PNMImage* image, size_t k)
{

}