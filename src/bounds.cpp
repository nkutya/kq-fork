/*! \page License
   KQ is Copyright (C) 2002 by Josh Bolduc

   This file is part of KQ... a freeware RPG.

   KQ is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2, or (at your
   option) any later version.

   KQ is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with KQ; see the file COPYING.  If not, write to
   the Free Software Foundation,
       675 Mass Ave, Cambridge, MA 02139, USA.
*/


/*! \file
 * \brief Bounding areas
 * \author TT
 * \date 20060720
 */

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/bounds.h"



/*! \brief Determine whether given coordinates are within any bounding boxes
 *
 * \param   sbound - Pointer to struct, which includes a size and array
 * \param   left - Left edge of current bounding area
 * \param   top - Top edge of current bounding area
 * \param   right - Right edge of current bounding area
 * \param   bottom - Bottom edge of current bounding area
 *
 * \returns index+1 in array if found, else 0 if not found
 */
unsigned int is_bound(
    s_bound_array *sbound,
    const unsigned short left,
    const unsigned short top,
    const unsigned short right,
    const unsigned short bottom)
{
    size_t i;
    unsigned short x1, y1, x2, y2;

    assert(sbound && "s_bound_array is NULL");

    if (left < right)
    {
        x1 = left;
        x2 = right;
    }
    else
    {
        x1 = right;
        x2 = left;
    }

    if (top < bottom)
    {
        y1 = top;
        y2 = bottom;
    }
    else
    {
        y1 = bottom;
        y2 = top;
    }

    for (i = 0; i < sbound->size; ++i)
    {
        if ((x1 > sbound->array[i].right)
         || (x2 < sbound->array[i].left)
         || (y1 > sbound->array[i].bottom)
         || (y2 < sbound->array[i].top))
        {
            continue;
        }
        else
        {
            return i + 1;
        }
    }

    return 0; // not found
}



/*! \brief Determine whether the coordinates are within any bounding boxes
 *
 * \param   boxes_array - Address of array
 * \param   num_boxes - Number of array elements
 * \param   left - Left edge of current bounding area
 * \param   top - Top edge of current bounding area
 * \param   right - Right edge of current bounding area
 * \param   bottom - Bottom edge of current bounding area
 *
 * \returns NULL if not found, else address within boxes_array array
 */
s_bound *is_contained_bound(
    s_bound *boxes_array,
    unsigned int num_boxes,
    int left,
    int top,
    int right,
    int bottom)
{
    unsigned int i;

    if (num_boxes > 0)
    {
        assert(boxes_array && "s_bound is NULL");
    }

    for (i = 0; i < num_boxes; i++)
    {
        if (left >= boxes_array[i].left
         && right <= boxes_array[i].right
         && top >= boxes_array[i].top
         && bottom <= boxes_array[i].bottom)
        {
            return (&boxes_array[i]);
        }
    }

    return NULL;
}



/*! \brief Load all bounds in from packfile
 *
 * Loads individual \sa s_bound objects from the specified PACKFILE.
 *
 * \param[in,out] barray - Current array of bounds to be reallocated
 * \param[in]     pf - PACKFILE from whence data are pulled
 * \return        Non-0 on error, 0 on success
 */
size_t load_bounds(
    s_bound_array *barray,
    PACKFILE *pf)
{
    s_bound *mbound = NULL;
    unsigned int i;

    assert(barray && "barray == NULL");
    assert(pf && "pf == NULL");

    if (!barray || !pf)
    {
        allegro_message("NULL passed into load_bounds()\n");
        return 1;
    }

    barray->size = pack_igetw(pf);
    if (pack_feof(pf))
    {
        allegro_message("Expected value for number of bounds. Instead, received EOF.\n");
        return 2;
    }

    if (barray->size > 0)
    {
        barray->array = (s_bound *) realloc(barray->array, barray->size * sizeof(s_bound));
        for (i = 0; i < barray->size; ++i)
        {
            mbound = &barray->array[i];

            mbound->left    = pack_igetw(pf);
            mbound->top     = pack_igetw(pf);
            mbound->right   = pack_igetw(pf);
            mbound->bottom  = pack_igetw(pf);
            mbound->btile   = pack_igetw(pf);

            if (pack_feof(pf))
            {
                /*printf ("Encountered EOF during bound #%d read.\n", i);*/
                return 3;
            }
        }
    }
    else
    {
        barray->array = NULL;
    }

    return 0;
}



/*! \brief Save all bounds out to packfile
 *
 * Saves individual \sa s_bound objects to the specified PACKFILE.
 *
 * \param[out] barray - Current array of bounds from whence data are pulled
 * \param[out] pf - PACKFILE to where data is written
 * \return     Non-0 on error, 0 on success
 */
size_t save_bounds(
    s_bound_array *barray,
    PACKFILE *pf)
{
    size_t i;
    s_bound *mbound = NULL;

    assert(barray && "barray == NULL");
    assert(pf && "pf == NULL");

    if (!barray || !pf)
    {
        printf("NULL passed into save_bounds()\n");
        return 1;
    }

    pack_iputw(barray->size, pf);
    if (pack_feof(pf))
    {
        printf("Encountered EOF when writing bound array size.\n");
        return 2;
    }

    for (i = 0; i < barray->size; ++i)
    {
        mbound = &barray->array[i];

        pack_iputw(mbound->left, pf);
        pack_iputw(mbound->top, pf);
        pack_iputw(mbound->right, pf);
        pack_iputw(mbound->bottom, pf);
        pack_iputw(mbound->btile, pf);

        if (pack_feof(pf))
        {
            allegro_message("Encountered EOF when writing bound %d.\n", (int)i);
            return 3;
        }
    }

    return 0;
}



/* Assign the given bounding area with the x and y coords.
 * Check whether left < right or top < bottom, and swap accordingly.
 *
 * \param   which_bound - Where we will store the correct x/y coords
 * \param   left   Left edge of boundary
 * \param   top    Top edge of boundary
 * \param   right  Right edge of boundary
 * \param   bottom Bottom edge of boundary
 * \param   btile  Tile
 */
void set_bounds(
    s_bound *which_bound,
    int left,
    int top,
    int right,
    int bottom,
    int btile)
{
    assert(which_bound && "s_bound is NULL");

    // This check ensures that the given `left' is always <= `right'
    if (left <= right)
    {
        which_bound->left = left;
        which_bound->right = right;
    }
    else
    {
        which_bound->left = right;
        which_bound->right = left;
    }

    // This check ensures that the given `top' is always <= `bottom'
    if (top <= bottom)
    {
        which_bound->top = top;
        which_bound->bottom = bottom;
    }
    else
    {
        which_bound->top = bottom;
        which_bound->bottom = top;
    }

    which_bound->btile = btile;
}

