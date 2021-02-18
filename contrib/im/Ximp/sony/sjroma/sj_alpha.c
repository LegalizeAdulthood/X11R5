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
 *	sj_alpha.c
 *	hankaku alphabet to zenkaku alphabet conversion
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi
 *	Sat May  9 01:44:52 JST 1987
 */
/*
 * $Header: sj_alpha.c,v 1.1 91/04/29 17:56:40 masaki Exp $ SONY;
 */

#include "sjparam.h"

#define SP	0x20
#define TILDA	0x7e
#define ALPHAn	(TILDA-SP+1)
#define HA_SUFFIX	0	/* Hankaku alphabet suffix	*/
#define ZA_SUFFIX	1	/* Zenkaku alphabet suffix	*/
#define ZK_SUFFIX	2	/* Zenkaku katakana suffix	*/
#define HK_SUFFIX	3	/* Hankaku katakana suffix	*/
#define SUFFIX_DIMENSION	4

extern char	*Atable[][SUFFIX_DIMENSION];

HAtoZA (in, out)
u_char	*in;	/* hankaku alphabet string input	*/
u_char	*out;	/* zenkaku katakana string output	*/
{
	HAtoZAorZK (in, out, ZA_SUFFIX);
}

HAtoZK (in, out)
u_char	*in;	/* hankaku alphabet string input	*/
u_char	*out;	/* zenkaku katakana string output	*/
{
	HAtoZAorZK (in, out, ZK_SUFFIX);
}

HAtoZAorZK (in, out, suffix)
register u_char	*in;	/* hankaku alphabet string input	*/
register u_char	*out;	/* zenkaku katakana string output	*/
register int	suffix;
{
	register u_char	c;
	u_char		Htmp[2], Ztmp[3];

	*out = '\0';
	while ((c = *(in++)) != '\0') {
		if (c >= SP && c <= TILDA) {
			strcat (out, Atable[c - SP][suffix]);
		}
		else if (iskana (c)) {
			Htmp[0] = c;
			Htmp[1] = '\0';
			HKtoZK (Htmp, Ztmp);
			strcat (out, Ztmp);
		}
		else if (iskanji (c)) {
			Ztmp[0] = c;
			Ztmp[1] = *(in++);
			Ztmp[2] = '\0';
			strcat (out, Ztmp);
		}
	}
}

ZAtoHK (in, out)
u_char	*in;	/* zenkaku alphabet string input	*/
u_char	*out;	/* hankaku alphabet string output	*/
{
	ZAtoHKorHA (in, out, HK_SUFFIX);
}

ZAtoHA (in, out)
u_char	*in;	/* zenkaku alphabet string input	*/
u_char	*out;	/* hankaku alphabet string output	*/
{
	ZAtoHKorHA (in, out, HA_SUFFIX);
}

ZAtoHKorHA (in, out, suffix)
register u_char	*in;	/* zenkaku alphabet string input	*/
register u_char	*out;	/* hankaku alphabet string output	*/
register int	suffix;
{
	register u_char	c1, c2;
	register int	i;
	int		flag;

	*out = '\0';
	while ((c1 = *(in++)) != '\0') {
		c2 = *(in++);
		flag = 1;
		for (i = 0 ; *Atable[i][ZK_SUFFIX] != '\0' ; i ++) {
			if (c1 == (u_char)Atable[i][ZK_SUFFIX][0]) {
				if (c2 == (u_char)Atable[i][ZK_SUFFIX][1]) {
					*(out++) = (u_char)*Atable[i][suffix];
					*out = '\0';
					flag = 0;
					break;
				}
			}
		}
		if (flag) {
			*(out ++) = c1;
			*(out ++) = c2;
			if (c2 == '\0')
				break;
			*out = '\0';
		}
	}
}

HKc2ZKs (c, out)
register u_char	c, *out;
{
	register int	i;

	*out = '\0';
	for (i = 0 ; *Atable[i][HK_SUFFIX] != '\0' ; i ++) {
		if (c == (u_char)*Atable[i][HK_SUFFIX]) {
			*(out ++) = (u_char)Atable[i][ZK_SUFFIX][0];
			*(out ++) = (u_char)Atable[i][ZK_SUFFIX][1];
			*out = '\0';
			return (1);
		}
	}
	for (i = 0 ; *Atable[i][HA_SUFFIX] != '\0' ; i ++) {
		if (c == (u_char)*Atable[i][HA_SUFFIX]) {
			*(out ++) = (u_char)Atable[i][ZK_SUFFIX][0];
			*(out ++) = (u_char)Atable[i][ZK_SUFFIX][1];
			*out = '\0';
			return (1);
		}
	}
	return (0);
}

