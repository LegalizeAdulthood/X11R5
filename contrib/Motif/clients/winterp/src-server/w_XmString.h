/* -*-C-*-
********************************************************************************
*
* File:         w_XmString.h
* RCS:          $Header: w_XmString.h,v 1.1 91/03/14 03:13:02 mayer Exp $
* Description:  Declare Puclic Interfaces to XmString routines
* Author:       Niels Mayer, HPLabs
* Created:      Sun Nov  5 14:46:20 1989
* Modified:     Thu Oct  3 20:14:56 1991 (Niels Mayer) mayer@hplnpm
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
*
********************************************************************************
*/

/******************************************************************************
 * XmString Get_String_or_XmString_Arg_Returning_XmString(item)
 *      LVAL *item;
 ******************************************************************************/
extern XmString Get_String_or_XmString_Arg_Returning_XmString();

/******************************************************************************
 * void Wxms_Garbage_Collect_XmString(lval_xmstring)
 *     LVAL lval_xmstring;
 ******************************************************************************/
extern void Wxms_Garbage_Collect_XmString();

/******************************************************************************
 * LVAL Wxms_XmStringTable_To_Lisp_Vector(xmstrtab, size)
 *     XmStringTable xmstrtab;
 *     int           size;
 ******************************************************************************/
extern LVAL Wxms_XmStringTable_To_Lisp_Vector();

typedef struct _Deallocator_Pair {
    XtPointer pointer;
    void    (*deallocator)();
} Deallocator_Pair;

typedef struct _SuperXmStringTable {
  XmString* xmstrtab;		/* public */
  int xmstrtab_end_idx;		/* public -- the number of elts in array */
  Deallocator_Pair *freeables;	/* private */
  int freeables_end_idx;	/* private */
  int freeables_size;		/* private */
} * SuperXmStringTable;

/******************************************************************************
 * SuperXmStringTable Wxms_Cvt_LispStringSequence_to_SuperXmStringTable(lisp_val)
 *     LVAL lisp_val;
 ******************************************************************************/
extern SuperXmStringTable Wxms_Cvt_LispStringSequence_to_SuperXmStringTable();

/******************************************************************************
 * void Wxms_Free_SuperXmStringTable(superstrtab)
 *     SuperXmStringTable superstrtab;
 ******************************************************************************/
extern void Wxms_Free_SuperXmStringTable();
