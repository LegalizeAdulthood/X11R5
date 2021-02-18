/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/winit.c,v 1.5 1991/09/12 16:01:18 bobg Exp $ */
/* $ACIS:init.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/winit.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/winit.c,v 1.5 1991/09/12 16:01:18 bobg Exp $";
#endif /* lint */

#include <class.h>
#define class_StaticEntriesOnly

#include <wfontd.ih>
#include <wgraphic.ih>
#include <wim.ih>
#include <wcursor.ih>
#include <mrl.ih>
#include <wws.ih>

int wmbasicsinit ()
{
    wmfontdesc_StaticEntry;
    wmgraphic_StaticEntry;
    wmim_StaticEntry;
    wmcursor_StaticEntry;
    mrl_StaticEntry;
    wmws_StaticEntry;
    return 1;
}
