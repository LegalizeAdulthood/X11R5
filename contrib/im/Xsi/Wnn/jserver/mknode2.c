/*
 * $Id: mknode2.c,v 1.2 1991/09/16 21:31:58 ohm Exp $
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
#include "commonhd.h"
#include "ddefine.h"
#include "de_header.h"
#include "kaiseki.h"
#include "wnn_malloc.h"

static void lnk_jkdbn(), lnk_jksbn(), lnk_jksone(), freejktsone();
void freejktsbn();

static struct JKT_DBN	*free_jkdbn_top = NULL;
static struct free_list	*free_list_jkdbn = NULL;
static struct JKT_SBN	*free_jksbn_top = NULL;
static struct free_list	*free_list_jksbn = NULL;
static struct JKT_SONE	*free_jksone_top = NULL;
static struct free_list	*free_list_jksone = NULL;

/************************************************/
/* initialize link struct JKT_SONE           */
/************************************************/
int
init_jktdbn()
{
	free_area(free_list_jkdbn);
	if (get_area(FIRST_JKDBN_KOSUU, sizeof(struct JKT_DBN),
		&free_list_jkdbn) < 0)
	    return (-1);
	lnk_jkdbn(free_list_jkdbn);
	return (0);
}

/* free_jkdbn が 0 でない時に呼んだらあかんよ */
static void
lnk_jkdbn(list)
struct	free_list *list;
{
	register int	n;
	register struct JKT_DBN *wk_ptr;

	free_jkdbn_top = wk_ptr =
	    (struct JKT_DBN *)((char *)list + sizeof(struct free_list));

	for (n = list->num - 1; n > 0; wk_ptr++, n--) 
		wk_ptr->lnk_br = wk_ptr + 1;
	wk_ptr->lnk_br = 0;
}

int
init_jktsbn()
{
	free_area(free_list_jksbn);
	if (get_area(FIRST_JKSBN_KOSUU, sizeof(struct JKT_SBN),
		&free_list_jksbn) < 0)
	    return (-1);
	lnk_jksbn(free_list_jksbn);
	return (0);
}

/* free_jksbn が 0 でない時に呼んだらあかんよ */
static void
lnk_jksbn(list)
struct	free_list *list;
{
	register int	n;
	register struct JKT_SBN *wk_ptr;

	free_jksbn_top = wk_ptr =
	    (struct JKT_SBN *)((char *)list + sizeof(struct free_list));

	for (n = list->num - 1; n > 0; wk_ptr++, n--) 
		wk_ptr->lnk_br = wk_ptr + 1;
	wk_ptr->lnk_br = 0;
}

int
init_jktsone()
{
	free_area(free_list_jksone);
	if (get_area(FIRST_JKSONE_KOSUU, sizeof(struct JKT_SONE),
		&free_list_jksone) < 0)
	    return (-1);
	lnk_jksone(free_list_jksone);
	return (0);
}

/* free_jksone_top が 0 でない時に呼んだらあかんよ */
static void
lnk_jksone(list)
struct	free_list *list;
{
	register int	n;
	register struct JKT_SONE *wk_ptr;

	free_jksone_top = wk_ptr =
	    (struct JKT_SONE *)((char *)list + sizeof(struct free_list));

	for (n = list->num - 1; n > 0; wk_ptr++, n--) 
		wk_ptr->lnk_br = wk_ptr + 1;
	wk_ptr->lnk_br = 0;
}

/*******************************************************/
/* struct JKT_SONE & JKT_SBN free エリア作成 */
/*******************************************************/
void
freejktdbn(dbn)		/* struct JKT_SBN を free_area へリンク */
register struct JKT_DBN *dbn;	/* クリアするノードのポインタ */
{
	if (dbn == 0)
		return;

	freejktsbn(dbn->sbn);
	dbn->lnk_br = free_jkdbn_top;
	free_jkdbn_top = dbn;
}

void
freejktsbn(sbn)		/* struct JKT_SBN を free_area へリンク */
struct JKT_SBN *sbn;	/* クリアするノードのポインタ */
{
	register struct JKT_SONE *sone;
	register struct JKT_SONE *next;
	if (sbn == 0)
		return;
	sbn->reference--;
	if (sbn->reference <= 0) {
		for (sone = sbn->sbn; sone != 0; ) {
			next = sone->lnk_br;
			freejktsone(sone);
			sone = next;
		}
		sbn->reference = 0;
		sbn->lnk_br = free_jksbn_top;
		free_jksbn_top = sbn;
		freejktsbn(sbn->parent);
	}
}

static void
freejktsone(sone)	/* struct JKT_SONE を free_area へリンク */
register struct JKT_SONE *sone;	/* クリアするノードのポインタ */
{
	if (sone == 0)
		return;
	sone->lnk_br = free_jksone_top;
	free_jksone_top = sone;
}

/******************************************/
/* JKT_SBN JKT_SONE area の獲得		  */
/******************************************/
struct JKT_DBN *getjktdbn()
{
	register struct	JKT_DBN	*rtnptr;

	if (free_jkdbn_top == 0) {
	    if (get_area(JKDBN_KOSUU, sizeof(struct JKT_DBN), &free_list_jkdbn) < 0)
		    return ((struct JKT_DBN *)-1);
	    lnk_jkdbn(free_list_jkdbn);
	}

	rtnptr = free_jkdbn_top;
	free_jkdbn_top = free_jkdbn_top->lnk_br;
	rtnptr->lnk_br = 0;
	return(rtnptr);
}

struct JKT_SBN *getjktsbn()
{
	register struct	JKT_SBN	*rtnptr;

	if (free_jksbn_top == 0) {
	    if (get_area(JKSBN_KOSUU, sizeof(struct JKT_SBN), &free_list_jksbn) < 0)
		    return ((struct JKT_SBN *)-1);
	    lnk_jksbn(free_list_jksbn);
	}

	rtnptr = free_jksbn_top;
	free_jksbn_top = free_jksbn_top->lnk_br;
	rtnptr->lnk_br = 0;
	rtnptr->reference = 0;
	rtnptr->status = 0;
	return(rtnptr);
}

struct JKT_SONE *getjktsone()
{
	register struct	JKT_SONE	*rtnptr;

	if (free_jksone_top == 0) {
	    if (get_area(JKSONE_KOSUU, sizeof(struct JKT_SONE), &free_list_jksone) < 0)
		    return ((struct JKT_SONE *)-1);
	    lnk_jksone(free_list_jksone);
	}

	rtnptr = free_jksone_top;
	free_jksone_top = free_jksone_top->lnk_br;
	rtnptr->lnk_br = 0;
	rtnptr->jentptr = 0;
	return(rtnptr);
}
