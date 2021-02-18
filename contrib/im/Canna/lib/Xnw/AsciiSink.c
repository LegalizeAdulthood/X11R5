#if (!defined(lint) && !defined(SABER))
static char Xrcsid[] = "$XConsortium: AsciiSink.c,v 1.49 89/12/14 19:15:55 converse Exp $";
#endif /* lint && SABER */

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "XawInit.h"
#include "AsciiSinkP.h"
#include "AsciiSrcP.h"	/* For source function defs. */
#include "TextP.h"	/* I also reach into the text widget. */
#include <iroha/kanji.h>

#ifdef GETLASTPOS
#undef GETLASTPOS		/* We will use our own GETLASTPOS. */
#endif

#define GETLASTPOS XawTextSourceScan(source, 0, XawstAll, XawsdRight, 1, TRUE)

static void Initialize(), Destroy();
static Boolean SetValues();

static void DisplayText(), InsertCursor(), FindPosition();
static void FindDistance(), Resolve(), GetCursorBounds();

#define offset(field) XtOffset(AsciiSinkObject, ascii_sink.field)

static XtResource resources[] = {
    {XtNecho, XtCOutput, XtRBoolean, sizeof(Boolean),
	offset(echo), XtRImmediate, (caddr_t) True},
    {XtNdisplayNonprinting, XtCOutput, XtRBoolean, sizeof(Boolean),
	offset(display_nonprinting), XtRImmediate, (caddr_t) True},
};
#undef offset

#define SuperClass		(&textSinkClassRec)
AsciiSinkClassRec asciiSinkClassRec = {
  {
/* core_class fields */	
    /* superclass	  	*/	(WidgetClass) SuperClass,
    /* class_name	  	*/	"AsciiSink",
    /* widget_size	  	*/	sizeof(AsciiSinkRec),
    /* class_initialize   	*/	XawInitializeWidgetSet,
    /* class_part_initialize	*/	NULL,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* obj1		  	*/	NULL,
    /* obj2		  	*/	NULL,
    /* obj3		  	*/	0,
    /* resources	  	*/	resources,
    /* num_resources	  	*/	XtNumber(resources),
    /* xrm_class	  	*/	NULLQUARK,
    /* obj4		  	*/	FALSE,
    /* obj5		  	*/	FALSE,
    /* obj6			*/	FALSE,
    /* obj7		  	*/	FALSE,
    /* destroy		  	*/	Destroy,
    /* obj8		  	*/	NULL,
    /* obj9		  	*/	NULL,
    /* set_values	  	*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* obj10			*/	NULL,
    /* get_values_hook		*/	NULL,
    /* obj11		 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* obj12		   	*/	NULL,
    /* obj13			*/	NULL,
    /* obj14			*/	NULL,
    /* extension		*/	NULL
  },
/* text_sink_class fields */
  {
    /* DisplayText              */      DisplayText,
    /* InsertCursor             */      InsertCursor,
    /* ClearToBackground        */      XtInheritClearToBackground,
    /* FindPosition             */      FindPosition,
    /* FindDistance             */      FindDistance,
    /* Resolve                  */      Resolve,
    /* MaxLines                 */      XtInheritMaxLines,
    /* MaxHeight                */      XtInheritMaxHeight,
    /* SetTabs                  */      XtInheritSetTabs,
    /* GetCursorBounds          */      GetCursorBounds
  },
/* ascii_sink_class fields. */
  {
    /* Keep Compiler happy.     */      NULL
  }
};

WidgetClass asciiSinkObjectClass = (WidgetClass)&asciiSinkClassRec;

/* Utilities */

static int 
CharWidth (w, x, c)
Widget w;
int x;
unsigned char c;
{
    register int    i, width, nonPrinting;
    AsciiSinkObject sink = (AsciiSinkObject) w;
    XFontStruct *font = sink->text_sink.font;
    Position *tab;

    if ( c == LF ) return(0);

    if (c == TAB) {
	/* Adjust for Left Margin. */
	x -= ((TextWidget) XtParent(w))->text.margin.left;

	if (x >= XtParent(w)->core.width) return 0;
	for (i = 0, tab = sink->text_sink.tabs ; 
	     i < sink->text_sink.tab_count ; i++, tab++) {
	    if (x < *tab) {
		if (*tab < XtParent(w)->core.width)
		    return *tab - x;
		else
		    return 0;
	    }
	}
	return 0;
    }

    if ( (nonPrinting = (c < (unsigned char) SP)) )
	if (sink->ascii_sink.display_nonprinting)
	    c += '@';
	else {
	    c = SP;
	    nonPrinting = False;
	}

    if (font->per_char &&
	    (c >= font->min_char_or_byte2 && c <= font->max_char_or_byte2))
	width = font->per_char[c - font->min_char_or_byte2].width;
    else
	width = font->min_bounds.width;

    if (nonPrinting)
	width += CharWidth(w, x, (unsigned char) '^');

    return width;
}

static
int XnwCharWidth (w, kanji)
  Widget w;
  unsigned char *kanji;
{
    AsciiSinkObject sink = (AsciiSinkObject)w;
    int     width;
    XFontStruct *font;
    long num;

    if(kanji[0] == 0x8e) {
        font = sink->text_sink.kana_font;
        if (font->per_char &&
            (kanji[1] >= font->min_char_or_byte2 &&
             kanji[1] <= font->max_char_or_byte2))
            width = font->per_char[kanji[1] - font->min_char_or_byte2].width;
        else
            width = font->min_bounds.width;
    } else {
        font = sink->text_sink.kanji_font;
        if (font->per_char &&
            (kanji[0] >= font->min_byte1 && kanji[0] <= font->max_byte1) &&
            (kanji[1] >= font->min_char_or_byte2 &&
             kanji[1] <= font->max_char_or_byte2)) {
            num = (kanji[0] - font->min_byte1) *
                  (font->max_char_or_byte2 - font->min_char_or_byte2 + 1) +
                  (kanji[1] - font->min_char_or_byte2);
            width = font->per_char[num].width;
        } else {
            width = font->min_bounds.width;
        }
    }
    return width;
}

int GetCodeStatus(w, pos)
Widget w;
XawTextPosition pos;
{
    Widget source = XawTextGetSource(XtParent(w));
    register int i, flag;
    XawTextPosition left;
    XawTextBlock block;
    int inJisCode = Xnw_Ascii;
    int length;

    left = XawTextSourceScan(source, pos, XawstEOL, XawsdLeft, 1, False);
    XawTextSourceRead(source, left, &block, pos - left);
    length = block.length - 2;
    for(i=0; i<block.length; i++) {
	if(i < length) {
            if(ISKANJICODE(block.ptr[i], block.ptr[i+1], block.ptr[i+2]))
            	inJisCode = Xnw_Kanji;
            else if(ISKANACODE(block.ptr[i], block.ptr[i+1], block.ptr[i+2]))
            	inJisCode = Xnw_Kana;
            else
            	inJisCode = Xnw_Ascii;
	}
/*
        if(block.ptr[i] == 0x1b) {
            if(block.ptr[i+1] == '(' && block.ptr[i+2] == 'I') {
                inJisCode = Xnw_Kana;
            } else if(block.ptr[i+1] == '$' && block.ptr[i+2] == 'B') {
                inJisCode = Xnw_Kanji;
            } else
                inJisCode = Xnw_Ascii;
        }
*/
    }
    return inJisCode;
}

/*	Function Name: PaintText
 *	Description: Actually paints the text into the windoe.
 *	Arguments: w - the text widget.
 *                 gc - gc to paint text with.
 *                 x, y - location to paint the text.
 *                 buf, len - buffer and length of text to paint.
 *	Returns: the width of the text painted, or 0.
 *
 * NOTE:  If this string attempts to paint past the end of the window
 *        then this function will return zero.
 */

static Dimension
PaintText(w, gc, x, y, buf, len, tw)
Widget w;
GC *gc;
Position x, y;
unsigned char * buf;
int len;
Dimension *tw;
{
    AsciiSinkObject sink = (AsciiSinkObject) w;
    TextWidget ctx = (TextWidget) XtParent(w);

    Position max_x;
    Dimension width;
    register int flag, i;
    max_x = (Position) ctx->core.width;


/*
{int ii;
 printf("PaintText\n");
 printf("len=%d\n", len);
 printf("buf=");
 for(ii=0; ii<len; ii++)
  printf("%c", buf[ii]);
 printf("\n");
 for(ii=0; ii<len; ii++)
  printf("%x", buf[ii]);
 printf("\n");
}
*/
    for(flag=0, i=0; i<len; i++) {
        if(buf[i] & 0x80)
            flag = ~flag;
    }
    if(flag) {
        len--;
        buf[len] = 0;
    }

    width = XKanjiTextWidth(&(sink->text_sink.font), (char *)buf,
                                                len, fs_jis_euc3_mode);

    if ( ((int) width) <= -x) {	           /* Don't draw if we can't see it. */
      *tw = width;
      return(width);
    }
    XDrawImageKanjiString(XtDisplay(ctx), XtWindow(ctx), gc,
                                &(sink->text_sink.font), (int)x, (int)y,
                                (char *)buf, len, fs_jis_euc3_mode);
    if ( (((Position) width + x) > max_x) && (ctx->text.margin.right != 0) ) {
	x = ctx->core.width - ctx->text.margin.right;
	*tw = width;
	width = ctx->text.margin.right;
        XFillRectangle(XtDisplay((Widget) ctx), XtWindow( (Widget) ctx),
                       sink->ascii_sink.normgc, (int) x,
                       (int) y - sink->text_sink.max_ascent,
                       (unsigned int) width,
                       (unsigned int) (sink->text_sink.max_ascent +
                                       sink->text_sink.max_descent));
	return(0);
    }
    *tw = width;
    return(width);
}

/* Sink Object Functions */

/*
 * This function does not know about drawing more than one line of text.
 */
 
static void 
DisplayText(w, x, y, pos1, pos2, highlight)
Widget w;
Position x, y;
Boolean highlight;
XawTextPosition pos1, pos2;
{
    AsciiSinkObject sink = (AsciiSinkObject) w;
    Widget source = XawTextGetSource(XtParent(w));
    unsigned char buf[BUFSIZ];

    int j, k;
    XawTextBlock blk;
    Dimension tw;
    GC *gc;
    GC *invgc;

/*
{
printf("Pos1=%d Pos2=%d\n", pos1, pos2);
}
*/
    if (!sink->ascii_sink.echo) return;

    switch(highlight) {
        case MARK:
            gc = &(sink->ascii_sink.mgc);
            invgc = &(sink->ascii_sink.minvgc);
            break;
        case NORMAL:
            gc = &(sink->ascii_sink.normgc);
            invgc = &(sink->ascii_sink.invgc);
            break;
        case REVERSE:
            gc = &(sink->ascii_sink.invgc);
            invgc = &(sink->ascii_sink.normgc);
            break;
    }
    y += sink->text_sink.max_ascent;
    for ( j = 0 ; pos1 < pos2 ; ) {
	pos1 = XawTextSourceRead(source, pos1, &blk, pos2 - pos1);
	for (k = 0; k < blk.length; k++) {
	    if (j >= BUFSIZ) {	/* buffer full, dump the text. */
	        x += PaintText(w, gc, x, y, buf, j, &tw);
		j = 0;
	    }
	    buf[j] = blk.ptr[k];
            if (buf[j] & 0x80)
                ;
            else if(buf[j] == LF)
	        continue;

	    else if (buf[j] == '\t') {
	        Position temp = 0;
		Dimension width;

	        if ((j != 0) && ((temp = PaintText(w, gc, x, y, buf, j, &tw)) == 0))
		  return;

	        x += temp;
		width = CharWidth(w, x, (unsigned char) '\t');
                XFillRectangle(XtDisplayOfObject(w), XtWindowOfObject(w),
                               invgc[0], (int) x,
                               (int) y - sink->text_sink.max_ascent,
                               (unsigned int) width,
                               (unsigned int) (sink->text_sink.max_ascent +
                                               sink->text_sink.max_descent));
		x += width;
		j = -1;
	    }
	    else if ( buf[j] < (unsigned char) ' ' ) {
	        if (sink->ascii_sink.display_nonprinting) {
		    buf[j + 1] = buf[j] + '@';
		    buf[j] = '^';
		    j++;
		}
		else
		    buf[j] = ' ';
	    }
	    j++;
	}
    }
    if (j > 0) {
        Dimension width;
        width = PaintText(w, gc, x, y, buf, j, &tw);
        if((!sink->text_sink.magician) && (highlight == MARK)) {
            XDrawLine(XtDisplay(XtParent(w)), XtWindow(XtParent(w)),
                                                gc[0], x, y, x + tw, y);
        }
    }
}

#define insertCursor_width 6
#define insertCursor_height 3
static char insertCursor_bits[] = {0x0c, 0x1e, 0x33};

static Pixmap
CreateInsertCursor(s)
Screen *s;
{
    return (XCreateBitmapFromData (DisplayOfScreen(s), RootWindowOfScreen(s),
		  insertCursor_bits, insertCursor_width, insertCursor_height));
}

/*	Function Name: GetCursorBounds
 *	Description: Returns the size and location of the cursor.
 *	Arguments: w - the text object.
 * RETURNED        rect - an X rectangle to return the cursor bounds in.
 *	Returns: none.
 */

static void
GetCursorBounds(w, rect)
Widget w;
XRectangle * rect;
{
    AsciiSinkObject sink = (AsciiSinkObject) w;

    rect->width = (unsigned short) insertCursor_width;
    rect->height = (unsigned short) insertCursor_height;
    rect->x = sink->ascii_sink.cursor_x - (short) (rect->width / 2);
    rect->y = sink->ascii_sink.cursor_y - (short) rect->height;
}

/*
 * The following procedure manages the "insert" cursor.
 */

static void
InsertCursor (w, x, y, state)
Widget w;
Position x, y;
XawTextInsertState state;
{
    AsciiSinkObject sink = (AsciiSinkObject) w;
    Widget text_widget = XtParent(w);
    XRectangle rect;

    sink->ascii_sink.cursor_x = x;
    sink->ascii_sink.cursor_y = y;

    GetCursorBounds(w, &rect);
    if (state != sink->ascii_sink.laststate && XtIsRealized(text_widget)) 
        XCopyPlane(XtDisplay(text_widget),
		   sink->ascii_sink.insertCursorOn,
		   XtWindow(text_widget), sink->ascii_sink.xorgc,
		   0, 0, (unsigned int) rect.width, (unsigned int) rect.height,
		   (int) rect.x, (int) rect.y, 1);
    sink->ascii_sink.laststate = state;
}

/*
 * Given two positions, find the distance between them.
 */

static void
FindDistance (w, fromPos, fromx, toPos, resWidth, resPos, resHeight)
Widget w;
XawTextPosition fromPos;	/* First position. */
int fromx;			/* Horizontal location of first position. */
XawTextPosition toPos;		/* Second position. */
int *resWidth;			/* Distance between fromPos and resPos. */
XawTextPosition *resPos;	/* Actual second position used. */
int *resHeight;			/* Height required. */
{
    AsciiSinkObject sink = (AsciiSinkObject) w;
    Widget source = XawTextGetSource(XtParent(w));

    register XawTextPosition index, lastPos;
    register unsigned char c;
    register unsigned char k[2];
    XawTextBlock blk;

    /* we may not need this */
    lastPos = GETLASTPOS;
    XawTextSourceRead(source, fromPos, &blk, toPos - fromPos);
    *resWidth = 0;
    for (index = fromPos; index != toPos && index < lastPos; index++) {
	if (index - blk.firstPos >= blk.length)
	    XawTextSourceRead(source, index, &blk, toPos - fromPos);
	c = blk.ptr[index - blk.firstPos];
        if(c & 0x80) {
            k[0] = c;
            index++;
            k[1] = blk.ptr[index - blk.firstPos];
            *resWidth += XnwCharWidth(w, k);
        } else {
            *resWidth += CharWidth(w, fromx + *resWidth, c);
        }
	if (c == LF) {
	    index++;
	    break;
	}
    }
    *resPos = index;
    *resHeight = sink->text_sink.max_ascent +sink->text_sink.max_descent;
}


static void
FindPosition(w, fromPos, fromx, width, stopAtWordBreak, 
		  resPos, resWidth, resHeight)
Widget w;
XawTextPosition fromPos; 	/* Starting position. */
int fromx;			/* Horizontal location of starting position.*/
int width;			/* Desired width. */
int stopAtWordBreak;		/* Whether the resulting position should be at
				   a word break. */
XawTextPosition *resPos;	/* Resulting position. */
int *resWidth;			/* Actual width used. */
int *resHeight;			/* Height required. */
{
    AsciiSinkObject sink = (AsciiSinkObject) w;
    Widget source = XawTextGetSource(XtParent(w));

    XawTextPosition lastPos, index, whiteSpacePosition;
    int     lastWidth, whiteSpaceWidth;
    Boolean whiteSpaceSeen;
    unsigned char c;
    XawTextBlock blk;
    unsigned char k[2];
    register Boolean kanji;

    lastPos = GETLASTPOS;

    XawTextSourceRead(source, fromPos, &blk, BUFSIZ);
    *resWidth = 0;
    whiteSpaceSeen = FALSE;
    c = 0;
    for (index = fromPos; *resWidth <= width && index < lastPos; index++) {
	lastWidth = *resWidth;
	if (index - blk.firstPos >= blk.length)
	    XawTextSourceRead(source, index, &blk, BUFSIZ);
	c = blk.ptr[index - blk.firstPos];
        if(c & 0x80) {
            kanji = True;
            k[0] = c;
            index++;
            k[1] = blk.ptr[index - blk.firstPos];
            *resWidth += XnwCharWidth(w, k);
        } else {
            kanji = False;
            *resWidth += CharWidth(w, fromx + *resWidth, c);
        }

	if ((c == SP || c == TAB) && *resWidth <= width) {
	    whiteSpaceSeen = TRUE;
	    whiteSpacePosition = index;
	    whiteSpaceWidth = *resWidth;
	}
	if (c == LF) {
	    index++;
	    break;
	}
    }
    if (*resWidth > width && index > fromPos) {
	*resWidth = lastWidth;
	index--;
        if(kanji)
            index--;
	if (stopAtWordBreak && whiteSpaceSeen) {
	    index = whiteSpacePosition + 1;
	    *resWidth = whiteSpaceWidth;
	}
    }
    if (index == lastPos && c != LF) index = lastPos + 1;
    *resPos = index;
    *resHeight = sink->text_sink.max_ascent +sink->text_sink.max_descent;
}

static void
Resolve (w, pos, fromx, width, leftPos, rightPos)
Widget w;
XawTextPosition pos;
int fromx, width;
XawTextPosition *leftPos, *rightPos;
{
    int resWidth, resHeight;
    Widget source = XawTextGetSource(XtParent(w));

    FindPosition(w, pos, fromx, width, FALSE, leftPos, &resWidth, &resHeight);
    if (*leftPos > GETLASTPOS)
      *leftPos = GETLASTPOS;
    *rightPos = *leftPos;
}

static void
GetGC(sink)
AsciiSinkObject sink;
{
    XtGCMask valuemask = (GCFont | 
			  GCGraphicsExposures | GCForeground | GCBackground );
    XGCValues values;

    values.font = sink->text_sink.font->fid;
    values.graphics_exposures = (Bool) FALSE;
    
    values.foreground = sink->text_sink.foreground;
    values.background = sink->text_sink.background;
    sink->ascii_sink.normgc = XtGetGC((Widget)sink, valuemask, &values);
    values.font = sink->text_sink.kanji_font->fid;
    sink->ascii_sink.kanji_normgc = XtGetGC((Widget)sink, valuemask, &values);
    values.font = sink->text_sink.kana_font->fid;
    sink->ascii_sink.kana_normgc = XtGetGC((Widget)sink, valuemask, &values);

    values.font = sink->text_sink.font->fid;
    
    values.foreground = sink->text_sink.background;
    values.background = sink->text_sink.foreground;
    sink->ascii_sink.invgc = XtGetGC((Widget)sink, valuemask, &values);
    values.font = sink->text_sink.kanji_font->fid;
    sink->ascii_sink.kanji_invgc = XtGetGC((Widget)sink, valuemask, &values);
    values.font = sink->text_sink.kana_font->fid;
    sink->ascii_sink.kana_invgc = XtGetGC((Widget)sink, valuemask, &values);

    values.font = sink->text_sink.font->fid;
    
    values.function = GXxor;
    values.background = (unsigned long) 0L;	/* (pix ^ 0) = pix */
    values.foreground = (sink->text_sink.background ^ 
			 sink->text_sink.foreground);
    valuemask = GCFunction | GCForeground | GCBackground;
    
    sink->ascii_sink.xorgc = XtGetGC((Widget)sink, valuemask, &values);
    values.font = sink->text_sink.kanji_font->fid;
    sink->ascii_sink.kanji_xorgc = XtGetGC((Widget)sink, valuemask, &values);
    values.font = sink->text_sink.kana_font->fid;
    sink->ascii_sink.kana_xorgc = XtGetGC((Widget)sink, valuemask, &values);

    if(sink->text_sink.magician) {
    	valuemask = (GCFont | GCGraphicsExposures | GCForeground | GCBackground );
    	values.font = sink->text_sink.font->fid;
    	values.graphics_exposures = (Bool) FALSE;

    	values.foreground = sink->text_sink.magical_foreground;
    	values.background = sink->text_sink.magical_background;
    	sink->ascii_sink.mgc = XtGetGC((Widget)sink, valuemask, &values);

    	values.font = sink->text_sink.kanji_font->fid;
    	sink->ascii_sink.kanji_mgc = XtGetGC((Widget)sink, valuemask, &values);

    	values.font = sink->text_sink.kana_font->fid;
    	sink->ascii_sink.kana_mgc = XtGetGC((Widget)sink, valuemask, &values);

    	values.font = sink->text_sink.font->fid;
    	values.foreground = sink->text_sink.magical_background;
    	values.background = sink->text_sink.magical_foreground;
    	sink->ascii_sink.minvgc = XtGetGC((Widget)sink, valuemask, &values);

    	values.font = sink->text_sink.kanji_font->fid;
    	sink->ascii_sink.kanji_minvgc = XtGetGC((Widget)sink, valuemask, &values);

    	values.font = sink->text_sink.kana_font->fid;
    	sink->ascii_sink.kana_minvgc = XtGetGC((Widget)sink, valuemask, &values);
    } else {
    	sink->ascii_sink.mgc = sink->ascii_sink.normgc;
    	sink->ascii_sink.kanji_mgc = sink->ascii_sink.kanji_normgc;
    	sink->ascii_sink.kana_mgc = sink->ascii_sink.kana_normgc;
    	sink->ascii_sink.minvgc = sink->ascii_sink.invgc;
    	sink->ascii_sink.kanji_minvgc = sink->ascii_sink.kanji_invgc;
    	sink->ascii_sink.kana_minvgc = sink->ascii_sink.kana_invgc;
    }
}


/***** Public routines *****/

/*	Function Name: Initialize
 *	Description: Initializes the TextSink Object.
 *	Arguments: request, new - the requested and new values for the object
 *                                instance.
 *	Returns: none.
 *
 */

/* ARGSUSED */
static void
Initialize(request, new)
Widget request, new;
{
    AsciiSinkObject sink = (AsciiSinkObject) new;

    GetGC(sink);
    
    sink->ascii_sink.insertCursorOn= CreateInsertCursor(XtScreenOfObject(new));
    sink->ascii_sink.laststate = XawisOff;
    sink->ascii_sink.cursor_x = sink->ascii_sink.cursor_y = 0;
}

/*	Function Name: Destroy
 *	Description: This function cleans up when the object is 
 *                   destroyed.
 *	Arguments: w - the AsciiSink Object.
 *	Returns: none.
 */

static void
Destroy(w)
Widget w;
{
   AsciiSinkObject sink = (AsciiSinkObject) w;

   XtReleaseGC(w, sink->ascii_sink.normgc);
   XtReleaseGC(w, sink->ascii_sink.invgc);
   XtReleaseGC(w, sink->ascii_sink.xorgc);
   XtReleaseGC(w, sink->ascii_sink.kanji_normgc);
   XtReleaseGC(w, sink->ascii_sink.kana_normgc);
   XtReleaseGC(w, sink->ascii_sink.kanji_invgc);
   XtReleaseGC(w, sink->ascii_sink.kana_invgc);
   XtReleaseGC(w, sink->ascii_sink.kanji_xorgc);
   XtReleaseGC(w, sink->ascii_sink.kana_xorgc);

   if(sink->text_sink.magician) {
   	XtReleaseGC(w, sink->ascii_sink.mgc);
   	XtReleaseGC(w, sink->ascii_sink.kanji_mgc);
   	XtReleaseGC(w, sink->ascii_sink.kana_mgc);
   	XtReleaseGC(w, sink->ascii_sink.minvgc);
   	XtReleaseGC(w, sink->ascii_sink.kanji_minvgc);
   	XtReleaseGC(w, sink->ascii_sink.kana_minvgc);
   }
   XFreePixmap(XtDisplayOfObject(w), sink->ascii_sink.insertCursorOn);
}

/*	Function Name: SetValues
 *	Description: Sets the values for the AsciiSink
 *	Arguments: current - current state of the object.
 *                 request - what was requested.
 *                 new - what the object will become.
 *	Returns: True if redisplay is needed.
 */

/* ARGSUSED */
static Boolean
SetValues(current, request, new)
Widget current, request, new;
{
    AsciiSinkObject w = (AsciiSinkObject) new;
    AsciiSinkObject old_w = (AsciiSinkObject) current;

    if((w->text_sink.font != old_w->text_sink.font) ||
       (w->text_sink.kanji_font != old_w->text_sink.kanji_font) ||
       (w->text_sink.kana_font != old_w->text_sink.kana_font) ||
       (w->text_sink.foreground != old_w->text_sink.foreground) ||
       (w->text_sink.background != old_w->text_sink.background)) {
	XtReleaseGC(w, w->ascii_sink.normgc);
	XtReleaseGC(w, w->ascii_sink.invgc);
	XtReleaseGC(w, w->ascii_sink.xorgc);
	XtReleaseGC(w, w->ascii_sink.kanji_normgc);
	XtReleaseGC(w, w->ascii_sink.kanji_invgc);
	XtReleaseGC(w, w->ascii_sink.kanji_xorgc);
	XtReleaseGC(w, w->ascii_sink.kana_normgc);
	XtReleaseGC(w, w->ascii_sink.kana_invgc);
	XtReleaseGC(w, w->ascii_sink.kana_xorgc);

	if(w->text_sink.magician) {
	    XtReleaseGC(w, w->ascii_sink.mgc);
	    XtReleaseGC(w, w->ascii_sink.minvgc);
	    XtReleaseGC(w, w->ascii_sink.kanji_mgc);
	    XtReleaseGC(w, w->ascii_sink.kanji_minvgc);
	    XtReleaseGC(w, w->ascii_sink.kana_mgc);
	    XtReleaseGC(w, w->ascii_sink.kana_minvgc);
	}
	GetGC(w);
	((TextWidget)XtParent(new))->text.redisplay_needed = True;
    } else {
	if ( (w->ascii_sink.echo != old_w->ascii_sink.echo) ||
	     (w->ascii_sink.display_nonprinting != 
                                     old_w->ascii_sink.display_nonprinting) )
	    ((TextWidget)XtParent(new))->text.redisplay_needed = True;
    }
    
    return False;
}
