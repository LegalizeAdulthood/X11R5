/*
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
* SCCS_data: @(#)XmpRegAll.c 1.2 ( 1 Jan 91 )
*
*     This module contains registration routine for all Motif
*     widget/gadget constructors and classes.  It also provides
*     Motif specific converters.
*
*     The two functions XmpRegisterAll() and MriRegisterMotif()
*     are exactly equivalent.
*
* Module_interface_summary: 
*
*     void XmpRegisterAll   ( XtAppContext app )
*     void MriRegisterMotif ( XtAppContext app )
*
*******************************************************************************
*/

#include <X11/StringDefs.h>

#include <Xm/Xm.h>

#include <Xm/ArrowB.h>
#include <Xm/ArrowBG.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/Command.h>
#include <Xm/DialogS.h>
#include <Xm/DrawingA.h>
#include <Xm/DrawnB.h>
#include <Xm/FileSB.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/MainW.h>
#include <Xm/MenuShell.h>
#include <Xm/MessageB.h>
#include <Xm/PanedW.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/SelectioB.h>
#include <Xm/SeparatoG.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#ifndef  MOTIF10
#include <Xm/TextF.h>
#endif
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#ifdef TABLE
#include "Table.h"
#endif

#include <Wc/WcCreate.h>
#include <Wc/WcCreateP.h> 

/*
*******************************************************************************
* Private_macro_definitions.
*******************************************************************************
*/

#define done( type, value )                     \
{                                               \
    if ( toVal->addr != NULL )                  \
    {                                           \
        if ( toVal->size < sizeof( type ) )     \
        {                                       \
            toVal->size = sizeof( type );       \
            return;                             \
        }                                       \
        *(type*)(toVal->addr) = (value);        \
    }                                           \
    else                                        \
    {                                           \
        static type static_val;                 \
        static_val = (value);                   \
        toVal->addr = (caddr_t)&static_val;     \
    }                                           \
    toVal->size = sizeof(type);                 \
    return;                                     \
}

/*
*******************************************************************************
**  Motif specific resource converters
*******************************************************************************
*/
/*
    -- LowerCase
*******************************************************************************
    This routine was an accidental global in X11R4, Motif depends upon it
*/
void LowerCase(source, dest)    /* %%% shouldn't be global */
    register char  *source, *dest;
{
    register char ch;
    int i;

    for (i = 0; (ch = *source) != 0 && i < 999; source++, dest++, i++) {
        if ('A' <= ch && ch <= 'Z')
            *dest = ch - 'A' + 'a';
        else
            *dest = ch;
    }
    *dest = 0;
}

/*
    -- Convert String To MenuWidget
*******************************************************************************
    This conversion converts strings into menu widgets for use on
    cascade button subMenuId resource specifications.
*/
static XtConvertArgRec name_to_widget_args[] = {
    { XtBaseOffset, (caddr_t)0, sizeof(Widget) },
    };


static void CvtStringToWidget (args, num_args, fromVal, toVal)
    XrmValue *args;
    Cardinal *num_args;
    XrmValue *fromVal;
    XrmValue *toVal;
{
    Widget widget = WcFullNameToWidget( *(Widget*)(args[0].addr), fromVal->addr);

    if ( widget == NULL )
    {
       XtStringConversionWarning (fromVal->addr,
         "Widget - no such widget.  Misspelled? Forgot the path?");
          return;
    }
    done ( Widget, widget );
}


static void CvtStringToMenuWidget (args, num_args, fromVal, toVal)
    XrmValue *args;
    Cardinal *num_args;
    XrmValue *fromVal;
    XrmValue *toVal;
{
    Widget widget = WcFullNameToWidget( *(Widget*)(args[0].addr), fromVal->addr);

    if ( widget == NULL )
    {
       XtStringConversionWarning (fromVal->addr,
         "MenuWidget - no such widget.  Misspelled? Forgot the path?");
          return;
    }
    done ( Widget, widget );
}

/*
    -- Action to popup a Motif PopupMenu
*******************************************************************************
*/

static void MriPopupACT( w, event, params, num_params )
    Widget      w;
    XEvent     *event;
    String     *params;
    Cardinal   *num_params;
{
    Widget      menuPane;

    if (*num_params < 1)
	return;

    menuPane = WcFullNameToWidget ( w, params[0] );
    XmMenuPosition( menuPane, event );
    XtManageChild( menuPane );
}

/*
    -- Register all Motif widgets, converters, callbacks, actions.
*******************************************************************************
*/
void XmpRegisterAll();

void MriRegisterMotif ( app )
    XtAppContext app;
{
    XmpRegisterAll(app);
}

void XmpRegisterAll ( app )
    XtAppContext app;
{
    ONCE_PER_XtAppContext( app );

/* -- Register Motif specific action functions */
    WcRegisterAction(app, "MriPopupACT", MriPopupACT );

/* -- Register Motif specific converters */
/*   Motif "Window" representation is wrong, but it works !?! */
    XtAddConverter       (XtRString,
                          WcRWidget,  
                          CvtStringToWidget,
                          name_to_widget_args,
                          XtNumber(name_to_widget_args) );

    XtAddConverter       (XtRString,
                          XmRMenuWidget,
                          CvtStringToMenuWidget,
                          name_to_widget_args,
                          XtNumber(name_to_widget_args) );

#define RCO( name, func  )  WcRegisterConstructor ( app, name, func  )
#define RCN( name, class )  WcRegisterClassName   ( app, name, class )
#define RCP( name, class )  WcRegisterClassPtr    ( app, name, class )

#ifdef TABLE
/* -- register all Xmp widget constructors */
 RCO( "CreateTable",			XtCreateTable			);

/* -- register all Xmp widget classes */
 RCN( "Table",			        tableWidgetClass		);
 RCP( "TableWidgetClass",		tableWidgetClass		);
#endif /* TABLE */

/* -- register all Motif constructors */
 RCO( "XmCreateArrowButton",		XmCreateArrowButton		);
 RCO( "XmCreateArrowButtonGadget",	XmCreateArrowButtonGadget	);
 RCO( "XmCreateBulletinBoard",		XmCreateBulletinBoard		);
 RCO( "XmCreateBulletinBoardDialog",	XmCreateBulletinBoardDialog	);
 RCO( "XmCreateCascadeButton",		XmCreateCascadeButton		);
 RCO( "XmCreateCascadeButtonGadget",	XmCreateCascadeButtonGadget	);
 RCO( "XmCreateCommand",		XmCreateCommand			);
 RCO( "XmCreateDialogShell",		XmCreateDialogShell		);
 RCO( "XmCreateDrawingArea",		XmCreateDrawingArea		);
 RCO( "XmCreateDrawnButton",		XmCreateDrawnButton		);
 RCO( "XmCreateErrorDialog",		XmCreateErrorDialog		);
 RCO( "XmCreateFileSelectionBox",	XmCreateFileSelectionBox	);
 RCO( "XmCreateFileSelectionDialog",	XmCreateFileSelectionDialog	);
 RCO( "XmCreateForm",			XmCreateForm			);
 RCO( "XmCreateFormDialog",		XmCreateFormDialog		);
 RCO( "XmCreateFrame",			XmCreateFrame			);
 RCO( "XmCreateInformationDialog",	XmCreateInformationDialog	);
 RCO( "XmCreateLabel",			XmCreateLabel			);
 RCO( "XmCreateLabelGadget",		XmCreateLabelGadget		);
 RCO( "XmCreateList",			XmCreateList			);
 RCO( "XmCreateMainWindow",		XmCreateMainWindow		);
 RCO( "XmCreateMenuBar",		XmCreateMenuBar			);
 RCO( "XmCreateMenuShell",		XmCreateMenuShell		);
 RCO( "XmCreateMessageBox",		XmCreateMessageBox		);
 RCO( "XmCreateMessageDialog",		XmCreateMessageDialog		);
 RCO( "XmCreateOptionMenu",		XmCreateOptionMenu		);
 RCO( "XmCreatePanedWindow",		XmCreatePanedWindow		);
 RCO( "XmCreatePopupMenu",		XmCreatePopupMenu		);
 RCO( "XmCreatePromptDialog",		XmCreatePromptDialog		);
 RCO( "XmCreatePulldownMenu",		XmCreatePulldownMenu		);
 RCO( "XmCreatePushButton",		XmCreatePushButton		);
 RCO( "XmCreatePushButtonGadget",	XmCreatePushButtonGadget	);
 RCO( "XmCreateQuestionDialog",		XmCreateQuestionDialog		);
 RCO( "XmCreateRadioBox",		XmCreateRadioBox		);
 RCO( "XmCreateRowColumn",		XmCreateRowColumn		);
 RCO( "XmCreateScale",			XmCreateScale			);
 RCO( "XmCreateScrollBar",		XmCreateScrollBar		);
 RCO( "XmCreateScrolledList",		XmCreateScrolledList		);
 RCO( "XmCreateScrolledText",		XmCreateScrolledText		);
 RCO( "XmCreateScrolledWindow",		XmCreateScrolledWindow		);
 RCO( "XmCreateSelectionBox",		XmCreateSelectionBox		);
 RCO( "XmCreateSelectionDialog",	XmCreateSelectionDialog		);
 RCO( "XmCreateSeparator",		XmCreateSeparator		);
 RCO( "XmCreateSeparatorGadget",	XmCreateSeparatorGadget		);
 RCO( "XmCreateText",			XmCreateText			);
#ifndef MOTIF10
 RCO( "XmCreateTextField",		XmCreateTextField		);
#endif
 RCO( "XmCreateToggleButton",		XmCreateToggleButton		);
 RCO( "XmCreateToggleButtonGadget",	XmCreateToggleButtonGadget	);
 RCO( "XmCreateWarningDialog",		XmCreateWarningDialog		);
 RCO( "XmCreateWorkingDialog",		XmCreateWorkingDialog		);

/* -- register Motif widget classes */
 RCP("xmArrowButtonWidgetClass",	xmArrowButtonWidgetClass	);
 RCN("XmArrowButton",			xmArrowButtonWidgetClass	);
 RCP("xmArrowButtonGadgetClass",	xmArrowButtonGadgetClass	);
 RCN("XmArrowButtonGadget",		xmArrowButtonGadgetClass	);
 RCP("xmBulletinBoardWidgetClass",	xmBulletinBoardWidgetClass	);
 RCN("XmBulletinBoard",			xmBulletinBoardWidgetClass	);
 RCP("xmCascadeButtonWidgetClass",	xmCascadeButtonWidgetClass	);
 RCN("XmCascadeButton",			xmCascadeButtonWidgetClass	);
 RCP("xmCascadeButtonGadgetClass",	xmCascadeButtonGadgetClass	);
 RCN("XmCascadeButtonGadget",		xmCascadeButtonGadgetClass	);
 RCP("xmCommandWidgetClass",		xmCommandWidgetClass		);
 RCN("XmCommand",			xmCommandWidgetClass		);
 RCP("xmDialogShellWidgetClass",	xmDialogShellWidgetClass	);
 RCN("XmDialogShell",			xmDialogShellWidgetClass	);
 RCP("xmDrawingAreaWidgetClass",	xmDrawingAreaWidgetClass	);
 RCN("XmDrawingArea",			xmDrawingAreaWidgetClass	);
 RCP("xmDrawnButtonWidgetClass",	xmDrawnButtonWidgetClass	);
 RCN("XmDrawnButton",			xmDrawnButtonWidgetClass	);
 RCP("xmFileSelectionBoxWidgetClass",	xmFileSelectionBoxWidgetClass	);
 RCN("XmFileSelectionBox",		xmFileSelectionBoxWidgetClass	);
 RCP("xmFormWidgetClass",		xmFormWidgetClass		);
 RCN("XmForm",				xmFormWidgetClass		);
 RCP("xmFrameWidgetClass",		xmFrameWidgetClass		);
 RCN("XmFrame",				xmFrameWidgetClass		);
 RCP("xmGadgetClass",			xmGadgetClass			);
 RCN("XmGadget",			xmGadgetClass			);
 RCP("xmLabelWidgetClass",		xmLabelWidgetClass		);
 RCN("XmLabel",				xmLabelWidgetClass		);
 RCP("xmLabelGadgetClass",		xmLabelGadgetClass		);
 RCN("XmLabelGadget",			xmLabelGadgetClass		);
 RCP("xmListWidgetClass",		xmListWidgetClass		);
 RCN("XmList",				xmListWidgetClass		);
 RCP("xmMainWindowWidgetClass",		xmMainWindowWidgetClass		);
 RCN("XmMainWindow",			xmMainWindowWidgetClass		);
 RCP("xmManagerWidgetClass",		xmManagerWidgetClass		);
 RCN("XmManager",			xmManagerWidgetClass		);
 RCP("xmMenuShellWidgetClass",		xmMenuShellWidgetClass		);
 RCN("XmMenuShell",			xmMenuShellWidgetClass		);
 RCP("xmMessageBoxWidgetClass",		xmMessageBoxWidgetClass		);
 RCN("XmMessageBox",			xmMessageBoxWidgetClass		);
 RCP("xmPanedWindowWidgetClass",	xmPanedWindowWidgetClass	);
 RCN("XmPanedWindow",			xmPanedWindowWidgetClass	);
 RCP("xmPrimitiveWidgetClass",		xmPrimitiveWidgetClass		);
 RCN("XmPrimitive",			xmPrimitiveWidgetClass		);
 RCP("xmPushButtonWidgetClass",		xmPushButtonWidgetClass		);
 RCN("XmPushButton",			xmPushButtonWidgetClass		);
 RCP("xmPushButtonGadgetClass",		xmPushButtonGadgetClass		);
 RCN("XmPushButtonGadget",		xmPushButtonGadgetClass		);
 RCP("xmRowColumnWidgetClass",		xmRowColumnWidgetClass		);
 RCN("XmRowColumn",			xmRowColumnWidgetClass		);
 RCP("xmScaleWidgetClass",		xmScaleWidgetClass		);
 RCN("XmScale",				xmScaleWidgetClass		);
 RCP("xmScrollBarWidgetClass",		xmScrollBarWidgetClass		);
 RCN("XmScrollBar",			xmScrollBarWidgetClass		);
 RCP("xmScrolledWindowWidgetClass",	xmScrolledWindowWidgetClass	);
 RCN("XmScrolledWindow",		xmScrolledWindowWidgetClass	);
 RCP("xmSelectionBoxWidgetClass",	xmSelectionBoxWidgetClass	);
 RCN("XmSelectionBox",			xmSelectionBoxWidgetClass	);
 RCP("xmSeparatorWidgetClass",		xmSeparatorWidgetClass		);
 RCN("XmSeparator",			xmSeparatorWidgetClass		);
 RCP("xmSeparatorGadgetClass",		xmSeparatorGadgetClass		);
 RCN("XmSeparatorGadget",		xmSeparatorGadgetClass		);
 RCP("xmTextWidgetClass",		xmTextWidgetClass		);
 RCN("XmText",				xmTextWidgetClass		);
#ifndef MOTIF10
 RCN("XmTextField",			xmTextFieldWidgetClass		);
#endif
 RCP("xmToggleButtonWidgetClass",	xmToggleButtonWidgetClass	);
 RCN("XmToggleButton",			xmToggleButtonWidgetClass	);
 RCP("xmToggleButtonGadgetClass",	xmToggleButtonGadgetClass	);
 RCN("XmToggleButtonGadget",		xmToggleButtonGadgetClass	);
}
