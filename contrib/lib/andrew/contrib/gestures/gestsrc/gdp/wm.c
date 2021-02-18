/***********************************************************************

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/
/*
  interface from objects to window manager
*/

#include <gestures/bool.h>
#include <gestures/util.h>
#include <gestures/gdev.h>
#include <stdio.h>

void
WmClear()
{
	GDEVstart();
}

void
WmInit(program)
char *program;
{
	GDEVinit(NULL);
	GDEVsets("currentfunction", "xor");
	GDEVseti("thickness", 0);	/* gives fast line drawing */
	GDEVsets("program", program ? program : "Dean's window");
	WmClear();
}

WmFlush()
{
	GDEVflush();
}

void
EraseOn()
{ /* GDEVsets("currentcolor", "white"); */
}

void
EraseOff()
{
	/* GDEVsets("currentcolor", "black"); */
}
