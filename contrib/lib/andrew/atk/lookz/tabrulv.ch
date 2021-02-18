/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */


#define  tabrulerview_PROGRAMMERVERSION    1

#define tabrulerview_NoValue (-999<<16)

class tabrulerview[tabrulv] : view {
overrides:
    FullUpdate( enum view_UpdateType type, 
		long left, long top, long width, long height );
    Update();
    Hit( enum view_MouseAction action, long x, long y, long n)
      returns struct view *;
    DesiredSize( long width, long height, enum view_DSpass pass, 
		 long *desiredWidth, long *desiredHeight ) 
      returns enum view_DSattributes;

methods:
    SetValues(/* struct tabrulerview *self, */ struct tabs *tabs);
    GetValues(/* struct tabrulerview *self, */ struct tabs **tabs);
macromethods:	
    SetValueChangeProc(proc, rock)   /* store a proc to call when a value changes */   \
      (self->ValueChangeRock = (long)rock, \
	self->ValueChangeProc = (void (*)())proc)
classprocedures:
    InitializeClass(/* struct classhdr *ClassID*/) returns boolean;
    InitializeObject(/* struct classhdr *ClassID;*/ struct tabrulerview *self) returns boolean;
    FinalizeObject(/* struct classhdr *ClassID;*/ struct tabrulerview *self);
data:
    boolean OnScreen;			/* if not view_Removed */
    boolean tabrulerchanged;
    boolean iconschanged;
    boolean textchanged;	/* indicate redraw requirements */

    /* Image patterns might differ between instances  on different displays: */
    struct graphic  *Grey25Pattern, *WhitePattern;

    long leftline, rightline, bottomline, topline, textloc;
    long cleartxt, canceltxt; /* Where to place the text */
    long clearpos, cancelpos; /* Where to place the button */
    long leftzero;
    struct TickTbl *TickTbl;

    void (*ValueChangeProc)();		/* called when a value changes */
    /* ValueChangeProc(tabrulerview, ValueChangeRock, iconcode, newvalue) */
    long ValueChangeRock;			/* passed as second arg to ValueChangeProc */

    enum style_Unit unit;
    struct tabs *tabs;
    short icony;			/* baseline for icons */
    
    boolean Moving;
    long Movex;
    int oldtab;

    int mul; /* Used to scale the ruler to pretend it's real... */
    int div;
};
