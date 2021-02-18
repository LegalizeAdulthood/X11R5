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

Uchar	*code2kanji();

Uchar	*getkan_none(s, d, ym, yl, flg)
Uchar	*s;
Uchar	*d;
Uchar	*ym;
Int	yl;
Int	flg;
{
	return d;
}

Uchar	*getkan_hira(s, d, ym, yl, flg)
Uchar	*d;
Reg2	Uchar	*ym;
Uchar	*s;
Int	yl;
Int	flg;
{
	Reg1	Int	cnt;

	cnt = (*s & KanjiCodeMask) + 1;

	if (flg && *s & KanjiStrEnd) ym += (yl - cnt) * 2;

	cnt *= 2;
	while (cnt-- > 0) *d++ = *ym++;

	return d;
}

Uchar	*getkan_kata(s, d, ym, yl, flg)
Reg1	Uchar	*d;
Reg2	Uchar	*ym;
Uchar	*s;
Int	flg;
{
	Reg3	Int	cnt;
	Reg4	Uchar	ch;

	cnt = (*s & KanjiCodeMask) + 1;

	if (flg && *s & KanjiStrEnd) ym += (yl - cnt) * 2;

	while (cnt-- > 0) {
		if (*ym == 0x82) {
			ch = *++ym;
			if (0x9f <= ch && ch <= 0xf1) {
				if (ch > 0xdd) ch++;
				ch -= (0x9f - 0x40);
				*d++ = 0x83;
			}
			else {
				*d++ = 0x82;
			}
			*d++ = ch;
			ym++;
		}
		else {
			*d++ = *ym++;
			*d++ = *ym++;
		}
	}

	return d;
}

Uchar	*getkan_knj(s, d, ym, yl, flg)
Uchar	*s;
Uchar	*d;
Uchar	*ym;
Int	yl;
Int	flg;
{
	return code2kanji(askknj[*s & KanjiCodeMask], d, ym, yl, flg);
}

Uchar	*getkan_ofs(s, d, ym, yl, flg)
Uchar	*s;
Uchar	*d;
Uchar	*ym;
Int	yl;
Int	flg;
{
	return code2kanji(dicbuf + ((*s & KanjiCodeMask) << 8) + *(s + 1),
				d, ym, yl, flg);
}

Uchar	*getkan_norm(s, d, ym, yl, flg)
Uchar	*s;
Uchar	*d;
Uchar	*ym;
Int	yl;
Int	flg;
{
	Uchar	ch;

	if ((ch = (*s++ | 0x80)) != LeadingHankaku) *d++ = ch;
	*d++ = *s;

	return d;
}

Static	Uchar	*code2kanji(s, d, ym, yl, flg)
Uchar	*s;
Uchar	*d;
Uchar	*ym;
Int	yl;
Int	flg;
{
	for ( ; ; ) {
		if (*s & KanjiStrEnd) {
			d = (*Getkan_func[*s >> 4])(s, d, ym, yl, flg);
			break;
		}
		else
			d = (*Getkan_func[*s >> 4])(s, d, ym, yl, FALSE);

		s += codesize(*s);
	}

	return d;
}

Int	getkanji(ym, yl, ptr, buf)
Uchar	*ym;
Int	yl;
Uchar	*ptr;
Uchar	*buf;
{
	Uchar	*q;

	q = code2kanji(ptr, buf, ym, yl, TRUE);

	return q - buf;
}

