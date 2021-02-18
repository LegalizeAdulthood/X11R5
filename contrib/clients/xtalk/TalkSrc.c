/* $Id: TalkSrc.c,v 1.0 1991/10/03 18:18:11 lindholm Exp $ */
/* TalkSrc.c - The TalkSrc gadget source code. */

/* This gadget is intended to be used as a source for the Text widget. */

/*
  Copyright 1991 by the University of British Columbia.
  
  All rights reserved.
  
  Permission to use, copy, modify, and distribute this software and its
  documentation for any purpose and without fee is hereby granted,
  provided that the above copyright notice appear in all copies and that
  both that copyright notice and this permission notice appear in
  supporting documentation, and that the name of the University of
  British Columbia (U.B.C.) not be used in advertising or publicity
  pertaining to distribution of the software without specific, written
  prior permission.
  
  U.B.C. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
  U.B.C. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
  ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
  SOFTWARE.
  
  */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "TalkSrcP.h"

#include <stdio.h>

/* TalkSrc Full Class Record */

static int replace();

static XtResource resources[] = {
#define offset(field) XtOffset(TalkSrcObject, talk_src.field)
    {XtNreplaceCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
	 offset(replace_callback), XtRCallback, NULL},
#undef offset
};

TalkSrcClassRec talkSrcClassRec = {
    /* core_class fields */
    {
	(WidgetClass) &asciiSrcClassRec,/* superclass */
	"TalkSrc",			/* class_name */
	sizeof(TalkSrcRec),		/* widget_size */
	NULL,				/* class_initialize */
	NULL,				/* class_part_initialize */
	FALSE,				/* class_inited */
	NULL,				/* initialize */
	NULL,				/* initialize_hook */
	NULL,				/* realize */
	NULL,				/* actions */
	0,				/* num_actions */
	resources,			/* resources */
	XtNumber(resources),		/* num_resources */
	NULLQUARK,			/* xrm_class */
	FALSE,				/* compress_motion */
	FALSE,				/* compress_exposure */
	FALSE,				/* compress_enterleave */
	FALSE,				/* visible_interest */
	NULL,				/* destroy */
	NULL,				/* resize */
	NULL,				/* expose */
	NULL,				/* set_values */
	NULL,				/* set_values_hook */
	XtInheritSetValuesAlmost,	/* set_values_almost */
	NULL,				/* get_values_hook */
	NULL,				/* accept_focus */
	XtVersion,			/* version */
	NULL,				/* callback_private */
	NULL,				/* tm_table */
	NULL,				/* query_geometry */
	NULL,				/* display_accelerator */
	NULL,				/* extension */
    },
    /* textSrc_class fields */
    {
	XtInheritRead,			/* Read */
	replace,			/* Replace */
	XtInheritScan,			/* Scan */
	XtInheritSearch,		/* Search */
	XtInheritSetSelection,		/* SetSelection */
	XtInheritConvertSelection,	/* ConvertSelection */
    },
    /* asciiSrc_class fields */
    {
	NULL,				/* a dummy for the compiler */
    },
    /* talkSrc_class fields */
    {
	0,				/* another dummy for the compiler */
    }
};

WidgetClass talkSrcObjectClass = (WidgetClass)&talkSrcClassRec;
    
    static int replace(w, startRep, endRep, insertText)
    TalkSrcObject w;
    XawTextPosition startRep, endRep;
    XawTextBlock *insertText;
{
    /* Replace the text between startRep and endRep with insertText.
       This routine calls our callback to see if it should be done. */
    
    TalkSrcCallbackStruct cbs;
    int rc;
    
    cbs.reason = 0;
    cbs.event = NULL;
    cbs.doit = TRUE;
    cbs.rc = XawEditDone;
    cbs.start = startRep;
    cbs.end = endRep;
    cbs.text = insertText;
    
    /* fprintf(stderr, "Before cbs.doit = %d\n", cbs.doit); */
    
    XtCallCallbacks((Widget)w, XtNreplaceCallback, &cbs);
    
    /* fprintf(stderr, " After cbs.doit = %d\n", cbs.doit); */
    
    if (!cbs.doit) {
	/* The callback said not to do the replace, so return the
	   callback's code. */
	return cbs.rc;
    }
    
    /* Call the AsciiSrc's replace routine to do the replacing. */
    rc = ((AsciiSrcObjectClass)(talkSrcClassRec.object_class.superclass))
	->text_src_class.Replace(w, cbs.start, cbs.end, cbs.text);
    
    if (rc != XawEditDone) fprintf(stderr, "replace rc: %d\n", rc);
    return rc;
}
