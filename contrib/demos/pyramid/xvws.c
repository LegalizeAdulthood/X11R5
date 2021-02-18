/* $XConsortium: xvws.c,v 5.1 91/02/16 09:33:02 rws Exp $ */

/***********************************************************
Copyright (c) 1989-1991 by Sun Microsystems, Inc. and the X Consortium.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include <xview/xview.h>
#include <xview/canvas.h>
#include <xview/window.h>
#include <X11/Xlib.h>

#define ATTR_H_INCLUDED
#include <phigs/phigs.h>

#define MAX_NUM_WS 15

#ifndef CAT
#undef IDENT
#define IDENT(x) x
#define CAT(a,b) IDENT(a)b
#endif

/** Holds the wks id's of the first opened wks, 2nd, 3rd, ..., **/
Pint	wks_table[MAX_NUM_WS];

/** Template for pexNotifyProcN().  This is the function that get's
 ** interposed for the N'th opened workstation.  It does a PHIGS
 ** REDRAW ALL STRUCTURES if this is the first time that the workstation
 ** has appeared on the screen.  **/
 
#define CREATE_NOTIFY_PROC(num) \
    static void CAT(pexNotifyProc,num) (frame, event, arg, when)	\
    Frame		frame;						\
    Event	       *event;						\
    Notify_arg		arg;						\
    Notify_event_type	when;						\
    {									\
	static int firstTime = 1;					\
									\
	if ( (event_action(event) == ACTION_OPEN) && firstTime) {	\
	    firstTime = 0;						\
	    predraw_all_structs(wks_table[num], PFLAG_ALWAYS);			\
	}								\
									\
	notify_next_event_func(frame, event, arg, when);		\
    }

CREATE_NOTIFY_PROC(0)
CREATE_NOTIFY_PROC(1)
CREATE_NOTIFY_PROC(2)
CREATE_NOTIFY_PROC(3)
CREATE_NOTIFY_PROC(4)
CREATE_NOTIFY_PROC(5)
CREATE_NOTIFY_PROC(6)
CREATE_NOTIFY_PROC(7)
CREATE_NOTIFY_PROC(8)
CREATE_NOTIFY_PROC(9)
CREATE_NOTIFY_PROC(10)
CREATE_NOTIFY_PROC(11)
CREATE_NOTIFY_PROC(12)
CREATE_NOTIFY_PROC(13)
CREATE_NOTIFY_PROC(14)

void (*notifyProcedureVector[MAX_NUM_WS]) = {
    pexNotifyProc0,
    pexNotifyProc1,
    pexNotifyProc2,
    pexNotifyProc3,
    pexNotifyProc4,
    pexNotifyProc5,
    pexNotifyProc6,
    pexNotifyProc7,
    pexNotifyProc8,
    pexNotifyProc9,
    pexNotifyProc10,
    pexNotifyProc11,
    pexNotifyProc12,
    pexNotifyProc13,
    pexNotifyProc14
};


/** Substitute for popen_ws().  This opens an XView PEX workstation.  Note
 ** that the frame argument is currently required, and should be the
 ** frame that the workstation canvas was created in.  This is only a 
 ** temporary measure until WIN_MAP_NOTIFY events start working in
 ** XView (predicted XView 1.1 - FCS June, 1990).			**/

pex_xv_popen_ws(ws_num, canvas, wstype, frame)
Pint	 ws_num;
Canvas	 canvas;
Pint	 wstype;
Frame	 frame;
{
    Pconnid_x_drawable	conn;
    static int wksIncrem = 0;
    
    conn.display = (Display *)XV_DISPLAY_FROM_WINDOW(canvas);
    conn.drawable_id = (Drawable)xv_get(canvas_paint_window(canvas), XV_XID);
    popen_ws(ws_num, (Pconnid)(&conn), wstype);
    
    xv_set(frame, WIN_CONSUME_EVENTS,
			ACTION_OPEN, 
			0,
		  0);

    notify_interpose_event_func(frame, 
	notifyProcedureVector[wksIncrem], NOTIFY_SAFE);

    wks_table[wksIncrem] = ws_num;
    wksIncrem++;
}

