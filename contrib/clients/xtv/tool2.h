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
 * they all return without attempting to change the subscriber list.
 *   offrightSubscr() - returns TRUE whenever called on a participant.
 *   getSubscr() - returns (Subscriber *) 0L whenever called on a participant.
 *
 * These functions provide access to the subscriber list for the given tool.
 * Since there is no way to know the current tool as an implicit parameter,
 * the tool must be explicitly provided as a parameter.
 * NOTE: The state of the subscriber list can be asserted consistent only
 * within the same function. SO, don't count on the cursor of the list
 * being the same between different subroutines that you write.
 * This is just a friendly warning; race conditions can be difficult to
 * diagnose.
 */
#ifdef __STDC__

void startSubscr(Tool *tool);
void forthSubscr(Tool *tool);
int offrightSubscr(Tool *tool);
void addSubscr(Tool *tool, Subscriber *newSubscr);
void deleteSubscr(Tool *tool);
Subscriber *getSubscr(Tool *tool);
void findSubscr(Tool *tool, Subscriber *targetSubscr);
void deleteAllSubscr(Tool *tool);

#else

/*
 * For parameters look at ANSI definitions above.
 */
void startSubscr();
void forthSubscr();
int offrightSubscr();
void addSubscr();
void deleteSubscr();
Subscriber *getSubscr();
void findSubscr();
void deleteAllSubscr();

#endif

/*
 * End Murray Addition
 */

