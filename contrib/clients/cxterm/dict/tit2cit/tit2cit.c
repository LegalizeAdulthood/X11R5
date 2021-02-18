/*
 *	$Id: tit2cit.c,v 1.4 1991/10/02 04:14:41 ygz Exp $
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

/*
 * Dynamic structure dynTrie is different with the TRIE structures
 * defined in hzinput.h, which is static.
 *
 * When we read the file line by line, keys-HZs mapping are inserted
 * into the dynTrie. After all lines are finished, the dynTrie is
 * reorganized (optimized and linearized) into a static TRIE structures,
 * which can be loaded easily by cxterm.
 */

typedef struct _dynTrie {
    unsigned char key;		/* the input key */
    char *hzptr;		/* HZ strings */
    struct _dynTrie *next;	/* next node in the same level */
    struct _dynTrie *son;	/* next node down one level */
} dynTrie;

static dynTrie rootTrie = {
    '\0', NULL, (struct _dynTrie *)NULL, (struct _dynTrie *)NULL
};
static dynTrie *root = &rootTrie;	/* root pointer to the dynTrie */

trieNode *trieList;		/* all Tries will be linearized to put here */
XChar2b *hzList;		/* all HZ codes will be put here */
unsigned int pTrieList;		/* pointer to Trie List */
unsigned int pHZList;		/* pointer to HZ List */

HZinputTable hzInputTable;

static char buffer[256];
static char keysbuf[32];
static char strbuf[256];
static int begindictionary = 0;
static int selection = 0;
static int lineno = 0;
static int totalHZbytes = 0;	/* total bytes of HZs */
static int totalTrieNode = 1;	/* total # of Trie nodes (including root) */

static int sl = 0;		/* explicitly define selection keys? */
static int bs = 0;		/* explicitly define backspace keys? */
static int kl = 0;		/* explicitly define kill keys? */
static int mr = 0;		/* explicitly define moveR keys? */
static int ml = 0;		/* explicitly define moveL keys? */
static int rp = 0;		/* explicitly define repeat keys? */

#define MAXCLEN	10240		/* 10K comment, large enough? */
static char comment[MAXCLEN];
static unsigned int clen = 0;

main(argc, argv)
    int argc;
    char *argv[];
{
    FILE *ifile, *ofile;

    if (argc == 1) {
	ifile = stdin;
	ofile = stdout;
    } else {
	ifile = fopen (argv[1], "r");
	if (! ifile) {
	    perror (argv[1]);
	    exit (1);
	}
	ofile = stdout;
    }

    InitTable ();

    ReadInput (ifile);
    if (ifile != stdin)
	fclose (ifile);

    Predefine ();

    BuildTrie ();

    Output (ofile);
}

Error (str)
    char *str;
{
    fprintf (stderr, "%s (at line %d)\n", str, lineno);
    exit (1);
}

/*
 * ReadInput -- read and parse input .tit format file
 */
ReadInput (ifile)
    FILE *ifile;
{
    while (fgets (buffer, 255, ifile) != NULL) {

	lineno++;

	if (! begindictionary) {
	    char key[256];
	    register char *kptr;

	    if ((sscanf (buffer, "%s", key) != 1) || (key[0] == '#'))
		continue;	/* empty line or comment line */

	    if (strcmp (key, "BEGINDICTIONARY") == 0) {

		/* BEGINDICTIONARY */
		begindictionary = 1;

	    } else if (strcmp (key, "VALIDINPUTKEY:") == 0) {

		/* VALIDINPUTKEY:  <keys> */
		getprompt (buffer, strbuf);
		ParseKey (strbuf);
		for (kptr = strbuf; *kptr; )
		    hzInputTable.keytype[*kptr++] |= HZ_KEY_INPUT_MASK;

	    } else if (strcmp (key, "BACKSPACE:") == 0) {

		/* BACKSPACE:  <keys> */
		getprompt (buffer, strbuf);
		ParseKey (strbuf);
		for (kptr = strbuf; *kptr; )
		    hzInputTable.keytype[*kptr++] = HZ_KEY_BACKSPACE;
		bs = 1;

	    } else if (strcmp (key, "DELETEALL:") == 0) {

		/* DELETEALL:  <keys> */
		getprompt (buffer, strbuf);
		ParseKey (strbuf);
		for (kptr = strbuf; *kptr; )
		    hzInputTable.keytype[*kptr++] = HZ_KEY_KILL;
		kl = 1;

	    } else if (strcmp (key, "MOVERIGHT:") == 0) {

		/* MOVERIGHT:  <keys> */
		getprompt (buffer, strbuf);
		ParseKey (strbuf);
		for (kptr = strbuf; *kptr; )
		    hzInputTable.keytype[*kptr++] = HZ_KEY_RIGHT;
		mr = 1;

	    } else if (strcmp (key, "MOVELEFT:") == 0) {

		/* MOVELEFT:  <keys> */
		getprompt (buffer, strbuf);
		ParseKey (strbuf);
		for (kptr = strbuf; *kptr; )
		    hzInputTable.keytype[*kptr++] = HZ_KEY_LEFT;
		ml = 1;

	    } else if (strcmp (key, "REPEATKEY:") == 0) {

		/* REPEATKEY:  <keys> */
		getprompt (buffer, strbuf);
		ParseKey (strbuf);
		for (kptr = strbuf; *kptr; )
		    hzInputTable.keytype[*kptr++] = HZ_KEY_REPEAT;
		rp = 1;

	    } else if (strcmp (key, "SELECTKEY:") == 0) {

		/* SELECTKEY:  <keys> <keys> ... */
		register char *ptr = buffer;

		sl = 1;
		while (isspace (*ptr++))  /* skip spaces */;
		while ((*ptr) && (! isspace (*ptr++)))  ;
		/* skip "SELECTKEY:" */
		while (sscanf (ptr, "%s", strbuf) == 1) {
		    if (strbuf[0] == '#')
			break;		/* ignore comment */

		    /* one selection */
		    if (selection > MAX_CHOICE)
			Error ("Too many selection choices");

		    ParseKey (strbuf);
		    for (kptr = strbuf; *kptr; ) {
			hzInputTable.keytype[*kptr++] |= 
				( (selection & HZ_KEY_SELECTION_NUM) |
				  HZ_KEY_SELECTION_MASK );
		    }

		    hzInputTable.choicelb[selection] = strbuf[0];
		    selection ++;

		    /* move ptr to the next token */
		    while (isspace (*ptr++))  ;	/* skip spaces */
		    while ((*ptr) && (! isspace (*ptr++)))  ;
		}

	    } else if (strncmp (key, "KEYPROMPT", strlen("KEYPROMPT")) == 0) {
			/* Beware of these two identical strings! */

		/* KEYPROMPT(<key>): <str> */
		unsigned char ch;

		if (sscanf (key, "KEYPROMPT(%[^)]):", keysbuf) != 1) {
		    Error ("Bad input format");
		}
		ParseKey (keysbuf);
		ch = keysbuf[0];

		if (getprompt (buffer, strbuf) != 1) {
		    Error ("Bad prompt format");
		}
		ParseKey (strbuf);
		if (strlen (strbuf) > MAX_KEYPROMPT) {
		    char *errstr = "KEYPROMPT too long (maximum %d bytes)";

		    sprintf (errstr, "KEYPROMPT too long (maximum %d bytes)",
			     MAX_KEYPROMPT);
		    Error (errstr);
		}
		hzInputTable.keyprompt[ch].ptlen = strlen (strbuf);
		strncpy (hzInputTable.keyprompt[ch].prompt, strbuf,
			 hzInputTable.keyprompt[ch].ptlen);

	    } else if (strcmp (key, "PROMPT:") == 0) {

		/* PROMPT:  <str> */
		getprompt (buffer, strbuf);
		ParseKey (strbuf);
		if (strlen (strbuf) > MAX_PROMPT) {
		    char *errstr = "PROMPT too long (maximum %d bytes)";

		    sprintf (errstr, "PROMPT too long (maximum %d bytes)",
			     MAX_PROMPT);
		    Error (errstr);
		}
		hzInputTable.lenPrompt = strlen (strbuf);
		strncpy (hzInputTable.prompt, strbuf, hzInputTable.lenPrompt);

	    } else if (strcmp (key, "ENCODE:") == 0) {

		/* ENCODE: [GB|BIG5] */
		if (getprompt (buffer, strbuf) != 1) {
		    Error ("Bad prompt format");
		}
		if ((strcmp (strbuf, "GB") == 0) ||
		    (strcmp (strbuf, "gb") == 0))
		{
		    hzInputTable.encode = GB_ENCODE;
		} else if ((strcmp (strbuf, "BIG5") == 0) ||
			   (strcmp (strbuf, "big5") == 0))
		{
		    hzInputTable.encode = BIG5_ENCODE;
		} else
		    Error ("Unknown ENCODE");

	    } else if (strcmp (key, "MULTICHOICE:") == 0) {

		/* MULTICHOICE: [YES|NO] */
		if (getprompt (buffer, strbuf) != 1) {
		    Error ("Bad prompt format");
		}
		if ((strcmp (strbuf, "YES") == 0) ||
		    (strcmp (strbuf, "yes") == 0) ||
		    (strcmp (strbuf, "TRUE") == 0) ||
		    (strcmp (strbuf, "true") == 0))
		{
		    hzInputTable.multiChoice = 1;
		} else if ((strcmp (strbuf, "NO") == 0) ||
			   (strcmp (strbuf, "no") == 0) ||
			   (strcmp (strbuf, "FALSE") == 0) ||
			   (strcmp (strbuf, "false") == 0))
		{
		    hzInputTable.multiChoice = 0;
		} else
		    Error ("Unknown MULTICHOICE");

	    } else if (strcmp (key, "COMMENT") == 0) {

		/* COMMENT  <any> */
		register char *ptr = buffer;

		while (isspace (*ptr))  ptr++;	/* skip leading spaces */
		while ((*ptr) && (! isspace (*ptr)))
		    ptr++;			/* skip word COMMENT */

		/* unlike '#', COMMENT body must be saved */
		while ((*ptr) && (clen < MAXCLEN))
		    comment[clen++] = *ptr++ ;

	    } else	/* unknown key */
		Error ("Unknown input");

	} else {

	    /* input here:	"<input keys>	<HANZIs> */

	    register char *ptr = buffer, *sptr;
	    register char *kptr, *hzptr;

	    while (isspace (*ptr))  ptr++;	/* skip leading spaces */
	    if (*ptr == '#' || *ptr == '\0')
		continue;	/* comment or empty line */

	    /* here is the input field */
	    kptr = ptr;
	    while ((*ptr) && (! isspace (*ptr)))
		ptr++;
	    if (*ptr == '\0')
	        Error ("Wrong input format");
	    *ptr++ = '\0';
	    ParseKey (kptr);
	    for (sptr = kptr; *sptr; sptr++) {
		if (*sptr & 0x80)
		    Error ("Unacceptable key for Chinese input");
		if (! (hzInputTable.keytype[ *sptr ] & HZ_KEY_INPUT_MASK))
		    Error ("Input key is not specified to be valid");
	    }

	    while (isspace(*ptr) && (!(*ptr & 0x80)))
		ptr++;		/* skip leading spaces */

	    /* here is the HANZI field */
	    hzptr = ptr;
	    while (*ptr++ & 0x80) {
		if (*ptr++ == '\0')	/* only half HZ */
		    Error ("Unacceptable HZ code");
		/* 2nd byte not check against 0x80 -- make a room for BIG5 */
	    }
	    *(--ptr) = '\0';

	    InsertTrie (kptr, hzptr);
	}
    }
}

/*
 * BuildTrie -- reorganize the dynTrie into static TRIE structure
 */
BuildTrie ()
{
    trieList = (trieNode *) calloc (totalTrieNode, sizeof(trieNode));
    hzList = (XChar2b *) malloc (totalHZbytes);
    if ((! hzList) || (! trieList)) {
	perror ("BuildTrie");
	exit (1);
    }

    pTrieList = 1;	/* 0 is the root, start from index 1 */
    pHZList = 0;

    Linearize (0, root);
    trieList[0].tn_hzidx = 0;
    trieList[0].tn_numHZchoice = 0;

    /* ASSERT: pTrieList = totalTrieNode, pHZList = totalHZbytes/2 */
    hzInputTable.sizeTrieList = pTrieList;
    hzInputTable.sizeHZList = pHZList;
}

Linearize (idxTrie, trie)
    unsigned int idxTrie;
    register dynTrie *trie;
{
    register dynTrie *tptr = trie->son;
    register unsigned int i;
    dynTrie *tptr2;
    unsigned int bTrieList = pTrieList;
    unsigned int bHZList = pHZList;

    if (tptr->key == '\0') {
	char *hzptr = tptr->hzptr;

	while (*hzptr) {
	    hzList[pHZList  ].byte1 = *hzptr++;
	    hzList[pHZList++].byte2 = *hzptr++;
	}
	tptr = tptr->next;
    }

    tptr2 = tptr;	/* save for second pass */
    while (tptr) {
	trieList[ pTrieList++ ].tn_key = tptr->key;
	tptr = tptr->next;
    }
    trieList[ idxTrie ].tn_nextKeys = bTrieList;
    trieList[ idxTrie ].tn_numNextKeys = pTrieList - bTrieList;

    for (tptr = tptr2, i = bTrieList; tptr; i++, tptr = tptr->next) {
	Linearize (i, tptr); 
    }
    trieList[ idxTrie ].tn_hzidx = bHZList;
    trieList[ idxTrie ].tn_numHZchoice = pHZList - bHZList;
}

/* 
 * InsertTrie -- insert the keys-HZs pair into dynTrie structure
 */
InsertTrie (kptr, hzptr)
    register char *kptr;
    char *hzptr;
{
    register dynTrie *tptr = root;
    dynTrie *NewTrieNode ();

    while (*kptr) {
	unsigned char key = *kptr++;

	if (tptr->son == NULL) {
	    tptr->son = NewTrieNode (key, (dynTrie *)NULL, (dynTrie *)NULL);
	    tptr = tptr->son;
	} else if (tptr->son->key > key) {
	    /* new key should be the 1st son, the old one becomes the next */
	    tptr->son = NewTrieNode (key, (dynTrie *)NULL, tptr->son);
	    tptr = tptr->son;
	} else if (tptr->son->key == key) {
	    tptr = tptr->son;
	} else {
	    /* ASSERT: (tptr->son->key < key) */
	    tptr = tptr->son;
	    while ((tptr->next != NULL) && (tptr->next->key < key)) {
	        tptr = tptr->next;	/* try next */
	    }
	    /* ASSERT: (tptr->next == NULL) || (tptr->next->key >= key) */
	    if ((tptr->next == NULL) || (tptr->next->key > key)) {
		/* add it here (to keep it sorted) */
		tptr->next = NewTrieNode (key, (dynTrie *)NULL, tptr->next);
		tptr = tptr->next;
	    } else {	/* tptr->next->key == key */
		tptr = tptr->next;
	    }
	}
    }

    /* come to the end of the key string kptr */

    if (tptr->son == NULL) {
	tptr->son = NewTrieNode ('\0', (dynTrie *)NULL, (dynTrie *)NULL);
	tptr->son->hzptr = malloc (strlen (hzptr) + 1);
	if (tptr->son->hzptr == NULL)
	    Error ("Run out of memory");
	strcpy (tptr->son->hzptr, hzptr);
    } else if (tptr->son->key != '\0') {
	/* new key should be the 1st son, the old one becomes the next */
	tptr->son = NewTrieNode ('\0', (dynTrie *)NULL, tptr->son);
	tptr->son->hzptr = malloc (strlen (hzptr) + 1);
	if (tptr->son->hzptr == NULL)
	    Error ("Run out of memory");
	strcpy (tptr->son->hzptr, hzptr);
    } else {
	tptr->son->hzptr = realloc (tptr->son->hzptr,
		strlen (tptr->son->hzptr) + strlen (hzptr) + 1);
	if (tptr->son->hzptr == NULL)
	    Error ("Run out of memory");
	strcat (tptr->son->hzptr, hzptr);
    }
    totalHZbytes += strlen (hzptr);
}

dynTrie *NewTrieNode (key, son, next)
    unsigned char key;
    struct _dynTrie *son;
    struct _dynTrie *next;
{
    register dynTrie *t = (dynTrie *) malloc (sizeof(dynTrie));

    if (t == NULL)
	Error ("Run out of memory");
    t->key = key;
    t->son = son;
    t->next = next;
    if (key)
	totalTrieNode++ ;
    return (t);
}

Output (ofile)
    FILE *ofile;
{
    (void) fwrite (MAGIC_CIT, 2, 1, ofile);	/* write the magic number */
    if ((fwrite (&hzInputTable, sizeof(HZinputTable), 1, ofile) == 0) ||
	(fwrite (trieList, sizeof(trieNode), hzInputTable.sizeTrieList, ofile)
	    != hzInputTable.sizeTrieList) ||
	(fwrite (hzList, sizeof(XChar2b), hzInputTable.sizeHZList, ofile)
	    != hzInputTable.sizeHZList))
    {
	perror ("Writing output file");
	exit (1);
    }
    if ((fwrite ((char *)(&clen), sizeof(unsigned int), 1, ofile) == 0) ||
	(fwrite (comment, 1, clen, ofile) != clen))
    {
	perror ("Writing output file");
	exit (1);
    }
}

ParseKey (buf)
    char buf[];
{
    register char *p1 = buf, *p2 = buf;

    while (*p1) {
	if (*p1 == '\\') {
	    p1++;
	    if ((p1[0] == '0') || (p1[0] == '1')) {
		if ((p1[1] >= '0') && (p1[1] <= '7') &&
		    (p1[2] >= '0') && (p1[2] <= '7')) {
			*p2 = (*p1++ - '0') << 6;
			*p2 += (*p1++ - '0') << 3;
			*(p2++) += (*p1++ - '0');
			continue;
		}
	    } else if (isdigit (*p1))
		Error ("Error in parsing input keys");
	}
	*p2++ = *p1++;
    }
    *p2 = '\0';
}

#define	DEF_PROMPT	"\272\272\327\326\312\344\310\353\241\313\241\241"
			/* han zi shu ru (Chinese Input) :: SPACE */

InitTable ()
{
    register int i;

    hzInputTable.version = CIT_VERSION;
    hzInputTable.encode = GB_ENCODE;	/* dafault: GB coding */
    hzInputTable.builtin = 0;
    hzInputTable.multiChoice = 1;
    for (i = 0; i < 128; i++) {
	hzInputTable.keytype[i] = HZ_KEY_INVALID;
	hzInputTable.keyprompt[i].prompt[0] = i;
	hzInputTable.keyprompt[i].prompt[1] = '\0';
	hzInputTable.keyprompt[i].ptlen = 1;
    }
    strcpy (hzInputTable.prompt, DEF_PROMPT);
    hzInputTable.lenPrompt = strlen(DEF_PROMPT);
}

/* predefined value */
Predefine ()
{
    if (! bs) {
	if (hzInputTable.keytype['\010'] == HZ_KEY_INVALID)
	    hzInputTable.keytype['\010'] = HZ_KEY_BACKSPACE;	/* \b */
	if (hzInputTable.keytype['\177'] == HZ_KEY_INVALID)
	    hzInputTable.keytype['\177'] = HZ_KEY_BACKSPACE;	/* DEL */
    }
    if (! kl) {
	if (hzInputTable.keytype['\015'] == HZ_KEY_INVALID)
	    hzInputTable.keytype['\015'] = HZ_KEY_KILL;		/* \r */
	if (hzInputTable.keytype['\025'] == HZ_KEY_INVALID)
	    hzInputTable.keytype['\025'] = HZ_KEY_KILL;		/* ^U */
    }
    if (! mr) {
	if (hzInputTable.keytype['.'] == HZ_KEY_INVALID)
	    hzInputTable.keytype['.'] = HZ_KEY_RIGHT;
	if (hzInputTable.keytype['>'] == HZ_KEY_INVALID)
	    hzInputTable.keytype['>'] = HZ_KEY_RIGHT;
    }
    if (! ml) {
	if (hzInputTable.keytype[','] == HZ_KEY_INVALID)
	    hzInputTable.keytype[','] = HZ_KEY_LEFT;
	if (hzInputTable.keytype['>'] == HZ_KEY_INVALID)
	    hzInputTable.keytype['>'] = HZ_KEY_LEFT;
    }
    if (! rp) {
	if (hzInputTable.keytype['\020'] == HZ_KEY_INVALID)
	    hzInputTable.keytype['\020'] = HZ_KEY_REPEAT;	/* ^P */
	if (hzInputTable.keytype['\022'] == HZ_KEY_INVALID)
	    hzInputTable.keytype['\022'] = HZ_KEY_REPEAT;	/* ^R */
    }
    if (! sl) {
	if ((hzInputTable.keytype['0'] == HZ_KEY_INVALID) &&
	    (hzInputTable.keytype['1'] == HZ_KEY_INVALID) &&
	    (hzInputTable.keytype['2'] == HZ_KEY_INVALID) &&
	    (hzInputTable.keytype['3'] == HZ_KEY_INVALID) &&
	    (hzInputTable.keytype['4'] == HZ_KEY_INVALID) &&
	    (hzInputTable.keytype['5'] == HZ_KEY_INVALID) &&
	    (hzInputTable.keytype['6'] == HZ_KEY_INVALID) &&
	    (hzInputTable.keytype['7'] == HZ_KEY_INVALID) &&
	    (hzInputTable.keytype['8'] == HZ_KEY_INVALID) &&
	    (hzInputTable.keytype['9'] == HZ_KEY_INVALID))
	{
	    hzInputTable.keytype['0'] = 0 | HZ_KEY_SELECTION_MASK;
	    if (hzInputTable.keytype[' '] == HZ_KEY_INVALID)
		hzInputTable.keytype[' '] = 0 | HZ_KEY_SELECTION_MASK;
	    hzInputTable.keytype['1'] = 1 | HZ_KEY_SELECTION_MASK;
	    hzInputTable.keytype['2'] = 2 | HZ_KEY_SELECTION_MASK;
	    hzInputTable.keytype['3'] = 3 | HZ_KEY_SELECTION_MASK;
	    hzInputTable.keytype['4'] = 4 | HZ_KEY_SELECTION_MASK;
	    hzInputTable.keytype['5'] = 5 | HZ_KEY_SELECTION_MASK;
	    hzInputTable.keytype['6'] = 6 | HZ_KEY_SELECTION_MASK;
	    hzInputTable.keytype['7'] = 7 | HZ_KEY_SELECTION_MASK;
	    hzInputTable.keytype['8'] = 8 | HZ_KEY_SELECTION_MASK;
	    hzInputTable.keytype['9'] = 9 | HZ_KEY_SELECTION_MASK;
	    strcpy (hzInputTable.choicelb, "0123456789");
	    hzInputTable.maxchoice = 10;
	}
    } else
	hzInputTable.maxchoice = selection;
}

getprompt (buffer, strbuf)
     char *buffer, *strbuf;
{
#define	is7space(c)	(isspace(c) && (!((c) & 0x80)))

    while (isspace (*buffer))  buffer++;
    while (*buffer && (! isspace(*buffer)))  buffer++;	/* skip 1st field */
    while (is7space (*buffer))  buffer++;
    if (! *buffer) {
	*strbuf = '\0';
	return (0);
    }
    while (*buffer && (! is7space(*buffer)))
	*strbuf++ = *buffer++;
    *strbuf = '\0';
    return(1);
}
