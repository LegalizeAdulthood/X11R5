/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/createinset/null/RCS/null.c,v 1.3 1991/09/12 16:11:39 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/createinset/null/RCS/null.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	char *null_c_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/createinset/null/RCS/null.c,v 1.3 1991/09/12 16:11:39 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* null.c		

	Code for the null data object
*/
/*
 *    $Log: null.c,v $
 * Revision 1.3  1991/09/12  16:11:39  bobg
 * Update copyright notice and rcsid
 *
 * Revision 1.2  1991/08/22  19:03:51  wjh
 * fix comparison in if(*nl=='\n')
 *
 * .
 *
 * Revision 1.1  1989/07/31  15:34:47  wjh
 * Initial revision
 *
 *  
 * Revision 1.0  88/05/14  15:40:33  wjh
 * Copied from /usr/andrew/lib/genericinset
 */


#include <null.eh>
#include <dataobj.ih>
/* $$$ include  xxx.ih  for any routine called below as xxx_routine() */


#define MAXFILELINE 255

/* $$$ */
	void
null__ClearDots(self)
	register struct null *self;
{
	register struct dotlist *d;
	for (d = null_GetFirstDot(self);  d != NULL; )  {
		register struct dotlist *t = null_GetNextDot(self, d);
		free(d);
		d = t;
	}
	self->dots = NULL;
	null_SetModified(self);	/* tell the enclosing document I have changed */
}

/* $$$ */
	void
null__AddDot(self, x, y)
	register struct null *self;
	long x, y;
{
	register struct dotlist *d;
	d = (struct dotlist *)malloc(sizeof (struct dotlist));
	d->next = null_GetFirstDot(self);
	d->x = x;
	d->y = y;
	self->dots = d;
	null_SetModified(self);	/* tell the enclosing document I have changed */
}


	boolean
null__InitializeClass(ClassID)
	struct classhdr *ClassID;
{
	/* initialize any global data used in this module */
	return TRUE;
}

	boolean
null__InitializeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct null  *self;
{
	/* here we give initial values to any fields that need them */
	
	self->dots = NULL;	/* $$$ */

	return TRUE;
}

	void 
null__FinalizeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct null  *self;
{
	/* free any storage allocated to self */
	null_ClearDots(self);	/* $$$ */
}

/* null__Read(self, file, id)
	Reads the data for 'self' from 'file',  see format in null.H
	If 'id' is 0, the data stream is at the outermost level and thus might
	be in an alternate format (see null__Write).  In this data object,
	the format is always the same.

	If 'id' is non-zero, the first line of the data stream will have been read
	by the caller and the value found in the line is passed as 'id'.  This must
	match the id value found in the enddata line.

	Returns a boolean indication of success.  Fails if syntax of headers
	is incorrect.
*/
	long
null__Read(self, file, id)
	register struct null  *self;
	register FILE  *file;
	register long  id;
{
	long result = dataobject_BADFORMAT;
	char s[MAXFILELINE + 2];
	long len;
	long sid, eid;

	if (id == 0) {
		if (fscanf(file, "\\begindata{null,%ld", &sid) != 1
				|| getc(file) != '}' || getc(file) != '\n') 
			return dataobject_NOTBE2DATASTREAM;
	}
	else {
		long c;
		sid = id;

		/*  The parent is supposed to have read the entire 
			header including the final \n.
		    Some ancient insets did not read the final \n,
			so we check and discard it here.
		    This must be removed if the internal data stream,
			might begin with \n.
		*/
		if ((c=getc(file)) != '\n')
			ungetc(c, file);
	}

	null_ClearDots(self);	/* $$$  initialize data structure */

	while (TRUE) {
		/* read the lines of the data stream */
		long x, y;

		char *nl;
		if ((fgets(s, MAXFILELINE + 2, file)) == 0) {
			/* EOF or error */
			result = dataobject_PREMATUREEOF;
			break;
		}
		if (*s == '\\') {
			/* \enddata */
			result = dataobject_NOREADERROR;
			break;
		}

		nl = s + strlen(s) - 1;		/* point at last character */
		if (*nl == '\n')
			*nl = '\0';	/* delete newline*/

		/* $$$ process an input line of the data stream */
		sscanf(s, "%d %d", &x, &y);
		null_AddDot(self, x, y);

	}
	null_NotifyObservers(self, null_DATACHANGED);
	null_SetModified(self);	/* tell the enclosing document I have changed */


	len = strlen("enddata{null,");
	if (result == dataobject_NOREADERROR &&
			( strncmp(s, "enddata{null,", len) != 0
			  || sscanf(s+len, "%d}\n", &eid) != 1
			  || eid != sid
			) )
		result = dataobject_MISSINGENDDATAMARKER;
	return result;
}


/* null__Write(self, file, writeID, level)
	Writes a data stream representation of 'self' to 'file'
	The same 'writeID' value is passed in for all writes during a given
	datastream construction.  Once this object has been written, it need 
	not be written again.

	The 'level' can be ignored.  (A level of 0 indicates that the
	output is to be the sole contents of a file, so the format could be 
	non-datastream.  This is only used to support data streams 
	defined prior to the base editor.)
	
	Returns the identifying number written into the header.  The caller will 
	use this number in referring to the object from the dictionary.
*/
	long
null__Write(self, file, writeID, level)
	register struct null  *self;
	FILE  *file;
 	long  writeID;
	int  level;
{
	char head[50];
	long id = null_GetID(self);
	if (self->header.dataobject.writeID != writeID) {
		/* new instance of write, do it */
		register struct dotlist *d;
		self->header.dataobject.writeID = writeID;
		sprintf(head, "data{%s, %d}\n", class_GetTypeName(self), id);
		fprintf(file, "\\begin%s", head);


		/* $$$ output lines of data stream */
		for (d = null_GetFirstDot(self);  d != NULL; 
				d = null_GetNextDot(self, d))  
			fprintf(file, "%d %d\n", null_GetDotX(self, d),
					null_GetDotY(self, d));


		fprintf(file, "\\end%s", head);
	}
	return id;
}
