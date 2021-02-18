/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/lexdef.ch,v 1.5 1989/06/01 10:09:34 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/lexdef.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *lexdef_H_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/lexdef.ch,v 1.5 1989/06/01 10:09:34 wjh Exp $";
#endif

/*
lexdef.H

	lexdef data object

	defines a lexical stream
	serves as parent to a collection of lex objects, 
		one for each compilation

	Tailored for the Ness project, but can be extended for
	other similar grammars.

	Typically, there is only one lexdef per compiler.
*/
/*
 *    $Log: lexdef.ch,v $
Revision 1.5  1989/06/01  10:09:34  wjh
revision for new securtiy system

Revision 1.1  88/10/21  10:41:26  wjh
Initial revision
 
 * Revision 1.0  88/09/05  12:22:00  wjh
 * Extracted from original lex.H
 */

#include "toksym.ih"

struct lexdescription {
	unsigned char **reswords;
	short *restoks;
	unsigned char **thongs;
	short *thongtoks;
	short idtoknum, inttoknum, realtoknum, strtoknum;
};

enum commentKind {
	SlashStar,
	DashDash,
};

class lexdef
{

methods:

macromethods:

	GetGrammarScope(/* struct lexdef *self */) (self->grammarScope)
		/* the scope to be used as parent scope for the
			scope of any given compile */
	GetPrototype(/* struct lexdef *self */) (self->prototype)
		/* the prototypical struct to be used in sym_Locate */

classprocedures:

	InitializeClass(/* struct classhdr *ClassID*/) returns boolean;
	InitializeObject(/* struct classhdr *ClassID, */ struct lexdef *self) returns boolean;
		/* use Create instead */
	FinalizeObject(/* struct classhdr *ClassID, */ struct lexdef *self);

	Create(/* struct classhdr *ClassID, */ void (*yyerror)(), 
			struct toksym *proto, 
			enum commentKind com, boolean foldCase,
			struct lexdescription *lexdesc) 
					returns struct lexdef *;
		/* create an instance of a lexical analyzer for a new grammar
			proto is an object of whatever type is to be 
			   stored in yylval, it should be a subclass of toksym
			com determines what comment convention is used
			if foldCase is TRUE, upper case in identifiers 
			   is folded to lower
			lexdesc is the value returned by <grammar>_lesdesc()
			   as constructed in the .y file by buildy
		*/
data:
	/* grammar description */
	enum commentKind comcode;
	boolean foldCase;
	struct toksym *prototype;	/* subclass of sym to be created */
	short IdTokNum, IntConTokNum, RealConTokNum, StringConTokNum;
	void (*yyerror)();

	/* created for the grammar */
	struct toksym *charNode[128];  /* symbol for each special character */
	struct toksym *EOFtoken;
	struct toksym *EmptyStringToken;
	unsigned char **thongTable;
	struct toksym **thongSym;
	boolean startsThong[128];	/* TRUE if the char starts a thong */
	long nextlongstring;
	toksym_scopeType grammarScope;	/* per grammar */

};

