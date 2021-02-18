/*
** Copyright (c) 1990 David E. Smyth
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
* SCCS_data: @(#)WcRegXt.c 1.1 ( 19 Nov 90 )
*
* Subsystem_group:
*
*     Widget Creation Library - Intrinsic Resource Interpreter
*
* Module_description:
*
*     This module contains registration routine for all Intrinsic
*     widget constructors and classes.  
*
* Module_interface_summary: 
*
*     void WcRegisterIntrinsic ( XtAppContext app )
*
* Module_history:
*                                                  
*   mm/dd/yy  initials  function  action
*   --------  --------  --------  ---------------------------------------------
*   07/23/90  D.Smyth   cleaned up function return values.
*   06/19/90  R.Whitby  all	  create.
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

#include <X11/Object.h>
#include <X11/RectObj.h>
#include <X11/Shell.h>
#include <X11/Vendor.h>
#include <X11/Core.h>

#include "WcCreateP.h"

/* -- Widget constructor routines */

Widget WcCreateApplicationShell	();
Widget WcCreateOverrideShell	();
Widget WcCreateShell		();
Widget WcCreateTopLevelShell	();
Widget WcCreateTransientShell	();
Widget WcCreateVendorShell	();
Widget WcCreateWMShell		();


void WcRegisterIntrinsic ( app )
    XtAppContext app;
{
    ONCE_PER_XtAppContext( app );

#define RCN( name, class ) WcRegisterClassName ( app, name, class );
#define RCP( name, class ) WcRegisterClassPtr  ( app, name, class );
#define RCR( name, func )  WcRegisterConstructor(app, name, func  );

    /* -- register all Intrinsic widget classes */

    RCN("Object",			objectClass );
    RCP("objectClass",			objectClass );
    RCN("RectObj",			rectObjClass );
    RCP("rectObjClass",			rectObjClass );
#ifndef MOTIF10
    RCN("Core",				coreWidgetClass );
    RCP("coreWidgetClass",		coreWidgetClass );
#endif
    RCN("Composite",			compositeWidgetClass );
    RCP("compositeWidgetClass",		compositeWidgetClass );
    RCN("Constraint",			constraintWidgetClass );
    RCP("constraintWidgetClass",	constraintWidgetClass );
    RCN("ApplicationShell",		applicationShellWidgetClass );
    RCP("applicationShellWidgetClass",	applicationShellWidgetClass );
    RCN("OverrideShell",		overrideShellWidgetClass );
    RCP("overrideShellWidgetClass",	overrideShellWidgetClass );
    RCN("Shell",			shellWidgetClass );
    RCP("shellWidgetClass",		shellWidgetClass );
    RCN("TopLevelShell",		topLevelShellWidgetClass );
    RCP("topLevelShellWidgetClass",	topLevelShellWidgetClass );
    RCN("TransientShell",		transientShellWidgetClass );
    RCP("transientShellWidgetClass",	transientShellWidgetClass );
    RCN("VendorShell",			vendorShellWidgetClass );
    RCP("vendorShellWidgetClass",	vendorShellWidgetClass );
    RCN("WmShell",			wmShellWidgetClass );
    RCP("wmShellWidgetClass",		wmShellWidgetClass );

    /* -- register all Intrinsic constructors */

    RCR("XtCreateApplicationShell",	WcCreateApplicationShell);
    RCR("XtCreateOverrideShell",	WcCreateOverrideShell);
    RCR("XtCreateShell",		WcCreateShell);
    RCR("XtCreateTopLevelShell",	WcCreateTopLevelShell);
    RCR("XtCreateTransientShell",	WcCreateTransientShell);
    RCR("XtCreateWMShell",		WcCreateWMShell);
    RCR("XtCreateVendorShell",		WcCreateVendorShell);
}

/*
    -- Create Application Shell
*******************************************************************************
    This function creates an application shell widget.
    
*/
Widget WcCreateApplicationShell ( pw, name, args, nargs )
    Widget	pw;	/* children's parent 				*/
    String      name;	/* widget name to create 			*/
    Arg        *args;	/* args for widget				*/
    Cardinal    nargs;	/* args count					*/
{

  return XtCreatePopupShell(name, applicationShellWidgetClass, pw, args, nargs);
}

/*
    -- Create Override Shell
*******************************************************************************
    This function creates an override shell widget.
    
*/
Widget WcCreateOverrideShell ( pw, name, args, nargs )
    Widget	pw;	/* children's parent 				*/
    String      name;	/* widget name to create 			*/
    Arg        *args;	/* args for widget				*/
    Cardinal    nargs;	/* args count					*/
{
  return XtCreatePopupShell(name, overrideShellWidgetClass, pw, args, nargs);
}

/*
    -- Create Shell
*******************************************************************************
    This function creates a shell widget.
    
*/
Widget WcCreateShell ( pw, name, args, nargs )
    Widget	pw;	/* children's parent 				*/
    String      name;	/* widget name to create 			*/
    Arg        *args;	/* args for widget				*/
    Cardinal    nargs;	/* args count					*/
{
  return XtCreatePopupShell(name, shellWidgetClass, pw, args, nargs);
}

/*
    -- Create TopLevel Shell
*******************************************************************************
    This function creates a top level shell widget.
    
*/
Widget WcCreateTopLevelShell ( pw, name, args, nargs )
    Widget	pw;	/* children's parent 				*/
    String      name;	/* widget name to create 			*/
    Arg        *args;	/* args for widget				*/
    Cardinal    nargs;	/* args count					*/
{
  return XtCreatePopupShell(name, topLevelShellWidgetClass, pw, args, nargs);
}

/*
    -- Create Transient Shell
*******************************************************************************
    This function creates an transient shell widget.
    
*/
Widget WcCreateTransientShell ( pw, name, args, nargs )
    Widget	pw;	/* children's parent 				*/
    String      name;	/* widget name to create 			*/
    Arg        *args;	/* args for widget				*/
    Cardinal    nargs;	/* args count					*/
{
  return XtCreatePopupShell(name, transientShellWidgetClass, pw, args, nargs);
}

/*
    -- Create Vendor Shell
*******************************************************************************
    This function creates a vendor shell widget.
    
*/
Widget WcCreateVendorShell ( pw, name, args, nargs )
    Widget	pw;	/* children's parent 				*/
    String      name;	/* widget name to create 			*/
    Arg        *args;	/* args for widget				*/
    Cardinal    nargs;	/* args count					*/
{
  return XtCreatePopupShell(name, vendorShellWidgetClass, pw, args, nargs);
}

/*
    -- Create WM Shell
*******************************************************************************
    This function creates an WM shell widget.
    
*/
Widget WcCreateWMShell ( pw, name, args, nargs )
    Widget	pw;	/* children's parent 				*/
    String      name;	/* widget name to create 			*/
    Arg        *args;	/* args for widget				*/
    Cardinal    nargs;	/* args count					*/
{
  return XtCreatePopupShell(name, wmShellWidgetClass, pw, args, nargs);
}
