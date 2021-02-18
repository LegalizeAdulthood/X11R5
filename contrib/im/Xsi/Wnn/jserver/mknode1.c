/*
 * $Id: mknode1.c,v 1.2 1991/09/16 21:31:57 ohm Exp $
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

void free_area();
static void lnk_sbn();

static struct SYO_BNSETSU	*free_sbn_top = NULL;
static struct free_list		*free_list_sbn = NULL;
/************************************************/
/* initialize link struct SYO_BNSETSU           */
/************************************************/
int
init_sbn()
{
	free_area(free_list_sbn);
	if (get_area(FIRST_SBN_KOSUU, sizeof(struct SYO_BNSETSU),
		&free_list_sbn) < 0)
	    return (-1);
	lnk_sbn(free_list_sbn);
	return (0);
}

int
get_area(kosuu, size, list)
register int	kosuu;
register int	size;
struct  free_list **list;
{
	register struct free_list *area;

	if ((area = (struct free_list *)
	    malloc(size * kosuu  + sizeof(struct free_list))
	    ) == NULL) {
		wnn_errorno = WNN_MALLOC_INITIALIZE;
		error1("Cannot Malloc in get_area.\n");
		return (-1);
	}
	area->lnk = *list;
	area->num = kosuu;
	*list = area;
	return (0);
}

void
free_area(list)
register struct  free_list *list;
{
	if (list == 0)
		return;
	free_area(list->lnk);
	free(list);
}

/* free_sbn が 0 でない時に呼んだらあかんよ */
static void
lnk_sbn(list)
struct	free_list *list;
{
	register int	n;
	register struct SYO_BNSETSU *wk_ptr;

	free_sbn_top = wk_ptr =
	    (struct SYO_BNSETSU *)((char *)list + sizeof(struct free_list));

	for (n = list->num - 1; n > 0; wk_ptr++, n--) 
		wk_ptr->lnk_br = wk_ptr + 1;
	wk_ptr->lnk_br = 0;
}


/*******************************************************/
/* struct SYO_BNSETSU & ICHBNP & KANGO free エリア作成 */
/*******************************************************/
void
freesbn(sbn)			/* struct SYO_BNSETSU を free_area へリンク */
register struct SYO_BNSETSU *sbn;	/* クリアするノードのポインタ */
{
	if (sbn == 0)
		return;
	sbn->reference--;
	if (sbn->reference <= 0) {
		sbn->lnk_br = free_sbn_top;
		free_sbn_top = sbn;
	}
}

void
clr_sbn_node(sbn)
register struct	SYO_BNSETSU	*sbn;
{
	if (sbn == 0)
		return;
	if ((sbn->reference) <= 1) {
		freesbn(sbn);
		clr_sbn_node(sbn->parent);
	} else {
		sbn->reference--;
	}
}
/******************************************/
/* SYO_BNSETSU area の獲得		  */
/******************************************/
struct SYO_BNSETSU *
getsbnsp()
{
	register struct	SYO_BNSETSU	*rtnptr;

	if (free_sbn_top == 0) {
	    if (get_area(SBN_KOSUU, sizeof(struct SYO_BNSETSU), &free_list_sbn) < 0)
		    return ((struct SYO_BNSETSU *)-1);
	    lnk_sbn(free_list_sbn);
	}

	rtnptr = free_sbn_top;
	free_sbn_top = free_sbn_top->lnk_br;
	rtnptr->lnk_br = 0;
	rtnptr->parent = 0;
	rtnptr->son_v = 0;
	rtnptr->reference = 0;
	rtnptr->jentptr = 0;
	rtnptr->status = 0;
	return(rtnptr);
}
