/* clock.h - External Interface to the clock module
 *
 * Copyright (C) 1990-1991 by Adobe Systems Incorporated.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notices appear in all copies and that
 * both those copyright notices and this permission notice appear in
 * supporting documentation and that the name of Adobe Systems
 * Incorporated not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.
 * 
 * ADOBE MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THE SOFTWARE FOR
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
 * ADOBE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO EVENT SHALL ADOBE BE LIABLE
 * TO YOU OR ANY OTHER PARTY FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE, STRICT LIABILITY OR ANY OTHER ACTION ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  ADOBE WILL NOT
 * PROVIDE ANY TRAINING OR OTHER SUPPORT FOR THE SOFTWARE.
 * 
 * PostScript, Display PostScript, and Adobe are trademarks of Adobe Systems
 * Incorporated registered in the U.S.A. and other countries.
 *
 * Author: Adobe Systems Incorporated
 */

#ifndef	DPS_CLOCK
#define	DPS_CLOCK

/* The following data structure defines the paramters that the	*/
/* creator of a clock can control. This struct is a parameter	*/
/* to the CreateClock() call below.				*/
typedef struct {
  char *font;			/* Font to use			*/
  Boolean analog;		/* 1 => analog clock		*/
  Boolean chime;		/* 1 => chime on half hours	*/
  int update;			/* seconds before updating hand	*/
  int padding;			/* Padding around clock in pxls	*/
  } ClockParams;


extern Widget CreateClock(/* ClockParams * params */);
  /* Call this to create a widget containing a running clock	*/

extern void RealizeClock(/* Widget w */);
  /* Call this proc after you've realized the widget returned	*/
  /* by CreateClock(). This proc performs initialization that	*/
  /* depends on having a realized widget.			*/

#endif	DPS_CLOCK
