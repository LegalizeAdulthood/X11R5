/*
 * $Id: history.c,v 1.2 1991/09/16 21:33:36 ohm Exp $
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
#include "sheader.h"
#include "jslib.h"

static struct history {
	w_char *hbuffer;
	struct history *previous;
	struct history *next;
	int	num;
};

static struct history *beginning_of_history; /* ヒストリの先頭 */
static struct history *end_of_history; /* ヒストリの最後 */
static struct history *add_history; /* 追加位置 */
static struct history *current_history = NULL; /* index */
static int	wchar_holding = 0;
static int	history_cunt;

static void set_up_history();

int
init_history()
{
    char *malloc();
    int	size1;
    char *area_pter;

    register struct history *hist, *pre, *end;

    if ((history_cunt = max_history) < 1) {
	history_cunt = 0;
	return(0);
    }
    size1 = history_cunt * sizeof(struct history);
    if ((area_pter = malloc(size1)) == NULL) {
	history_cunt = 0;
	wnn_errorno = WNN_MALLOC_ERR;
	return(-1);
    }
    hist = beginning_of_history = end_of_history = add_history =
	(struct history *)area_pter;
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


int
make_history(wstr, n)
register w_char *wstr;
register int	n;
{
    extern char *malloc();

    if (!history_cunt)
	return(0);
    if (n <= 0 || n > maxchg || *wstr == NULL) 
	return(-1);

    if (n == 1 && NORMAL_CHAR(*wstr)) {
	if (!wchar_holding) {
	    wchar_holding = 1;
	    set_up_history();
	    end_of_history->num = 0;
	}
	if (end_of_history->hbuffer == NULL)
	    end_of_history->hbuffer = (w_char *)malloc(sizeof(w_char));
	else
	    end_of_history->hbuffer = (w_char *)realloc(end_of_history->hbuffer,
				     (end_of_history->num + 1) * sizeof(w_char));
	Strncpy(end_of_history->hbuffer + end_of_history->num, wstr, 1);
	if (++(end_of_history->num) >= maxchg)
	    wchar_holding = 0;
	return(0);
    }
    if (wchar_holding)
	wchar_holding = 0;
    if (n == 1 && !(KANJI_CHAR(*wstr)))
	/* コントロールコードは、ヒストリには入れない */
	return(0);
    if (end_of_history->num == n &&
	    Strncmp(end_of_history->hbuffer, wstr, n) == 0) {
	/* 同じものは、いや */
	current_history = NULL;
	return(0);
    }
    set_up_history();
    if (end_of_history->hbuffer != NULL)
	free(end_of_history->hbuffer);
    end_of_history->hbuffer = (w_char *)malloc(n * sizeof(w_char));
    Strncpy(end_of_history->hbuffer, wstr, n);
    end_of_history->num = n;

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


/* 確定後に登録する漢字を取り出す */
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
