/* $Header: XpexStrCon.c,v 2.3 91/09/11 16:06:54 sinyaw Exp $ */
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
XpexApplicationData(dpy, data)
	Display  *dpy;
	XpexData *data;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		int extra = data->length + PADDING(data->length);
		pexApplicationData *oc;

		XpexGetOutputCmdExtra(ApplicationData,dpy,info,extra,oc);
		oc->numElements = (CARD16)(data->length);
		bcopy((char *)data->data, (char *)(oc + 1), data->length);
	}
	UnlockDisplay(dpy);
}

void
XpexExecuteStructure(dpy, id)
	Display *dpy;
	XpexStructure id;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexExecuteStructure *oc;
		XpexGetOutputCmd(ExecuteStructure,dpy,info,oc);
		oc->id = (pexStructure) id;
	}
	UnlockDisplay(dpy);
}

void
XpexGse(dpy, gse_id, gse_data)
	Display *dpy;
	int gse_id;
	XpexData *gse_data;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexGse *oc;
		int extra = gse_data->length + PADDING(gse_data->length);

		XpexGetOutputCmdExtra(Gse,dpy,info,extra,oc);
		oc->id = (CARD32) gse_id;
		oc->numElements = (CARD16)(gse_data->length);
		bcopy((char *)gse_data->data, (char *)(oc + 1), gse_data->length);
	}
	UnlockDisplay(dpy);
}

void
XpexLabel(dpy, label)
	Display *dpy;
	long label;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexLabel *oc;
		XpexGetOutputCmd(Label,dpy,info,oc);
		oc->label = (INT32) label;
	}
	UnlockDisplay(dpy);
}
