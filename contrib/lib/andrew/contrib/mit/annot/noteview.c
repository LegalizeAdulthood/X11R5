/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *noteview_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/annot/RCS/noteview.c,v 1.2 1991/09/12 16:51:21 bobg Exp $";

#include "class.h"
#include "view.ih"
#include "textv.ih"
#include "note.ih"
#include "iconview.ih"
#include "noteview.eh"
#include "bind.ih"
#include "menulist.ih"
#include "keymap.ih"
#include "text.ih"
#include "proctbl.ih"

#define ICONFONT "icon"
#define ICONSTYLE "fontdesc_Plain"
#define ICONPTS 12
#define ICONCHAR '4'
#define TITLEFONT "andysans"
#define TITLESTYLE "fontdesc_Plain"
#define TITLEPTS 12

struct menulist *noteviewMenus;
static struct keymap *noteviewKeyMap;


/****************************************************************/
/*		private functions				*/
/****************************************************************/

static void
close(v,l)
struct noteview *v;
long l;
{
    noteview_Close(v);
}
static void
open(v,l)
struct noteview *v;
long l;
{
    noteview_Open(v);
}
static void
closeall(v,l)
struct view *v;
long l;
{
    iconview_CloseRelated(v);
}
static void
openall(v,l)
struct view *v;
long l;
{
    iconview_OpenRelated(v);
}

static void
insert(tv,l)
struct textview *tv;
long l;
{
    struct text *t;
    long pos;
    t = (struct text *) textview_GetDataObject(tv);
    pos = textview_GetDotPosition(tv) + textview_GetDotLength(tv);
    tv->currentViewreference = text_InsertObject(t, pos,"note", NULL);
    text_NotifyObservers(t,0);
}

static struct bind_Description noteviewBindings[]={
    {"noteview-close",NULL,0,"notes,close~2", 0,0,close,"close the note"},
    {"noteview-closeall",NULL,0,"notes,close all~12", 0,0,closeall,"close all the notes"},
    {"noteview-openall",NULL,0,"notes,open all~11", 0,0,openall,"open all the notes"},
    NULL
};
void noteview__PostMenus(self, menulist)
struct noteview *self;
struct menulist *menulist;
{
    menulist_ClearChain(self->menus);
    menulist_ChainBeforeML(self->menus, menulist, menulist);
    super_PostMenus(self, self->menus);
}


/****************************************************************/
/*		class procedures				*/
/****************************************************************/

boolean
noteview__InitializeClass(classID)
    struct classheader * classID;
{
    struct classinfo *textviewtype = class_Load("textview");
    struct classinfo *viewtype = class_Load("view");

    noteviewMenus = menulist_New();
    noteviewKeyMap =  keymap_New();
    bind_BindList(noteviewBindings, noteviewKeyMap , noteviewMenus, &noteview_classinfo);
    proctable_DefineProc("noteview-insertnote",insert,textviewtype,NULL,"Insert Note Object");
    proctable_DefineProc("noteview-openallnotes",openall,viewtype,NULL,"open Note Views");
    proctable_DefineProc("noteview-closeallnotes",closeall,viewtype,NULL,"close Note Views");
    return TRUE;
}


boolean
noteview__InitializeObject(classID,self)
struct classheader * classID;
struct noteview * self;
{

    self->menus = menulist_DuplicateML(noteviewMenus, self);
    noteview_SetIconFont(self,ICONFONT,ICONSTYLE,ICONPTS); 
    noteview_SetIconChar(self,ICONCHAR);
    noteview_SetTitleFont(self,TITLEFONT,TITLESTYLE,TITLEPTS);


    return TRUE;
}

/****************************************************************/
/*		instance methods				*/
/****************************************************************/

void
noteview__Print(self, file, processor, finalformat, toplevel)
    struct noteview * self;
    FILE * file;
    char * processor;
    char * finalformat;
    boolean toplevel;
{
    short doit;
    if (self->header.iconview.child == (struct view *)0)
	return;

    doit = 0;

    if (!toplevel
	 && strcmp(processor,"troff") == 0) {
	fputs("\\**\n.FS\n",file);
	doit = 1;
    }
    view_Print(self->header.iconview.bottomview,file,
		processor,finalformat,toplevel);
    if (doit)
	fputs(".FE\n",file);
}

