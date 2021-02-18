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

Int	getkanji(), seldict();
Uchar	TFar	*getstb();
Void	get_askknj();

Void	cvtdict(krec, clrec, flg)
KHREC	*krec;
CLREC	*clrec;
Int	flg;
{
	TypeDicOfs	ofs;
	Uchar		*yptr;
	Int		jlen;
	Int		stblen;
	Int		sttlen;
	Uchar		TFar	*fptr;
	Int		cllen;
	JREC		*jrec;
	Int		i;

	ofs   = krec -> offs;
	jrec  = clrec -> jnode;
	cllen = clrec -> cllen;

	yptr = inputyomi;

	jlen = jrec -> jlen;

	if (i = jrec -> sttofs) {
		if (i == SETTOU_KIGOU) {
			sttlen = 1;
			fptr = yptr;
		}

		else {
			fptr = Settou_ptr(i);
			sttlen = SttYomiLen(fptr);
			fptr++;
			if (!krec -> sttkj) fptr += 2;
		}

		*kanjitmp++ = *fptr++;
		*kanjitmp++ = *fptr++;
		jlen -= sttlen;
		yptr += sttlen * 2;
	}

	else
		sttlen = 0;

	if (flg) {
		(*Cvtnum_func[krec->mode - 1])(ystart + sttlen, yptr, jrec);

		yptr += (i = jrec -> numlen) * 2;
		jlen -= i;

		if (jrec -> stbofs) {
			*kanjitmp++ = *yptr++;
			*kanjitmp++ = *yptr++;
			jlen--;
		}

		stblen = 0;
	}

	else if ((i = jrec -> stbofs) && (fptr = getstb(jrec -> hinsi))) {
		fptr += i - 1;
		stblen = StbYomiLen(fptr);
		jlen -= stblen;
	}

	else {
		stblen = 0;
	}

	if (ofs) {
		if (seldict(jrec -> dicid)) {
			(*curdict->getdic)(curdict, jrec -> jseg);
			get_askknj();
			kanjitmp +=
				getkanji(yptr, jlen, dicbuf + ofs, kanjitmp);
			yptr += jlen * 2;
		}
		else {
			jlen *= 2;
			while (jlen-- > 0) *kanjitmp++ = *yptr++;
		}
	}

	if (stblen) {
		i = StbKnjLen(fptr);
		fptr += stblen + 2;
		while (i-- > 0) *kanjitmp++ = *fptr++;
		yptr += stblen * 2;
	}

	i = (cllen - (jrec -> jlen)) * 2;
	while (i-- > 0) *kanjitmp++ = *yptr++;
}

Void	cvtminasi(len)
Int	len;
{
	Uchar	*ptr;

	ptr = inputyomi;

	while (len-- > 0) {

		if (isknj1(*ptr)) {
			*kanjitmp++ = *ptr++;
			*kanjitmp++ = *ptr++;
		}
		else {
			*kanjitmp++ = *ptr++;
		}
	}
}

Void	cvtwakachi(clrec)
CLREC	*clrec;
{
	Int	jlen;
	Uchar	*ym;
	Int	cnt;
	Uchar	ch;

	ym = inputyomi;

	cnt = jlen = clrec -> jnode -> jlen;
	while (cnt-- > 0) {
		if (isknj1(*ym)) {
			if (*ym == 0x82) {
				ch = *++ym;
				if (0x9f <= ch && ch <= 0xf1) {
					if (ch > 0xdd) ch++;
					ch -= (0x9f - 0x40);
					*kanjitmp++ = 0x83;
				}
				else {
					*kanjitmp++ = 0x82;
				}
				*kanjitmp++ = ch;
				ym++;
			}
			else {
				*kanjitmp++ = *ym++;
				*kanjitmp++ = *ym++;
			}
		}
		else {
			*kanjitmp++ = *ym++;
		}
	}

	cnt = clrec -> cllen - jlen;
	while (cnt-- > 0) {

		if (isknj1(*ym)) {
			*kanjitmp++ = *ym++;
			*kanjitmp++ = *ym++;
		}
		else {
			*kanjitmp++ = *ym++;
		}
	}
}

