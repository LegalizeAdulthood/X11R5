/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/******************************************************\
* 						       *
* reference.c:					       *
* reference count management and other storage issues  *
* 						       *
\******************************************************/

#include        "EXTERN.h"
#include 	"wool.h"


/*
 * The memory management of WOOL is implemented via a differed reference
 * count. That, each time an object's reference count attains 0, it is put in
 * the zrt, which is polled at regular intervals
 */

/************************************\
* 				     *
* Zero_reference table module (zrt)  *
* 				     *
\************************************/

/*
 * The zrt (Zero Reference Table) global structure  is used to mark ALL wobs
 * that have at any moment be of REF 0, that is either being created or via
 * decrease_reference.  Then you can call zrt_gc at strategic moments, (ie in
 * no enclosing WOOL function) to free all the zero-referenced objects in the
 * zrt.
 */

#ifdef STATS
WOOL_OBJECT
zrtstats()
{
    wool_printf("Zero-reference-table has %d", zrt_size);
    wool_printf("/%d slots\n", zrt_limit);
    return NIL;
}
#endif /* STATS */

zrt_init()
{
    zrt_size = 0;
    zrt_limit = 63;	/* pow(2,n)/4 -1 */
    zrt = (WOOL_OBJECT *) Malloc(zrt_limit * sizeof(WOOL_OBJECT));
    zrt_last = zrt;
}

/*
 * disposes really of objects stacked in zrt. Be warned that a WOOL_free might
 * trigger zrt_put during the zrt_gc !
 */

zrt_gc(from)
int	from;
{
    WOOL_OBJECT *zrt_from = zrt + from;

    while (zrt_last > zrt_from) {
	zrt_last--, zrt_size--;
	if (REF(*zrt_last)) {	/* ok, graduate to normal object */
	    (*zrt_last) -> reference_count |= 1;
	} else {		/* free it */
	    WOOL_send(WOOL_free, *zrt_last, (*zrt_last));
	}
    }
}

/*
 * Never call zrt_put if obj was already in it (sould not happen)
 */

WOOL_OBJECT
zrt_put(obj)
WOOL_OBJECT obj;
{
    WOOL_OBJECT	*old_zrt;

#ifdef DEBUG
    must_not_be_in_zrt(obj);
#endif /* DEBUG */
    if(zrt_size == zrt_limit) {
	zrt_limit = (zrt_limit + 1) * 2 -1;
	old_zrt = zrt;
	zrt = (WOOL_OBJECT *) Realloc(zrt, zrt_limit * sizeof(WOOL_OBJECT));
	zrt_last = zrt + (zrt_last - old_zrt);
    }
    zrt_size ++;
    obj -> reference_count = 0;
    return *zrt_last++ = obj;
}

/* when an object is physically replaced by another, update its entry
 * in the zrt
 */

zrt_replace_element(old, new)
WOOL_OBJECT old;
WOOL_OBJECT new;
{
    WOOL_OBJECT	*zrt_ptr = zrt;

    while (zrt_ptr < zrt_last) {
	if (*zrt_ptr == old) {
	    *zrt_ptr = new;
	    return;
	}
	zrt_ptr++;
    }
#ifdef DEBUG
    wool_error("replaced object 0x%x was not in zrt!", old);
#endif /* DEBUG */
}

#ifdef DEBUG
/* checks that the element is not in fact already in the zrt...
 */

must_not_be_in_zrt(obj)
WOOL_OBJECT obj;
{
    WOOL_OBJECT	*zrt_ptr = zrt;

    while (zrt_ptr < zrt_last) {
	if (*zrt_ptr == obj) {
	    wool_printf("at zrt[%d]", zrt_last - zrt);
	    wool_printf(" and zrt[%d], type: ", zrt_ptr - zrt);
	    wool_print(wool_type(obj));
	    wool_puts(", obj: ");
	    wool_print(obj);
	    wool_newline();
	    wool_error("object 0x%x was already in zrt!", obj);
	}
	zrt_ptr++;
    }
}
#endif /* DEBUG	     */

/***********************\
* 		        *
* reference management  *
* 		        *
\***********************/

/* increase_reference is a macro (REF(x)++) */

#ifdef DEBUG			/* macro otherwise */
increase_reference(obj)
WOOL_OBJECT obj;	/* obj may be UNDEFINED */
{
    REF(obj) += 2;
}

decrease_reference(obj)
WOOL_OBJECT obj;	/* obj may be UNDEFINED */
{
    if (obj) {
	if (((obj -> reference_count) -= 2) == 1)
	    zrt_put(obj);
	else if (obj -> reference_count < 0) {
	    wool_print(obj);
	    wool_error(": reference_count became %d", obj ->
		       reference_count);
	}
    }
}

decrease_reference_non_null(obj)
WOOL_OBJECT obj;	/* obj must be non-nil */
{
	if (((obj -> reference_count) -= 2) == 1)
	    zrt_put(obj);
	else if (obj -> reference_count < 0) {
	    wool_print(obj);
	    wool_error(": reference_count became %d", obj ->
		       reference_count);
	}
}
#endif /* DEBUG */

/*
 * decrease_reference_in_list:
 * decrease reference count of all the elements of the list.
 * but doesn't free the list.
 */

decrease_reference_in_list(count, list)
int    count;
WOOL_OBJECT *list;
{
    WOOL_OBJECT *last = list + count;

    while (list < last){
	decrease_reference(*list);
	list++;
    }
}

/*
 * duplicate an array of objects, increasing the reference count,
 * and mallocing
 */

duplicate_n_objects(source, dest, n)
WOOL_OBJECT    *source;		/* source is the array */
WOOL_OBJECT   **dest;		/* while dest is a POINTER to the array */
int             n;		/* how many to copy */
{
    WOOL_OBJECT *p = source, *q, *last = source + n;

    q = *dest = (WOOL_OBJECT *) Malloc(sizeof(WOOL_OBJECT) * n);
    while (p < last)
        increase_reference(*q++ = *p++);
}

/*
 * duplicate an array of objects, increasing the reference count,
 * without mallocing (dest already points to an malloced aera)
 */

copy_n_objects(source, dest, n)
WOOL_OBJECT    *source;		/* source is the array */
WOOL_OBJECT    *dest;		/* dest is  the array */
int             n;		/* how many to copy */
{
    WOOL_OBJECT *p = source, *q = dest, *last = source + n;

    while (p < last)
	increase_reference(*q++ = *p++);
}


/************************************\
* 				     *
* Delayed Free table module (dft)    *
* 				     *
\************************************/

/* this table holds all memory chunks that should be freed only when at the
 * top-level
 */

#define INITIAL_DFT_SIZE 31	/* pow(2,n)/4 - 1 */

/* This table holds memory chunks that are to be freed at the top-level
 */

dft_init()
{
    dft = (char **) Malloc(INITIAL_DFT_SIZE * sizeof(char *));
    dft_last = dft;
    dft_last_allocated = dft + INITIAL_DFT_SIZE;
}

/* dft_gc disposes really of memory stacked in dft. Defined as a macro
 * while (dft_last > dft) Free(*(--dft_last)) in wool.h
 */

/* put in dft for later freeing
 */

DelayedFree(ptr)
char * ptr;
{
    ASSERT(ptr != NULL);
    if(dft_last == dft_last_allocated) {
	char	**old_dft;
	int size = dft_last - dft, old_size = size;
	size = (size + 1) * 2 -1;
	old_dft = dft;
	dft = (char **) Realloc(dft, size * sizeof(char *));
	dft_last = dft + old_size;
	dft_last_allocated = dft + size;
    }
    *dft_last++ = ptr;
}
