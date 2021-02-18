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
 *	jctype.h
 *	sjx local definition.
 *	Copyright (c) 1989 Sony Corporation.
 */
/*
 * $Header: sjctype.h,v 1.1 91/09/04 23:06:02 root Locked $ SONY;
 */

#if (!defined(sony_news) || BSD < 43)
#ifndef issjis1
#define issjis1(x)	(((unsigned char)0x81<=(x)&&(x)<=(unsigned char)0x9f) \
		|| ((unsigned char)0xe0<=(x)&&(x)<=(unsigned char)0xfc))
#endif
#ifndef issjis2
#define issjis2(x)	(((unsigned char)0x40<=(x)&&(x)<=(unsigned char)0x7e) \
		|| ((unsigned char)0x80<=(x)&&(x)<=(unsigned char)0xfc))
#endif
#ifndef iskanji
#define iskanji(x)	(((unsigned char)0x81<=(x)&&(x)<=(unsigned char)0x9f) \
		|| ((unsigned char)0xe0<=(x)&&(x)<=(unsigned char)0xfc))
#endif
#ifndef iskana
#define iskana(x)	((unsigned char)0xa1<=(x) && (x)<=(unsigned char)0xdf)
#endif
#ifndef iskana2
#define iskana2(x)	((unsigned char)0xa1<=(x) && (x)<=(unsigned char)0xdf)
#endif
#ifndef iseuc
#define iseuc(x)	((unsigned char)0xa1<=(x) && (x)<=(unsigned char)0xfe)
#endif
#ifndef iseuckana
#define iseuckana(x)	((x)==(unsigned char)0x8e)
#endif
#endif	/* !sony_news	*/
