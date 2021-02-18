/* -*-C-*-
********************************************************************************
*
* File:         w_XmString.c
* RCS:          $Header: w_XmString.c,v 1.6 91/03/14 03:13:22 mayer Exp $
* Description:  Interfaces to XmString routines
* Author:       Niels Mayer, HPLabs
* Created:      Sun Nov  5 14:46:20 1989
* Modified:     Thu Oct  3 23:40:04 1991 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: w_XmString.c,v 1.6 91/03/14 03:13:22 mayer Exp $";

#include <stdio.h>
#include <Xm/Xm.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_XmString.h"

static LVAL s_XmSTRING_DIRECTION_L_TO_R, s_XmSTRING_DIRECTION_R_TO_L; /* init'd in Wxms_Init() */

/******************************************************************************
 *
 ******************************************************************************/
void Wxms_Garbage_Collect_XmString(lval_xmstring)
     LVAL lval_xmstring;
{
  XmStringFree(get_xmstring(lval_xmstring));
}


/******************************************************************************
 * the XmStrings created here will be freed when the lisp vector holding
 * the XmStrings gets garbage collected and  there are no more references
 * to the XmStrings.
 ******************************************************************************/
LVAL Wxms_XmStringTable_To_Lisp_Vector(xmstrtab, size)
     XmStringTable xmstrtab;
     int           size;
{
  LVAL result;
  int i;
  
  if (!xmstrtab || !size)
    return (NIL);

  xlsave1(result);
  result = newvector(size);
  for (i = 0; i < size; i++)
    setelement(result, i, cv_xmstring(XmStringCopy(xmstrtab[i])));
  xlpop();
  return (result);
}


/******************************************************************************
 * This is equivalent to an xlga*() function, in that it retrieves an argument
 * of a certain type from the argument stack. This particular xlga*() function
 * will retrieve a String OR XmString argument to an XLISP function. If the
 * argument was a string, it will convert it to an XmString and return that,
 * otherwise it returns the XmString-value of the argument.
 *
 * In calling this function, you must pass it a pointer to an LVAL in which
 * this function will store a lisp-node of type XLTYPE_XmString. If this 
 * function ended up converting a string to an XmString, that XmString will be
 * garbage collected unless it is explicitly saved by code in the calling function.
 * In many cases, this fn. is used in methods that return the XLTYPE_XmString
 * node, in which case the XLTYPE_XmString won't be garbage collected if it
 * is referenced in the user's lisp code.
 ******************************************************************************/
XmString Get_String_or_XmString_Arg_Returning_XmString(item)
     LVAL *item;		/* param to return an LVAL_XmString */
{
  XmString result;
  LVAL arg = xlgetarg();

  if (xmstring_p(arg)) {
    *item = arg;
    return (get_xmstring(arg));
  }
  else if (stringp(arg)) {
    result = XmStringCreate(getstring(arg), XmSTRING_DEFAULT_CHARSET);
    *item = cv_xmstring(result); /* note that the XmString created  will get g.c'd if not ref'd */
    return (result);
  }
  else
    xlerror("Bad type: expected either a XmString or a string.", arg);
}


/******************************************************************************
 * Wxms_Cvt_LispStringSequence_to_SuperXmStringTable() converts it's LVAL argument
 * and returns a SuperXmStringTable. The LVAL is a sequence (list or array) of
 * String(s) OR XmString(s).
 * If the sequence element was a string, this routine will automatically convert it
 * to an XmString. Thus this routine will return a SuperXmStringTable object, which
 * contains the XmStringTable representing the LVAL.
 *
 * You MUST call Wxms_Free_SuperXmStringTable() after calling this function and after
 * the SuperXmStringTable result of this function is no longer being used.
 * The cleanup function will deallocate any automatically converted XmStrings.
 ******************************************************************************/

#define XMSTRINGTABLE_SIZE_INCREMENT 50
#define FREEABLES_SIZE_INCREMENT 50

static SuperXmStringTable New_SuperXmStringTable(size)
     int size;
{
  SuperXmStringTable superstrtab = (SuperXmStringTable) XtMalloc((unsigned) sizeof(struct _SuperXmStringTable));
  superstrtab->xmstrtab = (XmString *) XtMalloc((unsigned) (size * sizeof(XmString)));
  superstrtab->freeables = NULL;
  return (superstrtab);
}

void Wxms_Free_SuperXmStringTable(superstrtab)
     SuperXmStringTable superstrtab;
{
  /*
   * deallocate stuff declared as freeable by SuperXmStringTable_Declare_Temporary_Storage()
   */
  register int i, freeables_end_idx;
  Deallocator_Pair* freeables;

  if (superstrtab->freeables) {
    freeables = superstrtab->freeables;
    freeables_end_idx = superstrtab->freeables_end_idx;
    for (i = 0 ; i < freeables_end_idx ; i++)
      (*(freeables[i].deallocator))(freeables[i].pointer);
    XtFree(superstrtab->freeables);
  }

  /*
   * Free the XmStringTable
   */
  if (superstrtab->xmstrtab)
    XtFree(superstrtab->xmstrtab);

  XtFree(superstrtab);
}

static void SuperXmStringTable_Declare_Temporary_Storage(superstrtab, pointer, deallocator)
     SuperXmStringTable superstrtab;
     XtPointer          pointer;
     void             (*deallocator)();
{
  if (superstrtab->freeables == NULL) {
    superstrtab->freeables_size = FREEABLES_SIZE_INCREMENT;
    superstrtab->freeables = (Deallocator_Pair *) XtMalloc((unsigned) (superstrtab->freeables_size * sizeof(struct _Deallocator_Pair)));
    superstrtab->freeables_end_idx = 0;
  }
  else if (superstrtab->freeables_end_idx >= superstrtab->freeables_size) {
    superstrtab->freeables_size += FREEABLES_SIZE_INCREMENT;
    superstrtab->freeables = (Deallocator_Pair *) XtRealloc(superstrtab->freeables, (unsigned) (superstrtab->freeables_size * sizeof(struct _Deallocator_Pair)));
  }

  superstrtab->freeables[superstrtab->freeables_end_idx].pointer = pointer;
  superstrtab->freeables[superstrtab->freeables_end_idx].deallocator = deallocator;
  ++(superstrtab->freeables_end_idx);
}

SuperXmStringTable Wxms_Cvt_LispStringSequence_to_SuperXmStringTable(lisp_val)
     LVAL lisp_val;
{
  LVAL elt;
  SuperXmStringTable superstrtab;
  XmString* xmstrtab;
  register int xmstrtab_end_idx;
  int xmstrtab_size;

  /*
   * if argument is a vector, then step through array building C XmString Table.
   */
  if (vectorp(lisp_val)) {
    xmstrtab_size = getsize(lisp_val);
    superstrtab = New_SuperXmStringTable(xmstrtab_size);
    xmstrtab = superstrtab->xmstrtab;
    
    for (xmstrtab_end_idx = 0; xmstrtab_end_idx < xmstrtab_size; xmstrtab_end_idx++) {
      elt = getelement(lisp_val, xmstrtab_end_idx);
      if stringp(elt)
	SuperXmStringTable_Declare_Temporary_Storage(superstrtab,
	   (XtPointer) (xmstrtab[xmstrtab_end_idx] = XmStringCreate((char*) getstring(elt), XmSTRING_DEFAULT_CHARSET)),
           XmStringFree);
      else if xmstring_p(elt)
	xmstrtab[xmstrtab_end_idx] = get_xmstring(elt);	/* note -- this XmString will get freed by garbage collection */
      else {
	Wxms_Free_SuperXmStringTable(superstrtab);	
	xlerror("Bad XmStringTable element -- expected a String or an XmString.", elt);
      }
    }
    superstrtab->xmstrtab_end_idx = xmstrtab_end_idx;
    return (superstrtab);
  }
  /*
   * if argument is a list, then cdr through list building C XmString Table.
   */
  else if (consp(lisp_val)) {
    xmstrtab_size = XMSTRINGTABLE_SIZE_INCREMENT;
    superstrtab = New_SuperXmStringTable(xmstrtab_size);
    xmstrtab = superstrtab->xmstrtab;

    for (xmstrtab_end_idx = 0 ; (consp(lisp_val)) ; lisp_val = cdr(lisp_val), xmstrtab_end_idx++) {
      if (xmstrtab_end_idx >= xmstrtab_size) { /* make sure it'll fit into allocated xmstrtab */
	xmstrtab_size += XMSTRINGTABLE_SIZE_INCREMENT;
	superstrtab->xmstrtab = xmstrtab = (XmString *) XtRealloc(xmstrtab, (unsigned) (xmstrtab_size * sizeof(XmString)));
      }
      elt = car(lisp_val);
      if (stringp(elt))
	SuperXmStringTable_Declare_Temporary_Storage(superstrtab,
	   (XtPointer) (xmstrtab[xmstrtab_end_idx] = XmStringCreate((char*) getstring(elt), XmSTRING_DEFAULT_CHARSET)),
           XmStringFree);
      else if (xmstring_p(elt))
	xmstrtab[xmstrtab_end_idx] = get_xmstring(elt);	/* note -- this XmString will get freed by garbage collection */
      else {
	Wxms_Free_SuperXmStringTable(superstrtab);
	xlerror("Bad XmStringTable element -- expected a String or an XmString.", elt);
      }
    }
    if (lisp_val) {		/* if loop terminated due to list pointer not being a CONS cell */
      Wxms_Free_SuperXmStringTable(superstrtab);	
      xlerror("Bad XmStringTable list element -- expected a list of Strings or XmStrings.", lisp_val);
    }
    superstrtab->xmstrtab_end_idx = xmstrtab_end_idx;
    return (superstrtab);
  }
  /*
   * if argument wasn't list or vector, then error
   */
  else
    xlerror("Bad argument type. Expected a list or vector of strings or XmStrings", lisp_val);
}


/******************************************************************************
 * (XM_STRING_CREATE <string> [<charset>])
 *	--> returns an XMSTRING version of string <string>.
 * 
 * Create a compound string from STRING <text>. Optional argument <charset>
 * is a STRING specifying an XmStringCharSet. If the <charset> argument is
 * ommitted, XmSTRING_DEFAULT_CHARSET is used.
 *
 * XmString XmStringCreate (text, charset)
 *     char	*text;
 *     XmStringCharSet charset;
 ******************************************************************************/
LVAL Wxms_Prim_XM_STRING_CREATE()
{
  char* string;
  XmStringCharSet charset;

  string = (char*) getstring(xlgastring());
  if (moreargs())
    charset = (XmStringCharSet) getstring(xlgastring());
  else
    charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;
  xllastarg();
  
  return (cv_xmstring(XmStringCreate(string, charset)));
}


/******************************************************************************
 * (XM_STRING_DIRECTION_CREATE :STRING_DIRECTION_L_TO_R)
 *	--> returns an XMSTRING with a single L->R direction component.
 * (XM_STRING_DIRECTION_CREATE :STRING_DIRECTION_R_TO_L)
 *	--> returns an XMSTRING with a single R->L direction component
 *
 *
 * XmString XmStringDirectionCreate (direction)
 *     XmStringDirection direction;
 ******************************************************************************/
LVAL Wxms_Prim_XM_STRING_DIRECTION_CREATE()
{
  LVAL direction = xlgetarg();
  xllastarg();
  
  if (direction == s_XmSTRING_DIRECTION_L_TO_R)
    return (cv_xmstring(XmStringDirectionCreate(XmSTRING_DIRECTION_L_TO_R)));
  else if (direction == s_XmSTRING_DIRECTION_R_TO_L)
    return (cv_xmstring(XmStringDirectionCreate(XmSTRING_DIRECTION_R_TO_L)));
  else
    xlerror("Bad <direction> keyword.", direction);
}


/******************************************************************************
 * (XM_STRING_SEPARATOR_CREATE)
 *	--> returns an XMSTRING with a single component, a separator.
 *
 * XmString XmStringSeparatorCreate ()
 ******************************************************************************/
LVAL Wxms_Prim_XM_STRING_SEPARATOR_CREATE()
{
  xllastarg();
  return (cv_xmstring(XmStringSeparatorCreate()));
}


/******************************************************************************
 * (XM_STRING_SEGMENT_CREATE <text> [<charset>] <direction> <separator_p>)
 *	--> returns a XMSTRING of the specified 
 * string <text>, 
 * optional string specifying character set <charset>, 
 * symbol <direction>, either :STRING_DIRECTION_L_TO_R, or
 *                            :STRING_DIRECTION_R_TO_L
 * if <separator_p> is not NIL, a separator component will be included.
 * 

 * XmString XmStringSegmentCreate (text, charset, direction, separator)
 *     char 		 *text;
 *     XmStringCharSet	 charset;
 *     XmStringDirection direction;
 *     Boolean		 separator;
 ******************************************************************************/
LVAL Wxms_Prim_XM_STRING_SEGMENT_CREATE()
{
  XmStringDirection direction;
  char* text;
  LVAL lval_direction, separator_p, lval_charset;
  XmStringCharSet charset;

  text = (char*) getstring(xlgastring());
  lval_charset = xlgetarg();
  if (stringp(lval_charset)) {
    charset = (XmStringCharSet) getstring(lval_charset);
    lval_direction = xlgetarg();
  }
  else {
    charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;
    lval_direction = lval_charset;
  }
  separator_p = xlgetarg();
  xllastarg();

  if (lval_direction == s_XmSTRING_DIRECTION_L_TO_R)
    direction = XmSTRING_DIRECTION_L_TO_R;
  else if (lval_direction == s_XmSTRING_DIRECTION_R_TO_L)
    direction = XmSTRING_DIRECTION_R_TO_L;
  else
    xlerror("Bad <direction> keyword.", lval_direction);
  
  return (cv_xmstring(XmStringSegmentCreate(text, charset, direction, 
					    (separator_p) ? TRUE : FALSE)));
}

/******************************************************************************
 * (XM_STRING_CREATE_L_TO_R <text> [<charset>])
 *	--> returns an XMSTRING version of string <text>.
 * 
 * Create a left-to-right compound string from STRING <text>. If <text>
 * includes '\n', they will be replaced by a separator.
 *
 * Optional argument <charset> is a STRING specifying an XmStringCharSet. If the
 * <charset> argument is ommitted, XmSTRING_DEFAULT_CHARSET is used.
 *
 * XmString XmStringCreateLtoR (text, charset)
 *     char	*text;
 *     XmStringCharSet charset;
 ******************************************************************************/
LVAL Wxms_Prim_XM_STRING_CREATE_L_TO_R()
{
  char* string;
  XmStringCharSet charset;

  string = (char*) getstring(xlgastring());
  if (moreargs())
    charset = (XmStringCharSet) getstring(xlgastring());
  else
    charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;
  xllastarg();
  
  return (cv_xmstring(XmStringCreateLtoR(string, charset)));
}


/******************************************************************************
 * XmStrings are gross! -- I'm ignoring this fn in this version of WINTERP.
 * 
 * Boolean XmStringInitContext (context, string)
 *     XmStringContext *context;
 *     XmString string;
 ******************************************************************************/
/******************************************************************************
 * XmStrings are gross! -- I'm ignoring this fn in this version of WINTERP.
 *
 * void XmStringFreeContext (context)
 *     XmStringContext context;
 ******************************************************************************/
/******************************************************************************
 * XmStrings are gross! -- I'm ignoring this fn in this version of WINTERP.
 *
 * XmStringComponentType 
 * XmStringGetNextComponent (context, text, charset, direction, 
 * 		unknown_tag, unknown_length, unknown_value)
 *     XmStringContext 	context;
 *     char		**text;
 *     XmStringCharSet	*charset;
 *     XmStringDirection	*direction;
 *     XmStringComponentType *unknown_tag;
 *     UShort		*unknown_length;
 *     UChar		**unknown_value;
 ******************************************************************************/
/******************************************************************************
 * XmStrings are gross! -- I'm ignoring this fn in this version of WINTERP.
 *
 * XmStringComponentType XmStringPeekNextComponent (context)
 *     XmStringContext	context;
 ******************************************************************************/
/******************************************************************************
 * XmStrings are gross! -- I'm ignoring this fn in this version of WINTERP.
 *
 * Boolean XmStringGetNextSegment (context, text, charset, direction, separator)
 *     XmStringContext 	context;
 *     char		**text;
 *     XmStringCharSet	*charset;
 *     XmStringDirection *direction;
 *     Boolean		*separator;
 ******************************************************************************/


/******************************************************************************
 * (XM_STRING_GET_L_TO_R <xmstring> [<charset>])
 *	--> returns a STRING, or NIL if no matching character set found.
 *
 * This function will fetch the first text segment of the XMSTRING <xmstring>
 * which matches the character set <charset>, and returns that as a STRING.
 * Optional argument <charset> is a STRING specifying an XmStringCharSet.
 * If the <charset> argument is ommitted, XmSTRING_DEFAULT_CHARSET is used.
 *
 * Boolean XmStringGetLtoR (string, charset, text)
 *     XmString 		string;
 *     XmStringCharSet	charset;
 *     char		**text;
 ******************************************************************************/
LVAL Wxms_Prim_XM_STRING_GET_L_TO_R()
{
  LVAL lval_string;
  char* result;
  XmStringCharSet charset;
  XmString xmstring;

  xmstring = get_xmstring(xlga_xmstring());
  if (moreargs())
    charset = (XmStringCharSet) getstring(xlgastring());
  else
    charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;
  xllastarg();

  if (XmStringGetLtoR(xmstring, charset, &result)) {
/*    
    lval_string = cvstring(result);
    XtFree(result);
*/
    lval_string = cv_string(result); /* don't copy string, let XLISP-GC free it... */
    return (lval_string);
  }
  else 
    return (NIL);
}


/******************************************************************************
 * XmStrings are gross! -- I'm ignoring this fn in this version of WINTERP.
 *
 * XmFontList XmFontListCreate (font, charset)
 *     XFontStruct 	*font;
 *     XmStringCharSet 	charset;
 ******************************************************************************/
/******************************************************************************
 * XmStrings are gross! -- I'm ignoring this fn in this version of WINTERP.
 *
 * XmFontList XmStringCreateFontList (font, charset)
 *     XFontStruct 	*font;
 *     XmStringCharSet 	charset;
 ******************************************************************************/
/******************************************************************************
 * XmStrings are gross! -- I'm ignoring this fn in this version of WINTERP.
 *
 * void  XmFontListFree (fontlist)
 *     XmFontList fontlist;
 ******************************************************************************/
/******************************************************************************
 * XmStrings are gross! -- I'm ignoring this fn in this version of WINTERP.
 *
 * XmFontList XmFontListAdd (old, font, charset)
 *     XmFontList 	old;
 *     XFontStruct		*font;
 *     XmStringCharSet	charset;
 ******************************************************************************/
/******************************************************************************
 * XmStrings are gross! -- I'm ignoring this fn in this version of WINTERP.
 *
 * XmFontList XmFontListCopy (fontlist)
 *     XmFontList fontlist;
 ******************************************************************************/


/******************************************************************************
 * (XM_STRING_CONCAT <xmstring_a> <xmstring_b>)
 *	--> returns a new XMSTRING  which is the concatenation of XMSTRINGS
 *	    <xmstring_a> and <xmstring_b>.
 *
 * XmString XmStringConcat (a, b)
 *     XmString a;
 *     XmString b;
 ******************************************************************************/
LVAL Wxms_Prim_XM_STRING_CONCAT()
{
  XmString xms_a = get_xmstring(xlga_xmstring());
  XmString xms_b = get_xmstring(xlga_xmstring());
  xllastarg();
  return (cv_xmstring(XmStringConcat(xms_a, xms_b)));
}


/******************************************************************************
 * I'm ignoring this one for now since it looks useless and low-level.
 *
 * XmString XmStringNConcat (a, b, n)
 *     XmString a, b;
 *     int n;
 ******************************************************************************/


/******************************************************************************
 * (XM_STRING_COPY <xmstring>)
 *	--> returns a new XMSTRING, a copy of <xmstring>
 *
 * XmString XmStringCopy (string)
 *     XmString string;
 ******************************************************************************/
LVAL Wxms_Prim_XM_STRING_COPY()
{
  XmString xms = get_xmstring(xlga_xmstring());
  xllastarg();
  return (cv_xmstring(XmStringCopy(xms)));
}


/******************************************************************************
 * I'm ignoring this one for now since it looks useless and low-level.
 *
 * XmString XmStringNCopy (a, n)
 *     XmString a;
 *     int n;
 ******************************************************************************/


/******************************************************************************
 * (XM_STRING_BYTE_COMPARE <xmstring_a> <xmxtring_b>)
 *	--> returns T if the two XMSTRING args are byte-for-byte equal, else
 *	    NIL.
 *
 * Note that this function will return NIL for an XMSTRING that is retrieved
 * from a widget via :get_values, a callback, or a method. It's pretty useless, 
 * just like all the other XmString bogosities.
 *
 * Boolean XmStringByteCompare (a, b)
 *     XmString a, b;
 ******************************************************************************/
LVAL Wxms_Prim_XM_STRING_BYTE_COMPARE()
{
  extern LVAL true;
  XmString xms_a = get_xmstring(xlga_xmstring());
  XmString xms_b = get_xmstring(xlga_xmstring());
  xllastarg();
  return (XmStringByteCompare(xms_a, xms_b) ? true : NIL);
}


/******************************************************************************
 * (XM_STRING_COMPARE <xmstring_a> <xmxtring_b>)
 *	--> returns T if the two XMSTRING args are "semantically equal", 
 *	    else NIL.
 *
 * Boolean XmStringCompare (a, b)
 *     XmString a, b;
 ******************************************************************************/
LVAL Wxms_Prim_XM_STRING_COMPARE()
{
  extern LVAL true;
  XmString xms_a = get_xmstring(xlga_xmstring());
  XmString xms_b = get_xmstring(xlga_xmstring());
  xllastarg();
  return (XmStringCompare(xms_a, xms_b) ? true : NIL);
}


/******************************************************************************
 * (XM_STRING_LENGTH <xmstring>)
 *	--> returns a FIXNUM representing the number of bytes in <xmstring>
 *
 * int XmStringLength (string)
 *     XmString string;
 ******************************************************************************/
LVAL Wxms_Prim_XM_STRING_LENGTH()
{
  XmString xms = get_xmstring(xlga_xmstring());
  xllastarg();
  return (cvfixnum((FIXTYPE) XmStringLength(xms)));
}


/******************************************************************************
 * (XM_STRING_EMPTY <xmstring>)
 *	--> returns T if all segments in XMSTRING <xmstring> are zero length
 *	    or if there are no text segments. Otherwiser returns NIL.
 *
 * Boolean XmStringEmpty (string)
 *     XmString string;
 ******************************************************************************/
LVAL Wxms_Prim_XM_STRING_EMPTY()
{
  extern LVAL true;
  XmString xms = get_xmstring(xlga_xmstring());
  xllastarg();
  return (XmStringEmpty(xms) ? true : NIL);
}

#ifdef WINTERP_MOTIF_11
/******************************************************************************
 * (XM_STRING_HAS_SUBSTRING <xmstring> <xmsubstring>)
 *	--> returns T if <xmsubstring> is a substring of <xmstring>;
 *	    both argumetns are XmStrings. Otherwise, returns NIL.
 *
 * Boolean XmStringHasSubstring( XmString string , XmString substring );
 *
 ******************************************************************************/
LVAL Wxms_Prim_XM_STRING_HAS_SUBSTRING()
{
  extern LVAL true;
  XmString xmstring    = get_xmstring(xlga_xmstring());
  XmString xmsubstring = get_xmstring(xlga_xmstring());
  xllastarg();

  return (XmStringHasSubstring(xmstring, xmsubstring) ? true : NIL);
}
#endif				/* WINTERP_MOTIF_11 */


/******************************************************************************
 * Don't need to interface this -- XMSTRINGS are freed by garbage collector.
 *
 * void XmStringFree (string)
 *     XmString string;
 ******************************************************************************/
/******************************************************************************
 * Add this when I add the type XmFontList to winterp.
 *
 * Dimension XmStringBaseline (fontlist, string)
 *     XmFontList fontlist;
 *     XmString string;
 ******************************************************************************/
/******************************************************************************
 * Add this when I add the type XmFontList to winterp.
 *
 * Dimension XmStringWidth (fontlist, string)
 *     XmFontList fontlist;
 *     XmString string;
 ******************************************************************************/
/******************************************************************************
 * Add this when I add the type XmFontList to winterp.
 *
 * Dimension XmStringHeight (fontlist, string)
 *     XmFontList fontlist;
 *     XmString string;
 ******************************************************************************/
/******************************************************************************
 * Add this when I add the type XmFontList to winterp.
 *
 * void XmStringExtent (fontlist, string, width, height)
 *     XmFontList fontlist;
 *     XmString string;
 *     Dimension *width, *height;
 ******************************************************************************/


/******************************************************************************
 * (XM_STRING_LINE_COUNT <xmstring>)
 *	--> returns a FIXNUM representing the number of lines
 *          in XMSTRING <xmstring>.
 *
 * int XmStringLineCount (string)
 *     XmString string;
 ******************************************************************************/
LVAL Wxms_Prim_XM_STRING_LINE_COUNT()
{
  XmString xms = get_xmstring(xlga_xmstring());
  xllastarg();
  return (cvfixnum((FIXTYPE) XmStringLineCount(xms)));
}


/******************************************************************************
 * Ignore -- too low-level.

 * void XmStringDraw (d, w, fontlist, string, gc, x, y, width, align, lay_dir, clip)
 *     Display	*d;
 *     Window 	w;
 *     XmFontList fontlist;
 *     XmString	string;
 *     GC		gc;
 *     Position	x, y;
 *     Dimension	width;
 *     UChar	align;
 *     UChar	lay_dir;
 *     XRectangle	*clip;
 ******************************************************************************/
/******************************************************************************
 * Ignore -- too low-level
 * void XmStringDrawImage (d, w, fontlist, string, gc, x, y, 
 * 	width, align, lay_dir, clip)
 *     Display	*d;
 *     Window 	w;
 *     XmFontList fontlist;
 *     XmString	string;
 *     GC		gc;
 *     Position	x, y;
 *     Dimension	width;
 *     UChar	align;
 *     UChar	lay_dir;
 *     XRectangle	*clip;
 ******************************************************************************/
/******************************************************************************
 * Ignore -- too low-level
 *
 * void XmStringDrawUnderline (d, w, fontlist, string, gc, x, y, 
 * 	width, align, lay_dir, clip, underline)
 *     Display	*d;
 *     Window 	w;
 *     XmFontList fontlist;
 *     XmString	string;
 *     GC		gc;
 *     Position	x, y;
 *     Dimension	width;
 *     UChar	align;
 *     UChar	lay_dir;
 *     XRectangle	*clip;
 *     XmString	underline;
 ******************************************************************************/


#ifdef WINTERP_MOTIF_11
/******************************************************************************
 * (XM_CVT_CT_TO_XM_STRING  <STRING>)
 *	--> returns an XMSTRING.
 *
 * "a compound string function that converts compound text to a compound string."
 * <STRING> is a string...
 *----------------------------------------------------------------------------
 *           XmString XmCvtCTToXmString (text)
 *                char        * text;
 ******************************************************************************/
LVAL Wxms_Prim_XM_CVT_CT_TO_XM_STRING()
{
  char* string = (char*) getstring(xlgastring());
  xllastarg();
  
  return (cv_xmstring(XmCvtCTToXmString(string)));
}
#endif				/* WINTERP_MOTIF_11 */


#ifdef WINTERP_MOTIF_11
/******************************************************************************
 * (XM_CVT_XM_STRING_TO_CT  <XMSTRING>)
 *	--> returns a STRING.
 *
 * "a compound string function that converts a compound string to compound text."
 * <XMSTRING> is an XMSTRING. 
 *
 *----------------------------------------------------------------------------
 *           char * XmCvtXmStringToCT (string)
 *                XmString    string;
 ******************************************************************************/
LVAL Wxms_Prim_XM_CVT_XM_STRING_TO_CT()
{
  XmString xmstring = get_xmstring(xlga_xmstring());
  xllastarg();
  
  return (cv_string(XmCvtXmStringToCT(xmstring))); /* cv_string() doesn't copy string returned by XmCvtXmStringToCT()... the string will be freed by garbage collection when appropriate... */
}
#endif				/* WINTERP_MOTIF_11 */


/*****************************************************************************
 * Initialization proc.
 ****************************************************************************/
Wxms_Init()
{
  s_XmSTRING_DIRECTION_L_TO_R = xlenter(":STRING_DIRECTION_L_TO_R");
  s_XmSTRING_DIRECTION_R_TO_L = xlenter(":STRING_DIRECTION_R_TO_L");
}
