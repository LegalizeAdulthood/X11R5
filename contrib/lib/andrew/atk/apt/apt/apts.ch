/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apt/apt/RCS/apts.ch,v 1.3 1991/09/12 19:20:03 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apt/apt/RCS/apts.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidapts_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apt/apt/RCS/apts.ch,v 1.3 1991/09/12 19:20:03 bobg Exp $";
#endif
/*
    $Log: apts.ch,v $
Revision 1.3  1991/09/12  19:20:03  bobg
Update copyright notice

Revision 1.2  1989/05/18  22:45:00  tom
Add more misc tool items.

Revision 1.1  89/05/18  20:31:13  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Apt Tool Set

MODULE	apts.ch

VERSION	0.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Procedures that support the Apt Tool Set.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  05/18/89	Created (TCP)

END-SPECIFICATION  ************************************************************/

#define  apts_VERSION    1

package apts
  {

classprocedures:

  CompareStrings( char *s1, char *s2 )			returns long;
  SubstringIndex( char *pattern, char *string )		returns long;
  StripString( char *string)				returns char *;
  CaptureString( char *source, char *target )		returns long;

  HourMinuteSecond( long *hour, long *minute, long *second );
  HourOfDay( char *hour );
  MinuteOfHour( char *minute );
  SecondOfMinute( char *second );

  YearMonthDay( long *year, long *month, long *day )	returns long;

  DaysInMonth( long year, long month )			returns long;
  WeekDayOffset( long year, long month, long day )	returns long;

  };
