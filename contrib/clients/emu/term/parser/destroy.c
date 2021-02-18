#ifndef lint
static char *rcsid = "$Header: /usr3/Src/emu/term/parser/RCS/destroy.c,v 2.8 91/09/30 18:08:17 me Exp $";
#endif


/*
 * This file is part of the PCS xterm+ program.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * Routines for viciously destroying data without warning.
 *
 * Author: Terry Jones and Jordan K. Hubbard
 * Date: March 20th, 1990.
 * Description: Routines for free'ing memory. 
 *
 * Revision History:
 *
 * $Log:	destroy.c,v $
 * Revision 2.8  91/09/30  18:08:17  me
 * got rid of a bunch of compiler warnings in R5
 * 
 * Revision 2.7  91/07/26  17:06:00  jkh
 * Made sure debugging was off by default.
 * 
 * Revision 2.6  90/11/20  17:46:07  jkh
 * Alpha.
 * 
 * Revision 2.5  90/10/12  12:49:16  jkh
 * Changed #includes for Term widget.
 * 
 * Revision 2.4  90/08/24  17:41:49  jkh
 * Changed destroy_possible to reflect change in IOP list handling.
 * 
 * Revision 2.3  90/07/26  02:11:10  jkh
 * Added new copyright.
 * 
 * Revision 2.2  90/06/26  19:09:40  jkh
 * Checkin for canvas testing.
 * 
 * Revision 2.1  90/05/06  09:04:22  jkh
 * Alles klar on a Sunday morning. Added stuff to disambiguate
 * %d vs 24 etc etc. Yay Jordan, Yay terry!
 * 
 * Revision 2.0  90/04/25  13:39:32  terry
 * Stable for first real test with canvas.
 * 
 * Revision 1.4  90/04/24  16:53:52  jkh
 * Clean-up to make things work with the new stuff. Fun fun fun.
 * 
 * Revision 1.3  90/04/18  17:24:22  terry
 * check in so jordan can take it home.
 * 
 * Revision 1.2  90/04/18  00:01:45  terry
 * Changes for reverse parsing go in.
 * 
 * Revision 1.1  90/04/16  22:47:08  terry
 * Initial revision
 * 
 *
 */

#include "TermP.h"

/*
 * destroy_transition(state, label)
 *
 * Remove the transition 'label' from this state, together with
 * everything it points to (i.e. the resulting state and everything
 * that it points to etc.). We know that the state has a transition
 * list and so there's no need to check that state->transition_list
 * is non-null.
 */
Export void
destroy_transition(state, label)
State *state;
int label;
{
    Transition *to_free = NULL;

#ifdef TRIE_ARG_CHECKING
    if (!state)
	fatal("Destroy transition called with null pointer.");
#endif /* TRIE_ARG_CHECKING */
    
    if (state->transition_list->label == label) {
	/* It's the first transition for this state. */
	destroy_state(state->transition_list->state);
	destroy_iops(state->transition_list->iop.head);
	to_free = state->transition_list;
	state->transition_list = state->transition_list->next;
    }
    else {
	Transition *t = state->transition_list;

	while (t->next) {
	    if (t->next->label == label) {
		destroy_state(t->next->state);
		destroy_iops(t->next->iop.head);
		to_free = t->next;
		t->next = t->next->next;
		break;
	    }
	    t = t->next;
	}
    }
    
    if (!to_free)
	fatal("Transition '%c' not found in destroy_transition.", label);

    XtFree((char *)to_free);
    
#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
    trans_count--;
    debug("destroy: trans_count now %d", trans_count);
#endif
    return;
}

/*
 * destroy_state(state)
 *
 * Remove the state from the trie, together with everything it points at.
 * This is recursive.
 *
 * This should not be used to destroy the zero_state unless
 * you know you are doing that and then set zero_state to NULL
 * afterwards. (That happens in init_parse()). When we call ourselves
 * we cannot be calling ourselves on the zero_state because we are moving
 * deeper into the trie with each call.
 *
 * destroy_transition() calls us too, but only with a state that is pointed
 * to by a transition - hence not with the zero state either.
 *
 */
Export void
destroy_state(state)
State *state;
{
    Transition *t;

    /* This used to be illegal, but isn't now with EPSILON transitions */
    if (!state)
	 return;

    /* Free the transition list. */
    t = state->transition_list;

    while (t) {
	Transition *next = t->next;
	destroy_transition(state, t->label);
	t = next;
    }
    
    destroy_requests(state->request_list);
    XtFree((char *)state);
#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
    state_count--;
    debug("destroy: state_count now %d", state_count);
#endif
}

Export void
destroy_requests(r)
Request *r;
{
    /* Free the request list. */

    while (r) {
	Request *next = r->next;
	XtFree((char *)r);
#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
	req_count--;
	debug("destroy: req_count now %d", req_count);
#endif
	r = next;
    }
}

Export void
destroy_iops(s)
register Iop *s;
{
     /* Free the iop list. */
     while (s) {
	  Iop *next = s->next;

	  XtFree((char *)s);
#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
	  iop_count--;
	  debug("destroy: iop_count now %d", iop_count);
#endif
	  s = next;
     }
}

Export void
destroy_possible(s)
Possible_state *s;
{
     register int i;

     /* Free any IOPs that were created temporarily */
     for (i = 0; i < s->niopl; i++)
	  if (bit_test(s->mask, i))
	       destroy_iops(s->iopl[i]);

     /* Free the possible state */
     XtFree((char *)s);
#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
     possible_state_count--;
     debug("destroy: possible_state_count now %d", possible_state_count);
#endif
}

Export void
destroy_rparse_list(r)
Rparse *r;
{
     while (r) {
	  Rparse *next = r->next;

#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
	  rparse_count--;
	  debug("destroy: rparse_count now %d", rparse_count);
#endif
	  destroy_iops(r->ops.head);
	  XtFree((char *)r);
	  r = next;
     }
}
