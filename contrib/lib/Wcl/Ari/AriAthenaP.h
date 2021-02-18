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

/* Core */
#include <X11/Xaw/ClockP.h>
#include <X11/Xaw/LogoP.h>
#include <X11/Xaw/MailboxP.h>
#include <X11/Xaw/SimpleP.h>

/* Simple */
#include <X11/Xaw/GripP.h>
#include <X11/Xaw/LabelP.h>
#include <X11/Xaw/ListP.h>
#include <X11/Xaw/ScrollbarP.h>
#include <X11/Xaw/StripCharP.h>
#include <X11/Xaw/TextP.h>

/* Label */
#include <X11/Xaw/CommandP.h>
#include <X11/Xaw/MenuButtoP.h>
#include <X11/Xaw/ToggleP.h>

/* Sme */
#include <X11/Xaw/SmeP.h>
#include <X11/Xaw/SimpleMenP.h>
#include <X11/Xaw/SmeBSBP.h>
#include <X11/Xaw/SmeLineP.h>


/* Text */
#include <X11/Xaw/AsciiTextP.h>
#include <X11/Xaw/TextSrcP.h>
#include <X11/Xaw/AsciiSrcP.h>
#include <X11/Xaw/TextSinkP.h>
#include <X11/Xaw/AsciiSinkP.h>

/* Composite and Constraint */
#include <X11/Xaw/BoxP.h>
#include <X11/Xaw/FormP.h>
#include <X11/Xaw/PanedP.h>

/* Form */
#include <X11/Xaw/DialogP.h>
#include <X11/Xaw/ViewportP.h>

#endif
