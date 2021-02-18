/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* zipoline.c	Zip Object -- Line					      */
/* Author	TC Peters						      */
/* Information Technology Center		   Carnegie-Mellon University */

static char	rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/zip/lib/RCS/zipoline.c,v 2.9 1991/09/12 16:43:09 bobg Exp $";

/*
    $Log: zipoline.c,v $
 * Revision 2.9  1991/09/12  16:43:09  bobg
 * Update copyright notice and rcsid
 *
 * Revision 2.8  1991/09/10  20:52:14  gk5g
 * Changes in support of SGI_4d platform.
 * Mostly added forward delcarations.
 *
 * Revision 2.7  1990/08/21  14:30:21  sg08
 * Add Ensure_Attribute usage on Draw and Print. Add Contains method
 *
 * Revision 2.6  89/09/08  17:42:06  ghoti
 * removal of unused variables
 * 
 * Revision 2.5  89/02/08  16:50:49  ghoti
 * change copyright notice
 * 
 * Revision 2.4  89/02/07  19:49:38  ghoti
 * first pass porting changes: filenames and references to them
 * 
 * Revision 2.3  88/11/18  21:11:10  tom
 * Handle variable line-widths.
 * 
 * Revision 2.2  88/10/11  20:29:38  tom
 * Change Printing interface to remove pane and figure args.
 * 
 * Revision 2.1  88/09/27  18:16:20  ghoti
 * adjusting rcs #
 * 
 * Revision 1.2  88/09/15  17:39:54  ghoti
 * copyright fix
 * 
 * Revision 1.1  88/09/14  17:45:32  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- Internal Facility Suite  *********************************

TITLE	Zip Object -- Line

MODULE	zipoline.c

NOTICE	IBM Internal Use Only

DESCRIPTION

.......................

HISTORY
  04/13/88	Created (TC Peters)
   08/16/90	Add Ensure_Attribute usage on Draw and Print. Add Contains method (SCG)

END-SPECIFICATION  ************************************************************/

#include "class.h"
#include "zipobj.ih"
#include "zipoline.eh"

static Draw();
static Compute_Handle_Positions();

char
zipoline__Object_Icon( self )
  register struct zipoline		 *self;
  {
  IN(zipoline__Object_Icon);
  OUT(zipoline__Object_Icon);
  return  'C';
  }

char
zipoline__Object_Icon_Cursor( self )
  register struct zipoline		 *self;
  {
  IN(zipoline__Object_Icon_Cursor);
  OUT(zipoline__Object_Icon_Cursor);
  return  'I';
  }

char
zipoline__Object_Datastream_Code( self )
  register struct zipoline		 *self;
  {
  IN(zipoline__Object_Datastream_Code);
  OUT(zipoline__Object_Datastream_Code);
  return  'C';
  }

long
zipoline__Show_Object_Properties( self, pane, figure )
  register struct zipoline		 *self;
  register zip_type_pane		  pane;
  register zip_type_figure		  figure;
  {
  zipview_Announce( View, "Draw Line from Left to Right." );
  return  zip_ok;
  }

long
zipoline__Build_Object( self, pane, action, x, y, clicks, X, Y )
  register struct zipoline		 *self;
  register zip_type_pane		  pane;
  register long				  action, x, y, clicks;
  register zip_type_point		  X, Y;
  {
  register long				  status = zip_ok;
  int					  position = 0; /*===*/
  register zip_type_figure		  figure;

  IN(zipoline__Build_Object);
  switch ( action )
    {
    case view_LeftDown:
      if ( (status =
	zip_Create_Figure( Data, &CurrentFigure, NULL, zip_line_figure,
			   CurrentImage, position )) == zip_success )
        {
        zipoline_Set_Object_Point( self, CurrentFigure, 9, X, Y );
        zipoline_Set_Object_Point( self, CurrentFigure, 5, X, Y );
	CurrentFigure->zip_figure_zoom_level = pane->zip_pane_zoom_level;
        pane->zip_pane_edit->zip_pane_edit_last_point_id = zip_figure_auxiliary_point;
	}
      break;
    case view_LeftUp:
      if ( figure = CurrentFigure )
        {
	if ( figure_x_point == figure_x_points(0)  &&
	     figure_y_point == figure_y_points(0) )
	  {
	  zipedit_Delete_Figure( Edit, figure, pane );
          break;
	  }
	}
      /* Fall-thru */
    case view_LeftMovement:
      if ( CurrentFigure )
	{
	zipview_Set_Pane_Painting_Mode( View, pane, zipview_paint_inverted );
	zipview_Draw_Figure( View, CurrentFigure, pane );
        zipoline_Set_Object_Point( self, CurrentFigure, 5, X, Y );
	zipview_Draw_Figure( View, CurrentFigure, pane );
	zipview_Set_Pane_Painting_Mode( View, pane, zip_default );
	}
      break;
    }
  OUT(zipoline__Build_Object);
  return  status;
  }

long
zipoline__Draw_Object( self, figure, pane )
  register struct zipoline		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  {
  register long				  status = zip_ok;

  IN(zipoline__Draw_Object);
  if ( zipview_Condition( View, pane, figure, zip_draw ) )
    status = Draw( self, figure, pane );
  OUT(zipoline__Draw_Object);
  return  status;
  }

long
zipoline__Clear_Object( self, figure, pane )
  register struct zipoline		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  {
  register long				  status = zip_ok;

  IN(zipoline__Clear_Object);
  if ( zipview_Condition( View, pane, figure, zip_clear ) )
    status = Draw( self, figure, pane );
  OUT(zipoline__Clear_Object);
  return  status;
  }

static
Draw( self, figure, pane )
  register struct zipoline		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  {
  register long				  status = zip_ok;
  register unsigned char		  width;

  IN(Draw);
  if ( zipview_Ensure_Line_Attributes( View, figure ) == zip_ok )
    {
    zipview_MoveTo( View, window_x_point, window_y_point );
    zipview_DrawLineTo( View, window_x_points(0), window_y_points(0) );
    if ( ExposePoints )
      zipoline_Expose_Object_Points( self, figure, pane );
    if ( HighlightPoints )
      zipoline_Highlight_Object_Points( self, figure, pane );
    }
  OUT(Draw);
  return  status;
  }

long
zipoline__Print_Object( self, figure, pane )
  register struct zipoline		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  {
  register long				  status = zip_ok;

  IN(zipoline__Print_Object);
  zipprint_Ensure_Line_Attributes( Print, figure );
  zipprint_Draw_Line( Print, print_x_point, print_y_point,
		      print_x_points( 0 ), print_y_points( 0 ) );
  OUT(zipoline__Print_Object);
  return  status;
  }

long
zipoline__Proximate_Object_Points( self, figure, pane, x, y )
  register struct zipoline		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  register zip_type_pixel		  x, y;
  {
  register int				  point = 0;
  zip_type_pixel			  X1, X2, X3, Y1, Y2, Y3;

  IN(zipoline__Proximate_Object_Points);
  Compute_Handle_Positions( self, figure, pane, &X1, &X2, &X3, &Y1, &Y2, &Y3 );
  /* Ordered thus to give preference to moving an End-point */
  /* Upper-Left Corner */
  if ( zipview_Proximate_Figure_Point( View, pane, figure, X1, Y1, x, y )) point = 9;
  else	/* Lower_Right Corner */
  if ( zipview_Proximate_Figure_Point( View, pane, figure, X3, Y3, x, y )) point = 5;
  else 	/* Center */
  if ( zipview_Proximate_Figure_Point( View, pane, figure, X2, Y2, x, y )) point = 1;
  else	/* 12 O'Clock */
  if ( zipview_Proximate_Figure_Point( View, pane, figure, X2, Y1, x, y )) point = 2;
  else	/* Upper-Right Corner */
  if ( zipview_Proximate_Figure_Point( View, pane, figure, X3, Y1, x, y )) point = 3;
  else	/* 3 O'Clock */
  if ( zipview_Proximate_Figure_Point( View, pane, figure, X3, Y2, x, y )) point = 4;
  else	/* 6 O'Clock */
  if ( zipview_Proximate_Figure_Point( View, pane, figure, X2, Y3, x, y )) point = 6;
  else	/* Lower-Left Corner */
  if ( zipview_Proximate_Figure_Point( View, pane, figure, X1, Y3, x, y )) point = 7;
  else	/* 9 O'Clock */
  if ( zipview_Proximate_Figure_Point( View, pane, figure, X1, Y2, x, y )) point = 8;
  OUT(zipoline__Proximate_Object_Points);
  return  point;
  }

boolean
zipoline__Enclosed_Object( self, figure, pane, x, y, w, h )
  register struct zipoline		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  register zip_type_pixel		  x, y, w, h;
  {
  register boolean			  enclosed = false;
  register zip_type_pixel		  X1, Y1, X2, Y2;

  IN(zipoline__Enclosed_Object);
  X1 = window_x_point;	    Y1 = window_y_point;
  X2 = window_x_points(0);  Y2 = window_y_points(0);
  if ( X1 > x  &&  Y1 > y  &&  X2 < (x + w)  &&  Y2 < (y + h) )
    enclosed = true;
  OUT(zipoline__Enclosed_Object);
  return  enclosed;
  }

long
zipoline__Object_Enclosure( self, figure, pane, x, y, w, h )
  register struct zipoline		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  register zip_type_pixel		 *x, *y, *w, *h;
  {
  zip_type_pixel			  X1, X2, X3, Y1, Y2, Y3;

  IN(zipoline__Object_Enclosure);
  Compute_Handle_Positions( self, figure, pane, &X1, &X2, &X3, &Y1, &Y2, &Y3 );
  *x = X1;  *y = Y1;  *w = abs(X3 - X1);  *h = abs(Y3 - Y1);
  OUT(zipoline__Object_Enclosure);
  return  zip_ok;
  }

long
zipoline__Highlight_Object_Points( self, figure, pane )
  register struct zipoline		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  {
  zip_type_pixel			  X1, X2, X3, Y1, Y2, Y3;
  register long				  status = zip_ok;

  IN(zipoline__Highlight_Object_Points);
  Compute_Handle_Positions( self, figure, pane, &X1, &X2, &X3, &Y1, &Y2, &Y3 );
  zipedit_Highlight_Handles( Edit, pane, X1, X2, X3, Y1, Y2, Y3 );
  OUT(zipoline__Highlight_Object_Points);
  return  status;
  }

long
zipoline__Normalize_Object_Points( self, figure, pane )
  register struct zipoline		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  {
  zip_type_pixel			  X1, X2, X3, Y1, Y2, Y3;
  register long				  status = zip_ok;

  IN(zipoline__Normalize_Object_Points);
  Compute_Handle_Positions( self, figure, pane, &X1, &X2, &X3, &Y1, &Y2, &Y3 );
  zipedit_Normalize_Handles( Edit, pane, X1, X2, X3, Y1, Y2, Y3 );
  OUT(zipoline__Normalize_Object_Points);
  return  status;
  }

long
zipoline__Expose_Object_Points( self, figure, pane )
  register struct zipoline		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  {
  register long				  status = zip_ok;

  IN(zipoline__Expose_Object_Points);
  zipedit_Expose_Point( Edit, pane, figure, figure_x_point,     figure_y_point );
  zipedit_Expose_Point( Edit, pane, figure, figure_x_points(0), figure_y_points(0) );
  OUT(zipoline__Expose_Object_Points);
  return  status;
  }

long
zipoline__Hide_Object_Points( self, figure, pane )
  register struct zipoline		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  {
  register long				  status = zip_ok;

  IN(zipoline__Hide_Points);
  zipedit_Hide_Point( Edit, pane, figure, figure_x_point,     figure_y_point );
  zipedit_Hide_Point( Edit, pane, figure, figure_x_points(0), figure_y_points(0) );
  OUT(zipoline__Hide_Points);
  return  status;
  }

static
Compute_Handle_Positions( self, figure, pane, X1, X2, X3, Y1, Y2, Y3 )
  register struct zipoline		 *self;
  register zip_type_figure		  figure;
  register zip_type_pane		  pane;
  register zip_type_pixel		 *X1, *X2, *X3, *Y1, *Y2, *Y3;
  {
  *X1 = zipview_X_Point_To_Pixel( View, pane, figure, figure_x_point );
  *X2 = *X1 + (window_x_points(0) - window_x_point)/2;
  *X3 = zipview_X_Point_To_Pixel( View, pane, figure, figure_x_points(0) );
  *Y1 = zipview_Y_Point_To_Pixel( View, pane, figure, figure_y_point );
  *Y2 = *Y1 + (window_y_points(0) - window_y_point)/2;
  *Y3 = zipview_Y_Point_To_Pixel( View, pane, figure, figure_y_points(0) );
  }

boolean
zipoline__Contains( self, figure, pane, x, y )
  register struct zipoline		*self;
  register zip_type_figure		figure;
  register zip_type_pane		pane;
  register zip_type_pixel		x, y;

  {
  register boolean			status = FALSE;
  register int				x1, y1, x2, y2;

  IN( zipoline__Contains )
  x1 = window_x_point;
  y1 = window_y_point;
  x2 = window_x_points(0);
  y2 = window_y_points(0);
  if (( x >= zipmin( x1, x2 )) && ( x <= zipmax( x1, x2 )) &&
      ( y >= zipmin( y1, y2 )) && ( y <= zipmax( y1, y2 )) &&
      (( y - y1 )*( x2 - x1 ) - ( y2 - y1 )*( x - x1 )) == 0 )
    status = TRUE;
  OUT( zipoline__Contains )
  return status;
  }
