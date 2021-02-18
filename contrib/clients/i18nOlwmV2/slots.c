/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

/* @(#) slots.c 50.5 90/12/12 Crucible */

#ifdef OW_I18N
#include "i18n.h"
#endif
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifdef OW_I18N
#include "ollocale.h"
#endif
#include "mem.h"
#include "st.h"
#include "olwm.h"
#include "win.h"
#include "globals.h"
#include "list.h"
#include "slots.h"

/***************************************************************************
* Local data
***************************************************************************/

static SlotOrder SOmajor = SOBottomToTop;
static SlotOrder SOminor = SOLeftToRight;

typedef struct _slotrgn {
	int ma, mi, maw, miw;
	} slotRgn;
typedef struct _slotabs {
	int x, y, w, h;
	} slotAbs;

static int *occupancy = NULL;
static int minoccupancy = 0;
static List *iconList = NULL_LIST;
static int maslots, mislots;
static int slotshoriz, slotsvert;
static int pixhoriz, pixvert;

#define PLUSINFINITY 0xfffffff	/* REMIND fix this for architecture-independence */


/***************************************************************************
* Local functions
***************************************************************************/


#define ROUNDUP(n,d) (((n)+(d)-1)/(d))
/* #define ROUND(n,d) ((0.5+((float)(n)/(float)(d)))*(int)(d)) */
#define ROUND(n,d) (((((n)%(d))>((d)/2))?(((n)/(d))+1):((n)/(d)))*(d))

#define SLOTOFFSET(ma, mi) (((ma))*mislots+(mi))
#define AOCCUPANCY(ma,mi) ((occupancy+SLOTOFFSET((ma),(mi))))
#define OCCUPANCY(ma,mi) (*(AOCCUPANCY((ma),(mi))))

#define ISVERT(so) (((so)==SOBottomToTop)||((so)==SOTopToBottom))
#define ISHORZ(so) (((so)==SOLeftToRight)||((so)==SORightToLeft))
#define ISDECREASING(so) (((so)==SORightToLeft)||((so)==SOBottomToTop))

static char *
sotochar(so)
SlotOrder so;
{
	if (so == SOTopToBottom)
		return ("SOTopToBottom");
	if (so == SOBottomToTop)
		return ("SOBottomToTop");
	if (so == SOLeftToRight)
		return ("SOLeftToRight");
	if (so == SORightToLeft)
		return ("SORightToLeft");
	return("SOIllegal");
}

#ifdef DEBUG
/* dumpocc -- print the occupancy count table to stderr
 */
static void
dumpocc()
{
	int ima, imi;

	fprintf(stderr,"dump of occupancy table:\n");
	for (ima = 0; ima<maslots; ima++)
	    for (imi = 0; imi<mislots; imi++)
		if (OCCUPANCY(ima,imi)!=0)
		    fprintf(stderr,"%x: occ[%d,%d]/[%d] = %d\n",AOCCUPANCY(ima,imi),ima,imi,SLOTOFFSET(ima,imi),OCCUPANCY(ima,imi));
	fprintf(stderr,"end of dump of occupancy table\n");
}
#endif


/* setMajorMinor -- set two output variables from two input variables,
 * based on the major/minor settings.
 */
static void
setMajorMinor(r,c,pma,pmi)
int r, c, *pma, *pmi;
{
	if (ISVERT(SOmajor))
	{
	    *pma = r;
	    *pmi = c;
	}
	else
	{
	    *pma = c;
	    *pmi = r;
	}
}

/* setAbsolute -- set two output variables from two input variables, based
 * on major/minor settings; this is the inverse of setMajorMinor.
 */
static void
setAbsolute(ma,mi,pr,pc)
int ma, mi;
int *pr, *pc;
{
    if (ISVERT(SOmajor))
    {
	*pr = ma;
	*pc = mi;
    }
    else
    {
	*pc = ma;
	*pr = mi;
    }
}

/* incrRegion -- increment the occupancy count of a region
 */
static void
incrRegion(slot,val)
IconSlot *slot;
int val;
{
	int ii,jj;

	for (ii = slot->ma; (ii>=0)&&(ii<maslots)&&(ii<slot->ma+slot->maw); ii++)
	    for (jj = slot->mi; (jj>=0)&&(jj<mislots)&&(jj<slot->mi+slot->miw); jj++)
	    {
		if ((OCCUPANCY(ii,jj) += val) < minoccupancy)
		    minoccupancy = OCCUPANCY(ii,jj);
	    }
}

/* findMinMaxRgn -- find the minimum and maximum occupancies of a region
 */
static void
findMinMaxRgn(ma, mi, nmajor, nminor, pminrgn, pmaxrgn)
int ma, mi, nmajor, nminor, *pminrgn, *pmaxrgn;
{
	int min, max;
	int ii, jj;
	int occ;

	min = PLUSINFINITY;
	max = -1;

	for (ii=ma; ii<ma+nmajor; ii++)
	    for (jj = mi; jj<mi+nminor; jj++)
	    {
		occ = OCCUPANCY(ii,jj);
		if (occ < min)
		    min = occ;
		if (occ > max)
		    max = occ;
	    }
	*pminrgn = min;
	*pmaxrgn = max;
}

static IconSlot *
incrDynamicSlots(nmajor, nminor)
int nmajor, nminor;
{
    	int minoccrgn; /* smallest occupancy found for a region of given size */
    	int minfound, minfoundrgn; 	/* smallest occupancy found for one slot
				         * and a region respectively; used to reset
				         * search values */
    	int ma, mi;	/* indices in major and minor directions */
    	int maxrgn, minrgn;	  /* max and min occupancies in a region */
	IconSlot *slot = MemNew(IconSlot);

	minoccrgn = minoccupancy;
	slot->positioned = False;

	for (;;)	/* will actually only iterate twice */
	{
	    minfound = minfoundrgn = PLUSINFINITY;
	    for (ma = 0; (ma <= maslots-nmajor); ma++)
	    {
	        for (mi = 0; (mi <= mislots-nminor); mi++)
	        {
		    findMinMaxRgn(ma, mi, nmajor, nminor, &minrgn, &maxrgn);
		    if (minrgn < minfound)
			minfound = minrgn;
		    if (maxrgn < minfoundrgn)
			minfoundrgn = maxrgn;
		    if (maxrgn <= minoccrgn)
		    {
			slot->ma = ma;
			slot->mi = mi;
			slot->maw = nmajor;
			slot->miw = nminor;
			incrRegion(slot,1);
			return slot;
		    }
	        }
	    }
	    /* no regions available with occupancy <= known minimum */
	    if (minfound > minoccupancy)	/* increase global slot minimum */
		minoccupancy = minfound;
	    minoccrgn = minfoundrgn;		/* increase local region min */
	}
}

/* setIconPos -- given a slot, position the icon frame window accordingly.
 */
static void 
setIconPos(win, slot)
WinIconFrame *win;
IconSlot *slot;
{
    int x,y;

    if (SOmajor == SOTopToBottom)
    {
	y = ICON_GRID_HEIGHT*slot->ma;
	if (SOminor == SOLeftToRight)
	{
	    x = ICON_GRID_WIDTH*slot->mi;
	}
	else
	{
	    x = pixhoriz - slot->mi*ICON_GRID_WIDTH - win->core.width;
	}
    }
    else if (SOmajor == SOBottomToTop)
    {
	y = pixvert - ICON_GRID_HEIGHT*slot->ma - win->core.height;
	if (SOminor == SOLeftToRight)
	{
	    x = ICON_GRID_WIDTH*slot->mi;
	}
	else
	{
	    x = pixhoriz - slot->mi*ICON_GRID_WIDTH - win->core.width;
	}
    }
    else if (SOmajor == SOLeftToRight)
    {
	x = ICON_GRID_WIDTH*slot->ma;
	if (SOminor == SOTopToBottom)
	{
	    y = ICON_GRID_HEIGHT*slot->mi;
	}
	else
	{
	    y = pixvert - slot->mi*ICON_GRID_HEIGHT - win->core.height;
	}
    }
    else if (SOmajor == SORightToLeft)
    {
	x = pixhoriz - ICON_GRID_WIDTH*slot->ma - win->core.width;
	if (SOminor == SOTopToBottom)
	{
	    y = ICON_GRID_HEIGHT*slot->mi;
	}
	else
	{
	    y = pixvert - slot->mi*ICON_GRID_HEIGHT - win->core.height;
	}
    }

    (WinFunc(win,core.newposfunc))(win,x,y);
}

/* incrPositionedSlots -- an icon has been explicitly positioned; create an
 * iconslot record and increment the occupancy count of the affected slots.
 */
static IconSlot *
incrPositionedSlots(x,y,w,h,snaptogrid)
int x,y,w,h;
Bool snaptogrid;
{
	IconSlot *slot = MemNew(IconSlot);
#define ROUNDIF(v,w) (snaptogrid?ROUND((v),(w)):(v))/(w)
#define ROUNDDIM(p,s,w) (ROUNDUP(((p)%(w))+(s),(w)))

	if (SOmajor == SOTopToBottom)
	{
	    slot->ma = ROUNDIF(y,ICON_GRID_HEIGHT);
	    slot->maw = ROUNDDIM(y,h,ICON_GRID_HEIGHT);
	    if (SOminor == SOLeftToRight)
	    {
		slot->mi = ROUNDIF(x,ICON_GRID_WIDTH);
	        slot->miw = ROUNDDIM(x,w,ICON_GRID_WIDTH);
	    }
	    else
	    {
		slot->mi = ROUNDIF(pixhoriz-(x+w),ICON_GRID_WIDTH);
	        slot->miw = ROUNDDIM(pixhoriz-(x+w),w,ICON_GRID_WIDTH);
	    }
	}
	else if (SOmajor == SOBottomToTop)
	{
	    slot->ma = ROUNDIF(pixvert-(y+h),ICON_GRID_HEIGHT);
	    slot->maw = ROUNDDIM(pixvert-(y+h),h,ICON_GRID_HEIGHT);
	    slot->miw = w;
	    if (SOminor == SOLeftToRight)
	    {
		slot->mi = ROUNDIF(x,ICON_GRID_WIDTH);
	        slot->miw = ROUNDDIM(x,w,ICON_GRID_WIDTH);
	    }
	    else
	    {
		slot->mi = ROUNDIF(pixhoriz-(x+w),ICON_GRID_WIDTH);
	        slot->miw = ROUNDDIM(pixhoriz-(x+w),w,ICON_GRID_WIDTH);
	    }
	}
	else if (SOmajor == SOLeftToRight)
	{
	    slot->ma = ROUNDIF(x,ICON_GRID_WIDTH);
	    slot->maw = ROUNDDIM(x,w,ICON_GRID_WIDTH);
	    if (SOminor == SOTopToBottom)
	    {
		slot->mi = ROUNDIF(y,ICON_GRID_HEIGHT);
		slot->miw = ROUNDDIM(y,h,ICON_GRID_HEIGHT);
	    }
	    else
	    {
		slot->mi = ROUNDIF(pixvert-(y+h),ICON_GRID_HEIGHT);
		slot->miw = ROUNDDIM(pixvert-(y+h),h,ICON_GRID_HEIGHT);
	    }
	}
	else if (SOmajor == SORightToLeft)
	{
	    slot->ma = ROUNDIF(pixhoriz-(x+w),ICON_GRID_WIDTH);
	    slot->maw = ROUNDDIM(pixhoriz-(x+w),w,ICON_GRID_WIDTH);
	    if (SOminor == SOTopToBottom)
	    {
		slot->mi = ROUNDIF(y,ICON_GRID_HEIGHT);
		slot->miw = ROUNDDIM(y,h,ICON_GRID_HEIGHT);
	    }
	    else
	    {
		slot->mi = ROUNDIF(pixvert-(y+h),ICON_GRID_HEIGHT);
		slot->miw = ROUNDDIM(pixvert-(y+h),h,ICON_GRID_HEIGHT);
	    }
	}
	if (slot->ma < 0) slot->ma = 0;
	if (slot->ma >= maslots) slot->ma = maslots-1;
	if (slot->mi < 0) slot->mi = 0;
	if (slot->mi >= mislots) slot->mi = mislots-1;
	slot->positioned = True;
	incrRegion(slot,1);
	return slot;
}


/***************************************************************************
* Global functions
***************************************************************************/

/*
 * SlotInit -- does nothing right now
 */
void 
SlotInit(dpy)
Display *dpy;
{
	if (occupancy != NULL)
	{
#ifdef OW_I18N
		ErrorGeneral(
		       gettext("Tried to multiply initialise Slots module\n"));
#else
		ErrorGeneral("Tried to multiply initialise Slots module\n");
#endif
	}

	iconList = NULL_LIST;
	pixhoriz = DisplayWidth(dpy,DefaultScreen(dpy));
	pixvert = DisplayHeight(dpy,DefaultScreen(dpy));
	slotshoriz = pixhoriz/ICON_GRID_WIDTH;
	slotsvert = pixvert/ICON_GRID_HEIGHT;
	occupancy = MemAlloc(slotshoriz*slotsvert*sizeof(int));
	SlotSetLocations(dpy);
}

/* SlotAlloc - given a sized and possibly positioned icon window, allocate 
 * the appropriate slots for it.  If the window is positioned, 
 * True should be passed for the second parameter, and the x,y
 * position will be honoured.  If the window is not positioned, it
 * will be positioned by this function to the appropriate slots(s).
 * If snaptogrid is true and positioned is true, the given position is
 * modified so that the upper left corner of the icon is at the closest
 * icon grid point.  Modifies the icon structure to assign it the icon
 * slot.
 * Returns an IconSlot pointer if successful; NULL otherwise.
 */
struct _iconSlot * 
SlotAlloc(winicon, positioned, snaptogrid)
WinIconFrame *winicon;
Bool positioned;
Bool snaptogrid;
{
    int nhoriz, nvert;	/* number of slots occupied, horizontally & vertically */
    int nmajor, nminor;	/* number of slots occupied, in major & minor directions */
    IconSlot *slot;
    int x,y;

    iconList = ListCons(winicon, iconList);

    if (positioned)
    {
	slot = incrPositionedSlots(winicon->core.x, winicon->core.y, 
		winicon->core.width, winicon->core.height, snaptogrid);
	winicon->iconslot = slot;
	if (snaptogrid)
	    setIconPos(winicon,slot);
	else
	    (WinFunc(winicon,core.newposfunc))(winicon,winicon->core.x,winicon->core.y);
    }
    else
    {
        nhoriz = ROUNDUP(winicon->core.width, ICON_GRID_WIDTH);
        nvert = ROUNDUP(winicon->core.height, ICON_GRID_HEIGHT);
	setMajorMinor(nvert, nhoriz, &nmajor, &nminor);
	slot = incrDynamicSlots(nmajor, nminor);
	winicon->iconslot = slot;
	setIconPos(winicon,slot);
    }

    return slot;
}

/* SlotFree -- An icon is going away, so its references to slots should also go
 * away.  Returns True iff the free was successful.
 * Note that if this code is changed, the similar code in SlotSetLocations
 * may need to be changed also.
 */
Bool 
SlotFree(winicon)
WinIconFrame *winicon;
{
	List **l;
	WinIconFrame *win;

	if (winicon->iconslot == NULL)
	{
		return;
	}

	incrRegion(winicon->iconslot,-1);
	MemFree(winicon->iconslot);
	winicon->iconslot = NULL;

	l = &iconList;
	for (l = &iconList ; (*l) != NULL ; l = &((*l)->next))
	{
	    if ((WinIconFrame *)((*l)->value) == winicon)
	    {
		ListDestroyCell(l);
		break;
	    }
	}
}

/* sets the order in which slots are allocated for icons which are
 * not explicitly positioned.  The order is gotten from the global
 * resource vector.
 * For example, the AlongBottom order is expressed as
 * major BottomToTop, minor LeftToRight.  The major and minor orders
 * cannot be both vertical or horizontal.  Any icons which were 
 * automatically positioned are repositioned to equivalent positions
 * in the new order.
 */
Bool 
SlotSetLocations(dpy)
Display *dpy;
{
	List *lauto, *lpos;
	List **l;
	WinIconFrame *win;
	int ima, imi;

	/* set up the new order of things */
        switch (GRV.IconPlacement)
        {
        case AlongTop:
		SOmajor = SOTopToBottom;
		SOminor = SOLeftToRight;
                break;

        case AlongTopRL:
		SOmajor = SOTopToBottom;
		SOminor = SORightToLeft;
                break;

        case AlongBottom:
		SOmajor = SOBottomToTop;
		SOminor = SOLeftToRight;
                break;

        case AlongBottomRL:
		SOmajor = SOBottomToTop;
		SOminor = SORightToLeft;
                break;

        case AlongLeft:
		SOmajor = SOLeftToRight;
		SOminor = SOTopToBottom;
                break;

        case AlongLeftBT:
		SOmajor = SOLeftToRight;
		SOminor = SOBottomToTop;
                break;

        case AlongRight:
		SOmajor = SORightToLeft;
		SOminor = SOTopToBottom;
                break;

        case AlongRightBT:
		SOmajor = SORightToLeft;
		SOminor = SOBottomToTop;
                break;
        }

	setMajorMinor(slotsvert,slotshoriz,&maslots,&mislots);

	if (iconList == NULL_LIST)
	{
		for (ima = 0; ima<maslots; ima++)
		    for (imi = 0; imi<mislots; imi++)
			OCCUPANCY(ima,imi)=0;
		return;
	}

	/* partition existing icons */
	lauto = NULL_LIST;
	lpos = NULL_LIST;
	l = &iconList;
	for (win = ListEnum(l); win != NULL; win = ListEnum(l))
	{
	    if (!win->iconslot->positioned)
	    {
		lauto = ListCons(win,lauto);
	    }
	    else
	    {
		lpos = ListCons(win,lpos);
	    }
	    /* this is the basics of a SlotFree */
	    MemFree(win->iconslot);
	    win->iconslot = NULL;
	}
	ListDestroy(iconList);
	iconList = NULL_LIST;

	for (ima = 0; ima<maslots; ima++)
	    for (imi = 0; imi<mislots; imi++)
		OCCUPANCY(ima,imi)=0;

	/* place positioned icons */
	l = &lpos;
	for (win = ListEnum(l); win != NULL; win = ListEnum(l))
	{
	    SlotAlloc(win,True,False);
	    (WinFunc(win,core.setconfigfunc))(win->core.client->dpy,win);
	}
	ListDestroy(lpos);

	/* place auto-positioned icons */
	l = &lauto;
	for (win = ListEnum(l); win != NULL; win = ListEnum(l))
	{
	    SlotAlloc(win,False,False);
	    (WinFunc(win,core.setconfigfunc))(win->core.client->dpy,win);
	}
	ListDestroy(lauto);
}
