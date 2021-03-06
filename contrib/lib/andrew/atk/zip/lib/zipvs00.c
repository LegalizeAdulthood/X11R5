/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/zip/lib/RCS/zipvs00.c,v 2.6 1991/09/12 16:46:09 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/zip/lib/RCS/zipvs00.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/zip/lib/RCS/zipvs00.c,v 2.6 1991/09/12 16:46:09 bobg Exp $";
#endif /* lint */

/*
 * P_R_P_Q_# (C) COPYRIGHT IBM CORPORATION 1988
 * LICENSED MATERIALS - PROPERTY OF IBM
 * REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
 */
/* zipvs00.c	Zip View-object	-- Stream			      */
/* Author	TC Peters					      */
/* Information Technology Center	   Carnegie-Mellon University */


/*
    $Log: zipvs00.c,v $
 * Revision 2.6  1991/09/12  16:46:09  bobg
 * Update copyright notice and rcsid
 *
 * Revision 2.5  1989/07/20  13:13:43  sg08
 * introduced Stream_Visible method
 *
 * Revision 2.4  89/02/17  18:11:25  ghoti
 * ifdef/endif,etc. label fixing - courtesy of Ness
 * 
 * Revision 2.3  89/02/08  16:53:22  ghoti
 * change copyright notice
 * 
 * Revision 2.2  89/02/07  21:38:47  ghoti
 * first pass porting changes: filenames and references to them
 * 
 * Revision 2.1  88/09/27  18:22:05  ghoti
 * adjusting rcs #
 * 
 * Revision 1.2  88/09/15  17:56:58  ghoti
 * copyright fix
 * 
 * Revision 1.1  88/09/14  17:48:59  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Zip View-object -- Stream

MODULE	zipvs00.c

NOTICE	IBM Internal Use Only

DESCRIPTION
	This is the suite of Methods that support the Figure facilities
	of the Zip View-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  03/31/88	Created (TCP)
  07/12/89	Added Stream_Visible method (SCG)

END-SPECIFICATION  ************************************************************/

#include "class.h"
#include "view.ih"
#include "zip.ih"
#include "zipv.ih"

#define	 Data			      (self->data_object)
#define	 View			      (self)


long
zipview__Display_Stream( self, stream, pane )
  register struct zipview		 *self;
  register zip_type_stream		  stream;
  register zip_type_pane		  pane;
  {
  register int				  status = zip_success;

  IN(zipview__Display_Stream);
  if ( pane  &&  stream )
    {
    zipview_Set_Pane_Stream( self, pane, stream );
    status = zipview_Display_Pane( self, pane );
    }
    else 
    {
    if ( pane == NULL )
      status = zip_pane_non_existent;
      else
      status = zip_stream_non_existent;
    }
  zipview_FlushGraphics( self );
  ZIP_STATUS();
  OUT(zipview__Display_Stream);
  return status;
  }

long
zipview__Draw_Stream( self, stream, pane )
  register struct zipview		 *self;
  register zip_type_stream		  stream;
  register zip_type_pane		  pane;
  {
  register int				  status = zip_success;

  IN(zipview__Draw_Stream);
  if ( pane  &&  stream )
    {
    status = zipview_Draw_Image( self, stream->zip_stream_image_anchor, pane );
    }
    else 
    {
    if ( pane == NULL )
      status = zip_pane_non_existent;
      else
      status = zip_stream_non_existent;
    }
  zipview_FlushGraphics( self );
  ZIP_STATUS();
  OUT(zipview__Draw_Stream);
  return status;
  }

long
zipview__Clear_Stream( self, stream, pane )
  register struct zipview		 *self;
  register zip_type_stream		  stream;
  register zip_type_pane		  pane;
  {
return zip_ok;/*===*/
  }

long
zipview__Hide_Stream( self, stream, pane )
  register struct zipview		 *self;
  register zip_type_stream		  stream;
  register zip_type_pane		  pane;
  {
return zip_ok;/*===*/
  }

long
zipview__Expose_Stream( self, stream, pane )
  register struct zipview		 *self;
  register zip_type_stream		  stream;
  register zip_type_pane		  pane;
  {
return zip_ok;/*===*/
  }

zip_type_stream
zipview__Which_Stream( self, x, y )
  register struct zipview		 *self;
  register long				  x, y;
  {
  register int				  status = zip_success;
  register zip_type_stream		  stream = NULL;
  register zip_type_pane		  pane;

  IN(zip_Which_Stream);
  if ( (pane = zipview_Which_Pane( self, x, y )) )
    if ( pane->zip_pane_attributes.zip_pane_attribute_stream_source )
      stream = pane->zip_pane_source.zip_pane_stream;
    else
    if ( pane->zip_pane_attributes.zip_pane_attribute_image_source )
      stream = pane->zip_pane_source.zip_pane_image->zip_image_stream;
    else
    if ( pane->zip_pane_attributes.zip_pane_attribute_figure_source )
      stream = pane->zip_pane_source.zip_pane_figure->zip_figure_image->zip_image_stream;
  ZIP_STATUS();
  OUT(zip_Which_Stream);
  return stream;
  }

zip_type_stream
zipview__Within_Which_Stream( self, x, y )
  register struct zipview		 *self;
  register long				  x, y;
  {
return zip_ok;/*===*/
  }

boolean
zipview__Stream_Visible( self, stream, pane )
  register struct zipview		    *self;
  register zip_type_stream		    stream;
  register zip_type_pane		    pane;
  {
  register boolean			    status = FALSE;

  IN( zipview__Stream_Visible );
  if ( stream && pane )
    status = zipview_Image_Visible( self, zip_Image_Root( Data, stream ), pane );
  OUT( zipview__Stream_Visible );
  return status;
  }
