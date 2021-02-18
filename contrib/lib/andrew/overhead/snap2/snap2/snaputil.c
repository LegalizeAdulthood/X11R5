/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *snaputil_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/snap2/snap2/RCS/snaputil.c,v 2.4 1991/09/12 17:21:27 bobg Exp $";

/*
 *	This package contains SNAP [Simple Network Application Protocol]
 *	Utility Functions.  These functions are used internally by the
 *	SNAP client and server layers and can also be used directly by
 *	SNAP application programs
 *
 *	Most of the procedures in this package implement the SNAP network
 *	data types (SNAP_integer, SNAP_string, and SNAP_bytestring).
 *	For each data type, 2 operations are provided: Append and Extract.
 *
 *	These routines all use SNAP_debugmask PCK (Pack/Unpack)
 *
 *	$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/snap2/snap2/RCS/snaputil.c,v 2.4 1991/09/12 17:21:27 bobg Exp $
 */


#include <stdio.h>
#include <andrewos.h> /* sys/types.h */
#include <sys/socket.h>
#include <netinet/in.h>
#include <snap.h>
#include <snapcom.h>

/*
 * Append a SNAP_integer to a message.
 * The integer is appended in "network byte order" (highest-order byte, next
 * highest-order byte, ... low-order byte).
 *
 * Returns the next available address in the message buffer.
 *
 */

char *SNAP_AppendIntToMsg (msg,val)
char *msg;
SNAP_integer val;
{
    long temp;
    char *p = (char *) &temp;
    int i;
    DBGMSG (PCK,("AppendInt - msg: %s, val: %ld", PTR(msg), val));

    temp = htonl((long) val);
    for (i=0; i<sizeof(long); i++)
	msg[i] = p[i];
    msg += sizeof(long);

    return (msg);
}

/*
  * Extract a SNAP_integer from a message.
  * The integer is extracted from "network byte order" format and converted
  * to host machine format.
  *
  * Returns the next address in the message buffer after the SNAP_integer.
  *
  */

char *SNAP_ExtractIntFromMsg (msg,valptr)
char *msg;
SNAP_integer *valptr;
{
    long temp;
    char *p = (char *) &temp;
    int i;

    for (i=0; i<sizeof(long); i++)
	p[i] = msg[i];
    *valptr = ntohl (temp);

    DBGMSG (PCK,("ExtractInt - msg: %s val: %ld", PTR(msg),*valptr));

    return (msg+sizeof(long));
}

/*
  * Append a string to a message.
  * String is appended with the following format:
  *
  *     1. SNAP_integer length; <--- [in network byte order]
    *     2. char string[length];
  *     3. char null = 0;
  *     4. char padding[x];     <--- [Padding to next SNAP_integer boundary]
    *
    * Returns the next available address in the message buffer.
    *
    */

#define NULLPTR 0xFF
char *SNAP_AppendStringToMsg (msg,s)
unsigned char *msg;
char *s;
{
    int i;
    char *str;

    if (s == NULL)
	str = "";
    else
	str = s;

    DBGMSG (PCK,("AppendString - msg: %s, str: %s", PTR(msg), str));

    i = strlen(str);
    msg = (unsigned char *) SNAP_AppendIntToMsg ((char *)msg, (SNAP_integer) i);
    strcpy (msg,str);
    msg += i+1;
    i = (sizeof(SNAP_integer) - ((i+1)%sizeof(SNAP_integer)))
      % sizeof(SNAP_integer);

    if (s == NULL) {
	*(msg++) = NULLPTR;
	i--;
    }

    while (i--)
	*(msg++) = 0;

    DBGMSG (PCK,("AppendString - msg: %s", PTR(msg)));

    return ((char *) msg);
}

/*
  * Extract a string from a message.
  * If you need the length of the string use ExtractIntFromMsg with
  * the same starting address in the message.
  * A pointer is set to point to the first character of the string.
  *
  * Returns the next address in the message buffer after the extracted
  * string (and its padding, if any).
  *
  */

char *SNAP_ExtractStringFromMsg (msg,strptr)
unsigned char *msg;
char **strptr;
{
    int i;

    DBGMSG (PCK,("ExtractString - msg: %s", PTR(msg)));

    *strptr = (char *) msg+sizeof(SNAP_integer);
    i = strlen(*strptr);
    msg = msg+i+1+sizeof(SNAP_integer);
    if ((i == 0) && (*msg == NULLPTR))
	*strptr = NULL;
    i = (sizeof(SNAP_integer) - ((i+1) % sizeof(SNAP_integer)))
      % sizeof(SNAP_integer);
    msg += i;

    DBGMSG (PCK,("ExtractString - msg: %s, str: %s", PTR(msg),
		  *strptr == NULL? "": *strptr));

    return ((char *) msg);
}

/*
  * Append arbitrary bytes to a message.
  * Bytes are appended with the following format:
  *
  *     1. SNAP_integer length; <--- [in network byte order]
    *     2. char bytes[length];
  *     3. char padding[x];     <--- [Padding to SNAP_integer boundary]
    *
    * Returns the next available address in the message buffer.
    *
    */

char *SNAP_AppendBytesToMsg (msg,bytes,len)
char *msg;
char *bytes;
int len;
{
    int i;

    DBGMSG (PCK,("AppendBytes - msg: %s, length: %d", PTR(msg), len));

    msg = SNAP_AppendIntToMsg (msg, (SNAP_integer) len);
    bcopy (bytes,msg,len);
    msg += len;
    i = (sizeof(SNAP_integer) - ((len)%sizeof(SNAP_integer)))
      % sizeof(SNAP_integer);
    while (i--)
	*(msg++) = 0;

    DBGMSG (PCK,("AppendBytes - msg: %s", PTR(msg)));

    return (msg);
}

/*
  * Extract bytes from a message.
  * If you need the length of the byte string use ExtractIntFromMsg with
  * the same starting address in the message.
  * A pointer is set to point to the first data byte.
  *
  * Returns the next address in the message buffer after the extracted
  * bytes (and its padding, if any).
  *
  */

char *SNAP_ExtractBytesFromMsg (msg,bytesptr)
char *msg;
char **bytesptr;
{
    SNAP_integer i;

    DBGMSG (PCK,("ExtractBytes - msg: %s", PTR(msg)));

    *bytesptr = msg+sizeof(SNAP_integer);
    msg = SNAP_ExtractIntFromMsg (msg, &i);
    i += (sizeof(SNAP_integer) - ((i) % sizeof(SNAP_integer)))
      % sizeof(SNAP_integer);
    msg += i;

    DBGMSG (PCK,("ExtractBytes - msg: %s", PTR(msg)));

    return (msg);
}

/*
  * Netaddr to Network Byte Order
  *
  */

SNAP_NetaddrToNBO (netaddr)
NETADDR *netaddr;
{
    netaddr -> sin_family = htons(netaddr->sin_family);
#ifdef IBMPC
    netaddr->sin_port = htons (netaddr->sin_port);
#endif /* IBMPC */
}

/*
  * Netaddr from Network Byte Order
  *
  */

SNAP_NetaddrFromNBO (netaddr)
NETADDR *netaddr;
{
    netaddr->sin_family = ntohs (netaddr->sin_family);
#ifdef IBMPC
    netaddr->sin_port = ntohs (netaddr->sin_port);
#endif /* IBMPC */
}
