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

#define	VersionPos		0

#define	PasswdPos		16
#define	PasswdLen		16

#define	StdyNormPos		32
#define	StdyNormLen		36
#define	StdyNormNum		40
#define	StdyNormCnt		44

#define	StdyClIdxPos		48
#define	StdyClIdxLen		52
#define	StdyClIdxStep		56

#define	StdyClSegPos		64
#define	StdyClSegLen		68

#define	DictIdxPos		32
#define	DictIdxLen		36

#define	DictSegPos		48
#define	DictSegLen		52
#define	DictSegNum		56
#define	DictSegMax		60

#define	DictAIdxPos		64
#define	DictAIdxLen		68

#define	DictASegPos		80
#define	DictASegLen		84

#define	HeaderPos		0
#define	HeaderLength		128
#define	CommentPos		128
#define	CommentLength		128

#define	Byte2Long(c3,c2,c1,c0)	(((c3) << (8*3)) | ((c2) << (8*2)) | \
				 ((c1) << (8*1)) | ((c0) << (8*0)))
#define	StdyVersion		Byte2Long('s', 1, 0, 0)
#define	DictVersion		Byte2Long('d', 1, 0, 0)
