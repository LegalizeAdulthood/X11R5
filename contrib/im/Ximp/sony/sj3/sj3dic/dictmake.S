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
		error_out("品詞文字列を過っています %s:%s:%s", y, k, h);
		if (force_flag) return;
		exit(1);
	}

	err = sj3_touroku(y, k, grm);

	switch (err) {
	case -1:
		error_out("サーバが落ちました");
		exit(1);

	case 0:
		if (verbose_flag)
			normal_out("%s:%s:%s を登録しました\n", y, k, h);
		break;

	case SJ3_DICT_ERROR:
		error_out("辞書がない、あるいは辞書に書き込めません");
		exit(1);

	case SJ3_DICT_LOCKED:
		error_out("辞書がロックされていて書き込めません");
		exit(1);

	case SJ3_WORD_EXIST:
		if (verbose_flag)
			normal_out("%s:%s:%s は登録されています\n", y, k, h);
		break;

	case SJ3_DOUON_FULL:
		error_out("これ以上同音語を登録できません %s:%s:%s", y, k, h);
		break;

	case SJ3_DICT_FULL:
		error_out("これ以上辞書を大きくできません %s:%s:%s", y, k, h);
		if (force_flag) break;
		exit(1);

	case SJ3_BAD_YOMI_STR:
		error_out("登録できない読みです %s", y);
		if (force_flag) break;
		exit(1);

	case SJ3_BAD_KANJI_STR:
		error_out("登録できない漢字です %s:%s", y, k);
		if (force_flag) break;
		exit(1);

	case SJ3_BAD_HINSI_CODE:
		error_out("登録できない品詞です %s:%s:%s", y, k, h);
		if (force_flag) break;
		exit(1);

	case SJ3_INDEX_FULL:
		error_out("インデックスがいっぱいです");

	case SJ3_TOUROKU_FAILED:
	default:
		error_out("登録できませんでした %s:%s:%s(%d)",
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
	static	char	*FormatErrorString = "フォーマットエラー:%s";

	if (input && *input) {
		fp = fopen(input, "r");
		if (!fp) {
			error_out("%s がオープンできません", input);
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

