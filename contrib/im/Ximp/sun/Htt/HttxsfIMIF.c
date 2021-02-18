/*
 *        %Z%%M% %I% %E%  
 * 
 * Derived from @(#)xsfIMIF.c		1.0 91/08/13 Fujitsu Limited */
/*
****************************************************************************

              Copyright 1991, by FUJITSU LIMITED
              Copyright 1991, by Sun Microsystems, Inc.	      

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of FUJITSU LIMITED and Sun
Microsystems, Inc. not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.  FUJITSU LIMITED and Sun Microsystems, Inc. makes no
representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.

FUJITSU LIMITED AND SUN MICROSYSTEMS, INC. DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL FUJITSU LIMITED AND SUN
MICROSYSTEMS, INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Auther: Riki	Kawaguchi	FUJITSU LIMITED
	Tomoki	Kuriki		FUJITSU LIMITED
	Hideki  Hiura           (hhiura@Sun.COM) Sun Microsystems, Inc.

****************************************************************************
*/
#ifndef lint
static char     sccsid[] = "%Z%%M% %I% %E% Sun Microsystems Inc.";
#endif

#include <locale.h>
#include <stdio.h>
#include <widec.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

#include <X11/keysymdef.h>

#include "Htt.h"
#include "XSunIMProt.h"
#include "XSunIMImpl.h"
#include "XSunExt.h"

#ifdef	XSF_IMLOGIC_NOP
#define	IMLogic()	Dummy_IMLogic()
#endif				/* XSF_IMLOGIC_NOP */

extern KeySym IMLogic();

/* IMLogic Open */
void
xsfIMLopen(ic)
    FXimp_Client   *ic;
{

    iml_inst       *rv = NULL;
    wchar_t         buff[512];
    int             status;
    int             rstatus = NULL;

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMOpen Called\n");
#endif				/* DEBUG */
    if ((status = (int) IMLogic(NULL, &rv,
				IML_OPEN, NULL, NULL)) < 0) {

	return;

    }
#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMGetStatus Called\n");
#endif				/* DEBUG */
    ic->id = status;
    xsfinterpret(ic, rv, buff, 512, &rstatus, 0);

    if ((status = (int) IMLogic(ic->id, &rv,
				IML_GET_STATUS, NULL, NULL)) < 0) {

	return;

    }
    xsfinterpret(ic, rv, buff, 512, &rstatus, 0);

    if ( ic->preedit_window ) {
	MapPreeditWindow() ;
	DisplayCursor() ;
    }

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMOpen Return\n");
#endif				/* DEBUG */
    return;

}

/* IMLogic Close */
void
xsfIMLclose(ic)
    FXimp_Client   *ic;
{

    iml_inst       *rv = NULL;

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMClose Called\n");
#endif				/* DEBUG */

    (void) IMLogic(ic->id, &rv, IML_CLOSE, NULL, NULL);

	/*
	 * Status message area Free
	 */
	if ( ic->status_message ) {
		free( ic->status_message ) ;
		ic->status_message = NULL ;
	}

	/*
	 * Preedit message area Free
	 */
	if ( ic->output_buff ) {
		free( ic->output_buff ) ;
		ic->current_colums = ic->last_colums = 0 ;
	}

	/*
	 * Preedit attrib area Free
	 */
	if ( ic->preedit_attr ) {
		free( ic->preedit_attr ) ;
		ic->preedit_attr = 0 ;
	}

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMClose Return\n");
#endif				/* DEBUG */
    return;

}

/* IMLogic KeyPress */
void
xsfKeyPress(ic, event)
    FXimp_Client   *ic;
    XEvent         *event;
{

    iml_inst       *rv = NULL;
    char            buff[512];
    KeySym          keysym;

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMKeyPress Called\n");
#endif				/* DEBUG */
    if ((int) (keysym = IMLogic(ic->id, &rv,
				IML_SEND_EVENT, event, NULL)) < 0) {

	return;

    }
    xsfinterpret(ic, rv, buff, 512, event, 0);

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMKeyPress Return\n");
#endif				/* DEBUG */
    return;

}

/* IMLogic SetFocus */
void
xsfIMLsetfocus(ic)
    FXimp_Client   *ic;
{

    int             rstatus = NULL;
    char            buff[512];
    int             ret, status, root_x, root_y, win_x, win_y;
    Window          root, child;
    unsigned int    keys_button;
    iml_inst       *rv = NULL;
    IMKeyState      keystate;

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMSetFocus Called\n");
#endif				/* DEBUG */
    (void) XQueryPointer(display, ic->focus_window, &root, &child,
			 &root_x, &root_y, &win_x, &win_y, &keys_button);

    if (keys_button & Mod3Mask) {

	keystate = MODE_SWITCH_ON;

    } else {

	keystate = MODE_SWITCH_OFF;

    }

    if ((status = (int) IMLogic(ic->id, &rv,
			   IML_SET_KEYBOARD_STATE, &keystate, NULL)) >= 0) {

	xsfinterpret(ic, rv, buff, 512, &rstatus, 0);

    }
#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMSetFocus Return\n");
#endif				/* DEBUG */
    return;

}

/* IMLogic UnSetFocus */
void
xsfIMLunsetfocus(ic)
    FXimp_Client   *ic;
{

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMUnSetFocus Called/Return\n");
#endif				/* DEBUG */
    /* No Operation */

}

/* IMLogic reset */
void
xsfIMLreset(ic)
    FXimp_Client   *ic;
{

    int             rstatus = NULL;
    iml_inst       *rv = NULL;
    char            buff[512];

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMReset Called\n");
#endif				/* DEBUG */
    (void) IMLogic(ic->id, &rv, IML_COMMIT, NULL, NULL);

    xsfinterpret(ic, rv, buff, 512, &rstatus, 0);

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMReset Return\n");
#endif				/* DEBUG */
    return;

}

/* IMLogic result */
void
xsfIMLlookstart(ic, event, prop)
    FXimp_Client   *ic;
    XEvent         *event;
    int            *prop;
{

    XIMLookupStartCallbackStruct *lsc;
    iml_inst       *rv = NULL;

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMLookupStart Called\n");
#endif				/* DEBUG */

    if (!ic->cell_link)
	return;
    if (!ic->cell_link->called_rv)
	return;

    lsc = (XIMLookupStartCallbackStruct *)
	& ic->cell_link->called_rv->operand;
    lsc->WhoIsMaster = (WhoIsMaster)prop[0];
    /*
     * lsc->CBPreference = (LayoutInfo *)malloc( sizeof( LayoutInfo ) ) ;
     */
    lsc->CBPreference->choice_per_window = prop[1];
    lsc->CBPreference->nrows = prop[2];
    lsc->CBPreference->ncolumns = prop[3];
    lsc->CBPreference->DrawUpDirection = (DrawUpDirection)prop[4];
    lsc->CBPreference->WhoOwnsLabel = (WhoOwnsLabel)prop[5];

#ifdef	DEBUG
    dbg_printf("	cell Address  : %8.8x\n", ic->cell_link);
    dbg_printf("	rrv Address  : %8.8x\n", ic->cell_link->called_rv);
    dbg_printf("	Operand Address  : %8.8x\n", lsc);
    dbg_printf("	CBP/WhoIsMaster  : %d\n", prop[0]);
    dbg_printf("	Pre/ChoicePerWin : %8.8x\n",
	       lsc->XIMPreference->choice_per_window);
    dbg_printf("	Pre/nrows        : %d\n", lsc->XIMPreference->nrows);
    dbg_printf("	Pre/ncolumns     : %d\n", lsc->XIMPreference->ncolumns);
    dbg_printf("	Pre/DrawUpDirect : %d\n",
	       lsc->XIMPreference->DrawUpDirection);
    dbg_printf("	Pre/WhoOwnsLabel : %d\n", lsc->XIMPreference->WhoOwnsLabel);
    dbg_printf("	CBP/ChoicePerWin : %d\n", prop[1]);
    dbg_printf("	CBP/nrows        : %d\n", prop[2]);
    dbg_printf("	CBP/ncolumns     : %d\n", prop[3]);
    dbg_printf("	CBP/DrawUpDirect : %d\n", prop[4]);
    dbg_printf("	CBP/WhoOwnsLabel : %d\n", prop[5]);
#endif				/* DEBUG */

    (void) IMLogic(ic->id, &rv,
		   IML_SEND_RESULTS, ic->cell_link->called_rv, NULL);

    xsfinterpret(ic, rv, NULL, NULL, event);

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMLookupStart Return\n");
#endif				/* DEBUG */
    return;

}

/* IMLogic Result2 */
void
xsfIMLlookprocess(ic, event, data)
    FXimp_Client   *ic;
    XEvent         *event;
    int             data;
{

    XIMLookupProcessCallbackStruct *lpc;
    iml_inst       *rv = NULL;

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMLookupProcess Called\n");
#endif				/* DEBUG */

    if (!ic->cell_link)
	return;
    if (!ic->cell_link->called_rv)
	return;

    lpc = (XIMLookupProcessCallbackStruct *)
	& ic->cell_link->called_rv->operand;

    lpc->index_of_choice_selected = data;

    (void) IMLogic(ic->id, &rv,
		   IML_SEND_RESULTS, ic->cell_link->called_rv, NULL);

    xsfinterpret(ic, rv, NULL, NULL, event);

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMLookupProcess Return\n");
#endif				/* DEBUG */
    return;

}

/* IMLogic Result3 */
void
xsfIMLpreeditstartret(ic, event, data)
    FXimp_Client   *ic;
    XEvent         *event;
    int             data;
{

    iml_inst       *rv = NULL;

#ifdef	XSF_PREEDIT_RETURN_USED
#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMPreeditstart Called\n");
#endif				/* DEBUG */

    if (!ic->cell_link)
	return;
    if (!ic->cell_link->called_rv)
	return;

    ic->cell_link->called_rv->operand = (caddr_t) & data;

    (void) IMLogic(ic->id, &rv,
		   IML_SEND_RESULTS, ic->cell_link->called_rv, NULL);

    xsfinterpret(ic, rv, NULL, NULL, event);

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMPreeditstart Return\n");
#endif				/* DEBUG */
#endif	/* XSF_PREEDIT_RETURN_USED */
    return;

}

/* IMLogic Result4 */
void
xsfIMLpreeditcaretret(ic, event, data)
    FXimp_Client   *ic;
    XEvent         *event;
    int             data;
{

    iml_inst       *rv = NULL;
    XIMPreeditCaretCallbackStruct *pcc;

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMPreeditcaret Called\n");
#endif				/* DEBUG */

    if (!ic->cell_link)
	return;
    if (!ic->cell_link->called_rv)
	return;

    pcc = (XIMPreeditCaretCallbackStruct *)&ic->cell_link->called_rv->operand ;
    pcc->position = data;

    (void) IMLogic(ic->id, &rv,
		   IML_SEND_RESULTS, ic->cell_link->called_rv, NULL);

    xsfinterpret(ic, rv, NULL, NULL, event);

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMPreeditcaret Return\n");
#endif				/* DEBUG */
    return;

}

void
xsfIMLbegin(ic, event)
    FXimp_Client   *ic;
    XEvent         *event;
{

    iml_inst       *rv = NULL;
    iml_status_t    status;

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMLbegin Called\n");
#endif				/* DEBUG */

    status = IMLSTATUS_Henkan_Mode;

    (void) IMLogic(ic->id, &rv,
		   IML_SET_STATUS, &status, NULL);

    xsfinterpret(ic, rv, NULL, NULL, event);

    (void)SetFocusClient();

#ifdef	DEBUG
    (void) dbg_printf("Xsf-IMLbegin Return\n");
#endif				/* DEBUG */

    return;

}
