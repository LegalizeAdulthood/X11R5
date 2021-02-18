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
#ifndef _StackObj_h
#define _StackObj_h

#include <X11/Object.h>

#define XtNvertDistance "vertDistance"
#define XtNhorizDistance "horizDistance"

typedef struct _StackObjRec *StackObj;
typedef struct _StackObjClassRec *StackObjClass;
extern WidgetClass stackObjClass;

#endif /* _StackObj_h */
/* DON'T ADD STUFF AFTER THIS #endif */
