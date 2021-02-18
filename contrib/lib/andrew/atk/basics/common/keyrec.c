/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/keyrec.c,v 2.4 1991/09/12 15:59:05 bobg Exp $ */
/* $ACIS:keyrec.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/keyrec.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/keyrec.c,v 2.4 1991/09/12 15:59:05 bobg Exp $";
#endif /* lint */

#include <class.h>
#include <keyrec.eh>

static struct keyitem *freeList = NULL;

boolean keyrec__InitializeObject(classID, self)
    struct classheader *classID;
    struct keyrec *self;
{
    if (freeList != NULL)  {
	self->head = freeList;
	freeList = freeList->next;
    }
    else
	self->head = (struct keyitem *) malloc(sizeof(struct keyitem));
    
    self->head->next = self->head;
    self->tail = self->head;
    return TRUE;
}

void keyrec__FinalizeObject(classID, self)
    struct classheader *classID;
    struct keyrec *self;
{
    self->tail->next = freeList;
    freeList = self->head;
}

void keyrec__Clear(self)
    struct keyrec *self;
{
    if (self->head != self->tail)  {
	self->tail->next = freeList;
	freeList = self->head->next;
	self->head->next = self->head;
	self->tail = self->head;
    }
}
	
boolean keyrec__StartRecording(self)
    struct keyrec *self;
{
    if (self->recording) return FALSE;
    self->recording = TRUE;
    keyrec_Clear(self);
}

boolean keyrec__StopRecording(self)
    struct keyrec *self;
{
    if (! self->recording) return FALSE;
    self->recording = FALSE;
    return TRUE;
}

boolean keyrec__Recording(self)
    struct keyrec *self;
{
    return self->recording;
}

struct keyrec *keyrec__Copy(self)
    struct keyrec *self;
{
    register struct keyrec *newkr;
    register struct keyitem *ki;
    register struct keyitem *newki;
    
    newkr = keyrec_NewFromObject(self);
    for (ki = self->head->next; ki != self->head; ki = ki->next)  {
	if (freeList != NULL)  {
	    newki = freeList;
	    freeList = freeList->next;
	}
	else
	    newki = (struct keyitem *) malloc(sizeof(struct keyitem));
	*newki = *ki;
	self->tail->next = newki;
	newki->next = self->head;
	self->tail = newki;
    }
    
    return newkr;
}

void keyrec__RecordEvent(self, type, view, value1, value2, value3)
    struct keyrec *self;
    enum keyrec_EventType type;
    struct view *view;
    union keyrec_KeyValue value1;
    union keyrec_KeyValue value2;
    union keyrec_KeyValue value3;
{
    register struct keyitem *newki;

    if (freeList != NULL)  {
	newki = freeList;
	freeList = freeList->next;
    }
    else
	newki = (struct keyitem *) malloc(sizeof(struct keyitem));
	
    newki->type = type;
    newki->view = view;
    newki->parm[0] = value1;
    newki->parm[1] = value2;
    newki->parm[2] = value3;
    self->tail->next = newki;
    newki->next = self->head;
    self->tail = newki;
}

boolean keyrec__StartPlaying(self)
    struct keyrec *self;
{
    if (self->recording || self->playing) return FALSE;
    self->playing = TRUE;
    self->current = self->head->next;
}

boolean keyrec__NextKey(self, type, view, value1, value2, value3)
    struct keyrec *self;
    enum keyrec_EventType *type;
    struct view **view;
    union keyrec_KeyValue *value1;
    union keyrec_KeyValue *value2;
    union keyrec_KeyValue *value3;
{
    if (! self->playing) return FALSE;
    if (self->current == self->head)  {
	self->playing = FALSE;
	return FALSE;
    }
    *type = self->current->type;
    *view = self->current->view;
    *value1 = self->current->parm[0];
    *value2 = self->current->parm[1];
    *value3 = self->current->parm[2];
    self->current = self->current->next;
    return TRUE;
}

void keyrec__StopPlaying(self)
    struct keyrec *self;
{
    self->playing = FALSE;
}

