/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*********************\
* 		      *
*  WOOL_OBJECT  List  *
*  BODY		      *
* 		      *
\*********************/

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_number.h"
#include "wl_atom.h"
#include "wl_name.h"
#include "wl_coll.h"
#include "wl_func.h"
#include "INTERN.h"
#include "wl_list.h"

/*
 * Constructor:
 * used mainly in parser, called by WLList_make
 */

WOOL_List 
WLList_make(col)
WOOL_Collection col;
{
    if (col -> size) {
	WOOL_List       object = (WOOL_List)
	Malloc(sizeof(struct _WOOL_List) +
	       ((col -> size -1) * sizeof(WOOL_OBJECT)));

	zrt_put(object);
	object -> type = WLList;
	copy_n_objects(col -> list, object -> list, col -> size);
	object -> size = col -> size;
	return object;
    } else {			/* a list of size 0 is just NIL */
	return (WOOL_List) NIL;
    }
}

/* makes a list with size slots, to be filled by the C programmer */

WOOL_List
wool_list_make(size)
int	size;
{
     WOOL_List       wl_list;

    wl_list = (WOOL_List) Malloc(sizeof(struct _WOOL_List) +
                                 (size -1) * sizeof(WOOL_OBJECT));
    zrt_put(wl_list);
    wl_list -> type = WLList;
    wl_list -> size = size;
    return wl_list;
}

/* makes a list of argc, argv WOOL_OBJECTS */

WOOL_List
wool_list_make_from_evaluated_array(argc, argv)
int argc;
WOOL_OBJECT     argv[];
{
    WOOL_List       wl_list;
    int             i;

    wl_list = (WOOL_List) Malloc(sizeof(struct _WOOL_List) +
				 (argc - 1) * sizeof(WOOL_OBJECT));
    zrt_put(wl_list);
    wl_list -> type = WLList;
    wl_list -> size = argc;
    for (i = 0; i < argc; i++)
	increase_reference(wl_list -> list[i] =
			   WOOL_send(WOOL_eval, argv[i], (argv[i])));
    return wl_list;
}

/* externally callable list-make:
 * (list-make n elements...)
 */

WOOL_OBJECT
wool_list_make_from_wool(argc, argv)
int argc;
WOOL_OBJECT     argv[];
{
    WOOL_List       wl_list;
    int             i;

    if (argc < 1)
	wool_error(BAD_NUMBER_OF_ARGS, argc);
    must_be_number(argv[0], 0);
    wl_list = wool_list_make(((WOOL_Number) argv[0]) -> number);
    if (argc == 1)
	for (i = 0; i < wl_list -> size; i++)
	    increase_reference(wl_list -> list[i] = NIL);
    else
	for (i = 0; i < wl_list -> size; i++)
	    increase_reference(wl_list -> list[i] =
			       argv[(i % (argc - 1)) + 1]);
    return (WOOL_OBJECT) wl_list;
}

/*
 * WLList_eval:
 * the heart of the interpreter:
 * evaluating a list is calling the function associated with car by
 * sending WOOL_execute to the CAR, with the list of parameters.
 */

WOOL_OBJECT 
WLList_eval(obj)
WOOL_List obj;
{
    WOOL_OBJECT     result;

    calling_function_push(obj);
#ifdef USER_DEBUG
    if (wool_tracing_on) {
	wool_tracing_level++;
	wool_put_spaces(wool_tracing_level);
	wool_puts("-> ");
	wool_print(obj);
	wool_puts("\n");
	if (wool_tracing_on_EXPR) {
	    wool_tracing_on = 0;
	    eval(wool_tracing_on_EXPR);
	    wool_tracing_on = 1;
	}
	result = WOOL_send(WOOL_execute, (*(obj -> list)),
			   (*(obj -> list), obj));
	if (wool_tracing_on_EXPR) {
	    wool_tracing_on = 0;
	    eval(wool_tracing_on_EXPR);
	    if (wool_still_tracing)
		wool_tracing_on = 1;
	    else
		wool_still_tracing = 1;
	}
	wool_put_spaces(wool_tracing_level);
	wool_puts("<- ");
	wool_print(result);
	wool_puts("\n");
	wool_tracing_level--;
    } else
#endif /* USER_DEBUG */
    {
#ifdef DEBUG			/* stack overflow check */
	if ((calling_function_current - calling_function_stack) >= 10000) {
	    printf("Stack size is %d!\n", (calling_function_current 
					   - calling_function_stack)); 
	    stop_if_in_dbx(); 
	}
#endif /* DEBUG */
	result = WOOL_send(WOOL_execute, (*(obj -> list)),
			   (*(obj -> list), obj));
    }
    calling_function_pop();
    return result;
}

/*
 * WLList_print:
 * classical: "(a b c d)"
 */

WOOL_OBJECT 
WLList_print(obj)
WOOL_List       obj;
{
    int             i;
    WOOL_OBJECT    *p = obj -> list;

    wool_putchar('(');
    if(++wool_print_level <= wool_max_print_level) {
	for (i = 0; i < obj -> size; i++, p++) {
	    if (i)
		wool_putchar(' ');
	    WOOL_send(WOOL_print, *p, (*p));
	}
    }else{
	wool_puts("...");
    }
    wool_print_level--;
    wool_putchar(')');
    return (WOOL_OBJECT) obj;
}

/*
 * freeing a list decreases the reference count of all the elements before
 * freeing itself!
 */

WOOL_OBJECT 
WLList_free(obj)
WOOL_List       obj;
{
    WOOL_OBJECT *last = obj -> list + obj -> size, *list = obj -> list;

    while (list < last) {
	decrease_reference(*list);
	list++;
    }
    Free(obj);
    return NULL;
}

/*
 * executing a list is evaluating it and sending execute to the result
 */

WOOL_OBJECT
WLList_execute(obj, list)
WOOL_List  obj;
WOOL_List list;
{
    WOOL_OBJECT     eval = WLList_eval(obj);

    if (eval -> type != WLList) {
	return WOOL_send(WOOL_execute, eval, (eval, list));
    } else {
	return (wool_error(UNDEFINED_FUNCTION, obj));
    }
}

/*
 * equality of two lists is equality of their elements
 */

WOOL_OBJECT
WLList_equal(l1, l2)
WOOL_List l1,l2;
{
    int             i;

    if ((l2 -> type != l1 -> type)
	|| (l2 -> size != l1 -> size))
	return NIL;
    for (i = 0; i < l1 -> size; i++) {
	if (WOOL_send(WOOL_equal, l1 -> list[i],
		      (l1 -> list[i], l2 -> list[i])) == NIL)
	    return NIL;
    }
    return (WOOL_OBJECT) l1;
}

WOOL_OBJECT *
wool_flatten_pairlist(argc, argv)
int    argc;
WOOL_List *argv;
{
    WOOL_OBJECT    *list =
    (WOOL_OBJECT *) Malloc(sizeof(WOOL_OBJECT) * 2 * argc);
    int             i;

    for (i = 0; i < argc; i++) {
	if ((argv[i] -> type != WLList) || (argv[i] -> size != 2))
	    return (WOOL_OBJECT *) wool_error(BAD_LOCAL_SYNTAX, "");
	list[i * 2] = argv[i] -> list[0];
	list[i * 2 + 1] = argv[i] -> list[1];
    }
    return list;
}
    

/*****************************\
* 			      *
* List manipulation routines  *
* 			      *
\*****************************/

/*
 * returns sublist ifrom... ito
 * (sublist from to list)
 * from included, to excluded
 */

WOOL_OBJECT
WLList_sub(argc, argv)
int argc;
WOOL_List argv[];
{
    WOOL_List newlist;
    int             from, to, i;

    if (argc != 3)
	wool_error(BAD_NUMBER_OF_ARGS, argc);
    must_be_number(argv[0], 0);
    must_be_number(argv[1], 1);
    must_be_or_nil(WLList, argv[2], 2);
    from = ((WOOL_Number) argv[0]) -> number;
    to = ((WOOL_Number) argv[1]) -> number;
    if (from >= to)
	return NIL;
    newlist = wool_list_make(to - from);
    for (i = 0; i < newlist -> size; i++)
	if (i + from < argv[2] -> size) {
	    increase_reference(newlist -> list[i] =
			       argv[2] -> list[i + from]);
	} else {
	    increase_reference(newlist -> list[i] = NIL);
	}
    return (WOOL_OBJECT) newlist;
}


WOOL_OBJECT
WLList_concat(argc,argv)
int argc;
WOOL_List       argv[];
{
    WOOL_List newlist;
    int             i, newsize = 0, size;

    for (i = 0; i < argc; i++) {
	must_be_or_nil(WLList, argv[i], i);
	newsize += ((argv[i] == (WOOL_List) NIL) ? 0 : argv[i] -> size);
    }
    if (!newsize)
	return NIL;
    newlist = wool_list_make(newsize);
    newsize = 0;
    for (i = 0; i < argc; i++) {
	if (size = ((argv[i] == (WOOL_List) NIL) ? 0 : argv[i] -> size))
	    copy_n_objects(argv[i] -> list, &(newlist -> list)[newsize],
			   size);
	newsize += size;
    }
    return (WOOL_OBJECT) newlist;
}

WOOL_List
WLList_physically_append(list, key, value)
WOOL_List 	list;
WOOL_OBJECT	key;
WOOL_OBJECT	value;
{
    WOOL_List   old_list = list;

    if (key -> type == WLNumber) {
	list = (WOOL_List) Realloc(list, sizeof(struct _WOOL_List) +
              list -> size * sizeof(WOOL_OBJECT));
    } else {
	list = (WOOL_List) Realloc(list, sizeof(struct _WOOL_List) +
              (list -> size + 1) * sizeof(WOOL_OBJECT));
	increase_reference(list -> list[list -> size] = key);
	list -> size ++;
    }
    increase_reference(list -> list[list -> size] = value);
    list -> size ++;
    if (list != old_list && !(REF(list) & 1)) {	/* old list was in zrt! */
	zrt_replace_element(old_list, list);
    }
    return list;
}
    

int
WLList_length(list)
WOOL_List       list;
{
    if (list == (WOOL_List) NIL)
	return 0;
    else
	return list -> size;
}

/*
 * (# n list) gives n-th element (nil if out bounds) returns obj
 * (# n list obj) sets n-th element (increasing size if needed) returns
 * 	newlist
 * referencing via an atom is like giving the n tof the position after atom
 * list can also be an atom or a wob
 */

#ifdef GWM
#include "EXTERN.h"
#include "gwm.h"		/* TO_DO remove and treat as method */
#endif /* GWM */

WOOL_OBJECT
WLList_nth(argc, argv)
int		argc;
WOOL_List	argv[];
{
    WOOL_List newlist, list;
    int             size, newsize, i;
    int             position, symbolic = 0;

    if (argc < 2 || argc > 3)
	return wool_error(BAD_NUMBER_OF_ARGS, argc);
    if (argv[1] -> type == WLList || argv[1] == (WOOL_List) NIL) {
      list = argv[1];
#ifdef GWM
    } else if (argv[1] -> type == WLNumber) { /* WOB */
	if (!WobIsValid(((WOOL_Number) argv[1]) -> number)
	    || !((Wob) ((WOOL_Number) argv[1])->number) -> property)
	    return NIL;
      list = (WOOL_List)
	((Wob) ((WOOL_Number) argv[1]) -> number) -> property;
#endif /* GWM */
    } else if (argv[1] -> type == WLAtom || argv[1] -> type == WLName) {
	list = (WOOL_List) WOOL_send(WOOL_eval, argv[1], (argv[1]));
	must_be_or_nil(WLList, list, 1);
    } else {
	bad_argument(argv[1], 1, "list or pointer to list");
    }

    size = (list == (WOOL_List) NIL ? 0 : list -> size);
    if (argv[0] -> type == WLNumber)
	position = ((WOOL_Number) argv[0]) -> number;
    else {
	symbolic = 1;
	position = size + 1;
	for (i = 0; i < size; i++) {
	    if (argv[0] == (WOOL_List) list -> list[i]) {
		position = i + 1;
		break;
	    }
	}
    }

    if (argc == 2) {		/* GET */
	if (position < 0 || position >= size)
	    return NIL;
	else
	    return (list) -> list[position];

    } else {			/* SET */
	newsize = Max(size, position + 1);
	newlist = wool_list_make(newsize);
	if (size)
	    copy_n_objects(&(list -> list)[0], newlist -> list, size);
	if (newsize > size) {
	    for (i = size; i < newsize - symbolic; i++)
		increase_reference(newlist -> list[i] = NIL);
	} else {
	    decrease_reference(newlist -> list[position]);
	}
	if (symbolic) {
	    if (size < position)
		increase_reference(newlist -> list[position - 1] =
				   (WOOL_OBJECT) argv[0]);
	    increase_reference(newlist -> list[position] =
			       (WOOL_OBJECT) argv[2]);
	} else {
	    increase_reference(newlist -> list[position] =
			       (WOOL_OBJECT) argv[2]);
	}
	return (WOOL_OBJECT) newlist;
    }
}

/* physical replacement of an element of a list
 * succeeds only if there is room, else returns ()
 * returns list
 */

WOOL_OBJECT
WLList_replace_nth(argc, argv)
int		argc;
WOOL_List	argv[];
{
    int             size, i;
    int             position = 0;
    WOOL_List	    list, *list_ptr;

    if (argc != 3)
	return wool_error(BAD_NUMBER_OF_ARGS, argc);
    if (argv[1] -> type == WLList || argv[1] == (WOOL_List) NIL) {
	list = argv[1];
	list_ptr = NULL;
#ifdef GWM
    } else if (argv[1] -> type == WLNumber) {/* WOB */
	if (!WobIsValid(((WOOL_Number) argv[1]) -> number)
	    || !((Wob) ((WOOL_Number) argv[1])->number) -> property)
	    return NIL;
	list_ptr = (WOOL_List*)&
		     ((Wob) ((WOOL_Number) argv[1])->number) -> property;
	list = *list_ptr;
	must_be_or_nil(WLList, list, 1);
#endif /* GWM */
    } else if (argv[1] -> type == WLAtom) {
	list_ptr = (WOOL_List*)& ((WOOL_Atom) argv[1])->c_val;
	list = *list_ptr;
	must_be_or_nil(WLList, list, 1);
    } else if (argv[1] -> type == WLName) {
	list_ptr = (WOOL_List *) WLName_slot(argv[1]);
	list = *list_ptr;
	must_be_or_nil(WLList, list, 1);
    } else {
	bad_argument(argv[1], 1, "list or pointer to list");
    }

    size = (list == (WOOL_List) NIL ? 0 : list -> size);
    if (argv[0] -> type == WLNumber)
	position = Max(0, ((WOOL_Number) argv[0]) -> number);
    else {
	position = size + 1;
	for (i = 0; i < size; i++) {
	    if (argv[0] == (WOOL_List) list -> list[i]) {
		position = i + 1;
		break;
	    }
	}
    }

    if (position >= size) {	/* increase size */
	if (list_ptr) {
	    if (REF(list) >= 4) {/* 2 or more objs point to it */
		decrease_reference(list);
		increase_reference(*list_ptr = list =
				   (WOOL_List) WLList_nth(argc, argv));
	    } else {		/* pointed by one object */
		*list_ptr = list =
		  WLList_physically_append(list, argv[0], argv[2]);
	    }
	} else {		/* cannot resize a non-pointed to list */
	    wool_error("Could not modify %s\n", "list");
	}
    } else {			/* replace an existing element */
	if (list_ptr && REF(list) >= 4) { /* duplicate first */
	    decrease_reference(list);
	    increase_reference(*list_ptr = list =
			       (WOOL_List) WLList_nth(argc, argv));
	} else {		/* physical modif */
	    decrease_reference(list -> list[position]);
	    increase_reference(list -> list[position]
			       = (WOOL_OBJECT) argv[2]);
	}
    }
    return (WOOL_OBJECT) list;
}

/* physical deletion of an element of a list
 * succeeds only if there is room, else returns ()
 * returns list
 */

WOOL_OBJECT
WLList_delete_nth(key, wl_list)
WOOL_OBJECT     key;
WOOL_List	wl_list;
{
    int             size, i;
    int             position = 0;
    WOOL_List	    list, *list_ptr;

    if (wl_list -> type == WLList || wl_list == (WOOL_List) NIL) {
	list = wl_list;
	list_ptr = NULL;
#ifdef GWM
    } else if (wl_list -> type == WLNumber) {/* WOB */
	if (!WobIsValid(((WOOL_Number) wl_list) -> number)
	    || !((Wob) ((WOOL_Number) wl_list)->number) -> property)
	    return NIL;
	list_ptr = (WOOL_List*)&
			((Wob) ((WOOL_Number) wl_list) -> number) -> property;
	list = *list_ptr;
	must_be_or_nil(WLList, list, 1);
#endif /* GWM */
    } else if (wl_list -> type == WLAtom) {
	list_ptr = (WOOL_List*) & ((WOOL_Atom) wl_list) -> c_val;
	list = *list_ptr;
	must_be_or_nil(WLList, list, 1);
    } else if (wl_list -> type == WLName) {
	list_ptr = (WOOL_List *) WLName_slot(wl_list);
	list = *list_ptr;
	must_be_or_nil(WLList, list, 1);
    } else {
	bad_argument(wl_list, 1, "list or pointer to list");
    }

    size = (list == (WOOL_List) NIL ? 0 : list -> size);
    if (key -> type == WLNumber)
	position = Max(0, ((WOOL_Number) key) -> number);
    else {
	position = size + 1;
	for (i = 0; i < size; i++) {
	    if (key == list -> list[i]) {
		position = i + 1;
		break;
	    }
	}
    }

    if (position < size) {	/* increase size */
	if (list_ptr && REF(list) >= 4) { /* duplicate first */
	    decrease_reference(list);
	    increase_reference(*list_ptr = list =
			       (WOOL_List) WLList_copy(list));
	}
				/* delete elements */
	{
	    int i;
	    int number = (key -> type == WLNumber ? 1 : 2);

	    decrease_reference(list -> list[position - number + 1]);
	    list -> size -= number;
	    for (i = position - number + 1; i < list -> size; i++)
	        list -> list[i] = list -> list[i + number];
	}
    }
    return (WOOL_OBJECT) list;
}

/* the quicksort of a list
 */

static WOOL_List WLList_qsort_compare_call;

int
WLList_qsort_compare_func(o1, o2)
WOOL_OBJECT	*o1, *o2;
{
    WOOL_Number wl_num;

    WLList_qsort_compare_call -> list[1] = *o1;
    WLList_qsort_compare_call -> list[2] = *o2;
    wl_num = (WOOL_Number) WOOL_send(WOOL_execute,
    	(WLList_qsort_compare_call -> list[0]),
	(WLList_qsort_compare_call -> list[0], WLList_qsort_compare_call));

    if (wl_num -> type != WLNumber)
        wool_error("sort compare function must return a number, not a %s",
	((WOOL_Atom) wl_num -> type[0]) -> p_name);
    return wl_num -> number;
}

WOOL_OBJECT
WLList_qsort(list, compare_func)
WOOL_List	list;
WOOL_OBJECT	compare_func;		/* any function */
{
    if (list->type != WLList)
	return NIL;
    if (!WLList_qsort_compare_call) {
	increase_reference(WLList_qsort_compare_call = wool_list_make(3));
    }
    WLList_qsort_compare_call -> list[0] = compare_func;
    qsort((char *) list -> list, list ->size, sizeof(WOOL_OBJECT),
    	WLList_qsort_compare_func);
    return (WOOL_OBJECT) list;	
}

/* copy of a list (but not of its elements)
 */

WOOL_OBJECT
WLList_copy(list)
WOOL_List	list;
{
    if (list -> type != WLList)		/* copy of () = () */
    	return (WOOL_OBJECT) list;
    else 				/* normal case */
	return (WOOL_OBJECT) wool_list_make_from_evaluated_array(
		list -> size, list -> list);
}

