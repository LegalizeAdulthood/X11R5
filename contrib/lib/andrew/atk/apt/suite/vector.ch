/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidvector_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apt/suite/RCS/vector.ch,v 1.2 1991/09/12 19:20:39 bobg Exp $";
#endif

/*
    $Log: vector.ch,v $
Revision 1.2  1991/09/12  19:20:39  bobg
Update copyright notice

Revision 1.1  1989/04/28  20:26:46  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Vector-object

MODULE	vector.ch

VERSION	0.0

AUTHOR	GW Keim
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Vector-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  04/10/89	Created (GW Keim)

END-SPECIFICATION  ************************************************************/

#define vector_status_no_error			    0
#define vector_status_not_in_vector		    1
#define vector_status_end_of_vector		    3
#define vector_status_root_of_vector		    4
#define vector_status_empty			    5
#define vector_status_no_sort_routine		    6
#define vector_status_null_element		    7

class vector {
    classprocedures:
	InitializeObject( struct vector *self ) returns boolean;
	FinalizeObject( struct vector *self ) returns void;
	Create(long init_data_count, long reallocfactor ) returns struct vector*;
    macromethods:
	SetDestroyRoutine(routine) (self->destroyer=(long(*)())routine)
	SetSortRoutine(routine) (self->sorter=(long(*)())routine)
	Item(i) (self->data[i])
	Count() (self->current_used_count)
	SetIncrement(inc) (self->reallocation_factor = inc)
    methods:
	AddItem( long item ) returns long;
	RemoveItem( long item ) returns long;
	ItemExists( long item ) returns boolean;
	Sort() returns long;
	Subscript( long item ) returns long;
	Apply( long (*proc)() ) returns void;
    data:
	long		*data,
			 initial_vector_count,
			 current_vector_count,
			 current_used_count,
			 reallocation_factor,
			 debug,
		       (*apply)(),
		       (*sorter)(), 
		       (*destroyer)();
};

