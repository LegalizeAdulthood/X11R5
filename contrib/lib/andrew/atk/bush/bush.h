/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidbush_h = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/bush/RCS/bush.h,v 1.7 1991/09/12 19:25:09 bobg Exp $";
#endif

/*
    $Log: bush.h,v $
 * Revision 1.7  1991/09/12  19:25:09  bobg
 * Update copyright notice
 *
 * Revision 1.6  1991/07/22  03:52:09  gk5g
 * #include <afs/param.h> for Ultrix4.x
 *
 * Revision 1.5  1991/04/02  16:54:02  susan
 * added SCO Unix support
 *
 * Revision 1.4  90/04/27  12:11:34  gk5g
 * Sweeping revisions including: (1) reformated, (2) added editing capability while viewing a file, (3) cleaned up unused variables, (4) additional information added to the ControlPanel (sort, detail, editor).
 * 
 * Revision 1.3  90/03/14  15:29:29  gk5g
 * macros for improved readability.
 * 
 * Revision 1.3  90/03/14  14:36:39  gk5g
 * Some added macros for improved readability.
 * 
 * Revision 1.2  89/09/01  13:30:33  gk5g
 * Made change from tree_NodeData to tree_NodeDatum.
 * 
 * Revision 1.1  89/04/28  21:24:00  tom
 * Initial revision
 * 
 * Revision 1.1  89/04/28  21:20:38  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	Bush Internal Symbolics

MODULE	bush.h

VERSION	0.0

AUTHOR	TC Peters & GW Keim
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This internal symbolics for the Bush modules.

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

END-SPECIFICATION  ************************************************************/
#include <stdio.h>
#include <class.h>
#include <andrewos.h>
#include <pwd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <ctype.h>
#ifdef M_UNIX
#include <dirent.h>
#define direct dirent
#else
#include <sys/dir.h>
#endif
#include <errno.h>
#ifdef AFS_ENV
#include <afs/param.h>
#include <util.h>		    /*getv*(), getc*() routine family*/
#ifdef WHITEPAGES_ENV
#include <wp.h>			    /*White Pages*/
#endif /* WHITEPAGES_ENV */
#include <afs/vice.h>
#include <afs/venus.h>
#endif /* AFS_ENV */
#include <apt.h>




