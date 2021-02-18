/****************************************************************************

	MultiList.c

	This file contains the implementation of the Picasso List
	widget.  Its functionality is intended to be similar to
	The Athena List widget, with some extra features added.

	This code is loosely based on the Athena List source which
	is why the MIT copyright notice appears below.

	July 1990, Brian Totty

 ****************************************************************************/

/*
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Original Athena Author:  Chris D. Peterson, MIT X Consortium
 */

#include <stdio.h>
#include <ctype.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
/*#include <X11/XawInit.h>*/
#include "MultiListP.h"

/*===========================================================================*

          D E C L A R A T I O N S    A N D    D E F I N I T I O N S

 *===========================================================================*/

Pixmap XmuCreateStippledPixmap();
extern void XawInitializeWidgetSet();

#define	SUPERCLASS	&(simpleClassRec)

#define	FontAscent(f)	((f)->max_bounds.ascent)
#define	FontDescent(f)	((f)->max_bounds.descent)
#define	FontH(f)	(FontAscent(f) + FontDescent(f) + 2)
#define	FontW(f,s)	(XTextWidth(f,s,strlen(s)) + 1)
#define	FontMaxCharW(f)	((f)->max_bounds.rbearing-(f)->min_bounds.lbearing+1)

#ifndef abs
#define abs(a)			((a) < 0 ? -(a) : (a))
#endif

#define max(a,b)		((a) > (b) ? (a) : (b))
#define min(a,b)		((a) < (b) ? (a) : (b))
#define XtStrlen(s)		((s) ? strlen(s) : 0)

#define	TypeAlloc(t,n)		(t *)malloc(sizeof(t) * n)
#define	StrCopy(s)		strcpy(TypeAlloc(char,strlen(s)+1),s)
#define	StrCopyRetLength(s,lp)	strcpy(TypeAlloc(char,(*lp=(strlen(s)+1))),s)

#define CoreFieldOffset(f)	XtOffset(Widget,core.f)
#define	SimpleFieldOffset(f)	XtOffset(MultiListWidget,simple.f)
#define MultiListFieldOffset(f)	XtOffset(MultiListWidget,multiList.f)

/*===========================================================================*

        I N T E R N A L    P R O C E D U R E    D E C L A R A T I O N S

 *===========================================================================*/

static void			Initialize();
static void			Redisplay();
static XtGeometryResult		PreferredGeometry();
static void			Resize();
static Boolean			SetValues();

static void			DestroyOldData();
static void			InitializeNewData();
static void			CreateNewGCs();

static void			RecalcCoords();
static void			NegotiateSizeChange();
static Boolean			Layout();

static void			RedrawAll();
static void			RedrawItem();
static void			RedrawRowColumn();

static void			PixelToRowColumn();
static void			RowColumnToPixels();
static Boolean			RowColumnToItem();
static Boolean			ItemToRowColumn();

static void			Set();
static void			Unset();
static void			Toggle();
static void			Open();
static void			OpenMany();
static void			Notify();

/*===========================================================================*

              R E S O U R C E    I N I T I A L I Z A T I O N

 *===========================================================================*/

static XtResource resources[] =
{
	{XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
	    CoreFieldOffset(width), XtRString, "0"},
	{XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
	    CoreFieldOffset(height), XtRString, "0"},
	{XtNbackground, XtCBackground, XtRPixel, sizeof(Pixel),
	    CoreFieldOffset(background_pixel),XtRString,"XtDefaultBackground"},

	{XtNcursor, XtCCursor, XtRCursor, sizeof(Cursor),
	    SimpleFieldOffset(cursor), XtRString, "left_ptr"},

	{XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	    MultiListFieldOffset(foreground), XtRString,"XtDefaultForeground"},
	{XtNhighlightForeground, XtCHForeground, XtRPixel, sizeof(Pixel),
	    MultiListFieldOffset(highlight_fg), XtRString, "XtDefaultBackground"},
	{XtNhighlightBackground, XtCHBackground, XtRPixel, sizeof(Pixel),
	    MultiListFieldOffset(highlight_bg), XtRString, "XtDefaultForeground"},
	{XtNcolumnSpacing, XtCSpacing, XtRDimension, sizeof(Dimension),
	    MultiListFieldOffset(column_space), XtRImmediate, (caddr_t)8},
	{XtNrowSpacing, XtCSpacing, XtRDimension, sizeof(Dimension),
	    MultiListFieldOffset(row_space), XtRImmediate, (caddr_t)0},
	{XtNdefaultColumns, XtCColumns, XtRInt,  sizeof(int),
	    MultiListFieldOffset(default_cols), XtRImmediate, (caddr_t)1},
	{XtNforceColumns, XtCColumns, XtRBoolean, sizeof(Boolean),
	    MultiListFieldOffset(force_cols), XtRString, (caddr_t) "False"},
	{XtNpasteBuffer, XtCBoolean, XtRBoolean, sizeof(Boolean),
	    MultiListFieldOffset(paste), XtRString, (caddr_t) "False"},
	{XtNverticalList, XtCBoolean, XtRBoolean,  sizeof(Boolean),
	    MultiListFieldOffset(row_major), XtRString, (caddr_t) "False"},
	{XtNlongest, XtCLongest, XtRInt,  sizeof(int),
	    MultiListFieldOffset(longest), XtRImmediate, (caddr_t)0},
	{XtNnumberStrings, XtCNumberStrings, XtRInt,  sizeof(int),
	    MultiListFieldOffset(nitems), XtRImmediate, (caddr_t)0},
	{XtNfont,  XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	    MultiListFieldOffset(font),XtRString, "XtDefaultFont"},
	{XtNlist, XtCList, XtRPointer, sizeof(char **),
	    MultiListFieldOffset(list), XtRString, NULL},
	{XtNsensitiveArray, XtCList, XtRPointer, sizeof(Boolean *),
	    MultiListFieldOffset(sensitive_array), XtRString, NULL},
	{XtNcallback, XtCCallback, XtRCallback, sizeof(caddr_t),
	    MultiListFieldOffset(callback), XtRCallback, NULL},
	{XtNmaxSelectable, XtCValue, XtRInt, sizeof(int),
	    MultiListFieldOffset(max_selectable), XtRImmediate, (caddr_t) 1},
	{XtNnotifyHighlights, XtCBoolean, XtRBoolean, sizeof(Boolean),
	    MultiListFieldOffset(notify_highlights), XtRString, "True"},
	{XtNnotifyUnhighlights, XtCBoolean, XtRBoolean, sizeof(Boolean),
	    MultiListFieldOffset(notify_unhighlights), XtRString, "True"},
	{XtNnotifyOpens, XtCBoolean, XtRBoolean, sizeof(Boolean),
	    MultiListFieldOffset(notify_opens), XtRString, "True"},

	{XtNcolumnWidth, XtCValue, XtRDimension, sizeof(Dimension),
	    MultiListFieldOffset(col_width), XtRImmediate, (caddr_t)0},
	{XtNrowHeight, XtCValue, XtRDimension, sizeof(Dimension),
	    MultiListFieldOffset(row_height), XtRImmediate, (caddr_t)0},
};

/*===========================================================================*

        A C T I O N    A N D    T R A N S L A T I O N    T A B L E S

 *===========================================================================*/


static char defaultTranslations[] =
"	Shift<Btn1Down>:			Toggle()\n\
	<Btn1Down>:				Set()\n\
	<Btn3Down>:				Unset()\n\
	<Btn1Up>:				Notify()\n\
	<Btn3Up>:				Notify()";

static XtActionsRec actions[] =
{
	{"Notify",	Notify},
	{"Set",		Set},
	{"Toggle",	Toggle},
	{"Open",	Open},
	{"OpenMany",	OpenMany},
	{"Unset",	Unset},
	{NULL,NULL}
};

/*===========================================================================*

                    C L A S S    A L L O C A T I O N

 *===========================================================================*/

MultiListClassRec multiListClassRec =
{
	{
		/* superclass		*/	(WidgetClass)SUPERCLASS,
		/* class_name		*/	"MultiList",
		/* widget_size		*/	sizeof(MultiListRec),
		/* class_initialize	*/	NULL,
		/* class_part_initialize*/	NULL,
		/* class_inited		*/	FALSE,
		/* initialize		*/	Initialize,
		/* initialize_hook	*/	NULL,
		/* realize		*/	XtInheritRealize,
		/* actions		*/	actions,
		/* num_actions		*/	XtNumber(actions),
		/* resources		*/	resources,
		/* resource_count	*/	XtNumber(resources),
		/* xrm_class		*/	NULLQUARK,
		/* compress_motion	*/	TRUE,
		/* compress_exposure	*/	FALSE,
		/* compress_enterleave	*/	TRUE,
		/* visible_interest	*/	FALSE,
		/* destroy		*/	NULL,
		/* resize		*/	Resize,
		/* expose		*/	Redisplay,
		/* set_values		*/	SetValues,
		/* set_values_hook	*/	NULL,
		/* set_values_almost	*/	XtInheritSetValuesAlmost,
		/* get_values_hook	*/	NULL,
		/* accept_focus		*/	NULL,
		/* version		*/	XtVersion,
		/* callback_private	*/	NULL,
		/* tm_table		*/	defaultTranslations,
		/* query_geometry       */	PreferredGeometry,
		/* display_accelerator  */	XtInheritDisplayAccelerator,
		/* extension            */	NULL
	}, /* Core Part */
	{
		/* change_sensitive     */	XtInheritChangeSensitive
	}
};

WidgetClass multiListWidgetClass = (WidgetClass)&multiListClassRec;

/*===========================================================================*

                       T O O L K I T    M E T H O D S

 *===========================================================================*/

/*---------------------------------------------------------------------------*

	Initialize()

	This procedure is called by the X toolkit to initialize
	the widget instance.  The hook to this routine is in the
	initialize part of the core part of the class.

 *---------------------------------------------------------------------------*/

/* ARGSUSED */
static void Initialize(request,new)
Widget request,new;
{
	MultiListWidget plw;

	plw = (MultiListWidget)new;
	CreateNewGCs(new);
	InitializeNewData(new);
	RecalcCoords(new,(MultiListWidth(new) == 0),(MultiListHeight(new) == 0));
} /* Initialize */


/*---------------------------------------------------------------------------*

	Redisplay(plw,event,rectangle_union)

	This routine redraws the MultiList widget <plw> based on the exposure
	region requested in <event>.

 *---------------------------------------------------------------------------*/

/* ARGSUSED */
static void Redisplay(plw,event,rectangle_union)
MultiListWidget plw;
XEvent *event;
Region rectangle_union;
{
	int i,x1,y1,w,h,x2,y2,row,col,item,ul_row,ul_col,lr_row,lr_col;

	if (event == NULL)
	{
		XClearWindow(XtDisplay(plw),XtWindow(plw));
		for (i = 0; i < MultiListNumItems(plw); i++) RedrawItem(plw,i);
	}
	    else
	{
		x1 = event->xexpose.x;
		y1 = event->xexpose.y;
		w = event->xexpose.width;
		h = event->xexpose.height;
		x2 = x1 + w;
		y2 = y1 + h;
		XFillRectangle(XtDisplay(plw),XtWindow(plw),
			       MultiListEraseGC(plw),x1,y1,w,h);
		PixelToRowColumn(plw,x1,y1,&ul_row,&ul_col);
		PixelToRowColumn(plw,x2,y2,&lr_row,&lr_col);
		lr_row = min(lr_row,MultiListNumRows(plw) - 1);
		lr_col = min(lr_col,MultiListNumCols(plw) - 1);
		for (col = ul_col; col <= lr_col; col++)
		{
			for (row = ul_row; row <= lr_row; row++)
			{
				RedrawRowColumn(plw,row,col);
			}
		}
	}
} /* End Redisplay */


/*---------------------------------------------------------------------------*

	PreferredGeometry(plw,parent_idea,our_idea)

	This routine is called by the parent to tell us about the
	parent's idea of our width and/or height.  We then suggest
	our preference through <our_idea> and return the information
	to the parent.

 *---------------------------------------------------------------------------*/

static XtGeometryResult PreferredGeometry(plw,parent_idea,our_idea)
MultiListWidget plw;
XtWidgetGeometry *parent_idea,*our_idea;
{
	Dimension nw,nh;
	Boolean parent_wants_w,parent_wants_h,we_changed_size;
    
	parent_wants_w = (parent_idea->request_mode) & CWWidth;
	parent_wants_h = (parent_idea->request_mode) & CWHeight;

	if (parent_wants_w)
		nw = parent_idea->width;
	    else
		nw = MultiListWidth(plw);

	if (parent_wants_h)
		nh = parent_idea->height;
	    else
		nh = MultiListHeight(plw);

	our_idea->request_mode = 0;
	if (!parent_wants_w && !parent_wants_h) return(XtGeometryYes);

	we_changed_size = Layout(plw,!parent_wants_w,!parent_wants_h,&nw,&nh);
	our_idea->request_mode |= (CWWidth | CWHeight);
	our_idea->width = nw;
	our_idea->height = nh;

	if (we_changed_size)
		return(XtGeometryAlmost);
	    else
		return(XtGeometryYes);
} /* End PreferredGeometry */


/*---------------------------------------------------------------------------*

	Resize(plw)

	This function is called when the widget is being resized.  It
	recalculates the layout of the widget.

 *---------------------------------------------------------------------------*/

static void Resize(plw)
MultiListWidget plw;
{
	Dimension width,height;

	width = MultiListWidth(plw);
	height = MultiListHeight(plw);
	Layout(plw,False,False,&width,&height);
} /* End Resize */


/*---------------------------------------------------------------------------*

	SetValues(cpl,rpl,npl)

	This routine is called when the user is changing resources.  <cpl>
	is the current widget before the user's changes have been instituted.
	<rpl> includes the original changes as requested by the user.  <npl>
	is the new resulting widget with the requested changes and with all
	superclass changes already made.

 *---------------------------------------------------------------------------*/

static Boolean SetValues(cpl,rpl,npl)
MultiListWidget cpl,rpl,npl;
{
	Boolean redraw,recalc;

	redraw = False;
	recalc = False;

		/* Graphic Context Changes */

	if ((MultiListFG(cpl) != MultiListFG(npl)) ||
	    (MultiListBG(cpl) != MultiListBG(npl)) ||
	    (MultiListHighlightFG(cpl) != MultiListHighlightFG(npl)) ||
	    (MultiListHighlightBG(cpl) != MultiListHighlightBG(npl)) ||
	    (MultiListFont(cpl) != MultiListFont(npl)))
	{
		XtDestroyGC(MultiListEraseGC(cpl));
		XtDestroyGC(MultiListDrawGC(cpl));
		XtDestroyGC(MultiListHighlightForeGC(cpl));
		XtDestroyGC(MultiListHighlightBackGC(cpl));
		XtDestroyGC(MultiListGrayGC(cpl));
		CreateNewGCs(npl);
		redraw = True;
	}

		/* Changes That Require Data Initialization */

	if ((MultiListList(cpl) != MultiListList(npl)) ||
	    (MultiListSensitiveArray(cpl) != MultiListSensitiveArray(npl)) ||
	    (MultiListSensitive(cpl) != MultiListSensitive(npl)) ||
	    (MultiListAncesSensitive(cpl) != MultiListAncesSensitive(npl)) ||
	    (MultiListNumItems(cpl) != MultiListNumItems(npl)) ||
	    (MultiListMaxSelectable(cpl) != MultiListMaxSelectable(npl)))
	{
		DestroyOldData(cpl);
		InitializeNewData(npl);
		recalc = True;
		redraw = True;
	}

		/* Changes That Require Recalculating Coordinates */

	if ((MultiListWidth(cpl) != MultiListWidth(npl)) ||
	    (MultiListHeight(cpl) != MultiListHeight(npl)) ||
	    (MultiListColumnSpace(cpl) != MultiListColumnSpace(npl)) ||
	    (MultiListRowSpace(cpl) != MultiListRowSpace(npl)) ||
	    (MultiListDefaultCols(cpl) != MultiListDefaultCols(npl)) ||
	    ((MultiListForceCols(cpl) != MultiListForceCols(npl)) &&
	     (MultiListNumCols(cpl) != MultiListNumCols(npl))) ||
	    (MultiListRowMajor(cpl) != MultiListRowMajor(npl)) ||
	    (MultiListFont(cpl) != MultiListFont(npl)) ||
	    (MultiListLongest(cpl) != MultiListLongest(npl)))
	{
		recalc = True;
		redraw = True;
	}

	if (MultiListColWidth(cpl) != MultiListColWidth(npl))
	{
		XtWarning("columnWidth Resource Is Read-Only");
		MultiListColWidth(npl) = MultiListColWidth(cpl);
	}
	if (MultiListRowHeight(cpl) != MultiListRowHeight(npl))
	{
		XtWarning("rowHeight Resource Is Read-Only");
		MultiListRowHeight(npl) = MultiListRowHeight(cpl);
	}

	if (recalc)
	{
		RecalcCoords(npl,!MultiListWidth(npl),!MultiListHeight(npl));
	}
    
	if (!XtIsRealized(cpl))
		return(False);
	    else
		return(redraw);
} /* End SetValues */

/*===========================================================================*

                  D A T A    I N I T I A L I Z A T I O N

 *===========================================================================*/

/*---------------------------------------------------------------------------*

	DestroyOldData(plw)

	This routine frees the internal list item array and sets the
	item count to 0.  This is normally done immediately before
	calling InitializeNewData() to rebuild the internal item
	array from new user specified arrays.

 *---------------------------------------------------------------------------*/

static void DestroyOldData(plw)
MultiListWidget plw;
{
	int i;

	if (MultiListItemArray(plw) != NULL)	/* Free Old List */
	{
		for (i = 0; i < MultiListNumItems(plw); i++)
		{
			free(MultiListItemString(MultiListNthItem(plw,i)));
		}
		free(MultiListItemArray(plw));
	}
	if (MultiListSelArray(plw) != NULL) free(MultiListSelArray(plw));
	MultiListSelArray(plw) = NULL;
	MultiListNumSelected(plw) = 0;
	MultiListItemArray(plw) = NULL;
	MultiListNumItems(plw) = 0;
} /* End DestroyOldData */


/*---------------------------------------------------------------------------*

	InitializeNewData(plw)

	This routine takes a MultiList widget <plw> and builds up new
	data item tables based on the string list and the sensitivity array.
	All previous data should have already been freed.  If the number
	of items is 0, they will be counted, so the array must be NULL
	terminated.  If the list of strings is NULL, this is treated as
	a list of 0 elements.  If the sensitivity array is NULL, all
	items are treated as sensitive.

	When this routine is done, the string list and sensitivity array
	fields will all be set to NULL, and the widget will not reference
	them again.

 *---------------------------------------------------------------------------*/

static void InitializeNewData(plw)
MultiListWidget plw;
{
	int i;
	MultiListItem *item;
	String *string_array;

	string_array = MultiListList(plw);
	if (MultiListNumItems(plw) == 0)		/* Count The Elements */
	{
		if (MultiListList(plw) == NULL)	/* No elements */
		{
			MultiListNumItems(plw) = 0;
		}
		    else
		{
			for (i = 0; string_array[i] != NULL; i ++);
			MultiListNumItems(plw) = i;
		}
	}
	if (MultiListNumItems(plw) == 0)		/* No Items */
	{
		MultiListItemArray(plw) = NULL;
	}
	    else
	{
		MultiListItemArray(plw) = TypeAlloc(MultiListItem,MultiListNumItems(plw));
		for (i = 0; i < MultiListNumItems(plw); i++)
		{
			item = MultiListNthItem(plw,i);
			if (MultiListSensitiveArray(plw) == NULL ||
			    (MultiListSensitiveArray(plw)[i] == True))
			{
				MultiListItemSensitive(item) = True;
			}
			    else
			{
				MultiListItemSensitive(item) = False;
			}
			MultiListItemString(item) = StrCopy(string_array[i]);
			MultiListItemHighlighted(item) = False;
		}
	}
	if (MultiListMaxSelectable(plw) == 0)
	{
		MultiListSelArray(plw) = NULL;
		MultiListNumSelected(plw) = 0;
	}
	    else
	{
		MultiListSelArray(plw) = TypeAlloc(int,MultiListMaxSelectable(plw));
		MultiListNumSelected(plw) = 0;
	}

	MultiListList(plw) = NULL;
	MultiListSensitiveArray(plw) = NULL;
} /* End InitializeNewData */
		

/*---------------------------------------------------------------------------*

	CreateNewGCs(plw)

	This routine takes a MultiList widget <plw> and creates a new set of
	graphic contexts for the widget based on the colors, fonts, etc.
	in the widget.  Any previous GCs are assumed to have already been
	destroyed.

 *---------------------------------------------------------------------------*/

static void CreateNewGCs(plw)
MultiListWidget plw;
{
	XGCValues values;
	unsigned int attribs;

	attribs = GCForeground | GCBackground | GCFont;
	values.foreground = MultiListFG(plw);
	values.background = MultiListBG(plw);
	values.font = MultiListFont(plw)->fid;
	MultiListDrawGC(plw) = XtGetGC(plw,attribs,&values);

	values.foreground = MultiListBG(plw);
	MultiListEraseGC(plw) = XtGetGC(plw,attribs,&values);

	values.foreground = MultiListHighlightFG(plw);
	values.background = MultiListHighlightBG(plw);
	MultiListHighlightForeGC(plw) = XtGetGC(plw,attribs,&values);

	values.foreground = MultiListHighlightBG(plw);
	values.background = MultiListHighlightBG(plw);
	MultiListHighlightBackGC(plw) = XtGetGC(plw,attribs,&values);

	attribs |= GCTile | GCFillStyle;
	values.foreground = MultiListFG(plw);
	values.background = MultiListBG(plw);
	values.fill_style = FillTiled;
	values.tile = XmuCreateStippledPixmap(XtScreen(plw),MultiListFG(plw),
					      MultiListBG(plw),MultiListDepth(plw));

	MultiListGrayGC(plw) = XtGetGC(plw,attribs,&values);
} /* End CreateNewGCs */

/*===========================================================================*

        L A Y O U T    A N D    G E O M E T R Y    M A N A G E M E N T

 *===========================================================================*/

/*---------------------------------------------------------------------------*

        RecalcCoords(plw,width_changeable,height_changeable)

	This routine takes a MultiList widget <plw> and recalculates
	the coordinates, and item placement based on the current
	width, height, and list of items.  The <width_changeable> and
	<height_changeable> indicate if the width and/or height can
	be arbitrarily set.

	This routine requires that the internal list data be initialized.

 *---------------------------------------------------------------------------*/

static void RecalcCoords(plw,width_changeable,height_changeable)
MultiListWidget plw;
Boolean width_changeable,height_changeable;
{
	String str;
	Dimension width,height;
	register int i,text_width;

	width = MultiListWidth(plw);
	height = MultiListHeight(plw);
	if (MultiListNumItems(plw) != 0 && MultiListLongest(plw) == 0)
	{
		for (i = 0; i < MultiListNumItems(plw); i++)
		{
			str = MultiListItemString(MultiListNthItem(plw,i));
			text_width = FontW(MultiListFont(plw),str);
			MultiListLongest(plw) = max(MultiListLongest(plw),text_width);
		}
	}
	if (Layout(plw,width_changeable,height_changeable,&width,&height))
	{
		NegotiateSizeChange(plw,width,height);
	}
} /* End RecalcCoords */


/*---------------------------------------------------------------------------*

        NegotiateSizeChange(plw,width,height)

	This routine tries to change the MultiList widget <plw> to have the
	new size <width> by <height>.  A negotiation will takes place
	to try to change the size.  The resulting size is not necessarily
	the requested size.

 *---------------------------------------------------------------------------*/

static void NegotiateSizeChange(plw,width,height)
MultiListWidget plw;
Dimension width,height;
{
	int attempt_number;
	Boolean w_fixed,h_fixed;
	Dimension *w_ptr,*h_ptr;
	
	XtWidgetGeometry request,reply;

	request.request_mode = CWWidth | CWHeight;
	request.width = width;
	request.height = height;
    
	for (attempt_number = 1; attempt_number <= 3; attempt_number++)
	{
		switch (XtMakeGeometryRequest(plw,&request,&reply))
		{
		    case XtGeometryYes:
		    case XtGeometryNo:
			return;
		    case XtGeometryAlmost:
			switch (attempt_number)
			{
			    case 1:
				w_fixed = (request.width != reply.width);
				h_fixed = (request.height != reply.height);
				w_ptr = &(reply.width);
				h_ptr = &(reply.height);
				Layout(plw,!w_fixed,!h_fixed,w_ptr,h_ptr);
				break;
			    case 2:
				w_ptr = &(reply.width);
				h_ptr = &(reply.height);
				Layout(plw,False,False,w_ptr,h_ptr);
				break;
			    case 3:
				return;
			}
			break;
		    default:
			XtAppWarning(XtWidgetToApplicationContext(plw),
				     "MultiList Widget: Unknown geometry return.");
			break;
		}
		request = reply;
	}
} /* End NegotiateSizeChange */


/*---------------------------------------------------------------------------*

	Boolean Layout(plw,w_changeable,h_changeable,w_ptr,h_ptr)

	This routine tries to generate a layout for the MultiList widget
	<plw>.  The Layout routine is free to arbitrarily set the width
	or height if the corresponding variables <w_changeable> and
	<h_changeable> are set True.  Otherwise the original width or
	height in <w_ptr> and <h_ptr> are used as fixed values.  The
	resulting new width and height are stored back through the
	<w_ptr> and <h_ptr> pointers.  False is returned if no size
	change was done, True is returned otherwise.

 *---------------------------------------------------------------------------*/

static Boolean Layout(plw,w_changeable,h_changeable,w_ptr,h_ptr)
MultiListWidget plw;
Boolean w_changeable,h_changeable;
Dimension *w_ptr,*h_ptr;
{
	Boolean size_changed;

	/*
	 * If force columns is set, then always use the number
	 * of columns specified by default_cols.
	 */

	MultiListColWidth(plw) = MultiListLongest(plw) + MultiListColumnSpace(plw);
	MultiListRowHeight(plw) = FontH(MultiListFont(plw)) + MultiListRowSpace(plw);
	if (MultiListForceCols(plw))
	{
		MultiListNumCols(plw) = max(MultiListDefaultCols(plw),1);
		if (MultiListNumItems(plw) == 0)
			MultiListNumRows(plw) = 1;
		    else
			MultiListNumRows(plw) = (MultiListNumItems(plw) - 1) /
				MultiListNumCols(plw) + 1;
		if (w_changeable)
		{
			*w_ptr = MultiListNumCols(plw) * MultiListColWidth(plw);
			size_changed = True;
		}
		    else
		{
			MultiListColWidth(plw) = *w_ptr / MultiListNumCols(plw);
		}
		if (h_changeable)
		{
			*h_ptr = MultiListNumRows(plw) * MultiListRowHeight(plw);
			size_changed = True;
		}
		return(size_changed);
	}

	/*
	 * If both width and height are free to change then use
	 * default_cols to determine the number of columns and set
	 * the new width and height to just fit the window.
	 */

	if (w_changeable && h_changeable)
	{
		MultiListNumCols(plw) = max(MultiListDefaultCols(plw),1);
		if (MultiListNumItems(plw) == 0)
			MultiListNumRows(plw) = 1;
		    else
			MultiListNumRows(plw) = (MultiListNumItems(plw) - 1) /
				MultiListNumCols(plw) + 1;
		*w_ptr = MultiListNumCols(plw) * MultiListColWidth(plw);
		*h_ptr = MultiListNumRows(plw) * MultiListRowHeight(plw);
		return(True);
	}

	/*
	 * If the width is fixed then use it to determine the
	 * number of columns.  If the height is free to move
	 * (width still fixed) then resize the height of the
	 * widget to fit the current MultiList exactly.
	 */

	if (!w_changeable)
	{
		MultiListNumCols(plw) = *w_ptr / MultiListColWidth(plw);
		MultiListNumCols(plw) = max(MultiListNumCols(plw),1);
		MultiListNumRows(plw) = (MultiListNumItems(plw) - 1) /
			MultiListNumCols(plw) + 1;
		MultiListColWidth(plw) = *w_ptr / MultiListNumCols(plw);
		if (h_changeable)
		{
			*h_ptr = MultiListNumRows(plw) * MultiListRowHeight(plw);
			size_changed = True;
		}
		return(size_changed);
	}

	/*
	 * The last case is xfree and !yfree we use the height to
	 * determine the number of rows and then set the width to
	 * just fit the resulting number of columns.
	 */

	if (!h_changeable)
	{
		MultiListNumRows(plw) = *h_ptr / MultiListRowHeight(plw);
		MultiListNumRows(plw) = max(MultiListNumRows(plw),1);
		MultiListNumCols(plw) = (MultiListNumItems(plw) - 1) /
			MultiListNumRows(plw) + 1;
		*w_ptr = MultiListNumCols(plw) * MultiListColWidth(plw);
		return(True);
	}      
} /* End Layout */

/*===========================================================================*

                    R E D R A W    R O U T I N E S

 *===========================================================================*/

/*---------------------------------------------------------------------------*

	RedrawAll(plw)

	This routine simple calls Redisplay to redraw the entire
	MultiList widget <plw>.

 *---------------------------------------------------------------------------*/

static void RedrawAll(plw)
MultiListWidget plw;
{
	Redisplay(plw,NULL,NULL);
} /* End RedrawAll */


/*---------------------------------------------------------------------------*

	RedrawItem(plw,item_index)

	This routine redraws the item with index <item_index> in the
	MultiList widget <plw>.  If the item number is bad, nothing is drawn.

 *---------------------------------------------------------------------------*/

static void RedrawItem(plw,item_index)
MultiListWidget plw;
int item_index;
{
	int row,column;

	if (ItemToRowColumn(plw,item_index,&row,&column))
	{
		RedrawRowColumn(plw,row,column);
	}
} /* End RedrawItem */


/*---------------------------------------------------------------------------*

	RedrawRowColumn(plw,row,column)

	This routine paints the item in row/column position <row>,<column>
	on the MultiList widget <plw>.  If the row/column coordinates are
	outside the widget, nothing is drawn.  If the position is empty,
	blank space is drawn.

 *---------------------------------------------------------------------------*/

static void RedrawRowColumn(plw,row,column)
MultiListWidget plw;
int row,column;
{
	GC bg_gc,fg_gc;
	MultiListItem *item;
	int ul_x,ul_y,str_x,str_y,w,h,item_index,has_item,text_h;

	if (!XtIsRealized(plw)) return;
	has_item = RowColumnToItem(plw,row,column,&item_index);
	RowColumnToPixels(plw,row,column,&ul_x,&ul_y,&w,&h);

	if (has_item == False)					/* No Item */
	{
		bg_gc = MultiListEraseGC(plw);
	}
	    else
	{
		item = MultiListNthItem(plw,item_index);
		if (!MultiListItemSensitive(item))			/* Disabled */
		{
			bg_gc = MultiListEraseGC(plw);
			fg_gc = MultiListGrayGC(plw);
		}
		    else if (MultiListItemHighlighted(item))	/* Selected */
		{
			bg_gc = MultiListHighlightBackGC(plw);
			fg_gc = MultiListHighlightForeGC(plw);
		}
		    else					/* Normal */
		{
			bg_gc = MultiListEraseGC(plw);
			fg_gc = MultiListDrawGC(plw);
		}
	}
	XFillRectangle(XtDisplay(plw),XtWindow(plw),bg_gc,ul_x,ul_y,w,h);
	if (has_item == True)
	{
		text_h = min(FontH(MultiListFont(plw)) + MultiListRowSpace(plw),
			     MultiListRowHeight(plw));
		str_x = ul_x + MultiListColumnSpace(plw) / 2;
		str_y = ul_y + FontAscent(MultiListFont(plw)) +
			(MultiListRowHeight(plw) - text_h) / 2;
		XDrawString(XtDisplay(plw),XtWindow(plw),fg_gc,
			    str_x,str_y,MultiListItemString(item),
			    strlen(MultiListItemString(item)));
	}
} /* End RedrawRowColumn */
	
/*===========================================================================*

               I T E M    L O C A T I O N    R O U T I N E S

 *===========================================================================*/

/*---------------------------------------------------------------------------*

	void PixelToRowColumn(plw,x,y,row_ptr,column_ptr)

	This routine takes pixel coordinates <x>, <y> and converts
	the pixel coordinate into a row/column coordinate.  This row/column
	coordinate can then easily be converted into the specific item
	in the list via the function RowColumnToItem().

	If the pixel lies in blank space outside of the items, the
	row & column numbers will be outside of the range of normal
	row & columns numbers, but will correspond to the row & column
	of the item, if an item was actually there.

 *---------------------------------------------------------------------------*/

static void PixelToRowColumn(plw,x,y,row_ptr,column_ptr)
MultiListWidget plw;
int x,y,*row_ptr,*column_ptr;
{
	*row_ptr = y / MultiListRowHeight(plw);
	*column_ptr = x / MultiListColWidth(plw);
} /* End PixelToRowColumn */

/*---------------------------------------------------------------------------*

	void RowColumnToPixels(plw,row,col,x_ptr,y_ptr,w_ptr,h_ptr)

	This routine takes a row/column coordinate <row>,<col> and
	converts it into the bounding pixel rectangle which is returned.

 *---------------------------------------------------------------------------*/

static void RowColumnToPixels(plw,row,col,x_ptr,y_ptr,w_ptr,h_ptr)
MultiListWidget plw;
int row,col,*x_ptr,*y_ptr,*w_ptr,*h_ptr;
{
	*x_ptr = col * MultiListColWidth(plw);
	*y_ptr = row * MultiListRowHeight(plw);
	*w_ptr = MultiListColWidth(plw);
	*h_ptr = MultiListRowHeight(plw);
} /* End RowColumnToPixels */

/*---------------------------------------------------------------------------*

	Boolean RowColumnToItem(plw,row,column,item_ptr)

	This routine takes a row number <row> and a column number <column>
	and tries to resolve this row and column into the index of the
	item in this position of the MultiList widget <plw>.  The resulting
	item index is placed through <item_ptr>.  If there is no item at
	this location, False is returned, else True is returned.

 *---------------------------------------------------------------------------*/

static Boolean RowColumnToItem(plw,row,column,item_ptr)
MultiListWidget plw;
int row,column,*item_ptr;
{
	register int x_stride,y_stride;

	if (row < 0 || row >= MultiListNumRows(plw) ||
	    column < 0 || column >= MultiListNumCols(plw))
	{
		return(False);
	}
	if (MultiListRowMajor(plw))
	{
		x_stride = 1;
		y_stride = MultiListNumCols(plw);
	}
	    else
	{
		x_stride = MultiListNumRows(plw);
		y_stride = 1;
	}
	*item_ptr = row * y_stride + column * x_stride;
	if (*item_ptr >= MultiListNumItems(plw))
		return(False);
	    else
		return(True);
} /* End RowColumnToItem */


/*---------------------------------------------------------------------------*

	Boolean ItemToRowColumn(plw,item_index,row_ptr,column_ptr)

	This routine takes an item number <item_index> and attempts
	to convert the index into row and column numbers stored through
	<row_ptr> and <column_ptr>.  If the item number does not
	corespond to a valid item, False is returned, else True is
	returned.

 *---------------------------------------------------------------------------*/

static Boolean ItemToRowColumn(plw,item_index,row_ptr,column_ptr)
MultiListWidget plw;
int item_index,*row_ptr,*column_ptr;
{
	if (item_index < 0 || item_index >= MultiListNumItems(plw))
	{
		return(False);
	}
	if (MultiListRowMajor(plw))
	{
		*row_ptr = item_index / MultiListNumCols(plw);
		*column_ptr = item_index % MultiListNumCols(plw);
	}
	else
	{
		*row_ptr = item_index % MultiListNumRows(plw);
		*column_ptr = item_index / MultiListNumRows(plw);
	}
	return(True);
} /* End ItemToRowColumn */

/*===========================================================================*

                E V E N T    A C T I O N    H A N D L E R S

 *===========================================================================*/

/*---------------------------------------------------------------------------*

	Set(plw,event,params,num_params)

	This function sets a single text item in the MultiList.  Any previously
	selected items will be unselected, even if the user later aborts
	the click.  The item clicked on will be highlighted, and the
	MultiListMostRecentItem(plw) variable will be set to the item clicked
	on if any, or -1 otherwise.

 *---------------------------------------------------------------------------*/

/* ARGSUSED */
static void Set(plw,event,params,num_params)
MultiListWidget plw;
XEvent *event;
String *params;
Cardinal *num_params;
{
	String string;
	int click_x,click_y;
	int status,item_index,string_length,row,column;

	click_x = event->xbutton.x;
	click_y = event->xbutton.y;
	PixelToRowColumn(plw,click_x,click_y,&row,&column);
	MultiListUnhighlightAll(plw);
	status = RowColumnToItem(plw,row,column,&item_index);
	if (status == False) item_index = -1;
	MultiListHighlightItem(plw,item_index);
	MultiListMostRecentAct(plw) = MULTILIST_ACTION_HIGHLIGHT;
	MultiListMostRecentItem(plw) = item_index;
} /* End Set */


/*---------------------------------------------------------------------------*

	Unset(plw,event,params,num_params)

	This function sets a single text item in the MultiList.  If the item
	is already unset, then nothing happens.  Otherwise, the item is
	unset and the selection array and selection count are updated.

 *---------------------------------------------------------------------------*/

/* ARGSUSED */
static void Unset(plw,event,params,num_params)
MultiListWidget plw;
XEvent *event;
String *params;
Cardinal *num_params;
{
	String string;
	int click_x,click_y;
	int status,item_index,string_length,row,column;

	click_x = event->xbutton.x;
	click_y = event->xbutton.y;
	PixelToRowColumn(plw,click_x,click_y,&row,&column);
	status = RowColumnToItem(plw,row,column,&item_index);
	if (status == False) item_index = -1;
	MultiListUnhighlightItem(plw,item_index);
	MultiListMostRecentAct(plw) = MULTILIST_ACTION_UNHIGHLIGHT;
	MultiListMostRecentItem(plw) = item_index;
} /* End Unset */


/*---------------------------------------------------------------------------*

	Toggle(plw,event,params,num_params)

	This function toggles a text item in the MultiList, while leaving the
	other selections intact (up to the allowed number of selections).
	The item clicked on will be highlighted, and MultiListMostRecentItem(plw)
	will be set to the item number clicked on, or -1 if no item was clicked
	on.

 *---------------------------------------------------------------------------*/

/* ARGSUSED */
static void Toggle(plw,event,params,num_params)
MultiListWidget plw;
XEvent *event;
String *params;
Cardinal *num_params;
{
	String string;
	int click_x,click_y;
	int status,item_index,string_length,row,column;

	click_x = event->xbutton.x;
	click_y = event->xbutton.y;
	PixelToRowColumn(plw,click_x,click_y,&row,&column);
	status = RowColumnToItem(plw,row,column,&item_index);
	if (status == False) item_index = -1;
	MultiListMostRecentAct(plw) = MultiListToggleItem(plw,item_index);
	MultiListMostRecentItem(plw) = item_index;
} /* End Toggle */


/*---------------------------------------------------------------------------*

	Open(plw,event,params,num_params)

	This routine handles the opening (normally double clicking)
	of an item.  All previous selections will be unselected, the
	clicked item will be selected, and the OPEN action will be recorded.

 *---------------------------------------------------------------------------*/

/* ARGSUSED */
static void Open(plw,event,params,num_params)
MultiListWidget plw;
XEvent *event;
String *params;
Cardinal *num_params;
{
	int click_x,click_y;
	int status,item_index,string_length,row,column;

	click_x = event->xbutton.x;
	click_y = event->xbutton.y;
	PixelToRowColumn(plw,click_x,click_y,&row,&column);
	MultiListUnhighlightAll(plw);
	status = RowColumnToItem(plw,row,column,&item_index);
	if (status == False) item_index = -1;
	MultiListHighlightItem(plw,item_index);
	MultiListMostRecentAct(plw) = MULTILIST_ACTION_OPEN;
	MultiListMostRecentItem(plw) = item_index;
} /* End Open */


/*---------------------------------------------------------------------------*

	OpenMany(plw,event,params,num_params)

	This routine handles the opening (normally double clicking)
	of an item.  All previous selections will remain selected,
	which is why this routine is called OpenMany.  Many selections
	can be returned on an Open.  The clicked item will be selected,
	and the OPEN action will be recorded.

 *---------------------------------------------------------------------------*/

/* ARGSUSED */
static void OpenMany(plw,event,params,num_params)
MultiListWidget plw;
XEvent *event;
String *params;
Cardinal *num_params;
{
	int click_x,click_y;
	int status,item_index,string_length,row,column;

	click_x = event->xbutton.x;
	click_y = event->xbutton.y;
	PixelToRowColumn(plw,click_x,click_y,&row,&column);
	status = RowColumnToItem(plw,row,column,&item_index);
	if (status == False) item_index = -1;
	MultiListHighlightItem(plw,item_index);
	MultiListMostRecentAct(plw) = MULTILIST_ACTION_OPEN;
	MultiListMostRecentItem(plw) = item_index;
} /* End OpenMany */


/*---------------------------------------------------------------------------*

	Notify(plw,event,params,num_params)

	This function performs the Notify action, which is what happens
	when a user releases a button after clicking on an item.  If there
	was no item under the click, or if the item was insensitive, then
	Notify simply returns.  Otherwise, notify notifies the user via
	a callback of the current list of selected items.

	In addition, if the XtNpasteBuffer resource is true and a valid
	sensitive item was clicked on, the name of the last clicked on item
	will be placed in the X cut buffer (buf(0)).

 *---------------------------------------------------------------------------*/

/* ARGSUSED */
static void Notify(plw,event,params,num_params)
MultiListWidget plw;
XEvent *event;
String *params;
Cardinal *num_params;
{
	String string;
	int final_x,final_y;
	Boolean item_valid;
	int item_index,string_length,row,column;
	MultiListReturnStruct ret_value;

	final_x = event->xbutton.x;
	final_y = event->xbutton.y;
	PixelToRowColumn(plw,final_x,final_y,&row,&column);
	item_valid = RowColumnToItem(plw,row,column,&item_index);
	if (!item_valid || (item_index != MultiListMostRecentItem(plw)))
	{
		if (MultiListMostRecentItem(plw) != -1)	/* Moved Away */
		{
			MultiListUnhighlightItem(plw,MultiListMostRecentItem(plw));
		}
		else					/* Bad Click */
		{
			MultiListUnhighlightAll(plw);
			if (MultiListNotifyUnsets(plw))
			{
				ret_value.action = MULTILIST_ACTION_UNHIGHLIGHT;
				ret_value.item = -1;
				ret_value.string = NULL;
				ret_value.num_selected = MultiListNumSelected(plw);
				ret_value.selected_items = MultiListSelArray(plw);
				XtCallCallbacks(plw,XtNcallback,&ret_value);
			}
		}
		return;
	}

	string = MultiListItemString(MultiListNthItem(plw,item_index));
	string_length = strlen(string);
	if (MultiListMostRecentAct(plw) == MULTILIST_ACTION_HIGHLIGHT)
	{
		if (MultiListPaste(plw))
			XStoreBytes(XtDisplay(plw),string,string_length);
	}

	if (((MultiListMostRecentAct(plw) == MULTILIST_ACTION_HIGHLIGHT) &&
	     (MultiListNotifySets(plw) == True)) ||
	    ((MultiListMostRecentAct(plw) == MULTILIST_ACTION_UNHIGHLIGHT) &&
	     (MultiListNotifyUnsets(plw) == True)) ||
	    ((MultiListMostRecentAct(plw) == MULTILIST_ACTION_OPEN) &&
	     (MultiListNotifyOpens(plw) == True)))
	{
		ret_value.action = MultiListMostRecentAct(plw);
		ret_value.item = MultiListMostRecentItem(plw);
		ret_value.string = string;
		ret_value.num_selected = MultiListNumSelected(plw);
		ret_value.selected_items = MultiListSelArray(plw);
		XtCallCallbacks(plw,XtNcallback,(caddr_t)&ret_value);
	}
} /* End Notify */

/*===========================================================================*

        U S E R    C A L L A B L E    U T I L I T Y    R O U T I N E S

 *===========================================================================*/

/*---------------------------------------------------------------------------*

	MultiListHighlightItem(plw,item_index)

	This routine selects an item with index <item_index> in the
	MultiList widget <plw>.  If a maximum number of selections is specified
	and exceeded, the earliest selection will be unselected.  If
	<item_index> doesn't correspond to an item the most recently
	clicked item will be set to -1 and this routine will immediately
	return, otherwise the most recently clicked item will be set to the
	current item,

 *---------------------------------------------------------------------------*/

void MultiListHighlightItem(plw,item_index)
MultiListWidget plw;
int item_index;
{
	int i;
	MultiListItem *item;

	if (MultiListMaxSelectable(plw) == 0) return;
	if (item_index < 0 || item_index >= MultiListNumItems(plw))
	{
		MultiListMostRecentItem(plw) = -1;
		return;
	}
	item = MultiListNthItem(plw,item_index);
	if (MultiListItemSensitive(item) == False)
	{
		MultiListUnhighlightAll(plw);
		return;
	}
	MultiListMostRecentItem(plw) = item_index;
	if (MultiListItemHighlighted(item) == True) return;
	if (MultiListNumSelected(plw) == MultiListMaxSelectable(plw))
	{
		MultiListUnhighlightItem(plw,MultiListSelArray(plw)[0]);
		for (i = 1; i < MultiListNumSelected(plw) + 1; i++)
		{
			MultiListSelArray(plw)[i - 1] = MultiListSelArray(plw)[i];
		}
	}
	MultiListItemHighlighted(item) = True;
	MultiListSelArray(plw)[MultiListNumSelected(plw)] = item_index;
	++ MultiListNumSelected(plw);
	RedrawItem(plw,item_index);
} /* End MultiListHighlightItem */


/*---------------------------------------------------------------------------*

	MultiListHighlightAll(plw)

	This routine highlights all highlightable items in the MultiList
	widget <plw>, up to the maximum number of allowed highlightable
	items;

 *---------------------------------------------------------------------------*/

void MultiListHighlightAll(plw)
MultiListWidget plw;
{
	int i;
	MultiListItem *item;

	MultiListNumSelected(plw) = 0;
	for (i = 0; i < MultiListNumItems(plw); i++)
	{
		item = MultiListNthItem(plw,i);
		MultiListItemHighlighted(item) = True;
	}
	for (i = 0; i < MultiListNumItems(plw); i++)
	{
		if (MultiListNumSelected(plw) == MultiListMaxSelectable(plw)) break;
		item = MultiListNthItem(plw,i);
		MultiListItemHighlighted(item) = True;
		MultiListSelArray(plw)[MultiListNumSelected(plw)] = i;
		++ MultiListNumSelected(plw);
	}
	RedrawAll(plw);
} /* End MultiListHighlightAll */


/*---------------------------------------------------------------------------*

	MultiListUnhighlightItem(plw,item_index)

	This routine unselects the item with index <item_index> in the
	MultiList widget <plw>.  If <item_index> doesn't correspond to a
	selected item, then nothing will happen.  Otherwise, the item
	is unselected and the selection array and count are updated.

 *---------------------------------------------------------------------------*/

void MultiListUnhighlightItem(plw,item_index)
MultiListWidget plw;
int item_index;
{
	int i;
	MultiListItem *item;

	if (MultiListMaxSelectable(plw) == 0) return;
	if (item_index < 0 || item_index >= MultiListNumItems(plw)) return;
	item = MultiListNthItem(plw,item_index);
	if (MultiListItemHighlighted(item) == False) return;
	MultiListItemHighlighted(item) = False;

	for (i = 0; i < MultiListNumSelected(plw); i++)
		if (MultiListSelArray(plw)[i] == item_index) break;
	for (i = i + 1; i < MultiListNumSelected(plw); i++)
		MultiListSelArray(plw)[i - 1] = MultiListSelArray(plw)[i];
	-- MultiListNumSelected(plw);

	RedrawItem(plw,item_index);
} /* End MultiListUnhighlightItem */


/*---------------------------------------------------------------------------*

	MultiListUnhighlightAll(plw)

	This routine unhighlights all items in the MultiList widget <plw>.

 *---------------------------------------------------------------------------*/

void MultiListUnhighlightAll(plw)
MultiListWidget plw;
{
	int i;
	MultiListItem *item;

	for (i = 0; i < MultiListNumItems(plw); i++)
	{
		item = MultiListNthItem(plw,i);
		if (MultiListItemHighlighted(item)) MultiListUnhighlightItem(plw,i);
	}
	MultiListNumSelected(plw) = 0;
} /* End MultiListUnhighlightAll */


/*---------------------------------------------------------------------------*

	int MultiListToggleItem(plw,item_index)

	This routine highlights the item with index <item_index>
	if it is unhighlighted and unhighlights it if it is already
	highlighted.  The action performed by the toggle is returned
	(MULTILIST_ACTION_HIGHLIGHT or MULTILIST_ACTION_UNHIGHLIGHT).

 *---------------------------------------------------------------------------*/

int MultiListToggleItem(plw,item_index)
MultiListWidget plw;
{
	int i;
	MultiListItem *item;

	if (MultiListMaxSelectable(plw) == 0) return;
	if (item_index < 0 || item_index >= MultiListNumItems(plw)) return;
	item = MultiListNthItem(plw,item_index);
	if (MultiListItemHighlighted(item))
	{
		MultiListUnhighlightItem(plw,item_index);
		return(MULTILIST_ACTION_UNHIGHLIGHT);
	}
	else
	{
		MultiListHighlightItem(plw,item_index);
		return(MULTILIST_ACTION_HIGHLIGHT);
	}
} /* End MultiListToggleItem */


/*---------------------------------------------------------------------------*

	MultiListReturnStruct *MultiListGetHighlighted(plw)

	This routine takes a MultiList widget <plw> and returns a
	MultiListReturnStruct whose num_selected and selected_items
	fields contain the highlight information.  The action field
	is set to MULTILIST_ACTION_STATUS, and the item_index and string
	fields are invalid.

 *---------------------------------------------------------------------------*/

MultiListReturnStruct *MultiListGetHighlighted(plw)
MultiListWidget plw;
{
	MultiListItem *item;
	static MultiListReturnStruct ret_value;

	ret_value.action = MULTILIST_ACTION_STATUS;
	if (MultiListNumSelected(plw) == 0)
	{
		ret_value.item = -1;
		ret_value.string = NULL;
	}
	    else
	{
		ret_value.item = MultiListSelArray(plw)[MultiListNumSelected(plw) - 1];
		item = MultiListNthItem(plw,ret_value.item);
		ret_value.string = MultiListItemString(item);
	}
	ret_value.num_selected = MultiListNumSelected(plw);
	ret_value.selected_items = MultiListSelArray(plw);
	return(&ret_value);
} /* End MultiListGetHighlighted */


/*---------------------------------------------------------------------------*

	Boolean MultiListIsHighlighted(plw,item_index)

	This routine checks if the item with index <item_index>
	is highlighted and returns True or False depending.  If
	<item_index> is invalid, False is returned.

 *---------------------------------------------------------------------------*/

Boolean MultiListIsHighlighted(plw,item_index)
MultiListWidget plw;
int item_index;
{
	MultiListItem *item;

	if (item_index < 0 || item_index >= MultiListNumItems(plw)) return(False);
	item = MultiListNthItem(plw,item_index);
	return(MultiListItemHighlighted(item));
} /* End MultiListIsHighlighted */


/*---------------------------------------------------------------------------*

	Boolean MultiListGetItemInfo(plw,item_index,str_ptr,h_ptr,s_ptr)

	This routine returns the string, highlight status and
	sensitivity information for the item with index <item_index>
	via the pointers <str_ptr>, <h_ptr> and <s_ptr>.  If the item
	index is invalid, False is returned, else True is returned.

 *---------------------------------------------------------------------------*/

Boolean MultiListGetItemInfo(plw,item_index,str_ptr,h_ptr,s_ptr)
MultiListWidget plw;
int item_index;
String *str_ptr;
Boolean *h_ptr,*s_ptr;
{
	MultiListItem *item;

	if (item_index < 0 || item_index >= MultiListNumItems(plw)) return(False);
	item = MultiListNthItem(plw,item_index);
	*str_ptr = MultiListItemString(item);
	*h_ptr = MultiListItemHighlighted(item);
	*s_ptr = MultiListItemSensitive(item);
	return(True);
} /* End MultiListGetItemInfo */


/*---------------------------------------------------------------------------*

	MultiListSetNewData(plw,list,nitems,longest,resize,sensitivity_array)

	This routine will set a new set of strings <list> into the
	MultiList widget <plw>.  If <resize> is True, the MultiList widget will
	try to resize itself.

 *---------------------------------------------------------------------------*/

void MultiListSetNewData(plw,list,nitems,longest,resize,sensitivity_array)
MultiListWidget plw;
String *list;
int nitems,longest;
Boolean resize;
Boolean *sensitivity_array;
{
	DestroyOldData(plw);
	MultiListList(plw) = list;
	MultiListNumItems(plw) = max(nitems,0);
	MultiListLongest(plw) = max(longest,0);
	MultiListSensitiveArray(plw) = sensitivity_array;
	InitializeNewData(plw);
	RecalcCoords(plw,resize,resize);
	if (XtIsRealized(plw)) Redisplay(plw,NULL,NULL);
} /* End MultiListSetNewData */
