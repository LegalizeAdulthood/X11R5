/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/fselect.c,v 2.6 1991/09/12 17:25:41 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/fselect.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

#include <stdio.h>
#include <andrewos.h>		/* sys/time.h */

static int NOFILES;

int fselect(nfds, rfiles, wfiles, xfiles, timeout)
int	nfds;
FILE	**rfiles, **wfiles, **xfiles;
struct timeval	*timeout;
{
    int rmask = 0, wmask = 0, xmask = 0, ret = 0;
    register int	i;

    if (NOFILES <= 0) {
	NOFILES = getdtablesize();
	if (NOFILES > 32)
	    NOFILES = 32;
    }
    for (i = nfds; --i >= 0;) {
	register int fd;
	if (rfiles && rfiles[i] != NULL && (fd = fileno(rfiles[i])) >= 0 && fd < NOFILES) 
	    if (rfiles[i]->_cnt > 0) 
		ret++;
	    else
		rmask |= 1<<fd;
	if (wfiles && wfiles[i] != NULL && (fd = fileno(wfiles[i])) >= 0 && fd < NOFILES)
	    wmask |= 1<<fd;
	if (xfiles && xfiles[i] != NULL && (fd = fileno(xfiles[i])) >= 0 && fd < NOFILES)
	    xmask |= 1<<fd;
    }
    if (ret==0) 
	ret = select(NOFILES, &rmask, &wmask, &xmask, timeout);
    else rmask = wmask = xmask = 0;
    for (i = nfds; --i >= 0;) {
	if (rfiles && rfiles[i] != NULL && rfiles[i]->_cnt <= 0 
	    && (rmask & (1<<fileno(rfiles[i]))) == 0)
	    rfiles[i] = NULL;
	if (wfiles && wfiles[i] != NULL && (wmask & (1<<fileno(wfiles[i]))) == 0)
	    wfiles[i] = NULL;
	if (xfiles && xfiles[i] != NULL && (xmask & (1<<fileno(xfiles[i]))) == 0)
	    xfiles[i] = NULL;
    }
    return (ret);
}
