/*
 * $Id: callback.c,v 1.1 1991/09/11 06:48:13 ohm Exp $
 */
/*
 * X11R5 Input Method Test Program
 * callback.c v 1.0   Tue Apr 23 13:33:07 JST 1991
 */

/*
 * Copyright 1991 by OMRON Corporation
 *
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include <stdio.h>

#include "exvalue.h"
#include "func.h"

void	geometry(ic, client_data, call_data)
XIC	ic;
XPointer client_data;
#ifdef SYSV
caddr_t	call_data;
#else
void	*call_data;
#endif
{
    prstatus("Calling function of \"GeometryCallback()\"...");
    cls(prdisp);
    trnpebuf(call_data, "Geometry");
    wlogcb(ic , client_data, call_data, "Geometry");
    prstatus("done.");
}

int	pe_start(ic, client_data, call_data)
XIC	ic;
XPointer client_data;
#ifdef SYSV
caddr_t	call_data;
#else
void	*call_data;
#endif
{
    int	bufsize;
    
    prstatus("Calling function of \"PreeditStartCallback()\"...");
    cls(prdisp);
    bufsize = trnpebuf(call_data, "Start");
    wlogcb(ic , client_data, call_data, "PreeditStart");
    prstatus("done.");
    return (bufsize);
}

void	pe_done(ic, client_data, call_data)
XIC	ic;
XPointer client_data;
#ifdef SYSV
caddr_t	call_data;
#else
void	*call_data;
#endif
{
    prstatus("Calling function of \"PreeditDoneCallback()\"...");
    cls(prdisp);
    trnpebuf(call_data, "Done");
    wlogcb(ic , client_data, call_data, "PreeditDone");
    prstatus("done.");
}

void	pe_draw(ic, client_data, call_data)
XIC	ic;
XPointer client_data;
XIMPreeditDrawCallbackStruct *call_data;
{
    prstatus("Calling function of \"PreeditDrawCallback()\"...");
    cls(prdisp);
    trnpebuf(call_data, "Draw");
    wlogcb(ic, client_data, call_data, "PreeditDraw");
    prstatus("done.");
}

void	pe_caret(ic, client_data, call_data)
XIC	ic;
XPointer client_data;
XIMPreeditCaretCallbackStruct *call_data;
{
    char	*direction;
    char	*style;

    prstatus("Calling function of \"PreeditCaretCallback()\"...");
    cls(prdisp);
    prprint("Valuables of \"PreeditCaretCallback()\"\n\n");
    chgcaretval(call_data, &direction, &style);
    prcaret(call_data->position, direction, style);
    wlogcb(ic, client_data, call_data, "PreeditCaret");
    prstatus("done.");
}

void	st_start(ic, client_data, call_data)
XIC	ic;
XPointer client_data;
#ifdef SYSV
caddr_t	call_data;
#else
void	*call_data;
#endif
{
    prstatus("Calling function of \"StatusStartCallback()\"...");
    cls(prdisp);
    wlogcb(ic, client_data, call_data, "StatusStart");
    prstatus("done.");
}

void	st_done(ic, client_data, call_data)
XIC	ic;
XPointer client_data;
#ifdef SYSV
caddr_t	call_data;
#else
void	*call_data;
#endif
{
    prstatus("Calling function of \"StatusDoneCallback()\"...");
    cls(prdisp);
    wlogcb(ic, client_data, call_data, "StatusDone");
    prstatus("done.");
}

void	st_draw(ic, client_data, call_data)
XIC	ic;
XPointer client_data;
XIMStatusDrawCallbackStruct *call_data;
{
    prstatus("Calling function of \"StatusDrawCallback()\"...");
    cls(prdisp);
    if (call_data->type == XIMTextType) {
	if (call_data->data.text.encoding_is_wchar == True) {
	    (XwcDrawImageString(dpy, window1.win, fontset[0], gc,
				WIN_ITV * 2 + NW_WIDTH,
				CALCY(W1_HEIGHT, 0),
			       call_data->data.text.string.wide_char,
			       call_data->data.text.length));
	} else {
	    (XmbDrawImageString(dpy, window1.win, fontset[0], gc,
				WIN_ITV * 2 + NW_WIDTH,
				CALCY(W1_HEIGHT, 0),
				call_data->data.text.string.multi_byte,
				call_data->data.text.length));
	}
    }
    wlogcb(ic, client_data, call_data, "StatusDraw");
    prstatus("done.");
}

void	wlogcb(ic, client_data, call_data, cat)
XIC	ic;
XPointer client_data;
#ifdef SYSV
caddr_t	call_data;
#else
void	*call_data;
#endif
char	*cat;
{
    XIMPreeditDrawCallbackStruct	*pedcd;
    XIMPreeditCaretCallbackStruct	*peccd;
    XIMStatusDrawCallbackStruct		*stdcd;

    char	*direction;
    char	*style;
    
    fprintf(icfp, "\nFunction \"%sCallback\" has just called.\n", cat);
    fprintf(icfp, "1. ic          : 0x%-18X\n", ic);
    fprintf(icfp, "2. client_data : 0x%-18X\n", client_data);
    if ((!strcmp(cat, "Geometry")) ||
	(!strcmp(cat, "PreeditStart")) || (!strcmp(cat, "PreeditDone")) ||
	(!strcmp(cat, "StatusStart")) || (!strcmp(cat, "StatusDone"))) {
	fprintf(icfp, "3. call_data     : 0x%-18X\n", call_data);
    } else if (!strcmp(cat, "PreeditDraw")) {
	pedcd = (XIMPreeditDrawCallbackStruct *)call_data;
	fprintf(icfp, "3. call_data\n");
	fprintf(icfp, "    1. caret         : %-20d\n", pedcd->caret);
	fprintf(icfp, "    2. chg_first     : %-20d\n", pedcd->chg_first);
	fprintf(icfp, "    3. chg_length    : %-20d\n", pedcd->chg_length);
	fprintf(icfp, "    4. text\n");
	wlogtext(pedcd->text);
    } else if (!strcmp(cat, "PreeditCaret")) {
	peccd = (XIMPreeditCaretCallbackStruct *)call_data;
	chgcaretval(peccd, &direction, &style);
	fprintf(icfp, "3. call_data\n");
	fprintf(icfp, "    1. position      : %-20d\n", peccd->position);
	fprintf(icfp, "    2. direction     : %-20s\n", direction);
	fprintf(icfp, "    3. style         : %-20s\n", style);
    } else if (!strcmp(cat, "StatusDraw")) {
	stdcd = (XIMStatusDrawCallbackStruct *)call_data;
	fprintf(icfp, "3. call_data\n");
	fprintf(icfp, "    1. type          : %-20s\n",
		((stdcd->type == XIMTextType) ? "XIMTextType" : "XIMBitmapType"));
	if (stdcd->type == XIMTextType) {
	    fprintf(icfp, "    2. text\n");
	    wlogtext(stdcd->data.text);
	} else {
	    fprintf(icfp, "    2. bitmap        : 0x%-18x\n", stdcd->data.bitmap);
	}
    }
}

void	wlogtext(text)
XIMText text;
{
    int		cnt;		/* CouNTer */
    XIMFeedback *feedback;
    
    fprintf(icfp, "        1. length        : %-20d\n", text.length);
    fprintf(icfp, "        2. feedback      : 0x%-18x\n", *text.feedback);
    fprintf(icfp, "        3. encording_is_wchar : %d (%s)\n",
	    text.encoding_is_wchar,
	    ((text.encoding_is_wchar == True) ? "True" : "False"));
    if (text.length == 0) {
	fprintf(icfp, "        4. character code [%s]   : None\n",
		((text.encoding_is_wchar ==True) ? "wc" : "mb"));
    } else {
	for (cnt = 0; cnt < text.length; cnt++) {
	    fprintf(icfp, "        4. character code [%s]   : 0x%-18X\n",
		    ((text.encoding_is_wchar == True) ? "wc" : "mb"),
		    ((text.encoding_is_wchar == True) ?
		     text.string.wide_char[cnt] : text.string.multi_byte[cnt]));
	}
    }
    fprintf(icfp, "\n");
}

void	chgcaretval(call_data, direction, style)
XIMPreeditCaretCallbackStruct *call_data;
char	**direction;
char	**style;
{
    switch (call_data->direction) {
      case XIMForwardChar:
	*direction = "XIMForwardChar";
	break;
      case XIMBackwardChar:
	*direction = "XIMBackwardChar";
	break;
      case XIMForwardWord:
	*direction = "XIMForwardWord";
	break;
      case XIMBackwardWord:
	*direction = "XIMBackwardWord";
	break;
      case XIMCaretUp:
	*direction = "XIMCaretUp";
	break;
      case XIMCaretDown:
	*direction = "XIMCaretDown";
	break;
      case XIMNextLine:
	*direction = "XIMNextLine";
	break;
      case XIMPreviousLine:
	*direction = "XIMPreviousLine";
	break;
      case XIMLineStart:
	*direction = "XIMLineStart";
	break;
      case XIMLineEnd:
	*direction = "XIMLineEend";
	break;
      case XIMAbsolutePosition:
	*direction = "XIMAbsolutePosition";
	break;
      case XIMDontChange:
	*direction = "XIMDontChange";
	break;
    }
    switch (call_data->style) {
      case XIMIsInvisible:
	*style = "XIMIsInvisible";
	break;
      case XIMIsPrimary:
	*style = "XIMIsPrimary";
	break;
      case XIMIsSecondary:
	*style = "XIMIsSecondary";
	break;
    }
}
	    
int	trnpebuf(call_data, cat)
#ifdef SYSV
caddr_t	call_data;
#else
void	*call_data;
#endif
char	*cat;
{
	
    static int		cnt;	/* CouNTer */
    static int		cntwk;	/* CouNTer for WorK buffer */
    static wchar_t	buf_pe[MAX_BUF]; /* BUFfer for PreEdit */
    static wchar_t	tmp_tx[MAX_BUF]; /* TeMPorary TeXt */
    static wchar_t	buf_wk[MAX_BUF]; /* BUFfer for WorK */
    XIMPreeditDrawCallbackStruct	*pedcd;
    				/* PreEditDrawCallbackData */
    
    if ((!strcmp(cat, "Start")) || (!strcmp(cat, "Done"))) {
	bzero(buf_pe, (sizeof(wchar_t)) * MAX_BUF);
	bzero(tmp_tx, (sizeof(wchar_t)) * MAX_BUF);
	bzero(buf_wk, (sizeof(wchar_t)) * MAX_BUF);
	cnt = 0;
	if (!strcmp(cat, "Start")) {
	    return (MAX_BUF);
	}
    } else if (!strcmp(cat, "Draw")) {
	pedcd = (XIMPreeditDrawCallbackStruct *)call_data;
	for (cnt = 0; buf_pe[cnt] != 0; cnt++)
	    ;
	cntwk = cnt - ((pedcd->chg_first) + (pedcd->chg_length));
/*	bzero(tmp_tx, (sizeof(wchar_t)) * MAX_BUF);
	bzero(buf_wk, (sizeof(wchar_t)) * MAX_BUF); */
	bcopy(&buf_pe[(pedcd->chg_first) + (pedcd->chg_length)],
	      &buf_wk, sizeof(wchar_t) * cntwk);
	bzero(&buf_pe[pedcd->chg_first],
	      sizeof(wchar_t) * (cnt - pedcd->chg_first));
	cnt = pedcd->chg_first;
	if (pedcd->text.length > 0) {
	    if (pedcd->text.encoding_is_wchar == False) {
		/* transaction of translation */
	    } else {
		bcopy(pedcd->text.string.wide_char, &tmp_tx[0],
		      sizeof(wchar_t) * (pedcd->text.length));
	    }
	    bcopy(&tmp_tx, &buf_pe[pedcd->chg_first],
		  sizeof(wchar_t) * (pedcd->text.length));
	    cnt += pedcd->text.length;
	}
	bcopy(&buf_wk, &buf_pe[cnt], sizeof(wchar_t) * cntwk);
	cnt += cntwk;
    }
    XClearArea(dpy, window1.win, 0, 0, 0, 80, True);
    XFlush(dpy);
    if (cnt > 0) {
	XwcDrawImageString(dpy, window1.win, fontset[0], gc,
		       0, 20, buf_pe, cnt);
    }
    XFlush(dpy);
}


