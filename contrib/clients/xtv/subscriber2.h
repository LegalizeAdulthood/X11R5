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
 * Begin Murray Addition
 */

/*
 * These functions only work on the host. If you are not on the host, then
 * they all return without attempting to change the tool list.
 *   offrightTool() - returns TRUE whenever called on a participant.
 *   getTool() - returns (Subscriber *) 0L whenever called on a participant.
 *
 * These functions provide access to the tool list for the given subscriber.
 * Since there is no way to know the current subscriber as an implicit parameter,
 * the subscriber must be explicitly provided as a parameter.
 * NOTE: The state of the tool list can be asserted consistent only
 * within the same function. SO, don't count on the cursor of the list
 * being the same between different subroutines that you write.
 * This is just a friendly warning; race conditions can be difficult to
 * diagnose.
 */
#ifdef __STDC__

void startTool(Subscriber *subscriber);
void forthTool(Subscriber *subscriber);
int offrightTool(Subscriber *subscriber);
void addTool(Subscriber *subscriber, Tool *tool);
void insertTool(Subscriber *subscriber, Tool *tool);
void deleteTool(Subscriber *subscriber);
Tool *getTool(Subscriber *subscriber);
void findTool(Subscriber *subscriber, Tool *targetTool);
void deleteAllTool(Subscriber *subscriber);
Tool *lastTool(Subscriber *subscriber);

#else

/*
 * For parameters look at ANSI definitions above.
 */
void startTool();
void forthTool();
int offrightTool();
void addTool();
void insertTool();
void deleteTool();
Tool *getTool();
void findTool();
void deleteAllTool();
Tool *lastTool();

#endif

/*
 * End Murray Addition
 */

