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
 *	sjparam.h
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 */
/*
 * $Header: sjparam.h,v 1.1 91/04/29 17:57:48 masaki Exp $ SONY;
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pwd.h>
#ifdef sony_news
#include <ctype.h>
#else /* sony_news */
#include "sjctype.h"
#endif /* sony_news */

/* buffer size		*/

#define BUFFLENGTH	128
#define	LONGLENGTH	256
#define	SHORTLENGTH	32
#define DOUONLEN	32
#define DOUON_N		256
#define SHORTBUFFSIZE	16
#define RSIZE		256

#define TCBUFSIZE	1024
#define AREASIZE	256

#define MAXLINE		256
#define MAXWORD		256
#define WORDN		32

#define KB_KANA		0
#define KB_ROMA		1
