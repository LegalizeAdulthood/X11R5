/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/winsys.c,v 2.7 1991/09/12 16:00:40 bobg Exp $ */
/* $ACIS:winsys.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/winsys.c,v $ */

#ifndef lint
	char *winsys_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/winsys.c,v 2.7 1991/09/12 16:00:40 bobg Exp $";
#endif /* lint */

#include <class.h>
#include <environ.ih>
#include <cursor.ih>
#include <fontdesc.ih>
#include <graphic.ih>
#include <im.ih>
#include <winsys.eh>


struct windowsysteminfo *windowSystem;

struct cursor *windowsystem__CreateCursor(self)
    struct windowsystem *self;
{
    return NULL;
}

struct fontdesc *windowsystem__CreateFontdesc(self)
    struct windowsystem *self;
{
    return NULL;
}

struct graphic *windowsystem__CreateGraphic(self)
    struct windowsystem *self;
{
    return NULL;
}

struct im *windowsystem__CreateIM(self)
    struct windowsystem *self;
{
    return NULL;
}

struct offscrwin * windowsystem__CreateOffScreenWindow(self,host,width,height)
    struct windowsystem *self;
    char * host;
    long width, height;
{
    return NULL;
}

void windowsystem__FlushAllWindows(self)
    struct windowsystem *self;
{
}

boolean windowsystem__HandleFiles(self, WaitTime, beCheap)
    struct windowsystem *self;
    long WaitTime;
    boolean beCheap;
{
    return FALSE;
}
