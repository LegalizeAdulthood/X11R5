#ifndef lint
static char *rcsid = "$Header: /usr/src/local/emu/term/parser/RCS/new.c,v 2.5 90/11/20 17:46:20 jkh Exp Locker: jkh $";
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
 * Allocation routines for trie parser.
 *
 * Author: Terry Jones and Jordan K. Hubbard
 * Date: June 1st, 1990.
 * Description: Safely allocate data for various trie structures, possibly
 *		keeping tabs on what's allocated.
 *
 * Revision History:
 *
 * $Log:	new.c,v $
 * Revision 2.5  90/11/20  17:46:20  jkh
 * Alpha.
 * 
 * Revision 2.4  90/10/12  12:48:41  jkh
 * Changed #includes for Term widget.
 * 
 * Revision 2.3  90/07/26  02:11:43  jkh
 * Added new copyright notice.
 * 
 * Revision 2.2  90/06/26  19:10:01  jkh
 * Checkin for canvas testing.
 * 
 * Revision 2.1  90/05/06  09:04:32  jkh
 * Alles klar on a Sunday morning. Added stuff to disambiguate
 * %d vs 24 etc etc. Yay Jordan, Yay terry!
 * 
 * Revision 2.0  90/04/25  13:40:08  terry
 * Changed new_rparse to actually return an Rparse *...
 * 
 * Revision 1.4  90/04/24  20:15:05  terry
 * silly.
 * 
 * Revision 1.3  90/04/24  16:54:22  jkh
 * Clean-up to make things work with the new stuff. Fun fun fun.
 * 
 * Revision 1.2  90/04/18  17:24:24  terry
 * check in so jordan can take it home.
 * 
 * Revision 1.1  90/04/16  22:47:01  terry
 * Initial revision
 * 
 */

#include "TermP.h"

#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
Export int trans_count = 0;
Export int state_count = 0;
Export int iop_count  = 0;
Export int req_count  = 0;
Export int rparse_count = 0;
Export int possible_state_count = 0;
#endif

/*
 * new_transition()
 *
 * Return a pointer to a new Transition struct.
 */
Export Inline Transition *
new_transition()
{
    Transition *new = Fresh(Transition);

#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
    new->id = trans_count++;
    debug("new: transition_count now %d", trans_count);
#endif
    return new;
}

/*
 * new_state()
 *
 * Return a pointer to a new State struct.
 */
Export Inline State *
new_state()
{
    State *new = Fresh(State);

#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
    new->id = state_count++;
    debug("new: state_count now %d", state_count);
#endif
    return new;
}

/*
 * new_iop()
 *
 * Return a pointer to a new Iop struct. Since we initialize all fields
 * anyway, we don't need to bzero() it.
 *
 */
Export Inline Iop *
new_iop(t, v)
int t, v;
{
    Iop *new = XtNew(Iop);

#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
    iop_count++;
    debug("new: iop_count now %d", iop_count);
#endif
    new->type = t;
    new->value = v;
    new->next = NULL;
#ifdef IOP_DEBUG
    new->break = 0;
#endif
    return new;
}

/*
 * new_request()
 *
 * Return a pointer to a new Request struct.
 */
Export Inline Request *
new_request()
{
    Request *new = Fresh(Request);

#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
    req_count++;
    debug("new: request_count now %d", req_count);
#endif
    return new;
}

/*
 * new_rparse()
 *
 * Return a pointer to a new Rparse struct.
 */
Export Inline Rparse *
new_rparse()
{
    Rparse *new = Fresh(Rparse);

#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
    rparse_count++;
    debug("new: rparse_count now %d", rparse_count);
#endif
    return new;
}

/*
 * new_possible_state()
 *
 * Return a pointer to a new Possible_state struct.
 */
Export Inline Possible_state *
new_possible_state()
{
    Possible_state *new = Fresh(Possible_state);

#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
    possible_state_count++;
    debug("new: possible_state_count now %d", possible_state_count);
#endif
    return new;
}
