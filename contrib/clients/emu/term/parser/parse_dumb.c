#ifndef lint
static char *rcsid = "$Header: /usr/src/local/emu/term/parser/RCS/parse_dumb.c,v 1.1 90/11/20 17:46:22 jkh Exp Locker: jkh $";
#endif

/*
 * This file is part of the PCS emu system.
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
 * This file implements some simple minded hard parsers.
 *
 * Author: Jordan K. Hubbard
 * Date: October 7th, 1990.
 * Description: Low intellect hard parsers for emulating dumb terminals.
 * all hard-coded parsers.
 *
 * Revision History:
 *
 * $Log:	parse_dumb.c,v $
 * Revision 1.1  90/11/20  17:46:22  jkh
 * Initial revision
 * 
 *
 */

#include "TermP.h"

/*
 * Do a simple "tty" style parser. Right now it assumes that it's supposed
 * to handle everything and just inserts what it doesn't understand. If you
 * wanted it use it as a base layer for a more sophisticated emulation,
 * however, you could simply make it return when it found something it didn't
 * know directly about.
 */
void parseDumb(w, client_data, call_data)
TermWidget w;
caddr_t client_data, call_data;
{
     register int i, nread = w->term.chars_to_parse;
     register String buf = (String)call_data;

     for (i = 0; i < nread; i++) {
	  int j;

	  /* First see how much we can insert in one go */
	  for (j = i; j < nread && !iscntrl(buf[j]); j++)
	       w->term.chars_to_parse--;
	  if (i - j) {
	       XpTermInsertText(w, buf + i, i - j);
	       i = j;
	       continue;
	  }
	  else if (iscntrl(buf[i])) {
	       --w->term.chars_to_parse;
	       /* Check for known control characters next */
	       switch (buf[i]) {
	       case '\r':	/* Carriage return */
		    XpTermSetRegister(w, 'x', CB_INT_TYPE, 0);
		    XpTermDispatchRequest(w, OP_MOVE_ABS_COLUMN);
		    break;

	       case '\n':	/* Newline */
		    XpTermSetRegister(w, 'y', CB_INT_TYPE, 1);
		    XpTermDispatchRequest(w, OP_MOVE_REL_ROW_SCROLLED);
		    break;

	       case '\b':	/* Backspace */
		    XpTermSetRegister(w, 'x', CB_INT_TYPE, -1);
		    XpTermDispatchRequest(w, OP_MOVE_REL_COLUMN);
		    break;

	       case '\007':	/* Bell */
		    XpTermDispatchRequest(w, OP_RING_BELL);
		    break;
	       default:
		    /* Don't know what to do with it, insert it I guess */
		    XpTermInsertText(w, buf + i, 1);
	       }
	  }
     }
}
