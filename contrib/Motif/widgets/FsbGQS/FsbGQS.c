/************************************************************************
# Copyright 1991 OPEN SOFTWARE FOUNDATION, INC.
#
# Permission is hereby granted to use, copy, modify and freely distribute
# the software in this file and its documentation for any purpose without
# fee, provided that the above copyright notice appears in all copies and
# that both the copyright notice and this permission notice appear in
# supporting documentation.  Further, provided that the name of Open
# Software Foundation, Inc. ("OSF") not be used in advertising or
# publicity pertaining to distribution of the software without prior
# written permission from OSF.  OSF makes no representations about the
# suitability of this software for any purpose.  It is provided "as is"
# without express or implied warranty.
#
# Open Software Foundation is a trademark of The Open Software Foundation, Inc.
# OSF is a trademark of Open Software Foundation, Inc.
# OSF/Motif is a trademark of Open Software Foundation, Inc.
# Motif is a trademark of Open Software Foundation, Inc.
************************************************************************/

/*** This module defines a function that automatically get the
  selection text out of a FileSelectionBox and qualify it.
  By default, when you get the text value in the selection area,
  it is not qualified, and as a result, typing ~daniel/something
  will not result in /u1/daniel/something, so you can just pass
  the value to the file system as is.  
***/

#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/Text.h>


/* stolen from FileSBP.h */
typedef void (*XmQualProc)(
#ifdef __STDC__
#ifndef _NO_PROTO
XmFileSelectionBoxWidget fs,
XmFileSelectionBoxCallbackStruct * searchData,
XmFileSelectionBoxCallbackStruct * qualifiedSearchData
#endif
#endif
);

String
#ifdef _NO_PROTO
XmcFsbGetQualifyString(fsb) Widget fsb;
#else 
XmcFsbGetQualifyString(Widget fsb)
#endif
{
    Arg args[1] ;
    Cardinal n ;
    char * file, * mask ;
    XmQualProc qsdp ;
    XmFileSelectionBoxCallbackStruct searchData ;
    XmFileSelectionBoxCallbackStruct qualifiedSearchData ;

    file = XmTextGetString(XmFileSelectionBoxGetChild(fsb, XmDIALOG_TEXT)) ;

    searchData.value = NULL ;
    searchData.length = 0 ;
    searchData.mask = XmStringLtoRCreate(file, XmSTRING_DEFAULT_CHARSET);
    searchData.mask_length =  XmStringLength( searchData.mask) ;
    searchData.dir = NULL ;
    searchData.dir_length = 0 ;
    searchData.pattern = NULL ;
    searchData.pattern_length = 0 ;
    searchData.reason = XmCR_NONE ;

    n = 0 ;
    XtSetArg(args[n], XmNqualifySearchDataProc, &qsdp); n++;
    XtGetValues(fsb, args, n);
    qsdp((XmFileSelectionBoxWidget) fsb, &searchData, &qualifiedSearchData);

    XmStringGetLtoR(qualifiedSearchData.mask, 
		    XmSTRING_DEFAULT_CHARSET,
		    &mask);
    return mask ;
}

