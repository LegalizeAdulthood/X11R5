/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
 *	Common typedefs, and manifest constants for
 *	SNAP [Simple Network Application Protocol] Application Programs
 *
 *	$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/snap2/snap2/RCS/snap.h,v 2.3 1991/09/12 20:33:28 bobg Exp $
 */

typedef struct {
    int maxtime;		/* maximum elapsed time for a SNAP request */
    int timeout;		/* timeout interval in seconds */
    int maxmsgsize;		/* largest message in bytes (server only) */

/* Note that the maxmsgsize field (which is otherwise unused on the client
 * side of a connection) is used to pass back error codes for guardian-detected
 * failures during SNAP_BeginConv.  The following synonym is provided for
 * this purpose.
 */
#define guardian_rc maxmsgsize

    int encryptlevel;		/* encryption level */
#define SNAP_NOENCRYPT	0
#define SNAP_ENCRYPT	1

    } SNAP_CPARMS;

#define SNAP_integer	    long

#define SNAP_SUCCESS	    0
#define SNAP_FAIL	    (-1)
#define SNAP_NOMORECONVS    (1002*SNAP_FAIL)
#define SNAP_NOMEMORY	    (1003*SNAP_FAIL)
#define SNAP_BADPARMS	    (1004*SNAP_FAIL)
#define SNAP_NOMORERETRIES  (1005*SNAP_FAIL)
#define SNAP_XMITFAIL	    (1006*SNAP_FAIL)
#define SNAP_INVALIDOP	    (1007*SNAP_FAIL)
#define SNAP_INVALIDCID     (1008*SNAP_FAIL)
#define SNAP_BUFFERLIMIT    (1009*SNAP_FAIL)
#define SNAP_SOCKETERROR    (1010*SNAP_FAIL)
#define SNAP_NOINIT	    (1011*SNAP_FAIL)
#define SNAP_NOAUTHENTICATE (1012*SNAP_FAIL)

/* For compatibility with existing programs the following error
 * is defined as a synonym for SNAP_NOAUTHENTICATE.  Whenever this
 * error code is returned from SNAP_BeginConv, a detailed guardian
 * error code is available in the guardian_rc field of the SNAP_CPARMS
 * structure.
 */
#define SNAP_GUARDIAN_ERROR (1012*SNAP_FAIL)

#define SNAP_NOREPLYDUE     (1013*SNAP_FAIL)
#define SNAP_SERVERREJECT   (1014*SNAP_FAIL)
#define SNAP_TIMEOUT	    (1015*SNAP_FAIL)
#define SNAP_INVALIDACK     (1016*SNAP_FAIL)
#define SNAP_WRONGVERSION   (1017*SNAP_FAIL)
#define SNAP_SELECTFAILED   (1018*SNAP_FAIL)
#define SNAP_RESEND	    (1019*SNAP_FAIL)
#define SNAP_SERVERDIED     (1020*SNAP_FAIL)

/* The following error is only returned by the IBM PC SNAP implementation */

#define SNAP_ABORTED	    (2000*SNAP_FAIL)

/* Op codes in SNAP packets */

#define SNAP_SENDWITHREPLY  1
#define SNAP_SENDNOREPLY    2
#define SNAP_BEGINCONV	    3
#define SNAP_ENDCONV	    4
#define SNAP_REPLY	    5
#define SNAP_AUTHENTICATE   6
#define SNAP_ACK	    7
#define SNAP_NOTIFY	    8

/* Notify type codes */

#define SNAP_SERVERGONE     1
#define SNAP_BROADCAST	    2

#define SNAP_KEYLEN	    48

char *SNAP_AppendIntToMsg ();
char *SNAP_ExtractIntFromMsg ();
char *SNAP_AppendStringToMsg ();
char *SNAP_ExtractStringFromMsg ();
char *SNAP_AppendBytesToMsg ();
char *SNAP_ExtractBytesFromMsg ();
