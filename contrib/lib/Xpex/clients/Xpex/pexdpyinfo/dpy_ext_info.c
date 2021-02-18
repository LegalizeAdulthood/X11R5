/* $Header: dpy_ext_info.c,v 1.1 91/09/18 13:50:03 sinyaw Exp $ */

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

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "Xpexlib.h"

void
dpy_ext_info(dpy, pex_codes, info)
	Display *dpy;
	XExtCodes *pex_codes;
	XpexExtensionInfo *info;
{
	(void) fprintf(stderr, "Extension Information\n\n");

	(void) fprintf(stderr, 
	"\tmajor_opcode: %d\n", pex_codes->major_opcode);

	(void) fprintf(stderr, 
	"\tfirst_event: %d\n", pex_codes->first_event);

	(void) fprintf(stderr, 
	"\tfirst_error: %d\n", pex_codes->first_error);

	(void) fprintf(stderr, "\tversion number: %d.%d\n", 
	info->proto_major, info->proto_minor);

	(void) fprintf(stderr, 
	"\tvendor string: %s\n",info->vendor);

	(void) fprintf(stderr, 
	"\tvendor release number: %d\n", info->release_number);

	(void) fprintf(stderr, 
	"\tsubset info: %d\n\n", info->subset_info);
}
