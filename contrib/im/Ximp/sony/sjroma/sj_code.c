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
 * Author: Masaki Takeuchi, Sony Corporation
 */
/*
 *	sj_code.c
 *	euc / sjis conversion
 *	Copyright (c) 1988 1989 Sony Corporation
 *	Written by Masaki Takeuchi
 *	Tue Apr  5 10:46:04 JST 1988
 */
/*
 * $Header: sj_code.c,v 1.1 91/04/29 17:56:42 masaki Exp $ SONY;
 */

#include "sjparam.h"

euctosjis (in, out)
register u_char	*in, *out;
{
	register int	c;
	u_char		tmp[3];

	while ((c = *(in ++)) != '\0') {
		if (iseuckana (c)) {
			tmp[0] = c;
			if ((c = *(in ++)) == '\0') {
				*(out ++) = tmp[0];
				break;
			}
			*(out ++) = c;
		}
		else if (iseuc (c)) {
			tmp[0] = c;
			if ((c = *(in ++)) == '\0') {
				*(out ++) = tmp[0];
				break;
			}
			tmp[1] = c;
			tmp[2] = '\0';
			sj_euc2sjis (tmp);
			*(out ++) = tmp[0];
			*(out ++) = tmp[1];
		}
		else {
			*(out ++) = c;
		}
	}
	*out = '\0';
}

#define MASK	0x7f
#define MSB	0x80

sj_euc2sjis (s)
u_char *s;
{
	s[0] &= MASK;
	s[1] &= MASK;
	sj_jis2sjis (s);
}

sj_jis2sjis (s)
u_char	*s;
{
	register int	high, low;
	register int	nh, nl;

	high = s[0];
	low = s[1];
	nh = ((high - 0x21) >> 1) + 0x81;
	if (nh > 0x9f)
		nh += 0x40;
	if (high & 1) {
		nl = low + 0x1f;
		if (low > 0x5f)
			nl ++;
	}
	else
		nl = low + 0x7e;
	s[0] = nh;
	s[1] = nl;
}

#define SS2	0x8e

sjistoeuc (in, out)
register u_char	*in, *out;
{
	register int	c;
	u_char		tmp[3];

	while ((c = *(in ++)) != '\0') {
		if (iskana (c)) {
			*(out ++) = SS2;
			*(out ++) = c;
		}
		else if (iskanji (c)) {
			tmp[0] = c;
			if ((c = *(in ++)) == '\0') {
				*(out ++) = tmp[0];
				break;
			}
			tmp[1] = c;
			tmp[2] = '\0';
			sj_sjis2euc (tmp);
			*(out ++) = tmp[0];
			*(out ++) = tmp[1];
		}
		else {
			*(out ++) = c;
		}
	}
	*out = '\0';
}

sj_sjis2euc (s)
u_char	*s;
{
	sj_sjis2jis (s);
	s[0] |= MSB;
	s[1] |= MSB;
}

sj_sjis2jis (s)
u_char	*s;
{
	register int	byte1, byte2;
	register u_char	*sp;

	sp = s;
	byte1 = *sp++;
	byte2 = *sp--;

	if (byte1 <= 0x80 || byte1 >= 0xf0
	 || ( byte1>=0xa0 && byte1<=0xdf )
	 || byte2 <= 0x3f || byte2 >= 0xfe || byte2==0x7f ) {
			byte1 = 0x81;
			byte2 = 0x40;
	}
	byte1 -= (byte1 >= 0xa0) ? 0xc1 : 0x81;
	if (byte2 >= 0x9f) {
		*sp++ = (byte1 << 1) + 0x22;
		*sp = byte2 - 0x7e;
	}
	else {
		*sp++ = (byte1 << 1) + 0x21;
		*sp = byte2 - ((byte2 <= 0x7e) ? 0x1f : 0x20 );
	}
}

sjis2euc (c)
int	c;
{
	u_char	s[4];

	s[0] = (c >> 8) & 0xff;
	s[1] = c & 0xff;
	sj_sjis2euc (s);
	return ((s[0] << 8) | s[1]);
}

euc2sjis (c)
int	c;
{
	u_char	s[4];

	s[0] = (c >> 8) & 0xff;
	s[1] = c & 0xff;
	sj_euc2sjis (s);
	return ((s[0] << 8) | s[1]);
}

sjis2jis (c)
int	c;
{
	u_char	s[4];

	s[0] = (c >> 8) & 0xff;
	s[1] = c & 0xff;
	sj_sjis2jis (s);
	return ((s[0] << 8) | s[1]);
}

jis2sjis (c)
int	c;
{
	u_char	s[4];

	s[0] = (c >> 8) & 0xff;
	s[1] = c & 0xff;
	sj_jis2sjis (s);
	return ((s[0] << 8) | s[1]);
}
