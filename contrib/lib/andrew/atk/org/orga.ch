/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/org/RCS/orga.ch,v 1.3 1991/09/12 19:45:34 bobg Exp $";
#endif

/*
    $Log: orga.ch,v $
Revision 1.3  1991/09/12  19:45:34  bobg
Update copyright notice

Revision 1.2  1989/05/02  21:16:20  tom
Added BackgroundShade Attribute;
Added SetHitHandler method;
Added NodeName method;
ReSpelled class-name from orga to orgapp.

Revision 1.1  89/05/01  16:23:52  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Org Application-Class

MODULE	orga.ch

VERSION	0.0

NOTICE	IBM Internal Use Only

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Org Application-Class.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  01/19/89	Created (TCP)
  05/02/89	Change class-name to orgapp (TCP)

END-SPECIFICATION  ************************************************************/

class orgapp [orga] : application [app]
  {
  classprocedures:
    InitializeObject( struct orga *)	    returns boolean;
    FinalizeObject();

  overrides:
    Start()				    returns boolean;
    ParseArgs( long argc, char **argv )	    returns boolean;

  data:
    char				    source[512];
    struct org				   *org_data_object;
    struct orgv				   *org_view_object;
    struct im				   *im;
    struct frame			   *frame;
  };
