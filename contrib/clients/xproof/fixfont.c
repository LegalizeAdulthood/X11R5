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
/*
 * Rearrange the glyphs in a .bdf file included in X11 Release 3 distribution.
 *
 * Input is a .bdf (bitmap distribution format) file.  Among
 * other things, it has groups of lines like the following:
 *     STARTCHAR quotedblleft
 *     ENCODING -1
 *     SWIDTH 444 0
 *     DWIDTH 6 0
 *     BBX 5 2 0 5
 *     BITMAP
 *     90
 *     D8
 *     ENDCHAR
 * This script alters the ENCODING line to fit the Adobe mapping.
 * It also fixes up a few lines describing global properties to specify
 * that the encoding is no longer ISO 8859.
 */

#ifndef lint
static char *rcsid="$Header: /src/X11/uw/xproof/RCS/fixfont.c,v 1.6 1991/10/04 22:05:44 tim Exp $";
#endif

#include <stdio.h>

/* The following table gives the standard Adobe encoding tables as specified
 * in "The PostScript(R) Lanugage Reference Manual", by Adobe Systems Inc.,
 * Reading, MA: Addison-Wesley, 1985.
 */
static char *charname[256] = {
#include "adobe.h"
};

char *sprintf(), *index(), *strcpy();

char special[256];  /* letters that can start a special line */

main() {
	char line[BUFSIZ];
	register char *p, *q;
	int code;

	special['S'] = special['E'] = special['C'] = special['F'] = 1;

	while (fgets(line, sizeof line, stdin)) {
		if (!special[((unsigned char *)line)[0]]) { /* quick check */
			(void) fputs(line,stdout);
			continue;
		}
		if (strncmp(line,"STARTCHAR",9)==0) {
			for (p=line+9; *p==' '; p++) continue;
			q = index(p,'\n');
			if (!q) {
				fprintf(stderr,"format error: \"%s\"\n",line);
				exit(1);
			}
			*q = 0;

			code = lookup(p);
			(void) puts(line);
			continue;
		}
		if (strncmp(line,"ENCODING",8)==0) {
			printf("ENCODING %d\n",code);
			continue;
		}
		if (strncmp(line,"CHARSET_REGISTRY",16)==0) {
			printf("CHARSET_REGISTRY \"ADOBE\"\n");
			continue;
		}
		if (strncmp(line,"CHARSET_ENCODING",16)==0) {
			printf("CHARSET_ENCODING \"FONTSPECIFIC\"\n");
			continue;
		}
		if (strncmp(line,"FONT ",5)==0) {
			for (p=line+4; p; p=index(p+1,'I')) {
				if (strncmp(p,"ISO8859-1",9)==0) {
					*p = 0;
					printf("%sADOBE-FONTSPECIFIC\n",line);
					break;
				}
			}
			continue;
		}
		(void) fputs(line,stdout);
	}
	if (ferror(stdin)) {
		perror("stdin");
		exit(1);
	}

	exit(0);
}

lookup(s)
char *s;
{
	register int i;

	for (i=0;i<256;i++) if (strcmp(charname[i],s)==0) return i;
	return -1;
}
