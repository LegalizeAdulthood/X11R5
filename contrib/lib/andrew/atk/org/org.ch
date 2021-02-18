/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidorg_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/org/RCS/org.ch,v 1.5 1991/09/12 19:45:31 bobg Exp $";
#endif

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Org Data-Class

MODULE	org.ch

VERSION	1.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Org Data-Class.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  01/19/89	Created (TCP)
  08/23/89	Remove Create method (TCP)

END-SPECIFICATION  ************************************************************/


#define  org_VERSION	      1

class org : apt
  {
overrides:

  Read( FILE *file, long id )				returns long;
  Write( FILE *file, long id, long level )		returns long;
  ViewName()						returns char *;

methods:

  SetDebug( boolean state );
  NodeName( node )					returns char *;

classprocedures:

  InitializeObject( struct org *self )			returns boolean;
  FinalizeObject( struct org *Self );

data:

  struct tree		  	 *tree_data_object;
  };
