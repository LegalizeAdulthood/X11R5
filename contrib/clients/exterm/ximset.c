/*
 * $Id: ximset.c,v 1.2 1991/09/28 08:56:30 proj Exp $
 */
/*
 * Copyright 1990, 1991 by OMRON Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of OMRON not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * OMRON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL OMRON BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 *	Author:	LU Qunyun	OMRON Corporation
 */				

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <setjmp.h>
#include "ptyx.h"
#include "data.h"
#include "Xi18nint.h"
#ifdef XML
#define OpenInputMethod XmlOpenIM
extern XIM XmlOpenIM();
#else
#define OpenInputMethod	XOpenIM
#endif /* XML */

#include <X11/Xlocale.h>


static int s_x = 0;
static int s_y = 0;


XIM xim;
extern char *getenv();
#ifdef XML
extern char language[6*MAX_XLC_NUM];
                   /* language is in the form of "xxxxx,xxxxx,..." */
XIC xic;
#endif /* XML */

#define MARGIN	 10

int OpenIM()
{
        XrmDatabase rdb;
        char *pp;
        char filename[256];
	TScreen *screen = &term->screen;
	extern int imioerror();

	if (xim) return(1);
    	pp = getenv("HOME");
    	sprintf(filename, "%s/.Xdefaults", pp);
    	rdb = XrmGetFileDatabase(filename);

    	if((xim = OpenInputMethod(screen->display,rdb, "xim", "Xim")) == NULL) {
   		fprintf(stderr, "BadIMOpen , input method is not created\n");
		return(-1);
	}
	_XipSetIOErrorHandler(imioerror);
	return(1);

}

static current_ic_mode;

XIC CreateIC()
{
        XVaNestedList preedit_nestedlist = NULL, status_nestedlist = NULL;
        XRectangle pre_area, sta_area, area_needed;
        XPoint point;
	TScreen *screen = &term->screen;
	XIC	ic;	
#ifdef XML
	char	*lanp, *lcp, curlang[16], *getlang;
#endif /* XML */
	XIMStyle  p;


#ifdef XML
        if(!strncmp(CurXlcInfo->lcname , "C", 1))
                strncpy(curlang, "de_DE",5);
        else {
                lcp = CurXlcInfo->lcname;
                for(lanp = curlang; *lcp != '@' /* && *lcp != '.' */ && *lcp != '\0';
                        lanp++, lcp++){
                        *lanp = *lcp;
                }
                *lanp = '\0';
        }
#endif /* XML */

	switch(term->misc.immode){
	case	OVERSPOT:	
		p = (XIMPreeditPosition|XIMStatusArea);
                s_x = point.x = CursorX(screen, screen->cur_col);
		s_y = point.y = CursorY(screen, screen->cur_row)
			      + GetAscent(screen->fset_norm);
		pre_area.x = screen->border + screen->scrollbar;
		pre_area.y = screen->border;
		pre_area.width = FullWidth(screen) - screen->border * 2
				- screen->scrollbar;
		pre_area.height = FullHeight(screen) - screen->border * 2;
	    	preedit_nestedlist = XVaCreateNestedList(0,
					XNLineSpace, 0,
					XNFontSet, screen->fset_norm,
					XNSpotLocation, &point,
					XNArea, &pre_area,
					NULL);
    		status_nestedlist = XVaCreateNestedList(0,
					XNLineSpace, 0,
					XNFontSet, screen->fset_norm,
					NULL);
		ic = XCreateIC(xim, XNInputStyle, p,
#ifdef XML
/*
			    XNUsingLanguage, language,
*/
			    XNCurrentLanguage, curlang,
#endif /* XML */
			    XNClientWindow, term->core.window,
			    XNFocusWindow, term->core.window,
			    XNPreeditAttributes, preedit_nestedlist,
			    XNStatusAttributes, status_nestedlist,
			    NULL);
		if (preedit_nestedlist) free((char *)preedit_nestedlist);
		if (status_nestedlist) free((char *)status_nestedlist);
		if (ic == NULL) {
		    return(NULL);
		}
#ifdef XML
		XGetICValues(ic, XNCurrentLanguage, &getlang, NULL);
		if (getlang == NULL || strcmp(getlang, curlang)) {
		    XDestroyIC(ic);
		    return(NULL);
		}
#endif /* XML */

		status_nestedlist = XVaCreateNestedList(0,
					XNAreaNeeded, &area_needed,
					NULL);
		XGetICValues(ic, XNStatusAttributes, status_nestedlist, NULL);
		if (status_nestedlist) free((char *)status_nestedlist);

		sta_area.x = screen->border + screen->scrollbar;
		sta_area.y = FullHeight(screen) - area_needed.height
				- screen->border;
		sta_area.width =  area_needed.width;
		sta_area.height = area_needed.height;
		status_nestedlist = XVaCreateNestedList(0, XNArea, &sta_area, 
					NULL);
		XSetICValues(ic, XNStatusAttributes, status_nestedlist, NULL);
		if (status_nestedlist) free((char *)status_nestedlist);

		break;

	case	OFFSPOT:
		p = (XIMPreeditArea|XIMStatusArea);
	    	preedit_nestedlist = XVaCreateNestedList(0,
					XNLineSpace, 0,
					XNFontSet, screen->fset_norm,
					NULL);
		status_nestedlist = XVaCreateNestedList(0,
                                        XNLineSpace, 0,
                                        XNFontSet, screen->fset_norm,
                                        NULL);
                ic = XCreateIC(xim, XNInputStyle, p,
#ifdef XML
/*
                            XNUsingLanguage, language,
*/
                            XNCurrentLanguage, curlang,
#endif /* XML */
                            XNClientWindow, term->core.window,
                            XNFocusWindow, term->core.window,
                            XNPreeditAttributes, preedit_nestedlist,
                            XNStatusAttributes, status_nestedlist,
                            NULL);

		if (preedit_nestedlist) free((char *)preedit_nestedlist);
		if (status_nestedlist) free((char *)status_nestedlist);
		if (ic == NULL) {
		    return(NULL);
		}
#ifdef XML
		XGetICValues(ic, XNCurrentLanguage, &getlang, NULL);
		if (getlang == NULL || strcmp(getlang, curlang)) {
		    XDestroyIC(ic);
		    return(NULL);
		}
#endif /* XML */

		status_nestedlist = XVaCreateNestedList(0,
					XNAreaNeeded, &area_needed,
					NULL);
		XGetICValues(ic, XNStatusAttributes, status_nestedlist, NULL);
		if (status_nestedlist) free((char *)status_nestedlist);

		pre_area.x = screen->border + area_needed.width 
				+ screen->scrollbar + MARGIN;
		pre_area.y = FullHeight(screen) - area_needed.height
				- screen->border;
		pre_area.width = FullWidth(screen) - pre_area.x;
		pre_area.height = area_needed.height;
		preedit_nestedlist = XVaCreateNestedList(0,
					XNArea, &pre_area,
					NULL);
	
		sta_area.x = screen->border + screen->scrollbar;
		sta_area.y = pre_area.y;
		sta_area.width =  area_needed.width;
		sta_area.height = area_needed.height;
		status_nestedlist = XVaCreateNestedList(0, XNArea, &sta_area, 
					NULL);
	
		XSetICValues(ic, XNPreeditAttributes, preedit_nestedlist,
				XNStatusAttributes, status_nestedlist,
				NULL);
		
		if (preedit_nestedlist) free((char *)preedit_nestedlist);
		if (status_nestedlist) free((char *)status_nestedlist);

		break;

	case	ROOTWINDOW:
		p = (XIMPreeditNothing|XIMStatusNothing);
		ic = XCreateIC(xim, XNInputStyle, p,
			    XNClientWindow, term->core.window,
			    XNFocusWindow, term->core.window,
			    NULL);
		if (ic == NULL) {
		    return(NULL);
		}
#ifdef XML
		XGetICValues(ic, XNCurrentLanguage, &getlang, NULL);
		if (getlang == NULL || strcmp(getlang, curlang)) {
		    XDestroyIC(ic);
		    return(NULL);
		}
#endif /* XML */
		break;
	}

	XSetICFocus(ic);
	current_ic_mode = term->misc.immode;
	return(ic);
}


Status ModifyICValue(ic)
XIC	ic;
{
        XVaNestedList preedit_nestedlist = NULL, status_nestedlist = NULL;
        XRectangle pre_area, sta_area, area_needed;
        XPoint point;
        register TScreen *screen = &term->screen;
#ifdef XML
	char	*lanp, *lcp, curlang[16], *getlang;
#endif /* XML */

	if (current_ic_mode != term->misc.immode) {
	    XDestroyIC(ic);
#ifdef XML
	    if ((xic = CreateIC()) == NULL) {
		return(False);
	    } else {
		return(True);
	    }
#else /* XML */
	    if ((CurXlcInfo->ic = CreateIC()) == NULL) {
		return(False);
	    } else {
		return(True);
	    }
#endif /* XML */
	}
#ifdef XML
        if(!strncmp(CurXlcInfo->lcname , "C", 1))
                strncpy(curlang, "de_DE",5);
        else {
                lcp = CurXlcInfo->lcname;
                for(lanp = curlang; *lcp != '@' /* && *lcp != '.' */ && *lcp != '\0';
                        lanp++, lcp++){
                        *lanp = *lcp;
                }
                *lanp = '\0';
        }
#endif /* XML */

	switch(term->misc.immode){
	case	OVERSPOT:
                s_x = point.x = CursorX(screen, screen->cur_col);
		s_y = point.y = CursorY(screen, screen->cur_row)
			      + GetAscent(screen->fset_norm);
		pre_area.x = screen->border + screen->scrollbar;
		pre_area.y = screen->border;
		pre_area.width = FullWidth(screen) - screen->border * 2
				- screen->scrollbar;
		pre_area.height = FullHeight(screen) - screen->border * 2;
		preedit_nestedlist = XVaCreateNestedList(0,
                                        XNSpotLocation, &point,
					XNArea, &pre_area,
                                        NULL);

                status_nestedlist = XVaCreateNestedList(0,
                                        XNAreaNeeded, &area_needed,
                                        NULL);
                XGetICValues(ic, XNStatusAttributes, status_nestedlist, NULL);
                if (status_nestedlist) free((char *)status_nestedlist);

		sta_area.x = screen->border + screen->scrollbar;
                sta_area.y = FullHeight(screen) - screen->border
                                - area_needed.height;
                sta_area.width =  area_needed.width;
                sta_area.height = area_needed.height;
                status_nestedlist = XVaCreateNestedList(0, XNArea, &sta_area,
                                        NULL);
                XSetICValues(ic,
#ifdef XML
			     XNCurrentLanguage, curlang,
#endif /* XML */
			     XNPreeditAttributes, preedit_nestedlist,
			     XNStatusAttributes, status_nestedlist,
			     NULL);
		if (preedit_nestedlist) free((char *)preedit_nestedlist);
                if (status_nestedlist) free((char *)status_nestedlist);
#ifdef XML
		XGetICValues(ic, XNCurrentLanguage, &getlang, NULL);
		if (getlang == NULL || strcmp(getlang, curlang)) {
		    return(False);
		}
#endif /* XML */
		break;

	case	OFFSPOT:
		status_nestedlist = XVaCreateNestedList(0,
					XNAreaNeeded, &area_needed,
					NULL);
		XGetICValues(ic, XNStatusAttributes, status_nestedlist, NULL);
		if (status_nestedlist) free((char *)status_nestedlist);

		pre_area.x = screen->border + area_needed.width 
				+ screen->scrollbar + MARGIN;
		pre_area.y = FullHeight(screen) - area_needed.height
				- screen->border;
		pre_area.width = FullWidth(screen) - pre_area.x;
		pre_area.height = area_needed.height;
		preedit_nestedlist = XVaCreateNestedList(0,
					XNArea, &pre_area,
					NULL);
	
		sta_area.x = screen->border + screen->scrollbar;
		sta_area.y = pre_area.y;
		sta_area.width =  area_needed.width;
		sta_area.height = area_needed.height;
		status_nestedlist = XVaCreateNestedList(0, XNArea, &sta_area, 
					NULL);
	
		XSetICValues(ic,
#ifdef XML
			     XNCurrentLanguage, curlang,
#endif /* XML */
			     XNPreeditAttributes, preedit_nestedlist,
			     XNStatusAttributes, status_nestedlist,
			     NULL);

		if (preedit_nestedlist) free((char *)preedit_nestedlist);
                if (status_nestedlist) free((char *)status_nestedlist);
#ifdef XML
		XGetICValues(ic, XNCurrentLanguage, &getlang, NULL);
		if (getlang == NULL || strcmp(getlang, curlang)) {
		    return(False);
		}
#endif /* XML */
		break;
	}

	XSetICFocus(ic);
	return(True);
}

void
IMCursorMove(screen)
register TScreen *screen;
{
	short x, y;
#ifndef	SPOT
        static XVaNestedList preedit_nestedlist = NULL;
        static XPoint point;
#endif	/* SPOT */

	if (term->misc.immode != OVERSPOT) return;
	x = CursorX(screen, screen->cur_col);
	y = CursorY(screen, screen->cur_row) + GetAscent(screen->fset_norm);
	if (s_x == x && s_y == y) return;
	s_x = x;
	s_y = y;
#ifdef	SPOT
#ifdef XML
	_XipChangeSpot(xic, x, y);
#else /* XML */
	_XipChangeSpot(CurXlcInfo->ic, x, y);
#endif /* XML */
#else	/* SPOT */
	point.x = x;
	point.y = y;
	if (preedit_nestedlist == NULL) {
	    preedit_nestedlist = XVaCreateNestedList(0, XNSpotLocation, &point,
						     NULL);
	}
#ifdef XML
	XSetICValues(xic, XNPreeditAttributes, preedit_nestedlist,
#else /* XML */
	XSetICValues(CurXlcInfo->ic, XNPreeditAttributes, preedit_nestedlist,
#endif /* XML */
		     NULL);
#endif	/* SPOT */
}


