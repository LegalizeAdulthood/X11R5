/*
 * $Id: select_ele.c,v 1.2 1991/09/16 21:37:19 ohm Exp $
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
#include "xim.h"
#include "sheader.h"
#include "ext.h"

static int kosuu; /* kosuu of elements */

static int
change_decimal(c1)
int c1;
{
    if(c1 >= '0' && c1 <= '9') return(c1 - '0');
    if(c1 >= 'A' && c1 <= 'Z') return(c1 - 'A' + 10);
    if(c1 >= 'a' && c1 <= 'z') return(c1 - 'a' + 10);
    return(-1);
}

int
xw_select_one_element(c, kosuu1, init, def, title, mode, key_tbl, in)
char **c;
int kosuu1;
int init, def;
char *title;
int mode;
int (**key_tbl)();
int in;
{
    static int d;
    int c1;
    int ret;
    static WnnClientRec *c_c_sv = 0;
    static int select_step = 0;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return(-1);
    }
    if (select_step == 0) {
	kosuu = kosuu1;
	if (init_ichiran((unsigned char **)c, kosuu1, init, def,
			 (unsigned char *)title, (unsigned char *)"",
			 (unsigned char *)"", 0, mode) == -1) {
	    ring_bell();
	    return(-1);
	}
	c_c_sv = c_c;
	select_step++;
	return(BUFFER_IN_CONT);
    }
    c1 = in;
    if (!xim->ichi->map) return(BUFFER_IN_CONT);
    if (xim->sel_ret == -1) {
        d = change_decimal(c1);
    } else if (xim->sel_ret == -2) {
        end_ichiran();
	xim->sel_ret = -1;
	c_c_sv = 0;
	select_step = 0;
	return(-1);
    } else {
    /*
	end_ichiran();
    */
	d = xim->sel_ret;
	xim->sel_ret = -1;
    }
    if(d >= 0 && d < kosuu){
	end_ichiran();
	c_c_sv = 0;
	select_step = 0;
	return(d);
    } 
    if((c1 < 256) && (key_tbl[c1] != NULL)){
	if((ret = (*key_tbl[c1])()) == 1){
	    d = find_hilite();
            end_ichiran();
	    c_c_sv = 0;
	    select_step = 0;
            return(d);
	}else if(ret == -1){
	    end_ichiran();
	    c_c_sv = 0;
	    select_step = 0;
	    return(-1);
	} else if(ret == -3){
      	    ring_bell();
	}
    }
    return(BUFFER_IN_CONT);
}

int
xw_select_one_jikouho(c, kosuu1, init, def, title, mode, key_tbl, in)
char **c;
int kosuu1;
int init, def;
char *title;
int mode;
int (**key_tbl)();
int in;
{
    int ret;
    static int d, c1;
    static WnnClientRec *c_c_sv = 0;
    static int select_step = 0;

    if (c_c_sv != 0 && c_c != c_c_sv) {
	ring_bell();
	return(-1);
    }
    if (select_step == 0) {
	kosuu = kosuu1;
	if (init_ichiran((unsigned char **)c, kosuu1, init, def,
			 (unsigned char *)title, (unsigned char *)"",
			 (unsigned char *)"", 0, mode) == -1) {
	    ring_bell();
	    return(-1);
	}
	c_c_sv = c_c;
	select_step++;
	return(BUFFER_IN_CONT);
    } else {
	c1 = in;
	if (!xim->ichi->map) return(BUFFER_IN_CONT);
	if (xim->sel_ret == -2) {
	    end_ichiran();
	    xim->sel_ret = -1;
	    c_c_sv = 0;
	    select_step = 0;
	    return(-1);
	} else if (xim->sel_ret != -1) {
	    d = find_ji_hilite();
	    end_ichiran();
	    xim->sel_ret = -1;
	    c_c_sv = 0;
	    select_step = 0;
	    return(d);
	}else if ((c1 < 256) && (key_tbl[c1] != NULL)){
	    if((ret = (*key_tbl[c1])()) == 1){
		d = find_ji_hilite();
		end_ichiran();
		c_c_sv = 0;
		select_step = 0;
		return(d);
	    } else if (ret == -1) {
		end_ichiran();
		c_c_sv = 0;
		select_step = 0;
		return(-1);
	    } else if (ret == -3) {
		ring_bell();
		return(BUFFER_IN_CONT);
	    } else {
		return(BUFFER_IN_CONT);
	    }
  	} else {
	    ring_bell();
	    return(BUFFER_IN_CONT);
	}
    }
}


static void
display_all()
{
    redraw_ichi_w();
}

int
forward_select()
{
    xw_forward_select();
    return(0);
}

int
backward_select()
{
    xw_backward_select();
    return(0);
}

int
lineend_select()
{
    xw_lineend_select();
    return(0);
}

int
linestart_select()
{
    xw_linestart_select();
    return(0);
}

int
select_select()
{
    return(1); /* return mm from upper function */
}

int
quit_select()
{
    return(-1);
}

int
previous_select()
{
    xw_previous_select();
    return(0);
}

int
next_select()
{
    xw_next_select();
    return(0);
}

int
redraw_select()
{
    display_all();
    return(0);
}

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/

