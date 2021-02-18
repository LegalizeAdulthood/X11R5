#ifndef _TERMCANVAS_H
#define _TERMCANVAS_H

/* $Header: /usr3/emu/canvas/RCS/TermCanvas.h,v 1.9 90/10/12 12:40:57 me Exp $ */

/*
 * This file is part of the PCS emu program.
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
 * TermCanvas public include file.
 *
 * Author: Michael Elbel
 * Date: March 20th, 1990.
 * Description: This file contains all the public includes for the
 *		TermCanvas widget.
 *
 * Revision History:
 *
 * $Log:	TermCanvas.h,v $
 * Revision 1.9  90/10/12  12:40:57  me
 * checkmark
 * 
 * Revision 1.8  90/08/31  19:51:31  me
 * Checkpoint vor dem Urlaub
 * 
 * Revision 1.7  90/08/09  10:20:05  me
 * added XpNnotifyFirstMap and XpCNotifyFirstMap
 * 
 * Revision 1.6  90/07/26  02:28:49  jkh
 * Added new copyright.
 * 
 * Revision 1.5  90/06/08  16:51:43  me
 * checkpoint
 * 
 * Revision 1.3  90/05/08  17:33:36  me
 * Added default Tab Width.
 * 
 * Revision 1.2  90/05/08  14:15:36  me
 * Jordan's infamous changes
 * 
 *
 */

/****************************************************************
 *
 * TermCanvas widget
 *
 ****************************************************************/

/* Resources:
 * 
 * Name		     Class		RepType		Default Value
 * ----		     -----		-------		-------------
 * lines             Lines              Cardinal        24 (read only)
 * columns           Columns            Cardinal        80 (read only)
 * cellWidth	     CellWidth		Cardinal	0 (read only)
 * cellHeigth	     CellHeight		Cardinal	0 (read only)
 * font	     	     Font		FontStruct	XtDefaultFont
 * boldFont	     BoldFont		FontStruct	XtDefaultFont
 * underlineWidth    UnderlineWidth	Cardinal	1
 * foreground	     Foreground		String		black
 * background	     Background		String		white
 * cursorFg	     CursorFg		String          black
 * cursorBg	     CursorBg		String          white
 * cursorHeight      CursorHeight       Cardinal        0
 * cursorWidth       CursorWidth        Cardinal        0
 * cursorBlinking    CursorBlinking     Boolean         True
 * blinkInterval     BlinkInterval      Cardinal        500 msec
 * blinkWOFocus      BlinkWOFocus       Boolean         False
 * textBlinkInterval TextBlinkInterval  Cardinal	500 msec
 * wrapAround	     WrapAround         Boolean         True
 * insertMode	     InsertMode		Boolean		False
 * bellVolume	     BellVolume		Cardinal	100 %
 * defTabWidth	     DefTabWidth	Cardinal	8
 * termType	     TermType		String		NULL
 * setSize	     SetSize		Pointer		NULL
 * output	     Output		Pointer		NULL
 * notifyFirstMap    NotifyFirstMap	Pointer		NULL
 * selectionInverse  SelectionInverse	Boolean		False
 * pointerShape	     PointerShape	Cursor		xterm
 * pointerColor	     PointerColor	String		black
 * reverseVideo      ReverseVideo	Boolean		False
 * saveLines	     SaveLines		Cardinal	64 or 'lines',
 * 							whichever is bigger
 * adjustScrollBar   AdjustScrollBar	Pointer		NULL
 * multiClickTime    MultiClickTime	Cardinal	300 msec
 */

/* resource types */
#define XpNlines		"lines"
#define XpNcolumns		"columns"
#define XpNcellWidth		"cellWidth"
#define XpNcellHeight		"cellHeight"
#define XpNboldFont		"boldFont"
#define XpNunderlineWidth	"underlineWidth"
#define XpNcursorFg		"cursorFg"
#define XpNcursorBg		"cursorBg"
#define XpNcursorHeight		"cursorHeight"
#define XpNcursorWidth		"cursorWidth"
#define XpNcursorBlinking	"cursorBlinking"
#define XpNblinkInterval	"blinkInterval"
#define XpNblinkWOFocus		"blinkWOFocus"
#define XpNtextBlinkInterval	"textBlinkInterval"
#define XpNwrapAround		"wrapAround"
#define XpNinsertMode		"insertMode"
#define XpNbellVolume		"bellVolume"
#define XpNdefTabWidth		"defTabWidth"
#define XpNdefaultTerm		"vt100c"
#define XpNtermType		"termType"
#define XpNsetSize		"setSize"
#define XpNoutput		"output"
#define XpNnotifyFirstMap	"notifyFirstMap"
#define XpNcomBlock		"comBlock"
#define XpNselectionInverse	"selectionInverse"
#define XpNpointerShape		"pointerShape"
#define XpNpointerColor		"pointerColor"
#define XpNsaveLines		"saveLines"
#define XpNadjustScrollBar	"adjustSBar"
#define XpNmultiClickTime	"multiClickTime"

/* class types */
#define XpCLines		"Lines"
#define XpCColumns		"Columns"
#define XpCCellWidth		"CellWidth"
#define XpCCellHeight		"CellHeight"
#define XpCBoldFont		"BoldFont"
#define XpCUnderlineWidth	"UnderlineWidth"
#define XpCCursorFg		"CursorFg"
#define XpCCursorBg		"CursorBg"
#define XpCCursorHeight		"CursorHeight"
#define XpCCursorWidth		"CursorWidth"
#define XpCCursorBlinking	"CursorBlinking"
#define XpCBlinkInterval	"BlinkInterval"
#define XpCBlinkWOFocus		"BlinkWOFocus"
#define XpCTextBlinkInterval	"TextBlinkInterval"
#define XpCWrapAround		"WrapAround"
#define XpCInsertMode		"InsertMode"
#define XpCBellVolume		"BellVolume"
#define XpCDefTabWidth		"DefTabWidth"
#define XpCTermType		"TermType"
#define XpCSetSize		"SetSize"
#define XpCOutput		"Output"
#define XpCNotifyFirstMap	"NotifyFirstMap"
#define XpCComBlock		"ComBlock"
#define XpCSelectionInverse	"SelectionInverse"
#define XpCPointerShape		"PointerShape"
#define XpCPointerColor		"PointerColor"
#define XpCSaveLines		"SaveLines"
#define XpCAdjustScrollBar	"AdjustSBar"
#define XpCMultiClickTime	"MultiClickTime"

/* declare specific TermCanvasWidget class and instance datatypes */

typedef struct _TermCanvasClassRec*	TermCanvasWidgetClass;
typedef struct _TermCanvasRec*		TermCanvasWidget;

/* declare the class constant */
extern WidgetClass termCanvasWidgetClass;

#endif /* _TermCanvas_h */
