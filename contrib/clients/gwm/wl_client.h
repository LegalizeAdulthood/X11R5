/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/***********************\
* 		        *
*  WOOL_OBJECT  Client  *
*  DEFINITIONS	        *
* 		        *
\***********************/

#ifndef INCLUDE_WL_CLIENT_H
#define INCLUDE_WL_CLIENT_H
#ifndef INCLUDE_WL_BAR_H
#include "wl_bar.h"
#endif
#ifndef INCLUDE_WL_LIST_H
#include "wl_list.h"
#endif

/* type */

/*
 * A client is the description of a window
 */

typedef struct _WOOL_Client {
    WOOL_HEADER;
    int             borderwidth;
    long            borderpixel;
    long            background;
    int		    inner_borderwidth;
    WOOL_OBJECT     bordertile;
    WOOL_OBJECT     tile;
    WOOL_OBJECT     fsm;
    WOOL_OBJECT     menu;
    WOOL_OBJECT	    cursor;
    WOOL_OBJECT	    opening;
    WOOL_OBJECT	    closing;
    WOOL_OBJECT	    property;
    WOOL_List	    grabs;
    WOOL_Bar	    titlebar;
    WOOL_Bar	    leftbar;
    WOOL_Bar	    rightbar;
    WOOL_Bar	    basebar;
    WOOL_Plug	    icon_plug; 
}              *WOOL_Client;

/* exported functions */

EXT WOOL_Client wool_client_make();
EXT WOOL_OBJECT WLClient_print();
EXT WOOL_OBJECT WLClient_free();
EXT WOOL_OBJECT WLClient_set_property();
EXT WOOL_OBJECT WLClient_get_property();

EXT WOOL_Client TargetWoolDesc;

/* methods */

EXT WOOL_METHOD WLClient[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLClient_print,		/* WOOL_print 2 */
   WLClient_free,		/* WOOL_free 3 */
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
#endif /* INCLUDE_WL_CLIENT_H */
