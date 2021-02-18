/*
 * $Id: setutmp.c,v 1.2 1991/09/16 21:33:55 ohm Exp $
 */
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/*	Version 4.0
 */
#include <stdio.h>
#include <sys/types.h>
#include <utmp.h>
#include <pwd.h>
#include "commonhd.h"
#ifdef SYSVR2
#include <memory.h>
#endif /* SYSVR2 */
#ifdef BSD42
#include <fcntl.h>
#endif /* BSD42 */

#define public

char *ttyname();

#ifdef BSD42
static struct utmp saveut;
static int savslotnum = 0;
static char savttynm[8];

static int suf = 0;
static struct utmp nullut;

public int saveutmp()
{
    register int utmpFd;
    register char *p;

    if (suf > 0) return 0;
    suf = -1;

    bzero(&nullut, sizeof nullut);
    if ((p = ttyname(0)) == NULL) return -1;
    strncpy(savttynm, rindex(p, '/') + 1, 8);
    if (!(savslotnum = ttyslot())) return -1;
    if ((utmpFd = open("/etc/utmp", O_RDONLY, 0)) < 0) return -1;
    lseek(utmpFd, savslotnum * (sizeof saveut), 0);
    read(utmpFd, &saveut, sizeof saveut);
    close(utmpFd);
    strncpy(nullut.ut_line, saveut.ut_line, 8);
    strncpy(nullut.ut_host, saveut.ut_host, 16);
    nullut.ut_time = saveut.ut_time;
    suf = 1;
    return 0;
}

public int setutmp(ttyFd)
int ttyFd;
{
    int utmpFd;
    struct utmp ut;
    char *p;
    int i;

    if (suf <= 0) return -1;
    bzero(&ut, sizeof ut);
    if ((p = ttyname(ttyFd)) == NULL) return -1;
    strncpy(ut.ut_line, rindex(p, '/') + 1, 8);
    strncpy(ut.ut_name, getpwuid(getuid())->pw_name, 8);
    ut.ut_time = time(0);
    strncpy(ut.ut_host, savttynm, 8);
    if (!(i = ttyfdslot(ttyFd))) return -1;
    if ((utmpFd = open("/etc/utmp", O_RDWR, 0)) < 0) return -1;
    lseek(utmpFd, savslotnum * (sizeof nullut), 0);
    write(utmpFd, &nullut, sizeof nullut);
    lseek(utmpFd, i * (sizeof ut), 0);
    write(utmpFd, &ut, sizeof ut);
    close(utmpFd);
    return 0;
}
#endif /* BSD42 */

#ifdef SYSVR2
public int setutmp(ttyFd)
int ttyFd;
{
    struct utmp ut;
    char *p;
    struct passwd *getpwuid();

    memset(&ut, 0, sizeof ut);
    if ((p = ttyname(ttyFd)) == NULL) return -1;
    strncpy(ut.ut_line, strrchr(p, '/') + 1, 12);
    strncpy(ut.ut_user, getpwuid(getuid())->pw_name, 8);
    ut.ut_time = time(0);
    strncpy(ut.ut_id, &ut.ut_line[2], 4);
    ut.ut_id[0] = 't';
    ut.ut_pid = getpid();
    ut.ut_type = USER_PROCESS;
    setutent(); /* is it necessary? */
    getutid(&ut);
    pututline(&ut);
    endutent();
    return 0;
}
#endif /* SYSVR2 */

#ifdef BSD42
public int resetutmp(ttyFd)
int ttyFd;
{
    int utmpFd;
    struct utmp ut;
    char *p;
    int i;

    bzero(&ut, sizeof ut);
    if ((p = ttyname(ttyFd)) == NULL) return -1;
    strncpy(ut.ut_line, rindex(p, '/') + 1, 8);
    ut.ut_time = time(0);
    if (!(i = ttyfdslot(ttyFd))) return -1;
    if ((utmpFd = open("/etc/utmp", O_RDWR, 0)) < 0) return -1;
    lseek(utmpFd, savslotnum * (sizeof saveut), 0);
    write(utmpFd, &saveut, sizeof saveut);
    lseek(utmpFd, i * (sizeof ut), 0);
    write(utmpFd, &ut, sizeof ut);
    close(utmpFd);
    return 0;
}
#endif /* BSD42 */

#ifdef SYSVR2
public int resetutmp(ttyFd)
int ttyFd;
{
    struct utmp ut;
    char *p;
    struct passwd *getpwuid();

    memset(&ut, 0, sizeof ut);
    if ((p = ttyname(ttyFd)) == NULL) return -1;
    strncpy(ut.ut_line, strrchr(p, '/') + 1, 12);
    strncpy(ut.ut_user, getpwuid(getuid())->pw_name, 8);
    ut.ut_time = time(0);
    strncpy(ut.ut_id, &ut.ut_line[2], 4);
    ut.ut_id[0] = 't';
    ut.ut_pid = getpid();
    ut.ut_type = DEAD_PROCESS;	/* not sure */
    setutent(); /* is it necessary? */
    getutid(&ut);
    pututline(&ut);
    endutent();
    return 0;
}
#endif /* SYSVR2 */
