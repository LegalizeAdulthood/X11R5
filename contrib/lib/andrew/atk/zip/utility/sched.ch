/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Sched Data-object Specification

MODULE	sched.ch

NOTICE	IBM Internal Use Only

DESCRIPTION
	This defines the suite of Methods that support the LightTable Data-object.

HISTORY
  01/20/88	Created (TCP)

END-SPECIFICATION  ************************************************************/

#define  sched_VERSION    1

class sched : dataobject [dataobj]
  {
overrides:

methods:

  Set_Debug( debug );

macromethods:


classprocedures:

  Create( char *stream_name )				returns struct sched *;
  InitializeObject( struct sched *self )		returns boolean;

data:

  struct zip			     *zip;
  struct zip_stream		     *stream;
  char				      stream_name[512];
  long				      id;
  };

