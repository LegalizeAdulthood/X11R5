/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/glist/RCS/glist.c,v 2.5 1991/09/12 16:21:51 bobg Exp $ */
/* $ACIS:glist.c 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/glist/RCS/glist.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/glist/RCS/glist.c,v 2.5 1991/09/12 16:21:51 bobg Exp $";
#endif /* lint */

/* List object
 * for Graph Editor
 */


#include <class.h>
#include <glist.eh>

#define newelt() (struct glistelt *) malloc(sizeof(struct glistelt))

boolean glist__InitializeObject(classID,self)
struct classheader *classID;
struct glist *self;

{
    self->head = self->tail = NULL;
    self->size = 0;
    self->DestroyProc = NULL;
    return TRUE;
}

struct glist *glist__Create(classID,Destroy)
struct classheader *classID;
procedure Destroy;
{
    struct glist *list = glist_New();
    list->DestroyProc = Destroy;
    return list;
}

static int copyElement(value,dest)
char *value;
struct glist *dest;
{
    glist_Insert(dest,value);
    return FALSE;
}


void glist__Copy(classID,dest,source)
struct classheader *classID;
struct glist *dest,*source;
{
    glist_Find(source,copyElement,dest);
}

void glist__Clear(self,destroy)
struct glist *self;
boolean destroy;
{
    struct glistelt *item = self->head, *next;

    while(item) {
	next = item->next;
	if (destroy && self->DestroyProc)
	    (*self->DestroyProc)(item->this);
        free(item);
        item = next;
    }

    self->head = self->tail = NULL;
    self->size = 0;
}

void glist__FinalizeObject(classID,self)
struct classheader *classID;
struct glist *self;
{
    struct glistelt *item = self->head, *next;

    while(item) {
	next = item->next;
	if (self->DestroyProc != NULL)
	    (*self->DestroyProc)(item->this);
        free(item);
        item = next;
    }
    self->head = self->tail = NULL;
    self->size = 0;

}


char * glist__Find(self,filter,rock)
struct glist *self;
procedure filter;
char * rock;
{
    char *rvalue;
    struct glistelt *item = self->head;

    while(item) {
        if ((*filter)(item->this,rock)) {
              rvalue = item->this;
              return rvalue;
          }
        else
            item = item->next;
    }
    return NULL;
}

/* push - inserts element at head of list
 *
 */

boolean glist__Push(self,element)
struct glist *self;
char * element;
{
    struct glistelt *temp = newelt();

    temp->next = self->head;
    temp->this = element;
    self->head = temp;
    ++(self->size);
    return TRUE;
}

/* insert - inserts element at tail of list
 *
 */

boolean glist__Insert(self,element)
struct glist *self;
char *element;
{
    struct glistelt *temp = newelt();

    temp->this = element;
    temp->next = NULL;

    if (self->size == 0) {
        self->head = self->tail = temp;
    }
    else {
        self->tail->next = temp;
        self->tail = temp;
    }
    ++(self->size);
    return TRUE;
}

/* pop - removes first element from list
 *
 */

char * glist__Pop(self)
struct glist *self;
{
    char *rvalue;

    if (self->size == 0)
	return NULL;
    else {
	rvalue = self->head->this;
	if (self->size == 1) {
	    free(self->head);
	    self->head = self->tail = NULL;
	}
	else {
	    struct glistelt *temp = self->head;
	    self->head = self->head->next;
	    free(temp);
	}
	--(self->size);
	return rvalue;
    }
}

/***********************************************************************/

boolean glist__InsertSorted(self,element,greater)
struct glist *self;
char * element;
procedure greater; /* greater(element_1,element_2) */

/* Greater takes two arguments, element_1 and element_2.
  It should return  a positive number if element_1 is greater than
  element_2, a negative number if element_1 is less than element_2,
  and zero if they are equal.
*/

{
    struct glistelt *temp = newelt();

    temp->this = element;

    if (self->size == 0) {
        temp->next = NULL;
        self->head = self->tail = temp;
    }
    else if (self->size == 1) {
        self->tail->next = temp;
        self->tail = temp;
        temp->next = NULL;
    }
    else {
        struct glistelt *i = self->head, *prev = self->head;

        /* move to the correct place in the list */
        while( (i != NULL) && ((*greater)(element,i->this) > 0)) {
            prev = i;
            i = i->next;
        }

        /* insert the element */
        if (i == prev) { /* at head of list */
            temp->next = self->head;
            self->head = temp;
        }
        else {
            if (i == NULL) { /* at tail of list */
                self->tail = temp;
            }
            prev->next = temp;
            temp->next = i;
        }
    }
    ++(self->size);
    return TRUE;
}


/* Insert an element only if it is not contained
 * (only tests pointers for equality)
 *
 */

boolean glist__InsertUnique(self,element)
struct glist *self;
char * element;
{
    if (glist_Contains(self,element))
        return FALSE;
    else
        return glist_Insert(self,element);
}


/***********************************************************************/

struct glist_SortStruct {
    struct glist *newlist;
    procedure compare;
};

static int MoveNew(listelt, ss)
char *listelt;
struct glist_SortStruct *ss;
{
    glist_InsertSorted(ss->newlist,listelt,ss->compare);
    return FALSE;
}

boolean glist__Sort(self,compare)
struct glist *self;
procedure compare;
{
    struct glist *temp = glist_New();
    struct glist_SortStruct ss;
    struct glistelt *temphead,*temptail;
    int tempsize;

    /* create new list, inserting old list elements in sorted order */
    ss.newlist = temp;
    ss.compare = compare;
    glist_Find(self,MoveNew,&ss);
    temphead = self->head;
    temptail = self->tail;
    tempsize = self->size;

    /* swap new and old lists */
    self->head = temp->head;
    self->tail = temp->tail;
    self->size = temp->size;
    temp->head = temphead;
    temp->tail = temptail;
    temp->size = tempsize;

    /* destroy old list */
    glist_Destroy(temp);
    return TRUE;
}

/***********************************************************************/


boolean glist__Delete(self,element,destroy)
struct glist *self;
char * element;
boolean destroy;
{
    struct glistelt *item = self->head;
    struct glistelt *prev = NULL;

    while (item) {
        if (item->this == element) {
            if (prev == NULL) {
                self->head = item->next;
		if (destroy && self->DestroyProc)
		    (*self->DestroyProc)(item->this);
		free(item);
                --(self->size);
                return TRUE;
            }
            else if (self->tail == item) {
                prev->next = NULL;
		if (destroy && self->DestroyProc)
		    (*self->DestroyProc)(item->this);
                free(item);
                --(self->size);
                self->tail = prev;
                return TRUE;
            }
            else {
                prev->next = item->next;
		if (destroy && self->DestroyProc)
		    (*self->DestroyProc)(item->this);
                free(item);
                --(self->size);
                return TRUE;
            }
        }
        else {
            prev = item;
            item = item->next;
        }
    }
    return FALSE;
}

/* check to see if a list contains a given element
 * (only checks pointer value)
 *
 */

boolean glist__Contains(self,element)
struct glist *self;
char * element;
{
    struct glistelt *item = self->head;

    while(item) {
        if (item->this == element)
              return TRUE;
        else
            item = item->next;
    }
    return FALSE;
}

void glist__Enumerate(self, proc, rock)
struct glist *self;
void (*proc)();
unsigned long rock;
{
  struct glistelt *elt;

  if(proc == NULL) return;
  for(elt = self->head; elt != NULL; elt = elt->next)
    (*proc)(elt->this,rock);
}
