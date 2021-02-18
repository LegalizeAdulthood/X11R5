/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/zip/lib/RCS/zipstat.ch,v 2.7 1991/09/12 20:09:18 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/zip/lib/RCS/zipstat.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidzipview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/zip/lib/RCS/zipstat.ch,v 2.7 1991/09/12 20:09:18 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 * P_R_P_Q_# (C) COPYRIGHT IBM CORPORATION 1988
 * LICENSED MATERIALS - PROPERTY OF IBM
 * REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
 */
/*
zipstatus.H

  04/15/88	Created for ATK (TCP)
*/

#define  zipstatus_VERSION		    1

struct zipstatus_options
  {
  unsigned int				    xxxx		: 1;
  };

struct zipstatus_states
  {
  unsigned int				    xxxx		: 1;
  };


class zipstatus[zipstat] : zipview[zipv]
  {
overrides:

methods:

  Issue_Message( char *msg )					    returns long;
  Acknowledge_Message( char *msg )				    returns long;
  Clear_Message()						    returns long;
  Issue_Status_Message( char *msg )				    returns long;
  Acknowledge_Status_Message( char *msg )			    returns long;
  Issue_Figure_Status_Message( figure )				    returns long;
  Acknowledge_Figure_Status_Message( figure )			    returns long;
  Issue_Image_Status_Message( image )				    returns long;
  Acknowledge_Image_Status_Message( image )			    returns long;
  Issue_Stream_Status_Message( stream )				    returns long;
  Acknowledge_Stream_Status_Message( stream )			    returns long;
  Issue_Pane_Status_Message( pane )				    returns long;
  Acknowledge_Pane_Status_Message( pane )			    returns long;
macromethods:

classprocedures:

  InitializeObject( struct zipstatus *self )			    returns boolean;
  FinalizeObject( struct zipstatus *self );


data:

  struct zipstatus_options	 options;
  struct zipstatus_states	 states;
  };
