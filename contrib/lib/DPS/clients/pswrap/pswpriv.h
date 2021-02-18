/*
 *  pswpriv.h
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

#ifndef PSWPRIV_H
#define PSWPRIV_H

#include "pswtypes.h"
#include "psw.h"

/********************/
/* Types */
/********************/

typedef struct _t_ItemRec *Item;
/* Forward type designator */

typedef struct {
  boolean constant;
  int val;      /* valid if constant */
  char *name;	/* valid if not constant */
  } SubscriptRec, *Subscript, ScaleRec, *Scale;

typedef int Type;

typedef struct _t_ItemRec { /* see above */
  struct _t_ItemRec *next;
  char *name;
  boolean starred, subscripted, scaled;
  Subscript subscript;         /* valid if subscripted */
  Scale scale;

  /* the fields below are filled in by PSWHeader */
  boolean isoutput;        /* true if this is an output parameter */
  int tag;        /* valid if output is true; the index of
                             this output parameter. starting from 0. */
  Type type;        /* copied from parent Arg */
  int sourceLine;
  } ItemRec;

typedef Item Items;

typedef struct _t_ArgRec {
  struct _t_ArgRec *next;
  Type type;
  Items items;
  } ArgRec, *Arg;

typedef Arg Args;

typedef struct {
  boolean isStatic;
  char *name;
  Args inArgs, outArgs;
  } HeaderRec, *Header;

typedef struct {
  long cnst;
  char *var;
  } Adr, *PAdr;

typedef struct _t_TokenRec {
  struct _t_TokenRec *next;
  Type type;
  Adr adr; /* of this token in the binary object sequence. */
  int val; /* loopholed */
  int tokenIndex;
  boolean wellKnownName; /* valid if type is T_NAME or T_LITNAME */
  int sourceLine;
  Item namedFormal;
    /* non-NIL if this token is a reference to a formal.
       (T_STRING, T_HEXSTRING, T_NAME, and T_LITNAME) */
  Adr body;
    /* Meaning depends on the token type, as follows:
         simple => unused
         array or proc => adr of body in binobjseq
         string or hexstring => adr of body in binobjseq
         name or litname => adr of namestring or array in binobjseq (named arg)
                            or cnst = the nametag (well-known name)
                            or cnst = 0 (name index filled in at runtime)
	 subscripted => index for element
    */
} TokenRec, *Token;

typedef Token Tokens;

typedef Tokens Body;

typedef struct _t_TokenListRec {
  struct _t_TokenListRec *next;
  Token token;
  } TokenListRec, *TokenList;


#endif /* PSWPRIV_H */
