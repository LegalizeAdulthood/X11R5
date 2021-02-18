/* $Header: XpexQueryExt.c,v 2.2 91/09/11 16:06:06 sinyaw Exp $ */
/*
 * QueryExt.c 
 */

/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

#include "Xpexlibint.h"

Bool
XpexQueryExtension(dpy, major_opcode, first_error, first_event)
	register Display  *dpy;
	int  *major_opcode;  /* RETURN */
	int  *first_error;  /* RETURN */
	int  *first_event;  /* RETURN */
{
    Bool status = XQueryExtension(dpy, PEX_NAME_STRING,
	major_opcode, first_event, first_error);

    return status;
}
