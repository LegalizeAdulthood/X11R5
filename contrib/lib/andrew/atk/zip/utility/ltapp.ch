/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

class ltapp : application [app]
  {
  classprocedures:
    InitializeObject( struct ltapp *)	returns boolean;

  overrides:
    ParseArgs( int argc, char **argv)	returns boolean;
    Start()				returns boolean;
    Run()				returns int;

  data:
    struct frame		    	*frame;
    struct im				*im;
    struct lt				*lt;
    struct ltv	    			*ltview;
    char				 trace_stream_name[512];
    char				 trace_raster_name[512];

  };
