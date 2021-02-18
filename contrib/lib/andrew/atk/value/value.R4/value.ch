/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/value.ch,v 1.1 1991/09/30 18:16:13 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/value.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_value_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/value.ch,v 1.1 1991/09/30 18:16:13 susan Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


#define value_ROCK1 1
#define value_ROCK2 2 
#define value_STRING 3
#define value_STRINGARRAY 4



struct observer
{
  struct basicobject * observer;
  procedure callBack;
  long rock;
};

class value : dataobject[dataobj]  { /* really it would be nice if these two were in the opposite relationship */
 overrides:
  NotifyObservers( long rock );
  Write(FILE *file, long writeID, int level) returns long;
  Read (FILE *file, long id) returns long;
  ViewName() returns char *;
methods:
  AddCallBackObserver( struct basicobject * observer,
	      procedure callBack, long rock );
  RemoveCallBackObserver( struct basicobject * observer );
  RemoveCallBack( struct basicobject * observer,
		 procedure callBack );
  SetValueType(long rock,int type);
  SetStrArrayAndSize(char **rock,long size);
macromethods:
  SetValue(rock) (value_SetValueType(self,(long)rock,value_ROCK1))
  SetArraySize(rock) (value_SetValueType(self,(long)rock,value_ROCK2))
  SetString(rock) (value_SetValueType(self,(long)rock,value_STRING))
  SetStringArray(rock) (value_SetValueType(self,(long)rock,value_STRINGARRAY))
  GetValue() (self->rock1)
  GetArraySize() (self->rock2)
  GetString()  (self->string)
  GetStringArray()  (self->stringarray)
  SetNotify(val) ((self)->notify = val)
  GetUpdateCount() ((self)->updatecount)
classprocedures:

  InitializeClass() returns boolean;

  InitializeObject( struct callBackObservable * self ) returns boolean;
  FinalizeObject( struct callBackObservable * self );
data:
  short maxObservers;			/* number of entries in observers table */
  struct observer *observers;	/* table of observers */
  long rock1,rock2;
  char *string, **stringarray;
  boolean notify;
  long updatecount;
};


#define value_OBJECTDESTROYED -1


/* 
  the call back is invoked
  callBack( self, observed, observed rock, observers rock );
*/
