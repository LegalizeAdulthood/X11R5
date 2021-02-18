/* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *writestmpv_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/time/RCS/writestmpv.c,v 1.2 1991/09/12 16:55:41 bobg Exp $";


#include <class.h>
#include <writestmpv.eh>
#include <writestmp.ih>
#include <proctbl.ih>
#include <util.h>

/* Defined constants and macros */

/* External Declarations */

/* Forward Declarations */

/* Global Variables */
static struct menulist *writestampview_menulist = NULL;

static char *formats[] = {
  "Default~1", NULL,
  "H:MM AM/PM~10", "%u:%M %P",
  "Month DD YYYY~11", "%o %A, %Y",
  NULL, NULL};


static void MenuSetFormat(self, format)
     struct writestampview *self;
     char *format;
{
  struct writestamp *b = (struct writestamp *) writestampview_GetDataObject(self);

  writestamp_SetFormat(b, format);
  writestamp_SetModified(b);
}


boolean
writestampview__InitializeClass(c)
struct classheader *c;
{
/* 
  Initialize all the class data.
*/
  int i;
  char *menuname;
  char temp[250];
  struct proctable_Entry *proc = NULL;
  char menutitlefmt[200];
  char procname[200];

  writestampview_menulist = menulist_New();
  
  sprintf(procname, "%s-set-format", "writestamp");
  proc = proctable_DefineProc(procname, MenuSetFormat, &writestampview_classinfo, NULL, "Set the writestamp's inset's format.");
  
  sprintf(menutitlefmt, "%s,%%s", "Write Stamp");
  
  for (i=0; formats[i]; i+=2) {
    sprintf(temp, menutitlefmt, formats[i]);
    menuname = NewString(temp);
    menulist_AddToML(writestampview_menulist, menuname, proc, formats[i+1], 0);
  }
  
  return(TRUE);
}


boolean
writestampview__InitializeObject(c, self)
struct classheader *c;
struct writestampview *self;
{
/*
  Set up the data for each instance of the object.
*/
  struct menulist *ml;

  if writestampview_GetMenuList(self) menulist_Destroy(writestampview_GetMenuList(self));
  ml = menulist_DuplicateML(writestampview_menulist, self);
  writestampview_SetMenuList(self, ml);

  return(TRUE);
}


void
writestampview__FinalizeObject(c, self)
struct classheader *c;
struct writestampview *self;
{
  return;
}


