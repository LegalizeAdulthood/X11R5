/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

/* @(#) slots.h 50.1 90/12/12 Crucible */

#ifndef _OLWMSLOTH
#define _OLWMSLOTH

typedef enum {SOTopToBottom, SOBottomToTop, SOLeftToRight, SORightToLeft} SlotOrder;

typedef struct _iconSlot {
	int ma, mi, maw, miw;
	Bool positioned;
} IconSlot;

extern void SlotInit();	 /* Display *dpy */
	/* initialises the Slots package; should be called only once 
	 * at startup.
	 */

extern struct _iconSlot *SlotAlloc();	/* WinIcon *, Bool, Bool */
	/* given a sized and possibly positioned icon window, allocate 
	 * the appropriate slots for it.  If the window is positioned, 
	 * True should be passed for the second parameter, and the x,y
	 * position will be honoured.  If the window is not positioned, it
	 * will be positioned by this function to the appropriate slots(s).
	 * If the icon is being manually positioned and should be positioned
	 * according to the icon grid, True should be passed for the third
	 * parameter; False should be passed otherwise.
	 */

extern Bool SlotFree();		/* WinIcon * */
 	/* An icon is going away, so its references to slots should also go
	 * away. 
	 */

extern Bool SlotSetLocations();	/* Display *dpy */
	/* sets the order in which slots are allocated for icons which are
	 * not explicitly positioned.  The new order is obtained from the
	 * global resource vector.
	 * For example, the AlongBottom order is expressed as
	 * major BottomToTop, minor LeftToRight.  Any icons which were 
	 * automatically positioned are repositioned to equivalent positions
	 * in the new order.
	 */

#endif /* _OLWMSLOTH */
