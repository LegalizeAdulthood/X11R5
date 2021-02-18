/* $XConsortium: XcmsGRAY.c,v 1.6 91/02/12 16:12:51 dave Exp $" */

/*
 * Code and supporting documentation (c) Copyright 1990 1991 Tektronix, Inc.
 * 	All Rights Reserved
 * 
 * This file is a component of an X Window System-specific implementation
 * of Xcms based on the TekColor Color Management System.  Permission is
 * hereby granted to use, copy, modify, sell, and otherwise distribute this
 * software and its documentation for any purpose and without fee, provided
 * that this copyright, permission, and disclaimer notice is reproduced in
 * all copies of this software and in supporting documentation.  TekColor
 * is a trademark of Tektronix, Inc.
 * 
 * Tektronix makes no representation about the suitability of this software
 * for any purpose.  It is provided "as is" and with all faults.
 * 
 * TEKTRONIX DISCLAIMS ALL WARRANTIES APPLICABLE TO THIS SOFTWARE,
 * INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  IN NO EVENT SHALL TEKTRONIX BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA, OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR THE PERFORMANCE OF THIS SOFTWARE.
 *
 *
 *	NAME
 *		XcmsGRAY.c
 *
 *	DESCRIPTION
 *		This file contains the conversion routines:
 *		    1. CIE XYZ to RGB intensity
 *		    2. RGB intensity to device RGB
 *		    3. device RGB to RGB intensity
 *		    4. RGB intensity to CIE XYZ
 *
 */

#include <X11/Xatom.h>
#include "Xlibint.h"
#include "Xcmsint.h"

/*
 *      EXTERNS
 *              External declarations required locally to this package
 *              that are not already declared in any of the included header
 *		files (external includes or internal includes).
 */
extern char	XcmsRGBi_prefix[];
extern char	XcmsRGB_prefix[];
extern int	_XcmsValueCmp();
extern int	_XcmsIntensityCmp();
extern int	_XcmsValueInterpolation();
extern int	_XcmsIntensityInterpolation();
extern Status	_XcmsGetTableType0();
extern Status	_XcmsGetTableType1();
extern unsigned long _XcmsGetElement();
extern void _XcmsFreeIntensityMaps();


/*
 *      LOCAL DEFINES
 *		#define declarations local to this package.
 */
#define EPS	0.001
#ifndef MIN
#define MIN(x,y) ((x) > (y) ? (y) : (x))
#endif /* MIN */
#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif /* MAX */
#ifndef MIN3
#define MIN3(x,y,z) (MIN(x, MIN(y, z)))
#endif /* MIN3 */
#ifndef MAX3
#define MAX3(x,y,z) (MAX(x, MAX(y, z)))
#endif /* MAX3 */

/*
 *      LOCAL TYPEDEFS
 *              typedefs local to this package (for use with local vars).
 *
 */

/*
 *      FORWARD DECLARATIONS
 */
static int GRAY_InitSCCData();
static void GRAY_FreeSCCData();
Status GrayRGBiToRGB();
Status GrayRGBToRGBi();
Status GrayRGBiToCIEXYZ();
Status GrayCIEXYZToRGBi();
extern int XcmsLRGB_RGB_ParseString();
extern int XcmsLRGB_RGBi_ParseString();

/*
 *      LOCALS VARIABLES
 *		Variables local to this package.
 *		    Usage example:
 *		        static int	ExampleLocalVar;
 */

static XcmsFuncPtr Fl_RGB_to_CIEXYZ[] = {
    GrayRGBToRGBi,
    GrayRGBiToCIEXYZ,
    NULL
};

static XcmsFuncPtr Fl_CIEXYZ_to_RGB[] = {
    GrayCIEXYZToRGBi,
    GrayRGBiToRGB,
    NULL
};

static XcmsFuncPtr Fl_RGBi_to_CIEXYZ[] = {
    GrayRGBiToCIEXYZ,
    NULL
};

static XcmsFuncPtr Fl_CIEXYZ_to_RGBi[] = {
    GrayCIEXYZToRGBi,
    NULL
};

cmsColorSpace	XcmsGrayRGBiColorSpace =
    {
	XcmsRGBi_prefix,		/* prefix */
	XcmsRGBiFormat,		/* id */
	XcmsLRGB_RGBi_ParseString,              /* parseString */
	Fl_RGBi_to_CIEXYZ,	/* to_CIEXYZ */
	Fl_CIEXYZ_to_RGBi,	/* from_CIEXYZ */
	1
    };
cmsColorSpace	XcmsGrayRGBColorSpace =
    {
	XcmsRGB_prefix,		/* prefix */
	XcmsRGBFormat,		/* id */
	XcmsLRGB_RGB_ParseString,               /* parseString */
	Fl_RGB_to_CIEXYZ,	/* to_CIEXYZ */
	Fl_CIEXYZ_to_RGB,	/* from_CIEXYZ */
	1
    };

static XcmsColorSpace	*DDColorSpaces[] = {
    &XcmsGrayRGBColorSpace,
    &XcmsGrayRGBiColorSpace,
    NULL
};


/*
 *      GLOBALS
 *              Variables declared in this package that are allowed
 *		to be used globally.
 */

cmsSCCFuncSet	XcmsGrayFunctionSet =
    {
	DDColorSpaces,		/* pDDColorSpaces */
	GRAY_InitSCCData,	/* pInitScrnFunc */
	GRAY_FreeSCCData	/* pFreeSCCData */
    };


/************************************************************************
 *									*
 *			PRIVATE ROUTINES				*
 *									*
 ************************************************************************/

/*
 *	NAME
 *		GRAY_InitSCCData()
 *
 *	SYNOPSIS
 */
static int
GRAY_InitSCCData(dpy, screenNumber, pPerScrnInfo)
    Display *dpy;
    int screenNumber;
    XcmsPerScrnInfo *pPerScrnInfo;
/*
 *	DESCRIPTION
 *
 *	RETURNS
 *		0 if failed.
 *		1 if succeeded with no modifications.
 *
 */
{
    Atom  CorrectAtom = XInternAtom (dpy, XDCCC_GRAY_CORRECT_ATOM_NAME, True);
    Atom  ScrnWhitePtAtom  = XInternAtom (dpy, XDCCC_SCREENWHITEPT_ATOM_NAME, True);
    int	  format, count, cType;
    unsigned long nitems, nbytes_return;
    char *property_return, *pChar;
    XcmsFloat *pValue;
    XcmsColor *pScrnWhitePt = &pPerScrnInfo->screenWhitePt;
    VisualID visualID;

    GRAY_SCCData *pScreenData, *pScreenDefaultData;
    XcmsIntensityMap *pNewMap;

    /* 
     *  Check to make sure both atoms existed.  If they did not
     *  the characterization data does not exist.
     */
    if (CorrectAtom == None || ScrnWhitePtAtom == None) {
	return (XcmsFailure);
    }
    /*
     * Allocate memory for pScreenData
     */
    if (!(pScreenData = pScreenDefaultData = (GRAY_SCCData *) 
	    Xcalloc (1, sizeof(GRAY_SCCData)))) {
	return (XcmsFailure);
    }

    /*
     *	1. Get the Screen's White Point
     */

    /* 
     *  First try to see if the XDCCC Screen White Point property is
     *  loaded on the root window.  If it is then read it and set the
     *  screen info using this property.  If it is not use the default.
     */
    if ((_XcmsGetProperty (dpy, RootWindow(dpy, screenNumber), 
			  ScrnWhitePtAtom, &format, &nitems, 
			  &nbytes_return, &property_return) == XcmsFailure)
	    || nitems != 3 || format != 32) {
	/* when debug is on then use the statics otherwise just return. */
	goto FreeAndReturnFailure;
    }

    /* Get the Screen White Point (assumed to be in XYZ) */
    pValue = (XcmsFloat *) &pScrnWhitePt->spec.CIEXYZ.X;
    pChar = property_return;
    for (count = 0; count < 3; count++) {
	*pValue++ = _XcmsGetElement (format, &pChar, &nitems) / 
		    (XcmsFloat) XDCCC_NUMBER;
    }
    XFree ((char *)property_return);
    if ((pScrnWhitePt->spec.CIEXYZ.Y < (1.0 - EPS) )
	    || (pScrnWhitePt->spec.CIEXYZ.Y > (1.0 + EPS))) {
	goto FreeAndReturnFailure;
    } else {
	pScrnWhitePt->spec.CIEXYZ.Y = 1.0;
    }
    pScrnWhitePt->format = XcmsCIEXYZFormat;
    pScrnWhitePt->pixel = 0;
#ifdef PDEBUG
    printf ("Screen White Pt value: %f %f %f\n",
	    pPerScrnInfo->screenWhitePt.spec.CIEXYZ.X,
	    pPerScrnInfo->screenWhitePt.spec.CIEXYZ.Y,
	    pPerScrnInfo->screenWhitePt.spec.CIEXYZ.Z);
#endif /* PDEBUG */

    /*
     *	4. Get the Intensity Profile if it exists
     */
    if ((_XcmsGetProperty (dpy, RootWindow(dpy, screenNumber), CorrectAtom,
			   &format, &nitems, &nbytes_return, 
			   &property_return) == XcmsFailure)
	     || nitems < 1) {
	XFree ((char *)property_return);
	goto FreeAndReturnFailure;
    }

    /* Get the gray correction data */
    pChar = property_return;

    while (nitems) {
	switch (format) {
	  case 8:
	    /*
	     * Must have at lease:
	     *		VisualID0
	     *		VisualID1
	     *		VisualID2
	     *		VisualID3
	     *		type
	     *		count
	     *		length
	     *		intensity1
	     *		intensity2
	     */
	    if (nitems < 9) {
		XFree ((char *)property_return);
		goto FreeAndReturnFailure;
	    }
	    count = 3;
	    break;
	  case 16:
	    /*
	     * Must have at lease:
	     *		VisualID0
	     *		VisualID3
	     *		type
	     *		count
	     *		length
	     *		intensity1
	     *		intensity2
	     */
	    if (nitems < 7) {
		XFree ((char *)property_return);
		goto FreeAndReturnFailure;
	    }
	    count = 1;
	    break;
	  case 32:
	    /*
	     * Must have at lease:
	     *		VisualID0
	     *		type
	     *		count
	     *		length
	     *		intensity1
	     *		intensity2
	     */
	    if (nitems < 6) {
		XFree ((char *)property_return);
		goto FreeAndReturnFailure;
	    }
	    count = 0;
	    break;
	  default:
		XFree ((char *)property_return);
		goto FreeAndReturnFailure;
	    break;
	}

	/*
	 * Get VisualID
	 */
	visualID = _XcmsGetElement(format, &pChar, &nitems);
	while (count--) {
	    visualID = visualID << 8;
	    visualID |= _XcmsGetElement(format, &pChar, &nitems);
	}

	if (visualID == 0) {
	    /*
	     * This is a shared intensity table
	     */
	    pScreenData = pScreenDefaultData;
	} else {
	    /*
	     * This is a per-Visual intensity table
	     */
	    if (!(pScreenData = (GRAY_SCCData *) 
			      Xcalloc (1, sizeof(GRAY_SCCData)))) {
		return(XcmsFailure);
	    }
	    /* copy matrices */
	    bcopy((char *)pScreenDefaultData, (char *)pScreenData,
		    18 * sizeof(XcmsFloat));

	    /* Create, initialize, and add map */
	    if (!(pNewMap = (XcmsIntensityMap *) 
			      Xcalloc (1, sizeof(XcmsIntensityMap)))) {
		Xfree(pScreenData);
		return(XcmsFailure);
	    }
	    pNewMap->visualID = visualID;
	    pNewMap->screenData = (XPointer)pScreenData;
	    pNewMap->pFreeScreenData = GRAY_FreeSCCData;
	    pNewMap->pNext =
		    (XcmsIntensityMap *)dpy->cms.perVisualIntensityMaps;
	    (XcmsIntensityMap *)dpy->cms.perVisualIntensityMaps = pNewMap;
	    dpy->free_funcs->intensityMaps = _XcmsFreeIntensityMaps;
	}
	cType = _XcmsGetElement(format, &pChar, &nitems);
	if (!(pScreenData->IntensityTbl = (IntensityTbl *)
		Xcalloc(1, sizeof(IntensityTbl)))) {
	    goto FreeAndReturnFailure;
	}
	switch (cType) {
	  case 0:
	    if (_XcmsGetTableType0(pScreenData->IntensityTbl, format,
		    &pChar, &nitems) == XcmsFailure) {
		goto FreeAndReturnFailure;
	    }
	    break;
	  case 1:
	    if (_XcmsGetTableType1(pScreenData->IntensityTbl, format,
		    &pChar, &nitems) == XcmsFailure) {
		goto FreeAndReturnFailure;
	    }
	    break;
	  default:
	    goto FreeAndReturnFailure;
	}
    }
    XFree ((char *)property_return);
#ifdef ALLDEBUG
    printf ("Intensity Table  GRAY   %d\n", nElements);
    pIRec = (IntensityRec *) pScreenData->IntensityTbl->pBase;
    for (count = 0; count < nElements; count++, pIRec++) {
	printf ("\t0x%4x\t%f\n", pIRec->value, pIRec->intensity);
    }
#endif /* ALLDEBUG */

    /* Free the old memory and use the new structure created. */
    (void) GRAY_FreeSCCData ((GRAY_SCCData *)pPerScrnInfo->screenData);

    pPerScrnInfo->functionSet = (caddr_t) &XcmsGrayFunctionSet;

    pPerScrnInfo->screenData = (caddr_t) pScreenData;

    pPerScrnInfo->state = XcmsInitSuccess;

    return (XcmsSuccess);

FreeAndReturnFailure:
    GRAY_FreeSCCData(pScreenData);
    pPerScrnInfo->state = XcmsInitNone;
    return (XcmsFailure);
}


/*
 *	NAME
 *		GRAY_FreeSCCData()
 *
 *	SYNOPSIS
 */
static void
GRAY_FreeSCCData(pScreenData)
    GRAY_SCCData *pScreenData;
/*
 *	DESCRIPTION
 *
 *	RETURNS
 *		0 if failed.
 *		1 if succeeded with no modifications.
 *
 */
{
    if (pScreenData) {
	if (pScreenData->IntensityTbl) {
	    if (pScreenData->IntensityTbl->pBase) {
		free (pScreenData->IntensityTbl->pBase);
	    }
	    free (pScreenData->IntensityTbl);
	}
	free (pScreenData);
    }
}




/************************************************************************
 *									*
 *			 PUBLIC ROUTINES				*
 *									*
 ************************************************************************/

/*
 *	NAME
 *		GrayCIEXYZToRGBi - convert CIE XYZ to RGB
 *
 *	SYNOPSIS
 */
/* ARGSUSED */
Status 
GrayCIEXYZToRGBi(ccc, pXcmsColors_in_out, nColors, pCompressed)
    XcmsCCC ccc;
    XcmsColor *pXcmsColors_in_out;/* pointer to XcmsColors to convert 	*/
    unsigned int nColors;	/* Number of colors			*/
    Bool *pCompressed;		/* pointer to an array of Bool		*/
{
    XcmsColor scrnWhitePt;
    XcmsFloat intensity;
    int hasCompressed = 0;
    int i;

    if (ccc == NULL) {
	return(XcmsFailure);
    }

    /*
     * XcmsColors should be White Point Adjusted, if necessary, by now!
     */

    /*
     * Convert Screen White Point to CIEuvY if not already.
     */
    bcopy((char *)&ccc->pPerScrnInfo->screenWhitePt, (char *)&scrnWhitePt,
	    sizeof(XcmsColor));
    if (scrnWhitePt.format != XcmsCIEuvYFormat) {
	if (_XcmsDIConvertColors(ccc, &scrnWhitePt, NULL, 1,
		XcmsCIEuvYFormat) == 0) {
	    return(XcmsFailure);
	}
    }


    for (i = 0; i < nColors; i++) {

	/* Make sure format is XcmsCIEXYZFormat */
	if (pXcmsColors_in_out->format != XcmsCIEXYZFormat) {
	    return (XcmsFailure);
	}
	/*
	 * Now convert to CIEuvY for comparison
	 */
	if (_XcmsDIConvertColors(ccc, pXcmsColors_in_out, NULL, 1,
		XcmsCIEuvYFormat) == 0) {
	    return(XcmsFailure);
	}
	/*
	 * If specifications u_prime and v_prime not equal to that of the
	 * Screen White Point then the color is out of gamut!!
	 * Otherwise, just convert Y to RGB.
	 */

	if ((pXcmsColors_in_out->spec.CIEuvY.u_prime <
		(scrnWhitePt.spec.CIEuvY.u_prime - EPS))
		|| (pXcmsColors_in_out->spec.CIEuvY.v_prime >
		(scrnWhitePt.spec.CIEuvY.v_prime + EPS))) {
	    /*
	     * Color out of screen's gamut
	     */
	    if (ccc->gamutCompProc == NULL) {
		return (XcmsFailure);
	    }
	    if (_XcmsDIConvertColors(ccc, pXcmsColors_in_out, NULL, 1,
		    XcmsCIEXYZFormat) == 0) {
		return(XcmsFailure);
	    }
	    if ((*ccc->gamutCompProc)(ccc, pXcmsColors_in_out, nColors,
		    i, pCompressed) == 0) {
		return (XcmsFailure);
	    }
	    /*
	     * The gamut compression function should return colors in CIEXYZ
	     */
	    if (pXcmsColors_in_out->format != XcmsCIEXYZFormat) {
		return (XcmsFailure);
	    }
	    /* flag as compressed */
	    if (pCompressed != NULL) {
		*pCompressed++ = True;
		hasCompressed++;
	    }
	} else {
	    /*
	     * Color in screen's gamut
	     *   It's still in CIEuvY so convert back to CIEXYZ
	     */
	    if (_XcmsDIConvertColors(ccc, pXcmsColors_in_out, NULL, 1,
		    XcmsCIEXYZFormat) == 0) {
		return(XcmsFailure);
	    }
	    /* flag as not compressed */
	    if (pCompressed != NULL) {
		*pCompressed++ = False;
	    }
	}
	intensity = pXcmsColors_in_out->spec.CIEXYZ.Y;
	pXcmsColors_in_out->spec.RGBi.red = 
	    pXcmsColors_in_out->spec.RGBi.green = 
	    pXcmsColors_in_out->spec.RGBi.blue = intensity;
	(pXcmsColors_in_out++)->format = XcmsRGBiFormat;
    }
    return (hasCompressed ? XcmsSuccessWithCompression : XcmsSuccess);
}


/*
 *	NAME
 *		GRAYi_to_CIEXYZ - convert RGBi to CIEXYZ
 *
 *	SYNOPSIS
 */
/* ARGSUSED */
Status 
GrayRGBiToCIEXYZ(ccc, pXcmsColors_in_out, nColors, pCompressed)
    XcmsCCC ccc;
    XcmsColor *pXcmsColors_in_out;/* pointer to XcmsColors to convert 	*/
    unsigned int nColors;	/* Number of colors			*/
    char *pCompressed;		/* pointer to a bit array		*/
{
    XcmsColor scrnWhitePt;
    XcmsFloat intensity;

    /*
     * pCompressed ignored in this function.
     */

    if (ccc == NULL) {
	return(XcmsFailure);
    }

    /*
     * Convert Screen White Point to CIEuvY if not already.
     */
    bcopy((char *)&ccc->pPerScrnInfo->screenWhitePt, (char *)&scrnWhitePt,
	    sizeof(XcmsColor));
    if (scrnWhitePt.format != XcmsCIEuvYFormat) {
	if (_XcmsDIConvertColors(ccc, &scrnWhitePt, NULL, 1, XcmsCIEuvYFormat)==0){
	    return(XcmsFailure);
	}
    }

    while (nColors--) {

	if ((pXcmsColors_in_out->spec.RGBi.red !=
		pXcmsColors_in_out->spec.RGBi.green) &&
		(pXcmsColors_in_out->spec.RGBi.green !=
		pXcmsColors_in_out->spec.RGBi.blue)) {
	    return(XcmsFailure);
	}
	intensity = pXcmsColors_in_out->spec.CIEXYZ.Y;
	pXcmsColors_in_out->spec.CIEuvY.u_prime =
		scrnWhitePt.spec.CIEuvY.u_prime;
	pXcmsColors_in_out->spec.CIEuvY.v_prime =
		scrnWhitePt.spec.CIEuvY.v_prime;
	pXcmsColors_in_out->spec.CIEuvY.Y = intensity;
	pXcmsColors_in_out->format = XcmsCIEuvYFormat;
	/*
	 * Now convert to CIEXYZ
	 */
	if (_XcmsDIConvertColors(ccc, pXcmsColors_in_out, NULL, 1,
		XcmsCIEXYZFormat) == 0) {
	    return(XcmsFailure);
	}
	pXcmsColors_in_out++;
    }
    return (XcmsSuccess);
}


/*
 *	NAME
 *		GrayRGBiToRGB
 *
 *	SYNOPSIS
 */
/* ARGSUSED */
Status 
GrayRGBiToRGB(ccc, pXcmsColors_in_out, nColors, pCompressed)
    XcmsCCC ccc;
    XcmsColor *pXcmsColors_in_out;/* pointer to XcmsColors to convert 	*/
    unsigned int nColors;	/* Number of colors			*/
    char *pCompressed;		/* pointer to a bit array		*/
/*
 *	DESCRIPTION
 *		Converts color specifications in an array of XcmsColor
 *		structures from RGBi format to RGB format.
 *
 *	RETURNS
 *		XcmsFailure if failed,
 *		XcmsSuccess if succeeded without gamut compression.
 *		XcmsSuccessWithCompression if succeeded with gamut
 *			compression.
 */
{
    GRAY_SCCData *pScreenData;
    XcmsRGB tmpRGB;
    IntensityRec keyIRec, answerIRec;

    /*
     * pCompressed ignored in this function.
     */

    if (ccc == NULL) {
	return(XcmsFailure);
    }

    pScreenData = (GRAY_SCCData *)ccc->pPerScrnInfo->screenData;

    while (nColors--) {

	/* Make sure format is XcmsRGBiFormat */
	if (pXcmsColors_in_out->format != XcmsRGBiFormat) {
	    return(XcmsFailure);
	}

	keyIRec.intensity = pXcmsColors_in_out->spec.RGBi.red;
	if (!_XcmsTableSearch((char *)&keyIRec, ccc->visual->bits_per_rgb,
		(char *)pScreenData->IntensityTbl->pBase,
		(unsigned)pScreenData->IntensityTbl->nEntries,
		(unsigned)sizeof(IntensityRec),
		_XcmsIntensityCmp, _XcmsIntensityInterpolation, (char *)&answerIRec)) {
	    return(XcmsFailure);
	}
	tmpRGB.red = answerIRec.value;

	keyIRec.intensity = pXcmsColors_in_out->spec.RGBi.green;
	if (!_XcmsTableSearch((char *)&keyIRec, ccc->visual->bits_per_rgb,
		(char *)pScreenData->IntensityTbl->pBase,
		(unsigned)pScreenData->IntensityTbl->nEntries,
		(unsigned)sizeof(IntensityRec),
		_XcmsIntensityCmp, _XcmsIntensityInterpolation, (char *)&answerIRec)) {
	    return(XcmsFailure);
	}
	tmpRGB.green = answerIRec.value;

	keyIRec.intensity = pXcmsColors_in_out->spec.RGBi.blue;
	if (!_XcmsTableSearch((char *)&keyIRec, ccc->visual->bits_per_rgb,
		(char *)pScreenData->IntensityTbl->pBase,
		(unsigned)pScreenData->IntensityTbl->nEntries,
		(unsigned)sizeof(IntensityRec),
		_XcmsIntensityCmp, _XcmsIntensityInterpolation, (char *)&answerIRec)) {
	    return(XcmsFailure);
	}
	tmpRGB.blue = answerIRec.value;

	bcopy((char *)&tmpRGB, (char *)&pXcmsColors_in_out->spec, sizeof(XcmsRGB));
	(pXcmsColors_in_out++)->format = XcmsRGBFormat;
    }
    return(XcmsSuccess);
}


/*
 *	NAME
 *		GrayRGBToRGBi
 *
 *	SYNOPSIS
 */
/* ARGSUSED */
Status 
GrayRGBToRGBi(ccc, pXcmsColors_in_out, nColors, pCompressed)
    XcmsCCC ccc;
    XcmsColor *pXcmsColors_in_out;/* pointer to XcmsColors to convert 	*/
    unsigned int nColors;	/* Number of colors			*/
    char *pCompressed;		/* pointer to a bit array		*/
/*
 *	DESCRIPTION
 *		Converts color specifications in an array of XcmsColor
 *		structures from RGB format to RGBi format.
 *
 *	RETURNS
 *		XcmsFailure if failed,
 *		XcmsSuccess if succeeded.
 */
{
    GRAY_SCCData *pScreenData;
    XcmsRGBi tmpRGBi;
    IntensityRec keyIRec, answerIRec;

    /*
     * pCompressed ignored in this function.
     */

    if (ccc == NULL) {
	return(XcmsFailure);
    }

    pScreenData = (GRAY_SCCData *)ccc->pPerScrnInfo->screenData;

    while (nColors--) {

	/* Make sure format is XcmsRGBFormat */
	if (pXcmsColors_in_out->format != XcmsRGBFormat) {
	    return(XcmsFailure);
	}

	keyIRec.value = pXcmsColors_in_out->spec.RGB.red;
	if (!_XcmsTableSearch((char *)&keyIRec, ccc->visual->bits_per_rgb,
		(char *)pScreenData->IntensityTbl->pBase,
		(unsigned)pScreenData->IntensityTbl->nEntries,
		(unsigned)sizeof(IntensityRec),
		_XcmsValueCmp, _XcmsValueInterpolation, (char *)&answerIRec)) {
	    return(XcmsFailure);
	}
	tmpRGBi.red = answerIRec.intensity;

	keyIRec.value = pXcmsColors_in_out->spec.RGB.green;
	if (!_XcmsTableSearch((char *)&keyIRec, ccc->visual->bits_per_rgb,
		(char *)pScreenData->IntensityTbl->pBase,
		(unsigned)pScreenData->IntensityTbl->nEntries,
		(unsigned)sizeof(IntensityRec),
		_XcmsValueCmp, _XcmsValueInterpolation, (char *)&answerIRec)) {
	    return(XcmsFailure);
	}
	tmpRGBi.green = answerIRec.intensity;

	keyIRec.value = pXcmsColors_in_out->spec.RGB.blue;
	if (!_XcmsTableSearch((char *)&keyIRec, ccc->visual->bits_per_rgb,
		(char *)pScreenData->IntensityTbl->pBase,
		(unsigned)pScreenData->IntensityTbl->nEntries,
		(unsigned)sizeof(IntensityRec),
		_XcmsValueCmp, _XcmsValueInterpolation, (char *)&answerIRec)) {
	    return(XcmsFailure);
	}
	tmpRGBi.blue = answerIRec.intensity;

	bcopy((char *)&tmpRGBi, (char *)&pXcmsColors_in_out->spec, sizeof(XcmsRGBi));
	(pXcmsColors_in_out++)->format = XcmsRGBiFormat;
    }
    return(XcmsSuccess);
}
