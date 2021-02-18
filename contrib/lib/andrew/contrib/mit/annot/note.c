/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *note_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/annot/RCS/note.c,v 1.3 1991/09/12 16:51:16 bobg Exp $";

#include <andrewos.h>
#include <class.h>
#include <text.ih>
#include <sys/types.h>
#include <pwd.h>
#include <note.eh>


/****************************************************************/
/*		private functions				*/
/****************************************************************/



/****************************************************************/
/*		class procedures				*/
/****************************************************************/
boolean
note__InitializeClass(classID)
    struct classheader * classID;
{
    return TRUE;
}

boolean
note__InitializeObject(classID,self)
struct classheader * classID;
struct note * self;
{
    struct text * to;
    struct passwd *userentry;

    userentry = getpwuid(getuid());
    note_SetTitle(self,userentry->pw_name);

    to = text_New();
    text_SetReadOnly(to,0);
    note_SetChild(self,to);
    return TRUE;
}

/****************************************************************/
/*		instance methods				*/
/****************************************************************/

void
note__SetChild(self,child)
    struct note * self;
    struct dataobject * child;
{
    super_SetChild(self,child);
    if (child != (struct dataobject *)0)
	text_SetReadOnly((struct text *)child, 0);
}
