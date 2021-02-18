/****************************************************************************

	ScrolledListP.h

	This file is the private definition file for the File Selector
	Widget.

	April 1990, Brian Totty

 ****************************************************************************/

#ifndef	_SCROLLEDLISTP_H_
#define _SCROLLEDLISTP_H_

typedef struct _ScrolledListClassPart
{
	int    empty;
} ScrolledListClassPart;

typedef struct
{
	Widget			list;
	Widget			viewport;
	XFontStruct		*font;
	char			**item_array;
	int			item_count;
	XtCallbackList		callback;
} ScrolledListPart;

typedef struct _ScrolledListClassRec
{
	CoreClassPart		core_class;
	CompositeClassPart	composite_class;
	ScrolledListClassPart	scrolledList_class;
} ScrolledListClassRec;

typedef struct _ScrolledListRec
{
	CorePart		core;
	CompositePart		composite;
	ScrolledListPart	scrolledList;
} ScrolledListRec;

extern ScrolledListClassRec scrolledListClassRec;

#endif
