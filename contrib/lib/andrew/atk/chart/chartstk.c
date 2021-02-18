/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *chartstk_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/chart/RCS/chartstk.c,v 1.3 1991/09/12 16:05:12 bobg Exp $";


/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidchartstk = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/chart/RCS/chartstk.c,v 1.3 1991/09/12 16:05:12 bobg Exp $";
#endif

/*
    $Log: chartstk.c,v $
 * Revision 1.3  1991/09/12  16:05:12  bobg
 * Update copyright notice and rcsid
 *
 * Revision 1.2  1989/05/31  17:24:15  tom
 * y
 * Added classID as parameter to FinalizeObject.
 *
 * Revision 1.1  89/04/28  22:45:16  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Stack Chart View-object

MODULE	chartstk.c

VERSION	0.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Chart View-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  03/28/89	Created (TCP)
  05/31/89	Add classID parameter in FinalizeObject (TCP)

END-SPECIFICATION  ************************************************************/

#include <stdio.h>
#include <math.h>
#include "class.h"
#include "graphic.ih"
#include "observe.ih"
#include "view.ih"
#include "im.ih"
#include "rect.h"
#include "apt.h"
#include "aptv.ih"
#include "chartobj.ih"
#include "chart.ih"
#include "chartstk.eh"
#include <ctype.h>

#define  Screen			1
#define  Paper			2

#define  Bounds			(chartstk_BodyBounds(self))
#define  Left			(chartstk_BodyLeft(self))
#define  Top			(chartstk_BodyTop(self))
#define  Width			(chartstk_BodyWidth(self))
#define  Height			(chartstk_BodyHeight(self))
#define  Right			(chartstk_BodyRight(self))
#define  Bottom			(chartstk_BodyBottom(self))

#define  PixelsPerUnit		(chartstk_PixelsPerUnit( self ))
#define  PixelsPerInterval	(chartstk_PixelsPerInterval( self ))

#define  Items			(chartstk_Items(self))
#define  ItemBounds(shadow)	(chartstk_ItemBounds(self,(shadow)))
#define  ItemLeft(shadow)	(chartstk_ItemLeft(self,(shadow)))
#define  ItemTop(shadow)	(chartstk_ItemTop(self,(shadow)))
#define  ItemWidth(shadow)	(chartstk_ItemWidth(self,(shadow)))
#define  ItemHeight(shadow)	(chartstk_ItemHeight(self,(shadow)))
#define  ItemCenter(shadow)	(chartstk_ItemCenter(self,(shadow)))
#define  ItemMiddle(shadow)	(chartstk_ItemMiddle(self,(shadow)))
#define  NextItem(shadow)	(chartstk_NextItem(self,(shadow)))

boolean 
chartstk__InitializeObject( classID, self)
  register struct classheader	 *classID;
  register struct chartstk	 *self;
  {
  IN(chartstk_InitializeObject);
  chartstk_SetShrinkIcon( self, 'e', "icon12", "StackChart", "andysans10b" );
  chartstk_SetHelpFileName( self, "/usr/andy/help/ez.help"/*=== ===*/ );
  OUT(chartstk_InitializeObject);
  return  TRUE;
  }

void
chartstk__FinalizeObject( classID, self )
  register struct classheader	 *classID;
  register struct chartstk	 *self;
  {}

void
chartstk__SetDebug( self, state )
  register struct chartstk	 *self;
  register char			  state;
  {
  IN(chartstk_SetDebug);
  super_SetDebug( self, debug = state );
  OUT(chartstk_SetDebug);
  }

struct view *
chartstk__HitChart( self, action, x, y, clicks )
  register struct chartstk	     *self;
  register enum view_MouseAction      action;
  register long			      x, y, clicks;
  {

  IN(chartstk_HitChart);
/*===*/
  OUT(chartstk_HitChart);
  return  (struct view *) self;
  }

void
chartstk__DrawChart( self )
  register struct chartstk	     *self;
  {
  IN(chartstk_DrawChart);
/*===*/
  OUT(chartstk_DrawChart);
  }

void
chartstk__PrintChart( self )
  register struct chartstk	     *self;
  {
  IN(chartstk_PrintChart);
/*===*/
  OUT(chartstk_PrintChart);
  }
