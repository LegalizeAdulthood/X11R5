/***********************************************************************

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/
#include "set.h"
#include <gestures/bool.h>
#include <gestures/util.h>
#include "object.h"
#include "iobject.h"
#include <gestures/matrix.h>
#include "transform.h"
#include <stdio.h>

struct dope dope[] = {
	{ Nothing, "nothing" },
	{ Line, "line",  2, AlwaysOK, LineDraw, LineTransform, LineDistance},
	{ Vline, "vline", 2, VlinePoint, LineDraw, LineTransform, LineDistance},
	{ Rect, "rect", 3, AlwaysOK, RectDraw, RectTransform, RectDistance},
	{ Vrect, "vrect", 3, AlwaysOK, RectDraw, RectTransform, RectDistance},
	{ Text, "text", 1, AlwaysOK, TextDraw, TextTransform, TextDistance },
	{ Circle, "circle", 2, AlwaysOK, CircleDraw, CircleTransform, CircleDistance },
	{ SetOfObjects, "set", 0, AlwaysOK, SetDraw, SetTransform, SetDistance },
};

#define NDOPE	(sizeof(dope)/sizeof(dope[0]))

Object
CreateObject(type)
ObjectType type;
{
	register Object o = allocate(1, struct object);
	register int i;

	if( (int) type < 0 || (int) type >= NDOPE)
		error("CreateObject: bad type %d\n", (int) type);
	if(dope[ (int) type ].type != type )
		error("CreateObject: internal inconsistency, dope[%d]\n",
				(int) type);

	o->type = type;
	for(i = 0; i < MAXPOINTS; i++)
		o->point[i].x = o->point[i].y = NOT_A_POINT;
	o->subobjects = NULL;
	if(o->type == SetOfObjects) {
		extern Set StEmptySet();
		o->subobjects = StEmptySet();
	}
	return o;
}

Bool
UpdatePoint(o, which, x, y)
register Object o;
int which;
int x, y;
{
	register struct dope *d = &dope[(int) o->type];
	struct object save;
	
	if(which < 0 || which > d->npoints)
		return FALSE;
	save = *o;
	o->point[which].x = x;
	o->point[which].y = y;
	if( (*d->point)(o, which) ) {	/* OK */
		Erase(&save);
		Draw(o);
		return TRUE;
	}
	*o = save;	/* restore */
	return FALSE;
}

Bool
AddSubObject(o, subo)
register Object o, subo;
{
	if(o->type != SetOfObjects)
		return FALSE;
	Draw(subo);
	(void) AddElement(o->subobjects, subo);
	return TRUE;
}

Bool
AddText(o, s)
register Object o;
char *s;
{
	Erase(o);
	o->text = scopy(s);
	Draw(o);
	return TRUE;
}

Object
CopyObject(old)
Object old;
{
	register Object o = allocate(1, struct object);
	Set CopySet();
	*o = *old;
	if(o->type == SetOfObjects)
		o->subobjects = CopySet(o->subobjects);
	return o;
}

void
FreeObject(o)
Object o;
{
	free( (char *) o);
}

void
Transform(o, t)
register Object o;
Transformation t;
{
	register struct dope *d = &dope[(int) o->type];
	if(o->type == SetOfObjects)
		(d->transform)(o, t);
	else {
		Erase(o);
		(d->transform)(o, t);
		Draw(o);
	}
}

/* --------------------- dope functions ---------------- */

Bool
AlwaysOK()
{
	return TRUE;
}

Bool
AlwaysBad()
{
	return FALSE;
}


Bool
VlinePoint(o, which)
register Object o;
register int which;
{
	/* make vertical or horizontal or 45 degrees */
	return TRUE;
}

int
Distance(o, x, y)
register Object o;
int x, y;
{
	(*dope[(int)o->type].distance)(o, x, y);	
}

/* ---------------- object manipulation ----------- */


ObjHighlight(o, highlight)
Object o;
{
	static depth = 0;

/*
	printf("%d ", depth);
	for(i = 0; i < depth; i++)
		printf("  ");
	printf("ObjHighlight(type=%d, highlight=%d)\n",
		o->type, highlight);
*/
	

	if(o->type == SetOfObjects) {
		depth++;
		Map(o->subobjects, ObjHighlight, highlight);
		depth--;
	} else {
		o->highlight = highlight;
	}

}
