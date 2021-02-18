/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/reyacc.c,v 1.4 1991/09/12 16:27:21 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/reyacc.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/reyacc.c,v 1.4 1991/09/12 16:27:21 bobg Exp $";
#endif
/* 
	reyacc.c

	Usage:
		reyacc filename.c yaccpar [oldyaccpar]

	Massage 'filename.c' (which is yacc output) removing /usr/lib/yaccpar
	(or the yaccpar given as third argument) and installing 'yaccpar' 
	in its place.  It is assumed that 'yaccpar' contains
	the string "$@" at the point where the semantics case statement goes.

	Also delete the stuff generated between the lines containing
		#define ZZLASTTOKEN nnn
	(for integer nnn) and
		/*ZZSTARTLEX...\n

	Output replaces the input file.  The input file is renamed to 
		filename.c.sysyacc

*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <class.h>		/* for boolean */

char *qsearch();

Die(msg)
	char *msg;
{
	printf("%s\n", msg);
	exit(9);
}

/* Massage(s, old, new, f)
	write to f a revised version of s:
		delete text  #define ZZ ... DESC
		replace yaccpar before and after semantics rules
*/
	void
Massage(s, old, new, f)
	char *s, *old, *new;
	FILE *f;
{
	long	oldlen = strlen(old), 
		newlen = strlen(new);
	char *sx, *send, *newtail, *oldtail;
	long k;
	static char *zzlast = "ZZLASTTOKEN";
	static char *zzlex = "/*ZZSTARTLEXDESC*/\n";

	send = s + strlen(s);

	/* output everything before #define ZZLASTTOKEN 
		and reset s and slen for the part after ZZSTARTLEXDESC */
	sx = qsearch(s, send-s, zzlast, strlen(zzlast));
	if (sx != NULL) {
		while (*(sx-1) != '\n') sx--;	/* ensure start of line */
		/* output up to sx and reset s to start after zzlex */
		fwrite(s, 1, sx - s, f);
		sx = qsearch(sx, send-sx, zzlex, strlen(zzlex));
		if (sx == NULL) 
			Die("input file missing ZZSTARTLEXDESC");
		while (*sx++ != '\n') {};   /* get to start of next line */
		s = sx;
	}

	/* find the prefix of old in s */
	sx = qsearch(s, send-s, old, 1600);  /* first find 1600 bytes */
	if (sx == NULL) 
		Die("file does not contain old yacc");
	fwrite(s, 1, sx-s, f);		/* write s up to old */
	s = sx;
	k = qmatch(s, send-s, old, oldlen);  /* match prefix of old */
	s = sx + k;      /* and skip the prefix in s */

	/* write new in place of old and set newtail to the tail end */
	newtail = qsearch(new, newlen, "$@", 2);
	if (newtail == NULL) 
		Die("new yacc does not contain $@");
	fwrite(new, 1, newtail-new, f);
	newtail += 2;

	/* set oldtail to the beginning in old of the final match 
			to the tail end of s */
	for (oldtail = old + oldlen - 1, sx = send - 1;
			*oldtail == *sx; 
			oldtail--, sx--)
		{}
	oldtail++;
	send = sx + 1;   /* remove oldtail from s */

	/* write the semantics rules and the tail piece of new */
	fwrite(s, 1, send-s, f);
	fwrite(newtail, 1, newlen - (newtail-new), f);
}

	char *
grabfile(fnm)
	char *fnm;
{
	char *v;
	struct stat sbuf;
	FILE *f = fopen(fnm, "r");
	if (f == NULL || fstat (fileno(f), &sbuf) < 0) {
		printf("failed to find %s\n", fnm);
		exit (9);
	}
	v = malloc(sbuf.st_size + 1);
	fread(v, 1, sbuf.st_size, f);
	fclose(f);
	return v;
}

main(argc, argv)
	char **argv;
{
	char *fnm = argv[1];
	char *ynm = argv[2];
	char buf[2000];
	FILE *f;
	char *instring, *newyacc, *sysyacc;
	
	if (argc < 3 || argc > 4) {
		printf("Usage: %s filename.c newyaccpar  [oldyaccpar]\n",
				argv[0]);
		exit(9);
	}

	instring = grabfile(fnm);
	newyacc = grabfile(ynm);
	sysyacc = grabfile((argc==3) ? "/usr/lib/yaccpar" : argv[3]);

	sprintf(buf, "%s.newyacc", fnm);
	f = fopen(buf, "w");
	Massage(instring, sysyacc, newyacc, f);
	fclose(f);	

	/* rename fnm to fnm.sysyacc and fnm.newyacc to fnm */
	sprintf(buf, "%s.sysyacc", fnm);
	rename(fnm, buf);
	sprintf(buf, "%s.newyacc", fnm);
	rename(buf, fnm);
	printf("File %s revised to use yacc %s\n", fnm, ynm);
	exit(0);
}
