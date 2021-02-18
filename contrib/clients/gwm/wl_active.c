/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/************************\
* 			 *
*  WOOL_OBJECT: Active	 *
*  BODY			 *
* 			 *
\************************/

/*
 * Actives stand for "active-values", i.e. variables triggering a function
 * when evaluated and another when set. Used like atoms to decrease number
 * of parentheses
 */

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_atom.h"
#include "wl_list.h"
#include "wl_number.h"
#include "wl_string.h"
#include "INTERN.h"
#include "wl_active.h"

/*
 * wool_active_make:
 * High level function callable from C. Makes a active with a string and a
 * C active
 */

WOOL_Active
wool_active_make(name, get, set)
char *name;
WOOL_METHOD get, set;
{
    WOOL_Atom       atom = wool_atom(name);

    atom -> type = WLActive;	/* just change type of object */
    ((WOOL_Active) atom) -> get = get;
    ActiveSet(atom) = set;
    return (WOOL_Active) atom;
}

/*
 * Evaluating an active executes the GET function
 */

WOOL_OBJECT
WLActive_eval(obj)
WOOL_Active obj;
{
    if (obj -> get)
	return (WOOL_OBJECT) (*(obj -> get)) ();
    else
	return NIL;
}

/*
 * Setting an active executes the SET function
 */

WOOL_OBJECT
WLActive_set(obj, value)
WOOL_Active obj;
WOOL_OBJECT value;
{
    if (ActiveSet(obj))
	return (WOOL_OBJECT) (*(ActiveSet(obj))) (
				      WOOL_send(WOOL_eval, value, (value)));
    else
	return NIL;
}

WOOL_OBJECT
WLActive_setq(obj, value)
WOOL_Active obj;
WOOL_OBJECT value;
{
    if (ActiveSet(obj))
	return (WOOL_OBJECT) (*(ActiveSet(obj))) (value);
    else
	return NIL;
}

/*
 * WLActive_execute:
 * 	with 1 arg, executes set
 * 	with 0 arg, executes get
 */

WOOL_OBJECT 
WLActive_execute(obj, list)
WOOL_Active     obj;
WOOL_List 	 list;
{
    if ((list -> size == 1) && obj -> get)	/* GET */
	return (*(obj -> get)) ();
    else if (ActiveSet(obj))			/* SET */
	return (*(ActiveSet(obj)))
	    (WOOL_send(WOOL_eval, (list -> list)[1], ((list -> list)[1])));
    else
	return NIL;
}

/*
 * The C value of a active is the C_value of the GET result
 */

WOOL_OBJECT
WLActive_get_C_value(obj)
WOOL_Active obj;
{
    WOOL_OBJECT tmp = (obj -> get ? (*(obj -> get))() : NIL);

    return WOOL_send(WOOL_get_C_value, tmp, (tmp));
}

