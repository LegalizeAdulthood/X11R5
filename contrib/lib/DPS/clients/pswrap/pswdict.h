/*
 *  pswdict.h
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

#ifndef	PSWDICT_H
#define	PSWDICT_H

typedef struct _t_PSWDictRec *PSWDict;
/* Opaque designator for a dictionary */

typedef int PSWDictValue; /* non-negative */
typedef char *PSWAtom;

/* PROCEDURES */

/* NOTES 
   The name parameters defined below are NULL-terminated C strings.
   None of the name parameters are handed off, i.e. the caller is
   responsible for managing their storage. */

extern PSWDict CreatePSWDict(/* int nEntries */);
/* nEntries is a hint. Creates and returns a new dictionary */

extern void DestroyPSWDict(/* PSWDict dict */);
/* Destroys a dictionary */

extern PSWDictValue PSWDictLookup(/* PSWDict dict; char *name */);
/* -1 => not found. */

extern PSWDictValue PSWDictEnter
  (/* PSWDict dict; char *name; PSWDictValue value; */);
/*  0 => normal return (not found)
   -1 => found. If found, the old value gets replaced with the new one. */

extern PSWDictValue PSWDictRemove(/* PSWDict dict; char *name */);
/* -1 => not found. If found, value is returned. */

extern PSWAtom MakeAtom(/* char *name */);

#endif /* PSWDICT_H */
