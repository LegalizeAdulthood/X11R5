/* $Header: /usr/src/local/emu/misc/RCS/rdb.c,v 1.4 90/11/20 17:34:26 jkh Exp Locker: jkh $ */

/*
 * This file is part of the PCS emu system.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * Useful functions for dealing with the resource Database
 *
 * Author: Michael Elbel
 * Date: 19. Juni 1990
 * Description: This module contains some routines for dealing with the
 * 		resource database.
 *
 * Revision History:
 *
 * $Log:	rdb.c,v $
 * Revision 1.4  90/11/20  17:34:26  jkh
 * Alpha.
 * 
 * Revision 1.3  90/10/04  00:31:18  jkh
 * Removed warning message; it's not always wanted.
 * 
 * Revision 1.2  90/07/26  02:45:43  jkh
 * Added new copyright.
 * 
 * Revision 1.1  90/07/09  02:21:53  jkh
 * Initial revision
 * 
 *
 */

#include "common.h"
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

/*
 * Get Subresources for the given emulation as string
 *
 * The Subresource path starts at the parent of the current widget using.
 * the current emulation's name (term_type), the different parts of
 * the Subresource are delimited by dashes.
 *
 * So Resources would look like *.term.<term_type>-<name>
 */
Export String get_sub_resource_string(w, term_type, name, class)
Widget w;
String term_type, name, class;
{
     String retval;
     char nbuffer[100];
     char cbuffer[100];
     
     XtResource res;
     
     res.resource_name = nbuffer;
     res.resource_class = cbuffer;
     res.resource_type = XtRString;
     res.resource_size = sizeof(String);
     res.resource_offset = 0;
     res.default_type = XtRImmediate;
     res.default_addr = NULL;

     if (term_type) {
	  strcpy(nbuffer, term_type);
	  strcpy(cbuffer, term_type);
     
	  strcat(nbuffer, "-");
	  strcat(cbuffer, "-");
     }
     else
	  nbuffer[0] = cbuffer[0] = '\0';
     
     strcat(nbuffer, name);
     strcat(cbuffer, class);
     
     XtGetSubresources(XtParent(w), &retval, "term", "Term",
		       &res, 1, NULL, 0);
     return retval;
}
