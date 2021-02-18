/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/glist/RCS/glist.ch,v 2.7 1991/09/12 19:40:21 bobg Exp $ */
/* $ACIS:glist.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/glist/RCS/glist.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidglist_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/glist/RCS/glist.ch,v 2.7 1991/09/12 19:40:21 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* List object
 * for Graph Editor
 */

struct glistelt {
    char * this;
    struct glistelt *next;
};

class glist {

classprocedures:
    Create(procedure Destroy) returns struct glist *;
    FinalizeObject(struct glist *self);
    InitializeObject(struct glist *self) returns boolean;    
    Copy(struct glist *dest, struct glist *source);
methods:
    Insert(char * element) returns boolean;
    Push(char *element) returns boolean;
    Pop() returns char *;
    InsertSorted(char *element, procedure greater) returns boolean;
    Sort(procedure greater) returns boolean;
    InsertUnique(char * element) returns boolean;
    Delete(char * element, boolean destroy) returns boolean;
    Find(procedure filter, char * rock) returns char *; /* filter is called: filter(char * element) */
    Enumerate(void (*proc)(), unsigned long rock);
    Contains(char * element) returns boolean;
    Clear(boolean destroy);
macromethods:
    First() ((self)->head->this)
    Last() ((self)->tail->this)
    Size() ((self)->size)
data:
    struct glistelt *head;
    struct glistelt *tail;
    long size;
    procedure DestroyProc;
};
