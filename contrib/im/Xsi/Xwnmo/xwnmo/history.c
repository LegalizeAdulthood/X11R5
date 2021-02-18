/*
 * $Id: history.c,v 1.2 1991/09/16 21:36:51 ohm Exp $
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
#include "jslib.h"
#include "ext.h"

int
init_history()
{
    int size1;
    char *area_pter;

    register History *hist, *pre, *end;

    if ((history_cunt = max_history) < 1) {
	history_cunt = 0;
	return(0);
    }
    size1 = history_cunt * sizeof(History);
    if ((area_pter = Malloc((unsigned)size1)) == NULL) {
	history_cunt = 0;
	wnn_errorno = WNN_MALLOC_ERR;
	return(-1);
    }
    hist = beginning_of_history = end_of_history = add_history =
	(History *)area_pter;
    c_c->save_hist = (History *)area_pter;
    pre = end = hist + (history_cunt - 1);
    do {
	hist->hbuffer = NULL;
	pre->next = hist;
	hist->previous = pre;
	hist->num = 0;
	pre = hist++;
    } while (hist <= end);
    return(0);
}

static void
set_up_history()
{
    if (add_history == beginning_of_history &&
	    end_of_history != beginning_of_history)
	beginning_of_history = beginning_of_history->next;
    end_of_history = add_history;
    add_history = add_history->next;
    current_history = NULL;
}

int
make_history(wstr, n)
register w_char *wstr;
register int	n;
{
    if (!history_cunt)
	return(0);
    if (n <= 0 || n > maxchg || *wstr == 0) 
	return(-1);

    if (n == 1 && NORMAL_CHAR(*wstr)) {
	if (!wchar_holding) {
	    wchar_holding = 1;
	    set_up_history();
	    end_of_history->num = 0;
	}
	if (end_of_history->hbuffer == NULL)
	    end_of_history->hbuffer = (w_char *)Malloc(sizeof(w_char));
	else
	    end_of_history->hbuffer
		= (w_char *)Realloc((char *)end_of_history->hbuffer,
			(unsigned)((end_of_history->num + 1) * sizeof(w_char)));
	Strncpy(end_of_history->hbuffer + end_of_history->num, wstr, 1);
	if (++(end_of_history->num) >= maxchg)
		wchar_holding = 0;
	return(0);
    }
    if (wchar_holding)
	wchar_holding = 0;
    if (n == 1 && !(KANJI_CHAR(*wstr)))
	/* don't insert cntrol code in history buffer */
	return(0);
    if (end_of_history->num == n &&
	    Strncmp(end_of_history->hbuffer, wstr, n) == 0) {
	current_history = NULL;
	return(0);
    }
    set_up_history();
    if (end_of_history->hbuffer != NULL)
	Free((char *)end_of_history->hbuffer);
    end_of_history->hbuffer = (w_char *)Malloc((unsigned)(n * sizeof(w_char)));
    Strncpy(end_of_history->hbuffer, wstr, n);
    end_of_history->num = n;

    return(0);
}

static int
get_current_history(wbuf)
register w_char *wbuf;
{
    if (!history_cunt)
	return(0);
    if (wchar_holding)
	wchar_holding = 0;
    if (current_history == NULL)
	current_history = end_of_history;
    Strncpy(wbuf, current_history->hbuffer, current_history->num);
    return(current_history->num);
}

void
get_end_of_history(wbuf)
register w_char	*wbuf;
{
    if (!history_cunt) {
	*wbuf = 0;
    } else {
    if (wchar_holding)
	    wchar_holding = 0;
	Strncpy(wbuf, end_of_history->hbuffer, end_of_history->num);
	*(wbuf + end_of_history->num) = 0;
    }
}

int
previous_history1(buffer)
register w_char *buffer;
{
    if (!history_cunt || current_history == beginning_of_history)
	return(0); /* do nothing */

    if (current_history != NULL)
	current_history = current_history->previous;
    return(get_current_history(buffer));
}

int
next_history1(buffer)
register w_char *buffer;
{
    if (!history_cunt || current_history == end_of_history || current_history == NULL)
	return(0);

    current_history = current_history->next;
    return(get_current_history(buffer));
}

void
destroy_history()
{
    register History *p;
    register int i;

    if (c_c == NULL || c_c->save_hist == NULL) return;
    for (i = 0, p = c_c->save_hist; i < history_cunt; i++, p++) {
	if (p->hbuffer) Free((char *)p->hbuffer);
    }
    Free((char *)c_c->save_hist);
}
