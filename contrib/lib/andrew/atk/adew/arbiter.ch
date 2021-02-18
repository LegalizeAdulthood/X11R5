/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/arbiter.ch,v 2.6 1991/09/12 19:19:01 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/arbiter.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_arbiter_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/arbiter.ch,v 2.6 1991/09/12 19:19:01 bobg Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class arbiter : cel {
macromethods:
	GetFirst() ((self)->first)
overrides:
 	Read (FILE *file, long id) returns long;
	ReadFile(FILE *file) returns long;
methods:
	DeclareRead(struct cel *cel) returns FILE *;
	FindChildCelByName(char *name) returns struct cel *;
	FindChildObjectByName(char *name) returns struct dataobject *;
	ReadObjects();
classprocedures:
	GetMaster() returns struct arbiter *; 
	SetMaster(struct thisobject *newmaster) returns struct arbiter *; 
	InitializeClass() returns boolean;
	InitializeObject(struct thisobject *self) returns boolean;
data:
	struct cel *first;
};
