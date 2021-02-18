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
 *	sj_roma.c
 *	romaji interface.
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Mon Feb 27 23:33:34 JST 1989
 */
/*
 * $Header: sj_roma.c,v 1.1 91/04/29 17:56:48 masaki Exp $ SONY;
 */

#include "sjparam.h"
#include "sj3lib.h"

struct romaji	**sj_Rtable;
int	Relement = 0;

sj_set_romajitbl (tbl, num)
struct romaji	**tbl;
int	num;
{
	sj_Rtable = tbl;
	Relement = num;
	sort_rtbl (tbl, num);
}

sj_clear_romajitbl ()
{
	Relement = 0;
}

char	romaji_buf[SHORTBUFFSIZE];
u_char	z_kana_buf[RSIZE*2];

sj_getromaji (romaji, yomi)
u_char	romaji;
u_char	*yomi;
{
	register int	i, len, remain;

	if (romaji == '\0') {
		len = strlen (romaji_buf);
		strcpy (yomi, romaji_buf);
		romaji_buf[0] = '\0';
		return (len);
	}
	len = strlen (romaji_buf);
	romaji_buf[len++] = romaji;
	romaji_buf[len] = '\0';
	remain = sj_getromaji2 (romaji_buf, z_kana_buf);
	if (remain) {
		for (i = 0 ; i <= remain ; i ++)
			romaji_buf[i] = romaji_buf[len - remain + i];
	}
	else
		romaji_buf[0] = '\0';
	len -= remain;
	if (len > 0)
		ZKtoHK (z_kana_buf, yomi);
	else
		len = 0;
	return (len);
}

