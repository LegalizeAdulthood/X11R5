/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/cmd/RCS/dumpbin.c,v 2.7 1991/09/12 16:47:06 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/cmd/RCS/dumpbin.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/cmd/RCS/dumpbin.c,v 2.7 1991/09/12 16:47:06 bobg Exp $ ";
#endif /* lint */

#include <andrewos.h>
#include <stdio.h>
#include <ctype.h>

#define NUMLONGS 1 /* EXP_MAXOPTS/32 */

extern char *index();

static char pref[]="messages.binaryoptions";
long hatol();

main(argc, argv)
int argc;
char **argv;
{
FILE *f;
char line[256];
register char *s = 0, *t, *u;
long dum, dum2;
register int i, offset=0;

    if (argc != 2) {
	printf("usage: dumpbin preferencefile\n");
	exit(0);
    }
    if ((f = fopen(*++argv, "r")) == NULL) {
	printf("Could not open preference file '%s' for input.\n", *argv);
	exit(0);
    }
    while (fgets(line, sizeof(line), f)) {
	fold(line);
	if (!strncmp(line, pref, sizeof(pref)-1)) {
	    s = line+sizeof(pref);
	    break;
	}
    }
    fclose(f);
    if (!s) exit(1);
    while (s && offset <= NUMLONGS) {
	t = index(s, ',');
	if (t) *t++ = '\0';
	u = index(s, '/');
	if (u) *u++ = '\0';
	dum = hatol(s);
	dum2 = u ? hatol(u) : 0;
	dum &= dum2;
	for (i=0;i<32;i++, dum>>=1) if (dum&1) printf("%.02d\n", i+offset*32);
	s = t;
	++offset;
    }
}

/***********************************************************************/

long hatol(s)
char *s;
{
    long n;
    char c;

    n = 0;
    while (c = *s) {
	if (c >= '0' && c <= '9') {
	    n = (16 * n) + c - '0';
	} else if (c >= 'a' && c <= 'f') {
	    n = (16 * n) + c - 'a' + 10;
	} /* ignore all other characters, including leading 0x and whitespace */
	++s;
    }
    return(n);
}	    

/***********************************************************************/

fold(s)
register char *s;
{
    while (*s) {
	if (isupper(*s)) *s = tolower(*s);
	s++;
    }
    if (*--s == '\n') *s = '\0';
}
