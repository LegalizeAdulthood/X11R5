/*
** Copyright (c) 1990 David E. Smyth
**
** This file was derived from work performed by Martin Brunecky at
** Auto-trol Technology Corporation, Denver, Colorado, under the
** following copyright:
**
*******************************************************************************
* Copyright 1990,91 by Auto-trol Technology Corporation, Denver, Colorado.
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
* SCCS_data: @(#)WcReg.c 1.1 ( 19 Nov 90 )
*
* Subsystem_group:
*
*     Widget Creation Library
*
* Module_description:
*
*     Since (for portability reasons) we can not assume runtime binding,
*     all widget classes, creation routines (constructors), and callbacks
*     must be "registered"  by the application BEFORE widget tree creation.
*
*     All four of the functions defined in this module load dynamically
*     allocated and extended arrays of structures.  The size increment
*     of the arrays starts at a reasonably small size (INCR_REGISTRY,
*     initially 32), and is doubled in size everytime a given registry is
*     filled.  This allows registries to be small, yet to not have to be
*     realloc'd frequently when they grow large.  
*
*     The registries are arrays of structs.  In all four cases, the
*     structs are very similar: they contain a name string which holds the
*     class, constructor, or callback name as it was registered; a quark
*     which is based on an all lower case representation of the name, and
*     class, constructor, or callback specific information.  The name
*     as registered should be as shown in reference documents and source
*     code, as it is used for user error messages.
*
*     The registries are intended to be used by string-to-whatever converters.
*
*     All four registration functions currently check for duplicate
*     entries, but do no fancy hashing scheme, nor any ties to the
*     application context.  Assumming a relatively small number of
*     entries in these regestries, it is assumed that a sequential
*     search using quarks will be adequate and simple.

* Module_interface_summary: 
*
*       WcRegisterClassPtr(
*	    XtAppContext  app,	    - application context
*	    String	  name,	    - class ptr name, as in ref manuals
*	    WidgetClass   class )   - class record pointer
*
*       WcRegisterClassName(
*	    XtAppContext  app,	    - application context
*	    String	  name,	    - class name, as in ref manuals
*	    WidgetClass   class )   - class record pointer
*
*       WcRegisterConstructor(
*	    XtAppContext  app,	    - application context
*	    String	  name,	    - constructor name, as in ref manuals
*	    (*Widget)()   const )   - constructor function pointer
*
*	WcRegisterCallback(
*	    XtAppContext  app,      - application context
*           String        name,     - callback name, "nice" capitalization
*	    void (*func)() )        - pointer to callback function
*
*	WcRegisterAction(
*	    XtAppContext  app,      - application context
*           String        name,     - action name
*	    XtActionProc  proc )    - action proc

*	WcAllowDuplicateRegistration   ( int allowed )
*	WcAllowDuplicateClassPtrReg    ( int allowed )
*	WcAllowDuplicateClassNameReg   ( int allowed )
*	WcAllowDuplicateConstructorReg ( int allowed )
*	WcAllowDuplicateCallbackReg    ( int allowed )
*
* Module_history:

*   mm/dd/yy  initials  function  action
*   --------  --------  --------  ---------------------------------------------
*   16Jul90   D.Smyth   Added WcAllowDuplicate*()
*   19Jun90   D.Smyth   Version 1.0 Widget Creation Library
*   06/08/90  D.Smyth   All Added "name" member for better user msgs.
*   02/26/90  MarBru    All       Created
*   02/16/90  MarBru    Create..  Limited creation to composite widgets/objects
*
* Design_notes:
*
*   For VMS, we could have used LIB$FIND_IMAGE_SYMBOL and use dynamic
*   (runtime) binding. But since most UNIX systems lack such capability,
*   we stick to the concept of "registration" routines.
*
*******************************************************************************
*/
/*
*******************************************************************************
* Include_files.
*******************************************************************************
*/

/*  -- X Window System Includes */
#include <X11/Intrinsic.h>

/*  -- Widget Creation Includes */
#include "WcCreate.h"
#include "WcCreateP.h"

/*
*******************************************************************************
* Private_data_definitions.
*******************************************************************************
    The following cache/registry of known widget classes, contructors, 
    and callbacks are initially empty, and are loaded by the application 
    using "registration" routines.
*/

static char     msg[MAX_ERRMSG];

static int allowDuplicateClassPtrReg    = FALSE;
static int allowDuplicateClassNameReg   = FALSE;
static int allowDuplicateConstructorReg = FALSE;
static int allowDuplicateCallbackReg    = FALSE;

/*  -- Named class pointer cache, intially empty */

int         classes_num = 0;
int         classes_max = 0;
ClCacheRec *classes_ptr = NULL;

/*  -- Class name cache, intially empty */

int             cl_nm_num = 0;
int             cl_nm_max = 0;
ClNameCacheRec* cl_nm_ptr = NULL;

/*  -- Named object constructor cache, intially empty */

int          constrs_num = 0;
int          constrs_max = 0;
ConCacheRec *constrs_ptr = NULL;

/*  -- Named callback procedures cache, intially empty */

int         callbacks_num = 0;
int         callbacks_max = 0;
CBCacheRec *callbacks_ptr = NULL;

/*
*******************************************************************************
* Private_function_declarations.
*******************************************************************************
*/

/*
*******************************************************************************
* Public_function_declarations.
*******************************************************************************
*/

/*
    -- Allow or Disallow Duplicate Registrations
*******************************************************************************
    By default, the Widget Creation Library does not allow a
    string-to-callback, string-to-class, etc bindings to be re-defined.
    Some applications, most noticably user interface builders, need to be
    able to change these bindings in order to provide additional
    flexibility.  Therefore the following functions are provided.  The
    typical user interface builder will make this single call:

	WcAllowDuplicateRegistration( TRUE );

    which allows class pointers, class names, constructors, and callbacks
    to be changed during execution.
*/

void WcAllowDuplicateRegistration( allowed )
    int allowed;
{
    allowDuplicateClassPtrReg = allowed;
    allowDuplicateClassNameReg = allowed;
    allowDuplicateConstructorReg = allowed;
    allowDuplicateCallbackReg = allowed;
}
void WcAllowDuplicateClassPtrReg( allowed )
    int allowed;
{
    allowDuplicateClassPtrReg = allowed;
}
void WcAllowDuplicateClassNameReg( allowed )
    int allowed;
{
    allowDuplicateClassNameReg = allowed;
}
void WcAllowDuplicateConstructorReg( allowed )
    int allowed;
{
    allowDuplicateConstructorReg = allowed;
}
void WcAllowDuplicateCallbackReg( allowed )
    int allowed;
{
    allowDuplicateCallbackReg = allowed;
}

/*
    -- Register Class Pointer Name
*******************************************************************************
    This procedure adds class pointer name to our list of registered
    classes. Note that the class ptr name is effectively case insensitive
    as it is being quarkified.  However, one should register the class ptr
    names using the "standard" capitalization (whatever is in the reference
    manual for the widget set) as the name as registered is used for error
    messages.

   The registry is primarily used by CvtStringToClassPtr().
*/

void WcRegisterClassPtr ( app, name, class )
    XtAppContext        app;    /* not used (yet), must be present      */
    char*               name;   /* class ptr name, case insensitive     */
    WidgetClass         class;  /* Xt object class pointer              */
{
    char          *lowerCaseName;
    XrmQuark       quark;
    ClCacheRec    *rec;
    int            i;

    /* Might need to grow cache.  Note that growth increment is exponential:
    ** if lots of classes, don't need to keep realloc'ing so often.
    */
    if (classes_num >= classes_max )
    {
        classes_max += (classes_max ? classes_max : INCR_REGISTRY);
        classes_ptr  = (ClCacheRec*) XtRealloc((char*)classes_ptr,
                             sizeof(ClCacheRec) * classes_max);
    }

    /* See if this object has been registered.  Compare quarks.
    */
    lowerCaseName = WcLowerCaseCopy( name );
    quark = XrmStringToQuark ( lowerCaseName );
    XtFree ( lowerCaseName );

    for (i = 0 ; i < classes_num ; i++ )
    {
        if (classes_ptr[i].quark == quark)
        {
            /* already registered this class */
	    if ( allowDuplicateClassPtrReg )
	    {
		rec = &classes_ptr[i];	/* overwrite this ClCacheRec */
		goto found_rec;
	    }
            sprintf(msg,
            "WcRegisterClassPtr (%s) - Failed \n\
             Problem: Duplicate class registration ignored.",
             name );
            XtWarning( msg );
            return;
        }
    }

    rec = &classes_ptr[classes_num++];	/* New ClCacheRec to be filled */

found_rec:
    rec->quark = quark;
    rec->class = class;
    rec->name  = XtMalloc( strlen(name) + 1 );
    strcpy ( rec->name, name );
}

/*
    -- Register Class Name
*******************************************************************************
    This procedure adds a class name to our list of registered
    classes. Note that the class name is effectively case insensitive
    as it is being quarkified.  However, one should register the class
    names using the "standard" capitalization (whatever is in the reference
    manual for the widget set) as the name as registered is used for error
    messages.

   The registry is primarily used by CvtStringToClassName().
*/

void WcRegisterClassName ( app, name, class )
    XtAppContext	app;	/* not used (yet), must be present      */
    char*		name;	/* class name, case insensitive   	*/
    WidgetClass		class;	/* Xt object class pointer              */
{
    char           *lowerCaseName;
    XrmQuark	    quark;
    ClNameCacheRec *rec;
    int		    i;

    /* Might need to grow cache.  Note that growth increment is exponential:
    ** if lots of classes, don't need to keep realloc'ing so often.
    */
    if (cl_nm_num >= cl_nm_max )
    {
	cl_nm_max += (cl_nm_max ? cl_nm_max : INCR_REGISTRY);
	cl_nm_ptr  = (ClNameCacheRec*) XtRealloc((char*)cl_nm_ptr, 
                             sizeof(ClNameCacheRec) * cl_nm_max);
    }

    /* See if this object has been registered.  Compare quarks.
    */
    lowerCaseName = WcLowerCaseCopy( name );
    quark = XrmStringToQuark ( lowerCaseName );
    XtFree ( lowerCaseName );

    for (i = 0 ; i < cl_nm_num ; i++ )
    {
	if (cl_nm_ptr[i].quark == quark)
	{
	    /* already registered this class */
	    if ( allowDuplicateClassNameReg )
	    {
		rec = &cl_nm_ptr[i];	/* overwrite this ClNameCacheRec */
		goto found_rec;
	    }
	    sprintf(msg, 
            "WcRegisterClassName (%s) - Failed \n\
             Problem: Duplicate class registration ignored.",
             name );
            XtWarning( msg );
	    return;
	}
    }

    rec = &cl_nm_ptr[cl_nm_num++];	/* New ClNameCacheRec to be filled */

found_rec:
    rec->quark = quark;
    rec->class = class;
    rec->name  = XtMalloc( strlen(name) + 1 );
    strcpy ( rec->name, name );
}

/*
    -- Register constructor
*******************************************************************************
    This procedure adds constructor procedure/name to our list of registered
    constructors. Note that the name is effectively case insensitive
    as it is being quarkified.  However, one should register the 
    names using the "standard" capitalization (whatever is in the reference
    manual for the widget set) as the name as registered is used for error
    messages.

    Note the constructor is a "Motif Style" widget creation routine,
    commonly called a "confusion function."  

   The registry is primarily used by CvtStringToConstructor().
*/

void WcRegisterConstructor ( app, name, constructor )
    XtAppContext app;	        /* not used (yet), must be present      */
    char*        name;	        /* constructor name, case insensitive   */
    Widget (*constructor) ();   /* pointer to a widget creation routine */
{
    char          *lowerCaseName;
    XrmQuark       quark;
    ConCacheRec   *rec;
    int		   i;

    /* Might need to grow cache.  Note that growth increment is exponential:
    ** if lots of constructors, don't need to keep realloc'ing so often.
    */
    if (constrs_num >= constrs_max )
    {
        constrs_max += (constrs_max ? constrs_max : INCR_REGISTRY);
        constrs_ptr  = (ConCacheRec*) XtRealloc((char*)constrs_ptr,
                             sizeof(ConCacheRec) * constrs_max);
    }

    /* See if this object has been registered.  Compare quarks.
    */
    lowerCaseName = WcLowerCaseCopy( name );
    quark = XrmStringToQuark ( lowerCaseName );
    XtFree ( lowerCaseName );

    for (i = 0 ; i < constrs_num ; i++ )
    {
        if (constrs_ptr[i].quark == quark)
        {
            /* already registered this class */
	    if ( allowDuplicateConstructorReg )
	    {
		rec = &constrs_ptr[i];	/* overwrite this ConCacheRec */
		goto found_rec;
	    }
            sprintf(msg,
            "WcRegisterConstructor (%s) - Failed \n\
             Problem: Duplicate constructor registration ignored.",
             name );
            XtWarning( msg );
            return;
        }
    }

    rec = &constrs_ptr[constrs_num++];	/* New ClCacheRec to be filled */

found_rec:
    rec->quark       = quark;
    rec->constructor = constructor;
    rec->name        = XtMalloc( strlen(name) + 1 );
    strcpy ( rec->name, name );
}

/*
  -- Register Callbacks
*******************************************************************************
    Register callback functions which can then be bound to widget
    callback lists by the string-to-callback converter 
    CvtStringToCallback().
*/

void WcRegisterCallback ( app, name, callback, closure )
    XtAppContext    app;        /* not used (yet), must be present      */
    String          name;       /* callback name, case insensitive      */
    XtCallbackProc  callback;   /* callback function pointer            */
    caddr_t	    closure;	/* default client data			*/
{
    char          *lowerCaseName;
    XrmQuark       quark;
    CBCacheRec    *rec;
    int		   i;

    /* Might need to grow cache.  Note that growth increment is exponential:
    ** if lots of constructors, don't need to keep realloc'ing so often.
    */
    if (callbacks_num >= callbacks_max )
    {
        callbacks_max += (callbacks_max ? callbacks_max : INCR_REGISTRY);
        callbacks_ptr  = (CBCacheRec*) XtRealloc((char*)callbacks_ptr,
                             sizeof(CBCacheRec) * callbacks_max);
    }

    /* See if this callback has been registered.  Compare quarks.
    */
    lowerCaseName = WcLowerCaseCopy( name );
    quark = XrmStringToQuark ( lowerCaseName );
    XtFree ( lowerCaseName );

    for (i = 0 ; i < callbacks_num ; i++ )
    {
        if (callbacks_ptr[i].quark == quark)
        {
            /* already registered this callback */
	    if ( allowDuplicateCallbackReg )
	    {
		rec = &callbacks_ptr[i];	/* overwrite this CBCacheRec */
		goto found_rec;
	    }
            sprintf(msg,
            "WcRegisterCallback (%s) - Failed \n\
             Problem: Duplicate callback registration ignored.",
             name );
            XtWarning( msg );
            return;
        }
    }

    rec = &callbacks_ptr[callbacks_num++];   /* New ClCacheRec to be filled */

found_rec:
    rec->quark    = quark;
    rec->callback = callback;
    rec->closure  = closure;
    rec->name     = XtMalloc( strlen(name) + 1 );
    strcpy ( rec->name, name );
}

/*
  -- Register Actions
*******************************************************************************
    A simple wrapper around XtAppAddActions().
    Register action procs which can then be bound to widget
    actions using standard Xt mechanisms.
*/

void WcRegisterAction(app, name, proc)
    XtAppContext app;
    String       name;
    XtActionProc proc;
{
    static XtActionsRec action_rec[] = {
        { (String)NULL, (XtActionProc)NULL }
    };

    action_rec[0].string = name;
    action_rec[0].proc = proc;
    XtAppAddActions(app, action_rec, 1);
}
