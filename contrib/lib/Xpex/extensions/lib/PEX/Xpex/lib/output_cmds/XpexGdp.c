/* $Header: XpexGdp.c,v 2.3 91/09/11 16:06:59 sinyaw Exp $ */
/* 
 *	XpexGdp.c
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
XpexGdp(dpy, gdp_id, points, numPoints, data)
	Display *dpy;
	int gdp_id;
	XpexCoord3D *points;
	int numPoints;
	XpexData *data;
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);
	XpexCheckLastReq(dpy,i);

	LockDisplay(dpy);
	{
		char *pStream;
		pexGdp *oc;
		int extra = numPoints * sizeof(pexCoord3D) + data->length;
		extra += PADDING(extra); /* make sure it's padded */

		XpexGetOutputCmdExtra(Gdp, dpy, i,extra, oc);
		oc->gdpId = (INT32) gdp_id;
		oc->numPoints = (CARD32) numPoints;
		oc->numBytes = (CARD32) data->length;

		bcopy((char *)points,(char *)(oc + 1), 
		numPoints * sizeof(pexCoord3D));

		pStream += numPoints * sizeof(pexCoord3D);

		bcopy(data->data, pStream, data->length);
	}
	UnlockDisplay(dpy);
}

void
XpexGdp2D(dpy, gdp_id, points, numPoints, data)
	Display *dpy;
	int gdp_id;
	XpexCoord2D *points;
	int numPoints;
	XpexData *data;
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);
	XpexCheckLastReq(dpy,i);
	
	LockDisplay(dpy);
	{
		char *pStream;
		pexGdp2D *oc;
		int extra = numPoints * sizeof(pexCoord2D) + data->length;
		extra += PADDING(extra); /* make sure it's padded */
		XpexGetOutputCmdExtra(Gdp2D, dpy, i,extra, oc);
		oc->gdpId = (INT32) gdp_id;
		oc->numPoints = (CARD32) numPoints;
		oc->numBytes = (CARD32) data->length;

		bcopy((char *)points,(char *)(oc + 1),
			numPoints * sizeof(pexCoord2D));

		pStream += numPoints * sizeof(pexCoord2D);

		bcopy(data->data, pStream, data->length);
	}
	UnlockDisplay(dpy);
}
