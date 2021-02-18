/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *calc_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/calc/RCS/calc.c,v 1.4 1991/09/12 16:02:50 bobg Exp $";


/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidcalc = "$Header $";
#endif

/*
    $Log: calc.c,v $
 * Revision 1.4  1991/09/12  16:02:50  bobg
 * Update copyright notice and rcsid
 *
 * Revision 1.3  1989/08/04  17:08:38  tom
 * Accept keyboard inputs;
 * Suppress Shrink & Help Icons.
 *
 * Revision 1.2  89/05/26  18:24:20  tom
 * Use apt_LeftArea, etc.
 * 
 * Revision 1.1  89/05/10  20:58:06  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Calc Data-object

MODULE	calc.c

VERSION	1.0

DESCRIPTION
	This is the suite of Methods that suport the Calc Data-object,
	a trivial example of an ATK Inset.

HISTORY
  02/23/88	Created (TCP)
  05/26/89	Use apt_LeftArea, etc (TCP)

END-SPECIFICATION  ************************************************************/

#include <andrewos.h>
#include "dataobj.ih"
#include "apt.h"
#include "apt.ih"
#include "calc.eh"

#define  Value			      self->value


boolean
calc__InitializeObject( classID, self )
  register struct classheader	     *classID;
  register struct calc		     *self;
  {
  IN(calc_InitializeObject);
  DEBUGst(RCSID,rcsidcalc);
  calc_SetAreaTitleFontName( self, "AndySans22b", apt_TopArea );
  calc_SetAreaLegendFontName( self, "Andy20i", apt_BottomArea );
  Value = 0;
  OUT(calc_InitializeObject);
  return TRUE;
  }

void
calc__FinalizeObject( classID, self )
  register struct classheader	     *classID;
  register struct calc		     *self;
  {
  IN(calc_FinalizeObject);
  DEBUGst(RCSID,rcsidcalc);
  /* Nop */
  OUT(calc_FinalizeObject);
  }

char *
calc__ViewName( self )
  register struct calc    	      *self;
  {
  IN(calc_ViewName);
  OUT(calc_ViewName);
  return "calcv";
  }

void
calc__SetValue( self, value )
  register struct calc    	      *self;
  register double		       value;
  {
  IN(calc__SetValue);
  Value = value;
  calc_SetModified( self );
  OUT(calc__SetValue);
  }

static
Reader( self )
  register struct calc	    	     *self;
  {
  register struct apt_field	     *field;

  IN(Reader);
  while ( field = calc_ReadObjectField( self ) )
    {
    if ( strcmp( "Value", field->name ) == 0 )
       sscanf( field->content, "%F", &Value );
    }
  OUT(Reader);
  }

long
calc__Read( self, file, id )
  register struct calc	    	     *self;
  register FILE			     *file;
  register long			      id;
  {
  register long			      status; 

  IN(calc__Read);
  if ( (status = calc_ReadObject( self, file, id, Reader )) ==
	dataobject_NOREADERROR )
    {
    calc_NotifyObservers( self, calc_value_changed );
    }
  OUT(calc__Read);
  return status;
  }

static
Writer( self )
  register struct calc		     *self;
  {
  struct apt_field		      field;
  char				      value[25];

  IN(Writer);
  field.name = "Value";
  field.content = (char *)sprintf( value, "%f", Value );
  calc_WriteObjectField( self, &field );
  OUT(Writer);
  }

long
calc__Write( self, file, id, level )
  register struct calc		     *self;
  register FILE			     *file;
  register long			      id;
  register int			      level;
  {
  IN(calc_Write);
  calc_WriteObject( self, file, id, level, Writer );
  OUT(calc_Write);
  return  id;
  }
