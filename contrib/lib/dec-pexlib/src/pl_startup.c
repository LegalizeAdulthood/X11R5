/***********************************************************
Copyright 1987,1991 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/


/*
**++
**  File Name:
**
**	pl_startup.c
**
**  Subsystem:
**
**	PEXlib
**
**  Version:
**
**	V1.0
**
**  Abstract:
**
**	This file contains the PEX initialization, termination, and
**	miscellaneous housekeeping routines.
**
**                                  NOTES:
**
** Unless otherwise noted, storage for any data (structures, strings, table
** entries, structure network paths, list of names for name sets, etc.) that is
** returned from the PEX server extension is allocated by PEXlib and must be
** deallocated by the client application when it is no longer needed, by
** calling PEXFree().
**
** The client application passes the library a pointer to a pointer to the
** data to be returned.  The library will overwrite the pointer pointed to
** with the address of the returned data.  Again, these data, and any
** additional storage allocated for them (e.g., strings, lists, lists of
** lists) can be freed by calling PEXFree with a pointer to the structure.
**
** WARNING:  The structures returned from PEXlib CANNOT BE REALLOC()ED.
** Explanation: The library is hiding some data before the location it returns
** to its caller (specifically, the free method and the length of the
** object).
**
**  Keywords:
**
**  Environment:
**
**	VMS and Ultrix
**
**
**  Creation Date:
**
**  Modification History:
**
**	19-Aug-88  
**--
*/

/*
 *   Table of Contents (routine names in order of appearance)
 */

/*
 * E   PEXInitialize
 * E   PEXTerminate
 * E   PEXGetEnumTypeInfo
 * E   PEXGetListEnumTypeInfo
 * E   PEXSetFloatingPointFormat
 * E   PEXGetFloatingPointFormat
 * E   PEXSetColourType
 * E   PEXGetColourType
 * E   PEXGetImpDepConstants
 * I   _PEXLookupErrorString
 */

/*
 *   Include Files
 */

#include "pex_features.h"
#include "pex_macros.h"

#include "PEXlib.h"
#include "PEXlibint.h"

#ifdef MPEX
#include "MPEX.h"
#endif

#include "pl_def_data.h"

extern	char	*_XAllocScratch();

#ifdef MPEX
extern Status	_MPEXConvertMaxHitsEvent();
#endif

char	*_PEXLookupErrorString();

/*+
 * NAME:
 * 	PEXInitialize
 *
 * FORMAT:
 * 	`PEXInitialize`
 *
 * ARGUMENTS:
 *	display     A pointer to a display structure
 *		    returned by a successful `XOpenDisplay" call.
 *
 *	pexinfo     Returns a pointer to a structure
 *		    containing information about the version
 *		    of the PEX server extension.
 *
 * RETURNS:
 *	Zero if the PEX server extension was
 *	initialized successfully, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine initializes PEXlib and the PEX server extension.  The
 *	major version
 *	number, minor version number, release number, vendor name, and subset
 *	information are returned in `pexinfo'.  If the subset field is
 *	zero, the extension is a full PEX implementation.  If the low-order bit
 *	of the subset field is set, the extension is an "immediate rendering
 *	only" subset of PEX.  If the next bit of the subset field is set, the
 *	extension is a "PHIGS workstation only" subset of PEX.
 *
 *	PEXlib allocates memory for the `pexinfo' structure.
 *	`PEXFree"(`*pexinfo') deallocates the memory.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXTerminate"
 *
 */

Status
PEXInitialize (display, info)

INPUT Display	*display;
OUTPUT pxlInfo	**info;

{
/* AUTHOR: */
    register pexGetExtensionInfoReq	*req;
    pexGetExtensionInfoReply		rep;
    pxlInfo				*pinfo;
    extern XExtCodes			*XInitExtension();
    XExtCodes				*pExtCodes;
    char				*string;

    /* lock the display for now */
    LockDisplay (display);

    /* check for the string the protocol specifies */
    if ((pExtCodes = XInitExtension (display, "X3D-PEX")) == NULL)
    {
	return (1);
    }
    pexExtCodes[ConnectionNumber(display)] = pExtCodes;

#ifdef MPEX
    /* tell Xlib how to convert the MaxHitReachedEvent */
    XESetWireToEvent (display, pExtCodes->first_event + MPEXMaxHitsReached,
	_MPEXConvertMaxHitsEvent);
#endif

    /* tell Xlib where to get the text for the PEX errors */
    XESetErrorString (display, pExtCodes->extension, _PEXLookupErrorString);

    /* ask the PEX server extension about itself */
    PEXGetReq (GetExtensionInfo, req);
    req->clientProtocolMajor = PEX_PROTO_MAJOR;
    req->clientProtocolMinor = PEX_PROTO_MINOR;

    /* get the reply */
    if (_XReply (display,  &rep, 0, xFalse) == 0)
    {/* there was an error */
        UnlockDisplay (display);
	PEXSyncHandle ();
	return (1);              /* return an error */
    }

    /* allocate some storage to return to the user and put the string in it */
    string = (char *)PEXAllocBuf ((unsigned)(rep.lengthName+1));

    _XReadPad (display, (char *)string, (long)(rep.lengthName));
    string[rep.lengthName] = 0;	/* null terminate the string */

    /* we can unlock now */
    UnlockDisplay (display);
    PEXSyncHandle ();

    /* put the data into the pxlInfo structure */
    pinfo = (pxlInfo *)PEXAllocBuf
	((unsigned)(sizeof(pxlInfo) + sizeof(PEXHeader)));
    ((PEXHeader *)pinfo)->free = _PEXFreeInfo;  /* routine to call to free it */

    pinfo = (pxlInfo *)&(((PEXHeader *)pinfo)[1]); /* update ptr past header */

    /* copy the data to the user's buffer */
    pinfo->majorVersion = rep.majorVersion;
    pinfo->minorVersion = rep.minorVersion;
    pinfo->release = rep.release;
    pinfo->subsetInfo = rep.subsetInfo;
    pinfo->vendorName = string;

    *info = pinfo;

    return (Success);
}

/*+
 * NAME:
 * 	PEXTerminate
 *
 * FORMAT:
 * 	`PEXTerminate`
 *
 * ARGUMENTS:
 *	display     A pointer to a display structure
 *		    returned by a successful `XOpenDisplay" call.
 *
 *	codes	    A pointer to a structure that defines the PEX
 *		    extension (should be NULL when this routine is
 *		    called by application programs).
 *
 * RETURNS:
 *	Zero if the termination was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *  	This routine gracefully closes down the PEXlib library and the PEXlib
 *	server extension for the connection specified by `display'.
 *	`PEXTerminate" is also called implicitly if the application program
 *	ends without calling it explicitly.  The `codes' argument is provided
 *	for compatibility with the X extension mechanism.  (It is passed when
 *	Xlib calls `PEXTerminate" to implicitly terminate a PEX session.)
 *	The `codes' argument is not used within `PEXTerminate", so application
 *	programs should pass the value `NULL' for the `codes' argument.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXInitialize"
 *
 */

Status
PEXTerminate (display, codes)

INPUT Display	*display;
INPUT XExtCodes	*codes;

{
/* AUTHOR: */
    /* At the very least, we must free the storage allocated for the
     * extension info, and mark extension isn't initialized */
    if (pexExtCodes[ConnectionNumber(display)] == NULL)
    {
#ifdef ULTRIX
	printf("Terminating uninitialized display %d\n",
	    ConnectionNumber(display));
#endif /* ULTRIX */
	return (-1);
    }

    /* free the extension codes */
    Xfree ((char *)(pexExtCodes[ConnectionNumber(display)]));
    pexExtCodes[ConnectionNumber(display)] = NULL;

    /* reset the global data */
#ifdef VAX
    pexFpFormat = PEXDEC_F_Floating;
#else
    pexFpFormat = PEXIeee_754_32;
#endif /* VAX */

    pexColourTypeHidden = PEXRgbFloatColour;
    pexOCError = 0;

    /* shouldn't we do something to finish freeing the extension??? */

    return (Success);
}

/*+
 * NAME:
 * 	PEXGetEnumTypeInfo
 *
 * FORMAT:
 * 	`PEXGetEnumTypeInfo`
 *
 * ARGUMENTS:
 *	display	    A pointer to a display structure
 *		    returned by a successful `XOpenDisplay" call.
 *
 *	drawable    The resource identifier of an X window or pixmap.
 *
 *	enumType    The enumerated type for which information is to
 *		    be returned.
 *
 *	returnMask  A mask indicating the enumerated type information
 *		    to be returned.
 *
 *	enumInfoReturn	Returns a pointer to an array of records
 *			containing the enumeration type information.
 *
 *	countReturn Returns the number of records in the
 *			`enumInfoReturn' array.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	PEX defines enumerated types to represent options such as
 *	line types, interior styles, and curve approximation methods.
 *	PEX defines a number of "registered values" for each enumerated
 *	type.  A PEX implementation need not support all
 *	registered values for each enumerated type and may add values.
 *	`PEXGetEnumTypeInfo" allows an application program to find
 *	the values a PEX implementation supports for each enumerated type.
 * 	`PEXGetEnumTypeInfo" returns an enumerated type descriptor for
 *	each supported value of the specified `enumType' on
 *	drawables having the same root window and depth as `drawable'.
 *
 *	`EnumType' may be one of the following constants.  (Include file
 *	`PEX.h' contains definitions of these constants):
 *
 *	>
 *	`PEXETATextStyle'
 *	`PEXETColourApproxModel'
 *	`PEXETColourApproxType'
 *	`PEXETColourType'
 *	`PEXETCurveApproxMethod'
 *	`PEXETDisplayUpdateMode'
 *	`PEXETFloatFormat'
 *	`PEXETGDP'
 *	`PEXETGDP3'
 *	`PEXETGSE'
 *	`PEXETHatchStyle'
 *	`PEXETHlhsrMode'
 *	`PEXETInteriorStyle'
 *	`PEXETLightType'
 *	`PEXETLineType'
 *	`PEXETMarkerType'
 *	`PEXETModelClipOperator'
 *	`PEXETParaSurfCharacteristics'
 *	`PEXETPickDeviceType'
 *	`PEXETPolylineInterpMethod'
 *	`PEXETPromptEchoType'
 *	`PEXETReflectionModel'
 *	`PEXETRenderingColourModel'
 *	`PEXETSurfaceApproxMethod'
 *	`PEXETSurfaceEdgeType'
 *	`PEXETSurfaceInterpMethod'
 *	`PEXETTrimCurveApproxMethod'
 *	<
 *
 *	In addition, if the PEX server extension supports the Digital
 *	extensions to renderer resources, `EnumType' may be one of the
 *	following constants.  (Include file
 *	`MPEX.h' contains definitions of these constants):
 *
 *	>
 *	`MPEXETHighlightMethod'
 *	`MPEXETEchoMethod'
 *	`MPEXETOverlapMode'
 *	`MPEXETLogicalOp'
 *	<
 *
 *	Enumerated type descriptors are made up of an `index'
 *	field and a `mnemonic' field.  The index field contains the numeric
 *	value of the supported type, and the mnemonic field contains a
 *	string describing the type.
 *
 *	`ReturnMask' specifies which of the fields is returned
 *	in each enumerated type descriptor.  The predefined bitmasks `PEXETIndex'
 *	and `PEXETMnemonic' create the bitmask value passed in `returnMask'.
 *	(Include file `PEX.h' contains definitions for both of
 *	these constants.)  If only the `PEXETIndex' bit is set in `returnMask',
 *	only the index values are returned for the supported values.
 *	If only the `PEXETMnemonic' bit is set, only the mnemonic string
 *	describing each  value is returned.  If both the `PEXETIndex' and
 *	the `PEXETMnemonic' bits are set, both indices and mnemonics are
 *	returned for the defined values.
 *
 *	The following code fragment is an example of calling this routine.
 *
 *	==
 *      pxlEnumTypeDescList *enumInfo;
 *      pxlEnumTypeDescList **enumInfoPtr;
 *      int count;
 *
 *      enumInfoPtr = &enumInfo;
 *      PEXGetEnumTypeInfo (myDisplay, myWindow, PEXETLineType,
 *          PEXETIndex | PEXETMnemonic, enumInfoPtr, &count);
 *
 *      for (i=0; i<count; i++)
 *          printf ("Record #%d: value = %d, mnemonic = \"%s\"\n",
 *              i, enumInfo[i].enumVal, enumInfo[i].string);
 *
 *      PEXFree ((char *)enumInfo);
 *	!=
 *
 *	If the PEX server extension supported all four registered types, it
 *	would return the value four in `count' and would return a pointer
 *	to the following four enumerated type descriptor records in `enumInfo':
 *
 *	>
 *	Record #1:  value = 1,    mnemonic = "Solid"
 *	Record #2:  value = 2,    mnemonic = "Dashed"
 *	Record #3:  value = 3,    mnemonic = "Dotted"
 *	Record #4:  value = 4,    mnemonic = "DashDot"
 *	<
 *
 *	PEXlib allocates storage for the array of returned enumerated type
 *	descriptors.  The number of descriptors in the array is
 *	returned in `countReturn'.  A pointer to the first descriptor in
 *	the array is returned in `enumInfoReturn'.  Call
 *	`PEXFree"(`*enumInfoReturn') to deallocate the memory for the
 *	returned enumerated type descriptor records.
 *
 *	The following registered enumerated type value/mnemonic pairs
 *	are defined in PEX.  (Include file `PEX.h' defines the values
 *	for use in application programs.)
 *
 *	`PEXETATextStyle"
 *		>
 *		`PEXATextNotConnected' / "NotConnected"
 *		`PEXATextConnected' / "Connected"
 *		<
 *	`PEXETCurveApproxMethod"
 *		>
 *		`PEXApproxImpDep' / implementation dependent string
 *		`PEXApproxConstantBetweenKnots' / "Constant"
 *		`PEXCurveApproxWcsChordalDev' / "WCS_ChordalDev"
 *		`PEXCurveApproxNpcChordalDev' / "NPC_ChordalDev"
 *		`PEXCurveApproxDcChordalDev' / "DC_ChordalDev"
 *		<
 *	`PEXETColourType"
 *		>
 *		`PEXIndexedColour' / "Indexed"
 *		`PEXRgbFloatColour' / "RGBFloat"
 *		`PEXCieFloatColour' / "CIEFloat"
 *		`PEXHsvFloatColour' / "HSVFloat"
 *		`PEXHlsFloatColour' / "HLSFloat"
 *		`PEXRgb8Colour' / "RGBInt8"
 *		`PEXRgb16Colour' / "RGBInt16"
 *		<
 *	`PEXETDisplayUpdateMode"
 *		>
 *		`PEXVisualizeEach' / "VisualizeEach"
 *		`PEXVisualizeEasy' / "VisualizeEasy"
 *		`PEXVisualizeNone' / "VisualizeNone"
 *		`PEXSimulateSome' / "SimulateSome"
 *		`PEXVisualizeWhenever' / "VisualizeWhenever"
 *		<
 *	`PEXETFloatFormat"
 *		>
 *		`PEXIeee_754_32' / "IEEE_754_32"
 *		`PEXDEC_F_Floating' / "DEC_F_Floating"
 *		<
 *	`PEXETHatchStyle"
 *		>
 *		Not supported in Digital implemenations of PEX.
 *		<
 *	`PEXETHlhsrMode"
 *		>
 *		`PEXHlhsrOff' / "Off"
 *		`PEXHlhsrZBuffer' / "ZBuffer"
 *		`PEXHlhsrPainters' / "Painters"
 *		`PEXHlhsrScanline' / "Scanline"
 *		`PEXHlhsrHiddenLineOnly' / "HiddenLineOnly"
 *		<
 *	`PEXETInteriorStyle"
 *		>
 *		`PEXInteriorStyleHollow' / "Hollow"
 *		`PEXInteriorStyleSolid' / "Solid"
 *		`PEXInteriorStylePattern' / "PatternLUT"
 *		`PEXInteriorStyleHatch' / "Hatch"
 *		`PEXInteriorStyleEmpty' / "Empty"
 *		<
 *	`PEXETLightType"
 *		>
 *		`PEXLightAmbient' / "Ambient"
 *		`PEXLightWcsVector' / "WCS_Vector"
 *		`PEXLightWcsPoint' / "WCS_Point"
 *		`PEXLightWcsSpot' / "WCS_Spot"
 *		<
 *	`PEXETLineType"
 *		>
 *		`PEXLineTypeSolid' / "Solid"
 *		`PEXLineTypeDashed' / "Dashed"
 *		`PEXLineTypeDotted' / "Dotted"
 *		`PEXLineTypeDashDot' / "DashDot"
 *		<
 *	`PEXETMarkerType"
 *		>
 *		`PEXMarkerDot' / "Dot"
 *		`PEXMarkerCross' / "Cross"
 *		`PEXMarkerAsterisk' / "Asterisk"
 *		`PEXMarkerCircle' / "Circle"
 *		`PEXMarkerX' / "X"
 *		<
 *	`PEXETModelClipOperator"
 *		>
 *		`PEXModelClipReplace' / "Replace"
 *		`PEXModelClipIntersection' / "Intersection"
 *		<
 *	`PEXETPickDeviceType"
 *		>
 *		No pick device types have been registered.
 *		<
 *	`PEXETPolylineInterpMethod"
 *		>
 *		`PEXPolylineInterpNone' / "None"
 *		`PEXPolylineInterpColour' / "Colour"
 *		<
 *	`PEXETPromptEchoType"
 *		>
 *		`PEXEchoPrimitive' / "EchoPrimitive"
 *		`PEXEchoStructure' / "EchoStructure"
 *		`PEXEchoNetwork' / "EchoNetwork"
 *		<
 *	`PEXETReflectionModel"
 *		>
 *		`PEXReflectionNoShading' / "NoShading"
 *		`PEXReflectionAmbient' / "Ambient"
 *		`PEXReflectionDiffuse' / "Diffuse"
 *		`PEXReflectionSpecular' / "Specular"
 *		<
 *	`ETSurfaceApproxMethod"
 *		>
 *		`PEXApproxImpDep' / implementation dependent string
 *		`PEXApproxConstantBetweenKnots' / "ConstantBetweenKnots"
 *		`PEXSurfaceApproxWcsPlanarDev' / "WCS_PlanarDev"
 *		`PEXSurfaceApproxNpcPlanarDev' / "NPC_PlanarDev"
 *		`PEXSurfaceApproxDcPlanarDev' / "DC_PlanarDev"
 *		<
 *	`PEXETSurfaceEdgeType"
 *		>
 *		`PEXSurfaceEdgeSolid' / "Solid"
 *		`PEXSurfaceEdgeDashed' / "Dashed"
 *		`PEXSurfaceEdgeDotted' / "Dotted"
 *		`PEXSurfaceEdgeDashDot' / "DashDot"
 *		<
 *	`PEXETSurfaceInterpMethod"
 *		>
 *		`PEXSurfaceInterpNone' / "None"
 *		`PEXSurfaceInterpColour' / "Colour"
 *		`PEXSurfaceInterpDotProduct' / "DotProduct"
 *		`PEXSurfaceInterpNormal' / "Normal"
 *		<
 *	`PEXETTrimCurveApproxMethod"
 *	>
 *		`PEXApproxImpDep' / implementation dependent string
 *		`PEXApproxConstantBetweenKnots' / "ConstantBetweenKnots"
 *		`PEXCurveApproxWcsChordalDev' / "WCS_ChordalDev"
 *	<
 *
 *	In addition, if the PEX server extension supports the Digital extensions
 *	to renderer resources,
 *	the following registered enumerated type value/mnemonic pairs
 *	are defined.  (Include file `MPEX.h' defines the values
 *	for use in application programs.)
 *
 *	`MPEXETHighlightMethod"
 *		>
 *		`MPEXChangeAllColours' / "ChangeAllColours"
 *		`MPEXChangeEdgeColour' / "ChangeEdgeColour"
 *		`MPEXChangeInteriorColour' / "ChangeInteriorColour"
 *		<
 *	`MPEXETEchoMethod"
 *		>
 *		`MPEXChangeAllColours' / "ChangeAllColours"
 *		`MPEXChangeEdgeColour' / "ChangeEdgeColour"
 *		`MPEXChangeInteriorColour' / "ChangeInteriorColour"
 *		<
 *	`MPEXETZOverlapMode"
 *		>
 *		`MPEXAnyPrim' / "AnyPrim"
 *		`MPEXFirstPrim' / "FirstPrim"
 *		`MPEXLastPrim' / "LastPrim"
 *		<
 *	`MPEXETLogicalOp'
 *		>
 *		`MPEXOpClear' / "Clear"
 *		`MPEXOpAnd' / "And"
 *		`MPEXOpAndReverse' / "AndReverse"
 *		`MPEXOpCopy' / " Copy"
 *		`MPEXOpAndInverted' / "AndInverted"
 *		`MPEXOpNoop' / "Noop"
 *		`MPEXOpXor' / "Xor"
 * 		`MPEXOpOr' / "Or"
 *		`MPEXOpNor' / "Nor"
 *		`MPEXOpEquiv' / "Equiv"
 *		`MPEXOpInvert' / "Invert"
 *		`MPEXOpOrReverse' / "OrReverse"
 *		`MPEXOpCopyInverted' / "CopyInverted"
 *		`MPEXOpOrInverted' / "OrInverted"
 *		`MPEXOpNand' / "Nand"
 *		`MPEXOpSet' / "Set"
 *		<
 *
 *	Also, if the PEX server extension supports the Digital extensions
 *	to renderer resources, some additional value/mnemonic pairs may be
 *	returned for the standard PEX enumerated types.
 *
 *	`PEXETLineType"
 *		>
 *		`MPEXDashPattern1' / "DashPattern1"
 *		`MPEXDashPattern2' / "DashPattern2"
 *		`MPEXDashPattern3' / "DashPattern3"
 *		`MPEXDashPattern4' / "DashPattern4"
 *		`MPEXDashPattern5' / "DashPattern5"
 *		`MPEXDashPattern6' / "DashPattern6"
 *		`MPEXDashPattern7' / "DashPattern7"
 *		`MPEXDashPattern8' / "DashPattern8"
 *		`MPEXDashPattern9' / "DashPattern9"
 *		`MPEXDashPattern10' / "DashPattern10"
 *		`MPEXDashPattern11' / "DashPattern11"
 *		`MPEXDashPattern12' / "DashPattern12"
 *		`MPEXDashPattern13' / "DashPattern13"
 *		`MPEXDashPattern14' / "DashPattern14"
 *		`MPEXDashPattern15' / "DashPattern15"
 *		`MPEXDashPattern16' / "DashPattern16"
 *		<
 *	`PEXETSurfaceEdgeType"
 *		>
 *		`MPEXDashPattern1' / "DashPattern1"
 *		`MPEXDashPattern2' / "DashPattern2"
 *		`MPEXDashPattern3' / "DashPattern3"
 *		`MPEXDashPattern4' / "DashPattern4"
 *		`MPEXDashPattern5' / "DashPattern5"
 *		`MPEXDashPattern6' / "DashPattern6"
 *		`MPEXDashPattern7' / "DashPattern7"
 *		`MPEXDashPattern8' / "DashPattern8"
 *		`MPEXDashPattern9' / "DashPattern9"
 *		`MPEXDashPattern10' / "DashPattern10"
 *		`MPEXDashPattern11' / "DashPattern11"
 *		`MPEXDashPattern12' / "DashPattern12"
 *		`MPEXDashPattern13' / "DashPattern13"
 *		`MPEXDashPattern14' / "DashPattern14"
 *		`MPEXDashPattern15' / "DashPattern15"
 *		`MPEXDashPattern16' / "DashPattern16"
 *		<
 *	`PEXETMarkerType"
 *		>
 *		`MPEXMarkerGlyph1' / "MarkerGlyph1"
 *		`MPEXMarkerGlyph2' / "MarkerGlyph2"
 *		`MPEXMarkerGlyph3' / "MarkerGlyph3"
 *		`MPEXMarkerGlyph4' / "MarkerGlyph4"
 *		`MPEXMarkerGlyph5' / "MarkerGlyph5"
 *		`MPEXMarkerGlyph6' / "MarkerGlyph6"
 *		`MPEXMarkerGlyph7' / "MarkerGlyph7"
 *		`MPEXMarkerGlyph8' / "MarkerGlyph8"
 *		`MPEXMarkerGlyph9' / "MarkerGlyph9"
 *		`MPEXMarkerGlyph10' / "MarkerGlyph10"
 *		`MPEXMarkerGlyph11' / "MarkerGlyph11"
 *		`MPEXMarkerGlyph12' / "MarkerGlyph12"
 *		`MPEXMarkerGlyph13' / "MarkerGlyph13"
 *		`MPEXMarkerGlyph14' / "MarkerGlyph14"
 *		`MPEXMarkerGlyph15' / "MarkerGlyph15"
 *		`MPEXMarkerGlyph16' / "MarkerGlyph16"
 *		<
 *	`PEXETHlhsrMode"
 *		>
 *		`MPEXHlhsrZBufferId' / "ZBufferId"
 *		<
 *
 * ERRORS:
 *	Drawable	Specified drawable resource identifier is invalid.
 *
 *	Value		Enumerated type number is invalid.
 *
 * SEE ALSO:
 *
 */

Status
PEXGetEnumTypeInfo (display, drawable, enumType, returnMask,
	enumInfoReturn, countReturn)

INPUT Display			*display;
INPUT Drawable			drawable;
INPUT int			enumType;
INPUT unsigned long		returnMask;
OUTPUT pxlEnumTypeDescList	**enumInfoReturn;
OUTPUT int			*countReturn;

{
/* AUTHOR: */
    pexGetEnumeratedTypeInfoReq		*req;
    pexGetEnumeratedTypeInfoReply	rep;
    unsigned char			*prepbuf;
    pxlEnumTypeDescList			*pretbuf;
    int					numDescs;
    int					length;
    int					i;
    char				*pstr;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReqExtra (GetEnumeratedTypeInfo, sizeof(CARD32), req);
    req->drawable = drawable;
    req->itemMask = returnMask;
    req->numEnums = 1;
    *((INT16 *)&req[1]) = enumType;

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
	UnlockDisplay (display);
	PEXSyncHandle ();
	return (1);	/* return an error */
    }

    /* allocate a scratch buffer */
    prepbuf = (unsigned char *)_XAllocScratch (display,
	(unsigned long)(rep.length << 2));

    /* copy the reply data to the buffer */
    _XRead (display, (char *)prepbuf, (long)(rep.length << 2));

    numDescs = *((CARD32 *)prepbuf);   /* number of records in the array */
    prepbuf += sizeof(CARD32);
    *countReturn = numDescs;

    /* allocate some storage to return the data records to the client */
    pretbuf = (pxlEnumTypeDescList *)PEXAllocBuf
	((unsigned)(sizeof(PEXHeader) + numDescs*sizeof(pxlEnumTypeDescList)));
    ((PEXHeader *)pretbuf)->free = _PEXFreeEnumTypeDesc; /* routine to free it */
    ((PEXHeader *)pretbuf)->size = numDescs;
    pretbuf = (pxlEnumTypeDescList *)&(((PEXHeader *)pretbuf)[1]);
    *enumInfoReturn = pretbuf;

    /* put the reply data into the records to be returned */
    for (i=0; i<numDescs; i++)
    {
	if (returnMask & PEXETIndex)   /* index values */
	{
	    pretbuf[i].enumVal = *((CARD16 *)prepbuf);
	    prepbuf += sizeof (CARD16);
	}
        else
        {
            pretbuf[i].enumVal = 0;
        }

	if (returnMask & PEXETMnemonic) /* mnemonic strings */
	{
	    length = *((CARD16 *)prepbuf);
	    prepbuf += sizeof (CARD16);
	    pretbuf[i].string = pstr =
		(char *)PEXAllocBuf ((unsigned)(length + 1));
	    COPY_AREA ((char *)prepbuf, (char *)pstr, length);
	    pstr[length] = 0;                   /* null terminate */
	    prepbuf += length;
	    if ((length & 1) != 0) prepbuf++;	/* pad to 16 if length odd   */
	}
	else
	    pretbuf[i].string = 0;
    }

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXGetListEnumTypeInfo
 *
 * FORMAT:
 * 	`PEXGetListEnumTypeInfo`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	drawable	The resource identifier of an X window or pixmap.
 *
 *	enumTypeList	List of enumerated types for which information is to
 *			be returned.
 *
 *	enumTypeCount   Number of entries in `enumTypeList'
 *
 *	returnMask	A mask indicating the enumerated type information
 *			to be returned.
 *
 *	enumInfoReturn	Returns a pointer to an array of records
 *			containing the enumeration type information.
 *
 *	countReturn	Returns a list of counts.  For each entry in 
 *			`enumTypeList' there is an entry in `countReturn'
 *			which indicates how many enum type records are stored
 *			stored in the `enumInfoReturn' array
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine is the same as `PEXGetEnumTypeInfo" except that a
 *	list of enum types may be specified.  This is faster than calling
 *	`PEXGetEnumTypeInfo" multiple times.
 *
 * ERRORS:
 *	Drawable	Specified drawable resource identifier is invalid.
 *
 *	Value		Enumerated type number is invalid.
 *
 * SEE ALSO:
 *
 */

Status
PEXGetListEnumTypeInfo (display, drawable, enumTypeList, enumTypeCount, 
	returnMask, enumInfoReturn, countReturn)

INPUT Display			*display;
INPUT Drawable			drawable;
INPUT int			*enumTypeList;
INPUT int			enumTypeCount;
INPUT unsigned long		returnMask;
OUTPUT pxlEnumTypeDescList	**enumInfoReturn;
OUTPUT int			**countReturn;

{
/* AUTHOR: */
    pexGetEnumeratedTypeInfoReq		*req;
    pexGetEnumeratedTypeInfoReply	rep;
    int					numDescs, totDescs;
    int					length;
    int					i,j;
    char				*pBuf;
    pxlEnumTypeDescList			*pretbuf;
    int					*pretcnt;
    char				*prepbuf, *pstartrep;
    CARD16				*dstEnumType;
    int					*srcEnumType;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReqExtra (GetEnumeratedTypeInfo, enumTypeCount * sizeof(CARD32), req);
    req->drawable = drawable;
    req->itemMask = returnMask;
    req->numEnums = enumTypeCount;
    for (i=0, dstEnumType= (CARD16*)&req[1], srcEnumType= enumTypeList;
	 i < enumTypeCount; 
	 i++, dstEnumType++, srcEnumType++)
    {
	*dstEnumType = (CARD16) *srcEnumType;
    }

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
	UnlockDisplay (display);
	PEXSyncHandle ();
	return (1);	/* return an error */
    }

    /* error if fewer than enumTypeCount lists returned */
    if (rep.numLists < enumTypeCount) 
    {
	UnlockDisplay (display);
	PEXSyncHandle ();
	return (1);	/* return an error */
    }

    /* allocate a scratch buffer */
    pstartrep = (char *)_XAllocScratch (display,
	(unsigned long)(rep.length << 2));

    /* copy the reply data to the buffer */
    _XRead (display, pstartrep, (long)(rep.length << 2));

    /* Count total number of enums returned */
    for (i = 0, totDescs = 0, prepbuf = pstartrep; i < rep.numLists; i++)
    { 
         numDescs = *((CARD32 *)prepbuf);
	 totDescs += numDescs; 

         prepbuf += sizeof(CARD32);	 /* skip count */
         if (i == rep.numLists - 1) break;

	 for (j = 0; j < numDescs; j++)  /* skip enum data */
	 {
	     if (returnMask & PEXETIndex)      /* skip index values */
	        prepbuf += sizeof (CARD16);
	     if (returnMask & PEXETMnemonic)   /* skip mnemonic strings */
	     {
	        length = *((CARD16 *)prepbuf);
	        if ( length & 0x1 ) length++;  /* pad to 16 bit boundary */
	        prepbuf += sizeof (CARD16) + length;
	     }
	 }

 	if ( ((unsigned long)prepbuf) & 0x2 )  /* skip padding */
	    prepbuf += sizeof( CARD16);
    }

    /* allocate mem for enum data to be returned to the client */
    pretbuf = (pxlEnumTypeDescList *)PEXAllocBuf
	((unsigned)(sizeof(PEXHeader) + totDescs*sizeof(pxlEnumTypeDescList)));
    ((PEXHeader *)pretbuf)->free = _PEXFreeEnumTypeDesc; /* free routine */
    ((PEXHeader *)pretbuf)->size = totDescs;
    pretbuf = (pxlEnumTypeDescList *)&(((PEXHeader *)pretbuf)[1]);
    *enumInfoReturn = pretbuf;

    /* allocate some storage to return the counts to the client */
    pretcnt = (int *)PEXAllocBuf
	((unsigned)(sizeof(PEXHeader) + enumTypeCount*sizeof(int)));
    ((PEXHeader *)pretcnt)->free = _PEXFreeList; /* free routine */
    pretcnt = (int *)&(((PEXHeader *)pretcnt)[1]);
    *countReturn = pretcnt;

    /* Retrieve the lists of enum info */
    for (i=0, prepbuf=pstartrep; i < rep.numLists; i++, pretcnt++)
    {
        /* retrieve count of enums in this list */
        *pretcnt = numDescs = *((CARD32 *)prepbuf);
        prepbuf += sizeof(CARD32);

        /* put the reply data into the records to be returned */
        for (j=0; j < numDescs; j++, pretbuf++)
        {
	    if (returnMask & PEXETIndex)   /* index values */
	    {
	        pretbuf->enumVal = *((CARD16 *)prepbuf);
	        prepbuf += sizeof (CARD16);
	    }
            else
            {
                pretbuf->enumVal = 0;
            }

	    if (returnMask & PEXETMnemonic) /* mnemonic strings */
	    {
	        length = *((CARD16 *)prepbuf);
	        prepbuf += sizeof (CARD16);
	        pretbuf->string = pBuf =
		   (char *)PEXAllocBuf ((unsigned)(length + 1));
	        COPY_AREA ((char *)prepbuf, (char *)pBuf, length);
	        pBuf[length] = 0;                   /* null terminate */
	        prepbuf += length;
	        if ( length & 0x1 ) prepbuf++;  /* pad to 16 bit boundary */
	    }
	    else
            {
	        pretbuf->string = 0;
            }
        }

	/* skip padding */
 	if ( ((unsigned long)prepbuf) & 0x2 )
	    prepbuf += sizeof( CARD16);
    }

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/*+
 * NAME:
 * 	PEXSetFloatingPointFormat
 *
 * FORMAT:
 * 	`PEXSetFloatingPointFormat`
 *
 * ARGUMENTS:
 *	fpFormat    An integer representing the floating-point format
 *		    to be used.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine sets the floating-point format to be used when sending
 *	floating-point values to or obtaining floating-point values from the
 *	PEX server extension.  Possible values for `fpFormat' are
 *	`PEXDEC_F_Floating' and `PEXIeee_754_32'. (Include file `PEX.h' contains
 *	definitions for these values.)  The default value for the
 *	version of PEXlib on Digital VAX workstations is `PEXDEC_F_Floating'.
 *	The default value for the version of PEXlib on Digital DECstations
 *	is `PEXIeee_754_32'.
 *	Typical application programs should never need to use this
 *	routine.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXGetFloatingPointFormat"
 *
 */

void
PEXSetFloatingPointFormat (fpFormat)

INPUT int	fpFormat;

{
/* AUTHOR: */
    pexFpFormat = fpFormat;

    return;
}

/*+
 * NAME:
 * 	PEXGetFloatingPointFormat
 *
 * FORMAT:
 * 	`PEXGetFloatingPointFormat`
 *
 * RETURNS:
 *	An integer representing the current floating-point format.
 *
 * DESCRIPTION:
 *	This routine queries the current floating-point format.  PEXlib uses the
 * 	floating-point format to understand how the application program
 * 	provides floating-point values to the PEX server extension.  PEXlib
 *	also uses the current floating-point format to return
 *	floating-point values to the application program in the application-
 *	specified format.  This routine returns one of the integer
 *	constants, `PEXDEC_F_Floating' or `PEXIeee_754_32'.  (Include file `PEX.h'
 *	contains definitions for these constants.)
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXSetFloatingPointFormat"
 *
 */

int
PEXGetFloatingPointFormat ()

{
/* AUTHOR: */
    return (pexFpFormat);
}

/*+
 * NAME:
 * 	PEXSetColourType
 *
 * FORMAT:
 * 	`PEXSetColourType`
 *
 * ARGUMENTS:
 *	colourType	An integer representing the colour type to be used.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine sets the colour type to be used when sending colour values to
 *	some PEXlib routines.  Some PEXlib routines require the colour type
 *	to be passed to them explicitly.  Other routines, such as output command
 *	routines having colours as part of vertex or facet lists, use the
 *	colour type set by calling this routine.
 *
 *	Possible values for `colourType' are `PEXIndexedColour', `PEXRgbFloatColour', `PEXCieFloatColour',
 *	`PEXHsvFloatColour', `PEXHlsFloatColour', `PEXRgb8Colour' and `PEXRgb16Colour'.
 *	`PEXIndexedColour' colours are passed as an integer colour table index.
 *	`PEXRgbFloatColour' colours are passed as a red, green, blue triple with
 *	values in the range of [0.0-1.0].
 *	`PEXCieFloatColour' colours consist of x, y, and luminance values in the
 *	range of [0.0-1.0].
 *	`PEXHsvFloatColour' colours consist of hue, saturation and value in the range
 *	of [0.0-1.0].
 *	`PEXHlsFloatColour' colours consist of hue, lightness, and
 *	saturation values in the range of [0.0-1.0].
 *	`PEXRgb8Colour' colours consist of red, green, and blue values
 *	in the range of [0-255].
 *	`PEXRgb16Colour' colours consist of red, green, and blue
 *	values in the range of [0-65535].
 *
 *	Include file `PEX.h' contains definitions for these constants.
 *	`PEXprotost.h' defines the data structures `pxlIndexedColour',
 *	`pxlRgbFloatColour',
 *	`pxlCieColour', `pxlHsvColour', `pxlHlsColour', `pxlRgb8Colour', and
 *	`pxlRgb16Colour' for creating and manipulating colours in
 *	each of the colour types.
 *
 *	The specified colour type should be one supported by the
 *	PEX server.  (See `PEXGetEnumTypeInfo".)  The default
 *	colour type is `PEXRgbFloatColour'.
 *
 *	The current colour type
 *	is used only when sending colours to PEXlib routines;
 *	colours returned from the PEXlib routines have a
 *	colour type field as part of the returned data.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXGetColourType"
 *
 */

void
PEXSetColourType (colourType)

INPUT int	colourType;

{
/* AUTHOR: */
    pexColourTypeHidden = colourType;

    return;
}

/*+
 * NAME:
 * 	PEXGetColourType
 *
 * FORMAT:
 * 	`PEXGetColourType`
 *
 * RETURNS:
 *	An integer representing the current colour type
 *
 * DESCRIPTION:
 *	This routine queries the current colour type.  PEXlib uses the
 *	colour type to understand how the application program is
 *	providing colour values to the PEX server extension.  This routine
 *	returns one of the integer constants `PEXIndexedColour', `PEXRgbFloatColour', `PEXCieFloatColour',
 *	`PEXHsvFloatColour', `PEXHlsFloatColour', `PEXRgb8Colour', or `PEXRgb16Colour'. (Include file `PEX.h'
 *	contains  definitions of these integer constants.)
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXSetColourType"
 *
 */

int
PEXGetColourType ()

{
/* AUTHOR: */
    return (pexColourTypeHidden);
}

/*+
 * NAME:
 * 	PEXGetImpDepConstants
 *
 * FORMAT:
 * 	`PEXGetImpDepConstants`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 *	drawable	The resource identifier of an X window or pixmap.
 *
 *	names		A list of constant values representing the
 *			implementation-dependent constants
 *			to be returned.
 *
 *	count		The number of entries in the `names' and
 *			`valuesReturn' arrays.
 *
 *	valuesReturn	The implementation-dependent values for those
 *			constants defined by the PEX server extension.
 *
 * RETURNS:
 *	Zero if the query was successful, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine allows an application program to query one or more
 * 	implementation-dependent constants in a PEX server extension.
 *	A single 32-bit integer or 32-bit floating-point value is
 *	returned for each value requested.  These values are returned
 *	in order, with one return value in `valuesReturn' for each
 *	requested value in `names'.  The number of constants being
 *	queried and returned is specified in `count'.  The
 *	implementation-dependent constants returned are based
 *	on the values used for a drawable having the same
 *	root and depth as `drawable'.
 *
 *	PEXGetImpDepConstants can query the following implementation-dependent
 *	constants:  (Include file `PEX.h' contains definitions of these
 *	constants.)
 *
 *	`PEXIDDitheringSupported' (integer) -
 *		Either `True' or `False', depending on whether the dithering
 *		hint in colour approximation lookup tables is
 *		actually used to control dithering.
 *
 *	`PEXIDMaxEdgeWidth' (integer) -
 *		Width (in pixels) of widest edge that can be drawn.
 *
 *	`PEXIDMaxLineWidth' (integer) -
 *		Width (in pixels) of widest line or curve that can be drawn.
 *
 *	`PEXIDMaxMarkerSize' (integer) -
 *		Largest dimension (either height or width, in pixels) of
 *		largest marker that can be drawn.  This maximum is exclusive
 *		of the marker type "PEXMarkerDot" which is always drawn as the
 *		smallest displayable point.
 *
 *	`PEXIDMaxModelClipPlanes' (integer) -
 *		Maximum number of modeling clipping planes may be defined.
 *
 *	`PEXIDMaxNameSetNames' (integer) -
 *		Maximum number of names allowed in a nameset.
 *
 *	`PEXIDMaxNonAmbientLights' (integer) -
 *		Maximum number of non-ambient light sources that can be enabled
 *		at one time.
 *
 *	`PEXIDMaxNURBOrder' (integer) -
 *		Maximum non-uniform rational B-spline order supported.
 *
 *	`PEXIDMaxTrimCurveOrder' (integer) -
 *		Maximum order for trim curves.
 *
 *	`PEXIDMinEdgeWidth' (integer) -
 *		Width (in pixels) of thinnest edge that can be drawn.
 *
 *	`PEXIDMinLineWidth' (integer) -
 *		Width (in pixels) of thinnest line or curve that can be drawn.
 *
 *	`PEXIDMinMarkerSize' (integer) -
 *		Largest dimension (either height or width, in pixels) of
 *		smallest marker that can be drawn.  This minimum is exclusive
 *	       	of the marker type "PEXMarkerDot" which is always drawn as the
 *		smallest displayable point.
 *
 *	`PEXIDNominalEdgeWidth' (integer) -
 *		Width (in pixels) of "standard" edge.
 *
 *	`PEXIDNominalLineWidth' (integer) -
 *		Width (in pixels) of "standard" line or curve.
 *
 *	`PEXIDNominalMarkerSize' (integer) -
 *		Largest dimension (either height or width, in pixels) of
 *		"standard" marker.
 *
 *	`PEXIDNumSupportedEdgeWidths' (integer) -
 *		Number of supported edge widths.  A value of 0 indicates
 *		all edge widths, including fractional widths, between the
 *		minimum and maximum edge width are supported.
 *
 *	`PEXIDNumSupportedLineWidths' (integer) -
 *		Number of supported line or curve widths.  A value of 0
 *		indicates all line widths, including fractional widths,
 *		between the minimum and maximum line width are supported.
 *
 *	`PEXIDNumSupportedMarkerSizes' (integer) -
 *		Number of supported marker sizes.  A value of 0 indicates
 *		all marker sizes, including fractional values, between the
 *		minimum and maximum marker size are supported.
 *
 *	`PEXIDBestColourApproximation' (integer) -
 *		Either `PEXColourApproxPowersOf2' or `PEXColourApproxAnyValues', depending
 *		on whether there is a signficant performance gain if the number
 *		of reds/greens/blues in the colour approximation table entry is
 *		a power of two, so pixels can be composed using shifts
 *		and adds.
 *
 *	`PEXIDTransparencySupported' (integer) -
 *		Either `True' or `False', depending on whether the
 *		transmission coefficient is utilized in the light source
 *		shading computations.
 *
 *	`PEXIDDoubleBufferingSupported' (integer) -
 *		Either `True' or `False', depending on whether or not the
 *		server supports double-buffering.
 *
 *	`PEXIDChromaticityRedU' (float) -
 *		CIELUV `u' chromaticity coefficient for the red channel
 *		of the (properly adjusted) display device.
 *
 *	`PEXIDChromaticityRedV' (float) -
 *		CIELUV `v' chromaticity coefficient for the red channel
 *		of the (properly adjusted) display device.
 *
 *	`PEXIDLuminanceRed' (float) -
 *		CIELUV luminance value for the red channel of the (properly
 *		adjusted) display device.
 *
 *	`PEXIDChromaticityGreenU' (float) -
 *		CIELUV `u' chromaticity coefficient for the green channel
 *		of the (properly adjusted) display device.
 *
 *	`PEXIDChromaticityGreenV' (float) -
 *		CIELUV `v' chromaticity coefficient for the green channel
 *		of the (properly adjusted) display device.
 *
 *	`PEXIDLuminanceGreen' (float) -
 *		CIELUV luminance value for the green channel of the (properly
 *		adjusted) display device.
 *
 *	`PEXIDChromaticityBlueU' (float) -
 *		CIELUV `u' chromaticity coefficient for the blue channel
 *		of the (properly adjusted) display device.
 *
 *	`PEXIDChromaticityBlueV' (float) -
 *		CIELUV `v' chromaticity coefficient for the blue channel
 *		of the (properly adjusted) display device.
 *
 *	`PEXIDLuminanceBlue' (float) -
 *		CIELUV luminance value for the blue channel of the (properly
 *		adjusted) display device.
 *
 *	`PEXIDChromaticityWhiteU' (float) -
 *		CIELUV `u' chromaticity coefficient for the reference white
 *		of the (properly adjusted) display device.
 *
 *	`PEXIDChromaticityWhiteV' (float) -
 *		CIELUV `v' chromaticity coefficient for the reference white
 *		of the (properly adjusted) display device.
 *
 *	`PEXIDLuminanceWhite' (float) -
 *		CIELUV luminance value for the reference white of the (properly
 *		adjusted) display device.
 *
 * ERRORS:
 *	Value			Specified constant name is invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	Drawable		Specified drawable resource identifier is
 *				invalid.
 *
 * SEE ALSO:
 *
 */

Status
PEXGetImpDepConstants (display, drawable, names, count, valuesReturn)

INPUT Display		*display;
INPUT Drawable		drawable;
INPUT short		names[];
INPUT int		count;
OUTPUT unsigned long	*valuesReturn;

{
/* AUTHOR: */
    pexGetImpDepConstantsReq	*req;
    pexGetImpDepConstantsReply	rep;

    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    /* put the request in the X request buffer */
    PEXGetReqExtra (GetImpDepConstants, count * sizeof(short),	req);
    req->fpFormat = pexFpFormat;
    req->drawable = drawable;
    req->numNames = count;
    COPY_AREA ((char *)names, (char *)&req[1], count * sizeof(short));

    /* get a reply */
    if (_XReply (display, &rep, 0, xFalse) == 0)
    {/* there was an error */
        UnlockDisplay (display);
        PEXSyncHandle ();
        return (1);            /* return an error */
    }

    /* copy the values into the client-supplied storage */
    _XRead (display, (char *)valuesReturn, (long)(count * sizeof(CARD32)));

    /* done, so unlock and check for synchronous-ness */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return (Success);
}

/* routine to return the PEX error message for PEX errors.  this is set up
in PEXInitialize */

char *
_PEXLookupErrorString (display, errorcode, extCodes, buffer, nbytes)

INPUT	Display		*display;
INPUT	int		errorcode;
INPUT	XExtCodes	*extCodes;
INPUT	char		*buffer;
INPUT	int		nbytes;

{
/* AUTHOR:  Sally C. Barry */
    char	*defaultp;
    char	buf[32];
    int		code;

    /* Note that there is no way with the current extension interface to
    access the opcode and numCorrect fields of the OutputCommand error */

    /* Xlib calls this for all errors.  Is this an error we know anything
    about? */

    code = errorcode - extCodes->first_error;
    if ((code <= PEXMaxError) && (code >= PEXMinError))
    {
	/* set up the default message for XGetErrorDatabaseText */
	defaultp = pexErrorList[code];

	/* even though the Xlib doc says this wants the application name in the
	2nd arg and the message type in the 3rd arg, Xlib calls it with the
	message type in the 2nd arg and the ASCIIized error code in the 3rd
	arg.  this seems to match what's in XErrorDB */

	/* convert the error code to ASCII */
	sprintf (buf, "%d", code);

	/* look up the error in the error database.  this is the proper way to
	do things, even though we have no errors there now. */
	XGetErrorDatabaseText (display, "PEXProtoError", buf, defaultp,
		buffer, nbytes);
    }

    /* the R3 code ignores the error return */
    return (buffer);
}
