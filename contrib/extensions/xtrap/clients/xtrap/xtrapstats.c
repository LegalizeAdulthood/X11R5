/*****************************************************************************
Copyright 1987, 1988, 1989, 1990, 1991 by Digital Equipment Corp., Maynard, MA

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

*****************************************************************************/
/*
 *
 *  CONTRIBUTORS:
 *
 *      Dick Annicchiarico
 *      Robert Chesler
 *      Dan Coutu
 *      Gene Durso
 *      Marc Evans
 *      Alan Jamison
 *      Mark Henry
 *      Ken Miller
 *
 */
#include "stdio.h"
#include "Xlib.h"
#include "xtraplib.h"
#include "xtraplibp.h"
#ifdef sun
#include "ctype.h"
#endif

#ifndef lint
static char SCCSID[] = "@(#)XTrapStats.c	1.6 - 90/09/18  ";
#endif

#ifdef FUNCTION_PROTOS
main(int argc, char *argv[])
#else
main(argc,argv)
    int argc;
    char *argv[];
#endif
{
    XETrapGetAvailRep     ret_avail;
    XETrapGetStatsRep     ret_stats;
    Widget  appW;
    XETC    *tc;
    Display *dpy;
    Bool done;
    char    buffer[10];
    ReqFlags   requests;
    EventFlags events;
    int i;

    /* Connect to Server */
    appW = XtInitialize(NULL,"XTrap",NULL,0L,&argc,argv);
    dpy = XtDisplay(appW);
#ifdef DEBUG
    XSynchronize(dpy, True);
#endif
    printf("Display:  %s \n", DisplayString(dpy));
    if ((tc = XECreateTC(dpy,0L, NULL)) == False)
    {
        fprintf(stderr,"%s: could not initialize extension\n",argv[0]);
        exit(1L);
    }

    (void)XEGetAvailableRequest(tc,&ret_avail);
    if (BitIsFalse(ret_avail.valid, XETrapStatistics))
    {
        printf("\nStatistics not available from '%s'.\n",
            DisplayString(dpy));
        exit(1L);
    }
    XETrapSetStatistics(tc, True);
    for (i=0; i<256L; i++)
    {
        BitTrue(requests, i);
    }
    XETrapSetRequests(tc, True, requests);
    for (i=KeyPress; i<=MotionNotify; i++)
    {
        BitTrue(events, i);
    }
    XETrapSetEvents(tc, True, events);
    done = False;
    while(done == False)
    {
        fprintf(stderr,"Stats Command (Zero, Quit, [Show])? ");
        fgets(buffer, sizeof(buffer), stdin);
        switch(toupper(buffer[0]))
        {
            case '\n':  /* Default command */
            case 'S':   /* Request fresh counters & display */
                (void)XEGetStatisticsRequest(tc,&ret_stats);
                (void)XEPrintStatistics(stdout,&ret_stats);
                break;
            case 'Z':  /* Zero out counters */
                XETrapSetStatistics(tc, False);
                break;
            case 'Q':
                done = True;
                break;
            default:
                printf("Invalid command, reenter!\n");
                break;
        }
    }
    (void)XEFreeTC(tc);
    (void)XCloseDisplay(dpy);
    exit(0L);
}
