/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/gob/RCS/gobv.ch,v 1.7 1991/09/12 19:40:37 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/gob/RCS/gobv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/gob/RCS/gobv.ch,v 1.7 1991/09/12 19:40:37 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 * P_R_P_Q_# (C) COPYRIGHT IBM CORPORATION 1987, 1988
 * LICENSED MATERIALS - PROPERTY OF IBM
 * REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
 */
/*
gobview.H

	Class definitions for gob view

*/
/*
 *    $Log: gobv.ch,v $
Revision 1.7  1991/09/12  19:40:37  bobg
Update copyright notice

Revision 1.6  1990/05/08  16:25:46  gk5g
Added instance variable to hold the AppMenus so we can destroy them in FinalizeObject.

Revision 1.5  89/02/20  12:50:49  ghoti
class syntax changes

Revision 1.4  89/02/17  16:16:49  ghoti
ifdef/endif,etc. label fixing - courtesy of Ness

Revision 1.3  89/02/08  16:20:31  ghoti
change copyright notice

Revision 1.2  89/02/07  10:58:06  ghoti
first pass porting changes: filenames and references to them

Revision 1.1  88/12/23  16:03:14  tpn
Initial revision
 
 * Revision 1.0  88/12/15  10:04:25  tpn
 * Copied from /usr/andrew/lib/nullinset
 */

#ifndef timercmp
#include <sys/time.h>
#endif /* timercmp */




class gobview[gobv] : view
{
overrides:

	FullUpdate(/* struct gobview *self, */ enum view_UpdateType type, 
			long left, long top, long width, long height);
	Update(/* struct gobview *self, */);
	Hit(/* struct gobview *self, */ enum view_MouseAction action, 
			long x, long y, long n)	returns struct view *;
	DesiredSize(/* struct gobview *self, */ long width, long height, 
			enum view_DSpass pass, long *desiredWidth, long *desiredHeight) 
			returns enum view_DSattributes;
	ReceiveInputFocus(/* struct gobview *self */);
	LoseInputFocus(/* struct gobview *self */);
	ObservedChanged(/* struct gobview *self, */ struct gob *dobj, long status);
	Print(/* struct gobview *self, */ FILE *file, 
			char *processor, char *finalFormat, boolean topLevel);
	GetApplicationLayer(/* struct gobview *self */) returns struct gobview *;
	

methods:

	/* $$$ methods unique to this view.
		These would be defined if a parent view were to be given 
		greater control over this view. */

classprocedures:

	InitializeClass() returns boolean; 	/* Create default Keymap & Menus */
	InitializeObject(struct gobview *self) returns boolean;
	FinalizeObject(struct gobview *self);

data:
	struct menulist  *Menus;
	struct menulist  *AppMenus;
	struct keystate *Keystate;

	struct graphic  *BlackPattern;	/* these might differ between instances . . . */
	struct graphic  *WhitePattern;	/*		. . . on different displays */

	boolean OnScreen;			/* if not view_Removed */
	boolean embedded;		/* TRUE if no call to GetApplicationLayer */
	boolean HasInputFocus;		/* T if received input focus */
	boolean ignoreUp;			/* T iff have just asked for InputFocus */
	boolean sizeknown;		/* T if haven't called ComputeArea */

	/* $$$ some facts basic to computing desired size
		gob chooses to have a given area if the parent
		constrains height or width */
	long DesiredArea;			/* total space desirable in square pixels */

	/* $$$ save desired size if the computation of it was expensive */
	long DesiredHeight, DesiredWidth;	/* requested dimensions; set by 
						ComputeArea*/
	struct timeval lasttime;
	float xscale,yscale;
};
