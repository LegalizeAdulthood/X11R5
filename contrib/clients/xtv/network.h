/*
 * Copyright (c) 1991 Old Dominion University.
 * Copyright (c) 1991 University of North Carolina at Chapel Hill.
 * All rights reserved.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  Old Dominion University and the University of North
 * Carolina at Chapel Hill make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

/*
	network.h
	Declarations for network-related routines

*/


#ifndef NETWORK__H
#define NETWORK__H

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <X11/Xmd.h>		/* For BYTE type definition */

typedef	int	Socket;

#define TIMEOUT			-2
#define	ERROR			-1
#define	NothingAvail		0xff
#define	HOST_SIZE		64		/* Maximum size of a host name */

#define DONTBLOCK 0
#define BLOCK 1

int TimedSendData();
int SendData();
int SendByte();
BYTE *ReadData();
BYTE CheckByte();
BYTE GetByte();
#endif /* NETWORK__H */
