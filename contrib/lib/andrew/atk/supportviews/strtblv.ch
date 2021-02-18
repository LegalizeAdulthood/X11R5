/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/strtblv.ch,v 2.7 1991/09/12 19:50:24 bobg Exp $ */
/* $ACIS:strtblv.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/strtblv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidstrtblview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/supportviews/RCS/strtblv.ch,v 2.7 1991/09/12 19:50:24 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
strtblview.H

	Class definitions for stringtbl view

	Displays the contents of a string table.
	When there is a hit on an item, the client can have a procedure called.
	The client calls strtblview_SetItemHitProc(table, proc, rock).
		When there is a hit on the table, a call to the proc is made with
		the following parameters:
			hitproc(stringtbl, rock, accnum)
		where rock is the value passed to SetItemHitProc and
		accnum is the accession number returned by stringtbl_AddString

		The visual organization of a stringtbl is based on the maximum
		number of elements that table has ever had, rather than its current
		contents.  To reset this number, use strtblview_Clear instead 
		of stringtbl_Clear;  the two _Clear routines are otherwise identical.

*/

#define  strtblview_PROGRAMMERVERSION    1

class strtblview[strtblv] : view {
overrides:

	FullUpdate( enum view_UpdateType type, 
		long left, long top, long width, long height );
	Update();
	Hit( enum view_MouseAction action, long x, long y, long n)
			returns struct view *;
	DesiredSize( long width, long height, enum view_DSpass pass, 
				long *desiredWidth, long *desiredHeight ) 
			returns enum view_DSattributes;
	ObservedChanged( struct stringtbl *dobj, long status );
	Print( FILE *file, char *processor, char *finalFormat, boolean topLevel );

methods:

	Clear();	/* clears the string table and also resets 
		the maximum number of elements seen in the view */

macromethods:	

	SetItemHitProc(proc, rock) (self->ItemHitRock = (long)rock, \
						self->ItemHitProc = (void (*)())proc)
		/* store a proc to call for a hit */
	GetItemHitProc()	((self->ItemHitProc))		/* untested */
	GetItemHitRock()	((self->ItemHitRock))		/* untested */

classprocedures:

	InitializeObject(/* struct classhdr *ClassID;*/ struct strtblview *self) returns boolean;
	FinalizeObject(/* struct classhdr *ClassID;*/ struct strtblview *self);

	/* the following are options for SetItemHitProc.  
		The rock passed to the stv parameter is ignored */
	ZeroOrOne(/* struct classhdr *ClassID;*/ 
			struct stringtbl *st, struct strtblview *stv, short item);
	ZeroOrMany(/* struct classhdr *ClassID;*/ 
			struct stringtbl *st, struct strtblview *stv, short item);
	OneOnly(/* struct classhdr *ClassID;*/ 
			struct stringtbl *st, struct strtblview *stv, short item);

data:

	unsigned long BlackOnes;		/* which items have black boxes on display */
	void (*ItemHitProc)();		/* called when mouse hits an item */
		/* ItemHitProc(stringtbl, ItemHitRock, accnum of hit item) */
	long ItemHitRock;			/* passed as second arg to ItemHitProc */
	boolean OnScreen;			/* if not view_Removed */
	boolean GaveSize;		/* as DesiredSize succeeded */
	boolean tablechanged;		/* set by ObservedChanged */
	boolean sizeknown;			/* set after scanning strings to get maxwidth */
	short rows, cols;			/* layout of the screen area */
	short itemheight, maxwidth;	/* max dimensions of an item */
	short maxused;				/* max number of items ever found */
	struct graphic  *BlackPattern;	/* these might differ between instances . . . */
	struct graphic  *WhitePattern;	/*		. . . on different displays */

};
