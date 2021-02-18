/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Noriyuki Hosoe, Sony Corporation
 */

#include "sj_rename.h"
#include "sj_typedef.h"

Uchar	taipri[7][45] = {

	{ 0,  4,  4,  4,  4,  4,  4,  4,  4,  4,
		  4,  4,  3,  3,  3,  3,  3,  3,  0,  0,
		  0,  3, 20,  4,  3,  3,  4,  4,  4,  4,
	 	  4,  4,  4, 20,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4},

	{ 0,  4,  4,  4,  4,  4,  4,  4,  4,  4,
		  4,  4,  3,  3,  3,  3,  3,  3,  0,  0,
		  0,  4,  4,  4,  4,  4,  4,  4,  4,  4,
	 	  4,  4,  4, 20,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4},

	{ 0,  4,  4,  4,  4,  4,  4,  4,  4,  4,
		  4,  4,  3,  3,  3,  3,  3,  3,  0,  0,
		  0,  4,  4,  4,  4,  4,  4,  4,  4,  4,
	 	  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  6},

	{ 0,  5,  5,  5,  5,  5,  5,  5,  5,  5,
		  5,  5,  3,  3,  3,  3,  3,  3,  0,  0,
		  0,  3,  3,  3,  4,  5,  4,  4,  4,  4,
	 	  4,  4,  4,  4,  4,  4,  6,  6,  6,  8,  4,  4,  4,  4,  6},

	{ 0,  7,  7,  7,  7,  7,  7,  7,  7,  7,
		  7,  7,  4,  4,  4,  4,  4,  4,  0,  0,
		  0,  4,  4,  4, 20, 20,  4,  4,  4,  4,
	 	  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  6},

	{ 0,  8,  8,  8,  8,  8,  7,  8,  8,  8,
		  8,  8,  3,  6,  7,  3,  3,  5,  0,  0,
		  0,  3,  3,  3,  3,  3,  5,  5,  5,  5,
	 	  5,  5,  5,  5,  5,  5,  8,  8, 11,  5,  7,  7,  7,  7,  7},

	{ 0,  5,  6,  6,  6,  6,  5,  6,  6,  6,
		  6,  6,  3,  3,  3,  3,  3,  5,  0,  0,
		  0,  3,  3,  3,  3,  3,  5,  5,  5,  5,
 		  4,  5,  5,  5,  5,  5,  5,  6,  6,  5,  5,  5,  5,  5,  5}
	};



Uchar	sttpri[5][31] = {

	{ 0,  7,  7,  7,  7,  7,  7,  7,  7,  7,
		  7,  7,  2,  4,  4,  4,  4,  4,  0,  0,
	 	  0,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4, },

	{ 4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
		  4,  4,  4,  4,  4,  4,  4,  4,  0,  0,
	 	  0,  4,  4,  4, 10, 10,  4,  4,  4,  4,  4, },

	{ 4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
		  4,  4,  4,  4,  4,  4,  4,  4,  0,  0,
	 	  0, 10, 10,  4,  4,  4,  4,  4,  4,  4,  4, },

	{ 0,  4,  4,  4,  4,  4,  4,  4,  4,  4,
		  4,  4,  4,  4,  4,  4,  4,  4,  0,  0,
	 	  0,  4,  4,  4,  4,  4,  4,  4,  4,  4,  15, },

	{ 0, 12, 12, 12, 12, 12, 12, 12, 12, 12,
		 12, 12,  9,  9,  9,  9,  9,  9,  0,  0,
	 	  0,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4, }
	};
