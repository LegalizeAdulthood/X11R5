/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nessv.ch,v 1.9 1991/09/12 19:44:25 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nessv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nessv.ch,v 1.9 1991/09/12 19:44:25 bobg Exp $";
#endif
/*
nessview.H

	Class definitions for ness view

*/
/*
 *    $Log: nessv.ch,v $
Revision 1.9  1991/09/12  19:44:25  bobg
Update copyright notice

Revision 1.8  1989/06/23  17:17:05  wjh
(Items that should not be sent to downstairs are marked "R4".)

Added a call to CheckForInterrupt in the function calls and internal gotos.  Thus loops can be terminated with ^G.   (interp.c)  R4

Changed dokeys() so it uses im_DoKeySequence.  This means it is now possible to send a key sequence which involves the message line.  (interp.c)  R4

Implemented DoMenu(object, menustring).   This function causes the same behavior as if the user selected the menu option.  At present the menu string must be exactly as originally defined;  see the warning above for im_DoMenu().  (interp.c, call.c)  R4

Changed access to Ness library routines so they are always compiled.  (They used ot get the default of NoCompilation, so they were useless.)  (call.c)

Removed a superflous {} pair.  {This is purely a cosmetic change.}  (nessmark.c)

Fixed value_GetStringValue.  Formerly it was getting an invalid initial value.  {The fix was done by adding a call to nessmark_Initialize() in the stackString section.}  (nevent.c)

Modified the data stream so errors will not occur when a ness object is the outermost object.  The fix was to add two bytes, "00", at the end of the origin string to prevent the former occurence of a spurious "\}".  (ness.c)

Fixed menu handling so Ness and child menus get posted when there is a mouse click in an inset within the Ness.  Formerly neither set of menus was posted. (nessv.c)

Fixed dostmt, the function called from ness-load, which is recommended to be bound to ESC-ESC.  It was using a NULL pointer, so people were getting core dumps if they typed ESC-ESC before doing a compile.  (ness.c)

Avoided an infinite loop which occurred if a library function referred to a non-existent entry point within itself.  Did this by checking to see if the library function is already Compiling just before trying to compile it.  (call.c call.hn, call.h)

Revised system marker allocation so the compilation will not get a subsequent error.  (gen.c)

Revised system marker allocation so it expands the space available if necessary. This will make it possible to compile larger programs.  (gen.c)

Changed the type of TType to long from struct object *.  This will allow compilation on stricter compilers.  (interp.h)

Fixed nessmark_FinalizeObject so it would not reference a NULL pointer.  {Somehow the assembler noticed this bug!}  (nessmark.c)

Changed functions which deal with constant strings to have (char *) as there argument type (SaveError, exprnode_Create, ReportError, ExprError, RunError, LocateErrorFunc, QueryReadOnly, makeConst, printallerrors) or return type (Freeze, argcounterr, argtypeerr).  This prevents compile errors on picky compilers.  (interp.c, error.c, call.c, ness.c, nessv.c, search.c, nevent.c, nessmark.c, nessrun.ci)  R4

Changed Imakefile to store Imakefile in checkin rule.  (Imakefile)

Revision 1.7  89/06/01  15:57:50  wjh
campus release version

Revision 1.3  88/12/07  22:44:12  wjh

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


Revision 1.2  88/11/16  14:05:04  wjh

implemented access level control
skip first line of script if starts with # (for shell script)
changed so the name of all initialization functions is init()
added ness-load
moved execution of init() from compilation to first execution
deferred compilation to first FullUpdate time

Revision 1.1  88/10/21  11:01:12  wjh
Initial revision

 * Revision 1.0  88/04/27  14:28:55  wjh
 * Copied from /usr/andrew/lib/dummy
 */



class nessview [nessv] : scroll
{
overrides:

	FullUpdate(/* struct nessview *self, */ enum view_UpdateType type,
			long left, long top, long width, long height);
	Update(/* struct nessview *self */);
	Hit (/* struct nessview *self, */ enum view_MouseAction action, 
			long x, long y, long numberOfClicks) returns struct view *;
	DesiredSize(/* struct nessview *self, */ long width, long height, 
			enum view_DSpass pass, long *desiredWidth, long *desiredHeight) 
			returns enum view_DSattributes;
	SetDataObject(/* struct nessview *self, */ struct dataobject *dobj);

	WantInputFocus(/* struct nessview *self, */ struct view *requestor);
	ReceiveInputFocus(/* struct nessview *self */);
	LoseInputFocus(/* struct nessview *self */);
	PostMenus (/* struct nessview *self, */ struct menulist *menulist);
	PostKeyState (/* struct nessview *self, */ struct keystate *keystate);

	Print(/* struct nessview *self, */ FILE *file,
			char *processor, char *finalFormat, boolean topLevel);
	ObservedChanged(/* struct nessview *self, */ struct ness *changed, long value);

methods:
	FirstError(/* struct nessview *self */);
		/* display message and highlight for first error */

macromethods:
	SetSubject(/* struct nessview *self,  struct textview *  */ textview)  \
		(self->Subject = textview)
		/* establish the 'textview' as the value of defaulttext() */

classprocedures:

	InitializeClass(/* struct classhdr *ClassID*/ ) returns boolean; 
			/* Create default Keymap & Menus */
	InitializeObject(/* struct classhdr *ClassID;*/ struct nessview *self) returns boolean;
	FinalizeObject(/* struct classhdr *ClassID;*/ struct nessview *self);

data:
	struct textview *SourceText;	/* display the ness script */
	char *ExecFunction;		/* default function for ^X^E */
	struct errornode *CurrError;		/* which one is being displayed */

	long compmod;			/* GetModified at last compile */
	long ButtonPending;		/* used in __Hit */

	struct menulist  *Menus;
	struct keystate *Keystate;

	boolean MenusPostedRecently;	/* internal to PostMenus, RcvIF,  */
	boolean KeystatePostedRecently; 	/*    and FullUpdate  */

	boolean HasIF;
	boolean ErrorHighlightPending;

	boolean inverted;
	boolean dialogboxup;
	boolean justscanned;	/* if most recent compile was a scan */

};
