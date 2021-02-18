/*
 *   GraphLib - A library and collection of programs to make
 *              X programming easier.
 *
 *   (c) Copyright 1990,1991 Eric Anderson 
 * Buttons and menus mostly by Jeffrey Hutzelman
 *
 * Our thanks to the contributed maintainers at andrew for providing
 * disk space for the development of this library.
 * My thanks to Geoffrey Collyer and Henry Spencer for providing the basis
 * for this copyright.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company, the Regents of the University of California, or
 * the Free Software Foundation.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. The authors are not responsible for the consequences of use of this
 *    software, no matter how awful, even if they arise from flaws in it.
 *
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Since few users ever read sources,
 *    credits must appear in the documentation.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.  Since few users
 *    ever read sources, credits must appear in the documentation.
 *
 * 4. This notice may not be removed or altered.
 */

#ifndef _GL_EVENTMGR_H
#define _GL_EVENTMGR_H

#include <varargs.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include "X11/gl/varargs.h"
#include <X11/Xos.h>

#define GL_Restricted             128
#define GL_MaxFunction           1023
#define GL_AssocData             1025
#define GL_EM_Max                1099
/* Eventmanager owns entries 1-1099 */
extern Display *display;

/* Associated data keys are reserved as follows:
     0 -  99	Event Manager
   100 - 199	Window Manager
   200 - 299	Buttons
   300 - 399	Menus
   400 - 499	ScrollBars
   500 - 599	Text Fields
   600 - 4999	GRAPHLIB FUTURE EXPANSION
   5000+	Available for use by program
   Ya might as well use your init proc for the key.
   That will probably happen from now on.  But consider <5000 still reserved.
*/

/* Callback Procedure Specification :
   void zzz(XEvent *report, GL_AssocListPtr assoclist);
   report is a pointer to the XEvent being called for.
   associnfo is the information associated with the function.
*/

typedef struct _GL_FuncList {
  struct _GL_FuncList *next;
  VPTF aFunc;
  short flags,top,bottom,left,right;
  GL_DataList AssocList;
} GL_FuncList,*GL_FuncListPtr;

typedef struct _GL_EventMgrInfo {
  struct _GL_EventMgrInfo *next;
  Window winid;
  GL_FuncListPtr FuncLists[LASTEvent];
  GL_DataList AssocList;
} GL_EventMgrInfo;

void GL_MainLoop(/*Window mainwin*/);
void GL_AddFuncPtr();
/* Window, Variable Args, see man page. */

GL_DataList GL_GetDataList(
#if NeedFuntionPrototypes
Window target
#endif
);

void GL_AddWindowAssocInfo(
#if NeedFunctionPrototypes
Window target,
VPTF key,
GLPointer associnfo
#endif
);

void GL_SetTimeOut(
#if NeedFunctionPrototypes
struct timeval *atimeout,
VPTF atimeoutfunc,
Window assocwindow
#endif
);

void GL_ProcessEvent(
#if NeedFunctionPrototypes
XEvent *report
#endif
);

void GL_MarkArgument();/* argnum,value(see GL_ARG_*) */
#define GL_ARG_UNUSED 0
#define GL_ARG_USED 1
#define GL_ARG_SHARED 2

extern int s_argc;
extern char **s_argv;
extern char *argv_used;
extern VPTF s_helpfunction;

void _GL_VA_Init_EventMgr();
void _GL_VA_Handle_EventMgr();
void _GL_VA_Destroy_EventMgr();
void _GL_VA_Execute_EventMgr();

#endif /* ndef _GL_EVENTMGR_H */

