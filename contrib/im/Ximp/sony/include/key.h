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
 *	key.h
 *	virtual function key number
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 */
/*
 * $Header: key.h,v 1.1 91/04/29 17:57:43 masaki Exp $ SONY;
 */

/* function key			*/

#define KEY_NORMAL	0
#define KEY_CONV	1
#define KEY_HENKAN	2
#define KEY_MUHENKAN	3
#define KEY_KETTEI	4
#define KEY_ZHIRA	5
#define KEY_HALPHA	6
#define KEY_ZALPHA	7
#define KEY_HKATA	8
#define KEY_ZKATA	9
#define KEY_CODE	10
#define KEY_ETC		11
#define KEY_RIGHT	12
#define KEY_LEFT	13
#define KEY_UP		14
#define KEY_DOWN	15
#define KEY_TOGGLE	16
#define KEY_EDIT	17

#define KEY_WRAP	18
#define KEY_WRAPBACK	19
#define KEY_HELP	20
#define KEY_TOROKU	21
#define KEY_SYOUKYO	22
#define KEY_SJRC	23

#define KEY_HA_KET	24
#define KEY_FLUSH	25
#define KEY_BUFFER	26

#define KEY_HANKAKU	27
#define KEY_ZENKAKU	28

#define KEY_HIRAGANA	29
#define KEY_KATAKANA	30
#define KEY_EISUU	31
#define KEY_EIDAI	32
#define KEY_ROMA	33

#define KEY_CANCEL	34
#define KEY_RECONV	35

#define KEY_COMPOSE	36

#define KEY_IGNORE	37
#define KEY_DEBUG	38
#define KEY_OTHER	39

#define KEY_MORE	40

/* others			*/

#define UKEYNUM		64
#define	SEQLEN		8
#define COMNUM		64

extern int	comnumber;
extern char	*comstr[];
extern int	comstrvalue[];
