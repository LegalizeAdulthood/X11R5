/*
 * $Id: readximrc.c,v 1.3 1991/09/16 21:37:15 ohm Exp $
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
/*	Version 4.0
 */
#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "config.h"
#include "ext.h"

/** Set parameter by reading ximrc file */
static int
read_xim_rcfile(filenm, expand_filenm)
char	*filenm;
int	(*expand_filenm)();
{
    char buf[BUFSIZ];
    char entrynm[64];
    char s[7][MAXPATHLEN + 1];
    int	n;
    int	warn_occur, illegal, no_entry;
    FILE *fp;
    extern int fclose();

    warn_occur = illegal = no_entry = 0;

    if ((fp = fopen(filenm, "r")) == NULL) {
	return(-1);
    }

    while(fgets(buf, BUFSIZ, fp)) {
	if (comment_char(*buf) ||
	    (n = sscanf(buf, "%s %s %s %s %s %s %s %s", entrynm,
			s[0],s[1],s[2],s[3],s[4],s[5],s[6])) < 1) {
		continue;
	}
	switch(*(entrynm + 3)) {
	case 'l':
	    if (!strcmp(entrynm, "preloadrkfile")) {
		if (n > 1) {
		    register ReadRkfileRec *rr, *p, *prev;
		    rr = (ReadRkfileList)Malloc(sizeof(ReadRkfileRec));
		    if (read_rkfile_list == NULL) {
			read_rkfile_list = rr;
		    } else {
			for (p = read_rkfile_list, prev = NULL; p != NULL;
			     prev = p, p = p->next);
			prev->next = rr;
		    }
		    rr->name = alloc_and_copy(s[0]);
		    rr->next = NULL;
		} else
		    illegal++;
	    } else
		no_entry++;
	    break;
	case 'u':
	    if (!strcmp(entrynm, "setuumrc")) {
		if (n > 2) {
		    register LangUumrcRec *lu, *p, *prev;
		    lu = (LangUumrcRec *)Malloc(sizeof(LangUumrcRec));
		    if (languumrc_list == NULL) {
			languumrc_list = lu;
		    } else {
			for (p = languumrc_list, prev = NULL; p != NULL;
			     prev = p, p = p->next);
			prev->next = lu;
		    }
		    lu->lang_name = alloc_and_copy(s[0]);
		    if ((*expand_filenm)(s[1]) != 0) {
			print_out1("I could not expand %s.", s[1]);
		    }
		    lu->uumrc_name = alloc_and_copy(s[1]);
		    lu->server_name = NULL;
		    lu->rkfile_name = NULL;
		    lu->rk_table = NULL;
#ifdef  USING_XJUTIL
		    lu->xjutil_act = 0;
		    lu->xjutil_id = 0;
		    lu->xjutil_pid = 0;
#endif  /* USING_XJUTIL */
		    lu->next = NULL;
		} else
		    illegal++;
	    } else
		no_entry++;
	    break;
	case 'b':
	    if (!strcmp(entrynm, "setbackspacechar")) {
		if (n > 1) {
		    rubout_code = s[0][0];
		} else
		    illegal++;
	    } else
		no_entry++;
	    break;
	default:
	    no_entry++;
	    break;
	}
	if (no_entry) {
	    warn_occur++;
	    print_out2(
	    "In ximrc file \"%s\", I found a unknown entry name \"%s\".",
		       filenm, entrynm);
	    no_entry = 0;
	}
	if (illegal) {
	    warn_occur++;
	    print_out2(
	    "In ximrc file \"%s\", I found a Illegal line %d.",
		       filenm, entrynm);
	    illegal = 0;
	}
    }
    fclose(fp);
    return(warn_occur ? 1 : 0);
}

int
read_ximrc()
{
    register char	*n;
    char	tmp_xim_rc_file[MAXPATHLEN + 1];
    extern char *getenv();

    if (ximrc_file) {
	n = ximrc_file;
    } else {
	n = getenv(XIM_RCENV);
    }
    if (n && *n) {
	if (read_xim_rcfile(n, expand_expr) >= 0) {
	    return(0);
	}
	print_out1("Can't open a specified ximrc \"%s\".", n);
    } else if ((n = getenv("HOME")) && *n) {
	strcat(strcpy(tmp_xim_rc_file, n), USR_XIMRC);
	ximrc_file = alloc_and_copy(tmp_xim_rc_file);
	if (read_xim_rcfile(ximrc_file, expand_expr) >= 0) {
	    return(0);
	}
	Free((char *)ximrc_file);
    }
    strcat(strcpy(tmp_xim_rc_file, LIBDIR), XIMRCFILE);
    ximrc_file = alloc_and_copy(tmp_xim_rc_file);
    if (read_xim_rcfile(ximrc_file, expand_expr) >= 0) {
	return(0);
    }
    print_out1("Can't open a default ximrc file \"%s\".", tmp_xim_rc_file);
    Free((char *)ximrc_file);
    ximrc_file = NULL;
    return(-1);
}
