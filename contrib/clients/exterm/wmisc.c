/*
 * $Id: wmisc.c,v 1.2 1991/09/28 08:57:11 proj Exp $
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
#include <setjmp.h>
#include "ptyx.h"
#include "data.h"
#include <X11/Xlocale.h>
#include "Xi18nint.h"

static Char rembuf[4]={
        0,0,
        0,0
};

static Char backstr[4] = {
       0x8, 0x20,
       0x8, 0
};
static Char backstr2[7] = {
        0x8, 0x20, 0x8,
        0x8, 0x20, 0x8,0
};

int two_backspace = 0;
static _State state = XDEFAULT_STATE;    /* keep the state for converter */


/*
 * adjcol():
 *   calculate how many of wchars are put in the limitation columns maxcol.
 *   The result are stored in the "wn", and the real number of columns
 *   are returned.
*/
int adjcol(wn, ws, len, maxcol)
int	*wn;		/* Output: number of wchar's */
wchar	*ws;		/* Input: wc string */
int	len;		/* Input: length of wc string */
int	maxcol;		/* Input: limitation of columns */
{
	int i, t, n;

	for (i = 0, n = 0; i < len; i++) {
		t = XwcGetColumn(*ws++);
		if (n + t > maxcol) 
			break;
		else
			n += t;
	}
	*wn = i;
	return(n);
}

int NumberWPENDCHAR(ws, len)
wchar	*ws;
int	len;
{	
	register int cnt = 0;

	while (len-- > 0) 
		if (XwcGetColumn(*ws++) > 1)
			cnt++;
	return(cnt);
}


int wFilterWPEND(ws, wps, len)
wchar *ws, *wps;
int	len;
{	
	register int cnt = 0;
	register wchar wc;

	while (len-- > 0) 
		if ((wc = *wps++) == WPENDCHAR) 
			cnt++;
		else
			*ws++ = wc;
	return(cnt);
}

void init_state()
{
	state = XDEFAULT_STATE;    /* initialize the state for converter */
}

int wread(fd, bp, bsize)
int     fd;
wchar   *bp;
int     bsize;
{
        Char buf1[BUF_SIZE + 10]; /* 10 for remained chars */
        int      bcnt;
        int      leng;
        int      remleng;
	int	 scanned;
	int	ret;

	remleng = strlen((char *)rembuf);
	if (remleng > 0)
                strncpy((char *)buf1, (char *)rembuf,remleng);
        if ((bcnt = read(fd, buf1 + remleng, bsize)) <= 0) 
		return(bcnt);
	rembuf[0] = '\0';
        leng = bcnt + remleng;
	buf1[leng] = '\0';

/* Tackle the problem of backspace before two-column characters. It is ugly.*/
        if ((two_backspace==0)&&(strncmp((char *)buf1, (char *)backstr,3)==0)){
                        multi_backspace();
        }
        else if((two_backspace==1) && (strncmp((char *)buf1, (char *)backstr2, 6)==0)) {
                        two_backspace = 0;
                        buf1[3]='\0';
                        leng=3;
        }
        else two_backspace = 0;
	bcnt = BUF_SIZE;
        /* input is NULL, so no need to do the initialization.
         * It is done inside the function .
         */
        ret = _XConvertMBToWC(NULL, buf1, leng, bp, &bcnt, &scanned, &state); 

        if((leng > scanned) && (ret == BadTerminate)) {
                strncpy((char *)rembuf, (char *)buf1 + scanned, (leng-scanned));
		rembuf[leng-scanned] = '\0';
        }
	if(ret == BadEncoding){
		state = XDEFAULT_STATE;
		return(ret);
	}

        return(bcnt);
}

wcopy(wsrc, wtgt, len)
register wchar *wsrc;
register wchar *wtgt;
register int   len;
{
        while (len-- > 0)
                *wtgt++ = *wsrc++;
}


int wwrite(fd, bp, bsize)
int     fd;
wchar   *bp;
int     bsize;
{
        unsigned char buf[BUFSIZ];
        int     *scanned;
        int     bcnt = bsize;

        /* input is NULL, so no need to do the initialization.
         * It is done inside the function .
         */
/*
        if (_XConvertWCToMB(NULL, bp, bsize, buf, &bcnt, &scanned,NULL) < 0)
*/
        if (_XConvertWCToMB(NULL, bp, bsize, buf, &bcnt, &scanned) < 0)
				return(-1);
        return(write(fd, buf, bcnt));

}

wbzero(ws, len)
wchar *ws;
int   len;
{
        while (len-- > 0)
                *ws++ = 0;
}

multi_backspace()
{

    register TScreen *screen = &term->screen;
    wchar c;
    char back_char = 0x08;
    int    pty = screen->respond;


    c = screen->buf[2 * screen->cur_row][screen->cur_col-1];
    if (XwcGetColumn(c) > 1) {
         unparseputc(back_char, pty);
         two_backspace = 1;
    }

}


int XwcGetCharsetId(wc)
wchar	wc;
{
        wchar           woffset;
	XLocale		xlc;

	xlc = _XFallBackConvert();
	return(_Xmbctidtocsid(xlc, _XcwGetWoffset(wc, &woffset)));
}

int XwcGetColumn(wc)
wchar   wc;
{
	return(_XcwGetLength(wc));	
}


XFontStruct *GetFirstFont(fontset)
XFontSet fontset;
{
	char **dummy;
	XFontStruct **font_struct_list;

	XFontsOfFontSet(fontset, &font_struct_list, &dummy);
	return(font_struct_list[0]);
}

int GetAscent(fontset)
XFontSet fontset;
{
	XFontStruct *firstfnt;

	firstfnt = GetFirstFont(fontset);
	return(firstfnt->ascent);
/*
	return(_XsiAscentOfFontSet(fontset));
*/
}


/*
 * _getlatin1():
 *   get Latin-1 string from CT string, i.e., the returned
 *   string contains only ISO Latin 1(+TAB+NEWLINE) text.
 *   The passed CT should conforms with X standard document
 *   "Compound Text Encoding, Version 1.1", but except:
 *
 *      Section 10. Extensions
 * 
 *   The directionality is supposed only left-to-right.
 *   So the passed CT is terminated with null character '\0'
 *   same as the retured Latin 1 string.
 */

static int
esclen(seq)
char	*seq;
{
    register int		len;
    register char		c;

    /*
     * skip ESC.
     */
    len = 1; seq++;
    /*
     * Standard Character Set Encoding:
     * GL:
     *   01/11 02/08 {I} F		94 character set
     *   01/11 02/04 02/08 {I} F	94N character set
     *
     * GR:
     *   01/11 02/09 {I} F		94 character set
     *   01/11 02/13 {I} F		96 character set
     *   01/11 02/04 02/09 {I} F	94N character set
     *
     * The 1st intermediate character:
     *
     *   02/01 -- 02/03
     *
     * The others of intermediate characters:
     *
     *   02/00 -- 02/15
     *
     * The final character:
     * 
     *   04/00 -- 07/14
     */
    c = *seq++;
    len++;
    if (c == 0x28 || c == 0x29 || c== 0x2d ||   
	(c == 0x24 && (*seq  == 0x28 || *seq  == 0x29))) {
	if (c == 0x24) {
	    seq++;
	    len++;
	}
	c = *seq++;
	len++;
	/*
	 * skip {I}
	 */
	if (c == 0x21 || c == 0x22 || c == 0x23) {
	    while ((c = *seq++) != '\0' && c >= 0x20 && c <= 0x2f)
		len++;
	}
	/*
	 * get final char.
	 */
	if (c != '\0' && c >= 0x40 && c <= 0x7e) 
	    return(len + 1);
	return(0);
    }
    /*
     * Non-Standard Character Set Encodings
     *
     *   01/11 02/05 02/15 03/00 M L
     *   01/11 02/05 02/15 03/01 M L
     *   01/11 02/05 02/15 03/02 M L
     *   01/11 02/05 02/15 03/03 M L
     *   01/11 02/05 02/15 03/04 M L
     */
    if (c == 0x25 && *seq  == 0x2f &&
	*(seq + 1) >= 0x30 && *(seq + 1) <= 0x34) {
	int	M, L;
	seq += 2;
	M = *seq++;
	L = *seq++;
	len += 4;
	if ((M & 0x80) != 0 && (L & 0x80) != 0) {
	    len += (M - 128) * 128 + (L - 128);
	    return(len);
	}
	return 0;
    }
    /*
     * Section 10. Extensions.
     * Ignored!
     */
    return 0;
}
#define ISO8859_1_GL    "\033(B"
#define ISO8859_1_GR    "\033)A"


char *
getlatin1(ctstr)
char	*ctstr;
{
    register char	*ptr;
    register int	len = 0;
    char		*new;

    /* 
     * Use macro for a little speed.
     */
#define PickLatin1(expr)	{					\
    register int	latin = 1;					\
    register char	c;						\
    char		*s;						\
    int			n;						\
    /* 									\
     * default state is GL of Latin 1.					\
     */									\
    latin = 1;								\
    while ((c = *ctstr) != '\0') {					\
	switch (c) {							\
	  case '\t':	/* TAB		*/				\
	  case '\n':	/* NEWLINE	*/				\
	  case ' ':	/* SPACE	*/				\
	    ctstr++;							\
	    expr;							\
	    break;							\
	  case '\233':	/* CSI		*/				\
	    /* CSI 03/01 05/13 */					\
	    /* CSI 03/02 05/13 */					\
	    /* CSI 05/13 */						\
	    s = ++ctstr;						\
	    c = *ctstr++;						\
	    if (c == '1' || c == '2')					\
		c = *ctstr++;						\
	    if (c == ']') {						\
		ctstr++;						\
		break;							\
	    }								\
	    /*								\
	     * wrong CSI sequence!, skip it.				\
	     */								\
	    ctstr = s;							\
	    break;							\
	  case '\033':  /* ESC		*/				\
	    if (!strncmp(ctstr, ISO8859_1_GL, 3) ||			\
		!strncmp(ctstr, ISO8859_1_GR, 3)) {			\
		ctstr += 3;						\
		latin = 1;						\
		break;							\
	    }								\
	    if ((n = esclen(ctstr)) > 0) {				\
		ctstr += n;						\
		latin = 0;						\
		break;							\
	    }								\
	    /*								\
	     * wrong escape sequence! skip it				\
	     */								\
	    ctstr++;							\
	    break;							\
	  default:							\
	    if (latin)							\
		expr;							\
	    ctstr++;							\
	    break;							\
	}								\
    }									\
}

    new = ctstr;	/* save pointer of ctstr */
    /* 
     * get length of Latin 1.
     */
    PickLatin1(len++);
    ctstr = new;	/* restore pointer of ctstr */
    ptr = new = (char *)malloc((len + 1) * sizeof(unsigned char));
    if (ptr == NULL)
	return(NULL);
    /* 
     * get Latin 1 string.
     */
    if (len > 0)
	PickLatin1(*ptr++ = c);
    *ptr = '\0';
    return(new);

}
