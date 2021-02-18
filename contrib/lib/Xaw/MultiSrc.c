/* $XConsortium: MultiSrc.c,v 1.55 91/07/25 18:09:27 rws Exp $ */

/*
 * Copyright 1991 by OMRON Corporation
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON and MIT not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON and MIT make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * OMRON AND MIT DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL OMRON OR MIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 *      Author: Li Yuhong	 OMRON Corporation
 */

/*
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Chris Peterson, MIT X Consortium.
 *
 * Much code taken from X11R3 String and Disk Sources.
 */

/*
 * MultiSrc.c - MultiSrc object. (For use with the text widget).
 *
 */

#include <X11/IntrinsicP.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/Xfuncs.h>
#include <X11/Xaw/XawInit.h>
#include <X11/Xaw/MultiSrcP.h>
#include <X11/Xmu/Misc.h>
#include <X11/Xmu/CharSet.h>
#include <X11/wchar.h>		/* for wchar operations */


/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static int magic_value = MAGIC_VALUE;

#define offset(field) XtOffsetOf(MultiSrcRec, multi_src.field)

static XtResource resources[] = {
    {XtNstring, XtCString, XtRString, sizeof (XtPointer),
       offset(string), XtRPointer, NULL},
    {XtNtype, XtCType, XtRMultiType, sizeof (XawMultiType),
       offset(type), XtRImmediate, (XtPointer)XawMultiString},
    /* not used. */
    {XtNdataCompression, XtCDataCompression, XtRBoolean, sizeof (Boolean),
       offset(data_compression), XtRImmediate, (XtPointer) FALSE},
    {XtNpieceSize, XtCPieceSize, XtRInt, sizeof (XawTextPosition),
       offset(piece_size), XtRImmediate, (XtPointer) BUFSIZ},
    {XtNcallback, XtCCallback, XtRCallback, sizeof(XtPointer), 
       offset(callback), XtRCallback, (XtPointer)NULL},
    {XtNuseStringInPlace, XtCUseStringInPlace, XtRBoolean, sizeof (Boolean),
       offset(use_string_in_place), XtRImmediate, (XtPointer) FALSE},
    {XtNlength, XtCLength, XtRInt, sizeof (int),
       offset(multi_length), XtRInt, (XtPointer) &magic_value},

};
#undef offset

static XawTextPosition Scan(), Search(), ReadText();
static int ReplaceText();
static MultiPiece * FindPiece(), * AllocNewPiece();
static FILE * InitStringOrFile();
static void FreeAllPieces(), RemovePiece(), BreakPiece(), LoadPieces();
static void RemoveOldStringOrFile(),  CvtStringToMultiType();
static void ClassInitialize(), Initialize(), Destroy(), GetValuesHook();
static String StorePiecesInString();
static Boolean SetValues(), WriteToFile();
static wchar *MyWStrncpy();
static int Input();
static XtPointer NewString();

extern char *tmpnam();
extern int errno, sys_nerr;
extern char* sys_errlist[];

extern wchar * _XawTextMBToWC();
extern char *_XawTextWCToMB();

#define superclass		(&textSrcClassRec)
MultiSrcClassRec multiSrcClassRec = {
  {
/* core_class fields */	
    /* superclass	  	*/	(WidgetClass) superclass,
    /* class_name	  	*/	"MultiSrc",
    /* widget_size	  	*/	sizeof(MultiSrcRec),
    /* class_initialize   	*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize		  	*/	NULL,
    /* actions		  	*/	NULL,
    /* num_actions	  	*/	0,
    /* resources	  	*/	resources,
    /* num_resources	  	*/	XtNumber(resources),
    /* xrm_class	  	*/	NULLQUARK,
    /* compress_motion	  	*/	FALSE,
    /* compress_exposure  	*/	FALSE,
    /* compress_enterleave	*/	FALSE,
    /* visible_interest	  	*/	FALSE,
    /* destroy		  	*/	Destroy,
    /* resize		  	*/	NULL,
    /* expose		  	*/	NULL,
    /* set_values	  	*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	NULL,
    /* get_values_hook		*/	GetValuesHook,
    /* accept_focus	 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* tm_table		   	*/	NULL,
    /* query_geometry		*/	NULL,
    /* display_accelerator	*/	NULL,
    /* extension		*/	NULL
  },
/* textSrc_class fields */
  {
    /* Input                    */      Input,
    /* Read                     */      ReadText,
    /* Replace                  */      ReplaceText,
    /* Scan                     */      Scan,
    /* Search                   */      Search,
    /* SetSelection             */      XtInheritSetSelection,
    /* ConvertSelection         */      XtInheritConvertSelection
  },
/* multiSrc_class fields */
  {
    /* Keep the compiler happy */       NULL
  }
};

WidgetClass multiSrcObjectClass = (WidgetClass)&multiSrcClassRec;

/************************************************************
 *
 * Semi-Public Interfaces.
 *
 ************************************************************/

/*      Function Name: ClassInitialize
 *      Description: Class Initialize routine, called only once.
 *      Arguments: none.
 *      Returns: none.
 */

static void
ClassInitialize()
{
  XawInitializeWidgetSet();
  XtAddConverter( XtRString, XtRMultiType, CvtStringToMultiType,
		 NULL, (Cardinal) 0);
}

/*      Function Name: Initialize
 *      Description: Initializes the simple menu widget
 *      Arguments: request - the widget requested by the argument list.
 *                 new     - the new widget with both resource and non
 *                           resource values.
 *      Returns: none.
 */

/* ARGSUSED */
static void
Initialize(request, new)
Widget request, new;
{
  MultiSrcObject src = (MultiSrcObject) new;
  FILE * file;

/*
 * Set correct flags (override resources) depending upon widget class.
 */

  src->multi_src.changes = FALSE;
  src->multi_src.allocated_string = FALSE;

  file = InitStringOrFile(src, src->multi_src.type == XawMultiFile);
  LoadPieces(src, file, NULL);

  if (file != NULL) fclose(file);
  src->text_src.text_format = FMT32BIT;

}

/*
 * Input():
 *   connect to X Input Method for inputting multiple languages.
 *   w  --   MultiSrc object.
 *
 * by Li Yuhong, 03/29/1991
 */

static int
Input(w, event, buffer, num_bytes, keysym, status)
Widget w;
XEvent *event;
char *buffer;
int num_bytes;
KeySym *keysym;
XComposeStatus *status;
{
#if defined(IM)
  return XawImWcLookupString(w->core.parent, &event->xkey, (wchar_t *)buffer,
			     num_bytes, keysym, status);
#else	/* defined(IM) */
  return XLookupString(&event->xkey, buffer, num_bytes,
			keysym, status);
#endif	/* defined(IM) */
}

/*	Function Name: ReadText
 *	Description: This function reads the source.
 *	Arguments: w - the MultiSource widget.
 *                 pos - position of the text to retreive.
 * RETURNED        text - text block that will contain returned text.
 *                 length - maximum number of characters to read.
 *	Returns: The number of characters read into the buffer.
 */

static XawTextPosition
ReadText(w, pos, text, length)
Widget w;
XawTextPosition pos;
XawTextBlock *text;	
int length;		
{
  MultiSrcObject src = (MultiSrcObject) w;
  XawTextPosition count, start;
  MultiPiece * piece = FindPiece(src, pos, &start);
    
  text->format = FMT32BIT;
  text->firstPos = pos;
  text->ptr = (char *)(piece->text + (pos - start));
  count = piece->used - (pos - start);
  text->length = (length > count) ? count : length;
  return(pos + text->length);
}

/*	Function Name: ReplaceText.
 *	Description: Replaces a block of text with new text.
 *	Arguments: w - the MultiSource widget.
 *                 startPos, endPos - ends of text that will be removed.
 *                 text - new text to be inserted into buffer at startPos.
 *	Returns: XawEditError or XawEditDone.
 */

/*ARGSUSED*/
static int 
ReplaceText (w, startPos, endPos, text)
Widget w;
XawTextPosition startPos, endPos;
XawTextBlock *text;
{
  MultiSrcObject src = (MultiSrcObject) w;
  MultiPiece *start_piece, *end_piece, *temp_piece;
  XawTextPosition start_first, end_first;
  int length, firstPos;
  wchar *wptr;

/*
 * Editing a read only source is not allowed.
 */

  if (src->text_src.edit_mode == XawtextRead) 
    return(XawEditError);

  start_piece = FindPiece(src, startPos, &start_first);
  end_piece = FindPiece(src, endPos, &end_first);

  src->multi_src.changes = TRUE; /* We have changed the buffer. */

/* 
 * Remove Old Stuff. 
 */

  if (start_piece != end_piece) {
    temp_piece = start_piece->next;

/*
 * If empty and not the only piece then remove it. 
 */

    if ( ((start_piece->used = startPos - start_first) == 0) &&
	 !((start_piece->next == NULL) && (start_piece->prev == NULL)) )
      RemovePiece(src, start_piece);

    while (temp_piece != end_piece) {
      temp_piece = temp_piece->next;
      RemovePiece(src, temp_piece->prev);
    }
    end_piece->used -= endPos - end_first;
    if (end_piece->used != 0)
      MyWStrncpy(end_piece->text, (end_piece->text + endPos - end_first),
		(int) end_piece->used);
  }
  else {			/* We are fully in one piece. */
    if ( (start_piece->used -= endPos - startPos) == 0) {
      if ( !((start_piece->next == NULL) && (start_piece->prev == NULL)) )
	RemovePiece(src, start_piece);
    }
    else {
      MyWStrncpy(start_piece->text + (startPos - start_first),
		start_piece->text + (endPos - start_first),
		(int) (start_piece->used - (startPos - start_first)) );
      if ( src->multi_src.use_string_in_place && 
	   ((src->multi_src.length - (endPos - startPos)) < 
	    (src->multi_src.piece_size - 1)) ) 
	start_piece->text[src->multi_src.length - (endPos - startPos)] = WNULL;
    }
  }

  src->multi_src.length += -(endPos - startPos) + text->length;

  if ( text->length != 0) {

    /* 
     * Put in the New Stuff.
     */
    
    start_piece = FindPiece(src, startPos, &start_first);
    
    length = text->length;
    firstPos = text->firstPos;
    
    while (length > 0) {
      wchar * ptr;
      int fill;
      
      if (src->multi_src.use_string_in_place) {
	if (start_piece->used == (src->multi_src.piece_size - 1)) {
          /*
           * The string is used in place, then the string
           * is not allowed to grow.
           */
          start_piece->used = src->multi_src.length =
                                                 src->multi_src.piece_size - 1;
          start_piece->text[src->multi_src.length] = WNULL;
	  return(XawEditError);
	}
      }


      if (start_piece->used == src->multi_src.piece_size) {
	BreakPiece(src, start_piece);
	start_piece = FindPiece(src, startPos, &start_first);
      }

      fill = Min((int)(src->multi_src.piece_size - start_piece->used), length);
      
      ptr = start_piece->text + (startPos - start_first);
      MyWStrncpy(ptr + fill, ptr, 
		(int) start_piece->used - (startPos - start_first));
      wptr =(wchar *)text->ptr;
      (void)wcsncpy(ptr, wptr + firstPos, fill);
      
      startPos += fill;
      firstPos += fill;
      start_piece->used += fill;
      length -= fill;
    }
  }

  if (src->multi_src.use_string_in_place)
    start_piece->text[start_piece->used] = WNULL;

  XtCallCallbacks(w, XtNcallback, NULL); /* Call callbacks, we have changed 
					    the buffer. */

  return(XawEditDone);
}

/*	Function Name: Scan
 *	Description: Scans the text source for the number and type
 *                   of item specified.
 *	Arguments: w - the MultiSource widget.
 *                 position - the position to start scanning.
 *                 type - type of thing to scan for.
 *                 dir - direction to scan.
 *                 count - which occurance if this thing to search for.
 *                 include - whether or not to include the character found in
 *                           the position that is returned. 
 *	Returns: the position of the item found.
 *
 * Note: While there are only 'n' characters in the file there are n+1 
 *       possible cursor positions (one before the first character and
 *       one after the last character.
 */

static 
XawTextPosition 
Scan (w, position, type, dir, count, include)
Widget                w;
XawTextPosition       position;
XawTextScanType       type;
XawTextScanDirection  dir;
int     	      count;
Boolean	              include;
{
  MultiSrcObject src = (MultiSrcObject) w;
  register int inc;
  MultiPiece * piece;
  XawTextPosition first, first_eol_position;
  register wchar * ptr;

  if (type == XawstAll) {	/* Optomize this common case. */
    if (dir == XawsdRight)
      return(src->multi_src.length);
    return(0);			/* else. */
  }

  if (position > src->multi_src.length)
    position = src->multi_src.length;

  if ( dir == XawsdRight ) {
    if (position == src->multi_src.length)
/*
 * Scanning right from src->multi_src.length???
 */
      return(src->multi_src.length);
    inc = 1;
  }
  else {
    if (position == 0)
      return(0);		/* Scanning left from 0??? */
    inc = -1;
    position--;
  }

  piece = FindPiece(src, position, &first);

/*
 * If the buffer is empty then return 0. 
 */

  if ( piece->used == 0 ) return(0); 

  ptr = (position - first) + piece->text;

  switch (type) {
  case XawstEOL: 
  case XawstParagraph: 
  case XawstWhiteSpace: 
    for ( ; count > 0 ; count-- ) {
      Boolean non_space = FALSE, first_eol = TRUE;
      while (TRUE) {
        register wchar c = *ptr;

	ptr += inc;
	position += inc;
	
	if (type == XawstWhiteSpace) {
	  if (iswspace(c)) {
	    if (non_space) 
	      break;
	  }
	  else
	    non_space = TRUE;
	}
	else if (type == XawstEOL) {
          if (c == atowc('\n')) break;
	}
	else { /* XawstParagraph */
	  if (first_eol) {
            if (c == atowc('\n')) {
	      first_eol_position = position;
	      first_eol = FALSE;
	    }
	  }
	  else
            if ( c == atowc('\n'))
              break;
            else if ( !iswspace(c) )
	      first_eol = TRUE;
	}
	      

	if ( ptr < piece->text ) {
	  piece = piece->prev;
	  if (piece == NULL)	/* Begining of text. */
	    return(0);
	  ptr = piece->text + piece->used - 1;
	}
	else if ( ptr >= (piece->text + piece->used) ) {
	  piece = piece->next;
	  if (piece == NULL)	/* End of text. */
	    return(src->multi_src.length);
	  ptr = piece->text;
	}
      }
    }
    if (!include) {
      if ( type == XawstParagraph)
	position = first_eol_position;
      position -= inc;
    }
    break;
  case XawstPositions: 
    position += count * inc;
    break;
/*  case XawstAll:		---- handled in special code above */
  }

  if ( dir == XawsdLeft )
    position++;

  if (position >= src->multi_src.length)
    return(src->multi_src.length);
  if (position < 0)
    return(0);

  return(position);
}

/*	Function Name: Search
 *	Description: Searchs the text source for the text block passed
 *	Arguments: w - the MultiSource Widget.
 *                 position - the position to start scanning.
 *                 dir - direction to scan.
 *                 text - the text block to search for.
 *	Returns: the position of the item found.
 */

static XawTextPosition 
Search(w, position, dir, text)
Widget                w;
XawTextPosition       position;
XawTextScanDirection  dir;
XawTextBlock *        text;
{
  MultiSrcObject src = (MultiSrcObject) w;
  register int inc, count = 0;
  register wchar * ptr;
  wchar * wptr;
  MultiPiece * piece;
  wchar * buf;
  XawTextPosition first;

  if ( dir == XawsdRight )
    inc = 1;
  else {
    inc = -1;
    if (position == 0)
      return(XawTextSearchError);	/* scanning left from 0??? */
    position--;
  }

  buf = (wchar *)XtMalloc(sizeof(wchar) * text->length);
  wptr = (wchar *)text->ptr;
  (void)wcsncpy(buf, (wptr + text->firstPos), text->length);
  piece = FindPiece(src, position, &first);
  ptr = (position - first) + piece->text;

  while (TRUE) {
    if (*ptr == ((dir == XawsdRight) ? *(buf + count) 
		                     : *(buf + text->length - count - 1)) ) {
      if (count == (text->length - 1))
	break;
      else
	count++;
    }
    else {
      if (count != 0) {
	position -=inc * count;
	ptr -= inc * count;
      }
      count = 0;
    }

    ptr += inc;
    position += inc;
    
    while ( ptr < piece->text ) {
      piece = piece->prev;
      if (piece == NULL) {	/* Begining of text. */
	XtFree((char *)buf);
	return(XawTextSearchError);
      }
      ptr = piece->text + piece->used - 1;
    }
   
    while ( ptr >= (piece->text + piece->used) ) {
      piece = piece->next;
      if (piece == NULL) {	/* End of text. */
	XtFree((char *)buf);
	return(XawTextSearchError);
      }
      ptr = piece->text;
    }
  }

  XtFree((char *)buf);
  if (dir == XawsdLeft)
    return(position);
  return(position - (text->length - 1));
}

/*	Function Name: SetValues
 *	Description: Sets the values for the MultiSource.
 *	Arguments: current - current state of the widget.
 *                 request - what was requested.
 *                 new - what the widget will become.
 *	Returns: True if redisplay is needed.
 */

/* ARGSUSED */
static Boolean
SetValues(current, request, new, args, num_args)
Widget current, request, new;
ArgList args;
Cardinal * num_args;
{
  MultiSrcObject src =      (MultiSrcObject) new;
  MultiSrcObject old_src = (MultiSrcObject) current;
  Boolean total_reset = FALSE, string_set = FALSE;
  FILE * file;
  int i;

  if ( old_src->text_src.code != src->text_src.code) {
      XtAppWarning( XtWidgetToApplicationContext(new),
           "MultiSrc: The XtNcode resources can not be changed.");
  }
  if ( old_src->multi_src.use_string_in_place != 
       src->multi_src.use_string_in_place ) {
      XtAppWarning( XtWidgetToApplicationContext(new),
	   "MultiSrc: The XtNuseStringInPlace resources may not be changed.");
       src->multi_src.use_string_in_place = 
	   old_src->multi_src.use_string_in_place;
  }

  for (i = 0; i < *num_args ; i++ ) 
      if (streq(args[i].name, XtNstring)) {
	  string_set = TRUE;
	  break;
      }
  
  if ( string_set || (old_src->multi_src.type != src->multi_src.type) ) {
    RemoveOldStringOrFile(old_src, string_set); /* remove old info. */
    file = InitStringOrFile(src, string_set);	/* Init new info. */
    LoadPieces(src, file, NULL);    /* load new info into internal buffers. */
    if (file != NULL) fclose(file);
    XawTextSetSource( XtParent(new), new, 0);   /* Tell text widget
						   what happened. */
    total_reset = TRUE;
  }

  if ( old_src->multi_src.multi_length != src->multi_src.multi_length ) 
      src->multi_src.piece_size = src->multi_src.multi_length;

  if ( !total_reset && 
      (old_src->multi_src.piece_size != src->multi_src.piece_size) ) {
      String string = StorePiecesInString(old_src);
      FreeAllPieces(old_src);
      LoadPieces(src, NULL, string);
      XtFree(string);
  }

  return(FALSE);
}

/*	Function Name: GetValuesHook
 *	Description: This is a get values hook routine that sets the
 *                   values specific to the multi source.
 *	Arguments: w - the MultiSource Widget.
 *                 args - the argument list.
 *                 num_args - the number of args.
 *	Returns: none.
 */

static void
GetValuesHook(w, args, num_args)
Widget w;
ArgList args;
Cardinal * num_args;
{
  MultiSrcObject src = (MultiSrcObject) w;
  register int i;

  if (src->multi_src.type == XawMultiString) {
    for (i = 0; i < *num_args ; i++ ) 
      if (streq(args[i].name, XtNstring)) {
	  if (src->multi_src.use_string_in_place) {
              *((char **) args[i].value) = (char *)
					src->multi_src.first_piece->text;
	  }
	  else {
	      if (XawMultiSave(w))	/* If save sucessful. */
		  *((char **) args[i].value) = src->multi_src.string;
	  }
	break;
      }
  }
}    

/*	Function Name: Destroy
 *	Description: Destroys an multi source (frees all data)
 *	Arguments: src - the Multi source Widget to free.
 *	Returns: none.
 */

static void 
Destroy (w)
Widget w;
{
  RemoveOldStringOrFile((MultiSrcObject) w, True);
/*
  DestroyIM(w);
*/
}

/************************************************************
 *
 * Public routines 
 *
 ************************************************************/

/*	Function Name: XawMultiSourceFreeString
 *	Description: Frees the string returned by a get values call
 *                   on the string when the source is of type string.
 *	Arguments: w - the MultiSrc widget.
 *	Returns: none.
 */

void
#if NeedFunctionPrototypes
XawMultiSourceFreeString(Widget w)
#else
XawMultiSourceFreeString(w)
Widget w;
#endif
{
  MultiSrcObject src = (MultiSrcObject) w;

  if (src->multi_src.allocated_string && src->multi_src.type != XawMultiFile) {
    src->multi_src.allocated_string = FALSE;
    XtFree(src->multi_src.string);
    src->multi_src.string = NULL;
  }
}

/*	Function Name: XawMultiSave
 *	Description: Saves all the pieces into a file or string as required.
 *	Arguments: w - the multiSrc Widget.
 *	Returns: TRUE if the save was successful.
 */

Boolean
#if NeedFunctionPrototypes
XawMultiSave(Widget w)
#else
XawMultiSave(w)
Widget w;
#endif
{
  MultiSrcObject src = (MultiSrcObject) w;

/*
 * If using the string in place then there is no need to play games
 * to get the internal info into a readable string.
 */

  if (src->multi_src.use_string_in_place) 
    return(TRUE);

  if (src->multi_src.type == XawMultiFile) {
    char * string;

    if (!src->multi_src.changes) 		/* No changes to save. */
      return(TRUE);

    string = StorePiecesInString(src);

    if (WriteToFile(string, src->multi_src.string) == FALSE) {
      XtFree(string);
      return(FALSE);
    }
    XtFree(string);
  }
  else {			/* This is a string widget. */
    if (src->multi_src.allocated_string == TRUE) 
      XtFree(src->multi_src.string);
    else
      src->multi_src.allocated_string = TRUE;
    
    src->multi_src.string = StorePiecesInString(src);
  }
  src->multi_src.changes = FALSE;
  return(TRUE);
}

/*	Function Name: XawMultiSaveAsFile
 *	Description: Save the current buffer as a file.
 *	Arguments: w - the MultiSrc widget.
 *                 name - name of the file to save this file into.
 *	Returns: True if the save was sucessful.
 */

Boolean
#if NeedFunctionPrototypes
XawMultiSaveAsFile(Widget w, _Xconst char* name)
#else
XawMultiSaveAsFile(w, name)
Widget w;
String name;
#endif
{
  MultiSrcObject src = (MultiSrcObject) w;
  String string;
  Boolean ret;

  string = StorePiecesInString(src); 

  ret = WriteToFile(string, name);
  XtFree(string);
  return(ret);
}

/*	Function Name: XawMultiSourceChanged
 *	Description: Returns true if the source has changed since last saved.
 *	Arguments: w - the multi source widget.
 *	Returns: a Boolean (see description).
 */

Boolean 
#if NeedFunctionPrototypes
XawMultiSourceChanged(Widget w)
#else
XawMultiSourceChanged(w)
Widget w;
#endif
{
  return( ((MultiSrcObject) w)->multi_src.changes );
}
  
/************************************************************
 *
 * Private Functions.
 *
 ************************************************************/

static void
RemoveOldStringOrFile(src, checkString) 
MultiSrcObject src;
Boolean checkString;
{
  FreeAllPieces(src);

  if (checkString && src->multi_src.allocated_string) {
    XtFree(src->multi_src.string);
    src->multi_src.allocated_string = False;
    src->multi_src.string = NULL;
  }
}

/*	Function Name: WriteToFile
 *	Description: Write the string specified to the begining of the file
 *                   specified.
 *	Arguments: string - string to write.
 *                 name - the name of the file
 *	Returns: returns TRUE if sucessful, FALSE otherwise.
 */

static Boolean
WriteToFile(string, name)
String string, name;
{
  int fd;
  
  if ( ((fd = creat(name, 0666)) == -1 ) ||
       (write(fd, string, sizeof(unsigned char) * strlen(string)) == -1) )
    return(FALSE);

  if ( close(fd) == -1 ) 
    return(FALSE);

  return(TRUE);
}

/*	Function Name: StorePiecesInString
 *	Description: store the pieces in memory into a char string.
 *	Arguments: data - the multi pointer data.
 *	Returns: char *string. 
 */

static String
StorePiecesInString(src)
MultiSrcObject src;
{
  wchar *string;
  XawTextPosition first;
  MultiPiece * piece;

  string = (wchar *)XtMalloc((src->multi_src.length + 1) * sizeof(wchar));

  for (first = 0, piece = src->multi_src.first_piece ; piece != NULL;
       first += piece->used, piece = piece->next)
    (void) wcsncpy(string + first, piece->text, piece->used);

  string[src->multi_src.length] = WNULL; /* WNULL terminate this sucker. */


/*
 * This will refill all pieces to capacity.
 */

  if (src->multi_src.data_compression) {
    /* The "string" already was encoded in WC char. */
    XawTextCode savecode = src->text_src.code;
    src->text_src.code = XawTextWC;
    FreeAllPieces(src);
    LoadPieces(src, NULL, (char *)string);
    src->text_src.code = savecode;

  }

  if (src->text_src.code == XawTextWC) {
    return((char *)string);
  } else {
    char *str;
    int len = src->multi_src.length;

    str = _XawTextWCToMB(XtDisplayOfObject((Widget)src), src->text_src.code, string, &len);
    XtFree((char *)string);
    return(str);
  }

}

/*	Function Name: InitStringOrFile.
 *	Description: Initializes the string or file.
 *	Arguments: src - the MultiSource.
 *	Returns: none - May exit though.
 */

static FILE *
InitStringOrFile(src, newString)
MultiSrcObject src;
Boolean newString;
{
    char * open_mode;
    FILE * file;
    char fileName[TMPSIZ];
    int bytes = sizeof(wchar);

#define StrLen(s, ccs)	((ccs == XawTextWC)? wcslen((wchar *)s): \
					     strlen((char *)s)) 
    if (src->multi_src.type == XawMultiString) {

	if (src->multi_src.string == NULL)
	    src->multi_src.length = 0;

	else if (! src->multi_src.use_string_in_place) {
	    src->multi_src.string = NewString(src->multi_src.string,
						src->text_src.code);
	    src->multi_src.allocated_string = True;
	    src->multi_src.length = StrLen(src->multi_src.string,
					src->text_src.code);
	}
	
	if (src->multi_src.use_string_in_place) {
	    /* Actually only encoding of XawTextWC is allowed to go here. */
	    src->multi_src.length = StrLen(src->multi_src.string,
					src->text_src.code);
	    /* In case the length resource is incorrectly set */
	    if (src->multi_src.length > src->multi_src.multi_length)
		src->multi_src.multi_length = src->multi_src.length;

	    if (src->multi_src.multi_length == MAGIC_VALUE) 
		src->multi_src.piece_size = src->multi_src.length;
	    else
		src->multi_src.piece_size = src->multi_src.multi_length + 1;
	}
		
	return(NULL);
    }

/*
 * type is XawMultiFile.
 */
    
    src->multi_src.is_tempfile = FALSE;

    switch (src->text_src.edit_mode) {
    case XawtextRead:
	if (src->multi_src.string == NULL)
	    XtErrorMsg("NoFile", "multiSourceCreate", "XawError",
		     "Creating a read only disk widget and no file specified.",
		       NULL, 0);
	open_mode = "r";
	break;
    case XawtextAppend:
    case XawtextEdit:
	if (src->multi_src.string == NULL) {
	    src->multi_src.string = fileName;
	    (void) tmpnam(src->multi_src.string);
	    src->multi_src.is_tempfile = TRUE;
	    open_mode = "w";
	} else
	    open_mode = "r+";
	break;
    default:
	XtErrorMsg("badMode", "multiSourceCreate", "XawError",
		"Bad editMode for multi source; must be Read, Append or Edit.",
		   NULL, NULL);
    }

    /* Allocate new memory for the temp filename, because it is held in
     * a stack variable, not static memory.  This widget does not need
     * to keep the private state field is_tempfile -- it is only accessed
     * in this routine, and its former setting is unused.
     */
    if (newString || src->multi_src.is_tempfile) {
	src->multi_src.string = XtNewString(src->multi_src.string);
	src->multi_src.allocated_string = TRUE;
    }
    
    if (!src->multi_src.is_tempfile) {
	if ((file = fopen(src->multi_src.string, open_mode)) != 0) {
	    (void) fseek(file, 0L, 2);
	    /* 
	     * Bug:
	     * Suppose the i18n OS guarantees the number:
	     *       (ftell (file))/sizeof(wchar)
	     * is integer.
	     */
	    if (src->text_src.code == XawTextWC) {
		src->multi_src.length = (ftell (file))/sizeof(wchar);
	    } else {
		src->multi_src.length = (ftell (file));
	    }
	    return file;
	} else {
	    String params[2];
	    Cardinal num_params = 2;
	    char msg[11];
	    
	    params[0] = src->multi_src.string;
	    if (errno <= sys_nerr)
		params[1] = sys_errlist[errno];
	    else {
		sprintf(msg, "errno=%.4d", errno);
		params[1] = msg;
	    }
	    XtAppWarningMsg(XtWidgetToApplicationContext((Widget)src),
			    "openError", "multiSourceCreate", "XawWarning",
			    "Cannot open file %s; %s", params, &num_params);
	}
    } 
    src->multi_src.length = 0;
    return((FILE *)NULL);
#undef StrLen
}

static void
LoadPieces(src, file, string)
MultiSrcObject src;
FILE * file;
char * string;
{
  Display *d = XtDisplayOfObject((Widget)src);
  wchar *local_str, *ptr;
  register MultiPiece * piece = NULL;
  XawTextPosition left;
  int bytes = sizeof(wchar);

  if (string != NULL) {
    local_str = _XawTextMBToWC(d, src->text_src.code, (char *)string,
			   &(src->multi_src.length));
  } else if (src->multi_src.type != XawMultiFile) {
    if (src->multi_src.use_string_in_place && 
		src->text_src.code == XawTextWC)
      local_str = (wchar *)src->multi_src.string;
    else {
      if (src->multi_src.use_string_in_place && 
		src->text_src.code != XawTextWC)
	XtAppWarning(XtWidgetToApplicationContext((Widget)src),
	  "MultiSrc: The XtNuseStrinInPlace is restricted to XawTextWC encoding only.");
      local_str = _XawTextMBToWC(d, src->text_src.code, src->multi_src.string,
			     &(src->multi_src.length));
    }
  } else {
    char *str;
    int unit = ((src->text_src.code == XawTextWC)?
		sizeof(wchar): sizeof(unsigned char));
	str = XtMalloc((src->multi_src.length + 1) * unit);
	if (src->multi_src.length != 0) {
	    fseek(file, 0L, 0);
	    if (fread(str, unit, src->multi_src.length, file)
			!= src->multi_src.length) 
	      XtErrorMsg("readError", "multiSourceCreate", "XawError",
			 "fread returned error.", NULL, NULL);
	    local_str = _XawTextMBToWC(d, src->text_src.code, str, 
				   &(src->multi_src.length));
	    if (src->text_src.code != XawTextWC)
	      XtFree(str);
	} else {
	    str[src->multi_src.length] = '\0';
	    local_str = (wchar *)str;
	}
  }

/*
 * If we are using the string in place then set the other fields as follows:
 *
 * piece_size = length;
 * piece->used = src->multi_src.length;
 */
  
  if (src->multi_src.use_string_in_place) {
    piece = AllocNewPiece(src, piece);
    piece->used = Min(src->multi_src.length, src->multi_src.piece_size);
    piece->text = (wchar *)src->multi_src.string;
    return;
  }

  ptr = local_str;
  left = src->multi_src.length;

  do {
    piece = AllocNewPiece(src, piece);

    piece->text = (wchar *)XtMalloc(src->multi_src.piece_size * bytes);
    piece->used = Min(left, src->multi_src.piece_size);
    if (piece->used != 0)
      (void) wcsncpy(piece->text, ptr, piece->used);

    left -= piece->used;
    ptr += piece->used;
  } while (left > 0);

  if (!src->multi_src.use_string_in_place || src->text_src.code != XawTextWC)
    XtFree((char *)local_str);
}


/*	Function Name: AllocNewPiece
 *	Description: Allocates a new piece of memory.
 *	Arguments: src - The MultiSrc Widget.
 *                 prev - the piece just before this one, or NULL.
 *	Returns: the allocated piece.
 */

static MultiPiece *
AllocNewPiece(src, prev)
MultiSrcObject src;
MultiPiece * prev;
{
  MultiPiece * piece = XtNew(MultiPiece);

  if (prev == NULL) {
    src->multi_src.first_piece = piece;
    piece->next = NULL;
  }
  else {
    if (prev->next != NULL)
      (prev->next)->prev = piece;
    piece->next = prev->next;
    prev->next = piece;
  }
  
  piece->prev = prev;

  return(piece);
}

/*	Function Name: FreeAllPieces
 *	Description: Frees all the pieces
 *	Arguments: src - The MultiSrc Widget.
 *	Returns: none.
 */

static void 
FreeAllPieces(src)
MultiSrcObject src;
{
  MultiPiece * next, * first = src->multi_src.first_piece;

  if (first->prev != NULL)
    printf("Programmer Botch in FreeAllPieces, there may be a memory leak.\n");

  for ( ; first != NULL ; first = next ) {
    next = first->next;
    RemovePiece(src, first);
  }
}
  
/*	Function Name: RemovePiece
 *	Description: Removes a piece from the list.
 *	Arguments: 
 *                 piece - the piece to remove.
 *	Returns: none.
 */

static void
RemovePiece(src, piece)
MultiSrcObject src;
MultiPiece * piece;
{
  if (piece->prev == NULL)
    src->multi_src.first_piece = piece->next;
  else
    (piece->prev)->next = piece->next;

  if (piece->next != NULL)
    (piece->next)->prev = piece->prev;

  if (!src->multi_src.use_string_in_place)
    XtFree((char *)piece->text);

  XtFree((char *)piece);
}

/*	Function Name: FindPiece
 *	Description: Finds the piece containing the position indicated.
 *	Arguments: src - The MultiSrc Widget.
 *                 position - the position that we are searching for.
 * RETURNED        first - the position of the first character in this piece.
 *	Returns: piece - the piece that contains this position.
 */

static MultiPiece *
FindPiece(src, position, first)
MultiSrcObject src;
XawTextPosition position, *first;
{
  MultiPiece * old_piece, * piece = src->multi_src.first_piece;
  XawTextPosition temp;

  for ( temp = 0 ; piece != NULL ; temp += piece->used, piece = piece->next ) {
    *first = temp;
    old_piece = piece;

    if ((temp + piece->used) > position) 
      return(piece);
  }
  return(old_piece);	  /* if we run off the end the return the last piece */
}
    
static wchar *
MyWStrncpy(ws1, ws2, n)
wchar	*ws1, *ws2;
int	n;
{
  wchar * temp = (wchar *)XtMalloc(sizeof(wchar) * n);

  wcsncpy(temp, ws2, n);     /* Saber has a bug that causes it to generate*/
  wcsncpy(ws1, temp, n);     /* a bogus warning message here (CDP 6/32/89)*/
  XtFree((char *)temp);
  return(ws1);
}
  
/*	Function Name: BreakPiece
 *	Description: Breaks a full piece into two new pieces.
 *	Arguments: src - The MultiSrc Widget.
 *                 piece - the piece to break.
 *	Returns: none.
 */

#define HALF_PIECE (src->multi_src.piece_size/2)

static void
BreakPiece(src, piece)
MultiSrcObject src;
MultiPiece * piece;
{
  MultiPiece * new = AllocNewPiece(src, piece);
  
  new->text = (wchar *)XtMalloc(src->multi_src.piece_size * sizeof(wchar));
  (void) wcsncpy(new->text, piece->text + HALF_PIECE,
          src->multi_src.piece_size - HALF_PIECE);
  piece->used = HALF_PIECE;
  new->used = src->multi_src.piece_size - HALF_PIECE; 
}

/*
 * Convert string "XawMultiString" and "XawMultiFile" to quaks.
 *
 * By Li Yuhong, Mar. 26, 1991
*/
/* ARGSUSED */
static void
CvtStringToMultiType(args, num_args, fromVal, toVal)
XrmValuePtr *args;		/* unused */
Cardinal	*num_args;	/* unused */
XrmValuePtr	fromVal;
XrmValuePtr	toVal;
{
  static XawMultiType type;
  static XrmQuark  XtQEstring = NULLQUARK;
  static XrmQuark  XtQEfile;
  XrmQuark q;
  char lowerName[BUFSIZ];

  if (XtQEstring == NULLQUARK) {
    XtQEstring = XrmPermStringToQuark(XtEstring);
    XtQEfile   = XrmPermStringToQuark(XtEfile);
  }

  XmuCopyISOLatin1Lowered(lowerName, (char *) fromVal->addr);
  q = XrmStringToQuark(lowerName);

  if (q == XtQEstring) type = XawMultiString;
  if (q == XtQEfile)  type = XawMultiFile;

  (*toVal).size = sizeof(XawMultiType);
  (*toVal).addr = (XPointer) &type;
  return;
}

static XtPointer
NewString(s, code)
XtPointer s;
XawTextCode code;
{
    register wchar *ws = (wchar *)s;

    if (s == NULL)
	return NULL;
    if (code != XawTextWC)
	return (XtPointer)XtNewString((char *)s);
    return (XtPointer)wcscpy((wchar *)XtMalloc(((unsigned)wcslen(ws) + 1) * sizeof(wchar)), ws);
}
