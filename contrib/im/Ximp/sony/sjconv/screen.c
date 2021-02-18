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
 *	screen.c
 *	display independent screen handling
 *	Copyright (c) 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Wed Aug  3 11:06:01 JST 1988
 */
/*
 * $Header: screen.c,v 1.1 91/04/29 17:56:08 masaki Exp $ SONY;
 */

#include "common.h"
#include "key.h"

#define LINE_LENGTH	80

static char	*Guide;		/* Guide Line, final row	*/
static char	Nguide[LINE_LENGTH + 1];
static char	Cguide[LINE_LENGTH + 1];
char		Mguide[LINE_LENGTH + 1];

/* virtual back characters	*/

vbackchar (n)
register int	n;
{
	register int	i;

	for (i=0 ; i < n ; i++)
		VBackspace ();
	for (i=0 ; i < n ; i++)
		Vput_space ();
	for (i=0 ; i < n ; i++)
		VBackspace ();
	Flush ();
}

/* wrap				*/

wrap_off ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	cv->wrap = 0;
	cv->PushCurrentVcol = cv->CurrentVcol;
}

wrap_on ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	cv->wrap = 1;
	cv->CurrentVcol = cv->PushCurrentVcol;
}

/* virtual put space		*/

Vput_space ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	put_space (1);
	cv->Vindex[cv->Vlen ++] = cv->CurrentVcol ++;
	if (cv->wrap && ((cv->CurrentVcol % cv->column) == 1)) {
		put_space (1);
		Backspace ();
	}
}

/* virtual backspace		*/

VBackspace ()
{
	register Conversion	*cv;
	register int	i;

	cv = GetConversion ();
	if (cv->wrap && ((cv->CurrentVcol % cv->column) == 1)) {
		Ccheck ();
		i = cv->Vindex[cv->Vlen - 1] % cv->column;
		if (cv->Vlen > 0 && i > 1)
			CursorSet (
			    cv->SavedRow + (cv->CurrentVcol / cv->column) - 1,
			    cv->Vindex[cv->Vlen - 1] % cv->column
			);
		else
			CursorSet (
			    cv->SavedRow + (cv->CurrentVcol / cv->column) - 1,
			    cv->column
			);
	}
	else
		Backspace ();
	if (cv->Vlen > 0)
		cv->CurrentVcol = cv->Vindex[--(cv->Vlen)];
	else {
		cv->Vlen = 0;
		cv->CurrentVcol = cv->Vindex[cv->Vlen];
	}
}

/* master output		*/

master_out (s, n)
char	*s;
int	n;
{
	master_write (s, n);
	master_flush ();
}

master_write (s, n)
char	*s;
int	n;
{
	SJ_write (s, n);
}

/* virtual printU		*/

vprintU (s, mod)
char	*s;
char	mod;	/* 0: hankaku, 1: zenkaku	*/
{
	register Conversion	*cv;
	register int		i, col, oldcol, len;
	register u_short	*p;

	cv = GetConversion ();
	printU (s);
	oldcol = cv->CurrentVcol % cv->column;
	if (cv->wrap && (mod == 1) && (oldcol == 0))
		cv->CurrentVcol ++;
	len = strlen (s);
	p = &(cv->Vindex[cv->Vlen]);
	cv->Vlen += len;
	for (i = 0 ; i < len ; i ++)
		*(p ++) = cv->CurrentVcol ++;
	if (cv->wrap) {
		col = cv->CurrentVcol % cv->column;
		if (col == 1) {
			put_space (1);
			Backspace ();
		}
		if ((mod == 1) && (oldcol + len > cv->column)
		 && ((col & 1) == 0)) {
			cv->CurrentVcol ++;
		}
	}
	*p = cv->CurrentVcol;
	if (cv->CurrentVcol > cv->MaxVcol)
		cv->MaxVcol = cv->CurrentVcol;
	Flush ();
}

/* virtual printR		*/

vprintR (s, mod)
char	*s;
char	mod;	/* 0: hankaku, 1: zenkaku	*/
{
	register Conversion	*cv;
	register int		i, col, oldcol, len;
	register u_short	*p;

	cv = GetConversion ();
	printR (s);
	oldcol = cv->CurrentVcol % cv->column;
	if (cv->wrap && (mod == 1) && (oldcol == 0))
		cv->CurrentVcol ++;
	len = strlen (s);
	p = &(cv->Vindex[cv->Vlen]);
	cv->Vlen += len;
	for (i = 0 ; i < len ; i ++)
		*(p ++) = cv->CurrentVcol ++;
	if (cv->wrap) {
		col = cv->CurrentVcol % cv->column;
		if (col == 1) {
			put_space (1);
			Backspace ();
		}
		if ((mod == 1) && (oldcol + len > cv->column)
		 && ((col & 1) == 0)) {
			cv->CurrentVcol ++;
		}
	}
	*p = cv->CurrentVcol;
	if (cv->CurrentVcol > cv->MaxVcol)
		cv->MaxVcol = cv->CurrentVcol;
	Flush ();
}

/* index printR		*/

IprintU (s, index)
char	*s;
int	index;
{
	Cgoto (index);
	printU (s);
	Flush ();
}

/* index printR		*/

IprintR (s, index)
char	*s;
int	index;
{
	Cgoto (index);
	printR (s);
	Flush ();
}

/* print with a under score	*/

printU (s)
char	*s;
{
	under_in ();
	SJ_print (s);
	under_out ();
}

/* print with a reverse		*/

printR (s)
char	*s;
{
	reverse_in ();
	SJ_print (s);
	reverse_out ();
}

/* cursor save and load position	*/

Csave ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	CursorRead (&(cv->SavedRow), &(cv->SavedCol));
	cv->CurrentVcol = cv->SavedCol;
	cv->Vlen = 0;
	cv->Vindex[cv->Vlen] = cv->MaxVcol = cv->CurrentVcol;
}

SetVcol (index)
int	index;
{
	register Conversion	*cv;

	cv = GetConversion ();
	cv->Vlen = index;
	cv->CurrentVcol = cv->Vindex[cv->Vlen];
}

Cload ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	CursorSet (cv->SavedRow, cv->SavedCol);
}

Cclear (redraw)
int	redraw;
{
	register Conversion	*cv;
	register int	spaces;
	u_short		row, col;

#ifdef SJX
	if (redraw == 1 || IsTxtWindow ()) {
#else /* SJX */
	if (redraw) {
#endif /* SJX */
		cv = GetConversion ();
		CursorRead (&row, &col);
		spaces = (row - cv->SavedRow) * cv->column
			+ (col - cv->SavedCol) + 1;
		Cload ();
		put_space (spaces);
	}
#ifdef SJX
	if (redraw != 2 || IsTxtWindow ())
#endif /* SJX */
	{
		Cload ();
		Flush ();
	}
}

Cgoto (index)
register int	index;
{
	register Conversion	*cv;
	register int	row, col;

	cv = GetConversion ();
	SetVcol (index);
	row = cv->SavedRow + (int)(cv->CurrentVcol / cv->column);
	col = cv->CurrentVcol % cv->column;
	CursorSet (row, col);
}

ClearToMax ()
{
	register Conversion	*cv;
	register int	space;

	cv = GetConversion ();
	space = cv->MaxVcol - cv->CurrentVcol;
	put_space (space);
}

Ccheck ()
{
	register Conversion	*cv;
	register int	row, tmp, diff;

	if (Cscroll ()) {
		cv = GetConversion ();
		tmp =  (int)(cv->CurrentVcol / cv->column);
		diff = cv->CurrentVcol - (tmp * cv->column);
		if (diff == 0)
			tmp --;
		row = cv->SavedRow + tmp;
		if (row >= cv->line)
			cv->SavedRow -= (row - cv->line);
	}
}

spaceR (s)
register char	*s;
{
	register char	*t;
	char		tmp[40];

	t = tmp;
	while (*s != '\0') {
		if (*s == ' ') {
			*t = '\0';
			printR (tmp);
			t = tmp;
			put_space (1);
		}
		else
			*(t ++) = *s;
		s ++;
	}
	*t = '\0';
	printR (tmp);
	Flush ();
}

/* Guide Line Routine		*/

clear_guide_line ()
{
	u_short	row, col;

	StartGuide (&row, &col);
	Clear_EOL ();
	EndGuide (row, col);
	Flush ();
}

print_guide_line ()
{
	u_short	row, col;

	StartGuide (&row, &col);
	spaceR (Guide);
	Clear_EOL ();
	EndGuide (row, col);
	Flush ();
}

set_guide_line (mode)
int	mode;
{
	/*
	 * mode: KEY_CONV: convert status
	 *	 KEY_NORMAL: normal staus
	 */
	extern char	*SJrun;

	switch (mode) {
		case KEY_NORMAL:
			strcpy (Nguide, SJrun);
			Guide = Nguide;
			break;
		case KEY_CONV:
			Guide = Cguide;
			break;
		default:
			break;
	}
}

disp_mode ()
{
	register Conversion	*cv;
	extern int	sj;
	extern int	cur_code;
	extern char	*Mode_Zhira, *Mode_Zkata, *Mode_Hkata,
			*Mode_Zalpha, *Mode_Halpha, *Mode_buff,
			*GCshift, *GCeuc, *GCjis, *GCkuten,
			*Mode_unbuff, *Gmode;
	register char	*Pmode, *Bmode;
	int		row, col;

	cv = GetConversion ();
	switch (cv->Imode) {
		case MODE_SCODE:
			Pmode = GCshift;
			break;
		case MODE_ECODE:
			Pmode = GCeuc;
			break;
		case MODE_JCODE:
			Pmode = GCjis;
			break;
		case MODE_KCODE:
			Pmode = GCkuten;
			break;
		case MODE_ZHIRA:
			Pmode = Mode_Zhira;
			break;
		case MODE_ZKATA:
			Pmode = Mode_Zkata;
			break;
		case MODE_HKATA:
			Pmode = Mode_Hkata;
			break;
		case MODE_ZALPHA:
			Pmode = Mode_Zalpha;
			break;
		case MODE_HALPHA:
			Pmode = Mode_Halpha;
			break;
		default:
			break;
	}
	switch (cv->BuffMode) {
		case BUFF:
			Bmode = Mode_buff;
			break;
		case UNBUFF:
			Bmode = Mode_unbuff;
			break;
		default:
			break;
	}

	strcpy (Cguide, Bmode);
	strcat (Cguide, Pmode);
	strcpy (Mguide, Cguide);
	if (Hlevel > 1)
		strcat (Cguide, Gmode);

	print_guide_line ();
}

TopGuide ()
{
	u_short	row, col;

	StartGuide (&row, &col);
}

guide_print (s1, s2)
u_char	*s1, *s2;
{
	u_short	row, col;

	StartGuide (&row, &col);
	if (s1)
		printR (s1);
	SJ_print (s2);
	Clear_EOL ();
	EndGuide (row, col);
	Flush ();
}
