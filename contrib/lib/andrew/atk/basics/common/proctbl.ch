/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/proctbl.ch,v 2.12 1991/09/12 19:23:18 bobg Exp $ */
/* $ACIS:proctbl.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/proctbl.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidproctable_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/proctbl.ch,v 2.12 1991/09/12 19:23:18 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* proctbl.ch -- A module that manages a mapping from strings to procedure pointers.
December, 1986 */

enum proctable_type {
	proctable_Void,		/* does not return a type */
	proctable_Boolean,  	/* class.c:boolean */
	proctable_Char,		/* 8 bit unsigned character */
	proctable_Short,		/* 16 bit integer */	
	proctable_Long,		/* 32 bit integer */
	proctable_Double,		/* double floating value */
	proctable_Object,		/* pointer to an object */
	proctable_NessMarker,	/* object of type nessmark */
	proctable_StaticString,	/* returns a pointer to a string which must
					NOT be freed by the recipient */
	proctable_DisposeString	/* returns a pointer to a string which 
					MUST be freed by recipient */
};

struct proctable_Entry {
	struct proctable_Entry *hnext;  /* hash table link */
	char *name;			/* name of the function */
	int	(*proc)();		/* pointer to it */
	struct classinfo *type;	/* type of object to apply it to */
	char *module;		/* what to dynamically load to get it */
	char *doc;			/* prose description of its function */
	enum proctable_type returntype;
};

struct proctable_Description {
	char *name;			/* name of the function */
	int	(*proc)();		/* pointer to it */
	struct classinfo *type;	/* type of object to apply it to */
	char *doc;			/* prose description of its function */
	char *module;		/* what to dynamically load to get it */
};
struct proctable_DescriptionWithType {
	char *name;			/* name of the function */
	int	(*proc)();		/* pointer to it */
	struct classinfo *type;	/* type of object to apply it to */
	char *doc;			/* prose description of its function */
	char *module;		/* what to dynamically load to get it */
	enum proctable_type returntype;
};


package proctable[proctbl] {

macros:

	Defined(self) 		((self)->proc != (int (*)()) 0)
	GetName(self)		((self)->name)
	GetFunction(self)		((self)->proc)
	GetType(self)		((self)->type)
	GetReturnType(self)	((self)->returntype)
	GetModule(self)		((self)->module)
	GetDocumentation(self) 	((self)->doc)


classprocedures:
	InitializeClass() returns boolean;	/* called automagically */
	/* Only the name field is required.  Later calls to DefineProc will update an existing entry. */
	DefineProc(char *name, procedure proc, struct classinfo *type, char *module, char *doc) returns struct proctable_Entry *;
	DefineProcs(struct proctable_Description *procs);
	DefineTypedProc(char *name, procedure proc, struct classinfo *type,
			char *module, char *doc, enum proctable_type returntype) 
			returns struct proctable_Entry *;
	DefineProcsWithTypes(struct proctable_DescriptionWithType *procs);
	Lookup(char *name) returns struct proctable_Entry *;
	Enumerate(procedure proc, char *procdata);
	ForceLoaded(struct proctable_Entry *pe);
};
