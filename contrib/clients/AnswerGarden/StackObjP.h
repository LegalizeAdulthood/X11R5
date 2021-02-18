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
#ifndef _StackObjP_h_
#define _StackObjP_h_

#include <X11/ObjectP.h>
#include "StackObj.h"


#define MAXCHILDREN   32

/**********************************************************
 * StackObj Instance Data Structures
 *
 **********************************************************/


typedef struct _StackObjPart {
  /* resources */
  Dimension             dx;
  Dimension             dy;
  Position              x;
  Position              y;

  /* private */
  Position              initial_x;
  Position              initial_y;
  Position              current_x;
  Position              current_y;
  Widget                children[MAXCHILDREN];
  int                   nchildren;
} StackObjPart;

typedef struct _StackObjRec {
    ObjectPart   object;
    StackObjPart  stackObj;
} StackObjRec;

/********************************************************
 * StackObj Class Data Structures
 *
 ********************************************************/


typedef struct _StackObjClassPart {
  char *makes_compiler_happy;
}StackObjClassPart;



typedef struct _StackObjClassRec {
  ObjectClassPart  object_class;
  StackObjClassPart stackObj_class;
} StackObjClassRec;

extern StackObjClassRec stackObjClassRec;

#endif /*_StackObjP_h_*/


