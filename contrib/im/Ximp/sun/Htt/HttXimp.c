/*
 *        %Z%%M% %I% %E%  
 * 
 * Derived from @(#)Ximp.c	2.1 91/08/13 11:28:42 FUJITSU LIMITED. */
/*
****************************************************************************

            Copyright 1991, by FUJITSU LIMITED
            Copyright 1991, by Sun Microsystems, Inc.

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of FUJITSU LIMITED or Sun
Microsystems, Inc.  not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.  FUJITSU LIMITED and Sun Microsystems, Inc. make no
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

Auther: Takashi Fujiwara		FUJITSU LIMITED
		fujiwara@a80.tech.yk.fujitsu.co.jp
	Kiyoaki oya			FUJITSU LIMITED
	Riki	Kawaguchi		FUJITSU LIMITED
	Tomoki	Kuriki			FUJITSU LIMITED
        Hideki Hiura (hhiura@Sun.COM) 	Sun Microsystems, Inc.

****************************************************************************
*/

#ifndef lint
static char     derived_from_sccs_id[] = "@(#)Ximp.c	2.1 91/08/13 11:28:42 FUJITSU LIMITED.";
static char     sccsid[] = "%Z%%M% %I% %E% Sun Microsystems Inc.";
#endif

#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "HttTransTbl.h"

#ifdef	XSF_X11R5
#include "XSunExt.h"
#include "Htt.h"
#endif				/* XSF_X11R5	 */

Atom            selection_atom;

extern unsigned char vendor_name[];
extern unsigned char server_name[];
extern unsigned char server_version[];

#define  XIMP_E_MAX_USER	101
#define  XIMP_TIME_OUT		5

XIMStyle        style[] = {
    XIMPreeditPosition | XIMStatusNothing,
    XIMPreeditNothing | XIMStatusNothing,
    XIMPreeditPosition | XIMStatusArea,
    XIMPreeditCallbacks | XIMStatusNothing,
    XIMPreeditCallbacks | XIMStatusCallbacks,
    XIMPreeditCallbacks | XIMStatusCallbacks | XIMLookupCallbacks,
};
#define  MAX_STYLE		sizeof( style ) / sizeof( style[0] )

#define  MAX_KEY     6
unsigned long   C_key[MAX_KEY] =
{0x00000000, (ControlMask | ShiftMask), XK_Henkan_Mode,
    ControlMask, (ControlMask | ShiftMask), XK_space,	/* CTRL space */
};

Atom            IMA_IMPROTOCOL;
Atom            IMA_VERSION, IMA_FOCUS;
Atom            IMA_PREEDIT, IMA_PREEDITFONT;
Atom            IMA_STATUS, IMA_STATUSFONT;
Atom            IMA_CTEXT, IMA_COMPOUND_TEXT;
Atom            IMA_EXT_SAMPLE_STATUSWINDOW;
Atom            IMA_EXT_SAMPLE_BACK_FRONT;
Atom            IMA_PREEDIT_DRAW_DATA;
Atom            IMA_FEEDBACKS;
Atom            IMA_EXT_XIMP_LOOKUPCHOICES;
Atom            IMA_EXT_XIMP_CHOICE_START_REQ;
Atom            IMA_EXT_XIMP_CHOICE_START_REP;
Atom            IMA_EXT_XIMP_CHOICE_DRAW_REQ;
Atom            IMA_EXT_XIMP_CHOICE_PROC_REQ;
Atom            IMA_EXT_XIMP_CHOICE_PROC_REP;
Atom            IMA_EXT_XIMP_CONVERSION;
Atom            XA_COMPOUND_TEXT;

Static FXimp_ICID icid;

int
IsMode(keysym, mod)
    KeySym          keysym;
    unsigned int    mod;
{
    int             i;

    for (i = 0; i < MAX_KEY; i = i + 3)
	if ((mod & C_key[i + 1]) == C_key[i] && keysym == C_key[i + 2])
	    return (1);
    return (0);
}

CreateXimpAtom()
{
    Window          imserver_window;
    Atom            id;
    Atom            type;
    Atom            selection_sub_atom;
#ifdef	XSF_X11R5
    /* 91/07/17    */
#define EXTENSIONS_LIST_SIZE	4
#else
#define EXTENSIONS_LIST_SIZE	3
#endif				/* XSF_X11R5	 */
    Atom            extensions_list[EXTENSIONS_LIST_SIZE];
    long            ext_ui_back_front;
    long            ext_ui_conversion;
    static char     IMsuport_sub_name[16] = "";
    static char     IMsuport_full_name[32] = "";
    static char     suport_locale_name[] = "ja_JP";

    sprintf(IMsuport_sub_name, "%s%s", _XIMP_BASE, suport_locale_name);
    sprintf(IMsuport_full_name, "%s%s@%s.%d", _XIMP_BASE, suport_locale_name,
	    server_name, screen);
    imserver_window = root_client->window;
#ifdef	DEBUG
    dbg_printf("imserver_window=%x\n", imserver_window);
    dbg_printf("IMsuport_sub_name=%s\n", IMsuport_sub_name);
    dbg_printf("IMsuport_full_name=%s\n", IMsuport_full_name);
#endif				/* DEBUG */
    if (selection_atom = XInternAtom(display, IMsuport_full_name, True)) {
	if (XGetSelectionOwner(display, selection_atom)) {
	    ErrorMessage(002);
	    exit(0);
	}
    } else
	selection_atom = XInternAtom(display, IMsuport_full_name, False);
    XSetSelectionOwner(display, selection_atom, imserver_window, 0L);

    if (selection_sub_atom = XInternAtom(display, IMsuport_sub_name, True)) {
	if (XGetSelectionOwner(display, selection_sub_atom)) {
	    WarningMessage(002);
	}
    } else
	selection_sub_atom = XInternAtom(display, IMsuport_sub_name, False);
    XSetSelectionOwner(display, selection_sub_atom, imserver_window, 0L);

#ifdef DEBUG
    XChangeProperty(display, root, selection_atom, XA_STRING, 8, PropModeReplace,
		    server_name, strlen(server_name));
#endif				/* DEBUG */

    id = XInternAtom(display, _XIMP_VERSION, False);
    IMA_VERSION = id;
    XChangeProperty(display, imserver_window, id, XA_STRING, 8, PropModeReplace,
    (unsigned char *) XIMP_PROTOCOL_VERSION, strlen(XIMP_PROTOCOL_VERSION));

    type = XInternAtom(display, _XIMP_STYLE, False);
    id = type;
    XChangeProperty(display, imserver_window, id, type, 32, PropModeReplace,
		    (unsigned char *) style, MAX_STYLE);

    type = XInternAtom(display, _XIMP_KEYS, False);
    id = type;
    XChangeProperty(display, imserver_window, id, type, 32, PropModeReplace,
		    (unsigned char *) C_key, MAX_KEY);

    id = XInternAtom(display, _XIMP_SERVERNAME, False);
    type = XA_STRING;
    XChangeProperty(display, imserver_window, id, type, 8, PropModeReplace,
		    server_name, strlen(server_name));

    id = XInternAtom(display, _XIMP_SERVERVERSION, False);
    type = XA_STRING;
    XChangeProperty(display, imserver_window, id, type, 8, PropModeReplace,
		    server_version, strlen(server_version));

    id = XInternAtom(display, _XIMP_VENDORNAME, False);
    type = XA_STRING;
    XChangeProperty(display, imserver_window, id, type, 8, PropModeReplace,
		    vendor_name, strlen(vendor_name));

    IMA_IMPROTOCOL = XInternAtom(display, _XIMP_PROTOCOL, False);
    IMA_FOCUS = XInternAtom(display, _XIMP_FOCUS, False);
    IMA_PREEDIT = XInternAtom(display, _XIMP_PREEDIT, False);
    IMA_STATUS = XInternAtom(display, _XIMP_STATUS, False);
    IMA_PREEDITFONT = XInternAtom(display, _XIMP_PREEDITFONT, False);
    IMA_STATUSFONT = XInternAtom(display, _XIMP_STATUSFONT, False);
    IMA_CTEXT = XInternAtom(display, _XIMP_CTEXT, False);
    IMA_COMPOUND_TEXT = XInternAtom(display, "COMPOUND_TEXT", False);
    IMA_EXT_SAMPLE_STATUSWINDOW
	= XInternAtom(display, "_XIMP_EXT_XIMP_STATUSWINDOW", False);
    IMA_EXT_SAMPLE_BACK_FRONT
	= XInternAtom(display, "_XIMP_EXT_XIMP_BACK_FRONT", False);
#ifdef	XSF_X11R5
    IMA_EXT_XIMP_CONVERSION
	= XInternAtom(display, "_XIMP_EXT_XIMP_CONVERSION", False);
    IMA_PREEDIT_DRAW_DATA = XInternAtom(display, _XIMP_PREEDIT_DRAW_DATA, False);
    IMA_FEEDBACKS = XInternAtom(display, _XIMP_FEEDBACKS, False);
    IMA_EXT_XIMP_LOOKUPCHOICES = XInternAtom(display, _XIMP_EXT_XIMP_LOOKUPCHOICES, False);
    IMA_EXT_XIMP_CHOICE_START_REQ = XInternAtom(display, _XIMP_EXT_XIMP_CHOICE_START_REQ, False);
    IMA_EXT_XIMP_CHOICE_START_REP = XInternAtom(display, _XIMP_EXT_XIMP_CHOICE_START_REP, False);
    IMA_EXT_XIMP_CHOICE_DRAW_REQ = XInternAtom(display, _XIMP_EXT_XIMP_CHOICE_DRAW_REQ,
					       False);
    IMA_EXT_XIMP_CHOICE_PROC_REQ = XInternAtom(display, _XIMP_EXT_XIMP_CHOICE_PROC_REQ, False);
    IMA_EXT_XIMP_CHOICE_PROC_REP = XInternAtom(display, _XIMP_EXT_XIMP_CHOICE_PROC_REP, False);
    XA_COMPOUND_TEXT = XInternAtom(display, "XA_COMPOUND_TEXT", False);
#endif				/* XSF_X11R5 */

    id = IMA_EXT_SAMPLE_BACK_FRONT;
    type = id;
    ext_ui_back_front = XIMP_FRONTEND;
    XChangeProperty(display, imserver_window, id, type, 32, PropModeReplace,
		    (unsigned char *) &ext_ui_back_front, 1);

    id = IMA_EXT_XIMP_CONVERSION;
    type = id;
    ext_ui_conversion = False;
    XChangeProperty(display, imserver_window, id, type, 32, PropModeReplace,
		    (unsigned char *) &ext_ui_conversion, 1);

    extensions_list[0] = IMA_EXT_SAMPLE_BACK_FRONT;
    extensions_list[1] = IMA_EXT_SAMPLE_STATUSWINDOW;
    extensions_list[2] = IMA_EXT_XIMP_CONVERSION;
#ifdef	XSF_X11R5
    /* 91/07/17  Lookup_Choise  */
    extensions_list[3] = IMA_EXT_XIMP_LOOKUPCHOICES;
#endif				/* XSF_X11R5	 */

    id = XInternAtom(display, _XIMP_EXTENSIONS, False);
    type = id;
    XChangeProperty(display, imserver_window, id, type, 32, PropModeReplace,
		    (unsigned char *) extensions_list, EXTENSIONS_LIST_SIZE);
}


XimpClientMessage(event)
    XEvent         *event;
{
    FXimp_Client   *ximp_client;
    switch (event->xclient.data.l[0]) {
    case XIMP_CREATE:
	XimpCreate(event);
	break;

    case XIMP_KEYPRESS:
    case XIMP_DESTROY:
    case XIMP_BEGIN:
    case XIMP_END:
    case XIMP_SETFOCUS:
    case XIMP_UNSETFOCUS:
    case XIMP_MOVE:
    case XIMP_RESET:
    case XIMP_SETVALUE:
    case XIMP_GETVALUE:
    case XIMP_EXTENSION:
#ifdef	XSF_X11R5
	case XIMP_PREEDITSTART_RETURN :
	case XIMP_PREEDITCARET_RETURN :
#endif	/* XSF_X11R5 */
	{
	    icid = (FXimp_ICID) event->xclient.data.l[1];
#ifdef DEBUG
	    dbg_printf("<<Ximp..>> icid=%x\n", icid);
#endif				/* DEBUG */
	    ximp_client = FindClientByICID(icid);
	    if (!ximp_client) {
		int             errno = 0;
		switch (event->xclient.data.l[0]) {
		case XIMP_KEYPRESS:
		    errno = 185;
		    break;
		case XIMP_DESTROY:
		    errno = 111;
		    break;
		case XIMP_BEGIN:
		    errno = 121;
		    break;
		case XIMP_END:
		    errno = 131;
		    break;
		case XIMP_SETFOCUS:
		    errno = 141;
		    break;
		case XIMP_UNSETFOCUS:
		    errno = 151;
		    break;
		case XIMP_MOVE:
		    errno = 161;
		    break;
		case XIMP_RESET:
		    errno = 181;
		    break;
		case XIMP_SETVALUE:
		    errno = 186;
		    break;
		case XIMP_GETVALUE:
		    errno = 187;
		    break;
		case XIMP_EXTENSION:
		    errno = 190;
		    break;
#ifdef  XSF_X11R5
		case XIMP_PREEDITSTART_RETURN:
		    errno = 171;
		    break;
		case XIMP_PREEDITCARET_RETURN:
		    errno = 172;
		    break;
#endif				/* XSF_X11R5	 */
		}
		ErrorMessage(errno);
		return (-1);
	    }
	    current_client = ximp_client;
	    switch (event->xclient.data.l[0]) {
	    case XIMP_KEYPRESS:
		XimpKeypress(event);
		break;
	    case XIMP_DESTROY:
		XimpDestroy(event);
		break;
	    case XIMP_BEGIN:
		XimpBegin(event);
		break;
	    case XIMP_END:
		XimpEnd(event);
		break;
	    case XIMP_SETFOCUS:
		XimpSetfocus(event);
		break;
	    case XIMP_UNSETFOCUS:
		XimpUnsetfocus(event);
		break;
	    case XIMP_MOVE:
		XimpMove(event);
		break;
	    case XIMP_RESET:
		XimpReset(event);
		break;
	    case XIMP_SETVALUE:
		XimpSetvalue(event);
		break;
	    case XIMP_GETVALUE:
		XimpGetvalue(event);
		break;
	    case XIMP_EXTENSION:
		XimpExtension(event);
		break;
#ifdef  XSF_X11R5
	    case XIMP_PREEDITSTART_RETURN:
		XimpPreeditStartRet(event);
		break;
	    case XIMP_PREEDITCARET_RETURN:
		XimpPreeditCaretRet(event);
		break;
#endif				/* XSF_X11R5	 */
	    }
	    break;
	}

    default:
	ErrorMessage(100);
	icid = (FXimp_ICID) event->xclient.data.l[1];
	ximp_client = FindClientByICID(icid);
	if (!ximp_client)
	    XimpSendErrorEvent(ximp_client->focus_window, 0,
			       event->xclient.serial, XIMP_BadProtocol, 0);
	break;
    }
    return (0);
}

Static int
XimpCreate(event)
    XEvent         *event;
{
    Window          window = event->xclient.data.l[1];
    XIMStyle        inputStyle = event->xclient.data.l[2];
    unsigned long   mask = event->xclient.data.l[3];
    FXimp_Client   *ximp_client;
    char           *GetXimpSubProperty();
    Window         *focus_window;
    int             rc;
    char           *ximp_version, prop_name[32];
    Ximp_StatusPropRec status, *current_status;
    Ximp_PreeditPropRec preedit, *current_preedit;

#ifdef DEBUG
    dbg_printf("<<XimpCreate>> window=%x, inputStyle=%x mask=%x\n",
	       window, inputStyle, mask);
#endif				/* DEBUG */

    if (inputStyle == 0) {
	ErrorMessage(101);
	XimpSendErrorEvent(window, 0, event->xclient.serial, XIMP_BadStyle, 0);
	return (-1);
    }
    ximp_client = NewClient(window);
    if (!ximp_client) {
	ErrorMessage(103);
	XimpSendErrorEvent(window, 0, event->xclient.serial,
			   XIMP_BadAlloc, XIMP_E_MAX_USER);
	return (-1);
    }
    current_client = ximp_client;
    current_client->prot_type = FPRT_XIM;
    current_client->imp_mode = FIMP_CREATE;
    Fj_InitialMode();
    current_client->style = inputStyle;

    sprintf(prop_name, "_XIMP_CTEXT_%d", current_client);
    current_client->ximp_readprop_atom = XInternAtom(display, prop_name, False);

    ximp_version = GetXimpSubProperty(window, IMA_VERSION, XA_STRING);
#ifdef DEBUG
    dbg_printf(" Client XIMP_VERSION : (%s)\n", ximp_version);
    if (strcmp(XIMP_PROTOCOL_VERSION, ximp_version))
	dbg_printf(" ** unliked XIMP_VERSION : (%s)\n", XIMP_PROTOCOL_VERSION);
#endif				/* DEBUG */

    if (mask & XIMP_FOCUS_WIN_MASK) {
	focus_window = (Window *) GetXimpSubProperty(window,
						     IMA_FOCUS, XA_WINDOW);
	if (focus_window) {
	    current_client->focus_window = *focus_window;
	    XFree((char *) focus_window);
	} else {
	    ErrorMessage(108, _XIMP_FOCUS);
	    XimpSendErrorEvent(current_client->focus_window, 0,
			     event->xclient.serial, XIMP_BadFocusWindow, 0);
	    DeleteClient(current_client);
	    return (-1);
	}
    } else {
	ErrorMessage(108, _XIMP_FOCUS);
	XimpSendErrorEvent(current_client->focus_window, 0,
			   event->xclient.serial, XIMP_BadFocusWindow, 0);
	DeleteClient(current_client);
	return (-1);
    }

    current_preedit = current_client->preedit;
    *current_preedit = *root_client->preedit;
    if (current_client->style & (XIMPreeditPosition | XIMPreeditNothing)) {
	if (mask & (XIMP_PRE_AREA_MASK | XIMP_PRE_FG_MASK | XIMP_PRE_BG_MASK |
		    XIMP_PRE_COLORMAP_MASK | XIMP_PRE_BGPIXMAP_MASK |
		    XIMP_PRE_LINESP_MASK | XIMP_PRE_CURSOR_MASK |
		    XIMP_PRE_AREANEED_MASK | XIMP_PRE_SPOTL_MASK)) {
	    rc = GetPreeditPropRec(window, current_preedit, mask);
	    if (rc != 0) {
		GetPropErrorSendEvent(rc, current_client->focus_window,
				      0, event->xclient.serial);
		goto error_imp_create;
	    }
	} else {
	    if (current_client->style & XIMPreeditNothing)
		current_client->window_type |= FC_SAME_ROOT_PREEDIT_WINDOW;
	    current_client->window_type |= FC_SAME_ROOT_PREEDIT_GC;
	}

	if (mask & XIMP_PRE_FONT_MASK) {
	    current_client->preedit_fontname =
		GetXimpSubProperty(window, IMA_PREEDITFONT, XA_STRING);
	    current_client->window_type &= ~FC_SAME_ROOT_PREEDIT_GC;
	} else
	    current_client->window_type |= FC_SAME_ROOT_PREEDIT_FONT;
    }
    current_status = current_client->status;
    *current_status = *root_client->status;
    if (current_client->style & (XIMStatusArea | XIMStatusNothing)) {
	if (mask & (XIMP_STS_AREA_MASK | XIMP_STS_FG_MASK | XIMP_STS_BG_MASK |
		    XIMP_STS_COLORMAP_MASK | XIMP_STS_BGPIXMAP_MASK |
		    XIMP_STS_LINESP_MASK | XIMP_STS_CURSOR_MASK |
		    XIMP_STS_AREANEED_MASK | XIMP_STS_WINDOW_MASK)) {
	    rc = GetStatusPropRec(window, current_status, mask);
	    if (rc != 0) {
		GetPropErrorSendEvent(rc, current_client->focus_window,
				      0, event->xclient.serial);
		goto error_imp_create;
	    }
	} else {
	    if (current_client->style & XIMStatusNothing)
		current_client->window_type |= FC_SAME_ROOT_STATUS_WINDOW;
	    current_client->window_type |= FC_SAME_ROOT_STATUS_GC;
	}

	if (mask & XIMP_STS_FONT_MASK) {
	    current_client->status_fontname =
		GetXimpSubProperty(window, IMA_STATUSFONT, XA_STRING);
	    current_client->window_type &= ~FC_SAME_ROOT_STATUS_GC;
	} else
	    current_client->window_type |= FC_SAME_ROOT_STATUS_FONT;
    }
    current_preedit_font = current_client->preedit_font;
    rc = CreateStatusWindow();
    if (rc != 0)
	goto error_imp_create;
    rc = CreatePreeditWindow();
    if (rc != 0)
	goto error_imp_create;

    XSelectInput(display, current_client->focus_window, StructureNotifyMask);
    XimpSendEvent(current_client->focus_window,
		  XIMP_CREATE_RETURN, current_client, 0, 0, 0);
#ifdef	DEBUG
    dbg_printf("** Ximp-Create-Return icid = %x\n", current_client);
#endif				/* DEBUG */
#ifdef	XSF_X11R5
    xsfmgr_client_init(current_client);
    xsfIMLopen(current_client);
#endif				/* XSF_X11R5 */

    return (0);

error_imp_create:
    ErrorMessage(101);
    DeleteClient(current_client);
    return (-1);
}

Static int
GetPreeditPropRec(window, current_preedit, mask)
    Window          window;
    Ximp_PreeditPropRec *current_preedit;
    unsigned long   mask;
{
    Ximp_PreeditPropRec preedit;
    int             rc;
    rc = GetXimpProperty(window, IMA_PREEDIT, &preedit,
			 sizeof(Ximp_PreeditPropRec), _XIMP_PREEDIT);
    if (rc != 0)
	return (rc);
    return SetPreeditPropRec(current_preedit, &preedit, mask);
}

Static int
SetPreeditPropRec(current_preedit, preedit, mask)
    Ximp_PreeditPropRec *current_preedit, *preedit;
    unsigned long   mask;
{
    if (mask & XIMP_PRE_AREA_MASK)
	current_preedit->Area = preedit->Area;
    if (mask & XIMP_PRE_FG_MASK)
	current_preedit->Foreground = preedit->Foreground;
    if (mask & XIMP_PRE_BG_MASK)
	current_preedit->Background = preedit->Background;
    if (mask & XIMP_PRE_COLORMAP_MASK)
	current_preedit->Colormap = preedit->Colormap;
    if (mask & XIMP_PRE_BGPIXMAP_MASK)
	current_preedit->Bg_Pixmap = preedit->Bg_Pixmap;
    if (mask & XIMP_PRE_LINESP_MASK)
	current_preedit->LineSpacing = preedit->LineSpacing;
    if (mask & XIMP_PRE_CURSOR_MASK)
	current_preedit->Cursor = preedit->Cursor;
    if (mask & XIMP_PRE_AREANEED_MASK)
	current_preedit->AreaNeeded = preedit->AreaNeeded;
    if (mask & XIMP_PRE_SPOTL_MASK)
	current_preedit->SpotLocation = preedit->SpotLocation;
#ifdef DEBUG
    dbg_PreeditDump(current_preedit);
#endif				/* DEBUG */
    return (0);
}

Static int
GetStatusPropRec(window, current_status, mask)
    Window          window;
    Ximp_StatusPropRec *current_status;
    unsigned long   mask;
{
    Ximp_StatusPropRec status;
    int             rc;
    rc = GetXimpProperty(window, IMA_STATUS, &status,
			 sizeof(Ximp_StatusPropRec), _XIMP_STATUS);
    if (rc != 0)
	return (rc);
    return SetStatusPropRec(current_status, &status, mask);
}

Static int
SetStatusPropRec(current_status, status, mask)
    Ximp_StatusPropRec *current_status, *status;
    unsigned long   mask;
{
    if (mask & XIMP_STS_AREA_MASK)
	current_status->Area = status->Area;
    if (mask & XIMP_STS_FG_MASK)
	current_status->Foreground = status->Foreground;
    if (mask & XIMP_STS_BG_MASK)
	current_status->Background = status->Background;
    if (mask & XIMP_STS_COLORMAP_MASK)
	current_status->Colormap = status->Colormap;
    if (mask & XIMP_STS_BGPIXMAP_MASK)
	current_status->Bg_Pixmap = status->Bg_Pixmap;
    if (mask & XIMP_STS_LINESP_MASK)
	current_status->LineSpacing = status->LineSpacing;
    if (mask & XIMP_STS_CURSOR_MASK)
	current_status->Cursor = status->Cursor;
    if (mask & XIMP_STS_AREANEED_MASK)
	current_status->AreaNeeded = status->AreaNeeded;
#ifdef DEBUG
    dbg_StatusDump(current_status);
#endif				/* DEBUG */
    return (0);
}

Static char    *
GetXimpSubProperty(window, atomid, type)
    Window          window;
    Atom            atomid;
    Atom            type;
{
    Atom            actual_type_ret;
    int             actual_format_ret;
    unsigned long   nitems_ret;
    unsigned long   bytes_after_ret;
    unsigned char  *win_data;

    if (atomid == 0)
	return ((char *) 0);

    XGetWindowProperty(display, window, atomid, 0L, 8192L,
	      True, type, &actual_type_ret, &actual_format_ret, &nitems_ret,
		       &bytes_after_ret, &win_data);
    if (actual_format_ret == 0 || nitems_ret == 0) {
#ifdef DEBUG
	dbg_printf("XGetWindowProperty error : atomid=%x (%s) type=%d actual_format_ret=%d nitems_ret=%d\n",
		   atomid, XGetAtomName(display, atomid), type, actual_format_ret, nitems_ret);
#endif				/* DEBUG */
	return ((char *) 0);
    }
#ifdef DEBUG
    if (atomid == IMA_FOCUS) {
	int            *data = (int *) win_data;
	dbg_printf("%s = %x\n", _XIMP_FOCUS, *data);
    } else if (atomid == IMA_PREEDITFONT
	       || atomid == IMA_STATUSFONT) {
	char           *s = (atomid == IMA_PREEDITFONT) ? _XIMP_PREEDITFONT
	: _XIMP_STATUSFONT;
	dbg_printf("%s = %s\n", s, win_data);
    }
#endif				/* DEBUG */

    return ((char *) win_data);
}

Static int
GetXimpProperty(window, atomid, win_area, size, type_name)
    Window          window;
    Atom            atomid;
    char           *win_area;
    int             size;
    char           *type_name;
{
    Atom            actual_type_ret;
    int             actual_format_ret;
    unsigned long   nitems_ret;
    unsigned long   bytes_after_ret;
    unsigned char  *win_data;

    if (atomid) {
	XGetWindowProperty(display, window, atomid, 0L, 1000000L,
	    True, atomid, &actual_type_ret, &actual_format_ret, &nitems_ret,
			   &bytes_after_ret, &win_data);
	if (actual_format_ret == 0) {
	    ErrorMessage(108, type_name);
	    return (-2);
	}
	if ((nitems_ret * 4) != size) {
	    ErrorMessage(109, type_name, nitems_ret * 4);
#ifdef DEBUG
	    dbg_printf("%s: actual_format_ret=%d actual_type_ret=%d nitems_ret=%d bytes_after_ret=%d\n",
		       type_name, actual_format_ret, actual_type_ret, nitems_ret, bytes_after_ret);
	    if (nitems_ret > 0) {
		int             di, *data = (int *) win_data;
		for (di = 0; di < nitems_ret; di++)
		    dbg_printf(" %x", *data++);
		dbg_printf("\n");
	    }
#endif				/* DEBUG */
	    XFree((char *) win_data);
	    return (-3);
	}
	bcopy(win_data, win_area, size);
	XFree((char *) win_data);
    } else
	return (-1);

#ifdef DEBUG
    if (atomid == IMA_PREEDIT) {
	dbg_PreeditDump((Ximp_PreeditPropRec *) win_area);
    } else if (atomid == IMA_STATUS) {
	dbg_StatusDump((Ximp_StatusPropRec *) win_area);
    }
#endif				/* DEBUG */

    return (0);
}

#ifdef DEBUG
dbg_PreeditDump(preedit)
    Ximp_PreeditPropRec *preedit;
{
    dbg_printf("  Preedit : x=%d y=%d w=%d h=%d fg=%x bg=%x colo=%x bgpix=%x LS=%d curs=%x a.w=%d a.h=%d sl.x=%d sl.y=%d\n",
	       preedit->Area.x, preedit->Area.y,
	       preedit->Area.width, preedit->Area.height,
	       preedit->Foreground, preedit->Background,
	       preedit->Colormap, preedit->Bg_Pixmap,
	       preedit->LineSpacing, preedit->Cursor,
	       preedit->AreaNeeded.width, preedit->AreaNeeded.height,
	       preedit->SpotLocation.x, preedit->SpotLocation.y);
}

dbg_StatusDump(status)
    Ximp_StatusPropRec *status;
{
    dbg_printf("  Status : x=%d y=%d w=%d h=%d fg=%x bg=%x colo=%x bgpix=%x LS=%d curs=%x a.w=%d a.h=%d wid=%x\n",
	       status->Area.x, status->Area.y,
	       status->Area.width, status->Area.height,
	       status->Foreground, status->Background,
	       status->Colormap, status->Bg_Pixmap,
	       status->LineSpacing, status->Cursor,
	       status->AreaNeeded.width, status->AreaNeeded.height,
	       status->window);
}
#endif				/* DEBUG */

Static int
GetPropErrorSendEvent(rc, focus_window, icid, serial)
    long            rc;
    Window          focus_window;
    unsigned long   icid, serial;
{
    if (rc == -2)
	XimpSendErrorEvent(focus_window, icid, serial, XIMP_BadProperty, 0);
    if (rc == -3)
	XimpSendErrorEvent(focus_window, icid, serial, XIMP_BadPropertyType, 0);
    return (0);
}

Static int
XimpSendEvent(window, imp_func, icid, data1, data2, data3)
    Window          window;
    unsigned long   imp_func, icid, data1, data2, data3;
{
    XEvent          event;

    bzero(&event, sizeof(event));
    event.type = ClientMessage;
    event.xclient.format = 32;
    event.xclient.window = window;
    event.xclient.message_type = IMA_IMPROTOCOL;
    event.xclient.data.l[0] = imp_func;
    event.xclient.data.l[1] = icid;
    event.xclient.data.l[2] = data1;
    event.xclient.data.l[3] = data2;
    event.xclient.data.l[4] = data3;
#ifdef DEBUG
    dbg_printf("XimpSendEvent:32:w=%d:%x:%x:%x:%x:%x\n",
	    window, imp_func, icid, data1, data2, data3);
#endif
    XSendEvent(display, window, False, NoEventMask, &event);
    XFlush(display);
    return (0);
}

#ifdef	XSF_X11R5
static int
XimpSendEvent2(ic, event, format)
    FXimp_Client   *ic;
    XEvent         *event;
    int             format;
{

    event->type = ClientMessage;
    event->xclient.format = format;
    event->xclient.window = ic->focus_window;
    event->xclient.message_type = IMA_IMPROTOCOL;
#ifdef DEBUG
    dbg_printf("XimpSendEvent2:%d:w=%d\n",
	    format, ic->focus_window);
    if(event)
      _iml_xdmem(event, sizeof(XEvent));
#endif
    XSendEvent(display, ic->focus_window, False, NoEventMask, event);
    XFlush(display);
    return (0);
}

static          Atom
XimpReplaceProp(ic, prop_key, type, str, len, format)
    FXimp_Client   *ic;
    char           *prop_key;
    Atom            type;
    char           *str;
    int             len;
    int             format;
{
    Atom            atom;
    char            prop_name[32];

#ifdef DEBUG
    dbg_printf("<<XimpReplaceProp>> root_client->window=%x\n", root_client->window);
    dbg_printf("<<XimpReplaceProp>> ic=%x\n", ic);
    dbg_printf("<<XimpReplaceProp>> prop_key=%s\n", prop_key);
    dbg_printf("<<XimpReplaceProp>> str=%s\n", str);
    dbg_printf("<<XimpReplaceProp>> len=%d\n", len);
    dbg_printf("<<XimpReplaceProp>> format=%d\n", format);
#endif				/* DEBUG */

    (void) sprintf(prop_name, "%s%d", prop_key, ic);
    atom = XInternAtom(display, prop_name, False);
    XChangeProperty(display, root_client->window, atom, type, format,
		    PropModeReplace, (unsigned char *) str, len);
    XFlush(display);
    return (atom);
}

static          Atom
XimpAppendProp(ic, prop_key, type, str, len, format)
    FXimp_Client   *ic;
    char           *prop_key;
    Atom            type;
    char           *str;
    int             len;
    int             format;
{
    Atom            atom;
    char            prop_name[32];

#ifdef DEBUG
    dbg_printf("<<XimpAppendProp>> current_client->focus_window=%x\n", current_client->focus_window);
    dbg_printf("<<XimpAppendProp>> ic=%x\n", ic);
    dbg_printf("<<XimpAppendProp>> prop_key=%s\n", prop_key);
    dbg_printf("<<XimpAppendProp>> str=%s\n", str);
    dbg_printf("<<XimpAppendProp>> len=%d\n", len);
    dbg_printf("<<XimpAppendProp>> format=%d\n", format);
#endif				/* DEBUG */

    (void) sprintf(prop_name, "%s%d", prop_key, ic);
    atom = XInternAtom(display, prop_name, False);
    XChangeProperty(display, current_client->focus_window, atom, type, format,
		    PropModeAppend, (unsigned char *) str, len);
    XFlush(display);
    return (atom);
}

int
XimpCommitSendProp(ic, csstr, cslen)
    FXimp_Client   *ic;
    char           *csstr;
    int             cslen;
{
    XEvent          event;

#ifdef DEBUG
    dbg_printf("<<XimpCommitSendProp>> ic=%x\n", ic);
    dbg_printf("<<XimpCommitSendProp>> csstr=%s\n", csstr);
    dbg_printf("<<XimpCommitSendProp>> cslen=%d\n", cslen);
    dbg_printf("<<XimpCommitSendProp>> event=%d\n", &event);
#endif				/* DEBUG */

    bzero(&event, sizeof(event));
    event.xclient.data.l[0] = XIMP_READPROP;
    event.xclient.data.l[1] = (long) ic;
    event.xclient.data.l[2] =
	XimpReplaceProp(ic, _XIMP_COMMIT, IMA_COMPOUND_TEXT, csstr, cslen, 8);
#ifdef DEBUG
    dbg_printf("<<XimpCommitSendProp>> atom=%x\n", event.xclient.data.l[2]);
#endif				/* DEBUG */
    XimpSendEvent2(ic, &event, 32);
    XFlush(display);
    return (0);
}

int
XimpCommitSendCmsg(ic, csstr, cslen)
    FXimp_Client   *ic;
    char           *csstr;
    int             cslen;
{
    XEvent          event;
    Ximp_CommitPropRec *comm = (Ximp_CommitPropRec *) & event.xclient.data.l[0];

    bzero(&event, sizeof(event));
    comm->icid = (unsigned long) ic;
    comm->size = (char) cslen;
    (void) bcopy(csstr, comm->ctext, cslen);
    XimpSendEvent2(ic, &event, 8);
#ifdef DEBUG
    dbg_printf("** Ximp-Commit-Send-Client-Message\n");
    dbg_printf("	ic=%x\n", ic);
    dbg_printf("	csstr=%s\n", csstr);
    dbg_printf("	cslen=%d\n", cslen);
#endif				/* DEBUG */

    XFlush(display);
    return (0);
}

int
XimpPreEditStartSend(ic)
    FXimp_Client   *ic;
{
    XEvent          event;

#ifdef DEBUG
    dbg_printf("** Ximp-PreEdit-Start-Client-Message-Send **\n");
    dbg_printf("	ic=%x\n", ic);
#endif				/* DEBUG */

    bzero(&event, sizeof(event));
    event.xclient.data.l[0] = XIMP_PREEDITSTART;
    event.xclient.data.l[1] = (long) ic;
    XimpSendEvent2(ic, &event, 32);
    XFlush(display);
    return (0);
}

int
XimpPreEditDrawSendProp(ic, pds, textarea, pdslen)
    FXimp_Client   *ic;
    XIMPreeditDrawCallbackStruct *pds;
    char           *textarea;
    int             pdslen;
{
    XEvent          event;
    long            atom1[4];

#ifdef DEBUG
    dbg_printf("<<XimpPreEditDrawSendProp>> ic=%x\n", ic);
    dbg_printf("<<XimpPreEditDrawSendProp>> pds=%x\n", pds);
    dbg_printf("<<XimpPreEditDrawSendProp>> pdslen=%d\n", pdslen);
    dbg_printf("<<XimpPreEditDrawSendProp>> event=%x\n", &event);
#endif				/* DEBUG */

    bzero(&event, sizeof(event));
    event.xclient.data.l[0] = XIMP_PREEDITDRAW;
    event.xclient.data.l[1] = (long) ic;
    atom1[0] = (long) pds->caret;
    atom1[1] = (long) pds->chg_first;
    atom1[2] = (long) pds->chg_length;
    atom1[3] = 0;
    event.xclient.data.l[2] =
	XimpReplaceProp(ic, _XIMP_CALLBACKS_1, IMA_PREEDIT_DRAW_DATA,
		    (char *) atom1, 3, 32);
    event.xclient.data.l[3] =
	XimpReplaceProp(ic, _XIMP_CALLBACKS_2, IMA_COMPOUND_TEXT,
		    textarea, pdslen, 32);

    if (pds->text) {
	event.xclient.data.l[4] =
	    XimpReplaceProp(ic, _XIMP_CALLBACKS_3, IMA_FEEDBACKS,
			(char *) pds->text->feedback, pds->text->length, 32);
    } else {
	event.xclient.data.l[4] =
	    XimpReplaceProp(ic, _XIMP_CALLBACKS_3, IMA_FEEDBACKS,
			NULL, NULL, 32);
    }

#ifdef DEBUG
    dbg_printf("	Atom1 : %x\n", event.xclient.data.l[2]);
    dbg_printf("	Atom2 : %x\n", event.xclient.data.l[3]);
    dbg_printf("	Atom3 : %x\n", event.xclient.data.l[4]);
#endif				/* DEBUG */

    XimpSendEvent2(ic, &event, 32);
    XFlush(display);
    return (0);
}

int
XimpPreEditDrawSendCmsg(ic, pds, textarea, pdslen)
    FXimp_Client   *ic;
    XIMPreeditDrawCallbackStruct *pds;
    char           *textarea;
    int             pdslen;
{
    XEvent          event;
    Ximp_uslong     chg;
    Ximp_PreEditDrawCallbackPropRec *pdc =
    (Ximp_PreEditDrawCallbackPropRec *) & event.xclient.data.l[0];

#ifdef DEBUG
    dbg_printf("** Ximp-PreEdit-Draw-Client-Message\n");
    dbg_printf("	ic = %x\n", ic);
    dbg_printf("	caret = %d\n", pds->caret);
    dbg_printf("	chg_first = %d\n", pds->chg_first);
    dbg_printf("	chg_length = %d\n", pds->chg_length);
    dbg_printf("	pdslen = %d\n", pdslen);
    dbg_printf("	textarea = %s\n", textarea);
#endif				/* DEBUG */

    bzero(&event, sizeof(event));
    event.xclient.data.l[0] = XIMP_PREEDITDRAW_CM;
    event.xclient.data.l[1] = (long) ic;
    event.xclient.data.l[2] = pds->caret;
    chg.slong.chg_first = (short) pds->chg_first;
    chg.slong.chg_length = (short) pds->chg_length;
    event.xclient.data.l[3] = chg.l;
    if (pds->text) {
	event.xclient.data.l[4] = *pds->text->feedback;
    } else {
	event.xclient.data.l[4] = 0;
	event.xclient.data.l[2] |= (XIMP_PDCBSTATUS_NOTEXT|XIMP_PDCBSTATUS_NOFEEDBACK)<<16 ;
	
    }
    XimpSendEvent2(ic, &event, 32);

    bzero(&event, sizeof(event));
    pdc->icid = (unsigned long) ic;
    pdc->size = (char) pdslen;
    (void) bcopy(textarea, pdc->ctext, pdslen);
    XimpSendEvent2(ic, &event, 8);

    XFlush(display);
    return (0);
}

int
XimpPreEditDoneSend(ic)
    FXimp_Client   *ic;
{
    XEvent          event;

#ifdef DEBUG
    dbg_printf("<<XimpPreEditDoneSend>> ic=%x\n", ic);
    dbg_printf("<<XimpPreEditDoneSend>> event=%x\n", &event);
#endif				/* DEBUG */

    bzero(&event, sizeof(event));
    event.xclient.data.l[0] = XIMP_PREEDITDONE;
    event.xclient.data.l[1] = (long) ic;
    XimpSendEvent2(ic, &event, 32);
    XFlush(display);
    return (0);
}

int
XimpPreEditCaretSend(ic, pcs)
    FXimp_Client   *ic;
    XIMPreeditCaretCallbackStruct *pcs;
{
    XEvent          event;

#ifdef DEBUG
    dbg_printf("<<XimpPreEditCaretSend>> ic=%x\n", ic);
    dbg_printf("<<XimpPreEditCaretSend>> pcs=%x\n", pcs);
    dbg_printf("<<XimpPreEditCaretSend>> event=%x\n", &event);
#endif				/* DEBUG */

    bzero(&event, sizeof(event));
    event.xclient.data.l[0] = XIMP_PREEDITCARET;
    event.xclient.data.l[1] = (long) ic;
    event.xclient.data.l[2] = pcs->position;
    event.xclient.data.l[3] = pcs->direction;
    event.xclient.data.l[4] = pcs->style;
    XimpSendEvent2(ic, &event, 32);
    XFlush(display);
    return (0);
}

int
XimpStatusStartSend(ic)
    FXimp_Client   *ic;
{
    XEvent          event;

    bzero(&event, sizeof(event));
    event.xclient.data.l[0] = XIMP_STATUSSTART;
    event.xclient.data.l[1] = (long) ic;
    XimpSendEvent2(ic, &event, 32);
    XFlush(display);
#ifdef DEBUG
    dbg_printf("**Ximp: Status-Start Callback\n");
    dbg_printf("<<XimpStatusStartSend>> ic=%x\n", ic);
    dbg_printf("<<XimpStatusStartSend>> event=%x\n", &event);
#endif				/* DEBUG */

    return (0);
}

int
XimpStatusDrawSendProp(ic, sds, textarea, sdslen)
    FXimp_Client   *ic;
    XIMStatusDrawCallbackStruct *sds;
    char           *textarea;
    int             sdslen;
{
    XEvent          event;

#ifdef DEBUG
    dbg_printf("<<XimpStatusDrawSendProp>> ic=%x\n", ic);
    dbg_printf("<<XimpStatusDrawSendProp>> sds=%x\n", sds);
    dbg_printf("<<XimpStatusDrawSendProp>> sdslen=%d\n", sdslen);
    dbg_printf("<<XimpStatusDrawSendProp>> event=%x\n", &event);
#endif				/* DEBUG */

    bzero(&event, sizeof(event));
    event.xclient.data.l[0] = XIMP_STATUSDRAW;
    event.xclient.data.l[1] = (long) ic;
    event.xclient.data.l[2] = (long) sds->type;
    if (sds->type == XIMTextType) {
	event.xclient.data.l[3] =
	    XimpReplaceProp(ic, _XIMP_CALLBACKS_1, IMA_COMPOUND_TEXT,
			textarea, sdslen, 8);
	event.xclient.data.l[4] =
	    XimpReplaceProp(ic, _XIMP_CALLBACKS_2, IMA_FEEDBACKS,
	     (char *) sds->data.text->feedback, sds->data.text->length, 32);
    } else {
	event.xclient.data.l[3] = sds->data.bitmap;
    }

    XimpSendEvent2(ic, &event, 32);
    XFlush(display);
    return (0);
}

int
XimpStatusDrawSendCmsg(ic, sds, textarea, sdslen)
    FXimp_Client   *ic;
    XIMStatusDrawCallbackStruct *sds;
    char           *textarea;
    int             sdslen;
{
    XEvent          event;
    Ximp_StatusDrawCallbackPropRec *sdc =
    (Ximp_StatusDrawCallbackPropRec *) & event.xclient.data.l[0];

#ifdef DEBUG
    dbg_printf("** Ximp-Status-DrawCM-Callback (part- 1)\n");
    dbg_printf("  	ic=%x\n", ic);
    dbg_printf("	type=%d\n", sds->type);
#endif				/* DEBUG */

    bzero(&event, sizeof(event));
    event.xclient.data.l[0] = XIMP_STATUSDRAW_CM;
    event.xclient.data.l[1] = (long) ic;
    event.xclient.data.l[2] = (long) sds->type;
    if (sds->type == XIMTextType) {
	if (sds->data.text) {
	    event.xclient.data.l[3] = *sds->data.text->feedback;
	} else {
	    event.xclient.data.l[3] = 0;
	}
    } else {
	event.xclient.data.l[3] = sds->data.bitmap;
    }
    XimpSendEvent2(ic, &event, 32);

#ifdef DEBUG
    dbg_printf("** Ximp-Status-DrawCM-Callback (part- 2)\n");
    dbg_printf("	sdslen=%d\n", sdslen);
#endif				/* DEBUG */

    bzero(&event, sizeof(event));
    sdc->icid = (unsigned long) ic;
    sdc->size = (char) sdslen;
    (void) bcopy(textarea, sdc->ctext, sdslen);
    XimpSendEvent2(ic, &event, 8);

    XFlush(display);
    return (0);
}

int
XimpStatusDoneSend(ic)
    FXimp_Client   *ic;
{
    XEvent          event;

#ifdef DEBUG
    dbg_printf("<<XimpStatusDoneSend>> ic=%x\n", ic);
    dbg_printf("<<XimpStatusDoneSend>> event=%x\n", &event);
#endif				/* DEBUG */

    bzero(&event, sizeof(event));
    event.xclient.data.l[0] = XIMP_STATUSDONE;
    event.xclient.data.l[1] = (long) ic;
    XimpSendEvent2(ic, &event, 32);
    XFlush(display);
    return (0);
}

int
XimpLookupStartSend(ic, lss, e)
    FXimp_Client   *ic;
    XIMLookupStartCallbackStruct *lss;
    XEvent         *e;
{
    XEvent          event;
    long            prop0[9];

#ifdef DEBUG
    dbg_printf("** Ximp-Lookup-Start-Callback\n");
    dbg_printf("	ic = %x\n", ic);
    dbg_printf("	lss = %x\n", lss);
#endif				/* DEBUG */

    bzero(&event, sizeof(event));
    event.xclient.data.l[0] = XIMP_EXTENSION;
    event.xclient.data.l[1] = (long) ic;
    event.xclient.data.l[2] = IMA_EXT_XIMP_LOOKUPCHOICES;
    event.xclient.data.l[3] = LOOKUP_CHOICES_START_REQ;
    prop0[0] = e->xkey.keycode;
    prop0[1] = e->xkey.state;
    prop0[2] = lss->WhoIsMaster;
    prop0[3] = lss->XIMPreference->choice_per_window;
    prop0[4] = lss->XIMPreference->nrows;
    prop0[5] = lss->XIMPreference->ncolumns;
    prop0[6] = lss->XIMPreference->DrawUpDirection;
    prop0[7] = lss->XIMPreference->WhoOwnsLabel;
    prop0[8] = 0;

#ifdef DEBUG
    dbg_printf("	keycode = %8.8x\n", e->xkey.keycode);
    dbg_printf("	state   = %8.8x\n", e->xkey.state);
    dbg_printf("	lss   = %8.8x\n", lss);
    dbg_printf("	WhoIdMaster = %d\n", prop0[2]);
    dbg_printf("	ChoicePerWindow = %8.8x\n", prop0[3]);
    dbg_printf("	nrows = %d\n", prop0[4]);
    dbg_printf("	ncolumns = %d\n", prop0[5]);
    dbg_printf("	DrawUpDirection = %d\n", prop0[6]);
    dbg_printf("	WhoOwnsLabel = %d\n", prop0[7]);
#endif				/* DEBUG */

    event.xclient.data.l[4] =
	XimpReplaceProp(ic, _XIMP_LOOKUP, IMA_EXT_XIMP_CHOICE_START_REQ,
		    (char *) prop0, 8, 32);

    XimpSendEvent2(ic, &event, 32);
    XFlush(display);
    return (0);
}

int
XimpLookupDrawSend(ic, lds)
    FXimp_Client   *ic;
    XIMLookupDrawCallbackStruct *lds;
{
    XEvent          event;
    char           *choice, **chptr;
    char           *text, *textadr;
    long           *feedback, *feedbackadr;
    int             i, ldslen, length;
    long            prop0[6];

#ifdef DEBUG
    dbg_printf("** Ximp-Lookup-Draw-Callback\n");
    dbg_printf("	ic = %x\n", ic);
    dbg_printf("	lds = %x\n", lds);
    dbg_printf("	lds->n_choices = %d\n", lds->n_choices);
#endif				/* DEBUG */

    bzero(&event, sizeof(event));
    event.xclient.data.l[0] = XIMP_EXTENSION;
    event.xclient.data.l[1] = (long) ic;
    event.xclient.data.l[2] = IMA_EXT_XIMP_LOOKUPCHOICES;
    event.xclient.data.l[3] = LOOKUP_CHOICES_DRAW_REQ;
    prop0[0] = lds->max_len;
    prop0[1] = lds->index_of_first_candidate;
    prop0[2] = lds->index_of_last_candidate;

    choice = (char *) malloc(lds->n_choices * 8);

    for (ldslen = 0, i = 0, chptr = (char **) choice; i < lds->n_choices; i++) {
	*chptr = (char *) malloc(64);
	bzero(*chptr, 64);
	xsf_textconv(lds->choices[i].label, *chptr, &length);
	ldslen += length;
	chptr++;
	*chptr = (char *) malloc(64);
	bzero(*chptr, 64);
	xsf_textconv(lds->choices[i].value, *chptr, &length);
	ldslen += length;
	chptr++;
	ldslen += 2;
    }

    text = (char *) malloc(ldslen);

    for (textadr = text, i = 0, chptr = (char **) choice;
	 i < lds->n_choices * 2; i++) {
	(void) strcpy(textadr, (char *) *chptr);
	textadr += strlen(*chptr);
	*textadr = 0;
	textadr++;
	chptr++;
    }

    prop0[3] = XimpReplaceProp(ic, _XIMP_LOOKUP_1, XA_COMPOUND_TEXT,
			   text, ldslen, 8);
#ifdef DEBUG
    dbg_printf("	text prop Atom = %x\n", prop0[3]);
#endif				/* DEBUG */

    for (i = 0, chptr = (char **) choice; i < lds->n_choices; i++, chptr++) {
	free(*chptr);
    }
    free(choice);
    free(text);

    feedback = (long *) malloc(lds->n_choices * 8);
    for (feedbackadr = feedback, i = 0; i < lds->n_choices; i++) {
	if (lds->choices[i].label) {
	    *feedbackadr = (long) lds->choices[i].label->feedback;
	} else {
	    *feedbackadr = 0;
	}
	feedbackadr++;

	if (lds->choices[i].value) {
	    *feedbackadr = (long) lds->choices[i].value->feedback;
	} else {
	    *feedbackadr = 0;
	}
	feedbackadr++;

    }

    prop0[4] = XimpReplaceProp(ic, _XIMP_LOOKUP_2, IMA_FEEDBACKS,
			   (char *) feedback, lds->n_choices * 2, 32);
#ifdef DEBUG
    dbg_printf("	feedback prop Atom = %x\n", prop0[4]);
#endif				/* DEBUG */

    free(feedback);

    prop0[5] = 0;
    event.xclient.data.l[4] = XimpReplaceProp(ic, _XIMP_LOOKUP_3,
					  IMA_EXT_XIMP_CHOICE_DRAW_REQ,
					  (char *) prop0, 5, 32);
#ifdef DEBUG
    dbg_printf("	prop0 prop Atom = %x\n", event.xclient.data.l[4]);
    dbg_printf("	prop0 [0]  = %x\n", prop0[0]);
    dbg_printf("	prop0 [1]  = %x\n", prop0[1]);
    dbg_printf("	prop0 [2]  = %x\n", prop0[2]);
    dbg_printf("	prop0 [3]  = %x\n", prop0[3]);
    dbg_printf("	prop0 [4]  = %x\n", prop0[4]);
#endif				/* DEBUG */

    XimpSendEvent2(ic, &event, 32);
    XFlush(display);
    return (0);
}

int
XimpLookupProcessSend(ic, e)
    FXimp_Client   *ic;
    XEvent         *e;
{
    XEvent          event;
    long            prop0[3];

#ifdef DEBUG
    dbg_printf("** Ximp-Lookup-Process-Callback\n");
    dbg_printf("	ic = %x\n", ic);
#endif				/* DEBUG */

    bzero(&event, sizeof(event));
    event.xclient.data.l[0] = XIMP_EXTENSION;
    event.xclient.data.l[1] = (long) ic;
    event.xclient.data.l[2] = IMA_EXT_XIMP_LOOKUPCHOICES;
    event.xclient.data.l[3] = LOOKUP_CHOICES_PROCESS_REQ;
    prop0[0] = e->xkey.keycode;
    prop0[1] = e->xkey.state;
    prop0[2] = 0;
    event.xclient.data.l[4] =
	XimpReplaceProp(ic, _XIMP_LOOKUP, IMA_EXT_XIMP_CHOICE_PROC_REQ,
		    (char *) prop0, 2, 32);

    XimpSendEvent2(ic, &event, 32);
    XFlush(display);
    return (0);
}

int
XimpLookupDoneSend(ic)
    FXimp_Client   *ic;
{
    XEvent          event;

#ifdef DEBUG
    dbg_printf("<<XimpLookupDoneSend>> ic=%x\n", ic);
    dbg_printf("<<XimpLookupDoneSend>> event=%x\n", &event);
#endif				/* DEBUG */

    bzero(&event, sizeof(event));
    event.xclient.data.l[0] = XIMP_EXTENSION;
    event.xclient.data.l[1] = (long) ic;
    event.xclient.data.l[2] = IMA_EXT_XIMP_LOOKUPCHOICES;
    event.xclient.data.l[3] = LOOKUP_CHOICES_DONE_REQ;
    XimpSendEvent2(ic, &event, 32);
    XFlush(display);
    return (0);
}
#endif				/* XSF_X11R5 */

Static int
XimpSendErrorEvent(window, icid, serial, error_no, detail_error_no)
    Window          window;
    unsigned long   icid, serial;
    int             error_no, detail_error_no;
{
    return XimpSendEvent(window, XIMP_ERROR, icid, serial,
			 error_no, detail_error_no);
}

int
XimpSendKeyEvent(key_event)
    XEvent         *key_event;
{
    XEvent          event;
    Window          window;

    if (current_client == root_client)
	return (0);
    window = current_client->focus_window;
    bzero(&event, sizeof(event));
    event.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = IMA_IMPROTOCOL;
    event.xclient.format = 32;
    event.xclient.data.l[0] = XIMP_KEYPRESS;
    event.xclient.data.l[1] = (long) icid;
    event.xclient.data.l[2] = key_event->xkey.keycode;
    event.xclient.data.l[3] = key_event->xkey.state;
#ifdef DEBUG
    dbg_printf("XimpSendKeyEvent:32:w=%d:%x:%x:%x:%x:%x\n",
	    window, XIMP_KEYPRESS, icid, key_event->xkey.keycode,
	    key_event->xkey.state);
#endif
    XSendEvent(display, window, False, NoEventMask, &event);
    XFlush(display);
    return (0);
}

/* #ifndef XSF_X11R5 TEMPORARY SOLUITION */
int
XimpSendBuffByLength(window, input_data, input_length)
    Window          window;
    ushort         *input_data;
    int             input_length;
{
    XEvent          event;
    int             cnt;
    int             total;
    char            buff[256];

    total = EUC_To_CompoundText(buff, input_data, input_length);
    bzero(&event, sizeof(event));
    event.type = ClientMessage;
    event.xclient.format = 8;
    event.xclient.message_type = IMA_IMPROTOCOL;
    event.xclient.window = window;

    if (total <= 15) {
	event.xclient.data.l[0] = (long) current_client;
	event.xclient.data.b[4] = total;
	strncpy(&(event.xclient.data.b[5]), buff, 15);
    } else {
	XChangeProperty(display, root_client->window, current_client->ximp_readprop_atom,
	IMA_COMPOUND_TEXT, 8, PropModeAppend, (unsigned char *) buff, total);
	event.xclient.format = 32;
	event.xclient.data.l[0] = XIMP_READPROP;
	event.xclient.data.l[1] = (long) current_client;
	event.xclient.data.l[2] = current_client->ximp_readprop_atom;
    }
    XFlush(display);
    XSendEvent(display, window, False, NoEventMask, &event);
    XFlush(display);
    return (0);
}
/* #endif				/* XSF_X11R5 */

/* #ifndef XSF_X11R5 TEMPORARY SOLUTION */
int
XimpResetSendBuffByLength(window, input_data, input_length)
    Window          window;
    ushort         *input_data;
    int             input_length;
{
    XEvent          event;
    int             cnt;
    int             total;
    Atom            atom;
    char            prop_name[32];
    char            buff[256];

    total = EUC_To_CompoundText(buff, input_data, input_length);
    bzero(&event, sizeof(event));
    event.type = ClientMessage;
    event.xclient.format = 8;
    event.xclient.message_type = IMA_IMPROTOCOL;
    event.xclient.window = window;

    sprintf(prop_name, "_XIMP_RESETDATA%d", current_client);
    atom = XInternAtom(display, prop_name, False);
    XChangeProperty(display, root_client->window, atom,
       IMA_COMPOUND_TEXT, 8, PropModeAppend, (unsigned char *) buff, total);
    XFlush(display);
    event.xclient.format = 32;
    event.xclient.data.l[0] = XIMP_RESET_RETURN;
    event.xclient.data.l[1] = (long) current_client;
    event.xclient.data.l[2] = atom;
    XSendEvent(display, window, False, NoEventMask, &event);
    XFlush(display);
    return (0);
}
/*#endif				/* XSF_X11R5 */

Static int
XimpDestroy(event)
    XEvent         *event;
{
#ifdef DEBUG
    dbg_printf("<<XimpDestroy>> icid=%x\n", icid);
#endif				/* DEBUG */

#ifdef	XSF_X11R5
    xsfIMLclose(current_client);
#endif				/* XSF_X11R5 */

    return XimpDestroyClient(current_client, 0);
}

int
XimpDestroyClient(ximp_client, flag)
   FXimp_Client   *ximp_client;
    int             flag;
{

    current_client = ximp_client;
    current_client->imp_mode = FIMP_DESTROY;
    if (flag == 0) {
	if (current_client->focus_window) {
	    XSelectInput(display, current_client->focus_window, NoEventMask);
	    current_client->frontend_key_press_mask = 0;
	}
    }
    UnsetFocusClient();
    DestroyPreeditWindow(flag);
    DestroyStatusWindow(flag);
#ifdef	XSF_X11R5
    xsfIMLclose(current_client);
#endif				/* XSF_X11R5 */
    DeleteClient(current_client);
    return (0);
}

Static int
XimpKeypress(event)
    XEvent         *event;
{
    FXimp_ICID      icid = (FXimp_ICID) event->xclient.data.l[1];
    int             keycode = event->xclient.data.l[2];
    int             state = event->xclient.data.l[3];
#ifdef DEBUG2
    dbg_printf("<<XimpKeypress>> icid=%x keycode=%x state=%x\n",
	       icid, keycode, state);
#endif				/* DEBUG2 */
    event->xkey.type = KeyPress;
    event->xkey.window = current_client->focus_window;
    event->xkey.root = root;
    event->xkey.subwindow = 0;
    event->xkey.x = 0;
    event->xkey.y = 0;
    event->xkey.x_root = 0;
    event->xkey.y_root = 0;
    event->xkey.keycode = keycode;
    event->xkey.state = state;
    event->xkey.same_screen = 1;

#ifdef	XSF_X11R5
    xsfKeyPress(current_client, event);
#else
    CurrentKeyPress(event);
#endif				/* XSF_X11R5 */

    return (0);
}

Static int
XimpBegin(event)
    XEvent         *event;
{
    Ximp_StatusPropRec *current_status;
    Window          parent;
#ifdef DEBUG
    dbg_printf("<<XimpBegin>> icid=%x\n", icid);
#endif				/* DEBUG */

    if (current_client->ext_ui_back_front == XIMP_FRONTEND) {
	XSelectInput(display, current_client->focus_window,
		     KeyPressMask | StructureNotifyMask);
	current_client->frontend_key_press_mask = 1;
    }
    current_client->imp_mode |= FIMP_BEGIN;

    current_status = current_client->status;
    if (current_client->ext_ui_statuswindow && current_status->window) {
	parent = current_status->window;
	if (parent)
	    XReparentWindow(display, current_client->status_window,
		    parent, current_status->Area.x, current_status->Area.y);
    }
    Fj_ChangeMode(0, 0, 0, 0, FK_NIHONGO_ON);
    SetFocusClient();
#ifdef	XSF_X11R5
    xsfIMLbegin(current_client, event);
#endif	/* XSF_X11R5 */
    return (0);
}

int
XimpCnversionEnd(current_client)
    FXimp_Client   *current_client;
{
    if (current_client->frontend_key_press_mask) {
	current_client->frontend_key_press_mask = 0;
	XSelectInput(display, current_client->focus_window, StructureNotifyMask);
    }
    XimpSendEvent(current_client->focus_window,
		  XIMP_CONVERSION_END, current_client, 0, 0, 0);
    current_client->imp_mode &= ~FIMP_BEGIN;
    return (0);
}

Static int
XimpEnd(event)
    XEvent         *event;
{
#ifdef DEBUG
    dbg_printf("<<XimpEnd>> icid=%x\n", icid);
#endif				/* DEBUG */

    UnsetFocusClient();

    if (current_client->frontend_key_press_mask) {
	current_client->frontend_key_press_mask = 0;
	XSelectInput(display, current_client->focus_window, StructureNotifyMask);
    }
    current_client->imp_mode &= ~FIMP_BEGIN;
    Fj_ChangeMode(0, 0, 0, 0, FK_NIHONGO_OFF);
    return (0);
}

Static int
XimpSetfocus(event)
    XEvent         *event;
{
#ifdef DEBUG
    dbg_printf("<<XimpSetfocus>> icid=%x\n", icid);
#endif				/* DEBUG */
    current_client->imp_mode |= FIMP_SETFOCUS;

#ifdef	XSF_X11R5
    SetFocusClient() ;
#endif				/* XSF_X11R5 */

    return (0);
}

Static int
XimpUnsetfocus(event)
    XEvent         *event;
{
#ifdef DEBUG
    dbg_printf("<<XimpUnsetfocus>> icid=%x\n", icid);
#endif				/* DEBUG */
    current_client->imp_mode &= ~FIMP_SETFOCUS;

#ifdef	XSF_X11R5
    UnsetFocusClient() ;
    xsfIMLunsetfocus(current_client);
#endif				/* XSF_X11R5 */

    return (0);
}

Static int
XimpMove(event)
    XEvent         *event;
{
    int             x = event->xclient.data.l[2], y = event->xclient.data.l[3];
#ifdef DEBUG2
    dbg_printf("<<XimpMove>> icid=%x x=%d y=%d\n", icid, x, y);
#endif				/* DEBUG2 */

    if (x < 0)
	x = 0;
    if (y < 0)
	y = current_client->preedit_font->max_font_ascent;

    if (current_client->style & XIMPreeditPosition) {
	current_client->preedit->SpotLocation.x = x;
	current_client->preedit->SpotLocation.y = y;
	current_client->input_flag = FINF_MODY;
	if (current_client->henkan_mode)
	    ResizePreeditArea();
    }
    return (0);
}

Static int
XimpSetvalue(event)
    XEvent         *event;
{
    unsigned long   mask = event->xclient.data.l[2];
    Window          window;
    int             rc;
    long            event_mask;
#ifdef DEBUG
    dbg_printf("<<XimpSetvalue>> icid=%x mask=%x\n", icid, mask);
#endif				/* DEBUG */

    window = current_client->window;

    if (mask & XIMP_FOCUS_WIN_MASK) {
	Window         *focus_window;
	focus_window = (Window *) GetXimpSubProperty(window, IMA_FOCUS, XA_WINDOW);
	if (!focus_window) {
	    ErrorMessage(108, _XIMP_FOCUS);
	    XimpSendErrorEvent(current_client->focus_window,
			  icid, event->xclient.serial, XIMP_BadProperty, 0);
	    return (-1);
	}
	XSelectInput(display, current_client->focus_window, NoEventMask);
	current_client->frontend_key_press_mask = 0;
	if (current_client->style & XIMPreeditPosition) {
#ifdef	USE_X11R5_LIB
	    FXimp_PreeditAreaInfo  *info = current_client->preedit_area_info ;
	    XReparentWindow(display, current_client->preedit_window,
			    *focus_window, info[0].preedit_area.x,
			    info[0].preedit_area.y);
	    XReparentWindow(display, current_client->preedit_window2,
			    *focus_window, info[1].preedit_area.x,
			    info[1].preedit_area.y);
#else
	    XReparentWindow(display, current_client->preedit_window,
			    *focus_window, current_client->preedit_area[0].x,
			    current_client->preedit_area[0].y);
	    XReparentWindow(display, current_client->preedit_window2,
			    *focus_window, current_client->preedit_area[1].x,
			    current_client->preedit_area[1].y);
#endif	/* USE_X11R5_LIB */
	}
	current_client->focus_window = *focus_window;
	if (current_client->ext_ui_back_front == XIMP_FRONTEND) {
	    event_mask = KeyPressMask | StructureNotifyMask;
	    current_client->frontend_key_press_mask = 1;
	} else
	    event_mask = StructureNotifyMask;
	XSelectInput(display, current_client->focus_window, event_mask);
    }
    if (mask & (XIMP_PRE_AREA_MASK | XIMP_PRE_FG_MASK | XIMP_PRE_BG_MASK |
		XIMP_PRE_COLORMAP_MASK | XIMP_PRE_BGPIXMAP_MASK |
		XIMP_PRE_LINESP_MASK | XIMP_PRE_CURSOR_MASK |
		XIMP_PRE_AREANEED_MASK | XIMP_PRE_SPOTL_MASK)) {
	if (current_client->style & (XIMPreeditPosition | XIMPreeditNothing)) {
	    Ximp_PreeditPropRec preedit;
	    preedit = *current_client->preedit;
	    rc = GetPreeditPropRec(window, &preedit, mask);
	    if (rc) {
		GetPropErrorSendEvent(rc, current_client->focus_window,
				      icid, event->xclient.serial);
		return (-1);
	    }
	    ChangePreeditWindow(&preedit);
	}
    }
    if (mask & (XIMP_STS_AREA_MASK | XIMP_STS_FG_MASK | XIMP_STS_BG_MASK |
		XIMP_STS_COLORMAP_MASK | XIMP_STS_BGPIXMAP_MASK |
		XIMP_STS_LINESP_MASK | XIMP_STS_CURSOR_MASK |
		XIMP_STS_AREANEED_MASK | XIMP_STS_WINDOW_MASK)) {
	if (current_client->style & (XIMStatusArea | XIMStatusNothing)) {
	    Ximp_StatusPropRec status;
	    status = *current_client->status;
	    rc = GetStatusPropRec(window, &status, mask);
	    if (rc) {
		GetPropErrorSendEvent(rc, current_client->focus_window,
				      icid, event->xclient.serial);
		return (-1);
	    }
	    ChangeStatusWindow(&status);
	}
    }
    if (mask & XIMP_PRE_FONT_MASK) {
	if (current_client->style & (XIMPreeditPosition | XIMPreeditNothing)) {
	    current_client->preedit_fontname =
		GetXimpSubProperty(window, IMA_PREEDITFONT, XA_STRING);
	    ChangePreeditFont();
	}
    }
    if (mask & XIMP_STS_FONT_MASK) {
	if (current_client->style & (XIMStatusArea | XIMStatusNothing)) {
	    current_client->status_fontname =
		GetXimpSubProperty(window, IMA_STATUSFONT, XA_STRING);
	    ChangeStatusFont();
	}
    }
    return (0);
}

Static int
XimpGetvalue(event)
    XEvent         *event;
{
    unsigned long   mask = event->xclient.data.l[2];
    Window          window;
    int             rc;
    long            event_mask;
#ifdef DEBUG
    dbg_printf("<<XimpGetvalue>> icid=%x mask=%x\n", icid, mask);
#endif				/* DEBUG */

    window = current_client->window;

    if (mask & XIMP_FOCUS_WIN_MASK) {
	SetXimpSubProperty(window, IMA_FOCUS, XA_WINDOW, 32,
			   &current_client->focus_window, 1);
    }
    if (mask & (XIMP_PRE_AREA_MASK | XIMP_PRE_FG_MASK | XIMP_PRE_BG_MASK |
		XIMP_PRE_COLORMAP_MASK | XIMP_PRE_BGPIXMAP_MASK |
		XIMP_PRE_LINESP_MASK | XIMP_PRE_CURSOR_MASK |
		XIMP_PRE_AREANEED_MASK | XIMP_PRE_SPOTL_MASK)) {
	Ximp_PreeditPropRec preedit;
	bzero(&preedit, sizeof(Ximp_PreeditPropRec));
	SetPreeditPropRec(&preedit, current_client->preedit, mask);
	SetXimpSubProperty(window, IMA_PREEDIT, IMA_PREEDIT, 32,
			   &preedit, sizeof(Ximp_PreeditPropRec) / 4);
    }
    if (mask & (XIMP_STS_AREA_MASK | XIMP_STS_FG_MASK | XIMP_STS_BG_MASK |
		XIMP_STS_COLORMAP_MASK | XIMP_STS_BGPIXMAP_MASK |
		XIMP_STS_LINESP_MASK | XIMP_STS_CURSOR_MASK |
		XIMP_STS_AREANEED_MASK | XIMP_STS_WINDOW_MASK)) {
	Ximp_StatusPropRec status;
	bzero(&status, sizeof(Ximp_StatusPropRec));
	SetStatusPropRec(&status, current_client->status, mask);
	SetXimpSubProperty(window, IMA_STATUS, IMA_STATUS, 32,
			   &status, sizeof(Ximp_StatusPropRec) / 4);
    }
    if (mask & XIMP_PRE_FONT_MASK) {
	if (current_client->style & (XIMPreeditPosition | XIMPreeditNothing)) {
	    SetXimpSubProperty(window, IMA_PREEDITFONT, XA_STRING, 8,
			       current_client->preedit_fontname,
			       strlen(current_client->preedit_fontname));
	}
    }
    if (mask & XIMP_STS_FONT_MASK) {
	if (current_client->style & (XIMStatusArea | XIMStatusNothing)) {
	    SetXimpSubProperty(window, IMA_STATUSFONT, XA_STRING, 8,
			       current_client->status_fontname,
			       strlen(current_client->status_fontname));
	}
    }
    XFlush(display);
    XimpSendEvent(current_client->focus_window,
		  XIMP_GETVALUE_RETURN, current_client, 0, 0, 0);
    return (0);
}

Static int
SetXimpSubProperty(window, atomid, type, format, data, nelements)
    Window          window;
    Atom            atomid, type;
    int             format, nelements;
    unsigned char  *data;
{
    Atom            actual_type_ret;
    int             actual_format_ret;
    unsigned long   nitems_ret;
    unsigned long   bytes_after_ret;
    unsigned char  *win_data;
    int             j;
    for (j = XIMP_TIME_OUT; j > 0; j--) {
	XGetWindowProperty(display, window, atomid, 0L, 8192L,
	     False, type, &actual_type_ret, &actual_format_ret, &nitems_ret,
			   &bytes_after_ret, &win_data);
	if (actual_format_ret == 0 || nitems_ret == 0) {
	    XChangeProperty(display, window, atomid, type, format,
			    PropModeAppend, data, nelements);
	    return (0);
	}
	XFree((char *) win_data);
	sleep(1);
    }
#ifdef DEBUG
    dbg_printf("** SetXimpSubProperty time out !! atomid=%x (%s) type=%d\n",
	       atomid, XGetAtomName(display, atomid));
#endif				/* DEBUG */
    return (-1);
}

Static int
XimpReset(event)
    XEvent         *event;
{
#ifdef DEBUG
    dbg_printf("<<XimpReset>> icid=%x\n", icid);
#endif				/* DEBUG */

#ifdef	XSF_X11R5
    xsfIMLreset(current_client);
#endif				/* XSF_X11R5 */

    return (0);
}

Static int
XimpExtension(event)
    XEvent         *event;
{
    Atom            atom = (Atom) event->xclient.data.l[2];
    int             data1 = event->xclient.data.l[3];
    int             data2 = event->xclient.data.l[4];
#ifdef DEBUG
    dbg_printf("<<XimpExtension>> icid=%x atom=%x data=%x %x\n",
	       icid, atom, data1, data2);
#endif				/* DEBUG */

    if (atom == IMA_EXT_SAMPLE_BACK_FRONT) {
	current_client->ext_ui_back_front = data1;
    } else if (atom == IMA_EXT_SAMPLE_STATUSWINDOW) {
	if ((current_client->imp_mode & FIMP_BEGIN) != 0) {
	    current_client->ext_ui_statuswindow = 1;
	}
    } else if (atom == IMA_EXT_XIMP_CONVERSION) {
	event->xclient.data.l[4] = (long)(current_client->iml_status &
					IMLSTATUS_Henkan_Mode);
	XimpSendEvent2( current_client, event, 32 );
    }
#ifdef	XSF_X11R5
    /* 91/07/17  Lookup_Choise   */
    else if (atom == IMA_EXT_XIMP_LOOKUPCHOICES) {
	switch (data1) {
	case LOOKUP_CHOICES_BEGIN:
	    (void) xsfmgr_client_ext_init(current_client);
	    current_client->lookup_choice = 1;
	    break;
	case LOOKUP_CHOICES_START_REP:
	    XimpLookupStartRep(event, data2);
	    break;
	case LOOKUP_CHOICES_PROCESS_REP:
	    XimpLookupProcessRep(event, data2);
	    break;
	}
    }
#endif				/* XSF_X11R5	 */
    return (0);
}

#ifdef XSF_X11R5
/* 91/07/17  Lookup_Choise_Start_Rep  */
static int
XimpLookupStartRep(event, data)
    XEvent         *event;
    int             data;
{
    long           *prop;
    int             rc;

#ifdef	DEBUG
    dbg_printf("*** Ximp-Lookup-Start-Reply ***\n");
    dbg_printf("    Prop-id : %8.8x,  Atomid : %8.8x\n",
	       event->xclient.data.l[4], IMA_EXT_XIMP_CHOICE_START_REP);
#endif				/* DEBUG */

    prop = (long *) GetXimpSubProperty(current_client->focus_window,
		   event->xclient.data.l[4], IMA_EXT_XIMP_CHOICE_START_REP);

    if (prop == 0)
	return (-1);

    xsfIMLlookstart(current_client, event, prop);

    XFree(prop);
    return (0);
}

/* 91/07/17  Lookup_Choise_Process_Rep  */
static int
XimpLookupProcessRep(event, data)
    XEvent         *event;
    int             data;
{

#ifdef	DEBUG
    dbg_printf("*** Ximp-Lookup-Process-Reply ***\n");
#endif				/* DEBUG */

    xsfIMLlookprocess(current_client, event, data);

#ifdef	DEBUG
    dbg_printf("*** Ximp-Lookup-Process-Reply(return) ***\n");
#endif				/* DEBUG */
    return (0);
}

/* 91/07/19  Preedit_Start_Return  */
static int
XimpPreeditStartRet(event)
    XEvent         *event;
{
    int             data = event->xclient.data.l[2];

    xsfIMLpreeditstartret(current_client, event, data);

    return (0);
}

/* 91/07/19  Preedit_Caret_Return  */
static int
XimpPreeditCaretRet(event)
    XEvent         *event;
{
    int             data = event->xclient.data.l[2];

    xsfIMLpreeditcaretret(current_client, event, data);

    return (0);
}
#endif				/* XSF_X11R5 */
