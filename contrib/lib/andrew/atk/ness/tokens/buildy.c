/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/buildy.c,v 1.8 1991/08/15 20:27:10 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/buildy.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/buildy.c,v 1.8 1991/08/15 20:27:10 wjh Exp $";
#endif

/*
	buildy.c

	WJHansen, 21 November, 1987

	The program processes a xxx.gra file to generate xxx.y.
	Its principal effect is to append lexdesc(), which returns a pointer 
	to a static area with descriptions of the lexemes.

	Process %token lines to extract token names.
	The reserved word for token WORD is "word".
	The prefix "tok" is stripped from the string versions
	so the token names tokNULL tokFALSE and tokTRUE do
	not conflict with C defined symbols.
	The prefix "set" means to ignore the token: it is
	handled otherwise.

	A new line, %thong, is defined.  It has a token name
	followed by a string of characters making up a
	multi-character token.  The line is converted to a %token
	line and information is generated to build the thonglist.

	Four "standard" token classes are looked for:
		setID  setINTCON   setREALCON   setSTRINGCON
	the toknum values for these classes are stored in the struct lexdescription.

The generated lexdesc() has this form:

	/*ZZSTARTLEXDESC...
	static char *reswords[] = {
		<one constant for each reserved word>,
		NULL
	};
	static short restoks[] = {
		<one token number for each reserved word>
	};
	static char *thongs[] = {
		<one constant for each thong>,
		NULL
	};
	static short thongtoks[] = {
		<one token number for each thong>
	};
	
	static struct lexdescription lexdescription = {
		(unsigned char **)reswords, restoks,
		(unsigned char **)thongs, thongtoks,
		setID, setINTCON, setREALCON, setSTRINGCON
	};

		struct lexdescription *
	<filestem>_lexdesc()
	{
		return &lexdescription;
	}

*/

#include <andrewos.h>	/* for rindex */
#include <class.h>		/* for boolean */
#include <stdio.h>
#include <ctype.h>

unsigned char filestem[100];		/* stem of input filename */

unsigned char toknms[10000];	/* stores token names each followed by a space
			      the string is terminated with \0	*/

struct thong {
	char name[30], chars[10];
	struct thong *next;
} *thonglist = NULL;		/* list of thongs */


#define getput(inf,outf) (((c=getc(inf))==EOF) ? c : putc(c, outf))


PutLexDesc(outfile)
	FILE *outfile;
{
	unsigned char lower[50];
	register c;
	unsigned char *tx, *lx, *startx;
	struct thong *t;

	fprintf(outfile, "/*ZZSTARTLEXDESC*/\n");

	/*	static char *reswords[] = {
			<one constant for each reserved word>,
			NULL
		};
	*/
	fprintf(outfile, "static char *reswords[] = {\n");
	tx = toknms;
	while (*tx) {
		/* accumulate name */
		startx = tx;  lx = lower;
		while ((c = *tx++) != ' ')
			*lx++ = (isupper(c)) ? tolower(c) : c;
		*lx = '\0';

		/* filter out special token names */
		if (strncmp(lower, "set", 3)==0) continue;
		lx = (strncmp(lower, "tok", 3) == 0) ? lower + 3 : lower;

		fprintf(outfile, "\t\"%s\",\n", lx);
	}
	fprintf(outfile, "\tNULL\n};\n");
	/*
		static short restoks[] = {
			<one token number for each reserved word>
		};
	*/
	fprintf(outfile, "static short restoks[] = {\n");
	tx = toknms;
	while (*tx) {
		/* accumulate name */
		startx = tx;  lx = lower;
		while ((c = *tx++) != ' ') 
			*lx++ = (isupper(c)) ? tolower(c) : c;

		/* filter out special token names */
		if (strncmp(lower, "set", 3)==0) continue;

		fprintf(outfile, "\t%.*s,\n", tx-startx-1, startx);
	}
	fprintf(outfile, "};\n");
	/*
		static char *thongs[] = {
			<one constant for each thong>,
			NULL
		};
	*/
	fprintf(outfile, "static char *thongs[] = {\n");
	for (t = thonglist; t; t = t->next) 
		fprintf(outfile, "\t\"%s\",\n", t->chars);
	fprintf(outfile, "\tNULL\n};\n");
	/*
		static short thongtoks[] = {
			<one token number for each thong>
		};
	*/
	fprintf(outfile, "static short thongtoks[] = {\n");
	for (t = thonglist; t; t = t->next) 
		fprintf(outfile, "\t%s,\n", t->name);
	fprintf(outfile, "};\n");
	/*	
		static struct lexdescription lexdescription = {
			(unsigned char **)reswords, restoks,
			(unsigned char **)thongs, thongtoks,
			setID, setINTCON, setREALCON, setSTRINGCON
		};

			struct lexdescription *
		<filestem>_lexdesc()
		{
			return &lexdescription;
		}
	*/
	fprintf(outfile, "static struct lexdescription lexdescription = {\n");
	fprintf(outfile, "	(unsigned char **)reswords, restoks,\n");
	fprintf(outfile, "	(unsigned char **)thongs, thongtoks,\n");
	fprintf(outfile, "	setID, setINTCON, setREALCON, setSTRINGCON\n");
	fprintf(outfile, "};\n");
	fprintf(outfile, "\n");
	fprintf(outfile, "	struct lexdescription *\n");
	fprintf(outfile, "%s_lexdesc()\n", filestem);
	fprintf(outfile, "{\n");
	fprintf(outfile, "	return &lexdescription;\n");
	fprintf(outfile, "}\n");
}

static enum parsestate {
	Start,
	SawPercent,
	InType,
	InTokens,
	Copying,
} State;
static char typebuf[100] = "", *tbx;

	static enum parsestate
CheckForLastPercent(inf, outf)
	FILE *inf, *outf;
{
	register c;		/* set by getput() */
	c = getc(inf);
	if (c == '%') {
		/* bracket %% with markers to locate code to be ripped out 
			by reyacc 
			note that initial % is already in outf */
		*tbx = '\0';	/* terminate the most recently saved type name */
		if (*typebuf)
			fprintf(outf, "token <%s> ZZLASTTOKEN\n%%%%\n",typebuf);
		else
			fprintf(outf, "token ZZLASTTOKEN\n%%%%\n");
		return Copying;
	}
	else {
		ungetc(c, inf);
		return SawPercent;
	}
}

SkipComment(inf, outf)
	FILE *inf, *outf;
{
	register c;		/* set by getput() */
	boolean SawStar = FALSE;
	while (getput(inf,outf) != EOF)
		if (c == '/'  &&  SawStar)
			return;
		else SawStar = (c == '*');
}

main(argc, argv)
	unsigned char **argv;
{
	FILE *outf, *inf;
	register c;	    /* is set by getput */
	register unsigned char *tx;  /* where to store tokens */
	unsigned char buf[200];
	unsigned char *bx, *dot;

	dot = (unsigned char *)rindex(argv[1], '.');
	if (dot)
		sprintf(filestem, "%.*s", dot-argv[1], argv[1]);
	else
		strcpy(filestem, argv[1]);

	inf = fopen(argv[1], "r");
	if (argc > 2)
		outf = fopen(argv[2], "w");
	else {
		char outname[300];
		sprintf(outname, "%s.y", filestem);
		outf = fopen(outname, "w");
	}
	if (inf == NULL) {
		printf("Invalid input file name.  ");
		perror("");
		exit(1);
	}
	if (outf == NULL) {
		printf("Invalid output file name.  ");
		perror("");
		exit(1);
	}


	tbx = typebuf;
	tx = toknms;
	State = Start;
	while (getput(inf,outf) != EOF) switch(State) {

	case Start:
		if (c == '/') {
			if (getput(inf,outf) == '*')
				SkipComment(inf, outf);
			else if (c == '%')
				State = CheckForLastPercent(inf, outf);
		}
		else if (c == '%')
			State = CheckForLastPercent(inf, outf);
		break;

	case SawPercent:
		if (c == '%') {
			State = Copying;
			break;
		}
		bx = buf;
		while (isalpha(c = getc(inf)))
			*bx++ = c;
		*bx = '\0';
		ungetc(c, inf);
		if (strcmp("oken", buf) == 0) {
			fprintf(outf, "oken");
			State = InTokens;
		}
		else if (strcmp("hong", buf) == 0) {
			struct thong *t = (struct thong
					*)malloc(sizeof(struct thong));
			char *cx;
			fprintf(outf, "oken");

			/* parse name of token (& output it) */
			while (isspace(getput(inf, outf))) {}
			cx = t->name;
			do
				*cx++ = c;
			while (isalpha(getput(inf, outf)));
			*cx = '\0';

			/* parse characters of token (& do not output) */
			while (isspace(c=getc(inf))) {}
			ungetc(c, inf);
			cx = t->chars;
			while ( ! isspace(c=getc(inf)))
				*cx++ = c;
			*cx = '\0';
			ungetc(c, inf);

			t->next = thonglist;
			thonglist = t;
			State = Start;
		}
		else {
			/* unknown, dumpit and continue */
			fprintf(outf, buf);
			State = Start;
		}
		break;

	case InType:
		if (c == '/') {
			if (getput(inf,outf) == '*')
				SkipComment(inf, outf);
			else if (c == '%')
				State = CheckForLastPercent(inf, outf);
		}
		else if (c == '%')
			State = CheckForLastPercent(inf, outf);
		else if (c == '>')
			State = InTokens;
		else *tbx++ = c;  /* save type name */
		break;
	case InTokens:
		if (c == '/') {
			if (getput(inf,outf) == '*')
				SkipComment(inf, outf);
			else if (c == '%')
				State = CheckForLastPercent(inf, outf);
		}
		else if (c == '%')
			State = CheckForLastPercent(inf, outf);
		else if (c == '<') {
			State = InType;
			tbx = typebuf;    /* start saving type name */
		}
		else if (isascii(c) && isalpha(c)) {
			do {
				*tx++ = c;
				c = getput(inf,outf);
			} while (isascii(c) && isalpha(c));
			*tx++ = ' ';
		}
		break;
	case Copying:
		break;

	}    /* end of while()switch() */

	*tx = '\0';

	PutLexDesc(outf);

	exit(0);

}
