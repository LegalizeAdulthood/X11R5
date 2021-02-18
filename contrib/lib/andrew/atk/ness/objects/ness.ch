/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/ness.ch,v 1.16 1991/09/12 19:44:06 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/ness.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/ness.ch,v 1.16 1991/09/12 19:44:06 bobg Exp $";
#endif

/*
ness.H

	ness data object

	Provides for the source and object of one script.  Actually stored as a text.
	Has methods for compilation and execution.

*/
/*
 *    $Log: ness.ch,v $
Revision 1.16  1991/09/12  19:44:06  bobg
Update copyright notice

Revision 1.15  1990/10/12  21:15:23  wjh
change to version 1.5

Revision 1.14  90/09/16  20:14:55  wjh
see ness/objects/changes.sept.90

Revision 1.13  90/07/15  15:24:09  wjh
call.c:  fix a coreleak in class()
ness.ch: defined CURRENTMODIFICATIONLEVEL
nessruna.c: set version number as 
	CURRENTSYNTAXLEVEL.CURRENTMODIFICATIONLEVEL
nessruna.c: add -f switch to cause a fork after compile and before exec
Imakefile:  switch to using nessrunapp instead of nessrun.ci
nessfunc.d: document the new proctable entries in framecmd.c
nessauth.d: describe usage of the framecmd stuff.
nesssauth.d: describe --$syntaxlevel
nessauth.d: give example of a long string

search.c: redo nextstylegroup
search.c: addstyles will now complain if the argument is a constant
gen.c: fix so it will not crash after allocating all the initial sysmarks
		that is: it won't crash after a few compiles
search.c: revise HasStyles to check for name equality as well as style location equality
search.c: add nextstylesegment() which gives the segment from end of subject to next style change
search.c: fix enclosingstyle, nextstylegroup, and nextstylesegment to know about 
	the bogus length values used in the outermost environment


Revision 1.12  89/11/04  17:26:06  wjh
patch from Guy Harris to ness.c:
Do initializeEnvt() in ness_EstablishViews() to be sure the pointers are initialized.  This avoids "!!! Disasterous error..."

patch from Guy Harris to ness.c:
Change call.c so it passes the long variant of anytype to proc in callCfunc.  This again avoids a "!!! Disasterous error."  Similarly change the argument to free() in FreeTheMallocs.

Changed delimiter in origin line to double backslash to avoid error warning in sendmessage about illegal character in text.

Added ness_GetOriginData to extract fields from origin data.

ESC-~ gave funny results in a nessview if text wass initially read-only, but had no warning.  It is now fixed to keep read-onliness and the warning message in synch.

Implemented  pragmat:  --$Ness <level>   
Initial syntax level is CURRENTSYNTAXLEVEL.
Level before read is set to UNSPECIFIEDSYNTAXLEVEL   (i.e., -1)
After compile check syntax level and give error if too HIGH.

If the left operand of a catenation has a style, that style was imposed on the right operand, but should not have been.   It no longer is.


Revision 1.11  89/09/17  17:08:18  wjh
add entry point to dump all errors and method to dump errors of a ness

Revision 1.10  89/09/10  17:50:29  wjh
change method of defining key and mouse bindings for views

Revision 1.9  89/09/03  22:46:17  wjh
newness

Revision 1.8  89/06/01  15:57:44  wjh
campus release version

Revision 1.5  88/11/23  14:44:31  wjh
proc table calls now work correctly with type-free procs  (the first arg can be anything)
added "cheat_" functions which will remain undocumented
changed inset() to a function
fixed some bugs in initial access

Revision 1.4  88/11/16  12:00:23  wjh

implemented access level control
skip first line of script if starts with # (for shell script)
changed so the name of all initialization functions is init()
added ness-load
moved execution of init() from compilation to first execution
deferred compilation to first FullUpdate time

Revision 1.3  88/11/01  17:42:49  wjh

an empty Ness is now given a template (ness.template or default.template)
implemented:  extend proctable on proc "name" ...
added currentmark()
fixed a core dump by not recompile during UnlinkTree()

Revision 1.2  88/10/26  11:53:54  wjh
fixed a malloc abort bug that happened when certain error messages were discarded
added call to init() when initial compilation is done
fixed so events are posted to views whenever the view name is registered with the arbiter

Revision 1.1  88/10/21  10:59:00  wjh
Initial revision

 * Revision 1.0  88/04/27  14:28:52  wjh
 * Copied from /usr/andrew/lib/dummy
 */
#include "error.h"
#include "nesssym.ih"

#define CURRENTSYNTAXLEVEL			1
/* 	Will get compile error if syntax level of program exceeds
	CURRENTSYNTAXLEVEL because that means we have
	a new program and an old compiler. 
*/
#define CURRENTMODIFICATIONLEVEL		5

/* nessrun generates the VERSION NUMBER as 
 *
 *	<CURRENTSYNTAXLEVEL>.<CURRENTMODIFICATIONLEVEL>
 *
 */

#define UNSPECIFIEDSYNTAXLEVEL -1
/* this value is assumed if the syntax level is otherwise unspecified */

/* values for NotifyObservers */
#define ness_NEWERROR 2		/* to notify the nessview */
#define ness_WARNINGTEXTCHANGED 3	/* to notify the nessview */

#if 0
/* enum ness_access {
	ness_codeInfrared, /* was tampered with.  Do not compile unless changed */
	ness_codeRed,	/* not even compiled */
	ness_codeOrange,	/* give compile errors for all modification ops */
	ness_codeYellow,	/* give compile errors for file system ops */
	ness_codeGreen,	/* prompt before first executing */
	ness_codeBlue	/* compile without prompting */
	ness_codeUV	/* no warningNotice and no dialog box */
};
#endif
#define ness_access unsigned long
#define ness_codeInfrared		-1
#define ness_codeRed		0
#define ness_codeOrange		3
#define ness_codeYellow		6
#define ness_codeGreen		10
#define ness_codeBlue		11
#define ness_codeUV		13


class ness : text
{

overrides:

	Read(/* struct ness *self, */ FILE *file, long id) returns long;
	ReadTemplate(/* struct ness *self, */ char *templateName, 
				boolean inserttemplatetext) returns long;
	HandleKeyWord(/* struct ness *self, */ long pos, char *keyword, FILE *file)
				returns long;
	Write (/* struct ness *self, */ FILE *file, long writeid, int level) returns long;
	NotifyObservers(/* struct ness *self, */ long status);
	SetReadOnly(/* struct ness *self, */ boolean readOnly);

methods:

	ReadNamedFile(/* struct ness *self, */ unsigned char *name) returns long;
		/* read an object from file, checking that it is a ness */

	EstablishViews(/* struct ness *self, */ struct view *child);
		/* set DefaultText and Arbiter using hueristics.  The 'child'
		  should be a child of the arbiter.  */

	Compile(/* struct ness *self */) returns struct errornode *;
		/* compile the source (needn't be called, Execute will call it) 
		  should be called to establish any event handlers */

	Execute(/* struct ness *self, */  char *func) returns struct errornode *;
		/* execute named function in the current script 
			using nessview as access to arbiter
			and using textview as subject for currentselection() */
		/* if value returned by compile or execute is NULL, it succeeded
		    otherwise value is a  (struct errornode *)
		*/

	ClearErrors(/* struct ness *self */);	/* empty the error chain */
	NextError(/* struct ness *self, */ struct errornode *curr) returns struct errornode *;
		/* finds next err after 'curr'.  If 'curr is NULL, finds first err 
			returns NULL if 'curr' is the last */
	PreviousError(/* struct ness *self, */ struct errornode *curr) returns struct errornode *;
		/* finds error before 'curr'.  If 'curr is NULL, finds first err 
			returns NULL if 'curr' is the first */

	Expose(/* struct ness *self */);	/* show the ness and first error */

	SetAccessLevel(/* struct ness * self, */ ness_access newlevel);
		/* set the access level for execution */
	AddWarningText(/* struct ness *self */);
		/* adds to the text the initial warning and final buttons.  make read/only  */
	RemoveWarningText(/* struct ness *self */);
		/* removes from the text the initial warning and final buttons. restores r/w  */
	GetOrigin(/* struct ness *self */) returns unsigned char *;
		/* returns the current origin string */
	GetOriginData(/* struct ness *self, */ char **date, char **author);
		/* extracts origin data.  Caller must free it. */

	dumpattrs(/* struct ness *self, */  FILE *file);
		/* print the attributes to 'file' */
	printerrors(/* struct ness *self, */  FILE *file) returns long;
		/* format and print error messages for 'self' to 'file' 
		    return number of errors */

macromethods:

	SetName(/* struct ness *self, */ n)		(self->name = (n))
	GetName(/* struct ness *self */)		(self->name)
	SetArbiter(/* struct ness *self, */ v)		(self->Arbiter = (v))
	GetArbiter(/* struct ness *self */)		(self->Arbiter)
	SetDefaultText(/* struct ness *self, */ t)	(self->DefaultText = (t))
	GetDefaultText(/* struct ness *self */)	(self->DefaultText)
	SupplyMarkerArg(/* struct ness *self, */ a)	(self->arg = (a))
	GetNext(/* struct ness *self */)		(self->next)
	GetLex(/* struct ness *self */)		(self->lex)
	GetErrors(/* struct ness *self */)		(self->ErrorList)
	HasWarning(/* struct ness *self */)		(self->hasWarningText)
	NeedsDialogBox(/* struct ness *self */)	(self->DisplayDialogBox)
	SetNeedsDialogBox(/* struct ness *self, */ v)	(self->DisplayDialogBox = (v))

classprocedures:

	InitializeClass() returns boolean;
	InitializeObject(struct ness *self) returns boolean;
	FinalizeObject(struct ness *self);
	SetDebug(boolean d);
	GetList() returns struct ness *;
	PrintAllErrors(char *when) returns long;
		/* formats error messages for all loaded Nesses 
		    and prints them to stderr.   'when' is printed in header
		    return number of errors found */

data:

	struct ness *next;		/* list of all nesses for mapping errors */
	unsigned char *name;	/* reference name;  not owned by ness */

	/* management of this text */
	unsigned char *Origin;	/* syntax level \0 date file written \0 file writer */
	long syntaxlevel;		/* from the Origin value */
	long OriginalModValue;	/* used to test if user has modified the text */
	long ScriptLoc, 		/* position of first char of script */
		AfterScriptLoc,	/* position of first char after script */
		DeauthButtonLoc,	/* warning text loc of Deauth button */
		ScanButtonLoc,	/*   "   "   "  of Scan  " */
		CompileButtonLoc, /*   "   "   "  of Compile  " */
		AuthorButtonLoc;	/*   "   "   "  of Author  " */
	ness_access accesslevel;	/* control what operations allowed */
	struct errornode *ErrorList;	/* list of errors */
	boolean IsNowOriginator;	/* set T when modify the Origin value */ 
	boolean hasWarningText;	/* T if the warningtext is present */
	boolean DisplayDialogBox;	/* (for nessview) needs dialog box before compile */
	boolean PromptBeforeCompile;	/* T if should prompt before doing next compile */
	boolean ReadingTemplate;	/* just while in ReadTemplate */
	boolean ClassEnabled;	/* T if wants to access class methods, et al. */

	/* compilation information */
	boolean compiled;		/* T iff compiled okay and no text change */
	struct lex *lex;
	nesssym_scopeType outerScope;	/* symbol table in use */
	nesssym_scopeType constScope;	/* scope for constants */
			/* the first character of a name in constScope tells its type:
				"	short string.  name follows
				0	integer or real. 0x<hexvalue>
				'	one character constant. the char
				/	long string.  number
				&	object reference. the name
			*/
	struct nesssym *globals;	/* list of symbols and function s*/
	struct libnode *libnode;	/* if this ness is for a library file */
	struct libusenode *libuseList;  /* list of libraries used */
	long compilationid;		/* value for useid in libusenodes */

	/* used during compilation */
	struct nesssym **AttrDest;		/* where to store attributes */
	struct nesssym **saveAttrDest;	/* AttrDest saved at start of EXTEND */
	struct nesssym *CurrentObject;	/* the current EXTEND object */
	struct nesssym *InitFunc;		/* function to call to do initialization */

	/* runtime information */
	boolean needsInit;		/* T if there is unexecuted function init */
	boolean ToldUser;		/* if just displayed something in msgline */
	struct textview *DefaultText;  /* value of defaulttext() function */
	struct arbiterview *Arbiter;	/* access to name space */
	struct view *CurrentInset;
	struct nessmark *arg;	/* from SupplyMarkerArg */

};

