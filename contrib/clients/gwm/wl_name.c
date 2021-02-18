/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/**************************\
* 			   *
*  WOOL_OBJECT: Namespace  *
*  BODY			   *
* 			   *
\**************************/

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_atom.h"
#include "wl_list.h"
#include "wl_number.h"
#include "wl_string.h"
#include "INTERN.h"
#include "wl_name.h"

#define WLNAMESPACE_INITIAL_SIZE 3	/* initial size (in longs) */
#define DELETED (WOOL_OBJECT *)	-1	/* slot is vacant */
#define NONE_DELETED		-1	/* no slot is vacant */

int WLNamespace_search_deleted();

/* High level function callable from wool. Makes a new namespace.
 */

WOOL_Namespace
WLNamespace_make()
{
    WOOL_Namespace object = (WOOL_Namespace)
    Malloc(sizeof(struct _WOOL_Namespace));

    bzero(object, sizeof(struct _WOOL_Namespace));
    object -> type = WLNamespace;
    zrt_put(object);
    object -> first_deleted = NONE_DELETED;
    return object;
}

/* adding a namespace domain to a namespace. returns int of created namespace
 */

WOOL_OBJECT
WLNamespace_add(namespace)
WOOL_Namespace namespace;
{
    int             new_index;

    must_be_namespace(namespace, 0);
    if (!namespace -> number)		/* non-empty namespaces */
	increase_reference(namespace);	/* must never disapear */
    if (namespace -> first_deleted == NONE_DELETED) {
	new_index = namespace -> number++;
	namespace -> namespaces = (WOOL_OBJECT **)
	    Realloc(namespace -> namespaces,
		    namespace -> number * sizeof(WOOL_OBJECT *));
    } else {
	new_index = namespace -> first_deleted;
	namespace -> first_deleted =
	    WLNamespace_search_deleted(namespace, new_index);
    }
    if (namespace -> limit)
	namespace -> namespaces[new_index] = (WOOL_OBJECT *)
	    Malloc(sizeof(WOOL_OBJECT) * namespace -> limit);
    else
	namespace -> namespaces[new_index] = 0;
    return (WOOL_OBJECT) WLNumber_make(new_index);
}

/* removing a namespace.
 * just puts a DELETED in the entry
 */

WOOL_OBJECT
WLNamespace_remove(namespace, number)
WOOL_Namespace namespace;
WOOL_Number	number;
{
    int             index;
    WOOL_OBJECT    *p;

    must_be_namespace(namespace, 0);
    must_be_number(number, 1);
    index = number -> number;
    if (index < 0 || index >= namespace -> number)
	return NIL;
    for (p = namespace -> namespaces[index];
	 p < (namespace -> namespaces[index]) + namespace -> limit;
	 p++)
	decrease_reference(*p);
    Free(namespace -> namespaces[index]);
    namespace -> namespaces[index] = DELETED;
    if (namespace -> first_deleted == NONE_DELETED
	|| namespace -> first_deleted > index)
	namespace -> first_deleted = index;
    return NIL;
}

/* searches for first vacant spot
 */

int
WLNamespace_search_deleted(namespace, index)
WOOL_Namespace namespace;
int	       index;
{
    WOOL_OBJECT   **p;

    for (p = namespace -> namespaces + index;
	 p < namespace -> namespaces + namespace -> number;
	 p++) {
	if (*p == DELETED)
	    return p - namespace -> namespaces;
    }
    return NONE_DELETED;
}


/* adding a new name entry in a namespace
 * returns new index
 */

Num
WLNamespace_add_name(namespace)
WOOL_Namespace namespace;
{
    int             i;
    WOOL_OBJECT    *p;

    if (namespace -> size >= namespace -> limit) {	/* must realloc */
	if (namespace -> limit)
	    namespace -> limit = namespace -> limit * 2 + 1;
	else
	    namespace -> limit = WLNAMESPACE_INITIAL_SIZE;
	for (i = 0; i < namespace -> number; i++) {
	    namespace -> namespaces[i] = (WOOL_OBJECT *) Realloc(
			    namespace -> namespaces[i],
			    namespace -> limit * sizeof(WOOL_OBJECT));
	    for (p = (namespace -> namespaces[i]) + namespace -> size;
		 p < (namespace -> namespaces[i]) + namespace -> limit;
		 p++)
		increase_reference(*p = UNDEFINED_WOOL_VALUE);
	}
    }
    return (Num) namespace -> size++;
}

/* adding a name in a namespace
 * (defname 'name namespace [value]) ==> sets value in each namespace state
 * if no value given, and atom had a previous value, sets all name values
 * this value
 */

WOOL_OBJECT
WLName_add(argc, argv)
int		argc;
WOOL_OBJECT      argv[];
{
    WOOL_OBJECT     value;

    switch (argc) {
    case 2:
	value = 0;
	break;
    case 3:
	value = argv[2];
	break;
    default:
	return wool_error(BAD_NUMBER_OF_ARGS, argc);
    }

    must_be_namespace(argv[1], 1);
    if (argv[0] -> type == WLAtom) {
	if (!value && ((WOOL_Atom) argv[0]) -> c_val)
	    value = (WOOL_OBJECT)
		WLQuotedExpr_make(((WOOL_Atom) argv[0]) -> c_val);
	decrease_reference(((WOOL_Atom) argv[0]) -> c_val);
	argv[0] -> type = WLName;
	NameNamespace(((WOOL_Name) argv[0])) = ((WOOL_Namespace) argv[1]);
	((WOOL_Name) argv[0]) -> index =
	    WLNamespace_add_name((WOOL_Namespace) argv[1]);
    } else if (argv[0] -> type == WLName) {
	if (NameNamespace(((WOOL_Name) argv[0])) !=
	    ((WOOL_Namespace) argv[1])) {
	    WLName_release_values(argv[0]);
	    NameNamespace(((WOOL_Name) argv[0])) = ((WOOL_Namespace) argv[1]);
	    ((WOOL_Name) argv[0]) -> index =
		WLNamespace_add_name((WOOL_Namespace) argv[1]);
	} else {
	    return argv[0];
	}
    } else
	bad_argument(argv[0], 0, "symbol");
    if (value) {			/* set to arg */
	WLName_set_all(argv[1], argv[0], value);
    }
    return argv[0];
}

/* manage active namespace
 * (set-current-namespace namespace current)
 * with current out-of-bounds (e.g -1) returning current one
 */

WOOL_OBJECT
WLNamespace_set_current(namespace, index)
WOOL_Namespace namespace;
WOOL_Number    index;
{
    must_be_namespace(namespace, 0);
    must_be_number(index, 1);
    if (index -> number >= 0 && index -> number < namespace -> number) {
	namespace -> current = index -> number;
	if (namespace -> trigger)
	    (*(namespace -> trigger))(index -> number);
	return (WOOL_OBJECT) index;
    } else {
	return (WOOL_OBJECT) WLNumber_make(namespace -> current);
    }
}

/* size of states in the namespace
 */

WOOL_OBJECT
WLNamespace_size(namespace)
WOOL_Namespace namespace;
{
    must_be_namespace(namespace, 0);
    return (WOOL_OBJECT) WLNumber_make(namespace -> number);
}

/* namespace-of:
 * on a name returns namespace or NIL if global
 */

WOOL_OBJECT
WLName_namespace(name)
WOOL_Name      name;
{
    if (name -> type == WLAtom)
	return NIL;
    else if (name -> type == WLName)
	return (WOOL_OBJECT) NameNamespace(name);
    else
	return bad_argument(name, 0, "symbol");
}

/* free all c_val values */

WLName_release_values(name)
WOOL_Name      name;
{
    WOOL_Namespace  namespace = NameNamespace(name);
    int             i;

    for (i = 0; i < namespace -> number; i++)
	decrease_reference((namespace -> namespaces[i])[name -> index]);
}

WOOL_OBJECT
WLName_unbind(name)
WOOL_Name      name;
{
    WOOL_Namespace  namespace = NameNamespace(name);

    decrease_reference((namespace -> namespaces[namespace -> current])
		       [name -> index]);
    (namespace -> namespaces[namespace -> current])
	[name -> index] = UNDEFINED_WOOL_VALUE;
    return NIL;
}

/* namespace methods */

WOOL_OBJECT
WLNamespace_print(obj)
WOOL_Namespace obj;
{
    wool_printf("{NAMESPACE 0x%x ", obj);
    wool_printf("(%d spaces", obj -> number);
    wool_printf(" of %d names)}", obj -> size);
    return (WOOL_OBJECT) obj;
}

WOOL_OBJECT
WLNamespace_free(namespace)
WOOL_Namespace namespace;
{
    free(namespace);
    return NULL;
}	    

/* name methods */

/*
 * Evaluating an name returns the pointed slot
 */

WOOL_OBJECT
WLName_eval(name)
WOOL_Name name;
{
    WOOL_Namespace  namespace = NameNamespace(name);
    WOOL_OBJECT     value;

    if ((value = ((namespace -> namespaces)[namespace -> current])
	[name -> index]) != UNDEFINED_WOOL_VALUE)
	return value;
    else
	return wool_error(UNDEFINED_VARIABLE, name -> p_name);
}

WOOL_OBJECT *
WLName_slot(name)
WOOL_Name name;
{
    WOOL_Namespace  namespace = NameNamespace(name);

    return &(((namespace -> namespaces)[namespace -> current])[name -> index]);
}

WOOL_OBJECT
WLName_silent_eval(name)
WOOL_Name name;
{
    WOOL_Namespace  namespace = NameNamespace(name);

    return ((namespace -> namespaces)[namespace -> current]) [name -> index];
}

WOOL_OBJECT
WLName_set(name, value)
WOOL_Name name;
WOOL_OBJECT value;
{
    WOOL_OBJECT new = WOOL_send(WOOL_eval, value, (value));
    WOOL_Namespace  namespace = NameNamespace(name);
    WOOL_OBJECT *value_ptr = &(((namespace -> namespaces)
				    [namespace -> current])[name -> index]);

    decrease_reference(*value_ptr);
    increase_reference(*value_ptr = new);
    return new;
}

WOOL_OBJECT
WLName_setq(name, value)
WOOL_Name name;
WOOL_OBJECT value;
{
    WOOL_Namespace  namespace = NameNamespace(name);
    WOOL_OBJECT *value_ptr = &(((namespace -> namespaces)
				    [namespace -> current])[name -> index]);

    decrease_reference(*value_ptr);
    increase_reference(*value_ptr = value);
    return value;
}

/* like set but on all spaces */

WLName_set_all(namespace, name, value)
WOOL_Namespace namespace;
WOOL_Name name;
WOOL_OBJECT value;
{
    WOOL_OBJECT *value_ptr;
    int             i, old_current = namespace -> current;

    if (namespace -> save_state)
	(*(namespace -> save_state)) ();
    for (i = 0; i < namespace -> number; i++) {
	value_ptr = &(((namespace -> namespaces)[i])[name -> index]);
	decrease_reference(*value_ptr);
	namespace -> current = i;
	if (namespace -> trigger)
	    (*(namespace -> trigger)) (i);
	increase_reference(*value_ptr = WOOL_send(WOOL_eval, value, (value)));
    }
    namespace -> current = old_current;
    if (namespace -> trigger)
	(*(namespace -> trigger)) (old_current);
    if (namespace -> restore_state)
	(*(namespace -> restore_state)) ();
}

/*
 * WLName_execute:
 * 	executes the object in the C_val
 */

WOOL_OBJECT 
WLName_execute(name, list)
WOOL_Name        name;
WOOL_List 	 list;
{
    WOOL_Namespace  namespace = NameNamespace(name);
    WOOL_OBJECT value = ((namespace -> namespaces)
				  [namespace -> current])[name -> index];

    if (value != UNDEFINED_WOOL_VALUE && (value -> type != WLAtom)) {
	return WOOL_send(WOOL_execute, value, (value, list));
    } else if (value && (value == NIL)) {
	return NIL;
    } else {
	return (wool_error(UNDEFINED_FUNCTION, name));
    }
}
