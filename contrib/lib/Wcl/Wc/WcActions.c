/*
** Copyright (c) 1990 Rodney J. Whitby
**
** This file was derived from work performed by David E. Smyth under the
** following copyright:
**
*******************************************************************************
** Copyright (c) 1990 David E. Smyth
**
** This file was derived from work performed by Martin Brunecky at
** Auto-trol Technology Corporation, Denver, Colorado, under the
** following copyright:
**
*******************************************************************************
* Copyright 1990 by Auto-trol Technology Corporation, Denver, Colorado.
*
*                        All Rights Reserved
*
* Permission to use, copy, modify, and distribute this software and its
* documentation for any purpose and without fee is hereby granted, provided
* that the above copyright notice appears on all copies and that both the
* copyright and this permission notice appear in supporting documentation
* and that the name of Auto-trol not be used in advertising or publicity
* pertaining to distribution of the software without specific, prior written
* permission.
*
* Auto-trol disclaims all warranties with regard to this software, including
* all implied warranties of merchantability and fitness, in no event shall
* Auto-trol be liable for any special, indirect or consequential damages or
* any damages whatsoever resulting from loss of use, data or profits, whether
* in an action of contract, negligence or other tortious action, arising out
* of or in connection with the use or performance of this software.
*******************************************************************************
**
** Redistribution and use in source and binary forms are permitted
** provided that the above copyright notice and this paragraph are
** duplicated in all such forms and that any documentation, advertising
** materials, and other materials related to such distribution and use
** acknowledge that the software was developed by David E. Smyth.  The
** name of David E. Smyth may not be used to endorse or promote products
** derived from this software without specific prior written permission.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/

/*
* SCCS_data: @(#)WcActions.c 1.1 ( 19 Nov 90 )
*
* Subsystem_group:
*
*     Widget Creation Library
*
* Module_description:
*
*     This module contains the convenience actions used to create and 
*     manage a widget tree using the Xrm databse.
*
*     Several convenience actions are provided with the package, allowing 
*     deferred widget creation, control (manage/unmanage) and other utility
*     functions.
*
* Module_interface_summary: 
*
*     Convenience Actions:
*
* Module_history:
*                                                  
*   All actions and the action registration routine were made by
*   Rod Whitby following about 30 June 1990.
*
* Design_notes:
*
*******************************************************************************
*/
/*
*******************************************************************************
* Include_files.
*******************************************************************************
*/

#include <X11/Intrinsic.h>
#include "WcCreate.h"
#include "WcCreateP.h"

/*
*******************************************************************************
* Private_type_declarations.
*******************************************************************************
*/

/*
*******************************************************************************
* Private_macro_definitions.
*******************************************************************************
*/

/*
*******************************************************************************
* Private_data_definitions.
*******************************************************************************
*/

static char msg[1024];

/*
*******************************************************************************
* Private_function_declarations.
*******************************************************************************
*/

#define COMMAS 1
#define NO_COMMAS 0

static char* AllocAndBuildString( params, num_params, commas )
    char *params[];
    int  *num_params;
    int   commas;
{
    char *data;
    int   len, i;

    for ( len = 0, i = 0; i < *num_params; i++ )
        len += strlen( params[i] ) + 1;

    data = XtMalloc(len + 1);

    (void)strcpy(data, params[0]);
    for ( i = 1; i < *num_params; i++ )
    {
	if (commas)
	    strcat(data, ",");
	else
	    strcat(data, " ");
        (void)strcat(data, params[i]);
    }
    return data;
}

static void SendToCallback( callback, w, params, num_params, min_reqd, commas )
    XtCallbackProc callback;
    Widget         w;
    char          *params[];
    int           *num_params;
    int            min_reqd;
    int            commas;
{
    char* data;

    if ( *num_params < min_reqd )
    {
	callback( w, "", NULL );
	return;
    }

    data = AllocAndBuildString( params, num_params, commas );

    callback( w, data, NULL );
    XtFree(data);
}

/*
*******************************************************************************
* Public_action_function_declarations.
*******************************************************************************
*/

/*
    -- Create Dynamic Children from Xrm Database
*******************************************************************************
    WcCreateChildrenACT( parent, child [, child ... ] )
*/
void    WcCreateChildrenACT( w, event, params, num_params )
    Widget      w;
    XEvent     *event;
    String     *params;
    Cardinal   *num_params;
{
    SendToCallback( WcCreateChildrenCB, w, params, num_params, 2, COMMAS );
}

/*
    -- Create Dynamic Popup Children from Xrm Database
*******************************************************************************
    WcCreatePopupsACT( parent, child [, child ... ] )
*/
void    WcCreatePopupsACT( w, event, params, num_params )
    Widget      w;
    XEvent     *event;
    String     *params;
    Cardinal   *num_params;
{
    SendToCallback( WcCreatePopupsCB, w, params, num_params, 2, COMMAS );
}

/*
    -- Manage or Unmanage named widget(s)
*******************************************************************************
    WcManageACT  ( widget_path [, widget_path ... ] )
    WcUnmanageACT( widget_path [, widget_path ... ] )
*/
void	WcManageACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcManageCB, w, params, num_params, 1, COMMAS );
}

void	WcUnmanageACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcUnmanageCB, w, params, num_params, 1, COMMAS );
}

/*
    -- Manage or unamange named children action
*******************************************************************************
    WcManageChildrenACT  ( parent, child [, child ... ] )
    WcUnmanageChildrenACT( parent, child [, child ... ] )
*/
void	WcManageChildrenACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcManageChildrenCB, w, params, num_params, 2, COMMAS );
}

void	WcUnmanageChildrenACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcUnmanageChildrenCB, w, params, num_params, 2, COMMAS );
}

/*
    -- Destroy named children action
*******************************************************************************
    WcDestroyACT( widget_path [, widget_path ... ] )
*/
void	WcDestroyACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcDestroyCB, w, params, num_params, 1, COMMAS );
}

/*
    -- Set Resource Value on Widget
*******************************************************************************
    WcSetValueACT( widget_path.res_name: res_val )
*/
void	WcSetValueACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    /* note: blanks are optional, so we really don't know how many params 
    ** we get from the translation manager: anything from 1 to 3. 
    */
    SendToCallback( WcSetValueCB, w, params, num_params, 1, NO_COMMAS );
}

/*
    -- Set Resource Value on Widget
*******************************************************************************
    WcSetTypeValueACT( widget_path.res_name: type, res_val )
*/
void	WcSetTypeValueACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcSetTypeValueCB, w, params, num_params, 1, NO_COMMAS );
}

/*
    -- Change sensitivity of widgets.
*******************************************************************************
    WcSetSensitiveACT  ( widget_path [, widget_path ... ] )
    WcSetInsensitiveACT( widget_path [, widget_path ... ] )
*/
void	WcSetSensitiveACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcSetSensitiveCB, w, params, num_params, 1, COMMAS );
}

void	WcSetInsensitiveACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcSetInsensitiveCB, w, params, num_params, 1, COMMAS );
}

/*
    -- Load Resource File
*******************************************************************************
    WcLoadResourceFileACT( file_name )
*/
void	WcLoadResourceFileACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcLoadResourceFileCB, w, params, num_params, 1, COMMAS );
}

/*
    -- WcTraceAction
*******************************************************************************
    WcTraceACT( [ annotation ] )
*/
void	WcTraceACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcTraceCB, w, params, num_params, 1, COMMAS );
}

/*
  -- Popup and Popdown named widget
*******************************************************************************
    WcPopupACT    ( widget_path )
    WcPopupGrabACT( widget_path )
    WcPopdownACT  ( widget_path )
*/
void    WcPopupACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcPopupCB, w, params, num_params, 1, COMMAS );
}

void    WcPopupGrabACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcPopupGrabCB, w, params, num_params, 1, COMMAS );
}

void    WcPopdownACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcPopdownCB, w, params, num_params, 1, COMMAS );
}

/*
  -- Map and Unmap named widget
*******************************************************************************
    WcMapACT  ( widget_path )
    WcUnmapACT( widget_path )
*/
void	WcMapACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcMapCB, w, params, num_params, 1, COMMAS );
}

void	WcUnmapACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcUnmapCB, w, params, num_params, 1, COMMAS );
}

/*
    -- Invoke shell command
*******************************************************************************
    WcSystemACT( any shell command line )
*/
void	WcSystemACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcSystemCB, w, params, num_params, 1, NO_COMMAS );
}

/*
    -- Exit the application
*******************************************************************************
    WcExitACT( [ integer_exit_code ] )
*/
void	WcExitACT( w, event, params, num_params )
    Widget    w;
    XEvent   *event;
    String   *params;
    Cardinal *num_params;
{
    SendToCallback( WcExitCB, w, params, num_params, 1, COMMAS );
}

/*
  -- WcRegisterWcActions
*******************************************************************************
   Convenience routine, registering all standard actions in one application
   call.   Called from WcWidgetCreation(), so application usually never needs
   to call this.
*/

void WcRegisterWcActions ( app )
XtAppContext app;
{
    static XtActionsRec WcActions[] = {
      {"WcCreateChildrenACT",	WcCreateChildrenACT	},
      {"WcCreatePopupsACT",	WcCreatePopupsACT	},
      {"WcManageACT",		WcManageACT		},
      {"WcUnmanageACT",		WcUnmanageACT		},
      {"WcManageChildrenACT",	WcManageChildrenACT	},
      {"WcUnmanageChildrenACT",	WcUnmanageChildrenACT	},
      {"WcDestroyACT",		WcDestroyACT		},
      {"WcSetValueACT",		WcSetValueACT		},
      {"WcSetTypeValueACT",	WcSetTypeValueACT	},
      {"WcSetSensitiveACT",	WcSetSensitiveACT	},
      {"WcSetInsensitiveACT",	WcSetInsensitiveACT	},
      {"WcLoadResourceFileACT",	WcLoadResourceFileACT	},
      {"WcTraceACT",		WcTraceACT		},
      {"WcPopupACT",		WcPopupACT		},
      {"WcPopupGrabACT",	WcPopupGrabACT		},
      {"WcPopdownACT",		WcPopdownACT		},
      {"WcMapACT",		WcMapACT		},
      {"WcUnmapACT",		WcUnmapACT		},
      {"WcSystemACT",		WcSystemACT		},
      {"WcExitACT",		WcExitACT		},
    };

    ONCE_PER_XtAppContext( app );

    XtAppAddActions(app, WcActions, XtNumber(WcActions));
}
