/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/labelv.ch,v 2.8 1991/09/12 19:49:54 bobg Exp $ */
/* $ACIS:labelv.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/labelv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidlabelview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/labelv.ch,v 2.8 1991/09/12 19:49:54 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
labelview.H

	Class definitions for label view


	A labelview displays the text stored by a label data object.
	In addition, the labelview can provide image inversion and a hit proc.
	The HitProc is called whenever there is a mouse action on the 
	text within the label image.  The proc is called with these parameters:
		HitProc(labelview, mouseaction, rock)
	where the rock is a 32 bit value passed as a parameter to SetHitProc.
*/

#define  labelview_PROGRAMMERVERSION    1

class labelview[labelv] : view {
overrides:

	FullUpdate(/* struct labelview *self, */ enum view_UpdateType type, 
			long left, long top, long width, long height );
	Update(/* struct labelview *self, */);
	Hit(/* struct labelview *self, */ enum view_MouseAction action, 
			long x, long y, long n)	returns struct view *;
	DesiredSize(/* struct labelview *self, */ long width, long height, 
			enum view_DSpass pass, long *desiredWidth, long *desiredHeight ) 
			returns enum view_DSattributes;
	ObservedChanged(/* struct labelview *self, */ struct label *dobj, long status );
	Print(/* struct labelview *self, */ FILE *file, 
			char *processor, char *finalFormat, boolean topLevel );
	GetApplicationLayer(/* struct labelview *self */) returns struct labelview *;
	
methods:

	SetHitProc(/* struct labelview *self, */ void (*proc)(), char *rock);
	GetHitRock(/* struct labelview *self */) returns char *;
	SetInversion(/* struct labelview *self, */ boolean invert);
	GetInversion(/* struct labelview *self */) returns boolean;

macromethods:

	/* the following is a macromethod because the class processor cannot handle 
			"  returns void(*)()  "		XXX
	*/
	GetHitProc() (self->hitproc)

classprocedures:

	InitializeObject(/* struct classhdr *ClassID;*/ struct labelview *self) returns boolean;
	FinalizeObject(/* struct classhdr *ClassID;*/ struct labelview *self);

data:

	boolean GaveSize;			/* set after successful DesiredSize */
	
	struct graphic  *BlackPattern;	/* these might differ between instances . . . */
	struct graphic  *WhitePattern;	/*		. . . on different displays */

	boolean embedded;		/* TRUE if no call to GetApplicationLayer */
	boolean OnScreen;			/* if not view_Removed */

	void (*hitproc)();			/* called for a mouse hit */
	char *hitrock;				/* passed to the hit proc */
	boolean inverted;			/* TRUE if image is supposed to be inverted */
	long minwidth, minheight;		/* minimum width and height of the text */
};
