/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidchartapp_ch = "$Header $";
#endif

/*
    $Log: chartapp.ch,v $
Revision 1.2  1991/09/12  19:26:42  bobg
Update copyright notice

Revision 1.1  1989/04/28  22:44:33  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Chart Application-object

MODULE	chartapp.ch

VERSION	0.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that suport the Chart Application-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  02/23/88	Created (TCP)

END-SPECIFICATION  ************************************************************/

class chartapp: application [app]
  {
  classprocedures:
    InitializeObject( struct chartapp *)	    returns boolean;
    FinalizeObject();

  overrides:
   Start()				    returns boolean;
   ParseArgs( long argc, char **argv )	    returns boolean;

  data:
    char				    source[512];
    struct chart			   *chart_data_object;
    struct chartv			   *chart_view_object;
    struct im				   *im;
    struct frame			   *frame;
  };
