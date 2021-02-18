/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/fnote.ch,v 1.4 1991/09/12 19:51:55 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/fnote.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_celview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/fnote.ch,v 1.4 1991/09/12 19:51:55 bobg Exp $ ";
#endif

class fnote: text {
classprocedures:
	CloseAll(struct text *text);
	UpdateAll(struct text *text,long count) returns int;
	CopyAll(struct text *text,struct text *desttext,long count,boolean number) returns int;
	OpenAll(struct text *text);
	InitializeObject(struct fnote *self)returns boolean;
	InitializeClass() returns boolean; 
methods:
	Close(struct text *text);
	CopyNote(struct text *text,struct text * desttext,long count,boolean number) returns long;
	Open(struct text *text);
	IsOpen() returns boolean;
	addenv(struct text *txt,long pos);
	GetLocLength() returns long;
overrides:
	 Read (FILE *file, long id) returns long;
	ViewName() returns char *;
macromethods:
    GetLoc() ((self)->loc)
    SetLoc(long newValue) ( ((self)->loc) = (newValue) )
    GetOwnLoc() ((self)->ownloc)
    SetOwnLoc(long newValue) ( ((self)->ownloc) = (newValue) )
    GetParenttxt() ((self)->parenttext)
    GetEnv() ((self)->env)
    SetEnv(long newValue) ( ((self)->env) = (newValue) )
data:
   long loc,ownloc,notecount;
   struct text *parenttext;
   struct environment *env;
   struct style *vstyle , *hstyle;
   boolean open;
};

