/* $Header: itimer.c,v 1.4 91/09/11 17:39:37 sinyaw Exp $ */
 
/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>

static void (*old_alrm)();
static void (*user_func)();
static caddr_t user_data;

start_clock(secs, func, data)
    int secs;
    void (*func) ();
    caddr_t data;
{
    struct itimerval value;
    void clock_timer();

    timerclear(&(value.it_value));
    timerclear(&(value.it_interval));

    value.it_value.tv_sec = secs;
    user_func = func;
    user_data = data;

    /* if someone is trapping SIGALRM, don't disturb */
    if ((old_alrm = signal(SIGALRM, SIG_IGN)) !=
	    SIG_DFL) {
	signal(SIGALRM, old_alrm);
	return;
    }

    old_alrm = signal(SIGALRM, clock_timer);
    setitimer(ITIMER_REAL, &value, NULL);
}


static void 
clock_timer()
{
    (void) signal(SIGALRM, old_alrm);
    if (user_func)
	(*user_func) (user_data);
}
