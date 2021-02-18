/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/bind.c,v 2.7 1991/09/12 15:57:52 bobg Exp $ */
/* $ACIS:bind.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/bind.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/bind.c,v 2.7 1991/09/12 15:57:52 bobg Exp $";
#endif /* lint */

/* bind.c
 */

/*  */

#include <class.h>
#include <keymap.ih>
#include <menulist.ih>
#include <proctbl.ih>
#include <bind.eh>

void bind__BindList(classID, bl, km, ml, type)
struct classheader *classID;
struct bind_Description *bl;
struct keymap *km;
struct menulist *ml;
struct classinfo *type;
{
    while(bl && (bl->procName || bl->keyVector || bl->menuEntry)) {
	struct proctable_Entry *pe;

	if(bl->procName)
	    pe = proctable_DefineProc(bl->procName, (procedure) bl->proc, type, bl->module, bl->doc);
	else
	    pe = NULL;

	if(km && bl->keyVector)
	    keymap_BindToKey(km, bl->keyVector, pe, bl->keyRock);
	if(ml && bl->menuEntry)
	    menulist_AddToML(ml, bl->menuEntry, pe, bl->menuRock, bl->menuMask);

	bl++;
    }
}
