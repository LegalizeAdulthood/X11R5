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
 *	toroku.c
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Fri Jul 17 19:53:53 JST 1987
 */
/*
 * $Header: toroku.c,v 1.1 91/04/29 17:56:13 masaki Exp $ SONY;
 */

#include "common.h"
#include "key.h"

#define YOMILEN	32
#define KLEN	64
#define H_CODE	1	/* meisi	*/

extern int	keyvalue;

exec_toroku ()
{
	register Conversion	*cv;
	extern char		*TOUROKU_FAIL, *TOUROKU_SUCC;
	extern char		*Gtoroku;
	char			yomi[YOMILEN+1], kanji[KLEN+1];
	int			inc;
	int			val;

	cv = GetConversion ();
	if (getkstr (kanji, KLEN, Gtoroku))
		return (1);
	if (*kanji == '\0')
		return (0);

	while (1) {
		if (getyomi (yomi, YOMILEN, Gtoroku, kanji))
			return (1);
		if (*yomi == '\0')
			return (0);
		if ((val = getcheck (yomi, kanji, Gtoroku)) == 2)
			return (1);
		else if (val == 1)
			continue;
		else
			break;
	}
	if (*yomi == '\0')
		return (0);

	TopGuide ();
	printR (Gtoroku);
	Clear_EOL ();
	Flush ();
	cv->word_regist = 1;
	if (SJ2_toroku (yomi, kanji, (char) H_CODE))
		SJ_print (TOUROKU_FAIL);
	else
		SJ_print (TOUROKU_SUCC);
	Clear_EOL ();
	Flush ();

	inc = inkey ();
	if (AnotherConversion ()) {
		unget_key (inc, keyvalue);
#ifdef SJX
		PopGotoTxtwin ();
#endif	/* SJX	*/
	}
	return (0);
}

getyomi (yomi, lim, prompt, kanji)
char	*yomi, *prompt, *kanji;
int	lim;
{
	register int	inc, c;

	prologue (prompt, kanji);

	SaveConversion ();
	while ((inc = inkey ()) != EOF) {
		if (AnotherConversion ()) {
#ifdef SJX
			PopGotoTxtwin ();
#endif	/* SJX	*/
			unget_key (inc, keyvalue);
			return (1);
		}
		if (keyvalue == KEY_CONV) {
			epilogue ();
			return (1);
			break;
		}
		if (keyvalue == KEY_KETTEI || keyvalue == KEY_HENKAN) {
			break;
		}
		else if (keyvalue != KEY_NORMAL) {
			if ((c = write_pseq (2)) != 0)
				inc = c;
		}
		
		if (is_bsdel (inc)) {
			/* 0: if not exists in buffer, beep	*/
			/* span_len (): instead of ha_point	*/
			if (span_len () <= 0
			 || exec_backspace (inc, span_len (), 0, 0))
				beep ();
		}
		else if (inc == '\n' || inc == '\r')
			break;
		else if (IsESC (inc))
			break;
		else if (current_len () >= lim - 1)	/* for overflow	*/
			beep ();
		else if (' ' < inc && inc < '~' || iskana (inc))
			exec_conv (inc);	/* convert hiragana	*/
		else
			beep ();
	}

	get_current_span (yomi);

	epilogue ();
	return (0);
}

current_len ()
{
	u_char	Ztmp[YOMILEN * 2];

	get_current_span2 (Ztmp);
	return (strlen (Ztmp));
}

static int	PushMode, PushBmode;

prologue (prompt, kanji)
char	*prompt, *kanji;
{
	register Conversion	*cv;
	extern char		*Myomi;
	u_char			tmp[BUFFLENGTH];

	cv = GetConversion ();
	TopGuide ();
	printR (prompt);
	sprintf (tmp, Myomi, kanji);
	SJ_print (tmp);
	Clear_EOL ();
	Flush ();
	next_span ();
	wrap_off ();
	PushMode = cv->Imode;
	cv->Imode = MODE_ZHIRA;
	if (BuffYes ()) {
		PushBmode = BUFF;
	}
	else {
		PushBmode = UNBUFF;
		ChangeBuffMode ();
	}
}

epilogue ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	delete_current_span ();
	wrap_on ();
	if (PushBmode == UNBUFF)
		ChangeBuffMode ();
	cv->Imode = PushMode;
}

getcheck (yomi, kanji, prompt)
char	*yomi, *kanji, *prompt;
{
	extern char	*TOUROKU_OK;
	register int	c;
	u_char		work[BUFFLENGTH];

	TopGuide ();
	printR (prompt);
	sprintf (work, TOUROKU_OK, kanji, yomi);
	SJ_print (work);
	Clear_EOL ();
	SJ_print ("y");
	Backspace ();
	Flush ();

	SaveConversion ();
	c = inkey ();
	if (AnotherConversion ()) {
		unget_key (c, keyvalue);
#ifdef SJX
		PopGotoTxtwin ();
#endif	/* SJX	*/
		return (1);
	}

	if (keyvalue == KEY_CONV)
		return (2);

	if (c == 'y' || c == 'Y' || c == '\n' || c == '\r' || c == ' ')
		return (0);
	if (keyvalue == KEY_HENKAN || keyvalue == KEY_KETTEI)
		return (0);
	if (IsESC (c)) {
		*yomi = '\0';
		return (0);
	}
	return (1);
}

getkstr (s, lim, guide)
char	*s, *guide;
int	lim;
{
	register Conversion	*cv;
	char			tmp[BUFFLENGTH];
	int			t_point;
	register int		i;
	char			*ptmp;
	int			plen;

	cv = GetConversion ();
	end_span ();
	if (cv->Kanji != 0) {
		Bkanji (tmp);
		if (cv->Bnum > 1) {
			if (getbunsetu (tmp, &plen, guide))
				return (1);
		}
		else
			plen = strlen (tmp);
		ptmp = tmp;
	}
	else if (isspaned ()) {
		t_point = 0;
		for (i = 0 ; i < cv->span_point ; i ++) {
			load_span (i, &ptmp, &plen);
			Strncpy (&tmp[t_point], ptmp, plen);
			t_point += plen;
			tmp[t_point] = '\0';
		}
		ptmp = tmp;
		plen = t_point;
	}
	for (i = 0 ; i < plen && i < lim ; i ++)
		*(s ++) = *(ptmp ++);
	*s = '\0';
	return (0);
}

getbunsetu (s, len, guide)
char	*s, *guide;
int	*len;
{
	register Conversion	*cv;
	extern char		*Mhani;
	register int		c;
	register int		bstart, bend;
	u_short			row, col;
	char			*p, *Bstr ();
	int			plen;
	register int		i;

	cv = GetConversion ();
	*s = '\0';
	*len = 0;
	bstart = bend = cv->CurBun;
	/* print guide			*/
	StartGuide (&row, &col);
	printR (guide);
	SJ_print (Mhani);
	Clear_EOL ();
	EndGuide (row, col);
	Flush ();
	SaveConversion ();
	while ((c = inkey ()) != EOF) {
		if (AnotherConversion ()) {
			unget_key (c, keyvalue);
#ifdef SJX
			PopGotoTxtwin ();
#endif	/* SJX	*/
			return (1);
		}
		cv = GetConversion ();
		if (keyvalue == KEY_RIGHT) {
			if ((bend == bstart) && (bend < cv->Bnum - 1)) {
				cv->PreBun = bend;
				bend ++;
				cv->CurBun = bstart = bend;
				Rdisp ();
			}
			else
				beep ();
		}
		else if (keyvalue == KEY_LEFT) {
			if ((bend == bstart) && (bend > 0)) {
				cv->PreBun = bend;
				bend --;
				cv->CurBun = bstart = bend;
				Rdisp ();
			}
			else
				beep ();
		}
		else if (keyvalue == KEY_UP) {
			if (bend < cv->Bnum - 1) {
				bend ++;
				cv->CurBun = cv->PreBun = bend;
				Rdisp ();
			}
			else
				beep ();
		}
		else if (keyvalue == KEY_DOWN) {
			if (bend > bstart) {
				cv->PreBun = bend;
				bend --;
				cv->CurBun = bend;
				Rdisp ();
			}
			else
				beep ();
		}
		else if (keyvalue == KEY_HENKAN) {
			break;
		}
		else if (keyvalue == KEY_KETTEI) {
			break;
		}
		else if (keyvalue == KEY_NORMAL) {
			if (c == '\n' || c == '\r') {
				bstart = 0;
				bend = cv->Bnum - 1;
				break;
			}
			else if (c == ' ') {
				break;
			}
			else if (IsESC (c)) {
				return (0);
			}
			else
				beep ();
		}
		else
			beep ();
		Flush ();
	}
	/* we get bunsetu parameter	*/
	for (i = bstart ; i <= bend ; i ++) {
		if ((p = Bstr (i, &plen)) == (char *)0 || plen == 0)
			continue;
		strncat (s, p, plen);
	}
	*len = strlen (s);
	return (0);
}

exec_syoukyo ()
{
	extern char	*SYOUKYO_FAIL, *SYOUKYO_SUCC;
	extern char	*Gsyoukyo;
	char		yomi[YOMILEN], kanji[KLEN];
	int		inc;
	int		val;

	if (getkstr (kanji, KLEN, Gsyoukyo))
		return (1);
	if (*kanji == '\0')
		return (0);

	while (1) {
		if (getyomi (yomi, YOMILEN, Gsyoukyo, kanji))
			return (1);
		if (*yomi == '\0')
			return (0);
		if ((val = getcheck (yomi, kanji, Gsyoukyo)) == 2)
			return (1);
		else if (val == 1)
			continue;
		else
			break;
	}
	if (*yomi == '\0')
		return (0);

	TopGuide ();
	printR (Gsyoukyo);
	if (SJ2_syoukyo (yomi, kanji))
		SJ_print (SYOUKYO_FAIL);
	else
		SJ_print (SYOUKYO_SUCC);
	Clear_EOL ();
	Flush ();

	inc = inkey ();
	if (AnotherConversion ()) {
		unget_key (inc, keyvalue);
#ifdef SJX
		PopGotoTxtwin ();
#endif	/* SJX	*/
	}
	return (0);
}
