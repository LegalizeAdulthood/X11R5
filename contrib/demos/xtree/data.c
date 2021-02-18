/*
 * $XConsortium: data.c,v 1.3 90/05/08 14:06:48 jim Exp $
 *
 * Copyright 1990 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Jim Fulton, MIT X Consortium
 */

#include "xtree.h"
#include <X11/Xos.h>
#include <X11/Xmu/Converters.h>
#include <ctype.h>

#define NNODESTOALLOC 100

extern char *malloc(), *calloc();

void read_data (fp)
    FILE *fp;
{
    int lineno = 0;			/* line number for error messages */
    char tmpbuf[1024];			/* input line */
    char *line, *cp;			/* temp variables */
    Format fmt = GlobalResources.input_format;	/* format of input */
    char *sep = GlobalResources.separator;  /* separator character if needed */
    int seplen = strlen (sep);		/* len of separator if needed */
    int curdepth, depth;		/* counters for building tree */
    Node *freenodes, *cur = &rootnode;	/* list data struction */
    int nfreenodes = 0;			/* count of free nodes */
    Bool needbase = True;		/* need numeric base from input */

    switch (fmt) {
      case FORMAT_SEPARATOR:
      case FORMAT_NUMBER:
	break;

      default:
	fprintf (stderr,
		 "%s: invalid input format; must be separator or numeric\n",
		 ProgramName);
	exit (1);
    }

    curdepth = -1;			/* root */
    while (line = fgets (tmpbuf, sizeof tmpbuf, fp)) {
	lineno++;
	if (line[0] == '\n' || line[0] == '\0') continue;

	if (fmt == FORMAT_SEPARATOR) {
	    /* count depth, strip prefix */
	    for (depth = 0; strncmp (line, sep, seplen) == 0;
		 depth++, line += seplen) ;
	} else {
	    char *colon = index (line, ':');
	    if (!colon) {
		fprintf (stderr, "%s: line %d:  missing colon\n",
			 ProgramName, lineno);
		continue;
	    }
	    *colon = '\0';
	    depth = atoi (line);
	    if (needbase) {
		GlobalResources.number_origin = depth;
		needbase = False;
	    }
	    depth -= GlobalResources.number_origin;

	    for (line = colon + 1; *line && isascii(*line) && isspace(*line);
		 line++) ;
	}

	for (cp = line; *cp; cp++) {	/* nuke the \n */
	    if (*cp == '\n') {
		*cp = '\0';
		break;
	    }
	}

	/*
	 * allocate new nodes if necessary
	 */
	if (nfreenodes < 1) {
	    freenodes = (Node *) calloc (NNODESTOALLOC, sizeof(Node));
	    if (!freenodes) {
		fprintf (stderr, "%s:  unable to allocate %d nodes; exiting\n",
			 ProgramName, NNODESTOALLOC);
		exit (1);
	    }
	    nfreenodes = NNODESTOALLOC;
	}

	/*
	 * link into list
	 */

	freenodes->name = malloc (strlen(line) + 1);
	if (!freenodes->name) {
	    fprintf (stderr,
		     "%s: line %d: unable to allocate space for name\n",
		     ProgramName, lineno);
	    exit (1);
	}
	strcpy (freenodes->name, line);

	/*
	 * see if need to go back up tree; then insert in right place
	 */
	for (; curdepth > depth; curdepth--) cur = cur->parent;
	if (depth == curdepth) {	/* sibling */
	    cur->siblings = freenodes;
	    freenodes->parent = cur->parent;
	} else {			/* child */
	    cur->children = freenodes;
	    freenodes->parent = cur;
	    curdepth++;
	}


	/*
	 * remove from array of free nodes
	 */
	cur = freenodes;
	freenodes++;
	nfreenodes--;
    }
    return;
}


#define done(address, type) \
        { (*toVal).size = sizeof(type); (*toVal).addr = (caddr_t) address; }

static struct _namepair {
    XrmQuark quark;
    char *name;
    Format gravity;
} names[] = {
    { NULLQUARK, "name", FORMAT_NAME },
    { NULLQUARK, "resource", FORMAT_RESOURCE },
    { NULLQUARK, "res", FORMAT_RESOURCE },
    { NULLQUARK, "num", FORMAT_NUMBER },
    { NULLQUARK, "number", FORMAT_NUMBER },
    { NULLQUARK, "separator", FORMAT_SEPARATOR },
    { NULLQUARK, "sep", FORMAT_SEPARATOR },
    { NULLQUARK, NULL, 0 }
};

/* ARGSUSED */
void cvt_data_format (args, num_args, fromVal, toVal)
    XrmValuePtr args;
    Cardinal    *num_args;
    XrmValuePtr fromVal;
    XrmValuePtr toVal;
{
    static Boolean haveQuarks = FALSE;
    char lowerName[40];
    XrmQuark q;
    char *s;
    struct _namepair *np;

    if (*num_args != 0)
        XtWarningMsg("wrongParameters","cvt_data_format","XtToolkitError",
                  "String to DataFormat conversion needs no extra arguments",
                  (String *) NULL, (Cardinal *)NULL);

    if (!haveQuarks) {
	for (np = names; np->name; np++) {
	    np->quark = XrmStringToQuark (np->name);
	}
	haveQuarks = TRUE;
    }

    s = (char *) fromVal->addr;
    if (strlen(s) < sizeof lowerName) {
	XmuCopyISOLatin1Lowered (lowerName, s);
	q = XrmStringToQuark (lowerName);

	for (np = names; np->name; np++) {
	    if (np->quark == q) {
		done (&np->gravity, XtGravity);
		return;
	    }
	}
    }
    XtStringConversionWarning((char *) fromVal->addr, XtRGravity);
}
