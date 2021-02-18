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

#include "sjctype.h"

#define	LONGLENGTH	256
#define	MAXLINE		256
#define	MAXWORD		256
#define	WORDN		32

#ifndef	SYS_SJIS
#	define	SYS_SJIS	0
#endif
#ifndef	SYS_EUC
#	define	SYS_EUC		1
#endif

#define	TRUE		-1
#define	FALSE		!(TRUE)

#define	EXEC_TEXT	1
#define	EXEC_DICT	2
#define	EXEC_INIT	10
#define	EXEC_VERBOSE	11
#define	EXEC_SERVER	12
#define	EXEC_FORCE	13

#define	SingleShift2	0x8e

