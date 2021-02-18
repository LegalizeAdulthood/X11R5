/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/zip/lib/RCS/zipobj.ch,v 2.10 1991/09/12 20:08:07 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/zip/lib/RCS/zipobj.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/zip/lib/RCS/zipobj.ch,v 2.10 1991/09/12 20:08:07 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 * P_R_P_Q_# (C) COPYRIGHT IBM CORPORATION 1988
 * LICENSED MATERIALS - PROPERTY OF IBM
 * REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
 */
/*
zipobject.ch

  03/31/88	Create for ATK (TCP)
  07/12/89	Added Object_Visible method (SCG)
  08/07/89	Add Object_Modified method (TCP)
   08/14/90	Add Contains stub. Remove Object_Visible stub (SCG)

*/

#define  zipobject_VERSION    1

#include "zip.ih"
#include "zipv.ih"
#include "zipedit.ih"
#include "zipprint.ih"


#define  CurrentFigure			((pane)->zip_pane_current_figure)
#define  CurrentImage			((pane)->zip_pane_current_image)
#define  CurrentStream			((pane)->zip_pane_current_stream)

class zipobject[zipobj]
  {

methods:

  Set_Debug( state );
  Set_Data_Object( data_object );
  Set_View_Object( view_object );
  Set_Edit_Object( edit_object );
  Set_Print_Object( print_object );

  Object_DesiredSize( width, height, pass, dWidth, dHeight )
							returns enum view_DSattributes;
  Object_FullUpdate( type, left, top, width, height );
  Object_Update();
  Object_Hit( object, action, x, y, clicks )		returns struct view *;

  Object_Icon()						returns char;
  Object_Icon_Font_Name()				returns char *;
  Object_Icon_Cursor()					returns char;
  Object_Icon_Cursor_Font_Name()			returns char *;
  Object_Datastream_Code()				returns char;
  Object_Modified( object )				returns long;

  Build_Object( pane, action, x, y, clicks, X, Y )	returns long;
  Destroy_Object( object );
  Show_Object_Properties( pane, figure )		returns long;
  Read_Object( object )					returns long;
  Read_Object_Stream( object, file, id )		returns long;
  Write_Object( object )				returns long;
  Draw_Object( object, pane )				returns long;
  Clear_Object( object, pane )				returns long;
  Print_Object( object, pane )				returns long;
  Proximate_Object_Points( object, pane, x, y )		returns long;
  Within_Object( object, pane, x, y )			returns long;
  Enclosed_Object( object, pane, x, y, w, h )		returns boolean;
  Object_Enclosure( object, pane, *x, *y, *w, *h )	returns long;
  Highlight_Object_Points( object, pane )		returns long;
  Normalize_Object_Points( object, pane )		returns long;
  Expose_Object_Points( object, pane )			returns long;
  Hide_Object_Points( object, pane )			returns long;
  Set_Object_Point( object, point, x, y )		returns long;
  Object_Point( object, point, *x, *y )			returns long;
  Adjust_Object_Point_Suite( object, x_delta, y_delta )	returns long;
  Set_Object_Shade( object, shade )			returns long;
  Set_Object_Font( object, font )			returns long;
  Contains( figure, pane, x, y )			returns boolean;

classprocedures:

  InitializeObject( struct zipobject *self )		returns boolean;

data:

  struct zip			 *data_object;
  struct zipview		 *view_object;
  struct zipedit		 *edit_object;
  struct zipprint		 *print_object;
  };

