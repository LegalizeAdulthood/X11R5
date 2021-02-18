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
 *	kigou.c
 *	Copyright (c) 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Wed Feb 24 01:10:48 JST 1988
 */
/*
 * $Header: kigou.c,v 1.1 91/04/29 17:56:05 masaki Exp $ SONY;
 */

#include "common.h"
#include "key.h"

#define BEGIN_CODE	0x8140		/* Shift-JIS Zenkaku Space	*/
#define END_CODE	0xfcfc
#define INCRESE		16

static int	lastcode	= BEGIN_CODE;

IsCode (mode)
int mode;
{
	if (mode == MODE_SCODE || mode == MODE_ECODE ||
	    mode == MODE_JCODE || mode == MODE_KCODE)
		return (1);
	else
		return (0);
}

static u_short	row, col;

kigou (PushMode)
int PushMode;
{
	register Conversion	*cv;
	extern int	keyvalue;
	register int	i, increse, code, basecode;
	int		inc, kigou, num, low, hi;
	char		number[5];
	int		rval;

	cv = GetConversion ();
	increse = INCRESE;
	if (cv->Imode != MODE_SCODE) {
		ChangeMode (MODE_SCODE);
		disp_mode ();
	}
	basecode = lastcode & 0xfff0;
	code = lastcode;
	StartGuide (&row, &col);
	Clear_EOL ();
	guide_kigou (basecode, code, increse);

	rval = 0;
	kigou = 1;
	SaveConversion ();
	while (kigou) {
		inc = inkey ();
		cv = GetConversion ();
		if (AnotherConversion ()) {
			unget_key (inc, keyvalue);
#ifdef SJX
			PopGotoTxtwin ();
			PopConversion ();
			ChangeSpan ();
			ChangeMode (PushMode);
			SetConversion (cv);
			disp_mode ();
			return (rval);
#endif	/* SJX	*/
			break;
		}
top:
		switch (keyvalue) {
			case KEY_CONV:
				rval = 1;
				kigou = 0;
				break;
			case KEY_RIGHT:
				if (code >= END_CODE - 1) {
					beep ();
					break;
				}
				code ++;
				if (code >= basecode + increse) {
					basecode += increse;
					low = basecode & 0xff;
					if (low < 0x40) {
						basecode &= 0xff00;
						basecode |= 0x0040;
					}
					if (basecode > 0x9ff0 &&
					    basecode < 0xe040)
						basecode = 0xe040;
					code = basecode;
					guide_kigou (basecode, code, increse);
				}
				else {
					forw_kigou (code - 1);
				}
				break;
			case KEY_LEFT:
				if (code <= BEGIN_CODE) {
					beep ();
					break;
				}
				code --;
				if (code < basecode) {
					basecode -= increse;
					low = basecode & 0xff;
					if (low < 0x40) {
						basecode -= 0x40;
						basecode &= 0xff00;
						basecode |= 0x00f0;
					}
					if (basecode > 0x9ff0 &&
					    basecode < 0xe040)
						basecode = 0x9ff0;
					code = basecode + increse - 1;
					guide_kigou (basecode, code, increse);
				}
				else {
					back_kigou (1, code);
				}
				break;
			case KEY_UP:
				if (basecode <= BEGIN_CODE) {
					beep ();
					break;
				}
				basecode -= increse;
				low = basecode & 0xff;
				if (low < 0x40) {
					basecode -= 0x40;
					basecode &= 0xff00;
					basecode |= 0x00f0;
				}
				if (basecode > 0x9ff0 && basecode < 0xe040)
					basecode = 0x9ff0;
				code = basecode;
				guide_kigou (basecode, code, increse);
				break;
			case KEY_DOWN:
				if (basecode >= END_CODE - increse) {
					beep ();
					break;
				}
				basecode += increse;
				low = basecode & 0xff;
				if (low < 0x40) {
					basecode &= 0xff00;
					basecode |= 0x0040;
				}
				if (basecode > 0x9ff0 && basecode < 0xe040)
					basecode = 0xe040;
				code = basecode;
				guide_kigou (basecode, code, increse);
				break;
			case KEY_HENKAN:
				input_kigou (basecode, code, increse);
				break;
			case KEY_KETTEI:
				input_kigou (basecode, code, increse);
				kigou = 0;
				break;
			default:
				if ((keyvalue = IsCKey (inc)) != KEY_NORMAL)
					goto top;
				if (IsHex (inc)) {
					number[0] = inc;
					number[3] = 0;
					for (i = 0 ; i < 3 ; i ++) {
						number[i+1] = '\0';
						inc = inkey ();
						cv = GetConversion ();
						if (AnotherConversion ()) {
						    unget_key (inc, keyvalue);
#ifdef SJX
						    PopGotoTxtwin ();
						    PopConversion ();
						    ChangeSpan ();
						    ChangeMode (PushMode);
						    SetConversion (cv);
						    disp_mode ();
						    return (rval);
#endif	/* SJX	*/
						    break;
						}
						if (IsHex (inc))
							number[i+1] = inc;
						else {
							beep ();
							break;
						}
					}
					if (number[3] != '\0') {
						number[4] = '\0';
						sscanf (number, "%x", &num);
						if (checkcode (num)) {
							beep ();
							break;
						}
						basecode = num & 0xfff0;
						code = num;
						guide_kigou (basecode,
							code, increse);
					}
				}
				else if (inc == ESC)
					kigou = 0;
				else if (inc == '\n' || inc == '\r' ||
					 inc == ' ')
					input_kigou (basecode, code, increse);
				else if (IsRedraw (inc))
					guide_kigou (basecode, code, increse);
				else
					beep ();
				break;
		}
	}

	EndGuide (row, col);
	ChangeSpan ();
	ChangeMode (PushMode);
	disp_mode ();
	return (rval);
}

IsRedraw (c)
int	c;
{
	if (c == 0x0c)
		return (1);
	else
		return (0);
}

IsCKey (inc)
register int	inc;
{
	switch (inc) {
		case 'l':
			return (KEY_RIGHT);
		case 'k':
			return (KEY_UP);
		case 'j':
			return (KEY_DOWN);
		case 'h':
			return (KEY_LEFT);
		default:
			return (KEY_NORMAL);
	}
}

guide_kigou (bcode, code, increse)
register int	bcode, code, increse;
{
	register int	i;
	extern char	Mguide[];
	char		tmp[20];

	TopGuide ();
	printR (Mguide);
	put_space (1);
	put_space (1);
	for (i = 0 ; i < increse ; i ++)
		pkigou (bcode + i);
	sprintf (tmp, "  [%04x]", code);
	SJ_print (tmp);
	backspace (8 + (bcode + increse - code) * 2);
#ifdef SJX
	tmp[0] = code >> 8;
	tmp[1] = code & 0xff;
	tmp[2] = '\0';
	CursorOn (tmp);
#endif /* SJX */
	Flush ();
}

back_kigou (n, code)
register int	n, code;
{
#ifdef SJX
	char	tmp[3];

	CursorOff ();
#endif /* SJX */
	backspace (n * 2);
#ifdef SJX
	tmp[0] = code >> 8;
	tmp[1] = code & 0xff;
	tmp[2] = '\0';
	CursorOn (tmp);
#endif /* SJX */
	Flush ();
}

forw_kigou (code)
register int	code;
{
#ifdef SJX
	char	tmp[3];

	CursorOff ();
#endif /* SJX */
	pkigou (code);
#ifdef SJX
	code ++;
	tmp[0] = code >> 8;
	tmp[1] = code & 0xff;
	tmp[2] = '\0';
	CursorOn (tmp);
#endif /* SJX */
	Flush ();
}

pkigou (code)
register int	code;
{
	u_char	tmp[3];

	if (checkcode (code))
		code = BEGIN_CODE;
	tmp[0] = code >> 8;
	tmp[1] = code & 0xff;
	tmp[2] = '\0';
	SJ_print (tmp);
}

input_kigou (basecode, code, increse)
register int	basecode, code, increse;
{
	register Conversion	*cv;
	u_char	s[2];

	cv = GetConversion ();
	if (checkcode (code)) {
		beep ();
		return;
	}
	EndGuide (row, col);
	s[0] = code >> 8;
	s[1] = code & 0xff;
	if (BuffYes ()) {
		cv->Halpha[cv->ha_point++] = s[0];
		cv->Halpha[cv->ha_point++] = s[1];
		cv->Halpha[cv->ha_point] = '\0';
		exec_romaji ((int)s[0], (int)s[1]);
	}
	else {
		master_out (s, 2);
	}
	StartGuide (&row, &col);
	guide_kigou (basecode, code, increse);
	lastcode = code;
}

