/****************************************************************************

	MultiList.h

	This file is the public header file for the MultiList widget, an
	extension to the Athena List widget.

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

#ifndef _MULTILIST_H_
#define _MULTILIST_H_

#include <X11/Xaw/Simple.h>

/*---------------------------------------------------------------------------*

      R E S O U R C E    D E S C R I P T I O N S    A N D    N O T E S

 *---------------------------------------------------------------------------*/

/*

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 callback            Callback           XtCallbackList  NULL                *1
 columnWidth         Width              Dimension       0                   *10
 columnSpacing       Spacing            Dimension       8
 cursor		     Cursor		Cursor		left_ptr
 defaultColumns      Columns            int             1                   *2
 destroyCallback     Callback		Pointer		NULL 
 font		     Font		XFontStruct*	XtDefaultFont
 forceColumns        Columns            Boolean         False               *2
 foreground	     Foreground		Pixel		XtDefaultForeground
 height		     Height		Dimension	0                   *3
 highlightBackground HBackground	Pixel		XtDefaultForeground *4
 highlightForeground HForeground	Pixel		XtDefaultBackground *4
 insensitiveBorder   Insensitive	Pixmap		Gray
 list                List               String *        NULL                *5
 longest             Longest            int             0                   *6
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 maxSelectable       Value              int             1                   *7
 notifyHighlights    Boolean            Boolean         True                *8
 notifyOpens         Boolean            Boolean         True                *8
 notifyUnhighlights  Boolean            Boolean         True                *8
 numberStrings       NumberStrings      int             0                   *5
 pasteBuffer         Boolean            Boolean         False
 rowHeight           Height             Dimension       0                   *10
 rowSpacing          Spacing            Dimension       2
 sensitive	     Sensitive		Boolean		True
 sensitiveArray      List               Boolean *       NULL                *9
 verticalList        Boolean            Boolean         False
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

 *1 -  The callback functions are called whenever a highlight, unhighlight,
       or open takes place, and the appropriate notification resource is
       set true.  The callbacks get called with a MultiListReturnStruct pointer
       which points to the data about the action that just occurred.

 *2 -  The defaultColumns resource is used in two cases.  If forceColumns
       is true, the widget will set the number of columns to the value of
       default columns.  If the widget width is unconstrained by the parent
       widgets, the defaultColumns is also used to determine the number of
       columns and the resulting width.  Otherwise, the number of columns
       will be calcultaed based on the current width and will be changed to
       an appropriate value.

 *3 -  If the width or height is set to zero (0), which is the default case,
       then the widget will calculate the size of that dimension to be just
       large enough to hold the contents of the widget.

 *4 -  The highlightForeground and highlightBackground colors specify the
       colors used to highlight the text (foreground) and the surrounding
       background space of a list item when it is selected (highlighted).
       The default is the reverse of the default foreground and background
       colors.

 *5 -  The list resource is an array of strings (char * array) which tell
       the names of each item of the list.  The number of elements of this
       array are indicated by the resource numberStrings.  If numberStrings
       is set to 0 (the default), then the MultiList widget will count the
       number of strings in the list.  This requires that the list be
       NULL terminated.  If list is NULL, then the widget treats it as an
       empty list.  Once the list is set the list resource is set to NULL,
       so you won't be able to read back the list after it has been set.  The
       widgets copies the strings internally, so the user can free the list
       storage after setting it.

 *6 -  This resource represent the longest string in pixels.  If this
       resource is zero (0), which is the default and probably the value
       most people should use, the longest string length is calculated
       and the resource is updated.

 *7 -  The maxSelectable resource indicates the maximum number of items
       which can be selected at any one time.  In the original Athena
       widget, you could have at most one item selected at a time.  In
       this widget, you can choose how many will be selected at a time.

 *8 -  When an item is highlighted or becomes unhighlighted, or when
       the Open() action is run, a callback can be issued indicating the
       new state of what items are highlighted and what action occurred.
       The notify resources are booleans that enable or disable callbacks
       in various cases.  If you are only interested in being notified
       when the user selects an item, and not when he deselects items,
       then only notifyHighlights needs to be True.

 *9 -  Each item in the MultiList can be made insensitive, so it is printed in
       gray shading and can not be highlighted.  This can be done by
       setting the sensitivity list, which is an array of Booleans which
       indicate whether or not the corresponding item is sensitive (can be
       selected).  If sensitivity list is NULL, all items are sensitive.  The
       widget copies the sensitivity information, so the user can delete the
       sensitivity array storage after setting it.  The widget sets the
       resource to NULL after it has been set, so the user cannot read the
       old list back.

 *10 - These values are intended for reading only.  They indicate the pixel
       width/height of the column/row.

*/

/*---------------------------------------------------------------------------*

                    S T R I N G    D E F I N I T I O N S

 *---------------------------------------------------------------------------*/

#define XtCList			"List"
#define XtCSpacing		"Spacing"
#define XtCColumns		"Columns"
#define XtCLongest		"Longest"
#define XtCNumberStrings	"NumberStrings"
#define	XtCHForeground		"HForeground"
#define	XtCHBackground		"HBackground"

#define XtNcursor		"cursor"

#define	XtNhighlightForeground	"highlightForeground"
#define	XtNhighlightBackground	"highlightBackground"
#define XtNcolumnSpacing	"columnSpacing"
#define XtNrowSpacing		"rowSpacing"
#define XtNdefaultColumns	"defaultColumns"
#define XtNforceColumns		"forceColumns"
#define XtNpasteBuffer		"pasteBuffer"
#define XtNverticalList		"verticalList"
#define XtNlongest		"longest"
#define XtNnumberStrings	"numberStrings"
#define XtNlist			"list"
#define	XtNsensitiveArray	"sensitiveArray"
#define	XtNmaxSelectable	"maxSelectable"
#define	XtNnotifyHighlights	"notifyHighlights"
#define	XtNnotifyUnhighlights	"notifyUnhighlights"
#define	XtNnotifyOpens		"notifyOpens"

#define	XtNrowHeight		"rowHeight"
#define	XtNcolumnWidth		"columnWidth"
 
	/* Class Record Constants */

extern WidgetClass multiListWidgetClass;

typedef struct _MultiListClassRec *MultiListWidgetClass;
typedef struct _MultiListRec      *MultiListWidget;

/*---------------------------------------------------------------------------*

                     R E T U R N    S T R U C T U R E

 *---------------------------------------------------------------------------*/

#define	MULTILIST_ACTION_HIGHLIGHT		0
#define	MULTILIST_ACTION_UNHIGHLIGHT		1
#define	MULTILIST_ACTION_OPEN			2
#define	MULTILIST_ACTION_STATUS			3

typedef struct _MultiListReturnStruct
{
	int action;
	int item;
	String string;
	int num_selected;
	int *selected_items;
} MultiListReturnStruct;

/*---------------------------------------------------------------------------*

                     U T I L I T Y    R O U T I N E S

 *---------------------------------------------------------------------------*/

void				MultiListHighlightItem();
void				MultiListHighlightAll();
void				MultiListUnhighlightItem();
void 				MultiListUnhighlightAll();
int 				MultiListToggleItem();
Boolean				MultiListIsHighlighted();
Boolean				MultiListGetItemInfo();
MultiListReturnStruct		*MultiListGetHighlighted();

#endif
