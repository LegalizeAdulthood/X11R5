/**********************************************************

  The ANSWER GARDEN PROJECT

Copyright 1989, 1990, 1991 by the Massachusetts Institute of Technology,
Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of MIT not be used in
advertising or publicity pertaining to distribution of the software
without specific, written prior permission.

Answer Garden is a trademark of the Massachusetts Institute of
Technology.  All rights reserved.

  Mark Ackerman
  MIT/Center for Coordination Science
  MIT/Project Athena

  ackerman@athena.mit.edu
  ackerman@xv.mit.edu

***********************************************************/
#ifndef _FuncObj_h
#define _FuncObj_h

#include <X11/Object.h>

typedef struct _FuncObjRec *FuncObj;
typedef struct _FuncObjClassRec *FuncObjClass;
extern WidgetClass funcObjClass;


#define XtNcrushCallback "crushCallback"
#define XtNdynamicCallback "dynamicCallback"
#define XtNuseInstanceBlackboard "useInstanceBlackboard"
#define XtNblackboard "blackboard"

#define XtCUseInstanceBlackboard "UseInstanceBlackboard"
#define XtCBlackboard "Blackboard"

#define XtRBlackboard "Blackboard"

typedef struct  {
    int nparams;
    String *params;
  } FuncObjReturnStruct;



void AGFuncObjHandle();

#endif /* _FuncObj_h */
/* DON'T ADD STUFF AFTER THIS #endif */
