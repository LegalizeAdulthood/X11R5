/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The LightTable View-object Specification

MODULE	ltv.c

NOTICE	IBM Internal Use Only

DESCRIPTION
	This defines the suite of Methods that support the LightTable View-object.

HISTORY
  010/10/88	Created (TCP)

END-SPECIFICATION  ************************************************************/

#define  ltv_VERSION		    1


class ltv : view
  {
overrides:

  DesiredSize( long width, long height, enum view_DSpass pass, long *dwidth, long *dheight )
	returns enum view_DSattributes;
  FullUpdate( enum view_UpdateType type, long left, long top, long width, long height );
  Update();
  ObservedChanged( struct view *changed, long value );
  Hit( enum view_MouseAction action, long x, long y, long n )
	returns struct view *;
  SetDataObject( struct lt * );
  ReceiveInputFocus();
  LoseInputFocus();

methods:

  Set_Debug( debug );

classprocedures:

  InitializeClass( struct classheader *classID )			    returns boolean;
  InitializeObject( struct classheader *classID, struct ltv *self )   returns boolean;
  FinalizeObject( struct classheader *classID, struct ltv *self );

data:

  struct lt			    *data;
  struct zipview		    *zipview;
  struct zip_pane		    *background_pane, *foreground_pane;
  struct rasterview		    *rasterview;
  struct suite			    *buttons;
  struct suite_item		    *left_name_item, *right_name_item;
  struct rectangle		     block, enclosure;
  struct menulist		    *menu;
  struct cursor			    *cursor;
  unsigned char			     background_light;
  boolean			     background_exposed;
  boolean			     enclosure_exposed;
  boolean			     building;
  boolean			     build;
  boolean			     tracking;
  boolean			     modifying;
  boolean			     modified;
  boolean			     foreground_panning;
  boolean			     input_focus;
  struct zip_stream		    *stream;
  struct zip_image		    *image;
  struct zip_figure		    *figure;
  long				     point;
  long				     initial_x_pixel, initial_y_pixel;
  long				     background_left, background_top,
				     background_x_offset, background_y_offset;
  unsigned char			     wm_type;
  char				     tentative[512];
  };
