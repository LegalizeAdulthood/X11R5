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
#include "sj_hinsi.h"

JREC	*argjrec();

Int	setj_atrb(p)
Uchar		*p;
{
	return 2;
}

Int	setj_ofs(p)
Uchar	*p;
{
	Int	contf = TRUE;

	p = dicbuf + ((*p & KanjiCodeMask) << 8) + *(p + 1);

	do {
		if (*p & KanjiStrEnd) contf = FALSE;
		p += (*Setjrec_func[*p >> 4])(p);
	} while (contf);

	return 2;
}

Int	setj_knj(p)
Uchar	*p;
{
	Int	contf = TRUE;

	p = askknj[*p & KanjiCodeMask];

	do {
		if (*p & KanjiStrEnd) contf = FALSE;
		p += (*Setjrec_func[*p >> 4])(p);
	} while (contf);

	return 1;
}

Int	setj_norm1(p)
Uchar	*p;
{
	return 1;
}

Int	setj_norm2(p)
Uchar	*p;
{
	return 2;
}

Static	Uchar	chkhead(gram)
TypeGram	gram;
{
	if (!headcode) return TRUE;

	if (headcode == SETTOU_DAI) {
		if (gram == SUUSI) return TRUE;
	}

	else {
		gram--;
		if ((Scncttbl(headcode-1, gram >> 3) << (gram & 0x07)) & 0x80) {
			return TRUE;
		}
	}

	return FALSE;
}

Void	setjrec(tagp, mode)
Uchar	*tagp;
Int	mode;
{
	TypeGram	gram;
	Uchar	*ptr;
	Uchar	*endp;
	Uchar	*tmp;
	JREC	*rec;
	Int	plen;
	Int	nlen;
	Int	len;

	plen = getplen(tagp);
	nlen = getnlen(tagp);

	len = plen + nlen;

	if (headcode) len += headlen;

	ptr = tagp + DouonBlkSizeNumber + nlen;

	endp = tagp + getsize(tagp);

	for ( ; ptr < endp ; ptr = tmp + 1) {

		for (tmp = ptr + 1; *tmp != HinsiBlkTerm ; ) {
			tmp += (*Setjrec_func[*tmp >> 4])(tmp);
		}

		if ((gram = *ptr) == IKKATU && !(mode & DO_IKKATU)) continue;

		if (!chkhead(gram)) continue;

		if (!(rec = argjrec(len, (JREC *)NULL))) continue;

		rec -> jseg   = prevseg;
		rec -> jofsst = ptr - dicbuf;
		rec -> jofsed = tmp - dicbuf;
		rec -> class  = C_DICT;
		rec -> dicid  = curdict->dicid;
		rec -> hinsi  = gram;
		rec -> sttofs = headcode;
	}
}

Void	setnumrec(tagp, rec, gram)
Uchar		*tagp;
JREC		*rec;
TypeGram	gram;
{
	Uchar		*ptr;
	Uchar		*endp;
	Uchar		*tmp;
	Int		plen, nlen, len;
	JREC		*jrec;


	plen = getplen(tagp);
	nlen = getnlen(tagp);

	len = plen + nlen;

	ptr = tagp + DouonBlkSizeNumber + nlen;

	endp = tagp + getsize(tagp);

	for ( ; ptr < endp ; ptr = tmp + 1) {

		for (tmp = ptr + 1; *tmp != HinsiBlkTerm ; ) {
			tmp += (*Setjrec_func[*tmp >> 4])(tmp);
		}

		if (gram != *ptr) continue;

		if (!(jrec = argjrec((Int)(rec -> jlen + len), rec))) continue;

		jrec -> jseg   = prevseg;
		jrec -> jofsst = ptr - dicbuf;
		jrec -> jofsed = tmp - dicbuf;
		jrec -> dicid  = curdict->dicid;
	}
}

Void	setcrec(tagp)
Uchar	*tagp;
{
	JREC	*rec;

	if (!(rec = argjrec((int)ClYomiLen(tagp), (JREC *)NULL))) return;

	rec -> jofsst = tagp - ClStudyDict;
	rec -> class  = C_BUNSETU;
	rec -> hinsi  = ClGramCode(tagp);
}

