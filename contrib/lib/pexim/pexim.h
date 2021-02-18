/*		@(#)pexim.h	1.4 Stardent 91/04/30	*/
/*
 *			Copyright (c) 1991 by
 *			Stardent Computer Inc.
 *			All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Stardent Computer not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission. Stardent Computer
 * makes no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */
#include <phigs/phigs.h>
/*
 * PEXIM errors, Those with PEXIMPHIGS...are PHIGS Errors from the 
 * transformation utility.
 */
#define POK 0

#define PEXIMPHIGSERR2       2
#define PEXIMPHIGSERR151   151
#define PEXIMPHIGSERR152   152
#define PEXIMPHIGSERR155   155
#define PEXIMPHIGSERR158   158
#define PEXIMPHIGSERR159   159
#define PEXIMPHIGSERR160   160
#define PEXIMPHIGSERR161   161
#define PEXIMPHIGSERR162   162
#define PEXIMPHIGSERR163   163
#define PEXIMPHIGSERR164   164
#define PEXIMPHIGSERR505   505

#define PEXIMERRBASE 1000
#define PEXIMERRNotPEXServer PEXIMERRBASE + 1
#define PEXIMERRBadPEXServer PEXIMERRBASE + 2
#define PEXIMERRAlreadyAllocced PEXIMERRBASE + 3
#define PEXIMERRAlloc PEXIMERRBASE + 4
#define PEXIMERRBadWKID PEXIMERRBASE + 5
#define PEXIMERRNotRendering PEXIMERRBASE + 6
#define PEXIMERRNotIdle PEXIMERRBASE + 7
#define PEXIMERRStructureNotOpen PEXIMERRBASE + 8
#define PEXIMERRNoCurrentWKID PEXIMERRBASE + 9
#define PEXIMERRNotPaused PEXIMERRBASE + 10

/*
 * PEXIM state - mutually exclusive.
 *
 * O.K. transitions
 * IDLE -> {RENDERING,STRUCTURE} -> IDLE
 * RENDERING -> PAUSED -> RENDERING
 * 
 */
#define PEXIM_IDLE 0
#define PEXIM_RENDERING 1
#define PEXIM_STRUCTURE 2
#define PEXIM_PAUSED 3

#define PEXIM_NO_WKID -1

Pint pinit();
Pint popen_renderer( Pint wkid, Display *display, Drawable d);
Pint pbegin_rendering(Pint wkid);
Pint pend_rendering(Pint wkid, Bool flush);
Pint pclose_rendering(Pint wkid);
Pint pbegin_struct(Pint s_id);
Pint pend_struct();
Pint pset_struct_ws(Pint wkid);
Pint pinq_renderer_state(Pint *curr_wkid, Pint *state, Pint *curr_struct);
Pint ppause_rendering(Pint wkid);
Pint presume_rendering(Pint wkid);
