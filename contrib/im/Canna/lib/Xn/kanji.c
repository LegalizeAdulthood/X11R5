/***********************************************************
 *							   *	
 *            COPYRIGHT (C) 1988 NEC CORPORATION	   * 
 *      	     ALL RIGHTS RESERVED		   *
 *						           *	
 ***********************************************************/
/*****************************
 *
 *	8/16 bit String Manipulations.
 *
 *****************************/

static char rcs_id[] = "$Header: /work/nk/ui/RCS/kanji.c,v 3.1 91/03/13 15:10:19 kon Exp $@(#)kanji.c	102.1.1.2.3 91/03/13 15:06:47";

#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <iroha/kanji.h>

/*
  以下の定義は kanji.h の中にある。

  typedef enum {single_byte_code, double_byte_code} code_len;
  
  typedef struct {
  code_len nbyte;
  int (*func)();
  } fs_mode_rec, *fs_mode;
  */

#define LOCALBUFSIZE 512 /* mallocがLOCALBUFSIZE以下の時はmallocせずに
			    localbufを使う */

int XKanjiTextWidth(font_structs, string, count, mode)
     XFontStruct **font_structs;
     char *string;
     int count;	/* [Byte]     */
     fs_mode mode;
{
  char lbuf[LOCALBUFSIZE];
  char *p = lbuf;
  int useMalloc = False;
  int len, retval = 0, fontIndex = 0, nextIndex;
  
  if (count > LOCALBUFSIZE) {
    p = (char *)malloc(count);
    if ( !p ) return -1;
    useMalloc = True;
  }

  while (count > 0) {
    len = (mode[fontIndex].func)(string, p, count,
				 &string, &count, &nextIndex);
    if (mode[fontIndex].nbyte == single_byte_code) {
      if (font_structs[fontIndex])
	retval += XTextWidth(font_structs[fontIndex], p, len);
    }
    else {
      if (font_structs[fontIndex])
	retval += XTextWidth16(font_structs[fontIndex], p, len);
    }
    fontIndex = nextIndex;
  }
  if (useMalloc) {
    free(p);
  }
  return retval;
}

XKanjiTextExtents(font_structs, string, nchars,
		  direction_return, ascent_return, descent_return,
		  overall_return, mode)
     XFontStruct **font_structs;
     char *string;
     int nchars;	/* [Byte]     */
     int *direction_return;
     int *ascent_return, *descent_return;
     XCharStruct *overall_return;
     fs_mode mode;
{
  char lbuf[LOCALBUFSIZE];
  char *p = lbuf;
  int useMalloc = False;
  int len, fontIndex = 0, nextIndex;
  int direction, ascent, descent;
  XCharStruct overall;
  
  *direction_return = 0;
  *ascent_return = 0;
  *descent_return = 0;
  overall_return->width = 0;
  
  if (nchars > LOCALBUFSIZE) {
    p = (char *)malloc(nchars);
    if ( !p ) return -1;
    useMalloc = True;
  }

  while (nchars > 0) {
    len = (mode[fontIndex].func)(string, p, nchars,
				 &string, &nchars, &nextIndex);
    if (mode[fontIndex].nbyte == single_byte_code) {
      if (font_structs[fontIndex])
	XTextExtents(font_structs[fontIndex], p, len,
		     &direction, &ascent, &descent, &overall);
    }
    else {
      if (font_structs[fontIndex])
	XTextExtents16(font_structs[fontIndex], p, len,
		       &direction, &ascent, &descent, &overall);
    }
    if(ascent > *ascent_return)
      overall_return->ascent = *ascent_return = ascent;
    if(descent > *descent_return)
      overall_return->descent = *descent_return = descent;
    *direction_return = direction;
    overall_return->width += overall.width;
    fontIndex = nextIndex;
  }
  overall_return->lbearing = overall.lbearing;
  overall_return->rbearing = overall_return->width 
    - overall.width + overall.rbearing;
  if (useMalloc)
    free(p);
}

XQueryKanjiTextExtents(display, font_IDs, font_structs, string, nchars,
		       direction_return, ascent_return, descent_return,
		       overall_return, mode)
     Display *display;
     XID *font_IDs;
     XFontStruct **font_structs;
     char *string;
     int nchars;	/* [Byte]     */
     int *direction_return;
     int *ascent_return, *descent_return;
     XCharStruct *overall_return;
     fs_mode mode;
{
  char lbuf[LOCALBUFSIZE];
  char *p = lbuf;
  int useMalloc = False;
  int len, fontIndex = 0, nextIndex;
  int direction, ascent, descent;
  XCharStruct overall;
  
  *direction_return = 0;
  *ascent_return = 0;
  *descent_return = 0;
  overall_return->width = 0;
  
  if (nchars > LOCALBUFSIZE) {
    p = (char *)malloc(nchars);
    if ( !p ) return -1;
    useMalloc = True;
  }

  while (nchars > 0) {
    len = (mode[fontIndex].func)(string, p, nchars,
				 &string, &nchars, &nextIndex);
    if (mode[fontIndex].nbyte == single_byte_code) {
      if (font_IDs[fontIndex])
	XQueryTextExtents(display, font_IDs[fontIndex], p, len,
			  &direction, &ascent, &descent, &overall);
    }
    else {
      if (font_IDs[fontIndex])
	XQueryTextExtents16(display, font_IDs[fontIndex], p, len,
			    &direction, &ascent, &descent, &overall);
    }
    if(ascent > *ascent_return)
      overall_return->ascent = *ascent_return = ascent;
    if(descent > *descent_return)
      overall_return->descent = *descent_return = descent;
    *direction_return = direction;
    overall_return->width += overall.width;
    fontIndex = nextIndex;
  }
  overall_return->lbearing = overall.lbearing;
  overall_return->rbearing = overall_return->width 
    - overall.width + overall.rbearing;
  if (useMalloc) free(p);
}

XDrawKanjiText(display, d, gcs, x, y, items, nitems, mode)
     Display *display;
     Drawable d;
     GC *gcs;
     int x, y;
     XKanjiTextItem *items;
     int nitems;	/* [Byte]     */
     fs_mode mode;
{
  char lbuf[LOCALBUFSIZE];
  char *p = lbuf;
  int useMalloc = False;
  int len, fontIndex = 0, nextIndex;
  char *string;
  int length;
  XTextItem item8;
  XTextItem16 item16;
  
  for( ; nitems--; items++)
    {
      string = items->chars;
      length = items->nchars;
      if (length > LOCALBUFSIZE) {
	p = (char *)malloc(length);
	if ( !p ) return -1;
	useMalloc = True;
      }

      while (length > 0) {
	len = (mode[fontIndex].func)(string, p, length,
				     &string, &length, &nextIndex);
	if (mode[fontIndex].nbyte == single_byte_code) {
	  if (items->fonts[fontIndex]) {
	    item8.chars = p;
	    item8.nchars = len;
	    item8.delta = items->delta;
	    item8.font = items->fids[fontIndex];
	    XDrawText(display, d, gcs[fontIndex], x, y, &item8, 1);
	    x += XTextWidth(items->fonts[fontIndex], p, len);
	  }
	}
	else {
	  if (items->fonts[fontIndex]) {
	    item16.chars = (XChar2b *)p;
	    item16.nchars = len;
	    item16.delta = items->delta;
	    item16.font = items->fids[fontIndex];
	    XDrawText16(display, d, gcs[fontIndex], x, y, &item16, 1);
	    x += XTextWidth16(items->fonts[fontIndex], p, len);
	  }
	}
	fontIndex = nextIndex;
      }
      if (useMalloc) free(p);
    }
}

XDrawKanjiString(display, d, gcs, font_structs, x, y, string, length, mode)
     Display *display;
     Drawable d;
     GC *gcs;
     XFontStruct **font_structs;
     int x, y;
     char *string;
     int length;	/* [Byte]     */
     fs_mode mode;
{
  char lbuf[LOCALBUFSIZE];
  char *p = lbuf;
  int useMalloc = False;
  int len, fontIndex = 0, nextIndex;
  
  if (length > LOCALBUFSIZE) {
    p = (char *)malloc(length);
    if ( !p ) return -1;
    useMalloc = True;
  }

  while (length > 0) {
    len = (mode[fontIndex].func)(string, p, length,
				 &string, &length, &nextIndex);
    if (mode[fontIndex].nbyte == single_byte_code) {
      if (font_structs[fontIndex]) {
	XDrawString(display, d, gcs[fontIndex], x, y, p, len);
	x += XTextWidth(font_structs[fontIndex], p, len);
      }
    }
    else {
      if (font_structs[fontIndex]) {
	XDrawString16(display, d, gcs[fontIndex], x, y, p, len);
	x += XTextWidth16(font_structs[fontIndex], p, len);
      }
    }
    fontIndex = nextIndex;
  }
  if (useMalloc) free(p);
}

XDrawImageKanjiString(display, d, gcs, font_structs,
		      x, y, string, length, mode)
     Display *display;
     Drawable d;
     GC *gcs;
     XFontStruct **font_structs;
     int x, y;
     char *string;
     int length;	/* [Byte]     */
     fs_mode mode;
{
  char lbuf[LOCALBUFSIZE];
  char *p = lbuf;
  int useMalloc = False;
  int len, fontIndex = 0, nextIndex;
  
  if (length > LOCALBUFSIZE) {
    p = (char *)malloc(length);
    if ( !p ) return -1;
    useMalloc = True;
  }

  while (length > 0) {
    len = (mode[fontIndex].func)(string, p, length,
				 &string, &length, &nextIndex);
    if (mode[fontIndex].nbyte == single_byte_code) {
      if (font_structs[fontIndex]) {
	XDrawImageString(display, d, gcs[fontIndex],
			 x, y, p, len);
	x += XTextWidth(font_structs[fontIndex], p, len);
      }
    }
    else {
      if (font_structs[fontIndex]) {
	XDrawImageString16(display, d, gcs[fontIndex],
			   x, y, p, len);
	x += XTextWidth16(font_structs[fontIndex], p, len);
      }
    }
    fontIndex = nextIndex;
  }
  if (useMalloc) free(p);
}
