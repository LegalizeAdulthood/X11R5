/* $Header: XpexGeoAttrs.c,v 2.3 91/09/11 16:06:52 sinyaw Exp $ */
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
XpexSetLocalTransform(dpy, compType, matrix)
	Display *dpy;
	XpexComposition compType;
	XpexMatrix matrix;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		register int i;
		pexLocalTransform *oc;

		XpexGetOutputCmd(LocalTransform,dpy,info,oc);
		oc->compType = (pexComposition) compType;
		for (i = 0; i <= 3; i++) {
			oc->matrix[i][0] = matrix[i][0];
			oc->matrix[i][1] = matrix[i][1];
			oc->matrix[i][2] = matrix[i][2];
			oc->matrix[i][3] = matrix[i][3];
		}
	}
	UnlockDisplay(dpy);
}

void
XpexSetLocalTransform2D(dpy, compType, matrix3X3)
	Display *dpy;
	XpexComposition compType;
	XpexMatrix3X3 matrix3X3;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		register int i;
		pexLocalTransform2D *oc;

		XpexGetOutputCmd(LocalTransform2D,dpy,info,oc);

		oc->compType = (pexComposition) compType;

		for (i = 0; i <= 2; i++) {
			oc->matrix3X3[i][0] = matrix3X3[i][0];
			oc->matrix3X3[i][1] = matrix3X3[i][1];
			oc->matrix3X3[i][2] = matrix3X3[i][2];
		}
	}
	UnlockDisplay(dpy);
}

void
XpexSetGlobalTransform(dpy, matrix)
	Display *dpy;
	XpexMatrix matrix;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		register int i;
		pexGlobalTransform *oc;

		XpexGetOutputCmd(GlobalTransform,dpy,info,oc);

		for (i = 0; i <= 3; i++) {
			oc->matrix[i][0] = matrix[i][0];
			oc->matrix[i][1] = matrix[i][1];
			oc->matrix[i][2] = matrix[i][2];
			oc->matrix[i][3] = matrix[i][3];
		}
	}
	UnlockDisplay(dpy);
}

void
XpexSetGlobalTransform2D(dpy, matrix3X3)
	Display *dpy;
	XpexMatrix3X3 matrix3X3;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);	

	LockDisplay(dpy);
	{
		register int i;
		pexGlobalTransform2D *oc;

		XpexGetOutputCmd(GlobalTransform2D,dpy,info,oc);

		for (i = 0; i <= 2; i++) {
			oc->matrix3X3[i][0] = matrix3X3[i][0];
			oc->matrix3X3[i][1] = matrix3X3[i][1];
			oc->matrix3X3[i][2] = matrix3X3[i][2];
		}
	}
	UnlockDisplay(dpy);
}

void
XpexSetModelClip(dpy, on_off)
	Display *dpy;
	XpexSwitch on_off;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexModelClip *oc;

		XpexGetOutputCmd(ModelClip, dpy, info,oc);

		oc->onoff = on_off;
	}
	UnlockDisplay(dpy);
}

void
XpexSetModelClipVolume(dpy, modelClipOperator, 
	halfSpaces, numHalfSpaces)
	Display *dpy;
	XpexEnumTypeIndex modelClipOperator;
	XpexHalfSpace *halfSpaces;
	int numHalfSpaces;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexHalfSpace *dest;
		pexModelClipVolume *oc;
		int extra = numHalfSpaces * sizeof(pexHalfSpace);

		XpexGetOutputCmdExtra(ModelClipVolume, dpy, info,extra, oc);

		oc->modelClipOperator = (pexEnumTypeIndex) modelClipOperator;
		oc->numHalfSpaces = (CARD16) numHalfSpaces;

		dest = (pexHalfSpace *) (oc + 1);
		bcopy((char *)halfSpaces, (char *)(oc + 1),
		numHalfSpaces * sizeof(pexHalfSpace));
	}
	UnlockDisplay(dpy);
}

void
XpexSetModelClipVolume2D(dpy, modelClipOperator, 
	halfSpaces, numHalfSpaces)
	Display *dpy;
	XpexEnumTypeIndex modelClipOperator;
	XpexHalfSpace2D *halfSpaces;
	int numHalfSpaces;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexHalfSpace2D *dest;
		pexModelClipVolume2D *oc;
		int extra = numHalfSpaces * sizeof(pexHalfSpace2D);

		XpexGetOutputCmdExtra(ModelClipVolume2D, dpy, info,extra, oc);

		oc->modelClipOperator = (pexEnumTypeIndex) modelClipOperator;
		oc->numHalfSpaces = (CARD16) numHalfSpaces;

		dest = (pexHalfSpace2D *) (oc + 1);
		while (numHalfSpaces--) {
			dest->point.x = halfSpaces->point.x;
			dest->point.y = halfSpaces->point.y;
			dest->vector.x = halfSpaces->vector.x;
			dest->vector.y = halfSpaces->vector.y;
			dest++; 
			halfSpaces++;
		}
	}
	UnlockDisplay(dpy);
}

void
XpexRestoreModelClip(dpy)
	Display *dpy;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexRestoreModelClip *oc;

		XpexGetOutputCmd(RestoreModelClip, dpy, info,oc);
	}
	UnlockDisplay(dpy);
}

void
XpexSetViewIndex(dpy, viewIndex)
	Display *dpy;
	XpexTableIndex viewIndex;
{
	XExtDisplayInfo *info = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,info);
	XpexCheckLastReq(dpy,info);

	LockDisplay(dpy);
	{
		pexViewIndex *oc;
	
		XpexGetOutputCmd(ViewIndex,dpy,info,oc);
		oc->index = (pexTableIndex) viewIndex;
	}
	UnlockDisplay(dpy);
}
