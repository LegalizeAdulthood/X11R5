/*
 * Copyright 1991 by David A. Curry
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation.  The
 * author makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied warranty.
 */
#ifndef lint
static char	*RCSid = "$Header: /home/orchestra/davy/progs/xpostit/RCS/util.c,v 1.4 91/09/06 18:29:31 davy Exp $";
#endif

/*
 * util.c - utility routines.
 *
 * David A. Curry
 * Purdue University
 * Engineering Computer Network
 * West Lafayette, IN 47907
 * davy@ecn.purdue.edu
 *
 * $Log:	util.c,v $
 * Revision 1.4  91/09/06  18:29:31  davy
 * Added copyright/permission notice for submission to MIT R5 contrib.
 * 
 * Revision 1.3  91/09/06  17:15:00  davy
 * Changed my address.
 * 
 * Revision 1.2  90/06/14  11:21:14  davy
 * Ported to X11 Release 4.
 * 
 * Revision 1.1  90/06/13  09:48:49  davy
 * Initial revision
 * 
 */
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <sys/param.h>
#include <stdio.h>
#include <pwd.h>

#include "xpostit.h"

/*
 * ByeBye - clean up and exit.
 */
void
ByeBye()
{
	/*
	 * If saving notes is on, save all notes.
	 */
	if (app_res.save_notes)
		SaveAllNotes();

	XtUnmapWidget(toplevel);
	XCloseDisplay(display);
	exit(0);
}

/*
 * SetNoteDir - determine the path to the note directory.
 */
void
SetNoteDir()
{
	char *getenv();
	char *home, *s;
	struct passwd *pwd;
	char fname[MAXPATHLEN];

	/*
	 * If it's an absolute path name,
	 * we're done.
	 */
	if (app_res.note_dir[0] == '/')
		return;

	/*
	 * Find the user's home directory.
	 */
	if ((home = getenv("HOME")) == NULL) {
		if ((pwd = getpwuid(getuid())) == NULL) {
			fprintf(stderr, "xpostit: who are you?\n");
			exit(1);
		}

		home = pwd->pw_dir;
	}

	/*
	 * Save the path.
	 */
	sprintf(fname, "%s/%s", home, app_res.note_dir);

	s = SafeAlloc(strlen(fname) + 1);
	app_res.note_dir = s;
	strcpy(s, fname);
}

/*
 * MakeFname - make a file name from a note index number.
 */
char *
MakeFname(index)
register int index;
{
	char *s;
	char fname[MAXPATHLEN];

	sprintf(fname, "%s/%s%d", app_res.note_dir, PostItNoteFname, index);
	s = SafeAlloc(strlen(fname) + 1);
	strcpy(s, fname);
	return(s);
}

/*
 * SafeAlloc - allocate n bytes of memory, exit if we run out.
 */
char *
SafeAlloc(nbytes)
register int nbytes;
{
	char *malloc();
	register char *s;

	if ((s = malloc(nbytes)) == NULL) {
		fprintf(stderr, "xpostit: out of memory.\n");
		exit(1);
	}

	bzero(s, nbytes);
	return(s);
}
