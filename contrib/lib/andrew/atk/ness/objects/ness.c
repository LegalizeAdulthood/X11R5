/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/ness.c,v 1.33 1991/09/24 18:34:39 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/ness.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/ness.c,v 1.33 1991/09/24 18:34:39 wjh Exp $";
#endif

/* ness.c

	Code for the ness data object
*/
/*
 *    $Log: ness.c,v $
 * Revision 1.33  1991/09/24  18:34:39  wjh
 * cast the value returned from malloc
 * .
 * .de freeze
 * .
 *
 * Revision 1.32  1991/09/12  16:25:58  bobg
 * Update copyright notice and rcsid
 *
 * Revision 1.31  1990/11/28  14:38:48  susan
 * moved andrewos.h to be first include file
 *
 * Revision 1.30  90/10/12  21:11:20  wjh
 * Revised the getc/ungetc code in ness__ReadNamedFile to try to make it work on PMAX and Sun4
 * 	revised yaccpar to eliminate saving parser state.
 * 	moved yycompile to gen.c
 * 	eliminated ErrorList
 * 	made the argument to MapRunError be a ness instead of an ErrorList
 * 
 * 
 * Revision 1.29  90/09/17  11:11:17  wjh
 * reordered the code of ness__ReadNamedFile to try to make ungetc work on all systems.
 * 
 * Revision 1.28  90/09/16  20:14:42  wjh
 * see ness/objects/changes.sept.90
 * 
 * Revision 1.27  89/11/18  20:57:18  wjh
 * removed two declarations of unused variables: version and origin
 * 
 * Revision 1.26  89/11/04  17:27:58  wjh
 * fixed the message line execution error messages so they point correctly to the location of the error
 * 
 * Revision 1.25  89/11/04  17:23:57  wjh
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
 * Revision 1.24  89/10/24  09:05:55  wjh
 * SetCopyAsText so that cut and paste from a ness inset will not be pasted as an entire Ness insert.
 * 
 * Revision 1.23  89/09/19  19:39:19  wjh
 * fix type errors on Sun
 * 
 * Revision 1.22  89/09/17  17:07:37  wjh
 * call printallerrors for errors during procload (called from .XYZinit)
 * 
 * Revision 1.21  89/09/17  08:54:14  wjh
 * do not bring up a NessLibrary window if im hasn't any windows yet
 * 
 * Revision 1.20  89/09/10  17:51:38  wjh
 * change method of defining key and mouse bindings for views
 * 
 * Revision 1.19  89/09/03  22:48:13  wjh
 * newness
 * 
 * Revision 1.18  89/06/23  17:20:34  wjh
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
 * Revision 1.17  89/06/01  16:00:24  wjh
 * campus release version
 * 
 * Revision 1.12  88/12/20  19:46:37  wjh
 * fixed various bugs
 * 
 * Revision 1.11  88/12/18  17:40:27  wjh
 * fixed a number of bugs
 * added im_ForceUpdate()
 * changed order of args to DoHit()
 * 
 * Revision 1.10  88/12/07  22:43:27  wjh
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
 * Revision 1.9  88/11/23  14:42:53  wjh
 * proc table calls now work correctly with type-free procs  (the first arg can be anything)
 * added "cheat_" functions which will remain undocumented
 * changed inset() to a function
 * fixed some bugs in initial access
 * 
 * Revision 1.8  88/11/17  09:35:10  wjh
 * (expletive deleted)
 * 
 * Revision 1.7  88/11/16  17:30:02  wjh
 * fixed a bug
 * 
 * Revision 1.5  88/11/16  12:00:30  wjh
 * 
 * implemented access level control
 * skip first line of script if starts with # (for shell script)
 * changed so the name of all initialization functions is init()
 * added ness-load
 * moved execution of init() from compilation to first execution
 * deferred compilation to first FullUpdate time
 * 
 * Revision 1.4  88/11/02  14:42:11  wjh
 * fixed bugs with byte swapping
 * added some more corrected copyrights
 * 
 * Revision 1.3  88/11/01  17:41:46  wjh
 * 
 * an empty Ness is now given a template (ness.template or default.template)
 * implemented:  extend proctable on proc "name" ...
 * added currentmark()
 * fixed a core dump by not recompile during UnlinkTree()
 * 
 * Revision 1.2  88/10/26  11:53:32  wjh
 * fixed a malloc abort bug that happened when certain error messages were discarded
 * added call to init() when initial compilation is done
 * fixed so events are posted to views whenever the view name is registered with the arbiter
 * 
 * Revision 1.1  88/10/21  10:59:16  wjh
 * Initial revision
 * 
 * Revision 1.0  88/04/27  14:28:53  wjh
 * Copied from /usr/andrew/lib/dummy
 */

#include <andrewos.h>	/* for strings.h */
#include <stdio.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <svcconf.h>
#include <util.h>

#include <nodeclss.h>
#include <ness.eh>
#include <dataobj.ih>
#include <view.ih>
#include <proctbl.ih>
#include <message.ih>
#include <text.ih>
#include <xgetchar.h>
#include <arbiterv.ih>
#include <arbiter.ih>
#include <im.ih>
#include <frame.ih>
#include <buffer.ih>
#include <environ.ih>
#include <filetype.ih>
#include <mark.ih>
#include <stylesht.ih>

#include <nesssym.ih>
#include <lexdef.ih>
#include <lex.ih>
#include <interp.h>
#include <compdefs.h>
#include <envt.h>
#include <nevent.h>
#include <error.h>
#include <gen.h>
#include <call.h>


static struct ness *NessList = NULL;		/* list of all nesses 
				(so we can access library) */

struct lexdef *NessLex = NULL;

static boolean WantsDialogBox = FALSE;
static boolean ReadyToCompile = FALSE;   /* avoid loading lex and scanning library */

extern struct ness *InterpretationInProgress;   /* defined in interp.c */

void dostmt();
void procload();


static boolean debug;
#define DEBUG(s) {if (debug) {printf s ; fflush(stdout);}}
#define ENTER(r) DEBUG(("Enter %s(0x%lx)\n", "r", self))
#define LEAVE(r) DEBUG(("Leave %s(0x%lx)\n", "r", self))


/* releaseResources(self)
	release all data structures allocated for 'self' by a compilation
	XXX later need to delete nodes, object code, and SysMark space.
*/
releaseResources(self)
	struct ness *self;
{
	struct nesssym *g, *tg;
	struct libusenode *tuse;

	neventUnpost(self, debug);	/* remove old postings */

	/* delete from codeloc */
	for (g = self->globals;  g != NULL;  g = g->next) 
		if (g->flags == (flag_function | flag_ness))
			codelocForget(g);
		else if (g->flags == flag_xobj	) 
			/* traverse attributes of extended object */
			for (tg = nesssym_NGetINode(g, objnode)->attrs; 
					tg != NULL; tg = tg->next)
				if (tg->flags == (flag_function | flag_ness | flag_xfunc)
						|| tg->flags == flag_event)
					codelocForget(tg);

	if (self->outerScope != nesssym_GLOBAL)
		nesssym_NDestroyScope(self->outerScope);
	if (self->constScope != nesssym_GLOBAL)
		nesssym_NDestroyScope(self->constScope);

	ness_ClearErrors(self);

	self->globals = NULL;
	self->compiled = FALSE;

	while (self->libuseList != NULL) {
		tuse = self->libuseList;
		self->libuseList = self->libuseList->next;
		libusenode_Destroy(tuse);
	}
}

	static struct errornode *
execute(self, func)
	struct ness *self;
	char *func;
{
	struct nesssym *funcsym;
	ENTER(execute);

	if ( ! self->compiled) 
		self->ErrorList = ness_Compile(self);
	if (self->ErrorList != NULL)
		return self->ErrorList;

	for (funcsym = self->globals; funcsym != NULL; 
			funcsym = funcsym->next)
		if (strcmp(funcsym->header.sym.name, func) == 0) 
			break;
	if (funcsym == NULL) {
		unsigned char buf[200];
		sprintf(buf, "*Couldn't find %s\n", func);
		return (self->ErrorList = 
			errornode_Create(self, 0, 0, 0, freeze(buf), TRUE, self->ErrorList));
	}
	if (self->ErrorList == NULL)
		self->ErrorList = callInitAll(self);	/* sigh XXX check all libraries */

	if (self->ErrorList == NULL)
		self->ErrorList = interpretNess(
			((struct funcnode *)funcsym->header.toksym.info.node)
						->SysMarkOffset, 
			self->arg, self);
	if (self->ErrorList != NULL) {
		neventUnpost(self, FALSE);	/* remove old postings */
		MapRunError(self);
	}

	/* the arg is only used once ! */
	if (self->arg != NULL) {
		nessmark_Destroy(self->arg);
		self->arg = NULL;
	}
	LEAVE(execute);
	return self->ErrorList;
}


/* ThisUser()
	get user id and person name for the user
*/
	static unsigned char *
ThisUser()
{
	unsigned char *login, *name;
	struct CellAuth *thiscell;
	long uid, n;
	struct passwd *pw;
	static unsigned char buf[300];

	CheckServiceConfiguration();
	if (ThisDomain  && FindCell(ThisDomain, &thiscell) == 0
			&& (FillInCell(thiscell), thiscell->WpError != -1)) {
		/* got name from cell */
		login = (unsigned char *)thiscell->UserName;
		name = (unsigned char *)thiscell->PersonName;
	}
	else if ((uid = getvuid()) != -1 && (pw = getvpwuid(uid)) != NULL) {
		/* got a name from passwd file */
		login = (unsigned char *)pw->pw_name;
		name = (unsigned char *)pw->pw_gecos;
	}
	else {
		/* punt */
		login = (unsigned char *)"???";
		name = (unsigned char *)"Unknown user";
	}
	strcpy(buf, login);
	strcat(buf, ":  ");
	n = strlen(buf);
	strncpy (buf+n, name, 299-n);
	buf[299] = '\0';
	return buf;
}

/* udate(self, d, u)
	Construct an origin value from the date d and user name n
	THIS VALUE IS FORGABLE.  Just edit the file with (say) ed.
*/
	static unsigned char *
udate(self, d, n)
	struct ness *self;
	unsigned char *d;
	unsigned char *n;
{
	static unsigned char buf[356];
	if (strlen(n) > 299) n[299] = '\0';
	sprintf(buf, "%02d\\\\%s\\\\%s\\\\00", self->syntaxlevel, d, n); 
	return buf;
}

/* ResetOrigin(self)
	create new origin information
	use current time and user
*/
	static void
ResetOrigin(self)
	struct ness *self;
{
	unsigned char *neworigin;
	long now = time(0);
	if (self->Origin != NULL) free(self->Origin);
	neworigin = udate(self, NiceTime(now), ThisUser());
	self->Origin = (unsigned char *)strcpy(malloc(strlen(neworigin) + 1), neworigin);
	self->IsNowOriginator = TRUE;
}

	static void
parsepragmat(self, prag)
	struct ness * self;
	char *prag;
{
	char *enable = "enable class access";
	char *Ness = "Ness";

	if (strncmp(prag, enable, strlen(enable)) == 0)
		/* --$enable class access */
		self->ClassEnabled = TRUE;
	else if (strncmp(prag, Ness, strlen(Ness)) == 0) {
		/*  --$Ness <version-number>
			 check the version number */
		while (*prag && !isdigit(*prag)) prag++;
		if (*prag) 
			self->syntaxlevel = atoi(prag);
		else {
			/* Illegal $Ness pragmat */
			self->syntaxlevel = UNSPECIFIEDSYNTAXLEVEL;
		}
	}
}


	boolean
ness__InitializeClass(ClassID)
	struct classhdr *ClassID;
{
	WantsDialogBox = environ_GetProfileSwitch("NessUseDialogBoxInsteadOfWarning",
			FALSE);

	proctable_DefineProc("ness-dostmt", dostmt, class_Load("view"),
			"ness", "Read and execute a Ness statement");

		/* ness-load is designed to be called from the -call- 
		facility of init.c.  Its first argument is to be the name of
		a file to be loaded, compiled, and posted.
		This file can use -extend proctable-  */
	proctable_DefineProc("ness-load", procload, NULL,  "ness", 
			"Read, compile, and post procs from a named file");
	return TRUE;
}

	boolean
ness__InitializeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct ness  *self;
{
	self->Origin = NULL;
	self->syntaxlevel = CURRENTSYNTAXLEVEL;
	ResetOrigin(self);	/* IsNowOriginator & OriginalModValue */

	self->ScriptLoc = -1;
	self->AfterScriptLoc = -1;
	self->DeauthButtonLoc = -1;
	self->ScanButtonLoc = -1;
	self->CompileButtonLoc = -1;
	self->AuthorButtonLoc = -1;
	ness_SetAccessLevel(self, ness_codeBlue);
	self->ErrorList = NULL;
	self->hasWarningText = FALSE;
	self->DisplayDialogBox = FALSE;
	self->PromptBeforeCompile = FALSE;
	self->ReadingTemplate = FALSE;
	self->ClassEnabled = FALSE;

	self->compiled = FALSE;
	self->lex = NULL;
	self->outerScope = nesssym_GLOBAL;
	self->constScope = nesssym_GLOBAL;
	self->globals = NULL;
	self->libnode = NULL;
	self->libuseList = NULL;
	self->compilationid = im_GetWriteID();  /* give it a unique value */

	self->needsInit = FALSE;
	self->ToldUser = FALSE;
	self->DefaultText = NULL;
	self->Arbiter = NULL;
	self->CurrentInset = NULL;
	self->arg = NULL;
	ness_SetCopyAsText(self, TRUE);

	self->name = NULL;
	self->next = NessList;
	NessList = self;
	return TRUE;
}

	void 
ness__FinalizeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct ness  *self;
{
	struct ness *n, *prev;

	releaseResources(self);

	if(self->Origin) {
		free(self->Origin);
		self->Origin = NULL;
	}
	if (self->lex)
		lex_Destroy(self->lex);

	/* remove from NessList */
	if (NessList == self)
		NessList = self->next;
	else {
		for (n = NessList; n != NULL && n != self; n = n->next) 
			prev = n; 
		if (n == self)
			prev->next = n->next;
	}
}

	void 
ness__SetDebug(ClassID, d)
	struct classhdr *ClassID;
	boolean d;
{
	debug = d;
	printf("Ness: debug is now %d\n", debug);
}


/* ness__Write(self, file, writeID, level)
	write the text to a file as (more or less) a text
	revise or insert the initial line with author, date, and checksum
	be sure not to write the warning text or buttons
*/
	long 
ness__Write(self, file, writeID, level)
	struct ness *self;
	FILE *file;
	long writeID;
	int level;
{
	if (ness_GetWriteID(self) != writeID)  {
		ness_SetWriteID(self, writeID);
		fprintf(file, "\\begindata{%s,%ld}\n", class_GetTypeName(self),
			ness_GetID(self));
		fprintf(file, "\\origin{%s}\n", ness_GetOrigin(self));

		if (self->header.text.styleSheet->templateName)
			fprintf(file, "\\template{%s}\n",
				self->header.text.styleSheet->templateName);
		stylesheet_Write(self->header.text.styleSheet, file);
		if (self->hasWarningText)
			text_WriteSubString((struct text *)self, self->ScriptLoc, 
				self->AfterScriptLoc - self->ScriptLoc, file, TRUE);
		else
			text_WriteSubString((struct text *)self, 0, 
				ness_GetLength(self), file, TRUE);
		fprintf(file, "\\enddata{%s,%d}\n", class_GetTypeName(self),
			ness_GetID(self));
		fflush(file);
	}
	return ness_GetID(self);
}




/* ness__Read(self, file, id)
	read the file into the ness
	check the origin
*/
	long
ness__Read(self, file, id)
	struct ness *self;
	FILE *file;
	long id;
{
	long retval;
	if (self->ReadingTemplate)
		return super_Read(self, file, id);

	if (self->Origin) {
		free(self->Origin);
		self->Origin = NULL;
	}
	self->syntaxlevel = UNSPECIFIEDSYNTAXLEVEL;

	retval = super_Read(self, file, id);

	if (self->Origin == NULL) {
		/* self->Origin should have been set by HandleKeyWord, 
			but wasn't */
		unsigned char *neworigin;
		neworigin = udate(self, "unknown date",
						"????: Unknown User");
		self->Origin = (unsigned char *)strcpy(malloc(strlen(neworigin)+1), neworigin);
		self->IsNowOriginator = FALSE;
	}
	self->OriginalModValue = ness_GetModified(self);
	self->PromptBeforeCompile = TRUE;
	self->DisplayDialogBox = WantsDialogBox;	/* tell nessview to give dialog */
	if (self->accesslevel < ness_codeUV) {
		if ( ! WantsDialogBox) 
			ness_AddWarningText(self);
		ness_SetAccessLevel(self, ness_codeRed);
	}
	ness_SetReadOnly(self, TRUE);
	return retval;
}

/* ness__ReadTemplate(self, templateName, inserttemplatetext)
	read the template file
	set switch to tell __Read we are reading a template
*/
	long
ness__ReadTemplate(self, templateName, inserttemplatetext)
	struct ness *self;
	char *templateName;
	boolean inserttemplatetext;
{
	long val;
	self->ReadingTemplate = TRUE;
	val = super_ReadTemplate(self, templateName, inserttemplatetext);
	self->ReadingTemplate = FALSE;
	return val;
}


	long
ness__HandleKeyWord(self, pos, keyword, file)
	struct ness *self;
	long pos;
	char *keyword;
	FILE *file;
{
	if ( ! self->ReadingTemplate && strcmp(keyword, "origin") == 0)  {
		unsigned char buf[356], *bx = buf;
		long cnt = 0;
		int c;

		while ((c = getc(file)) != EOF && c != '}' && cnt < 355) {
			*bx++ = c;
			cnt++;
		}
		*bx = '\0';
		while (c != EOF && (c = getc(file)) != '\n') {}
		self->Origin = (unsigned char *)strcpy(malloc(strlen(buf)+1), buf);
		if (1 != sscanf(self->Origin, "%d", &self->syntaxlevel))
			self->syntaxlevel = UNSPECIFIEDSYNTAXLEVEL;
		self->IsNowOriginator = FALSE;
		return 0;
	}
	super_HandleKeyWord(self, pos, keyword, file);
}

	unsigned char *
ness__GetOrigin(self)
	struct ness *self;
{
	if ( ! self->IsNowOriginator 
			&& ness_GetModified(self) != self->OriginalModValue) 
		/* the current user has modified the text.  
			generate new origin information */
		ResetOrigin(self);
	return self->Origin;
}

	void
ness__GetOriginData(self, date, author)
	struct ness *self;
	char **date, **author;
{
	char *origin = (char *)ness_GetOrigin(self);
	char *start, *end;
	long n;

	/* (in early days, the origin fields were separated by single backslashes) */

	start = index(origin, '\\');
	if (start == NULL) {
		*date = freeze("(unknown date)");
		*author = freeze("(unknown author)");
		return;
	}

	while (*start == '\\') start++;
	end = index(start, '\\');
	if (end == NULL  ||  end-start > 300)  
		n = 28;  	/* (length of `date` value) */
	else n = end-start;
	*date = (char *)malloc(n+1);
	strncpy(*date, start, n);
	*((*date) + n) = '\0';

	start = end;
	while (*start == '\\') start++;
	end = rindex(start, '\\');
	if (end == NULL) n = strlen(start);
	else {
		if (*(end-1) == '\\') end--;
		n = end - start;
	}
	if (n > 299) n = 299;
	*author = (char *)malloc(n+1);
	strncpy(*author, start, n);
	*((*author) + n) = '\0';
}


/* ness__NotifyObservers(self, status)
	we override this call because it is made whenever textviewcmds
	modifies the underlying text
	We set the compiled flag to false.
*/
	void 
ness__NotifyObservers(self, status)
	register struct ness  *self;
	long status;
{
	super_NotifyObservers(self, status);

	/* doing the super_NotifyObservers first is important
		for menu updating via nessview_ObservedChanged */
	if (status != ness_WARNINGTEXTCHANGED)
		self->compiled = FALSE;
}


/* ness__SetReadOnly(self, readOnly)
	override this method to prevent being not read-only 
	while the warning text is in place
*/
	void 
ness__SetReadOnly(self, readOnly)
	register struct ness  *self;
	boolean readOnly;
{
	if (self->hasWarningText && ! readOnly) return;
	super_SetReadOnly(self, readOnly);
}


/* ness__ReadNamedFile(self, name)
	read an object from file, checking that it is a ness
	opens the file named by 'name' and checks that it is a ness data stream
	or a data stream which is a subclass of ness.  
	Also reads and sets attributes.
	returns TRUE for a successful read and FALSE otherwise
*/
	long
ness__ReadNamedFile(self, name)
	struct ness *self;
	unsigned char *name;
{
	long val;
	FILE *inputfile;
	long c;

	if ((inputfile=fopen(name, "r" )) == NULL) {
		printf("File %s not found\n", name);
		return dataobject_PREMATUREEOF;
	}
	/* check to see if we might be a shell script !  */
	c = getc(inputfile);
	if (c == '#') {
		/* skip first line */
		while ( ! feof(inputfile)) {
			c = getc(inputfile);
			if (c == '\n') break;
		}
	}
	else ungetc(c, inputfile);

	val = ReadTextFileStream((struct text *)self, name, inputfile);

	fclose(inputfile);
	return val;
}


/* ness__EstablishViews(self, child)
	ensure that DefaultText and Arbiter are set, if possible.
	First set the arbiter:
		if it is already non-NULL, do nothing
		if there is a defaulttext, do WantHandler from it
		otherwise do WantHandler from child
	Then set the defaulttext value:
		if it is already non-NULL, do nothing
		if there is an arbiter, ask it for "defaulttext"
			and then check its ancestors
		check the ancestors of the child
	The default text is changed if the ness is ever
	told about a child of the arbiter called "defaulttext".
*/
	void
ness__EstablishViews(self, child)
	register struct ness *self;
	struct view *child;
{
	struct textview *deftext = ness_GetDefaultText(self);
	struct arbiterview *arbview = ness_GetArbiter(self);
	struct view *textchild, *textsecond;

	initializeEnvt();	/* set up textviewClass for EstablishViews */

	/* 1. set self->Arbiter */

	if (arbview == NULL) {
		arbview = arbiterview_FindArb(
				(deftext != NULL) 
					? (struct view *)deftext 
					: (struct view *)child);
		ness_SetArbiter(self, arbview);
		if (debug)
			printf("arbiterview at 0x%lx\n", arbview);
	}


	/* 2. set self->DefaultText  */

	if (deftext == NULL && arbview != NULL) {
		struct view *dt;
		dt = arbiterview_GetNamedView(arbview, "defaulttext");
		deftext = (struct textview *)ProperPtr(dt, textviewClass);
		if (dt != NULL)
			DEBUG(("dt 0x%lx   deftext from dt 0x%lx\n", dt, deftext));
		if (deftext == NULL)
			/* try for a child of the arbview itself. */
			deftext = (struct textview *)ProperPtr((struct basicobject *)arbview,
					 textviewClass);
		if (deftext != NULL)
			DEBUG(("deftext from arbview 0x%lx\n", dt, deftext));
		if (deftext == NULL)
		    /* still no default text. search upward from arb and then child */
		    textchild = (struct view *)arbview, textsecond = (struct view *)child;
	}
	else 
		/* no default text and no arbiter.  search upward from child */
		textchild = (struct view *)child, textsecond = NULL;
	for ( ; deftext == NULL && textchild != NULL; 
				textchild = textsecond, textsecond = NULL) {
		register struct view *v;
		/* XXX BOGOSITY ALERT:  scan up tree for parent textview */
		for (v = textchild; v != NULL; v = v->parent)  {
			DEBUG(("parent is a %s\n",class_GetTypeName(v)));
			if (class_IsType(v, textviewClass)) {
				deftext = (struct textview *)v;
				break;
			}
		}
	}
	ness_SetDefaultText(self, deftext);
}



	struct errornode *
ness__Compile(self)
	register struct ness *self;
{
	static char *RedMsg = 
		":You must Empower this Ness before trying to compile";
	static char *RecurMsg = 
		":Ness cannot compile while an execution is in progress";

	ENTER(Compile);

	if (InterpretationInProgress != NULL) 
		return (self->ErrorList = 
			errornode_Create(self, 0, 0, 0, RecurMsg, 
						FALSE, NULL));
	releaseResources(self);

	if (self->accesslevel < ness_codeOrange)
		/* no compilation permitted */
		return (self->ErrorList = 
			errornode_Create(self, 0, 0, 0, RedMsg, 
						FALSE, NULL));
	if ( ! ReadyToCompile) {
		initializeEnvt();	/* initialize Ness interpreter */
		NessLex = lexdef_Create(yyerror, nesssym_New(), DashDash, TRUE,
				nesscomp_lexdesc());
		genInit(NessLex);
		callDirLibs();	/* find all library files */
		ReadyToCompile = TRUE;
	}
	if (self->lex == NULL) {
		self->lex = lex_Create(NessLex);
		lex_SetPragmatParser(self->lex, '$', parsepragmat, self);
	}

	self->outerScope = toksym_TNewScope(lexdef_GetGrammarScope(NessLex));
	self->constScope = toksym_TNewScope(lexdef_GetGrammarScope(NessLex));

	if (self->hasWarningText)
		lex_SetSource(self->lex, self, self->ScriptLoc, 
			self->AfterScriptLoc - self->ScriptLoc, 
			self->outerScope, self->constScope);
	else
		lex_SetSource(self->lex, self, 0, ness_GetLength(self), 
			self->outerScope, self->constScope);
	self->needsInit = FALSE;
	self->compilationid = im_GetWriteID();

	self->ErrorList = yycompile(self);
	if (FALSE /* debug */ ) 
		ness_dumpattrs(self, stdout);

	if (self->syntaxlevel > CURRENTSYNTAXLEVEL)
		self->ErrorList = errornode_Create(self, 0, 0, 0,
			":This compiler is out of date for this script", 
			TRUE, self->ErrorList);

	if (self->ErrorList != NULL) 
		return (self->ErrorList);

	self->needsInit = TRUE;
	if (self->accesslevel >= ness_codeGreen) {
		self->compiled = TRUE;
		self->PromptBeforeCompile = FALSE;
		neventPost(self, debug);		/* establish event handling */
	}

	LEAVE(Compile);
	return(NULL);
}

	struct errornode *
ness__Execute(self, func)
	register struct ness *self;
	char *func;
{
	return execute(self, func);
}

	void
ness__ClearErrors(self)
	struct ness *self;
{
	while (self->ErrorList != NULL) {
		struct errornode *t = self->ErrorList;
		self->ErrorList = t->next;
		errornode_Destroy(t);
	}
}

	struct errornode *
ness__NextError(self, curr)
	struct ness *self;
	struct errornode *curr;
{
	if (curr == NULL || self->ErrorList == NULL)
		return self->ErrorList;
	else 
		return curr->next;
}

/* ness__PreviousError(self, curr)
	return error node preceding curr
	if curr is NULL, return last error
	return NULL for the predecessor of first error node 
*/
	struct errornode *
ness__PreviousError(self, curr)
	struct ness *self;
	struct errornode *curr;
{
	struct errornode *pre = self->ErrorList;
	while (pre != NULL  &&  pre->next != curr)
		pre = pre->next;
	return pre;	/* NULL will indicate *front* of list */
}



	void
ness__dumpattrs(self, file)
	struct ness *self;
	FILE *file;
{
	dumpAttrList(file, self->globals);
}

	static void
dostmt(parentview)
	struct view *parentview;
{
	struct ness *tempNess = ness_New();
	unsigned char stmt [1025];
	unsigned char def[1025];
	unsigned char prompt[200];
	static unsigned char prefix [] = "function main()";
	static unsigned char suffix [] = ";end function;";
	struct errornode *success;

	ness_EstablishViews(tempNess, parentview);
	tempNess->CurrentInset = parentview;
	ness_SetAccessLevel(tempNess, ness_codeBlue);
	*def = '\0';
	strcpy(prompt, "Ness: ");
	while (TRUE) {
		if (message_AskForString(parentview, 0, 
				prompt, def, stmt, sizeof(stmt)-1) < 0)    {
			message_DisplayString(parentview, 0, "Cancelled");
			break;
		}
		ness_Clear(tempNess);	/* text_Clear() removes all text */
		ness_InsertCharacters(tempNess, 0, suffix, strlen(suffix));
		ness_InsertCharacters(tempNess, 0, stmt, strlen(stmt));
		ness_InsertCharacters(tempNess, 0, prefix, strlen(prefix));

		tempNess->ToldUser = FALSE;

		success = ness_Compile(tempNess);
		if (success == NULL) {
			message_DisplayString(parentview, 0, ". . .");
			success = execute(tempNess, "main");
		}
		if (success == NULL) {
			if ( ! tempNess->ToldUser)
				message_DisplayString(parentview, 0, "Done");
			break;
		}
		sprintf(prompt, "%s at %d.  Try again: ", success->msg+1,
				mark_GetPos(success->where) - strlen(prefix));
		ness_ClearErrors(tempNess);
		tempNess->compiled = FALSE;
		strcpy(def, stmt);
	}
	ness_Destroy(tempNess);
}

	static void
procload(filename)
	unsigned char *filename;
{
	/* XXX eventually use library for names without leading slash */
	/* XXX use the libnode if there is one for the file */

	struct ness *tempNess = ness_New();
	if (ness_ReadNamedFile(tempNess, filename) != dataobject_NOREADERROR) {
		fprintf(stderr, "ness-load: Could not read %s\n", filename);
		return;
	}

	ness_SetAccessLevel(tempNess, ness_codeBlue);

	/* XXX we need to make buffer for object,
	 *	but done the simple way creates a Ness object in the file. 
	 *	if (buffer_FindBufferByFile(filename) == NULL) {
	 *		char *slash = (char *)rindex(filename, '/');
	 *		buffer_Create((slash == NULL) ? filename : slash + 1, 
	 *			filename, NULL, tempNess);
	 *	}
	 */

	if (ness_Compile(tempNess) != NULL) 
		ness_PrintAllErrors("ness-load Compilation");

	/* note that the tempNess is NOT destroyed.  Its compiled code
	is probably referred to from proctable entries */
}

static struct frame *LibFrame = NULL;

	void
ness__Expose(self)
	struct ness *self;
{
	struct im *im;
	struct buffer *Buffer;
	unsigned char fullname[MAXPATHLEN+1];

	if (im_GetLastUsed() == NULL) {
		/* there aren't any active windows.  forget it. */
		return;
	}

	if (self->libnode != NULL) {
		/* it is a library file.  display it in the LibFrame */
		if (LibFrame == NULL) {
			/* we need to create a LibFrame window */
			im_SetProgramName("NessLibrary");
			im = im_Create(NULL);
			if (im == NULL) {	
	      			fprintf(stderr, "ness: Could not create new window.\n");
	     			return;
			}
			LibFrame = frame_New();
			if (LibFrame == NULL) {
	        			fprintf(stderr, "ness: Could not allocate enough memory to display library file.\n");
	      			return;
			}
			im_SetView(im, LibFrame);
			frame_PostDefaultHandler(LibFrame, "message", 
					frame_WantHandler(LibFrame, "message"));
			frame_SetCommandEnable(LibFrame, TRUE);
		}
		Buffer = buffer_FindBufferByData(self);
		if (Buffer == NULL) {
			strcpy(fullname, self->libnode->path);
			strcat(fullname, self->libnode->filename);
			Buffer = buffer_Create(ness_GetName(self), fullname, NULL, self);
		}
		frame_SetBuffer(LibFrame, Buffer, TRUE);
	}
	else 
		ness_NotifyObservers(self, ness_NEWERROR);
}

	struct ness *
ness__GetList(ClassID)
	struct classhdr *ClassID;
{
	return NessList;
}

/* ness__SetAccessLevel(self, level)
	sets the access level of the Ness
	checks the validity
	clears errors and sets to indicate that a compilation is needed
*/
	void
ness__SetAccessLevel(self, level)
	struct ness *self;
	long level;
{
	if (level < ness_codeInfrared) level = ness_codeRed;
	if (level > ness_codeUV) level = ness_codeGreen;

	self->accesslevel = level;
	self->compiled = FALSE;
}

	static void
xferfromwarning(self, pLoc, t, pPrevi, i, codelen)
	struct ness *self;
	struct text *t;
	long *pLoc, *pPrevi, i;
{
	long len = i - codelen - *pPrevi;
	ness_AlwaysCopyText(self, *pLoc, t, *pPrevi + 1, len);
	*pPrevi = i;
	*pLoc += len;
}

/* ness__AddWarningText(self)
	add the warning text and buttons to the ness text
	set read-only
*/
	/* copy text from t to self.
		The text in t has the form

			NESS script.  This inset is ...
			$name $date
			first part
			----
			----	Ness SCRIPT FOLLOWS
			----
			$script

			final part with buttons

		The $name string is replaced with the name field from the 
		origin and $date is replaced with the date.  
		$script will, in effect, be replaced with the script (in 
		fact the portion of the warningNotice before $script is 
		copied to before the script and the remainder is copied to 
		after the script.  The button locations are given with
		code $X, where X is one of A, S, D, or E.  The code is deleted
		and the position of the next character is taken as the 
		position of the button.  The capital letters are
			D for deauthenticate
			E for empower (compile)
			S for scan
			A for author mode
		(strange effects will be observed if $ is used otherwise)
 */
	void
ness__AddWarningText(self)
	struct ness *self;
{
	unsigned char *name;
	FILE *wtext;
	struct text *t;
	long i, len, loc, previ;
	unsigned char *date, *uname;

	ness_SetReadOnly(self, TRUE);
	if (self->hasWarningText) return;

#ifdef WJH
	name = (unsigned char *)"nesswarn.d";
#else
	name = (unsigned char *)environ_AndrewDir("/lib/ness/nesswarn.d");
#endif
	wtext = fopen(name, "r");
	if (wtext == NULL)
		return;
	t = text_New();
	ReadTextFileStream(t, name, wtext);

	len = text_GetLength(t);
	if (len == 0) {
		/* sigh, file cannot be found */
		fprintf(stderr, "Cannot find %s", name);
		return;
	}

	ness_GetOriginData(self, &date, &uname);

	loc = 0;	/* where are we in the script */
	previ = -1;	/* just before start of text 
				to next xfer from warning */
	/* process each character of the warning text */
	for (i = 0; i < len; i++) {
		unsigned char c = text_GetChar(t, i);
		unsigned char *codex;
		if (c != '$') 
			continue;

		i++; /* advance input to char after '$' */
		c = text_GetChar(t, i);
		switch(c) {
		case 'A':  xferfromwarning(self, &loc, t, &previ, i, 2);
			self->AuthorButtonLoc = loc;
			break;
		case 'S':  xferfromwarning(self, &loc, t, &previ, i, 2);
			self->ScanButtonLoc = loc;
			break;
		case 'D':  xferfromwarning(self, &loc, t, &previ, i, 2);
			self->DeauthButtonLoc = loc;
			break;
		case 'E':  xferfromwarning(self, &loc, t, &previ, i, 2);
			self->CompileButtonLoc = loc;
			break;
		case 'd':
			codex = (unsigned char *)"date";
			while (*++codex) {
				i++;
				if (*codex != text_GetChar(t, i)) break;
			}
			if (*codex == '\0') {
				/* insert date */
				xferfromwarning(self, &loc, t, &previ, i, 5);
				ness_AlwaysInsertCharacters(self, loc, 
						date, strlen(date));
				loc += strlen(date);
			}
			break;
		case 'n':
			codex = (unsigned char *)"name";
			while (*++codex) {
				i++;
				if (*codex != text_GetChar(t, i)) break;
			}
			if (*codex == '\0') {
				/* insert name */
				xferfromwarning(self, &loc, t, &previ, i, 5);
				ness_AlwaysInsertCharacters(self, loc,
						uname, strlen(uname));
				loc += strlen(uname);
			}
			break;
		case 's':
			codex = (unsigned char *)"script";
			while (*++codex) {
				i++;
				if (*codex != text_GetChar(t, i)) break;
			}
			if (*codex == '\0') {
				/* insert surrounding text around script */
				xferfromwarning(self, &loc, t, &previ, i, 7);
				self->ScriptLoc = loc;
				loc = ness_GetLength(self);
				self->AfterScriptLoc = loc;
			}
			break;

		default: 
			/* just skip the '$' */
			i--;	/* reread the char after the '$' */
			break;
		}
	}
	xferfromwarning(self, &loc, t, &previ, i, 1);
	text_Destroy(t);
	self->hasWarningText = TRUE;
	if ( ! self->IsNowOriginator)
		self->OriginalModValue = ness_GetModified(self);

	if (date != NULL) free((char *)date);
	if (uname != NULL) free((char *)uname);
}

/* ness__RemoveWarningText(self)
	remove the warning text and set the Ness to read-write 
*/
	void
ness__RemoveWarningText(self)
	struct ness *self;
{
	long len1, len2;
	if ( ! self->hasWarningText) return;
	/* do the deletions in the order indicated (superstition: maybe it will fix a bug) */
	len1 = self->ScriptLoc;
	len2 = ness_GetLength(self) - self->AfterScriptLoc;
	ness_AlwaysDeleteCharacters(self, 0, len1);
	ness_AlwaysDeleteCharacters(self, self->AfterScriptLoc - len1, len2);
	self->hasWarningText = FALSE;
	if ( ! self->IsNowOriginator)
		self->OriginalModValue = ness_GetModified(self);

}

/* ness__printerrors(self, file)
	format and print error messages for 'self' to 'file'
	return number of errors 
*/
	long
ness__printerrors(self, file)
	struct ness *self;
	FILE *file;
{
	register struct errornode *list = ness_GetErrors(self);
	long cnt = 0;
	while (list != NULL) {
		register long loc, len, tloc, textend, c;
		cnt++;
		loc = mark_GetPos(list->where);
		len = mark_GetLength(list->where);
		if (list->execloc != 0)
			fprintf(file, "Code Loc: %d\n", list->execloc);
		fprintf(file, "   at %d (len %d): %s\n   source text is:  ", 
				loc, len, list->msg+1);
		textend = ness_GetLength(self);

		for (tloc = loc; tloc >= 0 && simpletext_CorrectGetChar(
						(struct simpletext *)self, tloc)
					!= '\n';
				tloc--) {}
		tloc++;	/* first character of line */
		for ( ; tloc < textend & (c=simpletext_CorrectGetChar(
						(struct simpletext *)self, tloc))
					!= '\n';  tloc++) {
			if (tloc == loc)  fprintf(file, " >>> ");
			fputc(c, file);
			if (tloc == loc+len - 1)  fprintf(file, " <<< ");
		}
		fprintf(file, "\n\n");

		list = list->next;
	}
	return cnt;
}

/* ness__PrintAllErrors()
	formats error messages for all loaded Nesses and prints them to stderr
	'when' is printed in header
	return number of errors found
*/
	long
ness__PrintAllErrors(class, when)
	struct classheader *class;
	char *when;
{
	struct ness *n;
	long cnt = 0;
	struct errornode *errs;
	unsigned char *name;
	for (n = ness_GetList(); n != NULL; n = ness_GetNext(n)) {
		errs = ness_GetErrors(n);
		if (errs == NULL) 
			continue;
		name = ness_GetName(n);
		if (name != NULL)
			fprintf(stderr, "%s error(s) in %s\n", when, name);
		else 
			fprintf(stderr, "%s error(s):\n", when);
		cnt += ness_printerrors(n, stderr);
	}
	return cnt;
}
