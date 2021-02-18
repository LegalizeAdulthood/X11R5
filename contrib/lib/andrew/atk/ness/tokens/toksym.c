/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/toksym.c,v 1.7 1991/09/12 16:27:29 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/toksym.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/toksym.c,v 1.7 1991/09/12 16:27:29 bobg Exp $";
#endif

/* toksym.c		

	Code for the toksym object
*/
/*
 *    $Log: toksym.c,v $
 * Revision 1.7  1991/09/12  16:27:29  bobg
 * Update copyright notice and rcsid
 *
 * Revision 1.6  1991/05/13  18:17:04  susan
 * added end of line
 * .
 *
 * Revision 1.5  1991/01/14  17:23:03  wjh
 * added a routine to extract the symbol value from the original text
 *
 * Revision 1.4  89/06/01  10:09:24  wjh
 * revision for new securtiy system
 * 
 * Revision 1.1  88/10/21  10:43:34  wjh
 * Initial revision
 *  
 * Revision 1.0  88/07/14  13:22:05WJHansen
 * Copied from sym.c and discarded EVERYTHING
 */


#include <class.h>
#include <toksym.eh>
#include <smpltext.ih>
#include <xgetchar.h>


	boolean
toksym__InitializeObject(ClassID, self)
	struct classhdr *ClassID;
	struct toksym *self;
{
	self->loc = 0;	/* (because error terminator set uses -3 as flag) */
	self->len = 0;
	self->info.obj = NULL;
	return TRUE;
}


	void
toksym__FinalizeObject(ClassID, self)
	struct classhdr *ClassID;
	struct toksym *self;
{
}

/* toksym__ToC(t, buf, maxlen)
	copies the token from the text t to the buffer buf, up to maxlen bytes
	returns buf 
*/
	char *
toksym__ToC(self, t, buf, maxlen)
	struct toksym *self;
	register struct text *t;
	register char *buf; 
	long maxlen;
{
	register char *cx;
	register long loc, len;
	cx = buf;
	loc = self->loc;
	len = self->len;
	if (len > maxlen-1) 
		len = maxlen - 1;
	for ( ;  len > 0;  len--, loc++)
		*cx++ = simpletext_CorrectGetChar(&t->header.simpletext, loc);
	*cx = '\0';
	return buf;
}
