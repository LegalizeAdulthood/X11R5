/* @(#)Ximp.c	2.3 91/07/31 15:23:45 FUJITSU LIMITED. */
/******************************************************************

              Copyright 1991, by FUJITSU LIMITED

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of FUJITSU LIMITED
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.
FUJITSU LIMITED makes no representations about the suitability of
this software for any purpose.  It is provided "as is" without
express or implied warranty.

FUJITSU LIMITED DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJITSU LIMITED BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

  Author: Takashi Fujiwara     FUJITSU LIMITED 
                               fujiwara@a80.tech.yk.fujitsu.co.jp
          Kiyoaki oya          FUJITSU LIMITED

******************************************************************/

/*  Xfeoak : Input Server (kana-kanji covertion front end)	*/
/*         : Version 2.0					*/

#ifndef lint
static char sccs_id[] = "@(#)Ximp.c	2.3 91/07/31 15:23:45 FUJITSU LIMITED.";
#endif

#include <stdio.h>
#include "Xfeoak.h"
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "TransTbl.h"

Static Atom	selection_atom;
extern unsigned char	vendor_name[];
extern unsigned char	server_name[];
extern unsigned char	server_version[];

#define  XIMP_E_MAX_USER	101
#define  XIMP_TIME_OUT		5

#define  MAX_STYLE		3
Static XIMStyle  style[MAX_STYLE] = {
	XIMPreeditPosition | XIMStatusNothing,
	XIMPreeditNothing  | XIMStatusNothing,
	XIMPreeditPosition | XIMStatusArea,
	};

#define  MAX_KEY     6
#define  MAX_KEY1    3
Static int	max_key = MAX_KEY;
Static unsigned long   C_key[MAX_KEY]   =
	{ 0x00000000,	(ControlMask|ShiftMask), XK_Henkan_Mode,
          ControlMask,	(ControlMask|ShiftMask), XK_space,       /* CTRL space */
        };

Atom	IMA_IMPROTOCOL;
Static Atom	IMA_VERSION,	IMA_FOCUS;
Static Atom	IMA_PREEDIT,	IMA_PREEDITFONT;
Static Atom	IMA_STATUS,	IMA_STATUSFONT;
Static Atom	IMA_CTEXT,	IMA_COMPOUND_TEXT;
Static Atom	IMA_EXT_XIMP_STATUSWINDOW;
Static Atom	IMA_EXT_XIMP_BACK_FRONT;

Static FXimp_ICID	icid;

int
IsMode(keysym, mod)
    KeySym        keysym;
    unsigned int  mod;
    {
    int   i;

    for(i=0; i<max_key; i=i+3)
        if( (mod & C_key[i+1]) == C_key[i] && keysym == C_key[i+2])
               return(1);
    return(0);
    }

CreateXimpAtom()
    {
    Window	imserver_window;
    Atom 	id;
    Atom	type;
    Atom	selection_sub_atom;
#define EXTENSIONS_LIST_SIZE	2
    Atom	extensions_list[EXTENSIONS_LIST_SIZE];
    long	ext_ui_back_front;
    static char	IMsuport_sub_name[16]  = "";
    static char	IMsuport_full_name[32] = "";
    static char	suport_locale_name[] = "ja_JP";

    if( keybord_type == FKEY_NONE )
	max_key = MAX_KEY;
    else
	max_key = MAX_KEY1;

    sprintf( IMsuport_sub_name, "%s%s", _XIMP_BASE, suport_locale_name );
    sprintf( IMsuport_full_name, "%s%s@%s.%d", _XIMP_BASE, suport_locale_name,
					server_name, screen );
    imserver_window = root_client->window;
    if(selection_atom = XInternAtom( display, IMsuport_full_name, True)) {
        if(XGetSelectionOwner( display, selection_atom)) {
             ErrorMessage( 002 );
             exit(0);
             }
        }
    else 
        selection_atom = XInternAtom( display, IMsuport_full_name, False);
    XSetSelectionOwner( display, selection_atom, imserver_window, 0L);

    if(selection_sub_atom = XInternAtom( display, IMsuport_sub_name, True)) {
        if(XGetSelectionOwner( display, selection_sub_atom)) {
             WarningMessage( 002 );
             }
        }
    else 
        selection_sub_atom = XInternAtom( display, IMsuport_sub_name, False);
    XSetSelectionOwner( display, selection_sub_atom, imserver_window, 0L);

#ifdef DEBUG
    XChangeProperty( display, root, selection_atom, XA_STRING, 8, PropModeReplace,
                     server_name, strlen(server_name));
#endif /*DEBUG*/

    id   = XInternAtom( display, _XIMP_VERSION, False);
    IMA_VERSION = id;
    XChangeProperty( display, imserver_window, id, XA_STRING, 8, PropModeReplace,
         (unsigned char *)XIMP_PROTOCOL_VERSION, strlen(XIMP_PROTOCOL_VERSION));

    type = XInternAtom( display, _XIMP_STYLE, False);
    id   = type;
    XChangeProperty( display, imserver_window, id, type, 32, PropModeReplace,
                     (unsigned char *)style, MAX_STYLE);
     
    type = XInternAtom( display, _XIMP_KEYS, False);
    id   = type;
    XChangeProperty( display, imserver_window, id, type, 32, PropModeReplace,
                     (unsigned char *)C_key, max_key);
     
    id   = XInternAtom( display, _XIMP_SERVERNAME, False);
    type = XA_STRING;
    XChangeProperty( display, imserver_window, id, type, 8, PropModeReplace,
                     server_name, strlen(server_name));
     
    id   = XInternAtom( display, _XIMP_SERVERVERSION, False);
    type = XA_STRING;
    XChangeProperty( display, imserver_window, id, type, 8, PropModeReplace,
                     server_version, strlen(server_version));

    id   = XInternAtom( display, _XIMP_VENDORNAME, False);
    type = XA_STRING;
    XChangeProperty( display, imserver_window, id, type, 8, PropModeReplace,
                     vendor_name, strlen(vendor_name));
     
    IMA_IMPROTOCOL  = XInternAtom( display, _XIMP_PROTOCOL, 	False);
    IMA_FOCUS	    = XInternAtom( display, _XIMP_FOCUS,	False);
    IMA_PREEDIT	    = XInternAtom( display, _XIMP_PREEDIT,	False);
    IMA_STATUS	    = XInternAtom( display, _XIMP_STATUS,	False);
    IMA_PREEDITFONT = XInternAtom( display, _XIMP_PREEDITFONT,	False);
    IMA_STATUSFONT  = XInternAtom( display, _XIMP_STATUSFONT,	False);
    IMA_CTEXT	    = XInternAtom( display, _XIMP_CTEXT,	 	False);
    IMA_COMPOUND_TEXT = XInternAtom( display, "COMPOUND_TEXT", 	False);
    IMA_EXT_XIMP_STATUSWINDOW
		    = XInternAtom( display, "_XIMP_EXT_XIMP_STATUSWINDOW",False);
    IMA_EXT_XIMP_BACK_FRONT
		    = XInternAtom( display, "_XIMP_EXT_XIMP_BACK_FRONT",False);

    id = IMA_EXT_XIMP_BACK_FRONT;
    type = id;
    ext_ui_back_front = XIMP_FRONTEND;
    XChangeProperty( display, imserver_window, id, type, 32, PropModeReplace,
                     (unsigned char *)&ext_ui_back_front, 1);

    extensions_list[0] = IMA_EXT_XIMP_BACK_FRONT;
    extensions_list[1] = IMA_EXT_XIMP_STATUSWINDOW;

    id   = XInternAtom( display, _XIMP_EXTENSIONS, False);
    type = id;
    XChangeProperty( display, imserver_window, id, type, 32, PropModeReplace,
                     (unsigned char *)extensions_list, EXTENSIONS_LIST_SIZE);
    }


XimpClientMessage(event)
    XEvent	*event;
    {
    FXimp_Client	*ximp_client;
    switch( event->xclient.data.l[0] ) {
    case XIMP_CREATE: 	  XimpCreate( event);	break;

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
	{
        icid = (FXimp_ICID)event->xclient.data.l[1];
#ifdef DEBUG
        dbg_printf("<<Ximp..>> icid=%x\n", icid );
#endif DEBUG
        ximp_client = FindClientByICID( icid);
        if( !ximp_client ) {
	    int		errno = 0;
	    switch( event->xclient.data.l[0] ) {
	    case XIMP_KEYPRESS:	  errno = 185;	break;
	    case XIMP_DESTROY:	  errno = 111;	break;
	    case XIMP_BEGIN:	  errno = 121;	break;
	    case XIMP_END:	  errno = 131;	break;
	    case XIMP_SETFOCUS:	  errno = 141;	break;
	    case XIMP_UNSETFOCUS: errno = 151;	break;
	    case XIMP_MOVE:	  errno = 161;	break;
	    case XIMP_RESET:	  errno = 181;	break;
	    case XIMP_SETVALUE:	  errno = 186;	break;
	    case XIMP_GETVALUE:	  errno = 187;	break;
	    case XIMP_EXTENSION:  errno = 190;	break;
		}
	    ErrorMessage( errno );
	    return(-1);
            }
        current_client = ximp_client;
        switch( event->xclient.data.l[0] ) {
        case XIMP_KEYPRESS:	XimpKeypress( event);	break;
        case XIMP_DESTROY:   	XimpDestroy( event);	break;
        case XIMP_BEGIN:   	XimpBegin( event);	break;
        case XIMP_END: 	  	XimpEnd( event);	break;
        case XIMP_SETFOCUS:   	XimpSetfocus( event); 	break;
        case XIMP_UNSETFOCUS: 	XimpUnsetfocus( event); break;
        case XIMP_MOVE:	  	XimpMove( event);	break;
        case XIMP_RESET:	XimpReset( event);	break;
        case XIMP_SETVALUE:	XimpSetvalue( event);	break;
        case XIMP_GETVALUE:	XimpGetvalue( event);	break;
        case XIMP_EXTENSION:  	XimpExtension( event);	break;
            }
	break;
        }

    default:
	ErrorMessage( 100 );
        icid = (FXimp_ICID)event->xclient.data.l[1];
        ximp_client = FindClientByICID( icid);
        if( !ximp_client )
	    XimpSendErrorEvent( ximp_client->focus_window, 0,
			event->xclient.serial, XIMP_BadProtocol, 0);
        break;
	}
    }

Static int
XimpCreate( event)
    XEvent	*event;
    {
    Window	window	   = event->xclient.data.l[1];
    XIMStyle	inputStyle = event->xclient.data.l[2];
    unsigned long mask	   = event->xclient.data.l[3];
    FXimp_Client *ximp_client;
    char	*GetXimpSubProperty();
    Window     	*focus_window;
    int		rc;
    char	*ximp_version, prop_name[32];
    Ximp_StatusPropRec  status,  *current_status;
    Ximp_PreeditPropRec preedit, *current_preedit;

#ifdef DEBUG
    dbg_printf("<<XimpCreate>> window=%x, inputStyle=%x mask=%x\n",
				window, inputStyle, mask );
#endif DEBUG

    if( inputStyle == 0 ) {
	ErrorMessage( 101 );
	XimpSendErrorEvent( window, 0, event->xclient.serial, XIMP_BadStyle, 0);
	return(-1);
	}

    ximp_client = NewClient( window);
    if( !ximp_client ) {
	ErrorMessage( 103 );
	XimpSendErrorEvent( window, 0, event->xclient.serial,
				XIMP_BadAlloc, XIMP_E_MAX_USER);
	return(-1);
        }

    current_client = ximp_client;
    current_client->prot_type = FPRT_XIM;
    current_client->imp_mode = FIMP_CREATE;
    Fj_InitialMode();
    current_client->style = inputStyle;

    sprintf( prop_name, "_XIMP_CTEXT_%d", current_client );
    current_client->ximp_readprop_atom = XInternAtom( display, prop_name, False);

    ximp_version = GetXimpSubProperty( window, IMA_VERSION, XA_STRING) ;
#ifdef DEBUG
    dbg_printf(" Client XIMP_VERSION : (%s)\n", ximp_version );
    if( strcmp( XIMP_PROTOCOL_VERSION, ximp_version) ) 
	dbg_printf(" ** unliked XIMP_VERSION : (%s)\n", XIMP_PROTOCOL_VERSION );
#endif DEBUG

    if( mask & XIMP_FOCUS_WIN_MASK ) {
        focus_window    = (Window *)GetXimpSubProperty( window,
					IMA_FOCUS, XA_WINDOW) ;
        if( focus_window ) {
            current_client->focus_window = *focus_window;
            XFree( (char *)focus_window);
	    }
        else {
	    ErrorMessage( 108, _XIMP_FOCUS);
	    XimpSendErrorEvent( current_client->focus_window, 0,
		event->xclient.serial, XIMP_BadFocusWindow, 0);
            DeleteClient( current_client );
	    return(-1);
	    }
	}
    else {
	ErrorMessage( 108, _XIMP_FOCUS);
	XimpSendErrorEvent( current_client->focus_window, 0,
		event->xclient.serial, XIMP_BadFocusWindow, 0);
        DeleteClient( current_client );
	return(-1);
	}

    current_preedit = current_client->preedit;
    *current_preedit = *root_client->preedit;
    if( current_client->style & (XIMPreeditPosition | XIMPreeditNothing) ) {
	if( mask & ( XIMP_PRE_AREA_MASK | XIMP_PRE_FG_MASK | XIMP_PRE_BG_MASK |
		     XIMP_PRE_COLORMAP_MASK | XIMP_PRE_BGPIXMAP_MASK |
		     XIMP_PRE_LINESP_MASK | XIMP_PRE_CURSOR_MASK | 
		     XIMP_PRE_AREANEED_MASK | XIMP_PRE_SPOTL_MASK ) ) {
	    rc = GetPreeditPropRec( window, current_preedit, mask );
            if( rc != 0 ) {
		GetPropErrorSendEvent( rc, current_client->focus_window,
			0, event->xclient.serial );
		goto	error_imp_create;
	        }
	    }
	else {
	    if( current_client->style & XIMPreeditNothing )
	        current_client->window_type |= FC_SAME_ROOT_PREEDIT_WINDOW;
	    current_client->window_type     |= FC_SAME_ROOT_PREEDIT_GC;
	    }
	
	if( mask & XIMP_PRE_FONT_MASK ) {
            current_client->preedit_fontname =
		GetXimpSubProperty( window, IMA_PREEDITFONT,  XA_STRING);
	    current_client->window_type &= ~FC_SAME_ROOT_PREEDIT_GC;
	    }
	else
	    current_client->window_type |= FC_SAME_ROOT_PREEDIT_FONT;
	}
    
    current_status = current_client->status;
    *current_status = *root_client->status;
    if( current_client->style & (XIMStatusArea | XIMStatusNothing) ) {
	if( mask & ( XIMP_STS_AREA_MASK | XIMP_STS_FG_MASK | XIMP_STS_BG_MASK |
		     XIMP_STS_COLORMAP_MASK | XIMP_STS_BGPIXMAP_MASK |
		     XIMP_STS_LINESP_MASK | XIMP_STS_CURSOR_MASK |
		     XIMP_STS_AREANEED_MASK | XIMP_STS_WINDOW_MASK ) ) {
	    rc = GetStatusPropRec( window, current_status, mask );
            if( rc != 0 ) {
		GetPropErrorSendEvent( rc, current_client->focus_window,
			0, event->xclient.serial );
		goto	error_imp_create;
	        }
	    }
	else {
	    if( current_client->style & XIMStatusNothing )
	        current_client->window_type |= FC_SAME_ROOT_STATUS_WINDOW;
	    current_client->window_type     |= FC_SAME_ROOT_STATUS_GC;
	    }

	if( mask & XIMP_STS_FONT_MASK ) {
            current_client->status_fontname =
		GetXimpSubProperty( window, IMA_STATUSFONT,  XA_STRING);
	    current_client->window_type &= ~FC_SAME_ROOT_STATUS_GC;
	    }
	else
	    current_client->window_type |= FC_SAME_ROOT_STATUS_FONT;
	}
    
    current_preedit_font = current_client->preedit_font;
    rc = CreateStatusWindow();
    if( rc != 0 ) goto	error_imp_create;
    rc = CreatePreeditWindow();
    if( rc != 0 ) goto	error_imp_create;

    XSelectInput( display, current_client->focus_window, StructureNotifyMask);
    XimpSendEvent( current_client->focus_window,
		XIMP_CREATE_RETURN, current_client, 0, 0, 0 );
    return(0);

error_imp_create:
    ErrorMessage( 101 );
    DeleteClient( current_client );
    return(-1);
    }

Static int
GetPreeditPropRec( window, current_preedit, mask )
    Window		window;
    Ximp_PreeditPropRec	*current_preedit;
    unsigned long	mask;
    {
    Ximp_PreeditPropRec	preedit;
    int		rc;
    rc = GetXimpProperty( window, IMA_PREEDIT, &preedit,
			sizeof(Ximp_PreeditPropRec),_XIMP_PREEDIT);
    if( rc != 0 )
	return( rc );
    return SetPreeditPropRec( current_preedit, &preedit, mask );
    }

Static int
SetPreeditPropRec( current_preedit, preedit, mask )
    Ximp_PreeditPropRec	*current_preedit, *preedit;
    unsigned long	mask;
    {
    if( mask & XIMP_PRE_AREA_MASK ) 
	current_preedit->Area		= preedit->Area;
    if( mask & XIMP_PRE_FG_MASK ) 
	current_preedit->Foreground 	= preedit->Foreground;
    if( mask & XIMP_PRE_BG_MASK ) 
	current_preedit->Background 	= preedit->Background;
    if( mask & XIMP_PRE_COLORMAP_MASK ) 
	current_preedit->Colormap 	= preedit->Colormap;
    if( mask & XIMP_PRE_BGPIXMAP_MASK ) 
	current_preedit->Bg_Pixmap 	= preedit->Bg_Pixmap;
    if( mask & XIMP_PRE_LINESP_MASK ) 
	current_preedit->LineSpacing 	= preedit->LineSpacing;
    if( mask & XIMP_PRE_CURSOR_MASK ) 
	current_preedit->Cursor 	= preedit->Cursor;
    if( mask & XIMP_PRE_AREANEED_MASK ) 
	current_preedit->AreaNeeded 	= preedit->AreaNeeded;
    if( mask & XIMP_PRE_SPOTL_MASK ) 
	current_preedit->SpotLocation 	= preedit->SpotLocation;
#ifdef DEBUG
    dbg_PreeditDump( current_preedit );
#endif /*DEBUG*/
    return(0);
    }

Static int
GetStatusPropRec( window, current_status, mask )
    Window		window;
    Ximp_StatusPropRec	*current_status;
    unsigned long	mask;
    {
    Ximp_StatusPropRec	status;
    int		rc;
    rc = GetXimpProperty( window, IMA_STATUS, &status,
			sizeof(Ximp_StatusPropRec),_XIMP_STATUS);
    if( rc != 0 ) 
	return( rc );
    return SetStatusPropRec( current_status, &status, mask );
    }

Static int
SetStatusPropRec( current_status, status, mask )
    Ximp_StatusPropRec	*current_status, *status;
    unsigned long	mask;
    {
    if( mask & XIMP_STS_AREA_MASK ) 
	current_status->Area		= status->Area;
    if( mask & XIMP_STS_FG_MASK ) 
	current_status->Foreground	= status->Foreground;
    if( mask & XIMP_STS_BG_MASK ) 
	current_status->Background	= status->Background;
    if( mask & XIMP_STS_COLORMAP_MASK ) 
	current_status->Colormap	= status->Colormap;
    if( mask & XIMP_STS_BGPIXMAP_MASK ) 
	current_status->Bg_Pixmap	= status->Bg_Pixmap;
    if( mask & XIMP_STS_LINESP_MASK ) 
	current_status->LineSpacing	= status->LineSpacing;
    if( mask & XIMP_STS_CURSOR_MASK ) 
	current_status->Cursor		= status->Cursor;
    if( mask & XIMP_STS_AREANEED_MASK ) 
	current_status->AreaNeeded	= status->AreaNeeded;
#ifdef DEBUG
    dbg_StatusDump( current_status );
#endif /*DEBUG*/
    return(0);
    }

Static char *
GetXimpSubProperty( window,atomid,type)
    Window  	window;
    Atom    	atomid;
    Atom    	type;
    {
    Atom            actual_type_ret;
    int		    actual_format_ret;
    unsigned long   nitems_ret;
    unsigned long   bytes_after_ret;
    unsigned char   *win_data;

    if(atomid == 0) 
	return((char *)0);

    XGetWindowProperty( display, window, atomid, 0L, 8192L,
		True, type, &actual_type_ret, &actual_format_ret, &nitems_ret,
		&bytes_after_ret, &win_data);
    if( actual_format_ret == 0 || nitems_ret == 0) {
#ifdef DEBUG
	dbg_printf("XGetWindowProperty error : atomid=%x (%s) type=%d actual_format_ret=%d nitems_ret=%d\n",
	atomid, XGetAtomName(display,atomid), type, actual_format_ret, nitems_ret );
#endif DEBUG
	return((char *)0);
	}

#ifdef DEBUG
    if( atomid == IMA_FOCUS ) {
	int *data = (int *)win_data;
	dbg_printf("%s = %x\n", _XIMP_FOCUS, *data );
	}
    else
    if( atomid == IMA_PREEDITFONT
    ||  atomid == IMA_STATUSFONT ) {
	char *s = (atomid == IMA_PREEDITFONT) ? _XIMP_PREEDITFONT
					      : _XIMP_STATUSFONT;
	dbg_printf("%s = %s\n", s, win_data );
	}
#endif DEBUG

    return((char *)win_data);
    }

Static int
GetXimpProperty( window, atomid, win_area, size, type_name)
     Window  window; 
     Atom    atomid;
     char    *win_area;
     int     size;
     char    *type_name;
     {
     Atom            actual_type_ret;
     int	     actual_format_ret;
     unsigned long   nitems_ret;
     unsigned long   bytes_after_ret;
     unsigned char   *win_data;

     if(atomid) {
	 XGetWindowProperty( display, window, atomid, 0L, 1000000L,
		True, atomid, &actual_type_ret, &actual_format_ret, &nitems_ret,
		&bytes_after_ret, &win_data);
        if(actual_format_ret == 0) {
	    ErrorMessage( 108, type_name );
	    return( -2);
	    }
	if((nitems_ret*4) != size) {
	    ErrorMessage( 109, type_name, nitems_ret*4 );
#ifdef DEBUG
	    dbg_printf("%s: actual_format_ret=%d actual_type_ret=%d nitems_ret=%d bytes_after_ret=%d\n",
	      type_name, actual_format_ret, actual_type_ret, nitems_ret, bytes_after_ret );
	    if( nitems_ret > 0 ) {
	    	int di, *data=(int *)win_data;
	    	for(di=0; di<nitems_ret; di++ )
		    dbg_printf(" %x", *data++ );
		dbg_printf("\n");
		}
#endif DEBUG
            XFree( (char *)win_data);
	    return( -3);
	    }
	 bcopy( win_data, win_area, size);
         XFree( (char *)win_data);
	 }
    else
	return( -1);

#ifdef DEBUG
    if( atomid == IMA_PREEDIT ) {
	dbg_PreeditDump( (Ximp_PreeditPropRec *)win_area );
	}
    else
    if( atomid == IMA_STATUS ) {
	dbg_StatusDump( (Ximp_StatusPropRec *)win_area );
	}
#endif DEBUG

    return( 0 );
    }

#ifdef DEBUG
dbg_PreeditDump( preedit )
    Ximp_PreeditPropRec  *preedit;
    {
    dbg_printf("  Preedit : x=%d y=%d w=%d h=%d fg=%x bg=%x colo=%x bgpix=%x LS=%d curs=%x a.w=%d a.h=%d sl.x=%d sl.y=%d\n",
    	preedit->Area.x, preedit->Area.y,
    	preedit->Area.width, preedit->Area.height,
    	preedit->Foreground, preedit->Background,
    	preedit->Colormap, preedit->Bg_Pixmap,
    	preedit->LineSpacing, preedit->Cursor,
    	preedit->AreaNeeded.width, preedit->AreaNeeded.height,
    	preedit->SpotLocation.x, preedit->SpotLocation.y );
    }

dbg_StatusDump( status )
    Ximp_StatusPropRec  *status;
    {
    dbg_printf("  Status : x=%d y=%d w=%d h=%d fg=%x bg=%x colo=%x bgpix=%x LS=%d curs=%x a.w=%d a.h=%d wid=%x\n",
    	status->Area.x, status->Area.y,
    	status->Area.width, status->Area.height,
    	status->Foreground, status->Background,
    	status->Colormap, status->Bg_Pixmap,
    	status->LineSpacing, status->Cursor,
    	status->AreaNeeded.width, status->AreaNeeded.height,
    	status->window );
    }
#endif DEBUG

Static int
GetPropErrorSendEvent( rc, focus_window, icid, serial )
    long	rc;
    Window	focus_window;
    unsigned long icid, serial;
    {
    if( rc == -2 )
	XimpSendErrorEvent( focus_window, icid, serial, XIMP_BadProperty, 0);
    if( rc == -3 )
	XimpSendErrorEvent( focus_window, icid, serial, XIMP_BadPropertyType, 0);
    return( 0 );
    }

Static int
XimpSendEvent( window, imp_func, icid, data1, data2, data3)
    Window  window;
    unsigned long   imp_func, icid, data1, data2, data3;
    {
    XEvent  event;

    bzero(&event,sizeof(event));
    event.type = ClientMessage;
    event.xclient.format = 32;
    event.xclient.window = window;
    event.xclient.message_type = IMA_IMPROTOCOL;
    event.xclient.data.l[0] = imp_func;
    event.xclient.data.l[1] = icid;
    event.xclient.data.l[2] = data1;
    event.xclient.data.l[3] = data2;
    event.xclient.data.l[4] = data3;
    XSendEvent( display, window, False, NoEventMask, &event);
    XFlush( display);
    return(0);
    }

Static int
XimpSendErrorEvent( window, icid, serial, error_no, detail_error_no)
    Window  window;
    unsigned long   icid, serial;
    int     error_no, detail_error_no;
    {
    return XimpSendEvent( window, XIMP_ERROR, icid, serial,
					error_no, detail_error_no );
    }

int
XimpSendKeyEvent( key_event )
    XEvent  *key_event;
    {
    XEvent  event;
    Window  window;

    if( current_client == root_client )
	return( 0 );
    window = current_client->focus_window;
    bzero( &event, sizeof(event));
    event.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = IMA_IMPROTOCOL;
    event.xclient.format = 32;
    event.xclient.data.l[0] = XIMP_KEYPRESS;
    event.xclient.data.l[1] = (long)current_client;
    event.xclient.data.l[2] = key_event->xkey.keycode;
    event.xclient.data.l[3] = key_event->xkey.state;
    XSendEvent( display, window, False, NoEventMask, &event);
    XFlush( display);
    return( 0);
    }

int
XimpSendBuffByLength( window, input_data, input_length )
    Window	window;
    ushort	*input_data;
    int		input_length;
    {
    XEvent 	event;
    int		cnt;
    int		total;
    char	buff[256];
	
    total = EUC_To_CompoundText( buff, input_data, input_length );
    bzero( &event, sizeof(event));
    event.type = ClientMessage;
    event.xclient.format = 8;
    event.xclient.message_type = IMA_IMPROTOCOL;
    event.xclient.window = window;

    if(total <= 15) {
    	event.xclient.data.l[0] = (long)current_client;
	event.xclient.data.b[4] = total;
	strncpy(&(event.xclient.data.b[5]), buff, 15);
	}
    else {
        XChangeProperty( display, root_client->window, current_client->ximp_readprop_atom,
	  IMA_COMPOUND_TEXT, 8, PropModeAppend, (unsigned char *)buff, total);
        event.xclient.format = 32;
    	event.xclient.data.l[0] = XIMP_READPROP;
    	event.xclient.data.l[1] = (long)current_client;
    	event.xclient.data.l[2] = current_client->ximp_readprop_atom;
	}
    XFlush( display);
    XSendEvent( display, window, False, NoEventMask, &event);
    XFlush( display);
    return( 0);
    }

int
XimpResetSendBuffByLength( window, input_data, input_length )
    Window	window;
    ushort	*input_data;
    int		input_length;
    {
    XEvent 	event;
    int		cnt;
    int		total;
    Atom	atom;
    char	prop_name[32];
    char	buff[256];
	
    total = EUC_To_CompoundText( buff, input_data, input_length );
    bzero( &event, sizeof(event));
    event.type = ClientMessage;
    event.xclient.format = 8;
    event.xclient.message_type = IMA_IMPROTOCOL;
    event.xclient.window = window;

    sprintf( prop_name, "_XIMP_RESETDATA%d", current_client );
    atom = XInternAtom( display, prop_name, False);
    XChangeProperty( display, root_client->window, atom,
	  IMA_COMPOUND_TEXT, 8, PropModeAppend, (unsigned char *)buff, total);
    XFlush( display);
    event.xclient.format = 32;
    event.xclient.data.l[0] = XIMP_RESET_RETURN;
    event.xclient.data.l[1] = (long)current_client;
    event.xclient.data.l[2] = atom;
    XSendEvent( display, window, False, NoEventMask, &event);
    XFlush( display);
    return( 0);
    }

Static int
XimpDestroy( event)
    XEvent	*event;
    {
#ifdef DEBUG
    dbg_printf("<<XimpDestroy>> icid=%x\n", icid );
#endif DEBUG

    return XimpDestroyClient( current_client, 0 );
    }

int
XimpDestroyClient( ximp_client,flag)
    FXimp_Client	*ximp_client;
    int		flag;
    {

    current_client = ximp_client;
    current_client->imp_mode = FIMP_DESTROY;
    if( flag == 0 ) {
        if( current_client->focus_window ) {
            XSelectInput( display, current_client->focus_window, NoEventMask);
	    current_client->frontend_key_press_mask = 0;
	    }
	}
    UnsetFocusClient();
    DestroyPreeditWindow( flag);
    DestroyStatusWindow( flag);
    DeleteClient( current_client );
    return(0);
    }

Static int
XimpKeypress( event)
    XEvent	*event;
    {
    FXimp_ICID    icid	 = (FXimp_ICID)event->xclient.data.l[1];
    int		  keycode= event->xclient.data.l[2];
    int		  state	 = event->xclient.data.l[3];
#ifdef DEBUG2
    dbg_printf("<<XimpKeypress>> icid=%x keycode=%x state=%x\n",
				 icid,   keycode,   state );
#endif DEBUG
    event->xkey.type	= KeyPress; 
    event->xkey.window	= current_client->focus_window;
    event->xkey.root	= root;
    event->xkey.subwindow = 0;
    event->xkey.x  	= 0;
    event->xkey.y	= 0;
    event->xkey.x_root  = 0;
    event->xkey.y_root  = 0;
    event->xkey.keycode = keycode;
    event->xkey.state   = state;
    event->xkey.same_screen = 1;
    CurrentKeyPress( event);
    return(0);
    }

Static int
XimpBegin( event )
    XEvent	*event;
    {
    Ximp_StatusPropRec	*current_status;
    Window	  	parent;
#ifdef DEBUG
    dbg_printf("<<XimpBegin>> icid=%x\n", icid );
#endif DEBUG

    if( current_client->ext_ui_back_front == XIMP_FRONTEND ) {
        XSelectInput( display, current_client->focus_window,
			KeyPressMask | StructureNotifyMask);
	current_client->frontend_key_press_mask = 1;
	}
    current_client->imp_mode |= FIMP_BEGIN;

    current_status = current_client->status;
    if( current_client->ext_ui_statuswindow && current_status->window ) {
	parent = current_status->window;
	if( parent )
	    XReparentWindow( display, current_client->status_window,
		parent, current_status->Area.x, current_status->Area.y );
	}
    Fj_ChangeMode( 0,0,0,0,FK_NIHONGO_ON);
    SetFocusClient();
    return(0);
    }

int
XimpCnversionEnd( current_client )
    FXimp_Client *current_client;
    {
    if( current_client->frontend_key_press_mask ) {
	current_client->frontend_key_press_mask = 0;
        XSelectInput( display, current_client->focus_window, StructureNotifyMask);
	}
    XimpSendEvent( current_client->focus_window,
		XIMP_CONVERSION_END, current_client, 0, 0, 0 );
    current_client->imp_mode &= ~FIMP_BEGIN;
    return(0);
    }

Static int
XimpEnd( event )
    XEvent	*event;
    {
#ifdef DEBUG
    dbg_printf("<<XimpEnd>> icid=%x\n", icid );
#endif DEBUG

    UnsetFocusClient();

    if( current_client->frontend_key_press_mask ) {
	current_client->frontend_key_press_mask = 0;
        XSelectInput( display, current_client->focus_window, StructureNotifyMask);
	}
    current_client->imp_mode &= ~FIMP_BEGIN;
    Fj_ChangeMode(0,0,0,0,FK_NIHONGO_OFF);
    return(0);
    }

Static int
XimpSetfocus( event )
    XEvent	*event;
    {
#ifdef DEBUG
    dbg_printf("<<XimpSetfocus>> icid=%x\n", icid );
#endif DEBUG
    current_client->imp_mode |= FIMP_SETFOCUS;
    SetFocusClient();
    return(0);
    }

Static int
XimpUnsetfocus( event)
    XEvent	*event;
    {
#ifdef DEBUG
    dbg_printf("<<XimpUnsetfocus>> icid=%x\n", icid );
#endif DEBUG
    current_client->imp_mode &= ~FIMP_SETFOCUS;
    UnsetFocusClient();
    return(0);
    }

Static int
XimpMove( event)
    XEvent	*event;
    {
    int     	x	= event->xclient.data.l[2],
		y	= event->xclient.data.l[3];
#ifdef DEBUG2
    dbg_printf("<<XimpMove>> icid=%x x=%d y=%d\n", icid, x, y );
#endif DEBUG2

    if( x < 0 ) x = 0;
    if( y < 0 ) y = current_client->preedit_font->max_font_ascent;

    if(current_client->style & XIMPreeditPosition) {
	current_client->preedit->SpotLocation.x = x;
	current_client->preedit->SpotLocation.y = y;
	current_client->input_flag = FINF_MODY;
	if( current_client->henkan_mode )
	    ResizePreeditArea();
        }
    return(0);
    }

Static int
XimpSetvalue( event)
    XEvent	*event;
    {
    unsigned long mask	 = event->xclient.data.l[2];
    Window	window;
    int		rc;
    long	event_mask;
#ifdef DEBUG
    dbg_printf("<<XimpSetvalue>> icid=%x mask=%x\n", icid, mask );
#endif DEBUG

    window = current_client->window;

    if( mask & XIMP_FOCUS_WIN_MASK ) {
	Window     *focus_window;
	focus_window    = (Window *)GetXimpSubProperty( window, IMA_FOCUS,     XA_WINDOW);
	if( !focus_window ) {
	    ErrorMessage( 108, _XIMP_FOCUS );
	    XimpSendErrorEvent( current_client->focus_window,
		    icid, event->xclient.serial, XIMP_BadProperty, 0);
	    return(-1);
	    }
	XSelectInput( display, current_client->focus_window, NoEventMask);
	current_client->frontend_key_press_mask = 0;
	if( current_client->style & XIMPreeditPosition ) {
	    XReparentWindow( display, current_client->preedit_window,
			     *focus_window, current_client->preedit_area[0].x,
					    current_client->preedit_area[0].y );
	    XReparentWindow( display, current_client->preedit_window2,
			     *focus_window, current_client->preedit_area[1].x,
					    current_client->preedit_area[1].y );
	    }
	current_client->focus_window = *focus_window;
        if( current_client->ext_ui_back_front == XIMP_FRONTEND ) {
	    event_mask = KeyPressMask | StructureNotifyMask;
	    current_client->frontend_key_press_mask = 1;
	    }
	else
	    event_mask = StructureNotifyMask;
        XSelectInput( display, current_client->focus_window, event_mask );
	}

    if( mask & ( XIMP_PRE_AREA_MASK | XIMP_PRE_FG_MASK | XIMP_PRE_BG_MASK |
		 XIMP_PRE_COLORMAP_MASK | XIMP_PRE_BGPIXMAP_MASK |
		 XIMP_PRE_LINESP_MASK | XIMP_PRE_CURSOR_MASK | 
		 XIMP_PRE_AREANEED_MASK | XIMP_PRE_SPOTL_MASK ) ) {
        if( current_client->style & (XIMPreeditPosition | XIMPreeditNothing) ) {
	    Ximp_PreeditPropRec	preedit;
	    preedit = *current_client->preedit;
            rc = GetPreeditPropRec( window, &preedit, mask );
	    if( rc ) {
		GetPropErrorSendEvent( rc, current_client->focus_window,
			icid, event->xclient.serial );
	        return(-1);
	        }
	    ChangePreeditWindow( &preedit );
	    }
	}

    if( mask & ( XIMP_STS_AREA_MASK | XIMP_STS_FG_MASK | XIMP_STS_BG_MASK |
		 XIMP_STS_COLORMAP_MASK | XIMP_STS_BGPIXMAP_MASK |
		 XIMP_STS_LINESP_MASK | XIMP_STS_CURSOR_MASK |
		 XIMP_STS_AREANEED_MASK | XIMP_STS_WINDOW_MASK ) ) {
        if( current_client->style & (XIMStatusArea | XIMStatusNothing) ) {
	    Ximp_StatusPropRec	status;
	    status = *current_client->status;
            rc = GetStatusPropRec( window, &status, mask );
	    if( rc ) {
		GetPropErrorSendEvent( rc, current_client->focus_window,
			icid, event->xclient.serial );
	        return(-1);
	        }
	    ChangeStatusWindow( &status );
	    }
	}

    if( mask & XIMP_PRE_FONT_MASK ) {
        if( current_client->style & (XIMPreeditPosition | XIMPreeditNothing) ) {
            current_client->preedit_fontname =
		GetXimpSubProperty( window, IMA_PREEDITFONT,  XA_STRING);
	    ChangePreeditFont();
	    }
	}

    if( mask & XIMP_STS_FONT_MASK ) {
        if( current_client->style & (XIMStatusArea | XIMStatusNothing) ) {
            current_client->status_fontname =
		GetXimpSubProperty( window, IMA_STATUSFONT,  XA_STRING);
	    ChangeStatusFont();
	    }
	}

    return(0);
    }

Static int
XimpGetvalue( event)
    XEvent	*event;
    {
    unsigned long mask	 = event->xclient.data.l[2];
    Window	window;
    int		rc;
    long	event_mask;
#ifdef DEBUG
    dbg_printf("<<XimpGetvalue>> icid=%x mask=%x\n", icid, mask );
#endif DEBUG

    window = current_client->window;

    if( mask & XIMP_FOCUS_WIN_MASK ) {
	SetXimpSubProperty( window, IMA_FOCUS, XA_WINDOW, 32,
				&current_client->focus_window, 1 );
	}

    if( mask & ( XIMP_PRE_AREA_MASK | XIMP_PRE_FG_MASK | XIMP_PRE_BG_MASK |
		 XIMP_PRE_COLORMAP_MASK | XIMP_PRE_BGPIXMAP_MASK |
		 XIMP_PRE_LINESP_MASK | XIMP_PRE_CURSOR_MASK | 
		 XIMP_PRE_AREANEED_MASK | XIMP_PRE_SPOTL_MASK ) ) {
	Ximp_PreeditPropRec	preedit;
	bzero( &preedit, sizeof( Ximp_PreeditPropRec));
        SetPreeditPropRec( &preedit, current_client->preedit, mask );
	SetXimpSubProperty( window, IMA_PREEDIT, IMA_PREEDIT, 32, 
				&preedit, sizeof(Ximp_PreeditPropRec)/4);
	}

    if( mask & ( XIMP_STS_AREA_MASK | XIMP_STS_FG_MASK | XIMP_STS_BG_MASK |
		 XIMP_STS_COLORMAP_MASK | XIMP_STS_BGPIXMAP_MASK |
		 XIMP_STS_LINESP_MASK | XIMP_STS_CURSOR_MASK |
		 XIMP_STS_AREANEED_MASK | XIMP_STS_WINDOW_MASK ) ) {
	Ximp_StatusPropRec	status;
	bzero( &status, sizeof( Ximp_StatusPropRec));
        SetStatusPropRec( &status, current_client->status, mask );
	SetXimpSubProperty( window, IMA_STATUS, IMA_STATUS, 32,
				&status, sizeof(Ximp_StatusPropRec)/4);
	}

    if( mask & XIMP_PRE_FONT_MASK ) {
        if( current_client->style & (XIMPreeditPosition | XIMPreeditNothing) ) {
	    SetXimpSubProperty( window, IMA_PREEDITFONT,  XA_STRING, 8,
				current_client->preedit_fontname, 
				strlen( current_client->preedit_fontname ));
	    }
	}

    if( mask & XIMP_STS_FONT_MASK ) {
        if( current_client->style & (XIMStatusArea | XIMStatusNothing) ) {
	    SetXimpSubProperty( window, IMA_STATUSFONT,  XA_STRING, 8,
				current_client->status_fontname,
				strlen( current_client->status_fontname ));
	    }
	}

    XFlush( display);
    XimpSendEvent( current_client->focus_window,
		XIMP_GETVALUE_RETURN, current_client, 0, 0, 0 );
    return(0);
    }

Static int
SetXimpSubProperty( window, atomid, type, format, data, nelements )
    Window  	window;
    Atom    	atomid, type;
    int		format, nelements;
    unsigned char *data;
    {
    Atom            actual_type_ret;
    int		    actual_format_ret;
    unsigned long   nitems_ret;
    unsigned long   bytes_after_ret;
    unsigned char   *win_data;
    int		    j;
    for( j=XIMP_TIME_OUT; j>0; j-- ) {
        XGetWindowProperty( display, window, atomid, 0L, 8192L,
		False, type, &actual_type_ret, &actual_format_ret, &nitems_ret,
		&bytes_after_ret, &win_data);
        if( actual_format_ret == 0 || nitems_ret == 0) {
	    XChangeProperty( display, window, atomid, type, format,
				PropModeAppend, data, nelements );
	    return( 0 );
	    }
	XFree( (char *)win_data );
	sleep( 1 );
	}
#ifdef DEBUG
    dbg_printf("** SetXimpSubProperty time out !! atomid=%x (%s) type=%d\n",
			atomid, XGetAtomName( display, atomid ) );
#endif DEBUG
    return( -1 );
    }

Static int
XimpReset( event )
    XEvent	*event;
    {
#ifdef DEBUG
    dbg_printf("<<XimpReset>> icid=%x\n", icid );
#endif DEBUG
    
    fj_HenkanReset();
    return(0);
    }

Static int
XimpExtension( event)
    XEvent	*event;
    {
    Atom	  atom  = (Atom)event->xclient.data.l[2];
    int		  data1 = 	event->xclient.data.l[3];
    int		  data2 = 	event->xclient.data.l[4];
#ifdef DEBUG
    dbg_printf("<<XimpExtension>> icid=%x atom=%x data=%x %x\n",
				  icid,   atom,   data1, data2 );
#endif DEBUG
    
    if( atom == IMA_EXT_XIMP_BACK_FRONT ) {
	current_client->ext_ui_back_front = data1;
	}
    else
    if( atom == IMA_EXT_XIMP_STATUSWINDOW ) {
	if( (current_client->imp_mode & FIMP_BEGIN) != 0 ) {
	    current_client->ext_ui_statuswindow = 1;
	    }
	}
    return(0);
    }
