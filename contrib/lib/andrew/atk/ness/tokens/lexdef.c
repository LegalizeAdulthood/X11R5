/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/lexdef.c,v 1.4 1989/06/01 10:09:08 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/lexdef.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/lexdef.c,v 1.4 1989/06/01 10:09:08 wjh Exp $";
#endif

/* lexdef.c		

	Code for the lexdef data object
*/
/*
 *    $Log: lexdef.c,v $
 * Revision 1.4  1989/06/01  10:09:08  wjh
 * revision for new securtiy system
 *
 * Revision 1.1  88/10/21  10:41:58  wjh
 * Initial revision
 *  
 * Revision 1.0  88/07/13  09:35:59  wjh
 * Copied from /usr/andrew/lib/nullinset
 */

#include <ctype.h>

#include <class.h>
#include <lexdef.eh>		/* must follow toksym.ih */
#include <text.ih>

	boolean
lexdef__InitializeClass(ClassID)
	struct classhdr *ClassID;
{
	return TRUE;
}

	boolean
lexdef__InitializeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct lexdef  *self;
{
	self->grammarScope = toksym_GLOBAL;
	self->thongSym = NULL;
	self->prototype = NULL;
	return TRUE;
}

	void 
lexdef__FinalizeObject(ClassID, self)
	struct classhdr *ClassID;
	register struct lexdef  *self;
{
	if (self->grammarScope != toksym_GLOBAL)
		toksym_TDestroyScope(self->grammarScope);
	if (self->thongSym != NULL)
		free (self->thongSym);
	if (self->prototype != NULL)
		toksym_Destroy(self->prototype);
}


/* initrange(self, proto, lo, hi)
	initialize charNode for a range of special chars from lo to hi
*/
	static void
initrange (self, proto, lo, hi)
	register struct lexdef *self;
	struct toksym *proto;
	register unsigned char lo, hi;
{
	register unsigned char c;
	static unsigned char s[2] = "0";
	for (c = lo; c <= hi;  c++) {
		s[0] = c;
		self->charNode[c] = toksym_TDefine(s, proto, self->grammarScope);
		/* the toknum for a character is itself */
		self->charNode[c]->toknum = c;
	}
}


	struct lexdef *
lexdef__Create(ClassID, yyerror, proto, com, foldCase, lexdesc)
	struct classhdr *ClassID;
	void (*yyerror)();
	struct toksym *proto;
	enum commentKind com;
	boolean foldCase;
	struct lexdescription *lexdesc;
{
	register struct lexdef *self;
	struct toksym *toksymproto;
	unsigned char **s;
	short *ptTok;
	struct toksym **ptSym;
	unsigned char c;
	long cnt;

	self = lexdef_New();
	toksymproto = toksym_New();

	self->yyerror = yyerror;
	self->prototype = proto;
	self->comcode = com;
	self->foldCase = foldCase;
	self->IdTokNum = lexdesc->idtoknum;
	self->IntConTokNum = lexdesc->inttoknum;
	self->RealConTokNum = lexdesc->realtoknum;
	self->StringConTokNum = lexdesc->strtoknum;

	self->grammarScope = toksym_TNewScope(toksym_GLOBAL);

	self->EOFtoken = toksym_TDefine("*EOF*", toksymproto, self->grammarScope);

	self->EOFtoken->toknum = 0;
	self->EOFtoken->loc = 0;
	self->EOFtoken->len = 0;
	self->EmptyStringToken = toksym_TDefine("\"", proto,
				 self->grammarScope);
	self->EmptyStringToken->toknum = self->StringConTokNum;
	self->EmptyStringToken->loc = 0;
	self->EmptyStringToken->len = 0;

	self->nextlongstring = 1;

	/* create symbols for all special characters */
	initrange(self, toksymproto, ' '+1, '\"'-1);
	initrange(self, toksymproto, '\"'+1, '0'-1);
	initrange(self, toksymproto, '9'+1, 'A'-1);
	initrange(self, toksymproto, 'Z'+1, 'a'-1);
	initrange(self, toksymproto, 'z'+1, '\177'-1);

	for (ptTok = lexdesc->restoks, s = lexdesc->reswords; *s != NULL; s++, ptTok++) {
		register struct toksym *symbol = 
			toksym_TDefine(*s, toksymproto, self->grammarScope);
		symbol->toknum = *ptTok;
	}

	/* clear the startsThong table */
	for (c = '\0'; c <= '\177'; c++)
		self->startsThong[c] = FALSE;

	/* allocate space for pointers to syms for thongs */
	self->thongTable = lexdesc->thongs;
	for (cnt = 0, s = lexdesc->thongs; *s != NULL; s++, cnt++)  {}
	self->thongSym = (struct toksym **)calloc(cnt, sizeof(struct toksym *));

	/* make a sym for each thong */
	ptSym = self->thongSym;
	ptTok = lexdesc->thongtoks;
	for (s = lexdesc->thongs; *s != NULL; s++, ptTok++, ptSym++)  {
		*ptSym = toksym_TDefine(*s, toksymproto, self->grammarScope);
		(*ptSym)->toknum = *ptTok;
		self->startsThong[**s] = TRUE;
	}

	toksym_Destroy(toksymproto);
	return self;
}

