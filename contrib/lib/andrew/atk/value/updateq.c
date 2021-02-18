/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/updateq.c,v 2.5 1991/09/30 18:26:45 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/updateq.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/updateq.c,v 2.5 1991/09/30 18:26:45 susan Exp $ ";
#endif /* lint */

#include <class.h>
#include <updateq.eh>
#define New(TYPE) ( TYPE *)malloc( sizeof (TYPE) )

/**************** statics ****************/


/**************** class procedures ****************/
boolean updateq__InitializeObject(classID, self )
struct classheader *classID;
     struct updateq * self;
{
  self->view = NULL;
  self->queue = NULL;
  return TRUE;
}


void updateq__FinalizeObject( self )
     struct updateq * self;
{
  updateq_ClearUpdateQueue(self);
}


void updateq__SetView( self, view )
     struct updateq * self;
     struct view * view;
{
  self->view = view;
}


void updateq__EnqueueUpdateFunction( self, fp )
     struct updateq * self;
     procedure fp;
{
  struct fp_queue *  newEntry;
  struct fp_queue ** qpointer;

  newEntry = New( struct fp_queue );
  newEntry->fp = fp;
  newEntry->next = NULL;
  for (qpointer = &self->queue; *qpointer != NULL;
       qpointer = &((*qpointer)->next));

  *qpointer = newEntry;
}


void updateq__ExecuteUpdateQueue(self)
     struct updateq * self;
{
  struct fp_queue * qpointer;

  /* The implimentation of this allows the clearq and enque methods
    to be envoked by one of the functions that gets called here.  if you
    break this, you will loose in a serious way */

  while (self->queue != NULL)
    {
      qpointer = self->queue;
      self->queue = self->queue->next;
      (*(qpointer->fp))(self->view);
      free(qpointer);
    }

}


void updateq__ClearUpdateQueue(self)
     struct updateq * self;
{
  struct fp_queue * qpointer;
  
  while (self->queue != NULL)
    {
      qpointer = self->queue;
      self->queue = self->queue->next;
      free(qpointer);
    }
}

