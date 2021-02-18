/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/arbiter.c,v 2.5 1991/09/12 15:55:03 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/arbiter.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/arbiter.c,v 2.5 1991/09/12 15:55:03 bobg Exp $ ";
#endif /* lint */

#include <class.h>
#include <cel.ih>
#include <arbiter.eh>
struct arbiter *master;

struct arbiter *arbiter__GetMaster(classID)
struct classheader *classID;
{
    return master;
}
struct arbiter *arbiter__SetMaster(classID,newmaster)
struct classheader *classID;
struct arbiter *newmaster;
{
    struct arbiter *oldmaster;
    oldmaster = master;
    master = newmaster;
    return oldmaster;
}
boolean arbiter__InitializeClass(classID)
struct classheader *classID;
{
    master = NULL;
    return TRUE;
}

long arbiter__Read(self, file, id)
struct arbiter *self;
FILE *file;
long id;
{
    long result; /* set self as master so that as child cells are read, they will
		   call declare read on self, thus the arbiter will find all
		   of it's children */
    struct arbiter *lastmaster;
    lastmaster = arbiter_SetMaster(self);
    arbiter_SetArbiter(self,lastmaster);
    self->first = (struct cel *)self;
    result = super_Read(self, file, id);
    arbiter_SetMaster(lastmaster);
    arbiter_SetArbiter(self,lastmaster);
    arbiter_ReadObjects(self);
    return result;
}
long arbiter__ReadFile(self,thisFile)
struct arbiter *self;
FILE *thisFile;
{  
    long result; /* set self as master so that as child cells are read, they will
		   call declare read on self, thus the arbiter will find all
		   of it's children */
    struct arbiter *lastmaster;
    lastmaster = arbiter_SetMaster(self);
    arbiter_SetArbiter(self,lastmaster);
    self->first = (struct cel *)self;
    result = super_ReadFile(self, thisFile);
    arbiter_SetMaster(lastmaster);
    arbiter_SetArbiter(self,lastmaster);
    arbiter_ReadObjects(self);
    return result;
}
FILE *arbiter__DeclareRead(self,cel)
struct arbiter *self;
struct cel *cel;
{
#ifdef DEBUG
    fprintf(stdout,"declaring %d(%s) to %d(%s)\n",cel,cel_GetRefName(cel),self,arbiter_GetRefName(self));fflush(stdout);
#endif /* DEBUG */
    if(cel != (struct cel *) self){
	cel->chain = self->first;
	self->first = cel;
    }
    else if(arbiter_GetArbiter(self) && arbiter_GetArbiter(self) != self)
	    arbiter_DeclareRead(arbiter_GetArbiter(self),(struct cel *)self);

    return NULL;
}
struct cel *arbiter__FindChildCelByName(self,name)
struct arbiter *self;
char *name;
{
    struct cel *cel,*ncel;
    char *tname;
    cel = arbiter_GetFirst(self);
    while(cel != (struct cel *) self){
	if((tname = cel_GetRefName(cel)) != NULL &&
	   strcmp(tname,name) == 0)
	    return cel;
	ncel = cel_GetNextChain(cel);
	if(ncel == cel) return NULL; /* shouldn't happen */
	cel = ncel;
    }
    return NULL;
}
struct dataobject *arbiter__FindChildObjectByName(self,name)
struct arbiter *self;
char *name;
{
    struct cel *cel;
    cel = arbiter_FindChildCelByName(self,name);
    if(cel != NULL) return cel_GetObject(cel);
    return NULL;
}
void arbiter__ReadObjects(self)
struct arbiter *self;
{
/* for subclass to use */
}
boolean arbiter__InitializeObject(classID, self)
struct classheader *classID;
struct arbiter *self;
{
self->first = (struct cel *)self;
return TRUE;
}
