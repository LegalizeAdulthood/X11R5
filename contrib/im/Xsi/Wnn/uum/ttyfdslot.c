/*
 * $Id: ttyfdslot.c,v 1.2 1991/09/16 21:34:04 ohm Exp $
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
#include "commonhd.h"

/*
	BSD42
 */
#if defined(BSD42) && (! defined(BSD43))

#include <stdio.h>
#define SLOTSIZ 32

char *ttyname();
char *rindex();

int ttyfdslot(fd)
int fd;
{
    char eachslot[SLOTSIZ];
    register char *fullnamp;
    register char *ttynamp;
    register FILE *ttysfp;
    register char *p;
    register int slotnum;

    if ((fullnamp = ttyname(fd)) == NULL) return NULL;
    if ((ttynamp = rindex(fullnamp, '/')) == NULL) {
        ttynamp = fullnamp;
    } else {
        ttynamp++;
    }
    if ((ttysfp = fopen("/etc/ttys", "r")) == NULL) return NULL;
    for (slotnum = 0; fgets(eachslot, SLOTSIZ, ttysfp); ) {
        p = eachslot + strlen(eachslot) - 1;
        if (*p == '\n') *p = '\0';
        slotnum++;
        if (strcmp(ttynamp, &eachslot[2]) == 0) {
            fclose(ttysfp);
            return slotnum;
        }
    }
    fclose(ttysfp);
    return NULL;
}
#endif	/*	BSD42	*/


/*
	BSD43
 */
#ifdef BSD43

#include <ttyent.h>
#include <stdio.h>

char *ttyname();
char *rindex();

int ttyfdslot(fd)
int fd;
{
    register char *fullnamp;
    register char *ttynamp;
    register int slotnum;
    register struct ttyent	*te ;
    if ((fullnamp = ttyname(fd)) == NULL) return NULL;
    if ((ttynamp = rindex(fullnamp, '/')) == NULL) {
        ttynamp = fullnamp;
    } else {
        ttynamp++;
    }
    for (slotnum = 1; (te = getttyent()) != NULL; slotnum++) {
	if ( strcmp(te->ty_name, ttynamp) == 0 ) {
		endttyent() ;
		return	slotnum ;
	}
    }
    endttyent() ;
    return NULL;
}

#endif	/*	BSD43	*/
