/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
  definitions shared between loadserv server and loadav client
*/
#include <snap.h>

#define SNAP_INTEGER long

/*opcode from client to server to ask load*/
#define OP_LOAD_REQ (1)

#define NETADDR struct sockaddr_in

/*maximum size of a request*/
#define SNAP_BUF_SIZE (8000)
extern int SNAP_debugmask;
#define getint		SNAP_ExtractIntFromMsg
extern char *SNAP_ExtractIntFromMsg();
#define getstr		SNAP_ExtractStringFromMsg
extern char *SNAP_ExtractStringFromMsg();
#define getbytes	SNAP_ExtractBytesFromMsg
extern char *SNAP_ExtractBytesFromMsg();
#define putint		SNAP_AppendIntToMsg
extern char *SNAP_AppendIntToMsg();
#define putstr		SNAP_AppendStringToMsg
extern char *SNAP_AppendStringToMsg();
#define putbytes	SNAP_AppendBytesToMsg
extern char *SNAP_AppendBytesToMsg();
