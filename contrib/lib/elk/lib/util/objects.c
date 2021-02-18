#include <varargs.h>
#include <scheme.h>
#include "objects.h"

#define INIT_SIZE	50
#define SIZE_INCR	20

typedef struct {
    GENERIC group;
    Object obj;
    PFO term;
    char flags;
} OBJECT;
static OBJECT *Pool;
static pool_size = INIT_SIZE;

#define USED    0x1   /* flags */
#define LEADER  0x2
#define MARK    0x4

extern char *malloc(), *realloc();

/* Register an object with the given group and termination function;
 * object can be marked as LEADER.
 */
Register_Object (obj, group, term, leader_flag) Object obj; GENERIC group;
	PFO term; {
    register OBJECT *p;

    for (p = Pool; p < Pool+pool_size; p++)
	if (!(p->flags & USED)) break;
    if (p == Pool+pool_size) {
	pool_size += SIZE_INCR;
	if ((Pool = (OBJECT *)realloc ((char *)Pool,
		pool_size * sizeof (OBJECT))) == 0)
	    Fatal_Error ("realloc: out of memory");
	p = Pool + pool_size - SIZE_INCR;
	Clear_Pool (p, SIZE_INCR);
    }
    p->obj = obj;
    p->group = group;
    p->term = term;
    p->flags = leader_flag ? (USED|LEADER) : USED;
}

Deregister_Object (obj) Object obj; {
    register OBJECT *p;

    for (p = Pool; p < Pool+pool_size; p++)
	if ((p->flags & USED) && EQ(p->obj, obj))
	    p->flags = 0;
}

/* Search for an object of a given type and group.
 * Use the given match function; it is called with an object and
 * the remaining arguments of Find_Object() (a va_list).
 * Null is returned when the object has not been found.
 */
/*VARARGS*/
Object Find_Object (va_alist) va_dcl {
    register OBJECT *p;
    register type;
    register GENERIC group;
    PFO match;
    va_list args;

    va_start (args);
    type = va_arg (args, int);
    group = va_arg (args, GENERIC);
    match = va_arg (args, PFO);
    for (p = Pool; p < Pool+pool_size; p++) {
	if (!(p->flags & USED) || TYPE(p->obj) != type || p->group != group)
	    continue;
	if (match (p->obj, args)) {
	    va_end (args);
	    return p->obj;
	}
    }
    va_end (args);
    return Null;
}

/* Terminate all objects belonging to the given group except LEADERs.
 */
Terminate_Group (group) GENERIC group; {
    register OBJECT *p;

    for (p = Pool; p < Pool+pool_size; p++)
	if ((p->flags & USED) && p->group == group && !(p->flags & LEADER)) {
	    if (p->term)
		(void)p->term (p->obj);
	    p->flags = 0;
	}
}

/* The after-GC function.  LEADERs are terminated in a second pass.
 */
static void Terminate_Objects () {
    register OBJECT *p;
    register Object *tag;

    for (p = Pool; p < Pool+pool_size; p++) {
	if (!(p->flags & USED))
	    continue;
	tag = (Object *)POINTER(p->obj);
	if (TYPE(*tag) == T_Broken_Heart) {
	    SETPOINTER(p->obj, POINTER(*tag));
	} else if (p->flags & LEADER) {
	    p->flags |= MARK;
	} else {
	    if (p->term)
		(void)p->term (p->obj);
	    p->flags = 0;
	}
    }
    for (p = Pool; p < Pool+pool_size; p++) {
	if (p->flags & MARK) {
	    if (p->term)
		(void)p->term (p->obj);
	    p->flags = 0;
	}
    }
}

/* Compute a unique integer from an object.
 * -1 is returned if the object is not in the pool.
 */
Unique_Id (obj) Object obj; {
    register OBJECT *p;

    for (p = Pool; p < Pool+pool_size; p++)
	if ((p->flags & USED) && EQ(p->obj, obj))
	    return Make_Fixnum (p-Pool);
    return -1;
}

static Clear_Pool (p, n) register OBJECT *p; register n; {
    for ( ; n > 0; n--, p++)
	p->flags = 0;
}

init_util_objects () {
    if ((Pool = (OBJECT *)malloc (INIT_SIZE *
	    (sizeof (OBJECT)))) == 0)
	Fatal_Error ("malloc: out of memory");
    Clear_Pool (Pool, INIT_SIZE);
    Register_After_GC (Terminate_Objects);
}
