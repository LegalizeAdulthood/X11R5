/* -*-C-*-
********************************************************************************
*
* File:         w_savedobjs.c
* RCS:          $Header: w_savedobjs.c,v 1.3 91/03/14 03:14:12 mayer Exp $
* Description:  Hashtable of LVAL's to be protected against garbage coll.
* Author:       Niels Mayer, HPLabs
* Created:      Sun Sep 24 22:31:43 1989
* Modified:     Fri Oct  4 20:18:15 1991 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r5 contrib tape release
*
* WINTERP Copyright 1989, 1990, 1991 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
**
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: w_savedobjs.c,v 1.3 91/03/14 03:14:12 mayer Exp $";

#include <stdio.h>
#include <Xm/Xm.h>		/* Xm/Xm.h only needed for "winterp.h"*/
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"

/*
  We put an initializer for LVAL v_savedobjs in xlsym:xlsinit(), and add
  this to the list of structures that gets marked by gc. v_savedobjs is a
  hashtable for storing LVAL's that need to persist across garbage
  collections.

  v_savedobjs is typically used for storing callback-objects,
  timeout-objects, etc.  These objects need to persist even though they are
  not directly referenced by any user-space global variables because a
  callback or timout may occur at any time and we must not allow the code or
  lexical environment associated with one of these delayed calls to be gc'd.

  v_savedobjs is used to store LVAL's. The hash function removes the bits
  from the LVAL address used for adressing within the LVAL (i.e. rightshift
  by the # of bits in an LVAL struct. Then we take that value modulo
  VSAVEDOBJS_SIZE to come up with the index into the hashtable.
*/

#define HASHTAB_ADDR_MASK 0xffL	/* value must be (2^x - 1) for any x */
#define HASHTAB_SIZE ((int) HASHTAB_ADDR_MASK + 1)
static int LVAL_ADDRESS_WIDTH;	/* must be initialized by Wso_Init(). */

/******************************************************************************
 * Given an LVAL, returns the hash index into v_savedobjs for that object.
 * This is done quite sleazily/simply by using the adress of the lisp object
 * right shifted by the number of bits used to index inside a LVAL structure
 * Then masked by the size of the power-of-2-sized hashtable. The result is
 * a value ranging from 0 to HASH_MASK.
 ******************************************************************************/
int Wso_Hash(object)
     LVAL object;
{
  unsigned long i;
  i = ((unsigned long) object >> LVAL_ADDRESS_WIDTH) & HASHTAB_ADDR_MASK;
  return ((int) i);
}


/******************************************************************************
 *
 ******************************************************************************/
Wso_Init()
{
  extern LVAL v_savedobjs;	/* xlglob.c */
  int i;
  LVAL sym;

  /* compute the number of bits used to index within an LVAL structure */
  i = sizeof(struct node);	/* size of an LVAL* in bytes */
  LVAL_ADDRESS_WIDTH = 1;
  while (i >= 2) {
    i /= 2;
    ++LVAL_ADDRESS_WIDTH;
  }

  sym = xlenter("*SAVED_OBJS*");
  v_savedobjs = newvector(HASHTAB_SIZE);
  setvalue(sym, v_savedobjs); /* allow lisp access to v_savedobjs for debugging */
}


