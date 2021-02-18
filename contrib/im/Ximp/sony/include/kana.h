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
 *	kana.h
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 */
/*
 * $Header: kana.h,v 1.1 91/04/29 17:57:42 masaki Exp $ SONY;
 */

#define ZKATA1	0x83	/* zenkaku katakana first byte			*/
#define ZHIRA1	0x82	/* zenkaku hiragana first byte			*/
#define ZKATAF2	0x40	/* zenkaku katakana first char second byte	*/
#define ZKATAE2	0x96	/* zenkaku katakana end char second byte	*/
#define ZHIRAF2	0x9f	/* zenkaku hiragana first char second byte	*/
#define ZHIRAE2	0xf1	/* zenkaku hiragana end char second byte	*/
#define ZKATAMU	0x80	/* zenkaku katakana MU code second byte		*/
#define ZHIRAMU	0xde	/* zenkaku hiragana MU code second byte		*/

#define ZKATAn	(ZKATAE2-ZKATAF2)	/* number of zenkaku katakana	*/
				/*
				 * hikizan de 1 sukunaku naru hazu daga,
				 * MU no tokorode 1-tu ton de irunode,
				 * chyoudo yoi.
				 */

#define ZK_SUFFIX		0
#define ZH_SUFFIX		1
#define HK_SUFFIX		2
#define SUFFIX_DIMENSION	3

#define CHAR_TEN		0xde
#define CHAR_MARU		0xdf
#define CHAR_WA			0xdc
