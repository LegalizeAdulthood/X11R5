/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/type/RCS/err.c,v 1.2 1991/09/12 16:27:38 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/type/RCS/err.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
char *err_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/type/RCS/err.c,v 1.2 1991/09/12 16:27:38 bobg Exp $";
#endif

/* err.c

	signal error handling routines
*/
/*
 *    $Log: err.c,v $
 * Revision 1.2  1991/09/12  16:27:38  bobg
 * Update copyright notice and rcsid
 *
 * Revision 1.1  1989/08/22  15:28:53  wjh
 * Initial revision
 *
 */

#include "signal.h"
#include "setjmp.h"
#include "err.h"

jmp_buf err_env;

err_HandleSignal()
{
    err_MarkEnd();
    longjmp(err_env, 1);
}


_err_LookAhead()
{
    signal(SIGBUS, err_HandleSignal);
    signal(SIGSEGV, err_HandleSignal);
}

void
err_MarkEnd()
{
    signal(SIGBUS, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
}
