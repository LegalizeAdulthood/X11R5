#ifndef lint
static char     sccsid[] = "@(#)fb_err.c 1.1 91/09/07 FJ";
#endif

/***********************************************************
Copyright 1989, 1990, 1991, by Fujitsu Limited, Sun Microsystems, Inc.
and the X Consortium.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Fujitsu, Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity
pertaining to distribution of the software without specific, written
prior permission.  

FUJITSU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJITSU BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
******************************************************************/

/* Error functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"



/* SET ERROR HANDLING MODE */
void
pserhm_( erhm)
Pint	*erhm;	/* error handling mode	*/
{
    Phg_args			cp_args;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_set_err_hand_mode)) {
        if ( *erhm < POFF || *erhm > PON) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else {
	    cp_args.data.idata = *erhm;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_ERR_HAND_MODE, &cp_args, NULL);
        }
    }
}


/* INQUIRE ERROR HANDLING MODE */
void
pqerhm_( errind, erhm)
Pint 	*errind;	/* OUT error indicator	*/
Pint	*erhm;		/* error handling mode	*/
{
    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else {
	*errind = 0;
	*erhm = (Pint)phg_cur_cph->erh->mode;
    }
}


/* ERROR LOGGING */
void
perlog_( errnr, fctid, errfil)
Pint	*errnr;		/* error number	*/
Pint	*fctid;		/* function identification	*/
Pint	*errfil;	/* error file	*/
{
    FILE	*erf;
    char	msg_buf[CB_MAX_ERR_MSG_LENGTH];
    static char unit_no[5];
    char	*fname;

    GETENV_UNIT_NO( *errfil, unit_no, fname);
    phg_cb_format_err_msg( PHG_FTN_BINDING, *errnr, *fctid, msg_buf);
    if ( fname && ( erf = fopen( fname, "a"))) {
	(void) fprintf( erf, "%s", msg_buf);
	(void) fclose( erf);
    } else {
	(void) fprintf( stderr, "%s", msg_buf);
    }
}

extern char *phg_path();


static	int  err_punt_msg();
static char  err_msg_format[] = "PHIGS error %d in %s: %s\n";

static FILE *errmsgfile, *funcmsgfile;

# define MSG_BUF_SIZE	1024

static int
err_lookup_string (fn, num, msgbuf, msgbufsize)
    FILE *fn;
    int num;
    char *msgbuf;
    int msgbufsize;
{
    int curnum = -9999;

    /* look for the message with num at it's start */
    do {
        if (fgets (msgbuf, msgbufsize, fn) == NULL) {
                return FALSE;
        }
        (void) sscanf (msgbuf, "%d", &curnum);
    } while (curnum != num);
    return TRUE;
}

static void
err_add_nl (buf, linelen)       /* add new lines to long message */
    char        *buf;
    int         linelen;
{
    char *p;

    if (strlen (buf) > linelen) {
        p = buf + linelen;
        while (--p != buf)
            /* replace blank with new line, check for more. */
            if (*p == ' ') {
                *p = '\n';
                err_add_nl (p+1, linelen);
                break;
        }
    }
}

/* Special case certain errors, and do our best with others,
 * so the message can be printed without run-time files.
 */
static
int     /* TRUE if message is known; FALSE (and numbers formatted) otherwise */
err_punt_msg( errnum, funcnum, funcname, buf )
    int         errnum;         /* IN: error number */
    int         funcnum;        /* IN: function id */
    char        *funcname;      /* IN: function name string */
    char        *buf;           /* OUT: error message */
{
    char msgbuf [MSG_BUF_SIZE];
    char funcbuf [MSG_BUF_SIZE];
    int         known = TRUE;
    /* Could build a list of (msg number, msg pointer) pairs & search in loop */
    /* Further compact these fallback messages using FIXED %s escapes */
    static      char ignoring_function[] = "Ignoring function";
    static      char cannot_open_phigs[] = "cannot open PHIGS";
    static      char msg_for_ERRN51[] =
                    "%s, %s, cannot locate PEX API file \"phigsmon\"";
    static      char msg_for_ERRN54[] =
                    "%s, cannot locate PHIGS API support file";
    static      char msg_for_ERRN55[] =
                    "%s, %s, cannot open font files";
    static      char function_0[] = "OPEN PHIGS";

    if (funcname == (char*)NULL) {
        /* failed to find function name, use old format */
        funcname = funcbuf;
        (void) sprintf (funcbuf, "function number %d", funcnum);
    }

    /* Could separate finding function number from finding msg number in list */
    if (known = (funcnum == 0)) {  /* only 1 known function name at this time */
        switch (errnum) {
        case ERRN51:
            (void) sprintf(msgbuf, err_msg_format, errnum, function_0,
                                msg_for_ERRN51);
            break;
        case ERRN54:
            (void) sprintf(msgbuf, err_msg_format, errnum, function_0,
                                msg_for_ERRN54);
            break;
        case ERRN55:
            (void) sprintf(msgbuf, err_msg_format, errnum, function_0,
                                msg_for_ERRN55);
            break;
        default:
            known = FALSE;
            /* Override input, possibly "function number 0" */
            funcname = function_0;
            break;
        }
    }
    if (known) {
        (void) sprintf (buf, msgbuf, ignoring_function, cannot_open_phigs);
        err_add_nl (buf, 80);
    } else
        (void) sprintf (buf, "PHIGS error %d in %s\n", errnum, funcname);
    return known;
}
