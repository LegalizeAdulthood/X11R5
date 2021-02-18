/* $Header: XpexInit.c,v 2.2 91/09/11 16:06:08 sinyaw Exp $ */
/* 
 * XpexInit.c
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

#include <X11/extensions/Xext.h>
#include "Xpexlibint.h"
#include "Xpexlib.h"
#include "extutil.h"

static XExtensionInfo *pex_ext_info;

/* NOTE: for now, set all to NULL */
static XExtensionHooks pex_extension_hooks = {
	NULL, /* create_gc */
	NULL, /* copy_gc */
	NULL, /* flush_gc */
	NULL, /* free_gc */
	NULL, /* create_font */
	NULL, /* free_font */
	NULL, /* close_display */
	NULL, /* wire_to_event */
	NULL, /* event_to_wire */
	NULL, /* error */
	NULL  /* error_string */
};

XExtCodes *
XpexInitialize(dpy)
	Display *dpy;
{
	XExtDisplayInfo *ext_dpy_info;
	XExtCodes *ext_codes;

	pex_ext_info = XextCreateExtension();

	ext_dpy_info = XextAddDisplay(pex_ext_info, dpy, PEX_NAME_STRING,
	&pex_extension_hooks, (int) 0, (caddr_t) NULL);

	if (!ext_dpy_info) {
		return (XExtCodes *) NULL;
	}

	ext_codes = (XExtCodes *) Xmalloc(sizeof(XExtCodes));

	if (!ext_codes) {
		return (XExtCodes *) NULL;
	}

	ext_codes->extension	= ext_dpy_info->codes->extension;
	ext_codes->major_opcode	= ext_dpy_info->codes->major_opcode;
	ext_codes->first_error	= ext_dpy_info->codes->first_error;
	ext_codes->first_event	= ext_dpy_info->codes->first_event;

	if (!ext_dpy_info->data) {
		ext_dpy_info->data = (caddr_t) Xmalloc(sizeof(XpexDisplayInfo));
	}
	return ext_codes;
}

XExtDisplayInfo *
XpexFindDisplay(dpy)
	Display *dpy;
{
	return XextFindDisplay(pex_ext_info, dpy);
}
