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
 *	comm.c
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Wed Sep 20 16:50:56 JST 1989
 */
/*
 * $Header: comm.c,v 1.1 91/04/29 17:55:58 masaki Exp $ SONY;
 */

#include "sj2.h"
#include "key.h"

int	comvalue[COMNUM];
char	comkey[COMNUM];
int	comnumber = 0;

char	*comstr[KEY_OTHER] = {
		"start",	"henkan",	"muhen",
		"kettei",	"hira",		"halpha",
		"zalpha",	"hkata",	"zkata",
		"code",		"etc",		"right",
		"left",		"kaku",		"syuku",
		"edit",		"toggle",	"wrapback",
		"wrap",		"sjrc",		"buffer",
		"toroku",	"syoukyo",	"help",
		"ha-kettei",	"flush",	"debug",
		"cancel",	"saihenkan",	"ignore",
		""
	};

int	comstrvalue[KEY_OTHER] = {
		KEY_CONV,	KEY_HENKAN,	KEY_MUHENKAN,
		KEY_KETTEI,	KEY_ZHIRA,	KEY_HALPHA,
		KEY_ZALPHA,	KEY_HKATA,	KEY_ZKATA,
		KEY_CODE,	KEY_ETC,	KEY_RIGHT,
		KEY_LEFT,	KEY_UP,		KEY_DOWN,
		KEY_EDIT,	KEY_TOGGLE,	KEY_WRAPBACK,
		KEY_WRAP,	KEY_SJRC,	KEY_BUFFER,
		KEY_TOROKU,	KEY_SYOUKYO,	KEY_HELP,
		KEY_HA_KET,	KEY_FLUSH,	KEY_DEBUG,
		KEY_CANCEL,	KEY_RECONV,	KEY_IGNORE,
		KEY_OTHER
	};

clear_key ()
{
	clear_commkeys ();
	clear_etckeys ();
	clear_ukeys ();		/* new	*/
}

clear_commkeys ()
{
	comnumber = 0;
}

set_comkey (word)
struct wordent	word[];
{
	register int	i, c;

	if (comnumber >= COMNUM)
		return;
	for (i = 0 ; i < KEY_OTHER && *comstr[i] != '\0' ; i ++) {
		if (mutch (word[1].word_str, comstr[i])) {
			if ((c = eval_key (word[2].word_str)) != -1) {
				comkey[comnumber] = c;
				comvalue[comnumber] = comstrvalue[i];
				comnumber ++;
				return;
			}
		}
	}
}

eval_key (s)
char	*s;
{
	register u_char	c;

	c = *s;

	/* this is control code		*/
	if (c == '^') {
		c = s[1];
		if ('@' <= c && c <= '_')
			return (c - '@');
		else if (c == '?')
			return (DEL);
	}

	/* normal code			*/
	else if (c <= '~')
		return (c);

	return (-1);
}

#define ETCKEYS	10
int	u_etckeys = 0;
int	Uetckey[ETCKEYS];
int	Uetcval[ETCKEYS];

clear_etckeys ()
{
	u_etckeys = 0;
}

set_etckeys (word)
struct wordent	word[];
{
	register int	i;

	if (u_etckeys >= ETCKEYS)
		return (1);
	for (i = 0 ; i < KEY_OTHER && *comstr[i] != '\0' ; i ++) {
		if (mutch (word[1].word_str, comstr[i])) {
			Uetcval[u_etckeys] = comstrvalue[i];
			break;
		}
	}
	if (i >= KEY_OTHER)
		return (1);
	for (i = 0 ; i < KEY_OTHER && *comstr[i] != '\0' ; i ++) {
		if (mutch (word[2].word_str, comstr[i])) {
			Uetckey[u_etckeys] = comstrvalue[i];
			break;
		}
	}
	if (i >= KEY_OTHER)
		return (1);
	u_etckeys ++;
	return (0);
}

/* check control code = command key ?	*/

parse_command (s, count)
register u_char	*s;
register int	*count;
{
	extern char	pars_seq[];
	extern int	pars_n;
	register int	c, i, j;

	c = *s;
	for (i = 0; i < comnumber ; i ++) {
		if (c == comkey[i]) {
			for (j = 1 ; j < *count ; j ++)
				s[j - 1] = s[j];
			(*count)--;
			*pars_seq = c;
			pars_n = 1;
			return (comvalue[i]);
		}
	}
	return (KEY_NORMAL);
}

