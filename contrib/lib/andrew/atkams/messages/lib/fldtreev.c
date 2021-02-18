/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *fldtreev_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/fldtreev.c,v 1.5 1991/09/12 16:48:09 bobg Exp $";

#include <andrewos.h>
#include <class.h>
#include <stdio.h>
#include <keymap.ih>
#include <keystate.ih>
#include <menulist.ih>
#include <bind.ih>
#include <view.ih>
#include <im.ih>
#include <lpair.ih>
#include <org.ih>
#include <ams.ih>
#include <fldtreev.eh>

#define  menu_default		  (1<<0)
#define  menu_buttons		  (1<<3)
#define  menu_exploded		  (1<<4)
#define  menu_imploded		  (1<<5)
#define  menu_description_hidden  (1<<6)
#define  menu_description_exposed (1<<7)
#define  menu_palette_hidden	  (1<<8)
#define  menu_palette_exposed	  (1<<9)
#define  menu_horizontal	  (1<<10)
#define  menu_vertical		  (1<<11)
#define  menu_folded		  (1<<12)
#define  menu_unfolded		  (1<<13)
#define  menu_debug		  (1<<14)

static struct keymap *foldertreev_standardkeymap = NULL;
static struct menulist *foldertreev_standardmenulist = NULL;
static void QuitFolderTree( /* self */ );
static void QuitMessagesProc( /* self */ );

static struct bind_Description foldertreev_standardbindings [] = {
    {"foldertreev-delete-window", "\030\004", NULL, "Delete Window~89", NULL, 0, QuitFolderTree, "exit folder tree"},
    {"foldertreev-quit", "\030\003", NULL, "Quit~99", NULL, 0, QuitMessagesProc, "exit messages process"},
    {NULL,NULL,0,NULL,0,0,NULL,NULL,NULL}
};

boolean
foldertreev__InitializeClass( classID )
  struct classheader *classID;
{
    foldertreev_standardmenulist = menulist_New();
    foldertreev_standardkeymap = keymap_New();
    bind_BindList(foldertreev_standardbindings, foldertreev_standardkeymap, foldertreev_standardmenulist, &foldertreev_classinfo);
    return(TRUE);
}

void
foldertreev__FinalizeObject( classID, self )
  struct classheader	*classID;
  struct foldertreev	*self;
{
  if(self->menulist) {
      menulist_Destroy(self->menulist);
      self->menulist = NULL;
  }
  if(self->keystate) {
      keystate_Destroy(self->keystate);
      self->keystate = NULL;
  }
}

boolean
foldertreev__InitializeObject( classID, self )
  struct classheader	*classID;
  struct foldertreev	*self;
{
  self->menulist = menulist_DuplicateML(foldertreev_standardmenulist,self);
  self->keystate = keystate_Create(self,foldertreev_standardkeymap);
  return((self->menulist && self->keystate) ? TRUE : FALSE);
}

void
foldertreev__PostMenus( self, menulist )
  struct foldertreev	*self;
  struct menulist	*menulist;
{
  if(menulist) {
      menulist_SetMask(menulist,menulist_GetMask(menulist) & ~menu_description_exposed & ~menu_palette_exposed & ~menu_description_hidden & ~menu_palette_hidden & ~menu_default & ~menu_folded & ~menu_unfolded);
      menulist_ChainBeforeML(self->menulist,menulist,NULL);
  }
  view_PostMenus((struct view*)self->header.view.parent,self->menulist);
}

static void
QuitFolderTree( self )
  struct foldertreev	*self;
{
  struct org *org = NULL;

  org = (struct org*)foldertreev_GetDataObject(self);
  foldertreev_UnlinkTree(self);
  foldertreev_Destroy(self);
  org_Destroy(org);
}

static void
QuitMessagesProc( self )
  struct foldertreev	*self;
{
  ams_CommitState(TRUE, FALSE, TRUE, TRUE);
}

void
foldertreev__PostKeyState( self, keystate )
  struct foldertreev	*self;
  struct keystate	*keystate;
{
  if(keystate)
      keystate_AddBefore(self->keystate,keystate);
  view_PostKeyState((struct view*)self->header.view.parent,self->keystate);
}

void
foldertreev__FullUpdate( self, type, left, top, width, height )
  struct foldertreev	*self;
  enum view_UpdateType type;
  long left, top, width, height;
{
  struct view *v1, *v2;
  struct lpair *lp = NULL;

  lp = self->header.orgv.pair_view;
  v1 = lpair_GetNth(lp, 0);
  v2 = lpair_GetNth(lp, 1);
  lpair_VSplit(lp, v1, v2, 0, 1);
  super_FullUpdate(self,type,left,top,width,height);
}
