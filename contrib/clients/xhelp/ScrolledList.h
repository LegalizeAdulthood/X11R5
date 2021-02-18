/*****************************************************************************

	ScrolledList.h

	This file contains the user includes for the ScrolledList widget.

	April 1990, Brian Totty

******************************************************************************/

#ifndef _SCROLLEDLIST_H_
#define _SCROLLEDLIST_H_

extern WidgetClass		scrolledListWidgetClass;

typedef struct _ScrolledListClassRec *ScrolledListWidgetClass;
typedef struct _ScrolledListRec      *ScrolledListWidget;

typedef struct
{
	Widget multilist;		/* The list widget clicked on */
	String string;		/* The name of the item clicked on */
	int index;		/* The index of the item clicked on */
	Boolean highlighted;	/* Are any items highlighted? */
} ScrolledListReturnStruct;

#define	XtNlist			"list"
#define	XtNnumberStrings	"numberStrings"

/*===========================================================================*

                    E X P O R T E D    F U N C T I O N S

 *===========================================================================*/

void				ScrolledListSetList();
void				ScrolledListUnhighlightAll();
void				ScrolledListHighlightItem();
ScrolledListReturnStruct	*ScrolledListGetHighlighted();
Boolean				ScrolledListIsHighlighted();
Boolean				ScrolledListGetItem();

#endif

