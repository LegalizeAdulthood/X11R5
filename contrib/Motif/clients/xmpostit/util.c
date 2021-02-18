#ifndef lint
static char	*RCSid = "$Header: /usr/src/local/bin/X11/xmpostit/RCS/util.c,v 1.4 91/09/27 11:29:54 dm Exp $";
#endif

/*
 * util.c - utility routines.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94086
 * davy@riacs.edu
 *
 *
 * Copyright 1989, 1990 Genentech, Incorporated
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Genentech not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Genentech makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * GENENTECH DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL GENENTECH BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 * Author:  David Mischel - Genentech, Inc.
 *

 * $Log:	util.c,v $
 * Revision 1.4  91/09/27  11:29:54  dm
 * add chk_dirname to confirm existence and writability of notedir, or create it.
 * 
 * Revision 1.3  91/09/26  14:22:15  dm
 * add genentech copyright headers
 * 
 * Revision 1.2  89/12/01  16:05:14  dm
 * beta version with stash, scroll bars, etc.
 * 
 * Revision 1.1  89/11/29  12:52:47  dm
 * Initial revision
 * 
 */
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <sys/param.h>
#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "xmpostit.h"

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
	if (app_res.note_dir[0] == '/') {
		chk_dirname(app_res.note_dir);
		return;
	}

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
	chk_dirname(fname);

	s = SafeAlloc(strlen(fname) + 1);
	app_res.note_dir = s;
	strcpy(s, fname);
}

/*
 * chk_dirname - confirm a dir exists or can be created, and is writable.
 * else warn the user.
 */
chk_dirname(dir)
	char *dir;
{
	struct stat sbuf;

	/* check that it exists and is a directory */
	if (stat(dir, &sbuf) == 0) {
		if ((sbuf.st_mode&S_IFDIR) == 0) {
			fprintf(stderr, "xmpostit: %s is not a directory; cannot save notes\n", dir);
			return;
		}
	}
	/* else try to create it */
	else if (mkdir(dir, 0777) == -1) {
		fprintf(stderr, "xmpostit: cannot create ");
		perror(dir);
		return;
	}

	/* make sure we can write it */
	if (access(dir,X_OK) || access(dir,R_OK) || access(dir,W_OK)) {
		fprintf(stderr, "xmpostit: access problem in ");
		perror(dir);
	}
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
