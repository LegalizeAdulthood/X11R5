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
#include "sj_const.h"
#include "sj_typedef.h"

Int	cmpstr(src, dst)
Reg1	Uchar	*src;
Reg2	Uchar	*dst;
{
	while (*src) {
		if (*src != *dst) return FALSE;
		src++;
		dst++;
	}

	if (*dst) return FALSE;

	return TRUE;
}

