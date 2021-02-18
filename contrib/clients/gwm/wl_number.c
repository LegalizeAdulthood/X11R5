/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/***********************\
* 		        *
*  WOOL_OBJECT: Number  *
*  BODY		        *
* 		        *
\***********************/

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_atom.h"
#include "wl_list.h"
#include "wl_pointer.h"
#include "INTERN.h"
#include "wl_number.h"

/* pre-calculated numbers for efficiency */
#define MIN_BUILT_IN_NUMBER -1
#define MAX_BUILT_IN_NUMBER 10
static WOOL_Number
	built_in_numbers[MAX_BUILT_IN_NUMBER - MIN_BUILT_IN_NUMBER + 1];

/*
 * Constructor: WLNumber_make
 *   argument 1: The Num (int of size char *) which is to be our number.
 */

WOOL_Number 
WLNumber_make(i)
Num            i;	/* just the Num value of the integer */
{
    WOOL_Number object;

    if (i <= MAX_BUILT_IN_NUMBER && i >= MIN_BUILT_IN_NUMBER)
	return built_in_numbers[i - MIN_BUILT_IN_NUMBER];
    object = (WOOL_Number) Malloc(sizeof(struct _WOOL_Number));
    zrt_put(object);
    object -> type = WLNumber;
    object -> number = i;
    return object;
}

WOOL_Number
WLNumber_raw_make(i)
long            i;     /* just the LONG value of the integer */
{
    WOOL_Number object =
    (WOOL_Number) Malloc(sizeof(struct _WOOL_Number));

    zrt_put(object);
    object -> type = WLNumber;
    object -> number = i;
    return object;
}


/*
 * pre-calculate "low" numbers (most used ones)
 */

WLNumber_init()
{
    long            i;

    for (i = MIN_BUILT_IN_NUMBER; i <= MAX_BUILT_IN_NUMBER; i++)
	increase_reference(built_in_numbers[i - MIN_BUILT_IN_NUMBER]
			   = WLNumber_raw_make(i));
    /* calculates the UNDEFINED_WOOL_VALUE */
    UNDEFINED_WOOL_VALUE = (WOOL_OBJECT) WLNumber_raw_make(0);
    increase_reference(UNDEFINED_WOOL_VALUE);
}

/*
 * WLNumber_eval:
 * A number evaluates to itself.
 */

WOOL_OBJECT 
WLNumber_eval(obj)
WOOL_OBJECT obj;
{
    return obj;
}

/*
 * WLNumber_print:
 * a number prints as a long.
 */

WOOL_OBJECT 
WLNumber_print(obj)
WOOL_Number     obj;
{
    wool_printf("%ld", obj -> number);
    return (WOOL_OBJECT) obj;
}

/*
 * WLNumber_free:
 * The structure is just freed.
 */

WOOL_OBJECT 
WLNumber_free(obj)
WOOL_Number     obj;
{
    Free(obj);
    return NULL;
}

/*
 * handles its number as an C object. This is the normal way to use pointers
 */

long *
WLNumber_get_C_value(obj)
WOOL_Number obj;
{
    return (long *) (obj->number);
}

WOOL_OBJECT
WLNumber_equal(n1, n2)
WOOL_Number n1, n2;
{
    if (n2 -> type == n1 -> type && n2 -> number == n1 -> number)
	return (WOOL_OBJECT) n1;
    else
	return NIL;
}


WOOL_OBJECT
add_numbers(argc,argv)
int		argc;
WOOL_Number     argv[];
{
    int             i, result = 0;

    for (i = 0; i < argc; i++) {
	must_be_number(argv[i], i);
	result += argv[i] -> number;
    }
    return (WOOL_OBJECT) WLNumber_make(result);
}
