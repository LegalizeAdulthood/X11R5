/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The LightTable Data-object Specification

MODULE	lt.H

NOTICE	IBM Internal Use Only

DESCRIPTION
	This defines the suite of Methods that support the LightTable Data-object.

HISTORY
  10/10/88	Created (TCP)

END-SPECIFICATION  ************************************************************/

#define  lt_VERSION    1

class lt : dataobject [dataobj]
  {
overrides:

methods:

  Set_Debug( debug );
  Read_Visuals( foreground, background )	returns long;

macromethods:


classprocedures:

  InitializeObject( struct lt *self )		returns boolean;

data:

  struct zip			     *zip;
  struct zip_stream		     *foreground_stream;
  char				      foreground_stream_name[512];
  struct raster			     *raster;
  char				      background_raster_name[512];
  long				      background_width, background_height;
  long				      id;
  };

