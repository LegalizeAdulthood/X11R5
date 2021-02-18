/*
 * $Id: cursor.c,v 1.2 1991/09/16 21:36:34 ohm Exp $
 */
/*
 * Copyright 1989, 1990, 1991 by OMRON Corporation
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON and MIT not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON and MIT make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * OMRON, MIT AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL OMRON, MIT OR WNN CONSORTIUM BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author:	Seiji Kuwari	OMRON Corporation
 *				kuwa@omron.co.jp
 *				kuwa%omron.co.jp@uunet.uu.net
 */				
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/*	Version 4.0
 */
#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#ifdef	XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else	/* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif	/* XJUTIL */

#ifdef	XJUTIL
int cursor_colum = 0;
static int cursor_reverse = 0;
static int cursor_underline = 0;
static int cursor_bold = 0;
int cursor_invisible = 1;
#endif	/* XJUTIL */

void
throw_col(col)
int col;
{
    throw_cur_raw(col);
    cursor_colum = col;
}

void
h_r_on()
{
    if(!cursor_reverse){
        h_r_on_raw();
        cursor_reverse = 1;
    }
}

void
h_r_off()
{
    if(cursor_reverse){
        h_r_off_raw();
        cursor_reverse = 0;
    }
    if(cursor_bold){
	b_s_on_raw();
    }
}

void
u_s_on()
{
    if(!cursor_underline){
        u_s_on_raw();
        cursor_underline = 1;
    }
}

void
u_s_off()
{
    if(cursor_underline){
        u_s_off_raw();
        cursor_underline = 0;
    }
}

static void
b_s_on()
{
    if(!cursor_bold){
        b_s_on_raw();
        cursor_bold = 1;
    }
    if(cursor_reverse){
	h_r_on_raw();
    }
}

static void
b_s_off()
{
    if(cursor_bold){
        b_s_off_raw();
        cursor_bold = 0;
    }
}

void
kk_cursor_invisible()
{
      if(cursor_invisible == 0){
          cursor_invisible_raw();
          cursor_invisible = 1;
      }
}
 
void
kk_cursor_normal()
{
    if(cursor_invisible){
        cursor_normal_raw();
        cursor_invisible = 0;
    }
}

void
reset_cursor_status()
{
    kk_cursor_normal();
    h_r_off();
    u_s_off();
}

void
set_cursor_status()
{
    if(cursor_invisible){
	cursor_invisible_raw();
    }else{
	cursor_normal_raw();
    }
    if(cursor_reverse){
	  h_r_on_raw();
    }
    if(cursor_underline){
	  u_s_on_raw();
    }
    throw_cur_raw(cursor_colum);
}

void
clr_line_all()
{
    throw_cur_raw(0);
    clr_end_screen();
}	    

static int saved_cursor_rev;
static int saved_cursor_und;
void
push_hrus()
{
    saved_cursor_rev = cursor_reverse;
    saved_cursor_und = cursor_underline;
    h_r_off();
    u_s_off();
}

void
pop_hrus()
{
    if(saved_cursor_rev)
        h_r_on();
    if(saved_cursor_und)
        u_s_on();
}

void
set_hanten_ul(x,y)
int x,y;
{
    if(!x)h_r_off();
    if(!y)u_s_off();
    if(x)h_r_on();
    if(y)u_s_on();
}

void
set_bold(x)
int x;
{
    if(x)b_s_on();
}

void
reset_bold(x)
int x;
{
    if(x)b_s_off();
}
