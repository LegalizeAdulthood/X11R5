/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *sched_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/zip/utility/RCS/sched.c,v 1.2 1991/09/12 16:46:41 bobg Exp $";

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Sched Data-object Program

MODULE	sched.c

NOTICE	IBM Internal Use Only

DESCRIPTION
	This is the suite of Methods that support the LightTable Data-object.

HISTORY
  10/10/88	Created (TCP)

END-SPECIFICATION  ************************************************************/

#include "sched.eh"
#include "zip.ih"
#include <errno.h>

void
sched__Set_Debug( self, mode )
  register struct sched		     *self;
  {
  debug = mode;
  }

struct sched *
sched__Create( ClassID, stream_name )
  register struct  classheader	     *ClassID;
  register char			     *stream_name;
  {
  register struct sched		     *self;
  register long			      status;

  IN(sched_Create);
  if ( self = sched_New() )
    {
    if ( stream_name  &&  *stream_name )
      {
      strcpy( self->stream_name, stream_name );
      DEBUGst(Stream-name,self->stream_name);
      if ( status = zip_Open_Stream(self->zip, &self->stream,
				self->stream_name, NULL ) )
        { DEBUG(Open Failure);
        printf( "Schedule: Unable to Open %s\n", self->stream );
        }
        else  status = zip_Read_Stream( self->zip, self->stream );
      }
    }
  OUT(sched_Create);
  return  self;
  }


boolean
sched__InitializeObject( classID, self )
  register struct classheader	     *classID;
  register struct sched		     *self;
  {
  register long			      status = false;

  IN(sched_InitializeObject);
  self->stream = NULL;
  *self->stream_name = 0;
  if ( self->zip = zip_New() )
    status = true;
  OUT(sched_InitializeObject);
  return  status;
  }
