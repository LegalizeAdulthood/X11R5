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


(h, hmany_count, masks, row0, 
row1, rule, nrule, wrap_left,
wrap_right, irule, palette, output)
int h; int hmany_count; int masks; Comp *row0; 
Comp *row1; int **rule; int nrule; Comp **wrap_left;
Comp **wrap_right; int *irule; int *palette ; unsigned char *output ;
{
Comp *rk, *rl, *rc, *rr, *rt ;
int col ;
int c = h * hmany_count ;
int cc = c + hmany_count ;
int *jrule = masks ? rule[nrule+h] : rule[irule[h]] ;
row1[c] = jrule[COMPUTATION (row0[wrap_left[h][1]],
				     row0[wrap_left[h][0]],
			     	     row0[c], row0[c + 1], row0[c + 2])] ;
output[c] = palette[row1[c]] ;
row1[c+1] = jrule[COMPUTATION (row0[wrap_left[h][0]],
			     row0[c], row0[c + 1], row0[c + 2], row0[c + 3])];
output[c+1] = palette[row1[c+1]] ;


rk = &row0[c    ] ;
rl = &row0[c + 1] ;
rc = &row0[c + 2] ;
rr = &row0[c + 3] ;
rt = &row0[c + 4] ;

for (col=c+2;  col < cc-2;  col++)
	{
	row1[col] = jrule [COMPUTATION (*rk, *rl, *rc, *rr, *rt)] ;
	output[col] = palette[row1[col]] ;

	rk ++ ;
	rl ++ ;
	rc ++ ;
	rr ++ ;
	rt ++ ;
	}

row1[cc-2]=jrule[COMPUTATION(row0[cc-4], row0[cc-3], row0[cc-2],
		 	     row0[cc-1], row0[wrap_right[h][0]])] ;
output[cc-2] = palette[row1[cc-2]] ;
row1[cc-1]=jrule[COMPUTATION(row0[cc-3], row0[cc-2], row0[cc-1],
			     row0[wrap_right[h][0]], row0[wrap_right[h][1]])];
output[cc-1] = palette[row1[cc-1]] ;

return ;
}


