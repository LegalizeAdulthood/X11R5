/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidchartstk_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/chart/RCS/chartstk.ch,v 1.2 1991/09/12 19:27:12 bobg Exp $";
#endif

/*
    $Log: chartstk.ch,v $
Revision 1.2  1991/09/12  19:27:12  bobg
Update copyright notice

Revision 1.1  1989/04/28  22:44:47  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Stack Chart View-object

MODULE	chartstk.ch

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

#define  chartstk_VERSION		    1

class chartstk : chartobj
  {
overrides:

  DrawChart();
  PrintChart();
  HitChart( action, x, y, clicks )	    returns struct view *;
  SetDebug( boolean state );

methods:

classprocedures:

  InitializeObject( struct chartstk *self ) returns boolean;
  FinalizeObject();

data:

  };
