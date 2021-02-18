/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidchartdot_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/chart/RCS/chartdot.ch,v 1.3 1991/09/12 19:26:51 bobg Exp $";
#endif

/*
    $Log: chartdot.ch,v $
Revision 1.3  1991/09/12  19:26:51  bobg
Update copyright notice

Revision 1.2  1989/06/02  16:53:12  tom
Provide default HitHandler.

Revision 1.1  89/04/28  22:44:37  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Dot Chart View-object

MODULE	chartdot.ch

VERSION	0.0

NOTICE	IBM Internal Use Only

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
  06/02/89	Use super-class Hit method (TCP)

END-SPECIFICATION  ************************************************************/

#define  chartdot_VERSION		    1

class chartdot : chartobj
  {
overrides:

  DrawChart();
  PrintChart();
  Moniker()				    returns char *;
  SetDebug( boolean state );

methods:

classprocedures:

  InitializeObject( struct chartdot *self ) returns boolean;
  FinalizeObject();

data:

  };
