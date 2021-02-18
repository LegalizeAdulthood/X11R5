/*
 * $Id: xcvtkey.c,v 1.3 1991/09/16 21:37:33 ohm Exp $
 */
/*
 * Copyright 1989, 1990, 1991 by OMRON Corporation
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
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * OMRON, MIT AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL OMRON, MIT OR WNN CONSORTIUM BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author:	Seiji Kuwari	OMRON Corporation
 *				kuwa@omron.co.jp
 *				kuwa%omron.co.jp@uunet.uu.net
 */				

/*************************************************************************
		xcvtkey.c
*/

#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#ifdef  XJUTIL
#include "xjutil.h"
#include "sxheader.h"
#include "xext.h"
#else   /* XJUTIL */
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#endif  /* XJUTIL */


#define	ENTRY_CNT	256

typedef struct _ConvCode {
    int		tokey;			/* output keycode		*/
    KeySym	fromkey;		/* input KeySym		*/
} ConvCode;

ConvCode cvt_fun_tbl[ENTRY_CNT];
ConvCode cvt_meta_tbl[ENTRY_CNT];

int cvt_fun_tbl_cnt = 0; /* convert table count */
int cvt_meta_tbl_cnt = 0; /* convert table count */

char *comment_char_str = "#;";

int
comment_char(c)
register char	c;
{
    return((strchr(comment_char_str, c) != NULL) ? 1 : 0);
}

int
cvt_fun_setup(filename)
char *filename;
{
    int compar();
    register int k, cnt = 0;
    KeySym c;
    char ksname[32], code[32];
    char buf[BUFSIZ];
    FILE *fp;
    extern long strtol();

    cvt_fun_tbl_cnt = 0;
    if ((fp = fopen(filename, "r")) == NULL) {
	return(-1);
    }

    while(fgets(buf, BUFSIZ, fp)) {
	if (comment_char(*buf) || (k = sscanf(buf, "%s %s", ksname, code)) <= 0)
	    continue;
	if (k != 2) {
	} else if ((c = XStringToKeysym(ksname)) == 0) {
	    /*msg(0, "\t\"%s\" is not define Keysym.", ksname); */
	} else {
	    cvt_fun_tbl[cnt].fromkey = c;
	    cvt_fun_tbl[cnt].tokey = strtol(code, (char **)NULL, 0);
	    cnt++;
	}
    }
    fclose(fp);
    cvt_fun_tbl_cnt = cnt;
    return(0);
}

int
cvt_meta_setup(filename)
char *filename;
{
    register int k, cnt = 0;
    KeySym c;
    char ksname[32], code[32];
    char buf[BUFSIZ];
    FILE *fp;
    extern long strtol();

    cvt_meta_tbl_cnt = 0;
    if ((fp = fopen(filename, "r")) == NULL) {
		return(-1);
    }

    while(fgets(buf, BUFSIZ, fp)) {
	if (comment_char(*buf) || (k = sscanf(buf, "%s %s", ksname, code)) <= 0)
	    continue;
	if (k != 2) {
	} else if ((c = XStringToKeysym(ksname)) == 0) {
	    /*msg(0, "\t\"%s\" is not define Keysym.", ksname); */
	} else {
	    cvt_meta_tbl[cnt].fromkey = c;
	    cvt_meta_tbl[cnt].tokey = strtol(code, (char **)NULL, 0);
	    cnt++;
	}
    }
    fclose(fp);
    cvt_meta_tbl_cnt = cnt;
    return(0);
}


/* Function of convert */

int
cvtfun(keysym)
KeySym keysym;
{
    register int cnt;

    for(cnt = 0; cnt < cvt_fun_tbl_cnt; cnt++) {
	if (cvt_fun_tbl[cnt].fromkey == keysym)
	    return(cvt_fun_tbl[cnt].tokey);
    }
    return(-1);
}


int
cvtmeta(keysym)
KeySym keysym;
{
    register int cnt;

    for(cnt = 0; cnt < cvt_meta_tbl_cnt; cnt++) {
	if (cvt_meta_tbl[cnt].fromkey == keysym)
	    return(cvt_meta_tbl[cnt].tokey);
    }
    return(-1);
}
