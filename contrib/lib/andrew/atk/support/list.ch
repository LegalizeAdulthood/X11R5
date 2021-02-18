/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/list.ch,v 1.5 1991/09/12 19:48:51 bobg Exp $ */
/* $ACIS$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/list.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidlist_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/list.ch,v 1.5 1991/09/12 19:48:51 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 * Generic Linked List / Stack / Queue datatype
 *
 * Although char *'s are used here, pointers to any type of object
 * or structure may be used with minimal casting.
 */

struct list_Entry {
    struct list_Entry *next;
    char *data;
};

class list {
classprocedures:
    InitializeObject(struct list *self) returns boolean;
    FinalizeObject(struct list *self);

    /* Copies dst to the end of src.   Src is left alone. */
    Merge(struct list *dst, struct list *src);

    /* Tacks src list onto the end of dst. Src is emptied. */
    Append(struct list *dst, struct list *src);

methods:
    InsertFront(char *data);
    InsertEnd(char *data);
    InsertUnique(char *data) returns boolean;
    InsertSorted(char *data, procedure compare);

    RemoveFront() returns char *;

    Delete(char *data) returns boolean;
    Member(char *data) returns boolean;

    Sort(procedure greater) returns boolean;

    /* Enumerate proc receives (data, rock).  If the */
    /* proc returns FALSE, enumerate terminates and */
    /* returns the piece of data responsible. */

    Enumerate(procedure proc, char *rock) returns char *;

    Clear();

macromethods:
    Enqueue(char *data) (list_InsertEnd(self, data))
    Dequeue() (list_RemoveFront(self))

    Push(char *data) (list_InsertFront(self, data))
    Pop() (list_RemoveFront(self))

    Size() ((self)->size)

    First() ((self)->size == 0 ? NULL : (self)->head->data)
    Last() ((self)->size == 0 ? NULL : (self)->tail->data)

    /* Way to traverse the list from head to tail: */
    /* Start and Advance return NULL at end of list. */
    /* Data returns the data at the current position, NULL at end */

    Start() ((self)->trav = (self)->head)
    Advance() ((self)->trav == NULL ? NULL : \
              ((self)->trav = (self)->trav->next))
    Data() ((self)->trav == NULL ? NULL : \
              (self)->trav->data)

data:
    struct list_Entry *head;
    struct list_Entry *tail;
    long size;

    struct list_Entry *trav;
};
