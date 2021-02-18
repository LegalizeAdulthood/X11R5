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
#include "sj_suuji.h"

Void	setwdnum();
Int	setucnum();

Static	Int	word2char(wd, tbl, keta, flg)
Ushort	wd;
Uchar	TFar	*tbl;
Int	keta;
Int	flg;
{
	Int	mask;
	Int	num;
	Uchar	TFar	*src;

	for (mask = 0x1000 ; mask ; mask >>= 4) {
		switch (num = wd / mask) {
		case 0:
			break;

		case _Num0:
			if (!flg) break;

		default:
			src = tbl + (num - _Num0) * 2;
			*kanjitmp++ = *src++;
			*kanjitmp++ = *src;
			flg = TRUE;

			if (keta && !(keta % 3)) {
				*kanjitmp++ = CommaHigh;
				*kanjitmp++ = CommaLow;
			}
		}
		if (keta) keta--;

		wd &= (mask - 1);
	}

	return flg;
}

Static	Void	words2num(wd, tbl, flgc)
Ushort	*wd;
Uchar	TFar	*tbl;
Int	flgc;
{
	Int	i;
	Int	keta;
	Int	flg;
	Uchar	*kp = kanjitmp;

	if (flgc) {
		keta = NumKetaLength - 1;
		flg = FALSE;
	}
	else {
		keta = 0;
		flg = TRUE;
	}

	for (i = NumWordBuf ; i-- > 0 ; ) {
		flg = word2char(*(wd + i), tbl, keta, flg);
		if (keta) keta -= 4;
	}

	if (kp == kanjitmp) {
		*kanjitmp++ = *tbl++;
		*kanjitmp++ = *tbl;
	}
}

Void	num_type00(s1, s2, jrec)
Uchar	*s1;
Uchar	*s2;
JREC	*jrec;
{
	Ushort	num[NumWordBuf];

	setwdnum(s1, (Int)jrec -> numlen, num);

	words2num(num, Num1tbl, FALSE);
}

Void	num_type01(s1, s2, jrec)
Uchar	*s1;
Uchar	*s2;
JREC	*jrec;
{
	Ushort	num[NumWordBuf];

	setwdnum(s1, (Int)jrec -> numlen, num);

	words2num(num, Num1tbl, FALSE);
}

Void	num_type02(s1, s2, jrec)
Uchar	*s1;
Uchar	*s2;
JREC	*jrec;
{
	Ushort	num[NumWordBuf];

	setwdnum(s1, (Int)jrec -> numlen, num);

	words2num(num, Num1tbl, TRUE);
}

Void	num_type03(s1, s2, jrec)
Uchar	*s1;
Uchar	*s2;
JREC	*jrec;
{
	Ushort	num[NumWordBuf];

	setwdnum(s1, (Int)jrec -> numlen, num);

	words2num(num, Num2tbl, FALSE);
}

Void	num_type04(s1, s2, jrec)
Uchar	*s1;
Uchar	*s2;
JREC	*jrec;
{
	Ushort	num[NumWordBuf];

	setwdnum(s1, (Int)jrec -> numlen, num);

	words2num(num, Num2tbl, FALSE);
}

Void	num_type05(s1, s2, jrec)
Uchar	*s1;
Uchar	*s2;
JREC	*jrec;
{
	Ushort	num[NumWordBuf];

	setwdnum(s1, (Int)jrec -> numlen, num);

	words2num(num, Num2tbl, TRUE);
}

Void	num_type06(s1, s2, jrec)
Uchar	*s1;
Uchar	*s2;
JREC	*jrec;
{
	Ushort	num[NumWordBuf];

	setwdnum(s1, (Int)jrec -> numlen, num);

	words2num(num, Num3tbl, FALSE);
}

Void	num_type07(s1, s2, jrec)
Uchar	*s1;
Uchar	*s2;
JREC	*jrec;
{
	Ushort	num[NumWordBuf];

	setwdnum(s1, (Int)jrec -> numlen, num);

	words2num(num, Num3tbl, FALSE);
}

Static	Void	kan_num(wd, tbl1, tbl2)
Ushort	*wd;
Uchar	TFar	*tbl1;
Uchar	TFar	*tbl2;
{
	Int	ii;
	Ushort	tmp;
	Int	mask;
	Int	num;
	Int	flg;
	Uchar	TFar	*kurai1;
	Uchar	TFar	*kurai2;
	Uchar	TFar	*src;
	Uchar	*keep = kanjitmp;

	kurai2 = Num6tbl - 2;
	mask = 0;
	wd += NumWordBuf - 1;
	flg = FALSE;

	for (ii = NumKetaLength ; ii-- > 0 ; ) {
		if (!mask) {
			if (flg) {
				*kanjitmp++ = *kurai2++;
				*kanjitmp++ = *kurai2++;
			}
			else
				kurai2 += 2;

			mask = 0x1000;
			tmp = *wd--;
			flg = FALSE;
			kurai1 = tbl2;
		}

		if ((num = tmp / mask) > _Num0) {
			if ((num != _Num1) || !*kurai1) {
				src = tbl1 + (num - _Num0) * 2;
				*kanjitmp++ = *src++;
				*kanjitmp++ = *src;
			}

			if (*kurai1) {
				*kanjitmp++ = *kurai1++;
				*kanjitmp++ = *kurai1++;
			}
			flg = TRUE;
		}
		else
			kurai1 += 2;

		tmp &= (mask - 1);
		mask >>= 4;
	}

	if (kanjitmp == keep) {
		*kanjitmp++ = *tbl1++;
		*kanjitmp++ = *tbl1;
	}
}

Void	num_type08(s1, s2, jrec)
Uchar	*s1;
Uchar	*s2;
JREC	*jrec;
{
	Ushort	num[NumWordBuf];

	setwdnum(s1, (Int)jrec -> numlen, num);

	kan_num(num, Num2tbl, Num4tbl);
}

Void	num_type09(s1, s2, jrec)
Uchar	*s1;
Uchar	*s2;
JREC	*jrec;
{
	Ushort	num[NumWordBuf];

	setwdnum(s1, (Int)jrec -> numlen, num);

	kan_num(num, Num3tbl, Num5tbl);
}

Void	num_type10(s1, s2, jrec)
Uchar	*s1;
Uchar	*s2;
JREC	*jrec;
{
	Int	i;

	for (i = jrec -> numlen ; i > 0 ; i--) {
		if (Chrtbl[*s1++] & NUMBER) {
			*kanjitmp++ = *s2++;
			*kanjitmp++ = *s2++;
		}
		else
			s2 += 2;
	}
}

Void	num_type11(s1, s2, jrec)
Uchar	*s1;
Uchar	*s2;
JREC	*jrec;
{
	Int	i;
	Int	j;

	i = j = jrec -> numlen;
	if (!(Chrtbl[*(s1 + j - 4)] & NUMBER)) j -= (j - 1) / 4;

	for ( ; i > 0 ; i--) {
		if (Chrtbl[*s1++] & NUMBER) {
			*kanjitmp++ = *s2++;
			*kanjitmp++ = *s2++;
			if (--j > 0) {
				if ((j % 3) == 0) {
					*kanjitmp++ = CommaHigh;
					*kanjitmp++ = CommaLow;
				}
			}
		}
		else
			s2 += 2;
	}
}

Void	num_type12(s1, s2, jrec)
Uchar	*s1;
Uchar	*s2;
JREC	*jrec;
{
	Int	i;
	Int	j;

	for (i = jrec -> numlen ; i > 0 ; i--) {
		if (Chrtbl[j = *s1++] & NUMBER) {
			j = (j - N_0) * 2;
			*kanjitmp++ = Num2tbl[j];
			*kanjitmp++ = Num2tbl[j + 1];
		}
	}
}

Static	Void	num_kurai(p, len, tbl)
Uchar	*p;
Int	len;
Uchar	TFar	*tbl;
{
	Int	i;
	Int	j;
	Uchar	tmp;

	for (i = 0 ; i < len ; i++) {
		tmp = *p++;

		*kanjitmp++ = tbl[j = ((tmp & 0x0f) - _Num0) * 2];
		*kanjitmp++ = tbl[j + 1];
		if (j = ((tmp >> 4) & 3)) {
			*kanjitmp++ = Num4tbl[j = (3 - j) * 2];
			*kanjitmp++ = Num4tbl[j + 1];
		}
		if (j = ((tmp >> 6) & 3)) {
			*kanjitmp++ = Num6tbl[j = (3 - j) * 2];
			*kanjitmp++ = Num6tbl[j + 1];
		}
	}
}

Void	num_type13(s1, s2, jrec)
Uchar	*s1;
Uchar	*s2;
JREC	*jrec;
{
	Uchar	num[NumKetaLength];
	Int	len;

	len = setucnum(s1, (Int)jrec -> numlen, num);

	num_kurai(num, len, Num1tbl);
}

Void	num_type14(s1, s2, jrec)
Uchar	*s1;
Uchar	*s2;
JREC	*jrec;
{
	Uchar	num[NumKetaLength];
	Int	len;

	len = setucnum(s1, (Int)jrec -> numlen, num);

	num_kurai(num, len, Num2tbl);
}

