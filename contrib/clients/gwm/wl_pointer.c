/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/************************\
* 			 *
*  WOOL_OBJECT: Pointer	 *
*  BODY			 *
* 			 *
\************************/

/* Pointers are like atoms, but instead of pointing toward a WOOL_OBJECT,
 * they refer via their "ptr" field to just ANY memory location able to hold
 * a long.
 * 
 * Thus setting a pointer to a value COPIES this value at the location pointed
 * to.
 * 
 * If a pointer is NULL, then it is an offset in the Context structure,
 * whose offset is taken in the pre-field (set field of active values)
 */

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_atom.h"
#include "wl_list.h"
#include "wl_number.h"
#include "wl_string.h"
#include "wl_active.h"
#include "INTERN.h"
#include "wl_pointer.h"

/*
 * Constructor: WLPointer_make
 *     arg 1: The atom (or pointer) to be used 
 *     arg 2: The pointer to the location which will be updated by
 *            setting this pointer
 *     returns the pointer, which is our arg1 but modified in place
 * 
 * C programmers should rather use wool_pointer_make below.
 */

WOOL_Pointer
WLPointer_make(atom, ptr)
WOOL_Atom atom;			/* previously allocated atom */
long	  *ptr;			/* location pointed to */
{
    must_be_atom(atom, 0);
    if (atom -> type == WLAtom) {	/* free what was pointed */
	decrease_reference(atom -> c_val);
    }
    atom -> type = WLPointer;	/* just change type of object */
    ((WOOL_Pointer) atom) -> ptr = ptr;
    WLPointerBase(atom) = NULL;
    return (WOOL_Pointer) atom;
}

/*
 * wool_pointer_make:
 * High level function callable from C. Makes a pointer with a string and a
 * C pointer
 */

WOOL_Pointer
wool_pointer_make(name, ptr)
char *name;
char *ptr;
{
    return WLPointer_make(wool_atom(name), ptr);
}

/* wool_base_pointer_make:
 * High level function callable from C. Makes a pointer with a string, a base
 * and a C pointer
 */

WOOL_Pointer
wool_base_pointer_make(name, base, ptr)
char *name;
char **base, *ptr;
{
    WOOL_Pointer wl_ptr = WLPointer_make(wool_atom(name), ptr - *base);
    WOOL_Pointer_internal int_ptr =  WL_Pointer_internal(wl_ptr);

    int_ptr -> base = base;
    return wl_ptr;
}

/*
 * wool_self_pointer_make:
 * to make a pointer to a given value (stored in the Quark field)
 * 
 * the prefix is a control char prefixed to the name to act as a domain name
 * for names. Currently chars are used for:
 * 
 * 	^F	for X fonts id
 * 	^X	for X intern atoms
 * 	^M	for mallocated blocs (MLEAK tracing)
 *	^T	for tags
 */

WOOL_Pointer
wool_self_pointer_make(name, prefix, ppointer)
char *name;
char prefix;
WOOL_Pointer *ppointer;
{
    char	prefixed_name[MAX_TEMP_STRING_SIZE + 1];

    prefixed_name[0] = prefix;
    prefixed_name[1] = '\0';
    strcat(prefixed_name, name);
    *ppointer = (WOOL_Pointer) wool_atom(prefixed_name);
    if((*ppointer) -> type == WLAtom) {
	(*ppointer) -> ptr = (long *) (((char *) *ppointer) - 
		(sizeof(struct _WOOL_Active_internal)
		- sizeof(struct _WOOL_Active)));
	(*ppointer) -> type = WLPointer;
	*((*ppointer) -> ptr) = 0;
	return NULL;
    }
    return *ppointer;
}

/*
 * Evaluating a pointer yields the WOOL number containing the value pointed
 * to by the Pointer
 */

WOOL_OBJECT
WLPointer_eval(obj)
WOOL_Pointer obj;
{
    if (WLPointerBase(obj)) {
	WOOL_Pointer_internal int_ptr = WL_Pointer_internal(obj);

	return (WOOL_OBJECT) WLNumber_make(
			   *((long *)( *(int_ptr -> base) + int_ptr -> ptr)));
    } else
	return (WOOL_OBJECT) WLNumber_make(*(obj -> ptr));
}

/*
 * returns the raw value
 */

long
WLPointer_get_C_value(obj)
WOOL_Pointer obj;
{
    if (WLPointerBase(obj)) {
	WOOL_Pointer_internal int_ptr = WL_Pointer_internal(obj);

	return *((long *) (*(int_ptr -> base) + int_ptr -> ptr));
    } else
	return *(obj -> ptr);
}

/*
 * WLPointer_execute:
 * like active-values: without args gets, with 1 arg, sets
 */

WOOL_OBJECT 
WLPointer_execute(obj, list)
WOOL_Pointer     obj;
WOOL_List 	 list;
{
    if (list -> size == 1) {
	return (WOOL_OBJECT) WLPointer_eval(obj);
    } else if (list -> size == 2) {
	return (WOOL_OBJECT) WLPointer_set(obj, list -> list[1]);
    } else {
	return wool_error(BAD_NUMBER_OF_ARGS, list -> size - 1);
    }
}

/*
 * used to set value pointed to
 */

WOOL_OBJECT
WLPointer_set(obj, value)
WOOL_Pointer obj;
WOOL_OBJECT value;
{
    WOOL_OBJECT evaluated_value = WOOL_send(
						 WOOL_eval, value, (value));
    long            C_value = (long) WOOL_send(
		      WOOL_get_C_value, evaluated_value, (evaluated_value));
    WOOL_OBJECT     result = (WOOL_OBJECT) WLNumber_make(C_value);

    if (WLPointerBase(obj)) {
	WOOL_Pointer_internal int_ptr = WL_Pointer_internal(obj);

	*((long *) (*(int_ptr -> base) + int_ptr -> ptr)) = C_value;
    } else
	*(obj -> ptr) = C_value;
    return result;
}

WOOL_OBJECT
WLPointer_setq(obj, value)
WOOL_Pointer obj;
WOOL_OBJECT value;
{
    WOOL_OBJECT     result = (WOOL_OBJECT) WLNumber_make(value);
    long            C_value = (long) WOOL_send(
		      WOOL_get_C_value, value, (value));

    if (WLPointerBase(obj)) {
        WOOL_Pointer_internal int_ptr = WL_Pointer_internal(obj);

        *((long *) (*(int_ptr -> base) + int_ptr -> ptr)) = C_value;
    } else
        *(obj -> ptr) = C_value;
    return result;
}
