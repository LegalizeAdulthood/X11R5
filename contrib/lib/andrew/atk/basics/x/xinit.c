/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xinit.c,v 1.3 1991/09/12 16:02:28 bobg Exp $ */
/* $ACIS:init.c 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xinit.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xinit.c,v 1.3 1991/09/12 16:02:28 bobg Exp $";
#endif /* lint */

#ifdef SGI_4D_ENV
#include <sys/stat.h>
#endif
#include <class.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <cmenu.h>

#define class_StaticEntriesOnly

#include <xfontd.ih>
#include <xgraphic.ih>
#include <xim.ih>
#include <xcursor.ih>
#include <xws.ih>

int xbasicsinit ()
{
    xfontdesc_StaticEntry;
    xgraphic_StaticEntry;
    xim_StaticEntry;
    xcursor_StaticEntry;
    xws_StaticEntry;
    return 1;
}
