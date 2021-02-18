/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *chartapp_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/chart/RCS/chartapp.c,v 1.9 1991/09/12 16:04:24 bobg Exp $";


#ifndef lint
static char *rcsidchartapp_c = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/chart/RCS/chartapp.c,v 1.9 1991/09/12 16:04:24 bobg Exp $";
#endif

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Chart Application-Class

MODULE	chartapp.c

VERSION	0.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Chart Application-Class.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  01/19/89	Created (TCP)
  05/04/89	Add Major/Minor Version settings (TCP)
  05/10/89	Move im_Create outside im_SetView (macro problem) (TCP)
  05/31/89	Add classID parameter in FinalizeObject (TCP)
  09/01/89	Use super_ParseArgs (TCP)
  09/07/89	Remove Open-msg for non-existent new file (TCP)

END-SPECIFICATION  ************************************************************/

#include  <andrewos.h>
#include  <im.ih>
#include  <filetype.ih>
#include  <frame.ih>
#include  <apt.h>
#include  <chart.ih>
#include  <chartv.ih>
#include  <chartapp.eh>


#define  Chart		    (self->chart_data_object)
#define  ChartView	    (self->chart_view_object)
#define  Frame		    (self->frame)
#define  Im		    (self->im)
#define  Source		    (self->source)

chart_Specification	    data_specification[] =
  {
  NULL
  };

chartv_Specification	    view_specification[] =
  {
  NULL
  };

boolean 
chartapp__InitializeObject( classID, self )
  register struct classheader	 *classID;
  register struct chartapp	 *self;
  {
  IN(chartapp_InitializeObject);
  *Source = 0;
  chartapp_SetMajorVersion( self, 1 );
  chartapp_SetMinorVersion( self, 0 );
  OUT(chartapp_InitializeObject);
  return TRUE;
  }

void
chartapp__FinalizeObject( classID, self )
  register struct classheader	 *classID;
  register struct chartapp	 *self;
  {}

boolean
chartapp__ParseArgs( self, argc, argv )
  register struct chartapp	 *self;
  register int			  argc;
  register char			**argv;
  {
  IN(chartapp_ParseArgs);
  super_ParseArgs( self, argc, argv );
  while ( *++argv )
    { DEBUGst(ARGV,*argv);
    if ( **argv == '-' )
      {
      if ( strcmp( *argv, "-D" ) == 0 )
        debug = 1;
      else  printf( "Chart: Unrecognized switch '%s'\n", *argv );
      }
      else
      {
      if ( *(Source) == 0 )
        strcpy( Source, *argv );
	else
	printf( "Chart: Excessive Argument (Ignored) '%s'\n", *argv );
      }
    }
  OUT(chartapp_ParseArgs);
  return TRUE;
  }

boolean
chartapp__Start( self )
  struct chartapp	         *self;
  {
  register FILE			 *file;
  long				  id, status = true;

  super_Start( self );
  if ( Chart = chart_Create( data_specification, self ) )
    chart_SetDebug( Chart, debug );
  if ( ChartView = chartv_Create( view_specification, self ) )
    chartv_SetDebug( ChartView, debug );
  if ( Chart  &&  ChartView )
    {
    if((Frame = frame_New()) == NULL) {
	fprintf(stderr,"Could not allocate enough memory.\nexiting.\n");
	exit(-1);
    }
    frame_SetView( Frame, ChartView );
    if((Im = im_Create(NULL)) == NULL) {
	fprintf(stderr,"Could not create new window.\nexiting.\n");
	exit(-1);
    }
    im_SetView( Im, Frame );
    if ( *Source )
      {
      chart_SetChartAttribute( Chart, chart_FileName(Source) );
      frame_SetTitle( Frame, Source );
      if ( file = fopen( Source, "r" ) )
        {
        filetype_Lookup( file, Source, &id, 0 );
        chart_Read( Chart, file, id );
        fclose( file );
        }
      }
    chartv_SetDataObject( ChartView, Chart );
    chartv_WantInputFocus( ChartView, ChartView );
    return  status;
    }
    else 
    {
    printf( "Chart: Failed to Create objects\n" );
    return FALSE;
    }
  }
