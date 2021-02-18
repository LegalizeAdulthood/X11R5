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
 *	etc.c
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Mon Jun 22 10:25:16 JST 1987
 */
/*
 * $Header: etc.c,v 1.1 91/04/29 17:56:02 masaki Exp $ SONY;
 */

#include "common.h"
#include "key.h"

extern int	keyvalue;
extern char	*GEtc, *Getc, *GHelp, *MHelp;

exec_etc (key)
int	key;
{
	u_short		row, col;
	register int	inc;
	int 		edited;
	int		etckey;
	register Conversion	*cv;

	cv = GetConversion ();

	edited = 0;

	if (key == KEY_ETC) {
		StartGuide (&row, &col);
		printR (GEtc);
		if (cv->column >= MINCOL)
			spaceR (Getc);
		Clear_EOL ();
		EndGuide (row, col);
		SaveConversion ();
		inc = inkey ();
		if (AnotherConversion ())
			return;
		cv = GetConversion ();
		etckey = etc_map (keyvalue);
	}
	else {
		etckey = key;
	}

	switch (etckey) {
		case KEY_NORMAL:
normal:
			switch (inc) {
				case 'h':
					helplevel ();
					break;
				case 't':
					if (!IsCflag ())
						exec_toroku ();
					break;
				case 's':
					if (!IsCflag ())
						exec_syoukyo ();
					break;
				default:
					break;
			}
			break;
		case KEY_HELP:
			helplevel ();
			break;
		case KEY_TOROKU:
			if (!IsCflag ())
				exec_toroku ();
			break;
		case KEY_SYOUKYO:
			if (!IsCflag ())
				exec_syoukyo ();
			break;
		case KEY_SJRC:
			if (IsCflag ())
				exec_sjrc ();
			break;
		case KEY_EDIT:
			if (IsHenkan ()) {
				if (cv->CurBun < 0) {
					cv->CurBun = 0;
					Bdisp (1);
				}
				edited = exec_edit ();
				EndGuide (row, col);
			}
			break;
		case KEY_CONV:
			break;
		default:
			if ((inc = write_pseq (2)) != 0)
				goto normal;
			break;
	}

	if (edited) {

		if (!Sdouon (3))	/* 0: No Choise		*/
			Rdisp ();
	}
}

etc_map (key)
int	key;
{
	extern int	u_etckeys;
	extern int	Uetckey[], Uetcval[];
	register int	i;
	int		val;

	for (i = 0 ; i < u_etckeys ; i ++) {
		if (key == Uetckey[i])
			return (Uetcval[i]);
	}

	switch (key) {
		case KEY_HENKAN:
			val = KEY_TOROKU;
			break;
		case KEY_MUHENKAN:
			val = KEY_SYOUKYO;
			break;
		case KEY_ETC:
			val = KEY_HELP;
			break;
		case KEY_KETTEI:
			val = KEY_SJRC;
			break;
		case KEY_CODE:
			val = KEY_EDIT;
			break;
		default:
			val = KEY_NORMAL;
			break;
	}
	return (val);
}

helplevel ()
{
	u_char			tmp[BUFFLENGTH];
	register Conversion	*cv;
	int			c;

	cv = GetConversion ();

	if (Hlevel == 1 && cv->column >= MINCOL) {
		HHlevel = 2;
		Hlevel = 2;
		sprintf (tmp, MHelp, "on");
	}
	else {
		HHlevel = 1;
		Hlevel = 1;
		sprintf (tmp, MHelp, "off");
	}
	guide_print_pause (GHelp, tmp);
}

exec_sjrc ()
{
	extern char	RCfile[];
	extern char	*SUCCsjrc, *FAILsjrc;
	u_char		tmp[BUFFLENGTH];

	if (*RCfile == (char)NULL)
		return;
	if (getsjrc () == TRUE)
		sprintf (tmp, "%s %s ", RCfile, SUCCsjrc);
	else
		sprintf (tmp, "%s %s ", RCfile, FAILsjrc);
	guide_print_pause (0, tmp);
}

guide_print_pause (s1, s2)
u_char	*s1, *s2;
{
	int	c;

	guide_print (s1, s2);
	SaveConversion ();
	c = inkey ();
	if (AnotherConversion ()) {
		unget_key (c, keyvalue);
		return (EOF);
	}
	return (c);
}
