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
#include "sj_const.h"
#include "sj_macro.h"

static	long	lineloc;

static	int	yomi[MaxYomiLength + 1];
static	int	kanji[MaxKanjiLength + 1];
static	int	hinsi[MaxHinsiNumber + 1];
static	int	atr[MaxAtrNumber + 1];

static	error(s)
char	*s;
{
	register int	c;

	if (s) fprintf(stderr, "%s\n", s);
	mark_file(stderr);

	exit(1);
}

static	readchar()
{
	register int	c1;
	register int	c2;

	c1 = getch(); if (c1 == EOF) return EOF;

	if (Isknj1(c1)) {
		c2 = getch(); if (c2 == EOF) error(NULL);
		c1 = ((c1 & 0xff) << 8) + (c2 & 0xff);
	}

	return c1;
}

static	skip_blank()
{
	register int	c;

	c = readchar();
	while (Isblank(c)) c = readchar();

	return c;
}

static	readhinsi()
{
	static int	c = 0;
	register int	i;
	unsigned char	hinsi[128];
	int	flg;

retry:
	if (c == '\n') {
		c = 0;
		return 0;
	}

	c = skip_blank();
	if (c == '\n') {
		c = 0;
		return 0;
	}

	for (i = 0 ; !Isblank(c) ; ) {
		if (c == '\n' || c == ':') break;
		if (Isillegal(c)) error(IllegalFormat);
		if (c > 0xff) {
			if (i >= 126) error(TooLongHinsi);
			hinsi[i++] = ((c >> 8) & 0xff);
		}
		if (i >= 127) error(TooLongHinsi);
		hinsi[i++] = (c & 0xff);
		c = readchar();
	}

	if (i == 0) return 0;

	if (hinsi[0] == '[' && hinsi[i - 1] == ']') {
		hinsi[i - 1] = 0;
		i = cnvhinsi(hinsi + 1);
		if (i > 0) {
			fprintf(stderr, "品詞 \"%s\" に括弧がついている\n",
				hinsi + 1);
		}
		else if (!i) {
			fprintf(stderr, "\"%s\" がコード化できません\n",
				hinsi + 1);
			goto retry;
		}
	}
	else {
		hinsi[i] = 0;
		i = cnvhinsi(hinsi);
		if (!i) {
			fprintf(stderr, "\"%s\" がコード化できません\n", hinsi);
			goto retry;
		}
	}

	return i;
}

int	*readline()
{
	register int	c;
	register int	i;
	register int	j;

	for ( ; ; ) {
		mark_file(NULL);

		c = skip_blank();

		if (c == '#') {
			while (c != '\n') c = skip_blank();
		}

		if (c == EOF) return NULL;

		if (c != '\n') break;
	}

	for (i = 0 ; !Isblank(c) ; ) {
		if (Isillegal(c)) error(IllegalFormat);
		if (i >= MaxYomiLength) error(TooLongYomi);
		yomi[i++] = c;
		c = readchar();
	}
	yomi[i] = 0;
	if (i == 0) error(NoYomiString);

	c = skip_blank();

	if (c != '"') {
		for (i = 0 ; !Isblank(c) ; ) {
			if (Isillegal(c)) error(IllegalFormat);
			if (i >= MaxKanjiLength) error(TooLongKanji);
			kanji[i++] = c;
			c = readchar();
		}
	}
	else {
		c = readchar();
		for (i = 0 ; c != '"' ; ) {
			if (Isillegal(c)) error(IllegalFormat);
			if (i >= MaxKanjiLength) error(TooLongKanji);
			kanji[i++] = c;
			c = readchar();
		}
	}
	kanji[i] = 0;
	if (i == 0) error(NoKanjiString);

	i = j = 0;
	while (c = readhinsi()) {
		if (c < 0) {
			if (i >= MaxAtrNumber) error(TooManyAtr);
			atr[i++] = -c;
		}
		else {
			if (j >= MaxHinsiNumber) error(TooManyHinsi);
			hinsi[j++] = c;
		}
	}
	atr[i] = 0;
	if (j == 0) hinsi[j++] = HinsiTankan;
	hinsi[j] = 0;

	return yomi;
}

setline(func)
int	(*func)();
{
	register int	i;

	for (i = 0 ; hinsi[i] ; i++) (*func)(yomi, kanji, hinsi[i], atr);
}
