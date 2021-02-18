/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nessmark.c,v 1.13 1991/09/12 16:26:14 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nessmark.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nessmark.c,v 1.13 1991/09/12 16:26:14 bobg Exp $";
#endif

/* nessmark.c		

	Code for the nessmark data object

	Deficiency:  Should test for objectFree more places XXX

*/
/*
    $Log: nessmark.c,v $
 * Revision 1.13  1991/09/12  16:26:14  bobg
 * Update copyright notice and rcsid
 *
 * Revision 1.12  1990/02/21  13:08:29  wjh
 * fix core leak
 *
 * Revision 1.11  89/11/04  17:25:57  wjh
 * patch from Guy Harris to ness.c:
 * Do initializeEnvt() in ness_EstablishViews() to be sure the pointers are initialized.  This avoids "!!! Disasterous error..."
 * 
 * patch from Guy Harris to ness.c:
 * Change call.c so it passes the long variant of anytype to proc in callCfunc.  This again avoids a "!!! Disasterous error."  Similarly change the argument to free() in FreeTheMallocs.
 * 
 * Changed delimiter in origin line to double backslash to avoid error warning in sendmessage about illegal character in text.
 * 
 * Added ness_GetOriginData to extract fields from origin data.
 * 
 * ESC-~ gave funny results in a nessview if text wass initially read-only, but had no warning.  It is now fixed to keep read-onliness and the warning message in synch.
 * 
 * Implemented  pragmat:  --$Ness <level>   
 * Initial syntax level is CURRENTSYNTAXLEVEL.
 * Level before read is set to UNSPECIFIEDSYNTAXLEVEL   (i.e., -1)
 * After compile check syntax level and give error if too HIGH.
 * 
 * If the left operand of a catenation has a style, that style was imposed on the right operand, but should not have been.   It no longer is.
 * 
 * 
 * Revision 1.10  89/10/08  07:28:00  wjh
 * test for ObjectFree in SetText
 * 
 * Revision 1.9  89/09/03  22:48:25  wjh
 * newness
 * 
 * Revision 1.8  89/06/23  17:20:55  wjh
 * (Items that should not be sent to downstairs are marked "R4".)
 * 
 * Added a call to CheckForInterrupt in the function calls and internal gotos.  Thus loops can be terminated with ^G.   (interp.c)  R4
 * 
 * Changed dokeys() so it uses im_DoKeySequence.  This means it is now possible to send a key sequence which involves the message line.  (interp.c)  R4
 * 
 * Implemented DoMenu(object, menustring).   This function causes the same behavior as if the user selected the menu option.  At present the menu string must be exactly as originally defined;  see the warning above for im_DoMenu().  (interp.c, call.c)  R4
 * 
 * Changed access to Ness library routines so they are always compiled.  (They used ot get the default of NoCompilation, so they were useless.)  (call.c)  
 * 
 * Removed a superflous {} pair.  {This is purely a cosmetic change.}  (nessmark.c) 
 * 
 * Fixed value_GetStringValue.  Formerly it was getting an invalid initial value.  {The fix was done by adding a call to nessmark_Initialize() in the stackString section.}  (nevent.c) 
 * 
 * Modified the data stream so errors will not occur when a ness object is the outermost object.  The fix was to add two bytes, "00", at the end of the origin string to prevent the former occurence of a spurious "\}".  (ness.c) 
 * 
 * Fixed menu handling so Ness and child menus get posted when there is a mouse click in an inset within the Ness.  Formerly neither set of menus was posted. (nessv.c) 
 * 
 * Fixed dostmt, the function called from ness-load, which is recommended to be bound to ESC-ESC.  It was using a NULL pointer, so people were getting core dumps if they typed ESC-ESC before doing a compile.  (ness.c) 
 * 
 * Avoided an infinite loop which occurred if a library function referred to a non-existent entry point within itself.  Did this by checking to see if the library function is already Compiling just before trying to compile it.  (call.c call.hn, call.h) 
 * 
 * Revised system marker allocation so the compilation will not get a subsequent error.  (gen.c)
 * 
 * Revised system marker allocation so it expands the space available if necessary. This will make it possible to compile larger programs.  (gen.c)
 * 
 * Changed the type of TType to long from struct object *.  This will allow compilation on stricter compilers.  (interp.h) 
 * 
 * Fixed nessmark_FinalizeObject so it would not reference a NULL pointer.  {Somehow the assembler noticed this bug!}  (nessmark.c) 
 * 
 * Changed functions which deal with constant strings to have (char *) as there argument type (SaveError, exprnode_Create, ReportError, ExprError, RunError, LocateErrorFunc, QueryReadOnly, makeConst, printallerrors) or return type (Freeze, argcounterr, argtypeerr).  This prevents compile errors on picky compilers.  (interp.c, error.c, call.c, ness.c, nessv.c, search.c, nevent.c, nessmark.c, nessrun.ci)  R4 
 * 
 * Changed Imakefile to store Imakefile in checkin rule.  (Imakefile)
 * 
 * 
 * Revision 1.7  89/06/01  16:00:37  wjh
 * campus release version
 * 
 * Revision 1.3  89/01/05  16:19:53  wjh
 * allow negative argument to nextn()
 * 
 * Revision 1.2  88/12/07  22:43:50  wjh
 * 
 * 9 Nov
 * implemented access level control
 * skip first line of script if starts with # (for shell script)
 * changed so the name of all initialization functions is init()
 * added ness-load
 * moved execution of init() from compilation to first execution
 * deferred compilation to first FullUpdate time
 * 
 * 22 Nov
 * proc table calls now work correctly with type-free procs  (the first arg can be anything)
 * added "cheat_" functions which will remain undocumented
 * changed inset() to a function
 * fixed some bugs in initial access
 * 
 * 25 November
 * added long strings
 * added Next Error menu option
 * made replace() work correctly in all cases
 * added class() and new()
 * 
 * 29 Nov
 * added ^<upper-case> and \e as characters in strings
 * added nextn() and length()
 * 
 * 6 Dec
 * added functions: parseint(), parsereal(), firstobject(), whereitwas(), replacewithobject(), addstyles(), nextstylegroup(), enclosingstylegroup(), clearstyles(), hasstyles()
 * catch bus and segmentation errors
 * 
 * 
 * Revision 1.1  88/10/21  11:00:09  wjh
 * Initial revision
 * 
 * Creation 88/03/24 15:05:00 wjh
 * 	Created by WJHansen following definitions in 
 *	"A Practical Algebra for Substring Expressions"

XXX code in the basic operations here will be repeated in the interpreter 

*/

#include <class.h>
#include <xgetchar.h>	/* includes smpltext.ih */
#include <mark.ih>
#include <text.ih>
#include <nstdmark.ih>

#include <nessmark.eh>

static char  debug = FALSE;
#define DEBUG(s) {if (debug) {printf s ; fflush(stdout);}}
#define ENTER(r) DEBUG(("Enter %s(0x%lx)\n", "r", self))
#define LEAVE(r) DEBUG(("Leave %s(0x%lx)\n", "r", self))


/* the following booleans are side arguments 
	from nessmark_Replace to nessmark_UpdateMarks.
	XXX Ugly global vars */
static boolean ReplacingEmpty = FALSE;
static boolean ReplacingNonEmpty = FALSE;



/* an empty marker is any marker with a length of zero 
	When we need to generate one on a unique text,
	the text we use is EmptyText.
*/
struct simpletext *nessmark_EmptyText;


	boolean
nessmark__InitializeClass(ClassID)
	struct classhdr *ClassID;
{
	nessmark_EmptyText = simpletext_New();
	simpletext_SetReadOnly(nessmark_EmptyText, TRUE);
	nessmark_SetText(nessmark_New(), nessmark_EmptyText);
			/* create a marker chained to EmptyText so it will
				not be discarded */
	return TRUE;
}

	boolean
nessmark__InitializeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct nessmark  *self;
{
	nessmark_SetLength(self, 0);
	nessmark_SetPos(self, 0);
	nessmark_SetStyle(self, FALSE, TRUE);
	nessmark_AttachToText(self, nessmark_EmptyText);
	return TRUE;
}

	void 
nessmark__FinalizeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct nessmark  *self;
{
	struct simpletext *text = nessmark_GetText(self);
	nessmark_DetachFromText(self);
	if (text->markList == text->fence && text->fence->next == NULL) 
		simpletext_Destroy(text);
}


	void 
nessmark__UpdateMarks(self, pos, size)
	struct nessmark *self;
	register long pos;
	register long size;
{
	register struct nessmark *mark;
	register long endPos;

	if (size > 0)  for (mark = self; mark != NULL; 
				mark = (struct nessmark *)
					nessmark_GetNext(mark))  {
		endPos = nessmark_GetEndPos(mark);
		if (pos > endPos)
			{}
		else if (pos == endPos)  {
			if (nessmark_IncludeEnding(mark) 

					/* The next line differs from
					 mark_UpdateMarks.  It ensures that
					 nessmarks before the replacement
					 of an empty nessmark are not extended.   */
					
					&& ! ReplacingEmpty)  {


					/* the following test and then-clause
					 also differ from mark_Update marks.
					 They ensure that an empty mark
					 that follows a non-empty replaced
					 mark will follow the replacement.  */

				if (nessmark_GetLength(mark) == 0 
						&& ReplacingNonEmpty)
					nessmark_SetPos(mark,
						nessmark_GetPos(mark) + size);
				else {		
					nessmark_SetLength(mark, 
						nessmark_GetLength(mark) + size);
					nessmark_SetModified(mark, TRUE);
				}
			}
		}
		else if (pos < nessmark_GetPos(mark) 
				|| pos == nessmark_GetPos(mark) 
				&& ! nessmark_IncludeBeginning(mark))
			nessmark_SetPos(mark, nessmark_GetPos(mark) + size);
		else {
			nessmark_SetLength(mark, 
				nessmark_GetLength(mark) + size);
			nessmark_SetModified(mark, TRUE);
		}
	} /* end if() for (mark...) */
	else if (size == 0)
		{}
	else for (mark = self; mark != NULL; 
			mark = (struct nessmark *)nessmark_GetNext(mark))  {
		/* size < 0 :  deletion */
		if (pos < (endPos = nessmark_GetEndPos(mark)))  {
			/* this mark needs to be adjusted */
			if (pos - size <= nessmark_GetPos(mark)) 
				/* Deletion is before this mark */
				nessmark_SetPos(mark, 
						nessmark_GetPos(mark) + size);
			else  {
				register long tsize = size;
				if (pos <= nessmark_GetPos(mark)) {
					/* part of deletion is before mark */
					tsize += nessmark_GetPos(mark) - pos;
					nessmark_SetPos(mark, pos);
				}
				/* restrict deletion size to that of mark */
				if (pos - tsize > endPos)
					tsize = pos - endPos;
				/* reduce size of mark per deletion */
				nessmark_SetLength(mark, 
					nessmark_GetLength(mark) + tsize);
				if (nessmark_GetLength(mark) < 0) 
					nessmark_SetLength(mark, 0);
				nessmark_SetModified(mark, TRUE);
			}
		}  /* end pos < endPos */
	} /* end for mark */
}


	void 
nessmark__SetText(self, text)
	register struct nessmark *self;
	register struct simpletext *text;
{
	register long tl = simpletext_GetLength(text),
		pos = nessmark_GetPos(self);
	register struct simpletext *oldtext;
ENTER(nessmark__SetText);
	if (pos > tl)    
		nessmark_SetPos(self, pos = tl);
	if (pos+nessmark_GetLength(self) > tl)  
		nessmark_SetLength(self, tl - pos);
	nessmark_SetModified(self, FALSE);
	oldtext = nessmark_GetText(self);
	if (text != oldtext) {
		if (oldtext != NULL && ! nessmark_ObjectFree(self)) {
			nessmark_DetachFromText(self);
			if (oldtext->markList == oldtext->fence 
					&& oldtext->fence->next == NULL) 
				simpletext_Destroy(oldtext);
		}
		nessmark_AttachToText(self,  (text != NULL) ? text : nessmark_EmptyText);
		nessmark_SetObjectFree(self, FALSE);
	}
LEAVE(nessmark__SetText);
}

	void 
nessmark__Set(self, text, pos, len)
	register struct nessmark *self;
	register struct simpletext *text;
	register long pos, len;
{
ENTER(nessmark__Set);
	if (len < 0)  {pos = pos + len;  len = -len;}
	if (pos < 0)  {len = len+pos;  pos = 0;}
	nessmark_SetPos(self, pos);
	nessmark_SetLength(self, len);
	nessmark_SetText(self, text);
LEAVE(nessmark__Set);
}


	void 
nessmark__MakeConst(self, cx)
	register struct nessmark *self;
	register char *cx;
{
	register long len = strlen(cx);

	if (len == 0)
		nessmark_Set(self, nessmark_EmptyText, 0, 0);
	else {
		register struct simpletext *t = simpletext_New();
		simpletext_InsertCharacters(t, 0, cx, len);
		simpletext_SetReadOnly(t, TRUE);
		nessmark_Set(self, t, 0, len);
	}
}

/* XXX code in these routines is duplicated in the ness interpreter */

	void 
nessmark__Next(self)
	register struct nessmark *self;
{
	register struct simpletext *t = nessmark_GetText(self);
	register long pos;
	nessmark_SetPos(self,  
			pos = (nessmark_GetPos(self)
			   +  nessmark_GetLength(self))   );
	nessmark_SetLength(self,  
			(pos < simpletext_GetLength(t)) ? 1 : 0);
}

	void 
nessmark__Start(self)
	register struct nessmark *self;
{
	nessmark_SetLength(self, 0);
}

	void 
nessmark__Base(self)
	register struct nessmark *self;
{
	nessmark_SetPos(self, 0);
	nessmark_SetLength(self, simpletext_GetLength(nessmark_GetText(self)));
}

	void 
nessmark__Extent(self, tail)
	register struct nessmark *self;
	register struct nessmark *tail;
{
ENTER(nessmark__Extent);
	if (nessmark_GetText(self) != nessmark_GetText(tail))
		nessmark_Set(self, nessmark_EmptyText, 0, 0);
	else {
		register start = nessmark_GetPos(self);
		register end = nessmark_GetPos(tail) 
				+ nessmark_GetLength(tail);
		if (end < start)
			start = end;
		nessmark_SetPos(self, start);
		nessmark_SetLength(self, end - start);
LEAVE(nessmark__Extent);
	}
}

	void 
nessmark__Replace(self, replacement)
	register struct nessmark *self;
	register struct nessmark *replacement;
{
	struct text *text = (struct text *)nessmark_GetText(self);
	register long destpos = nessmark_GetPos(self);
	register long destlen = nessmark_GetLength(self);
	register long srcpos = nessmark_GetPos(replacement);
	register long srclen = nessmark_GetLength(replacement);
	boolean oldgliso;
ENTER(nessmark__Replace);
	DEBUG(("dest(%d,%d)   src(%d,%d)  destbaselen %d\n", 
		destpos, destlen, srcpos, srclen, text_GetLength(text)));

	/* arrange so that styles that end at the beginning of the point of replacement, 
		will not apply to the replacement text. */
	oldgliso = nestedmark_SetGlobalIsolation(TRUE);

	/* insert the new text at end of the destination, then delete dest */
	if (destlen == 0) {
		ReplacingEmpty = TRUE;
		text_AlwaysCopyText(text, destpos + destlen, 
			nessmark_GetText(replacement), srcpos, srclen);
		ReplacingEmpty = FALSE;
	}
	else {
		ReplacingNonEmpty = TRUE;
		text_AlwaysCopyText(text, destpos + destlen, 
			nessmark_GetText(replacement), srcpos, srclen);
		text_AlwaysDeleteCharacters(text, destpos, destlen);
		ReplacingNonEmpty = FALSE;
	}
	nestedmark_SetGlobalIsolation(oldgliso);
	nessmark_SetLength(self, srclen);
	text_NotifyObservers(text, 0);

	DEBUG(("dest(%d,%d)   src(%d,%d)  finallen %d  destbaselen %d\n", 
		destpos,  destlen,  srcpos,  srclen,  
		nessmark_GetLength(self), 
		text_GetLength(text)));
}

	boolean
nessmark__Equal(self, comparand)
	register struct nessmark *self, *comparand;
{
	register i, iend, j;
	register struct simpletext *itext, *jtext;

	if (nessmark_GetLength(self) 
				!= nessmark_GetLength(comparand))
		return FALSE;

	i = nessmark_GetPos(self);
	iend = nessmark_GetEndPos(self);
	j = nessmark_GetPos(comparand);
	itext = nessmark_GetText(self);
	jtext = nessmark_GetText(comparand);
	for (  ;  i < iend;  i++, j++)
		if (simpletext_CorrectGetChar(itext, i) != simpletext_CorrectGetChar(jtext, j))
			return FALSE;
	return TRUE;
}

	boolean
nessmark__IsEmpty(self)
	register struct nessmark *self;
{
	return (nessmark_GetLength(self) == 0);
}

	long
nessmark__Length(self)
	register struct nessmark *self;
{
	return nessmark_GetLength(self);
}

	void
nessmark__NextN(self, n)
	register struct nessmark *self;
	long n;
{
	register struct simpletext *t = nessmark_GetText(self);
	long tlen = simpletext_GetLength(t);
	register long pos;
	if (n == 0) return;
	if (n < 0) {
		pos = nessmark_GetPos(self)  +  n;
		if (pos < 0) pos = 0;
	}
	else {
		pos = nessmark_GetPos(self)  +  nessmark_GetLength(self) + n-1;
		if (pos > tlen) pos = tlen;
	}
	nessmark_SetPos(self,  pos);
	nessmark_SetLength(self,  (pos < tlen) ? 1 : 0);
}


	void
nessmark__SetFrom(self, src)
	register struct nessmark *self, *src;
{
	nessmark_Set(self, nessmark_GetText(src), 
			nessmark_GetPos(src), 
			nessmark_GetLength(src));
}

	unsigned char *
nessmark__ToC(self)
	struct nessmark *self;
{
	register struct simpletext *t ;
	register long loc, len;
	register unsigned char *bx;
	unsigned char *buf;

	t = nessmark_GetText(self);
	len = nessmark_GetLength(self);
	buf = (unsigned char *)malloc(len + 1);
	bx = buf;
	/* XXX really should use simpletext_GetBuffer()  */
	for (loc = nessmark_GetPos(self);  len > 0;  len--, loc++)
		*bx++ = simpletext_CorrectGetChar(t, loc);
	*bx = '\0';
	return buf;
}

