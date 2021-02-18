/*
 * This example shows a classic way to set variables in a client routine.
 * When a button is hit, a callback function is called to set the client
 * variable, using the value in the client_data argument.
 */

static void set_x () ;

#include "header.h"

XcuWlmAddCallback (
		  wlm_id,	/* XcuWlm widget id	*/
		  XcuWLM_EVENTS,	/* Device argument	*/
		  "Command",	/* Widget Class		*/
		  "1",		/* Widget Name		*/
		  "callback",	/* Callback List Name	*/
		  set_x,	/* Function to call	*/
		  1		/* client_data		*/
		 ) ;

XcuWlmAddCallback (
		  wlm_id,	/* XcuWlm widget id	*/
		  XcuWLM_EVENTS,	/* Device argument	*/
		  "Command",	/* Widget Class		*/
		  "2",		/* Widget Name		*/
		  "callback",	/* Callback List Name	*/
		  set_x,	/* Function to call	*/
		  2		/* client_data		*/
		 ) ;

#include "trailer.h"

static void
set_x (w, client_data, call_data)
    Widget w ;
    XPointer client_data ;
    XPointer call_data ;
{
x = (int) client_data ;
return ;
}

/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */

