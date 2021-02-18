/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/util/RCS/vutils.c,v 1.3 1991/08/30 23:28:38 gk5g Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/util/RCS/vutils.c,v $ */
/* $Author: gk5g $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/util/RCS/vutils.c,v 1.3 1991/08/30 23:28:38 gk5g Exp $";
#endif /* lint */

/*************************************************
 * View utility package
 * Contains stuff useful to all views.
 *************************************************/

/*************************************************
 * Copyright (C) 1990 by the Massachusetts Institute of Technology
 * Permission to use, copy, modify, distribute, and sell this
 * software and its documentation for any purpose is hereby
 * granted without fee, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation,
 * and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission.  M.I.T. makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 *************************************************/


#include <andrewos.h>

#include <ctype.h>

#include <class.h>

#include <bind.ih>
#include <view.ih>
#include <im.ih>
#include <message.ih>
#include <environ.ih>
#include <envrment.ih>

#include <vutils.eh>

#define DIALOG 100
#define MESSAGE 0

/* Routines appear in this file in "bottom-up" order. */
/* This is so I don't have to deal with declaring forward */
/* references. */

/* Added friendly read-only behavior from txtvcmds.c */

#define H_D_STRING "Help Could not start. It was "

static void helpDeath (pid, self, status)
int pid;
struct view *self;
union wait *status;
{
    char buf[128],*em,*statustostr();

    strcpy(buf, H_D_STRING);
    em=statustostr(status, buf+strlen(H_D_STRING), 128 - strlen(H_D_STRING));
    if (em == NULL) {
	/* successful completion */
	message_DisplayString(self, MESSAGE, "If you don't see a help window now, something is wrong.");
    } else {
	message_DisplayString(self, MESSAGE, buf);
    }
}

static void forkhelpproc (self, key)
struct view *self;
long key;
{
    char *helpname = environ_AndrewDir("/bin/help");
    int pid, fd;

    switch (pid = osi_vfork()) {
	case 0:
	    for (fd = getdtablesize(); fd > 2; --fd) close(fd);
	    execl(helpname, helpname, im_GetProgramName(), 0);
	    printf ("Exec of %s failed.\n", helpname);
	    fflush (stdout);
	    _exit(-1);
	case -1:
	    message_DisplayString(self, DIALOG, "Could not start the help!");
	    break;
	default:
	    message_DisplayString(self, MESSAGE, "A Help window should appear shortly.");
	    im_AddZombieHandler(pid, helpDeath, (long)self);
	    break;
    }
    return;
}

boolean vutils__InitializeClass(classID)
struct classheader *classID;
{
    static struct bind_Description compat_fns[] = {
	{"vutils-fork-help", NULL, 0, NULL, 0, 0, forkhelpproc, "Call Andrew help in a new process.", "vutils"},

        {NULL},
    };
    struct classinfo *viewClassinfo;

    viewClassinfo = class_Load("view");
    if (viewClassinfo != NULL) {
        bind_BindList(compat_fns, NULL, NULL, viewClassinfo);
        return TRUE;
    }
    else
        return FALSE;
}
