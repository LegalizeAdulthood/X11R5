/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/lex.c,v 1.17 1991/01/02 18:17:13 gk5g Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/lex.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/lex.c,v 1.17 1991/01/02 18:17:13 gk5g Exp $";
#endif

/* lex.c		

	Code for the lex data object
*/
/*
 *    $Log: lex.c,v $
 * Revision 1.17  1991/01/02  18:17:13  gk5g
 * Removed hacked to get around apparent failure of tolower on sparc arch.
 * The real problem was related to dynamic loading and the fact the tolower/toupper are now function that live in libc.a.
 * They were previously macros.
 *
 * Revision 1.16  90/12/18  15:43:00  wjh
 * worked around error in C compiler which caused Illegal Instruction trap in Ness compilation of present.n
 * 
 * Revision 1.15  90/07/29  14:54:13  wjh
 * fixed so styles and objects are fully supported in short strings
 * 
 * Revision 1.14  89/09/10  17:36:28  wjh
 * added SetPragmatParser
 * 
 * Revision 1.13  89/08/23  16:11:57  wjh
 * 
 * TokenUpTo
 * 
 * Revision 1.12  89/07/26  09:45:25  wjh
 * added semicolon in union declaration
 * 
 * Revision 1.11  89/06/01  10:08:58  wjh
 * revision for new securtiy system
 * 
 * Revision 1.5  88/12/08  15:23:35  wjh
 * Teach it to ignore illegal characters in source without looping.
 * 
 * Revision 1.4  88/12/07  22:38:52  wjh
 * fix error not caught by hc
 * 
 * Revision 1.3  88/12/07  22:24:48  wjh
 * added ness_ParseNumber()
 * 
 * Revision 1.2  88/11/29  16:39:45  wjh
 * fixed RecentPosition at end of file
 * 
 * Revision 1.1  88/10/21  10:41:12  wjh
 * Initial revision
 *  
 * Revision 1.0  88/07/13  09:35:59  wjh
 * Copied from /usr/andrew/lib/nullinset
 */

#include <ctype.h>

#include <class.h>
#include <toksym.ih>
#include <lexdef.ih>
#include <lex.eh>
#include <xgetchar.h>
#include <envrment.ih>

#undef environment_GetParent
#define environment_GetParent(env) \
		((struct environment *)(env)->header.nestedmark.parent)


/* a set of routines to access text objects like a file */

#define CurCh(self)	(self->curchar)
#define NextCh(self)	(self->curpos++, self->curchar=((self->curpos<self->endpos)   \
				? ( text_CorrectGetChar(self->source, self->curpos))  : EOF))
#define UngetCh(self)	(self->curpos--, self->curchar=text_CorrectGetChar(self->source, self->curpos))
#define TellCh(self)	(self->curpos)
#define SeekCh(self, where)	(self->curpos = where,  \
				self->curchar=text_CorrectGetChar(self->source, \
					self->curpos))
#define StorePos(sym, start, nextloc) ((sym)->loc = (start), (sym)->len = (nextloc) - (start))


static struct lex *CurLex;	/* backdoor arg to NextToken() */

static struct classinfo *textClass = NULL;


	struct lex *
lex__Create(ClassID, def)
	struct classhdr *ClassID;
	struct lexdef *def;
{
	struct lex *result = lex_New();
	result->def = def;
	return result;
}


	void
lex__SetSource(self, text, loc, len, scope, constscope)
	register struct lex *self;
	struct text *text;
	long loc, len;
	toksym_scopeType scope, constscope;
{
	long i;
	self->source = text;
	self->endpos = loc+len;
	SeekCh(self, loc);		/* XXX numeric seek */

	if (scope == toksym_GLOBAL) 
		self->currentScope = toksym_TNewScope(self->def->grammarScope);
	else
		self->currentScope = scope; 
	if (constscope == toksym_GLOBAL) 
		self->constScope = toksym_TNewScope(self->def->grammarScope);
	else
		self->constScope = constscope; 
	
	self->RecentIndex = 0;
	for (i = lex_RECENTSIZE; i--; )
		self->RecentPos[i] = self->RecentLen[i] = 0;

	CurLex = self;
}

	void
lex__SetPragmatParser(self, flag, parser, rock)
	register struct lex *self;
	char flag;
	void (* parser)();
	long *rock;
{
	self->pragmatFlag = flag;
	self->pragmatParser = parser;
	self->pragmatRock = rock;
}

	void
lex__SetCurrent(self)
	register struct lex *self;
{
	CurLex = self;
}

	boolean
lex__InitializeClass(ClassID)
	struct classhdr *ClassID;
{
	CurLex = NULL;
	return TRUE;
}

	boolean
lex__InitializeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct lex  *self;
{
	self->def = NULL;
	self->source = NULL;
	self->pragmatFlag = '\0';	/* will not match (I hope) */
	return TRUE;
}

	void 
lex__FinalizeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct lex  *self;
{
}


/* lex_RecentPosition(index, lenloc)
	returns the character index of the first character of the 'index'th token
	indices are zero for the most recent token, -1 for its predecessor, and so on
	indices must be in range [- RECENTSIZE+1 . . . 0]
*/
	long
lex__RecentPosition(ClassID, index, lenloc)
	struct classhdr *ClassID;
	long index;
	long *lenloc;
{
	long x;
	x = CurLex->RecentIndex - 1 + index;
	while (x < 0) x += lex_RECENTSIZE;
	if (lenloc != NULL)
		*lenloc = CurLex->RecentLen[x];
	return CurLex->RecentPos[x];
}

/* lex_RecentIndent(index)
	returns the indent of 'index'th most recent token
	'index' must be in range [- RECENTSIZE+1 . . . 0]
	if token is preceded by anything other than whitespace,
		its indent is 999
*/
	long
lex__RecentIndent(ClassID, index)
	struct classhdr *ClassID;
	long index;
{
	long x;
	long indent = 0;
	long curpos = TellCh(CurLex);

	x = CurLex->RecentIndex - 1 + index;
	while (x < 0) x += lex_RECENTSIZE;
	SeekCh(CurLex, CurLex->RecentPos[x]);

	while (TellCh(CurLex) > 0) 
		if (UngetCh(CurLex) == ' ')  indent ++;
		else if (CurCh(CurLex) == '\t') indent +=8;
		else break;

	if (TellCh(CurLex) >= 0 && CurCh(CurLex) != '\n') 
		indent = 999;

	SeekCh(CurLex, curpos);
	return (indent);
}

/* lex_Repeat(index)
	backup so the next token reported is the index'th, where
	index is as for RecentPosition.
*/
	void
lex__Repeat(ClassID, index)
	struct classhdr *ClassID;
	long index;
{
	long x;
	x = CurLex->RecentIndex - 1 + index;
	while (x < 0) x += lex_RECENTSIZE;
	SeekCh(CurLex, CurLex->RecentPos[x]);	/* XXX numeric seek */
	CurLex->RecentIndex = x;
}


/* lex_GetCurrent()
	return the current lex
*/
	struct lex *
lex__GetCurrent(ClassID)
	struct classhdr *ClassID;
{
	return CurLex;
}



/* = = = = = = = = = = = = = = = = = = = = = = = 
	the rest of this file is for 
		NextToken()
= = = = = = = = = = = = = = = = = = = = = = = = */



	static
ReportError(self, msg, loc, len)
	register struct lex *self;
	unsigned char *msg;
	long loc, len;
{
	register long savex = self->RecentIndex;
	/* make dummy last token for RecordError */
	self->RecentPos[self->RecentIndex] = loc;
	self->RecentLen[self->RecentIndex] = len;
	self->RecentIndex++;
	if (self->RecentIndex >= lex_RECENTSIZE) self->RecentIndex = 0;

	self->def->yyerror(msg);	/* report the error */

	self->RecentIndex = savex;
}


/* doThong(self, c)
	CurCh() is possibly the start of a multi-character token
	doThong examines subsequent characters and generates the
	proper token.
*/
	static struct toksym *
doThong(self, c)
	register struct lex *self;
	unsigned char c;
{
	unsigned char buf[50];
	register unsigned char *bx = buf;
	long thonglen;
	struct toksym *tyylval;
	register struct lexdef *def = self->def;

	*bx++ = c;
	tyylval = def->charNode[c];
	thonglen = 1;
	while (TRUE) {
		long len;
		register index;
		*bx++ = NextCh(self);
		len = bx - buf;
		/* look for buf in thongTable */
		for (index = 0; def->thongTable[index]; index++) {
			if (strncmp(buf, def->thongTable[index], len) == 0) {
				if (def->thongTable[index][len] == '\0') {
					/* found a match */
					tyylval = def->thongSym[index];
					thonglen = len;
					/* continue checking for longer thong */
				}
				break;	/* go on to next character */
			}
		}
		if (def->thongTable[index] == 0) {
			/* no match; thong is shorter than current length 
				set to char after the thong */
			SeekCh(self, TellCh(self) - len + thonglen + 1);
			return (tyylval);
		}
	}
}

/* translate all characters to those of interest for numbers:
	0..9 a..f  +  ,  -  .   */
static unsigned char xlate[] = {	16, 20, 17, 18, 20, 		  /*  +  ,  -  .  /  */
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9,	  /* 0 ... 9 */
		20, 20, 20, 20, 20, 20, 	  /* :  ;  <  =  >  ? */
		20, 10, 11, 12, 13, 14, 15, 20,	  /* @  A ... G */
		20, 20, 20, 20, 20, 20, 20, 20,	  /* H ... O */
		20, 20, 20, 20, 20, 20, 20, 20,   /* P ... W */
		19, 20, 20, 20, 20, 20, 20, 20,	  /* X ... */
		20, 10, 11, 12, 13, 14, 15, 20,	  /* `  a ... g */
		20, 20, 20, 20, 20, 20, 20, 20,	  /* h ... o */
		20, 20, 20, 20, 20, 20, 20, 20, 19,  /* p ... x */
};

/* for each of the 21 kinds of character that may appear in or after numbers, 
		specify a transition from each of the states 0...6 */
static unsigned char newstate [8][21] = {
/*	char:	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, a, b, c, d, e, f, +,  -, ., x other */
/* 0 start */ {	0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8, 8, 8, 8, 5, 8, 8, 8, 4, 3, 8  },
/* 1 decimal */  {	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 8, 8, 8, 8, 5, 8, 8, 8, 4, 8, 8  }, 
/* 2 octal */  {	2, 2, 2, 2, 2, 2, 2, 2, 7, 7, 8, 8, 8, 8, 7, 8, 8, 8, 7, 9, 8  }, 
/* 3 hex */  {	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 8, 8, 7, 9, 8  }, 
/* 4 after . */  {	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 8, 8, 5, 8, 8, 8, 9, 9, 8  }, 
/* 5 after e */  {	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 9, 9, 9, 9, 9, 9, 6, 6, 9, 9, 9  }, 
/* 6 exponent */ { 	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 9, 8, 8, 8, 9, 9, 8  }, 
/* 7 has error */ {	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 9, 9, 9, 9, 9, 9, 9, 9, 7, 9, 9  },
/* 8 accept */
/* 9 error halt */
}; 


/* lex_ParseNumber(text, pos, len, intval, realval)
		Parses a number from text, starting at pos, which must 
		select a character valid at the beginning of a number. 
		Sets len to length, intval to integer value, and dblval 
		to real value if the number was real.  
		Returns success status:
			0-error,  1-integer,   2- real.

	Integers are a string of digits.
	Reals have one of the following formats where
			ddd is a digit sequence
			p is empty or + or -
			E may be 'e' or 'E'  and means the exponent
		.ddd
		ddd.
		ddd.ddd
		.dddEpddd
		ddd.dddEpddd
		dddEpddd
*/
	unsigned long
lex__ParseNumber(ClassID, text, pos, plen, intval, dblval)
	struct classhdr *ClassID;
	struct text *text;
	unsigned long pos, *plen, *intval;
	double *dblval;
{
	unsigned long val, len;
	unsigned long startpos;
	unsigned char oldstate, curstate;
	register unsigned x;
	unsigned long success;

	/* compute length of number */
	len = 0;
	val = 0;
	oldstate = 0;

	x = text_CorrectGetChar(text, pos);
	curstate = (x == '0') ? 0 : 1;
	startpos = pos;
	while (TRUE) {
		len ++;
		x = (x < '+' || x > 'x')  ?  20  :  xlate[x - '+'];
		oldstate = curstate;
		curstate = newstate[oldstate][x];
		if (curstate >= 8) break;
	
		/* accumulate value */
		switch(curstate) {
		case 1:	/* accumulate decimal value */
			val = val * 10 + x;
			break;
		case 2:	/* accumulate octal value */
			val = val * 8 + x;
			break;
		case 3:	/* accumulate hexadecimal value */
			if (x != 19) 
				val = val * 16 + x;
			break;
		case 7:	/* in erroneous value */
			if (oldstate != curstate) 
				/* record the error character */
				{} /* ErrorChar = CurCh(self);  */
		}
		pos ++;
		x = text_CorrectGetChar(text, pos);
	}
	len--;	/* last character terminated number, but is not in it */
	*plen = len;	/* send back len */

	/* convert */
	if (curstate == 9)
		success = 0;
	else if (oldstate >= 4) {
		/* is real value */
		extern double atof();
		register unsigned char *buf, *bx;

		success = 2;
		/* go back to beginning of number */
		pos = startpos;
		x = text_CorrectGetChar(text, pos);
		
		/* malloc a buffer and copy number into it */
		buf = (unsigned char *)malloc(len+1);
		bx = buf;
		while ( len-- > 0 ) {
			*bx++ = x;
			pos++;
			x = text_CorrectGetChar(text, pos);
		}
		*bx = '\0';
		/* convert number */
		*dblval = atof(buf);
		*intval = *dblval + 0.5;	/* round to integer */
		free(buf);
	}
	else {
		/* is integer */
		success = 1;
		*intval = val;
	}
	return success;
}


/* ParseNumber()
	Parses the input string for an integer or real number
	CurCh() is first digit.
	Integers are a string of digits.
	Reals have one of the following formats where
			ddd is a digit sequence
			p is empty or + or -
			E may be 'e' or 'E'  and means the exponent
		.ddd
		ddd.
		ddd.ddd
		.dddEpddd
		ddd.dddEpddd
		dddEpddd

	sets yylval to a struct toksym for the numeric value
	leaves curpos set to next character after the number
*/
	static struct toksym *
ParseNumber(self)
	register struct lex *self;
{
	long startpos = TellCh(self);
	long len, intval, success;
	union {double d; long i[2];} realval;
	unsigned char symbuf[30];
	boolean new;
	struct toksym *t;

	/* XXX we drop out of NextCh() and friends */
	success = lex_ParseNumber(self->source, self->curpos, &len, 
				&intval, &realval.d);

	SeekCh(self, startpos + len);
	if (success == 0) {
		ReportError(self, ":error ends number", startpos, len);
		intval = -999;
	}

	if (success == 2)
		/* double value */
		sprintf(symbuf, "0x%lx%lx", realval.i[0], realval.i[1]);
	else
		sprintf(symbuf, "0x%lx", intval);

	t = toksym_TLocate(symbuf, self->def->prototype, 
			self->constScope, &new);
	if (new) {
		if (success == 2) {
			t->info.realval = (double *)malloc(sizeof(double));
			*(t->info.realval) = realval.d;
			t->toknum = self->def->RealConTokNum;
		}
		else {
			/* integer value   (or error) */
			t->info.intval = intval;
			t->toknum = self->def->IntConTokNum;
		}
	}
	StorePos(t, startpos, TellCh(self));
	return(t);
}


/* ParseWord()
	parses a keyword or identifier
	first character must be in CurCh(self)
	accepts only upper and lower case letters, _, and digits
	maximum length is 256
	sets yylval to struct toksym for the word
	leaves next character in the text to be read
*/
	static struct toksym *
ParseWord(self)
	register struct lex *self;
{
	unsigned char buf[258];
	unsigned char *cx = buf;
	unsigned char *cend = buf + 256;
	register unsigned long c;
	register struct toksym *t;
	boolean new, ok = TRUE;
	unsigned long startpos = TellCh(self);

	UngetCh(self);
	if (self->def->foldCase)
		while ((c=NextCh(self)) != EOF  &&
				(c == '_'  ||  isalpha(c)  ||  isdigit(c)) ) {
			if (cx < cend) {
				*cx++ = (isalpha(c) && isupper(c)) ? tolower(c) : c;
			}
			else  ok = FALSE;
		}
	else
		while ((c=NextCh(self)) != EOF  &&
				(c == '_'  ||  isalpha(c)  ||  isdigit(c)) ) {
			if (cx < cend)
				*cx++ = c;
			else  ok = FALSE;
		}
	*cx = '\0';

	if (! ok) 
		ReportError(self, ":Word too long", startpos, TellCh(self) - startpos);

	t = toksym_TLocate(buf, self->def->prototype, self->currentScope, &new);
	if (new) {
		t->toknum = self->def->IdTokNum;
		StorePos(t, startpos, TellCh(self));
	}
	else  {} /* do not change toknum: it may be a reserved word */
	return (t);
}

/* there are four cases of setStringConTokNum.  They are distinguished by the first character
of the name field:
	'	a character constant: one character follows in name
			the info.intval field has the numeric value of the character
			this is reported as an IntConTokNum
	"	if no character follows in name, this is an empty string
	"	if characters follow in name, the \ reduced version of the string
			follows in name
			the original string is indexed by the loc/len fields
	~	a short string with styles
	/	long string.  

For ~ and /, the name contains only a sequence number.
In all cases, the loc/len values select only the string contents, not the delimiters.
Error messages for strings contain loc/len value which select the delimiters as well.
*/


unsigned char esctab[] = "r\rn\nf\ft\tb\bv\v\"\"\'\'\\\\";

/* ParseShortString(self, delim)
	parses a string or C characer constant ending with 'delim'
	(uses \ as escape character)
	string must not cross line boundary
	length limited to 256
	must not contain object references

	at start, the opening 'delim' must be in CurCh(self)
	at completion, CurCh(self) is the char after closing 'delim'
	leaves in yylval a pointer to struct toksym
	the toksym string value begins with a dummy instance of 'delim'
*/
	static struct toksym *
ParseShortString(self, delim)
	register struct lex *self;
	unsigned char delim;
{
	unsigned char buf[260];
	unsigned char *cx = buf;
	unsigned char *cend = buf + 257;	/* just past last char acceptable */
	register unsigned long c;
	register struct toksym *t;
	boolean new, styled;
	long startpos = TellCh(self);		/* loc of initial delimiter */
	long endpos;

	*cx++ = delim;   /* dummy character to avoid confusion with
			  other classes of symbols */

	while ((c=NextCh(self)) != EOF) {
		if (c == '\\') {
			unsigned char *ex = esctab;
			c = NextCh(self);
			if (c == '\n') {
				/* CurrentLine++; */
				continue;
			}
			for ( ; *ex && *ex != c; ex +=2) {}
			if (*ex)
				c = *(ex+1);
			else if (isdigit(c)) {
				/* parse digit string */
				c = 0;
				while (isdigit(CurCh(self))) {
					c = 8 * c + (CurCh(self) - '0');
					NextCh(self);
				}
				UngetCh(self);
			}
			else { /* c is the next character */ }
		}
		else if (c == delim) {
			endpos = TellCh(self);
			NextCh(self);
			break;
		}
		else if (c == '\n') {
			endpos = TellCh(self);
			ReportError(self, 
				(delim == '\"') ? ":String crosses end of line"
					: ":Character constant crosses line end", 
				startpos, endpos + 1 - startpos);
			break;
		}

		if (cx < cend)
			*cx++ = c;
		else {
			/* string too long.  Assume missing 'delim'.
				Exit in hopes of catching further errors.  */
			endpos = TellCh(self);
			break;
		}
	}
	*cx = '\0';

	if (c == EOF) {
		endpos = TellCh(self);
		ReportError(self, (delim == '\"') ? ":missing \" at end of file"
					: ":missing \' at end of file", 
			startpos, endpos - startpos);	/* pos'n arith XXX */
	}
	else if (delim == '\'' && cx-buf != 2)
		ReportError(self, ":character constant is not exactly one character", 
			startpos, endpos + 1 - startpos);	/* pos'n arith XXX */
	else if (cx >= cend)
		ReportError(self, ":String constant longer than 256 characters", 
			startpos, endpos - startpos);	/* pos'n arith XXX */

	if (cx == buf+1) 
		return self->def->EmptyStringToken;

	if (textClass == NULL)
		textClass = class_Load("text");
	if (! class_IsType(self->source, textClass))
		styled = FALSE;
	else {
		struct environment *env;
		env = environment_GetInnerMost(self->source->rootEnvironment,
				startpos+1);
		while (env != NULL && (env->type != environment_Style 
				|| env->data.style == NULL))
			env = environment_GetParent(env);
		styled = ( ! env == NULL)
			||  environment_GetNextChange(self->source->rootEnvironment,
				startpos+1) < endpos - startpos - 1;
	}
	if (styled) {
		/* short styled string */
		char buf[50];
		sprintf(buf, "~%d", self->def->nextlongstring);
		self->def->nextlongstring ++;
		t = toksym_TLocate(buf, self->def->prototype, self->constScope, &new);
		t->info.str = NULL;
		t->toknum = self->def->StringConTokNum;
		StorePos(t, startpos+1, endpos);	/* position arith XXX */
	}
	else {
		t = toksym_TLocate(buf, self->def->prototype, self->constScope, &new);
		if (new) {
			StorePos(t, startpos+1, endpos);	/* position arith XXX */
			if (delim == '\'') {
				t->toknum = self->def->IntConTokNum;
				t->info.intval = buf[1];   /* store character value */
			}
			else
				t->toknum = self->def->StringConTokNum;
		}
	}
	return (t);
}

/* ParseLongString(self)
	parses a long embedded string
	opening delimiter is  \n//\n	(two slashes alone on a line)
	closing delimiter is \n\\\\	(two backslashes at the start of a line)
	the string may be of any length and contain objects
	the internals are not processed for backslashes
	there is no way for the enclosed text to contain a line
		beginning with two backslashes
	neither the beginning nor trailing \n is included in the string value

	initially curpos is for the \n  just before the string
	returns toksym for result.  symbol string in toksym is a dummy
		of form  /ddd, where ddd is a number
	final curpos is the final \n
*/
	static struct toksym *
ParseLongString(self)
	register struct lex *self;
{
	register unsigned long c;
	register struct toksym *t;
	boolean new;
	long startpos, afterpos;

	c = CurCh(self);			/* get preceding \n */
			/* start with \n in case this is an empty string */
	startpos = TellCh(self) + 1;		/* record loc of  first char *//* XXX pos arithmetic */
	while (c != EOF)
		if (c == '\n') {
			afterpos = TellCh(self);
			if ((c=NextCh(self)) == '\\')
				if ((c=NextCh(self)) == '\\') {
					NextCh(self);
					break;
				}
		}
		else c = NextCh(self);
	/* leave the character after final \ as CurCh(self) */

	if (c == EOF) {
		afterpos = TellCh(self);
		ReportError(self, ":missing \\\\ at end of file", 
				startpos-2, afterpos - startpos + 2); /* XXX pos'n arith */
	}
		
	if (afterpos-startpos <= 0)		/* XXX pos arithmetic */
		t = self->def->EmptyStringToken;
	else {
		char buf[50];
		sprintf(buf, "/%d", self->def->nextlongstring);
		self->def->nextlongstring ++;
		t = toksym_TLocate(buf, self->def->prototype, self->constScope, &new);
		t->info.str = NULL;
		t->toknum = self->def->StringConTokNum;
		StorePos(t, startpos, afterpos);
	}
	return (t);
}


#define case4(c) case(c): case(c+1): case(c+2): case(c+3)
#define case6(c) case4(c): case(c+4): case(c+5)
#define case10(c) case4(c): case6(c+4)
#define case26(c) case10(c): case10(c+10): case6(c+20)

#define SaveAndRETURN(val) {*Pyylval = val;  \
	self->RecentPos[self->RecentIndex] = tokstart; \
	self->RecentLen[self->RecentIndex] = TellCh(self) - tokstart; \
	self->RecentIndex = self->RecentIndex + 1; \
	if (self->RecentIndex >= lex_RECENTSIZE) self->RecentIndex = 0;  \
	return (*Pyylval)->toknum;}

/* lex__NextToken(Pyylval)
	scan source text for the next token
	Set *Pyylval to the symbol for the token and return the token number

	stores the locations of most recent tokens in RecentPos/RecentLen
	recognizes:
		multi-thongs
		single special characters
		identifiers and reserved words
		integers and reals
		strings (syntax is C-like)
		long strings  delimited with // ... \\ on separate lines
	comments are  -- ... \n
	; is mapped to ,    
	[ and { are mapped to (   
	] and } are mapped to )
*/
	int
lex__NextToken(ClassID, Pyylval)
	struct classhdr *ClassID;
	struct toksym **Pyylval;
{
	register unsigned c;
	long tokstart;
	register struct lex *self = CurLex;
	unsigned char buf [101];
	unsigned char *bx, *endx;
	boolean IsPragmat;
	enum {Normal, HaveStar, Done} state;

	while ((c=CurCh(self)) != EOF)   /* repeat to skip characters */
	if (c >= '\177') {
		NextCh(self);	/* ignore DEL and above */
	}
	else {
		tokstart = TellCh(self);
	switch (c) {

	default:	/* the character represents itself */
defaultcase:
	{	register struct toksym *val;
		if (self->def->startsThong[c])
			val = doThong(self, c);
		else {
			val = self->def->charNode[c];
			NextCh(self);
		}
		SaveAndRETURN (val);
	}
	case10('0'):    /* digits - integer or real number */
		SaveAndRETURN (ParseNumber(self));

	case('_'):
	case26('a'):    /* letters - identifier or keyword */
	case26('A'):
		SaveAndRETURN (ParseWord(self));

	case('\"'):     /* "  -  parse a string */
		SaveAndRETURN (ParseShortString(self, '\"'));

	case('\''):     /* "  -  parse a C character constant */
		SaveAndRETURN (ParseShortString(self, '\''));

	case('\n'):	/* newline - test for long string */
		if (NextCh(self) == '/') {
			long startpos = TellCh(self);
			if (NextCh(self) == '/' && NextCh(self) =='\n') {
				SaveAndRETURN (ParseLongString(self));
			}
			else 
				/* did not find long string start */
				SeekCh(self, startpos);
		}
		break;

	case(' '):                  		/* ignore   white space        */
	case10('\000'): case10('\013'):	/*     and control characters  */
	case10('\025'): case('\037'):
		NextCh(self);
		break;

	case('-'):      /* '-'   comment,  minus sign, or thong  */
		if (NextCh(self) == '-' && self->def->comcode == DashDash) {
			/* it's a comment.  Skip to end of line */
			if ((c=NextCh(self)) == self->pragmatFlag
					&& self->pragmatFlag != '\0') {
				bx = buf;
				endx = bx + sizeof(buf) - 1;
				while ((c=NextCh(self)) != '\n'  &&  c != EOF)
					if (bx < endx) *bx++ = c;
				*bx = '\0';
				self->pragmatParser(self->pragmatRock, buf);
			}
			else if (c != '\n') 
				while ((c=NextCh(self)) != '\n'  &&  c != EOF) {}
			/* (leave \n as next char to read) */
			break;
		}
		else  {
			UngetCh(self);
			goto defaultcase;	/* still have c == '-' */
		}

	case('/'):      /* '/'  -  comment,  slash, or thong  */
			/* XXX we do not detect long string at beginning of file */
		if (NextCh(self) == '*' && self->def->comcode == SlashStar) {
			/* it's a comment.  Skip to end */
			bx = buf;
			if ((c=NextCh(self)) == self->pragmatFlag
					&& self->pragmatFlag != '\0') {
				endx = bx + sizeof(buf) - 1;
				IsPragmat = TRUE;
			}
			else {endx = bx;  IsPragmat = FALSE;}

			/* we have the first character */
			state = Normal;
			while (state != Done) {
				switch (c) {
				case EOF:  state = Done; 
					break;
				case '*': if (state == HaveStar  &&  bx < endx) 
						*bx++ = '*';  /* output deferred '*' */
					state = HaveStar;  /* defer output */
					break;
				case '/':  if (state == HaveStar) {
						state = Done;
						/* the NextCh below will 
						    move just beyond the '/'  */
						break;
					}
					/* DROP THRU */
				default:
					if (state == HaveStar) {
						if (bx < endx) 
							/* output deferred '*' */
							*bx++ = '*';
						state = Normal;
					}
					if (bx < endx) *bx++ = c;
					break;
				}
				if (c != EOF) 
					c = NextCh(self);
			}
			if (IsPragmat) {
				*bx = '\0';
				self->pragmatParser(self->pragmatRock, buf);
			}
			break;	/* out of switch */
		}
		else {
			UngetCh(self);	/* put back character after '/' */
			goto defaultcase;	/* still have c == '/' */
		}


	case('.'):    {  /*  '.'  -   itself ,  thong ,  or the beginning of a real value */
		unsigned long nextc;
		nextc = NextCh(self);
		UngetCh(self);
		if (isdigit(nextc)) 
			SaveAndRETURN (ParseNumber(self));
		goto defaultcase;
	    }
	}   /* end of  switch */
	}   /* end of  while if */

	/* we exit the loop only for EOF */
	tokstart = TellCh(self);
	SaveAndRETURN(self->def->EOFtoken);
}


/* lex__TokenUpTo(Pyylval, cend)
	fetch raw text from the current text.  Returns as a
	token in Pyylval.  Text is defined by the current
	position and the next occurance of cend.
*/
	int
lex__TokenUpTo(ClassID, Pyylval, cend)
	struct classhdr *ClassID;
	struct toksym **Pyylval;
	unsigned char cend;
{
	register unsigned c;
	long tokstart;
	char buf[50];
	struct toksym *t;
	register struct lex *self = CurLex;
	boolean new;

	tokstart = TellCh(self);

	/* scan upto cend */
	while ((c=CurCh(self)) != EOF && c != cend)
		NextCh(self);

	/* scan over cend */
	NextCh(self);
	
	sprintf(buf, "/%d", self->def->nextlongstring);
	self->def->nextlongstring ++;
	t = toksym_TLocate(buf, self->def->prototype, self->constScope, &new);
	t->info.str = NULL;
	t->toknum = self->def->StringConTokNum;
	StorePos(t, tokstart, TellCh(self));

	/* return token */
	*Pyylval = t;
	return (t->toknum);
}

