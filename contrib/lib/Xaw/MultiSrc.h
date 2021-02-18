/*
 * $XConsortium: MultiSrc.h,v 1.13 91/07/22 19:02:47 converse Exp $
 */

/*
 * Copyright 1991 by OMRON Corporation
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON and MIT not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON and MIT make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * OMRON AND MIT DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL OMRON OR MIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 *      Author: Li Yuhong	 OMRON Corporation
 */

/* Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 */


/*
 * MultiSrc.h - Public Header file for Multi Text Source.
 *
 * This is the public header file for the Multi Text Source.
 * It is intended to be used with the Text widget, the simplest way to use
 * this text source is to use the MultiText Object.
 *
 * Date:    June 29, 1989
 *
 * By:      Chris D. Peterson
 *          MIT X Consortium 
 *          kit@expo.lcs.mit.edu
 */

/*
 * This file was changed from AsciiSrc.h.
 *
 * By Li Yuhong, Sept. 18, 1990
 */

#ifndef _XawMultiSrc_h
#define _XawMultiSrc_h

#include <X11/Xaw/TextSrc.h>
#include <X11/Xfuncproto.h>

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 callback	     Callback		Callback	(none)
 dataCompression     DataCompression	Boolean		True
 length		     Length		int		(internal)
 pieceSize	     PieceSize		int		BUFSIZ
 string		     String		String		NULL
 type		     Type		XawMultiType	XawMultiString
 useStringInPlace    UseStringInPlace	Boolean		False

*/
 
/* Class record constants */

extern WidgetClass multiSrcObjectClass;

typedef struct _MultiSrcClassRec *MultiSrcObjectClass;
typedef struct _MultiSrcRec      *MultiSrcObject;

/*
 * Just to make people's lives a bit easier.
 */

#define MultiSourceObjectClass MultiSrcObjectClass
#define MultiSourceObject      MultiSrcObject

/*
 * Resource Definitions.
 */

#define XtCDataCompression "DataCompression"
#define XtCPieceSize "PieceSize"
#define XtCType "Type"
#define XtCUseStringInPlace "UseStringInPlace"

#define XtNdataCompression "dataCompression"
#define XtNpieceSize "pieceSize"
#define XtNtype "type"
#define XtNuseStringInPlace "useStringInPlace"

#define XtRMultiType "MultiType"

#define XtEstring "string"
#define XtEfile "file"

typedef enum {XawMultiFile, XawMultiString} XawMultiType;

/************************************************************
 *
 * Public routines 
 *
 ************************************************************/

_XFUNCPROTOBEGIN

/*	Function Name: XawMultiSourceFreeString
 *	Description: Frees the string returned by a get values call
 *                   on the string when the source is of type string.
 *	Arguments: w - the MultiSrc object.
 *	Returns: none.
 */

extern void XawMultiSourceFreeString(
#if NeedFunctionPrototypes
    Widget		/* w */
#endif
);

/*	Function Name: XawMultiSave
 *	Description: Saves all the pieces into a file or string as required.
 *	Arguments: w - the multiSrc Object.
 *	Returns: TRUE if the save was successful.
 */

extern Boolean XawMultiSave(
#if NeedFunctionPrototypes
    Widget		/* w */
#endif
);

/*	Function Name: XawMultiSaveAsFile
 *	Description: Save the current buffer as a file.
 *	Arguments: w - the multiSrc object.
 *                 name - name of the file to save this file into.
 *	Returns: True if the save was successful.
 */

extern Boolean XawMultiSaveAsFile(
#if NeedFunctionPrototypes
    Widget		/* w */,
    _Xconst char*	/* name */
#endif 
);

/*	Function Name: XawMultiSourceChanged
 *	Description: Returns true if the source has changed since last saved.
 *	Arguments: w - the multiSource object.
 *	Returns: a Boolean (see description).
 */

extern Boolean XawMultiSourceChanged(
#if NeedFunctionPrototypes
    Widget		/* w */
#endif
);

_XFUNCPROTOEND

#endif /* _XawMultiSrc_h  - Don't add anything after this line. */

