/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/interp.c,v 1.37 1991/09/24 18:34:39 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/interp.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/interp.c,v 1.37 1991/09/24 18:34:39 wjh Exp $";
#endif

/* interp.c
	interpret byte code representation of ness programs

	Entry points:

	initializeEnvt()  -  initialize the run-time environment
	stackArg(argvalue, type) - puts an argument on the stack
	interpretNess(func, ness)  -  interpret a call on func
*/

/*
 * $Log: interp.c,v $
 * Revision 1.37  1991/09/24  18:34:39  wjh
 * cast the value returned from signal()
 * .de freeze
 * .
 *
 * Revision 1.36  1991/09/12  16:25:44  bobg
 * Update copyright notice and rcsid
 *
 * Revision 1.35  1991/02/21  10:47:02  wjh
 * extend error check in readfile so it does not try to read file if the name is an empty string
 *
 * Revision 1.34  91/02/08  14:34:57  wjh
 * readfile and writefile will no longer signal a runtime error if they cannot access the given file. 
 * Instead they send a message to stderr which shows up in the console.
 * For an errroneous file, readfile and writefile return an empty strings.
 * The control-G will no longer be treated as a runtime error.  Instead it will calmly terminate Ness execution and display the message "Interrupted !"
 * 
 * Revision 1.33  90/12/11  17:05:23  wjh
 * fix so stack pointer is correctly set after calling a C function
 * 
 * Revision 1.32  90/09/30  09:48:00  wjh
 * improved compilability of the bcopys. compliments of Guy Harris
 * 
 * Revision 1.31  90/09/16  20:14:27  wjh
 * see ness/objects/changes.sept.90
 * 
 * Revision 1.30  90/07/16  15:41:54  gk5g
 * The second argument to longjmp was being casted to (char*) when it should have been casted to (int).
 * 
 * Revision 1.29  90/03/22  11:08:27  wjh
 * Fix "Disasterous Ness error" introduced by earlier fix of core leak.  (nessmarks were being deleted as part of returning a nessmark value if another marker to the same text was in the variables of the function returning the value.)
 * 
 * Revision 1.28  89/11/21  11:56:33  wjh
 * Correct the decision in writefile() of whether and what template to use.  
 * This fixes the bug in files written by gendoc.  They were getting style menus for only
 * a few styles.  Now they get all styles.
 * 
 * Revision 1.27  89/11/04  17:24:51  wjh
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
 * Revision 1.26  89/10/25  23:37:42  zs01
 * Yet again.
 * 
 * Revision 1.25  89/10/25  23:16:03  zs01
 * Extended VAX ifdefs to cover PMAX on isnan crap.
 * 
 * Revision 1.24  89/10/06  16:28:28  ghoti
 * changed defined(SYSV_ENV) to SY_U5x
 * 
 * Revision 1.23  89/10/06  11:33:19  ghoti
 * changed #ifndef HPUX_ENV's around things like 'isnan' and 'lgamma' to be:
 * #if (! defined(SYSV_ENV) && ! SY_AIXx)
 * 
 * Revision 1.22  89/09/18  20:05:08  wjh
 * convert for _ENV
 * 
 * Revision 1.21  89/09/17  08:53:19  wjh
 * make sure textimage generates real values with a "."
 * 	and null pointers are printed as NULL
 * 
 * Revision 1.20  89/09/10  17:51:20  wjh
 * change method of defining key and mouse bindings for views
 * 
 * Revision 1.19  89/09/03  22:47:49  wjh
 * newness
 * 
 * Revision 1.18  89/06/23  17:20:09  wjh
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
 * Revision 1.17  89/06/01  15:59:55  wjh
 * campus release version
 * 
 * Revision 1.11  89/01/04  16:27:52  wjh
 * fix another stupid coredump
 * 
 * Revision 1.10  89/01/04  12:57:05  wjh
 * fix stupid incompatible type messages for unsigned char *
 * 
 * Revision 1.9  88/12/20  19:46:00  wjh
 * fixed various bugs
 * 
 * Revision 1.8  88/12/18  17:40:11  wjh
 * fixed a number of bugs
 * added im_ForceUpdate()
 * changed order of args to DoHit()
 * 
 * Revision 1.7  88/12/07  22:43:01  wjh
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
 * Revision 1.6  88/11/23  14:43:27  wjh
 * proc table calls now work correctly with type-free procs  (the first arg can be anything)
 * added "cheat_" functions which will remain undocumented
 * changed inset() to a function
 * fixed some bugs in initial access
 * 
 * Revision 1.5  88/11/16  12:00:04  wjh
 * 
 * implemented access level control
 * skip first line of script if starts with # (for shell script)
 * changed so the name of all initialization functions is init()
 * added ness-load
 * moved execution of init() from compilation to first execution
 * deferred compilation to first FullUpdate time
 * 
 * Revision 1.4  88/11/02  14:41:41  wjh
 * fixed bugs with byte swapping
 * added some more corrected copyrights
 * 
 * Revision 1.3  88/11/01  17:41:53  wjh
 * 
 * an empty Ness is now given a template (ness.template or default.template)
 * implemented:  extend proctable on proc "name" ...
 * added currentmark()
 * fixed a core dump by not recompile during UnlinkTree()
 * 
 * Revision 1.2  88/10/26  11:54:00  wjh
 * fixed a malloc abort bug that happened when certain error messages were discarded
 * added call to init() when initial compilation is done
 * fixed so events are posted to views whenever the view name is registered with the arbiter
 * 
 * Revision 1.1  88/10/21  10:58:35  wjh
 * Initial revision
 * 
 * Creation 0.0  88/03/29 10:16:00  wjh
 * Initial creation by WJHansen
 * 
*/

#include <andrewos.h>
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>


#include <view.ih>
#include <textv.ih>
#include <stylesht.ih>
#include <envrment.ih>
#include <keystate.ih>
#include <im.ih>
#include <proctbl.ih>
#include <celv.ih>
#include <message.ih>
#include <attribs.h>

#include <ness.ih>
#include <nessmark.ih>

#include <xgetchar.h>
#include <nodeclss.h>
#include <error.h>
#include <interp.h>
#include <envt.h>
#include <nevent.h>
#include <call.h>

#define APPENDSLOP 100		/* bytes to increase append rcvr beyond required */
#define FUNCSTACKSLOP 55	/* number of markers to leave space for after entering
					a function */


/* defining instances of globals  from envt.h */

struct simpletext *ObjectCode = NULL;	/* the compiled bytecode for the functions */
struct simpletext *EmptyText = NULL;	/* points to the special mark for error extent */
struct nessmark *SysMarkFree = NULL, *SysMarkLowEnd = NULL, *SysMarkHiEnd = NULL;
long SysMarkSize = InitialSysMarkSize;
TType nessmarkHdr = NULL;
struct frameelt *FramePtr = NULL;	/* points to current stack frame */
union stackelement *NSPstore = NULL, 
		*NSPbase = NULL, *NSLowEnd = NULL, *NSHiEnd = NULL;
long NSSize = InitialNSSize;		/* the number of words in the stack. */
/* long nArgsGiven = 0;		number of arguments on stack when interpreter is called */
struct classinfo *celClass = NULL;
struct classinfo *textClass = NULL;
struct classinfo *textviewClass = NULL;
struct classinfo *dataobjectClass = NULL;
struct classinfo *lpairClass = NULL;
struct classinfo *viewClass = NULL;
struct classinfo *celviewClass = NULL;
struct classinfo *valueviewClass = NULL;
struct classinfo *imClass = NULL;
struct ness *InterpretationInProgress = NULL;	/* used in ness.c */

static jmp_buf ExecutionExit;
static unsigned char *iarStore;
static char *ErrorMsg;
static struct errornode *SavedMsg;


/* XXX the iar is no longer needed as an argument since it is saved in memory on every cycle */

/*RunError(msg, iar)
	execution errors call this routine, which passes the error along.
	'msg' describes the error.  If 'msg' begins with ':' it is a constant;
	if with a '*', it has been malloced especially for this error and
	will be freed when the error is discarded.
	'iar' is the index of the opcode that failed (it has been adjusted backward).
*/
	boolean	/* type is to fool the conditional expr in PTSTOMARK */
RunError(msg, iar) 
	char *msg;
	unsigned char *iar;
{
/* printf("RunError @ %d: %s\n", iar, msg); */
	ErrorMsg = msg;
	longjmp (ExecutionExit, 99);
	return TRUE;		/* more stuff to fool compiler */
}

	void
SigHandler()
{
	RunError(":! ! !   Disastrous Ness error in this function.   Quit soon.   !!!!!!!!!!", 0); 
}


/* initializeEnvt()
	initialize all facets of the Ness environment
*/
	void
initializeEnvt()
{
	short i;
	static boolean Initialized = FALSE;
	if (Initialized) 
		return;
	Initialized  = TRUE;

	celClass = class_Load("cel");
	textClass = class_Load("text");
	textviewClass = class_Load("textview");
	dataobjectClass = class_Load("dataobject");
	lpairClass = class_Load("lpair");
	viewClass = class_Load("view");
	celviewClass = class_Load("celview");
	valueviewClass = class_Load("valueview");
	imClass = class_Load("im");

	ObjectCode = simpletext_New();

	/* SysMark array */
	SysMarkSize = InitialSysMarkSize;
	SysMarkLowEnd = (struct nessmark *)calloc(SysMarkSize, sizeof(struct nessmark));
	SysMarkHiEnd = SysMarkLowEnd + SysMarkSize;
	for (i = 0; i < SysMarkSize; i++)
		nessmark_SetNext(SysMarkLowEnd+i, (struct mark *)(SysMarkLowEnd+i+1));
	nessmark_SetNext(SysMarkHiEnd-1, NULL);
	SysMarkFree = SysMarkLowEnd + 1;
		/* discard SysMark[SysMarkLowEnd] so no function
		will have index 0 */

	/* allocate EmptyText. */
	nessmark_Initialize(SysMarkLowEnd);
	nessmark_MakeConst(SysMarkLowEnd, "");
	EmptyText = nessmark_GetText(SysMarkLowEnd);

	/* XXX Ugh.  We save the first word of a nessmark to identify other nessmarks */
	nessmarkHdr = (TType)(SysMarkLowEnd->header.nessmark_methods);


	/* Ness Execution Stack */
	NSSize = InitialNSSize;
	NSLowEnd = (union stackelement *)calloc(NSSize, sizeof(union stackelement));
	NSHiEnd = NSLowEnd + NSSize;
	NSPstore = NSHiEnd;

	/* create an initial frame record */
	NSPstore = (union stackelement *)((unsigned long)NSPstore - sizeof(struct frameelt));
	NSPstore->f.hdr = frameHdr;
	NSPstore->f.returnaddress = NULL;  
	NSPstore->f.prevframe = NULL;

	NSPbase = NSPstore;
	FramePtr = &NSPbase->f;
	/*  nArgsGiven = 0; */
}


/* popValue(NSP)
	removes topmost value from stack
	unlinks marks and frames
*/
	union stackelement *
popValue(NSP)
	union stackelement *NSP;
{
	switch (NSP->l.hdr) {
	      case (longHdr):
		NSPopSpace(longstkelt);
		break;
	      case (boolHdr):
		NSPopSpace(boolstkelt);
		break;
	      case (dblHdr):
		NSPopSpace(dblstkelt);
		break;
	      case (ptrHdr):
		NSPopSpace(ptrstkelt);
		break;
	      case (frameHdr):
		if (((long)NSP) >= ((long)FramePtr))
			FramePtr = NSP->f.prevframe;
		else {};	/* XXX illegal framepointer skipped ??? */
		NSPopSpace(frameelt);
		break;
	      default:
	      	if (NSP->l.hdr == nessmarkHdr) {
			/* remove link to this mark.  */
			nessmark_Finalize(&NSP->m);
			NSPopSpace(nessmark);
		}
		else
			/* ERROR: just discard one long word */
			NSPstore=NSP=(union stackelement *)(((unsigned long)NSP) 
						+ sizeof(long));
		break;
	}
	return NSPstore = NSP;
}


#if notdef
/* we do not need stackArg for now.  The single arg passed to interpretNess is enough for nessrun
the code is useful because it shows how to stack things */

union  argType {
	long l;
	unsigned long b;
	double d;
	struct basicobject *p;
	struct nessmark *m;
};


/* stackArg(arg, type)
	 puts the argument onto NSP and keeps track of the number of arguments supplied
*/
	boolean
stackArg(arg, type)
	union  argType *arg;
	TType type;
{
	register union stackelement *NSP = NSPstore;
	if (NSP-1 <= NSLowEnd) {
		/* stack overflow */
		/* XXX */
		return FALSE;
	}

	switch (type) {
	case (longHdr):
	case (boolHdr):
		NSPushSpace(longstkelt);
		NSP->l.hdr = type;
		NSP->l.v = arg->l; 
		break; 
	case (dblHdr):
		NSPushSpace(dblstkelt);
		NSP->d.hdr = type;
		NSP->d.v = arg->d; 
		break;
	case (ptrHdr):
		if ((TType)arg->m->header.nessmark_methods
				== nessmarkHdr) {
			NSPushSpace(nessmark); 
			nessmark_InitFrom(&NSP->m, arg->m);
		}
		else {
			/* some other kind of pointer */
			NSPushSpace(ptrstkelt);
			NSP->p.hdr = type;
			NSP->p.v = arg->p; 
		}
		break;
	}
	nArgsGiven ++;
	return TRUE;
}
#endif

	static void
InterruptNess(ness)
	struct ness *ness;
{
	message_DisplayString(im_GetLastUsed(), 0, "Interrupted !");
	im_ForceUpdate();
	ness->ToldUser = TRUE;
	longjmp(ExecutionExit, 1);	/* normal exit */
}

	static void
QueryReadOnly(ness, s, msg, iar)
	struct ness *ness;
	struct simpletext *s;
	char *msg;
	unsigned char *iar;
{
	if (s == (struct simpletext *)ness_GetDefaultText(ness)) {
		/* see if user wants to make defaulttext read/write */
		long choice;
		static char *choices[] = {
			"Cancel - Leave text read-only", 
			"Read-Write - Let the Ness script modify the text", 
			NULL
		};
		if (message_MultipleChoiceQuestion(NULL, 50, 
			"The main text is read-only, but the script is trying to modify it.  Okay?",
				0, &choice, choices, "cr") >= 0 
				&& choice == 1) {
			/* ok, make it read write */
			simpletext_SetReadOnly(s, FALSE);
			return;
		}
	}
	RunError(msg, iar);
}

#define PTSTOMARK(arg,iar)  ((TType)((struct nessmark *)arg)->header.nessmark_methods \
		== nessmarkHdr) ? TRUE :   \
		RunError(":Not a pointer to a mark (uninitialized variable?)", iar);

#define PUSHMARK(src) {NSPushSpace(nessmark);  nessmark_InitFrom(&NSP->m, src); }


/* interpretNess(func, ness)  -- interpret the code for the ness function func

	'func' is the location value returned by makeFunction
	presumably a compiler has put bytecodes within this mark.

	A single marker argument is supplied to the top level function.

	'ness' is used for GetArbiter,  GetDefaultText,  and GetCelScope
*/
	struct errornode *
interpretNess(func, arg, ness)
	short func;
	struct nessmark *arg;
	struct ness *ness;
{
	register union stackelement *NSP;	/* stack pointer */
	register unsigned char *iar;	/* next opcode to execute */
	unsigned char *iarzero;	/* point to first byte of object code */
	unsigned char *PrevAddr;	/* former value of iar before goto or call */
	long objlen = simpletext_GetLength(ObjectCode);
	long nextiar;		/* beginning of func */
	long lengthgotten;
	long CondCode;		/* set by compares; tested by branches
					0: EQ    1:GT   -1:LT   -2:error */
	int (*oldBus)(), (*oldSeg)(); 	/* save signal handlers */
	boolean Success;
	long gocount;
	struct osi_Times starttime;
	boolean Gmessaged = FALSE;
	long exitcode;
	jmp_buf SaveExecExit;
	struct frameelt *SaveFramePtr = FramePtr;
	struct ness *SaveInterpInProg = InterpretationInProgress;

	/* SaveExecExit = ExecutionExit; */
	bcopy((char *)ExecutionExit, (char *)SaveExecExit, sizeof(jmp_buf));

	if (InterpretationInProgress == NULL) {	
		/* reinitialize stack pointer */
		NSPstore = NSPbase;

		/* XXX The next two lines attempt to move the gap to the end of ObjectCode 
			XXX so iar++ will sequence through the code */
		simpletext_InsertCharacters(ObjectCode, objlen, " ", 1);
		simpletext_DeleteCharacters(ObjectCode, objlen, 1);
	}
	InterpretationInProgress = ness;
	NSP = NSPstore;
	FramePtr = NULL;	/* terminate the unstacking loop */
	PrevAddr = NULL;	/* this will terminate the while loop */
	/* the first frame will be built by the EnterFunction ('P') operator 
		at the beginning of the first function */
	if (arg != NULL) 
		PUSHMARK(arg);

	nextiar = nessmark_GetPos(&SysMarkLowEnd[func]);
	iar = (unsigned char *)simpletext_GetBuf(ObjectCode, nextiar, 
		objlen - nextiar, &lengthgotten);
	iarzero = iar - nextiar;		/* for debugging */
/* printf("iarzero: %d\n", iarzero); */
	if (lengthgotten < objlen-nextiar) 
		return LocateErrorFunc(iar, iarzero, 
			":Object code management failure !!  Quit soon!", ness);
	gocount = 0;
	osi_GetTimes(&starttime);

	if ((exitcode=setjmp(ExecutionExit)) != 0) {
		struct errornode *msg;

		/* return here from longjmp after execution terminates
			 either an error or normal end of execution */

		/* reset the destination of the error longjmp
				(in case popValue fails) */
/* printf("ExitCode: %d\n", exitcode); */
		if (setjmp(ExecutionExit) == 0) {
			/* setjmp returns 0 when first called */
			InterpretationInProgress = NULL;

			if (exitcode == 1)  
				msg = NULL;	/* normal exit */
			else if (exitcode == 8)
				msg = SavedMsg;
			else 
				msg = LocateErrorFunc(iarStore, iarzero, 
						ErrorMsg, ness);

			/* pop stack.  Necessary to free up marks to text */
			NSP = NSPstore;
			while (FramePtr != NULL)
				NSP = popValue(NSP);
		}

		/* restore error traps */
		signal(SIGBUS, oldBus);
		signal(SIGSEGV, oldSeg);

		/* restore global variables */
		/* ExecutionExit = SaveExecExit; */
		bcopy((char *)SaveExecExit, (char *)ExecutionExit,
				sizeof(jmp_buf));
		InterpretationInProgress= SaveInterpInProg;
		FramePtr = SaveFramePtr;

		if (exitcode != 1 && 
				InterpretationInProgress != NULL) {
			/* we erred in a nested execution
			  (most likely from an event within
			  launchApplication)
			  exit from the outer ness. */
/* printf("ReExit for: %s\n", msg->msg); */
			SavedMsg = msg;
			longjmp (ExecutionExit, 8);
		}

		return msg;
	}
	/* when first called setjmp returns zero so execution initially continues here */

	/* catch disasterous errors */
	oldBus = (int (*)())signal(SIGBUS, SigHandler);
	oldSeg = (int (*)())signal(SIGSEGV, SigHandler);

while (TRUE)  {
    iarStore = iar;
    switch(*iar++) {
	default:  RunError(":Illegal Opcode.  Compiler failure!", iar-1);   break;
	case '\n':	break;	/* No-Op */
	case '^': 		/* push NULL */
		NSPushSpace(ptrstkelt);
		NSP->p.hdr = ptrHdr;
		NSP->p.v = NULL;
		break;
	case '_':		/* unary minus */
		if (NSP->l.hdr != longHdr)
			RunError(":operand is not an integer value", iar-1);
		NSP->l.v = - NSP->l.v; 
		break;
	case '+':
	case '-':
	case '*':	
	case '/':
	case '%':  {
		register struct longstkelt *left ;
		if (NSP->l.hdr != longHdr)
			RunError(":right operand is not an integer value", iar-1);
		left = &(&(NSP->l))[1];
		if (left->hdr != longHdr)
			RunError(":left operand is not an integer value", iar-1);
		switch (*(iar-1)) {
		case '+':  left->v += NSP->l.v;   break;
		case '-':  left->v -= NSP->l.v;   break;
		case '*':  left->v *= NSP->l.v;   break;
		case '/':  if (NSP->l.v == 0) RunError(":divide by zero", iar-1); 
			else left->v /= NSP->l.v;   break;
		case '%':  if (NSP->l.v == 0) RunError(":divide by zero", iar-1); 
			else left->v %= NSP->l.v;   break;
		}
		NSP = popValue(NSP);	/* discard right operand */
	}	break;
	case '0': 		/* push zero long */
		NSPushSpace(longstkelt);
		NSP->l.hdr = longHdr;
		NSP->l.v = 0;
		break;
	case '1': 		/* push TRUE */
		NSPushSpace(boolstkelt);
		NSP->b.hdr = boolHdr;
		NSP->b.v = TRUE;	/* use C version for now */
		break;
	case '9': 		/* push FALSE */
		NSPushSpace(boolstkelt);
		NSP->b.hdr = boolHdr;
		NSP->b.v = FALSE;	/* use C version for now */
		break;

	case 'a':		/* branch LT */
		Success = (CondCode == -1);
		goto brancher;
	case 'b':		/* branch GT */
		Success = (CondCode == 1);
		goto brancher;
	case 'c':		/* branch LE */
		Success = (CondCode != 1);
		goto brancher;
	case 'd':		/* branch GE */
		Success = (CondCode != -1);
		goto brancher;
	case 'e':		/* branch EQ */
		Success = (CondCode == 0);
		goto brancher;
	case 'f':		/* branch NE */
		Success = (CondCode != 0);
		goto brancher;
	case 'g':		/* goto */
		Success = TRUE;
		goto brancher;
	case 'h':		/* branch error */
		Success = (CondCode == -2);
		goto brancher;

brancher: {
		long offset;
		unsigned char chi, clo;
		if (gocount++ > 600) {
			if (im_CheckForInterrupt()) 
				InterruptNess(ness);
			if (! Gmessaged) {
				struct osi_Times now;
				osi_GetTimes(&now);
				if (now.Secs > starttime.Secs + 7) {
					message_DisplayString(im_GetLastUsed(), 0, 
	"You can use control-G if your script is stuck in a loop");
					im_ForceUpdate();
					Gmessaged = TRUE;
				}
			}
			gocount = 0;
		}
		chi = *iar++;
		clo = *iar++;
		offset = ExtendShortSign((chi<<8) + clo);
		if (Success) {
			/* set iar to loc indicated by offset from addr of branch */
			PrevAddr = iar - 3;
			iar = PrevAddr + offset;
		}
	}	break;

	case 'i':	{	/* read named file to stack top */
		struct text *t;
		FILE *f;
		unsigned char *s;
		long val;

		PTSTOMARK(NSP, iar-1);	/* file name */
		/* open the file */
		s = nessmark_ToC(&NSP->m);
		f = (*s == '\0') ? NULL : fopen(s, "r");
		NSP = popValue(NSP);		/* discard filename */

		/* create a new mark for the file to read into */
		t = text_New();		
		NSPushSpace(nessmark);
		nessmark_Initialize(&NSP->m);
		nessmark_Set(&NSP->m, t, 0, 0);

		if (f == NULL) 
			fprintf(stderr,  "Ness: cannot read file \"%s\"\n", s);
		else {
			/* read the file into mark on top of stack */
			val = ReadTextFileStream(t, s, f);
			if (val != dataobject_NOREADERROR)
				fprintf(stderr, "Ness: file not in ATK format \"%s\"", s);
			fclose(f);
		}
		free(s);
		nessmark_SetLength(&NSP->m, text_GetLength(t));
	}	break;
	case 'j':	{	/* print string */
		register long i, end;
		register struct simpletext *t;
		PTSTOMARK(NSP, iar-1);
		t = nessmark_GetText(&NSP->m);
		i = nessmark_GetPos(&NSP->m);
		end = i + nessmark_GetLength(&NSP->m);
		while (i < end)
			putchar(simpletext_CorrectGetChar(t, i)), i++;
		/* leave value on stack because print is a function and all such return values. */
	}	break;
	case 'k':	{	/* load string from SysMark */
		register unsigned long strloc;
		strloc = (unsigned long)*iar++ << 8;
		strloc += (unsigned long)*iar++;

		PTSTOMARK(&SysMarkLowEnd[strloc], iar-3);
		PUSHMARK(&SysMarkLowEnd[strloc]);
	}	break;
	case 'l':	{	/* load string from stack */
			/* operand is index of arg from FramePtr */
		register struct nessmark *m 
			= (struct nessmark *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
		PTSTOMARK(m, iar-2);
		PUSHMARK(m);
	}	break;
	case 'm':	{	/* dup */
		register struct nessmark *m;
		PTSTOMARK(NSP, iar-1);
		m = &NSP->m;	/* copy pointer to mark before NSP changes */
		PUSHMARK(m);	
	}	break;
	case 'n':	{	/* string next() */
		register long pos;
		PTSTOMARK(NSP, iar-1);
		nessmark_SetPos(&NSP->m, pos = (nessmark_GetPos(&NSP->m) +
			nessmark_GetLength(&NSP->m)));
		nessmark_SetLength(&NSP->m, 
			(pos < simpletext_GetLength(
				nessmark_GetText(&NSP->m))) 
			? 1 : 0);
	}	break;
	case 'o':		/* string start() */
		PTSTOMARK(NSP, iar-1);
		nessmark_SetLength(&NSP->m, 0);
		break;
	case 'p':		/* string base() */
		PTSTOMARK(NSP, iar-1);
		nessmark_SetPos(&NSP->m, 0);
		nessmark_SetLength(&NSP->m,
			simpletext_GetLength(nessmark_GetText(&NSP->m)));
		break;
	case 'q':	{	/* string newbase() */
		NSPushSpace(nessmark);
		nessmark_Initialize(&NSP->m);
		nessmark_SetText(&NSP->m, text_New());
	}	break;
	case 'r':	{	/* string replace()  top arg is 2nd operand*/
		register struct nessmark *left, *right;
		struct simpletext *stext;
		right = &NSP->m;
		left = &(&(NSP->m))[1];
		PTSTOMARK(right, iar-1);
		PTSTOMARK(left, iar-1);
		stext = nessmark_GetText(left);
		if (simpletext_GetReadOnly(stext)) {
			NSPstore = NSP;
			QueryReadOnly(ness, stext, 
				":cannot replace any part of constant", iar-1);
		}

		nessmark_Replace(left, right);

		NSP = popValue(NSP);	/* discard right operand */
	}	break;
	case 's':	{	/* store string to stack */
			/* operand is index of arg from FramePtr */
		register struct nessmark *m		/* where to store */
			= (struct nessmark *)((unsigned long)FramePtr 
					+ sizeof(struct frameelt)
					+ (unsigned long)*iar++);
		PTSTOMARK(&NSP->m, iar-2);

		if ((TType)m->header.nessmark_methods != nessmarkHdr) {
			/* we are storing into an area which was not a mark */	
			nessmark_InitFrom(m, &NSP->m);
		}
		else 
			nessmark_SetFrom(m, &NSP->m);
		NSP = popValue(NSP);
	}	break;
	case 't':	{	/* compare strings */
		register long i, j, k;
		long len, ilen, jlen, d;
		register struct simpletext *itext, *jtext;
		register struct nessmark *left, *right;

		right = &NSP->m;
		left = &(&(NSP->m))[1];
		PTSTOMARK(right, iar-1);
		PTSTOMARK(left, iar-1);

		i = nessmark_GetPos(left);
		ilen = nessmark_GetLength(left);
		j = nessmark_GetPos(right);
		jlen = nessmark_GetLength(right);
		len = ilen;  if (jlen < len) len = jlen;	/* len = MIN(ilen, jlen); */
		itext = nessmark_GetText(left);
		jtext = nessmark_GetText(right);

		for (k=0  ;  k < len;  k++, i++, j++)
			if ((d = ((long)simpletext_CorrectGetChar(itext, i)) 
					- ((long)simpletext_CorrectGetChar(jtext, j))) != 0)
				break;
		if (k == len) 
			/* both equal until go off the end of one 
				the longer is the greater */
			CondCode = (ilen > jlen)  ?  1  :  (ilen == jlen)  ?  0  :  -1;
		else 
			/* reached an unequal character */
			CondCode = (d>0)  ?  1  :  -1;

		NSP = popValue(NSP);	/* discard right operand */
		NSP = popValue(NSP);	/* discard left operand */
	}	break;
	case 'u':		/* compare string to EMPTY  (is = or >) */
		PTSTOMARK(&NSP->m, iar-1);
		CondCode = (nessmark_GetLength(&NSP->m) == 0)  ?  0  :  1;
		NSP = popValue(NSP);	/* discard operand */
		break;
	case 'v':	{	/*store string to SysMark */
		register struct nessmark *m;
		register unsigned long strloc;
		strloc = (unsigned long)*iar++ << 8;
		strloc += (unsigned long)*iar++;
		m = &SysMarkLowEnd[strloc];	/* where to store */

		PTSTOMARK(&NSP->m, iar-3);	/* check the value to be stored */

		if ((TType)m->header.nessmark_methods != nessmarkHdr) {
			/* we are storing into an area which was not a nessmark */
			nessmark_InitFrom(m, &NSP->m);
		}
		else
			nessmark_SetFrom(m, &NSP->m);
		NSP = popValue(NSP);	/* discard value stored */
	}	break;
	case 'w':	{	/* previous(), nextn(), length()*/
		unsigned char subop = *iar++;
		long n;
		switch (subop) {
		case 'p':
			PTSTOMARK(&NSP->m, iar-2);
			if (nessmark_GetPos(&NSP->m) > 0) {
				nessmark_SetPos(&NSP->m, 
					nessmark_GetPos(&NSP->m)  - 1);
				nessmark_SetLength(&NSP->m, 1);
			}
			else nessmark_SetLength(&NSP->m, 0);
			break;
		case 'n':
			if (NSP->l.hdr != longHdr)
				RunError(":tried to nextn with non-integer value", iar - 2);
			n = NSP->l.v;
			NSP = popValue(NSP);
			/* leave mark on stack and revise its value */
			PTSTOMARK(&NSP->m, iar-2);
			nessmark_NextN(&NSP->m, n);
			break;
		case 'l':
			PTSTOMARK(&NSP->m, iar-2);
			n = nessmark_Length(&NSP->m);
			NSP = popValue(NSP);
			/* push n */
			NSPushSpace(longstkelt); 
			NSP->l.hdr = longHdr;
			NSP->l.v = n;
			break;
		}
	}	break;
	case 'x':	{	/* string extent()  top arg is 2nd operand*/
		register struct nessmark *left, *right;
		right = &NSP->m;
		left = &(&(NSP->m))[1];
		PTSTOMARK(right, iar-1);
		PTSTOMARK(left, iar-1);
		if (nessmark_GetText(left) != nessmark_GetText(right))
			nessmark_Set(left, EmptyText, 0, 0);
		else {
			register start = nessmark_GetPos(left);
			register end = nessmark_GetPos(right) 
				+ nessmark_GetLength(right);
			if (end < start)
				start = end;
			nessmark_SetPos(left, start);
			nessmark_SetLength(left, end - start);
		}
		NSP = popValue(NSP);	/* discard right operand */
	}	break;
	case 'y':			/*  pop  */
		NSP = popValue(NSP);
		break;
	case 'z':	{		/* swap top two operand strings */
		register struct nessmark *second, *topmost;
		struct nessmark tempmark;
		topmost = &NSP->m;
		second = &(&(NSP->m))[1];
		PTSTOMARK(topmost, iar-1);
		PTSTOMARK(second, iar-1);
		if (nessmark_GetText(second) == nessmark_GetText(topmost)) {
			tempmark = *second;
			*second = *topmost;
			*topmost = tempmark;
			/* same base, unswap the links */
			nessmark_SetNext(topmost, (struct mark *)second);
			nessmark_SetNext(second, nessmark_GetNext(&tempmark));
		}
		else {
			struct simpletext *topt, *sect;
			topt = nessmark_GetText(topmost);
			sect = nessmark_GetText(second);
			/* different bases, redo links from texts */
			nessmark_DetachFromText(topmost);
			nessmark_DetachFromText(second);
			tempmark = *second;
			*second = *topmost;
			*topmost = tempmark;
			nessmark_AttachToText(topmost, sect);
			nessmark_AttachToText(second, topt);
		}
	}	break;	

	case 'A':	{	/* append top value on stack to second */
		register struct nessmark *source, *rcvr;
		long rcvrlen;
		struct simpletext *stext;

		source = &NSP->m;
		rcvr = &(&(NSP->m))[1];
		PTSTOMARK(rcvr, iar-1);
		PTSTOMARK(source, iar-1);
		stext = nessmark_GetText(rcvr);
		if (simpletext_GetReadOnly(stext)) {
			NSPstore = NSP;
			QueryReadOnly(ness, stext, ":cannot append to constant", iar-1);
		}

		/* ensure that rcvr is the finish of its base */
		rcvrlen = simpletext_GetLength(stext);
		nessmark_SetPos(rcvr, rcvrlen);
		nessmark_SetLength(rcvr, 0);

		/* put source into rcvr and compute base()*/
		nessmark_Replace(rcvr, source);
		nessmark_SetPos(rcvr, 0);
		nessmark_SetLength(rcvr, rcvrlen + nessmark_GetLength(source));

		NSP = popValue(NSP);	/* discard source */
	}	break;
	case 'B':	{	/* boolean operations */
		unsigned char *opiar = iar-1;
		unsigned char op = *iar++;
		struct boolstkelt *src, *dest;
		register unsigned long sysloc;
		switch (op) {
		case 'k':		/* load from sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			src = (struct boolstkelt *)&SysMarkLowEnd[sysloc];
			goto Bload;
		case 'l':		/* load from stack */
			src = (struct boolstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
		Bload:
			if (src->hdr != boolHdr)
				RunError(":tried to load non-Boolean value", opiar);
			NSPushSpace(boolstkelt); 
			NSP->b = *src;
			break;
		case 's':		/* store to stack */
			dest = (struct boolstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
			goto Bstore;
		case 'v':		/* store to sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			dest = (struct boolstkelt *)&SysMarkLowEnd[sysloc];
		Bstore:
			if (NSP->b.hdr != boolHdr)
				RunError(":tried to store non-Boolean value", opiar);
			*dest = NSP->b;
			NSPopSpace(boolstkelt);
			break;
		case 't':		/* compare */
			src = &(&(NSP->b))[1];
			if (NSP->b.hdr != boolHdr)
				RunError(":right operand is not a boolean value", opiar);
			if (src->hdr != boolHdr)
				RunError(":left operand is not a boolean value", opiar);
			CondCode = (NSP->b.v == src->v) ? 0 :
					(src->v < NSP->b.v) ? -1 : 1;
			NSP = popValue(NSP);	/* discard right operand */
			NSP = popValue(NSP);	/* discard left operand */
			break;
		}
	}	break;
	case 'C':	{	/* call an unknown function on the object atop stack */
			/* operand is four bytes giving address of a callnode  */
		register union {
			struct callnode *call;
			unsigned long symloc;
		} pcall;
		unsigned char c0, c1, c2, c3;
		c0 = *iar++;	/* hi end */
		c1 = *iar++;
		c2 = *iar++;
		c3 = *iar++;	/* lo end */
		pcall.symloc = (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;

		NSPstore = NSP;
		PrevAddr = iar;
		if (pcall.call->variety == callSym)
			/* go find out what is being called and check arg types
				This will CHANGE pcall.call->variety */
			callCheck(pcall.call, iar-5, ness);

		/* now execute function as C code or Ness code */
		if (pcall.call->variety == callNess) {
			/* (XXX) (we could modify the object code to 
				be a direct call 'O' instead of indirect 'C') */
			nextiar = nessmark_GetPos(&SysMarkLowEnd
						[pcall.call->where.Nproc]);
			iar = (unsigned char *)simpletext_GetBuf(ObjectCode, nextiar, 1, &lengthgotten);
		}
		else {
			/* callC, callPE, callGet, callSet, 
				callMethod, callClProc */
			callCfunc(pcall.call, iar-5, ness);
			NSP= NSPstore;
		}
	}	break;
	case 'E':		/* load to stack top a pointer to the current textview object */
		if (ness_GetDefaultText(ness) == NULL)
			RunError(":there is no default text", iar-1);
		NSPushSpace(ptrstkelt);
		NSP->p.hdr = ptrHdr;
		NSP->p.v = (struct basicobject *)ness_GetDefaultText(ness); 
		break;
	case 'F': {		/* one of the search functions */
		unsigned char op = *iar++;
		SearchOp(op, iar-2);
		NSP= NSPstore;
	}	break;
	case 'G':	 {	/* make inset have input focus */
		struct view *v;
		if (NSP->p.hdr != ptrHdr || NSP->p.v == NULL 
				|| ! class_IsType(NSP->p.v, viewClass)) 
			 RunError(":Arg was not a pointer to a view", iar-1);
		else v = (struct view *)NSP->p.v;
		NSP = popValue(NSP);	/* discard view pointer */
		view_WantInputFocus(v, v);	  /* try to give it the input focus */
	}	break;
	case 'H': {	/* unary operations on real values */
		unsigned char op = *iar++;
		realUnary(op, iar-2);
	}	break;
	case 'I':	{	/* integer operations */
		unsigned char *opiar = iar-1;
		unsigned char op = *iar++;
		register struct longstkelt *src, *dest;
		register unsigned long sysloc;
		register long t;
		switch (op) {
		case 'k':		/* load from sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			src = (struct longstkelt *)&SysMarkLowEnd[sysloc];
			goto Iload;
		case 'l':		/* load from stack */
			src = (struct longstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
		Iload:
			if (src->hdr != longHdr)
				RunError(":tried to load non-integer value", opiar);
			NSPushSpace(longstkelt); 
			NSP->l = *src;
			break;
		case 's':		/* store to stack */
			dest = (struct longstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
			goto Istore;
		case 'v':		/* store to sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			dest = (struct longstkelt *)&SysMarkLowEnd[sysloc];
		Istore:
			if (NSP->l.hdr != longHdr)
				RunError(":tried to store non-integer value", opiar);
			*dest = NSP->l;
			NSPopSpace(longstkelt);
			break;
		case 't':		/* compare */
			src = &(&(NSP->l))[1];
			if (NSP->l.hdr != longHdr)
				RunError(":right operand is not an integer value", opiar);
			if (src->hdr != longHdr)
				RunError(":left operand is not an integer value", opiar);
			t = src->v - NSP->l.v;
			CondCode = (t == 0) ? 0 : (t<0) ? -1 : 1;
			NSP = popValue(NSP);	/* discard right operand */
			NSP = popValue(NSP);	/* discard left operand */
			break;
		}
	}	break;
	case 'J':	{	/* call one of various object functions */
		unsigned char op = *iar++;
		callCheat(op, iar-2, ness);
		NSP = NSPstore;
	}	break;
	case 'K':	{	/* dokeys(obj, keys)  or DoMenu(obj, item) */
		struct view *OldIF;
		struct ptrstkelt *p;
		struct view *t;
		struct im *im;
		unsigned char *option;
		unsigned char op = *iar++;

		PTSTOMARK(&NSP->m, iar-2);	    /* check option (the 2nd arg) */
		p = (struct ptrstkelt *)((unsigned long)NSP + sizeof(struct nessmark));
		t = (struct view *)p->v;
		if (p->hdr != ptrHdr  || t == NULL 
				|| ! class_IsType(t, viewClass))
			 RunError(":first argument was not a view", iar-2);
		/*  can't use ProperPtr because we do not know the type of the child of the cel */
		if (t == ness->CurrentInset  &&  class_IsType(t, celviewClass)) {
			/* is celview:  use application or truechild */
			if (celview_GetApplication((struct celview *)t) != NULL)
				t = celview_GetApplication((struct celview *)t);
			else if (celview_GetTrueChild((struct celview *)t) != NULL)
				t = celview_GetTrueChild((struct celview *)t);
		}
		im = view_GetIM(t);
		if (im == NULL) goto pop1;
		OldIF = im_GetInputFocus(im);
		if (t != OldIF) {
			/* try to give the input focus to t */
			view_WantInputFocus(t, t);
			if (im_GetInputFocus(im) != t)
				/* t did not get IF.  Give up ? XXX */
				goto pop1;
		}
		option = nessmark_ToC(&NSP->m);
		if (op == 'k')
			im_DoKeySequence(im, option);
		else if (op == 'm')
			im_DoMenu(im, option);
		free(option);

		if (t != OldIF)
			/* restore input focus */
			view_WantInputFocus(OldIF, OldIF);
	pop1:
		NSP = popValue(NSP);	/* discard operand, leave one value to pop */
	}	break;
	case 'L':	{	/* convert value to string */
		char buf[50];
		if (NSP->p.hdr == ptrHdr)  {
			if (NSP->p.v == NULL)
				sprintf(buf, "NULL");
			else  sprintf(buf, "0x%lx", NSP->p.v);
		}
		else if (NSP->b.hdr == boolHdr) {
			if (NSP->b.v == TRUE) strcpy(buf, "TRUE");
			else if (NSP->b.v == FALSE) strcpy(buf, "FALSE");
			else sprintf(buf, "0x%lx", NSP->b.v);
		}
		else if (NSP->l.hdr == longHdr)
			sprintf(buf, "%d", NSP->l.v);
		else if (NSP->d.hdr == dblHdr) {
			char *bx;			
#if !defined(VAX_ENV) && !defined(PMAX_ENV)
#if (! SY_U5x && ! SY_AIXx)
			if (isnan(NSP->d.v) == 1)
				sprintf(buf, "*NaN*");
			else
#endif /* (!SYSV && !AIX) */
#endif /* !defined(VAX_ENV) && !defined(PMAX_ENV) */
			    sprintf(buf, "%0.5g", NSP->d.v);
			for (bx = buf; *bx != '\0' 
					&& *bx != '.'
					&& *bx != 'e'  
					&& *bx != 'N';    bx++)  {}
			if (*bx == '\0')
				/* value does not indicate it is real */
				strcat(buf, ".");   /* make it real */
		}
		else if ((TType)NSP->m.header.nessmark_methods == nessmarkHdr) {
			/* leave the arg as the value
				XXX should probably make a copy  */
			break;
		}
		else RunError(":cannot convert value to string", iar-1);
		NSP = popValue(NSP);	/* discard value */
		NSPushSpace(nessmark);
		nessmark_Initialize(&NSP->m);
		nessmark_MakeConst(&NSP->m, buf);
	}	break;
	case 'M': {	/* operations on real numbers */
		unsigned char op = *iar++;
		realOther(op, iar-2);
		NSP = NSPstore;
	}	break;
	case 'N':		/* print a newline */
		putchar('\n');
		break;
	case 'O':	{	/* call a function */
			/* operand is two bytes giving index into SysMark */
		register unsigned long funcloc;
		long lengthgotten;
		if (gocount++ >600) {
			if (im_CheckForInterrupt()) 
				InterruptNess(ness);
			else gocount = 0;
		}
		funcloc = (unsigned long)*iar++ << 8;
		funcloc += (unsigned long)*iar++;
		PrevAddr = iar;
		nextiar = nessmark_GetPos(&SysMarkLowEnd[funcloc]);
		iar = (unsigned char *)simpletext_GetBuf(ObjectCode, nextiar, 1, &lengthgotten);
	}	break;
	case 'P':	{	/* enter a function */
			/* operand is number of bytes of locals */
		register unsigned long NlocBytes = (unsigned long)*iar++;
		unsigned long *t = (unsigned long *)NSP;

		NSP = (union stackelement *)((unsigned long)NSP - NlocBytes);
		/* NSPstore gets set with NSPushSpace(frameelt) below */

		if (NSP - FUNCSTACKSLOP <= NSLowEnd)
			RunError(":stack overflow on function entry", iar-2);

		/* zero out the locals */
			/* bzero(NSP, NlocBytes); */
		while (--t >= (unsigned long *)NSP) 
			*t = NULL; 

		NSPushSpace(frameelt);	
		NSP->f.hdr = frameHdr;
		NSP->f.returnaddress = PrevAddr;  
		NSP->f.prevframe = FramePtr;
		FramePtr = &NSP->f;
	}	break;
	case 'Q':  {	/* return from function call */
		register long eltsize;
		boolean ismark = FALSE;
		struct simpletext *oldtext;
		register union stackelement *tsp = NSP,
			*targ = (union stackelement *)(((unsigned long)FramePtr) 
					+ sizeof(struct frameelt)
					+(unsigned long)*iar++);
		iar = FramePtr->returnaddress;
		switch (tsp->l.hdr) {
		      case (longHdr):
			eltsize = sizeof(struct longstkelt);  break;
		      case (boolHdr):
			eltsize = sizeof(struct boolstkelt);  break;
		      case (dblHdr):
			eltsize = sizeof(struct dblstkelt);  break;
		      case (ptrHdr):
			eltsize = sizeof(struct ptrstkelt);  break;
		      default:
			if (tsp->l.hdr == nessmarkHdr) {
				ismark = TRUE;
				eltsize = sizeof(struct nessmark);
		     	}
			else 
				/* ERROR - unknown value type; return 0 */
				eltsize = 0;
			break;
		}

		/* pop everything off stack, unlinking marks
			and resetting FramePtr  */
		/* but first skip over return value */
		NSP = (union stackelement *)((unsigned long)NSP + eltsize); 
		while (NSP < targ) 
			NSP = popValue(NSP);

		if (eltsize > 0) {
			/* copy return value, last word first */
			long nwords = eltsize/sizeof(unsigned long);
			register unsigned long *src, *dest;
			src = (unsigned long *)tsp + nwords;
			dest = (unsigned long *)NSP;
			NSP = (union stackelement *)((unsigned long) NSP - eltsize);
			NSPstore = NSP;
			/* it is (allegedly) slower to unroll the following loop
				because it fits in the instruction cache */
			while (src > (unsigned long *)tsp) 
				 *--dest = *--src;
			if (ismark) {
				oldtext = nessmark_GetText(&tsp->m);
				nessmark_DetachFromText(&tsp->m);
				nessmark_AttachToText(&NSP->m, oldtext);
			}
		}
		else {
			/* unknown type, push 0L */
			NSPushSpace(longstkelt);
			NSP->l.hdr = longHdr;
			NSP->l.v = 0;
		}
		if (iar == 0) 
			longjmp(ExecutionExit, 1);
	} 	break;	
	case 'R': {	/* operations on real numbers */
		unsigned char *opiar = iar-1;
		unsigned char op = *iar++;
		register struct dblstkelt *src, *dest;
		register unsigned long sysloc;
		double t;
		switch (op) {
		case 'k':		/* load from sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			src = (struct dblstkelt *)&SysMarkLowEnd[sysloc];
			goto Rload;
		case 'l':		/* load from stack */
			src = (struct dblstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
			goto Rload;
		Rload:
			if (src->hdr != dblHdr)
				RunError(":tried to load non-real value", opiar);
			NSPushSpace(dblstkelt); 
			NSP->d = *src;
			break;
		case 's':		/* store to stack */
			dest = (struct dblstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
			goto Rstore;
		case 'v':		/* store to sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			dest = (struct dblstkelt *)&SysMarkLowEnd[sysloc];
			goto Rstore;
		Rstore:
			if (NSP->d.hdr != dblHdr)
				RunError(":tried to store non-real value", opiar);
#if !defined(VAX_ENV) && !defined(PMAX_ENV)
#if (! SY_U5x && ! SY_AIXx)
			if (isnan(NSP->d.v) == 1)
				RunError(":tried to store NaN value", opiar);
#endif /* (!SYSV && !AIX) */
#endif /* !defined(VAX_ENV) && !defined(PMAX_ENV) */
			*dest = NSP->d;
			NSPopSpace(dblstkelt);
			break;
		case 't':		/* compare */
			src = &(&(NSP->d))[1];
			if (NSP->d.hdr != dblHdr)
				RunError(":right operand is not a real value", opiar);
			if (src->hdr != dblHdr)
				RunError(":left operand is not a real value", opiar);
			t = src->v - NSP->d.v;
			CondCode = (t == 0) ? 0 : (t<0) ? -1 : 1;
			NSP = popValue(NSP);	/* discard right operand */
			NSP = popValue(NSP);	/* discard left operand */
			break;
		}
	}	break;
	case 'S':	{	/* get current selection as a marker */
		struct textview *v;
		unsigned char *opiar = iar-1;
		unsigned char op = *iar++;
		if (NSP->p.hdr != ptrHdr ||  
				(v=(struct textview *)ProperPtr((struct basicobject *)NSP->p.v,
						textviewClass)) == NULL) 
			 RunError(":Arg was not a pointer to a textview", opiar);
		NSP = popValue(NSP);	/* discard view pointer */
		NSPushSpace(nessmark);
		nessmark_Initialize(&NSP->m);
		if (v == NULL)  {}
		else if (op == 's') {
			/* currentselection(textview) */
			nessmark_Set(&NSP->m, 
				(struct simpletext *)v->header.view.dataobject,
				textview_GetDotPosition(v),
				textview_GetDotLength(v));
		}
		else {
			/* op == 'm' :  currentmark(textview) */ 
			nessmark_Set(&NSP->m, 
				(struct simpletext *)v->header.view.dataobject,
				mark_GetPos(v->atMarker),
				mark_GetLength(v->atMarker));
		}
	}	break;	
	case 'T':		/* test TOS for TRUE (or FALSE) */
		if (NSP->b.hdr != boolHdr) 
			RunError(":Not a pointer to a mark", iar);
		CondCode = (NSP->b.v == TRUE) ? 0 : -1;
		NSP = popValue(NSP);	/* discard value */
		break;
	case 'U':	{	/* call a user interface information function */
		unsigned char op = *iar++;
		neventInfo(op, iar-2, ness);
		NSP = NSPstore;
	}	break;
	case 'V':	{	/* pointer operations */
		unsigned char *opiar = iar-1;
		unsigned char op = *iar++;
		register struct ptrstkelt *src, *dest;
		register unsigned long sysloc;
		register long t;
		switch (op) {
		case 'k':		/* load from sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			src = (struct ptrstkelt *)&SysMarkLowEnd[sysloc];
			goto Vload;
		case 'l':		/* load from stack */
			src = (struct ptrstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
		Vload:
			if (src->hdr != ptrHdr)
				RunError(":tried to load non-pointer value", opiar);
			NSPushSpace(ptrstkelt); 
			NSP->p = *src;
			break;
		case 's':		/* store to stack */
			dest = (struct ptrstkelt *)((unsigned long)FramePtr
					+ sizeof(struct frameelt) 
					+ (unsigned long)*iar++);
			goto Vstore;
		case 'v':		/* store to sysmark */
			sysloc = (unsigned long)*iar++ << 8;
			sysloc += (unsigned long)*iar++;
			dest = (struct ptrstkelt *)&SysMarkLowEnd[sysloc];
		Vstore:
			if (NSP->p.hdr != ptrHdr)
				RunError(":tried to store non-pointer value", opiar);
			*dest = NSP->p;
			NSPopSpace(ptrstkelt);
			break;
		case 't':		/* compare */
			src = &(&(NSP->p))[1];
			if (NSP->p.hdr != ptrHdr)
				RunError(":right operand is not a pointer value", opiar);
			if (src->hdr != ptrHdr)
				RunError(":left operand is not a pointer value", opiar);
			t = ((long)src->v) - ((long)NSP->p.v);
			CondCode = (t == 0) ? 0 : (t<0) ? -1 : 1;
			NSP = popValue(NSP);	/* discard right operand */
			NSP = popValue(NSP);	/* discard left operand */
			break;
		}
	}	break;
	case 'W':{		/* write second stack elt to file named by topmost */
				/* the writefile() function in ness has file name as 1st arg
					a 'swap' is compiled for that function
					prior to the call on this operator,
					so the contents can be returned as
					the value of the writefile */
		register struct nessmark *contents, *filename;
		struct text *t, *tempt;
		unsigned char *s;
		FILE *f;
		long pos, len;

		filename = &NSP->m;
		contents = &(&(NSP->m))[1];
		PTSTOMARK(contents, iar-1);
		PTSTOMARK(filename, iar-1);

		/* open the file */
		f = fopen(s=nessmark_ToC(filename), "w");
		free(s);
		if (f == NULL) {
			/* return start(arg) */
			NSP = popValue(NSP);	/* discard filename */
			nessmark_SetLength(&NSP->m, 0);	/* nessmark_Start */
			break;
		}

		/* write contents to file */
		pos = nessmark_GetPos(contents);
		len = nessmark_GetLength(contents);
		t = (struct text *)nessmark_GetText(contents);
		if (environment_NumberOfChildren(t->rootEnvironment) > 0
				&& t->templateName == NULL) {
			/*  There's at least one style, and no template.  Read one.  */
			struct stylesheet *s =text_GetStyleSheet(t);
			char *sstpl;
			text_ReadTemplate(t, 
				(s != NULL
					&&  (sstpl=stylesheet_GetTemplateName(s)) 
						!= NULL
					&&  *sstpl != '\0')
				    ? sstpl : "default",      FALSE);
		}

		if (pos == 0  &&  len == text_GetLength(t)) 
			/* write the whole thing */
			text_Write(t, f, im_GetWriteID(), 0);
		else {
			/* copy to a temp text to do the substring */
			tempt = text_New();
			text_AlwaysCopyText(tempt, 0, t, pos , len);
			text_Write(tempt, f, im_GetWriteID(), 0);
			text_Destroy(tempt);
		}
		fclose(f);
		NSP = popValue(NSP);	/* discard filename */

	}	break;
	case 'X':	 {		/* system() */
		unsigned char *buf, *combuf;
		FILE *output;
		unsigned char frombuf[1025];
		struct simpletext *t;
		long i, end;
				
		PTSTOMARK(&NSP->m, iar-1);
		buf = nessmark_ToC(&NSP->m);
		i = strlen(buf);
		combuf = (unsigned char *)malloc(i+20);
		sprintf(combuf, "(%s)</dev/null", buf);		/* supply empty stdin */
		free(buf);
		NSP = popValue(NSP);	/* discard the argument */

		/* create a new mark for the file to read into  */
		NSPushSpace(nessmark);
		nessmark_Initialize(&NSP->m);
		t = simpletext_New();
		nessmark_Set(&NSP->m, t, 0, 0);

		output = popen(combuf, "r");
		free(combuf);

		end = 0;
		if (output != NULL) {
			/* now read the file into mark on top of stack */
			while ((i=fread(frombuf, sizeof(*frombuf), sizeof(frombuf), output)) > 0) {
				simpletext_InsertCharacters(t, end, frombuf, i);
				end += i;
			}
			pclose(output);
		}
		else {
			static char *msg = "ERROR: Could not execute command";
			simpletext_InsertCharacters(t, end, msg, strlen(msg));
			end += strlen(msg);
		}
		nessmark_SetLength(&NSP->m, end);
	}	break;

	case 'Y':	{	/*  setcurrentselection(textview, mark) */
		struct ptrstkelt *p;
		struct textview *t;
		PTSTOMARK(&NSP->m, iar-1);
		p = (struct ptrstkelt *)((unsigned long)NSP + sizeof(struct nessmark));
		if (p->hdr != ptrHdr  ||
				(t=(struct textview *)ProperPtr((struct basicobject *)p->v,
					textviewClass)) == NULL)
			 RunError(":first argument was not a textview", iar-1);
		if ((struct simpletext *)t->header.view.dataobject
					== nessmark_GetText(&NSP->m))  {
			textview_SetDotPosition(t, nessmark_GetPos(&NSP->m));
			textview_SetDotLength(t, nessmark_GetLength(&NSP->m));
			textview_FrameDot(t, nessmark_GetPos(&NSP->m));
		}
		else
			 RunError(":marker was not for text in the given object", iar-1);
		NSP = popValue(NSP);	/* discard selection marker */
		NSP = popValue(NSP);	/* discard object */
	}	break;
	case 'Z':		/* last() */
		PTSTOMARK(&NSP->m, iar-1);
		if (nessmark_GetLength(&NSP->m) > 0) {
			nessmark_SetPos(&NSP->m, 
				nessmark_GetPos(&NSP->m)
					+ nessmark_GetLength(&NSP->m)
					-1);
			nessmark_SetLength(&NSP->m, 1);
		}
		break;

    }	/* end switch(*iar++) */
}	/* end while(TRUE) */

}	/* end interpretNess() */

