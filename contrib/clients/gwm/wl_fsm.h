/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/********************\
* 		     *
*  WOOL_OBJECT  Fsm  *
*  DEFINITIONS	     *
* 		     *
\********************/

#ifndef INCLUDE_WL_FSM_H
#define INCLUDE_WL_FSM_H

#ifndef INCLUDE_WL_EVENT_H
#include "wl_event.h"
#endif

#ifndef INCLUDE_WL_LIST_H
#include "wl_list.h"
#endif

#ifndef INCLUDE_WL_ATOM_H
#include "wl_atom.h"
#endif

/* type */

typedef struct _WOOL_StateArc {
    WOOL_HEADER;
    WOOL_Event		event;		/* on event, */
    WOOL_List		action;		/* do action */
    WOOL_Atom 		state;		/* and goto state */
}		*WOOL_StateArc;

typedef struct _WOOL_FsmState {
    WOOL_HEADER;
    unsigned int	mask;		/* for all these events */
    unsigned int	user_mask;	/* do we expect user events? */
    struct _WOOL_FsmState *copy_of;	/* is an copy of what state? */
    int			arcs_size;	/* try each of these arcs */
    WOOL_StateArc	arcs[1];
}		*WOOL_FsmState;

typedef struct _WOOL_Fsm {
    WOOL_HEADER;
    unsigned int	mask;		/* what events are awaited */
    unsigned int	user_mask;	/* do we expect user events? */
    int			states_size;	/* list of the states */
    WOOL_FsmState	states[1];
}              *WOOL_Fsm;

/* exported functions */

EXT WOOL_Fsm wool_fsm_make();
EXT WOOL_Fsm WLFsm_make();
EXT WOOL_FsmState wool_fsm_state_make();
EXT WOOL_StateArc wool_state_arc_make();
EXT WOOL_OBJECT WLFsm_free();
EXT WOOL_OBJECT WLFsmState_free();
EXT WOOL_OBJECT WLStateArc_free();
EXT WOOL_OBJECT WLFsm_open();
EXT WOOL_OBJECT WLFsm_print();
EXT WOOL_OBJECT WLFsm_action();
EXT void	fix_fsm();

/* exported objects */

EXT WOOL_Fsm NIL_FSM;		/* the FSM doing nothing! */

/* methods */

EXT WOOL_METHOD WLFsm[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,		/* WOOL_eval 1 */
   WLFsm_print,			/* WOOL_print 2 */
   WLFsm_free,			/* WOOL_free 3 */
   wool_undefined_method_2,	/* WOOL_execute 4 */
   wool_undefined_method_2,	/* WOOL_set 5 */
   wool_undefined_method_1,	/* WOOL_get_C_value 6 */
   WLFsm_open,			/* WOOL_open 7 */
   wool_undefined_method_1,	/* WOOL_close 8 */
   WLFsm_action,		/* WOOL_process_event 9 */
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

EXT WOOL_METHOD WLFsmState[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,
   WLFsm_print,
   WLFsmState_free,
   wool_undefined_method_2,
   wool_undefined_method_2,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_2,
   wool_undefined_method_1,
   wool_undefined_method_2,
   wool_undefined_method_2,
   wool_undefined_method_2,
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

EXT WOOL_METHOD WLStateArc[]
#ifdef DO_INIT
= {
   (WOOL_METHOD) 0,		/* METHODS_ARRAY */
   WLNumber_eval,
   WLFsm_print,
   WLStateArc_free,
   wool_undefined_method_2,
   wool_undefined_method_2,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_1,
   wool_undefined_method_2,
   wool_undefined_method_1,
   wool_undefined_method_2,
   wool_undefined_method_2,
   wool_undefined_method_2,
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
#endif /* INCLUDE_WL_FSM_H */
