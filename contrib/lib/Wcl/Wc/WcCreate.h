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
* SCCS_data: @(#)WcCreate.h 1.1 ( 19 Nov 90 )
*
* Include_name:
*
*     WcCreate.h
*
* Subsystem_group:
*
*     Widget Creation Library
*
* Include_description:
*
*     Public defines for the Widget Creation Library supporting widget 
*     tree creation from the Xrm database.
*
* Include_history:
*
*   mm/dd/yy  initials  action
*   --------  --------  -------------------------------------------------------
*   07/16/90   D.Smyth  added WcAllowDuplicate*Reg... decls
*   06/30/90   R.Whitby added WcRegisterWcActions declaration
*   06/19/90   D.Smyth  Widget Creation Library version 1.0 Release
*   04/04/90   marbru   updated, added new callbacks
*   03/27/90   marbru   updated for new names
*   03/02/90   marbru   created
*
*******************************************************************************
*/
#ifndef _WcCreate_h
#define _WcCreate_h

#include <X11/Intrinsic.h>

/***************************** MACRO DEFINITIONS *****************************/

/* For Motif 1.0 Intrinsics the XtIsWidget macro is defined here, since we 
 * don't want to accidentally pick up the R4 version somewhere (OpenWindows2.0)
 * Not defined in R3 intrinsics.  
 */
#ifdef MOTIF10
#undef  XtIsWidget
#endif
#ifndef XtIsWidget
#define XtIsWidget(w) XtIsSubclass(w, widgetClass)
#endif

/* GMWB - Not defined in Motif R3 intrinsics.*/
#ifndef XtRWidget
#define XtRWidget "Widget"
#endif

#ifdef FUNCTION_PROTOTYPES
/****************************** ANSI FUNC DECLS ******************************/

#define APP XtAppContext
#define EV extern void
#define EW extern Widget
#define EC extern char*

/* -- Widget class, constructor, and callback proc registration routines */

EV WcRegisterClassPtr   (APP, char* name,   WidgetClass class);
EV WcRegisterClassName  (APP, char* name,   WidgetClass class);
EV WcRegisterConstructor(APP, char* name,   Widget(*constructor) () );
EV WcRegisterCallback   (APP, char* CBname, XtCallbackProc, caddr_t defCliData);
EV WcRegisterAction	(APP, char* name,   XtActionProc proc );
EV WcRegisterWcCallbacks(APP );

/* -- Allow duplicate registration of classes, constructors, and callbacks */

EV WcAllowDuplicateRegistration   ( int allowed );
EV WcAllowDuplicateClassPtrReg    ( int allowed );
EV WcAllowDuplicateClassNameReg   ( int allowed );
EV WcAllowDuplicateConstructorReg ( int allowed );
EV WcAllowDuplicateCallbackReg    ( int allowed );

/* -- Widget action registration routine */

EV WcRegisterWcActions   ( APP );

/* -- Widget creation routines */

EI WcWidgetCreation      ( Widget root );
EV WcCreateNamedChildren ( Widget parent, char* names );
EV WcCreateNamedPopups   ( Widget parent, char* names );
EW WcCreateDatabaseChild ( Widget parent, char* name, int* managed );
EW WcCreateDatabasePopup ( Widget parent, char* name );

/* -- Widget name routines 
**	The character buffer returned by WcNamesToWidgetList contains the
**	names which could not be converted to widgets.  This buffer is static,
**	so its contents are changed everytime WcNamesToWidgetList is called.
**	The character buffer returned by WcWidgetToFullName must be XtFree'd
**	by the caller.
*/

EW WcChildNameToWidget ( Widget w, char* childName );
EW WcFullNameToWidget  ( Widget w, char* name );
EC WcNamesToWidgetList ( Widget, char* names, Widget widgetList[], int* count);
EC WcWidgetToFullName  ( Widget w );

/*  -- Useful for argument parsing */

EC WcLowerCaseCopy        ( char* in );			/* caller frees buf */
EC WcSkipWhitespace       ( char* cp );
EC WcSkipWhitespace_Comma ( char* cp );
EC WcCleanName            ( char* in, char* out );	/* out[] must exist */
EC WcStripWhitespaceFromBothEnds (char* name );		/* caller frees buf */

EC WcGetResourceType          ( Widget, char* rName );	/* caller frees buf */
EV WcSetValueFromString       ( Widget, char* rName, char* rVal );
EV WcSetValueFromStringAndType( Widget, char* rName, char* rVal, char* rType );

EC WcStrStr( char* searchThis, char* forThisPattern );	/* like ANSI strstr */

#undef APP
#undef EV
#undef EW
#undef EC

#else
/**************************** NON-ANSI FUNC DECLS ****************************/

/* -- Widget constructor registration routine */

extern void WcRegisterClassPtr    ();
extern void WcRegisterClassName   ();
extern void WcRegisterConstructor ();
extern void WcRegisterCallback    ();
extern void WcRegisterAction	  ();
extern void WcRegisterWcCallbacks ();

/* -- Allow duplicate registration of classes, constructors, and callbacks */

extern void WcAllowDuplicateRegistration   ();
extern void WcAllowDuplicateClassPtrReg    ();
extern void WcAllowDuplicateClassNameReg   ();
extern void WcAllowDuplicateConstructorReg ();
extern void WcAllowDuplicateCallbackReg    ();

/* -- Widget action registration routine */

extern void WcRegisterWcActions       ();

/* -- Widget creation routines */

extern int    WcWidgetCreation     	();
extern void   WcCreateNamedChildren	();
extern void   WcCreateNamedPopups	();
extern Widget WcCreateDatabaseChild	();
extern Widget WcCreateDatabasePopup	();

/* -- Widget name routine */

extern Widget WcChildNameToWidget	();
extern Widget WcFullNameToWidget	();
extern char*  WcNamesToWidgetList	();	/* rets: names not converted */
extern char*  WcWidgetToFullName	();	/* ret'd buff must be free'd */

extern char* WcLowerCaseCopy               ();	/* ret'd buff must be free'd */
extern char* WcSkipWhitespace              ();
extern char* WcSkipWhitespace_Comma        ();
extern char* WcCleanName                   ();
extern char* WcStripWhitespaceFromBothEnds ();	/* ret'd buff must be free'd */

extern char* WcGetResourceType             ();	/* ret'd buff must be free'd */
extern void  WcSetValueFromString          ();
extern void  WcSetValueFromStringAndType   ();

extern char* WcStrStr ();			/* same as ANSI strstr() */

#endif /* FUNCTION_PROTOTYPES */

#endif /* _WcCreate_h */
