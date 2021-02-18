/*
 * Copyright 1991 University of Wisconsin-Madison
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of Wisconsin-Madison not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  The University of
 * Wisconsin-Madison makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF WISCONSIN-MADISON DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF WISCONSIN-MADISON BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:   Marvin Solomon
 *           Computer Sciences Department
 *           University of Wisconsin, Madison WI
 *           solomon@cs.wisc.edu or seismo!uwvax!solomon
 *
 * Current Maintainer:
 *
 *           Tim Theisen           Systems Programmer
 * Internet: tim@cs.wisc.edu       Department of Computer Sciences
 *     UUCP: uwvax!tim             University of Wisconsin-Madison
 *    Phone: (608)262-0438         1210 West Dayton Street
 *      FAX: (608)262-9777         Madison, WI   53706
 */
#include "xproof.h"
#include <ctype.h>

#ifndef lint
static char *rcsid="$Header: /src/X11/uw/xproof/RCS/showpage.c,v 3.7 1991/10/04 22:05:44 tim Exp $";
#endif

extern char *malloc();
extern char *device;

static void Devinit();
static int LookupSize();
static void ShowSpecial();

/* Main driver routine.  Display one page of output (from the current position
 * in file 'f' up through the next 'p', 'x trailer', 'x pause', or 'x stop'
 * command, or until EOF).
 * Return zero on success (found 'p' or 'x trailer' command).
 * Return non-zero on failure (including 'x stop' or 'x pause' command.
 */
ShowPage(w, f)
ProofWidget w;
register FILE *f;
{
	register int c;
#ifdef sequent
	int c1;
#endif
	int n, m, n1, m1;
	char str[BUFSIZ];
	int nargs;
#	define NL() do c = getc(f); while (c != '\n' && c != EOF)
#	define Fscanf(x,y,z) if ((nargs=fscanf(x,y,z))!=1) \
		{ (void)fprintf(stderr,"line %d(%s) expected 1 value, found %d\n", \
		__LINE__,y,nargs); break; }
#	define Fscanf2(x,y,z1,z2) if ((nargs=fscanf(x,y,z1,z2))!=2) \
		{ (void)fprintf(stderr,"line %d(%s): expected 2 values, found %d\n", \
		__LINE__,y,nargs); break; }

	for (;;) {
		c = getc(f);
		switch (c) {
		/* The following commands (up through 'w') only consume a few characters
		 * of input and allow the rest of the input line to be inspected by the
		 * next iteration.
		 */
			case '\n':
			case ' ':
			case 0:
				/* skip excess white space */
				break;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				hpos += (c-'0')*10 + getc(f)-'0';
				/* fall through */
			case 'c':	/* single ascii character */
				ShowChar(w, getc(f));
				break;
			case 'C':
				Fscanf(f, "%s", str);
				ShowSpecial(w, str);
				break;
			case 'h':	/* relative horizontal motion */
				Fscanf(f, "%d", &n);
				hpos += n;
				break;
			case 'v':   /* relative vertical motion */
				Fscanf(f, "%d", &n);
				vpos += n;
				break;
			case 'w':	/* word space */
				break;
		/* The remaining commands consume a whole line */
			case 's':
				Fscanf(f, "%d", &n);
				cursize = LookupSize(n);
				NL(); break;
			case 'f':
				Fscanf(f, "%d", &curmount);
				curface = mounted[curmount];
				NL(); break;
			case 'i':   /* set stipple "font".  Ignore this nonsense! */
				Fscanf(f, "%d", &n);
				NL(); break;
			case 'H':	/* absolute horizontal position */
				Fscanf(f, "%d", &n);
				hpos = n;
				NL(); break;
			case 'V':   /* absolute vertical position */
				Fscanf(f, "%d", &n);
				vpos = n;
				NL(); break;
			case 'p':	/* new page */
				Fscanf(f, "%d", &n);
				NL();
				return 0;
			case 'n':	/* end of line */
				hpos = 0;
				NL(); break;
			case '#':	/* comment */
				NL(); break;
			case 'x':	/* device control */
				while ((c=getc(f))==' ') /* empty body */;
				/* c is the first character of the subcommand.
				 * Ignore the rest of the word.
				 */
#ifdef sequent
				while (isprint(c1=getc(f)) && !isspace(c1)) /* empty body */;
#else
				while (isgraph(getc(f))) /* empty body */;
#endif
				switch (c) {
					case 'i':	/* initialize */
						Devinit(w);
						break;
					case 'T':	/* device name */
						/* ignored */
						break;
					case 't':	/* trailer */
						NL();
						return 0;
					case 'p':	/* pause */
					case 's':	/* stop */
						return -1;
					case 'r':	/* resolution assumed when prepared */
						Fscanf(f, "%d", &n);
						resolution = n;
						break;
					case 'f':	/* font used */
						Fscanf(f, "%d", &n);
						Fscanf(f, " %s", str);
						MountFace(n, str);
						break;
					case 'H':	/* char height (ignored) */
						Fscanf(f, "%d", &n);
						break;
					case 'S':	/* slant (ignored) */
						Fscanf(f, "%d", &n);
						break;
				} /* end of case 'x' */
				NL(); break;
			case 'D':	/* draw function */
				switch (c = getc(f)) {
					case 'l':	/* draw a line */
						Fscanf2(f, "%d %d", &n, &m);
						drawline(w, n, m);
						break;
					case 'c':	/* circle */
						Fscanf(f, "%d", &n);
						drawcirc(w, n);
						break;
					case 'e':	/* ellipse */
						Fscanf2(f, "%d %d", &n, &m);
						drawellipse(w, n, m);
						break;
					case 'a':	/* arc */
						Fscanf2(f, "%d %d", &n, &m);
						Fscanf2(f, "%d %d", &n1, &m1);
						drawarc(w, n, m, n1, m1);
						break;
					case 'P':   /* borderless polygon */
						Fscanf(f, "%d", &m);  /* Fill color */
						drawpolygon(w, f, m, 0);
						break;
					case 'p':	/* polygon */
						Fscanf(f, "%d", &m);  /* Fill color */
						drawpolygon(w, f, m, 1);
						break;
					case 'g':	/* 'gremlin' (interpolatory) spline */
						drawcurve(w, f, 0);
						break;
					case '~':	/* 'pic' (B ?) spline */
					case 'z':	/* fake Bezier curve */
						drawcurve(w, f, 1);
						break;
					case 't':	/* set line thickness */
						Fscanf(f, "%d", &n);
						drawthick(w, n);
						break;
					case 's':	/* line style */
						Fscanf(f, "%d", &n);
						drawstyle(w, n);
						break;
					default:
						(void)fprintf(stderr,
							"unknown drawing function %c\n",c);
						break;
				} /* end of case 'D' */
				NL(); break;
			case EOF:
				return -2;
			default:
				(void)fprintf(stderr,"unknown input character %o %c\n", c, c);
				break;
		} /* switch(c) */
	} /*  for(;;) */
}

static void Devinit(w)
ProofWidget w;
{
	GetDevInfo();
	cursize = LookupSize(10);
	curface = mounted[1];
	curmount = 1;
	GetXfontInfo(w);
}

static int LookupSize(n)   /* round point size to nearest available */
int n;
{
	int i;

	for (i=0; i<nsizes; i++)
		if (n <= size[i]) return i;
	return nsizes;
}


/* Show  "special" character \(xx, where 'name'=="xx".  Just do a linear
 * search through the 'fancy_char' array, but first look where we found
 * it last time.  This latter hack can be a SUBSTANTIAL speedup for tbl
 * output, which has long sequences of special line-drawing characters.
 */
static void ShowSpecial(w, name)
ProofWidget w;
register char *name;
{
	static int last = 0;
	register int i;

	if (strcmp(name, fancy_char[last]) == 0) {
		ShowChar(w, last+128);
		return;
	}
	for (i=0; i<nspecial; i++) {
		if (strcmp(name, fancy_char[i]) == 0) {
			last = i;
			ShowChar(w, i+128);
			return;
		}
	}
}

/* Prime the pump by scanning through the first few lines of the file
 * executing the commands there.  Stop at the first unrecognized command.
 * The only commands recognized are 'x init', 'x T', 'x res', and 'x font'.
 * This procedure is essentially a duplication of a tiny portion of ShowPage,
 * which is called before any windows are created, so it must be careful
 * not to try to show anything.  It's main purpose is to determine the
 * device resolution and mount some fonts.
 * It returns non-zero if the file appears not to be in troff output format:
 *    -1:  file is empty
 *    -2:  bad format (doesn't start with "x T")
 *    -3:  I/O error of some sort
 */
PrimePump(w, f)
ProofWidget w;
register FILE *f;
{
	char buf[BUFSIZ];
	int c, n, nargs;
#ifdef sequent
	int c1;
#endif

	if (!fgets(buf,sizeof buf,f))
		return feof(f) ? -1 : -3;

	device = malloc(strlen(buf));
	if ( device == NULL ) {
		perror("malloc");
		exit(1);
	}

	if ( sscanf( buf, "x T %s", device) != 1 )
		return -2;

	while ((c = getc(f))=='x') {
		while ((c=getc(f))==' ') /* empty body */;
		switch (c) {
			case 'i':	/* initialize */
				Devinit(w);
				break;
			case 'T':	/* device name */
				/* ignored */
				break;
			case 'r':	/* resolution assumed when prepared */
				/* Ignore the rest of the subcommand. */
#ifdef sequent
				while (isprint(c1=getc(f)) && !isspace(c1)) /* empty body */;
#else
				while (isgraph(getc(f))) /* empty body */;
#endif
				Fscanf(f, "%d", &n);
				resolution = n;
				break;
			case 'f':	/* font used */
				/* Ignore the rest of the subcommand. */
#ifdef sequent
				while (isprint(c1=getc(f)) && !isspace(c1)) /* empty body */;
#else
				while (isgraph(getc(f))) /* empty body */;
#endif
				Fscanf(f, "%d", &n);
				Fscanf(f, " %s", buf);
				MountFace(n, buf);
				break;
			default:
				/* Oops, went too far.  We'd like to simply seek back,
				 * but this may be stdin (hence unseekable).
				 * I sure hope I can put back three characters!
				 */
				(void) ungetc(c, f);
				(void) ungetc(' ', f);
				(void) ungetc('x', f);
				return 0;
		}
		NL();
	}
	(void) ungetc(c, f);
	return 0;
}
