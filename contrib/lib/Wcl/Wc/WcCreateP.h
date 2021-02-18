/*
** Copyright (c) 1991 Martin Brunecky
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
* SCCS_data: @(#)WcCreateP.h 1.1 ( 19 Nov 90 )
*
* Include_name:
*
*     WcCreateP.h
*
* Subsystem_group:
*
*     Widget Creation Library
*
* Include_description:
*
*     Private defines for the Widget Creation Library supporting widget
*     tree creation from the Xrm database.
*
* Include_history:
*
*   mm/dd/yy  initials  action
*   --------  --------  -------------------------------------------------------
*   09/23/91   marbru   Added R5 defines missing in R3,R4 
*   09/23/91   marbru   Added template resources
*   11/14/90   D.Smyth  Added popup resources
*   06/30/90   R.Whitby added Action declarations
*   05/24/90   D.Smyth  created from provate section of WsCreate.h
*   03/02/90   marbru   created
*
*******************************************************************************
*/
#ifndef _WcCreateP_h
#define _WcCreateP_h

/*
*******************************************************************************
* Private_constant_declarations.
*******************************************************************************
*/
#undef  NUL
#define NUL '\0'
#define MAX_XRMSTRING   4096		/* max length of the Xrm DB string  */
#define MAX_ERRMSG      1024		/* max length of error message      */
#define MAX_CHILDREN    1024		/* max number of widget's children  */
#define MAX_PATHNAME    1024		/* max length of the pathname       */
#define INCR_REGISTRY     32		/* incr of cl, con, cb registries   */
#define MAX_CALLBACKS     64            /* max callbacks per Xrm resource   */
#define MAX_WIDGETS      512		/* max depth of a widget tree       */
#define MAX_ROOT_WIDGETS  32		/* max # separate widget trees	    */
#define MAX_RES_FILES    512		/* max # res file names per interf  */

#define WcNwcResFile		"wcResFile"
#define WcNwcChildren		"wcChildren"
#define WcNwcPopups		"wcPopups"
#define WcNwcClass		"wcClass"
#define WcNwcClassName		"wcClassName"
#define WcNwcConstructor	"wcConstructor"
#define WcNwcTemplate           "wcTemplate"
#define WcNwcManaged		"wcManaged"
#define WcNwcTrace		"wcTrace"
#define WcNwcCallback		"wcCallback"

#define WcCWcResFile		"WcResFile"
#define WcCWcChildren		"WcChildren"
#define WcCWcPopups		"WcPopups"
#define WcCWcClass		"WcClass"
#define WcCWcClassName		"WcClassName"
#define WcCWcConstructor	"WcConstructor"
#define WcCWcTemplate           "WcTemplate"
#define WcCWcManaged		"WcManaged"
#define WcCWcTrace		"WcTrace"
#define WcCWcCallback		"WcCallback"

/* Motif 1.0 has a bug: widgets ask for Windows 
** instead of Widgets for their resources...
*/
#define WcRWidget		"Window"
#define WcRClassPtr		"ClassPtr"
#define WcRClassName		"ClassName"
#define WcRConstructor		"Constructor"

/* Definitions not available in X11R4,R3 
** Note we detect R5 absence by a single check only
*/
#ifndef XrmEnumAllLevels
#define XrmEnumAllLevels 0
#define XrmEnumOneLevel  1
typedef caddr_t  XPointer;
#endif

/*
*******************************************************************************
* Private_type_declarations.
*******************************************************************************
*/

typedef void   (*PtrFuncVoid)();	/* ptr to func returning void	*/
typedef Widget (*PtrFuncWidget)();	/* ptr to func returning Widget	*/

/* Registration structs: It is a good idea if the classes, class names,
** constructors, and callbacks are registered with the same upper & lower 
** case names as the names in the ref manuals and source files, as this 
** makes the user error messages clearer.
*/

typedef struct                          /* Class cache record		*/
{
    String	   name;		/* class ptr name as registered	*/
    XrmQuark       quark;               /* quarkified class ptr name	*/
    WidgetClass    class;               /* widget class pointer		*/
} ClCacheRec;

typedef struct                          /* Class Name cache record	*/
{
    String	   name;		/* class name as registered	*/
    XrmQuark       quark;               /* quarkified class name	*/
    WidgetClass    class;               /* widget class pointer		*/
} ClNameCacheRec;

typedef struct                          /* Constructor cache record	*/
{
    String	   name;		/* constructor as registered	*/
    XrmQuark       quark;               /* quarkified constructor name	*/
    Widget         (*constructor)();    /* constructor function ptr	*/
} ConCacheRec;

typedef struct				/* Callback cache record	*/
{
    String         name;		/* name as registered 		*/
    XrmQuark       quark;               /* quarkified callback name	*/
    XtCallbackProc callback;            /* callback procedure pointer	*/
    caddr_t	   closure;		/* default client data		*/
} CBCacheRec;

typedef struct  _ResourceRec
{
    String          resFile;            /* additional resource file name    */
    String          children;           /* list of children names to create */
    String          popups;             /* list of popup children to create */
    WidgetClass     class;              /* widget class pointer             */
    WidgetClass     classFromName;      /* widget class pointer             */
    ConCacheRec*    constructor;	/* ptr to Constructo cache record   */
    String          template;           /* ptr to template name string      */
    Boolean         scratch_db;         /* template created into scratch db */ 
    Boolean         managed;            /* created  managed (default TRUE)  */
    Boolean         deferred;           /* deferred creation, (def FALSE)   */
    Boolean         trace;              /* creation trace required          */
    XtCallbackList  callback;           /* creation callback list           */
} ResourceRec, *ResourceRecPtr;

/*
*******************************************************************************
* Private_macro_definitions.
*******************************************************************************
    ONCE_PER_XtAppContext(app) should be invoked at the beginning of each 
    function which performs registration, like MriRegisterMotif and 
    AriRegisterAthena.  Note that this IS a macro: therefore, the return 
    statement actually causes the return from the registration function.
*/

#define ONCE_PER_XtAppContext( app )	\
{					\
    static XtAppContext already[1024];	\
    static int numApps = 0;		\
    int i;				\
					\
    for (i = 0; i < numApps ; i++)	\
        if (app == already[i])		\
            return;			\
					\
    already[numApps++] = app;		\
}

/*
*******************************************************************************
* Private_data_definitions.
*******************************************************************************
    The following cache/registry of known widget classes and contructors,
    initially empty, are loaded by the application using "registration"
    routines.
    Assuming small numbers of constructors, the sequential search
    of such cache is (initially) considered acceptable.
*/

/*  -- Named class pointer cache, intially empty */

extern int         classes_num;
extern int         classes_max;
extern ClCacheRec* classes_ptr;

/*  -- Class name cache, intially empty */

extern int             cl_nm_num;
extern int             cl_nm_max;
extern ClNameCacheRec* cl_nm_ptr;

/*  -- Named object constructor cache, intially empty */

extern int          constrs_num;
extern int          constrs_max;
extern ConCacheRec* constrs_ptr;

/*  -- Callback function cache, initially empty */

extern int	    callbacks_num;
extern int	    callbacks_max;
extern CBCacheRec*  callbacks_ptr;
/*  -- Widget Creation resources */

extern XtResource wc_resources[];

/*
*******************************************************************************
* Private_function_declarations.
*******************************************************************************
    The following functions are generally private functions to the
    WcCreate routines, but they may be defined in different files from
    where they are used.  Client programs probably should not invoke
    these functions directly.
*/

#ifdef FUNCTION_PROTOTYPES
/****************************** ANSI FUNC DECLS ******************************/

#define EV extern void

/*  -- Resource converters */

EV WcAddConverters     ( XtAppContext );
EV CvtStringToClassPtr    ( XrmValue*, Cardinal*, XrmValue*, XrmValue* );
EV CvtStringToClassName   ( XrmValue*, Cardinal*, XrmValue*, XrmValue* );
EV CvtStringToConstructor ( XrmValue*, Cardinal*, XrmValue*, XrmValue* );
EV CvtStringToCallback    ( XrmValue*, Cardinal*, XrmValue*, XrmValue* );
EV XmuCvtStringToWidget   ( XrmValue*, Cardinal*, XrmValue*, XrmValue* );

/*  -- Find root widget of argument, remember if never seen before */

extern Widget WcRootWidget	( Widget );

/*  -- Convenience Callbacks - Clients generally invoke these only
       by binding them to widgets via the resource file.  They can
       be programmatically bound, but generally should not be.
*/

EV WcCreateChildrenCB     (Widget w, char* parent_children, caddr_t unused   );
EV WcCreatePopupsCB       (Widget w, char* parent_children, caddr_t unused   );
EV WcManageCB             (Widget w, char* widgetNames,     caddr_t unused   );
EV WcUnmanageCB           (Widget w, char* widgetNames,     caddr_t unused   );
EV WcManageChildrenCB     (Widget w, char* parent_children, caddr_t unused   );
EV WcUnmanageChildrenCB   (Widget w, char* parent_children, caddr_t unused   );
EV WcDestroyCB            (Widget w, char* widgetNames,     caddr_t unused   );
EV WcSetValueCB           (Widget w, char* name_res_resVal, caddr_t unused   );
EV WcSetTypeValueCB       (Widget w, char* nm_res_type_val, caddr_t unused   );
EV WcSetSensitiveCB       (Widget w, char* widgetNames,     caddr_t unused   );
EV WcSetInsensitiveCB     (Widget w, char* widgetNames,     caddr_t unused   );
EV WcLoadResourceFileCB   (Widget w, char* resFileName,     caddr_t unused   );
EV WcTraceCB              (Widget w, char* annotation,      caddr_t unused   );
EV WcPopupCB              (Widget w, char* widgetName,	    caddr_t unused   );
EV WcPopupGrabCB          (Widget w, char* widgetName,	    caddr_t unused   );
EV WcPopdownCB            (Widget w, char* widgetName,      caddr_t unused   );
EV WcMapCB                (Widget w, char* widgetName,      caddr_t unused   );
EV WcUnmapCB              (Widget w, char* widgetName,      caddr_t unused   );
EV WcSystemCB             (Widget w, char* shellCmdString,  caddr_t unused   );
EV WcExitCB               (Widget w, char* exitValue,       caddr_t unused   );

/*  -- Convenience Actions - Clients generally invoke these only
       by binding them to widgets via the resource file.  They can
       be programmatically bound, but generally should not be.
*/

#define ACT_ARGS Widget w, XEvent *event, String *params, Cardinal *num_params

EV WcCreateChildrenACT    ( ACT_ARGS );
EV WcCreatePopupsACT      ( ACT_ARGS );
EV WcManageACT            ( ACT_ARGS );
EV WcUnmanageACT          ( ACT_ARGS );
EV WcManageChildrenACT    ( ACT_ARGS );
EV WcUnmanageChildrenACT  ( ACT_ARGS );
EV WcDestroyACT           ( ACT_ARGS );
EV WcSetValueACT          ( ACT_ARGS );
EV WcSetTypeValueACT      ( ACT_ARGS );
EV WcSetSensitiveACT      ( ACT_ARGS );
EV WcSetInsensitiveACT    ( ACT_ARGS );
EV WcLoadResourceFileACT  ( ACT_ARGS );
EV WcTraceACT             ( ACT_ARGS );
EV WcPopupACT             ( ACT_ARGS );
EV WcPopupGrabACT         ( ACT_ARGS );
EV WcPopdownACT           ( ACT_ARGS );
EV WcMapACT               ( ACT_ARGS );
EV WcUnmapACT             ( ACT_ARGS );
EV WcSystemACT            ( ACT_ARGS );
EV WcExitACT              ( ACT_ARGS );

#undef ACT_ARGS
#undef EV

#else
/**************************** NON-ANSI FUNC DECLS ****************************/

/*  -- Resource converters */

extern void WcAddConverters        ();
extern void CvtStringToClassPtr    ();
extern void CvtStringToClassName    ();
extern void CvtStringToConstructor ();
extern void CvtStringToCallback    ();
extern void XmuCvtStringToWidget   ();	/* note naming! replace Xmu... */

/*  -- Find root widget of argument, remember if never seen before */

extern Widget WcRootWidget ();

/*  -- Convenience Callbacks - Clients generally invoke these only
       by binding them to widgets via the resource file.  They can
       be programmatically bound, but generally should not be.
*/

extern void WcCreateChildrenCB     ();
extern void WcCreatePopupsCB       ();
extern void WcManageCB             ();
extern void WcUnmanageCB           ();
extern void WcManageChildrenCB     ();
extern void WcUnmanageChildrenCB   ();
extern void WcDestroyCB            ();
extern void WcSetValueCB           ();
extern void WcSetTypeValueCB       ();
extern void WcSetSensitiveCB       ();
extern void WcSetInsensitiveCB     ();
extern void WcLoadResourceFileCB   ();
extern void WcTraceCB              ();
extern void WcPopupCB              ();
extern void WcPopupGrabCB          ();
extern void WcPopdownCB            ();
extern void WcMapCB                ();
extern void WcUnmapCB              ();
extern void WcSystemCB             ();
extern void WcExitCB               ();

/*  -- Convenience Actions - Clients generally invoke these only
       by binding them to widgets via the resource file.  They can
       be programmatically bound, but generally should not be.
*/

extern void WcCreateChildrenACT    ();
extern void WcCreatePopupsACT      ();
extern void WcManageACT            ();
extern void WcUnmanageACT          ();
extern void WcManageChildrenACT    ();
extern void WcUnmanageChildrenACT  ();
extern void WcDestroyACT           ();
extern void WcSetValueACT          ();
extern void WcSetTypeValueACT      ();
extern void WcSetSensitiveACT      ();
extern void WcSetInsensitiveACT    ();
extern void WcLoadResourceFileACT  ();
extern void WcTraceACT             ();
extern void WcPopupACT             ();
extern void WcPopupGrabACT         ();
extern void WcPopdownACT           ();
extern void WcMapACT               ();
extern void WcUnmapACT             ();
extern void WcSystemACT            ();
extern void WcExitACT              ();

#endif /* FUNCTION_PROTOTYPES */

#endif /* _WcCreateP_h */
