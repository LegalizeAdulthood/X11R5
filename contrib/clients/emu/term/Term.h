#ifndef _TERM_H
#define _TERM_H

/* $Header: /usr3/Src/emu/term/RCS/Term.h,v 1.6 91/09/30 21:41:59 jkh Exp Locker: jkh $ */

/*
 * This file is part of the PCS emu system.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * Term widget public include file.
 *
 * Author: Jordan K. Hubbard
 * Date: June 6th, 1990.
 * Description: Everything a client of the term widget is going to need.
 *
 * Revision History:
 *
 * $Log:	Term.h,v $
 * Revision 1.6  91/09/30  21:41:59  jkh
 * Checkin prior to DECWRL merge.
 * 
 * Revision 1.5  90/11/20  17:45:14  jkh
 * Alpha.
 * 
 * Revision 1.4  90/11/13  14:57:43  jkh
 * Double file descriptor version.
 * 
 * Revision 1.1  90/07/26  01:54:44  jkh
 * Initial revision
 *
 */

/* Resources:
 * 
 * Name			Class		RepType		Default Value
 * ----			-----		-------		-------------
 * command		Command		String		XpNdefaultCommand
 * commandArgs		CommandArgs	Pointer		NULL
 * termType		TermType	String		XpNdefaultTermType
 * canvas		Canvas		Widget		NULL
 * readSize		ReadSize	Int		XpNdefaultReadSize
 * iopRequest		IopRequest	Callback	NULL
 * layoutProc		LayoutProc	Callback	NULL
 * childDeath		ChildDeath	Callback	NULL
 * inParser		InParser	Callback	NULL
 * outParser		OutParser	Callback	NULL
 * utmpInhibit		UtmpInhibit	Boolean		FALSE
 * loginShell		LoginShell	Boolean		FALSE
 * rops			Rops		String		NULL
 *
 */

/* resource types */
#define XpNcommand		"command"
#define XpNcommandArgs		"commandArgs"
#define XpNtermType		"termType"
#define XpNcanvas		"canvas"
#define XpNiopRequestProc	"iopRequestProc"
#define XpNlayoutProc		"layoutProc"
#define XpNprocessDeath		"processDeath"
#define XpNreadSize		"readSize"
#define XpNinParser		"inParser"
#define XpNoutParser		"outParser"
#define XpNutmpInhibit		"utmpInhibit"
#define XpNloginShell		"loginShell"
#define XpNrops			"rops"

/* class types */
#define XpCCommand		"Command"
#define XpCCommandArgs		"CommandArgs"
#define XpCTermType		"TermType"
#define XpCCanvas		"Canvas"
#define XpCIopRequestProc	"IopRequestProc"
#define XpCLayoutProc		"LayoutProc"
#define XpCProcessDeath		"ProcessDeath"
#define XpCReadSize		"ReadSize"
#define XpCInParser		"InParser"
#define XpCOutParser		"OutParser"
#define XpCUtmpInhibit		"UtmpInhibit"
#define XpCLoginShell		"LoginShell"
#define XpCRops			"Rops"

/* Defaults */
#define XpNdefaultCommand	"/bin/sh"
#define XpNdefaultReadSize	64
#define XpNdefaultTermType	"vt220"

/* declare specific TermWidget class and instance datatypes */

typedef struct _TermClassRec	*TermWidgetClass;
typedef struct _TermRec		*TermWidget;

/* declare the class constant */
extern WidgetClass termWidgetClass;

/* public procedures */
extern Boolean XpTermDoEmulation();
extern void XpTermDoRop();
extern void XpTermDispatchRequest();
extern void XpTermSetRegister();
extern void XpTermInsertText();
extern caddr_t XpTermGetRegister();
extern caddr_t XpTermComBlock();	/* real value is opaque */

#endif /* _TERM_H */
