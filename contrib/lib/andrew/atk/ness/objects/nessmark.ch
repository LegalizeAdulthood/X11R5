/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nessmark.ch,v 1.6 1991/09/12 19:44:10 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nessmark.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nessmark.ch,v 1.6 1991/09/12 19:44:10 bobg Exp $";
#endif
/*
nessmark.H

	nessmark object

	A mark object that also identifies the text that is marked.

*/
/*
    $Log: nessmark.ch,v $
Revision 1.6  1991/09/12  19:44:10  bobg
Update copyright notice

Revision 1.5  1989/06/01  15:58:17  wjh
campus release version

Revision 1.2  88/12/07  22:43:44  wjh

9 Nov
implemented access level control
skip first line of script if starts with # (for shell script)
changed so the name of all initialization functions is init()
added ness-load
moved execution of init() from compilation to first execution
deferred compilation to first FullUpdate time

22 Nov
proc table calls now work correctly with type-free procs  (the first arg can be anything)
added "cheat_" functions which will remain undocumented
changed inset() to a function
fixed some bugs in initial access

25 November
added long strings
added Next Error menu option
made replace() work correctly in all cases
added class() and new()

29 Nov
added ^<upper-case> and \e as characters in strings
added nextn() and length()

6 Dec
added functions: parseint(), parsereal(), firstobject(), whereitwas(), replacewithobject(), addstyles(), nextstylegroup(), enclosingstylegroup(), clearstyles(), hasstyles()
catch bus and segmentation errors


Revision 1.1  88/10/21  10:59:54  wjh
Initial revision

 * Creation 88/03/24 14:56:00 wjh
 * Created by WJHansen following definitions in 
 *	"A Practical Algebra for Substring Expressions"
*/

 
class  nessmark  :  mark

{
overrides:

	UpdateMarks(/* struct nessmark *self, */ long pos, long size);
		/* update mark locations for a change to base of size at pos.
			'size' may be negative for deletion. */

methods:

	SetText(/* struct nessmark *self, */ struct simpletext *text);
		/* changes to refer to 'text'.  Updates links in chain from text. */

	Set(/* struct nessmark *self, */ struct simpletext *text, long pos, long length);
		/* sets the mark to the indicated position and length in the given text */
	MakeConst(/* struct nessmark *self, */ char * cx);
		/* sets the mark to refer to the constant */
	Next(/* struct nessmark *self */);
		/* modifies the mark to point to the character after the former value */
	Start(/* struct nessmark *self */);
		/* modifies the mark to refer to the start of the former value */
	Base(/* struct nessmark *self */);
		/* modifies the mark to refer to the entire text */
	Extent(/* struct nessmark *self, */ struct nessmark *tail);
		/* modifies the mark to extend from its former start to the end of tail */
	Replace(/* struct nessmark *self, */ struct nessmark *replacement);
		/* replaces the contents of the mark with the replacement text */

	Equal(/* struct nessmark *self, */ struct nessmark *comparand) returns boolean;
		/* compares the text denoted by the two marks */
	IsEmpty(/* struct nessmark *self */) returns boolean;
		/* tests the text of self to see if it has any characters */

	Length(/* struct nessmark *self */) returns long;
		/* returns the number of characters in the mark */
	NextN(/* struct nessmark *self, */ long n);
		/* apply next 'n' times */

	SetFrom(/* struct nessmark *self, */ struct nessmark *src);
		/* copies src marker to self  (they share text) */
	ToC(/* struct nessmark *self */) returns unsigned char *;
		/* mallocs a string and copies mark into it.  The caller must free the value. */

macromethods:


	GetText(/* struct nessmark *self */)   \
		((struct simpletext *)(mark_GetObject((struct mark *)self)))

	/* DetachFromText and AttachToText are intended for use when moving
	   a mark, as on a stack.  The text is not deleted even if it has
	   no marks after the DetachFromText.  To get rid of a mark, 
	   nessmark_Destroy should be used instead.  To change the text
	   associated with a mark, use SetText instead. */
	DetachFromText(/* struct nessmark *self */) { \
		simpletext_RemoveMark(nessmark_GetText(self), self);  \
		mark_SetObject((struct mark *)self, NULL);  \
	}
		/* XXX dirty:  Should have an AddMark method in simpletext.H */
	AttachToText(/* struct nessmark *self,  struct simpletext * */  text) { \
		mark_SetObject((struct mark *)self, (struct simpletext *)text);  \
		mark_SetNext((struct mark *)(self), ((struct simpletext *)text)->markList);  \
		((struct simpletext *)text)->markList = (struct mark *)(self);  \
	}
	InitFrom(/* struct nessmark *self,  struct nessmark * */  src) {*(self) = *(src);  \
		nessmark_AttachToText(self, nessmark_GetText(self));  }
		/* copies src marker to self  (they share text);
		   self did not previously have a marker value */

classprocedures:

	InitializeClass(/* struct classhdr *ClassID*/) returns boolean;
	InitializeObject(/* struct classhdr *ClassID, */ struct nessmark *self) returns boolean;
	FinalizeObject(/* struct classhdr *ClassID, */ struct nessmark *self);

	/* Warning:  If a nessmark is attached to a text that has no other marks, 
	the text will be deleted when the nessmark is Destroyed  XXX */

data:

};

