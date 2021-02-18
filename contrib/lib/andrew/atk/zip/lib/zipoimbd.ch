/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/zip/lib/RCS/zipoimbd.ch,v 2.9 1991/09/12 20:08:30 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/zip/lib/RCS/zipoimbd.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/zip/lib/RCS/zipoimbd.ch,v 2.9 1991/09/12 20:08:30 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 * P_R_P_Q_# (C) COPYRIGHT IBM CORPORATION 1988
 * LICENSED MATERIALS - PROPERTY OF IBM
 * REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
 */
/*
zipoimbed.H

  03/31/88	Create for ATK (TCP)
  08/07/89	Add Object_Modified override (TCP)
*/

#define  zipoimbed_VERSION    1

#define  Data				(self->header.zipobject.data_object)
#define  Env				(Data->env)
#define  View				(self->header.zipobject.view_object)
#define  Edit				(self->header.zipobject.edit_object)
#define  Print				(self->header.zipobject.print_object)


class zipoimbed[zipoimbd] : ziporect
  {

overrides:

  Object_Icon()						returns char;
  Object_Icon_Cursor()					returns char;
  Object_Datastream_Code()				returns char;

  Object_Hit( object, action, x, y, clicks )		returns struct view *;
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

classprocedures:
  InitializeObject( struct zipoimbed *self )		returns boolean;

data:
  boolean						no_outline;

  };

