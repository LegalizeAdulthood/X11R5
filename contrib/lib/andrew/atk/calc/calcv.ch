/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidcalcv_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/calc/RCS/calcv.ch,v 1.4 1991/09/12 19:25:33 bobg Exp $";
#endif

/*
    $Log: calcv.ch,v $
Revision 1.4  1991/09/12  19:25:33  bobg
Update copyright notice

Revision 1.3  1989/10/13  18:12:05  gk5g
Fix for Bug#64.  Calc captions not appearing on Sun3.  The problem was that the placement value for each caption was being stored in a char.  The possible range of values for the placement (from graphic.ch) is 0 - 0200.  The stored placement value was being clipped and then compared against a literal constant that exceeded that constant.  All captions were being drawn at x = 0 and were not visible because the ClipRect was being set.

Revision 1.2  89/08/04  17:09:35  tom
Accept keyboard inputs;
Suppress Shrink & Help Icons.

Revision 1.1  89/05/10  20:58:01  tom
Initial revision


*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Calcv Data-object

MODULE	calcv.ch

VERSION	1.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that suport the Calcv Data-object.

HISTORY
  02/23/88	Created (TCP)

END-SPECIFICATION  ************************************************************/

#define  calcv_VERSION		    1

struct calcv_states
  {
  unsigned int		   inputfocus		    : 1;
  unsigned int		   pending_display	    : 1;
  unsigned int		   pending_outline	    : 1;
  unsigned int		   point_present	    : 1;
  };

struct calcv_area_states
  {
  unsigned int		   highlighted		    : 1;
  };


struct calcv_area
  {
  char				     *string;
  struct fontdesc		     *font;
  struct rectangle		      bounds;
  char				      shape;
  int				      mode;
  void				    (*hit_handler)();
  struct calcv_setup		     *spec;
  struct calcv_area_states	      states;
  };


class calcv : aptv
  {
overrides:

  SetDataObject( struct calc *calc );
  FullUpdate( enum view_UpdateType type, long left, long top, long width, long height );
  Update();
  ObservedChanged( struct view *changed, long value );
  Hit( enum view_MouseAction action, long x, long y, long n )	returns struct view *;
  Print( FILE *file, char *processor, char *finalFormat, boolean topLevel );
  ReceiveInputFocus();
  LoseInputFocus();

methods:

classprocedures:

  InitializeClass()			    returns boolean;
  InitializeObject( struct calcv *self )    returns boolean;
  FinalizeObject( struct calcv *self );

data:

  struct calcv_states	  states;
  struct keystate	 *keystate;
  long			  area_count;
  struct calcv_area	  areas[20];
  struct rectangle	  bounds;
  char			  expression[50];
  char			  prior_expression[50];
  char			  operand_1[25], operand_2[25];
  char			  pending_op;
  };
