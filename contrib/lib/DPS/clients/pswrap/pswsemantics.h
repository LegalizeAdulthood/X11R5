/*
 *  pswsemantics.h
 *
 * Copyright (C) 1988 by Adobe Systems Incorporated.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notices appear in all copies and that
 * both those copyright notices and this permission notice appear in
 * supporting documentation and that the name of Adobe Systems
 * Incorporated not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  If any portion of this software is changed, it cannot be
 * marketed under Adobe's trademarks and/or copyrights unless Adobe, in
 * its sole discretion, approves by a prior writing the quality of the
 * resulting implementation.
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

#ifndef	PSWSEMANTICS_H
#define	PSWSEMANTICS_H

#include "pswpriv.h"

/* PROCEDURES */

extern PSWName(/* char *s */);
extern FinalizePSWrapDef(/* hdr, body */);
extern Header PSWHeader(/* isStatic, inArgs, outArgs */);
extern Token PSWToken(/* type, val */);
extern Token PSWToken2(/* type, val, ind */);
extern Arg PSWArg(/* type, items */);
extern Item PSWItem(/* name */);
extern Item PSWStarItem(/* name */);
extern Item PSWSubscriptItem(/* name, subscript */);
extern Item PSWScaleItem(/* name, subscript */);
extern Subscript PSWNameSubscript(/* name */);
extern Subscript PSWIntegerSubscript(/* val */);
extern Args ConsPSWArgs(/* arg, args */);
extern Tokens AppendPSWToken(/* token, tokens */);
extern Args AppendPSWArgs(/* arg, args */);
extern Items AppendPSWItems(/* item, items */);

#endif /* PSWSEMANTICS_H */
