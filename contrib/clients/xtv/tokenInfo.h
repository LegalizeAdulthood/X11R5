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

#ifndef _TOKENINFO_H
#define _TOKENINFO_H

/*
 * tokenInfo.h
 *
 * Murray Anderegg -- 30-V-91
 *
 * tokenInfo.h contains the interface to the token information code.
 *
 * token information posts information in the xtv/xdesk message box about
 * the selected tool.
 */

#include "subscriber.h"
#include "token.h"
#include "tool.h"

/*
 * tokenInfoSubscr(BYTE toolID, short numQueued, BYTE queuedEntries[256])
 * Callback routine for HTOS_QueuedList.
 * This is the entry point for return information from the host.
 */
void tokenInfoBuild();

/*
 * tokenInfo(Widget widget, XtPointer clientData, XtPointer callData)
 * Invoke token information on the currently selected tool.
 */
XtCallbackProc tokenInfo();

#endif _TOKENINFO_H
