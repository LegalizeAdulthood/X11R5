/* $Header: XpexInfo.c,v 2.2 91/09/11 16:07:34 sinyaw Exp $ */
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

Status
XpexGetExtensionInfo(dpy, proto_major, proto_minor, info)
    Display *dpy;
	int proto_major;
	int proto_minor;
	XpexExtensionInfo *info;  /* RETURN */
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	pexGetExtensionInfoReq *req;
	pexGetExtensionInfoReply reply;
	unsigned size;

	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	XpexGetReq(GetExtensionInfo, dpy, i, req);
	req->clientProtocolMajor = (CARD16) proto_major;
	req->clientProtocolMinor = (CARD16) proto_minor;
	if (!_XReply(dpy, (xReply *) &reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	info->proto_major = (int) reply.majorVersion;
	info->proto_minor = (int) reply.minorVersion;
	info->release_number = (int) reply.release;
	info->subset_info = (int) reply.subsetInfo;
	size = MAX(reply.lengthName+1, reply.length*sizeof(CARD32));
	if (info->vendor = Xmalloc(size)) {
		_XRead(dpy, info->vendor, 
		(long)(reply.length * sizeof(CARD32)));
		(info->vendor)[reply.lengthName] = '\0';
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return 1;
}

Status
XpexGetEnumTypeInfo(dpy, drawable, 
	enum_types, count, returnFormat, info_list, actual_count)
	Display *dpy;
	Drawable drawable;
	short *enum_types;
	int count;
	XpexBitmask returnFormat;
	XpexListOfEnumTypeDesc *info_list[]; /* RETURN */
	int *actual_count; /* RETURN */
{
	static Status _XpexGetEnumTypeInfo();

	Status status;
	register long i, j;

	XpexListOfEnumTypeDesc *list;

	CARD32 *count_of_each_et;
	CARD32 numTypesReturn;
	char *typesReturn;

	/* get counts of the specified enum types first */
	status = _XpexGetEnumTypeInfo(dpy, drawable, enum_types, count,
	returnFormat, &typesReturn, &numTypesReturn);

	*actual_count = (int) numTypesReturn;

	if (*actual_count) {
		*info_list = list = 
		(XpexListOfEnumTypeDesc *) malloc((unsigned) 
		(*actual_count * sizeof(XpexListOfEnumTypeDesc)));

		switch ((int) returnFormat) {
		case 0x0:
			count_of_each_et = (CARD32 *) typesReturn;	
			for (i = 0; i < *actual_count; i++) {
				list[i].count = (int) *count_of_each_et++;
			}
			break;
		case PEXETIndex:
		{
			int sizeofindices;

			for (i = 0; i < numTypesReturn; i++) {
				list[i].count = (int) *((CARD32 *) typesReturn);
				typesReturn += sizeof(CARD32);
				if (list[i].desc = (XpexEnumTypeDesc *) 
				Xmalloc(list[i].count * sizeof(XpexEnumTypeDesc))) {
					pexEnumTypeIndex *index = 
					(pexEnumTypeIndex *) typesReturn;
					for (j = 0; j < list[i].count; j++) {
						list[i].desc[j].index = *index++;
					}
					sizeofindices = list[i].count * sizeof(pexEnumTypeIndex);
					typesReturn += (sizeofindices + PADDING(sizeofindices));
				}
			}
			break;
		}
		case PEXETMnemonic:
		{
			int temp;
			CARD16 *c16; /* string length */

			for (i = 0; i < numTypesReturn; i++) {
				list[i].count = (int) *((CARD32 *) typesReturn);
				typesReturn += sizeof(CARD32);
				if (list[i].desc = (XpexEnumTypeDesc *)
				Xmalloc(list[i].count * sizeof(XpexEnumTypeDesc))) {
				for (j = 0; j < list[i].count; j++) {
					c16 = (CARD16 *) typesReturn;
					list[i].desc[j].name = (char *) malloc(*c16+1);
					bcopy(typesReturn+2,list[i].desc[j].name,*c16);
					list[i].desc[j].name[*c16] = NULL;
					temp = sizeof(CARD16) + *c16;
					typesReturn += temp + PADDING(temp);
				}
				} /* end-if */
			}
			break;
		}
		case PEXETBoth:
		{
			int temp;
			CARD16 *c16; /* string length */

			for (i = 0; i < numTypesReturn; i++) {
				list[i].count = (int) *((CARD32 *) typesReturn);
				typesReturn += sizeof(CARD32);
				if (list[i].desc = (XpexEnumTypeDesc *)
				Xmalloc(list[i].count * sizeof(XpexEnumTypeDesc))) {
					for (j = 0; j < list[i].count; j++) {
						/* get enum type index */
						list[i].desc[j].index = 
						*((pexEnumTypeIndex *)typesReturn);
						/* move pointer */
						typesReturn += sizeof(pexEnumTypeIndex);
						/* get string length */
						c16 = (CARD16 *) typesReturn;
						/* move pointer */
						typesReturn += sizeof(CARD16);
						/* alloc string memory. don't forget NULL */
						list[i].desc[j].name = (char *)Xmalloc(*c16+1);
						/* copy string into alloc'd space */
						bcopy(typesReturn, list[i].desc[j].name, *c16);
						/* terminated string with NULL */
						list[i].desc[j].name[*c16] = NULL;
						temp = *c16;
						typesReturn += temp + PADDING(temp);
					} /* end-for */
				} else {
					return 0; /* malloc failed */
				}
			} /* end-for */
			break;
		}
		default:
			break;
		} /* end-switch */
		} /* end-if (returnFormat) */

	return status;
}

Status
XpexGetAllEnumTypeInfo(dpy, drawable, info_list, count)
	register Display  *dpy;
	Drawable  drawable;
	XpexListOfEnumTypeDesc  *info_list[];  /* RETURN */
	int count; /* RETURN */
{
    static CARD16 enum_types[27] = {
        PEXETMarkerType,
        PEXETATextStyle,
        PEXETInteriorStyle,
        PEXETHatchStyle,
        PEXETLineType,
        PEXETSurfaceEdgeType,
        PEXETPickDeviceType,
        PEXETPolylineInterpMethod,
        PEXETCurveApproxMethod,
        PEXETReflectionModel,
        PEXETSurfaceInterpMethod,
        PEXETSurfaceApproxMethod,
        PEXETModelClipOperator,
        PEXETLightType,
        PEXETColourType,
        PEXETFloatFormat,
        PEXETHlhsrMode,
        PEXETPromptEchoType,
        PEXETDisplayUpdateMode,
        PEXETColourApproxType,
        PEXETColourApproxModel,
        PEXETGDP,
        PEXETGDP3,
        PEXETGSE,
        PEXETTrimCurveApproxMethod,
        PEXETRenderingColourModel,
        PEXETParaSurfCharacteristics
    };
	Status status;
	XpexBitmask returnFormat = PEXETBoth;

	status = XpexGetEnumTypeInfo(dpy, drawable, enum_types, 27,
	returnFormat, info_list, count);
}

/* 
 *	Function:	XpexGetImpDepConstants
 *
 *	Purpose:	Return one of more ImpDepConstants
 *
 */

Status
XpexGetImpDepConstants(dpy, drawable, names, numNames, values)
	register Display *dpy;
	Drawable drawable;
	XpexImpDepConstantName *names;
	int numNames;
	unsigned long **values; /* RETURN */
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexGetImpDepConstantsReq *req;
	pexGetImpDepConstantsReply reply;
	Status status = 1;
	unsigned size;
	CARD16 *dest;
	int extra = numNames * sizeof(CARD16);
	extra += PADDING(extra);

	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	XpexGetReqExtra(GetImpDepConstants, dpy, i, extra, req);
	req->fpFormat = (pexEnumTypeIndex) XpexGetFloatingPointFormat(dpy);
	req->drawable = drawable;
	req->numNames = (CARD32) numNames;
	dest = (CARD16 *)(req + 1);
	while (numNames--) {
		*dest++ = (CARD16) *names++;
	}
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*values = (unsigned long *)Xmalloc(size)) {
			_XRead(dpy, (char *)*values, (long)size);
		} else {
			status = 0;
			_XpexClearReply(dpy, reply.length);
		}
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return status;
}

/* 
 *	Function:	XpexGetImpDepConstant
 *
 *	Purpose:	Return specified ImpDepConstant
 *
 */

Status
XpexGetImpDepConstant(dpy, drawable, name, value)
	Display *dpy;
	Drawable drawable;
	XpexImpDepConstantName name;
	unsigned long *value; /* RETURN */
{
	return XpexGetImpDepConstants(dpy, drawable, name, 1, &value);
}

/* 
 *	Function:	XpexGetAllImpDepConstants
 *
 *	Purpose:	Return all core PEX ImpDepConstants
 *
 */

Status
XpexGetAllImpDepConstants(dpy, drawable, idc)
	Display *dpy;
	Drawable drawable;
	XpexAllImpDepConstants *idc; /* RETURN */
{
	Status status = 0;

    static pexImpDepConstantName names[33] = {
        PEXIDNominalLineWidth,
        PEXIDNumSupportedLineWidths,
        PEXIDMinLineWidth,
        PEXIDMaxLineWidth,
        PEXIDNominalEdgeWidth,
        PEXIDNumSupportedEdgeWidths,
        PEXIDMinEdgeWidth,
        PEXIDMaxEdgeWidth,
        PEXIDNominalMarkerSize,
        PEXIDNumSupportedMarkerSizes,
        PEXIDMinMarkerSize,
        PEXIDMaxMarkerSize,
        PEXIDChromaticityRedU,
        PEXIDChromaticityRedV,
        PEXIDLuminanceRed,
        PEXIDChromaticityGreenU,
        PEXIDChromaticityGreenV,
        PEXIDLuminanceGreen,
        PEXIDChromaticityBlueU,
        PEXIDChromaticityBlueV,
        PEXIDLuminanceBlue,
        PEXIDChromaticityWhiteU,
        PEXIDChromaticityWhiteV,
        PEXIDLuminanceWhite,
        PEXIDMaxNameSetNames,
        PEXIDMaxModelClipPlanes,
        PEXIDTransparencySupported,
        PEXIDDitheringSupported,
        PEXIDMaxNonAmbientLights,
        PEXIDMaxNURBOrder,
        PEXIDMaxTrimCurveOrder,
        PEXIDBestColourApproximation,
        PEXIDDoubleBufferingSupported,
    };

    CARD32 numNames = 33;
    unsigned long *idc_data;

	status = XpexGetImpDepConstants(dpy, drawable, names, numNames,
	&idc_data);

	bcopy((char *)idc_data,(char *)idc,sizeof(pexAllImpDepConstants));

	XFree((char *)idc_data);

	return (status);
}

static Status
_XpexGetEnumTypeInfo(dpy, drawable, 
	enumTypes, numEnumTypes, itemMask, typesReturn, numTypesReturn)
	Display *dpy;
	Drawable drawable;
	CARD16 *enumTypes;
	CARD32 numEnumTypes;
	pexBitmask itemMask;
	char **typesReturn;
	CARD32 *numTypesReturn;
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexGetEnumeratedTypeInfoReq *req;
	pexGetEnumeratedTypeInfoReply reply;
	unsigned size;
	Status status = 1;

	int extra = numEnumTypes * sizeof(CARD16);
	extra += PADDING(extra);


	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	XpexGetReqExtra(GetEnumeratedTypeInfo, dpy, i, extra, req);
	req->drawable = drawable;
	req->itemMask = itemMask;
	req->numEnums = numEnumTypes;
	bcopy((char *)enumTypes, (char *)(req + 1), 
	numEnumTypes * sizeof(CARD16));

	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*numTypesReturn = reply.numLists;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*typesReturn = Xmalloc(size)) {
			_XRead(dpy, (char *)*typesReturn, (long)size);
		} else {
			status = 0;
			_XpexClearReply(dpy, reply.length);
		}
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return status;
}
