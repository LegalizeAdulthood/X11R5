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
 *	sj_romaji.c
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Wed May  6 16:45:52 JST 1987
 */
/*
 * $Header: sj_romaji.c,v 1.1 91/04/29 17:56:09 masaki Exp $ SONY;
 */

#include "common.h"
#include "rtbl.h"

int		converted_characters;

char		CDbuff[BUFFLENGTH];

/* clear romaji buffer		*/

SJ_romaji_clear ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	cv->alpha_point = 0;
	*(cv->alpha_buff) = '\0';
}

/* check is there romaji	*/

SJ_romaji_check ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	if (cv->alpha_point > 0)
		return (cv->alpha_point);
	else
		return (0);
}

/* delete last character in romaji buffer */

SJ_romaji_backspace ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	if (cv->alpha_point > 0) {
		cv->alpha_point --;
		cv->alpha_buff[cv->alpha_point] = '\0';
	}
}

SJ_romaji_flush (mod, s, s2)
register int	mod;
register char	*s, *s2;
{
	register Conversion	*cv;
	char		tmp[SHORTBUFFSIZE];
	char		Htmp[SHORTBUFFSIZE];

	cv = GetConversion ();
	*s = '\0';
	strcpy (s2, cv->alpha_buff);
	if (cv->alpha_point > 0) {
		if (cv->alpha_buff[cv->alpha_point - 1] == 'n' &&
		    (mod == MODE_ZHIRA || mod == MODE_ZKATA ||
		     mod == MODE_HKATA)) {
			Strncpy (tmp, cv->alpha_buff, cv->alpha_point - 1);
			tmp[cv->alpha_point - 1] = '\0';
			HAtoZK (tmp, s);
			SJ_romaji_clear ();
			SJ_romaji ('n', tmp, Htmp);
			return (1);
		}
		else {
			HAtoZK (cv->alpha_buff, s);
			SJ_romaji_clear ();
		}
	}
	return (0);
}

/*
 *	SJ_romaji (c, s)
 *	convert alphabet to kana (now zenkaku katakana)
 *	int	c;	input alphabet character
 *	char	*s;	output zenkaku kana strings
 *	RETURN;		MITEI	0	no output produce, c is buffered.
 *			ZANTEI	1	output produce, c is buffered.
 *			KAKUTEI	2	output produce, no buffered.
 *			(defined in rtbl.h)
 */

SJ_romaji (c, s, sh)
register int	c;
register char	*s, *sh;
{
	register Conversion	*cv;
	extern int	tuduri_missmatch;
	extern int	tuduri_match;
	register int	i, len;

	cv = GetConversion ();
	*s = '\0';
	cv->alpha_buff[cv->alpha_point++] = c;
	cv->alpha_buff[cv->alpha_point] = '\0';

	if (cv->Imode == MODE_SCODE || cv->Imode == MODE_ECODE ||
	    cv->Imode == MODE_JCODE || cv->Imode == MODE_KCODE) {
		/* 4 digit -> code kettei	*/
		if (cv->alpha_point == 4) {
			CODEtoKANJI (cv->alpha_buff, CDbuff);
			converted_characters = 4;
			HAtoZA (cv->alpha_buff, s);
			strcpy (sh, cv->alpha_buff);
			SJ_romaji_clear ();
			return (KAKUTEI);
		}
		else {
			return (MITEI);
		}
	}

	if (cv->Imode == MODE_ZALPHA || cv->Imode == MODE_HALPHA) {
		converted_characters = 1;
		HAtoZA (cv->alpha_buff, s);
		strcpy (sh, cv->alpha_buff);
		SJ_romaji_clear ();
		return (KAKUTEI);
	}

	len = sj_getromaji2 (cv->alpha_buff, s);

	if (len) {
		if (len == cv->alpha_point) {
			/* buffer overflow	*/
			if (cv->alpha_point >= SHORTBUFFSIZE) {
				converted_characters = cv->alpha_point;
				HAtoZK (cv->alpha_buff, s);
				strcpy (sh, cv->alpha_buff);
				SJ_romaji_clear ();
				return (KAKUTEI | MISS_MATCH);
			}
			else {
				return (MITEI);
			}
		}
		else {
			strcpy (sh, &(cv->alpha_buff[cv->alpha_point-len]));
			for (i = 0 ; i < len ; i ++)
				cv->alpha_buff[i] = cv->alpha_buff[cv->alpha_point-len+i];
			converted_characters = cv->alpha_point - len;
			cv->alpha_point = len;
			cv->alpha_buff[cv->alpha_point] = '\0';
			return (ZANTEI | MISS_MATCH);
		}
	}
	else {
		converted_characters = cv->alpha_point;
		strcpy (sh, cv->alpha_buff);
		SJ_romaji_clear ();
		if (tuduri_missmatch)
			return (KAKUTEI | MISS_MATCH);
		else
			return (KAKUTEI);
	}
}

SJ_kana (c, out)
register int	c;
char	*out;
{
	char	s[2];

	s[0] = c;
	s[1] = '\0';
	HKtoZK (s, out);
	converted_characters = 1;
	return (KAKUTEI);
}

SJ_kanji (c1, c2, out)
register int	c1, c2;
char	*out;
{
	out[0] = c1;
	out[1] = c2;
	out[2] = '\0';
	converted_characters = 2;
	return (KAKUTEI);
}

struct romaji	*Utable[MAXELEMENT];
int		Uelement = 0;

clear_Utable ()
{
	while (Uelement > 0) {
		Uelement --;
		free (Utable[Uelement]->kana);
		free (Utable[Uelement]->tuduri);
		free ((char *)Utable[Uelement]);
	}
	sj_clear_romajitbl ();
}

set_Utable (utuduri, ukana)
register u_char	*utuduri, *ukana;
{
	u_char	*malloc ();
	u_char	tmp[256];

	if (Uelement >= MAXELEMENT)
		return (1);

	if ((Utable[Uelement] =
		(struct romaji *) malloc (sizeof(struct romaji)))
		== (struct romaji *) NULL)
		return (1);
	if ((Utable[Uelement]->tuduri =
	    malloc (strlen (utuduri) + 1)) == NULL) {
		free (Utable[Uelement]);
		return (1);
	}
	if ((Utable[Uelement]->kana =
	    malloc (strlen (ukana) + 1)) == NULL) {
		free (Utable[Uelement]->tuduri);
		free (Utable[Uelement]);
		return (1);
	}
	strcpy (Utable[Uelement]->tuduri, utuduri);
	compile_kana (ukana, tmp);
	strcpy (Utable[Uelement]->kana, tmp);
	Uelement ++;
	return (0);
}

set_sjRtable ()
{
	sj_set_romajitbl (Utable, Uelement);
}

static
set_sjRtbl_europe ()
{
	sj_e_romajitbl ();
}

SJ2_romaji_init (sj)
int	sj;
{
	if (!sj)
		set_sjRtbl_europe ();
}

print_Utable ()
{
	register int	i;

	aprintf ("romaji tuduri sort\n\r");
	for (i = 0 ; i < Uelement ; i ++)
		aprintf ("%s\t%s\n\r", Utable[i]->tuduri, Utable[i]->kana);
	fflush (stdout);
}

compile_kana (s, t)
register u_char	*s, *t;
{
	while (*s) {
		if (*s == '0' && *(s+1) == 'x'
		    && IsHex(*(s+2)) && IsHex(*(s+3))) {
			*(t ++) = (htoi(*(s+2)) << 4) | htoi(*(s+3));
			s += 4;
		}
		else {
			*(t ++) = *(s ++);
		}
	}
	*t = 0;
}

htoi (c)
u_char	c;
{
	if (isdigit (c))
		return (c - '0');
	else if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	else if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	else
		return (0);
}
