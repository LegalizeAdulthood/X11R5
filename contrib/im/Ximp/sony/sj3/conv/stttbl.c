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

#include "sj_rename.h"
#include "sj_typedef.h"

static	Uchar	o_settou[] = {
	0xA1,	0x8C, 0xE4,	0x82, 0xA8
};

static	Uchar	go_settou[] = {
	0xA1,	0x8C, 0xE4,	0x82, 0xb2
};

static	Uchar	dai_settou[] = {
	0x22,	0x91, 0xE6,	0x91, 0xE6
};

static	Uchar	kigou_settou[] = {
	0x01
};

Uchar	*settou_ptr[] = {
	NULL, o_settou, go_settou, dai_settou, kigou_settou
};

Uchar	scncttbl[2][24] = {
	{
		 0x48,  0x80,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,

		 0x18,  0x18,  0x00,  0x00,  0x1f,  0xf0,  0x00,  0xff,

		 0x00,  0x1f,  0xf0,  0x00,  0xff,  0x00,  0x00,  0x00
	},

	{
		 0x22,  0x40,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,

		 0x00,  0x04,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,

		 0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00
	}
};
