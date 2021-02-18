/*
** Copyright (c) 1990 David E. Smyth
** 
** Redistribution and use in source and binary forms are permitted
** provided that the above copyright notice and this paragraph are
** duplicated in all such forms and that any documentation, advertising
** materials, and other materials related to such distribution and use
** acknowledge that the software was developed by David E. Smyth.  The
** name of David E. Smyth may not be used to endorse or promote products
** derived from this software without specific prior written permission.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
** 
*/

/*
* SCCS_data: @(#)AriAthenaP.h 1.1 ( 19 Nov 90 )
*/

#ifndef _AriAthenaP_h_
#define _AriAthenaP_h_

/* Core, Object, RectObj, WindowObj, 
** XmGadget, XmPrimitive, and XmComposite, 
** Shell, OverrideShell, WMShell, VendorShell, TopLevelShell, ApplicationShell, 
** Constraint, XmManager.
*/
#include <X11/IntrinsicP.h>

/* include all the *P.h files in heirarchical order */

#include <X11/CoreP.h>
#include <X11/ObjectP.h>
#include <X11/Xaw/SimpleP.h>
#include <X11/CompositeP.h>
#include <X11/ConstrainP.h>

/* Xcu Privates */
#include <Xcu/BmgrP.h>
#include <Xcu/ButtonP.h>
#include <Xcu/CommandP.h>
#include <Xcu/DeckP.h>
#include <Xcu/LabelP.h>
#include <Xcu/RcP.h>
#include <Xcu/TblP.h>
#include <Xcu/EntryP.h>
/* #include <Xcu/WlmP.h> */

#endif
