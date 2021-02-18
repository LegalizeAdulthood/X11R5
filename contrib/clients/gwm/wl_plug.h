/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*********************\
* 		      *
*  WOOL_OBJECT  Plug  *
*  DEFINITIONS	      *
* 		      *
\*********************/

#ifndef INCLUDE_WL_PLUG_H
#define INCLUDE_WL_PLUG_H

/* type */

/*
 * A plug is an icon with a FSM associated
 */

typedef struct _WOOL_Plug {
    WOOL_HEADER;
    int             width;
    int             heigth;
    int             borderwidth;
    long            borderpixel;
    long            background;
    WOOL_OBJECT	    property;
    WOOL_OBJECT	    bordertile;
    WOOL_OBJECT     graphic;
    WOOL_OBJECT     fsm;
    WOOL_OBJECT     menu;
    WOOL_OBJECT     cursor;
}              *WOOL_Plug;

/* exported functions */

EXT WOOL_Plug wool_plug_make();
EXT WOOL_Plug WLPlug_make();
EXT WOOL_OBJECT WLPlug_print();
EXT WOOL_OBJECT WLPlug_free();

/* methods */

EXT WOOL_METHOD WLPlug[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLPlug_print,		/* WOOL_print 2 */
   WLPlug_free,			/* WOOL_free 3 */
   wool_undefined_method_2,		/* WOOL_execute 4 */
   wool_undefined_method_2,		/* WOOL_set 5 */
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
#endif /* INCLUDE_WL_PLUG_H */
