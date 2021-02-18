/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apt/apt/RCS/apt.ch,v 1.4 1991/09/12 19:19:54 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apt/apt/RCS/apt.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidapt_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apt/apt/RCS/apt.ch,v 1.4 1991/09/12 19:19:54 bobg Exp $";
#endif
/*
    $Log: apt.ch,v $
Revision 1.4  1991/09/12  19:19:54  bobg
Update copyright notice

Revision 1.3  1990/04/11  14:11:06  gk5g
Removed initialization of variable debug from apt.h and put it in apt.c:apt__InitializeClass.  Create apt__InitializeClass.

Revision 1.2  89/05/24  19:44:52  tom
Add apt_LeftArea, etc.

Revision 1.1  89/04/28  17:45:45  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Apt Data-object

MODULE	apt.H

VERSION	0.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Apt Data-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  02/23/88	Created (TCP)
  05/23/89	Add apt_LeftArea, etc (TCP)

END-SPECIFICATION  ************************************************************/

#define  apt_VERSION    1

/*  Placements  */
#define  apt_Left		      (1<<0)
#define  apt_Center		      (1<<1)
#define  apt_Right		      (1<<2)
#define  apt_Top		      (1<<3)
#define  apt_Middle		      (1<<4)
#define  apt_Bottom		      (1<<5)

/*  Enclosure Areas  */
#define  apt_LeftArea		      (0)
#define  apt_TopArea		      (1)
#define  apt_RightArea		      (2)
#define  apt_BottomArea		      (3)

/*  Title/Legend Formatting Options  */
#define  apt_Spread		      (1<<0)
#define  apt_LeftRight		      (1<<1)

struct apt_strings
  {
  long				      strings_count;
  char				  *((*strings)[]);
  char				     *font_name;
  long				      mode;
  };

struct apt_field
  {
  char				     *name;
  char				     *content;
  };

struct apt_fields
  {
  long				      count;
  struct apt_field		      field[1];
  };

struct apt_field_contents
  {
  long				      count;
  struct apt_field		      field[1];
  };

class apt : dataobject[dataobj]
  {

overrides:

methods:

  SetAreaTitle( char *title, long area );
  SetAreaSpreadTitle( char **title, long count, long area, mode );
  SetAreaTitleFontName( char *font, long area );
  SetAreaLegend( char *legend, long area  );
  SetAreaSpreadLegend( char **legend, long count, long area, mode );
  SetAreaLegendFontName( char *font, long area );
  ReadObject( FILE *file, long id, reader )    returns long;
  ReadObjectField()				returns struct apt_field *;
  ParseFieldContent( char *string )		returns char **;
  ParseFieldContents( char *string )		returns struct apt_field_contents *;
  WriteObject( FILE *file, long id, long level, writer );
  WriteObjectField(struct apt_field *field);

macromethods:

  AreaTitlesAnchor( area )	    ((self)->titles[area].strings)
  AreaTitlesCount( area )	    ((self)->titles[area].strings_count)
  AreaTitle( area )		    (((self)->titles[area].strings) ?\
					 (*(self)->titles[area].strings)[0] : NULL)
  AreaTitles( area, i )		    ((*(self)->titles[area].strings)[i])
  AreaTitleFontName( area )	    ((self)->titles[area].font_name)
  AreaLegendsAnchor( area )	    ((self)->legends[area].strings)
  AreaLegendsCount( area )	    ((self)->legends[area].strings_count)
  AreaLegend( area )		    (((self)->legends[area].strings) ?\
					 (*(self)->legends[area].strings)[0] : NULL)
  AreaLegends( area, i )	    ((*(self)->legends[area].strings)[i])
  AreaLegendFontName( area )	    ((self)->legends[area].font_name)

classprocedures:

  InitializeObject( struct apt *self )		returns boolean;
  FinalizeObject( struct apt *self );
  InitializeClass()				returns boolean;

data:

  struct apt_strings		      titles[4];
  struct apt_strings		      legends[4];
  long				      id;
  FILE				     *field_file;
  long				      field_index;
  struct apt_fields		     *fields;
  };
