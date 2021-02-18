/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/dataobj.ch,v 2.9 1991/09/12 19:21:34 bobg Exp $ */
/* $ACIS:dataobj.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/dataobj.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsiddataobject_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/dataobj.ch,v 2.9 1991/09/12 19:21:34 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


#include <atom.ih>
#include <namespc.ih>

struct property {
    struct atom * type;
    long data;
};

class dataobject[dataobj]: observable[observe] {
methods:
    Read (FILE *file, long id) returns long;
    Write (FILE *file, long writeid, int level) returns long;
    GetModified() returns long;
    SetModified();
    ViewName() returns char *;
    SetAttributes(struct attributes *attributes);
    Put( struct atom * property, struct atom * type, long rock );
    Get( struct atom * property, struct atom ** type, long * rock )
      returns short;
    ListCurrentViews(struct view **array,int size) returns int;
    RestoreModified(long oldmodified);
macromethods:
    UniqueID() ((long)(self))
    GetWriteID() ( (self)->writeID )
    SetWriteID(long newValue) ( ((self)->writeID) = (newValue) )
    GetID()      ( (self)->id )
    SetID(long newValue)      ( ((self)->id) = (newValue) )
data:
    long id;
    long writeID;
    long modified;
    struct namespace * properties;
};

#define dataobject_UNDEFINEDID -1

/* return values from Read */
#define	dataobject_NOREADERROR	0
#define	dataobject_PREMATUREEOF	1
#define	dataobject_NOTBE2DATASTREAM 2 /* backward compatibility */
#define	dataobject_NOTATKDATASTREAM 2 /* preferred version */
#define dataobject_MISSINGENDDATAMARKER 3
#define	dataobject_OBJECTCREATIONFAILED	4
#define dataobject_BADFORMAT 5
