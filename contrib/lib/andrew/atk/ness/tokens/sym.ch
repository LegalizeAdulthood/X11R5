/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/sym.ch,v 1.7 1991/09/12 19:44:52 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/sym.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *sym_H_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/sym.ch,v 1.7 1991/09/12 19:44:52 bobg Exp $";
#endif


/* SYM.H
 *
 * A sym object is an entry in a symbol table.  Each symbol has associated with it a uniqe
 * name and scope.  sym should be subclassed if any additional data is desired.
 *
 * History:
 *
 *	5-24-88 Fred Hansen:	original symbol table module created.
 *	6-29-88 George Baggott:	rewritten as an object.
 *	7-07-88 George Baggott:	no longer a subclass of node.
 *	7-13-88 George Baggott:	partitions axed.
 */
/*
 *    $Log: sym.ch,v $
Revision 1.7  1991/09/12  19:44:52  bobg
Update copyright notice

Revision 1.6  1989/08/23  16:12:59  wjh
?

Revision 1.5  89/06/01  10:09:38  wjh
revision for new securtiy system

Revision 1.1  88/10/21  10:42:15  wjh
Initial revision
 
 * Revision 1.0  88/06/23  12:15:00  gb17
 * Copied from /usr/andrew/lib/nullinset
 */


typedef long sym_ScopeType;

#define sym_GLOBAL ((sym_ScopeType) 0L)

class sym
{

overrides:

methods:

macromethods:

	GetName()		((self)->name)

	GetScope()		((self)->scope)
	SetScope(newScope)	((self)->scope = (newScope))

classprocedures:

	InitializeClass(/* struct classhdr *ClassID*/) returns boolean;
	InitializeObject(/* struct classhdr *ClassID, */ struct sym *self) returns boolean;
	FinalizeObject(/* struct classhdr *ClassID, */ struct sym *self);

	NewScope(sym_ScopeType enclosingScope) returns sym_ScopeType;
		/* creates a new scope in enclosingScope
		 */

	DestroyScope(sym_ScopeType scope);
		/* destroys all the symbols and scopes defined in the given scope
		 */

	ParentScope(sym_ScopeType scope) returns sym_ScopeType;
		/* returns the enclosing scope for the given scope 
		 */

	Define(unsigned char *name, struct sym *proto, sym_ScopeType scope) returns struct sym*;
		/* defines the symbol (name, scope).  If it already exists, NULL is
		 * returned.
		 */

	Undefine(unsigned char *name, sym_ScopeType scope) returns boolean;
		/* undefines (name, scope).  Returns TRUE if successful,
		 * false otherwise.
		 */

	Find(unsigned char *name, sym_ScopeType scope) returns struct sym*;
		/* attempts to find a match for (name, scope).  If one is found
		 * it is returned.  If not, then NULL is returned.
		 */

	Locate(unsigned char *name, struct sym *proto, sym_ScopeType scope, boolean *new)
	          returns struct sym*;
		/* attempts to find a match for (name, scope).  If one is found, 
		 * it is returned.  If not, (name, scope) is defined.
		 */

	FindAll(unsigned char *name, sym_ScopeType scope, long (*proc)(), long rock) returns long;
		/* finds all of name's matches that are defined in scope or any of scope's
		 * descendent scopes.  For each match, proc is called with sym and rock.
		 * proc should be declared as:
		 *
		 * 	long proc(sym, rock);
		 *	sym_partition part;
		 *	struct sym *sym;
		 *	long rock;
		 *
		 * if a call to proc returns a value other than NULL, the search
		 * is terminated, and FindAll will return that value.  Otherwise,
		 * FindAll will return NULL.
		 */

	printtable();

data:
	unsigned char *name;		/* the name of the symbol */
	long scope;		/* the scope in which the symbol is defined */
	struct sym *next;		/* used for handling collisions in hash table */
};

