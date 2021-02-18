/* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *circpi_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/demos/circlepi/RCS/circpi.c,v 1.2 1991/09/12 16:50:04 bobg Exp $";


#include <stdio.h>
#include <math.h>
#include <circpi.eh>
#include <andrewos.h>
#include <observe.ih>

/* Defined constants and macros */

/* External declarations */

/* Forward Declarations */

/* Global variables */


boolean
circlepi__InitializeClass(c)
struct classheader *c;
{
/* 
  Initialize all the class data.
*/
  return(TRUE);
}


boolean
circlepi__InitializeObject(c, self)
struct classheader *c;
struct circlepi *self;
{
/*
  Inititialize the object instance data.
*/
    self->depth_limit = 4;
    return(TRUE);
}


void
circlepi__FinalizeObject(c, self)
struct classheader *c;
struct circlepi *self;
{
/*
  Finalize the object instance data.
*/
  return;
}


void
circlepi__SetDepth(self, limit)
     struct circlepi *self;
     int limit;
{
    if (self->depth_limit != limit) {
	self->depth_limit = limit;
	circlepi_NotifyObservers(self, observable_OBJECTCHANGED);
    }
}
