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
* SCCS_data: @(#)WcConvert.c 1.1 ( 19 Nov 90 )
*
* Subsystem_group:
*
*     Widget Creation Library
*
* Module_description:
*
*     This module contains Xt converter functions which convert strings,
*     as found in the Xrm database, into useful types.  
*
*     It also contains the routine which registers all Wc converters.
*
*     The CvtStringToWidget converter takes a pathname which starts 
*     from the application shell an proceeds to a specific widget.  The
*     widget must already have been created.   Note that this converter
*     needs to be used INSTEAD of the XmuCvtStringToWidget which gets
*     registered by the Athena widgets.  The Xmu converter is not so
*     user friendly.  This means this file also declares an external
*     function XmuCvtStringToWidget() which is really CvtStringToWidget,
*     and this needs to be linked before Xmu.
*
*     The CvtStringToCallback converter parses the resource string in 
*     the format:
*
*       ...path:   name[(args)][,name[(args)]]...
*
*     where:  name:   specifies the registered callback function name
*             args:   specifies the string passed to a callback as
*		      "client data".
*
*     Multiple callbacks can be specified for a single callback list
*     resource.  Any callbacks must be "registered" by the application
*     prior converter invocation (.i.e.prior widget creation).
*     If no "args" string is provided, the default "client data" 
*     specified at callback registration are used.
*
*     The CvtStringToConstructor converter searches the Constructor
*     cache for a registered constructor.  
*
*     The CvtStringToClass converter searches the Class cache for a 
*     registered object (widget) class pointer name.
*
*     The CvtStringToClassName converter searches the ClassName cache
*     for a registered object (widget) class name.

*
* Module_interface_summary: 
*
*     Resource converter is invoked indirectly by the toolkit. The
*     converter is added to the toolkit by widgets calling
*     WcAddConverters() in the Wc intialization code.
*
* Module_history:
*                                                  
*   mm/dd/yy  initials  function  action
*   --------  --------  --------  ---------------------------------------------
*   09/25/91  MarBru    Cvt(s)    make sure converters return NULL on err.
*   09/25/91  MarBru    Cvt(s)    make most converters static 
*   06/08/90  D.Smyth   Class, ClassName, and Constructor converters.
*   05/24/90  D.Smyth   WcAddConverters created from something similar
*   04/03/90  MarBru    CvtStr..  Fixed argument termination with a NUL char
*   02/26/90  MarBru    All       Created
*
* Design_notes:
*
*   For VMS, we could have used LIB$FIND_IMAGE_SYMBOL and use dynamic
*   (runtime) binding. But since most UNIX systems lack such capability,
*   we stick to the concept of "registration" routines.
*
*   One time, I considered applying conversion to callback argument, which
*   would take the burden of conversion from the callback code (runtime)
*   to the callback  conversion (one time initialization). The problem is
*   that some conversions are widget context specific (color to pixel needs
*   display connection), and at the time of callback conversion I do not
*   have a widget. I could require the widget argument, but this would kill
*   caching of the conversion result.
*
*   The sequential search of the callback cache is far from optimal. I should
*   use binary search, or the R4 conversion cache.  I can't use the R4 cache
*   until Motif 1.1 is released which will (supposedly) run on R4 Intrinsics.
*   
*******************************************************************************
*/
/*
*******************************************************************************
* Include_files.
*******************************************************************************
*/

#include <ctype.h>	/* isupper() and tolower macros */
#include <stdio.h>

/*  -- X Window System includes */
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/Core.h>
#include <X11/CoreP.h>
#include <X11/StringDefs.h> 

/*  -- Widget Creation Library includes */
#include "WcCreate.h"
#include "WcCreateP.h"

/*
*******************************************************************************
* Private_constant_declarations.
*******************************************************************************
*/

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

#define done( type, value ) 			\
{						\
    if ( toVal->addr != NULL )			\
    {						\
	if ( toVal->size < sizeof( type ) )	\
	{					\
	    toVal->size = sizeof( type );	\
	    return;				\
	}					\
	*(type*)(toVal->addr) = (value);	\
    }						\
    else					\
    {						\
	static type static_val;			\
	static_val = (value);			\
	toVal->addr = (caddr_t)&static_val;	\
    }						\
    toVal->size = sizeof(type);			\
    return;					\
}

#define newDone( type, value )                  \
{                                               \
   if (toVal->addr != NULL) {                   \
      if (toVal->size < sizeof(type)) {         \
          toVal->size = sizeof(type);           \
          return False;                         \
      }                                         \
      *(type*)(toVal->addr) = (value);          \
   }                                            \
   else {                                       \
      static type static_val;                   \
      static_val = (value);                     \
      toVal->addr = (caddr_t)&static_val;       \
   }                                            \
   toVal->size = sizeof(type);                  \
   return True;                                 \
}


/*
*******************************************************************************
* Private_data_definitions.
*******************************************************************************
*/

/*
*******************************************************************************
* Private_function_declarations.
*******************************************************************************
*/

/*
    -- Convert String To ClassPtr
*******************************************************************************
    This conversion searches the Object Class cache for the appropriate
    Cache record.  The resource database string is simply the name
    of the class pointer, case insensitive.  The value provided is the
    widget class pointer, as passed to XtCreateWidget().
*/

static void CvtStringToClassPtr (args, num_args, fromVal, toVal )
    XrmValue  *args;
    Cardinal  *num_args;
    XrmValue  *fromVal;
    XrmValue  *toVal;
{
    WidgetClass none   = (WidgetClass)NULL;
    char*       string = (char *) fromVal->addr;
    char        cleanName[MAX_XRMSTRING];
    char*       lowerCase;
    XrmQuark    quark;
    int         i;

    (void)WcCleanName ( string, cleanName );
    lowerCase = WcLowerCaseCopy ( cleanName );
    quark = XrmStringToQuark ( lowerCase );
    XtFree ( lowerCase );

    for (i=0; i<classes_num; i++)
    {
        if ( classes_ptr[i].quark == quark )
            done( WidgetClass, classes_ptr[i].class );
    }
    XtStringConversionWarning (cleanName, "Object Class, not registered.");
    done( WidgetClass, none );
}

/*
    -- Convert String To ClassName
*******************************************************************************
    This conversion searches the Class Name cache for the appropriate
    Cache record.  The resource database string is simply the name
    of the class, case insensitive.  The value provided is the widget 
    class pointer, as passed to XtCreateWidget().
*/

static void CvtStringToClassName (args, num_args, fromVal, toVal )
    XrmValue  *args;
    Cardinal  *num_args;
    XrmValue  *fromVal;
    XrmValue  *toVal;
{
    WidgetClass none   = (WidgetClass)NULL;
    char*	string = (char *) fromVal->addr;
    char	cleanName[MAX_XRMSTRING];
    char* 	lowerCase;
    XrmQuark	quark;
    int		i;

    (void)WcCleanName ( string, cleanName );
    lowerCase = WcLowerCaseCopy ( cleanName );
    quark = XrmStringToQuark ( lowerCase );
    XtFree ( lowerCase );

    for (i=0; i<cl_nm_num; i++)
    {
        if ( cl_nm_ptr[i].quark == quark )
	    done( WidgetClass, cl_nm_ptr[i].class );
    }
    XtStringConversionWarning (cleanName, "Class Name, not registered.");
    done( WidgetClass, none );
}

/*
    -- Convert String To Constructor
*******************************************************************************
    This conversion searches the Constructor Cache for the appropriate
    Cache record.  The resource database string is simply the name
    of the constructor, case insensitive.  The value provided is a
    Contrstructor Cache Record.  The constructor (func ptr) itself is
    not provided, as the user of this value (generally WcCreateDatabaseChild)
    also likes to have the constructor name as registered for error messages.
*/

static void CvtStringToConstructor (args, num_args, fromVal, toVal)
    XrmValue *args;
    Cardinal *num_args;
    XrmValue *fromVal;
    XrmValue *toVal;
{
    ConCacheRec *none  = (ConCacheRec*)NULL;
    char*       string = (char *) fromVal->addr;
    char        cleanName[MAX_XRMSTRING];
    char*       lowerCase;
    XrmQuark    quark;
    int         i;

    (void)WcCleanName ( string, cleanName );
    lowerCase = WcLowerCaseCopy ( cleanName );
    quark = XrmStringToQuark ( lowerCase );
    XtFree ( lowerCase );

    for (i=0; i<constrs_num; i++)
    {
	if ( constrs_ptr[i].quark == quark )
           done( ConCacheRec*, &(constrs_ptr[i]) );
    }
    XtStringConversionWarning (cleanName, "Constructor, not registered.");
    done( ConCacheRec*, none );
}

/*
    -- Convert String To Callback
*******************************************************************************
    This conversion creates a callback list structure from the X resource
    database string in format:

    name(arg),name(arg).....

    Note "name" is not case sensitive, while "arg" may be - it is passed to
    a callback as client data as a null terminated string (first level
    parenthesis stripped off).  Even if nothing is specified e.g.,
    SomeCallback() there is a null terminated string passed as client
    data to the callback.  If it is empty, then it is the null string.

    Note also that the argument CANNOT be converted at this point: frequently,
    the argument refers to a widget which has not yet been created, or
    uses the context of the callback (i.e., WcUnmanageCB( this ) uses the
    widget which invoked the callback).  
*/

typedef struct
{
    char *name_start, *name_end;    /* callback name start, end */
    char *arg_start,  *arg_end;     /* argument string start, end */
} Segment;

#ifdef FUNCTION_PROTOTYPES
static int ParseCallbackString( char*, Segment*, int );
static XtCallbackRec* BuildCallbackList( Segment* );
static int GetCallbackAddrFromCache(char*, char*, XtCallbackRec* );
static void CopyArgsToCallbackClosure( char*, char*, XtCallbackRec* );
#else
static int ParseCallbackString();
static XtCallbackRec* BuildCallbackList();
static int GetCallbackAddrFromCache();
static void CopyArgsToCallbackClosure();
#endif

/*
    -- Convert String To Callback
*******************************************************************************
*/

static void CvtStringToCallback (args, num_args, fromVal, toVal)
    XrmValue *args;
    Cardinal *num_args;
    XrmValue *fromVal;
    XrmValue *toVal;
{
    XtCallbackRec* retval = (XtCallbackRec*)NULL;
    Segment	   name_arg_segments[MAX_CALLBACKS];	
    char*	   string = (char *) fromVal->addr;

    if (!ParseCallbackString( string, name_arg_segments, MAX_CALLBACKS ))
       {
       /* name_arg_segments[MAX_CALLBACKS-1].name_start MUST be NULL */
       retval = BuildCallbackList( name_arg_segments );
       }
    done ( XtCallbackRec*, retval );
}

/*
    -- Parse string into name+argument segments: returns 1 if failed
*******************************************************************************
*/

static int ParseCallbackString( string, seg, num_segs )
    char*	string;
    Segment*	seg;
    int		num_segs;
{
    register char* 	cp = string;
    register char* 	ws;
    register int	in_parens = 0;

/*  -- Don't need to do anything if there is nothing interesting in string */
    if ( cp == NULL) return 1;
    if (*cp == NULL) return 1;

    /* skip leading whitespace */
    while( *cp <= ' ' )
	if (*++cp == NULL) return 1;

/*  -- Always end the parse on a null to make life easier */
#define NEXT_CHAR if (*++cp == NULL) goto end_of_parse;
#define BARF(str) { XtStringConversionWarning( string, str ); return 1; }

    while (1)		/* parsing loop */
    {
	/* start new segment: we've got the start of the callback name,
	** and nothing else.
	*/
	seg->name_start = cp;	
        seg->name_end = seg->arg_start = seg->arg_end = (char*)NULL;


	/* take care of possible error - barf if *cp is left paren or comma */
	if ( *cp == '(' || *cp == ',' )
	    BARF( "Callback, name cannot start with `(' or `,'" )

	/* name is everything up to whitespace, a left paren, or comma */
	while(   ' ' < *cp    &&    *cp != '('    &&    *cp != ','   )
	    NEXT_CHAR

	seg->name_end = cp-1;	/* found end of the callback name */

	/* There may be whitespace between name and left paren or comma */
	while( *cp <= ' ' )
            NEXT_CHAR

	/* if we've found a left paren, collect the argument */
	if ( *cp == '(' )
	{
	    ws = cp;	/* actually points at initial left paren */
	    in_parens = 1;
	    while (in_parens)
	    {
		NEXT_CHAR	/* 1st time skips the initial left paren */
		if ( '(' == *cp ) in_parens++;
		if ( ')' == *cp ) in_parens--;
	    }
	    /* Now cp actually points at final right paren.  Lets get rid of
	    ** leading and trailing argument whitespace.  If only whitespace
	    ** then leave seg->arg_start == seg->arg_end == NULL 
	    */
	    do {
		ws++;	/* skip initial '(', will hit ')' at cp if no arg */
	    } while ( *ws <= ' ' );
	    if (ws != cp)
	    {
		seg->arg_start = ws; /* first non-whitespace of argument */
		ws = cp;	     /* now look at the final right paren */
		do {
		    ws--;
		} while ( *ws <= ' ' );
		seg->arg_end = ws;
	    }
	    NEXT_CHAR		/* skip final right paren */
	}
	/* Skip optional comma separator, then do next segment */
	while( *cp <= ' ' || *cp == ',' )
	    NEXT_CHAR

	seg++;
	/* NB: we must have space for following `NULL' segment */
	if ( --num_segs < 2 )
	    BARF( "Callback, Too many callbacks" );
    }

end_of_parse:
    /* Got here because we've detected the NULL terminator.  We
    ** could have hit the NULL at any of the "NEXT_CHAR" invocations.
    */

    if (seg->name_start && !seg->name_end)
	seg->name_end = cp-1;			/* callback name */
    else if ( in_parens )
	BARF( "Callback, Unbalanced parens in callback argument" )

    seg++; seg->name_start = (char*)NULL;	/* following seg is NULL */

    return 0; /* sucessful parse */
}

/*
    -- Build XtCallbackList based on name+arg segments
*******************************************************************************
*/

static XtCallbackRec* BuildCallbackList( seg )
    Segment* seg;
{
    XtCallbackRec	callback_list[MAX_CALLBACKS];	/* temporary */
    XtCallbackRec*	rec = callback_list;
    int                 callback_list_len = 0;
    int			bytes;

/*  -- process individual callback string segments "name(arg)" */
    for( ;  seg->name_start  ;  seg++ )
    {
	if ( GetCallbackAddrFromCache( seg->name_start, seg->name_end, rec ) )
	{
	    CopyArgsToCallbackClosure( seg->arg_start,  seg->arg_end,  rec );
	    rec++;
	    callback_list_len++;
	}
    }

/*  -- terminate the callback list */
    {
        rec->callback = NULL;
	rec->closure  = NULL;
	callback_list_len++;
    }

/*  -- make a permanent copy of the new callback list, and return a pointer */
    bytes = callback_list_len * sizeof(XtCallbackRec);
    rec = (XtCallbackRec*)XtMalloc( bytes );
    memcpy ( (char*)rec, (char*)callback_list,  bytes );
    return rec;
}

/*
    -- Find named callback in callback cache: TRUE if found
*******************************************************************************
    Searches the Callback cache, gets the address of the callback
    function, and the default closure data as registered.
*/

static int GetCallbackAddrFromCache( name_start, name_end, rec )
    char*          name_start;
    char*          name_end;
    XtCallbackRec* rec;
{
    static   char  lowerCaseName[MAX_XRMSTRING];
    register char* cp   = name_start;
    register char* dest = lowerCaseName;
    register int   i;
    XrmQuark       quark;

    /* our callback cache names are case insensitive */
    while( cp <= name_end )
        *dest++ = (isupper(*cp) ) ? tolower (*cp++) : *cp++;
    *dest = NUL;

    /* try to locate callback in our cache of callbacks */
    quark = XrmStringToQuark (lowerCaseName);

    for ( i=0 ; i < callbacks_num ; i++ )
    {
        if ( callbacks_ptr[i].quark == quark )
	{
	    rec->callback = callbacks_ptr[i].callback;
	    rec->closure  = callbacks_ptr[i].closure; /* default closure */
	    return 1;
	}
    }
    XtStringConversionWarning ( lowerCaseName,
	"Callback, unknown callback name");
    return 0;
}

/*
    -- Copy arguments from segment into XtCallbackList
*******************************************************************************
*/

static void CopyArgsToCallbackClosure( arg_start,  arg_end,  rec )
    char*          arg_start;
    char*          arg_end;
    XtCallbackRec* rec;
{
    register char *arg;
    register int   alen;
	   
    if ( arg_start )
    {
	/* arg in parens - pass as string replacing default closure */
	alen = 1 + (int)arg_end - (int)arg_start;
	arg  = XtMalloc(alen+1);
	strncpy ( arg, arg_start, alen );
	arg[alen]    = NUL;
	rec->closure = (caddr_t)arg;
    }
    else
    {
	/* no arg in parens.  Make sure closure is something -
	** do NOT return NULL in any event.  Causes SEGV too
	** easily (is this what Dave preaches to young fellows?).  
	*/
	if (rec->closure == NULL)
	    rec->closure = (caddr_t)"";
    }
}

/*
    -- Convert String To Widget
*******************************************************************************
    This conversion creates a Widget id from the X resource database string.
    The conversion will fail, and WcFullNameToWidget() will issue a warning,
    if the widget so named has not been created when this converter is called.
    For example, if a widget refers to itself for some reason, during
    its creation when this converter is called, it is not yet created: 
    therefore, the converter will fail.

    This converter is made extern to overload the Xmu converter
    (registered by Xaw widget set), as this one is more user friendly.
*/

static XtConvertArgRec name_to_widget_args[] = {
    { XtBaseOffset, (caddr_t)XtOffset(Widget,core.self), sizeof(Widget) },
    };

void XmuCvtStringToWidget (args, num_args, fromVal, toVal)
    XrmValue *args;
    Cardinal *num_args;
    XrmValue *fromVal;
    XrmValue *toVal;
{
    Widget wid;

    if (*num_args != 1)
        XtErrorMsg("wrongParameters", "cvtStringToWidget", "xtToolkitError",
                   "StringToWidget conversion needs parent arg", NULL, 0);

    wid  = WcFullNameToWidget(*(Widget*)(args[0].addr), fromVal->addr);
    if ( !wid )
      XtStringConversionWarning (fromVal->addr, "Widget - no such widget");

    done ( Widget, wid );
}

/*
    The new-style converter is provided only as an override for the
    Xaw provided one - to prvent Xaw widgets from using their new style
    flavor of less functional Xmu converter.
    It is not explicitly installed, because the old-style Xt do not have
    XtSetAppConverter
*/
/*ARGSUSED*/
Boolean XmuNewCvtStringToWidget(dpy, args, num_args, fromVal, toVal,
                                converter_data)
     Display *dpy;
     XrmValue *args;            /* parent */
     Cardinal *num_args;        /* 1 */
     XrmValue *fromVal;
     XrmValue *toVal;
     caddr_t  *converter_data;
{
    Widget wid;

    if (*num_args != 1)
        XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
                        "wrongParameters","cvtStringToWidget","xtToolkitError",
                        "String To Widget conversion needs parent argument",
                        (String *)NULL, (Cardinal *)NULL);

    wid = WcFullNameToWidget(*(Widget*)(args[0].addr), fromVal->addr);
    if ( !wid )
      XtStringConversionWarning (fromVal->addr, "Widget - no such widget");

    newDone ( Widget, wid );
}


/*
*******************************************************************************
* Public_function_declarations.
*******************************************************************************
*/

/*
    -- Add String To ... Convertors
*******************************************************************************
*/

void WcAddConverters ( app )
    XtAppContext app;
{
    ONCE_PER_XtAppContext( app );

    XtAddConverter       (XtRString,
                          WcRClassPtr,
                          CvtStringToClassPtr,
                          (XtConvertArgList)NULL,
                          (Cardinal)0);

    XtAddConverter       (XtRString,
                          WcRClassName,
                          CvtStringToClassName,
                          (XtConvertArgList)NULL,
                          (Cardinal)0);

    XtAddConverter       (XtRString,
                          WcRConstructor,
                          CvtStringToConstructor,
                          (XtConvertArgList)NULL,
                          (Cardinal)0);

    XtAddConverter       (XtRString, 
                          XtRCallback,
                          CvtStringToCallback,
                          (XtConvertArgList)NULL,
                          (Cardinal)0);

    XtAddConverter       (XtRString,
                          XtRWidget,
                          XmuCvtStringToWidget,
                          name_to_widget_args,
                          XtNumber(name_to_widget_args));
}
