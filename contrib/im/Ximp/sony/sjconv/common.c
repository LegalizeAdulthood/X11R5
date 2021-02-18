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
 *	common.c
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Wed May  6 16:45:52 JST 1987
 */
/*
 * $Header: common.c,v 1.1 91/04/29 17:55:59 masaki Exp $ SONY;
 */

#include "conversion.h"

u_char	PZbuff[BUFFLENGTH * 2];
u_char	PHbuff[BUFFLENGTH];
u_char	Object[BUFFLENGTH * 2];

int	allways_buff = 0;		/* allways buff mode		*/
int	no_flush = 0;			/* kettei flush mode		*/
int	termflag;			/* terminal identifier		*/
int	HHlevel = 2;
int	Hlevel = 2;			/* help level			*/
int	oasys = 0;			/* OYAYUBI Shift		*/
int	flush_conversion;		/* Flush After Conversion	*/

Conversion	*current_conversion;
Conversion	*default_conversion;

Conversion *
AllocConversion ()
{
	register Conversion	*c;

	if ((c = (Conversion *)malloc (sizeof(Conversion))) == NULL) {
		fprintf (stderr, "conversion: can't alloc memory\n\r");
		return (0);
	}

	c->Imode = MODE_ZHIRA;
	c->BuffMode = BUFF;
	c->ha_point = 0;
	c->moji_count = 0;
	c->span_point = 0;
	c->word_regist = 0;
	c->Cflag = 1;

	c->SavedRow = 0;
	c->SavedCol = 0;
	c->CurrentVcol = 0;
	c->MaxVcol = 0;
	c->wrap = 1;
	return (c);
}

static Conversion	*push_conversion;

FreeConversion (c)
Conversion	*c;
{
	if (c == push_conversion)
		push_conversion = 0;
	free (c);
}

SetConversion (c)
Conversion	*c;
{
	PushConversion (current_conversion);
	current_conversion = c;
}

PushConversion (c)
Conversion	*c;
{
	push_conversion = c;
}

PopConversion ()
{
	if (push_conversion)
		current_conversion = push_conversion;
}

Conversion *
GetConversion ()
{
	return (current_conversion);
}

InitConversion ()
{
	default_conversion = AllocConversion ();
	DefaultConversion ();
}

DefaultConversion ()
{
	current_conversion = default_conversion;
}

EndConversion ()
{
	free (default_conversion);
}

