/*
 *	$Id: HZinMthd.c,v 1.3 1991/10/02 20:42:13 ygz Exp $
 */

/***********************************************************
Copyright 1991 by Yongguang Zhang.  All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of the authors not
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifdef HANZI

#include "ptyx.h"		/* X headers included here. */
#include <X11/Xos.h>
#include <stdio.h>
#include <setjmp.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/param.h>

#include "data.h"
#include "error.h"

#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#else
extern char *calloc();
#endif
#if defined(macII) && !defined(__STDC__)  /* stdlib.h fails to define these */
char *calloc();
#endif /* macII */

#include "HZinput.h"

/*** Only these variables and functions are to be exported ***/

extern HZinputTable *cHZtbl;
extern int (*HZfilterInput)();
extern Char dpyInbuf[];
extern int dpyInbufLen;
extern Char dpyChoices[];
extern int dpyChoicesLen;
extern SetHZMode();
extern HZInitInputTable();
extern HZClearInputTable();
extern RemakeHZInput();
extern void HandleSwitchHZMode();
extern void switchHZmodeByNum();
extern int HZParseInput();
extern int HZencode();

/*** local variables ***/

static HZinputTable hzInputTables[MAX_HZTBL];	/* HZ input tables */
static int (*hzInputFilters[MAX_HZTBL])();	/* input filter functions */
HZinputTable *cHZtbl;				/* current HZ input table */
int (*HZfilterInput)();				/* current filter function */

int hzTableFilter();
int hzASCIIfilter();
int hzICfilter();
int hzQWfilter();

struct pdHZinput {
    char *name;			/* name of input method */
    char *prompt;		/* prompt of the input method */
    unsigned char multiChoice;	/* Need multiple choices? */
    int (*filter)();		/* HZ filter routine */
};

/* GB predefined */
static struct pdHZinput GBpredefine[] = {
  {
/* 0: The first one must be ASCII -- ISO 8859-1 */
	"ASCII",
	"\323\242\316\304\312\344\310\353 (ASCII input)",
	    /* yin wen shu ru (English Input) */
        FALSE,
	hzASCIIfilter,
  },
  {
/* 1: IC -- Internal Code, encoding independent */
	"IC",
	"\272\272\327\326\312\344\310\353\241\313\304\332\302\353\241\313",
	    /* han zi shu ru (Chinese Input) :: nei ma (internal code) :: */
	FALSE,
	hzICfilter,
  },
  {
/* 2: QW -- Position, for GB coding */
	"QW",
	"\272\272\327\326\312\344\310\353\241\313\307\370\316\273\241\313",
	    /* han zi shu ru (Chinese Input) :: qu wei (position) :: */
	FALSE,
	hzQWfilter,
  },
  {
/* last: null-pad */
	NULL, NULL, FALSE, hzTableFilter,
  },
};

/* BIG5 predefined */
static struct pdHZinput BIG5predefine[] = {
  {
/* 0: The first one must be ASCII -- ISO 8859-1 */
	"ASCII",
	"\255\136\244\345\277\351\244\112 (ASCII input)",
	    /* yin wen shu ru (English Input) */
        FALSE,
	hzASCIIfilter,
  },
  {
/* 1: IC -- Internal Code, encoding independent */
	"IC",
	"\272\176\246\162\277\351\244\112\241\107\244\272\275\130\241\107",
	    /* han zi shu ru (Chinese Input) : nei ma (internal code) : */
	FALSE,
	hzICfilter,
  },
  {
/* 2: null-pad */
	NULL, NULL, FALSE, hzTableFilter,
  },
};

static char *hzInputMethodNames[MAX_HZTBL];
static int numHZinputs;		/* number of HZ input methods loaded */

static Char hzinbuf[MAX_INBUF];	/* input buffer for conversion to HZ */
static int hzinbufCount;	/* input buffer character count */
static trieNode *hzinbufTriePtr[MAX_INBUF];
		/* pointers to Trie Nodes for each char in hzinbuf[] */
static trieNode *cTrieNode;	/* current Trie Node */
static XChar2b *hzChoicePtr;	/* HZ choices to be selected */
static int numChoice;		/* number of choices on the screen */
static int numChoiceR;		/* number of choices to the right */
static int numChoiceL;		/* number of choices to the left */

/* saved input state variable */
static int inputSaved = 0;
static Char save_hzinbuf[MAX_INBUF];
static int save_hzinbufCount;
static XChar2b *save_hzChoicePtr;
static int save_numChoice, save_numChoiceR, save_numChoiceL;

/*
 * We do not display hzinbuf[] directly on the input area. Instead,
 * each key in the input buffer for HZ conversion is replaced by its
 * "keyprompt" string and is put into dpyInbuf[]. We show
 * dpyInbuf[] directly on the input area.
 */
#define	MAX_DPYINBUF	( MAX_INBUF * MAX_KEYPROMPT )
Char dpyInbuf[MAX_DPYINBUF];	/* user input keys to be displayed */
int dpyInbufLen;		/* length of dpyInbuf[] */

/*
 * We put HZ choices from hzChoicePtr into dpyChoices[] in the form of
 * " l.XX", l is the label of the choice, and XX is the HZ choice.
 */
#define	MAX_DPYCHOICE	( MAX_CHOICE * 5  + 2 )
Char dpyChoices[MAX_DPYCHOICE];	/* input choices to be displayed */
int dpyChoicesLen;		/* length of dpyChoices[] */


/* ARGSUSED */
void HandleSwitchHZMode(w, event, params, nparams)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *nparams;
{
    TScreen *screen = &term->screen;

    if (*nparams == 1) {
	SetHZMode (screen, params[0], True);	/* redraw */
    }
}

SetHZMode (screen, name, doredraw)
    TScreen *screen;
    String name;
    Bool doredraw;
{
    register int i;

    for (i = 0; i < numHZinputs; i++) {
	if (! strcmp(name, hzInputMethodNames[i])) {
	    /* already loaded */
	    break;
	}
    }
    if (i == numHZinputs) {	/* not loaded */
	if (numHZinputs == MAX_HZTBL) {
	    /* too many input methods */
	    HZi_ShowMesg (screen, "Too many input methods");
	    return;
	} else {
	    if (HZLoadInputTable (screen, name, &(hzInputTables[i]))) {
		/* non-zero return: loading unsuccessfully */
		return;
	    }
	    hzInputMethodNames[i] = XtNewString(name);
	    hzInputFilters[i] = hzTableFilter;
	    numHZinputs++ ;
	}
    }

    switchHZmodeByNum (screen, i);
    if (doredraw)
	RefreshHZInputArea (screen);
}

void switchHZmodeByNum (screen, num)
    TScreen *screen;
    int num;
{
    if (num >= numHZinputs)
	return;

    /* switch cxterm HZ state to mode i */
    screen->hzIwin.hzMode = num;	/* change hzIwin.hzMode to i */
    cHZtbl = &(hzInputTables[num]);
    HZfilterInput = hzInputFilters[num];
    setMaxChoices (screen, cHZtbl->maxchoice);	/* reset screen->hzIwin.max_rc */
    initBufAndChoice (cHZtbl);
    clearInputHistory ();

    return;
}

/*
 * HZInitInputTable -- initialize the HZ Input Tables
 */
HZInitInputTable(screen)
    TScreen *screen;
{
    register struct pdHZinput *p = GBpredefine;

    if (screen->hzIwin.hzEncoding == GB_ENCODE) {
	p = GBpredefine;
    } else if (screen->hzIwin.hzEncoding == BIG5_ENCODE) {
	p = BIG5predefine;
    }

    numHZinputs = 0;
    while (p->name) {
	hzInputMethodNames[ numHZinputs ] = p->name;
	hzInputTables[ numHZinputs ].builtin = TRUE;
	strcpy (hzInputTables[ numHZinputs ].prompt, p->prompt);
	hzInputTables[ numHZinputs ].lenPrompt = strlen (p->prompt);
	hzInputTables[ numHZinputs ].multiChoice = p->multiChoice;
	hzInputFilters[ numHZinputs ] = p->filter;

	numHZinputs++;
	p++;
    }

    switchHZmodeByNum (screen, 0);	/* initially, ASCII mode */
}

/*
 * HZClearInputTable -- clear the HZ Input Tables
 */
HZClearInputTable(screen)
    TScreen *screen;
{
    register int i;

    for (i = 0; i < numHZinputs; i++) {
	if (! hzInputTables[i].builtin) {
	    if (hzInputTables[i].trieList)
		free (hzInputTables[i].trieList);
	    if (hzInputTables[i].hzList)
		free (hzInputTables[i].hzList);
	}
    }
    numHZinputs = 0;
}

int HZParseInput (screen, nbytes, strbuf)
    TScreen *screen;
    int nbytes;
    char strbuf[];
{
    /* optimize the simply cases */
    if (nbytes == 1) {
	return ((* HZfilterInput) (screen, cHZtbl, strbuf[0], strbuf));
    } else if (nbytes == 0) {
	return (0);
    } else {
	char ostr[100];	/* match the STRBUFSIZE in input.c */
	int i, nbytesOut = 0;

	strncpy (ostr, strbuf, nbytes);
	for (i = 0; i < nbytes; i++) {
	    nbytesOut += (* HZfilterInput) (screen, cHZtbl, ostr[i],
					&strbuf[nbytesOut]);
	}
	return (nbytesOut);
    }
}

/*
 * HZLoadInputTable -- load the HZ input table
 */
static HZLoadInputTable(screen, name, hztbl)
    TScreen *screen;
    char *name;
    HZinputTable *hztbl;
{
    char filename[MAXPATHLEN];
    char *dir, *pcolon;
    char tmpstr[80], magic[2];
    Boolean found = FALSE; 
    FILE *file;

    /* search name in different dirs */

    dir = screen->hzIwin.it_dirs;
    if (dir) {
	do {
	    pcolon = index(dir, ':');
	    if (pcolon)
		*pcolon++ = '\0';
	    strcpy (filename, dir);
	    strcat (filename, "/");
	    strcat (filename, name);
	    strcat (filename, CIT_SUFFIX);
	    if (access (filename, R_OK) == 0)
		found = TRUE;
	    dir = pcolon;
	} while (pcolon && ! found);
    } else {
	/* try . */
	strcpy (filename, name);
	strcat (filename, CIT_SUFFIX);
	if (access (filename, R_OK) == 0)
	    found = TRUE;
    }

    if (! found) {
	sprintf (tmpstr, "Unable to load input table for %s", name);
	HZi_ShowMesg (screen, tmpstr);
	return(-1);
    }
    file = fopen (filename, "r");
    if (! file) {
	sprintf (tmpstr, "Unable to load input table for %s", name);
	HZi_ShowMesg (screen, tmpstr);
	return(-1);
    }
    HZi_ShowMesg (screen, "Loading input table ...");
    (void) fread (magic, 2, 1, file);
    if (strncmp (magic, MAGIC_CIT, 2) != 0) {
	sprintf (tmpstr, "Not a legal %s file for %s", CIT_SUFFIX, name);
	HZi_ShowMesg (screen, tmpstr);
	return(-1);
    }
    if (fread(hztbl, sizeof(HZinputTable), 1, file) == 0) {
	sprintf(tmpstr, "Error in loading input table for %s", name);
	HZi_ShowMesg (screen, tmpstr);
	return(-1);
    }
    if (hztbl->encode != screen->hzIwin.hzEncoding) {
	sprintf(tmpstr, "%s is not a %s encoding input method", name,
		term->misc.hz_encoding);
	HZi_ShowMesg (screen, tmpstr);
	return(-1);
    }
    hztbl->trieList = (trieNode *) calloc (hztbl->sizeTrieList,
					   sizeof(trieNode));
    hztbl->hzList = (XChar2b *) calloc (hztbl->sizeHZList, sizeof(XChar2b));
    if ((! hztbl->hzList) || (! hztbl->trieList)) {
	    sprintf(tmpstr, "No memory to load input table for %s", name);
	    HZi_ShowMesg (screen, tmpstr);
	    if (hztbl->trieList)  free (hztbl->trieList);
	    if (hztbl->hzList)  free (hztbl->hzList);
	    return(-1);
    }
    if ((fread(hztbl->trieList, sizeof(trieNode), hztbl->sizeTrieList, file) 
		!= hztbl->sizeTrieList) ||
	(fread(hztbl->hzList, sizeof(XChar2b), hztbl->sizeHZList, file)
		!= hztbl->sizeHZList)) {
	    sprintf(tmpstr, "Error in loading input table for %s", name);
	    HZi_ShowMesg (screen, tmpstr);
	    free (hztbl->trieList);
	    free (hztbl->hzList);
	    return(-1);
    }
    return(0);
}

/**************************** Filters ***********************************/
/* Function Template:							*/
/*									*/
/* int hzXXXfilter (screen, hztbl, ch, strbuf)				*/
/*    TScreen *screen;							*/
/*    HZinputTable *hztbl;						*/
/*    unsigned char ch;							*/
/*    char strbuf[];							*/
/************************************************************************/

/*
 * hzTableFilter -- filter the input character using Input Table.
 *		Converted string are stored in strbuf, the length is returned.
 *		Return 0 if the character is eaten here.
 */
static int hzTableFilter(screen, hztbl, ch, strbuf)
    TScreen *screen;
    HZinputTable *hztbl;
    unsigned char ch;
    char strbuf[];
{
    if ((ch & 0x80) || (hztbl->keytype[ch] == HZ_KEY_INVALID))
	return (1);	/* not a valid key for this input, pass out */

    if (hztbl->keytype[ch] & HZ_KEY_INPUT_MASK) {
	register int i;
	register trieNode *tnptr;

	if (hzinbufCount >= MAX_INBUF) {
	    Bell();
	    return (0);
	}

	tnptr = &(hztbl->trieList[cTrieNode->tn_nextKeys]);

	/* YGZ-XXX: it may be faster if we do binary search. */
	for (i = 0; i < cTrieNode->tn_numNextKeys; i++, tnptr++) {
	    int j;

	    if (ch != tnptr->tn_key)
		continue;

	    /* then we got it */

	    if ((! hztbl->multiChoice) &&
		(tnptr->tn_numNextKeys == 0) &&
		(tnptr->tn_numHZchoice == 1)) {
		    /* multi choice is not necessary,			*/
		    /* no more input key is needed,			*/
		    /* and only one possible choice -- that's it!	*/

		    strbuf[0] = hztbl->hzList[tnptr->tn_hzidx].byte1;
		    strbuf[1] = hztbl->hzList[tnptr->tn_hzidx].byte2;

		    initBufAndChoice (hztbl);	/* must reset before Clear */
		    HZi_ClearHzInput (screen);
		    return (2);		/* 2 bytes converted in strbuf[] */
	    }

	    hzinbuf[hzinbufCount] = ch;
	    hzinbufTriePtr[ hzinbufCount++ ] = cTrieNode;
	    cTrieNode = tnptr;

	    for (j = 0; j < hztbl->keyprompt[ch].ptlen; j++)
		dpyInbuf[ dpyInbufLen++ ] = hztbl->keyprompt[ch].prompt[j];

	    HZi_ReshowUserInput (screen);
	    makeChoice (hztbl, screen->hzIwin.max_rc);
	    HZi_ShowInputChoices (screen);
	    return (0);
	}
    }

    if (hztbl->keytype[ch] & HZ_KEY_SELECTION_MASK) {
	int sel;

	if (numChoice == 0)
	    return (1);		/* nothing to select, pass the key out */

	sel = (hztbl->keytype[ch] & HZ_KEY_SELECTION_NUM);
	if (sel >= numChoice) {
	    Bell();
	    return (0);
	}
	strbuf[0] = hzChoicePtr[sel].byte1;
	strbuf[1] = hzChoicePtr[sel].byte2;

	saveInputHistory ();	/* save this HZ input */

	initBufAndChoice (hztbl);
	HZi_ClearHzInput (screen);
	return (2);	/* 2 bytes converted in strbuf[] */
    }

    if (hztbl->keytype[ch] == HZ_KEY_BACKSPACE) {

	if (hzinbufCount == 0)
	    return (1);		/* nothing to delete, pass the key out */
	dpyInbufLen -= hztbl->keyprompt[ hzinbuf[--hzinbufCount] ].ptlen;
	HZi_ClearHzInput (screen);	/* clear from last input key */

	cTrieNode = hzinbufTriePtr[ hzinbufCount ];
	makeChoice (hztbl, screen->hzIwin.max_rc);
	HZi_ShowInputChoices (screen);
	return (0);

    } else if (hztbl->keytype[ch] == HZ_KEY_KILL) {

	if (hzinbufCount == 0)
	    return (1);		/* nothing to kill, pass the key out */
	initBufAndChoice (hztbl);
	HZi_ClearHzInput (screen);
	return (0);

    } else if (hztbl->keytype[ch] == HZ_KEY_RIGHT) {

	if (numChoice == 0)
	    return (1);		/* nothing to move, pass the key out */
	if (numChoiceR == 0) {
	    Bell ();
	    return (0);		/* no more choice to the right */
	}
	moveChoiceR (hztbl, screen->hzIwin.max_rc);
	HZi_ShowInputChoices (screen);
	return (0);

    } else if (hztbl->keytype[ch] == HZ_KEY_LEFT) {

	if (numChoice == 0)
	    return (1);		/* nothing to move, pass the key out */
	if (numChoiceL == 0) {
	    Bell ();
	    return (0);		/* no more choice to the left */
	}
	moveChoiceL (hztbl, screen->hzIwin.max_rc);
	HZi_ShowInputChoices (screen);
	return (0);

    } else if (hztbl->keytype[ch] == HZ_KEY_REPEAT) {

	if (hzinbufCount != 0)
	    return (1);		/* enable restore only when hzinbuf is empty */
	if (! inputSaved)
	    return (1);		/* nothing saved before, pass the key out */

	restoreInputHistory (hztbl);
	HZi_ReshowUserInput (screen);
	HZi_ShowInputChoices (screen);
	return (0);

    }

    /* unknown (yet valid) key */
    Bell ();
    return (0);	
}

static int hzASCIIfilter (screen, hztbl, ch, strbuf)
    TScreen *screen;
    HZinputTable *hztbl;
    unsigned char ch;
    char strbuf[];
{
    strbuf[0] = ch;
    return(1);
}

#define hex2dec(ch)			\
	(isdigit(ch) ? (ch)-'0' :	\
		(((ch)>='a') && ((ch)<='f')) ? (ch)-'a'+10 : (ch)-'A'+10)

static int hzICfilter (screen, hztbl, ch, strbuf)
    TScreen *screen;
    HZinputTable *hztbl;
    unsigned char ch;
    char strbuf[];
{
    if (isxdigit (ch)) {
	hzinbuf[ hzinbufCount++ ] = ch;
	dpyInbuf[ dpyInbufLen++ ] = ch;
	HZi_ReshowUserInput (screen);

	if (hzinbufCount == 4) {
	    strbuf[0] = hex2dec(hzinbuf[0]) * 16 + hex2dec(hzinbuf[1]);
	    strbuf[1] = hex2dec(hzinbuf[2]) * 16 + hex2dec(hzinbuf[3]);

	    hzinbufCount = 0;
	    dpyInbufLen = 0;
	    HZi_ClearHzInput (screen);
	    return (2);	/* 2 bytes converted in strbuf[] */
	} else
	    return (0);
    }

    switch (ch) {
	case '\010':	/* \b */
	case '\177':	/* DEL */
	    if (hzinbufCount == 0)
		return (1);	/* nothing to delete, pass the key out */
	    hzinbufCount-- ;
	    dpyInbufLen-- ;
	    HZi_ClearHzInput (screen);	/* clear last input key */
	    return (0);
	    break;

	case '\015':	/* \r */
	case '\025':	/* ^U */

	    if (hzinbufCount == 0)
		return (1);	/* nothing to kill, pass the key out */
	    hzinbufCount = 0;
	    dpyInbufLen = 0;
	    HZi_ClearHzInput (screen);
	    return (0);
	    break;
    }

    return (1);
}

/* GB: QW */
static int hzQWfilter (screen, hztbl, ch, strbuf)
    TScreen *screen;
    HZinputTable *hztbl;
    unsigned char ch;
    char strbuf[];
{
    if (isdigit (ch)) {
	hzinbuf[ hzinbufCount++ ] = ch;
	dpyInbuf[ dpyInbufLen++ ] = ch;
	HZi_ReshowUserInput (screen);

	if (hzinbufCount == 4) {
	    strbuf[0] = (hzinbuf[0] - '0') * 10 + (hzinbuf[1] - '0') + 0xa0;
	    strbuf[1] = (hzinbuf[2] - '0') * 10 + (hzinbuf[3] - '0') + 0xa0;

	    hzinbufCount = 0;
	    dpyInbufLen = 0;
	    HZi_ClearHzInput (screen);
	    return (2);	/* 2 bytes converted in strbuf[] */
	} else
	    return (0);
    }

    switch (ch) {
	case '\010':	/* \b */
	case '\177':	/* DEL */
	    if (hzinbufCount == 0)
		return (1);	/* nothing to delete, pass the key out */
	    hzinbufCount-- ;
	    dpyInbufLen-- ;
	    HZi_ClearHzInput (screen);	/* clear last input key */
	    return (0);
	    break;

	case '\015':	/* \r */
	case '\025':	/* ^U */

	    if (hzinbufCount == 0)
		return (1);	/* nothing to kill, pass the key out */
	    hzinbufCount = 0;
	    dpyInbufLen = 0;
	    HZi_ClearHzInput (screen);
	    return (0);
	    break;
    }

    return (1);
}


static initBufAndChoice (hztbl)
    HZinputTable *hztbl;
{
    hzinbufCount = 0;
    dpyInbufLen = 0;
    numChoiceL = 0;
    numChoice = 0;
    numChoiceR = 0;
    dpyChoicesLen = 0;
    cTrieNode = hztbl->trieList;
    hzChoicePtr = hztbl->hzList;
}

RemakeHZInput (screen)
    TScreen *screen;
{
    /* (re-) Calculate maximun number of choices on the screen */
    setMaxChoices (screen, cHZtbl->maxchoice);

    if ((! cHZtbl) || cHZtbl->builtin)	/* no Choices */
	return;
    if (numChoice > screen->hzIwin.max_rc) {
	numChoiceR += numChoice - screen->hzIwin.max_rc;
	numChoice = screen->hzIwin.max_rc;
	fillDpyChoices (cHZtbl);
    } else if ((numChoice < screen->hzIwin.max_rc) && (numChoiceR > 0)) {
	if (numChoice + numChoiceR <= screen->hzIwin.max_rc) {
	    numChoice += numChoiceR;
	    numChoiceR = 0;
	} else {
	    numChoiceR -= screen->hzIwin.max_rc - numChoice;
	    numChoice = screen->hzIwin.max_rc;
	}
	fillDpyChoices (cHZtbl);
    }
}

static makeChoice (hztbl, max_rc)
    HZinputTable *hztbl;
    int max_rc;		/* maximun choices on screen */
{
    hzChoicePtr = &(hztbl->hzList[cTrieNode->tn_hzidx]);
    numChoiceL = 0;		/* start from 1 */
    if (cTrieNode->tn_numHZchoice > max_rc) {
	numChoice = max_rc;
	numChoiceR = cTrieNode->tn_numHZchoice - numChoice;
    } else {
	numChoice = cTrieNode->tn_numHZchoice;
	numChoiceR = 0;
    }
    fillDpyChoices (hztbl);
}

static moveChoiceR (hztbl, max_rc)
    HZinputTable *hztbl;
    int max_rc;		/* maximun choices on screen */
{
    hzChoicePtr += numChoice;
    numChoiceL += numChoice;
    if (numChoiceR > max_rc) {
	numChoice = max_rc;
	numChoiceR -= numChoice;
    } else {
	numChoice = numChoiceR;
	numChoiceR = 0;
    }
    fillDpyChoices (hztbl);
}

static moveChoiceL (hztbl, max_rc)
    HZinputTable *hztbl;
    int max_rc;		/* maximun choices on screen */
{
    numChoiceR += numChoice;
    if (numChoiceL > max_rc) {
	numChoice = max_rc;
	numChoiceL -= numChoice;
    } else {
	numChoice = numChoiceL;
	numChoiceL = 0;
    }
    hzChoicePtr -= numChoice;
    fillDpyChoices (hztbl);
}

static fillDpyChoices (hztbl)
    HZinputTable *hztbl;
{
    register int i;

    dpyChoicesLen = 0;
    dpyChoices[ dpyChoicesLen++ ] = (numChoiceL ? '<' : ' ');
    for (i = 0; i < numChoice; i++) {
	dpyChoices[ dpyChoicesLen++ ] = ' ';
	dpyChoices[ dpyChoicesLen++ ] = hztbl->choicelb[i];
	dpyChoices[ dpyChoicesLen++ ] = '.';
	dpyChoices[ dpyChoicesLen++ ] = hzChoicePtr[i].byte1;
	dpyChoices[ dpyChoicesLen++ ] = hzChoicePtr[i].byte2;
    }
    dpyChoices[ dpyChoicesLen++ ] = ' ';
    dpyChoices[ dpyChoicesLen++ ] = (numChoiceR ? '>' : ' ');
}

struct codetbl {
	char *codestr;
	int   codeint;
} codeTbl[] = {

	"GB",		GB_ENCODE,
	"Gb",		GB_ENCODE,
	"gb",		GB_ENCODE,
	"GUOBIAO",	GB_ENCODE,
	"GuoBiao",	GB_ENCODE,
	"guobiao",	GB_ENCODE,

	"BIG5",		BIG5_ENCODE,
	"Big5",		BIG5_ENCODE,
	"big5",		BIG5_ENCODE,
	"B5",		BIG5_ENCODE,
	"b5",		BIG5_ENCODE,

	NULL,		GB_ENCODE,	/* By default, GB_ENCODE */
};

int HZencode (name)
    char *name;
{
    struct codetbl *pct = codeTbl;

    while (pct->codestr) {
	if (strcmp (name, pct->codestr) == 0)
	    return (pct->codeint);
	pct++;
    }
    return (pct->codeint);
}

static int saveInputHistory()
{
    /* make minimum effords in save */
    save_hzinbufCount = hzinbufCount;
    strncpy (save_hzinbuf, hzinbuf, hzinbufCount);
    save_hzChoicePtr = hzChoicePtr;
    save_numChoice  = numChoice;
    save_numChoiceR = numChoiceR;
    save_numChoiceL = numChoiceL;
    inputSaved = 1;
}

static int restoreInputHistory(hztbl)
    HZinputTable *hztbl;
{
  register int i;

    hzinbufCount = save_hzinbufCount;
    strncpy (hzinbuf, save_hzinbuf, hzinbufCount);
    hzChoicePtr = save_hzChoicePtr;
    numChoice  = save_numChoice;
    numChoiceR = save_numChoiceR;
    numChoiceL = save_numChoiceL;

    /* recompute everything */
    for (i = 0; i < hzinbufCount; i++) {
	unsigned char ch = hzinbuf[i];
	register int j, numNextKeys;

	hzinbufTriePtr[i] = cTrieNode;
	numNextKeys = cTrieNode->tn_numNextKeys;
	cTrieNode = &(hztbl->trieList[cTrieNode->tn_nextKeys]);
	for (j = 0; j < numNextKeys; j++, cTrieNode++) {
	    if (ch == cTrieNode->tn_key)
		break;
	}

	for (j = 0; j < hztbl->keyprompt[ch].ptlen; j++) {
	    dpyInbuf[ dpyInbufLen++ ] = hztbl->keyprompt[ch].prompt[j];
	}
    }

    fillDpyChoices (hztbl);
}

static int clearInputHistory()
{
    inputSaved = 0;
}

static int setMaxChoices (screen, maxchoice)
    TScreen *screen;
    int maxchoice;
{
    /* Set the maximun number of choices on the screen */
    /* from the width of the screen and the maximun choices allowed */
    /* max_col + 1 - 3: 3 means a leading "<" and ending " >" */
    screen->hzIwin.max_rc = (screen->max_col - 2) / 5;
    if (screen->hzIwin.max_rc > maxchoice)
	screen->hzIwin.max_rc = maxchoice;
}

#endif /* HANZI */
