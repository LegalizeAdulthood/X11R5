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
#include "sj_const.h"

Int	istrcmp(yomip, idxp, ylen, ilen)
Reg1	Uchar	*yomip;
Reg2	Uchar	*idxp;
Reg3	Int	ylen;
Reg4	Int	ilen;
{
	for (;;) {

		if ((ilen | ylen) == 0) return MATCH;

		if (ilen == 0) return UNDER;

		if ((ylen == 0) || (*yomip < *idxp)) return OVER;

		if (*yomip > *idxp) return UNDER;

		yomip++;
		idxp++;
		ylen--;
		ilen--;
	}
}

