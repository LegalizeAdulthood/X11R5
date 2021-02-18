/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */


#include "CuGrow.h"

void
Cu_grow_create (grow, chunk_size)
    Grow **grow ;
    int chunk_size ;
{
(*grow) = (Grow *) malloc (sizeof (Grow)) ;
(*grow)->chunk = chunk_size ;
(*grow)->s = (int *) malloc (chunk_size * sizeof (int)) ;
(*grow)->size = chunk_size ;
}

void
Cu_grow_check (grow, n)
    Grow *grow ;
    int n ;
{
int old_size ;
int *temp ;

if (n < grow->size)
    return ;

old_size = grow->size ;

while (n >= grow->size)
    grow->size += grow->chunk ;

temp = (int *) malloc (grow->size * sizeof (int)) ;
memcpy (temp, grow->s, old_size * sizeof (int)) ; 
grow->s = temp ;

return ;
}
