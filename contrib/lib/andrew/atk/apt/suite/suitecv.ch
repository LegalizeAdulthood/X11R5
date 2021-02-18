/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidsuitecv_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apt/suite/RCS/suitecv.ch,v 1.3 1991/09/12 19:20:30 bobg Exp $";
#endif

/*
    $Log: suitecv.ch,v $
Revision 1.3  1991/09/12  19:20:30  bobg
Update copyright notice

Revision 1.2  1989/05/08  16:43:18  gk5g
changed references from suiteEV to suiteev

Revision 1.1  89/05/04  12:36:31  gk5g
Initial revision

Revision 1.1  89/04/28  20:26:19  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Suite-object

MODULE	suitecv.ch

VERSION: 0.0

AUTHOR	TC Peters & GW Keim
 	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Suite-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  11/01/88	Created (GW Keim)

END-SPECIFICATION  ************************************************************/

class suitecv: textview [textv] {
    classprocedures:
	InitializeClass() returns boolean;
	InitializeObject() returns boolean;
	FinalizeObject();
    overrides:
	PostKeyState(struct keystate *kstate);
	ReceiveInputFocus();
	LoseInputFocus();
    data:
	struct keystate *kstate;
	struct suite_item *parent_item;
	struct suiteev *parent_EV;
	int debug;
};

