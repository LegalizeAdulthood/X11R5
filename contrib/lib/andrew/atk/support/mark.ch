/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/mark.ch,v 2.5 1991/09/12 19:48:55 bobg Exp $ */
/* $ACIS:mark.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/mark.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidmark_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/mark.ch,v 2.5 1991/09/12 19:48:55 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#define mark_VERSION 1

class mark {
methods:
    SetStyle(boolean beginning, boolean ending);
    UpdateMarks(long pos, long size);
classprocedures:
    NewWithStyle(boolean beginning, boolean ending) returns struct mark *;
    Allocate() returns struct mark *;
    Deallocate(struct mark *self);

macromethods:

	GetObject()	((self)->object)
	GetPos() 	((self)->pos)
	GetLength()	((self)->length)
	GetEndPos()	((self)->pos + (self)->length)
	IncludeBeginning()	((self)->includeBeginning)
	IncludeEnding()	((self)->includeEnding)
	GetModified()	((self)->modified)
	ObjectFree()	((self)->objectFree)
	GetNext()	((self)->next)

	SetObject(obj)	((self)->object = (struct basicobject *)(obj))
	SetPos(position)	((self)->pos = position)
	SetLength(len)	((self)->length = len)
	SetModified(val)	((self)->modified = val)
	SetObjectFree(v)	((self)->objectFree = v)
	SetNext(n)	((self)->next = n)

data:
    struct basicobject *object;
    struct mark *next;
    long pos;
    long length;
    char modified;
    char objectFree;
    char includeBeginning;
    char includeEnding;
};
