/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/annot/RCS/psview.ch,v 1.3 1991/09/12 20:15:46 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/annot/RCS/psview.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	char *sketch_ch_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/annot/RCS/psview.ch,v 1.3 1991/09/12 20:15:46 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class psview : iconview
{
 overrides:
    Print(FILE *file, char *processor, char *finalFormat, boolean topLevel);
    PostMenus(struct menulist *menulist);
    SetDataObject(struct dataobject * dobj);
 methods:
 classprocedures:
    InitializeObject(struct thisobject *self) returns boolean;
    InitializeClass() returns boolean;
 data:
    struct menulist *menus;
    int interpret;
};
