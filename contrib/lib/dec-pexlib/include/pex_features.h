
/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
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
 *
 *  File Name:
 *     pex_features.h
 *
 *  Subsystem:
 *     Common PEX
 *
 *  Version: 
 *     0.1
 *
 *  Abstract:
 *     Define features used in all of pex
 *
 *  Keywords:
 *     pex
 *
 *  Environment:
 *     Common pex, all components
 *
 *  Author:
 *     Andrew F. Vesper, HPWS, Common PEX
 *
 *  Creation Date:
 *     8-Aug-1988
 *
 *  Modification History:
 *     5-May-1989   AFV		Add MULTI_FORMAT
 *     6-Jun-1989   AFV		Change PEXNAME to X3D-PEX
 *    28-Jul-1989   AFV		Add definitions for VAX, UNIX and ULTRIX
 *				as required. Also change PEXNAME back to
 *				X3D-PEX.
 *    28-Sep-1989   AFV		Define USE_BUILTINS for vaxc compiles
 *				Define CONVERT_COLOR
 *     7-Nov-1989   AFV		Define NATIVE_FLOATING_POINT only if not
 *				already defined. Define
 *				DI_NATIVE_FLOATING_POINT always.
 *	16-Nov-1989 AFV	    Get rid of firefox picking stuff.
 *	 1-Dec-1989 AFV	    Add DEFAULT_FONT_NAME.
 *	28-Feb-1990 AFV	    Add PEX_RELEASE_NUMBER.
 *	18-Apr-1990  AFV    Add #ifndef around #define PRODUCTION
 *	18-Apr-1990  AFV    Fix the VENDOR_NAME string
 *	10-Apr-91   AFV	    Convert to PEX v5.0P:
 *				name changes
 *
 */

#ifndef FEATURES_DEFINED

#define FEATURES_DEFINED

#if defined (vax) && ! defined (VAX)
#   define  VAX
#endif

#if defined (unix) && ! defined (UNIX)
#   define  UNIX
#endif

#if defined (ultrix) && ! defined (ULTRIX)
#   define  ULTRIX
#endif



/* what options are to be enabled? */

#ifndef GEMC
#ifdef vaxc
#   define  USE_BUILTINS    1
#endif
#endif /* GEMC */

#define CONVERT_COLOR	    1

/* fixed capacities */


/* special features */

#ifndef NATIVE_FLOATING_POINT
# ifdef VAX
#   define NATIVE_FLOATING_POINT PEXDEC_F_Floating
# else
#   define NATIVE_FLOATING_POINT PEXIeee_754_32
# endif
#endif

#ifdef VAX
#   define DI_NATIVE_FLOATING_POINT PEXDEC_F_Floating
#else
#   define DI_NATIVE_FLOATING_POINT PEXIeee_754_32
#endif

#define NATIVE_COLOR_TYPE	    PEXRgbFloatColour
#define NATIVE_COLOR_SIZE	    sizeof (pexRgbFloatColour)

#define PEXNAME "X3D-PEX"

#define DEFAULT_VENDOR_NAME "Digital Equipment Corporation © 1990"

#define DEFAULT_FONT_NAME	    "font_neg01"

/*
 * Digital PEX release numbers correspond to the MPEX defining document
 * version number times 100. Currently the MPEX document is version 1.8.
 */

#define PEX_RELEASE_NUMBER	    180

/*
 * if PARANOID is defined, check all BOOL values for either BOOL_TRUE
 * or BOOL_FALSE.  Other checks may depend on this also.
 */

#ifdef DEBUG
#  define PARANOID		1
#endif

/*
 * do we want to use function prototypes?
 */

#if defined(__STDC__) || defined (VAXC)
#  define ROUTINE_TEMPLATES
#endif

/*
 * Is this production code?  0 means no, do extra checks and print out
 * error messages.
 */

#ifndef PRODUCTION
#   define PRODUCTION	0
#endif

#endif /* FEATURES_DEFINED */
/*  DEC/CMS REPLACEMENT HISTORY, Element PEX_FEATURES.H */
/*  *15   18-APR-1990 16:11:29 VESPER "Add #ifndef around #define PRODUCTION; change VENDOR_NAME" */
/*  *14   28-FEB-1990 16:27:17 VESPER "Added PEX_RELEASE_NUMBER" */
/*  *13    6-DEC-1989 17:19:02 VESPER "Get rid of FF picking; add DEFAULT_FONT_NAME" */
/*  *12   14-NOV-1989 16:31:46 VESPER "Don't REdefine NATIVE_FLOATING_POINT; define DI_NATIVE_FLOATING_POINT" */
/*  *11   23-OCT-1989 15:04:33 VESPER "No more non-DEBUG paranoia" */
/*  *10    6-OCT-1989 17:07:06 VESPER "Add USE_BUILTINS and CONVERT_COLOR" */
/*  *9     7-AUG-1989 14:52:45 VESPER "PEXNAME is X3D-PEX once more" */
/*  *8    25-JUL-1989 15:58:50 NISHIMOTO "change back to PEX for Siggraph" */
/*  *7    17-JUL-1989 11:21:22 VESPER "Can finally name this thing properly (thanks, Sally)" */
/*  *6    15-MAY-1989 16:14:25 VESPER "Add header, NATIVE_COLOR_FORMAT" */
/*  *5     9-MAR-1989 11:08:49 VESPER "VENDOR_NAME now defined" */
/*  *4    14-DEC-1988 13:39:54 VESPER "Added NATIVE_FLOAT_FORMAT" */
/*  *3     1-DEC-1988 13:24:07 VESPER "use routine templates" */
/*  *2    18-NOV-1988 13:37:44 VESPER "added FFPICK_WANTED and PRODUCTION" */
/*  *1     8-AUG-1988 14:03:35 VESPER "Features of PEX to be supported" */
/*  DEC/CMS REPLACEMENT HISTORY, Element PEX_FEATURES.H */
