/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *schedapp_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/zip/utility/RCS/schedapp.c,v 1.5 1991/09/12 16:46:46 bobg Exp $";


#include <andrewos.h>
#include <im.ih>
#include <frame.ih>
#include <message.ih>
#include <view.ih>
#include <environ.ih>
#include <lpair.ih>
#include <text.ih>
#include <textv.ih>
#include <sched.ih>
#include <schedv.ih>
#include <schedapp.eh>
#include <zip.ih>


boolean 
schedapp__InitializeObject( classID, self )
  register struct classheader		 *classID;
  register struct schedapp		 *self;
  {
  IN(schedapp_InitializeObject);
  *(self->stream_name) = 0;
  OUT(schedapp_InitializeObject);
  return TRUE;
  }

boolean
schedapp__ParseArgs( self, argc, argv )
  register struct schedapp		 *self;
  register int				  argc;
  register char				**argv;
  {
  IN(schedapp_ParseArgs);
  while ( *++argv )
    {
    DEBUGst(ARGV,*argv);
    if ( **argv == '-' )
      {
      if ( strcmp( *argv, "-d" ) == 0 )
        debug = 1;
      else  printf( "Sched: Unrecognized switch '%s'\n", *argv );
      }
      else
      {
      if ( *(self->stream_name) == 0 )
        strcpy( self->stream_name, *argv );
      }
    }
  OUT(schedapp_ParseArgs);
  return TRUE;
  }

boolean 
schedapp__Start( self )
  register struct schedapp			 *self;
  {
  IN(schedapp_Start);
  if( !super_Start(self) )
    return FALSE;
  if((self->im  = im_Create(NULL)) == NULL) {
    fprintf(stderr,"sched: Failed to create new window; exiting.\n");
    return(FALSE);
  }
  if (*self->stream_name == 0 ) strcpy( self->stream_name,"itcCR.scd");
  if((self->sched = sched_Create( self->stream_name )) == NULL) {
      fprintf(stderr,"sched: Could not allocate enough memory; exiting.\n");
      return(FALSE);
  }
  sched_Set_Debug( self->sched, debug );
  if((self->schedview = schedv_New()) == NULL) {
      fprintf(stderr,"sched: Could not allocate enough memory; exiting.\n");
      return(FALSE);
  }
  schedv_Set_Debug( self->schedview, debug );
  schedv_SetDataObject( self->schedview, self->sched );
  if((self->frame = frame_New()) == NULL) {
      fprintf(stderr,"sched: Could not allocate enough memory; exiting.\n");
      return(FALSE);
  }
  frame_SetView( self->frame, self->schedview );
  im_SetView( self->im, self->frame );
  OUT(schedapp_Start);
  return TRUE;
  }

int
schedapp__Run(self)
  register struct schedapp			  *self;
  {
  IN(schedapp_Run);
  schedapp_Fork( self );
  schedv_WantInputFocus( self->schedview, self->schedview );
  DEBUG(...Interacting...);
  while ( im_Interact( 0 ) ) ;
  im_KeyboardProcessor();
  OUT(schedapp_Run);
  return(0);
  }
