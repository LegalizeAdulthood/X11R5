/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*********************\
* 		      *
*  WOOL_OBJECT  Menu  *
*  DEFINITIONS	      *
* 		      *
\*********************/

#ifndef INCLUDE_WL_MENU_H
#define INCLUDE_WL_MENU_H

/* type */

/*
 * A menu is a window around bars
 */

typedef struct _WOOL_Menu {
    WOOL_HEADER;
    int		    direction;
    int		    bar_separator;
    Menu	    wob_menu;
    int             width;
    int             heigth;
    int             borderwidth;
    long            borderpixel;
    long            background;
    WOOL_OBJECT	    property;
    WOOL_OBJECT	    bordertile;
    WOOL_OBJECT     fsm;
    WOOL_OBJECT     menu;
    WOOL_OBJECT     cursor;
    int		    bars_size;
    WOOL_Bar	    bars[1];
}              *WOOL_Menu;

/* exported functions */

EXT WOOL_Menu wool_menu_make();
EXT WOOL_Menu WLMenu_make();
EXT WOOL_OBJECT WLMenu_print();
EXT WOOL_OBJECT WLMenu_free();
EXT WOOL_OBJECT WLMenu_get_dimensions();

#define must_be_menu(obj, n) must_be_or_nil(WLMenu, obj, n)

/* methods */

EXT WOOL_METHOD WLMenu[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLMenu_print,		/* WOOL_print 2 */
   WLMenu_free,			/* WOOL_free 3 */
   wool_undefined_method_2,		/* WOOL_execute 4 */
   wool_undefined_method_2,		/* WOOL_set 5 */
   wool_undefined_method_1,	/* WOOL_get_C_value 6 */
   wool_undefined_method_1,	/* WOOL_open 7 */
   wool_undefined_method_1,	/* WOOL_close 8 */
   wool_undefined_method_2,	/* WOOL_process_event 9 */
   wool_undefined_method_1,	/* WOOL_copy 10 */
   WLMenu_get_dimensions,	/* WOOL_get_dimensions 11 */
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
#endif /* INCLUDE_WL_MENU_H */
