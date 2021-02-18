/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/toksym.ch,v 1.8 1991/09/12 19:44:56 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/toksym.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *toksym_H_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/toksym.ch,v 1.8 1991/09/12 19:44:56 bobg Exp $";
#endif

/* toksym.H
 *
 * A toksym object is a subclass of sym.  Its adds four fields:
 *	toknum	-  the token number for compiler
 *	loc, len 	-  position of the token in the source stream
 *	info	-  type dependant information
 *
 */

/*
 *    $Log: toksym.ch,v $
Revision 1.8  1991/09/12  19:44:56  bobg
Update copyright notice

Revision 1.7  1991/01/14  17:23:26  wjh
added a routine to extract the symbol value from the original text

Revision 1.6  89/08/23  16:11:49  wjh
andrewos

Revision 1.5  89/06/01  10:09:43  wjh
revision for new securtiy system

Revision 1.1  88/10/21  10:43:10  wjh
Initial revision
 
 * Revision 1.0  88/07/13  20:50:00  WJHansen
 * Copied from sym.H
 */


#include "sym.ih"	
/* This include generates an error message claiming that sym.ih is included twice.  
   The conflict is with the include generated by class as part of toksym.ih, but
   the automatically generated include does NOT define the routines. */


/* the following doesn't work !?!?! */
/*	typedef sym_scopeType  toksym_scopeType;   */

typedef  long   toksym_scopeType;

#define toksym_GLOBAL sym_GLOBAL


union infotype {
	long intval;
	double *realval;
	char *str;
	long (*proc)();
	struct object *obj;
	struct node *node;
};


class toksym : sym
{
methods:

	ToC(/*struct toksym *self, */ struct text *t, char *buf, long maxlen) returns char *;
		/* copies the token from the text to the buffer, up to maxlen bytes
			returns buf */

macros:

/* the initial T in each of these procedure names is to get around a bug in the class system */

	TNewScope(enclosingScope) ((toksym_scopeType)sym_NewScope(enclosingScope))
	TDestroyScope(scope) (sym_DestroyScope(scope))
	TParentScope(scope) ((toksym_scopeType)sym_ParentScope(scope))
	TDefine(name, proto, scope) ((struct toksym *)sym_Define(name, proto, scope))
	TUndefine(name, scope) (sym_Undefine(name, scope))
	TFind(name, scope) ((struct toksym *)sym_Find(name, scope))
	TLocate(name, proto, scope, new) ((struct toksym *)sym_Locate(name, proto, scope, new))
	TFindAll(name, scope, proc, rock) (sym_FindAll(name, scope, proc, rock))

macromethods:

	TGetName()	((self)->header.sym.name)
	TGetScope()	((toksym_scopeType)(self)->header.sym.scope)
	GetLoc()		((self)->loc)
	GetLen()		((self)->len)

classprocedures:

	InitializeObject(/* struct classhdr *ClassID, */ struct toksym *self) returns boolean;
	FinalizeObject(/* struct classhdr *ClassID, */ struct toksym *self);

data:

	short toknum;	/* token code for yacc */
	long loc, len;	/* position of token in source */
	union infotype info;	/* type dependent */

};

