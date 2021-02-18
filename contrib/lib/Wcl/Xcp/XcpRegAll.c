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
* SCCS_data: @(#)XcpRegAll.c 1.0 ( 19 Jun 91 )
*
* Subsystem_group:
*
*     Widget Creation Library - Cornell Resource Interpreter
*
* Module_description:
*
*     This module contains registration routine for all Cornell
*     widget constructors and classes.  
*
*     The two functions XcpRegisterAll() and CriRegisterCornell()
*     are exactly equivalent.
*
* Module_interface_summary: 
*
*     void XcpRegisterAll     ( XtAppContext app )
*     void CriRegisterCornell ( XtAppContext app )
*
* Module_history:
*                                                  
*   mm/dd/yy  initials  function  action
*   --------  --------  --------  ---------------------------------------------
*   06/19/90  D.Smyth   all	  create.
*   06/19/91  K.Gillies           modified support for Cornell widgets
*				  based on Athena version.
* Design_notes:
*
*******************************************************************************
*/
/*
*******************************************************************************
* Include_files.
*******************************************************************************
*/
#include <stdio.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <Xcu/Bmgr.h>
#include <Xcu/Button.h>
#include <Xcu/Command.h>
#include <Xcu/Deck.h>
#include <Xcu/Label.h>
#include <Xcu/Rc.h>
#include <Xcu/Tbl.h>
#include <Xcu/Entry.h>
/* #include <Xcu/Wlm.h> 	Wlm isn't included yet. */
#ifdef TABLE
#include <Xp/Table.h>
#endif

#include <Wc/WcCreate.h>
#include <Wc/WcCreateP.h>

#ifdef DEBUG
#include "CriCornellP.h"
#endif

/* Some arbitrary constants and static storage for Xcp */
#define MAX_XCU_MSG              256
#define MAX_BMGR_WIDGETS         20        /* Arbitrary */

static char msg[MAX_XCU_MSG];
static char cleanName[MAX_PATHNAME];
static Widget buttonwidgets[MAX_BMGR_WIDGETS];
static caddr_t buttonvals[MAX_BMGR_WIDGETS];

/* Public functions */
void XcpRegisterAll();
void CriRegisterCornell();
/* Private functions */
static void registerXcuCBs();

void CriRegisterCornell(app)
XtAppContext app;
{
  XcpRegisterAll(app);
}

#define RCN( name, class ) WcRegisterClassName ( app, name, class );
#define RCP( name, class ) WcRegisterClassPtr  ( app, name, class );
#define RCR( name, func )  WcRegisterConstructor(app, name, func  )

void XcpRegisterAll(app)
XtAppContext app;
{
  ONCE_PER_XtAppContext( app );

  /* Add the Cornell specific callbacks */
  registerXcuCBs();

#ifdef TABLE
    /* -- register the Table widget classes */
  RCN("Table",                      tableWidgetClass        );
  RCP("tableWidgetClass",           tableWidgetClass        );
#endif

  /* -- register all Cornell Widget classes */
  RCN("Command",		xcuCommandWidgetClass	);
  RCP("xcuCommandWidgetClass",  xcuCommandWidgetClass	);
  RCN("Label",			xcuLabelWidgetClass	);
  RCP("xcuLabelWidgetClass",	xcuLabelWidgetClass	);
  RCN("Button",                 xcuButtonWidgetClass    );
  RCP("xcuButtonWidgetClass",   xcuButtonWidgetClass    );
  RCN("Simple",			xcuSimpleWidgetClass	);
  RCP("xcuSimpleWidgetClass",	xcuSimpleWidgetClass	);

  /* Composite and Constraint Widgets (Chapt 6) */
  RCN("ButtManager",            xcuBmgrWidgetClass      );
  RCP("xcuBmgrWidgetClass",     xcuBmgrWidgetClass      );
  RCN("Deck",			xcuDeckWidgetClass	);
  RCP("xcuDeckWidgetClass",	xcuDeckWidgetClass	);
  RCN("RowCol",			xcuRcWidgetClass	);
  RCP("xcuRcWidgetClass",	xcuRcWidgetClass	);
  RCN("Tbl",			xcuTblWidgetClass	);
  RCP("xcuTblWidgetClass",	xcuTblWidgetClass	);
/* Not supported yet.
  RCN("Wlm",                    xcuWlmWidgetClass       );
  RCP("xcuWlmWidgetClass",      xcuWlmWidgetClass       );
*/
  /* Others */
  RCN("Entry",                  xcuEntryWidgetClass     );
  RCP("xcuEntryWidgetClass",    xcuEntryWidgetClass     );
}

/* -----Xcu specific callbacks----------*/
/* First the button manager */

/* Callback for XcuBmgrManage.  This is wired to the not-to-elegant
 * value MAX_BMGR_WIDGETS.  I didn't want to count the names or allocate
 * memory on the fly. Maybe next time.
 */
static void XcuBmgrManageCB(w, wnames, call_data)
  Widget w;
  char *wnames;
  caddr_t call_data;
{
  int buttoncount;
  char *children;
  Widget thiswidget;
  Widget commonparent;

  if (*wnames == NUL ) {
    sprintf(msg, "XcuBmgrManage (%s) - Failed \n\
	      Usage: %s (parent, button [,button ] ...) \n\
              Problem: No widget names provided.", XtName(w));
    XtWarning(msg);
    return;
  }
  buttoncount = 0;
  commonparent = w;
  /* Skip up the the composite that owns the button to get the refwidget */
  while((commonparent = XtParent(commonparent)) != NULL &&
	!XtIsComposite(commonparent)) {
  }
  children = wnames;
  /* Get the widget values for each of the bmgr children */
  while(*children != NULL) {
    if (buttoncount >= MAX_BMGR_WIDGETS) {
      sprintf(msg, "XcuBmgrManage (%s) - Failed\n\
                  Problem:  Too many buttons to manage--Up MAX_BMGR_WIDGETS.",
		  XtName(w));
      XtWarning(msg);
      break;
    }
    children = WcCleanName(children, cleanName);
    children = WcSkipWhitespace_Comma(children);

    thiswidget = WcFullNameToWidget(commonparent, cleanName);

    buttonwidgets[buttoncount] = thiswidget;
    buttonvals[buttoncount] = (caddr_t)buttoncount;
    /* Increment address */
    buttoncount++;
  }
  /* Call the Xcu function with the arrays */
  XcuBmgrManage(w, buttonwidgets, buttonvals, buttoncount);
}

/* XcuBmgrSample */
static void XcuBmgrSampleCB(w, wnames, call_data)
  Widget w;
  char *wnames;
  caddr_t call_data;
{
  Widget commonparent;

  if (*wnames == NUL ) {
    XtWarning( "XcuBmgrSampleCB( ) - Failed \n\
	      Usage: %s(bmgr) \n\
              Problem: No bmgr widget name provided.");
    return;
  }
  /* this may not work well enough, but we'll see */
  commonparent = w;
  /* Skip up the the composite that owns the button to get the refwidget */
  while((commonparent = XtParent(commonparent)) != NULL &&
	!XtIsComposite(commonparent)) {
  }
  (void)WcCleanName(wnames, cleanName);
  XcuBmgrSample(WcFullNameToWidget(commonparent, cleanName));
}

/* XcuBmgrSetAll */
static void XcuBmgrSetAllCB(w, wnames, call_data)
  Widget w;
  char *wnames;
  caddr_t call_data;
{
  Widget commonparent;

  if (wnames == NUL ) {
    XtWarning( "XcuBmgrSetAll( ) - Failed \n\
	      Usage: %s(bmgr) \n\
              Problem: No bmgr widget name provided.");
    return;
  }
  /* This may not work well enough, but we'll see */
  commonparent = w;
  /* Skip up the the composite that owns the button to get the refwidget */
  while((commonparent = XtParent(commonparent)) != NULL &&
	!XtIsComposite(commonparent)) {
  }
  /* Get the widget values for each of the bmgr children */
  (void)WcCleanName(wnames, cleanName);
  XcuBmgrSetAll(WcFullNameToWidget(commonparent, cleanName));
}

/* XcuBmgrUnsetAll */
static void XcuBmgrUnsetAllCB(w, wnames, call_data)
  Widget w;
  char *wnames;
  caddr_t call_data;
{
  Widget commonparent;

  if (wnames == NUL ) {
    XtWarning( "XcuBmgrUnsetAll( ) - Failed \n\
	      Usage: %s(bmgr) \n\
              Problem: No bmgr widget name provided.");
    return;
  }
  /* This may not work well enough, but we'll see */
  commonparent = w;
  /* Skip up the the composite that owns the button to get the refwidget */
  while((commonparent = XtParent(commonparent)) != NULL &&
	!XtIsComposite(commonparent)) {
  }

  /* Get the widget values for each of the bmgr children */
  (void)WcCleanName(wnames, cleanName);
  XcuBmgrUnsetAll(WcFullNameToWidget(commonparent, cleanName));
}

/* XcuBmgrToggleAll */
static void XcuBmgrToggleAllCB(w, wnames, call_data)
  Widget w;
  char *wnames;
  caddr_t call_data;
{
  Widget commonparent;

  if (*wnames == NUL ) {
    XtWarning( "XcuBmgrToggleAllCB( ) - Failed \n\
	      Usage: %s(bmgr) \n\
              Problem: No bmgr widget name provided.");
    return;
  }
  /* This may not work well enough, but we'll see */
  commonparent = w;
  /* Skip up the the composite that owns the button to get the refwidget */
  while((commonparent = XtParent(commonparent)) != NULL &&
	!XtIsComposite(commonparent)) {
  }
  (void)WcCleanName(wnames, cleanName);
  XcuBmgrToggleAll(WcFullNameToWidget(commonparent, cleanName));
}

/* -------Support for decks----------- */

/* XcuDeckRaiseLowest */
static void XcuDeckRaiseLowestCB(w, wnames, call_data)
  Widget w;
  char *wnames;
  caddr_t call_data;
{
  Widget commonparent;

  if (*wnames == NUL ) {
    XtWarning( "XcuDeckRaiseLowest( ) - Failed \n\
	      Usage: %s(deck) \n\
              Problem: No deck widget name provided.");
    return;
  }
  /* This may not work well enough, but we'll see */
  commonparent = w;
  /* Skip up the the composite that owns the button to get the refwidget */
  while((commonparent = XtParent(commonparent)) != NULL &&
	!XtIsComposite(commonparent)) {
  }
  (void)WcCleanName(wnames, cleanName);
  XcuDeckRaiseLowest(WcFullNameToWidget(commonparent, cleanName));
}

/* XcuDeckLowerHighest */
static void XcuDeckLowerHighestCB(w, wnames, call_data)
  Widget w;
  char *wnames;
  caddr_t call_data;
{
  Widget commonparent;

  if (wnames == NUL ) {
    XtWarning( "XcuDeckLowerHighest( ) - Failed \n\
	      Usage: %s(deck) \n\
              Problem: No deck widget name provided.");
    return;
  }
  /* This may not work well enough, but we'll see */
  commonparent = w;
  /* Skip up the the composite that owns the button to get the refwidget */
  while((commonparent = XtParent(commonparent)) != NULL &&
	!XtIsComposite(commonparent)) {
  }
  (void)WcCleanName(wnames, cleanName);
  XcuDeckLowerHighest(WcFullNameToWidget(commonparent, cleanName));
}

/* XcuDeckRaiseWidget */
static void XcuDeckRaiseWidgetCB(w, wnames, call_data)
  Widget w;
  char *wnames;
  caddr_t call_data;
{
  Widget commonparent;
  Widget deckwidget, raisewidget;
  char *name;

  if (wnames == NUL ) {
    sprintf(msg, "XcuDeckRaiseWidget() - Failed \n\
	      Usage: %s(deck deckchild) \n\
              Problem: No good widget names provided.");
    return;
  }
  /* This may not work well enough, but we'll see */
  commonparent = w;
  /* Skip up the the shell since deck may not be immediately related to
   * the caller of the function.
   */
  while((commonparent = XtParent(commonparent)) != NULL &&
	!XtIsShell(commonparent)) {
  }
  /* Get the name of the deck and its id */
  name = WcCleanName(wnames, cleanName);
  name = WcSkipWhitespace_Comma(name);
  deckwidget = WcFullNameToWidget(commonparent, cleanName);
  /* Now get the name of the deck child */
  name = WcCleanName(name, cleanName);
  raisewidget = WcFullNameToWidget(deckwidget, cleanName);
  /* Now raise the widget by calling the Xcu public function */    
  XcuDeckRaiseWidget(deckwidget, raisewidget);
}

/* Register all the Xcu callbacks */
static void registerXcuCBs(app)
XtAppContext app;
{
  WcRegisterCallback(app, "XcuBmgrManageCB", XcuBmgrManageCB);
  WcRegisterCallback(app, "XcuBmgrSampleCB", XcuBmgrSampleCB);
  WcRegisterCallback(app, "XcuBmgrSetAllCB", XcuBmgrSetAllCB);
  WcRegisterCallback(app, "XcuBmgrUnsetAllCB", XcuBmgrUnsetAllCB);
  WcRegisterCallback(app, "XcuBmgrToggleAllCB", XcuBmgrToggleAllCB);
  WcRegisterCallback(app, "XcuDeckLowerHighestCB", XcuDeckLowerHighestCB);
  WcRegisterCallback(app, "XcuDeckRaiseLowestCB", XcuDeckRaiseLowestCB);
  WcRegisterCallback(app, "XcuDeckRaiseWidgetCB", XcuDeckRaiseWidgetCB);
/*
  WcRegisterCallback(app, "XcuBmgrSetChildCB", XcuBmgrSetChildCB);
  WcRegisterCallback(app, "XcuBmgrUnsetChildCB", XcuBmgrUnsetChildCB);
*/
}
