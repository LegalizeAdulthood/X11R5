/*LIBS: -lcmenu -lcwm -lX11 -lerrors -lutil
*/
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xws.c,v 1.9 1991/09/12 16:02:32 bobg Exp $ */
/* $ACIS:ws.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xws.c,v $ */

#ifndef lint
	char *xws_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xws.c,v 1.9 1991/09/12 16:02:32 bobg Exp $";
#endif /* lint */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <cmenu.h>

#include <xcursor.ih>
#include <xfontd.ih>
#include <xgraphic.ih>
#include <xim.ih>
#include <xws.eh>

	struct xcursor *
xws__CreateCursor(self)
	struct xws *self;
{
	return xcursor_New();
}

	struct xfontdesc *
xws__CreateFontdesc(self)
	struct xws *self;
{
	return xfontdesc_New();
}

	struct xgraphic *
xws__CreateGraphic(self)
	struct xws *self;
{
	return xgraphic_New();
}

	struct xim *
xws__CreateIM(self)
	struct xws *self;
{
	return xim_New();
}

	struct xoffscrwin *
xws__CreateOffScreenWindow(self,host,width,height)
	struct xws *self;
	char * host;
	long width, height;
{
/*
	return xoffscrwin_Create(host,width,height);
*/
	return NULL;
}

	boolean 
xws__HandleFiles(self, WaitTime, beCheap)
	struct xws *self;
	long WaitTime;
	boolean beCheap;
{
	return xim_HandleFiles(WaitTime, beCheap);
}

	void 
xws__FlushAllWindows(self)
	struct xws * self;
{
	xim_FlushAllWindows();
}

	boolean 
xws__InitializeClass(classID)
	struct classheader * classID;
{
	/* slimy way of getting all x related modules together */
	xim_StaticLoadOnlyThisClass();
	xfontdesc_StaticLoadOnlyThisClass();
	xgraphic_StaticLoadOnlyThisClass();
	xcursor_StaticLoadOnlyThisClass();
	/**/

	return TRUE;
}
