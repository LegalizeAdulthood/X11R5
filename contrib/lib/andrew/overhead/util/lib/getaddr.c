/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/getaddr.c,v 2.5 1991/09/12 17:25:50 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/getaddr.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

/* getaddr -- get our internet address */

#include <andrewos.h>		/* types.h */
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

#define NIFS		3

/* Return our internet address as a long in network byte order.  Returns zero if it can't find one. */
unsigned long getaddr ()
{
    int     s;
    int     i, len;
    struct ifconf   ifc;
    struct ifreq    ifs[NIFS];
    struct sockaddr_in *a;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
	return 0;
    ifc.ifc_len = sizeof(ifs);
    ifc.ifc_buf = (caddr_t) ifs;
    i = ioctl(s, SIOCGIFCONF, &ifc);
    close(s);
    if (i < 0)
	return 0;
    len = ifc.ifc_len / sizeof(struct ifreq);
    if (len > NIFS)
	len = NIFS;
    for (i = 0; i < len; ++i) {
	a = (struct sockaddr_in *) &ifs[i].ifr_addr;
	if (a->sin_addr.s_addr != 0 && strcmp(ifs[i].ifr_name, "lo0") != 0)
	    return a->sin_addr.s_addr;
    }
    return 0;
}

#ifdef TESTINGONLYTESTING
#include <stdio.h>
#include <arpa/inet.h>
main(argc,argv)
int argc;
char *argv[];
{
  struct in_addr a;

  a.s_addr = getaddr();
  printf("%ul == %s\n", a.s_addr, inet_ntoa(a));
}
#endif /* TESTINGONLYTESTING */
