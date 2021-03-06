/*
 * dpsXint.h	-- internal definitions to dpsXclient.c
 *
 * Copyright (C) 1989, 1991 by Adobe Systems Incorporated.
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

#ifndef DPSXINT_H
#define DPSXINT_H

/* The first part of this structure is generic; the last part is
   implementation-specific. */

typedef struct _t_DPSPrivContextRec {
  char *priv;
  DPSSpace space;
  DPSProgramEncoding programEncoding;
  DPSNameEncoding nameEncoding;
  DPSProcs procs;
  void (*textProc)();
  void (*errorProc)();
  DPSResults resultTable;
  unsigned int resultTableLength;
  struct _t_DPSContextRec *chainParent, *chainChild;
  
  struct _t_DPSPrivContextRec *next;
  integer lastNameIndex, cid;
  boolean eofReceived;
  char *wh;
  char *buf, *outBuf, *objBuf;
  integer nBufChars, nOutBufChars, nObjBufChars;
  DPSNumFormat numFormat;

/* Everthing after this is XDPS-specific */

  boolean creator;	/* Did this app. create the context? */
  int statusFromEvent;  /* Latest status reported by an event during reset. */
  void (*statusProc)();
  boolean zombie;    /* To avoid DPSAwaitReturnValues */
} DPSPrivContextRec, *DPSPrivContext;

/* The first part of this structure is generic; the last part is
   implementation-specific. */

typedef struct _t_DPSPrivSpaceRec {
  DPSSpaceProcs procs;
  
  struct _t_DPSPrivSpaceRec *next;
  long int lastNameIndex, sid;
  char *wh; /* KLUDGE to support DPSSendDestroySpace */
  DPSPrivContext firstContext;

/* Everthing after this is XDPS-specific */

  boolean  creator;	/* Did this app. create the space? */
} DPSPrivSpaceRec, *DPSPrivSpace;

#include "dpsint.h"

#endif /* DPSXINT_H */
