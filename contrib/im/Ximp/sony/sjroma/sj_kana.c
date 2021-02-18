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
 *	sj_kana.c
 *	kana handling routine
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written  by Masaki Takeuchi
 *	Fri May  8 16:55:12 JST 1987
 */
/*
 * $Header: sj_kana.c,v 1.1 91/04/29 17:56:45 masaki Exp $ SONY;
 */

#include "sjparam.h"
#include "kana.h"
#include "rtbl.h"

extern char	*Ktable[][SUFFIX_DIMENSION];

/*
 *	ZKtoZH (zkata, zhira)
 *	convert zenkaku katakana string to zenkaku hiragana string
 *	char	*zkata, *zhira;
 *	zkata;	input zenkaku katakana string's pointer
 *	zhira;	output zenkaku hiragana string's pointer
 */

ZKtoZH (zkata, zhira)
u_char	*zkata, *zhira;
{
	ZKtoZHorHK (zkata, zhira, ZH_SUFFIX);
}

/*
 *	ZKtoHK (zkata, hkata)
 *	convert zenkaku katakana string to hankaku katakana string
 *	char	*zkana, *hkata;
 *	zkana;	input zenkaku katakana string's pointer
 *	hkata;	output hankaku katakana string's pointer
 */

ZKtoHK (zkata, hkata)
u_char	*zkata, *hkata;
{
	ZKtoZHorHK (zkata, hkata, HK_SUFFIX);
}

ZKtoZHorHK (in, out, suffix)
register u_char	*in, *out;
int 		suffix;
{
	register int	c;
	u_char		s[3], temp[5];

	*out = '\0';
	while ((c = *(in++)) != '\0') {
		if (c == ZKATA1 && *in >= ZKATAF2 && *in <= ZKATAE2) {
			if ((c = *(in++)) == '\0')
				break;
			c -= ZKATAF2;
			strcat (out, Ktable[(int)c][suffix]);
		} else if (suffix == ZH_SUFFIX) {
			if (iskana (c)) {
				s[0] = c;
				s[1] = '\0';
				HKtoZH (s, temp);
				strcat (out, temp);
			}
			else {
				s[0] = c;
				s[1] = *(in++);
				s[2] = '\0';
				strcat (out, s);
				if (s[1] == '\0')
					break;
			}
		} else if (suffix == HK_SUFFIX) {
			if (iskana (c)) {
				s[0] = c;
				s[1] = '\0';
				strcat (out, s);
			}
			else {
				s[0] = c;
				s[1] = *(in++);
				s[2] = '\0';
				ZAtoHK (s, temp);
				strcat (out, temp);
				if (s[1] == '\0')
					break;
			}
		}
	}
}

ZHtoHK (in, out)
register u_char	*in, *out;
{
	register int	c;
	u_char		s[3], temp[5];

	*out = '\0';
	while ((c = *(in++)) != '\0') {
		if (c == ZHIRA1 && *in >= ZHIRAF2 && *in <= ZHIRAE2) {
			if ((c = *(in++)) == '\0')
				break;
			if (c >= ZHIRAMU)
				c ++;
			c -= ZHIRAF2;
			*(out ++) = Ktable[(int)c][HK_SUFFIX][0];
			if (Ktable[(int)c][HK_SUFFIX][1] != '\0')
				*(out ++) = Ktable[(int)c][HK_SUFFIX][1];
			*out = '\0';
		}
		else if (iskanji (c)) {
			temp[0] = c;
			temp[1] = *(in ++);
			temp[2] = 0;
			ZAtoHK (temp, s);
			*(out ++) = *s;
			if (s[1] != '\0')
				*(out ++) = s[1];
			*out = '\0';
		}
		else {
			*(out ++) = c;
			*out = '\0';
		}
	}
	*out = '\0';
}

/*
 *	new function, converts hankaku-kana to etc.
 */

HKtoZH (in, out)
register u_char	*in, *out;
{
	HKtoZHorZK (in, out, ZH_SUFFIX);
}

HKtoZK (in, out)
register u_char	*in, *out;
{
	HKtoZHorZK (in, out, ZK_SUFFIX);
}

HKtoZHorZK (in, out, suffix)
register u_char	*in, *out;
int		suffix;
{
	register int	c;
	register int	i, mutch;
	u_char		tmp[3];

	*out = '\0';
	while ((c = *(in ++)) != '\0') {
	    mutch = 0;
	    if (!iskanji (c))
		for (i = 0 ; *Ktable[i][HK_SUFFIX] != '\0' ; i ++) {
			if (c == (u_char)*Ktable[i][HK_SUFFIX]) {
				if (*in == CHAR_TEN && ZKavail_ten (c)) {
					in ++;
					i += ZKavail_ten (c);
				}
				else if (*in==CHAR_MARU && ZKavail_maru(c)) {
					in ++;
					i += 2;
				}
				else if (c == CHAR_WA) {
					i ++;	/* because of xwa	*/
				}
				*(out++) = Ktable[i][suffix][0];
				*(out++) = Ktable[i][suffix][1];
				*out = '\0';
				mutch ++;
				break;
			}
		}
	    if (!mutch) {
		if (iskanji (c)) {
			*(out ++) = c;
			*(out ++) = *(in ++);
			*out = '\0';
		}
		else if (HKc2ZKs (c, tmp)) {
			*(out ++) = tmp[0];
			*(out ++) = tmp[1];
			*out = '\0';
		}
	    }
	}
}

/*
 *	hankaku katakana code wo uketori,
 *	han dakuon kanou moji naraba,
 *	1 wo return, soude nakereba 0 wo return.
 */

ZKavail_maru (c)
register int	c;
{
	if (c >= 0xca && c <= 0xce)
		return (1);
	else
		return (0);
}

/*
 *	hankaku katakana code wo uketori,
 *	dakuon kanou moji naraba,
 *	2 byte me no shift bun wo return,
 *	soude nakereba 0 wo return.
 */

ZKavail_ten (c)
register int	c;
{
	if (c == 0xb3)				/* U			*/
		return (0x4f);			/* zenkaku katakana	*/
						/* Utenten - U		*/
	if (c >= 0xb6 && c <= 0xc4)		/* KA KI ..  TO		*/
		return (1);
	else
		return (ZKavail_maru (c));
}

