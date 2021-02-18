/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/cltextv.c,v 1.2 1991/09/12 16:34:34 bobg Exp $ */
/* $ACIS: $ */
/* $Source:  */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/cltextv.c,v 1.2 1991/09/12 16:34:34 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#include <class.h>
#include <textv.ih>
#include <cursor.ih>
#include <cltextv.eh>
#define INITIALNUMOBSERVERS 4

static short FindObserverCallBack(self, observer, callBack)
     struct cltextview * self;
     struct basicobject *observer;
     procedure callBack;
{
  short i = 0;

  for (i = 0; i < self->maxObservers; ++i)
    if (self->observers[i].observer == observer
	&& self->observers[i].callBack == callBack)
      return i;
  return -1;
}


static short FindObserver( self, observer )
     struct cltextview * self;
     struct basicobject *observer;
{
  short i = 0;

  for (i = 0; i < self->maxObservers; ++i)
    if (self->observers[i].observer == observer)
      return i;
  return -1;
}


static short FreeSlot( self )
     struct cltextview * self;
{
  short i,j;
  
  for (i = 0; i < self->maxObservers; ++i)
    if (self->observers[i].observer == NULL)
      return i;

  if (self->maxObservers == 0)
    {
      self->maxObservers = INITIALNUMOBSERVERS;
      self->observers = (struct cltextview_observer *) malloc (INITIALNUMOBSERVERS * sizeof (struct cltextview_observer));
    }
  else
    {
      self->maxObservers += self->maxObservers / 2;
      self->observers = (struct cltextview_observer *)
	realloc(self->observers, self->maxObservers * sizeof (struct cltextview_observer));
    }
  j = i;
  while (i < self->maxObservers)
    self->observers[i++].observer = NULL;
  return j;
}

void cltextview__GetClickPosition(self, position, numberOfClicks, action, startLeft, startRight, leftPos, rightPos)
    struct cltextview *self;
    long position;
    long numberOfClicks;
    enum view_MouseAction action;
    long startLeft;
    long startRight;
    long *leftPos;
    long *rightPos;
    {
	int i;
	for (i = 0; i < self->maxObservers; ++i)
	    if (self->observers[i].observer != NULL)
		(*(self->observers[i].callBack))(self->observers[i].observer, self,
						 &position, &numberOfClicks, &action, &startLeft, &startRight, leftPos, rightPos, self->observers[i].rock ,cltextview_PREPROCESS);
	super_GetClickPosition(self, position, numberOfClicks, action, startLeft, startRight, leftPos, rightPos);
	for (i = 0; i < self->maxObservers; ++i)
	    if (self->observers[i].observer != NULL)
		(*(self->observers[i].callBack))(self->observers[i].observer, self,
						 &position, &numberOfClicks, &action, &startLeft, &startRight, leftPos, rightPos, self->observers[i].rock ,cltextview_POSTPROCESS);
    }


void cltextview__AddClickObserver( self, observer, callBack, rock )
     struct cltextview * self;
     struct basicobject * observer;
     procedure callBack;
     long rock;
{
  short free;

  if (FindObserver( self, observer) == -1)
    {
      free = FreeSlot(self);
      self->observers[free].observer = observer;
      self->observers[free].callBack = callBack;
      self->observers[free].rock = rock;
    }
}


void cltextview__RemoveClick( self, observer, callBack )
     struct cltextview * self;
     struct basicobject * observer;
     procedure callBack;
{
  short i;

  if ((i = FindObserverCallBack(self, observer, callBack)) != -1)
    {
      self->observers[i].observer = NULL;
    }
}
     

void cltextview__RemoveClickObserver( self, observer )
     struct cltextview * self;
     struct basicobject * observer;
{
  short i;

  while ((i = FindObserver( self, observer )) != -1)
    self->observers[i].observer = NULL;
}


boolean cltextview__InitializeObject(ClassID,self)
struct classheader *ClassID;
struct cltextview *self;
{
  self->maxObservers = 0;
  self->observers = NULL;
  self->cursor = cursor_Create(self);
  cursor_SetStandard(self->cursor,Cursor_LeftPointer);
  return TRUE;
}
void cltextview__FullUpdate(self, type, left, top, width, height)
struct cltextview *self;
enum view_UpdateType type;
long left;
long top;
long width;
long height;
{
    struct rectangle rect;
    super_FullUpdate(self, type, left, top, width, height);
    cltextview_GetVisualBounds(self,&rect);
    cltextview_PostCursor(self,&rect,self->cursor);
}
void cltextview__FinalizeObject(classID, self)
struct classheader *classID;
struct cltextview *self;
{
    if(self->cursor)
	cursor_Destroy(self->cursor);
}
