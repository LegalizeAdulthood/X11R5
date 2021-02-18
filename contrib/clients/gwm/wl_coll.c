/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/**************************\
* 			   *
*  WOOL_OBJECT Collection  *
*  BODY			   *
* 			   *
\**************************/

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_number.h"
#include "wl_atom.h"
#include "wl_list.h"
#include "INTERN.h"
#include "wl_coll.h"

WOOL_OBJECT WLCollection_release();

/*
 * wlcf (wool collection free)
 * 
 * management of an stack of free collections to avoid calls to malloc
 */

#define WLCF_LIMIT 32
#if WLCF_LIMIT
static WOOL_Collection wlcf[WLCF_LIMIT];
#else
static WOOL_Collection wlcf[1];
#endif
static WOOL_Collection  *wlcf_last = wlcf;
static int	wlcf_size, wlcf_limit = WLCF_LIMIT;

/* to be called once */

#ifdef STATS
WOOL_OBJECT
wlcfstats()
{
    wool_printf("wool-coll-free (wlcf)  has %d", wlcf_size);
    wool_printf("/%d slots\n", wlcf_limit);
    return NIL;
}
#endif /* STATS */

/* to release wlcf  */

wlcf_flush()
{
    WOOL_Collection *p = wlcf_last - 1;

    while (p >= wlcf) {
	WLCollection_release(*p);
        p--;
    }
    wlcf_size = 0;
    wlcf_last = wlcf;
}

/* put in wlcf */

WOOL_OBJECT 
WLCollection_free(col)
WOOL_Collection col;
{
    if (wlcf_size >= wlcf_limit) {
	WLCollection_release(col);
    } else {
	wlcf_size++;
	*wlcf_last++ = col;
	col -> size = 0;
    }
    return NULL;
}

/*
 * Constructor:
 * WLCollection_make
 * do a wlcf_get in fact...
 */

WOOL_Collection 
WLCollection_make()
{
    WOOL_Collection col;

    if (wlcf_size) {
	col = *(--wlcf_last);
	wlcf_size--;
	zrt_put(col);
    } else {
	col = (WOOL_Collection)
	    Malloc(sizeof(struct _WOOL_Collection));
	zrt_put(col);
	col -> type = WLCollection;
	col -> size = 0;
	col -> limit = INITIAL_COLLECTION_SIZE;
	col -> list = (WOOL_OBJECT *) Malloc(col -> limit << 2);
    }
    return col;
}

/* 
 * WLCollection_print:
 * Normally, never to be called.
 */

WOOL_OBJECT 
WLCollection_print(obj)
WOOL_Collection obj;
{
    int             i;
    WOOL_OBJECT    *p = obj -> list;

    wool_puts("{COLLECTION ");
    for (i = 0; i < obj -> size; i++, p++) {
	if (i)
	    wool_putchar(' ');
	WOOL_send(WOOL_print, *p, (*p));
    }
    wool_putchar('}');
    return (WOOL_OBJECT) obj;
}

/*
 * WLCollection_free
 */

WOOL_OBJECT 
WLCollection_release(col)
WOOL_Collection col;
{
    Free(col -> list);
    Free(col);
    return NULL;
}

/*
 * trying to execute an collection is the same error than executing an atom.
 */

/*
 * WLCollection_add:
 * Adds arg2 to arg1, just catenating if there is room, increasing limit
 * of collection if not.
 * (we know we have 4 bytes of overhead, thats the reason for our
 * growing scheme: * 2 +4)
 * WARNING: since a zrt_gc cannot occur during parsing, we do not set
 * the reference count on the sons!
 */

WOOL_Collection 
WLCollection_add(col, obj)
WOOL_Collection col;
WOOL_OBJECT     obj;
{
    if (col -> size >= col -> limit) {
	WOOL_OBJECT *oldlist = col -> list;

	col -> limit = col -> limit << 1 + 1;
	col -> list = (WOOL_OBJECT *) Malloc((col -> limit) << 2);
	bcopy(oldlist, col -> list, col -> size << 2);
	Free(oldlist);
    }
    *(col -> list + (col -> size)++) = obj;
    return col;
}

/* makes a (progn <list>) of a collection
 */

WOOL_OBJECT
WLCollection_progn(col)
WOOL_Collection col;
{
    if (col -> size) {
	WOOL_List       object = wool_list_make(col->size +1);

	copy_n_objects(col -> list, object -> list + 1, col -> size);
	increase_reference(object -> list[0] = WA_progn);
	return (WOOL_OBJECT) object;
    } else {				/* a list of size 0 is just NIL */
	return NIL;
    }
}

/*******************************************************\
* 						        *
* QuotedExpr package for speeding up quoted constructs  *
* 						        *
\*******************************************************/

WOOL_QuotedExpr
WLQuotedExpr_make(expr)
WOOL_OBJECT	expr;
{
    WOOL_QuotedExpr object = (WOOL_QuotedExpr)
    Malloc(sizeof(struct _WOOL_QuotedExpr));

    zrt_put(object);
    object -> type = WLQuotedExpr;
    increase_reference(object -> expr = expr);
    return (WOOL_QuotedExpr) object;
}

WOOL_OBJECT
WLQuotedExpr_eval(obj)
WOOL_QuotedExpr obj;
{
    return (WOOL_OBJECT) obj -> expr;
}

WOOL_OBJECT
WLQuotedExpr_print(obj)
WOOL_QuotedExpr obj;
{
    wool_putchar('\'');
    WOOL_send(WOOL_print, obj -> expr, (obj -> expr));
    return (WOOL_OBJECT) obj;
}

WOOL_OBJECT
WLQuotedExpr_free(obj)
WOOL_QuotedExpr obj;
{
    decrease_reference(obj -> expr);
    Free(obj);
    return NULL;
}

WOOL_OBJECT
WLQuotedExpr_equal(o1, o2)
WOOL_QuotedExpr o1, o2;
{
    return WOOL_send(WOOL_equal, o1 -> expr, (o1 -> expr, o2 -> expr));
}
