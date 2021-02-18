/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidsuite_h = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apt/suite/RCS/suite.h,v 1.7 1991/09/12 19:20:24 bobg Exp $";
#endif

/*
    $Log: suite.h,v $
 * Revision 1.7  1991/09/12  19:20:24  bobg
 * Update copyright notice
 *
 * Revision 1.6  1990/04/27  15:36:59  gk5g
 * Added item_Name macro.
 *
 * Revision 1.5  89/09/29  15:55:06  gk5g
 * Added macro item_RealCaptionFontSize.
 * 
 * Revision 1.4  89/09/08  17:00:31  gk5g
 * Added item_CaptionAlignment macro.
 * 
 * Revision 1.3  89/08/24  19:48:00  gk5g
 * Changes in support of V1.0 of the SuiteProgGuide.doc.
 * 
 * Revision 1.2  89/07/13  16:10:42  gk5g
 * Added macros for List item break points.
 * 
 * Revision 1.1  89/04/28  20:27:08  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Suite-object

MODULE	suite.h

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

END-SPECIFICATION  ************************************************************/

#define item_Normalized			    (1)
#define item_Highlighted		    (1<<1)
#define item_Active			    (1<<2)

#define	item_SetUpperLeft(item,X,Y) (item->bounds).left = (X);(item->bounds).top = (Y)
#define	item_SetDimensions(item,WIDTH,HEIGHT)\
    (item->bounds).width = (WIDTH);(item->bounds).height = (HEIGHT)

#define item_Caption		(item->caption?item->caption:item->name)
#define	item_Name		(item->name?item->name:item->caption)
#define item_CaptionPlacement \
 ((item->captionplacement)?(item->captionplacement):(item->suite->captionplacement))
#define item_CaptionAlignment \
 ((item->captionalignment)?(item->captionalignment):(item->suite->captionalignment))
#define item_CaptionFontName \
 ((item->captionfontname)?(item->captionfontname):(item->suite->captionfontname))
#define item_CaptionFont \
 ((item->captionfont)?(item->captionfont):(item->suite->captionfont))
#define item_CaptionFontSize \
 ((item->captionfontsize)?(item->captionfontsize):(item->suite->captionfontsize))
#define item_RealCaptionFontSize \
 ((item->captionfontsize)?(item->captionfontsize):(item->suite->realcaptionfontsize))
#define item_CaptionFontType \
 ((item->captionfonttype)?(item->captionfonttype):(item->suite->captionfonttype))
#define item_Title				(item->title)
#define item_TitlePlacement \
 ((item->titleplacement)?(item->titleplacement):(item->suite->titleplacement))
#define item_TitleCaptionAlignment \
 ((item->titlecaptionalignment)?(item->titlecaptionalignment):\
 (item->suite->itemtitlecaptionalignment))
#define item_TitleFontName \
 ((item->titlefontname)?(item->titlefontname):(item->suite->titlefontname))
#define item_TitleFont \
 ((item->titlefont)?(item->titlefont):(item->suite->titlefont)
#define item_TitleFontSize \
 ((item->titlefontsize)?(item->titlefontsize):(item->suite->titlefontsize))
#define item_TitleFontType \
 ((item->titlefonttype)?(item->titlefonttype):(item->suite->titlefonttype))
#define item_BorderStyle \
 ((item->borderstyle)?(item->borderstyle):(item->suite->itemborderstyle))
#define item_BorderSize \
 ((item->bordersize)?(item->bordersize):(item->suite->itembordersize))
#define item_DataObjectName \
 ((item->dataobjectname)?(item->dataobjectname):(item->suite->itemdataobjectname))
#define item_DataObject				(item->dataobject)
#define item_DataObjectHandler \
 ((item->dataobjecthandler)?(item->dataobjecthandler):(item->suite->itemdataobjecthandler))
#define item_ViewObjectName \
 ((item->viewobjectname)?(item->viewobjectname):(item->suite->itemviewobjectname))
#define item_ViewObject				(item->viewobject)
#define item_ViewObjectHandler \
 ((item->viewobjecthandler)?(item->viewobjecthandler):(item->suite->itemviewobjecthandler))
#define item_HitHandler \
 ((item->hithandler)?(item->hithandler):(item->suite->hithandler))
#define item_Data \
 ((item->data)?(item->data):(item->suite->data))
#define item_Mode \
 ((item->mode)?(item->mode):(item->suite->mode))
#define item_AccessType \
 ((item->accesstype)?(item->accesstype):(item->suite->accesstype))
#define item_PassiveStyle \
 ((item->passivestyle)?(item->passivestyle):(item->suite->itempassivestyle))
#define item_HighlightStyle \
 ((item->highlightstyle)?(item->highlightstyle):(item->suite->itemhighlightstyle))
#define item_Debug				(item->debug)
#define item_Cursor \
    (item->cursor?item->cursor:item->suite->itemcursor)
#define item_CursorByte \
    (item->cursorbyte?item->cursorbyte:item->suite->itemcursorbyte)
#define item_CursorType \
    (item->cursortype?item->cursortype:item->suite->itemcursortype)
#define item_CursorFontName \
    (item->cursorfontname?item->cursorfontname:(item->suite->itemcursorfontname?item->suite->itemcursorfontname:item->suite->cursorfontname))
#define item_CursorFont \
    (item->cursorfont?item->cursorfont:item->suite->itemcursorfont)

#define NOFORCEDMODE				    (-1)

#define	Breaks(item)				    (item->breaks)
#define	BreakCount(item)			    (vector_Count(item->breaks))
#define	BreakPos(item,i)			    (vector_Item(item->breaks,i))
