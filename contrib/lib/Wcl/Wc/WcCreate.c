/*
** Copyright (c) 1991 Martin Brunecky
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
* SCCS_data: @(#)WcCreate.c 1.1 ( 19 Nov 90 )
*
* Subsystem_group:
*
*     Widget Creation Library
*
* Module_description:
*
*     This module contains the functions used to create and manage 
*     a widget tree using the Xrm database.
*
*     The Xrm database format used to prescribe widget's children
*     is as follows:
*
*     ...parent.wcChildren:         childName1, childName2, ...
*
*     The type of each child must be specified.  There are three
*     ways of doing this: (1) by widget class (class pointer name),
*     (2) by widget class name, (3) by widget constructor  and
*     (4) by a template indicated by the top template widget.
*     function name.  The resource value given is case insensitive,
*     although one is encouraged to use the capitalization given
*     in the reference manuals simply on stylistic grounds.  The 
*     three examples below are effectively equivalent:
*
*     ...drawing.wcClass:	xmDrawingAreaWidgetClass
*     ...drawing.wcClassName:	XmDrawingArea
*     ...drawing.wcConstructor: XmCreateDrawingArea
*     ...drawing.wcTemplate:    ...template.a
*
*     Since there are multiple ways of specifying the widget type,
*     a precedence is defined: wcClass is the highest precedence,
*     and wcClass > wcClassName > wcConstructor > wcTemplate.
*
*     Currently, the Motif widget set defines many constructors, such 
*     as XmCreateForm or XmCreateFileSelectionBox.  Constructors are 
*     somtimes called Convenience Functions or even Confusion Functions.  
*     It is highly recommended that you use the WcTrace resource when
*     first using Constructors in order to see the resulting widget
*     heirarchy more clearly.  The WcTrace resource is specified as:
*
*     ...drawing.wcTrace:	True
*
*     By default, all widgets created from the Xrm resource database
*     are managed.  In some cases, this is not what is desired: for
*     example, pop-up menus are generally not managed when they are
*     created, rather they are managed due to some mouse event.  In
*     such cases, the WcManaged resource should be set False, as below:
*
*     *fileMenu.wcManaged:	False
*
*     It is possible to bind one or more callback functions to the
*     creation of a widget by using the WcCallback resource.  For 
*     example, using the Motif widget set, when a menu bar is created 
*     as a child of an XmMainWindow, the menuBar resource of the 
*     XmMainWindow needs to be set to refer to the menu bar.  For 
*     example:
*
*     App.main.wcClassName:	XmMainWindow
*     App.main.wcChildren:	mbar
*     *mbar.wcConstructor:	XmCreateMenuBar
*     *mbar.wcCallback:		WcSetResourceCB( *main.menuBar: this)
*
*     Sometimes widget heirarchies are dynamic: many of the widgets
*     are created at initialization time, but others need to be created
*     at run time.  Simply do not include the names of the dynamic
*     created widgets in any wcChildren resource.  They will then still
*     be defined in the Xrm database, but they will not be created
*     during the initial widget tree creation.
*
*     The WcDeferred resource is an obsolete mechanism which should no
*     longer be used.
*
*     For example, let's say your interface includes a box world, where
*     the user can create an arbitrary number of various types of boxes:
*
*     *box_world.wcClass:	MyBoxWorldWidgetClass
*
*     *box_type1.wcClass:	MyBoxType1WidgetClass
*     *box_type2.wcClass:	MyBoxType2WidgetClass
*     *box_type3.wcClass:	MyBoxType3WidgetClass
*     *box_type3.wcChildren:	child1, child2, child3, child4
*
*     *button1.callback:	WcCreateChildrenCB( *box_world, box_type1 )
*     *button2.callback:	WcCreateChildrenCB( *box_world, box_type2 )
*     *button3.callback:	WcCreateChildrenCB( *box_world, box_type3 )
*
*     Initially, when box_world is created, it will have no children.
*     Each time button1 is pressed, another instance of box_type1 will
*     be created as a child of box_world, and managed (since wcManaged
*     is by default True).  Similarly, everytime button2 is pressed
*     another box_type2 will be created, and everytime button3 is
*     pressed another box_type3 will be created, along with children
*     named child1, child2, child3, and child4, and their children as 
*     applicable.
*
*     User interfaces are often rather complex.  Since the Widget
*     Creation Library allows much more of the interface to be
*     specified in resource files than in prehistoric days of
*     Widget application programming, it becomes necessary to allow
*     interfaces to be specified in a collection of resource files.
*     
*     It is very desirable that anything in the interface can be
*     tailored by the user.  Therefore, each resource filename
*     loaded to describe an interface must be searched for in 
*     all the locations defined by the Xt Intrinsics.  This is
*     exactly the same as the initial resource file.
*
*     Since a resource file may override any of the resources on
*     a given widget, including other creation resources, the
*     resource file name is first fetched before any of the other
*     wc* resources.
*
*     A resource file name is specified as follows:
*
*     *menuBar.wcResFile:	MenuBar
*
*     Each of the standard directories will be searched in order
*     for a file named MenuBar, and each merged into the resource database.
*
*     The following is the complete set of resources which are 
*     interpreted by the Widget Creation Library:
*
*     ...widget.wcChildren:	childName1, childName2, ...
*     ...widget.wcClass:	classPointerName
*     ...widget.wcClassName:	className
*     ...widget.wcTemplate:     template widget path
*     ...widget.wcConstructor:  constructorFunctionName
*     ...widget.wcTrace:	true/false (default = false)
*     ...widget.wcManaged:	true/false (default = true)
*     ...widget.wcCallback:	callback1(args), callback2(args), ...
*     ...widget.wcResFile:	filename
*
*     In all cases, the Widget Creation resource names can be
*     specified as Wc<name> or wc<name>, with the capitalized
*     form having looser binding (representing the resource class).
*
*     Example:
*
*		HelloWorld.wcChildren:  push
*
*		*push.wcClass:          xmPushButtonWidgetClass
*		*push.labelString:      Hello World
*		*push.activateCallback: WcExitCB(1)
*
*     Since (for portability reasons) we can not assume runtime binding,
*     all widget classes or creation routines (constructors) must be 
*     "registered"  by the application BEFORE widget tree creation.
*
*     The widget tree creation is performed by the WcCreateDatabaseChild()
*     function, which descends the widget tree recursively until no more
*     children are found, or a non-composite widget/object is found.
*
*     Several convenience callbacks are provided with the package, allowing 
*     deferred widget creation, control (manage/unmanage) and other utility
*     functions.  These are found in WcCallb.c
*
* Module_history:
*                                                  
*   mm/dd/yy  initials  function  action
*   --------  --------  --------  ---------------------------------------------
*   25Aug91   J.Cook    Create..  Returns status
*   09/23/91  MarBru    Creation  Added support for template instancing
*   13Aug90   D.Smyth	Got rid of WcDefered stuff
*   11Jul90   D.Smyth   Added wcResFile resource
*   30Jun90   R.Whitby	WcWidgetCreation added call to WcRegisterWcActions
*   19Jun90   D.Smyth	Version 1.0
*   04/18/90  MarBru    many..    Changed w->core.name to XrmQuarkToString...
*   03/27/90  MarBru    Creation  Converted to a completely new syntax
*   02/16/90  MarBru    Create..  Limited creation to composite widgets/objects
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

/*  -- Operating system includes */
#include <stdio.h>

/*  -- X Window System includes */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

/*  -- Widget Creation Includes */
#include "WcCreate.h"
#include "WcCreateP.h"

#ifdef PRE_R4
#  ifdef XtSpecificationRelease
#    if XtSpecificationRelease > 3
#       include "Forced error: Xt release >3, Makefile claims PRE_R4"
#    endif
#  endif
#endif

#ifdef PRE_R5
#  ifdef XtSpecificationRelease
#    if XtSpecificationRelease > 4
#       include "Forced error: Xt release >4, Makefile claims PRE_R5"
#    endif
#  endif
#endif


/*
*******************************************************************************
* Private_data_definitions.
*******************************************************************************
*/

static char     msg[MAX_ERRMSG];

/*  -- Creation resources 
	NOTE: All resource values are fetched at once, but if WcResFile
	is specified, then the resource files must be loaded and then
	the resources re-fetched.  This is an optimization: usually
	WcResFile is not specified, so two fetches from the Xrm database
	do not need to be done in the normal case.  If resource files are
	loaded and the resources are re-fetched, WcResFile is ignored.
	NOTE: The WcClass and WcClassName resources are both converted to
        a class pointer, as we can construct useful error messages using:
		class->core_class.class_name
	However, the Constructor must be the entire constructor cache
	record so we have the name of the constructor for the error
	messages.
	NOTE: WcClass and WcClassName write to different ResourceRec
	members, so we can provide better error messages.
*/

XtResource wc_resources[] =
  {
    { WcNwcResFile,	WcCWcResFile,		XtRString,	sizeof(String),
      XtOffset(ResourceRecPtr, resFile ),	XtRImmediate,	(caddr_t) NULL
    },
    { WcNwcChildren,	WcCWcChildren,		XtRString,	sizeof(String),
      XtOffset(ResourceRecPtr, children ),	XtRImmediate,	(caddr_t) NULL
    },
    { WcNwcPopups,	WcCWcPopups,		XtRString,	sizeof(String),
      XtOffset(ResourceRecPtr, popups ),	XtRImmediate,	(caddr_t) NULL
    },
    { WcNwcClass,	WcCWcClass,		WcRClassPtr,	sizeof(caddr_t),
      XtOffset(ResourceRecPtr, class ),		XtRImmediate,	(caddr_t) NULL
    },
    { WcNwcClassName,	WcCWcClassName,		WcRClassName,	sizeof(caddr_t),
      XtOffset(ResourceRecPtr, classFromName ),	XtRImmediate,	(caddr_t) NULL
    },
    { WcNwcConstructor,	WcCWcConstructor, 	WcRConstructor,	sizeof(caddr_t),
      XtOffset(ResourceRecPtr, constructor ),	XtRImmediate,	(caddr_t) NULL
    },
    { WcNwcTemplate,    WcCWcTemplate,          XtRString,      sizeof(String),
      XtOffset(ResourceRecPtr, template ),      XtRImmediate,   (caddr_t) NULL
    },
    { WcNwcManaged,	WcCWcManaged,		XtRBoolean,	sizeof(Boolean),
      XtOffset(ResourceRecPtr, managed),	XtRImmediate,	(caddr_t) TRUE
    },
    { WcNwcTrace,	WcCWcTrace,		XtRBoolean,	sizeof(Boolean),
      XtOffset(ResourceRecPtr, trace),		XtRImmediate,	(caddr_t) FALSE
    },
    { WcNwcCallback,	WcCWcCallback,	XtRCallback,	sizeof(XtCallbackList),
      XtOffset(ResourceRecPtr, callback ),	XtRImmediate,	(caddr_t) NULL
    }
  };


/* Data structures passed to CopyEntry routine */
typedef struct _CopyData
        {
        int          tight;    /* number of tigh-bound source prefix quarks */
        XrmQuarkList target;   /* target prefix quark list                  */
        XrmDatabase  db;       /* target resource database                  */
        String       name;     /* NULL or name to replace %n in strings     */
        String       parent;   /* NULL or parent path to replace %p         */
        }
        CopyData;

/*
*******************************************************************************
* Private_function_declarations.
*******************************************************************************
*/

#define TRACE_CREATION 1
#define TRACE_POPUP 2

#ifdef FUNCTION_PROTOTYPES
typedef Widget (*CreateFunc)( ResourceRec*, Widget, char* nm, char* callr );
static void   GetWcResources( ResourceRec*, Widget, char* nm, char* class );
static int    DuplicateType ( ResourceRec*, Widget, char* nm, char* callr );
static Bool   CopyEntry     ( XrmDatabase, XrmBindingList, XrmQuarkList, 
                              XrmRepresentation, XrmValuePtr, XPointer);
static Widget CreateTemplateInstance ( Widget, char*, ResourceRec*, 
                              CreateFunc, char* );
static Widget CreateWidget  ( ResourceRec*, Widget, char* nm, char* callr );
static Widget CreatePopup   ( ResourceRec*, Widget, char* nm, char* callr );
static void   CreationWarningMessage ( ResourceRec* rp, Widget pw, 
			char* name, char* caller, char* func );
static void   PrintCreationTrace   ( ResourceRec*, Widget child, int traceTyp );
static void   CallCreationCallbacks( ResourceRec*, Widget child );
#else
typedef Widget (*CreateFunc)(); 
static void   GetWcResources();
static int    DuplicateType ();
static Bool   CopyEntry     ();
static Widget CreateTemplateInstance();
static Widget CreateWidget();
static Widget CreatePopup();
static void   CreationWarningMessage();
static void   PrintCreationTrace();
static void   CallCreationCallbacks();
#endif

/*
*******************************************************************************
* Public_function_declarations.
*******************************************************************************
*/

/*
    -- Create Widget Tree from Xrm Database
*******************************************************************************
    This routine creates widget children as defined in Xrm database.
    It checks the widget resource "WcChildren", which is a list of
    names of children to create. Each child must then be further defined
    in the databse.

    This function is frequently called from an application's main()
    procedure after the application shell is created via XtInitialize().

    Note that this function registers the converters for StringToWidget,
    StringToCallback, and so forth.

    Function now returns status, so application can take some action
    if widget creation failed.
*/

int WcWidgetCreation ( root )
    Widget       root;
{
    XtAppContext app = XtWidgetToApplicationContext( root );
    char*	 fullName = WcWidgetToFullName( root );	/* must be XtFree'd */
    int          status   = 0;
    ResourceRec  res;

    /* register the root of this widget */
    (void)WcRootWidget(root);

    /* register the Xt standard widgets */
    WcRegisterIntrinsic ( app );

    /* register the Wc converters */
    WcAddConverters( app );

    /* register the Wc callbacks */
    WcRegisterWcCallbacks ( app );

    /* register the Wc actions */
    WcRegisterWcActions ( app );

    if ( XtIsSubclass( root, compositeWidgetClass ) )
    {
        XtGetApplicationResources ( root, &res,
              wc_resources, XtNumber(wc_resources), NULL, 0 );
    
	if ( res.popups || res.children )
	{
	    if ( res.popups )
		WcCreateNamedPopups ( root, res.popups );
            if ( res.children )
		WcCreateNamedChildren ( root, res.children );
	}
        else
        {
           sprintf( msg,
           "WcWidgetCreation (%s) - Failed \n\
           Problem:  No children defined in Xrm database.\n\
	   Possible: resource file not found (check XAPPLRESDIR,XENVIRONEMENT),\n\
	   Possible: top level widget in resource file not named %s",
           fullName, root->core.name );
           XtWarning( msg );
           status = 1;
	}
    }

    else
    {
        sprintf( msg,
            "WcWidgetCreation (%s) - Failed \n\
             Problem: %s is not a composite widget, cannot have children.",
             fullName, fullName );
        XtWarning( msg );
        status = 1;
    }

    XtFree ( fullName );
    return ( status );
}

/*
    -- Create And Manage Named Children from Xrm Database
*******************************************************************************
    This function creates widget's children specified by names list,
    by calling WcCreateDatabaseChild() for each of the names provided.

    All the children are then managed, unless WcManaged resource is FALSE.

    Note that widgets created by WcCreateDatabaseChild may or may not
    be children of `pw' due to the use of constructors.  Only children
    of `pw' may be managed via a call to XtManageChildren().  Other
    widgets must be managed individually.  Usually, these widgets
    are created by the XmCreateScrolled*() or XmCreate*Dialog confusion 
    functions.
*/

void WcCreateNamedChildren ( pw, names )
    Widget      pw;         /* children's parent                            */
    char*       names;      /* (list of) widget names to create             */
{
    Widget	child;
    int		children = 0;
    Widget	widget_children[MAX_CHILDREN];
    int		other = 0;
    Widget	widget_other[MAX_CHILDREN];
    char	cleanName[MAX_XRMSTRING];
    char*	next;
    int		managed;
    int		i;

    if  ( !names ) return;

    next = WcCleanName( names, cleanName );

    while ( cleanName[0] )
    {
	child = WcCreateDatabaseChild ( pw, cleanName, &managed );
	if ( child )
	{
	    if ( managed && (XtParent( child ) == pw ) )
		widget_children[children++] = child;
	    else if ( managed )
		widget_other[other++] = child;
	}
	next = WcSkipWhitespace_Comma( next );
	next = WcCleanName( next, cleanName );
    }

    if ( children ) 
	XtManageChildren( widget_children, children );

    for (i = 0 ; i < other ; i++)
	XtManageChild( widget_other[i] );
}

/*
    -- Create Named Popup Children from Xrm Database
*******************************************************************************
    This function creates widget's popup children specified by names list,
    by calling WcCreateDatabasePopup() for each of the names provided.
*/

void WcCreateNamedPopups ( pw, names )
    Widget      pw;         /* children's parent                            */
    char*       names;      /* (list of) names of popup widgets to create   */
{
    Widget      child;
    char        cleanName[MAX_XRMSTRING];
    char*       next;

    if  ( !names ) return;

    next = WcCleanName( names, cleanName );

    while ( cleanName[0] )
    {
        child = WcCreateDatabasePopup ( pw, cleanName );
        next = WcSkipWhitespace_Comma( next );
        next = WcCleanName( next, cleanName );
    }
}

/*
    -- Create Widget from the Resource Database
*******************************************************************************
    WcCreateDatabaseChild() causes a normal child to be created.  This
    implies that the parent widget is a composite widget.
*/

Widget WcCreateDatabaseChild ( pw, name, managed  )
    Widget      pw;         /* child's parent */
    char*       name;       /* child name to create */
    int        *managed;    /* returned T/F: this child to be managed ? */
{
    ResourceRec res;		/* child's creation resources	*/
    Widget      child;		/* what we create		*/
    char*	className;	/* get after child is created	*/

    *managed = FALSE;		/* Assume error */

    if ( !XtIsSubclass( pw, compositeWidgetClass ) )
    {
	/* child is not a manager widget, so cannot create children */
	char* parentName = WcWidgetToFullName( pw );
	sprintf( msg,
            "WcCreateDatabaseChild (%s) - children ignored \n\
             Problem: %s is not a composite, cannot have children.",
             parentName, parentName );
	XtWarning( msg );
	XtFree( parentName );
	return (Widget)NULL;
    }

    /* As we don't have the class name, use the instance name only here */ 
    GetWcResources		( &res, pw, name, name );
    if (DuplicateType		( &res, pw, name, "WcCreateDatabaseChild" ))
	return (Widget)NULL;

    child = CreateWidget	( &res, pw, name, "WcCreateDatabaseChild" );
    if (!child)
        return (Widget)NULL;

    /* Having a widget, we can now pick up class resources */
    className = child->core.widget_class->core_class.class_name;
    GetWcResources		( &res, pw, name, className );
    

    if (res.trace)
	PrintCreationTrace	( &res, child, TRACE_CREATION );
    if (res.callback)
	CallCreationCallbacks	( &res, child );
    if (res.popups)
	WcCreateNamedPopups	( child, res.popups );
    if (res.children)
	WcCreateNamedChildren	( child, res.children );

    *managed = res.managed;
    return child;
}

/*
    -- Create Popup Widget from the Resource Database
*******************************************************************************
    WcCreateDatabasePopup() causes a popup widget to be created.  Any
    widget can have popup chidren.
*/

Widget WcCreateDatabasePopup ( pw, name )
    Widget      pw;         /* child's parent */
    char*       name;       /* child name to create */
{
    ResourceRec res;            /* child's creation resources   */
    Widget      child;          /* what we create               */
    char*	className;	/* get after child is created	*/

    /* As we don't have the class name, use the instance name only here */ 
    GetWcResources		( &res, pw, name, name );
    if (DuplicateType		( &res, pw, name, "WcCreateDatabasePopup" ))
	return (Widget)NULL;

    child = CreatePopup		( &res, pw, name, "WcCreateDatabasePopup" );
    if (!child)
        return (Widget)NULL;

    /* Having a widget, we can now pick up class resources */
    className = child->core.widget_class->core_class.class_name;
    GetWcResources		( &res, pw, name, className );

    if (res.trace)
	PrintCreationTrace	( &res, child, TRACE_POPUP );
    if (res.callback)
	CallCreationCallbacks	( &res, child );
    if (res.popups)
	WcCreateNamedPopups	( child, res.popups );
    if (res.children)
	WcCreateNamedChildren	( child, res.children );

    return child;
}

/*
******************************************************************************* 
**  Private functions used by WcCreateDatabaseChild and WcCreateDatabasePopup
*******************************************************************************
*/

/*
    -- Get Wc* Resources for named child of pw from Xrm resource database
*******************************************************************************
*/

static void GetWcResources( rp, pw, name, className )
    ResourceRec* rp;
    Widget       pw;
    char*        name;
    char*        className;
{
    /* Get creation resources for the child to be created.
    ** After this XtGetSubresources() call, the resource structure `res'
    ** contains resources specified in the Xrm database or the defaults.
    */
    XtGetSubresources ( pw, rp, name, className, 
       wc_resources, XtNumber(wc_resources), NULL, 0 );

    /* if a resource file is specified for this widget, first
    ** load the resource file, then re-fetch the resources.
    ** Notice that we don't check for resFile again.
    */
    if ( rp->resFile )
    {
	WcLoadResourceFileCB ( pw, rp->resFile, NULL );
	XtGetSubresources ( pw, rp, name, className,
	   wc_resources, XtNumber(wc_resources), NULL, 0 );
    }
}

/*
    -- See if duplicate type resources are specified for this widget
*******************************************************************************
   Exactly one of WcClass, WcClassName, WcConstructor or WcTemplate must be
   specified in order to create a widget.  If none are specified,
   or if more than one is specified, then this is an error, and
   this function returns TRUE, and the widget should not then be
   created.
*/

static int DuplicateType( rp, pw, name, caller )
    ResourceRec* rp;
    Widget	 pw;
    char*	 name;
    char*        caller;
{
    /* Booleans */
    int class       = (NULL != rp->class);
    int className   = (NULL != rp->classFromName);
    int constructor = (NULL != rp->constructor);
    int template    = (NULL != rp->template);
    char* parentName;
    
    if ( class + className + constructor + template == 0 )
       {
       char* reason = "no WcClass, WcClassName, WcConstructor nor WcTemplate";
       parentName   = WcWidgetToFullName( pw );
       sprintf( msg,
       "%s (%s.%s) - Failed (undefined) \n\
       Problem: %s specified, \n\
                child %s.%s not created.",
                caller, parentName, name, reason, parentName, name );
       XtWarning( msg );
       XtFree( parentName );
       return(1);
       }
    else if ( class + className + constructor + template > 1  && rp->trace )
       {
       parentName = WcWidgetToFullName( pw );
       fprintf(stderr,"Wc    notice: %s.%s class defined non-uniquely\n",
               parentName, name );
       XtFree( parentName );
       }
    return (0);
}
/*
  -- PrintBindingQuarkList in the X resource specification format: a.b*c
*******************************************************************************
*/
#ifdef DEBUG
static void PrintBindingQuarkList(bindings, quarks, stream)
    XrmBindingList      bindings;
    XrmQuarkList        quarks;
    FILE                *stream;
{
    Bool        firstNameSeen;

    for (firstNameSeen = False; *quarks; bindings++, quarks++) {
        if (*bindings == XrmBindLoosely) {
            (void) fprintf(stream, "*");
        } else if (firstNameSeen) {
            (void) fprintf(stream, ".");
        }
        firstNameSeen = True;
        (void) fputs(XrmQuarkToString(*quarks), stream);
    }
}
#endif /* DEBUG */
/*
    -- Copy Database Entry 
*******************************************************************************
   This function used in conjunction with XrmEnumerateDatabase copies selected
   database entries into another database. Originally, I copied data directly
   into the source database, but the X11R5 code had problems with that -).

   In addition to selection provided by XrmEnumerateDatabase, this function 
   filters out any entries which do not satisfy the requirement of "n" tight
   bindings at the beginning of the resource path.
*/
static Bool CopyEntry(db, bindings, quarks, type, value, data)
    XrmDatabase         *db;
    XrmBindingList      bindings;
    XrmQuarkList        quarks;
    XrmRepresentation   *type;
    XrmValuePtr         value;
    XPointer            data;
{
    static XrmQuark     StringQuark = NULLQUARK;
    XrmValuePtr         new_val = value;
    CopyData*           cd      = (CopyData*) data;
    XrmQuarkList        target  = cd->target;
    XrmQuark            new_quarks[400];
    XrmBinding          new_binds[400];
    XrmValue            loc_val;
    register  int       i,num;

    if ( StringQuark == NULLQUARK )
         StringQuark  = XrmStringToQuark ( "String" );

    /* I ONLY want resources with tight binding above my copy point */
    for (i=0; i<cd->tight; i++)
        if ( quarks[i] == NULLQUARK || bindings[i] != XrmBindTightly )
           return( False );

    /* Concatenate the target leading path and the path below the src. point */
    num = 0;
    while ( cd->target[num] )
          {
          new_quarks[num] = target[num];
          new_binds [num] = XrmBindTightly;
          num++;
          }
    for ( i = cd->tight; quarks[i] != NULLQUARK; i++ )
          {
          new_quarks[num] = quarks[i];
          new_binds [num] = bindings[i];
          num++;
          }
    new_quarks[num] = NULLQUARK;
    new_binds[num]  = XrmBindTightly;

    /* Perform string substitution in String resources, if specified 
    ** %n substituted by instance name,
    ** %p substituted by parent name
    */
    if ( (cd->name || cd->parent) && *type == StringQuark)
       {
       extern char* index();
       char *old = (char*)value->addr;
       char *ind = index ( old, '%' );
       if ( ind )
          {
          char     new[MAX_XRMSTRING];
          char    *n, *src, *tgt;

          for ( src = old, tgt = new; *src; src++ )
              {
              if ( *src == '%' )
                 {
                 if ( src[1] == '%' )          /* " %% - skipped " */
                    {
                    *tgt++ = *src++;
                    *tgt++ = *src;
                    }
                 if ( src[1] == 'n' )          /* " %n - name      */
                    {
                    for ( n=cd->name;   n && *n; n++ ) *tgt++ = *n;
                    src++;
                    }
                 if ( src[1] == 'p' )          /* " %p - parent    */
                    {
                    for ( n=cd->parent; n && *n; n++ ) *tgt++ = *n;
                    src++;
                    }
                 }
              else
                *tgt++ = *src;
              }
              *tgt = '\0';

          loc_val.addr = new;
          loc_val.size = strlen(new)+1;
          new_val  = &loc_val;
          }
       } 
          
    /* Load resource into the target database */
#   ifdef DEBUG
    PrintBindingQuarkList(new_binds, new_quarks, stdout);
    printf( ":  %s\n", new_val->addr);
#   endif
    XrmQPutResource ( &cd->db, new_binds, new_quarks, *type, new_val );

    return ( False );
}


/*
    -- Create child widget using template definition
*******************************************************************************
   To avoid picking up tons of loose-bound resources, the template specification
   must be defined using tight bound names and/or classes. Loose bindings in
   template specifier are thus rejected as an error.
   Note:
   This code originally supported the concept of instancing into a "scratch"
   database. Unfortunatelly, I found it impossible to synchronize recursive
   children creation (originating underneath me) with which database to use
   for what.
   At any rate, the BEST approach to instancing would be ability to create
   SOFT LINKS in the database. Instead of copying template definition, only
   provide necessary DB links .....
*/
static Widget CreateTemplateInstance ( pw, name, rp, create_func, caller )
Widget pw;
char  *name;
ResourceRec *rp;
CreateFunc create_func;
char  *caller;
{
    char        clean_name [MAX_XRMSTRING];
    char       *parent_name = WcWidgetToFullName( pw );
    char       *template    = clean_name;
    Display    *dpy         = XtDisplay(pw);
    XrmDatabase db          = dpy->db;
    CopyData    cd; 
    Widget      child;
    XrmQuark    name_pref  [MAX_WIDGETS];
    XrmQuark    class_pref [MAX_WIDGETS];
    XrmQuark    target_pref[MAX_WIDGETS];
    char        templ_class[MAX_XRMSTRING];
    char        target     [MAX_XRMSTRING];
    char        new_caller [MAX_ERRMSG];
    char       *t;
    char       *tc;

    /* create an empty instancing database - put there something to be sure */
    cd.db = XrmGetStringDatabase ( "_scratch: scratch" );

    /* Clean off any leading/trailing garbage */
    (void) WcCleanName ( rp->template, template );

    /* Manufacture the template class path uppercasing first name letters */
    for ( t=template, tc=templ_class; *t; t++, tc++ )
        {
        if (  *t == '*' )   /* We have found LOOSE binding, violation ! */
           {
           sprintf( msg,"%s (%s.%s) - Failed \n\
             Problem: template %s contains loose bindings,\n\
                      child %s not created",
	              caller, parent_name, name, template, name);
           XtWarning( msg );
           XtFree (parent_name);
           return (Widget)NULL;
           }
        *tc = *t;
        if ( t == template || t[-1] == '.' ) 
           *tc = toupper (*t);
        }

    /* Manufacture the target widget path: parent_path.name */
    (void) sprintf ( target, "%s.%s", parent_name, name );

    /* Convert the template and target specifications into quark list */
    XrmStringToQuarkList ( template,    name_pref  );
    XrmStringToQuarkList ( templ_class, class_pref );
    XrmStringToQuarkList ( target,      target_pref);

    /* Count how many tight bindings are presented by the template specifier */
    for (cd.tight=0; name_pref[cd.tight]!=NULLQUARK; ) 
         cd.tight++;
        
    /* Copy the template definition into scratch DB under the target name */
    cd.target = target_pref;
    cd.name   = name;
    cd.parent = parent_name;

    (void)XrmEnumerateDatabase(db, name_pref, class_pref, XrmEnumAllLevels,
                               CopyEntry, (XPointer) &cd );

    /* Merge instance DB into def DB */
    XrmMergeDatabases(cd.db, &dpy->db); 
    /* the scratch database is destroyed by XrmMergeDatabases */

    /* Update resources for this child (assuming template provided wcClass) */
    GetWcResources		( rp, pw, name, name );

    /* Create the instance using definition(s) provided by the template */
    if ( !rp->class && !rp->classFromName && !rp->constructor )
       {
       sprintf( msg,"%s (%s.%s) - Failed \n\
             Problem:  template %s did not specify\n\
                       WcClass, WcClassName nor WcConstructor,\n\
                       child %s not created\n\
             Possible: Wrong template name ?",
	               caller, parent_name, name, template, name);
       XtWarning( msg );
       child = (Widget)NULL;
       }
    else
       {
       sprintf (new_caller, "%s from template %s", caller, template); 
       rp->template  = NULL;  /* prevent duplicite creator warning */
       if (DuplicateType         ( rp, pw, name, new_caller ))
	 child = (Widget)NULL;
       else
         child = (*create_func)  ( rp, pw, name, new_caller );
       }

    XtFree (parent_name);
    return ( child );
}

/*
    -- Create child widget using XtCreateWidget or constructor
*******************************************************************************
*/

static Widget CreateWidget ( rp, pw, name, caller )
    ResourceRec* rp;
    Widget       pw;
    char*        name;
    char*	 caller;
{
    Widget child;
    CreateFunc func = (CreateFunc)CreateWidget;

    if ( rp->class )
	child = XtCreateWidget ( name, rp->class, pw, NULL, 0 );
    else if ( rp->classFromName )
	child = XtCreateWidget ( name, rp->classFromName , pw, NULL, 0 );
    else if ( rp->constructor )
	child = rp->constructor->constructor( pw, name, NULL, 0 );
    else if ( rp->template )
        return (CreateTemplateInstance ( pw, name, rp, func, caller ));
    else
        child = (Widget)NULL;

    if (child)
	return child;
    CreationWarningMessage( rp, pw, name, caller, "XtCreateWidget" );
    return (Widget)NULL;
}

/*
    -- Create popup widget using XtCreateWidget or constructor
*******************************************************************************
*/

static Widget CreatePopup ( rp, pw, name, caller )
    ResourceRec* rp;
    Widget       pw;
    char*        name;
    char*        caller;
{
    Widget child;
    CreateFunc func = (CreateFunc)CreateWidget;

    if ( rp->class )
        child = XtCreatePopupShell ( name, rp->class, pw, NULL, 0 );
    else if ( rp->classFromName )
        child = XtCreatePopupShell ( name, rp->classFromName , pw, NULL, 0 );
    else if ( rp->constructor ) 
	child = rp->constructor->constructor( pw, name, NULL, 0 );
    else if ( rp->template )
        return (CreateTemplateInstance ( pw, name, rp, func, caller ));
    else
        child = (Widget)NULL;

    if (child)
        return child;
    CreationWarningMessage( rp, pw, name, caller, "XtCreatePopupShell" );
    return (Widget)NULL;
}

/*
    -- Print warning message when CreateWidget or CreatePopup fail
*******************************************************************************
   This function should never be called when template is used for creation,
   however, the template creation code will call this one when creation
   using template's class, class name or constructor fails
*/

static void CreationWarningMessage ( rp, pw, name, caller, func )
    ResourceRec* rp;
    Widget       pw;
    char*        name;
    char*        caller;
    char*	 func;
{
    char* parentName = WcWidgetToFullName( pw );
    static char funcWithArgs[1024];

    if ( rp->class || rp->classFromName )
    {
        sprintf( funcWithArgs, "%s( %s, %s )", 
                    func, name, rp->class->core_class.class_name );
    }
    else if ( rp->constructor ) 
    {
        sprintf( funcWithArgs, "%s( %s )", rp->constructor->name, name );
    }
    else
        strcpy ( funcWithArgs, "Wcl internals" );

    sprintf( msg,
            "%s (%s.%s) - Failed \n\
             Problem: %s failed.",
             caller, parentName, name, 
	     funcWithArgs );

    XtWarning( msg );
    XtFree( parentName );
}

/*
    -- Print creation trace message
*******************************************************************************
*/

static void PrintCreationTrace( rp, child, traceType )
    ResourceRec* rp;
    Widget       child;
    int		 traceType;
{
    char* childName = WcWidgetToFullName( child );
    char* trace;

    if (traceType == TRACE_CREATION)
	trace = (rp->managed) ? "  managed" : "unmanaged";
    else
	trace = "   pop-up";

    fprintf(stderr,"Wc %s: %s of class %s\n",
	trace,
	childName, 
	child->core.widget_class->core_class.class_name);

    XtFree( childName  );
}

/*
    -- Call creation time callbacks specified by WcCallbacks resource.
*******************************************************************************
*/

static void CallCreationCallbacks( rp, child )
    ResourceRec* rp;
    Widget       child;
{
    XtCallbackRec *cb = rp->callback;
    for ( ; cb->callback; cb++ )
        (*cb->callback)( child, cb->closure, NULL );
}
