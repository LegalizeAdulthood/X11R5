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
 *	romaji.c
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Wed May 27 14:02:03 JST 1987
 */
/*
 * $Header: romaji.c,v 1.1 91/04/29 17:56:07 masaki Exp $ SONY;
 */

#include <stdio.h>
#include "common.h"
#include "kana.h"
#include "rtbl.h"

extern char	CDbuff[];

int	display_count = 0;		/* screen out romaji count	*/

static int	UnBuffCsave = 0;	/* UnBufferModeCursorSaveFlag	*/

display_count_clear ()
{
	display_count = 0;
	UnBuffCsave = 0;
}

romaji_backchar ()
{
	register Conversion	*cv;
	cv = GetConversion ();

	/* delete last one byte in hankaku alphabet buffer	*/
	if (cv->ha_point > 0) {
		cv->ha_point --;
		cv->Halpha[cv->ha_point] = '\0';
	}

	/* remove displaied characters				*/
	switch (cv->Dmojimod[cv->moji_count]) {
		case MODE_SCODE:
		case MODE_ECODE:
		case MODE_JCODE:
		case MODE_KCODE:
		case MODE_ZHIRA:
		case MODE_ZKATA:
		case MODE_ZALPHA:
			display_count -= 2;
			vbackchar (2);
			break;
		case MODE_HKATA:
		case MODE_HALPHA:
			display_count --;
			vbackchar (1);
			break;
		default:
			break;
	}
	SJ_romaji_backspace ();
}

HAindex_set ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	cv->HAindex_p = cv->ha_point;
}

/* romaji flush		*/

static u_char	Ztmp[SHORTBUFFSIZE * 2];
static u_char	Htmp[SHORTBUFFSIZE];
static u_char	Dumm[SHORTBUFFSIZE];

romaji_flush (s)
register char	*s;
{
	register Conversion	*cv;
	register int	Zlen, Hlen;
	register int	N_flag;
	register int	i, c;

	cv = GetConversion ();
	*Ztmp = *Htmp = '\0';
	N_flag = SJ_romaji_flush (cv->Imode, Ztmp, Dumm);
	Zlen = strlen (Ztmp);
	if (Zlen) {
		strcpy (s, Ztmp);
		ZAtoHK (Ztmp, Htmp);
		Hlen = strlen (Htmp);
		if (N_flag || cv->Imode == MODE_HALPHA ||
		    cv->Imode == MODE_ZALPHA || IsaTOKUSYU (Htmp[Hlen - 1])) {
			for (i = 0 ; i < Hlen ; i ++) {
				cv->HAindex[cv->HAindex_p++] = cv->moji_count;
				cv->Amojilen[cv->moji_count] = 1;
				if (cv->Imode == MODE_HALPHA
				 || cv->Imode == MODE_HKATA)
					cv->Dmojilen[cv->moji_count] = 1;
				else if (cv->Imode == MODE_ZALPHA
				 || cv->Imode == MODE_ZKATA)
					cv->Dmojilen[cv->moji_count] = 2;
				cv->Dmojimod[cv->moji_count] = cv->Imode;
				cv->moji_count ++;
			}
		}
		else {
			vbackchar (display_count);
			cv->ha_point = cv->HAindex_p;
			cv->Halpha[cv->ha_point] = '\0';
		}
	}
	if (N_flag) {
		switch (cv->Imode) {
			case MODE_ZHIRA:
			case MODE_ZKATA:
				display_count = 2;
				exec_romaji ('n', c);
				cv->Amojilen[cv->moji_count - 1] --;
				cv->HAindex_p --;
				break;
			case MODE_HKATA:
				display_count = 1;
				exec_romaji ('n', c);
				cv->Amojilen[cv->moji_count - 1] --;
				cv->HAindex_p --;
				break;
			case MODE_ZALPHA:
			case MODE_HALPHA:
				break;
			default:
				break;
		}
	}
	display_count_clear ();
}

/*
 *	exec_romaji (c)
 *		perform romaji alphabet to kana conversion
 *		and perform display romaji kana handling
 *		and perform three types of buffering
 *	int	c;	input alphabet character
 *	char	ZKout;	output zenkaku katakana
 *	char	ZHout;	output zenkaku hiragana
 *	char	HKout;	output hankaku katakana
 */

exec_romaji (c, c2)
register int	c, c2;
{
	register Conversion	*cv;
	char	ZKout[RSIZE];	/* zenkaku katakana buffer	*/
	char	ZHout[RSIZE];	/* zenkaku hiragana buffer	*/
	char	HKout[RSIZE];	/* hankaku katakana buffer	*/
	char	ZAout[RSIZE];	/* zenkaku alphabet buffer	*/
	char	HAout[RSIZE];	/* hankaku alphabet buffer	*/

	extern int	vecho;	/* 1989.05.15	*/
	extern int	converted_characters;
	register int	stat;
	char		s[3];		/* temporary buffer		*/
	char		Zbuff[RSIZE];
	int		len, Zlen, Hlen, Alen, n_flag;
	register int	cc;
	register int	i;
	int		before;
	int		not_kana_ten;
	char		*output;

	cv = GetConversion ();
	len = 0;
	if (!isprint (c) && !iskana (c) && !iskanji (c))
		return (0);
	cv->Dmojimod[cv->moji_count] = cv->Imode;

	if (cv->Imode == MODE_SCODE || cv->Imode == MODE_ECODE ||
	    cv->Imode == MODE_JCODE || cv->Imode == MODE_KCODE) {
		if (!iskanji (c)) {
			if (isupper (c))
				cc = tolower (c);
			else
				cc = c;
			if (!isdigit (cc) && !('a' <= cc && cc <= 'f')) {
				cv->Halpha[cv->ha_point --] = '\0';
				beep ();
				return (0);
			}
		}
	}

	*HAout = '\0';
	not_kana_ten = 1;
	if (iskana (c)) {
		if (c == CHAR_TEN && kana_checkspan () && BuffYes ()) {
			before = cv->Halpha[cv->ha_point - 2];
			if (ZKavail_ten (before)) {	/* sj2lib func	*/
				kana_modify_moji_info ();
				s[0] = before;
				s[1] = c;
				s[2] = '\0';
				HKtoZK (s, ZKout);
				display_count = 2;
				stat = KAKUTEI;
				not_kana_ten = 0;
			}
		}
		else if (c == CHAR_MARU && kana_checkspan () && BuffYes ()) {
			before = cv->Halpha[cv->ha_point - 2];
			if (ZKavail_maru (before)) {	/* sj2lib func	*/
				kana_modify_moji_info ();
				s[0] = before;
				s[1] = c;
				s[2] = '\0';
				HKtoZK (s, ZKout);
				display_count = 2;
				stat = KAKUTEI;
				not_kana_ten = 0;
			}
		}
		if (not_kana_ten)
			stat = SJ_kana (c, ZKout);
	}
	else if (iskanji (c))
		stat = SJ_kanji (c, c2, ZKout);
	else
		stat = SJ_romaji (c, ZKout, HAout);
	/*
	 *	returned status in stat
	 *	and zenkaku katakana string in out
	 */

	if (stat & KAKUTEI || stat & ZANTEI) {
		/*
		 *	there is a output
		 */
		ZKtoZH (ZKout, ZHout);
		ZKtoHK (ZKout, HKout);

		if (BuffYes () && not_kana_ten) {
			i = converted_characters;
			set_moji_info (ZKout, HKout, i, stat);
		}

		switch (cv->Imode) {
			case MODE_SCODE:
			case MODE_ECODE:
			case MODE_JCODE:
			case MODE_KCODE:
				if (display_count) {
					vbackchar (display_count);
					display_count = 0;
				}
				if (iskanji (c)) {
					CDbuff[0] = ZKout[0];
					CDbuff[1] = ZKout[1];
					CDbuff[2] = '\0';
				}
				else if (*CDbuff == '\0') {
					cv->ha_point -= 4;
					cv->HAindex_p = cv->ha_point;
					cv->Halpha[cv->ha_point] = '\0';
					beep ();
					return (0);
				}
				len = strlen (CDbuff);
				if (BuffYes ()) {
					vprintU (CDbuff, 1);
				}
				else {
					master_out (CDbuff, len);
					if (UnBuffCsave)
						UnBuffCsave = 0;
				}
				break;
			case MODE_ZHIRA:
				len = strlen (ZHout);
				if (display_count) {
					vbackchar (display_count);
					display_count = 0;
				}
				if (BuffYes ())
					vprintU (ZHout, 1);
				else {
					master_out (ZHout, len);
					if (UnBuffCsave)
						UnBuffCsave = 0;
				}
				break;
			case MODE_ZKATA:
				len = strlen (ZKout);
				if (display_count) {
					vbackchar (display_count);
					display_count = 0;
				}
				if (BuffYes ())
					vprintU (ZKout, 1);
				else {
					master_out (ZKout, len);
					if (UnBuffCsave)
						UnBuffCsave = 0;
				}
				break;
			case MODE_HKATA:
				len = strlen (HKout);
				if (display_count) {
					vbackchar (display_count);
					display_count = 0;
				}
				if (BuffYes ())
					vprintU (HKout, 0);
				else {
					master_out (HKout, len);
					if (UnBuffCsave)
						UnBuffCsave = 0;
				}
				break;
			default:
				if (iskanji (c)) {
					len = strlen (ZKout);
					if (display_count) {
						vbackchar (display_count);
						display_count = 0;
					}
					if (BuffYes ())
						vprintU (ZKout, 1);
					else {
						master_out (ZKout, len);
						if (UnBuffCsave)
							UnBuffCsave = 0;
					}
				}
				else if (iskana (c)) {
					len = strlen (HKout);
					if (display_count) {
						vbackchar (display_count);
						display_count = 0;
					}
					if (BuffYes ())
						vprintU (HKout, 0);
					else {
						master_out (HKout, len);
						if (UnBuffCsave)
							UnBuffCsave = 0;
					}
				}
				break;
		}
	}

	if (iskana (c) || iskanji (c))
		return;

	*s = c;
	s[1] = '\0';
	switch (cv->Imode) {
		/*
		 *	 display a romaji alphabet character
		 */
		case MODE_ZALPHA:
			HAtoZA (s, ZAout);
			if (BuffYes ())
				vprintU (ZAout, 1);
			else
				master_out (ZAout, strlen (ZAout));
			break;
		case MODE_HALPHA:
			if (BuffYes ())
				vprintU (s, 0);
			else
				master_out (s, strlen (s));
			break;
		case MODE_SCODE:
		case MODE_ECODE:
		case MODE_JCODE:
		case MODE_KCODE:
		case MODE_ZHIRA:
		case MODE_ZKATA:
			if (stat & ZANTEI || stat & MITEI) {
				if (!BuffYes ())
					if (!UnBuffCsave) {
						Csave ();
						UnBuffCsave = 1;
					}
				HAtoZK (s, Zbuff);
				if (vecho) {	/* 1989.05.15 */
					display_count += strlen (Zbuff);
					vprintU (Zbuff, 1);
				}
			}
			break;
		case MODE_HKATA:
			if (stat & ZANTEI || stat & MITEI) {
				if (!BuffYes ())
					if (!UnBuffCsave) {
						Csave ();
						UnBuffCsave = 1;
					}
				if (vecho) {	/* 1989.05.15 */
					display_count ++;
					vprintU (s, 0);
				}
			}
			break;
		default:
			break;
	}
	return;
}

kana_checkspan ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	if (cv->Imode != MODE_ZHIRA && cv->Imode != MODE_ZKATA)
		return (0);
	if (span_len () < 2)
		return (0);
	return (1);
}

kana_modify_moji_info ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	cv->moji_count --;
	cv->Amojilen[cv->moji_count] += 1;
	cv->HAindex[cv->HAindex_p++] = cv->moji_count;
	cv->moji_count ++;
}

set_moji_info (ZKout, HKout, convn, kmod)
u_char	ZKout[], HKout[];
int	convn, kmod;
{
	register Conversion	*cv;
	extern int	tuduri_match;
	register int	i, count, div, len;
	int		Alen, lim;

	cv = GetConversion ();
	lim = convn;
	if (kmod & MISS_MATCH) {
		div = 1;
		if (tuduri_match)
			lim = convn - tuduri_match;
	}
	else {
		div = convn;
	}
	for (count = 0 ; count < lim ; ) {
		count += div;
		Alen = cv->Amojilen[cv->moji_count] = div;

		switch (cv->Imode) {
			case MODE_ZHIRA:
			case MODE_ZKATA:
				if (kmod & MISS_MATCH)
					len = Alen * 2;
				else
					len = strlen (ZKout);
				break;
			case MODE_HKATA:
				if (kmod & MISS_MATCH)
					len = Alen;
				else
					len = strlen (HKout);
				break;
			case MODE_HALPHA:
				len = Alen;
				break;
			case MODE_ZALPHA:
				len = strlen (ZKout);
				break;
			case MODE_SCODE:
			case MODE_ECODE:
			case MODE_JCODE:
			case MODE_KCODE:
				len = 2;
				break;
			default:
				break;
		}
		cv->Dmojilen[cv->moji_count] = len;
		for (i = 0 ; i < Alen ; i ++)
			cv->HAindex[cv->HAindex_p++] = cv->moji_count;
		cv->moji_count ++;
		cv->Dmojimod[cv->moji_count] = cv->Imode;
	}
	if (lim < convn) {
		Alen = cv->Amojilen[cv->moji_count] = convn - lim;
		switch (cv->Imode) {
			case MODE_ZHIRA:
			case MODE_ZKATA:
				len = strlen (ZKout) - lim * 2;
				break;
			case MODE_HKATA:
				len = strlen (HKout) - lim;
				break;
			case MODE_HALPHA:
				len = Alen;
				break;
			case MODE_ZALPHA:
				len = strlen (ZKout) - lim * 2;
				break;
			default:
				break;
		}
		cv->Dmojilen[cv->moji_count] = len;
		for (i = 0 ; i < Alen ; i ++)
			cv->HAindex[cv->HAindex_p++] = cv->moji_count;
		cv->moji_count ++;
		cv->Dmojimod[cv->moji_count] = cv->Imode;
	}
}

HAromajiZK (spanmod, in, out)
register int	spanmod;
register u_char	*in, *out;
{
	HAromajiZK2 (spanmod, in, out, 1);
}

HAromajiZK2 (spanmod, in, out, flush_mode)
register int	spanmod;
register u_char	*in, *out;
int		flush_mode;
{
	register Conversion	*cv;
	register int	c, len;
	register int	stat;
	int		push_mode, prec;
	u_char		HHtmp[RSIZE], ZZtmp[RSIZE];

	cv = GetConversion ();
	SJ_romaji_clear ();
	push_mode = cv->Imode;
	cv->Imode = spanmod;
	*out = '\0';
	prec = 0;
	while ((c = *(in ++)) != '\0') {
		if (iskanji (c)) {
			SJ_romaji_flush (spanmod, ZZtmp, Dumm);
			ZZtmp[0] = c;
			ZZtmp[1] = *(in ++);
			ZZtmp[2] = '\0';
			strcat (out, ZZtmp);
		}
		else if (iskana (c)) {
			SJ_romaji_flush (spanmod, ZZtmp, Dumm);
			if ((c == 0xde || c == 0xdf)) {
				len = strlen (out);
				ZZtmp[0] = out[len - 2];
				ZZtmp[1] = out[len - 1];
				ZZtmp[2] = '\0';
				ZKtoHK (ZZtmp, HHtmp);
				if (strlen (HHtmp) == 1) {
					prec = *HHtmp;
					if (c == 0xde &&
						ZKavail_ten (prec)) {
						/* dakuon	*/
						out[len - 1] +=
						 ZKavail_ten (prec);
						continue;
					}
					else if (c == 0xdf &&
						ZKavail_maru (prec)) {
						/* han dakuon	*/
						out[len - 1] += 2;
						continue;
					}
				}
			}
			*HHtmp = c;
			HHtmp [1] = '\0';
			HKtoZK (HHtmp, ZZtmp);
			strcat (out, ZZtmp);
		}
		else
		{
			stat = SJ_romaji (c, ZZtmp, HHtmp);
			if (!(stat & MITEI))
				strcat (out, ZZtmp);
		}
	}
	if (flush_mode) {
		if (SJ_romaji_flush (spanmod, ZZtmp, Dumm)) {
			/* yes, Nflag		*/
			strcat (out, ZZtmp);
			SJ_romaji ('n', ZZtmp, HHtmp);
			strcat (out, ZZtmp);
		}
		else if (strlen (ZZtmp)) {
			strcat (out, ZZtmp);
		}
	}
	else {
		if (SJ_romaji_flush (spanmod, ZZtmp, Dumm)) {
			HAtoZA ("n", HHtmp);
			strcat (out, HHtmp);
		}
		else if (strlen (ZZtmp)) {
			strcat (out, ZZtmp);
			in = Dumm;
			while ((c = *(in ++)) != '\0')
				SJ_romaji (c, ZZtmp, HHtmp);
		}
	}
	cv->Imode = push_mode;
}
