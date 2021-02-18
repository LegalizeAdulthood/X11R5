/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Sched View-object Specification

MODULE	schedv.c

NOTICE	IBM Internal Use Only

DESCRIPTION
	This defines the suite of Methods that support the LightTable View-object.

HISTORY
  01/20/89	Created (TCP)

END-SPECIFICATION  ************************************************************/

#define  schedv_VERSION		    1


class schedv : view
  {
overrides:

  FullUpdate( enum view_UpdateType type, long left, long top, long width, long height );
  Hit( enum view_MouseAction action, long x, long y, long n )
	returns struct view *;
  SetDataObject( struct sched * );
  ReceiveInputFocus();
  LoseInputFocus();

methods:

  Set_Debug( debug );

classprocedures:

  InitializeClass( struct classheader *classID )			    returns boolean;
  InitializeObject( struct classheader *classID, struct schedv *self )   returns boolean;
  FinalizeObject( struct classheader *classID, struct schedv *self );

data:

  struct sched			    *data;
  struct zipview		    *zipview;
  struct zip_pane		    *chart_pane;
  struct suite			    *control_buttons;
  struct rectangle		     block, chart_block;
  struct menulist		    *menu;
  struct cursor			    *cursor;
  char				     line_width, line_style, figure_type;
  boolean			     tracking, modified, input_focus,
				     pending_question, pending_duplicate;
  struct zip_stream		    *stream;
  struct zip_image		    *image;
  struct zip_figure		    *figure,
				    *current_slot_figure,  *current_text_figure,
				    *previous_slot_figure, *previous_text_figure;
  long				     point;
  long				     initial_x_pixel, initial_y_pixel;
  unsigned char			     wm_type;
  };
