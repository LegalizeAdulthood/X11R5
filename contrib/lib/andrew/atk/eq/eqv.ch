/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/eq/RCS/eqv.ch,v 2.10 1991/09/12 19:31:43 bobg Exp $ */
/* $ACIS:eqv.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/eq/RCS/eqv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsideqview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/eq/RCS/eqv.ch,v 2.10 1991/09/12 19:31:43 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 * eqv.H
 * class header file for eqv.
 */


class eqview[eqv] : view {
overrides:
    DesiredSize(long width, long height, enum view_DSpass pass, long *widthp, long *heightp) returns enum view_DSattributes;
    Update();
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
    ReceiveInputFocus();
    LoseInputFocus();
    Hit (enum view_MouseAction action, long x, long y, long clicks) returns struct eqview *;
    SetDataObject(struct dataobject *dataObject);
    Print(FILE *f, char *process, char *final, int toplevel);

methods:
    Draw(struct eq *eqptr, struct formula *f, long x, long y) returns struct formula *;
    Find(struct eq *eqptr, long mx, long my, long restrict) returns long;
    CalculateCaret();
    DrawCaret();
    SetDotPosition(long newp);
    SetDotLength(long newl);
    GetDotPosition() returns long;
    GetDotLength() returns long;
    Changed(enum changed changed);

classprocedures:
    InitializeObject(struct eq *self) returns boolean;
    FinalizeObject(struct eqview *self);
    InitializeClass()returns boolean;

data:
/*
 * The equation pointer in the data object is not view-independent,
 * so the eqview cheats and simply contains a pointer to an equation.
 * Since the equation has markers in it, it doesn't qualify as a bona
 * fide view-independent data object.
 */
    long off_x, off_y;		/* offset from 0,0 to origin of equation */
    enum changed {
	EQVIEW_nothing,		/* nada */
	EQVIEW_caret,		/* just the caret */
	EQVIEW_eq,		/* the equation, maybe including caret */
	EQVIEW_everything	/* redraw the whole inset */
    } changed;			/* what changed for Update */
    long			/* caret or selection as follows: */
	caret_x,		/* left if selection, x position if caret */
	caret_y,		/* top if selection, y position if caret */
	selection_width,	/* width if selection, 0 if caret */
	selection_height;	/* height if selection, 0 if caret */
    boolean hasinputfocus;	/* doc'n says this is in struct view! */
    boolean embedded;	/* true if GetApplicationLayer is not called */
    struct keystate *keystate;	/* for keymap routines */
    struct menulist *normalMenus;  /* for menu routines */
    struct menulist *cutMenus;  /* for menu routines */
    struct mark	*dot;
    char *filename;		/* name of file read in */
};

#define Eq(self) ((struct eq *) ((self)->header.view.dataobject))
