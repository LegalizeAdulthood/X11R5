/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/wim.ch,v 1.9 1991/09/12 19:24:21 bobg Exp $ */
/* $ACIS:im.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/wim.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidwmim_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/wim.ch,v 1.9 1991/09/12 19:24:21 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* 
 *  wim.ch
 * 	Class header file for the wm interface manager.
 *
 */

#define wmim_PROGRAMMERVERSION 1


class wmim[wim] : im  {
overrides:
    WhichWS() returns unsigned char *;
    PostMenus(struct menulist *menulist);
    PostCursor(struct rectangle *rec,struct cursor *cursor) ;

    ClearCursors(struct wmcursor *C);
    ClearCursorList();
    UpdateCursors();
    SetTitle(char *titleString);
    FromCutBuffer() returns FILE *;
    CloseToCutBuffer(FILE *writeFile);
    RotateCutBuffers(long count);
    AppendToCutBuffer(FILE *writeFile);
    /* These are window manager interface calls */
    SetWMFocus();
    ExposeWindow();
    HideWindow();
    VanishWindow();
    HandleRedraw();
    CreateWindow(char * host) returns boolean;

    RedrawWindow();

classprocedures:
    InitializeClass() returns boolean;
    InitializeObject(struct im *self) returns boolean;
    FinalizeObject(struct im *self);
    HandleFiles(long WaitTime, boolean beCheap) returns boolean;
    FlushAllWindows();

data:
    int cursorRegion; /* The region on which process and window cursors for this wmim are posted. */
    int menuRegion; /* Current wm region menus are installed on. WM dependent. */
    struct mlcacheNode *mlcache;
    struct cacheregion *freeRegions,*activeRegions;
    long buttonState;	/* button state as read in */
};

