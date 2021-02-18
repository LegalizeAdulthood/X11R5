/* dpsexec.c
 *
 * Copyright (C) 1990-1991 by Adobe Systems Incorporated.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notices appear in all copies and that
 * both those copyright notices and this permission notice appear in
 * supporting documentation and that the name of Adobe Systems
 * Incorporated not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.
 * 
 * ADOBE MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THE SOFTWARE FOR
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
 * ADOBE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO EVENT SHALL ADOBE BE LIABLE
 * TO YOU OR ANY OTHER PARTY FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE, STRICT LIABILITY OR ANY OTHER ACTION ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  ADOBE WILL NOT
 * PROVIDE ANY TRAINING OR OTHER SUPPORT FOR THE SOFTWARE.
 * 
 * PostScript, Display PostScript, and Adobe are trademarks of Adobe Systems
 * Incorporated registered in the U.S.A. and other countries.
 *
 * Author: Adobe Systems Incorporated
 */

#include <stdio.h>
#include <signal.h>
#include <X11/X.h>
#include <DPS/XDPS.h>
#include <X11/Xlib.h>
#include <DPS/XDPSlib.h>
#include <DPS/dpsXclient.h>

#define W_HEIGHT	512
#define W_WIDTH		512


static void MyStatusProc (ctxt, code)
DPSContext ctxt;
int code;
{
  
  if (code == PSZOMBIE) {
    /* Zombie event means context died */
    exit(0);
    }
}


main(argc, argv)
int argc;
char **argv;
{
    char *displayname = "";
    Display *dpy;
    int i;
    char buf[1000];
    XEvent ev;
    GC gc;
    long mask;
    int sync = 0;
    int backingStore = 0;
    int exe = 1;
    int c;
    Window win;
    XSetWindowAttributes xswa;
    char *wh;
    DPSContext ctxt;
    XStandardColormap ccube, gramp;
    XWMHints *hints;

    for (i = 1;  i < argc;  i++)
	if (strncmp(argv[i], "-display", strlen(argv[i])) == 0)
	    {
	    i++;
	    displayname = argv[i];
	    }
	else if (strncmp(argv[i], "-sync", strlen(argv[i])) == 0)
	    sync = 1;
	else if (strncmp(argv[i], "-backup", strlen(argv[i])) == 0)
	    backingStore = 1;
	else if (strncmp(argv[i], "-noexec", strlen(argv[i])) == 0)
	    exe = 0;
	else
	    {
	    fprintf(stderr,
"usage: %s [-display displayname][-sync][-backup][-noexec]\n", argv[0]);
	    exit(1);
	    }

    dpy = XOpenDisplay(displayname);
    if (dpy == NULL)
        {
	fprintf(stderr, "%s: Can't open display %s!\n", argv[0], displayname);
	exit(1);
        }
    
    if (sync)
	(void) XSynchronize(dpy, True);

    gc = XCreateGC(dpy, RootWindow (dpy, DefaultScreen (dpy)), 0, NULL);
    XSetForeground(dpy, gc, BlackPixel (dpy, DefaultScreen (dpy)));
    XSetBackground(dpy, gc, WhitePixel (dpy, DefaultScreen (dpy)));

    win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy),
		3, 385, W_WIDTH, W_HEIGHT, 1,
                BlackPixel(dpy, DefaultScreen(dpy)),
		WhitePixel(dpy, DefaultScreen(dpy)));

    hints = XAllocWMHints();
    if (hints != NULL) {
	hints->flags = InputHint;
	hints->input = False;
	XSetWMHints(dpy, win, hints);
	XFree((char *) hints);
    }

    XStoreName(dpy, win, "Display PostScript Executive");
    XSetIconName(dpy, win, "DPS Exec");

    xswa.bit_gravity = SouthWestGravity;
    if (backingStore)
      xswa.backing_store = WhenMapped;
    else
      xswa.backing_store = NotUseful;
    xswa.event_mask = 0x0;
    mask = CWBitGravity | CWBackingStore | CWEventMask;
    XChangeWindowAttributes(dpy, win, mask, &xswa);

    XMapWindow(dpy, win);

    ctxt = XDPSCreateSimpleContext(
      dpy, win, gc, 0, W_HEIGHT, DPSDefaultTextBackstop,
      DPSDefaultErrorProc, NULL);

    if (ctxt == NULL)
        {
	fprintf (stderr, "first: XDPSCreateSimpleContext returns NULL.\n");
	exit (-1);
	}

    DPSSetContext(ctxt);

    /* Allow zombie events to be delivered so application can exit
       if context dies. Detach context so it doesn't wait for a join 
       if it dies */

    XDPSRegisterStatusProc(ctxt, MyStatusProc);
    XDPSSetStatusMask(ctxt, PSZOMBIEMASK, 0, 0);
    DPSPrintf(ctxt, "currentcontext detach ");
    
    if (exe) DPSPrintf(ctxt, "executive");
    DPSPrintf(ctxt, "\n");
    DPSFlushContext(ctxt);
    
    while (1)
        {
	mask = (1<<0) | (1<<dpy->fd);
	DPSFlushContext(ctxt);
	select(dpy->fd+1, &mask, NULL, NULL, NULL);
	
	if (mask & (1<<0))
	    { /* Read from command line, send to context */
	    if (fgets(buf, 1000, stdin) == NULL)
		{
		DPSPrintf(ctxt, "start\n");
		DPSWaitContext(ctxt);
	        break;
		}
	    DPSWriteData(ctxt, buf, strlen(buf));
	    }
	    
        while (XPending(dpy) > 0)
	    { /* No special event handling - just throw them away.
		 Must call XNextEvent to allow DPS status events to
		 be dispatched. */
            XNextEvent(dpy, &ev);
	    }
	}

    DPSDestroySpace(DPSSpaceFromContext(ctxt));
    XFlush(dpy);
}
