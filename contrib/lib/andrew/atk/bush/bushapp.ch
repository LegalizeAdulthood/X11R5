/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#ifndef lint
static char *rcsidbushapp_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/bush/RCS/bushapp.ch,v 1.3 1991/09/12 19:25:16 bobg Exp $";
#endif
/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	Bush Application

MODULE	bushapp.ch

VERSION	0.0

AUTHOR	TC Peters & GW Keim
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the Application layer for the Bush Inset..

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...

HISTORY
  08/21/85	Created (TCP)
  01/15/89	Convert to ATK (GW Keim)

END-SPECIFICATION  ******************************************************/

class bushapp : application [app] {
  classprocedures:
    InitializeObject( struct bushapp *self ) returns boolean;
    FinalizeObject( struct bushapp *self );
  overrides:
    ParseArgs( int argc, char **argv ) returns boolean;
    Start() returns boolean;
  data:
    struct im	    *im;
    struct frame    *frame;
    struct bush	    *bush;
    struct bushv    *bushview;
    int		     argc;
    char	   **argv;
    int		     debug;
};
