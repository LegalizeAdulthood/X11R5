/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *fakeclnt_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/snap2/guardian/cmd/RCS/fakeclnt.c,v 1.7 1991/09/12 17:18:49 bobg Exp $";

/*
   A nop snap client, just created to test authentication
*/

#include <stdio.h>
#include <andrewos.h> /* sys/types.h sys/time.h */
#include <netinet/in.h>
#include <gasp.h>
#include <snap.h>

extern int SNAP_debugmask;
main(argc, argv)
int argc;
char *argv[];
{
    char *client;
    int clientfd;
    SNAP_CPARMS connparms;
    int auth;
    int rc;

    connparms.maxtime = 30;
    connparms.timeout = 1;
    connparms.maxmsgsize = 1000;
    connparms.encryptlevel = SNAP_ENCRYPT;
    rc=GASP_ServerInit(argc, argv, &connparms, &client, &clientfd, &auth);
    if(rc!=0) {
	printf ("snapfakeclient:GASP_ServerInit failed %d\n",rc);
	exit(1);
    }
    {
	char *buffer;
	int msgtype;
	int cid;
	rc=SNAP_Accept(&buffer,&msgtype,&cid,50);
	if(rc<0) {
	    printf("snapfakeclient:accept failed %d",rc);
	    exit(1);
	}
    }
    exit(0);
}
