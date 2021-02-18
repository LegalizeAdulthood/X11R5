/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidcharthst_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/chart/RCS/charthst.ch,v 1.2 1991/09/12 19:26:54 bobg Exp $";
#endif

/*
    $Log: charthst.ch,v $
Revision 1.2  1991/09/12  19:26:54  bobg
Update copyright notice

Revision 1.1  1989/04/28  22:44:39  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Histogram Chart View-object

MODULE	charthst.ch

VERSION	0.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Chart View-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  03/28/89	Created (TCP)

END-SPECIFICATION  ************************************************************/

#define  charthst_VERSION		    1

class charthst : chartobj
  {
overrides:

  DrawChart();
  PrintChart();
  HitChart( action, x, y, clicks )	    returns struct view *;
  Moniker()				    returns char *;
  SetDebug( boolean state );

methods:

classprocedures:

  InitializeObject( struct charthst *self ) returns boolean;
  FinalizeObject();

data:

  };
