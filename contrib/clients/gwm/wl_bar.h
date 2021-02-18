/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/********************\
* 		     *
*  WOOL_OBJECT  Bar  *
*  DEFINITIONS	     *
* 		     *
\********************/

#ifndef INCLUDE_WL_BAR_H
#define INCLUDE_WL_BAR_H
#ifndef INCLUDE_WL_PLUG_H
#include "wl_plug.h"
#endif

/* type */

/*
 * A bar is a list of plugs
 */

typedef struct _WOOL_Bar {
    WOOL_HEADER;
    int             min_width, max_width;
    int		    plug_separator;
    int             borderwidth;
    long            borderpixel;
    long            background;
    WOOL_OBJECT	    property;
    WOOL_OBJECT     bordertile;
    WOOL_OBJECT     fsm;
    WOOL_OBJECT     menu;
    WOOL_OBJECT     cursor;
    WOOL_OBJECT     tile;
    int 	    plugs_size;
    WOOL_Plug	    plugs[1];
}              *WOOL_Bar;

/* exported functions */

EXT WOOL_Bar wool_bar_make();
EXT WOOL_Bar WLBar_make();
EXT WOOL_OBJECT WLBar_print();
EXT WOOL_OBJECT WLBar_free();

/* methods */

EXT WOOL_METHOD WLBar[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLBar_print,			/* WOOL_print 2 */
   WLBar_free,			/* WOOL_free 3 */
   wool_undefined_method_2,	/* WOOL_execute 4 */
   wool_undefined_method_2,	/* WOOL_set 5 */
   wool_undefined_method_1,	/* WOOL_get_C_value 6 */
   wool_undefined_method_1,	/* WOOL_open 7 */
   wool_undefined_method_1,	/* WOOL_close 8 */
   wool_undefined_method_2,	/* WOOL_process_event 9 */
   wool_undefined_method_1,	/* WOOL_copy 10 */
   wool_undefined_method_2,	/* WOOL_get_dimensions 11 */
   wool_undefined_method_2,	/* WOOL_draw 12 */
   wool_undefined_method_2,	/* WOOL_equal 13 */
   wool_undefined_method_2,
   wool_undefined_method_2,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1
}
#endif /* DO_INIT */
               ;
#endif /* INCLUDE_WL_BAR_H */
