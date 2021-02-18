/*
 *	$Id: cit2tit.c,v 1.4 1991/10/02 04:14:41 ygz Exp $
 */

/***********************************************************************
* Copyright 1990, 1991 by Purdue University, and Yongguang Zhang.
* All rights reserved.  Some individual files may be covered by other
* copyrights.
*
* Redistribution and use in source and binary forms are permitted
* provided that this entire copyright notice is duplicated in all such
* copies.  No charge, other than an "at-cost" distribution fee, may be
* charged for copies, derivations, or distributions of this material
* without the express written consent of the copyright holders.  Neither
* the name of the University, nor the name of the authors may be used to
* endorse or promote products derived from this material without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
* MERCHANTIBILITY AND FITNESS FOR ANY PARTICULAR PURPOSE.
************************************************************************/

/* cit2tit.c */

#include <X11/Xos.h>	/* OS dependent stuff */
#include <ctype.h>
#include <stdio.h>
extern char *malloc();
extern char *calloc();
extern char *realloc();

typedef	struct {	/* define to avoid "#include <X11/Xlib.h>" */ 
    unsigned char byte1;
    unsigned char byte2;
} XChar2b;

#include "HZinput.h"

trieNode *trieList;		/* all Tries are saved here */
XChar2b *hzList;		/* all HZ codes will be put here */

HZinputTable hzInputTable;

char *progname, *ifname;

#define MAXCLEN 10240		/* 10K comment, large enough? */
static char comment[MAXCLEN];
static unsigned int clen;

main(argc, argv)
    int argc;
    char *argv[];
{
    FILE *ifile, *ofile;

    progname = argv[0];
    if (argc == 1) {
	ifname = "(stdin)";
	ifile = stdin;
	ofile = stdout;
    } else {
	ifname = argv[1];
	ifile = fopen (argv[1], "r");
	if (! ifile) {
	    perror (argv[1]);
	    exit (1);
	}
	ofile = stdout;
    }

    ReadInput (ifile);
    if (ifile != stdin)
	fclose (ifile);

    Output (ofile);
}

/*
 * ReadInput -- read .cit format input file
 */
ReadInput (ifile)
    FILE *ifile;
{
    char magic[2];

    (void) fread (magic, 2, 1, ifile);		/* read the magic number */
    if (strncmp (magic, MAGIC_CIT, 2) != 0) {
	fprintf (stderr, "Input is not in .cit format\n");
	exit (1);
    }
    if (fread (&hzInputTable, sizeof(HZinputTable), 1, ifile) == 0) {
	fprintf (stderr, "Incorrect .cit input\n");
	exit (1);
    }
    trieList = (trieNode *) calloc (hzInputTable.sizeTrieList,
				    sizeof(trieNode));
    hzList = (XChar2b *) calloc (hzInputTable.sizeHZList, sizeof(XChar2b));
    if ((! trieList) || (! hzList)) {
	perror (progname);
	exit (1);
    }
    if ((fread (trieList, sizeof(trieNode), hzInputTable.sizeTrieList, ifile)
	    != hzInputTable.sizeTrieList) ||
	(fread (hzList, sizeof(XChar2b), hzInputTable.sizeHZList, ifile)
	    != hzInputTable.sizeHZList))
    {
	fprintf (stderr, "Incorrect .cit input\n");
	exit (1);
    }
    if ((fread ((char *)(&clen), sizeof (unsigned int), 1, ifile) != 1) ||
	(fread (comment, 1, clen, ifile) != clen))
    {
	clen = 0;
    }
    if (clen > MAXCLEN)
	clen = MAXCLEN;
}

#define	MAXLW	50
static unsigned char inputs[128], *ip = inputs;
static unsigned char selects[MAX_CHOICE][128], *sp[MAX_CHOICE];
static unsigned char backspaces[128], *bp = backspaces;
static unsigned char kills[128], *kp = kills;
static unsigned char moveRs[128], *mrp = moveRs;
static unsigned char moveLs[128], *mlp = moveLs;
static unsigned char repeats[128], *rpp = repeats;

unsigned char keystack[32];

Output (ofile)
    FILE *ofile;
{
    int cnt, i;
    register unsigned char *ptr;
    int pcomment;

    fprintf (ofile, "# HANZI input table for cxterm\n");
    fprintf (ofile, "# Generated from %s by %s\n", ifname, progname);
    fprintf (ofile, "# To be used by cxterm, convert me to %s format first\n",
		CIT_SUFFIX);
    fprintf (ofile, "# %s version %d\n", CIT_SUFFIX, hzInputTable.version);

    fprintf (ofile, "ENCODE:\t%s\n",
		hzInputTable.encode == GB_ENCODE ? "GB" : "BIG5");
    fprintf (ofile, "MULTICHOICE:\t%s\n",
		hzInputTable.multiChoice ? "YES" : "NO");
    fprintf (ofile, "PROMPT:\t");
    for (i = 0; i < hzInputTable.lenPrompt; i++) {
	(void) Putc (hzInputTable.prompt[i], ofile);
    }
    fprintf (ofile, "\n");

    fprintf (ofile, "#\n");
    pcomment = 1;
    for (i = 0; i < clen; i++) {
	if (pcomment)
	    fprintf (ofile, "COMMENT");
	(void) putc (comment[i], ofile);
	pcomment = (comment[i] == '\n');
    }

    for (i = 0; i < hzInputTable.maxchoice; i++)
	sp[i] = selects[i];

    for (i = 0; i < 128; i++) {
	if (hzInputTable.keytype[i] & HZ_KEY_INPUT_MASK)
	    *ip++ = i;
	if (hzInputTable.keytype[i] & HZ_KEY_SELECTION_MASK)
	    *sp[ hzInputTable.keytype[i] & HZ_KEY_SELECTION_NUM ]++ = i;
	if (hzInputTable.keytype[i] == HZ_KEY_BACKSPACE)
	    *bp++ = i;
	if (hzInputTable.keytype[i] == HZ_KEY_KILL)
	    *kp++ = i;
	if (hzInputTable.keytype[i] == HZ_KEY_RIGHT)
	    *mrp++ = i;
	if (hzInputTable.keytype[i] == HZ_KEY_LEFT)
	    *mlp++ = i;
	if (hzInputTable.keytype[i] == HZ_KEY_REPEAT)
	    *rpp++ = i;
    }

    fprintf (ofile, "# define keys");
    for (cnt = 0, ptr = inputs; ptr < ip; ptr++) {
	if (cnt == 0 || cnt >= MAXLW) {
	    fprintf (ofile, "\nVALIDINPUTKEY:\t");
	    cnt = 0;
	}
	cnt += Putc (*ptr, ofile);
    }

    for (i = 0; i < hzInputTable.maxchoice; i++) {
	fprintf (ofile, "\nSELECTKEY:\t");
	(void) Putc (hzInputTable.choicelb[i], ofile);
	for (ptr = selects[i]; ptr < sp[i]; ptr++) {
	    if (*ptr != (unsigned char)(hzInputTable.choicelb[i]))
		(void) Putc (*ptr, ofile);
	}
    }

    for (cnt = 0, ptr = backspaces; ptr < bp; ptr++, cnt) {
	if (cnt == 0 || cnt >= MAXLW) {
	    fprintf (ofile, "\nBACKSPACE:\t");
	    cnt = 0;
	}
	cnt += Putc (*ptr, ofile);
    }

    for (cnt = 0, ptr = kills; ptr < kp; ptr++, cnt) {
	if (cnt == 0 || cnt >= MAXLW) {
	    fprintf (ofile, "\nDELETEALL:\t");
	    cnt = 0;
	}
	cnt += Putc (*ptr, ofile);
    }

    for (cnt = 0, ptr = moveRs; ptr < mrp; ptr++, cnt) {
	if (cnt == 0 || cnt >= MAXLW) {
	    fprintf (ofile, "\nMOVERIGHT:\t");
	    cnt = 0;
	}
	cnt += Putc (*ptr, ofile);
    }

    for (cnt = 0, ptr = moveLs; ptr < mlp; ptr++, cnt) {
	if (cnt == 0 || cnt >= MAXLW) {
	    fprintf (ofile, "\nMOVELEFT:\t");
	    cnt = 0;
	}
	cnt += Putc (*ptr, ofile);
    }

    for (cnt = 0, ptr = repeats; ptr < rpp; ptr++, cnt) {
	if (cnt == 0 || cnt >= MAXLW) {
	    fprintf (ofile, "\nREPEATKEY:\t");
	    cnt = 0;
	}
	cnt += Putc (*ptr, ofile);
    }

    for (ptr = inputs; ptr < ip; ptr++) {
	if ((hzInputTable.keyprompt[*ptr].ptlen == 1) &&
	    (hzInputTable.keyprompt[*ptr].prompt[0] == *ptr)) {
		continue;
	}
	fprintf (ofile, "\nKEYPROMPT(");
	(void) Putc (*ptr, ofile);
	fprintf (ofile, "):\t");
	for (i = 0; i < hzInputTable.keyprompt[*ptr].ptlen; i++) {
	    (void) Putc (hzInputTable.keyprompt[*ptr].prompt[i], ofile);
	}
    }

    fprintf (ofile, "\n");

    fprintf (ofile, "# the following line must not be removed\n");
    fprintf (ofile, "BEGINDICTIONARY\n");
    fprintf (ofile, "#\n");

    LineOut (0, 0, ofile);
}

/* output line by line while traveling the Trie */
LineOut (top, trieIdx, ofile)
    int top;
    unsigned int trieIdx;
    FILE *ofile;
{
    register trieNode *tnptr = &trieList[trieIdx];
    register int i;
    int num;		/* number of HZs under the key sequence in stack */

    keystack[top] = tnptr->tn_key;

    if (tnptr->tn_numNextKeys)		/* there can be more input keys */
	num = trieList[tnptr->tn_nextKeys].tn_hzidx - tnptr->tn_hzidx;
    else
	num = tnptr->tn_numHZchoice;

    if ((num > 0) || (tnptr->tn_numNextKeys == 0)) {
	/* there are HZs exclusively under the keys, or no more input keys */

	int cnt = 0;

	for (i = 1; i <= top; i++) {
	    (void) Putc (keystack[i], ofile);
	}
	(void) putc ('\t', ofile);
	for (i = 0; i < num; i++) {
	    if (cnt >= MAXLW) {
		register int j;

		(void) putc ('\n', ofile);
		for (j = 1; j <= top; j++) {
		    (void) Putc (keystack[j], ofile);
		}
		(void) putc ('\t', ofile);
		cnt = 0;
	    }
	    (void) putc (hzList[tnptr->tn_hzidx + i].byte1, ofile);
	    (void) putc (hzList[tnptr->tn_hzidx + i].byte2, ofile);
	    cnt += 2;
	}
	(void) putc ('\n', ofile);
    }

    for (i = 0; i < tnptr->tn_numNextKeys; i++) {
	LineOut (top + 1, tnptr->tn_nextKeys + i, ofile);
    }
}

Putc (ch, ofile)
    unsigned char ch;
    FILE *ofile;
{
    if ((ch & 0x80) || (isprint (ch) && (! isspace (ch)))) {
	/* special cases */
	switch (ch) {
	    case '#'  :
	    case '\\' :
		fprintf (ofile, "\\%03o", ch);
		break;
	    default   :
		(void) putc (ch, ofile);
	}
	return (1);
    } else {
	fprintf (ofile, "\\%03o", ch);
	return (4);
    }
}
