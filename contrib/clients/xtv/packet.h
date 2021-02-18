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
	packet.h
	Definitions and declarations for packet.c

*/

#ifndef PACKET__H
#define PACKET__H

#include <X11/Intrinsic.h>

XtInputCallbackProc HandleIncomingClientPacket();
XtInputCallbackProc HandleIncomingServerPacket();
XtInputCallbackProc HandleIncomingViewerPacket();
XtInputCallbackProc HandlePortException();

BYTE *GetReply();
int SendReply();

BOOL    PopUpPacket;
INT16   RelativeX;
INT16   RelativeY;

typedef struct _PopUpInfo {
        BYTE    opCode;
        BYTE    pad1,pad2,pad3;
        Window  AbsoluteWindow;
        INT16   PopUpX;
        INT16   PopUpY;
} PopUpInfo;

#endif /* PACKET__H */
