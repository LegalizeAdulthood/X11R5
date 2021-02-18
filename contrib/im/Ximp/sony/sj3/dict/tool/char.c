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

#include <stdio.h>
#include <sys/types.h>
#include "sj_const.h"

cnvyomi(code)
int	code;
{
	register u_char	high;
	register u_char low;

	high = ((code >> 8) & 0xff);
	low = (code & 0xff);

	if (high == 0x81) {
		switch (low) {
		case 0x5b:	return 1;
		case 0x94:	return 2;
		case 0x97:	return 3;
		case 0xa7:	return 4;
		}
	}
	else if (high == 0x82) {
		if (low < 0x4f)
			;
		else if (low <= 0x58)
			return(low - 0x4f + 0x10);
		else if (low < 0x60)
			;
		else if (low <= 0x79)
			return(low - 0x60 + 0x1A);
		else if (low < 0x81)
			;
		else if (low <= 0x9a)
			return(low - 0x81 + 0x34);
		else if (low < 0x9f)
			;
		else if (low <= 0xf1)
			return(low - 0x9f + 0x4e);
	}
	else if (high == 0x83) {
		switch (low) {
		case 0x94:	return 0xa1;
                case 0x95:	return 0xa2;
                case 0x96:	return 0xa3;
		}
	}

	return 0;
}

h2kcode(code)
int	code;
{
	register u_char	high;
	register u_char	low;

	high = ((code >> 8) & 0xff);
	low = (code & 0xff);

	if (high != 0x82)
		return code;
	else if (low < 0x9f)
		return code;
	else if (low < 0xde)
		return(0x8300 + low - 0x9f + 0x40);
	else
		return(0x8300 + low - 0xde + 0x80);
}

codesize(code)
u_char	code;
{
	static	int	val[] = {
		2,
		2,
		1,
		1,
		1,
		2,
		2,
		2
	};

	return	val[(code & KanjiModeMask) >> 4];
}

output_knj(fp, p, l)
FILE	*fp;
register u_char	*p;
register int	l;
{
	while (l > 0) {
		switch (*p & KanjiModeMask) {
		case NormalKanji0:
		case NormalKanji1:
		case NormalKanji2:	fputc(*p++ | 0x80, fp); l--;
					fputc(*p++, fp); l--;
					break;

		case ZenHiraAssyuku:	fprintf(fp, "\202\320%d", (*p++ & 0x0f) + 1);
					l--;
					break;

		case ZenKataAssyuku:	fprintf(fp, "\203\112%d", (*p++ & 0x0f) + 1);
					l--;
					break;

		case KanjiAssyuku:	fprintf(fp, "\210\263%1x", (*p++ & 0x0f));
					l--;
					break;

		case OffsetAssyuku:	fprintf(fp, "ofs%1x", (*p++ & 0x0f));
					l--;
					fprintf(fp, "%02x", *p++); l--;
					break;

		case AiAttribute:	fprintf(fp, "\221\256%1x", (*p++ & 0x0f));
					l--;
					fprintf(fp, "%02x", *p++); l--;
					break;

		default:		l = 0;
					fprintf(stderr, "\202\250\202\251\202\265\202\242");
					break;
		}
	}
}

output_str(fp, p)
FILE	*fp;
char	*p;
{
	while (*p) { fputc(*p, fp); p++; }
}

output_int(fp, p)
FILE	*fp;
int	*p;
{
	while (*p) {
		if (*p < 0x100) {
			fputc(*p, fp); p++;
		}
		else {
			fputc(*p / 0x100, fp);
			fputc(*p & 0xff, fp); p++;
		}
	}
}

yomi2zen(code)
int	code;
{
	static	char	num[] = {
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
	};

	switch (code) {
	case 0x01:	return 0x815b;
	case 0x02:	return 0x8194;
	case 0x03:	return 0x8197;
	case 0x04:	return 0x81a7;
	case 0xa1:	return 0x8394;
	case 0xa2:	return 0x8395;
	case 0xa3:	return 0x8396;
	default:	break;
	}

	if (code < 0x10)
		;

	else if (code < 0x1a)
		return(0x8200 + code - 0x10 + 0x4f);

	else if (code < 0x34)
		return(0x8200 + code - 0x1a + 0x60);

	else if (code < 0x4e)
		return(0x8200 + code - 0x34 + 0x81);

	else if (code < 0xa1)
		return(0x8200 + code - 0x4e + 0x9f);

	return ((num[(code >> 4) & 0x0f] << 8) | num[code & 0x0f]);
}

output_yomi(fp, p)
FILE	*fp;
u_char	*p;
{
	int	i;

	while (*p) {
		i = yomi2zen(*p++);
		fputc((i >> 8) & 0xff, fp);
		fputc(i & 0xff, fp);
	}
}
