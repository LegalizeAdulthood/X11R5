/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/************************\
* 			 *
*  WOOL_OBJECT: WLEvent	 *
*  DEFINITIONS		 *
* 			 *
\************************/

#ifndef INCLUDE_WL_EVENT_H
#define INCLUDE_WL_EVENT_H

/* type */

typedef struct _WOOL_Event {
    WOOL_HEADER;
    int		(*match)();	/* func to match Xevents. Kind of a subtype */
    int		(*release)();   /* func to wait for release (or NULL) */
    int	         x_type; 	/* the X type of the event */
    unsigned int mask;		/* the XSelectInput mask */
    unsigned int user_mask;	/* the user-events mask */
    unsigned int state;		/* state (modifier) */
    unsigned int detail;	/* detail (keysym, button) */
    unsigned int flags;		/* misc flags */
}              *WOOL_Event;

/* defs */

#define UNDEFINED_KEYCODE -2		/* -1 is any key */
 					/* flags field: */
#define FREEZE_ON_GRABS (1 << 0)	/* replayable grab ? */

#define must_be_event(obj, n) must_be(WLEvent, obj, n)

/* exported functions */

EXT void wool_all_events_make();
EXT WOOL_Event wool_event_make();
EXT WOOL_Event WLEvent_make();
EXT WOOL_OBJECT WLEvent_print();
EXT int WLEvent_buttonpress();
EXT int WLEvent_keypress();
EXT WOOL_OBJECT set_grab();
EXT WOOL_OBJECT remove_grab();
EXT WOOL_OBJECT wool_set_grab();
 
/* fields */

EXT int GWM_invalid_context INIT(1);

/* methods */

EXT WOOL_METHOD WLEvent[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLEvent_print,		/* WOOL_print 2 */
   WLNumber_free,		/* WOOL_free 3 */
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

#endif /* INCLUDE_WL_EVENT_H */
