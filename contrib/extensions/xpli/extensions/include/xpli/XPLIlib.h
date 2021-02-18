/*
 * Copyright IBM Corporation 1991
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
 
/********************************************************************

EXTENSION XLIB STUB INCLUDE FILE FOR PAPER-LIKE INTERFACE (PLI)




********************************************************************/



static char *XPLIlib_h ="%Z% %M% %H% %T% Original Version %Z%";

#include "PLIproto.h"             /* Protocol definitions */

typedef struct {
	int type;
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window window;
        BYTE detail;
           /* 00X - motion report (no stroke) */
           /* 01X - start of stroke report */
           /* 02X - in-stroke motion report */
           /* 03X - end of stroke report */
	   /* 04X - stroke deleted by server */
           /* 05X - 07X unused and reserved codes */
	   /* 08X - unused and reserved flag */
	   /* 3-X - entire incomplete stroke in window */
	   /* 7-X - entire stroke in window */
           /* 8-X - entire stroke outside of window */
           /* 9-X - some in, some out, start out, end out */
           /* B-X - some in, some out, start in,  end out */
           /* D-X - some in, some out, start out, end in  */
           /* F-X - some in, some out, start in,  end in  */
           /* all other codes in upper nibble are invalid */
        Time time B32;
        CARD16 strokeID B16;
        CARD16 numXYs B16;   /* michael sacks - 03/09/89 */
        CARD16 startX B16, startY B16; /* xys are pixels, window relative */
        CARD16 endX B16, endY B16;
        CARD16 minX B16, minY B16;
        CARD16 maxX B16, maxY B16;
} XStrokeEvent;	


typedef struct
  {
  CARD32 eventCode;
  CARD32 eventMask;
  CARD32 digtype;  /* digitizer type */
  CARD16 scalefac; /* coordinate scale factor being used */
  CARD32 sampsec;  /* samples per second */
  CARD16 resolX, resolY;      /* scaled coords per cm */
  CARD32 resolTimeSec;        /* accuracy of stroke timestamp */
  CARD32 resolTimeNsec;       
  } PLIExtData;



/* Functional prototypes for all routines */
extern BOOL       PLICopyStrokeToClient();
extern BOOL       PLIDeleteStrokes();
extern BOOL       PLIEnqueueStrokes();
extern BOOL       PLIMoveStrokeToClient();
extern PLIExtData *PLIRegisterExtension();
extern BOOL       PLIRejectStroke();
extern BOOL       PLIAlign();






