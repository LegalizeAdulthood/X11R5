/*

Copyright 1991 by OTC Limited


Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of OTC Limited not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission. OTC Limited makes no representations about the
suitability of this software for any purpose. It is provided "as is"
without express or implied warranty.

Any person supplied this software by OTC Limited may make such use of it
including copying and modification as that person desires providing the
copyright notice above appears on all copies and modifications including
supporting documentation.

The only conditions and warranties which are binding on OTC Limited in
respect of the state, quality, condition or operation of this software
are those imposed and required to be binding by statute (including the
Trade Practices Act 1974). and to the extent permitted thereby the
liability, if any, of OTC Limited arising from the breach of such
conditions or warranties shall be limited to and completely discharged
by the replacement of this software and otherwise all other conditions
and warranties whether express or implied by law in respect of the
state, quality, condition or operation of this software which may
apart from this paragraph be binding on OTC Limited are hereby
expressly excluded and negatived.

Except to the extent provided in the paragraph immediately above OTC
Limited shall have no liability (including liability in negligence) to
any person for any loss or damage consequential or otherwise howsoever
suffered or incurred by any such person in relation to the software
and without limiting the generality thereof in particular any loss or
damage consequential or otherwise howsoever suffered or incurred by
any such person caused by or resulting directly or indirectly from any
failure, breakdown, defect or deficiency of whatsoever nature or kind
of or in the software.

*/

/*
 *  File: widgeti.h
 *
 *  Description: Prototypes and defines for widgeti
 */

#ifndef WIDGET_H
#define WIDGET_H

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/List.h>

#include    "common.h"

#define MaxNumXArgs 20

#define AddArg(arg, val)						    \
	XtSetArg(arglist[numargs], arg, (XtArgVal)val), numargs ++

#define DeclareArgs							    \
static Arg			arglist[MaxNumXArgs];			    \
static Cardinal			numargs;

/* function prototypes: */
/* widgeti.c: */
Global Widget init_widgeti P((char *name , char *app_class , int *argcp
, char **argv ));
Global Widget create_form P((Widget parent , Widget fromH , Widget
fromV , int borderWidth ));
Global Widget create_label P((Widget parent , Widget fromH , Widget
fromV , int width , int borderWidth , char *string ));
Global Widget create_command_button P((Widget parent , Widget fromH ,
Widget fromV , char *string , VoidCallback callback , Pointer data ));
Global Widget create_one_of_many_toggle P((char *label , Widget parent
, Widget fromH , Widget fromV , char *strings , VoidCallback callback ));
Global void set_toggle_state P((Widget widget , char *data ));
Global char *get_toggle_state P((Widget widget ));
Global Widget create_viewlist P((Widget parent , Widget fromH , Widget
fromV , char **list , int width , int height , int borderWidth ,
VoidCallback callback , Pointer data ));
Global void set_viewlist P((Widget widget , char **list , int nitems ,
int longest ));

/* end function prototypes */

#endif	/* WIDGET_H */

