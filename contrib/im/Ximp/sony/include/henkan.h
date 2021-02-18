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
 *	henkan.h
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Mon Jun 15 22:55:19 JST 1987
 */
/*
 * $Header: henkan.h,v 1.1 91/04/29 17:57:41 masaki Exp $ SONY;
 */

#define ObjEnd		0
#define ObjHenkan	1
#define ObjMuhenkan	2

#define	Bend		0
#define Bhenkan		1
#define Bmuhenkan	2
#define Bedit		4
#define BEDIT		8		/* bunsetu yomi modify	*/

#define DLEN	(DOUONLEN*2)
#define EDITLEN (BUFFLENGTH*DLEN)
#define MAXENUM (EDITLEN/DLEN)
