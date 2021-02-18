/* Copyright (C) 1989, 1990, 1991 Free Software Foundation, Inc.

                        All Rights Reserved

In order to better advertise the GNU 'plot' graphics package, the
standard GNU copyleft on this software has been lifted, and replaced
by the following:
   Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both the copyright notice and this permission notice appear in
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

xplot is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY.  No author or distributor accepts responsibility to
anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GNU General Public License for full details. Also:

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.  */

/* To take care of fonts and font sizes */

#include "config.h"
#include <X11/Xlib.h>

double font_size = 14.;
char *font_name = NULL;
int font_name_length = 0;
char *user_font_name = NULL;    /* user specified font name */
int user_font_name_length = 0;
char *x_name = NULL;	    /* x font name */
int x_name_length = 0;

extern Display *dpy;
extern Pixmap pixmap;
extern GC gc;
extern XFontStruct *font;

extern int debug_flag;
extern short curpt[];
extern short xmin, ymax;
extern double xfactor, yfactor;

#define DEFAULTFONT "fixed"

struct name_info_struct
{
  char *plot;
  char *x;
};

struct name_info_struct name_info[] = {
  {
    "courier-bold",
    "courier-bold-r"},
  {
    "courier-boldoblique",
    "courier-bold-o"},
  {
    "courier-oblique",
    "courier-medium-o"},
  {
    "courier",
    "courier-medium-r"},
  {
    "helvetica-bold",
    "helvetica-bold-r"},
  {
    "helvetica-boldoblique",
    "helvetica-bold-o"},
  {
    "helvetica-oblique",
    "helvetica-medium-o"},
  {
    "helvetica",
    "helvetica-medium-r"},
  {
    "symbol",
    "symbol-medium-r"},
  {
    "times-bold",
    "times-bold-r"},
  {
    "times-bolditalic",
    "times-bold-i"},
  {
    "times-italic",
    "times-medium-i"},
  {
    "times-roman",
    "times-medium-r"},
  {
    NULL,
    NULL}
};

/* font structures necessary for finding the bounding boxes */
int direction_return;
int font_ascent_return, font_descent_return;
XCharStruct overall_return;

int
alabel (x_justify, y_justify, s)
     char x_justify, y_justify;
     char *s;
{
  int alabel_width, alabel_height, alabel_decent;
  int xpos, ypos;
  double x_char_offset, y_char_offset;

  switch (x_justify)	    /* placement of label with respect
				   to x coordinate */
    {
    case 'l':		   /* left justified */
      x_char_offset = 0.0;
      break;

    case 'c':		   /* centered */
      x_char_offset = -0.5;
      break;

    case 'r':		   /* right justified */
      x_char_offset = -1.0;
      break;

    default:		   /* None of the above? */
      x_char_offset = 0.0;
#ifdef DEBUG
	fprintf(stderr,"Unknown x justification: %c\n", x_justify);
#endif
      break;
    }

  switch (y_justify)	    /* placement of label with respect
				   to y coordinate */
    {
    case 'b':		   /* bottom */
      y_char_offset = 0.0;
      break;

    case 'c':		   /* centered */
      y_char_offset = 0.5;
      break;

    case 't':		   /* top */
      y_char_offset = 1.0;
      break;

    default:		/* None of the above? */
      y_char_offset = 0.0;
#ifdef DEBUG
	fprintf(stderr,"Unknown y justification: %c\n", y_justify);
#endif
      break;
    }

  alabel_width = XTextWidth (font, s, strlen (s));
  XTextExtents (font, s, strlen (s), &direction_return,
		&font_ascent_return, &font_descent_return, &overall_return);
  alabel_height = font_ascent_return + font_descent_return;
  alabel_decent = font_descent_return;

    /* add .5 to round off rather than truncate by (int) cast. */
    xpos = (int) ((curpt[0] - xmin) * xfactor
			 + x_char_offset * alabel_width + .5);
    ypos = (int) ((ymax - curpt[1]) * yfactor
		  + y_char_offset * alabel_height - alabel_decent + .5);
#ifdef DEBUG
    if (debug_flag)
      fprintf(stderr, "drawstring %d %d %d %s\n", gc, xpos, ypos, s);
#endif
    XDrawString (dpy, pixmap, gc, xpos, ypos, s, strlen (s));
  return 0;
}

/* The font has been set beforehand */

int set_font_and_size (name, size)
     char *name;
     int size;
{
  int name_size;
  int i;
  XFontStruct *newfont;

#ifdef DEBUG
  if (debug_flag)
    fprintf(stderr, "fontname `%s' size %d\n", name ? name : "", size);
#endif

  /* If the name is null or zero length, don't change the font */

  if ((name == NULL)
      || (strlen(name) == 0))
    {
      return 0;
    }
/* If size = 0 then name should be an X font name */
  if (size == 0)
    {
      newfont = XLoadQueryFont (dpy, name);
      if (newfont)	 /* if the font does exist */
	{
      	  if (font)
		XFreeFont (dpy, font);
	  font = newfont;
	  XSetFont(dpy, gc, font->fid);
#ifdef DEBUG
	  if (debug_flag)
	    fprintf(stderr, "Font `%s' %d\n", name, font);
#endif
	  return 1;	     /* font found */
	}
      return 0;		 /* font not found */
    }

  /* Try buildin an x font name from the name and size: */

  /* save the user specified name for later use */
  if (user_font_name_length < strlen (name) + 256)
    {
      user_font_name_length = strlen (name) + 256;
      if (user_font_name)
	user_font_name = (char *) realloc (user_font_name,
					   user_font_name_length);
      else
	user_font_name = (char *) malloc (user_font_name_length);
    }
  strcpy (user_font_name, name);


  /* search for the X font name correxponding to the requested name */
  i = -1;
  while (name_info[++i].plot)
    {
      if (strcmp (name_info[i].plot, name) == 0)
	break;
    }

  /* if the X font name is found, use it.  Otherwise, assume the supplied
     name is an X font name itself. */
  if (name_info[i].plot)
    {
      if (!x_name)
	{
	  x_name_length = strlen (name_info[i].x) + 24;
	  x_name = malloc (x_name_length);
	}
      else if (strlen (name_info[i].x) + 24 >= x_name_length)
	{
	  x_name_length = strlen (name_info[i].x) + 24;
	  x_name = realloc (x_name, x_name_length);
	}
      sprintf (x_name, "*-%s-*-%d-*", name_info[i].x, (int) font_size);
      if (set_font_and_size (x_name, 0))
	return 1;
    }
  else
    {
      /* Try using the name as an X font name: */
      if (set_font_and_size (name, 0))
	return 1;
    }

  return 0; /* Font not changed */
}

int
font_by_name (name)
     char *name;
{
  if (!set_font_and_size (name, (int) font_size)) {
#ifdef DEBUG
    fprintf(stderr, "warning: font `%s' not found.\n", user_font_name);
#endif
    return 0;
  }
  return 1;
}

int
fontsize (points)
     int points;
{
  font_size = points;
  if (!set_font_and_size (user_font_name, (int) font_size)) {
#ifdef DEBUG
    fprintf(stderr, "warning: font `%s' at size %d not found.\n",
	    user_font_name, points);
#endif
  return 0;
  }
  return 1;
}

/* I don't know how we could rotate text yet.  Using InterViews is probably
   an easier way of getting this implemented in a reasonable ammount of time. */

int
rotate (angle, width, height)
     int angle, width, height;
{
  return 0;
}
