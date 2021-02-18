/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/wws.c,v 1.8 1991/09/12 16:01:22 bobg Exp $ */
/* $ACIS:ws.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/wws.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/wws.c,v 1.8 1991/09/12 16:01:22 bobg Exp $";
#endif /* lint */

#include <class.h>
#include <andyenv.h>
#include <wmclient.h>
#include <wcursor.ih>
#include <wfontd.ih>
#include <wgraphic.ih>
#include <wim.ih>
#include <mrl.ih>
#include <wws.eh>

struct wmcursor *wmws__CreateCursor(self)
    struct wmws *self;
{
    return wmcursor_New();
}

struct wmfontdesc *wmws__CreateFontdesc(self)
    struct wmws *self;
{
    return wmfontdesc_New();
}

struct wmgraphic *wmws__CreateGraphic(self)
    struct wmws *self;
{
    return wmgraphic_New();
}

struct wmim *wmws__CreateIM(self)
    struct wmws *self;
{
    return wmim_New();
}

struct wmoffscrwin *wmws__CreateOffScreenWindow(self,host,width,height)
    struct wmws *self;
char * host;
long width, height;
{
/*
    return wmoffscrwin_Create(host,width,height);
*/
    return NULL;
}

boolean wmws__HandleFiles(self, WaitTime, beCheap)
    struct wmws *self;
    long WaitTime;
    boolean beCheap;
{
    return wmim_HandleFiles(WaitTime, beCheap);
}

void wmws__FlushAllWindows(self)
    struct wmws *self;
{
    wmim_FlushAllWindows();
}


boolean wmws__InitializeClass(classID)
struct classheader * classID;
{
    wmim_StaticLoadOnlyThisClass();
    wmfontdesc_StaticLoadOnlyThisClass();
    wmgraphic_StaticLoadOnlyThisClass();
    wmcursor_StaticLoadOnlyThisClass();
    mrl_StaticLoadOnlyThisClass();

    /* slimy way of getting all wm related modules together */
/*	%%% removed by pgc to get this running faster 
    wmim_StaticEntry;
    wmfontdesc_StaticEntry;
    wmgraphic_StaticEntry;
    wmcursor_StaticEntry;
    mrl_StaticEntry;
*/
    return TRUE;
}
