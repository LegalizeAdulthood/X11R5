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
#include <stdio.h>
#include <ctype.h>
#include <string.h>
extern char *malloc();
extern void exit(), perror();

char table_start[] = "} font_map[] = {\n";
char table_end[] = "0, NONE, NONE, NONE\n";

main(argc,argv)
char **argv;
{
	FILE *oldxf, *newtable;
	char buf[BUFSIZ];

	if (argc != 3) {
usage:	(void)fprintf(stderr,
				"usage: %s xfontinfo.c.bak newfonttable\n",argv[0]);
		exit(1);
	}
	oldxf = fopen(argv[1],"r");
	if (oldxf==0) {
		perror(argv[1]);
		exit(1);
	}
	newtable = fopen(argv[2],"r");
	if (newtable==0) {
		perror(argv[2]);
		exit(1);
	}

	/* copy the head of oldxf */
	for(;;) {
		if (!fgets(buf, sizeof buf, oldxf)) {
			(void)fprintf(stderr,"%s: bad format\n",argv[1]);
			exit(1);
		}
		(void)printf("%s",buf);
		if (strcmp(buf,table_start)==0) break;
	}
	/* read in the old table */
	for (;;) {
		if (!fgets(buf, sizeof buf, oldxf)) {
			(void)fprintf(stderr,"%s: bad format\n",argv[1]);
			exit(1);
		}
		if (strcmp(buf,table_end)==0) break;
		parse_line(buf);
	}
	/* run through the new table, looking things up in the old table */
	for (;;) {
		if (!fgets(buf, sizeof buf, newtable)) break;
		do_line(buf);
	}
	/* copy the tail of oldxf */
	(void)printf("%s",table_end);
	for (;;) {
		if (!fgets(buf, sizeof buf, oldxf)) break;
		(void)printf("%s",buf);
	}
	return 0;
}


struct item {
	char *name, *props;
} table[1000];
int tabsize;

struct item *lookup(s)
char *s;
{
	int i;
	for (i=0; i<tabsize; i++)
		if (strcmp(table[i].name, s)==0) return table+i;
	return 0;
}

char *copy(s)
char *s;
{
	char *res = malloc((unsigned)strlen(s)+1);
	return strcpy(res,s);
}

parse_line(line)
char *line;
#if 0
"AG"\t/* AvantGarde-BookOblique */,\t\tHELVETICA, MEDIUM, OBLIQUE,$
#endif
{
	char *s = index(line,'/');
	char *t;

	if (!*s) {
bad:
		(void)fprintf(stderr,"bad format: %s",line);
		exit(1);
	}
	s++;
	if (*s++ != '*') goto bad;
	while (isspace(*s)) s++;
	for (t=s; *t && !isspace(*t); t++) continue;
	if (*t) *t++ = 0;
	table[tabsize].name = copy(s);
	s = index(t,',');
	if (!s) goto bad;
	table[tabsize].props = copy(s);
	tabsize++;
}

do_line(line)
char *line;
#if 0
AG\tAvantGarde-DemiOblique$
#endif

{
	char *s;
	struct item *p;

	s = index(line,'\n');
	if (*s) *s = '\0';
	s = index(line,'\t');
	if (!s) {
bad:
		(void)fprintf(stderr,"bad format: %s",line);
		exit(1);
	}
	*s++ = '\0';
	(void)printf("\"%s\"\t",line);
	if (strlen(line)<2) (void)printf("\t");
	(void)printf("/* %s */",s);
	if (p = lookup(s)) (void)printf("%s",p->props);
	else (void)printf(",				??, ??, ??,\n");
}
