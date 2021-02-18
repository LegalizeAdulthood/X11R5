/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *snaperr_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/snap2/loadserv/RCS/snaperr.c,v 2.5 1991/09/12 17:19:52 bobg Exp $";

/*
  snaperr.c - return a next message to go with a snap error code
*/
#include <snap.h>

#define oSNAP_FIRST_ERR (SNAP_NOMORECONVS)

char *osnap_errlist[] = {
    "Too many connections active, run login and close some",
    "Not enough memory",
    "Did you spell the hostname correctly? OR server authenticate problem",
    "Internal error:Residual error code -- should not happen",
    "Can't talk to guardian server, perhaps it is dead?",
    "Internal error:Invalid op code to SNAP call",
    "Internal error:Invalid conversation id in SNAP call",
    "Internal error:SNAP buffer limit exceeded",
    "Internal error:SNAP was unable to create a socket",
    "Internal error:SNAP not initialized",
    "Host does not support loadserv or authentication failed",
    "Internal error:Unsolicited SNAP reply",
    "Server refused our connection",
    "Server timed out",
    "Internal SNAP protocol error",
    "SNAP version mismatch, do you have the most recent version?",
    "Internal error:SNAP select statement failed or was interrupted",
    "Things are weird; trying again might work",
    "Loadserv is dead",
};

int osnap_nerr = (sizeof osnap_errlist / sizeof osnap_errlist[0]);

#define SNAP_BADPARMS	    (1004*SNAP_FAIL)

char *oSNAP_ernum_to_str(aerr)
int aerr;
{
    int aindex;
    if(aerr==SNAP_ABORTED)return("Aborted by user ctl-break");
    aindex = oSNAP_FIRST_ERR - aerr;
    if((aindex >= 0)&&(aindex < osnap_nerr))
	return(osnap_errlist[aindex]);
    return 0;
}

char *ogasp_errlist[] = {
    "Impossible error 0",
    "Unknown service",
    "For some unknown reason, creation of server failed",
    "No more clients can be accepted by server",
    "Authentication failed",
    "No more servers can be created",
    "Unused",
    "Pipe sys call failed",
    "Fork sys call failed",
    "Bind sys call failed",
    "Client name is too long",
    "Guardian is not accepting remote requests",
    "Server is not accepting unauthenticated connections",
    "Setuid sys call failed",
    "A temporary failure due to a file server down occurred",
    "The format of the authentication string was bad",
    "RPC2/R initialization failed",
    "RPC2 authentication returned an impossible value",
    "User is not permitted to use this machine by user.permits",
    "Can't find user in /etc/passwd",
    "Authentication mismatch on existing connection",
    "No guest acct on cell",
    "Type of authentication not compiled",
    "Cell auth requested & can't find cell",
};

int ogasp_nerr = (sizeof ogasp_errlist / sizeof ogasp_errlist[0]);

char *oGASP_ernum_to_str(aerr)
int aerr;
{
    int aindex;
    aindex = aerr;
    if((aindex >= 0)&&(aindex < ogasp_nerr))
	return(ogasp_errlist[aindex]);
    return 0;
}
