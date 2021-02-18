/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */

/* "@(#)japanren.h	R14.1.0.0 88/02/05 15:17:47 " */
/************************************************************************/
/*	連文節カナ漢字変換ライブラリ諸元				*/
/************************************************************************/

#define	KANABUF_SIZE	128			/* かなBUFFERの大きさ	*/
#define	KANA_SUU	KANABUF_SIZE/2		/* 2byteかなの数	*/ 
#define	BUNSETU_SUU	KANA_SUU/2		/* 最大文節数		*/
#define	BUNSETU_SIZE	16			/* １文節の最大カナ数	*/

#ifndef NULL
#define NULL	0
#endif
