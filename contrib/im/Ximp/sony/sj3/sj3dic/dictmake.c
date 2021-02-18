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
#include <strings.h>
#include "sj3lib.h"

#define	IsEOL(c)	((c) == '\0')
#define	IsBlank(c)	((c) == ' ' || (c) == '\t')

extern	int	force_flag;
extern	int	verbose_flag;


static	touroku(y, k, h)
char	*y, *k, *h;
{
	int	err;
	int	grm;

	if (!(grm = str2hns(h))) {
		error_out("\225\151\216\214\225\266\216\232\227\361\202\360\211\337\202\301\202\304\202\242\202\334\202\267 %s:%s:%s", y, k, h);
		if (force_flag) return;
		exit(1);
	}

	err = sj3_touroku(y, k, grm);

	switch (err) {
	case -1:
		error_out("\203\124\201\133\203\157\202\252\227\216\202\277\202\334\202\265\202\275");
		exit(1);

	case 0:
		if (verbose_flag)
			normal_out("%s:%s:%s \202\360\223\157\230\136\202\265\202\334\202\265\202\275\n", y, k, h);
		break;

	case SJ3_DICT_ERROR:
		error_out("\216\253\217\221\202\252\202\310\202\242\201\101\202\240\202\351\202\242\202\315\216\253\217\221\202\311\217\221\202\253\215\236\202\337\202\334\202\271\202\361");
		exit(1);

	case SJ3_DICT_LOCKED:
		error_out("\216\253\217\221\202\252\203\215\203\142\203\116\202\263\202\352\202\304\202\242\202\304\217\221\202\253\215\236\202\337\202\334\202\271\202\361");
		exit(1);

	case SJ3_WORD_EXIST:
		if (verbose_flag)
			normal_out("%s:%s:%s \202\315\223\157\230\136\202\263\202\352\202\304\202\242\202\334\202\267\n", y, k, h);
		break;

	case SJ3_DOUON_FULL:
		error_out("\202\261\202\352\210\310\217\343\223\257\211\271\214\352\202\360\223\157\230\136\202\305\202\253\202\334\202\271\202\361 %s:%s:%s", y, k, h);
		break;

	case SJ3_DICT_FULL:
		error_out("\202\261\202\352\210\310\217\343\216\253\217\221\202\360\221\345\202\253\202\255\202\305\202\253\202\334\202\271\202\361 %s:%s:%s", y, k, h);
		if (force_flag) break;
		exit(1);

	case SJ3_BAD_YOMI_STR:
		error_out("\223\157\230\136\202\305\202\253\202\310\202\242\223\307\202\335\202\305\202\267 %s", y);
		if (force_flag) break;
		exit(1);

	case SJ3_BAD_KANJI_STR:
		error_out("\223\157\230\136\202\305\202\253\202\310\202\242\212\277\216\232\202\305\202\267 %s:%s", y, k);
		if (force_flag) break;
		exit(1);

	case SJ3_BAD_HINSI_CODE:
		error_out("\223\157\230\136\202\305\202\253\202\310\202\242\225\151\216\214\202\305\202\267 %s:%s:%s", y, k, h);
		if (force_flag) break;
		exit(1);

	case SJ3_INDEX_FULL:
		error_out("\203\103\203\223\203\146\203\142\203\116\203\130\202\252\202\242\202\301\202\317\202\242\202\305\202\267");

	case SJ3_TOUROKU_FAILED:
	default:
		error_out("\223\157\230\136\202\305\202\253\202\334\202\271\202\361\202\305\202\265\202\275 %s:%s:%s(%d)",
				y, k, hns2str(h), h);
		break;
	}
}

dictmake(input)
char	*input;
{
	unsigned char	buf[BUFSIZ];
	FILE	*fp;
	unsigned char	*p;
	unsigned char	*yom, *knj, *grm;
	int	c;
	int	flg;
	static	char	*FormatErrorString = "\203\164\203\110\201\133\203\175\203\142\203\147\203\107\203\211\201\133:%s";

	if (input && *input) {
		fp = fopen(input, "r");
		if (!fp) {
			error_out("%s \202\252\203\111\201\133\203\166\203\223\202\305\202\253\202\334\202\271\202\361", input);
			exit(1);
		}
	}
	else
		fp = stdin;

	while (fgets(buf, sizeof(buf), fp)) {
		cnvcode(buf);
		if (p = (unsigned char *)index(buf, '\n'))
			*p = '\0';
		else {
			do {
				c = fgetc(fp);
			} while (c != '\n' && c != EOF);
		}

		yom = buf;
		while (IsBlank(*yom)) yom++;
		if (IsEOL(*yom)) {
			error_out(FormatErrorString, buf);
			if (force_flag) continue;
			break;
		}

		knj = yom;
		while (!IsEOL(*knj) && !IsBlank(*knj)) knj++;
		if (IsEOL(*knj)) {
			error_out(FormatErrorString, buf);
			if (force_flag) continue;
			break;
		}
		*knj++ = '\0';
		while (IsBlank(*knj)) knj++;
		if (IsEOL(*knj)) {
			error_out(FormatErrorString, buf);
			if (force_flag) continue;
			break;
		}

		grm = knj;
		while (!IsEOL(*grm) && !IsBlank(*grm)) grm++;
		if (IsEOL(*grm)) {
			error_out(FormatErrorString, buf);
			if (force_flag) continue;
			break;
		}
		*grm++ = '\0';
		while (IsBlank(*grm)) grm++;
		if (IsEOL(*grm)) {
			error_out(FormatErrorString, buf);
			if (force_flag) continue;
			break;
		}

		flg = -1;
		while (*grm && flg) {
			p = grm;
			while (!IsEOL(*p) && !IsBlank(*p) && (*p != ':')) p++;
			if (IsEOL(*p))
				flg = 0;
			else
				*p++ = '\0';

			touroku(yom, knj, grm);

			if (flg) {
				grm = p;
				while (IsBlank(*grm)) grm++;
			}
		}
	}
}

