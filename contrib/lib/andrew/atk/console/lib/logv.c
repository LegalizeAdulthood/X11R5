/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/logv.c,v 2.7 1991/09/12 16:06:46 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/logv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/logv.c,v 2.7 1991/09/12 16:06:46 bobg Exp $";
#endif /* lint */

#include <class.h>
#include <view.ih>
#include <textv.ih>
#include <proctbl.ih>
#include <menulist.ih>
#include <logv.eh>

static struct menulist *logviewMenu;

static int logview_NoTextviewKey()
{
    return(0);
}

boolean logview__InitializeObject(classID, self)
    struct classheader *classID;
    struct logview *self;
{
    self->menu = menulist_DuplicateML(logviewMenu, self);
    return TRUE;
}

void logview__FinalizeObject(classID, self)
struct classheader *classID;
struct logview *self;
{
    if (self->menu != NULL)
	menulist_Destroy(self->menu);
}
	

void logview__LoseInputFocus(self)
    struct logview *self;
{
    logview_SetDotLength(self, 0);
    logview_PostMenus(self, NULL);
    self->header.textview.hasInputFocus = FALSE;
    logview_WantUpdate(self, self);
}

void logview__PostMenus(self, menulist)
    struct logview *self;
    struct menulist *menulist;
{
    if (logview_GetDotLength(self) > 0)  {
	menulist_SetMask(self->menu, 1);
    }
    else {
	menulist_SetMask(self->menu, 0);
    }
    super_PostMenus(self, self->menu);
}

void logview__FrameDot(self, pos)
    struct logview *self;
    long pos;
{
    struct linedesc *lastLine;
    long topPos;

    if (self->header.textview.nLines > 0)  {
	lastLine = &(self->header.textview.lines[self->header.textview.nLines - 1]);
	if (lastLine->nChars != 0 && lastLine->y + 2 * lastLine->height > logview_GetLogicalHeight(self))  {
	    topPos = logview_GetTopPosition(self);
	    topPos = logview_MoveBack(self, topPos, 0, textview_MoveByLines, 0, 0);
	    topPos = logview_MoveForward(self, topPos, 1, textview_MoveByLines, 0, 0);
	    if (self->header.textview.scroll == textview_ScrollBackward) self->header.textview.scroll = textview_MultipleScroll;
	    logview_SetTopPosition(self, topPos);
	}
    }
    super_FrameDot(self, pos);
}

boolean logview__InitializeClass(classID)
    struct classheader *classID;
{
    struct proctable_Entry *tempProc;
    struct classinfo *classInfo = &logview_classinfo;
    int (*textview_CopyCmd)();

    logviewMenu = menulist_New();

    class_Load("textview");

    if ((tempProc = proctable_Lookup("textview-copy-region")) != NULL)
        textview_CopyCmd = proctable_GetFunction(tempProc);
    else
        textview_CopyCmd = logview_NoTextviewKey;

    tempProc = proctable_DefineProc("logview-copy-region", textview_CopyCmd, classInfo, NULL, "Copy text from console log.");

    menulist_AddToML(logviewMenu, "Copy~12", tempProc, NULL, 1);
    return TRUE;
}
