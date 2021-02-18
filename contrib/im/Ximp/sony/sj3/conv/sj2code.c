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

#include "sj_kcnv.h"
#include "sj_yomicode.h"

Int	sj2cd_chr(sjis, yomi)
Uchar	*sjis;
Reg2	Uchar	*yomi;
{
	Reg1	Uchar	chr;

	if ((chr = *sjis++) == SJIS_f1) {
		chr = *sjis;

		if (chr < SJIS_HAJIME)
			*yomi = Y_ZENKAKU;
		else if (chr <= SJIS_KIGOUEND)
			*yomi = Kigou[chr - SJIS_HAJIME];
		else
			*yomi = Y_ZENKAKU;
	}
	else if (chr == SJIS_f2) {
		chr = *sjis;

		if (chr > SJIS_HIRAEND)
			*yomi = Y_ZENKAKU;
		else if (chr >= SJIS_HIRA)
			*yomi = chr - SJIS_HIRA + _XA;
		else if (chr > SJIS_aEND)
			*yomi = Y_ZENKAKU;
		else if (chr >= SJIS_a)
			*yomi = chr - SJIS_a + A_a;
		else if (chr > SJIS_AEND)
			*yomi = Y_ZENKAKU;
		else if (chr >= SJIS_A)
			*yomi = chr - SJIS_A + A_A;
		else if (chr > SJIS_0END)
			*yomi = Y_ZENKAKU;
		else if (chr >= SJIS_0)
			*yomi = chr - SJIS_0 + N_0;
		else
			*yomi = Y_ZENKAKU;
	}
	else if (chr == SJIS_f3) {
		chr = *sjis;

		if (chr == SJIS_VU)
			*yomi = _VU;
		else if (chr == SJIS_XKA)
			*yomi = _XKA;
		else if (chr == SJIS_XKE)
			*yomi = _XKE;
		else
			*yomi = Y_ZENKAKU;
	}
	else {
		chr &= 0xf0;

		if (chr == SJIS_f8 || chr == SJIS_f9 ||
		    chr == SJIS_fe || chr == SJIS_ff) {
			*yomi = Y_ZENKAKU;
		}
		else {
			*yomi = Y_HANKAKU;

			return 1;
		}
	}

	return 2;
}

Int	sj2cd_str(sjis, yomi, len)
Reg1	Uchar	*sjis;
Reg2	Uchar	*yomi;
Reg3	Int	len;
{
	if (!len--) return FALSE;

	while (*sjis && len--) sjis += sj2cd_chr(sjis, yomi++);
	*yomi = 0;

	return *sjis ? FALSE : TRUE;
}

