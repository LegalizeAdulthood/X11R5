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

#define	SJ3_IKKATU_YOMI			512


#define	SJ3_BUNSETU_YOMI		128
#define	SJ3_BUNSETU_KANJI		256


#ifndef	SJ3_WORD_ID_SIZE
#	define	SJ3_WORD_ID_SIZE		32
#endif

typedef	struct	studyrec {
	unsigned char	dummy[SJ3_WORD_ID_SIZE];
} SJ3_STUDYREC;


typedef	struct bunsetu {
	int		srclen;
	int		destlen;
	unsigned char	*srcstr;
	unsigned char	*deststr;
	struct studyrec	dcid;
} SJ3_BUNSETU;


typedef	struct douon {
	unsigned char	ddata[SJ3_BUNSETU_KANJI];
	int		dlen;
	SJ3_STUDYREC	dcid;
} SJ3_DOUON;

struct romaji {
	unsigned char	*tuduri;
	unsigned char	*kana;
};

#define HIRAGANA	0
#define KATAKANA	1
