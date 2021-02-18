/*
 * sunCG8C.c --
 *	Functions to support the sun CG8 board as a memory frame buffer.
 */

/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the names of Sun or MIT not be used in
advertising or publicity pertaining to distribution  of  the
software  without specific prior written permission. Sun and
M.I.T. make no representations about the suitability of this
software for any purpose. It is provided "as is" without any
express or implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/

#ifndef	lint
static char sccsid[] = "@(#)sunCG8C.c	1.4 6/1/87 Copyright 1987 Sun Micro";
#endif

#include    "sun.h"

#include    <sys/mman.h>
#include    <pixrect/pixrect_hs.h>
#include    "colormap.h"
#include    "colormapst.h"
#include    "resource.h"
#include    <struct.h>

/*-
 * The cg8 frame buffer is divided into several pieces.
 *	1) an array of 24-bit pixels
 *	2) a one-bit deep overlay plane
 *	3) an enable plane
 *	4) a colormap and status register
 *
 * XXX - put the cursor in the overlay plane
 */
#define	CG8_HEIGHT	900
#define	CG8_WIDTH	1152

/*
 * These pointers must be used instead of explicit arrays, because the
 * rasterops may put the frame buffer somewhere new.
 */
typedef struct cg8c {
	u_char *overlay;	/* bit-per-pixel memory */
	u_char *enable;		/* enable plane */
	u_long *cg8;		/* word-per-pixel memory */
} CG8CRec, *CG8CPtr;

/*
 * These constants are defined in the cgeight man page
 */
#define	CG8C_MONOLEN	    (128*1024)
#define	CG8C_ENABLELEN	    CG8C_MONOLEN
#define CG8C_24BITLEN	    (((4*CG8_HEIGHT*CG8_WIDTH + 4095)/4096)*4096)

#define	CG8C_MONOOFF	    0x0
#define	CG8C_ENABLEOFF	    CG8C_MONOLEN
#define CG8C_24BITOFF	    (CG8C_MONOLEN + CG8C_ENABLELEN)

static CG8CPtr CG8Cfb;
static CG8CPtr CG8CfbBackup;	     /* a virgin copy of the address
                                      * for the GiveUp routine.
				      */
#define True  1
#define False 0

static int  sunCG8CScreenIndex;

static u_char overlayBack[128*1024];			/* backup for overlay */

extern Pixrect *sun_pr_screen;
 
/*-
 *-----------------------------------------------------------------------
 * sunCG8CInit --
 *	Attempt to find and initialize a cg8 framebuffer used as mono
 *
 * Results:
 *	TRUE if everything went ok. FALSE if not.
 *
 * Side Effects:
 *	Most of the elements of the ScreenRec are filled in. Memory is
 *	allocated for the frame buffer and the buffer is mapped. The
 *	video is enabled for the frame buffer...
 *
 *-----------------------------------------------------------------------
 */
/*ARGSUSED*/
static Bool
sunCG8CInit (index, pScreen, argc, argv)
    int	    	  index;    	/* The index of pScreen in the ScreenInfo */
    ScreenPtr	  pScreen;  	/* The Screen to initialize */
    int	    	  argc;	    	/* The number of the Server's arguments. */
    char    	  **argv;   	/* The arguments themselves. Don't change! */
{
    if (!cfbScreenInit (pScreen, (pointer)CG8Cfb->cg8,
			sunFbs[index].info.fb_width,
			sunFbs[index].info.fb_height,
			monitorResolution, monitorResolution,
			sunFbs[index].info.fb_width))
    	return (FALSE);
    
    if (!sunScreenAllocate (pScreen))
	return FALSE;

    sunCGScreenInit (pScreen);

    if (!sunScreenInit (pScreen))
	return FALSE;

    sunSaveScreen( pScreen, SCREEN_SAVER_OFF );

    return cfbCreateDefColormap(pScreen);
}

/*-
 *--------------------------------------------------------------
 * sunCG8CSwitch --
 *      Enable or disable color plane 
 *
 * Results:
 *      Color plane enabled for select =0, disabled otherwise.
 *
 *--------------------------------------------------------------
 */
static void
sunCG8CSwitch (pScreen, select)
    ScreenPtr  pScreen;
    u_char     select;
{
    int index, mindex;
    register int  *i, *j, *k, *end;

    index = pScreen->myNum;
    mindex = index?0:1;
    CG8Cfb = (CG8CPtr) sunFbs[index].fb;

    i = (int *) CG8Cfb->enable;
    j = (int *) CG8Cfb->overlay;
    k = (int *) overlayBack;
    end = i + (128 / sizeof (int)) * 1024;
    if (!select)
      while (i < end) {
	*i++ = 0;
	*k++ = *j;
	*j++ = 0;
      }
    else
      while (i < end) {
        *i++ = ~0;
        *j++ = *k++;
      }
}

#ifndef FBTYPE_ROPS_TCP
#define FBTYPE_ROPS_TCP -1
#endif

caddr_t
sunMapFb(fd, length, fbName)
    int fd;
    size_t length;
    char *fbName;
{
    caddr_t address, mappedAddress;

#ifdef _MAP_NEW
    address = (caddr_t) 0;
#else
    address = valloc(length);
    if (address == (caddr_t)NULL) {
	ErrorF("Could not allocate room for %s.\n", fbName);
	return ((caddr_t)NULL);
    }
#endif

    mappedAddress = mmap(address,
			 length,
			 PROT_READ | PROT_WRITE,
			 MAP_SHARED, fd, 0);
    if ((int)mappedAddress == -1) {
	Error(fbName);
	(void) close(fd);
	return ((caddr_t)NULL);
    }

    return (mappedAddress);
}

/*-
 *-----------------------------------------------------------------------
 * sunCG8CProbe --
 *	Attempt to find and initialize a cg8 framebuffer used as mono
 *
 * Results:
 *	TRUE if everything went ok. FALSE if not.
 *
 * Side Effects:
 *	Memory is allocated for the frame buffer and the buffer is mapped.
 *
 *-----------------------------------------------------------------------
 */
/*ARGSUSED*/
Bool
sunCG8CProbe (pScreenInfo, index, fbNum, argc, argv)
    ScreenInfo	  *pScreenInfo;	/* The screenInfo struct */
    int	    	  index;    	/* The index of pScreen in the ScreenInfo */
    int	    	  fbNum;    	/* Index into the sunFbData array */
    int	    	  argc;	    	/* The number of the Server's arguments. */
    char    	  **argv;   	/* The arguments themselves. Don't change! */
{
    int         fd;
    struct fbtype fbType;
    size_t      length;
    caddr_t     base;

    if ((fd = sunOpenFrameBuffer(FBTYPE_MEMCOLOR, &fbType, index, fbNum,
				 argc, argv)) < 0)
	return FALSE;

    if ((CG8Cfb = (CG8CPtr) xalloc(sizeof(CG8CRec))) == NULL) {
	close(fd);
	return FALSE;
    }

#ifdef RASTEROPS
    if (! roProbe (pScreenInfo, index, fbNum, fd, &fbType,
		   &CG8Cfb->overlay, &CG8Cfb->enable, &CG8Cfb->cg8))
	return FALSE;
#else
    length = CG8C_MONOLEN + CG8C_ENABLELEN + CG8C_24BITLEN;

    if ((base = sunMapFb(fd, length, "cg8c")) == (caddr_t)NULL)
	return FALSE;
    CG8Cfb->overlay = (u_char *)(base + CG8C_MONOOFF);
    CG8Cfb->enable = (u_char *)(base + CG8C_ENABLEOFF);
    CG8Cfb->cg8 = (u_long *)(base + CG8C_24BITOFF);
#endif

    
    sunFbs[index].fd = fd;
    sunFbs[index].info = fbType;
    sunFbs[index].fb = (pointer) CG8Cfb;
    sunFbs[index].EnterLeave = sunCG8CSwitch;
    sunSupportsDepth24 = TRUE;
    CG8CfbBackup = CG8Cfb;   /* need to save a virgin copy of the address
			      * for the GiveUp routine.
			      */

    for (argc--, argv++; argc; argc--,argv++)
	if (strcmp(*argv, "-multimonitor") == 0) {
	    sunFbs[index].EnterLeave = NULL;
	    break;
	}

    return TRUE;
}

Bool
sunCG9CProbe (pScreenInfo, index, fbNum, argc, argv)
    ScreenInfo	  *pScreenInfo;	/* The screenInfo struct */
    int	    	  index;    	/* The index of pScreen in the ScreenInfo */
    int	    	  fbNum;    	/* Index into the sunFbData array */
    int	    	  argc;	    	/* The number of the Server's arguments. */
    char    	  **argv;   	/* The arguments themselves. Don't change! */
{
    int         fd;
    struct fbtype fbType;
    size_t length;
    caddr_t base;

    if ((fd = sunOpenFrameBuffer(FBTYPE_SUNROP_COLOR, &fbType, index, fbNum,
				 argc, argv)) < 0)
	return FALSE;

    if ((CG8Cfb = (CG8CPtr) xalloc(sizeof(CG8CRec))) == NULL) {
	close(fd);
	return FALSE;
    }

    length = CG8C_MONOLEN + CG8C_ENABLELEN + CG8C_24BITLEN;
    if ((base = sunMapFb(fd, length, "cg8c")) == (caddr_t)NULL)
	return FALSE;
    CG8Cfb->overlay = (u_char *)(base + CG8C_MONOOFF);
    CG8Cfb->enable = (u_char *)(base + CG8C_ENABLEOFF);
    CG8Cfb->cg8 = (u_long *)(base + CG8C_24BITOFF);

    sunFbs[index].fd = fd;
    sunFbs[index].info = fbType;
    sunFbs[index].fb = (pointer) &CG8Cfb;
    sunFbs[index].EnterLeave = sunCG8CSwitch;
    sunSupportsDepth24 = TRUE;
    return TRUE;
}

Bool
sunCG8CCreate(pScreenInfo, argc, argv)
    ScreenInfo	  *pScreenInfo;
    int	    	  argc;
    char    	  **argv;
{
    int i;

    i = AddScreen(sunCG8CInit, argc, argv);
    if (i >= 0)
    {
	/* Now set the enable plane for screen 0 */
	sunCG8CSwitch(pScreenInfo->screens[i], i != 0);
#ifdef RASTEROPS
	if (! roCreate (pScreenInfo, i))
	    return FALSE;
#endif
	return TRUE;
    }
    return FALSE;
}

/*-
 *--------------------------------------------------------------
 * sunCG8CGiveUp --
 *      Clear the screens and return the console screen
 *
 * Results:
 *      mono plane cleared and enabled.
 *
 *--------------------------------------------------------------
 */
void
sunCG8CGiveUp ()
{
    register int  *i, *j, *end;

    i = (int *) CG8CfbBackup->enable;
    j = (int *) CG8CfbBackup->overlay;
    end = i + (128 / sizeof (int)) * 1024;

      while (i < end) {
	*i++ = ~0;
	*j++ = 0;
      }
}
