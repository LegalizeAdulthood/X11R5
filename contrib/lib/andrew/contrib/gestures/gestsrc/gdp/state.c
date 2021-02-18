/***********************************************************************

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/
#include <gestures/bool.h>
#include <gestures/util.h>
#include "object.h"
#include "set.h"
#include "iobject.h"
#include <gestures/matrix.h>
#include "transform.h"
#include <stdio.h>

#define	EtoO(e)	( (Object) ElementPointer(e))

/* The state of the picture is entirely described by these variables: */

static	Set	Objs;	/* set of all visible objects */

static	VersionNumber	lastckp;

Set
StEmptySet()
{
	return EmptySet(NULL, NULL, Objs);
}

void
StInit()
{
	void object_added(), object_deleted();

	Objs = EmptySet(object_added, object_deleted, NULL);
	lastckp = CheckpointSetGroup(Objs);
}

#define LINESIZE 100
static char bottom_line[LINESIZE];

static void
DrawBottomLine()
{
	char line[102];
	sprintf(line, "%-100.100s", bottom_line);
	GDEVtext(0, 0, line);
}

void
StBottomLine(string)
char *string;
{
	DrawBottomLine();  /* Erases old bottom line */
	strncpy(bottom_line, string, LINESIZE);
	DrawBottomLine();
}


void
StRedraw()
{
	void Draw();

	DrawClear();
	Map(Objs, Draw, NULL);
	DrawBottomLine();
	WmFlush();
}

Element
StNewObj(t)
ObjectType t;
{
	register Object o = CreateObject(t);
	return AddElement(Objs, o);
}

void
StAddSubObject(e, sube)
Element e, sube;
{
	AddSubObject(EtoO(e),EtoO(sube));
}

Element
StCopyElement(e)
Element e;
{
	register Object o = CopyObject(EtoO(e));
	return AddElement(Objs, o);
}


void
StDelete(e)
Element e;
{
	DeleteElement(Objs, e);
}

static
void
object_added(s, e, o)
Set s;
Element e;
Object o;
{
	Draw(o);
}

static
void
object_deleted(s, e, o)
Set s;
Element e;
Object o;
{
	Erase(o);
}

void
StUpdatePoint(e, point_number, x, y)
Element e;
int point_number, x, y;
{
	UpdatePoint(EtoO(e), point_number, x, y);
}

void
StTransform(e, t)
Element e;
Transformation t;
{
	Transform(EtoO(e), t);
}

void
StMove(e, x, y)
Element e;
int x, y;
{
	static Transformation t;

	if(t == NULL) t = AllocTran();
	StTransform(e, TranslateTran(t, x, y));
}

static int _x, _y, _min_d;
static Element _min_e;

static void
sizem(e)
Element e;
{
	int d = Distance(EtoO(e), _x, _y);
	if(d < _min_d) {
		_min_d = d;
		_min_e = e;
	}
}

Element
StPick(x, y)	/* pick the item pointed to by x, y */
int x, y;
{
	_x = x; _y = y;
	_min_d = 5;	/* must be within five pixels */
	_min_d = 20;	/* must be within 20 pixels */
	_min_e = NULL;
	MapE(Objs, sizem, NULL);
	return _min_e;
}

void
StReplaceObject(e, s)
Element e;
Object s;
{
	register Object d = EtoO(e);

/*
	printf("StReplaceObject: \nreplacing ("); DumpObject(d);
	printf("with ("); DumpObject(s);
*/

	if(d->type != s->type)
		error("ReplaceObject: type change");
	if(d->type == SetOfObjects) {
		Element se, de;
		IterateSet(s->subobjects);
		IterateSet(d->subobjects);
		for(;;) {
			se = NextElement(s->subobjects);
			de = NextElement(d->subobjects);
			if(se == NULL && de == NULL)
				break;
			if(se == NULL || de == NULL)
				error("ReplaceObject: set sizes differ");
			StReplaceObject(de, EtoO(se));
		}
	}
	else {
		Erase(d);
		*d = *s;
		Draw(d);
	}
}

Element
StEdit(e, oldo)
Element e;
Object *oldo;
{
	register Object o = CopyObject(EtoO(e));
	if(oldo)
		*oldo = EtoO(e);
	/* printf("StEdit old=%x, copy=%x\n", EtoO(e), o); */

	DeleteElement(Objs, e);
	return AddElement(Objs, o);
/*
	return e;
*/
}

StCheckpoint()
{
	lastckp = CheckpointSetGroup(Objs);
}

StUndo()
{
	UndoSetGroup(Objs, lastckp);
}

static void
CopySetHelp(s)
Set s;
{
	Element e = AnElement(s);
	Object o;

	if(e == NULL)
		return;
	o = CopyObject(EtoO(e));
	printf("."); fflush(stdout);
	DeleteElement(s, e);
	CopySetHelp(s);
	AddElement(s, o);
}

static void
copysethelp(o, s)
Object o;
Set s;
{
	AddElement(s, CopyObject(o));
}

Set
CopySet(s)
Set s;
{
	Set new = EmptySet(NULL, NULL, Objs);
	Map(s, copysethelp, new);
	return new;
}


StHighlight(e, highlight)
Element e;
{
	ObjHighlight(EtoO(e), highlight);
}

StErase(e)
Element e;
{
	Erase(EtoO(e));
}

StDraw(e)
Element e;
{
	Draw(EtoO(e));
}
