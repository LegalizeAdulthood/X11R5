/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */

/*

  This program is made by modifying Converters.c.

 */

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
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

#include <X11/Intrinsic.h>
#include "Xnw.h"

#define done(address, type) \
        { (*toVal).size = sizeof(type); (*toVal).addr = (caddr_t) address; }

void XnwCvtStringToFontStruct(args, num_args, fromVal, toVal)
    XrmValuePtr args;
    Cardinal    *num_args;
    XrmValuePtr fromVal;
    XrmValuePtr toVal;
{
    static XFontStruct  *f;
    Screen          *screen;
    char            lcfont[1000];
    static XrmQuark XrmQEdefaultfont;
    static int	    haveQuarks;
    XrmQuark        q;

    if (*num_args != 1)
	return;
/*
     XtErrorMsg("wrongParameters","cvtStringToFontStruct","XtToolkitError",
             "String to cursor conversion needs screen argument",
              (String *) NULL, (Cardinal *)NULL);
*/

    if(!haveQuarks) {
	XrmQEdefaultfont = XrmStringToQuark(XtEdefaultfont);
	haveQuarks = 1;
    }
    screen = *((Screen **) args[0].addr);
    LowerCase((char *) fromVal->addr, lcfont);
    q = XrmStringToQuark(lcfont);

    if (q != XrmQEdefaultfont) {
        f = XLoadQueryFont(DisplayOfScreen(screen), (char *)fromVal->addr);
        if (f != NULL) {
            done(&f, XFontStruct *);
            return;
        }
/*
        XtStringConversionWarning((char *) fromVal->addr, "XFontStruct");
*/
    }
    f = XLoadQueryFont(DisplayOfScreen(screen), "latin1");
    if (f != 0) {
	done(&f, XFontStruct *);
	return;
    }
    f = XLoadQueryFont(DisplayOfScreen(screen), "7x14");
    if (f != 0) {
	done(&f, XFontStruct *);
	return;
    }
    f = XLoadQueryFont(DisplayOfScreen(screen), "fixed");
    if (f != 0)	done(&f, XFontStruct *);
}


void XnwCvtStringToKanjiFontStruct(args, num_args, fromVal, toVal)
    XrmValuePtr args;
    Cardinal    *num_args;
    XrmValuePtr fromVal;
    XrmValuePtr toVal;
{
    static XFontStruct  *f;
    Screen          *screen;
    char            lcfont[1000];
    static XrmQuark XrmQEdefaultkanjifont;
    static int	    haveQuarks;
    XrmQuark        q;

    if (*num_args != 1)
	return;
/*
     XtErrorMsg("wrongParameters","cvtStringToFontStruct","XtToolkitError",
             "String to cursor conversion needs screen argument",
              (String *) NULL, (Cardinal *)NULL);
*/

    if(!haveQuarks) {
	XrmQEdefaultkanjifont = XrmStringToQuark(XtEdefaultkanjifont);
	haveQuarks = 1;
    }
    screen = *((Screen **) args[0].addr);
    LowerCase((char *) fromVal->addr, lcfont);
    q = XrmStringToQuark(lcfont);

    if (q != XrmQEdefaultkanjifont) {
        f = XLoadQueryFont(DisplayOfScreen(screen), (char *)fromVal->addr);
        if (f != NULL) {
            done(&f, XFontStruct *);
            return;
        }
/*
        XtStringConversionWarning((char *) fromVal->addr, "XFontStruct");
*/
    }
    f = XLoadQueryFont(DisplayOfScreen(screen), "kanji");
    if (f != 0) {
	done(&f, XFontStruct *);
	return;
    }
    f = XLoadQueryFont(DisplayOfScreen(screen), "k14");
    if (f != 0) {
	done(&f, XFontStruct *);
	return;
    }
    f = XLoadQueryFont(DisplayOfScreen(screen), "fixed");
    if (f != 0)	done(&f, XFontStruct *);
}

void XnwCvtStringToKanaFontStruct(args, num_args, fromVal, toVal)
    XrmValuePtr args;
    Cardinal    *num_args;
    XrmValuePtr fromVal;
    XrmValuePtr toVal;
{
    static XFontStruct  *f;
    Screen          *screen;
    char            lcfont[1000];
    static XrmQuark XrmQEdefaultkanafont;
    static int	    haveQuarks;
    XrmQuark        q;

    if (*num_args != 1)
	return;
/*
     XtErrorMsg("wrongParameters","cvtStringToFontStruct","XtToolkitError",
             "String to cursor conversion needs screen argument",
              (String *) NULL, (Cardinal *)NULL);
*/

    if(!haveQuarks) {
	XrmQEdefaultkanafont = XrmStringToQuark(XtEdefaultkanafont);
	haveQuarks = 1;
    }
    screen = *((Screen **) args[0].addr);
    LowerCase((char *) fromVal->addr, lcfont);
    q = XrmStringToQuark(lcfont);

    if (q != XrmQEdefaultkanafont) {
        f = XLoadQueryFont(DisplayOfScreen(screen), (char *)fromVal->addr);
        if (f != NULL) {
            done(&f, XFontStruct *);
            return;
        }
/*
        XtStringConversionWarning((char *) fromVal->addr, "XFontStruct");
*/
    }
    f = XLoadQueryFont(DisplayOfScreen(screen), "kana");
    if (f != 0) {
	done(&f, XFontStruct *);
     	return;
    }
    f = XLoadQueryFont(DisplayOfScreen(screen), "a14");
    if (f != 0) {
	done(&f, XFontStruct *);
     	return;
    }
    f = XLoadQueryFont(DisplayOfScreen(screen), "fixed");
    if (f != 0)	done(&f, XFontStruct *);
}

void XnwCvtEucCodeToJisCode(euc, num_euc, jis, num_jis)
unsigned char *euc, *jis;
int num_euc, *num_jis;
{
    register int i, j, mode;

    mode = Xnw_Ascii;
    for(i=0, j=0; i<num_euc; i++) {
        if(euc[i] == 0x8e) {
            if(mode != Xnw_Kana) {
                mode = Xnw_Kana;
                jis[j++] = '\033';
                jis[j++] = '(';
                jis[j++] = 'I';
            }
            i++;
            jis[j++] = 0x7f & euc[i];
        } else if(euc[i] & 0x80) {
            if(mode != Xnw_Kanji) {
                mode = Xnw_Kanji;
                jis[j++] = '\033';
                jis[j++] = '$';
                jis[j++] = 'B';
            }
            jis[j++] = 0x7f & euc[i];
            i++;
            jis[j++] = 0x7f & euc[i];
        } else {
            if(mode != Xnw_Ascii) {
                mode = Xnw_Ascii;
                jis[j++] = '\033';
                jis[j++] = '(';
                jis[j++] = 'B';
            }
            jis[j++] = euc[i];
        }
    }
    if(mode != Xnw_Ascii) {
        jis[j++] = '\033';
        jis[j++] = '(';
        jis[j++] = 'B';
    }
    *num_jis = j;
}

void XnwCvtJisCodeToEucCode(jis, num_jis, euc, num_euc)
unsigned char *jis, *euc;
int num_jis, *num_euc;
{
    register int i, j, mode;

    mode = Xnw_Ascii;
    for(i=0, j=0; i<num_jis; i++) {
	if(ISKANACODE(jis[i], jis[i+1], jis[i+2])) {
                mode = Xnw_Kana;
                i += 2;
		continue;
	} else if(ISKANJICODE(jis[i], jis[i+1], jis[i+2])) {
                mode = Xnw_Kanji;
                i += 2;
		continue;
	} else if(ISROMANCODE(jis[i], jis[i+1], jis[i+2])) {
                mode = Xnw_Ascii;
                i += 2;
		continue;
        }
/*
        if(jis[i] == '\033') {
            if(jis[i+1] == '(' && jis[i+2] == 'I') {
                mode = Xnw_Kana;
                i += 3;
            } else if(jis[i+1] == '$' && jis[i+2] == 'B') {
                mode = Xnw_Kanji;
                i += 3;
            } else if(jis[i+1] == '(' && jis[i+2] == 'B') {
                mode = Xnw_Ascii;
                i += 3;
            }
        }
*/
        if(i < num_jis) {
            switch(mode) {
                 case Xnw_Kanji:
			if(euc)
                        	euc[j] = 0x80 | jis[i];
			j++;
                        break;
                 case Xnw_Kana:
			if(euc) {
                        	euc[j] = 0x8e;
                        	euc[j+1] = 0x80 | jis[i];
			}
			j += 2;
                        break;
                 case Xnw_Ascii:
			if(euc)
                        	euc[j] = jis[i];
			j++;
                        break;
            }
        }
    }
    *num_euc = j;
}

int XnwMax3(x, y, z)
int x, y, z;
{
    if(x > y) {
	if(x > z)
	    return x;
	else
	    return z;
    } else {
	if(y > z)
	    return y;
	else
	    return z;
    }
}
