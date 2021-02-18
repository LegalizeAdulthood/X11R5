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
	popups.h
	Declarations and definitions for popups.c

*/

#ifndef POPUPS_H
#define POPUPS_H

#define Quit 0
#define  TermTool 1
#define  DropPart 2

extern void InitializeQuit();
extern void InitializeAllPopUps();
extern void InitializeNewTool();

/*
extern void InitializeTermTool();
extern void InitializeDropPart();
*/

extern XtCallbackProc PopUpBox();
extern XtCallbackProc PopDownBox();

extern Widget      TranShell[];
extern Widget      TranShellNewTool;
extern Widget      NewToolDialog;

extern Widget      TranShellJoinTool;
extern Widget      JoinToolList;
extern Widget      JoinToolViewPort;

#endif 
