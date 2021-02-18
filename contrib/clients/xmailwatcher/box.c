/*

Copyright 1991 by the University of Edinburgh, Department of Computer Science

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of the University of Edinburgh not be used
in advertising or publicity pertaining to distribution of the software
without specific, written prior permission.  The University of Edinburgh
makes no representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.

*/

/*
	Author:	George Ross
		Department of Computer Science
		University of Edinburgh
		gdmr@dcs.ed.ac.uk
*/

/* Parse a mailbox.  Fill the supplied buffer with From/Subject lines */

#include "box.h"

#include <stdio.h>
#include <X11/Xos.h>

static char *mungeSender(from, flags)
char *from;
int flags;
{	char *a, *b;
	int n;

	if (flags & BOX_FULLNAMES) return from;
	if (index(from, '"')) return from;

	/* Prefer (..) */
	if (a = index(from, '(')) a++;
	else a = from;
	if (b = index(a, ')')) *b = '\0';
	/* Dump <..> */
	if (b = index(a, '<')) *b = '\0';
	/* Strip trailing blanks */
	n = strlen(a);
	if (n == 0) return from;  /* Nothing left */
	b = a + n;
	for (;;) {
		b--;
		if (isgraph(*b)) break;
		if (a == b) return from;  /* Nothing left */
		*b = '\0';
	}
	return a;
}

static void handlePrevious(buffer, remaining, flags, from, stat, subj, tally)
char *buffer;
int *remaining;
int flags;
char *from;
char*stat;
char *subj;
int *tally;
{	char line[240];
	if (*from == '\0') return;
#ifdef DEBUG
	(void) fprintf(stderr, "%d: %s '%s' << %s >>\n",
		*tally, from, stat, subj);
#endif
	if ((flags & BOX_NEWONLY) && *stat) {
		/* Not a new message, so skip it */
		*from = '\0';
		*stat = '\0';
		*subj = '\0';
		return;
	}
	if (*stat == '\0')
		(void) sprintf(line, "%s << %s >>\n",
				mungeSender(from, flags), subj);
	else
		(void) sprintf(line, "%s (%s) << %s >>\n",
				mungeSender(from, flags), stat, subj);
	if ((*remaining -= strlen(line)) > 0)
		(void) strcat(buffer, line);
	*from = '\0';
	*stat = '\0';
	*subj = '\0';
	(*tally)++;
}

int parseMailbox(mailbox, buffer, remaining, flags)
char *mailbox;
char *buffer;
int remaining;
int flags;
{	FILE *mf;
	char line[160];
	char fromfrom[80];
	char from[80];
	char subj[80];
	char stat[80];
	char *c, *n;
	int lastBlank;
	int headers;
	int tally = 0;
	int mmdf = 0;

#define TERMINATE(x) x[79] = '\0'; if (n = index(x, '\n')) *n = '\0';

#ifdef DEBUG
	(void) fprintf(stderr, "Mailbox: %s, flags: %d\n", mailbox, flags);
#endif

	if ((mf = fopen(mailbox, "r")) == NULL) {
		sprintf(buffer, "Failed to open %s", mailbox);
		return -1;
	}

	from[0] = '\0';
	subj[0] = '\0';
	stat[0] = '\0';
	lastBlank = 1;
	headers = 1;
	buffer[0] = '\0';

	for (;;) {
		if ((c = fgets(line, 160, mf)) == NULL) break;
		if (*c == 001) {
			/* Start of a new MMDF message */
			handlePrevious(buffer, &remaining, flags,
				from, stat, subj, &tally);
			lastBlank = 0;
			headers = 1;
			mmdf = 1;
			while (*c == 001) c++;
			if (*c == '\n') continue;
		}
#ifdef DEBUG
		(void) printf("%d, %d, <%s>\n", lastBlank, headers, c);
#endif
		if (*c == '\n') {
			lastBlank = 1;
			if (*from) headers = 0;
		}
		else if (!mmdf && lastBlank
				&& (sscanf(c, "From %s", fromfrom) > 0) 
				&& *fromfrom != ':') {
			/* Start of a new sendmail message */
			handlePrevious(buffer, &remaining, flags,
				from, stat, subj, &tally);
			(void) strcpy(from, fromfrom);
			lastBlank = 0;
			headers = 1;
		}
		else if (headers && (sscanf(c, "From: %70c", from) > 0)) {
			/* Proper 822 From: header */
			TERMINATE(from);
			lastBlank = 0;
		}
		else if (headers && (sscanf(c, "Subject: %70c", subj) > 0)) {
			/* Message subject */
			TERMINATE(subj);
			lastBlank = 0;
		}
		else if (headers && (sscanf(c, "Status: %s", stat) > 0)) {
			/* Message status */
			lastBlank = 0;
		}
		/* else something we're not interested in, so skip it */
		else lastBlank = 0;
	}
	handlePrevious(buffer, &remaining, flags, from, stat, subj, &tally);
	(void) fclose(mf);
	return tally;
}
