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
#ifndef _FuncObjP_h_
#define _FuncObjP_h_

#include <X11/ObjectP.h>
#include "FuncObj.h"




typedef struct _BlackboardEntry
{
  XrmQuark  entry_quark;
  union 
    {
      String *entry;
      XtPointer *proc;
    } innards;
} BlackboardEntry;


/**********************************************************
 * FuncObj Instance Data Structures
 *
 **********************************************************/


typedef struct _FuncObjPart {
  /* resources */
  Boolean               use_instance_blackboard;
  XtCallbackList        crush_callback;
  XtCallbackList        dynamic_callback;
  BlackboardEntry       *blackboard;

  /* private */
  int                   num_entries;
} FuncObjPart;

typedef struct _FuncObjRec {
    ObjectPart   object;
    FuncObjPart  funcObj;
} FuncObjRec;

/********************************************************
 * FuncObj Class Data Structures
 *
 ********************************************************/


typedef struct _FuncObjClassPart {
  void                  (*external_handle)();
  int                   (*parse_command)();
  Boolean               (*trigger_command)();
  void                  (*destroy_parsing_tree)();
  Boolean               first_flag;
  BlackboardEntry       *blackboard;
  int                   num_entries;
}FuncObjClassPart;

#define XtInheritExternalHandle ((void (*)())_XtInherit)
#define XtInheritParseCommand ((int (*)())_XtInherit)
#define XtInheritTriggerCommand ((Boolean (*)())_XtInherit)
#define XtInheritDestroyParsingTree ((void (*)())_XtInherit)


typedef struct _FuncObjClassRec {
  ObjectClassPart  object_class;
  FuncObjClassPart funcObj_class;
} FuncObjClassRec;

extern FuncObjClassRec funcObjClassRec;

#endif /*_FuncObjP_h_*/
