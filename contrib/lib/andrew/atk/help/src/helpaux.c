/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *helpaux_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/help/src/RCS/helpaux.c,v 1.9 1991/09/12 16:22:51 bobg Exp $";

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/help/src/RCS/helpaux.c,v 1.9 1991/09/12 16:22:51 bobg Exp $ */
/* $ACIS$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/help/src/RCS/helpaux.c,v $ */

#include <class.h>

#define label gezornenplatz
/* sys/types.h in AIX PS2 defines "struct label", causing a type name clash.
   Avoid this by temporarily redefining "label" to be something else in the preprocessor. */
#include <andrewos.h> /* sys/types.h sys/file.h */
#undef label

#include <complete.ih>
#include <cursor.ih>
#include <dataobj.ih>
#include <environ.ih>
#include <filetype.ih>
#include <im.ih>
#include <frame.ih>
#include <keymap.ih>
#include <keystate.ih>
#include <label.ih>
#include <labelv.ih>
#include <lpair.ih>
#include <menulist.ih>
#include <message.ih>
#include <print.ih>
#include <panel.ih>
#include <proctbl.ih>
#include <rect.h>
#include <scroll.ih>
#include <text.ih>
#include <textv.ih>
#include <view.ih>

#include <errno.h>
#include <ctype.h>
#include <sys/dir.h>
#include <sys/errno.h>
#include <sys/param.h>

#include <config.h>
#include <helpsys.h>
#include <help.h>
#include <helpdb.ih>
#define AUXMODULE 1
#include <help.eh>

/* statics representing information cache */
extern char *tutorialDirs[MAX_TUTORIAL_DIRS];
extern char changesDir[MAXPATHLEN];

/* a list of instances of help */
extern struct self_help *ego;

extern struct keymap *Help_Map;
extern struct menulist *Help_Menus;

extern struct cursor *waitCursor; /* the watch cursor */

extern char **panelList; /* used for enumerating across the help index */

/* hooks to textview and frame procs */
extern void (*textSearch)();
extern void (*textRevSearch)();
extern void (*textSearchAgain)();
extern void (*textCopyRegion)();
extern void (*textPageDown)();
extern void (*textPageUp)();
extern void (*frameSetPrinter)();

extern void ToggleProgramListSize();
extern void HistoryHelp();
extern void OverviewHelp();
extern struct view *SetupLpairs();
extern void SetupMenus();
extern void FreePanelListData();
extern int GetHelpOn();
extern char *LowerCase();
extern long SetupPanel();
extern void NewHelp();
extern void AddHistoryItem();

#ifdef DEBUGGING
/*
 * debugging statements can be included by compiling add modules with
 * -DDEBUGGING.  Debugging output is toggled by existance/nonexistance
 * of the environment variable HELPDEBUG.
 */
extern int HELPDEBUG;
#undef DEBUG
#define DEBUG(arg) if (HELPDEBUG != 0) { printf arg; fflush(stdout); }
#else
#undef DEBUG
#define DEBUG(arg)
#endif /* DEBUGGING */

/*---------------------------------------------------------------------------*/
/*			     HELP CLASSPROCEDURES			     */
/*---------------------------------------------------------------------------*/

/*
 * help__ method for adding a directory to the searchpath
 */
void help__AddSearchDir(classID,  dirName)
struct classheader *classID;
char *dirName;
{
    if (access(dirName, 4) < 0) {
	fprintf(stderr, "help: cannot open search directory '%s'\n", dirName);
	return;
    }
    DEBUG(("adding dir: '%s'\n",dirName));
    helpdb_AddSearchDir(dirName);
}


/*
 * initializes a new help object.  Sets up the view hiearchy, sets up panels, etc
 */
boolean help__InitializeObject(classID,self)
struct classheader *classID;
register struct help *self;
{
    char pathName[MAXPATHLEN], *tmp = NULL, *colon = NULL;
    struct proctable_Entry *pe;
    struct self_help *id;
    struct view *v;
    int	 i = 0;


    DEBUG(("IN init obj\n"));
    waitCursor = cursor_Create(0);
    cursor_SetStandard(waitCursor, Cursor_Wait);
    im_SetProcessCursor(waitCursor);

    tmp = environ_GetConfiguration(SETUP_CHANGEDIR);
    if (tmp == NULL) {
	tmp = environ_LocalDir(DEFAULT_CHANGEDIR);
    }
    strcpy(changesDir, tmp);

    for(i = 0; i < MAX_TUTORIAL_DIRS; i++) 
	tutorialDirs[i] = NULL;
    i = 0;
    if(tmp = environ_GetConfiguration(SETUP_TUTORIALDIR)) {
	if((colon = rindex(tmp,':')) == NULL) {
	    tutorialDirs[i] = (char*) malloc(strlen(tmp) + 1);
	    strcpy(tutorialDirs[i],tmp);
	}
	else {
	    while(colon && (colon != '\0')) {
		*colon = '\0';
		tutorialDirs[i] = (char*) malloc(strlen(tmp) + 1);
		strcpy(tutorialDirs[i],tmp);
		*colon = ':';
		tmp = colon + 1;
		colon = rindex(tmp,':');
		i++;
	    }
	    tutorialDirs[i] = (char*) malloc(strlen(tmp) + 1);
	    strcpy(tutorialDirs[i],tmp);
	}
    }
    else {
	tmp = environ_AndrewDir(DEFAULT_TUTORIALDIR);
	tutorialDirs[i] = (char*) malloc(strlen(tmp) + 1);
	strcpy(tutorialDirs[i],tmp);
    }

    im_SetProcessCursor((struct cursor *) NULL);

    /* general variables */
    self->expandedList = 0;
    self->showPanels = environ_GetProfileSwitch("ShowPanels", TRUE);
    self->showOverview = environ_GetProfileSwitch("ShowOverviews", TRUE);
    self->showList = environ_GetProfileSwitch("ShowPrograms", TRUE);
    self->showHistory = environ_GetProfileSwitch("ShowHistory", FALSE);

    self->state = keystate_Create(self, Help_Map);
    self->info = (struct cache *)malloc(sizeof(struct cache));
    if (!self->info || !self->state)
	return FALSE;
    DEBUG(("info state "));
    self->info->scroll = NULL;
    self->info->menus = menulist_DuplicateML(Help_Menus, self);
    if (!self->info->menus)
	return FALSE;
    DEBUG(("menus "));
    self->info->flags = 0;
    self->info->flags = MENU_ToggleSizeExpand; /* start out shrunk initially */
    self->info->flags |= (self->showPanels) ? MENU_TogglePanelHide : MENU_TogglePanelShow;
    self->info->flags |= (self->showOverview) ? MENU_ToggleOverHide : MENU_ToggleOverShow;
    self->info->flags |= (self->showList) ? MENU_ToggleListHide : MENU_ToggleListShow;
    self->info->flags |= (self->showHistory) ? MENU_ToggleHistHide : MENU_ToggleHistShow;
    self->info->flags |= MENU_ToggleFilterShow;
    self->info->all = self->info->cur = (struct helpFile *)NULL;
    self->info->histat = help_HIST_NOADD;
    self->info->histent[0] = '\0';
    self->info->lastHist = NULL;

    self->mainLpair = NULL;

    /* the help text object */
    self->info->data = (struct dataobject *)text_New();
    self->info->view = (struct view *)textview_New();
    if (!self->info->data || !self->info->view)
	return FALSE;
    DEBUG(("data view "));

    view_SetDataObject(self->info->view, self->info->data);
    
    self->info->scroll = scroll_Create(self->info->view, scroll_LEFT);
    textview_SetEmbeddedBorder((struct textview *) self->info->view, 20, 5);
    if (!self->info->scroll)
	return FALSE;
    DEBUG(("scroll "));
    
    /* panels and their scrollbars */
    self->oldpanel = (struct panel *)NULL;
    self->tmpanel = panel_New();
    self->overviewPanel = panel_New();
    self->listPanel = panel_New();
    self->historyPanel = panel_New();
    if (!self->tmpanel || !self->overviewPanel || !self->listPanel || !self->historyPanel)
	return FALSE;
    DEBUG(("panels "));
    self->info->hist = self->historyPanel;

    self->overviewScroll = (struct scroll *) panel_GetApplicationLayer(self->overviewPanel);
    self->listScroll = (struct scroll *) panel_GetApplicationLayer(self->listPanel);
    self->historyScroll = (struct scroll *) panel_GetApplicationLayer(self->historyPanel);
    if (!self->overviewScroll || !self->listScroll)
	return FALSE;


    /* labels */
    self->overviewLab = label_New();
    self->overviewLabV = labelview_New();
    self->listLab = label_New();
    self->listLabV = labelview_New();
    self->historyLab = label_New();
    self->historyLabV = labelview_New();
    if (!self->overviewLab || !self->overviewLabV || !self->historyLab || !self->historyLabV
	|| !self->listLab || !self->listLabV)
	return FALSE;

    labelview_SetDataObject(self->overviewLabV, self->overviewLab);
    label_SetFlags(self->overviewLab, label_CENTERED | label_BOXED);
    label_SetText(self->overviewLab, "Overviews");

    labelview_SetDataObject(self->listLabV, self->listLab);
    label_SetFlags(self->listLab, label_CENTERED | label_BOXED);
    label_SetText(self->listLab, "Programs");
    
    labelview_SetDataObject(self->historyLabV, self->historyLab);
    label_SetFlags(self->historyLab, label_CENTERED | label_BOXED);
    label_SetText(self->historyLab, "Help History");


    /* lpairs */
    self->overviewLpair = lpair_Create(self->overviewLabV, self->overviewScroll, LABPCT);
    self->listLpair = lpair_Create(self->listLabV, self->listScroll, LABPCT);
    self->historyLpair = lpair_Create(self->historyLabV, self->historyScroll, LABPCT);
    self->panelLpair1 = lpair_Create(NULL, NULL, -PANELPCT);
    self->panelLpair2 = lpair_Create(NULL, NULL, -33);
    if (!self->overviewLpair || !self->listLpair || !self->historyLpair
	|| !self->panelLpair1 || !self->panelLpair2)
	return FALSE;
    
    lpair_VTFixed(self->overviewLpair, self->overviewLabV, self->overviewScroll, LABPCT, 0);
    lpair_VTFixed(self->listLpair, self->listLabV, self->listScroll, LABPCT, 0);
    lpair_VTFixed(self->historyLpair, self->historyLabV, self->historyScroll, LABPCT, 0);

    /* if the user has all panels off, use the overview panel by default */
    v = SetupLpairs(self);
    self->mainLpair = lpair_Create(self->info->scroll,
				   v ? v : (struct view *)self->overviewScroll, -MAINPCT);
    if (!self->mainLpair)
	return FALSE;
    DEBUG(("main lpair "));
    
    lpair_SetLPState(self->mainLpair, lpair_NOCHANGE, lpair_VERTICAL, 1);

    DEBUG(("handlers "));

    /* default panel handlers */
    panel_SetHandler(self->overviewPanel, OverviewHelp, (char *) self);
    panel_SetHandler(self->listPanel, OverviewHelp, (char *) self);
    panel_SetHandler(self->tmpanel, OverviewHelp, (char *) self);
    panel_SetHandler(self->historyPanel, HistoryHelp, (char *) self);

    if (self->showPanels)
	lpair_LinkTree(self->mainLpair, self);
    else
	scroll_LinkTree(self->info->scroll, self);

    /* add minimum set of entries to listPanel */
    if((tmp = environ_GetConfiguration(SETUP_PANELSDIR)) == NULL) {
	tmp = environ_GetConfiguration(SETUP_LIBDIR);
	if (tmp == NULL)
	    tmp = environ_AndrewDir(DEFAULT_LIBDIR);
    }
    sprintf(pathName, "%s%s", tmp, PROGRAMFILE);
    
    if (0 == SetupPanel(TRUE, pathName, self->listPanel, NULL)) {
	/* got nothing from lib/help.programs;  use extensions */
	tmp = environ_GetConfiguration(SETUP_HELPDIR);
	if (tmp == NULL)
		tmp = environ_AndrewDir(DEFAULT_HELPDIR);
	strcpy(pathName, tmp);
	help_AddSearchDir(pathName);	/* ??? XXX */
   
	SetupPanel(FALSE, pathName, self->listPanel, program_ext_array);
    }

    DEBUG(("expanding "));
    if (environ_GetProfileSwitch("ExpandedList", FALSE))
	ToggleProgramListSize(self, help_EXPAND);

    /* get overviews entries */
    if((tmp = environ_GetConfiguration(SETUP_PANELSDIR)) == NULL) {
	tmp = environ_GetConfiguration(SETUP_LIBDIR);
	if (tmp == NULL)
	    tmp = environ_AndrewDir(DEFAULT_LIBDIR);
    }
    sprintf(pathName, "%s%s", tmp, OVERVIEWFILE);
    if (0 == SetupPanel(TRUE, pathName, self->overviewPanel, NULL)) {
	tmp = environ_GetConfiguration(SETUP_HELPDIR);
	if (tmp == NULL)
		tmp = environ_AndrewDir(DEFAULT_HELPDIR);
	strcpy(pathName, tmp);
	help_AddSearchDir(pathName);	/* ??? XXX */

	SetupPanel(FALSE, pathName, self->overviewPanel, overview_ext_array);
    }

    /* post our menus */
    SetupMenus(self->info);

    /* runtime-bound procedures */
    pe = proctable_Lookup("textview-search");
    if (pe) textSearch = (void (*)()) proctable_GetFunction(pe);
    else return FALSE;
    pe = proctable_Lookup("textview-reverse-search");
    if (pe) textRevSearch = (void (*)()) proctable_GetFunction(pe);
    else return FALSE;
    pe = proctable_Lookup("textview-search-again");
    if (pe) textSearchAgain = (void (*)()) proctable_GetFunction(pe);
    else return FALSE;
    pe = proctable_Lookup("textview-copy-region");
    if (pe) textCopyRegion = (void (*)()) proctable_GetFunction(pe);
    else return FALSE;
    pe = proctable_Lookup("textview-next-screen");
    if (pe) textPageDown = (void (*)()) proctable_GetFunction(pe);
    else return FALSE;
    pe = proctable_Lookup("textview-prev-screen");
    if (pe) textPageUp = (void (*)()) proctable_GetFunction(pe);
    else return FALSE;
    pe = proctable_Lookup("frame-set-printer");
    if (pe) frameSetPrinter = (void (*)()) proctable_GetFunction(pe);
    else return FALSE;

    /* now add ourself to the head of the instance list */
    DEBUG(("ego: %d\n",(int)ego));
    id = (struct self_help *)malloc(sizeof(struct self_help));
    if (!id)
	return FALSE;
    id->this = self;
    id->next = ego;
    ego = id;
    DEBUG(("ego: %d\n",(int)ego));

    self->app = NULL;

    DEBUG(("OUT init obj\n"));
    return TRUE;
}


/*
 * destroys the resources of the help object
 */
void help__FinalizeObject(classID, self)
struct classhead *classID;
register struct help *self;
{
    DEBUG(("IN finalize\n"));
    self->mainLpair->obj[0] = NULL;
    self->mainLpair->obj[1] = NULL;
    lpair_Destroy(self->mainLpair);

    view_Destroy(self->info->view);
    dataobject_Destroy(self->info->data);

    self->panelLpair1->obj[0] = NULL;
    self->panelLpair1->obj[1] = NULL;
    lpair_Destroy(self->panelLpair1);

    if(self->panelLpair2) {
	self->panelLpair2->obj[0] = NULL;
	self->panelLpair2->obj[1] = NULL;
	lpair_Destroy(self->panelLpair2);
    }

    if(self->overviewLpair) {
	self->overviewLpair->obj[0] = NULL;
	self->overviewLpair->obj[1] = NULL;
	lpair_Destroy(self->overviewLpair);
	scroll_Destroy(self->overviewScroll);
	panel_Destroy(self->overviewPanel);
	labelview_Destroy(self->overviewLabV);
	label_Destroy(self->overviewLab);
    }

    if(self->listLpair) {
	self->listLpair->obj[0] = NULL;
	self->listLpair->obj[1] = NULL;
	lpair_Destroy(self->listLpair);
	scroll_Destroy(self->listScroll);
	panel_Destroy(self->listPanel);
	labelview_Destroy(self->listLabV);
	label_Destroy(self->listLab);
    }

    if(self->historyLpair) {
	self->historyLpair->obj[0] = NULL;
	self->historyLpair->obj[1] = NULL;
	lpair_Destroy(self->historyLpair);
	scroll_Destroy(self->historyScroll);
	panel_Destroy(self->historyPanel);
	labelview_Destroy(self->historyLabV);
	label_Destroy(self->historyLab);
    }

    /* the presence/absence of self->info indicates whether we are
       in the process of finalizing, so that in update we know we
       can't post menus */
    free(self->info);
    self->info = NULL;
    if(panelList) FreePanelListData();
    DEBUG(("info\n"));
    DEBUG(("OUT finalize\n"));
}

     
/*
 * help__ method interface for using an alias file
 */
void help__SetAliasesFile(classID,alias)
struct classheader *classID;
register char *alias;
{
    helpdb_ReadAliasesFile(alias);
}

/*
 * help__ method for adding a help index directory
 */
void help__SetIndex(classID,aindex)
struct classheader *classID;
register char *aindex;
{
    DEBUG(("IN SetIndex: %s\n",aindex));
    helpdb_SetIndex(aindex);
}

/*
 * outside interface help__ method for GetHelpOn.  Used by helpapp
 *
 * Returns:
 *	-1: if a server was down while trying to get help on this topic
 *	 0: if no help found for this topic
 *	 1: if successful
 */
int help__HelpappGetHelpOn(classID, aname, isnew, ahistory, errmsg)
struct classheader *classID;
char *aname;	/* what topic */
long isnew;	/* is this a new topic? */
int ahistory;	/* show in history log? 1-show aname 2-show tail of filename */
char *errmsg;	/* error to print if failure. "Error" if this is NULL */
{
    if (aname[0] == '\0') {
	/* so -e flag with no file shown works */
	return 1;
    }
    /* use the last instance added to the list */
    DEBUG(("using %d view %d asking %d\n", (int)ego->this,(int)ego->this->info->view,
	   message_Asking(ego->this->info->view)));
    if (message_Asking(ego->this->info->view)) {
	DEBUG(("retracting\n"));
	message_CancelQuestion(ego->this->info->view);
    }
    return GetHelpOn(ego->this, aname, isnew, ahistory, errmsg);
}


/*
 * return the first view in the list of instances, so helpapp can expose
 * a hidden window.
 */
struct view *help__GetInstance(classID)
struct classheader *classID;
{
    return (struct view *)(ego->this);
}


/*---------------------------------------------------------------------------*/
/*				HELP METHODS				     */
/*---------------------------------------------------------------------------*/

/*
 * self explanatory
 */
void help__PostMenus(self, menuList)
register struct help *self;
struct menulist *menuList;
{
    DEBUG(("post menus\n"));
    if (self->info != NULL)
	super_PostMenus(self, self->info->menus);
}

/*
 * override parents' keybindings
 */
void help__PostKeyState(self, keyState)
register struct help *self;
struct keystate *keyState;
{
    DEBUG(("post keys\n"));
    keystate_AddBefore(self->state, keyState);
    super_PostKeyState(self, self->state);
}

/*
 * mouse action handler
 */
struct view *help__Hit(self, action, x, y, numberOfClicks)
struct help *self;
enum view_MouseAction action;
long x, y;
long numberOfClicks;
{
    if (self->showPanels)
	return lpair_Hit(self->mainLpair, action, x, y, numberOfClicks);
    else
	return scroll_Hit(self->info->scroll, action, x, y, numberOfClicks);
}

/*
 * set up the view tree based on whether panels or scrollbar is the top level view
 */
void help__LinkTree(self, parent)
struct help *self;
struct view *parent;
{
    DEBUG(("IN link tree\n"));
    DEBUG(("\tsuper..."));
    super_LinkTree(self, parent);

    if (self->showPanels) {
	if (self->mainLpair)
	    lpair_LinkTree(self->mainLpair, self);
    } else {
	if (self->info!=NULL && self->info->scroll)
	    scroll_LinkTree(self->info->scroll, self);
    }
    DEBUG(("OUT link tree\n"));
}

/*
 * refreshing!
 */
void help__FullUpdate(self, type, left, top, width, right)
register struct help *self;
enum view_UpdateType type;
long left, top, width, right;
{
    struct rectangle childRect;

    DEBUG(("IN fullupdate\n"));

    help_GetLogicalBounds(self, &childRect);

    if (self->showPanels) {	/* need the main lpair as the root */
	lpair_InsertView(self->mainLpair, self, &childRect);
	lpair_FullUpdate(self->mainLpair, type, left, top, width, right);
    } else {			/* we just have the textview to show */
	scroll_InsertView(self->info->scroll, self, &childRect);
	scroll_FullUpdate(self->info->scroll, type, left, top, width, right);
    }
    /* NEVER EVER mess with the input focus in a fullupdate routine (or at least if you do so, check to make sure you have the input focus to start with...) */
    /* super_WantInputFocus(self, self->info->view); */
    DEBUG(("OUT fullupdate\n"));
}

/*
 * erase and refresh the screen
 */
void help__Update(self)
register struct help *self;
{
    DEBUG(("IN update\n"));
    /* clear out the region, then do a full redraw */
    help_EraseVisualRect(self);
    help_FullUpdate(self, view_FullRedraw, help_GetLogicalTop(self), help_GetLogicalLeft(self),
        help_GetLogicalWidth(self), help_GetLogicalHeight(self));
    DEBUG(("OUT update\n"));
}

/*
 * update event handler
 */
void help__WantUpdate(self, requestor)
register struct help *self;
register struct view *requestor;
{
    /* if the scrollbar took the hit, check if the user has selected something,
       and turn on some menus items if so */
    if (self->info && (requestor == (struct view *)self->info->scroll)) {
	/* if the menumask changes, post menus */
	if (menulist_SetMask(self->info->menus,
			 (textview_GetDotLength((struct textview *)self->info->view) != 0) ?
			 menulist_GetMask(self->info->menus) | MENU_SwitchCopy :
			 menulist_GetMask(self->info->menus) & ~MENU_SwitchCopy))
	    help_PostMenus(self, self->info->menus);
    }
    super_WantUpdate(self, requestor);
}


/*
 * classproc to handle getting help using a terminal-based interface
 */
void help__GetHelpOnTerminal(classID,akey,list,print)
struct classheader *classID;
register char *akey;		/* topic string */
register int list;		/* do help on topic, or just list files? */
register int print;		/* prompt for printing each helpfile? */
{
    FILE *tfile;
    char *alias, *pager, *index, *tmp;
    char pathName[MAXPATHLEN], cmdLine[MAXPATHLEN];
    char hbuffer[HNSIZE], tbuffer[HNSIZE];
    long digitMode, digitValue, firstChar;
    struct helpFile *tmpf;
    struct cache c;
    int tc;
    int first = 0;

    DEBUG(("key: %s\n",akey));
    if (!helpdb_CheckIndex(NULL)) { /* if we haven't SetIndex */
	index = environ_GetConfiguration(SETUP_INDEXDIR);
	if (index == NULL) 
	    index = environ_AndrewDir(DEFAULT_INDEXDIR);
	help_SetIndex(index);
    }

    if ((pager = environ_Get(PAGERVAR)) == (char *) NULL)
	pager = DEFAULTPAGER;

    if (!helpdb_CheckIndex(NULL)) {
        fprintf(stderr, err_index, pathName);
        return;
    }

    /* map helpKey */
    strncpy(tbuffer, LowerCase(akey), HNSIZE);
    tbuffer[HNSIZE-1] = 0;

    /* now see if it has an illegal, command running, alias */
    alias = (char *)helpdb_MapAlias(tbuffer);
    DEBUG(("map alias on: %s\n",tbuffer));
    if (alias) {
	DEBUG(("alias: %s\n", alias));
        if (alias[0] == '#') {
            fprintf(stderr,err_terminal);
	    putchar('\n');
            exit(1);
	    /*NOTREACHED*/
        }
    }

    alias = tbuffer;		/* src */
    tmp = hbuffer;		/* dst */
    digitMode = 0;
    digitValue = 0;
    firstChar = 1;
    /* copy alias (points to tbuffer) to hbuffer, suppressing spaces and
       computing digitValue */
    while((tc = *alias++) != NULL) {
        if (tc == ' ' || tc == '\n' || tc == '\t')
	    continue;
        if (tc == '(' || (!firstChar && tc == '.'))
	    digitMode = 1;
        else if (digitMode) {
            if (tc >= '0' && tc <= '9') {
                digitValue *= 10;
                digitValue += tc - '0';
            }
        } else {
            *tmp++ = tc;
	}
        firstChar = 1;
    }
    *tmp++ = 0;

    DEBUG(("tbuf: %s hbuf: %s\n",tbuffer,hbuffer));

    c.all = (struct helpFile *)NULL;

    if (helpdb_SetupHelp(&c, hbuffer, FALSE) == 0) {
        fprintf(stderr, err_sorry, akey);
	putchar('\n');
        exit(1);
	/*NOTREACHED*/
    }

    /* if non-0 return, c->all was set up for us */

    DEBUG(("setup done\n"));
    
    if (list) {
	printf("Using search directories: ");
	helpdb_PrintSearchDirs();
	printf("\n");
        printf("For topic %s the files are:\n",akey);
	for (tmpf = c.all; tmpf; tmpf = tmpf->next) {
	    printf("\t%s\n",tmpf->fileName);
	}
	exit(0);
	/*NOTREACHED*/
    }

    /* o.w. do real help on the topic */
    first = TRUE;
    for(tmpf = c.all; tmpf; tmpf = tmpf->next) {
	int in = 0;
	char *fn;
	int skipFlag = FALSE;

	tfile = fopen(tmpf->fileName, "r");
	if (!tfile) {
	    fprintf(stderr, err_file, tmpf->fileName);
	    continue;
	}

	if (!first) {
	    skipFlag = FALSE;
	    fn = rindex(tmpf->fileName, '/');
	    printf(msg_term_prompt, fn ? fn+1 : tmpf->fileName);
	    in = getchar();
	    DEBUG(("'%c'\n", in));
	    if (in < 0) exit(1);
	    else if (in == 'q' || in == 'Q') exit(0);
	    else if (in == 'n' || in == 'N') skipFlag = TRUE;
	    if (in != '\n')
		while ((in = getchar()) > 0 && in != '\n')
		    ;
	}

	first = FALSE;
	
	if (!skipFlag) {
	    /* show the file */
	    tc = fgetc(tfile);
	    ungetc(tc,tfile);
	    fclose(tfile);
	    if ((tc == '.') || (tc == '#')) { /* troff file */
		sprintf(cmdLine, ROFFCMD, tmpf->fileName, pager);
	    } else {
		sprintf(cmdLine, ATKROFFCMD, tmpf->fileName, pager);
	    }
	    DEBUG(("cmd: %s\n",cmdLine));
	    system(cmdLine);
	}

	if (print) {
	    /* see if they want to print it */
	    if (in < 0) exit(1);
	    fn = rindex(tmpf->fileName, '/');
	    printf(msg_print_prompt, fn ? fn+1 : tmpf->fileName);
 	    in = getchar();
	    DEBUG(("'%c'\n", in));
	    if (in < 0) exit(1);
	    else if (in != 'n' && in != 'N') { /* print it */
		if ((tc == '.') || (tc == '#')) { /* troff file */
		    sprintf(cmdLine, ROFFPRINTCMD, tmpf->fileName);
		} else {
		    sprintf(cmdLine, ATKPRINTCMD, tmpf->fileName);
		}
		DEBUG(("cmd: %s\n",cmdLine));
		system(cmdLine);
	    }
	    if (in != '\n')
		while ((in = getchar()) > 0 && in != '\n')
		    ;
	}
    }
}

struct frame *
getframe(vw)
struct view *vw;
{
    while(vw->parent) {
	vw = vw->parent;
	if(class_IsTypeByName(class_GetTypeName(vw),"frame"))
	    return((struct frame*) vw);
    }
    return(NULL);
}

void 
destroyWindow(self)
struct help *self;
{
    struct frame *fr;
    struct proctable_Entry *pr;
    int (*proc)();

    if((pr = proctable_Lookup("frame-delete-window")) != NULL && 
	proctable_Defined(pr) &&
	(fr = getframe((struct view*)self)) != NULL) {
	proc = proctable_GetFunction(pr) ;
	help_UnlinkTree(self);
	help_DeleteApplicationLayer(self,self->app);
	(*proc)(fr,0);
    }
}

/*
 * Allows help to just delete one window
 */
void ExitProc(self)
register struct help *self;
{
    struct self_help *t, *p;

    DEBUG(("IN exit\n"));

    DEBUG(("ego: %d self: %d\n",(int)ego, (int)self));
    for (t=ego;t;t=t->next) {
	DEBUG(("t: %d this: %d next: %d\n", (int)t, (int)t->this, (int)t->next));
    }

    /* delete self from the instance list */
    p = NULL;
    t = ego;
    while (t) {
	if (t->this == self) {
	    if (p == NULL) {	/* deleting head */
		ego = t->next;
		free(t);
		t = ego;
	    } else {
		p->next = t->next;
		free(t);
		t = p->next;
	    }
	} else {
	    p = t;
	    t = t->next;
	}
    }
    DEBUG(("ego: %d self: %d\n",(int)ego, (int)self));
    for (t=ego;t;t=t->next) {
	DEBUG(("t: %d this: %d next: %d\n", (int)t, (int)t->this, (int)t->next));
    }

    /* take care of the cases with 0 and 1 instances left in the list */
    if (ego) {			/* something in the list */
	if (ego->next == (struct self_help *)NULL) { /* only one left */
	    /* remove "Delete this window" menu item */
	    DEBUG(("one left, Delete delete\n"));
	    ego->this->info->flags &= ~MENU_SwitchDeleteMenu;
	    SetupMenus(ego->this->info);
	}
    } else {			/* nobody left, all gone, we be outta here */
	im_KeyboardExit();
    }

    destroyWindow(self);

    DEBUG(("OUT exit\n"));
}

/*
 * print a help file
 */
void Print(self)
register struct help *self;
{
    message_DisplayString(self, 0, msg_print_queue);
    im_ForceUpdate();
    im_SetProcessCursor(waitCursor);
    print_ProcessView((struct textview *)self->info->view, 1, 0, "", "");
    message_DisplayString(self, 0, msg_queue_done);
    im_SetProcessCursor((struct cursor *) NULL);
}

/*
 * get help on a prompted-for topic or a selected word, bringing up a
 * new window if necessary
 */
void NewHelp(self, type)
register struct help *self;
long type;		/* help_ON 			if Help On... */
			/* help_ON & help_NEW_WIN	if New Help On... */
			/* help_SEL			if Help On Selected */
			/* help_SEL & help_NEW_WIN	if New Help On Selected */
{
    register int i, pos, len, code;
    char tc;
    char helpName[HNSIZE];
    char buf[HNSIZE + HELP_MAX_ERR_LENGTH];
    struct help *hv;

    if (type & help_SEL) {
	pos = textview_GetDotPosition((struct textview *)self->info->view);
	len = textview_GetDotLength((struct textview *)self->info->view);

	/*  only shows menu item if a selection has been made */
	/*  but, since can bind the procedure, must handle this */
	/*  case anyway. */
	if (len == 0) {
	    ERRORBOX(self->info->view, err_no_sel);
	    return;
	} else {
	    if (len >= HNSIZE) {
		ERRORBOX(self->info->view, err_sel_too_long);
		return;
	    }
	}
	i = 0;			/* position in text object */
	code = 0;               /* position in helpName */
	while(i<len) {
	    tc = text_GetChar((struct text *)self->info->data, i+pos);
	    i++;
	    if (tc == ' ' || tc == '\t' || tc == '\n') continue;
	    helpName[code++] = tc;
	}
	helpName[code++] = '\0';
    } else {			/* prompt for topic */
	code = message_AskForString(self, 0, msg_ask_prompt,
				    0, helpName, HNSIZE);
	if ((code < 0) || (helpName[0] == '\0')) return;
    }

    if (type & help_NEW_WIN) {
	struct im *im;
	struct frame *frame;

	im = im_Create(NULL);
	frame = frame_New();
	hv = (struct help*) class_NewObject("help");
	if (!hv || !im || !frame) {
	    ERRORBOX(self->info->view, err_no_new_view);
	    return;
	}

	/* since we exits, ego must exist now, too */
	if (ego->next != (struct self_help *)NULL) { /* > 1 instances */
	    struct self_help *tmp;

	    for (tmp = ego; tmp; tmp = tmp->next) {
		/* add "delete this window" menu item" */
		DEBUG(("Add delete\n"));
		tmp->this->info->flags |= MENU_SwitchDeleteMenu;
		SetupMenus(tmp->this->info);
	    }
	}
	
	/* frame for frame_SetView must have associated im */
	hv->app = (struct scroll*) help_GetApplicationLayer(hv);
	frame_SetView(frame, hv->app);
	im_SetView(im, frame);

	/* add in a message handler */
	frame_PostDefaultHandler(frame, "message",
				 frame_WantHandler(frame, "message"));

    }
    
    sprintf(buf, err_sorry, helpName);
    if (GetHelpOn((!(type & help_NEW_WIN)) ? self : hv
		  , helpName, help_NEW, help_HIST_NAME, buf) != 0) {
	panel_ClearSelection(self->overviewPanel);
	panel_ClearSelection(self->listPanel);
	panel_ClearSelection(self->historyPanel);
    }
}


/*
 * Add a history item for a given file with given dot, dotlen and top
 */
void AddBookmark(self)
register struct help *self;
{
    AddHistoryItem(self, help_HE_BOOKMARK, help_SHOW_HIST);
}


/*
 * Adds a search directory to the searchpath
 */
void AddSearchDir(self)
struct help *self;
{
    char buf[MAXPATHLEN];
    char buf2[MAXPATHLEN+100];
    int code;

    sprintf(buf2, "%s", "Added directory: ");
    code = completion_GetFilename((struct view *)self, msg_dir_prompt, NULL,
				  buf, sizeof(buf), TRUE, TRUE);
    if(code != -1) {
	help_AddSearchDir(buf);
	strcat(buf2, buf);
	message_DisplayString(self, 0, buf2);
    }
}

