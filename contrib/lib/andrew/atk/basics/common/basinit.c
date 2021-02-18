/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/basinit.c,v 2.6 1991/09/12 15:57:47 bobg Exp $ */
/* $ACIS:basinit.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/basinit.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/basinit.c,v 2.6 1991/09/12 15:57:47 bobg Exp $";
#endif /* lint */

#include <class.h>
#define class_StaticEntriesOnly

#include <im.ih>
#include <view.ih>
#include <fontdesc.ih>
#include <graphic.ih>
#include <menulist.ih>
#include <keyrec.ih>
#include <keystate.ih>
#include <keymap.ih>
#include <observe.ih>
#include <proctbl.ih>
#include <updlist.ih>
#include <filetype.ih>
#include <dataobj.ih>
#include <msghndlr.ih>
#include <message.ih>
#include <cursor.ih>
#include <event.ih>
#include <init.ih>
#include <environ.ih>
#include <bind.ih>
#include <atom.ih>
#include <atomlist.ih>
#include <namespc.ih>
#include <rm.ih>
#include <winsys.ih>

int basics_Initialize()
{
    im_StaticEntry;
    windowsystem_StaticEntry;
    view_StaticEntry;
    fontdesc_StaticEntry;
    graphic_StaticEntry;
    menulist_StaticEntry;
    keyrec_StaticEntry;
    keystate_StaticEntry;
    keymap_StaticEntry;
    observable_StaticEntry;
    proctable_StaticEntry;
    updatelist_StaticEntry;
    filetype_StaticEntry;
    dataobject_StaticEntry;
    msghandler_StaticEntry;
    message_StaticEntry;
    cursor_StaticEntry;
    event_StaticEntry;
    init_StaticEntry;
    environ_StaticEntry;
    bind_StaticEntry;
    atom_StaticEntry;
    atomlist_StaticEntry;
    namespace_StaticEntry;
    rm_StaticEntry;
    return 1;
}
