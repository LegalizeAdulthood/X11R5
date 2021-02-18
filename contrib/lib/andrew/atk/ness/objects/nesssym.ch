/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nesssym.ch,v 1.6 1991/09/12 19:44:21 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nesssym.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nesssym.ch,v 1.6 1991/09/12 19:44:21 bobg Exp $";
#endif
/*
 *    $Log: nesssym.ch,v $
Revision 1.6  1991/09/12  19:44:21  bobg
Update copyright notice

Revision 1.5  1989/09/03  22:46:25  wjh
newness

Revision 1.4  89/06/01  15:58:12  wjh
campus release version

Revision 1.1  88/10/21  11:00:48  wjh
Initial revision
 
 * Revision 1.0  88/07/14  08:37:00  WJHansen
 * Copied from toksym.H
 */

/*****************************************************************************\
\*****************************************************************************/


/* nesssym.H
 *
 * A nesssym object is a subclass of toksym.  Its adds these fields:
 *	
 *	next	- for linking syms in a list
 *	
 *
 */

#include "toksym.ih"

typedef long nesssym_scopeType;
#define nesssym_GLOBAL toksym_GLOBAL

typedef unsigned long Texpr;

class nesssym : toksym
{

overrides:

macros:

	NNewScope(enclosingScope) ((nesssym_scopeType)  \
			toksym_TNewScope ((toksym_scopeType)(enclosingScope)))
	NDestroyScope(scope) (toksym_TDestroyScope((toksym_scopeType)(scope)))
	NParentScope(scope) ((nesssym_scopeType)  \
			toksym_TParentScope((toksym_scopeType)(scope)))
	NDefine(name, proto, scope) ((struct nesssym *)toksym_TDefine(name, proto, scope))
	NUndefine(name, scope) (toksym_TUndefine(name, scope))
	NFind(name, scope) ((struct nesssym *)toksym_TFind(name, scope))
	NLocate(name, proto, scope, new) \
			((struct nesssym *)toksym_TLocate(name, proto, scope, new))
	NFindAll(name, scope, proc, rock) (toksym_TFindAll(name, scope, proc, rock))

macromethods:

	NGetName()	((unsigned char *)(self)->header.sym.name)
	NGetScope()	((nesssym_scopeType)(self)->header.sym.scope)
	NGetINode(type)	((struct type *)((self)->header.toksym.info.node))
	NGetInfo(type)	((type)((self)->header.toksym.info.intval))
	NSetINode(type, val)	((self)->header.toksym.info.node  \
					= (struct node *)(struct type *)(val))
	NSetInfo(type, val)	((self)->header.toksym.info.intval = (long)(type)(val))

classprocedures:

	InitializeObject(/* struct classhdr *ClassID, */ struct nesssym *self) returns boolean;
	FinalizeObject(/* struct classhdr *ClassID, */ struct nesssym *self);

data:

	struct nesssym *next;
	union {		struct ness *ness;  
			struct nesssym *nesssym;
			struct node *node; 
		} parent;
	Texpr type;
	long flags;	/* see compdefs.hn */

};

