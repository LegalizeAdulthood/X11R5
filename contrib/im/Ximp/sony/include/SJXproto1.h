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
 *	SJXproto1.h
 *	Copyright (c) 1988 1989 Sony Corporation
 *	Thu Jun 23 02:58:53 JST 1988
 *	SJX Protocol Version 1
 */
/*
 * $Header: SJXproto1.h,v 1.1 91/04/29 17:57:36 masaki Exp $ SONY;
 */

/* protocol version		*/

#define SJX_PROTOCOL_VERSION	1
#define SJX_SERVICE		"sjx"
#define SJX_PROTO		"tcp"
#define SJX_TCP_PORT		3104
#define SJX_PORT_LIMIT		512

/* protocol number		*/

#define SJX_CONFIGURE_REQUEST	0x01
#define SJX_MAP_REQUEST		0x02
#define SJX_UNMAP_REQUEST	0x03
#define SJX_SEND_KEYEVENT	0x04
#define SJX_CODE_REQUEST	0x05

/* export information		*/

#define SJX_MACHINE		"_SJX_MACHINE"
#define SJX_PORT		"_SJX_PORT"

/*
 *	SJX Configure Request Structure
 */

typedef	struct _SJXconf {
	int	x;
	int	y;
	int	width;
	int	height;
} SJXconf;

#define SOCK_INIT	-1

/*
 *	SJX Code Request
 */

#define SJX_CODE_SJIS	0
#define SJX_CODE_EUC	1

