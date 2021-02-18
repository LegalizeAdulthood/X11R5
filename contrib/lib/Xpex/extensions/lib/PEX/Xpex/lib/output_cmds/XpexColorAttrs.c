/* $Header: XpexColorAttrs.c,v 2.5 91/09/11 16:07:01 sinyaw Exp $ */
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
XpexSetLightState(dpy, enable, numEnable, disable, numDisable)
	Display *dpy;
	XpexTableIndex *enable;
	int numEnable;
	XpexTableIndex *disable;
	int numDisable;
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);
	XpexCheckLastReq(dpy,i);

	LockDisplay(dpy);	
	{
		char *pStream;
		pexLightState *oc;
		pexTableIndex *e, *d;
		int a = numEnable * sizeof(pexTableIndex);
		int b = numDisable * sizeof(pexTableIndex);
		int extra = a + PADDING(a) + b + PADDING(b);
		XpexGetOutputCmdExtra(LightState, dpy, i,extra, oc);
		oc->numEnable = (CARD16) numEnable;
		oc->numDisable = (CARD16) numDisable;

		pStream = (char *)(oc + 1);
		bcopy((char *)enable,pStream,a + PADDING(a));
		pStream += a + PADDING(a);
		bcopy((char *)disable,pStream,a + PADDING(a));
	}
	UnlockDisplay(dpy);	
}

void
XpexSetDepthCueIndex(dpy, index)
	Display *dpy;
	XpexTableIndex index;
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);
	XpexCheckLastReq(dpy,i);

	LockDisplay(dpy);
	{
		pexDepthCueIndex *oc;
		XpexGetOutputCmd(DepthCueIndex, dpy, i,oc);
		oc->index = (pexTableIndex) index;
	}
	UnlockDisplay(dpy);
}

void
XpexSetColorApproxIndex(dpy, index)
	Display *dpy;
	XpexTableIndex index;
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);
	XpexCheckLastReq(dpy,i);

	LockDisplay(dpy);
	{
		pexColourApproxIndex *oc;
		XpexGetOutputCmd(ColourApproxIndex, dpy, i,oc);
		oc->index = (pexTableIndex) index;
	}
	UnlockDisplay(dpy);
}

void
XpexSetRenderingColorModel(dpy, model)
	Display *dpy;
	XpexEnumTypeIndex model;
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);
	XpexCheckLastReq(dpy,i);

	LockDisplay(dpy);	
	{
		pexRenderingColourModel *oc;
		XpexGetOutputCmd(RenderingColourModel, dpy, i,oc);
		oc->model = (pexEnumTypeIndex) model;
	}
	UnlockDisplay(dpy);	
}

void
XpexSetParaSurfChar(dpy, psc)
	Display *dpy;
	XpexParaSurfChar *psc;
{
	extern int size_of_psc_data();
	void pack_psc_data();

	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);
	XpexCheckLastReq(dpy,i);

	LockDisplay(dpy);	
	{

		pexParaSurfCharacteristics *oc;
		int bytesWritten;
		int extra = size_of_psc_data(psc);
		XpexGetOutputCmdExtra(ParaSurfCharacteristics, dpy, i,extra, oc);
		oc->characteristics = (pexEnumTypeIndex) psc->characteristics;
		oc->length = (CARD16) extra;
		pack_psc_data((CARD8 *)(oc +1), psc, &bytesWritten);
	}
	UnlockDisplay(dpy);	
}

void
pack_psc_data(dst, src, length)
	CARD8 *dst; 
	XpexParaSurfChar *src;
	int *length;  /* RETURN */
{
	*length = 0;

	switch (src->characteristics) {
	case PEXPSCIsoCurves:
	{
		pexPSC_IsoparametricCurves *psc =
		(pexPSC_IsoparametricCurves *) dst;
		psc->placementType = src->record.iso_curves.placementType;
		psc->numUcurves = src->record.iso_curves.numUcurves;
		psc->numVcurves = src->record.iso_curves.numVcurves;
		*length = sizeof(pexPSC_IsoparametricCurves);
		break;
	}
	case PEXPSCMcLevelCurves:
	{
		register int count;
		XpexCoord3D *s;
		pexPSC_LevelCurves *psc = (pexPSC_LevelCurves *) dst;
		psc->origin.x = src->record.mc_curves.origin.x;
		psc->origin.y = src->record.mc_curves.origin.y;
		psc->origin.z = src->record.mc_curves.origin.z;
		psc->direction.x = src->record.mc_curves.direction.x;
		psc->direction.y = src->record.mc_curves.direction.y;
		psc->direction.z = src->record.mc_curves.direction.z;
		count = src->record.mc_curves.numberIntersections;
		psc->numberIntersections = (CARD16) count;
		*length = sizeof(pexPSC_LevelCurves) +
		(count * sizeof(pexCoord3D));
		dst += sizeof(pexPSC_LevelCurves);
		s = src->record.mc_curves.intersections;
		bcopy((char *)s,(char *)dst,count * sizeof(pexCoord3D));
		break;
	}
	case PEXPSCWcLevelCurves:
	{
		register int count;
		XpexCoord3D *s;
		pexPSC_LevelCurves *psc = (pexPSC_LevelCurves *) dst;
		psc->origin.x = src->record.wc_curves.origin.x;
		psc->origin.y = src->record.wc_curves.origin.y;
		psc->origin.z = src->record.wc_curves.origin.z;
		psc->direction.x = src->record.wc_curves.direction.x;
		psc->direction.y = src->record.wc_curves.direction.y;
		psc->direction.z = src->record.wc_curves.direction.z;
		count = src->record.wc_curves.numberIntersections;
		psc->numberIntersections = (CARD16) count;
		*length = sizeof(pexPSC_LevelCurves) +
		(count * sizeof(pexCoord3D));
		dst += sizeof(pexPSC_LevelCurves);
		s = src->record.wc_curves.intersections;
		bcopy((char *)s,(char *)dst,count * sizeof(pexCoord3D));
		break;
	}
	case PEXPSCNone :
	case PEXPSCImpDep:
	default:
	 	*length = 0;
		break;
	} /* end-switch */
}

void
unpack_psc_data(dst, src, length)
	XpexParaSurfChar *dst;
	CARD8 *src; 
	int *length; /* RETURN */
{
	INT16 *characteristics = (INT16 *) src;
	src += sizeof(CARD32);

	*length = 0;

	switch (*characteristics) {
	case PEXPSCIsoCurves:
	{
		pexPSC_IsoparametricCurves *psc =
		(pexPSC_IsoparametricCurves *) src;
		dst->record.iso_curves.placementType = psc->placementType;
		dst->record.iso_curves.numUcurves = psc->numUcurves;
		dst->record.iso_curves.numVcurves = psc->numVcurves;
		*length = sizeof(pexPSC_IsoparametricCurves);
		break;
	}
	case PEXPSCMcLevelCurves:
	{
		register int count;
		int size;
		XpexCoord3D *s;
		pexPSC_LevelCurves *psc = (pexPSC_LevelCurves *) src;

		dst->record.mc_curves.origin.x = psc->origin.x;
		dst->record.mc_curves.origin.y = psc->origin.y;
		dst->record.mc_curves.origin.z = psc->origin.z;
		dst->record.mc_curves.direction.x = psc->direction.x;
		dst->record.mc_curves.direction.y = psc->direction.y;
		dst->record.mc_curves.direction.z = psc->direction.z;
		count = dst->record.mc_curves.numberIntersections = 
		psc->numberIntersections;

		src += sizeof(pexPSC_LevelCurves);

		if (count) {
			size = count * sizeof(pexCoord3D);

			if(dst->record.mc_curves.intersections = 
			(pexCoord3D *) Xmalloc(size)) {
				bcopy((char *)src,
				(char *)(dst->record.mc_curves.intersections),
				size);
			}
			*length = 
			sizeof(pexPSC_LevelCurves) + (count * sizeof(pexCoord3D));
		}
		break;
	}
	case PEXPSCWcLevelCurves:
	{
		register int count;
		int size;
		XpexCoord3D *s;
		pexPSC_LevelCurves *psc = (pexPSC_LevelCurves *) src;

		dst->record.wc_curves.origin.x = psc->origin.x;
		dst->record.wc_curves.origin.y = psc->origin.y;
		dst->record.wc_curves.origin.z = psc->origin.z;
		dst->record.wc_curves.direction.x = psc->direction.x;
		dst->record.wc_curves.direction.y = psc->direction.y;
		dst->record.wc_curves.direction.z = psc->direction.z;
		count = dst->record.wc_curves.numberIntersections = 
		psc->numberIntersections;

		src += sizeof(pexPSC_LevelCurves);

		if (count) {
			size = count * sizeof(pexCoord3D);

			if(dst->record.wc_curves.intersections = 
			(pexCoord3D *) Xmalloc(size)) {
				bcopy((char *)src,
				(char *)(dst->record.wc_curves.intersections),
				size);
			}
			*length = 
			sizeof(pexPSC_LevelCurves) + (count * sizeof(pexCoord3D));
		}
		break;
	}
	case PEXPSCNone :
	case PEXPSCImpDep:
	default:
	 	*length = 0;
		break;
	} /* end-switch */
}

