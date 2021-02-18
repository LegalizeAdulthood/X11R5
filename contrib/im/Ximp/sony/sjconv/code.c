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
 *	code.c
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi
 *	Tue Jun 16 20:11:21 JST 1987
 */
/*
 * $Header: code.c,v 1.1 91/04/29 17:55:57 masaki Exp $ SONY;
 */

#include "sj2.h"
#include "key.h"

int		def_code = MODE_SCODE;
static int	cur_code = MODE_SCODE;

#define BEGIN_CODE	0x8140
#define END_CODE	0xfcfc

getcode (str, dest)
char	*str, *dest;
{
	int	val;

	sscanf (str, "%x", &val);
	if (cur_code == MODE_ECODE)
		val = euc (val);
	else if (cur_code == MODE_JCODE)
		val = jis (val);
	else if (cur_code == MODE_KCODE)
		val = kuten (str);
	if (checkcode (val))
		return (1);
	dest[0] = val >> 8;
	dest[1] = val & 0xff;
	dest[2] = '\0';
	return (0);
}

checkcode (val)
register int	val;
{
	register int	low;

	low = val & 0xff;
	if (val < BEGIN_CODE || val > END_CODE || checklow (low))
		return (1);
	return (0);
}

checklow (low)
register int	low;
{
	if (low == 0x7f || low < 0x40 || low > 0xfc)
		return (1);
	else
		return (0);
}

kuten (s)
char	*s;
{
	register int	val;

	val = atoi (s);
	val = ((val/100) << 8) | (val%100);
	val += 0x2020;
	val = jis (val);
	return (val);
}

jis (val)
int	val;
{
	register int	high, low;
	register int	nh, nl;

	high = (val >> 8) & 0xff;
	low = val & 0xff;
	nh = ((high - 0x21) >> 1) + 0x81;
	if (nh > 0x9f)
		nh += 0x40;
	if (high & 1) {
		nl = low + 0x1f;
		if (low > 0x5f)
			nl ++;
	}
	else
		nl = low + 0x7e;
	return((nh << 8) | nl);
}

euc (val)
int	val;
{
	return (jis (val & 0x7f7f));
}

CODEtoKANJI (in, out, white)
register u_char	*in, *out;
int		white;
{
	int	val;

	*out = '\0';

	sscanf (in, "%x", &val);
	if (cur_code == MODE_ECODE)
		val = euc (val);
	else if (cur_code == MODE_JCODE)
		val = jis (val);
	else if (cur_code == MODE_KCODE)
		val = kuten (in);
	if (checkcode (val)) {
		if (white) {
			out[0] = 0x81;
			out[1] = 0x40;
			out[2] = '\0';
		}
		return;
	}
	out[0] = val >> 8;
	out[1] = val & 0xff;
	out[2] = '\0';
}

static int	push_code;

PushCode (code)
int	code;
{
	push_code = cur_code;
	cur_code = code;
}

PopCode ()
{
	cur_code = push_code;
}

ChangeCode (code)
int	code;
{
	cur_code = code;
}
