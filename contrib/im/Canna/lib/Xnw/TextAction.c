#if (!defined(lint) && !defined(SABER))
static char Xrcsid[] = "$XConsortium: TextAction.c,v 1.24 90/02/01 16:02:26 keith Exp $";
#endif /* lint && SABER */

/***********************************************************
Copyright 1989 by the Massachusetts Institute of Technology,
Cambridge, Massachusetts.

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
 * NOTE:  There are some ASCII Dependancies on '\n' and '\0' that I
 *        am not too thrilled with.  There is also the implicit assumption
 *        that the individual characters will fit inside a "char".
 *        It would be nice if we could generalize this a but more.  If
 *        anyone out there comes up with an implementation of this stuff
 *        that has no dependency on ASCII, please send the code back to us.
 *
 *						Chris D. Peterson     
 *						MIT X Consortium 
 */

#include <stdio.h>
#include <ctype.h>
#include <X11/Xatom.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <X11/Xmu/Atoms.h>
#include <X11/Xmu/Misc.h>

#include "TextP.h"
# ifdef NNES_RPQ
#include "Mikaku.h"
# endif
#include <X11/kanji.h>


#define SrcScan                XawTextSourceScan
#define FindDist               XawTextSinkFindDistance
#define FindPos                XawTextSinkFindPosition
int __clear_status = 0;
#define CHECK_INPUT_STATE(tw)   \
		if(__clear_status) { \
		    XnwClearHenkanStatus(((TextWidget)(tw))->text.ki.imode);\
	 	    __clear_status = 0; \
		} \
		if(((TextWidget)(tw))->text.ki.mode_mapping) { \
		    XtUnmapWidget(((TextWidget)(tw))->text.ki.draw_str); \
		} \
                if(((TextWidget)(tw))->text.ki.incomplete) { \
                    if(event->type == KeyPress || event->type == KeyRelease) { \
                        InsertChar((tw), event); \
                    } \
                    return; \
                }

/*
 * These are defined in TextPop.c
 */

void _XawTextInsertFileAction(), _XawTextInsertFile(), _XawTextSearch();
void _XawTextSearch(), _XawTextDoSearchAction(), _XawTextDoReplaceAction();
void _XawTextSetField(), _XawTextPopdownSearchAction();

/*
 * These are defined in Text.c
 */

char * _XawTextGetText();
void _XawTextBuildLineTable(), _XawTextAlterSelection(), _XawTextVScroll();
void _XawTextSetSelection(), _XawTextCheckResize(), _XawTextExecuteUpdate();
void _XawTextSetScrollBars(), _XawTextClearAndCenterDisplay();
Atom * _XawTextSelectionList();
void InsertChar();

static void
StartAction(ctx, event)
TextWidget ctx;
XEvent *event;
{
  _XawTextPrepareToUpdate(ctx);
  if (event != NULL) {
    switch (event->type) {
    case ButtonPress:
    case ButtonRelease:
      ctx->text.time = event->xbutton.time;
      ctx->text.ev_x = event->xbutton.x;
      ctx->text.ev_y = event->xbutton.y;
      break;
    case KeyPress:
    case KeyRelease:
      ctx->text.time = event->xkey.time;
      ctx->text.ev_x = event->xkey.x;
      ctx->text.ev_y = event->xkey.y;
      break;
    case MotionNotify:
      ctx->text.time = event->xmotion.time;
      ctx->text.ev_x = event->xmotion.x;
      ctx->text.ev_y = event->xmotion.y;
      break;
    case EnterNotify:
    case LeaveNotify:
      ctx->text.time = event->xcrossing.time;
      ctx->text.ev_x = event->xcrossing.x;
      ctx->text.ev_y = event->xcrossing.y;
    }
  }
}

static void
EndAction(ctx)
TextWidget ctx;
{
  _XawTextCheckResize(ctx);
  _XawTextExecuteUpdate(ctx);
  ctx->text.mult = 1;
}


#ifdef XAW_BC

/*
 * These functions are superceeded by insert-selection.
 */

static void
StuffFromBuffer(ctx, buffer)
TextWidget ctx;
int buffer;
{
  extern char *XFetchBuffer();
  XawTextBlock text;
  char *paste;
  int num;

  paste = XFetchBuffer(XtDisplay(ctx), &num, buffer);
  text.ptr = XtMalloc(MAXCUT);
  XnwCvtJisCodeToEucCode(paste, Min(strlen(paste), MAXCUT),
                                        text.ptr, &(text.length));
  text.firstPos = 0;
  if (_XawTextReplace(ctx, ctx->text.insertPos, ctx->text.insertPos, &text)) {
    XBell(XtDisplay(ctx), 0);
    return;
  }
  ctx->text.insertPos = SrcScan(ctx->text.source, ctx->text.insertPos,
				XawstPositions, XawsdRight, text.length, TRUE);
  XtFree(text.ptr);
  XtFree(paste);
}

static void 
UnKill(ctx, event)
TextWidget ctx;
XEvent *event;
{
   Widget w = (Widget)ctx;
   CHECK_INPUT_STATE(w);
  StartAction(ctx, event);
  StuffFromBuffer(ctx, 1);
  EndAction(ctx);
}

static void
Stuff(ctx, event)
TextWidget ctx;
XEvent *event;
{
   Widget w = (Widget)ctx;
   CHECK_INPUT_STATE(w);
  StartAction(ctx, event);
  StuffFromBuffer(ctx, 0);
  EndAction(ctx);
}

#endif	/* XAW_BC */

struct _SelectionList {
    String *params;
    Cardinal count;
    Time time;
};

static void GetSelection();

/* ARGSUSED */
static void 
_SelectionReceived(w, client_data, selection, type, value, length, format)
Widget w;
caddr_t client_data;
Atom *selection, *type;
caddr_t value;
unsigned long *length;
int *format;
{
  TextWidget ctx = (TextWidget)w;
  XawTextBlock text;
  register char *end;
  char *line = (char *)value;
  char lbuf[256+1];
  
  if (*type == 0 /*XT_CONVERT_FAIL*/ || *length == 0) {
    struct _SelectionList* list = (struct _SelectionList*)client_data;
    if (list != NULL) {
      GetSelection(w, list->time, list->params, list->count);
      XtFree(client_data);
    }
    return;
  }
  
    if (*type == XA_STRING) {
        char *p, *q;
        int n = *length;

        line = (n > 256) ? XtMalloc(n + 1) : lbuf;
        bcopy((char *)value, line, n);
        line[n] = '\0';
        p = (char *)value;
        q = line;
        while (n-- > 0) {
            if (!(*p & 0x80)) {
                *q++ = *p;
            }
            p++;
        }
        end = q;
    } else if (*type == XA_COMPOUND_TEXT(XtDisplay(w))) {
        unsigned char *ct = (unsigned char *)value;
        unsigned short *ws;
        int n;
        int _Xnw_convWStoEUC(), _Xnw_convWStoSJIS(), _Xnw_convWStoJIS();
        unsigned short wbuf[256 + 1];

        n = _Xnw_convCTtoJWS(ct, *length, NULL);
        if (n < 0) {    /* data broken */
            XtFree(client_data);
            XtFree(value);
            return;
        }
        ws = (n > 256) ? (unsigned short *)
			XtMalloc((n + 1) * sizeof(unsigned short)) : wbuf;
        (void)_Xnw_convCTtoJWS(ct, *length, ws);

        n = _Xnw_convWStoEUC(ws, NULL);
        line = (n > 256) ? XtMalloc(n + 1) : lbuf;
        (void)_Xnw_convWStoEUC(ws, line);
        end = line + n;
        if (ws != wbuf) XtFree(ws);
    } else {
        /* I don't know */
        XtFree(client_data);
        XtFree(value);
        return;
    }
  StartAction(ctx, NULL);
  text.ptr = (char*)line;
  text.firstPos = 0;
  text.length = end - line;
  text.format = FMT8BIT;
  if (_XawTextReplace(ctx, ctx->text.insertPos, ctx->text.insertPos, &text)) {
    XBell(XtDisplay(ctx), 0);
    EndAction(ctx);
    return;
  }
  ctx->text.insertPos = SrcScan(ctx->text.source, ctx->text.insertPos,
                                XawstPositions, XawsdRight, text.length, TRUE);

  EndAction(ctx);
  _XawTextSetScrollBars(ctx);
  XtFree(client_data);
  XtFree(value);
}

static void 
GetSelection(w, time, params, num_params)
Widget w;
Time time;
String *params;			/* selections in precedence order */
Cardinal num_params;
{
    Atom selection;
    int buffer;

    XmuInternStrings(XtDisplay(w), params, (Cardinal)1, &selection);
    switch (selection) {
      case XA_CUT_BUFFER0: buffer = 0; break;
      case XA_CUT_BUFFER1: buffer = 1; break;
      case XA_CUT_BUFFER2: buffer = 2; break;
      case XA_CUT_BUFFER3: buffer = 3; break;
      case XA_CUT_BUFFER4: buffer = 4; break;
      case XA_CUT_BUFFER5: buffer = 5; break;
      case XA_CUT_BUFFER6: buffer = 6; break;
      case XA_CUT_BUFFER7: buffer = 7; break;
      default:	       buffer = -1;
    }
    if (buffer >= 0) {
	int nbytes;
	unsigned long length;
	int fmt8 = 8;
	Atom type = XA_STRING;
	char *line = XFetchBuffer(XtDisplay(w), &nbytes, buffer);
	if (length = nbytes)
	    _SelectionReceived(w, NULL, &selection, &type, (caddr_t)line,
			       &length, &fmt8);
	else if (num_params > 1)
	    GetSelection(w, time, params+1, num_params-1);
    } else {
	struct _SelectionList* list;
	if (--num_params) {
	    list = XtNew(struct _SelectionList);
	    list->params = params + 1;
	    list->count = num_params;
	    list->time = time;
	} else list = NULL;
	XtGetSelectionValue(w, selection, XA_TEXT(XtDisplay(w)),
				_SelectionReceived, (caddr_t)list, time);
    }
}

static void 
InsertSelection(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;		/* precedence list of selections to try */
Cardinal *num_params;
{
   CHECK_INPUT_STATE(w);
  StartAction((TextWidget)w, event); /* Get Time. */
  GetSelection(w, ((TextWidget)w)->text.time, params, *num_params);
  EndAction((TextWidget)w);
}

/************************************************************
 *
 * Routines for Moving Around.
 *
 ************************************************************/

static void
Move(ctx, event, dir, type, include)
TextWidget ctx;
XEvent *event;
XawTextScanDirection dir;
XawTextScanType type;
Boolean include;
{
  StartAction(ctx, event);
  ctx->text.insertPos = SrcScan(ctx->text.source, ctx->text.insertPos,
				type, dir, ctx->text.mult, include);
  EndAction(ctx);
}

static void 
MoveForwardChar(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
   Move((TextWidget) w, event, XawsdRight, XawstCharacter, TRUE);
}

static void 
MoveBackwardChar(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  Move((TextWidget) w, event, XawsdLeft, XawstCharacter, TRUE);
}

static void 
MoveForwardWord(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  Move((TextWidget) w, event, XawsdRight, XawstWhiteSpace, FALSE);
}

static void 
MoveBackwardWord(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  Move((TextWidget) w, event, XawsdLeft, XawstWhiteSpace, FALSE);
}

static void MoveForwardParagraph(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  Move((TextWidget) w, event, XawsdRight, XawstParagraph, FALSE);
}

static void MoveBackwardParagraph(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  Move((TextWidget) w, event, XawsdLeft, XawstParagraph, FALSE);
}

static void 
MoveToLineEnd(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  Move((TextWidget) w, event, XawsdRight, XawstEOL, FALSE);
}

static void 
MoveToLineStart(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  Move((TextWidget) w, event, XawsdLeft, XawstEOL, FALSE);
}


static void
MoveLine(ctx, event, dir)
TextWidget ctx;
XEvent *event;
XawTextScanDirection dir;
{
  XawTextPosition new, next_line, junk;
  int from_left, garbage;

  StartAction(ctx, event);

  if (dir == XawsdLeft)
    ctx->text.mult++;

  new = SrcScan(ctx->text.source, ctx->text.insertPos,
		XawstEOL, XawsdLeft, 1, FALSE);

  FindDist(ctx->text.sink, new, ctx->text.margin.left, ctx->text.insertPos,
	   &from_left, &junk, &garbage);

  new = SrcScan(ctx->text.source, ctx->text.insertPos, XawstEOL, dir,
		ctx->text.mult, (dir == XawsdRight));

  next_line = SrcScan(ctx->text.source, new, XawstEOL, XawsdRight, 1, FALSE);

  FindPos(ctx->text.sink, new, ctx->text.margin.left, from_left, FALSE,
	  &(ctx->text.insertPos), &garbage, &garbage);
  
  if (ctx->text.insertPos > next_line)
    ctx->text.insertPos = next_line;

  EndAction(ctx);
}

static void 
MoveNextLine(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  MoveLine( (TextWidget) w, event, XawsdRight);
}

static void 
MovePreviousLine(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  MoveLine( (TextWidget) w, event, XawsdLeft);
}

static void
MoveBeginningOfFile(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  Move((TextWidget) w, event, XawsdLeft, XawstAll, TRUE);
}

static void 
MoveEndOfFile(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  Move((TextWidget) w, event, XawsdRight, XawstAll, TRUE);
}

static void 
Scroll(ctx, event, dir)
TextWidget ctx;
XEvent *event;
XawTextScanDirection dir;
{
  StartAction(ctx, event);

  if (dir == XawsdLeft)
    _XawTextVScroll(ctx, ctx->text.mult);
  else
    _XawTextVScroll(ctx, -ctx->text.mult);

  EndAction(ctx);
}

static void 
ScrollOneLineUp(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  Scroll( (TextWidget) w, event, XawsdLeft);
}

static void 
ScrollOneLineDown(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  Scroll( (TextWidget) w, event, XawsdRight);
}

static void 
MovePage(ctx, event, dir)
TextWidget ctx;
XEvent *event;
XawTextScanDirection dir;
{
  int scroll_val = Max(1, ctx->text.lt.lines - 2);

  if (dir == XawsdLeft)
    scroll_val = -scroll_val;

  StartAction(ctx, event);
  _XawTextVScroll(ctx, scroll_val);
  ctx->text.insertPos = ctx->text.lt.top;
  EndAction(ctx);
}

static void 
MoveNextPage(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  MovePage((TextWidget) w, event, XawsdRight);
}

static void 
MovePreviousPage(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  MovePage((TextWidget) w, event, XawsdLeft);
}

/************************************************************
 *
 * Delete Routines.
 *
 ************************************************************/

static void 
_DeleteOrKill(ctx, from, to, kill)
TextWidget ctx;
XawTextPosition from, to;
Boolean	kill;
{
  XawTextBlock text;
  char *ptr;
  char *cut;
  int num;
  
  if (kill && from < to) {
    ptr = _XawTextGetText(ctx, from, to);
    cut = XtMalloc(MAXCUT);
    XnwCvtEucCodeToJisCode(ptr, Min(strlen(ptr), MAXCUT), cut, &num);
    XStoreBuffer(XtDisplay(ctx), cut, num, 1);
    XtFree(cut);
    XtFree(ptr);
  }
  text.length = 0;
  text.firstPos = 0;
  if (_XawTextReplace(ctx, from, to, &text)) {
    XBell(XtDisplay(ctx), 50);
    return;
  }
  ctx->text.insertPos = from;
  ctx->text.showposition = TRUE; 
}

static void
DeleteOrKill(ctx, event, dir, type, include, kill)
TextWidget	   ctx;
XEvent *event;
XawTextScanDirection dir;
XawTextScanType type;
Boolean	   include, kill;
{
  XawTextPosition from, to;
  
  StartAction(ctx, event);
  to = SrcScan(ctx->text.source, ctx->text.insertPos,
	       type, dir, ctx->text.mult, include);
  
  if (dir == XawsdLeft) {
    from = to;
    to = ctx->text.insertPos;
  }
  else 
    from = ctx->text.insertPos;

  _DeleteOrKill(ctx, from, to, kill);
  _XawTextSetScrollBars(ctx);
  EndAction(ctx);
}

static void 
DeleteForwardChar(w, event)
Widget w;
XEvent *event;
{
  CHECK_INPUT_STATE(w);
  DeleteOrKill((TextWidget) w, event, XawsdRight, XawstCharacter, TRUE, FALSE);
}

static void
DeleteBackwardChar(w, event)
Widget w;
XEvent *event;
{
  CHECK_INPUT_STATE(w);
  DeleteOrKill((TextWidget) w, event, XawsdLeft, XawstCharacter, TRUE, FALSE);
}

static void 
DeleteForwardWord(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  DeleteOrKill((TextWidget) w, event,
	       XawsdRight, XawstWhiteSpace, FALSE, FALSE);
}

static void 
DeleteBackwardWord(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  DeleteOrKill((TextWidget) w, event,
	       XawsdLeft, XawstWhiteSpace, FALSE, FALSE);
}

static void 
KillForwardWord(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  DeleteOrKill((TextWidget) w, event, 
	       XawsdRight, XawstWhiteSpace, FALSE, TRUE);
}

static void 
KillBackwardWord(w, event)
TextWidget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  DeleteOrKill((TextWidget) w, event,
	       XawsdLeft, XawstWhiteSpace, FALSE, TRUE);
}

static void
KillToEndOfLine(w, event)
Widget w;
XEvent *event;
{
  TextWidget ctx = (TextWidget) w;
  XawTextPosition end_of_line;

   CHECK_INPUT_STATE(w);
  StartAction(ctx, event);
  end_of_line = SrcScan(ctx->text.source, ctx->text.insertPos, XawstEOL, 
			XawsdRight, ctx->text.mult, FALSE);
  if (end_of_line == ctx->text.insertPos)
    end_of_line = SrcScan(ctx->text.source, ctx->text.insertPos, XawstEOL, 
			  XawsdRight, ctx->text.mult, TRUE);

  _DeleteOrKill(ctx, ctx->text.insertPos, end_of_line, TRUE);
  EndAction(ctx);
  _XawTextSetScrollBars(ctx);
}

static void 
KillToEndOfParagraph(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  DeleteOrKill((TextWidget) w, event, XawsdRight, XawstParagraph, FALSE, TRUE);
}

void 
_XawTextZapSelection(ctx, event, kill)
TextWidget ctx;
XEvent *event;
Boolean kill;
{
   StartAction(ctx, event);
   _DeleteOrKill(ctx, ctx->text.s.left, ctx->text.s.right, kill);
   EndAction(ctx);
  _XawTextSetScrollBars(ctx);
}

static void 
KillCurrentSelection(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  _XawTextZapSelection( (TextWidget) w, event, TRUE);
}

static void 
DeleteCurrentSelection(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  _XawTextZapSelection( (TextWidget) w, event, FALSE);
}

/************************************************************
 *
 * Insertion Routines.
 *
 ************************************************************/

static int 
InsertNewLineAndBackupInternal(ctx)
TextWidget ctx;
{
  int count, error = XawEditDone;
  XawTextBlock text;
  char *buf, *ptr;

  ptr = buf = XtMalloc(sizeof(char) * ctx->text.mult);
  for (count = 0; count < ctx->text.mult; count++, ptr++)
    ptr[0] = '\n';

  text.length = ctx->text.mult;
  text.ptr = buf;
  text.firstPos = 0;
  text.format = FMT8BIT;

  if (_XawTextReplace(ctx, ctx->text.insertPos, ctx->text.insertPos, &text)) {
    XBell( XtDisplay(ctx), 50);
    error = XawEditError;
  }
  else 
    ctx->text.showposition = TRUE;

  XtFree(buf);
  return(error);
}

static void 
InsertNewLineAndBackup(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  StartAction( (TextWidget) w, event );
  (void) InsertNewLineAndBackupInternal( (TextWidget) w );
  EndAction( (TextWidget) w );
  _XawTextSetScrollBars( (TextWidget) w);
}

static int
LocalInsertNewLine(ctx, event)
TextWidget ctx;
XEvent *event;
{
  StartAction(ctx, event);
  if (InsertNewLineAndBackupInternal(ctx) == XawEditError)
    return(XawEditError);
  ctx->text.insertPos = SrcScan(ctx->text.source, ctx->text.insertPos,
                             XawstCharacter, XawsdRight, ctx->text.mult, TRUE);
  EndAction(ctx);
  _XawTextSetScrollBars(ctx);
  return(XawEditDone);
}

static void
InsertNewLine(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  (void) LocalInsertNewLine( (TextWidget) w, event);
}

static void 
InsertNewLineAndIndent(w, event)
Widget w;
XEvent *event;
{
  XawTextBlock text;
  XawTextPosition pos1, pos2;
  TextWidget ctx = (TextWidget) w;

   CHECK_INPUT_STATE(w);
  StartAction(ctx, event);
  pos1 = SrcScan(ctx->text.source, ctx->text.insertPos, 
		 XawstEOL, XawsdLeft, 1, FALSE);
  pos2 = SrcScan(ctx->text.source, pos1, XawstEOL, XawsdLeft, 1, TRUE);
  pos2 = SrcScan(ctx->text.source, pos2, XawstWhiteSpace, XawsdRight, 1, TRUE);
  text.ptr = _XawTextGetText(ctx, pos1, pos2);
  text.length = strlen(text.ptr);
  if (LocalInsertNewLine(ctx, event)) return;
  text.firstPos = 0;
  if (_XawTextReplace(ctx,ctx->text.insertPos, ctx->text.insertPos, &text)) {
    XBell(XtDisplay(ctx), 50);
    EndAction(ctx);
    return;
  }
  ctx->text.insertPos = SrcScan(ctx->text.source, ctx->text.insertPos,
				XawstPositions, XawsdRight, text.length, TRUE);
  XtFree(text.ptr);
  EndAction(ctx);
  _XawTextSetScrollBars(ctx);
}

/************************************************************
 *
 * Selection Routines.
 *
 *************************************************************/

static void 
SelectWord(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
  TextWidget ctx = (TextWidget) w;
  XawTextPosition l, r;

   CHECK_INPUT_STATE(w);
  StartAction(ctx, event);
  l = SrcScan(ctx->text.source, ctx->text.insertPos, 
	      XawstWhiteSpace, XawsdLeft, 1, FALSE);
  r = SrcScan(ctx->text.source, l, XawstWhiteSpace, XawsdRight, 1, FALSE);
  _XawTextSetSelection(ctx, l, r, params, *num_params);
  EndAction(ctx);
}

static void 
SelectAll(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
  TextWidget ctx = (TextWidget) w;

   CHECK_INPUT_STATE(w);
  StartAction(ctx, event);
  _XawTextSetSelection(ctx,zeroPosition,ctx->text.lastPos,params,*num_params);
  EndAction(ctx);
}

static void
ModifySelection(ctx, event, mode, action, params, num_params)
TextWidget ctx;
XEvent *event;
XawTextSelectionMode mode;
XawTextSelectionAction action;
String *params;		/* unused */
Cardinal *num_params;	/* unused */
{
  StartAction(ctx, event);
  _XawTextAlterSelection(ctx, mode, action, params, num_params);
  EndAction(ctx);
}
		
/* ARGSUSED */
static void 
SelectStart(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;		/* unused */
Cardinal *num_params;	/* unused */
{
   CHECK_INPUT_STATE(w);
  ModifySelection((TextWidget) w, event, 
		  XawsmTextSelect, XawactionStart, params, num_params);
}

/* ARGSUSED */
static void 
SelectAdjust(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;		/* unused */
Cardinal *num_params;	/* unused */
{
   CHECK_INPUT_STATE(w);
  ModifySelection((TextWidget) w, event, 
		  XawsmTextSelect, XawactionAdjust, params, num_params);
}

static void 
SelectEnd(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
   CHECK_INPUT_STATE(w);
  ModifySelection((TextWidget) w, event, 
		  XawsmTextSelect, XawactionEnd, params, num_params);
}

/* ARGSUSED */
static void 
ExtendStart(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;		/* unused */
Cardinal *num_params;	/* unused */
{
   CHECK_INPUT_STATE(w);
  ModifySelection((TextWidget) w, event, 
		  XawsmTextExtend, XawactionStart, params, num_params);
}

/* ARGSUSED */
static void 
ExtendAdjust(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;		/* unused */
Cardinal *num_params;	/* unused */
{
   CHECK_INPUT_STATE(w);
  ModifySelection((TextWidget) w, event, 
		  XawsmTextExtend, XawactionAdjust, params, num_params);
}

static void 
ExtendEnd(w, event, params, num_params)
TextWidget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
   CHECK_INPUT_STATE(w);
  ModifySelection((TextWidget) w, event, 
		  XawsmTextExtend, XawactionEnd, params, num_params);
}

static void
SelectSave(w, event, params, num_params)
TextWidget  w;
XEvent *event;
String *params;
Cardinal *num_params;
{
    Atom    selections[256];
    int     num_atoms;

   CHECK_INPUT_STATE(w);
    StartAction (w, event);
    num_atoms = *num_params;
    if (num_atoms > 256)
        num_atoms = 256;
    XmuInternStrings(XtDisplay((Widget)w), params, num_atoms, selections);
    _XawTextSaltAwaySelection (w, selections, num_atoms);
    EndAction (w);
}

/************************************************************
 *
 * Misc. Routines.
 *
 ************************************************************/

/* ARGSUSED */
static void 
RedrawDisplay(w, event)
Widget w;
XEvent *event;
{
   CHECK_INPUT_STATE(w);
  StartAction( (TextWidget) w, event);
  _XawTextClearAndCenterDisplay((TextWidget) w);
  EndAction( (TextWidget) w);
}

/*ARGSUSED*/
static void
TextFocusIn (w, event)
TextWidget w;
XEvent *event;
{
  TextWidget ctx = (TextWidget) w;

   CHECK_INPUT_STATE(w);
  ctx->text.hasfocus = TRUE; 
}

/*ARGSUSED*/
static void
TextFocusOut(w, event)
TextWidget w;
XEvent *event;
{
  TextWidget ctx = (TextWidget) w;

   CHECK_INPUT_STATE(w);
  ctx->text.hasfocus = FALSE;
}

static XComposeStatus compose_status = {NULL, 0};

/*	Function Name: AutoFill
 *	Description: Breaks the line at the previous word boundry when
 *                   called inside InsertChar.
 *	Arguments: ctx - The text widget.
 *	Returns: none
 */

static void
AutoFill(ctx)
TextWidget ctx;
{
  int width, height, x, line_num, max_width;
  XawTextPosition ret_pos;
  XawTextBlock text;

  if ( !((ctx->text.auto_fill) && (ctx->text.mult == 1)) )
    return;

  for ( line_num = 0; line_num < ctx->text.lt.lines ; line_num++)
    if ( ctx->text.lt.info[line_num].position >= ctx->text.insertPos )
      break;
  line_num--;			/* backup a line. */

  max_width = Max(0, ctx->core.width - HMargins(ctx));

  x = ctx->text.margin.left;
  XawTextSinkFindPosition( ctx->text.sink,ctx->text.lt.info[line_num].position,
			  x, max_width, TRUE, &ret_pos, &width, &height);
  
  if ( ret_pos >= ctx->text.insertPos )
    return;
  
  text.ptr = "\n";
  text.length = 1;
  text.firstPos = 0;
  text.format = FMT8BIT;

  _XawTextReplace(ctx, ret_pos, ret_pos, &text);
  ctx->text.insertPos =
      SrcScan(ctx->text.source, ctx->text.insertPos,
              XawstPositions, XawsdRight, text.length, TRUE);
}

XKanjiStatus kanji_status;

static MitsuiDebug(nbytes, strbuf, ks)
int nbytes;
char *strbuf;
XKanjiStatus *ks;
{
#ifndef NNES_RPQ
    char *ptr;

    ptr = XtMalloc(nbytes+1);
    strncpy(ptr, strbuf, nbytes);
    ptr[nbytes] = '\0';
#endif
    printf("ret_val=%d\n", nbytes);
#ifdef NNES_RPQ
    printf("strbuf=%x\n", strbuf[0]);
#else
    printf("strbuf=%s\n", ptr);
#endif
    printf("XKanjiStatus\n");
    printf("\techoStr=%s\n", ks->echoStr);
    printf("\tlength=%d\n", ks->length);
    printf("\trevPos=%d\n", ks->revPos);
    printf("\trevLen=%d\n", ks->revLen);
    printf("\tinfo=%d\n", ks->info);
    printf("\tmode=%s\n", ks->mode);
    printf("\tgline\n");
    printf("\t\tline=%s\n", ks->gline.line);
    printf("\t\tlength=%d\n", ks->gline.length);
    printf("\t\trevPos=%d\n", ks->gline.revPos);
    printf("\t\trevLen=%d\n", ks->gline.revLen);
}

static int
LineForPosition (ctx, position)
TextWidget ctx;
XawTextPosition position;
{
  int line;

  for (line = 0; line < ctx->text.lt.lines; line++)
    if (position < ctx->text.lt.info[line + 1].position)
      break;
  return(line);
}

static Boolean
LineAndXYForPosition (ctx, pos, line, x, y)
TextWidget ctx;
XawTextPosition pos;
int *line;
Position *x, *y;
{
  XawTextPosition linePos, endPos;
  Boolean visible;
  int realW, realH;

  *line = 0;
  *x = ctx->text.margin.left;
  *y = ctx->text.margin.top;
  if (visible = IsPositionVisible(ctx, pos)) {
    *line = LineForPosition(ctx, pos);
    *y = ctx->text.lt.info[*line].y;
    *x = ctx->text.margin.left;
    linePos = ctx->text.lt.info[*line].position;
    XawTextSinkFindDistance( ctx->text.sink, linePos,
                            *x, pos, &realW, &endPos, &realH);
    *x += realW;
  }
  return(visible);
}

void
InsertChar(w, event)
Widget w;
XEvent *event;
{
  TextWidget ctx = (TextWidget) w;
  char *ptr, strbuf[BUFSIZ];
  int     keycode, count, error;
  XawTextBlock text;
  int nbytes;
  int line;
  Position x, y;
  unsigned char *mptr = 0;

  if(__clear_status) {
    XnwClearHenkanStatus(ctx->text.ki.imode);
    __clear_status = 0;
  }

  if(ctx->text.ki.mode_mapping) {
    XtUnmapWidget(ctx->text.ki.draw_str);
  }
  
  event->xkey.window = XtWindow(w);
  nbytes = XLookupKanjiString(event, strbuf, BUFSIZ,
                        &keycode, &compose_status, &kanji_status);
/*
MitsuiDebug(nbytes, strbuf, &kanji_status);
*/

  if(nbytes == -1) {
    if(ctx->text.ki.mode_display) {
      kanji_status.info = KanjiGLineInfo;
      kanji_status.gline.line = (unsigned char *)jrKanjiError;
      kanji_status.gline.length = strlen(kanji_status.gline.line);
      kanji_status.gline.revPos = 0;
      kanji_status.gline.revLen = 0;
      XnwSetHenkanStatus(ctx->text.ki.imode, &kanji_status);
      __clear_status = 1;
    } else {
      Arg args[1];
      unsigned char mode_s[100];

      strcpy(mode_s, jrKanjiError);
      XtSetArg(args[0], XtNlabel, mode_s);
      XtSetValues(ctx->text.ki.draw_str, args, 1);
      LineAndXYForPosition (ctx, ctx->text.insertPos, &line, &x, &y);
      XtMoveWidget(ctx->text.ki.draw_str, x, y);
      XtMapWidget(ctx->text.ki.draw_str);
      ctx->text.ki.mode_mapping = True;
    }
    return;
  }

  StartAction(ctx, event);
  if(ctx->text.ki.beforeLength == 0)
    ctx->text.ki.beforeInsertPos = ctx->text.insertPos;

  if(nbytes > 0) {
    text.ptr = ptr = XtMalloc(sizeof(char) * nbytes * ctx->text.mult);
    for (count = 0 ; count < ctx->text.mult ; count++) {
      strncpy(ptr, strbuf, nbytes);
      ptr += nbytes;
    }

    text.length = nbytes * ctx->text.mult;
    text.firstPos = 0;
    text.format = FMT8BIT;

    error = _XawTextReplace(ctx, ctx->text.ki.beforeInsertPos,
        ctx->text.ki.beforeInsertPos + ctx->text.ki.beforeLength, &text);

    if (error == XawEditDone) {
      _XawTextNeedsUpdating(ctx, ctx->text.ki.beforeInsertPos,
		ctx->text.ki.beforeInsertPos + ctx->text.ki.beforeLength);
      ctx->text.ki.beforeInsertPos = ctx->text.insertPos =
                SrcScan(ctx->text.source, ctx->text.ki.beforeInsertPos,
                XawstPositions, XawsdRight, text.length, TRUE);
      ctx->text.ki.mark_start = ctx->text.ki.rev_start =
                ctx->text.ki.rev_end = ctx->text.ki.mark_end = 0;
      ctx->text.ki.beforeLength = 0;
      ctx->text.ki.incomplete = False;
      AutoFill(ctx);
      XawTextUnsetSelection((Widget)ctx);
    } else {
      XBell(XtDisplay(ctx), 50);
    }
    if(text.ptr)
      XtFree(text.ptr);
  }
  if((kanji_status.length > 0) ||
     ((kanji_status.length == 0) && (ctx->text.ki.beforeLength))) {
#ifdef NNES_RPQ
    if(ctx->text.ki.insert_mi_string) {
#endif
      text.length = kanji_status.length;
      if(kanji_status.length != 0) {
        text.ptr = XtMalloc(kanji_status.length + 1);
        strncpy(text.ptr, kanji_status.echoStr, kanji_status.length);
      }
      text.firstPos = 0;
      text.format = FMT8BIT;
      error = _XawTextReplace(ctx, ctx->text.ki.beforeInsertPos,
          ctx->text.ki.beforeInsertPos + ctx->text.ki.beforeLength, &text);
      if(error == XawEditDone) {
        _XawTextNeedsUpdating(ctx, ctx->text.ki.beforeInsertPos,
		ctx->text.ki.beforeInsertPos + ctx->text.ki.beforeLength);
        ctx->text.ki.mark_start = ctx->text.ki.beforeInsertPos;
        ctx->text.ki.rev_start = ctx->text.ki.mark_start + kanji_status.revPos;
        ctx->text.ki.rev_end = ctx->text.ki.rev_start + kanji_status.revLen;
        ctx->text.ki.mark_end = ctx->text.ki.mark_start + kanji_status.length;
        ctx->text.ki.beforeLength = kanji_status.length;
#ifndef NNES_RPQ
        if(kanji_status.length == 0)
            ctx->text.ki.incomplete = False;
        else
            ctx->text.ki.incomplete = True;
#endif
        ctx->text.insertPos =
                SrcScan(ctx->text.source, ctx->text.ki.beforeInsertPos,
                XawstPositions, XawsdRight, text.length, TRUE);
        XawTextUnsetSelection((Widget)ctx);
      } else {
        XBell(XtDisplay(ctx), 50);
      }
      if(kanji_status.length != 0)
        XtFree(text.ptr);
#ifdef NNES_RPQ
    } else {
      Arg margs[10];

      if(kanji_status.length != 0) {
        mptr = XtMalloc(kanji_status.length + 1);
        strncpy(mptr, kanji_status.echoStr, kanji_status.length);
      }
      mptr[kanji_status.length] = '\0';
      XtSetArg(margs[0], XtNlabel, mptr);
      XtSetArg(margs[1], XtNreverseStart, kanji_status.revPos);
      XtSetArg(margs[2], XtNreverseEnd,
			kanji_status.revPos + kanji_status.revLen);
      XtSetValues(ctx->text.ki.no_return, margs, 3);
      LineAndXYForPosition (ctx, ctx->text.insertPos, &line, &x, &y);
      XtMoveWidget(ctx->text.ki.no_return, x, y);
      if(!ctx->text.ki.no_return_mapping) {
      	XtMapWidget(ctx->text.ki.no_return);
      	ctx->text.ki.no_return_mapping = True;
      }
      if(mptr)
	XtFree(mptr);
    }
    if(kanji_status.length == 0)
      ctx->text.ki.incomplete = False;
    else
      ctx->text.ki.incomplete = True;
#endif
  }
/* 1991.5.29    R8.1 RPQ only */
  if((kanji_status.info & KanjiModeInfo) ||
     (kanji_status.info & KanjiGLineInfo) ||
     ((kanji_status.info & KanjiEmptyInfo) && (kanji_status.length == 0))) {
/* 1991.5.29	R7.2 RPQ only
  if((kanji_status.info & KanjiModeInfo) ||
     (kanji_status.info & KanjiGLineInfo)) {
*/
    ctx->text.ki.incomplete = True;
    if(ctx->text.ki.mode_display) {
      XnwSetHenkanStatus(ctx->text.ki.imode, &kanji_status);
    } else {
	if((kanji_status.info & KanjiModeInfo) &&
		strcmp(kanji_status.mode, ctx->text.ki.mode_str)) {
	    Arg targs[1];
	    unsigned char mode_s[50], *modep;
	    int nchar;

	    modep = ctx->text.ki.mode_str = kanji_status.mode;
	    for(nchar = 0; *modep != '\0'; modep++) {
		if(*modep != ' ')
		    mode_s[nchar++] = *modep;
	    }
	    mode_s[nchar++] = '\0';
	    if(strlen(mode_s)) {
   	    	XtSetArg(targs[0], XtNlabel, mode_s);
	    	XtSetValues(ctx->text.ki.draw_str, targs, 1);
#ifdef NNES_RPQ		/* 1991.5.29    R8.1 RPQ only */
		if(kanji_status.length == 0) {
	    	    LineAndXYForPosition (ctx, ctx->text.insertPos,
						&line, &x, &y);
		} else {
		    Position tx, ty;
		    Dimension tw;
		    Arg ttargs[5];
		
		    XtSetArg(ttargs[0], XtNx, &tx);
		    XtSetArg(ttargs[1], XtNy, &ty);
		    XtSetArg(ttargs[2], XtNwidth, &tw);

    		    XtGetValues(ctx->text.ki.no_return, ttargs, 3);
		    x = tx + tw;
		    y = ty;
		}
#else			
	    	LineAndXYForPosition (ctx, ctx->text.insertPos, &line, &x, &y);
#endif
	    	XtMoveWidget(ctx->text.ki.draw_str, x, y);
	    	XtMapWidget(ctx->text.ki.draw_str);
	    	ctx->text.ki.mode_mapping = True;
	    }
	}
    }
    if(kanji_status.length > 0)
	ctx->text.ki.incomplete = True;
    else if(((kanji_status.info & KanjiModeInfo) &&
                (strlen(kanji_status.mode) == 0)) ||
       (!(kanji_status.info & KanjiGLineInfo) &&
                (kanji_status.length == 0)) ||
       ((kanji_status.info & KanjiGLineInfo) &&
                (kanji_status.gline.length == 0))) 
        ctx->text.ki.incomplete = False;
  }
#ifdef NNES_RPQ
  if(!ctx->text.ki.incomplete &&
     ctx->text.ki.no_return &&
     ctx->text.ki.no_return_mapping) {
    XtUnmapWidget(ctx->text.ki.no_return);
    ctx->text.ki.no_return_mapping = False;
  }
#endif
  EndAction(ctx);
  _XawTextSetScrollBars(ctx);
}

Boolean
XnwIsLocalInput(w, event)
Widget w;
XEvent *event;
{
    TextWidget tw = (TextWidget)w;

    if(__clear_status) {
        XnwClearHenkanStatus(tw->text.ki.imode);
        __clear_status = 0;
    }
    if(tw->text.ki.mode_mapping) {
    	XtUnmapWidget(tw->text.ki.draw_str);
    }
    if(tw->text.ki.incomplete) {
	if(event->type == KeyPress || event->type == KeyRelease)
            InsertChar(w, event);
        return False;
    } else
	return True;
}


/*ARGSUSED*/
static void 
InsertString(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
  TextWidget ctx = (TextWidget) w;
  XawTextBlock text;
  int	   i;

   CHECK_INPUT_STATE(w);
  text.firstPos = 0;
  StartAction(ctx, event);
  for (i = *num_params; i; i--, params++) {
    unsigned char hexval;
    if ((*params)[0] == '0' && (*params)[1] == 'x' && (*params)[2] != '\0') {
      char c, *p;
      hexval = 0;
      for (p = *params+2; (c = *p); p++) {
	hexval *= 16;
	if (c >= '0' && c <= '9')
	  hexval += c - '0';
	else if (c >= 'a' && c <= 'f')
	  hexval += c - 'a' + 10;
	else if (c >= 'A' && c <= 'F')
	  hexval += c - 'A' + 10;
	else break;
      }
      if (c == '\0') {
	text.ptr = (char*)&hexval;
	text.length = 1;
      } else text.length = strlen(text.ptr = *params);
    } else text.length = strlen(text.ptr = *params);
    if (text.length == 0) continue;
    if (_XawTextReplace(ctx, ctx->text.insertPos, 
			ctx->text.insertPos, &text)) {
      XBell(XtDisplay(ctx), 50);
      EndAction(ctx);
      return;
    }
    ctx->text.insertPos =
      SrcScan(ctx->text.source, ctx->text.insertPos,
	      XawstPositions, XawsdRight, text.length, TRUE);
  }
  EndAction(ctx);
}

static void 
DisplayCaret(w, event, params, num_params)
Widget w;
XEvent *event;		/* CrossingNotify special-cased */
String *params;		/* Off, False, No, On, True, Yes, etc. */
Cardinal *num_params;	/* 0, 1 or 2 */
{
  TextWidget ctx = (TextWidget)w;
  Boolean display_caret = True;

   CHECK_INPUT_STATE(w);
  if (event->type == EnterNotify || event->type == LeaveNotify) {
    /* for Crossing events, the default case is to check the focus
     * field and only change the caret when focus==True.  A second
     * argument of "always" will cause the focus field to be ignored.
     */
    Boolean check_focus = True;
    if (*num_params == 2 && strcmp(params[1], "always") == 0)
      check_focus = False;
    if (check_focus && !event->xcrossing.focus) return;
  }

  if (*num_params > 0) {	/* default arg is "True" */
    XrmValue from, to;
    from.size = strlen(from.addr = params[0]);
    XtConvert(w, XtRString, &from, XtRBoolean, &to);
    if (to.addr != NULL) display_caret = *(Boolean*)to.addr;
    if (ctx->text.display_caret == display_caret) return;
  }
  StartAction(ctx, event);
  ctx->text.display_caret = display_caret;
  EndAction(ctx);
}

/*	Function Name: Multiply
 *	Description: Multiplies the current action by the number passed.
 *	Arguments: w - the text widget.
 *                 event - ** NOT USED **.
 *                 params, num_params - The parameter list, see below.
 *	Returns: none.
 *
 * Parameter list;
 *  
 * The parameter list may contain either a number or the string 'Reset'.
 * 
 * A number will multiply the current multiplication factor by that number.
 * Many of the text widget actions will will perform n actions, where n is
 * the multiplication factor.
 *
 * The string reset will reset the mutiplication factor to 1.
 * 
 */

/* ARGSUSED */
static void 
Multiply(w, event, params, num_params)
Widget w;
XEvent *event;
String * params;
Cardinal * num_params;
{
  TextWidget ctx = (TextWidget) w;
  int mult;

   CHECK_INPUT_STATE(w);
  if (*num_params != 1) {
    XtAppError(XtWidgetToApplicationContext(w), 
	       "The multiply action takes exactly one argument.");
    XBell(XtDisplay(w), 0);
    return;
  }

  if ( (params[0][0] == 'r') || (params[0][0] == 'R') ) {
    XBell(XtDisplay(w), 0);
    ctx->text.mult = 1;
    return;
  }

  if ( (mult = atoi(params[0])) == 0 ) {
    char buf[BUFSIZ];
    sprintf(buf, "%s %s", "Text Widget: The multiply action's argument",
	    "must be a number greater than zero, or 'Reset'.");
    XtAppError(XtWidgetToApplicationContext(w), buf);
    XBell(XtDisplay(w), 0);
    return;
  }

  ctx->text.mult *= mult;
}

/*	Function Name: StripOutOldCRs
 *	Description: strips out the old carrige returns.
 *	Arguments: ctx - the text widget.
 *                 from - starting point.
 *                 to - the ending point
 *	Returns: the new ending location (we may add some caracters).
 */

static XawTextPosition
StripOutOldCRs(ctx, from, to)
TextWidget ctx;
XawTextPosition from, to;
{
  XawTextPosition startPos, endPos, eop_begin, eop_end, temp;
  Widget src = ctx->text.source;
  XawTextBlock text;
  char *buf;

  text.ptr= "  ";
  text.firstPos = 0;
  text.format = FMT8BIT;
   
/*
 * Strip out CR's. 
 */

  eop_begin = eop_end = startPos = endPos = from;
  while (TRUE) {
    endPos=SrcScan(src, startPos, XawstEOL, XawsdRight, 1, FALSE);

    temp=SrcScan(src, endPos, XawstWhiteSpace, XawsdLeft, 1, FALSE);
    temp=SrcScan(src, temp, XawstWhiteSpace, XawsdRight, 1, FALSE);
    if (temp > startPos)
	endPos = temp;

    if (endPos >= to)
      break;

    if (endPos >= eop_begin) {
      startPos = eop_end;
      eop_begin = SrcScan(src, startPos, XawstParagraph, XawsdRight, 1, FALSE);
      eop_end = SrcScan(src, startPos, XawstParagraph, XawsdRight, 1, TRUE);
    }
    else {
      XawTextPosition periodPos, next_word;
      int i, len;

      periodPos= SrcScan(src, endPos, XawstCharacter, XawsdLeft, 1, TRUE);
      next_word = SrcScan(src, endPos, XawstWhiteSpace, XawsdRight, 1, FALSE);

      len = next_word - periodPos;

      text.length = 1;
      buf = _XawTextGetText(ctx, periodPos, next_word);
      if ( (periodPos < endPos) && (buf[0] == '.') )
	  text.length++;	/* Put in two spaces. */

      /*
       * Remove all extra spaces. 
       */

      for (i = 1 ; i < len; i++) 
	  if ( (((0xff&buf[i]) & 0x80) || !isspace(buf[i])) ||
	       ((periodPos + i) >= to) ) {
	      break;
	  }
      
      XtFree(buf);

      to -= (i - text.length - 1);
      startPos = SrcScan(src, periodPos, XawstPositions, XawsdRight, i, TRUE);
      _XawTextReplace(ctx, endPos, startPos, &text);
      startPos -= i - text.length;
    }
  }
  return(to);
}

/*	Function Name: InsertNewCRs
 *	Description: Inserts the new Carrige Returns.
 *	Arguments: ctx - the text widget.
 *                 from, to - the ends of the region.
 *	Returns: none
 */

static void
InsertNewCRs(ctx, from, to)
TextWidget ctx;
XawTextPosition from, to;
{
  XawTextPosition startPos, endPos, space, eol;
  XawTextBlock text;
  int i, width, height, len;
  char * buf;

  text.ptr = "\n";
  text.length = 1;
  text.firstPos = 0;
  text.format = FMT8BIT;

  startPos = from;
  while (TRUE) {
    XawTextSinkFindPosition( ctx->text.sink, startPos, 
			    (int) ctx->text.margin.left,
			    (int) (ctx->core.width - HMargins(ctx)), 
			    TRUE, &eol, &width, &height);
    if (eol >= to)
      break;

    eol = SrcScan(ctx->text.source, eol, XawstCharacter, XawsdLeft, 1, TRUE);
    space= SrcScan(ctx->text.source, eol, XawstWhiteSpace, XawsdRight, 1,TRUE);
    
    startPos = endPos = eol;
    if (eol == space) 
      return;

    len = (int) (space - eol);
    buf = _XawTextGetText(ctx, eol, space);
    for ( i = 0 ; i < len ; i++)
      if (((0xff&buf[i]) & 0x80) || !isspace(buf[i]))
	break;

    to -= (i - 1);
    endPos = SrcScan(ctx->text.source, endPos,
		     XawstPositions, XawsdRight, i, TRUE);
    XtFree(buf);
    
    _XawTextReplace(ctx, startPos, endPos, &text);
    startPos = SrcScan(ctx->text.source, startPos,
                       XawstCharacter, XawsdRight, 1, TRUE);
  }
}  
  
/*	Function Name: FormRegion
 *	Description: Forms up the region specified.
 *	Arguments: ctx - the text widget.
 *                 from, to - the ends of the region.
 *	Returns: none.
 */

static void
FormRegion(ctx, from, to)
TextWidget ctx;
XawTextPosition from, to;
{
  if (from >= to) return;

  to = StripOutOldCRs(ctx, from, to);
  InsertNewCRs(ctx, from, to);
  _XawTextBuildLineTable(ctx, ctx->text.lt.top, TRUE);
}

/*	Function Name: FromParagraph.
 *	Description: reforms up the current paragraph.
 *	Arguments: w - the text widget.
 *                 event - the X event.
 *                 params, num_params *** NOT USED ***.
 *	Returns: none
 */

/* ARGSUSED */
static void 
FormParagraph(w, event, params, num_params)
Widget w;
XEvent *event;
String * params;
Cardinal * num_params;
{
  TextWidget ctx = (TextWidget) w;
  XawTextPosition from, to;

   CHECK_INPUT_STATE(w);
  StartAction(ctx, event);

  from =  SrcScan(ctx->text.source, ctx->text.insertPos,
		  XawstParagraph, XawsdLeft, 1, FALSE);
  to  =  SrcScan(ctx->text.source, from,
		 XawstParagraph, XawsdRight, 1, FALSE);

  FormRegion(ctx, from, to);
  EndAction(ctx);
  _XawTextSetScrollBars(ctx);
}

/*	Function Name: TransposeCharacters
 *	Description: Swaps the character to the left of the mark with
 *                   the character to the right of the mark.
 *	Arguments: w - the text widget.
 *                 event - the event that cause this action.
 *                 params, num_params *** NOT USED ***.
 *	Returns: none.
 */
	     
/* ARGSUSED */
static void 
TransposeCharacters(w, event, params, num_params)
Widget w;
XEvent *event;
String * params;
Cardinal * num_params;
{
  TextWidget ctx = (TextWidget) w;
  XawTextPosition start, end;
  XawTextBlock text;
  unsigned char * buf, c;
  unsigned char tmpc;
  int i;

   CHECK_INPUT_STATE(w);
  StartAction(ctx, event);

/*
 * Get bounds. 
 */

  start = SrcScan(ctx->text.source, ctx->text.insertPos, XawstCharacter,
                  XawsdLeft, 1, TRUE);
  end = SrcScan(ctx->text.source, ctx->text.insertPos, XawstCharacter,
                XawsdRight, ctx->text.mult, TRUE);

  if ( (start == ctx->text.insertPos) || (end == ctx->text.insertPos) ) 
    XBell(XtDisplay(w), 0);	/* complain. */
  else {
    ctx->text.insertPos = end;

    /*
     * Retrieve text and swap the characters. 
     */
    
    buf = (unsigned char *) _XawTextGetText(ctx, start, end);
    text.length = strlen(buf);
    text.firstPos = 0;
    text.format = FMT8BIT;
    
    if((c = buf[0]) & 0x80) {
	tmpc = buf[1];
    	for (i = 2 ; i < text.length ; i++)
            buf[i - 2] = buf[i];
    	buf[i - 2] = c;
    	buf[i - 1] = tmpc;
    } else {
    	for (i = 1 ; i < text.length ; i++)
            buf[i - 1] = buf[i];
    	buf[i - 1] = c;
    }
    
    /* 
     * Store new text is source.
     */
    
    text.ptr = (char *) buf;
    _XawTextReplace (ctx, start, end, &text);
    
    XtFree(buf);
  }
  EndAction(ctx);
}

/*	Function Name: NoOp
 *	Description: This action performs no action, and allows
 *                   the user or application programmer to unbind a 
 *                   translation.
 *	Arguments: w - the text widget.
 *                 event - *** UNUSED ***.
 *                 parms and num_params - the action parameters.
 *	Returns: none.
 *
 * Note: If the parameter list contains the string "RingBell" then
 *       this action will ring the bell.
 */

static void
NoOp(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
    if (*num_params != 1)
	return;

    switch(params[0][0]) {
    case 'R':
    case 'r':
	XBell(XtDisplay(w), 0);
    default:			/* Fall Through */
	break;
    }
}
	
/* Action Table */

XtActionsRec textActionsTable[] = {
/* motion bindings */
  {"forward-character", 	MoveForwardChar},
  {"backward-character", 	MoveBackwardChar},
  {"forward-word", 		MoveForwardWord},
  {"backward-word", 		MoveBackwardWord},
  {"forward-paragraph", 	MoveForwardParagraph},
  {"backward-paragraph", 	MoveBackwardParagraph},
  {"beginning-of-line", 	MoveToLineStart},
  {"end-of-line", 		MoveToLineEnd},
  {"next-line", 		MoveNextLine},
  {"previous-line", 		MovePreviousLine},
  {"next-page", 		MoveNextPage},
  {"previous-page", 		MovePreviousPage},
  {"beginning-of-file", 	MoveBeginningOfFile},
  {"end-of-file", 		MoveEndOfFile},
  {"scroll-one-line-up", 	ScrollOneLineUp},
  {"scroll-one-line-down", 	ScrollOneLineDown},
/* delete bindings */
  {"delete-next-character", 	DeleteForwardChar},
  {"delete-previous-character", DeleteBackwardChar},
  {"delete-next-word", 		DeleteForwardWord},
  {"delete-previous-word", 	DeleteBackwardWord},
  {"delete-selection", 		DeleteCurrentSelection},
/* kill bindings */
  {"kill-word", 		KillForwardWord},
  {"backward-kill-word", 	KillBackwardWord},
  {"kill-selection", 		KillCurrentSelection},
  {"kill-to-end-of-line", 	KillToEndOfLine},
  {"kill-to-end-of-paragraph", 	KillToEndOfParagraph},
#ifdef XAW_BC
/* unkill bindings */
  {"unkill", 			UnKill},
  {"stuff", 			Stuff},
#endif /* XAW_BC */
/* new line stuff */
  {"newline-and-indent", 	InsertNewLineAndIndent},
  {"newline-and-backup", 	InsertNewLineAndBackup},
  {"newline", 			(XtActionProc)InsertNewLine},
/* Selection stuff */
  {"select-word", 		SelectWord},
  {"select-all", 		SelectAll},
  {"select-start", 		SelectStart},
  {"select-adjust", 		SelectAdjust},
  {"select-end", 		SelectEnd},
  {"select-save",               SelectSave},
  {"extend-start", 		ExtendStart},
  {"extend-adjust", 		ExtendAdjust},
  {"extend-end", 		ExtendEnd},
  {"insert-selection",		InsertSelection},
/* Miscellaneous */
  {"redraw-display", 		RedrawDisplay},
  {"insert-file", 		_XawTextInsertFile},
  {"search",		        _XawTextSearch},
  {"insert-char", 		InsertChar},
  {"insert-string",		InsertString},
  {"focus-in", 	 	        TextFocusIn},
  {"focus-out", 		TextFocusOut},
  {"display-caret",		DisplayCaret},
  {"multiply",		        Multiply},
  {"form-paragraph",            FormParagraph},
  {"transpose-characters",      TransposeCharacters},
  {"no-op",                     NoOp},
/* Action to bind special translations for text Dialogs. */
  {"InsertFileAction",          _XawTextInsertFileAction},
  {"DoSearchAction",            _XawTextDoSearchAction},
  {"DoReplaceAction",           _XawTextDoReplaceAction},
  {"SetField",                  _XawTextSetField},
  {"PopdownSearchAction",       _XawTextPopdownSearchAction},
};

Cardinal textActionsTableCount = XtNumber(textActionsTable);
