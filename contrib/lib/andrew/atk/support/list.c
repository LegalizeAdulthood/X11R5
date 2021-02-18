/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/list.c,v 1.6 1991/09/12 16:31:33 bobg Exp $ */
/* $ACIS$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/list.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/list.c,v 1.6 1991/09/12 16:31:33 bobg Exp $";
#endif /* lint */

/*
 * Generic Linked List / Stack / Queue datatype
 *
 * Although char *'s are used for data, pointers to any type
 * of object or structure may be used.  Minimal casting may
 * be required by some compilers.
 */

#include <class.h>
#include "list.eh"

#define new() \
  (struct list_Entry *) malloc(sizeof (struct list_Entry))

/*
 * Class procedures
 */

boolean list__InitializeObject(classID, self)
struct classheader *classID;
struct list *self;
{
    self->head = NULL;
    self->tail = NULL;
    self->size = 0;

    return TRUE;
}

void list__FinalizeObject(classID, self)
struct classheader *classID;
struct list *self;
{
    list_Clear(self);
}

static int CopyEntry(value, dst)
char *value;
struct list *dst;
{
    list_InsertEnd(dst, value);

    return TRUE;
}

void list__Merge(classID, dst, src)
struct classheader *classID;
struct list *dst, *src;
{
    list_Enumerate(src, CopyEntry, (char *) dst);
}

void list__Append(classID, dst, src)
struct classheader *classID;
struct list *dst, *src;
{
    if (dst->size == 0) {
        dst->head = src->head;
        dst->tail = src->tail;
        dst->size = src->size;
    } else {
        dst->tail->next = src->head;
        dst->size += src->size;
    }

    src->head = NULL;
    src->tail = NULL;
    src->size = 0;
}

/*
 * Methods
 */

void list__InsertFront(self, data)
struct list *self;
char *data;
{
    register struct list_Entry *p;

    p = new();
    p->data = data;
    p->next = self->head;
    self->head = p;
    self->size++;
}

void list__InsertEnd(self, data)
struct list *self;
char *data;
{
    register struct list_Entry *p;

    p = new();
    p->data = data;
    p->next = NULL;

    if (self->size == 0)
        self->head = p;
    else
        self->tail->next = p;
    self->tail = p;
    self->size++;
}

boolean list__InsertUnique(self, data)
struct list *self;
char *data;
{
    if (list_Member(self, data))
        return FALSE;

    list_InsertEnd(self, data);
    return TRUE;
}

/* Compare takes two arguments, data1 and data2. */
/* It should return a positive number if data1 is greater than */
/* data2, a negative number if data1 is less than data2, */
/* or zero if they are equal. */

void list__InsertSorted(self, data, compare)
struct list *self;
char *data;
procedure compare;
{
    register struct list_Entry *n, *p, **pp;

    n = new();
    n->data = data;

    pp = &self->head;
    for (p = *pp; p != NULL &&
      (*compare)(data, p->data) > 0; p = *pp)
        pp = &p->next;

    if (p == NULL)
        self->tail = n;

    n->next = *pp;
    *pp = n;

    self->size++;
}

char *list__RemoveFront(self)
struct list *self;
{
    register struct list_Entry *p = self->head;
    register char *data;

    if (self->size == 0)
	return NULL;

    if (--self->size == 0)
        self->head = self->tail = NULL;
    else
        self->head = p->next;

    data = p->data;
    free(p);
    return data;
}

/*
 * Find and delete
 */

boolean list__Delete(self, data)
struct list *self;
char *data;
{
    register struct list_Entry *p, **pp;

    pp = &self->head;
    for (p = *pp; p != NULL && p->data != data; p = *pp)
        pp = &p->next;

    if (p == NULL)
        return FALSE;

    if (self->tail == p)
        self->tail = *pp;

    *pp = p->next;
    free(p);

    self->size--;
    return TRUE;
}

boolean list__Member(self, data)
struct list *self;
char *data;
{
    register struct list_Entry *p;

    for (p = self->head; p != NULL; p = p->next)
        if (p->data == data)
              return TRUE;

    return FALSE;
}

/*
 * Simple sort,  O(N^2)
 *
 * Compare routine is same as that taken by InsertSorted.
 */

struct arg { struct list *newlist; procedure compare; };

static int MoveNew(data, ap)
char *data;
struct arg *ap;
{
    list_InsertSorted(ap->newlist, data, ap->compare);
    return FALSE;
}

boolean list__Sort(self, compare)
struct list *self;
procedure compare;
{
    struct arg a;

    /* Create new list, inserting old elements in sorted order */

    a.newlist = list_New();
    a.compare = compare;

    list_Enumerate(self, MoveNew, (char *) &a);

    {
        /* Swap new and old lists */
        register struct list_Entry *phead = self->head;
        register struct list_Entry *ptail = self->tail;
        register int psize = self->size;
        self->head = a.newlist->head;
        self->tail = a.newlist->tail;
        self->size = a.newlist->size;
        a.newlist->head = phead;
        a.newlist->tail = ptail;
        a.newlist->size = psize;
    }

    /* Destroy old list */

    list_Destroy(a.newlist);
    return TRUE;
}

/*
 * Enumerate runs proc on each entry in a list.
 * If proc returns FALSE, the enumeration terminates
 * and the piece of data responsible is returned.
 * Otherwise, the enumeration completes and NULL is returned.
 */

char *list__Enumerate(self, proc, rock)
struct list *self;
procedure proc;
char *rock;
{
    register struct list_Entry *p;

    for (p = self->head; p != NULL; p = p->next)
        if ((*proc)(p->data, rock) == FALSE)
            return p->data;

    return NULL;
}

void list__Clear(self)
struct list *self;
{
    register struct list_Entry *p, *n;

    for (p = self->head; p != NULL; p = n) {
	n = p->next;
        free(p);
    }

    self->head = NULL;
    self->tail = NULL;
    self->size = 0;
}
