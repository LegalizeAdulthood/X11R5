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
	htos.h
	Declarations for htos.c

*/

extern void HTOS_EndSession();
extern void EndSession();
extern void HTOS_DroppedYou();
extern void HTOS_X11Packet();
extern void HTOS_InternalAtom();
extern void HTOS_AllocColor();
extern void HTOS_YouAre();
extern void HTOS_AddNewSubscriber();
extern void HTOS_DeleteSubscriber();
extern void HTOS_AddNewTool();
extern void HTOS_DeleteTool();
extern void HTOS_TakeToken();
extern void HTOS_ForceTokenRelease();
extern void HTOS_AskTokenRelease();
extern void HTOS_CreatorTakeToken();
extern void HTOS_ContinueTakeToken();
extern void HTOS_CatchUpStart();
extern void HTOS_CatchUpEnd();
extern void HTOS_ToolsToJoin();

/*
 * Begin Murray Addition
 */
enum toWhom {tokInfo=0, partInfo=1};
extern void HTOS_QueuedList();
extern void HTOS_IncomingTools();
extern void HTOS_EndIncoming();
/*
 * End Murray Addition
 */


