/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/gen.c,v 1.23 1991/09/24 18:34:39 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/gen.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/gen.c,v 1.23 1991/09/24 18:34:39 wjh Exp $";
#endif

/* 
	gen.c  -  generate interpreter code for ness

*/
/*
 * $Log: gen.c,v $
 * Revision 1.23  1991/09/24  18:34:39  wjh
 * make BackSlashReduce non-static
 * .c inside freeze
 * .
 *
 * Revision 1.22  1991/09/12  16:25:30  bobg
 * Update copyright notice and rcsid
 *
 * Revision 1.21  1990/11/28  14:38:29  susan
 * moved andrewos.h to be first include file
 *
 * Revision 1.20  90/10/12  21:08:28  wjh
 * revised yaccpar to eliminate saving parser state.
 * 	moved yycompile to gen.c
 * 	eliminated ErrorList
 * 	made the argument to MapRunError be a ness instead of an ErrorList
 * 
 * 
 * Revision 1.19  90/07/15  15:23:55  wjh
 * call.c:  fix a coreleak in class()
 * ness.ch: defined CURRENTMODIFICATIONLEVEL
 * nessruna.c: set version number as 
 * 	CURRENTSYNTAXLEVEL.CURRENTMODIFICATIONLEVEL
 * nessruna.c: add -f switch to cause a fork after compile and before exec
 * Imakefile:  switch to using nessrunapp instead of nessrun.ci
 * nessfunc.d: document the new proctable entries in framecmd.c
 * nessauth.d: describe usage of the framecmd stuff.
 * nesssauth.d: describe --$syntaxlevel
 * nessauth.d: give example of a long string
 * 
 * search.c: redo nextstylegroup
 * search.c: addstyles will now complain if the argument is a constant
 * gen.c: fix so it will not crash after allocating all the initial sysmarks
 * 		that is: it won't crash after a few compiles
 * search.c: revise HasStyles to check for name equality as well as style location equality
 * search.c: add nextstylesegment() which gives the segment from end of subject to next style change
 * search.c: fix enclosingstyle, nextstylegroup, and nextstylesegment to know about 
 * 	the bogus length values used in the outermost environment
 * 
 * 
 * Revision 1.18  89/11/06  15:01:40  cfe
 * typo: disasterous is a misspelling.
 * 
 * Revision 1.17  89/10/30  13:42:26  wjh
 * added #include for andrewos to define bzero
 * 
 * Revision 1.16  89/10/11  09:24:17  wjh
 * fixed a bug that generated a spurious empty error message for a missing 'end'
 * 
 * Revision 1.15  89/09/17  08:51:56  wjh
 * to get scopes right for the global initialization function genSaveFunc 
 * and genRestoreFunc have to dinker scopes
 * 
 * Revision 1.14  89/09/10  17:51:05  wjh
 * change method of defining key and mouse bindings for views
 * 
 * Revision 1.13  89/09/03  22:47:32  wjh
 * newness
 * 
 * Revision 1.12  89/06/23  17:19:51  wjh
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
 * Revision 1.11  89/06/01  15:59:36  wjh
 * campus release version
 * 
 * Revision 1.6  89/01/04  10:54:06  wjh
 * fixed various core dump bugs
 * 
 * Revision 1.5  88/12/07  22:42:42  wjh
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
 * Revision 1.4  88/11/23  14:44:04  wjh
 * proc table calls now work correctly with type-free procs  (the first arg can be anything)
 * added "cheat_" functions which will remain undocumented
 * changed inset() to a function
 * fixed some bugs in initial access
 * 
 * Revision 1.3  88/11/16  11:59:43  wjh
 * 
 * implemented access level control
 * skip first line of script if starts with # (for shell script)
 * changed so the name of all initialization functions is init()
 * added ness-load
 * moved execution of init() from compilation to first execution
 * deferred compilation to first FullUpdate time
 * 
 * Revision 1.2  88/11/02  14:41:20  wjh
 * fixed bugs with byte swapping
 * added some more corrected copyrights
 * 
 * Revision 1.1  88/10/21  10:57:42  wjh
 * Initial revision
 * 
 * Creation 0.0  88/03/29 10:25:00  wjh
 * Initial creation by WJHansen
 * 
*/
#include <andrewos.h>	/* for bzero() */
#include <ctype.h>

#include <class.h>
#include <text.ih>

#include <nodeclss.h>
#include <nessmark.ih>
#include <interp.h>
#include <compdefs.h>
#include <envt.h>
#include <lexdef.ih>
#include <lex.ih>
#include <gen.h>
#include <y.tab.h>
#include <error.h>
#include <call.h>
#include <nesssym.ih>
#include <ness.ih>
#include <toksym.ih>

int yyparse();	/* in nesscomp.{gra,y,c} */

/* defining instances for declarations in gen.hn */
struct lex *curLex = NULL;		/* points to the lex used for grammar */
struct ness *curNess = NULL;		/* the current ness */
boolean predcond = FALSE;		/* condition under which to branch to target */

/* defining instance for declaration in compdefs.hn */
char *TypeName[/* Texpr */] = {
	"list-end", 
	"integer", 
	"boolean", 
	"boolean", 
	"real", 
	"marker", 
	"object", 
	"function", 
	"void",
	"unknown",
	"Cstring",
	"short",
	"char"
};

/* the order of operators in Xoptbl is EQ, NE, LT, GE, GT, LE, BR, NOP
		BR is used for predT and predAND
		NOP is used for predF and predOR
	the FopTbl has the opposite operation from the TopTbl */
static unsigned char TopTbl[] = {'e', 'f', 'a', 'd', 'b', 'c', 'g', '\n'};
static unsigned char FopTbl[] = {'f', 'e', 'd', 'a', 'c', 'b', '\n', 'g'};


/* these two are not pushed/popped;  they retain their own state */
static struct predstatenode predstack[MAXEXPRDEPTH];
static struct predstatenode *pssp = predstack - 1;   /* points to the in-use elt of predstate stack */


/* the following state variables are pushed/popped for recursive compilation */

static long scopeDepth = 0;			/* number of used elts in saveScope */
static nesssym_scopeType saveScope[3];	/* save scopes surrounding function */

static struct nessmark *curfuncmark;	/* marker for function currently being generated */

static struct gotonode *predtargetlist = NULL,	/* branchlist of gotos when predcond is met */
		*preddropthrulist = NULL;	/* branchlist of gotos that are supposed to drop thru */

static struct fixupstackrefnode *varfixups = NULL;
static struct fixupstackrefnode *freefixups = NULL;

static struct nessmark *savedCurfuncmark;
static struct fixupstackrefnode *savedVarFixups;

static boolean Locating = FALSE;	/* TRUE if searching for obj loc */
static long Sought;			/* what object offset are we seeking */
static boolean FoundIt = FALSE;	/* TRUE if search satisfied */
static long FoundLoc, FoundLen;	/* where we found it */
static long CurrOffset;		/* where we are in object code */
static boolean LocationAdvancing;	/* toggles depending on whether in InitFunc */
static long StmtStart;		/* where the most recent statement starts in source */

#define ENTERARGOFFSET 1	/* offset from start of object for function
				to the operand that must be fixed */

	struct errornode *
yycompile(ness)
	struct ness *ness;
{
	genPush();

	ness->AttrDest = &ness->globals;	/* where to put defined symbols */
	curNess = ness;
	SetupErrorHandling(ness);
	curLex = ness_GetLex(ness);
	lex_SetCurrent(curLex);
	ness->CurrentObject = NULL;

	yyparse();

	genPop();
	return ness->ErrorList;
}

static struct genState {
	long scopeDepth;
	nesssym_scopeType saveScope[3];
	struct nessmark *curfuncmark;
	boolean predcond;
	struct gotonode *predtargetlist, *preddropthrulist;
	struct fixupstackrefnode *varfixups, *freefixups;
	struct nessmark *savedCurfuncmark;
	struct fixupstackrefnode *savedVarFixups;
	boolean Locating, LocationAdvancing;
	long Sought, CurrOffset, StmtStart;
	struct ness *curNess;
	struct genState *next;
} *genStack = NULL;

	void
genPush()
{
	struct genState *T;
	T = (struct genState *)malloc(sizeof (struct genState));

	T->scopeDepth = scopeDepth;
	T->saveScope[0] = saveScope[0];
	T->saveScope[1] = saveScope[1];
	T->saveScope[2] = saveScope[2];
	T->curfuncmark = curfuncmark;
	T->predcond = predcond;
	T->predtargetlist = predtargetlist;
	T->preddropthrulist = preddropthrulist;
	T->varfixups = varfixups;
	T->freefixups = freefixups;
	T->savedCurfuncmark = savedCurfuncmark;
	T->savedVarFixups = savedVarFixups;
	T->Locating = Locating;
	T->LocationAdvancing = LocationAdvancing;
	T->Sought = Sought;
	T->CurrOffset = CurrOffset;
	T->StmtStart = StmtStart;
	T->curNess = curNess;

	T->next = genStack;
	genStack = T;
}

	void
genPop()
{
	struct genState *T = genStack;

	scopeDepth = T->scopeDepth;
	saveScope[0] = T->saveScope[0];
	saveScope[1] = T->saveScope[1];
	saveScope[2] = T->saveScope[2];
	curfuncmark = T->curfuncmark;
	predcond = T->predcond;
	predtargetlist = T->predtargetlist;
	preddropthrulist = T->preddropthrulist;
	varfixups = T->varfixups;
	freefixups = T->freefixups;
	savedCurfuncmark = T->savedCurfuncmark;
	savedVarFixups = T->savedVarFixups;
	Locating = T->Locating;
	LocationAdvancing = T->LocationAdvancing;
	Sought = T->Sought;
	CurrOffset = T->CurrOffset;
	StmtStart = T->StmtStart;
	curNess = T->curNess;
	if (curNess != NULL) {
		curLex = ness_GetLex(curNess);
		lex_SetCurrent(curLex);
	}

	genStack = T->next;
	free(T);
}



/* genNewScope()
	pushes the current scope and makes a new one
	returns the new scope 
*/
	nesssym_scopeType
genNewScope()
{
	nesssym_scopeType newscope;
	if (scopeDepth >= 3) {
		ReportError(":disastrous compile error: scope stack overflow", 0);
		return saveScope[2];	/* XXX compiler errror */
	}
	saveScope[scopeDepth++] = lex_GetScope(curLex);
	newscope = nesssym_NNewScope(lex_GetScope(curLex));

	lex_SetScope(curLex, newscope);
	return newscope;
}
 /* genPopScope()
	revert to a prior scope
*/
	void
genPopScope()
{
	if (scopeDepth == 0) return;
	scopeDepth--;
	lex_SetScope(curLex, saveScope[scopeDepth]);
}
 /* genPushScope(scope)
	enter a scope 
	(used for the global initialization function)
*/
	void
genPushScope(scope)
	nesssym_scopeType scope;
{
	if (scopeDepth >= 3) {
		ReportError(":disastrous compile error: scope stack overflow", 0);
		return;
	}
	saveScope[scopeDepth++] = lex_GetScope(curLex);
	lex_SetScope(curLex, scope);
}



/* TryLocate(bytes)
	Test to see if we have found the offset we wanted. 
	Update the notion of what the current offset is.
*/
	static
TryLocate(bytes)
	long bytes;
{
	if ( ! LocationAdvancing)
		return 0;
	if (! FoundIt && CurrOffset >= Sought) {
		long loc, len;
		FoundIt = TRUE;
		loc = lex_RecentPosition(0, &len); 
		FoundLoc = (pssp >= predstack && pssp->loc > StmtStart)
			? pssp->loc : StmtStart;
		FoundLen = loc+len - FoundLoc;
	}
	CurrOffset += bytes;
	return 0;
}

/*
   LocationAdvancing toggles back and forth in genSaveFuncState and genRestoreFuncState
   If it is true, the offset advances in TryLocate.  Otherwise not.
   Initially LocationAdvancing is True iff the location we want is within the InitFunc.
*/

/* genStartLocating(objloc)
	The next compilation will not generate object code, but will find the 
	source location corresponding to 'objloc'.
	If LocateInInitFunc is True, the desired code is in InitFunc.
	Call genFinishLocating for the result.
*/
	void
genStartLocating(objloc, LocateInInitFunc)
	unsigned long objloc;
	boolean LocateInInitFunc;
{
	Locating = TRUE;
	Sought = objloc;
	FoundIt = FALSE;
	CurrOffset = 0;
	StmtStart = 0;
	LocationAdvancing = LocateInInitFunc;
}
/* genSaveStmtStart(tokeninx)
	save location of statment start in source for errors 
*/
	void
genSaveStmtStart(tokeninx)
	long tokeninx;
{
	if (Locating) StmtStart = lex_RecentPosition(tokeninx, 0); 
}

/* genFinishLocating(Ploc, Plen)
	return the loc and len found on most recent compile
	return value indicates success
		it will be false if not found or if no compile
*/
	boolean
genFinishLocating(Ploc, Plen)
	unsigned long *Ploc, *Plen;
{
	TryLocate(0);
	Locating = FALSE;
	*Ploc = FoundLoc;
	*Plen = FoundLen;
	return FoundIt;
}


/* allocSysMark(erroff)
	allocate an element of the SysMark array
	if space exhausted, used 'erroff' to identify the error token 

	if we ever extend SysMark, we must not go over 65535 entries
		because there are only two bytes for addressing in operands
*/
	static struct nessmark *
allocSysMark(erroff)
	long erroff;
{
	struct nessmark * v;
	v = SysMarkFree;

	if (v == NULL && SysMarkSize < 65535) {
		/* allocate more space for sysmark array */
		long i;
		long oldsize = SysMarkSize;
		SysMarkSize += 1000;
		if (SysMarkSize > 65535) SysMarkSize = 65535;
		SysMarkLowEnd = (struct nessmark *)
			realloc(SysMarkLowEnd, SysMarkSize * sizeof(struct nessmark));
		bzero(SysMarkLowEnd+oldsize * sizeof(struct nessmark), 
			(SysMarkSize - oldsize) * sizeof(struct nessmark));
		SysMarkHiEnd = SysMarkLowEnd + SysMarkSize;
		for (i = oldsize; i < SysMarkSize; i++)
			nessmark_SetNext(SysMarkLowEnd+i, 
				(struct mark *)(SysMarkLowEnd+i+1));
		nessmark_SetNext(SysMarkHiEnd-1, NULL);
		SysMarkFree = SysMarkLowEnd + oldsize;
		v = SysMarkFree;
	}
	else if (v == NULL) {
		/* XXX need to realloc SysMark */
		ReportError(":Global storage exceeded", erroff);
		Locating = FoundIt = TRUE;  /* KLUDGE to turn off compilation */
		return NULL;
	}
	SysMarkFree = (struct nessmark *)nessmark_GetNext(v);
	return v;
}


/* makeFunction(loc)
	returns a pointer to a mark for space to store a function in ObjectCode
	sets 'loc' to the index of the function for use in calls
	
	adds a dummy RETURN after the function to separate its marks 
	from that of the next function
*/
	struct nessmark *
makeFunction(loc) 
	TSysMarkRef *loc;
{
	long endpos;
	struct nessmark *v = allocSysMark(-1);
	if (Locating) {*loc = 0; return NULL;}
	if (v == NULL) return NULL;

	nessmark_Initialize(v);
	endpos = simpletext_GetLength(ObjectCode);
	simpletext_InsertCharacters(ObjectCode, endpos, "Q", 1);
	nessmark_Set(v, ObjectCode, endpos, 0);
	*loc = v - SysMarkLowEnd;
	return v;	
}

/* makeConst(s)
	make a Ness constant for C string s
	returns an index into SysMark
*/
	TSysMarkRef
makeConst(s)
	char *s;
{
	struct nessmark *v = allocSysMark(0);

	if (Locating) return NULL;
	if (v == NULL) return 0;

	nessmark_Initialize(v);
	nessmark_MakeConst(v, s);
	return (v - SysMarkLowEnd);
}


/*BackSlashReduce(text)
	reduce any backslash constructs from the text
	if text ends with backslash, it will be deleted
	return length of resulting text
*/
	long
BackSlashReduce(text)
	register struct text *text;
{
	static unsigned char esctab[] = 
			"r\rn\nf\ft\tb\bv\ve\033";
	register long pos, len;
	register unsigned char c;
	register unsigned char *ex;
	len = text_GetLength(text);

	for (pos = 0; pos < len; ) {
		c = text_GetChar(text, pos);
		if (c == '^') {
			/* convert ^-uppercase to control char */
			c = text_GetChar(text, pos+1);
			if (c >= ('A' & ~0x1F)  && c <= ('A' | 0x1F)) {
				unsigned char x = c - ('A' & ~0xF);
				text_DeleteCharacters(text, pos, 2);
				text_InsertCharacters(text, pos, &x, 1);
				len --;
			}
			pos ++;	/*  leave the  ^  or  the new control char */
			continue;
		}
		if (c != '\\') {
			pos ++;
			continue;
		}
		/* get character after the backslash */
		c = text_GetChar(text, pos+1);
		for (ex = esctab; *ex && *ex != c; ex +=2) {}
		if (*ex) {
			/* recognized reduction */
			text_DeleteCharacters(text, pos, 2);
			text_InsertCharacters(text, pos, ex+1, 1);
			len --;  pos ++;	/* ignore replacement */
		}
		else if (isdigit(c)) {
			/* parse digit string */
			char newc;
			long ndigs;
			long tpos;
			tpos = pos+1;	/* point at first digit */
			ndigs = 0;	/* count the backslash */
			newc = 0;
			while (isdigit(c = text_GetChar(text, tpos))) {
				newc = 8 * newc + (c - '0');
				tpos ++;  	/* advance to next char */
				ndigs ++;	/* count each digit */
			}
			text_DeleteCharacters(text, pos, ndigs+1);
			text_InsertCharacters(text, pos, &newc, 1);
			pos ++;
			len -= ndigs;
		}
		else {
			/* One of    \"  \'  \\   or some unknown \ escape  
				delete \ and skip next char */
			text_DeleteCharacters(text, pos, 1);
			pos ++;  len --;
		}
	}
	return len;
}


/* makeStlyedConst(text, pos, len)
	make a Ness constant from the string in 'text' of length 'len'
		starting at 'pos'
	returns an index into SysMark
*/
	TSysMarkRef
makeStyledConst(text, pos, len, bsReduce)
	struct text *text;
	long pos, len;
	boolean bsReduce;
{
	struct nessmark *v;
	struct text *newt;

	if (Locating) return NULL;
	if (len == 0)
		return makeConst("");

	v = allocSysMark(0);
	if (v == NULL) return 0;

	nessmark_Initialize(v);
	newt = text_New();
	text_AlwaysCopyText(newt, 0, text, pos, len);
	if (bsReduce) 
		len = BackSlashReduce(newt);
	text_SetReadOnly(newt, TRUE);
	nessmark_Set(v, newt, 0, len);
	return (v - SysMarkLowEnd);
}

/* makeGlobal()
	make a Ness global variable
	returns an index into SysMark
*/
	TSysMarkRef
makeGlobal()
{
	struct nessmark *v = allocSysMark(0);

	if (Locating) return NULL;
	if (v == NULL)  return 0;

	v->header.nessmark_methods = NULL;
	return (v - SysMarkLowEnd);
}


/* addOp(m, op);
	appends operator 'op' to 'm'
*/
	void
addOp(m, op)
	struct nessmark *m;
	unsigned char op;
{
	long pos, len;
	if (Locating) {TryLocate(1); return;}
	pos = nessmark_GetPos(m);
	len = nessmark_GetLength(m);
	simpletext_InsertCharacters(nessmark_GetText(m), pos+len, &op, 1);
	nessmark_SetLength(m, len+1);
}

/* refStack(m, op, rand)
	append to m the opcode op with the one-byte operand rand 
	return location of the operand as an arg to RememberFixup

	the case of rand>0xFF is tested for in finishfunc 
*/
	TCodeRef
refStack(m, op, rand)
	struct nessmark *m;
	unsigned char op;
	TStackRef rand;
{
	long pos, len;
	unsigned char s[2];
	if (Locating) return TryLocate(2);
	pos = nessmark_GetPos(m);
	len = nessmark_GetLength(m);
	s[0] = op;
	s[1] = rand & 0xFF;
	simpletext_InsertCharacters(nessmark_GetText(m), pos+len, s, 2);
	nessmark_SetLength(m, len+2);
	return (len+1);
}

	void
fixRefStack(m, loc, val)
	struct nessmark *m;
	TCodeRef loc;
	TStackRef val;
{
	if (Locating)  {TryLocate(1); return;}
	simpletext_ReplaceCharacters(nessmark_GetText(m), nessmark_GetPos(m)+loc,
			sizeof(TStackRef), &val, sizeof(TStackRef));
}

/* refSysMark(m, op, rand)
	append to m the opcode op with the two-byte operand rand
	return location in code of the operand field

	overflow of the address field is covered in the MakeXxx functions above, 
	which do not allocate more than 65535 SysMarks
*/
	TCodeRef
refSysMark(m, op, rand)
	struct nessmark *m;
	unsigned char op;
	TSysMarkRef rand;
{
	long pos, len;
	unsigned char s[3];
	if (Locating) return TryLocate(3);
	pos = nessmark_GetPos(m);
	len = nessmark_GetLength(m);
	s[0] = op;
	s[1] = rand>>8;
	s[2] = rand & 0xFF;
	simpletext_InsertCharacters(nessmark_GetText(m), pos+len, s, 3);
	nessmark_SetLength(m, len+3);
	return len+1;
}

	void
fixRefSysMark(m, loc, val)
	struct nessmark *m;
	TCodeRef loc;
	TSysMarkRef val;
{
	if (Locating)  {TryLocate(1); return;}
	simpletext_ReplaceCharacters(nessmark_GetText(m), nessmark_GetPos(m)+loc,
			sizeof(TSysMarkRef), &val, sizeof(TSysMarkRef));
}
/* refAddress(m, op, address)
	append to m the opcode op with the four-byte operand rand 
*/
	void
refAddress(m, op, address)
	struct nessmark *m;
	unsigned char op;
	struct object *address;
{
	long pos, len;
	unsigned char s[5];
	if (Locating)  {TryLocate(5); return;}
	s[0] = op;
	s[1] = ((unsigned long)address)>>24;
	s[2] = ((unsigned long)address)>>16;
	s[3] = ((unsigned long)address)>>8;
	s[4] = ((unsigned long)address) & 0xFF;
	pos = nessmark_GetPos(m);
	len = nessmark_GetLength(m);
	simpletext_InsertCharacters(nessmark_GetText(m), pos+len, s, 5);
	nessmark_SetLength(m, len+5);
}

/* genLinkGlobal(sym)
	attach sym to the global list given by *curNess->AttrDest
*/
	void
genLinkGlobal(sym)
	struct nesssym *sym;
{
	if (Locating) return;
	sym->next = *curNess->AttrDest;
	*curNess->AttrDest = sym;
}


/* genLocStore(sym)
	store the code location in codelocTable
*/
	void
genLocStore(sym)
	struct nesssym *sym;
{
	if (Locating) return;
	codelocStore(sym);
}


/* genInit(lex)
	record the lex as the one used for compilations
	define builtin functions
*/
	void
genInit(def)
	register struct lexdef *def;
{
	struct toksym *tok, *ttok;
	struct nesssym *nesssymproto = nesssym_New();
	nesssym_scopeType Gscope = lexdef_GetGrammarScope(def);
	callInit(nesssymproto, def);
	neventInit();

	/* set up to combine parens */

	tok = toksym_TFind("(", Gscope);
	ttok = toksym_TFind("{", Gscope);
	ttok->toknum = tok->toknum;	/* { will parse like ( */
	ttok = toksym_TFind("[", Gscope);
	ttok->toknum = tok->toknum;	/* [ will parse like ( */

	tok = toksym_TFind(")", Gscope);
	ttok = toksym_TFind("}", Gscope);
	ttok->toknum = tok->toknum;	/* } will parse like ) */
	ttok = toksym_TFind("]", Gscope);
	ttok->toknum = tok->toknum;	/* ] will parse like ) */
}


	static void
RememberFixup(refloc, sym)
	TCodeRef refloc;
	struct nesssym *sym;
{
	struct fixupstackrefnode *f;
	if (freefixups != NULL) {
		f = freefixups;
		freefixups= f->next;
		f->next = varfixups;
		f->refloc = refloc;
		f->sym = sym;
		varfixups = f;
	}
	else
		varfixups = fixupstackrefnode_Create(refloc, sym, varfixups);
}
	static void
DoFixups(lpsize)
	unsigned long lpsize;
{
	struct fixupstackrefnode *f, *t;
	for (f = varfixups; f != NULL; f = t) {
		fixRefStack(curfuncmark, f->refloc, 
			(f->sym == NULL) ? lpsize : nesssym_NGetInfo(f->sym, long));
		t = f->next;
		f->next = freefixups;
		freefixups = f;
	}
	varfixups = NULL;
}

/* allocate (syml, prior)
	set to the proper stack offset the 
		nesssym_GetInfo(s, long) of each s that is a symbol on the list at 'syml'
	begin with offset 'prior'
	Return total size allocated, including 'prior'

	Note that the list of parameters is in reverse order, but that this
	means they are in the order of increasing positive offset from the 
	frame header, which is at a higher location and lower address in the stack.
*/
	static unsigned long 
allocate(syml, prior)
	register struct nesssym *syml;
	register unsigned long prior;
{
	for ( ; syml != NULL; syml = syml->next) {
		nesssym_NSetInfo(syml, long, prior);
		if (syml->type == Tbool)
			prior += sizeof(struct boolstkelt);
		else if (syml->type == Tlong)
			prior += sizeof(struct longstkelt);
		else if (syml->type == Tdbl)
			prior += sizeof(struct dblstkelt);
		else if (syml->type == Tptr)
			prior += sizeof(struct ptrstkelt);
		else
			prior += sizeof(struct nessmark);
	}
	return prior;
}

	
	TSysMarkRef
genEnter()
{
	TSysMarkRef loc;
	varfixups = NULL;
	curfuncmark = makeFunction(&loc);
	/* generate: ENTER (sizeof locals)   (check return value in case the offset changes) */
	if (refStack(curfuncmark, 'P', 0) != ENTERARGOFFSET) 
		ReportError(":URP!", -1);
	return loc;
}

	void
genreturn(hasExpr)
	boolean hasExpr;
{
	if ( ! hasExpr)
		addOp(curfuncmark, '0');	/* provide zero as the default value */
	RememberFixup(refStack(curfuncmark, 'Q', 0), NULL);
}

/* genExit(parmlist, locallist)
	generates code at end of function and does fixups
	returns TRUE if offsets are less than 255 and FALSE otherwise
*/
	boolean
genExit(parmlist, locallist)
	struct nesssym *parmlist, *locallist;
{
	long locsize, totsize;
	locsize = allocate(locallist, 0);
	fixRefStack(curfuncmark, ENTERARGOFFSET, locsize);
	genreturn(FALSE);
	totsize=allocate(parmlist, locsize);
	DoFixups(totsize);
	return (totsize <= 255);
}

	void
startfunc(fname)
	struct nesssym *fname;
{
	nesssym_scopeType oldscope = lex_GetScope(curLex); /* save before changed
					by genNewScope() */
	nesssym_NSetINode(fname, funcnode, 
			funcnode_Create(genEnter(), oldscope, genNewScope(), 
				lex_RecentPosition(-1, 0), 999, NULL, NULL, Tstr));
}

	void
finishfunc(fname, locallist)
	struct nesssym *fname, *locallist;
{
	long lastloc, lastlen, srclen, srcloc;
	struct funcnode *func = nesssym_NGetINode(fname, funcnode);

	srcloc = func->srcloc;
	lastloc = lex_RecentPosition(0, &lastlen);
	srclen = lastloc + lastlen - srcloc;
	func->srclen = srclen;
	func->locallist = locallist;
	if ( ! genExit(func->parmlist, locallist))
		SaveError(":More than 10 arguments and locals", srcloc, srclen);
	genPopScope();
	genLocStore(fname);   genLinkGlobal(fname);
}

	void
abortfunc()
{
	/* XXX  gets wrong scope if inside EXTEND...END, but outside a function */
	genPopScope();
}


/* appendlists(list1, list2)
	Concatenate the symbol lists 'list1' and 'list2'
	The lists are in _reverse_ order, so put 'list2' before 'list1'.
*/
	struct nesssym *
appendlists(list1, list2)
	register struct nesssym *list1, *list2;
{
	register struct nesssym *lastElt, *t;
	if (list2 == NULL) return list1;
	if (list1 == NULL) return list2;

	lastElt = list2;
	while ((t=lastElt->next) != NULL)
		lastElt = t;
	lastElt->next = list1;	
	return list2;
}

	struct exprnode *
genarith(op, left, right)
	unsigned char op;
	struct exprnode *left, *right;
{
	struct exprnode *r;
	if (left->type == Tdbl) {
		demandnodetype(right, Tdbl);
		r = exprnode_Combine(left, right, Tdbl);
		if (op == '%') {
			ExprError(":% is not defined for real values", r);
			return r;
		}
		genop('M');
	}
	else {
		demandnodetype(left, Tlong);
		demandnodetype(right, Tlong);
		r = exprnode_Combine(left, right, Tlong);
	}
	genop(op);
	return r;
}

	void
gencomp(left, right)
	struct exprnode *left, *right;
{
	if (left->type != right->type) 
		SaveError(":comparison of different types", left->loc,
				right->loc+right->len - left->loc);
	else if (left->type == Tstr)
		{}
	else if (left->type == Tbool) 
		addOp(curfuncmark, 'B');
	else if (left->type == Tptr) 
		addOp(curfuncmark, 'V');
	else if (left->type == Tlong) 
		addOp(curfuncmark, 'I');
	else if (left->type == Tdbl) 
		addOp(curfuncmark, 'R');
	else 
		SaveError(":comparison is not implemented for this type",
				left->loc, right->loc+right->len - left->loc);
	addOp(curfuncmark, 't');
}

	void
genvarref(var)
	struct nesssym *var;
{
	if ((var->flags & flag_undef) != 0) 
		ReportError(":refers to undefined var", -1);
	else if (var->flags & flag_builtin)
		/* pre-defined identifier */
		callPredefId(var);
	else {
		if (var->type == Tbool)
			addOp(curfuncmark, 'B');
		else if (var->type == Tptr)
			addOp(curfuncmark, 'V');
		else if (var->type == Tlong)
			addOp(curfuncmark, 'I');
		else if (var->type == Tdbl)
			addOp(curfuncmark, 'R');
		else if (var->type != Tstr)
			ReportError(":cannot load variables of this type", -1);
		if (var->flags & flag_globalvar)
			refSysMark(curfuncmark, 'k', nesssym_NGetInfo(var, long));
		else
			RememberFixup(refStack(curfuncmark, 'l', 0), var);
	}
}
	void
genvarstore(varnode)
	struct varnode *varnode;
{
	struct nesssym *var = varnode->sym;
	if (var->type == Tbool)
		addOp(curfuncmark, 'B');
	else if (var->type == Tptr)
			addOp(curfuncmark, 'V');
	else if (var->type == Tlong)
			addOp(curfuncmark, 'I');
	else if (var->type == Tdbl)
			addOp(curfuncmark, 'R');
	else if (var->type != Tstr)
		SaveError(":cannot store into variables of this type", 
					varnode->loc, varnode->len);
	if (var->flags & flag_globalvar)
		refSysMark(curfuncmark, 'v', nesssym_NGetInfo(var, long));
	else
		RememberFixup(refStack(curfuncmark, 's', 0), var);
	varnode_Destroy(varnode);
}

	struct varnode *
genvarnode(sym)
	struct nesssym *sym;
{
	long loc, len;
	if ((sym->flags & (flag_function | flag_var)) == 0)
		sym->flags |= flag_undef;
	loc = lex_RecentPosition(0, &len);
	return varnode_Create(loc, len, sym, NULL);
}

	void
genconstref(constant)
	struct nesssym *constant;
{
		if (constant->type == Tlong)
			addOp(curfuncmark, 'I');
		else if (constant->type == Tdbl)
			addOp(curfuncmark, 'R');
		else if (constant->type != Tstr)
			ReportError(":cannot load constants of this type", 0);
	refSysMark(curfuncmark, 'k', nesssym_NGetInfo(constant, long));
}

	void
genop(op)
	unsigned char op;
{
	addOp(curfuncmark, op);
}


/* = = = = = = = = = = = = = = = = = = =
 *    Branch Instructions
 * = = = = = = = = = = = = = = = = = = */
/* setcurfuncmark(m)
	set 'curfuncmark' to 'm'
	Use This ONLY for Testing 
	and nevent
*/
	void
setcurfuncmark(m)
	struct nessmark *m;
{
	curfuncmark = m;
}
/* getcurfuncmark()
	return current value of curfuncmark
	used ONLY by call.c
*/
	struct nessmark *
getcurfuncmark()
{
	return curfuncmark;
}





/*
	The operand for a branch opcode is the offset from the opcode to the target.
*/

/* genlabel()
	returns the current location in the object code 
*/
	long
genlabel()
{
	if (Locating) return 0;
	return nessmark_GetLength(curfuncmark);
}

/* genbranch(op, dest)
	emits to the object code a branch with opcode 'op' to location 'dest'
	returns the location of the opcode of the branch
*/
	long
genbranch(op, dest)
	unsigned char op;
	long dest;
{
	long branchloc = genlabel();
	unsigned char buf[3];
	short t = dest - branchloc;	/* compute offset */
	long len;

	if (Locating) return  TryLocate(3);
	len = nessmark_GetLength(curfuncmark);

	buf[0] = op;
	buf[1] = ((unsigned short) t) >> 8;
	buf[2] = ((unsigned short) t) & 0xFF;
	simpletext_InsertCharacters(nessmark_GetText(curfuncmark),
			nessmark_GetPos(curfuncmark)+branchloc, buf, 3);
	nessmark_SetLength(curfuncmark, len+3);	
	return branchloc;
}

/* fixbranch(loc)
	fixes up the branch op at 'loc' to branch to next generated opcode
*/
	void
fixbranch(loc)
	long loc;
{
	long dest = genlabel();
	short t = dest - loc;		/* compute offset */
	unsigned char buf[2];
	if (Locating) return;
	buf[0] = ((unsigned short) t) >> 8;
	buf[1] = ((unsigned short) t) & 0xFF;
	simpletext_ReplaceCharacters(nessmark_GetText(curfuncmark), 
			nessmark_GetPos(curfuncmark)+loc+1, 2, buf, 2);
}



/* = = = = = = = = = = = = = = = = = = =
 *    Predicate Engine
 * = = = = = = = = = = = = = = = = = = */

/* state variables for predicate engine

	The predicate state is saved in a predstatenode having
		fixuploc - fixup loc (for ELIF)
		objloc - current location in object code (for WHILE)
		loc - the beginning of an expression (for TryLocate)
		targetlist - the prior predtargetlist
		dropthrulist - the prior preddropthrulist 
		cond - the prior predcond value 
		construct - a code saying what construct this is in  (see compdefs.hn)
*/

/* predpush(cond, loc, construct)
	Package the current predicate state in a predstatenode and 
	initialize for a new predicate, setting predcond from 'cond'
	save loc in predstack for TryLocate
*/
	struct predstatenode *
predpush(cond, loc, construct)
	boolean cond;
	long loc;
	char construct;
{
	if (pssp - predstack >= MAXEXPRDEPTH - 1) {
		ReportError(":expression is too complicated", 0);
		return NULL;
	}
	pssp ++;
	pssp->fixuploc = 0;
	pssp->objloc = genlabel();
	pssp->loc = loc;
	pssp->targetlist = predtargetlist;
	pssp->dropthrulist = preddropthrulist;
	pssp->cond = predcond;
	pssp->construct = construct;

	/* establish new conditions */
	predtargetlist = NULL;
	preddropthrulist = NULL;
	predcond = cond;		/* store new cond */
	return pssp;
}

	static void
appendgotolists(list, addon)
	struct gotonode **list, *addon;
{
	if (*list == NULL) *list = addon;
	else if (addon == NULL) {}
	else {
		register struct gotonode *t = *list;
		while (t->next != NULL) t = t->next;
		t->next = addon;
	}
}

/* predpop()
	restore the state of the predicate engine from the fields of 'state'
*/
	void
predpop()
{
	if (pssp < predstack) {
		ReportError(":COMPILER ERROR: expression stack underflow", 0);
		return;
	}
	predcond = pssp->cond;
	appendgotolists(&predtargetlist, pssp->targetlist);
	appendgotolists(&preddropthrulist, pssp->dropthrulist);
	pssp--;
}

/* predvalue(Ptype)
	Emit code to generate a value from a branching predicate
*/
	void
predvalue(Ptype)
	Texpr *Ptype;
{
	long branchloc;

	if (*Ptype != Tbra) 
		return;		/* it is already a value */
	/* convert branching to a value */
	predfixdropthru();
	addOp(curfuncmark, '1');		/* load TRUE */
	branchloc = genbranch('g', 0);
	predfixtarget();
	addOp(curfuncmark, '9');		/* load FALSE */
	fixbranch(branchloc);
	*Ptype = Tbool;
}
/* predbranch(expr)
	Emit code to convert boolean value to branching
*/
	void
predbranch(expr)
	struct exprnode *expr;
{
	if (expr->type == Tbra) 
		return;		/* it is already a branching predicate */
	else if (expr->type != Tbool) {
		ExprError(":expression should be of type Boolean", expr);
		return;
	}
	addOp(curfuncmark, 'T');	/* generate a comparison */
	predtarget(predEQ);	/* cmp to TRUE and branch */
}

/*
	An element of preddropthrulist or predtargetlist is a gotonode having:
		gotoloc - the location to be patched
		next - the next element on the list
*/
/* preddropthru(rop)
	Generates a branch and put a fixup for it on 'preddropthru' list
	opcode depends on predcond: 'topTbl[rop]' for TRUE, 'fopTbl[rop]' for FALSE
*/
	void
preddropthru(rop)
	long rop;
{
	unsigned char opcode = (predcond) ? TopTbl[rop] : FopTbl[rop];
	if (opcode == '\n')  return;
	preddropthrulist = gotonode_Create(genbranch(opcode, 0), preddropthrulist);
}

/* predtarget(rop)
	Generates a branch and put a fixup for it on 'predtarget' list
	opcode depends on predcond: 'topTbl[rop]' for TRUE, 'fopTbl[rop]' for FALSE
*/
	void
predtarget(rop)
	long rop;
{
	unsigned char opcode = (predcond) ? TopTbl[rop] : FopTbl[rop];
	if (opcode == '\n')  return;
	predtargetlist = gotonode_Create(genbranch(opcode, 0), predtargetlist);
}


/* fixbranchlist(list)
	generate fixups for all locations listed on 'list'
	cause them to branch to next opcode generated
	Discard list elements.
*/
	static void
fixbranchlist(list)
	struct gotonode * list;
{
	while (list != NULL) {
		struct gotonode *next = list->next;
		fixbranch(list->gotoloc);
		gotonode_Destroy(list);
		list = next;
	}
}

/* predfixdropthru()
	Fixes all branches on list 'preddropthrulist' to branch to 
	current position in object code.
*/
	void
predfixdropthru()
{
	fixbranchlist(preddropthrulist);
	preddropthrulist = NULL;
}

/* predfixtarget()
	Fixes all branches on list 'predtargetlist' to branch to 
	current position in object code.
*/
	void
predfixtarget()
{
	fixbranchlist(predtargetlist);
	predtargetlist = NULL;
}

/* predexit(construct)
	generate a branch to the 'target' location for the enclosing construct
	construct value must be 'I' for if or 'W' for while
	The predtargetlist we want may be the current one or the one on the stack
	in the frame one more recent than the most recent instance of 'construct'.
*/
	void
predexit(construct)
	char construct;
{
	struct gotonode **targetlist;
	struct predstatenode *sp;

	if (pssp->construct == construct)
		targetlist = &predtargetlist;
	else {
		/* scan the stack for desired construct  
			when the targetlist is empty, we must be in the Elsepart already*/
		for (sp = pssp-1; sp >= predstack; sp--) 
			if (sp->construct == construct && (sp+1)->targetlist != NULL)
				break;
		if (sp >= predstack)
			targetlist = &((sp+1)->targetlist);
		else {
			ReportError(":Not within a suitable context", 0);
			return;
		}
	}
	*targetlist = gotonode_Create(genbranch('g', 0), *targetlist);
}


/*  = = = = = = = = = = = = = = = = = = =
 *    symbols and variables
 * = = = = = = = = = = = = = = = = = =*/

/*demandsymboltype(sym, type) 
	Check that the symbol is of the required type 
	and give an error message if not.
	Cannot generate coercions because the symbol is not on the stack.
*/
	void
demandsymboltype(sym, type)
	struct nesssym *sym;
	Texpr type;
{
	if (sym->type != type)
		ReportError(":wrong type of variable", -1);
} 

/*demandnodetype(node, type, tokloc) 
	Check that the exprnode 'node' specifies the required 'type'.
	and give an error message if not.
	
*/
	void
demandnodetype(node, type)
	struct exprnode *node;
	Texpr type;
{
	if (node->type != type)
		ExprError(":expression is of the wrong type", node);
}

	struct varnode *
varIsStorable(var)
	struct varnode *var;
{
	struct nesssym *sym = var->sym;
	if ((sym->flags & (flag_var | flag_undef | flag_builtin)) != flag_var) {
		if (sym->flags & flag_function)
			SaveError(":cannot assign to function name", 
					var->loc, var->len);
		else if (sym->flags & flag_builtin)
			SaveError(":cannot assign to predefined name", 
					var->loc, var->len);
		else
			SaveError(":refers to an undefined variable", 
					var->loc, var->len);
	}
	return var;
}

	struct varnode *
varIsFunction(var)
	struct varnode *var;
{
	struct nesssym *sym = var->sym;
	if ((sym->flags & (flag_var | flag_const)) != 0) {
		ReportError(":not a proper function name", -2);
		var->sym = NULL;
	}
	else if (sym->flags == flag_undef) 
		/* unknown name:  assume it is a forward function call 
			or a call to an external function */
		callUnknown(sym);
	return var;
}

	struct nesssym *
uniqueinscope(var, flags, tokoff)
	struct nesssym* var;
	unsigned long flags;
	long tokoff;
{
	static long junknum = 91;
	 /* if var->flags is not zero, we have some old symbol
		and need to redefine it.
		XXX How better to know if previously defined within this function???
	*/
	if (var->flags != 0 && nesssym_NGetScope(var) == lex_GetScope(curLex)) {
		/* redefining a symbol already defined in this function */
		char buf[12];
		ReportError(":previously defined", tokoff);
		sprintf(buf, "X:%d", junknum++);
		var = nesssym_NDefine(buf, lexdef_GetPrototype(curLex->def),
				lex_GetScope(curLex));
	}
	else if (var->flags != 0) {
		/* redefining something global, use curScope to avoid conflict  */
		var = nesssym_NDefine(nesssym_NGetName(var), 
					lexdef_GetPrototype(curLex->def),
					lex_GetScope(curLex));
		var->header.toksym.toknum = curLex->def->IdTokNum;
	} 
	var->flags = flags; 
	return var;
}

/* process a list of id's being declared.  
	set flags and kind to given values. 
*/
	void
ProcessIdList(type, idlist, flags)
	Texpr type;
	struct nesssym *idlist;
	long flags;
{
	struct nesssym *id;
	for (id = idlist; id != NULL; id = id->next) {
		id-> flags = flags;
		id->type = type;
	}
}

	void
parencheck(left, right)
	struct varnode *left;
	struct toksym *right;
{
	char l = *left->paren->header.sym.name;
	char r = *right->header.sym.name;
	if ( ! ((l == '(' && r == ')')  ||  (l == '[' && r == ']')  ||  (l == '{' && r == '}'))) {
		/* mismatched parens */
		long loc, len;
		loc = lex_RecentPosition(0, &len);
		len = loc + len - left->loc;
		loc = left->loc;
		SaveError(":mismatched parentheses", loc, len);
	}	
}

/* genCheckEndtag(tag, desired)
	checks that the tag is the one desired
	pop the predstate stack to get to the desired one
		or until get to function or extend
*/
	void
genCheckEndtag(tag, desired)
	struct nesssym *tag;
	long desired;
{
	long tagtok;
	char *desiredname;
	char buf[300];
	char constructForTag;

	tagtok = tag->header.toksym.toknum;
	if (tagtok == desired)
		return;
	if (desired == ON && (tagtok == EVENT  ||  tagtok == MENU  ||  tagtok == MOUSE
				|| tagtok == KEYS))
		return;	/* will be dealt with in nevent */
	switch (desired) {
	case FUNCTION:  constructForTag = 'L';  desiredname = "function"; break;
	case EXTEND:  constructForTag = 'X';  desiredname = "extend"; break;
	case IF:  constructForTag = 'I';  desiredname = "if"; break;
	case WHILE:  constructForTag = 'W';  desiredname = "while"; break;
	case ON:  constructForTag = 'V';  desiredname = NULL; break;
		/* error message for ON will come from neventFinishEvent */
	}
	if (desiredname != NULL)  {
		sprintf(buf, "*Should be:   end %s.  Trying to fix by inventing ends.\n", 
				desiredname);
		ReportError(freeze(buf), -1);
	}
	while (pssp->construct != constructForTag
			&& pssp->construct != 'X'
			&& pssp->construct != 'L')
		predpop();
}


/* the next two functions are used by the grammar to save 
	the function state for the global init function 
	They provide only ONE level of save restore. 
*/
	void
genSaveFuncState()
{
	savedCurfuncmark = curfuncmark;
	savedVarFixups = varfixups;
	LocationAdvancing = ! LocationAdvancing;
	genPopScope();	/* scope is saved in the funcnode */
}
	void
genRestoreFuncState(func)
	 struct nesssym *func;
{
	curfuncmark = savedCurfuncmark;
	varfixups = savedVarFixups;
	LocationAdvancing = ! LocationAdvancing;
	genPushScope(nesssym_NGetINode(func, funcnode)->ownscope);
}
