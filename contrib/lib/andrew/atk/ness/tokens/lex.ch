/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/lex.ch,v 1.9 1991/09/12 19:44:46 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/lex.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *lex_H_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/lex.ch,v 1.9 1991/09/12 19:44:46 bobg Exp $";
#endif

/*
lex.H

	lex data object

	provides lexical analysis

	Tailored for the Ness project, but can be extended for
	other similar grammars.

	NextToken may call yyerror(msg).  In turn, it may call
	lex_RecentPosition(0, &len) to get the location of the erroneous token.
*/
/*
 *    $Log: lex.ch,v $
Revision 1.9  1991/09/12  19:44:46  bobg
Update copyright notice

Revision 1.8  1989/09/10  17:36:51  wjh
added SetPragmatParser

Revision 1.7  89/08/23  16:12:35  wjh
TokenUpTo & GetSource

Revision 1.6  89/06/01  10:09:29  wjh
revision for new securtiy system

Revision 1.2  88/12/07  22:24:21  wjh
added ness_ParseNumber()

Revision 1.1  88/10/21  10:39:59  wjh
Initial revision
 
 * Revision 1.0  88/07/13  09:35:59  wjh
 * Copied from /usr/andrew/lib/nullinset
 */

#include "toksym.ih"

#define lex_RECENTSIZE 10


/* XXX    KLUDGE ALERT   redefine yylex */
#undef yylex
#define yylex()  lex_NextToken(&yylval)

class lex
{

methods:

	SetSource(/* struct lex *self, */ struct text *text, long loc, long len, 
				toksym_scopeType scope, 
				toksym_scopeType constscope);
		/* specify a source text to process 
		if scope is toksym_GLOBAL, then a new source is assumed
			and a new constant pool is created
		otherwise it is assumed it is doing a recompilation within
			children of the named scope (as would be done for
			recompilation of a single function within a script.
			In this case the scope for the old compilation of
			the function should have been discarded by the caller.)
		similarly, constscope may be toksym_GLOBAL to generate a new
			constant scope or it may be a specific scope

		The caller is responsible for Destroying scopes created by passing
		toksym_GLOBAL.

		SetSource automatically does a SetCurrent()
		*/

	SetCurrent(/* struct lex *self */);
		/* establish self as the lex for compiles (this function 
		usually need not be called because SetSource calls it) */

	SetPragmatParser(/* struct lex *self, */  char flag,  void (* parser)(), long *rock);
		/* when an open comment delimiter is followed by the 'flag' character,
		it is deemed to be a pragmat.  The remainder of the comment is passed
		to the 'parser' function as follows:
			parser(rock, comment-contents)
		the comment-contents are the first 100 characters of the
		comment following the flag character.  */


macromethods:

	SetScope(/* struct lex *self, */ long scope) (self->currentScope = (scope))
		/* set the scope.  Could be used to do a partial source 
		compilation within a scope created earlier */
	GetScope(/* struct lex *self */) (self->currentScope)
		/* return current scope */
	SetConstScope(/* struct lex *self, */ long scope) (self->constScope = (scope))
		/* set the constant scope (normally a distinct constant scope is
		generated for each call to SetSource) */
	GetConstScope(/* struct lex *self */) (self->constScope)
		/* return the constant scope */
	GetGrammarScope(/* struct lex *self */) (self->def->grammarScope)
		/* the scope to be used as parent scope for the
			scope of any given compile */
	GetPrototype(/* struct lex *self */) (self->prototype)
		/* the prototypical struct to be used in sym_Locate */
	GetSource(/* struct lex *self */) ((self)->source)
		/* the source text being parsed */

classprocedures:

	InitializeClass(/* struct classhdr *ClassID*/) returns boolean;
	InitializeObject(/* struct classhdr *ClassID, */ struct lex *self) returns boolean;
		/* use Create instead */
	FinalizeObject(/* struct classhdr *ClassID, */ struct lex *self);

	Create(/* struct classhdr *ClassID, */ struct lexdef *def)
					returns struct lex *;
		/* create an instance of a lexical analyzer definition
			one is needed for each source compilation */

	RecentPosition(/* struct classhdr *ClassID, */ long index, long *len) returns long;
		/* for token 'index', set len to length and return position.
		index = 0 is the most recent token,
		its predecessors are indexed with negative numbers: 
			-1 -2 ... -lex_RECENTSIZE+1*/

	Repeat(/* struct classhdr *ClassID, */   long index);
		/* backup and repeat tokens starting with the index'th 
		    most recent token, where index is as for RecentPosition */

	RecentIndent(/* struct classhdr *ClassID, */   long index)
				returns long;
		/* report the indentation of the 'index'th most recent token
		    a token preceded by anything other than white space 
		    is reported as having indentation 999 */

	NextToken(/* struct classhdr *ClassID, */ struct toksym **Pyylval) returns int;
		/* scans the Current source text for the next token;
		   stores a pointer to a toksym for it into yylval
		   and returns the token number from the toksym */
	GetCurrent(/* struct classhdr *ClassID */) returns struct lex *;
		/* return the current lex */

	TokenUpTo(/* struct classhdr *ClassID, */ struct toksym **Pyylval, unsigned char cend)
				returns int;
		/* Constructs a token of type long string from the text
		   starting at the current position and extending to the first
		   subsequent occurrence of 'cend'.  Stores a pointer to this
		   token into *Pyylval and returns the token number.  */

	ParseNumber(/* struct classhdr *ClassID, */ struct text *text,
			unsigned long pos, unsigned long *len, 
			unsigned long *intval, double *dblval) 
				returns unsigned long;
		/* Parses a number from text, starting at pos, which must 
		select a character valid at the beginning of a number. 
		Sets len to length, intval to integer value, and dblval 
		to real value if the number was real.  
		Returns success status:
			0-error,  1-integer,   2- real.  */

data:
	struct lexdef *def;

	toksym_scopeType constScope;	/* per compilation */
	toksym_scopeType currentScope;	/* per scope */

	unsigned long curchar;
	struct text *source;
	long curpos, endpos;
	long RecentPos[lex_RECENTSIZE], RecentLen[lex_RECENTSIZE];
	short RecentIndex;  

	char pragmatFlag;
	void (*pragmatParser)();
	long *pragmatRock;

};

