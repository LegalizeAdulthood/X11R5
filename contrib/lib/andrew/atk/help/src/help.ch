/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/help/src/RCS/help.ch,v 2.20 1991/09/12 19:41:04 bobg Exp $ */
/* $ACIS$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/help/src/RCS/help.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/help/src/RCS/help.ch,v 2.20 1991/09/12 19:41:04 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/*---------------------------------------------------------------------------*/
/*	MODULE: help.ch							     */
/*		Subclass of view, includes all major lookup and display      */
/*		routines.						     */
/*---------------------------------------------------------------------------*/

#define help_VERSION		2

#include "config.h"

/*
 * this is done since macromethods don't work for help_ calls on OTHER
 * help object from within the help.c code
 */
#define help_GetInfo(hv) ((hv)->info)

class help : view {
    
 overrides:
    PostMenus(struct menulist *menuList);
    PostKeyState(struct keystate *keyState);
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
    WantUpdate(struct view *req);
    Update();
    LinkTree(struct view *parent);
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    
 methods:
    
 classprocedures:
    HelpappGetHelpOn(char *name, long number, int addToHistory, char *errmsg) returns int;
    GetHelpOnTerminal(char *akey, int list, int print);
    InitializeObject(struct help *self) returns boolean;
    InitializeClass(struct help *self) returns boolean;
    FinalizeObject(struct help *self);
    SetIndex(char *indexName);
    SetAliasesFile(char *aliasName);
    AddSearchDir(char *dirName);
    GetInstance() returns struct view *;

 data:
    struct keystate *state;	/* our key and menu bindings */
    char expandedList;		/* is the program list expanded? */
    char showPanels;		/* are we to show the panels or not? */
    char showHistory;		/* are we showing the history now */
    char showList;
    char showOverview;

    struct cache *info;		/* menulist, keystate, view, dataobj... */

    struct panel *overviewPanel;/* overview panel and its label */
    struct lpair *overviewLpair;
    struct label *overviewLab;
    struct labelview *overviewLabV;
    struct scroll *overviewScroll; /* and its scrollbar */

    struct panel *listPanel;	/* program list panel and its label */
    struct lpair *listLpair;
    struct label *listLab;
    struct labelview *listLabV;
    struct scroll *listScroll;	/* ditto */

    struct panel *historyPanel;	/* history  panel and its label */
    struct lpair *historyLpair;
    struct label *historyLab;
    struct labelview *historyLabV;
    struct scroll *historyScroll;	/* ditto */

    struct lpair *mainLpair;	/* lpair connecting the textview and the panels */
    struct lpair *panelLpair1;	/* lpair connecting the panels */
    struct lpair *panelLpair2;	/* another lpair connecting the panels, not always used */

    struct panel *tmpanel;	/* for filtered programList */
    struct panel *oldpanel;	/* to placehold original programPanel */

    struct scroll *app;		/* This view's application layer */
};
