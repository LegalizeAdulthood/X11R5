/* $Header: XpexState.c,v 2.3 91/09/11 16:06:11 sinyaw Exp $ */
/* 
 *	XpexState.c 
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
#include "Xpexlib.h"
#include <X11/extensions/Xext.h>
#include "extutil.h"
#include "Xpextutil.h"

void
XpexSetColorType(dpy, colorType)
	Display *dpy;
	XpexColorType colorType;
{
	XpexDisplayInfo *pexdpy;
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	
	pexdpy = (XpexDisplayInfo *)(info->data);
	pexdpy->color_type = colorType;
}

XpexColorType
XpexGetColorType(dpy)
	Display *dpy;
{
	XpexDisplayInfo *pexdpy;
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexCheckExtension(dpy,info,0);
	
	pexdpy = (XpexDisplayInfo *)(info->data);

	return (pexdpy->color_type);
}

void
XpexSetFloatingPointFormat(dpy, fpFormat)
	Display *dpy;
	XpexEnumTypeIndex fpFormat;
{
	XpexDisplayInfo *pexdpy;
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);

	pexdpy = (XpexDisplayInfo *)(info->data);
	pexdpy->fp_format = fpFormat;
}

XpexEnumTypeIndex
XpexGetFloatingPointFormat(dpy)
	Display *dpy;
{
	XpexDisplayInfo *pexdpy;
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexCheckExtension(dpy,info,0);

	pexdpy = (XpexDisplayInfo *)(info->data);
	return (pexdpy->fp_format);
}

/*
 * Set/Update the display's OCT (Output Command Target)
 */
void
XpexRenderOutputCmds(dpy, rdr_id)
	register Display *dpy;
	XpexRenderer rdr_id;
{
	XpexDisplayInfo *pex;
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);

	pex = (XpexDisplayInfo *)(info->data);
	pex->oc_target_type = PEX_RenderOutputCommands;
	pex->oc_target_res_id = rdr_id;
}

void
XpexStoreOutputCmds(dpy, s_id)
	register Display *dpy;
	XpexStructure s_id;
{
	XpexDisplayInfo *pex;
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);

	pex = (XpexDisplayInfo *)(info->data);
	pex->oc_target_type = PEX_StoreElements;
	pex->oc_target_res_id = s_id;
}

void
XpexCheckLastReq(d, i)
	register Display *d;
	register XExtDisplayInfo *i;
{
	XpexDisplayInfo *pex = (XpexDisplayInfo *)(i->data);
	pexOutputCommandsReq *req = (pexOutputCommandsReq *)(d->last_req);
/*  
 * if last req is not a 
 * PEXRenderOutputCommands or PEXStoreElements Request,
 * then start a new one by getting the target info from 
 * the display.
 */

	if (((req->reqType == i->codes->major_opcode) &&
	(req->opcode == pex->oc_target_type) &&
	(req->id == pex->oc_target_res_id))) {
		return;
	}

	LockDisplay(d);
	XFlush(d);
	XpexGetOutputCmdsReq(d,i,req);
	UnlockDisplay(d);
}
