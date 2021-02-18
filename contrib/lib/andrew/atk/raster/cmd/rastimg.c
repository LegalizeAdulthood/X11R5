/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/cmd/RCS/rastimg.c,v 2.5 1991/09/12 16:29:01 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/cmd/RCS/rastimg.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	char *rasterimage_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/cmd/RCS/rastimg.c,v 2.5 1991/09/12 16:29:01 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 * P_R_P_Q_# (C) COPYRIGHT IBM CORPORATION 1987
 * LICENSED MATERIALS - PROPERTY OF IBM
 * REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
 */

/* rastimg.c		

	Code for the rasterimage object

	Provides for file name storage with a pixelimage
	and for refcnt, WriteID, and ObjectID

*/
/*
    $Log: rastimg.c,v $
 * Revision 2.5  1991/09/12  16:29:01  bobg
 * Update copyright notice and rcsid
 *
 * Revision 2.4  1989/02/17  16:58:45  ghoti
 * ifdef/endif,etc. label fixing - courtesy of Ness
 *
 * Revision 2.3  89/02/08  16:31:51  ghoti
 * change copyright notice
 * 
 * Revision 2.2  89/02/04  12:44:12  ghoti
 * first pass porting changes: filenames and references to them
 * 
 * Revision 2.1  88/09/27  16:50:20  ghoti
 * adjusting rcs #
 * 
 * Revision 1.2  88/09/15  16:39:03  ghoti
 * copyright fix
 * 
 * Revision 1.1  88/09/01  02:03:14  zs01
 * "initial
 * 
 * Revision 5.2  88/08/19  14:18:51  ghoti
 * Includes now use '<>' instead of '""'
 * 
 * Revision 5.1  88/07/15  16:08:55  mp33
 * Changed Clone method to do the right thing.
 * 
 * Revision 1.1  87/12/06  16:37:14  wjh
 * Initial revision
 * 
 * 10 Nov 1987 WJHansen. Created.
 */

#include <stdio.h>
#include <class.h>
#include <rastimg.eh>


/* Stabilize(s)
	Copies 's' into newly malloced storage.
	XXX we need a home for this function 
*/
	static char *
Stabilize(s)
	char *s;
{
	return (char *)strcpy(malloc(strlen(s)+1), s);
}


boolean
rasterimage__InitializeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct rasterimage  *self;
{
	self->filename = self->resolutionPath = NULL;
	self->refcnt = 0;
	rasterimage_SetObjectID(self, 0);
	rasterimage_SetWriteID(self, 0);
	return TRUE;
}

void 
rasterimage__FinalizeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct rasterimage  *self;
{
	if (self->filename) free(self->filename);
	if (self->resolutionPath) free(self->resolutionPath);
	self->filename = self->resolutionPath = NULL;
}

	struct rasterimage *
rasterimage__Create(ClassID, width, height)
	struct classhdr *ClassID;
	long width, height;
{
	struct rasterimage *self = rasterimage_New();
	rasterimage_Resize(self, width, height);
	return self;
}



	void
rasterimage__AddObserver(self, observer)
	struct rasterimage *self;
	struct object *observer;
{
	super_AddObserver(self, observer);
	self->refcnt++;
}

	void
rasterimage__RemoveObserver(self, observer)
	struct rasterimage *self;
	struct object *observer;
{
	super_RemoveObserver(self, observer);
	self->refcnt--;
	if (self->refcnt <= 0)
		rasterimage_Destroy(self);
}

	struct rasterimage *
rasterimage__Clone(self)
	struct rasterimage *self;
{
	struct rasterimage *new = super_Clone(self);
	new->filename = self->filename;
	new->refcnt = 1;
	rasterimage_SetObjectID(new, 0);
	rasterimage_SetWriteID(new, 0);
	return new;	
}

/* rasterimage__FindFile(self, filename, path)
	Searches for a file of the given name.  Checks directories in this order
		the directory containing current document (??? how)
		the directories in the rasterpath (from preferences)
		the given path  (which may be NULL)
	records the file name and the path that was found to contain the file

	Returns an open stream for the file.

	XXX right now it just uses the filename
*/
	FILE *
rasterimage__FindFile(self, filename, path)
	struct rasterimage *self;
	char *filename, *path;
{
	self->filename = Stabilize(filename);
	self->resolutionPath = Stabilize(path);
	return  fopen(filename, "r");
}

/* Defile(self)
	Removes saved filename and path 
*/
	void
rasterimage__Defile(self)
	struct rasterimage *self;
{
	if (self->filename) free(self->filename);
	if (self->resolutionPath) free(self->resolutionPath);
	self->filename = self->resolutionPath = NULL;
}

