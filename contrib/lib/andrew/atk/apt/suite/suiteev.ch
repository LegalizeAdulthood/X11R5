/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidsuiteev_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apt/suite/RCS/suiteev.ch,v 1.10 1991/09/12 19:20:34 bobg Exp $";
#endif

/*
    $Log: suiteev.ch,v $
Revision 1.10  1991/09/12  19:20:34  bobg
Update copyright notice

Revision 1.9  1990/05/08  15:50:21  gk5g
Just cleaned up a bit.

Revision 1.8  89/11/02  18:58:27  gk5g
Added instance variable firstvisiblesubstring.

Revision 1.7  89/09/29  15:54:27  gk5g
Added extra argument to ShrinkWrap(width, height).

Revision 1.6  89/08/25  17:47:00  gk5g
More changes for V1.0 of the documentation.

Revision 1.5  89/08/24  19:47:54  gk5g
Changes in support of V1.0 of the SuiteProgGuide.doc.

Revision 1.4  89/07/28  19:05:02  gk5g
Moved methods suite_MaxStringSize, MaxSubStringSize, suite_SetItemToReadWrite from suite.c to suiteev.c.

Revision 1.3  89/07/13  16:14:18  gk5g
Added method suiteev_ShrinkWrap() to implement wrapping of long List items.

Revision 1.2  89/05/11  14:37:03  gk5g
Changed suiteev.c to not talke the input focus.
Made suite_Update() call suiteev_Update() and then scroll_Update() if there is a scroll.
Added endzone handlers.

Revision 1.1  89/05/04  12:35:28  gk5g
Initial revision

Revision 1.1  89/04/28  20:26:21  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Suite-object

MODULE	suiteev.ch

VERSION	0.0

AUTHOR	TC Peters & GW Keim
	Information Technology Center, Carnegie-Mellon University

DESCRIPTION
	This is the suite of Methods that support the Suite-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  11/01/88	Created (GW Keim)
  05/04/89	Changed to lower-case naming convention (GW Keim)
END-SPECIFICATION  
**********************************************************/

class suiteev : view {
    classprocedures:
	InitializeClass(struct classheader *ClassID) returns boolean;
	InitializeObject(struct classheader *ClassID, struct suiteev *self) returns boolean;
	FinalizeObject(struct classheader *ClassID, struct suiteev *self);
    overrides:
	FullUpdate(enum view_UpdateType type,long left,long top,long width,long height);
	GetInterface(char *type) returns char*;
	Hit(enum view_MouseAction action,long x,long y,long numClicks) returns struct view *;
	Update();
	PostMenus(struct menulist *menulist);
    methods:
        ShrinkWrap(long width, long height);
        LineCount(char *str) returns long;
	Arrange(struct rectangle *rect);
	DrawItems(struct rectangle *rect);
	NumberItems() returns long;
	NumberVisible() returns long;
	NumberExposed() returns long;
	Clear() returns void;
	WhichItem(long x,long y) returns struct suite_item *;
	ItemUpdate(struct suite_item *item);
	ItemHit(struct suite_item *item,enum view_MouseAction action,long x,long y,long numClicks) returns struct view *;
	ItemClear(struct suite_item *item);
	ItemBlackOut(struct suite_item *item);
	ItemToggle(struct suite_item *item);
	ItemHighlightReverseVideo(struct suite_item *item,boolean border);
	ItemHighlightBorder(struct suite_item *item );
	ItemHighlightCaptionBoldItalic(struct suite_item *item);
	ItemHighlightCaptionBold(struct suite_item *item);
	ItemHighlightCaptionItalic(struct suite_item *item);
	ItemNormalize(struct suite_item *item);
	ItemDrawCaption(struct suite_item *item,short forcedTransferMode);
	ItemDrawTitle(struct suite_item *item,short forcedTransferMode);
	ItemHighlight(struct suite_item *item);
	ItemClearCaption(struct suite_item *item);
	ItemShade(struct suite_item *item);
	Locate(long x,long y) returns long;
	DrawItemBorder(struct suite_item *item);
	AllocItemArray(long count);
	SetItemToReadWrite( struct suite_item *item) returns void;
	MaxStringSize(long *width,long *height);
    data:
	struct suite	    *parent;
	struct view	    *cvif;
	struct suite_item   *firsthit, *lasthit;
	struct menulist	    *menulist;
	int		     firstvisiblesubstring;
	long int	     debug;
};

