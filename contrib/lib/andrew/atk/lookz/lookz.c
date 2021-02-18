/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/lookz/RCS/lookz.c,v 2.10 1991/09/12 16:24:06 bobg Exp $ */
/* $ACIS:lookz.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/lookz/RCS/lookz.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/lookz/RCS/lookz.c,v 2.10 1991/09/12 16:24:06 bobg Exp $";
#endif /* lint */


/* lookz.c		

	Code for the lookz data object
*/

#include <class.h>
#include <dataobj.ih>	/* for dataobject_NOREADERROR */
#include <text.ih>
#include <lookz.eh>

#define MAXFILELINE 255

	boolean
lookz__InitializeObject(ClassID, self)
	struct classheader *ClassID;
	register struct lookz  *self;
{
	self->visible = TRUE;
	self->canClose = TRUE;
	self->text = NULL;
	return TRUE;
}

	void
lookz__FinalizeObject(ClassID, self)
	struct classheader *ClassID;
	register struct lookz  *self;
{
	if(self->text != NULL) {
	    text_RemoveObserver(self->text, self);
	    self->text = NULL;
	}
}

	long
lookz__Read( self, file, id )
	register struct lookz  *self;
	register FILE  *file;
	register long  id;			/* !0 if data stream, 0 if direct from file*/
{
	/* reads a lookz from -file-.  See file format in lookz.ch */
	/* This routine reads the \enddata, if any. Its syntax is not checked */

	unsigned char s[MAXFILELINE + 2];
	unsigned char c;

	if ((c=getc(file)) == '\n') {}		/* COMPATIBILITY KLUDGE */
	else if (c == '\\') fgets(s, MAXFILELINE+2, file);	/* skip header */
	else ungetc(c, file);

	while (TRUE) {
		/* read the lines of the data stream */
		if ((fgets(s, MAXFILELINE + 2, file)) == 0) 
			/* EOF or error */
			break;
		if (*s == '\\') 
			/* \enddata */
			break;

		lookz_SetVisibility(self, strncmp(s, "visible", strlen("visible")) == 0);	
	}
	return dataobject_NOREADERROR;
}
	  
	long
lookz__Write( self, file, writeID, level )
	register struct lookz  *self;
	FILE  *file;
 	long  writeID;
	int  level;
{
	unsigned char head[50];
	long id = lookz_UniqueID(self);
	if (self->header.dataobject.writeID != writeID) {
		/* new instance of write, do it */
		self->header.dataobject.writeID = writeID;
		sprintf(head, "data{%s, %d}\n", class_GetTypeName(self), id);
		fprintf(file, "\\begin%s", head);

		fprintf(file, "%s\n", (lookz_GetVisibility(self) ? "visible" : "hidden"));

		fprintf(file, "\\end%s", head);
	}
	return id;
}

	void
lookz__SetVisibility(self, visibility)
	register struct lookz  *self;
	boolean visibility;
{
	/* this routine ensures that self->visible will be either TRUE or FALSE */
	if (self->visible != (visibility ? TRUE : FALSE)) {
		self->visible = (visibility ? TRUE : FALSE);
		lookz_NotifyObservers(self, lookz_VISIBILITYCHANGED);
	}
}

void lookz__SetCanClose(self, canClose)
struct lookz *self;
boolean canClose;
{
    if (self->canClose != canClose) {
	self->canClose = canClose;
	lookz_SetVisibility(self, TRUE);
	lookz_NotifyObservers(self, lookz_CANCLOSECHANGED);
    }
}

void lookz__SetTextObject(self, text)
struct lookz *self;
struct text *text;
{
    if (self->text != text) {
	if (self->text != NULL) {
	    text_RemoveObserver(self->text, self);
	}
	self->text = text;
	if (text != NULL) {
	    text_AddObserver(text, self);
	}
	lookz_NotifyObservers(self, lookz_TEXTOBJECTCHANGED);
    }
}

void lookz__ObservedChanged(self, dobj, value)
struct lookz *self;
struct dataobject *dobj;
long value;
{
    if (value == observable_OBJECTDESTROYED &&
	 (struct dataobject *) self->text == dobj) {
	text_RemoveObserver(self->text, self);
	self->text = NULL;
	lookz_NotifyObservers(self, lookz_TEXTOBJECTCHANGED);
    }
}

	
