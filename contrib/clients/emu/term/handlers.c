#ifndef lint
static char *rcsid = "$Header: /usr3/Src/emu/term/RCS/handlers.c,v 1.9 91/09/30 21:40:16 jkh Exp Locker: jkh $";
#endif

/*
 * This file is part of the PCS emu program.
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
 * Interface forwarders.
 *
 * Author: Jordan K. Hubbard
 * Date: March 20th, 1990.
 * Description: All sections comprising the term widget are designed so
 *		that data is passed in and out via rigidly defined "contact"
 *		procedures. These routines in this file act as intermediaries
 *		to "glue" various modules together.
 *
 * Revision History:
 *
 * $Log:	handlers.c,v $
 * Revision 1.9  91/09/30  21:40:16  jkh
 * Checkin prior to DECWRL merge.
 * 
 * Revision 1.8  90/11/20  17:45:18  jkh
 * Alpha.
 * 
 * Revision 1.1  90/05/03  05:49:59  jkh
 * Initial revision
 * 
 *
 */

#include "TermP.h"

Local void
copy_comblock(from, to)
register ComBlockPtr from, to;
{
     if ((cb_opcode(to) = cb_opcode(from)) != OP_INSERT)
	  bcopy(cb_regs(from), cb_regs(to), sizeof(Register) * CB_NREGS);
     else
	  bcopy(cb_buffer(from), cb_buffer(to),
		(cb_nbytes(to) = cb_nbytes(from)));
}

/*
 * Dispatch an event to the canvas, attempting to do various intelligent
 * things to adjacent events along the way. These include bundling like
 * requests together into single requests, collapsing "delta" requests into
 * single ones and discarding requests who's effects have been negated by
 * later ones. This routine is registered as the trie parser's external data
 * contact.
 */
Export void
dispatch(w, cb)
TermWidget w;
ComBlockPtr cb;
{
     register ComBlockPtr old, new;
     Import void XpTermCanvasDispatch();
     Widget canvas = w->composite.children[0];

     old = w->term.save;
     new = cb;

     /*
      * If this event is to be dispatched IMMEDIATELY, flush saved
      * event (if there) and send this one directly out, not saving
      * it.
      */
     if (w->term.immediate) {
	  if (cb_opcode(old) != OP_NOP) {
	       /* No need to put the cursor back yet */
	       XpTermCanvasDispatch(canvas, old, False);
	       cb_opcode(old) = OP_NOP;
	  }
	  /*
	   * We don't know if another request will follow,
	   * so better put the cursor back
	   */
	  XpTermCanvasDispatch(canvas, new, True);
	  w->term.immediate = FALSE;
	  return;
     }

     if (cb_opcode(old) == OP_INSERT && cb_opcode(new) == OP_INSERT) {
	  /*
	   * If we can't combine, dispatch old first,
	   *  no need to put the cursor back here.
	    */
	  if (cb_nbytes(new) + cb_nbytes(old) >= BUF_SZ) {
	       XpTermCanvasDispatch(canvas, old, False);
	       cb_nbytes(old) = 0;
	  }
	  bcopy(cb_buffer(new), cb_buffer(old) + cb_nbytes(old),
		cb_nbytes(new));
	  cb_nbytes(old) += cb_nbytes(new);
     }
     /* If it's an outdated move, nuke it */
     else if (OP_IS_MOVE(cb_opcode(old)) && cb_opcode(new) == OP_MOVE_ABS)
	  copy_comblock(new, old);
     /* If it's something about to be obviated by a clear screen, punt */
     else if (cb_opcode(old) == OP_INSERT && cb_opcode(new) == OP_CLEAR_SCREEN)
	  copy_comblock(new, old);
     else {
	  /*
	   * Can't do anything cute, dump old (if existant) and reset,
	   * but don't draw the cursor yet
	   */
	  if (cb_opcode(old) != OP_NOP)
	       XpTermCanvasDispatch(canvas, old, False);
	  copy_comblock(new, old);
     }
     if (!w->term.chars_to_parse) {
	  /* Here we have to put the cursor back */
	  XpTermCanvasDispatch(canvas, old, True);
	  cb_opcode(old) = OP_NOP;
	  cb_nbytes(old) = 0;
     }
}
