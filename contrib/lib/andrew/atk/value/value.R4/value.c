/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/value.c,v 1.1 1991/09/30 18:16:13 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/value.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/value.c,v 1.1 1991/09/30 18:16:13 susan Exp $ ";
#endif /* lint */


#include <class.h>
#include <andrewos.h>
#include <value.eh>
#include <value.h>
#include <atom.ih>
#include <dataobj.ih>
#include <observe.ih>
#include <view.ih>
#define INITIALNUMOBSERVERS 4

static struct atom *rock1atom, *rock2atom, *stringatom, *stringarrayatom, *valueatom;

/****************************************************************/
/*		static support					*/
/****************************************************************/
static short FindObserverCallBack(self, observer, callBack)
     struct value * self;
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
     struct value * self;
     struct basicobject *observer;
{
  short i = 0;

  for (i = 0; i < self->maxObservers; ++i)
    if (self->observers[i].observer == observer)
      return i;
  return -1;
}


static short FreeSlot( self )
     struct value * self;
{
  short i,j;
  
  for (i = 0; i < self->maxObservers; ++i)
    if (self->observers[i].observer == NULL)
      return i;

  if (self->maxObservers == 0)
    {
      self->maxObservers = INITIALNUMOBSERVERS;
      self->observers = (struct observer *) malloc (INITIALNUMOBSERVERS * sizeof (struct observer));
    }
  else
    {
      self->maxObservers += self->maxObservers / 2;
      self->observers = (struct observer *)
	realloc(self->observers, self->maxObservers * sizeof (struct observer));
    }
  j = i;
  while (i < self->maxObservers)
    self->observers[i++].observer = NULL;
  return j;
}


/****************************************************************/
/*		class procedures 				*/
/****************************************************************/
boolean value__InitializeObject(classID,self)
struct classheader *classID;
     struct value * self;
{
  self->maxObservers = 0;
  self->updatecount = 0L;
  self->observers = NULL;
  self->rock1 = self->rock2 = 0l;
  value_Put(self,valueatom,rock1atom,self->rock1);
  self->string = NULL;
  self->stringarray = NULL;
  self->notify = TRUE;
  return TRUE;
}

void value__FinalizeObject(classID,self)
struct classheader *classID;
struct value * self;
{
  value_NotifyObservers(self, value_OBJECTDESTROYED);
  if (self->observers){
    free(self->observers);
    self->observers = NULL;
  }
}

/****************************************************************/
/*		instance methods				*/
/****************************************************************/


void value__AddCallBackObserver( self, observer, callBack, rock )
     struct value * self;
     struct basicobject * observer;
     procedure callBack;
     long rock;
{
  short free;

  if (FindObserverCallBack(self, observer, callBack) == -1)
    {
      free = FreeSlot(self);
      self->observers[free].observer = observer;
      self->observers[free].callBack = callBack;
      self->observers[free].rock = rock;
    }
}


void value__RemoveCallBack( self, observer, callBack )
     struct value * self;
     struct basicobject * observer;
     procedure callBack;
{
  short i;

  if ((i = FindObserverCallBack(self, observer, callBack)) != -1)
    {
      self->observers[i].observer = NULL;
    }
}
     

void value__RemoveCallBackObserver( self, observer )
     struct value * self;
     struct basicobject * observer;
{
  short i;

  while ((i = FindObserver( self, observer )) != -1)
    self->observers[i].observer = NULL;
}


void value__NotifyObservers( self, rock )
     struct value * self;
     long rock;
{
  short i;
  super_NotifyObservers( self, rock );
  if(self->observers){
      for (i = 0; i < self->maxObservers; ++i)
	  if (self->observers[i].observer != NULL)
	      (*(self->observers[i].callBack))(self->observers[i].observer, self,
					       self->observers[i].rock, rock);
  }
}




long value__Write(self, file, writeID, level)
    struct value *self;
    FILE *file;
    long writeID;
    int level;
{   /* NOTE : only the value (rock1)is saved */
    if (value_GetWriteID(self) != writeID)  {
	value_SetWriteID( self,writeID);
        fprintf(file, "\\begindata{%s,%ld}\n", class_GetTypeName(self),value_GetID(self));
	fprintf(file,">%d\n",value_GetValue(self));
        fprintf(file, "\\enddata{%s,%ld}\n",  class_GetTypeName(self),value_GetID(self));
    }

    return value_GetID(self);
}
long value__Read(self, file, id)
struct value *self;
FILE *file;
long id;
{
    char buf[256];
    while(1){
	if(fgets(buf,256,file) == NULL) return dataobject_PREMATUREEOF;
	if(*buf == '>') {
	    self->rock1 = atoi(buf + 1);
	    value_Put(self,valueatom,rock1atom,self->rock1);
	}
	else if(strncmp(buf,"\\enddata",8) == 0)return dataobject_NOREADERROR;
    }
    
}
	
void value__SetValueType(self,rock,type)
struct value *self;
long rock;
int type;
{
    
    switch(type){
	case  value_ROCK1:
	    self->rock1 = rock;
	    
	    value_Put(self,valueatom,rock1atom,rock);
	    
	    break;
	case  value_ROCK2:
	    self->rock2 = rock;
	    
	    value_Put(self,valueatom,rock2atom,rock);
	    
	    break;
	case  value_STRING:
	    self->string = (char *) rock;
	    
	    value_Put(self,valueatom,stringatom,rock);
	    
	    break;
	case  value_STRINGARRAY:
	    self->stringarray = (char **) rock;
	    
	    value_Put(self,valueatom,stringarrayatom,rock);
	    
	    break;
    }
    (self->updatecount)++;
    if(self->notify) value_NotifyObservers(self,value_NEWVALUE);
}
char *value__ViewName(self)
    struct value *self;
{
    return "buttonV";
}
void value__SetStrArrayAndSize(self,rock,size)
struct value *self;
char **rock;
long size;
{
    self->stringarray = (char **) rock;
    self->rock2 = size;
    if(self->notify) value_NotifyObservers(self,value_NEWVALUE);
}

boolean value__InitializeClass(classID)
struct classheader *classID;
{
    valueatom=atom_Intern("value");
    rock1atom=atom_Intern("rock1");
    rock2atom=atom_Intern("rock2");
    stringatom=atom_Intern("string");
    stringarrayatom=atom_Intern("stringarray");
    if(rock1atom &&  rock2atom && stringatom && stringarrayatom) return TRUE;
    else return FALSE;
}
