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
 * Author: Noriyuki Hosoe, Sony Corporation
 */

#include <stdio.h>
#include <ctype.h>
#include <locale.h>
#include "sjtool.h"

static	int	euc_mode;

init_code()
{
	char	*s;

	s = setlocale (LC_CTYPE, "");
	if (strcmp (s, "ja_JP.EUC") == 0)
		euc_mode = -1;
	else
		euc_mode = 0;
}

cnvcode(s)
unsigned char	*s;
{
	if (euc_mode) {
		unsigned short	i;
		unsigned char	*d = s;

		while (i = *s++) {
			if (iseuckana(i)) {
				if (*s) {
					*d++ = *s++;
				}
				else {
					*d++ = i;
					break;
				}
			}
			else if (iseuc(i)) {
				if (*s) {
					i = euc2sjis((i << 8) + *s++);
					*d++ = (i >> 8);
					*d++ = i;
				}
				else {
					*d++ = i;
					break;
				}
			}
			else {
				*d++ = i;
			}
		}
		*d = 0;
	}
}

sjistoeuc(s, d)
unsigned char	*s, *d;
{
	unsigned short	i;

	while (i = *s++) {
		if (isascii(i)) {
			*d++ = i;
		}
		else if (issjis1(i)) {
			if (*s) {
				i = sjis2euc((i << 8) + *s++);
				*d++ = (i >> 8);
				*d++ = i;
			}
			else {
				*d++ = i;
				break;
			}
		}
		else {
			*d++ = SingleShift2;
			*d++ = i;
		}
	}
	*d = 0;
}

printout(fp, s)
FILE	*fp;
unsigned char	*s;
{
	unsigned char	buf[BUFSIZ];

	if (euc_mode)
		sjistoeuc(s, buf);
	else
		strcpy(buf, s);
	fputs(buf, fp);
	fflush(fp);
}

normal_out(fmt, p1, p2, p3, p4, p5)
char	*fmt;
int	p1, p2, p3, p4, p5;
{
	char	buf[BUFSIZ];

	sprintf(buf, fmt, p1, p2, p3, p4, p5);
	printout(stdout, buf);
}

error_out(fmt, p1, p2, p3, p4, p5)
char	*fmt;
int	p1, p2, p3, p4, p5;
{
	char	buf[BUFSIZ];

	sprintf(buf, fmt, p1, p2, p3, p4, p5);
	strcat(buf, "\n");
	printout(stderr, buf);
}

