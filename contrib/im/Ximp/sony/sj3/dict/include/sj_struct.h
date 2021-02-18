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

#include "sj_const.h"

#ifndef	_SJ_STRUCT_
#define	_SJ_STRUCT_

typedef struct kanji_rec {
	int	klen;
	u_char	*kptr;
	int	alen;
	u_char	*aptr;
	struct kanji_rec *knext;
} KanjiRec;

typedef struct hinsi_rec {
	int	hinsi;
	KanjiRec *krec;
	struct hinsi_rec *hnext;
} HinsiRec;

typedef struct douon_rec {
	u_char	*yptr;
	int	dlen;
	int	hrec_num;
	HinsiRec *hrec;
	struct douon_rec *dnext;
} DouonRec;

typedef struct hindo_rec {
	int	klen;
	u_char	*kptr;
	int	exist;
	int	hindo;

	int	offset;

	struct hindo_rec *anext;
	struct assyuku_rec *arec;
} HindoRec;

typedef struct assyuku_rec {
	int	len;
	HindoRec *nrec;
	struct assyuku_rec *anext;
} AssyukuRec;

typedef struct offset_rec {
	int	klen;
	u_char	*kptr;
	int	offset;
} OffsetRec;

#endif
