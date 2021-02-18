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
#include <sys/types.h>
#include "sj_struct.h"

extern	DouonRec *douon_ptr;
extern	int	douon_num;
extern	int	yomi_len;
extern	int	hinsi_num;
extern	int	kanji_len;
extern	HindoRec *hindo[];
extern	int	hindo_num;
extern	HindoRec *askknj[];
extern	int	askknj_num;
extern	HindoRec *assyuku;

static	DouonRec *drectmp = NULL;

static	clearklist(krec)
register KanjiRec *krec;
{
	register KanjiRec *p;

	while (krec) {
		Free(krec -> kptr);
		Free(krec -> aptr);

		p = krec;
		krec = krec -> knext;

		Free(p);
	}
}

static	clearhlist(hrec)
register HinsiRec *hrec;
{
	register HinsiRec *p;

	while (hrec) {
		clearklist(hrec -> krec);

		p = hrec;
		hrec = hrec -> hnext;

		Free(p);
	}
}

clear_list()
{
	register DouonRec *p;
	register DouonRec *drec = douon_ptr;

	while (drec) {
		Free(drec -> yptr);

		clearhlist(drec -> hrec);

		p = drec;
		drec = drec -> dnext;

		Free(p);
	}

	douon_ptr = NULL;
}

static	u_char	*makekanji(yomi, kanji, atr, len)
int	*yomi;
int	*kanji;
int	*atr;
int	*len;
{
	int	kana[MaxYomiLength + 1];
	u_char	ktmp[MaxKanjiLength * 2 + MaxAtrNumber * 2 + 1];
	register int	i;
	register int	pos = 0;
	int	*p;
	u_char	*q;

	for (p = yomi, i = 0 ; *p ; )
		kana[i++] = h2kcode(*p++);
	kana[i] = 0;

	while (*atr) {
		i = *atr++ + AtrMsKanjiOfs;
#ifndef	NO_ATR
		ktmp[pos++] = ((i >> 8) & ~KanjiStrEnd);
		ktmp[pos++] = (i & 0xff);
#endif
	}

	if (i = top_strcmp(yomi, kanji)) {
		kanji += i;
		ktmp[pos++] = (ZenHiraAssyuku | (i - 1));
		if (!*kanji) ktmp[pos - 1] |= KanjiStrEnd;
	}
	else if (i = top_strcmp(kana, kanji)) {
		kanji += i;
		ktmp[pos++] = (ZenKataAssyuku | (i - 1));
		if (!*kanji) ktmp[pos - 1] |= KanjiStrEnd;
	}

	while (*kanji) {
		if (i = last_strcmp(yomi, kanji)) {
			kanji += i;
			ktmp[pos++] = (ZenHiraAssyuku | (i - 1) | KanjiStrEnd);
		}
		else if (i = last_strcmp(kana, kanji)) {
			kanji += i;
			ktmp[pos++] = (ZenKataAssyuku | (i - 1) | KanjiStrEnd);
		}
		else if (*kanji < 0x100) {
			ktmp[pos] = (LeadingHankaku & ~KanjiStrEnd);
			ktmp[pos + 1] = *kanji++;
			pos += 2;
		}
		else {
			ktmp[pos] = ((*kanji >> 8) & ~KanjiStrEnd);
			ktmp[pos + 1] = (*kanji++ & 0xff);
			pos += 2;
		}
	}
	*len = pos;

	q = (u_char *)Malloc(pos);
	bcopy(ktmp, q, pos);

	return q;
}

static	u_char	*makeyomi(yomi)
int	*yomi;
{
	u_char	tmp[MaxYomiLength + 1];
	register int	i;
	register int	j;
	register int	*y = yomi;
	register u_char	*p;

	for (i = 0 ; *y ; ) {
		j = cnvyomi(*y++);
		if (j == 0) {
			fprintf(stderr, "不正な文字が読みに使われています\n");
			output_int(stderr, yomi); fputc('\n', stderr);
			exit(1);
		}
		tmp[i++] = j;
	}
	tmp[i++] = 0;

	p = (u_char *)Malloc(i);
	bcopy(tmp, p, i);

	return p;
}

static	KanjiRec *make_krec(kcode, klen)
u_char	*kcode;
int	klen;
{
	register KanjiRec *krec;

	krec = (KanjiRec *)Malloc(sizeof(KanjiRec));

	krec -> klen = klen;
	krec -> kptr = kcode;
	krec -> alen = 0;
	krec -> aptr = NULL;
	krec -> knext = NULL;

	return krec;
}

static	HinsiRec *make_hrec(hinsi)
int	hinsi;
{
	register HinsiRec *hrec;

	hrec = (HinsiRec *)Malloc(sizeof(HinsiRec));

	hrec -> hinsi = hinsi;
	hrec -> krec  = NULL;
	hrec -> hnext = NULL;

	return hrec;
}

static	DouonRec *make_drec(ycode)
u_char	*ycode;
{
	register DouonRec *drec;

	drec = (DouonRec *)Malloc(sizeof(DouonRec));

	drec -> yptr = ycode;
	drec -> hrec_num = 0;
	drec -> hrec = NULL;
	drec -> dnext = NULL;

	return drec;
}

static	diff_ylen(drec)
DouonRec *drec;
{
	register DouonRec *dptr;
	register DouonRec *dprev;
	register u_char	*p1;
	register u_char	*p2;
	register int	ylen = 0;

	dptr = douon_ptr; dprev = NULL;
	while (dptr) {
		dprev = dptr;
		dptr = dptr -> dnext;
	}

	if (dprev) {
		p1 = dprev -> yptr; p2 = drec  -> yptr;
		while (*p1 && (*p1 == *p2)) { p1++; p2++; }

		if (*p1 == *p2) {
			fprintf(stderr, "等しい読みの同音語ブロックがある\n");
			output_yomi(stderr, dprev -> yptr); fputc('\n', stderr);
			exit(1);
		}
		else if (*p1 > *p2) {
			fprintf(stderr, "読みの順序がおかしい\n");
			output_yomi(stderr, dprev->yptr); fputc('\n', stderr);
			output_yomi(stderr, drec->yptr); fputc('\n', stderr);
			exit(1);
		}

		while (*p2++) ylen++;
	}

	return ylen;
}

static	douon_knj(drec)
DouonRec *drec;
{
	int	i;
	int	len = 0;
	u_char	*p;
	u_char	*knjofscvt();
	HinsiRec *hrec;
	KanjiRec *krec;

	for (hrec = drec -> hrec ; hrec ; hrec = hrec -> hnext) {
		for (krec = hrec -> krec ; krec ; krec = krec -> knext) {
			p = knjofscvt(krec -> kptr, krec -> klen, &i);

			len += i;

			knjhnd_set(p, i);

			krec -> alen = i;
			krec -> aptr = p;

			set_ofsrec(krec -> kptr, krec -> klen, 0);
		}
	}

	return len;
}

static	make_d_list(drec)
DouonRec *drec;
{
	DouonRec *dptr, *dprev;
	HinsiRec *hptr;
	KanjiRec *kptr;
	int	ylen;
	int	klen;
	int	hnum;
	int	len;
	int	i;

start:
	if (douon_ptr == NULL) {
		douon_num = 0;
		yomi_len  = 0;
		hinsi_num = 0;
		kanji_len = 0;
		assyuku = NULL;
	}

	klen = douon_knj(drec);

	ylen = diff_ylen(drec);

	hnum = drec -> hrec_num;

	i = 1 +
	    douon_num * DouonBlkSizeNumber + DouonBlkSizeNumber +
	    yomi_len + ylen +
	    hinsi_num + hnum +
	    hinsi_num + hnum +
	    kanji_len + klen;

	len = (i <= MainSegmentLength) ? 0 : decide_knjask();

	if ((i - len) <= MainSegmentLength) {
		if (douon_ptr) {
			dptr = douon_ptr; dprev = NULL;
			while (dptr) {
				dprev = dptr;
				dptr = dptr -> dnext;
			}

			dprev -> dnext = drec;
		}
		else {
			douon_ptr = drec;
		}
		drec  -> dnext = NULL;

		drec -> dlen = ylen;

		douon_num += 1;

		yomi_len += ylen;

		hinsi_num += hnum;

		kanji_len += klen;

		return;
	}
	else if (douon_ptr == NULL) {
		fprintf(stderr, "１同音語ブロックが大きすぎます\n");
		exit(1);
	}

	for (hptr = drec -> hrec ; hptr ; hptr = hptr -> hnext) {
		for (kptr = hptr -> krec ; kptr ; kptr = kptr -> knext) {

			knjhnd_reset(kptr -> aptr, kptr -> alen);

			Free(kptr -> aptr);
			kptr -> aptr = NULL;
		}
	}

	len = decide_knjask();

	makeseg();

	clear_list();

	clear_hindo();

	clear_ofsrec();

	askknj_num = 0;

	clear_assyuku();

	goto start;
}

flush_douon()
{
	if (drectmp) {
		make_d_list(drectmp);

		drectmp = NULL;
	}

	if (douon_ptr) {
		decide_knjask();

		makeseg();

		clear_list();

		clear_hindo();

		clear_ofsrec();

		askknj_num = 0;
	}
}

makelist(yomi, kanji, hinsi, atr)
int	*yomi;
int	*kanji;
int	hinsi;
int	*atr;
{
	u_char	*ycode;
	u_char	*kcode;
	int	klen;
	HinsiRec *hrec;
	KanjiRec *krec, *kprev;

	ycode = makeyomi(yomi);

	kcode = makekanji(yomi, kanji, atr, &klen);

	if (drectmp && strcmp(ycode, drectmp -> yptr)) {
		make_d_list(drectmp);

		drectmp = NULL;
	}

	if (!drectmp) {
		drectmp = make_drec(ycode);

		hrec = make_hrec(hinsi);
		drectmp -> hrec = hrec;
		drectmp -> hrec_num = 1;

		krec = make_krec(kcode, klen);
		hrec -> krec      = krec;

		return;
	}

	for (hrec = drectmp -> hrec ; hrec -> hnext ; hrec = hrec -> hnext)
		if (hrec -> hinsi == hinsi) break;

	if (hrec -> hinsi == hinsi) {
		for (krec=hrec->krec ; krec ; krec=krec->knext) {
			if ((krec -> klen == klen) &&
			    !bcmp(krec -> kptr, kcode, klen)) {
				fprintf(stderr, "同一の熟語が存在した\n");
				fprintf(stderr, "\t読み:");
				output_int(stderr, yomi);
				fprintf(stderr, "\n");
				fprintf(stderr, "\t漢字:");
				output_int(stderr, kanji);
				fprintf(stderr, "\n");

				Free(kcode);

				return;
			}
			kprev = krec;
		}

		kprev -> knext = make_krec(kcode, klen);

		return;
	}

	hrec -> hnext = make_hrec(hinsi);
	hrec = hrec -> hnext;
	drectmp -> hrec_num += 1;

	krec = make_krec(kcode, klen);
	hrec -> krec = krec;
}
