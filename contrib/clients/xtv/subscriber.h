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
	subscriber.h
	Declarations for subscriber management module

*/

#ifndef SUBSCRIBER__H
#define SUBSCRIBER__H

#include <time.h>
#include <X11/Intrinsic.h>
#include <sys/types.h>
#include "network.h"
/*
 * Begin Murray Addition
 */
#include "ptrlists.h"
/*
 * End Murray Addition
 */

#define	FULL_NAME_SIZE	50
#define	LOGIN_NAME_SIZE	32

typedef struct _ExposeWindow{
	CARD32	wid;
	int	ExposeCame;
	struct	_ExposeWindow *Next;
} ExposeWindow;

typedef struct _Subscriber {
	char			FullName[FULL_NAME_SIZE];
	char			LoginName[LOGIN_NAME_SIZE];
	char			HostName[HOST_SIZE];
	BYTE			MayHaveToken;
	BYTE			SubscriberID;
	Socket			Connection;
	struct sockaddr_in	ConnectionAddr;
	XtInputId		InputCallbackID;
	void                    *CatchUpList;
	void			*CurTool;
	off_t			CurPos;
	int			ExposeWaitState; /*0: Haven't sent enough packet
						   1: Waiting for exposures */
	ExposeWindow		*ExposeList;
	/*
	 * Begin Murray Addition
	 */
	PTRLIST                *toolList;
	/*
	 * A list of tools used by this subscriber
	 * This list is only valid on the host.
	 */

	/*
	 * End Murray Addition
	 */
} Subscriber;


typedef struct _SubscriberNode {
	Subscriber		*Subsc;
	struct _SubscriberNode	*Next;
} SubscriberNode;

typedef struct _PasswordQuery {
	char			ChairmanName[FULL_NAME_SIZE];
	char			PasswordRequired;
} PasswordQuery;

void SetSubID();
void SetLocalSubID();
BYTE GetLocalSubID();
Subscriber *FirstSubscriber();
Subscriber *NextSubscriber();
void AddSubscriber();
void DeleteSubscriber();
Subscriber *FindSubscriber();
Subscriber *FindSubscriberByID();

/*
 * Begin Murray Addition
 *

#if defined (__STDC__)
Subscriber *FindSubscriberByNameHost(char *nameHost);
#else
Subscriber *FindSubscriberByNameHost();
#endif

#if defined (__STDC__)
int CompareSubscrNameHost(Subscriber *thisSubscribe, char *nameHost);
#else
int CompareSubscrNameHost();
#endif

/*
 * End Murray Addition
 */

void RebuildSubscriberList();
Widget InitializeSubscriber();
XtInputCallbackProc SubscriberCheck();
Socket GetHostConnection();
void CleanUpSubscriber();

#ifndef NoIDAvailable
#define	NoIDAvailable		255
#endif

#endif /* SUBSCRIBER__H */
