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
	stoh.h
	Declarations for stoh.c

*/

extern void STOH_LeavingSession();
extern void STOH_Event();
extern void STOH_Reply();
extern void STOH_WantToken();
extern void STOH_ReleasingToken();
extern void STOH_SendArchivePacket();
extern void STOH_NewToolLocal();
extern void STOH_NewToolRemote();
extern void STOH_TerminateTool();
extern void STOH_StopTool();
extern void STOH_GiveTokenToMe();
extern void STOH_DeQueToken();
extern void STOH_WantJoinTools();
extern void STOH_JoinTools();

/*
 * Begin Murray's Addition
 */
extern void STOH_QueuedList();
extern void STOH_ConfirmNewTool();
/*
 * End Murray's Addition
 */
