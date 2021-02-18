/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*********************\
* 		      *
*  WOOL_OBJECT: Fsm  *
*  BODY		      *
* 		      *
\*********************/

#include "EXTERN.h"
#include <stdio.h>
#include <assert.h>
#include "wool.h"
#include "wl_number.h"
#include "wl_atom.h"
#include "wl_list.h"
#include "wl_func.h"
#include "gwm.h"
#include "wl_event.h"
#include "INTERN.h"
#include "wl_fsm.h"

WOOL_FsmState WLFsmState_copy();
WOOL_FsmState WLState_fp();
WOOL_StateArc WLArc_fp();

#define SAME_STATE  ((WOOL_Atom) -1)

/*
 * Constructor: wool_fsm_make callable from wool
 */

WOOL_Fsm
wool_fsm_make(argc, argv)
int    argc;
WOOL_FsmState *argv;
{
    WOOL_Fsm        fsm;
    int             i;

    for (i = 0; i < argc; i++) /* check args are all states */
	must_be(WLFsmState, argv[i], i);
    fsm = (WOOL_Fsm) Malloc(sizeof(struct _WOOL_Fsm) + Max(0,argc - 1) *
			    sizeof(WOOL_OBJECT));
    zrt_put(fsm);
    fsm -> type = WLFsm;
    fsm -> mask = 0;
    fsm -> user_mask = 0;
    fsm -> states_size = argc;
    for (i = 0; i < argc; i++) {
	fsm -> mask |= argv[i] -> mask;
	fsm -> user_mask |= argv[i] -> user_mask;
	increase_reference(fsm -> states[i] = WLFsmState_copy(argv[i]));
    }
    WLFsm_fix(fsm, argc, argv);
    return fsm;
}

/*
 * fixes the states to go to in the fsm
 */

WLFsm_fix(fsm, argc, argv)
WOOL_Fsm	fsm;
int		argc;
WOOL_FsmState   argv[];
{
    int    nstate, narc, i;

    for (nstate = 0; nstate < fsm -> states_size; nstate++)
	for (narc = 0; narc < fsm -> states[nstate] -> arcs_size; narc++)
	    if (fsm -> states[nstate] -> arcs[narc] -> state != SAME_STATE) {
		for (i = 0; i < argc; i++)
		    if (fsm -> states[nstate] -> arcs[narc] -> state -> c_val
			== (WOOL_OBJECT) ((argv[i]) -> copy_of)) {
			fsm -> states[nstate] -> arcs[narc] -> state =
			    (WOOL_Atom) i;
			break;
		    }
		if (i == argc)
		    wool_error("State %s not found!", fsm -> states[nstate]
			       -> arcs[narc] -> state -> p_name);
	    }
}

/*
 * WLFsm_print prints any object of this package.
 */

WOOL_OBJECT 
WLFsm_print(obj)
WOOL_Fsm     obj;
{
    wool_printf("{%s", WOOL_TYPE_P_NAME(obj -> type));
    wool_printf(" 0x%x}", obj);
    return (WOOL_OBJECT) obj;
}

#ifdef DEBUG
WOOL_Fsm
WLFsm_fp(obj)
    WOOL_Fsm     obj;
{
    int nstate;

    wool_printf("{%s", WOOL_TYPE_P_NAME(obj -> type));
    wool_printf(" 0x%x:\n", obj);
    for (nstate = 0; nstate < obj -> states_size; nstate++) {
	wool_printf("State %d:\n", nstate);
	WLState_fp(obj->states[nstate]);
    }
    wool_printf("}\n");
    return obj;
}

WOOL_FsmState
WLState_fp(obj)
WOOL_FsmState obj;
{
    int narc;
    for (narc = 0; narc < obj-> arcs_size; narc++) {
	wool_puts(" * ");
	WLArc_fp(obj->arcs[narc]);
	wool_puts("\n");
    }
    return obj;
}

WOOL_StateArc
WLArc_fp(obj)
WOOL_StateArc obj;
{
    wool_print(obj->event);
    wool_puts(" ");
    wool_print(obj->action);
    return obj;
}

#endif /*DEBUG*/


/* the main routine:
 * check the incoming event against all transitions of the current
 * state of the fsm, and triggers the action if necessary
 */

WOOL_OBJECT
WLFsm_action(fsm, wob, evt)
WOOL_Fsm	fsm;
Wob             wob;
Event           evt;
{
    int             i;
    WOOL_FsmState state;
    WOOL_StateArc arc;
    WOOL_List       l;
    int             local_zrt_size = zrt_size;

    TargetWob = wob;
    GWM_invalid_context = 1;
    state = fsm -> states[wob -> curstate];
    for (i = 0; i < state -> arcs_size; i++) {
	arc = state -> arcs[i];
	if (evt -> type == arc -> event -> x_type &&
	    ((*(arc -> event -> match)) (arc -> event, evt))) {

	    TraceDo('f',
		    GWM_prettyprint_fsm_event(wob, evt, "MATCH",
					      wob -> curstate,
					      arc -> state));
	    if (l = arc -> action) {
		save_wool_error_resume_point();
		if (GWM_invalid_context)
		    SetTarget(wob);
		TriggeringEvent = evt;
		if (set_wool_error_resume_point()) {
		} else {
		    WOOL_send(WOOL_execute, (*(l -> list)), (*(l -> list), l));
		    zrt_gc(local_zrt_size);
		}
		restore_wool_error_resume_point();   
	    }
	    
	    if (arc -> event -> release)
		(*(arc -> event -> release)) (arc -> event, evt);
	    if (arc -> state != SAME_STATE)
	    	wob -> curstate = (int) arc -> state;

	    return NULL;
	}
    }
    TraceDo('f',
	    GWM_prettyprint_fsm_event(wob, evt, "REJECT",
				      wob -> curstate,
				      arc -> state));
    return NULL;
}
    
/*
 * Routine used to construct a state 
 * USAGE:
 * 	(state-make arc1 arc2 ... arcn)
 */

WOOL_FsmState
wool_fsm_state_make(argc, argv)
int 		argc;
WOOL_StateArc	argv[];
{
    WOOL_FsmState   state;
    int             i, j, narc = argc;
    WOOL_FsmState   inc_state;

    for (i = 0; i < argc; i++)
	if (argv[i] -> type == WLFsmState)
	    narc += ((WOOL_FsmState) argv[i]) -> arcs_size - 1;
	else if (argv[i] == (WOOL_StateArc) NIL)
	    narc--;
    state = (WOOL_FsmState)
	Malloc(sizeof(struct _WOOL_FsmState) +
	       Max(0, narc - 1) * sizeof(WOOL_OBJECT));
    state -> type = WLFsmState;
    state -> mask = state -> user_mask = 0;
    zrt_put(state);
    state -> arcs_size = narc;
    state -> copy_of = state;
    narc = 0;
    for (i = 0; i < argc; i++) {
	if (argv[i] -> type == WLStateArc) {
	    increase_reference(state -> arcs[narc++] = argv[i]);
	    state -> mask |= argv[i] -> event -> mask;
	    state -> user_mask |= argv[i] -> event -> user_mask;
	} else if (argv[i] -> type == WLFsmState) {
	    inc_state = (WOOL_FsmState) argv[i];
	    for (j = 0; j < inc_state -> arcs_size; j++) {
		increase_reference(state -> arcs[narc++]
				   = inc_state -> arcs[j]);
		state -> mask |= inc_state -> arcs[j] -> event -> mask;
		state -> user_mask |=
		    inc_state -> arcs[j] -> event -> user_mask;
	    }
	} else if (argv[i] == (WOOL_StateArc) NIL) 
	    ;				/* skip () */
        else
	    bad_argument(argv[i], i, "fsm state");
    }
    return state;
}

    
/*
 * make an arc: (FSUBR)
 * syntax: (on event [action [state]])
 * action (list) and state (atom) MUST NOT BE QUOTED!
 */

WOOL_StateArc
wool_state_arc_make(argc, argv)
int 		argc;
WOOL_OBJECT	argv[];
{
    WOOL_StateArc   arc = (WOOL_StateArc)
    Malloc(sizeof(struct _WOOL_StateArc));

    arc -> type = WLStateArc;
    zrt_put(arc);
    if (argc == 0 || argc > 3)
	wool_error(BAD_NUMBER_OF_ARGS, argc);
    arc -> state = SAME_STATE;
    arc -> action = NULL;
    increase_reference(arc -> event = (WOOL_Event)
		       wool_type_or_evaluate(argv[0], WLEvent));
    if (arc -> event == (WOOL_Event) NIL)
	bad_argument(argv[0], 0, "event");
    switch (argc) {
    case 3:
	must_be_atom(argv[2], 2);
	arc -> state = (WOOL_Atom) argv[2];
    case 2:
	must_be_or_nil(WLList, argv[1], 1);
	if (argv[1] != (WOOL_OBJECT) NIL)
	    increase_reference(arc -> action = (WOOL_List) argv[1]);
    }
    return arc;
}

WOOL_OBJECT
WLFsm_free(obj)
WOOL_Fsm obj;
{
    decrease_reference_in_list(obj -> states_size, obj -> states);
    Free(obj);
    return NULL;
}

WOOL_OBJECT
WLFsmState_free(obj)
WOOL_FsmState obj;
{
    decrease_reference_in_list(obj -> arcs_size, obj -> arcs);
    Free(obj);
    return NULL;
}

WOOL_OBJECT
WLStateArc_free(obj)
WOOL_StateArc obj;
{
    decrease_reference(obj -> event);
    decrease_reference(obj -> action);
    Free(obj);
    return NULL;
}

WOOL_OBJECT
WLFsm_open(fsm)
WOOL_Fsm fsm;
{
    return 0;
}

WOOL_StateArc
WLStateArc_copy(oldarc)
WOOL_StateArc oldarc;
{
    WOOL_StateArc   arc =
    (WOOL_StateArc) Malloc(sizeof(struct _WOOL_StateArc));

    arc -> type = WLStateArc;
    zrt_put(arc);
    increase_reference(arc -> event = oldarc -> event);
    if(arc -> action = oldarc -> action)
	increase_reference(arc -> action);
    if((arc -> state = oldarc -> state) != (WOOL_Atom) ANY)
	increase_reference(arc -> state);
    return arc;
}

WOOL_FsmState
WLFsmState_copy(oldstate)
WOOL_FsmState oldstate;
{
    WOOL_FsmState   state = (WOOL_FsmState)
    Malloc(sizeof(struct _WOOL_FsmState) + Max(0,oldstate -> arcs_size - 1)
	   * sizeof(WOOL_OBJECT));
    int             i;

    state -> type = WLFsmState;
    state -> mask = oldstate -> mask;
    state -> user_mask = oldstate -> user_mask;
    state -> arcs_size = oldstate -> arcs_size;
    state -> copy_of = oldstate -> copy_of;
    for (i = 0; i < state -> arcs_size; i++)
	increase_reference(state -> arcs[i] =
			   WLStateArc_copy(oldstate -> arcs[i]));
    return state;
}

/*
 * NIL_FSM_make:
 * used to build the nil fsm on startup
 */

NIL_FSM_make()
{
    WOOL_FsmState   state = (WOOL_FsmState)
    Malloc(sizeof(struct _WOOL_FsmState));

    NIL_FSM = (WOOL_Fsm) Malloc(sizeof(struct _WOOL_Fsm));
    state -> type = WLFsmState;
    state -> mask = state -> user_mask = 0;
    zrt_put(state);
    increase_reference(state);
    state -> arcs_size = 0;
    zrt_put(NIL_FSM);
    NIL_FSM -> type = WLFsm;
    NIL_FSM -> mask = 0;
    NIL_FSM -> user_mask = 0;
    NIL_FSM -> states_size = 1;
    NIL_FSM -> states[0] = state;
    increase_reference(NIL_FSM);
}

void
fix_fsm(ptr)
WOOL_Fsm	*ptr;
{
    if (*ptr == (WOOL_Fsm) NIL)
	increase_reference(*ptr = (WOOL_Fsm) NIL_FSM);
}
