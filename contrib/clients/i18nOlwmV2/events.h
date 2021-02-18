/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

/* @(#)events.h 50.1 90/12/12 */

extern void EventLoop();
extern void InstallInterposer();
extern void UninstallInterposer();

/* event dispositions returned by interposer functions */
#define DISPOSE_DISPATCH 0
#define DISPOSE_USED	 1
#define DISPOSE_DEFER	 2

typedef void (*TimeoutFunc)();
extern void TimeoutRequest();	/* int time, TimeoutFunc f, void *closure */
extern void TimeoutCancel();	/* no params */

extern int PropagateEventToParent();

extern Time LastEventTime;
