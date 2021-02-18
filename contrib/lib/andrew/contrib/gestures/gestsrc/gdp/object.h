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
  Operations on objects
 */

typedef struct object *Object;

typedef	enum {
	Nothing,
	Line,
	Vline,
	Rect,
	Vrect,
	Text,
	Circle,
	SetOfObjects,
} ObjectType;


Object	CreateObject();		/* ObjectType */

Bool	UpdatePoint();		/* Object; int which_point; int x, y; */

Object	CopyObject();
void	FreeObject();
