/*
 * $XConsortium: xtail.c,v 1.9 91/05/01 11:01:52 rws Exp $
 */

/*
 * Copyright 1990, 1991 by OMRON Corporation, NTT Software Corporation,
 *                      and Nippon Telegraph and Telephone Corporation
 * Copyright 1991 by the Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON, NTT Software, NTT, and M.I.T.
 * not be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission. OMRON, NTT Software,
 * NTT, and M.I.T. make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * OMRON, NTT SOFTWARE, NTT, AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL OMRON, NTT SOFTWARE, NTT, OR M.I.T. BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 *	Author: Tatsuya Kato	NTT Software Corporation
 *   
 */


/*
 * xtail
 */

#include <X11/Xlocale.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>

#define RESTWIDTH	3
#define RESTHEIGHT	3

#define	BUFSIZE		1024
#define	MAXLINES	100
#define	DEFLINES	20
#define	COLUMNS		80

#define TABWIDTH	8

#define Masks	(KeyPressMask | ButtonPressMask | ExposureMask)

char *malloc();

Window          win;
Display        *display;
int             screen;

char           *Lines[MAXLINES];
int             NumofLines = DEFLINES;
int             Index = 0;
char           *FontList = "-*-fixed-*-r-*--14-*-*-*-c-*";

Bool            Forever = False;
char		*locale;

char           *displayname = NULL, *geometry, *filename = NULL;

char           *
strDupExtend(str)
char           *str;
{
    int             pos = 0, i;
    char            buf[BUFSIZE];
    char           *src = str, *dst = buf;

    while (*src) {
	switch (*src) {
	case '\t':
	    for (i = 0; i < (TABWIDTH - pos % TABWIDTH); i++) {
		*dst++ = ' ';
	    }
	    pos += (TABWIDTH - pos % TABWIDTH);
	    break;
	case '\n':
	    *src = '\0';
	    continue;
	default:
	    *dst++ = *src;
	    pos++;
	    break;
	}
	src++;
    }
    *dst = '\0';

    str = malloc(strlen(buf)+1);
    strcpy(str, buf);
    return str;
}

Syntax()
{
    fprintf(stderr, "Usage: xtail [+|-<num>] [-f] [filename]");
}

ParseArg(argc, argv)
int             argc;
char           *argv[];

{
    int             i;

    for (i = 1; i < argc; i++) {
	char           *arg = argv[i];

	if (arg[0] == '-') {
	    if (strcmp("-help", arg) == 0) {
		Syntax();
	    } else if (strcmp("-display", arg) == 0) {
		if (++i >= argc)
		    Syntax();
		displayname = argv[i];
		continue;
	    } else if (strcmp("-geometry", arg) == 0) {
		if (++i >= argc)
		    Syntax();
		geometry = argv[i];
		continue;
	    } else if (strcmp("-lc", arg) == 0) {
		if (++i >= argc)
		    Syntax();
		locale = argv[i];
		continue;
	    } else if (strcmp("-fn", arg) == 0) {
		if (++i >= argc)
		    Syntax();
		FontList = argv[i];
		continue;
	    } else if (strcmp("-f", arg) == 0) {
		Forever = True;
		continue;
	    } else {
		if ((NumofLines = atoi(&arg[1])) == 0) {
		    Syntax();
		}
		continue;
	    }
	} else if (arg[0] == '+') {
	    if ((NumofLines = atoi(&arg[1])) == 0) {
		Syntax();
	    }
	} else
	    filename = arg;
    }
}

GetInput(fname)
char  *fname;
{
    char           buf[BUFSIZE];
    FILE           *fp = stdin;
    int             loop = 0;

    if (fname) {
	if ((fp = fopen(fname, "r")) == NULL) {
	    perror(fname);
	    exit(1);
	}
    }
    while (fgets(buf, BUFSIZE, fp)) {
	if (loop > 0) {
	    free(Lines[Index]);
	}
	if ((Lines[Index] = strDupExtend(buf)) == NULL) {
	    perror(NULL);
	    exit(2);
	}
	Index++;
	if (Index >= NumofLines) {
	    Index = 0;
	    loop++;
	}
    }

    if (loop == 0) {
	NumofLines = Index;
    }
}

GC              gcontext;
XFontSet        fontset;
XFontSetExtents *fontsetext;

SetContext()
{
    char           *getenv();
    char          **missing_clist, *def_str;
    int             missing_count;

    if (!locale)
	locale = "";
    if (!setlocale(LC_ALL, locale)) {
	fprintf(stderr, "Can't setlocale to \"%s\".\n", locale);
	exit(1);
    }
    fprintf(stderr, "Locale: %s\n", setlocale(LC_ALL, NULL));
    if ((fontset = XCreateFontSet(display, FontList, &missing_clist,
				  &missing_count, &def_str)) == NULL) {
	fprintf(stderr, "Can't allocate fontset.\n");
	exit(1);
    }
    if (missing_count > 0) {
	int             i;

	printf("missing charset:");
	for (i = 0; i < missing_count; i++) {
	    printf(" %s", missing_clist[i]);
	}
	printf("\n");
    }
    fontsetext = XExtentsOfFontSet(fontset);
    gcontext = XCreateGC(display, win, 0, NULL);
    XSetForeground(display, gcontext, BlackPixel(display, screen));
}

CheckSize(width, height)
int            *width, *height;
{
    int             i;

    *height = fontsetext->max_logical_extent.height
	    * NumofLines + RESTHEIGHT * 2;
    *width = 0;

    for (i = 0; i < NumofLines; i++) {
	int             tmpw;

	tmpw = XmbTextEscapement(fontset, Lines[i], strlen(Lines[i]));
	if (tmpw > *width) {
	    *width = tmpw;
	}
    }

    *width += RESTWIDTH * 2;
}

WriteText(width, height)
int             width, height;
{
    int             i, y = fontsetext->max_logical_extent.height + RESTHEIGHT;

    XClearArea(display, win, 0, 0, width, height, False);
    for (i = 0; i < NumofLines; i++) {
	if (Index >= NumofLines) {
	    Index = 0;
	}
	XmbDrawString(display, win, fontset, gcontext, RESTWIDTH, y,
		      Lines[Index], strlen(Lines[Index]));
	Index++;
	y += fontsetext->max_logical_extent.height;
    }
}

main(argc, argv)
int             argc;
char           *argv[];

{
    XEvent          report;
    XSizeHints      hints;
    int             width, height;

    ParseArg(argc, argv);
    GetInput(filename);

    if ((display = XOpenDisplay(displayname)) == NULL) {
	fprintf(stderr, "Can not connect to display %s\n", XDisplayName(NULL));
	exit(1);
    }
    win = XCreateSimpleWindow(display, RootWindow(display, screen),
			      0, 0, 100, 100, 2,
		  BlackPixel(display, screen), WhitePixel(display, screen));

    XSelectInput(display, win, Masks);

    SetContext();
    CheckSize(&width, &height);

    hints.width = width;
    hints.height = height;
    hints.flags = PSize | USSize;

    XSetStandardProperties(display, win, "XTail", argv[0],
			   None, argv, argc, &hints);

    XResizeWindow(display, win, width, height);
    XMapWindow(display, win);

    for (;;) {
	XNextEvent(display, &report);
	switch (report.type) {
	case Expose:
	    if (report.xexpose.count == 0)
		WriteText(width, height);
	    break;
	case ButtonPress:
	case KeyPress:
	    XFreeFontSet(display, fontset);
	    XCloseDisplay(display);
	    exit(0);
	}
    }
}
