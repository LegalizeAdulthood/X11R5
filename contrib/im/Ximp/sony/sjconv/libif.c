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
 *	libif.c
 *	library interface routines.
 *	Copyright (c) 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Mon Feb 13 23:30:07 JST 1989
 */
/*
 * $Header: libif.c,v 1.1 91/04/29 17:56:06 masaki Exp $ SONY;
 */

#include "common.h"
#include "kana.h"

extern int	vflag;

#define BSIZE	BUFFLENGTH

/*
 *	Henkan Interface
 */
SJ2_henkan (kana, bun, yomiout, kanjiout)
u_char		*kana;
struct bunsetu	*bun;
u_char		*yomiout, *kanjiout;
{
	int	num;

	strcpy (yomiout, kana);
	num = sj3_getkan (yomiout, bun, kanjiout, 512);
	return (num);
}

/*
 *	Douon Interface
 */

SJ2_douoncnt (s, d, n)
char		*s;
struct douon	*d;
int		n;
{
	return (sj3_douoncnt (s));
}

SJ2_getdouon2 (s, d, n, choice, reconv)
char		*s;
struct douon	*d;
int		n;
int		choice;
int		reconv;
{
	return (sj3_getdouon (s, d));
}

SJ2_getdouon (s, d, n, choice, reconv)
char		*s;
struct douon	*d;
int		n;
int		choice;
int		reconv;
{
	return (sj3_getdouon (s, d));
}

SJ2_study (dcid)
struct studyrec	*dcid;
{
	sj3_gakusyuu (dcid);
}

SJ2_kettei ()
{
}

SJ2_clstudy (yomi, kanji, dcid)
u_char		*yomi, *kanji;
struct studyrec	*dcid;
{
	sj3_gakusyuu (yomi, kanji, dcid);
}

SJ2_toroku (yomi, kanji, hinshi)
char	*yomi, *kanji;
char	hinshi;
{
	int	r;

	r = sj3_touroku (yomi, kanji, hinshi);
	return (r);
}

SJ2_syoukyo (yomi, kanji)
char	*yomi, *kanji;
{
	int	r;

	r = sj3_syoukyo (yomi, kanji, 1);
	return (r);
}

SJ2_henkan_end ()
{
	sj3_close ();
}

SJ2_henkan_init ()
{
	int		err;
	extern char	user_name[];
	extern char	server_name[];

	err = sj3_open (server_name, user_name);
	if (err) {
		printf ("Warning: cannot connect server\n\r");
		return (1);
	}
	return (0);
}
