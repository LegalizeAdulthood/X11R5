/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/cui/RCS/morprntf.c,v 2.7 1991/09/12 15:51:13 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/cui/RCS/morprntf.c,v $ */

#ifndef NO_DUPLICATE_STATIC_CHARS
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/cui/RCS/morprntf.c,v 2.7 1991/09/12 15:51:13 bobg Exp $ ";
#endif /* NO_DUPLICATE_STATIC_CHARS */

#include <cui.h>
#include <hdrparse.h>
#include <errprntf.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/param.h>
#include <andrewos.h> /* sys/file.h sys/time.h */
#define CUI_SOURCE_CUIFNS_C
#include <cuimach.h>

extern char *index();
extern int CUI_SnapIsRunning;
extern char *GetLine();

int LinesOnTerminal = 24, LinesSincePause = 3,
    CharsOnLine = 1, TerminalLineWidth = 80;

NoMore() {
    LinesSincePause = 3;
    CharsOnLine = 1;
}

SetTerminalParams(h, w)
int h, w;
{
    LinesOnTerminal = h;
    TerminalLineWidth = w;
}

/* VARARGS */
moreprintf(format, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20)
char *format;
int  a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12,
     a13, a14, a15, a16, a17, a18, a19, a20;
{
    char    EnormousLine[10+MAXBODY],
	   *s,
	   *t;

    sprintf(EnormousLine, format, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20);
    for (s = t = EnormousLine; s && *t;) {
	s = index(t, '\n');
	if (s) {
	    if (SpitOutChars(t, s-t+1) == MORE_NO_MORE) return(MORE_NO_MORE);
	    t = s + 1;
	} else {
	    if (SpitOutChars(t, strlen(t)) == MORE_NO_MORE) return(MORE_NO_MORE);
	}
    }
    return(0);
}

SpitOutChars(t, len)
char *t;
int len;
{
    char *myline;
    int newlen;

    while (CharsOnLine+len > TerminalLineWidth) {
	newlen = TerminalLineWidth - CharsOnLine -1;
	fwrite(t, sizeof(char), newlen, stdout);
	fputc('\n', stdout);
	if (LinesOnTerminal && ++LinesSincePause > (LinesOnTerminal)) {
	    safefprintf(stdout, "-- More -- Press Enter to continue or 'q' to quit: ");
	    myline = GetLine();
	    if (myline == (char *) - 1) {
		clearerr(stdin);
	    } else {
		if (*myline == 'q' || *myline == 'Q' || *myline == 'n' || *myline == 'N') {
		    return(MORE_NO_MORE);
		} else if (*myline == 'c' || *myline == 'C') {
		    SetTerminalParams(0,80);
		}
	    }
	}
	CharsOnLine = 0;
	len -= newlen;
	t += newlen;
    }
    fwrite(t, sizeof(char), len, stdout);
    fflush(stdout);
    CharsOnLine += len;
    if (t[len-1] == '\n') {
	if (LinesOnTerminal && ++LinesSincePause > LinesOnTerminal) {
	    safefprintf(stdout, "-- More -- Press Enter to continue or 'q' to quit: ");
	    myline = GetLine();
	    if (myline == (char *) - 1) {
		clearerr(stdin);
	    } else if (*myline == 'q' || *myline == 'Q' || *myline == 'n' || *myline == 'N') {
		return(MORE_NO_MORE);
	    } else if (*myline == 'c' || *myline == 'C') {
		SetTerminalParams(0,80);
	    }
	}
	CharsOnLine = 0;
    }
    return(0);
}
