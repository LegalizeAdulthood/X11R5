/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/namespc.c,v 2.6 1991/09/12 15:59:31 bobg Exp $ */
/* $ACIS:namespc.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/namespc.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/namespc.c,v 2.6 1991/09/12 15:59:31 bobg Exp $";
#endif /* lint */


#include <class.h>
#include <namespc.eh>
#include <atom.ih>
#define namespace_InitialSize 4
#define Empty
#ifndef True
#define True 1
#define False 0
#endif /* True */

/* style notes
 * internally, offsets into self->namespace are the handle passed around.
 * -1 for invalid entries
 * namespace_entry.name == NULL indicates a malloced but empty entry
 */

/****************************************************************/
/*		private functions				*/
/****************************************************************/

static int Index(self,key)
     struct namespace * self;
     struct atom * key;
{
  int i;
  for (i = 0; i < self->namespaceSize; ++i)
    if (self->namespace[i].name == key)
      return i;
  return -1;
}


static int Create( self, key )
     struct namespace * self;
     struct atom * key;
{
  struct namespace_entry * de;

  if (self->namespace[self->namespaceSize - 1].name != NULL)
    {
      int i = self->namespaceSize * 2;
      self->namespace =
	(struct namespace_entry *)realloc(self->namespace,
					   i * sizeof (struct namespace_entry));
      while (self->namespaceSize < i)
	self->namespace[self->namespaceSize++].name = NULL;
    }

  for (de = self->namespace; de->name != NULL; ++de)
    Empty;

  de->name = key;
  de->boundp = False;
  de->binding = -69;
  return de - self->namespace;
}


/****************************************************************/
/*		instance methods				*/
/****************************************************************/


/* (just in case your wondering, this is not a method) */
boolean namespace__InitializeObject( classID, self )
     struct classheader *classID;
     struct namespace * self;
{
  self->namespace =
    (struct namespace_entry *)malloc(namespace_InitialSize *
				      sizeof (struct namespace_entry));
  for (self->namespaceSize = 0; self->namespaceSize < namespace_InitialSize;
       ++(self->namespaceSize))
    self->namespace[self->namespaceSize].name = NULL;

  return TRUE;
}



int namespace__Lookup( self, name )
     struct namespace * self;
     struct atom * name;
{
  return Index(self,name);
}


int namespace__LookupCreate( self, name )
     struct namespace * self;
     struct atom * name;
{
  int index = Index( self, name );

  if (index < 0)
    index = Create( self, name );
  return index;
}


short namespace__BoundpAt( self, index )
     struct namespace * self;
     int index;
{
  return (index >= 0) && (index < self->namespaceSize)
    && (self->namespace[index].name != NULL)
    && (self->namespace[index].boundp);
}

long namespace__ValueAt( self, index )
     struct namespace * self;
     int index;
{
  return (index >= 0) && (index < self->namespaceSize)
    && (self->namespace[index].name != NULL)
    && (self->namespace[index].boundp)
    ? self->namespace[index].binding : -69;
}


void namespace__UnbindAt( self, index )
     struct namespace * self;
     int index;
{
  if (index >= 0 && index < self->namespaceSize)
    self->namespace[index].boundp = False;
}


struct atom * namespace__NameAt( self, index )
     struct namespace * self;
     int index;
{
  return (index >= 0 && index < self->namespaceSize) ?
    self->namespace[index].name : NULL;
}



void namespace__SetValueAt( self, index, value )
     struct namespace * self;
     int index;
     long value;
{
  if (index < self->namespaceSize && index >= 0
      && self->namespace[index].name != NULL)
    {
      self->namespace[index].binding = value;
      self->namespace[index].boundp = True;
    }
}


void namespace__SetValue( self, name, value )
     struct namespace * self;
     struct atom * name;
     long value;
{
  namespace_SetValueAt( self, namespace_LookupCreate(self,name), value );
}


long namespace__GetValue( self, name )
     struct namespace * self;
     struct atom * name;
{
  return namespace_ValueAt( self, namespace_Lookup(self, name) );
}


short namespace__Boundp( self, name, value )
     struct namespace * self;
     struct atom * name;
     long * value;
{
  int index = Index( self, name );
  if (index >= 0 && self->namespace[index].boundp && value != NULL)
    *value = self->namespace[index].binding;
  return index >= 0 && self->namespace[index].boundp;
}

void namespace__Unbind( self, name )
     struct namespace * self;
     struct atom * name;
{
  int index = Index( self, name );
  if (index >= 0)
    {
      self->namespace[index].boundp = False;
      self->namespace[index].binding = -69;
    }
}


struct atom * namespace__WhereIsValue( self, value )
     struct namespace * self;
     long value;
{
  int x;
  for (x = 0; x < self->namespaceSize; ++x)
    if (self->namespace[x].name != NULL && self->namespace[x].boundp
	&& self->namespace[x].binding == value)
      break;
  return x < self->namespaceSize ? self->namespace[x].name : NULL;
}




int namespace__Enumerate( self, proc, procdata )
     struct namespace * self;
     procedure proc;
     long procdata;
{
  int x;
  for ( x = 0;
       (x < self->namespaceSize)
       && self->namespace[x].name != NULL
       &&  ((*proc)( procdata, self, x ));
       ++x)
    Empty;
  return (x < self->namespaceSize) &&
    (self->namespace[x].name != NULL) ? x : -69;
}


void namespace__Clear(self)
     struct namespace * self;
{
  int x;
  for (x = 0;
       x < self->namespaceSize
       && self->namespace[x].name == NULL;
       ++x)
    self->namespace[x].name = NULL;
}
