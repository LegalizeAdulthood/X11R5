/* -*-C-*-
********************************************************************************
*
* File:         xlisp.c
* RCS:          $Header: xlisp.c,v 1.5 91/03/24 22:25:04 mayer Exp $
* Description:  xlisp.c - a small implementation of lisp with object-oriented programming
* Author:       David Michael Betz
* Created:      
* Modified:     Fri Oct  4 04:02:02 1991 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r5 contrib tape release
*
* WINTERP Copyright 1989, 1990, 1991 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1987, 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
*
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: xlisp.c,v 1.5 91/03/24 22:25:04 mayer Exp $";

#include "xlisp.h"

/* define the banner line string */
#define BANNER	"XLISP version 2.1, Copyright (c) 1989, by David Betz"

/* global variables */
jmp_buf top_level;

/* external variables */
extern LVAL s_stdin,s_evalhook,s_applyhook;
extern LVAL s_1plus,s_2plus,s_3plus,s_1star,s_2star,s_3star,s_minus;
extern int xltrcindent;
extern int xldebug;
extern LVAL true;
extern char buf[];
extern FILE *tfp;

/* external routines */
extern FILE *osaopen();

/* main - the main routine */
main(argc,argv)
  int argc; char *argv[];
{
    char *transcript;
    CONTEXT cntxt;
    int verbose,i;
    LVAL expr;

    /* setup default argument values */
    transcript = NULL;
    verbose = FALSE;

    /* parse the argument list switches */
#ifndef LSC
    for (i = 1; i < argc; ++i)
	if (argv[i][0] == '-')
	    switch(argv[i][1]) {
	    case 't':
	    case 'T':
		transcript = &argv[i][2];
		break;
	    case 'v':
	    case 'V':
		verbose = TRUE;
		break;
	    }
#endif

    /* initialize and print the banner line */
    osinit(BANNER);

    /* setup initialization error handler */
    xlbegin(&cntxt,CF_TOPLEVEL|CF_CLEANUP|CF_BRKLEVEL,(LVAL)1);
    if (setjmp(cntxt.c_jmpbuf))
	xlfatal("fatal initialization error");
    if (setjmp(top_level))
	xlfatal("RESTORE not allowed during initialization");

    /* initialize xlisp */
    xlinit();
    xlend(&cntxt);

    /* reset the error handler */
    xlbegin(&cntxt,CF_TOPLEVEL|CF_CLEANUP|CF_BRKLEVEL,true);

    /* open the transcript file */
    if (transcript && (tfp = osaopen(transcript,"w")) == NULL) {
	sprintf(buf,"error: can't open transcript file: %s",transcript);
	stdputstr(buf);
    }

    /* load "init.lsp" */
    if (setjmp(cntxt.c_jmpbuf) == 0)
	xlload("init.lsp",TRUE,FALSE);

    /* load any files mentioned on the command line */
    if (setjmp(cntxt.c_jmpbuf) == 0)
	for (i = 1; i < argc; i++)
	    if (argv[i][0] != '-' && !xlload(argv[i],TRUE,verbose))
		xlerror("can't load file",cvstring(argv[i]));

    /* target for restore */
    if (setjmp(top_level))
	xlbegin(&cntxt,CF_TOPLEVEL|CF_CLEANUP|CF_BRKLEVEL,true);

    /* protect some pointers */
    xlsave1(expr);

    /* main command processing loop */
    for (;;) {

	/* setup the error return */
	if (setjmp(cntxt.c_jmpbuf)) {
	    setvalue(s_evalhook,NIL);
	    setvalue(s_applyhook,NIL);
	    xltrcindent = 0;
	    xldebug = 0;
	    xlflush();
	}

	/* print a prompt */
	stdputstr("> ");

	/* read an expression */
	if (!xlread(getvalue(s_stdin),&expr,FALSE))
	    break;

	/* save the input expression */
	xlrdsave(expr);

	/* evaluate the expression */
	expr = xleval(expr);

	/* save the result */
	xlevsave(expr);

	/* print it */
	stdprint(expr);
    }
    xlend(&cntxt);

    /* clean up */
    wrapup();
}

/* xlrdsave - save the last expression returned by the reader */
xlrdsave(expr)
  LVAL expr;
{
    setvalue(s_3plus,getvalue(s_2plus));
    setvalue(s_2plus,getvalue(s_1plus));
    setvalue(s_1plus,getvalue(s_minus));
    setvalue(s_minus,expr);
}

/* xlevsave - save the last expression returned by the evaluator */
xlevsave(expr)
  LVAL expr;
{
    setvalue(s_3star,getvalue(s_2star));
    setvalue(s_2star,getvalue(s_1star));
    setvalue(s_1star,expr);
}

/* xlfatal - print a fatal error message and exit */
xlfatal(msg)
  char *msg;
{
    oserror(msg);
    wrapup();
}

/* wrapup - clean up and exit to the operating system */
wrapup()
{
    if (tfp)
	osclose(tfp);
    osfinish();
    exit(0);
}

