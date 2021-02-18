/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *utest_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/mail/testing/RCS/utest.c,v 2.6 1991/09/12 17:16:46 bobg Exp $";

#include <stdio.h>
#include <signal.h>
#include <system.h>
#include <andrewos.h>
#ifdef HAS_SYSEXITS
#include <sysexits.h>
#endif /* HAS_SYSEXITS */
#include <mail.h>

/* Test the UnixError, UnixSignal, and UnixSysExits routines. */
main()
{
    int Val;
    extern int EX_Nerr;

    for (Val = 0; Val < 115; Val++) {
	printf("UnixError(%d) is ``%s''", Val, UnixError(Val));
	if (vdown(Val)) printf(" (Vice down)");
	else if (tfail(Val)) printf(" (temp fail)");
	printf("\n");
    }
    printf("\n");
    for (Val = 0; Val < NSIG+1; Val++)
	printf("UnixSignal(%d) is ``%s''\n", Val, UnixSignal(Val));
    printf("\n");
    printf("UnixSysExits(%d) is ``%s''\n", EX_OK, UnixSysExits(EX_OK));
    for (Val = EX__BASE; Val <= (EX__BASE + EX_Nerr); Val++)
	printf("UnixSysExits(%d) is ``%s''\n", Val, UnixSysExits(Val));
}
