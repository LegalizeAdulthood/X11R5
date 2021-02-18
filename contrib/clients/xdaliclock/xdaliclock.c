/* xdaliclock.c, Copyright (c) 1991 Jamie Zawinski <jwz@lucid.com>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

char *version =
  "XDaliClock v1.0; Copyright (c) 1991 Jamie Zawinski <jwz@lucid.com>";

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xresource.h>

#ifdef SHAPE
#include <X11/extensions/shape.h>
#endif

#include "defaults.h"

#ifndef isupper
# define isupper(c)  ((c) >= 'A' && (c) <= 'Z')
#endif
#ifndef _tolower
# define _tolower(c)  ((c) - 'A' + 'a')
#endif

#ifdef BUILTIN_FONT

int use_builtin_font;

# include "zero.xbm"
# include "one.xbm"
# include "two.xbm"
# include "three.xbm"
# include "four.xbm"
# include "five.xbm"
# include "six.xbm"
# include "seven.xbm"
# include "eight.xbm"
# include "nine.xbm"
# include "colon.xbm"
# include "slash.xbm"

struct raw_number {
  char *bits;
  int width, height;
};

struct raw_number numbers [] = {
  { zero_bits, zero_width, zero_height },
  { one_bits, one_width, one_height },
  { two_bits, two_width, two_height },
  { three_bits, three_width, three_height },
  { four_bits, four_width, four_height },
  { five_bits, five_width, five_height },
  { six_bits, six_width, six_height },
  { seven_bits, seven_width, seven_height },
  { eight_bits, eight_width, eight_height },
  { nine_bits, nine_width, nine_height },
  0
};

#endif

char *progname;
char *app_class;
char *window_title;
XrmDatabase db;
Display *display;
Window window;
Pixmap pixmap;
GC pixmap_draw_gc, pixmap_erase_gc;
GC window_draw_gc, window_erase_gc;
XColor fg_color, bg_color;
Colormap cmap;

struct frame *base_frames [10];
struct frame *current_frames [6];
struct frame *target_frames [6];
struct frame *clear_frame;
int character_width, character_height;
int x_offsets [6];
int window_offset_x, window_offset_y;
Pixmap colon, slash;
int colon_char_width;
int time_digits [6];
int last_time_digits [6];
int button_down = 0;

Pixmap memory_pig_zeros [9] [10];
Pixmap memory_pig_digits [8] [10];
Pixmap total_oinker_digits [9] [10];

int twelve_hour_time;
int be_a_pig;
int minutes_only;
int do_cycle;

#ifdef SHAPE
int do_shape;
#endif

#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))

typedef short unsigned int POS;

struct scanline {
  POS left1, right1, left2, right2;
};

struct frame {
  struct scanline scanlines [1]; /* scanlines are contiguous here */
};


struct frame *
image_to_frame (image)
     XImage *image;
{
  register int y;
  struct frame *frame;
  int width = image->width;
  int height = image->height;

  frame = (struct frame *)
    malloc (sizeof (struct frame) +
	    (sizeof (struct scanline) * (height - 1)));
  
  for (y = 0; y < height; y++) {
    register int left1, right1, left2, right2;

#define getbit(x) (XGetPixel (image, (x), y))
    for (left1 = 0; left1 < width; left1++)
      if (getbit (left1)) break;
    for (right1 = left1; right1 < width; right1++)
      if (! getbit (right1)) break;
    for (left2 = right1; left2 < width; left2++)
      if (getbit (left2)) break;
    for (right2 = left2; right2 < width; right2++)
      if (! getbit (right2)) break;
    if (left2 == right2)
      left2 = left1, right2 = right1;
#undef getbit
    if (left1 == width && right1 == width) left1 = right1 = width / 2;
    if (left2 == width && right2 == width) left2 = right2 = width / 2;

    frame->scanlines [y].left1  = left1;
    frame->scanlines [y].right1 = right1;
    frame->scanlines [y].left2  = left2;
    frame->scanlines [y].right2 = right2;
  }
  return frame;
}


/* This is kind of gross.
 */
char *default_fonts [] = {
  "-*-charter-bold-i-*-*-*-500-*-*-*-*-*-*",
  "-*-lucidabright-demibold-i-*-*-*-500-*-*-*-*-*-*",
  "-*-new century schoolbook-bold-i-*-*-*-500-*-*-*-*-*-*",
  "-*-helvetica-bold-o-*-*-*-500-*-*-*-*-*-*",
  "-*-lucida-bold-i-*-*-*-500-*-*-*-*-*-*",

  "-*-charter-bold-i-*-*-*-360-*-*-*-*-*-*",
  "-*-lucidabright-demibold-i-*-*-*-360-*-*-*-*-*-*",
  "-*-new century schoolbook-bold-i-*-*-*-360-*-*-*-*-*-*",
  "-*-helvetica-bold-o-*-*-*-360-*-*-*-*-*-*",
  "-*-lucida-bold-i-*-*-*-360-*-*-*-*-*-*",

  "-*-charter-bold-i-*-*-*-240-*-*-*-*-*-*",
  "-*-lucidabright-demibold-i-*-*-*-240-*-*-*-*-*-*",
  "-*-new century schoolbook-bold-i-*-*-*-240-*-*-*-*-*-*",
  "-*-helvetica-bold-o-*-*-*-240-*-*-*-*-*-*",
  "-*-lucida-bold-i-*-*-*-240-*-*-*-*-*-*",	/* too wide.  bug? */

  "-*-charter-bold-i-*-*-*-180-*-*-*-*-*-*",
  "-*-lucidabright-demibold-i-*-*-*-180-*-*-*-*-*-*",
  "-*-new century schoolbook-bold-i-*-*-*-180-*-*-*-*-*-*",
  "-*-helvetica-bold-o-*-*-*-180-*-*-*-*-*-*",	/* too wide */
  "-*-lucida-bold-i-*-*-*-180-*-*-*-*-*-*",	/* too wide */
  0
};

void
load_font (font_name)
     char *font_name;
{
  int i, max_lbearing, max_rbearing, max_ascent, max_descent;
  XFontStruct *font = 0;
  Pixmap pixmap;
  XImage *image = 0;
  XGCValues gcvalues;
  GC draw_gc, erase_gc;
  char **fonts = default_fonts;
  int bad_font_name = 0;

  if (font_name)
    if (! (font = XLoadQueryFont (display, font_name))) {
      bad_font_name = 1;
      fprintf (stderr, "%s: Couldn't load font \"%s\";\n", progname, font_name);
    }
  if (! font)
    for (; fonts; fonts++)
      if (font = XLoadQueryFont (display, font_name = *fonts))
	break;
  if (bad_font_name && font)
    fprintf (stderr, " using font \"%s\" instead.\n", fonts [0]);
  else if (! font) {
    if (bad_font_name)
      fprintf (stderr, " couldn't load any of the default fonts either.\n");
    else
      fprintf (stderr, "%s: Couldn't load any of the default fonts.\n",
	       progname);
    exit (-1);
  }

  if (font->min_char_or_byte2 > '0' || font->max_char_or_byte2 < '9') {
    fprintf (stderr, "%s: font %s doesn't contain characters '0'-'9'.\n",
	     progname);
    exit (-1);
  }
  max_lbearing = font->min_bounds.lbearing;
  max_rbearing = font->min_bounds.rbearing;
  max_ascent  = font->min_bounds.ascent;
  max_descent = font->min_bounds.descent;
  for (i = '0'; i <= '9'; i++) {
    XCharStruct *ch = &font->per_char [i - font->min_char_or_byte2];
    max_lbearing = MAX (max_lbearing, ch->lbearing);
    max_rbearing = MAX (max_rbearing, ch->rbearing);
    max_ascent  = MAX (max_ascent,  ch->ascent);
    max_descent = MAX (max_descent, ch->descent);
    if (ch->lbearing == ch->rbearing || ch->ascent == -ch->descent) {
      fprintf (stderr,
	    "%s: char '%c' has bbox %dx%d (%d - %d x %d + %d) in font %s\n",
	       progname,
	       i, ch->rbearing - ch->lbearing, ch->ascent + ch->descent,
	       ch->rbearing, ch->lbearing, ch->ascent, ch->descent,
	       font_name);
      exit (-1);
    }
  }
  character_width = max_rbearing + max_lbearing + 1; /* min enclosing rect */
  character_height = max_descent + max_ascent + 1;

  /* Now we know the combined bbox of the characters we're interested in;
     for each character, write it into a pixmap; grab the bits from that
     pixmap; and fill the scanline-buffers.
   */
  pixmap = XCreatePixmap (display,
			  RootWindow (display, DefaultScreen (display)),
			  character_width + 1, character_height + 1, 1);
  gcvalues.font = font->fid;
  gcvalues.foreground = 1L;
  draw_gc = XCreateGC (display, pixmap, GCFont | GCForeground, &gcvalues);
  gcvalues.foreground = 0L;
  erase_gc = XCreateGC (display, pixmap, GCForeground, &gcvalues);

  for (i = 0; i <= 9; i++) {
    XCharStruct *ch = &font->per_char [i + '0' - font->min_char_or_byte2];
    char s[1];
    s[0] = i + '0';
    XFillRectangle (display, pixmap, erase_gc, 0, 0,
		    character_width + 1, character_height + 1);
    XDrawString (display, pixmap, draw_gc, max_lbearing, max_ascent, s, 1);
    if (! image)
      image = XGetImage (display, pixmap, 0, 0,
			 character_width, character_height, 1, XYPixmap);
    else
      XGetSubImage (display, pixmap, 0, 0,
		    character_width, character_height, 1, XYPixmap,
		    image, 0, 0);
    base_frames [i] = image_to_frame (image);
  }

  {
    XCharStruct *ch1 = &font->per_char [':' - font->min_char_or_byte2];
    XCharStruct *ch2 = &font->per_char ['/' - font->min_char_or_byte2];
    int maxl = MAX (ch1->lbearing, ch2->lbearing);
    int maxr = MAX (ch1->rbearing, ch2->rbearing);
    int w = maxr + maxl + 1;
    colon =
      XCreatePixmap (display, RootWindow (display, DefaultScreen (display)),
		     w+1, character_height+1, 1);
    slash =
      XCreatePixmap (display, RootWindow (display, DefaultScreen (display)),
		     w+1, character_height+1, 1);
    XFillRectangle (display, colon, erase_gc, 0, 0, w+1, character_height+1);
    XFillRectangle (display, slash, erase_gc, 0, 0, w+1, character_height+1);
    XDrawString (display, colon, draw_gc, maxl+1, max_ascent, ":", 1);
    XDrawString (display, slash, draw_gc, maxl+1, max_ascent, "/", 1);
    colon_char_width = w;
  }

  XDestroyImage (image);
  XFreePixmap (display, pixmap);
  XFreeFont (display, font);
  XFreeGC (display, draw_gc);
  XFreeGC (display, erase_gc);
}

#ifdef BUILTIN_FONT
void
load_builtin_font ()
{
  int i;
  Pixmap pixmap;
  char **fonts = default_fonts;
  Screen *screen = DefaultScreenOfDisplay (display);
  Visual *visual = DefaultVisualOfScreen (screen);
  XImage *image =
    XCreateImage (display, visual,
		  1, XYBitmap, 0,	/* depth, format, offset */
		  (char *) 0,		/* data */
		  0, 0, 8, 0); 		/* w, h, pad, bytes_per_line */
  /* This stuff makes me nervous, but XCreateBitmapFromData() does it too. */
  image->byte_order = LSBFirst;
  image->bitmap_bit_order = LSBFirst;

  character_width = character_height = 0;

  for (i = 0; i < 10; i++) {
    struct raw_number *number = &numbers [i];
    character_width = MAX (character_width, number->width);
    character_height = MAX (character_height, number->height);
    image->width = number->width;
    image->height = number->height;
    image->data = (char *) number->bits;
    image->bytes_per_line = (number->width + 7) / 8;
    base_frames [i] = image_to_frame (image);
  }
  XDestroyImage (image);

  colon_char_width = MAX (colon_width, slash_width);
  colon = XCreateBitmapFromData (display, DefaultRootWindow (display),
				 colon_bits, colon_width, colon_height);
  slash = XCreateBitmapFromData (display, DefaultRootWindow (display),
				 slash_bits, slash_width, slash_height);
}
#endif /* BUILTIN_FONT */

#define MAX_SEGS 500
static XSegment segment_buffer [MAX_SEGS + 2];
static int segment_count = 0;

void
flush_segment_buffer (drawable, draw_gc)
     Drawable drawable;
     GC draw_gc;
{
  if (! segment_count) return;
  XDrawSegments (display, drawable, draw_gc, segment_buffer, segment_count);
  segment_count = 0;
}



void
draw_frame (frame, drawable, draw_gc, x_off)
     struct frame *frame;
     Drawable drawable;
     GC draw_gc;
     int x_off;
{
  register int y;
  for (y = 0; y < character_height; y++) {
    struct scanline *line = &frame->scanlines [y];
    if (line->left1 == line->right1) continue;
    segment_buffer [segment_count].x1 = x_off + line->left1;
    segment_buffer [segment_count].x2 = x_off + line->right1;
    segment_buffer [segment_count].y1 = y;
    segment_buffer [segment_count].y2 = y;
    segment_count++;
    if (line->left1 != line->left2) {
      segment_buffer [segment_count].x1 = x_off + line->left2;
      segment_buffer [segment_count].x2 = x_off + line->right2;
      segment_buffer [segment_count].y1 = y;
      segment_buffer [segment_count].y2 = y;
      segment_count++;
    }
    if (segment_count >= MAX_SEGS)
      flush_segment_buffer (drawable, draw_gc);
  }
}

void
set_current_scanlines (into_frame, from_frame)
     struct frame *into_frame, *from_frame;
{
  register int i;
  for (i = 0; i < character_height; i++)
    into_frame->scanlines [i] = from_frame->scanlines [i];
}

void
one_step (current_frame, target_frame, tick)
     struct frame *current_frame, *target_frame;
     int tick;
{
  register struct scanline *line = &current_frame->scanlines [0];
  register struct scanline *target = &target_frame->scanlines [0];
  register int i = 0;
  for (i = 0; i < character_height; i++) {
#define STEP(field) (line->field += ((int) (target->field - line->field)) / tick)
    STEP (left1);
    STEP (right1);
    STEP (left2);
    STEP (right2);
    line++;
    target++;
  }
}

char test_hack;
long
fill_time_digits ()
{
  long clock = time ((time_t *) 0);
  struct tm *tm = localtime (&clock);
  if (test_hack)
    {
      time_digits [0] = time_digits [1] = time_digits [2] =
	time_digits [3] = time_digits [4] = time_digits [5] =
	  (test_hack == '-' ? -1 : test_hack - '0');
      test_hack = 0;
    }
  else if (! button_down)
    {
      if (twelve_hour_time && tm->tm_hour > 12) tm->tm_hour -= 12;
      if (twelve_hour_time && tm->tm_hour == 0) tm->tm_hour = 12;
      time_digits [0] = (tm->tm_hour - (tm->tm_hour % 10)) / 10;
      time_digits [1] = tm->tm_hour % 10;
      time_digits [2] = (tm->tm_min - (tm->tm_min % 10)) / 10;
      time_digits [3] = tm->tm_min % 10;
      time_digits [4] = (tm->tm_sec - (tm->tm_sec % 10)) / 10;
      time_digits [5] = tm->tm_sec % 10;
    }
  else
    {
      tm->tm_mon++; /* 0 based */
      time_digits [0] = (tm->tm_mon - (tm->tm_mon % 10)) / 10;
      time_digits [1] = tm->tm_mon % 10;
      time_digits [2] = (tm->tm_mday - (tm->tm_mday % 10)) / 10;
      time_digits [3] = tm->tm_mday % 10;
      time_digits [4] = (tm->tm_year - (tm->tm_year % 10)) / 10;
      time_digits [5] = tm->tm_year % 10;
    }
  return clock;
}


void
fill_pig_cache (display, drawable, work_frame)
     Display *display;
     Drawable drawable;
     struct frame *work_frame;
{
  int i;
  /* do `[1-9]' to `0'
     We have very little to gain by caching the `[347]' to `0' transitions,
     but what the hell.
   */
  for (i = 0; i < 9; i++)
    {
      int tick;
      set_current_scanlines (work_frame, base_frames [0]);
      for (tick = 9; tick >= 0; tick--)
	{
	  Pixmap p = XCreatePixmap (display, drawable,
				    character_width, character_height, 1);
	  XFillRectangle (display, p, pixmap_erase_gc, 0, 0,
			  character_width, character_height);
	  draw_frame (work_frame, p, pixmap_draw_gc, 0);
	  flush_segment_buffer (p, pixmap_draw_gc);
	  memory_pig_zeros [i] [9 - tick] = p;
	  if (tick)
	    one_step (work_frame, base_frames [i+1], tick);
	}
    }
  /* do `[1-8]' to `[2-9]' */
  for (i = 0; i < 8; i++)
    {
      int tick;
      set_current_scanlines (work_frame, base_frames [i+1]);
      for (tick = 9; tick >= 0; tick--)
	{
	  Pixmap p = XCreatePixmap (display, drawable,
				    character_width, character_height, 1);
	  XFillRectangle (display, p, pixmap_erase_gc, 0, 0,
			  character_width, character_height);
	  draw_frame (work_frame, p, pixmap_draw_gc, 0);
	  flush_segment_buffer (p, pixmap_draw_gc);
	  memory_pig_digits [i] [9 - tick] = p;
	  if (tick)
	    one_step (work_frame, base_frames [i+2], tick);
	}
    }
  if (be_a_pig > 1)
    /* do `[1-7]' to `[3-9]' and `9' to `1' */
    for (i = 0; i < 9; i++)
      {
	int tick;
	if (i == 7) continue; /* zero transitions are already done */
	set_current_scanlines (work_frame, base_frames [i+1]);
	for (tick = 9; tick >= 0; tick--)
	  {
	    Pixmap p = XCreatePixmap (display, drawable,
				      character_width, character_height, 1);
	    XFillRectangle (display, p, pixmap_erase_gc, 0, 0,
			    character_width, character_height);
	    draw_frame (work_frame, p, pixmap_draw_gc, 0);
	    flush_segment_buffer (p, pixmap_draw_gc);
	    total_oinker_digits [i] [9 - tick] = p;
	    if (tick)
	      one_step (work_frame, base_frames [(i+3)%10], tick);
	  }
      }
}


void
initialize (font_name)
     char *font_name;
{
  int i;
#ifdef BUILTIN_FONT
  if (use_builtin_font)
    load_builtin_font ();
  else
#endif
    load_font (font_name);

  bzero (memory_pig_zeros, sizeof (memory_pig_zeros));
  bzero (memory_pig_digits, sizeof (memory_pig_digits));

  for (i = 0; i < 6; i++)
    current_frames [i] = (struct frame *)
      malloc (sizeof (struct frame) +
	      (sizeof (struct scanline) * (character_height - 1)));

  clear_frame = (struct frame *)
    malloc (sizeof (struct frame) +
	    (sizeof (struct scanline) * (character_height - 1)));
  for (i = 0; i < character_height; i++)
    clear_frame->scanlines [i].left1 =
      clear_frame->scanlines [i].left2 =
	clear_frame->scanlines [i].right1 =
	  clear_frame->scanlines [i].right2 = character_width / 2;
  
  x_offsets [0] = 0;
  x_offsets [1] = x_offsets [0] + character_width;
  x_offsets [2] = x_offsets [1] + character_width + colon_char_width;
  x_offsets [3] = x_offsets [2] + character_width;
  x_offsets [4] = x_offsets [3] + character_width + colon_char_width;
  x_offsets [5] = x_offsets [4] + character_width;
}


void
allocate_colors (fg, bg)
     char *fg, *bg;
{
  Screen *screen = DefaultScreenOfDisplay (display);
  cmap = DefaultColormapOfScreen (screen);
  if (do_cycle)
    {
      unsigned long plane_masks;
      unsigned long pixels [2];
      if (XAllocColorCells (display, cmap, False, &plane_masks, 0, pixels, 2))
	{
	  fg_color.pixel = pixels [0];
	  bg_color.pixel = pixels [1];
	  XParseColor (display, cmap, fg, &fg_color);
	  XParseColor (display, cmap, bg, &bg_color);
	  fg_color.flags = bg_color.flags = DoRed | DoGreen | DoBlue;
	  XStoreColor (display, cmap, &fg_color);
	  XStoreColor (display, cmap, &bg_color);
	}
      else
	{
	  fprintf (stderr,
		   "%s: couldn't allocate two read-write color cells.\n",
		   progname);
	  do_cycle = 0;
	  allocate_colors (fg, bg);
	}
    }
  else
    {
      if (! XParseColor (display, cmap, fg, &fg_color))
	{
	  fprintf (stderr, "%s: can't parse color %s; using black\n",
		   progname, fg);
	  fg_color.pixel = BlackPixelOfScreen (screen);
	}
      /* What a crock of shit. */
      else if (! strcmp (fg, "black") || ! strcmp (fg, "Black"))
	fg_color.pixel = BlackPixelOfScreen (screen);
      else if (! strcmp (fg, "white") || ! strcmp (fg, "White"))
	fg_color.pixel = WhitePixelOfScreen (screen);
      else if (! XAllocColor (display, cmap, &fg_color))
	{
	  fprintf (stderr,
		   "%s: couldn't allocate color \"%s\", using black\n",
		   progname, fg);
	  fg_color.pixel = BlackPixelOfScreen (screen);
	}

      if (! XParseColor (display, cmap, bg, &bg_color))
	{
	  fprintf (stderr, "%s: can't parse color %s; using white\n",
		   progname, bg);
	  bg_color.pixel = WhitePixelOfScreen (screen);
	}
      else if (! strcmp (bg, "black") || ! strcmp (bg, "Black"))
	bg_color.pixel = BlackPixelOfScreen (screen);
      else if (! strcmp (bg, "white") || ! strcmp (bg, "White"))
	bg_color.pixel = WhitePixelOfScreen (screen);
      else if (! XAllocColor (display, cmap, &bg_color))
	{
	  fprintf (stderr,
		   "%s: couldn't allocate color \"%s\", using white\n",
		   progname, bg);
	  bg_color.pixel = WhitePixelOfScreen (screen);
	}
    }
}


void 
draw_colon ()
{
  Pixmap glyph = button_down ? slash : colon;
  XCopyPlane (display, glyph, pixmap, pixmap_draw_gc,
	      0, 0, character_width, character_height,
	      x_offsets [1] + character_width, 0, 1);
  if (! minutes_only)
    XCopyPlane (display, glyph, pixmap, pixmap_draw_gc,
		0, 0, character_width, character_height,
		x_offsets [3] + character_width, 0, 1);
  XStoreName (display, window, button_down ? version : window_title);
}


void
initialize_window (fg, bg, bw, geom, def_geom)
     char *fg, *bg;
     int bw;
     char *geom, *def_geom;
{
  XSetWindowAttributes attributes;
  unsigned long attribute_mask;
  XClassHint class_hints;
  XSizeHints size_hints;
  XGCValues gcvalues;
  int ndigits = minutes_only ? 4 : 6;
  Screen *screen = DefaultScreenOfDisplay (display);
  int i;

  allocate_colors (fg, bg);

#ifdef SHAPE
  {
    int shape_event_base, shape_error_base;
    if (do_shape && !XShapeQueryExtension (display,
					   &shape_event_base,
					   &shape_error_base))
      {
	fprintf (stderr, "%s: no shape extension on this display\n", progname);
	do_shape = 0;
      }
  }
#endif

  attribute_mask = CWBackPixel | CWEventMask;
  attributes.event_mask =
    KeyPressMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask;
  attributes.background_pixel = bg_color.pixel;
  class_hints.res_name = progname;
  class_hints.res_class = app_class;
  size_hints.flags = PMinSize;
  size_hints.min_width = x_offsets [ndigits-1] + character_width + 1;
  size_hints.min_height = character_height + 1;

  i = XGeometry (display, DefaultScreen (display), geom, def_geom, bw,
		 1, 1, 0, 0, &size_hints.x, &size_hints.y,
		 &size_hints.width, &size_hints.height);

  if (i & (XValue|YValue))
    size_hints.flags |= USPosition;
  else
    size_hints.x = size_hints.y = 0;

  if (i & (WidthValue|HeightValue))
    size_hints.flags |= USSize;
  else {
    size_hints.width = x_offsets [ndigits-1] + character_width + 20;
    size_hints.height = character_height + 20;
  }
  window = XCreateWindow (display, DefaultRootWindow (display),
			  size_hints.x, size_hints.y,
			  size_hints.width, size_hints.height, bw,
			  CopyFromParent, /* depth */
			  InputOutput, /* class */
			  DefaultVisualOfScreen (screen),
			  attribute_mask, &attributes);

  XStoreName (display, window, window_title);
  XSetClassHint (display, window, &class_hints);
  XSetWMNormalHints (display, window, &size_hints);

  pixmap = XCreatePixmap (display, window,
			  x_offsets [ndigits-1] + character_width + 1,
			  character_height + 1, 1);

  gcvalues.foreground = fg_color.pixel;
  gcvalues.background = bg_color.pixel;
  gcvalues.function = GXcopy;
  gcvalues.graphics_exposures = False;
  window_draw_gc = XCreateGC (display, window,
			      GCForeground | GCBackground | GCFunction |
			      GCGraphicsExposures,
			      &gcvalues);
  gcvalues.foreground = bg_color.pixel;
  gcvalues.background = fg_color.pixel;
  window_erase_gc= XCreateGC (display, window,
			      GCForeground | GCBackground | GCFunction |
			      GCGraphicsExposures,
			      &gcvalues);
  gcvalues.foreground = 1;
  gcvalues.background = 0;
  pixmap_draw_gc = XCreateGC (display, pixmap,
			      GCForeground | GCBackground | GCFunction |
			      GCGraphicsExposures,
			      &gcvalues);
  gcvalues.foreground = 0;
  gcvalues.background = 1;
  pixmap_erase_gc= XCreateGC (display, pixmap,
			      GCForeground | GCBackground | GCFunction |
			      GCGraphicsExposures,
			      &gcvalues);
  
  XFillRectangle (display, pixmap, pixmap_erase_gc,
		  0, 0, x_offsets [ndigits-1] + character_width + 1,
		  character_height + 1);

  XMapWindow (display, window);
  XFlush (display);

  if (be_a_pig)
    fill_pig_cache (display, window, current_frames [0]);

  for (i = 0; i < ndigits; i++)
    {
      last_time_digits [i] = -1;
      set_current_scanlines (current_frames [i], clear_frame);
    }
  XFillRectangle (display, window, window_erase_gc,
		  0, 0, x_offsets [ndigits-1] + character_width,
		  character_height + 1);
  draw_colon ();
}


void
hsv_to_rgb (h,s,v, r,g,b)
     int h;			/* 0 - 360   */
     double s, v;		/* 0.0 - 1.0 */
     unsigned short *r, *g, *b;	/* 0 - 65535 */
{
  double H, S, V, R, G, B;
  double p1, p2, p3;
  double f;
  int i;
  S = s; V = v;
  H = (h % 360) / 60.0;
  i = H;
  f = H - i;
  p1 = V * (1 - S);
  p2 = V * (1 - (S * f));
  p3 = V * (1 - (S * (1 - f)));
  if	  (i == 0) { R = V;  G = p3; B = p1; }
  else if (i == 1) { R = p2; G = V;  B = p1; }
  else if (i == 2) { R = p1; G = V;  B = p3; }
  else if (i == 3) { R = p1; G = p2; B = V;  }
  else if (i == 4) { R = p3; G = p1; B = V;  }
  else		   { R = V;  G = p1; B = p2; }
  *r = R * 65535;
  *g = G * 65535;
  *b = B * 65535;
}

void
cycle_colors ()
{
  static int hue_tick;
  hsv_to_rgb (hue_tick,
	      1.0, 1.0,
	      &fg_color.red, &fg_color.green, &fg_color.blue);
  hsv_to_rgb ((hue_tick + 180) % 360,
	      1.0, 1.0,
	      &bg_color.red, &bg_color.green, &bg_color.blue);
  hue_tick = (hue_tick+1)%360;
  XStoreColor (display, cmap, &fg_color);
  XStoreColor (display, cmap, &bg_color);
}


#ifdef NO_SELECT
  /* If you don't have select() or usleep(), I guess you lose...
     Maybe you have napms() instead?  Let me know.
   */
# define clock_usleep(usecs) usleep((usecs))

#else

void
clock_usleep (usecs)
     unsigned long usecs;
{
  struct timeval tv;
  tv.tv_sec  = usecs / 1000000L;
  tv.tv_usec = usecs % 1000000L;
  (void) select (0, 0, 0, 0, &tv);
}

#endif

#define TICK_SLEEP()	clock_usleep (80000L)    /* 80/100th second */
#define SEC_SLEEP()	clock_usleep (1000000L)  /* 1 second */

/* The above pair of routines can't be implemented using a combination of
   sleep() and usleep() if you system has them, because they interfere with
   each other: using sleep() will cause later calls to usleep() to fail.
   The select() version is more efficient anyway (fewer system calls.)
 */


void
run_clock ()
{
  int ndigits = minutes_only ? 4 : 6;
  event_loop (display, window); /* wait for initial mapwindow event */
  while (1) {
    int n, tick;
    long clock;

    clock = fill_time_digits ();
    for (n = 0; n < ndigits; n++)
      if (time_digits [n] == last_time_digits [n])
	target_frames [n] = 0;
      else if (time_digits [n] == -1)
	target_frames [n] = clear_frame;
      else
	target_frames [n] = base_frames [time_digits [n]];

    if (twelve_hour_time && target_frames [0] && time_digits [0] == 0)
      {
	target_frames [0] = clear_frame;
	time_digits [0] = -1;
      }
    if (time_digits [0] == -1 && last_time_digits [0] == -1)
      target_frames [0] = 0;
    if (last_time_digits [0] == -2) /* evil hack for 12<->24 mode toggle */
      target_frames [0] = clear_frame;

    for (tick = 9; tick >= 0; tick--) {
      int j;
      if (do_cycle) cycle_colors (display, cmap, &fg_color, &bg_color);
      for (j = 0; j < ndigits; j++)
	{
	  if (target_frames [j])
	    {
	      if (be_a_pig && time_digits [j] == 0 &&
		  last_time_digits [j] != -1)
		{
		  Pixmap p =
		    memory_pig_zeros [last_time_digits [j] - 1] [tick];
		  XCopyPlane (display, p, pixmap, pixmap_draw_gc, 0, 0,
			      character_width, character_height,
			      x_offsets [j], 0, 1);
		}
	      else if (be_a_pig && last_time_digits [j] == 0 &&
		       time_digits [j] != -1)
		{
		  Pixmap p =
		    memory_pig_zeros [time_digits [j] - 1] [9 - tick];
		  XCopyPlane (display, p, pixmap, pixmap_draw_gc, 0, 0,
			      character_width, character_height,
			      x_offsets [j], 0, 1);
		}
	      else if (be_a_pig && last_time_digits [j] != -1 &&
		       time_digits [j] == last_time_digits [j] + 1)
		{
		  Pixmap p =
		    memory_pig_digits [last_time_digits [j] - 1] [9 - tick];
		  XCopyPlane (display, p, pixmap, pixmap_draw_gc, 0, 0,
			      character_width, character_height,
			      x_offsets [j], 0, 1);
		}
	      /* This case isn't terribly common, but we've got it cached,
		 so why not use it. */
	      else if (be_a_pig && time_digits [j] != -1 &&
		       last_time_digits [j] == time_digits [j] + 1)
		{
		  Pixmap p =
		    memory_pig_digits [time_digits [j] - 1] [tick];
		  XCopyPlane (display, p, pixmap, pixmap_draw_gc, 0, 0,
			      character_width, character_height,
			      x_offsets [j], 0, 1);
		}

	      else if (be_a_pig > 1 && last_time_digits [j] != -1 &&
		       time_digits [j] == ((last_time_digits [j] + 2) % 10))
		{
		  Pixmap p =
		    total_oinker_digits [last_time_digits [j] - 1] [9 - tick];
		  XCopyPlane (display, p, pixmap, pixmap_draw_gc, 0, 0,
			      character_width, character_height,
			      x_offsets [j], 0, 1);
		}
	      else if (be_a_pig > 1 && time_digits [j] != -1 &&
		       last_time_digits [j] == ((time_digits [j] + 2) % 10))
		{
		  Pixmap p =
		    total_oinker_digits [time_digits [j] - 1] [tick];
		  XCopyPlane (display, p, pixmap, pixmap_draw_gc, 0, 0,
			      character_width, character_height,
			      x_offsets [j], 0, 1);
		}
	      else
		{
#if 0
		  if (be_a_pig && tick == 9)
		    fprintf (stderr, "cache miss!  %d -> %d\n",
			     last_time_digits [j], time_digits [j]);
#endif
		  /* sends 20 bytes */
		  XFillRectangle (display, pixmap, pixmap_erase_gc,
				  x_offsets [j], 0, character_width + 1,
				  character_height);
		  draw_frame (current_frames [j], pixmap, pixmap_draw_gc,
			      x_offsets [j]);
		}
	      if (tick)
		one_step (current_frames[j], target_frames [j], tick);
	    }
	}
      /* sends up to 1k in non-pig mode */
      flush_segment_buffer (pixmap, pixmap_draw_gc);
      
#ifdef SHAPE
      if (do_shape)
	XShapeCombineMask (display, window, ShapeBounding, 0, 0,
			   pixmap, ShapeSet);
      else
#endif
	/* sends 28 bytes */
	XCopyPlane (display, pixmap, window, window_draw_gc, 0, 0, 
		    x_offsets [ndigits-1] + character_width,
		    character_height, window_offset_x, window_offset_y, 1);
      XFlush (display);
      TICK_SLEEP ();
    }
    bcopy (time_digits, last_time_digits, sizeof (time_digits));

    /* sends up to 1k in non-pig mode */
    flush_segment_buffer (pixmap, pixmap_draw_gc);

#ifdef SHAPE
    if (do_shape)
      XShapeCombineMask (display, window, ShapeBounding, 0, 0,
			 pixmap, ShapeSet);
    else
#endif
      /* sends 28 bytes */
      XCopyPlane (display, pixmap, window, window_draw_gc, 0, 0,
		  x_offsets [ndigits-1] + character_width, character_height,
		  window_offset_x, window_offset_y, 1);

    if (minutes_only)
      {
	/* This is slightly sleazy: when in no-seconds mode, wake up
	   once a second to cycle colors and poll for events, until the
	   minute has expired.  We could do this with an interrupt timer
	   or by selecting on the display file descriptor, but that would
	   be more work.
	 */
	long now = time ((time_t *) 0);
	struct tm *tm = localtime (&now);
	long target = now + (60 - tm->tm_sec);
	while ((now = time ((time_t *) 0)) <= target)
	  {
	    /* if event_loop returns true, we need to go and repaint stuff
	       right now, instead of waiting for the minute to elapse.
	       */
	    if (event_loop (display, window))
	      break;

	    if (now == target-1)	/* the home stretch; sync up */
	      TICK_SLEEP ();
	    else
	      {
		if (do_cycle)
		  cycle_colors (display, cmap, &fg_color, &bg_color);
		SEC_SLEEP ();
	      }
	  }
      }
    else
      {
	/* Sync to the second-strobe by repeatedly sleeping for about
	   1/10th second until time() returns a different value.
	 */
	long now = clock;
	while (clock == now)
	  {
	    TICK_SLEEP ();
	    now = time ((time_t *) 0);
	    event_loop (display, window);
	  }
      }
  }
}


int
event_loop (display, window)
     Display *display;
     Window window;
{
  static int mapped_p = 0;
  int wait_for_buttonup = button_down;
  int redraw_p = 0;
  XSync (display, False);
  while (XPending (display) || !mapped_p ||
	 (wait_for_buttonup && button_down))
    {
      XEvent event;
      XNextEvent (display, &event);
      switch (event.xany.type)
	{
	case KeyPress:
	  {
	    KeySym keysym;
	    XComposeStatus status;
	    char buffer [10];
	    int nbytes = XLookupString (&event.xkey, buffer, sizeof (buffer),
					&keysym, &status);
	    if (nbytes == 0) break;
	    if (nbytes != 1) buffer [0] = 0;
	    switch (buffer [0]) {
	    case 'q': case 'Q': case 3:
	      exit (0);
	    case ' ':
	      twelve_hour_time = !twelve_hour_time;
	      if (twelve_hour_time && time_digits [0] == 0)
		last_time_digits [0] = -2; /* evil hack */
	      redraw_p = 1;
	      break;
	    case '0': case '1': case '2': case '3': case '4': case '5':
	    case '6': case '7': case '8': case '9': case '-':
	      if (event.xkey.state)
		{
		  test_hack = buffer [0];
		  redraw_p = 1;
		  break;
		}
	    default:
	      XBell (display, 0);
	      break;
	    }
	  }
	  break;
	case ButtonPress:
	  button_down++;
	  if (! wait_for_buttonup) draw_colon ();
	  redraw_p = 1;
	  break;
	case ButtonRelease:
	  button_down--;
	  if (! button_down) draw_colon ();
	  redraw_p = 1;
	  break;
	case ConfigureNotify:
	  {
	    int width = x_offsets [minutes_only ? 3 : 5] + character_width;
	    window_offset_x = (event.xconfigure.width - width) / 2;
	    window_offset_y = (event.xconfigure.height - character_height) / 2;
	    XClearWindow (display, window);
	    redraw_p = 1;
	  }
	  break;
	case MapNotify:
	  mapped_p = 1;
	  redraw_p = 1;
	  break;
	case UnmapNotify:
	  mapped_p = 0;
	  break;
	}
    }
  return redraw_p;
}


void
usage ()
{
  fprintf (stderr, "%s\n\
usage: %s [ switches ]\nwhere switches contain\n\n\
  -12				Display twelve hour time (default).\n\
  -24				Display twenty-four hour time.\n\
  -seconds			Display seconds (default).\n\
  -noseconds			Don't display seconds.\n\
  -cycle			Do color-cycling.\n\
  -nocycle			Don't do color-cycling (default).\n\
  -display <host:dpy>		The display to run on.\n\
  -geometry <geometry>		Size and position of window.\n\
  -foreground or -fg <color>	Foreground color (default black).\n\
  -background or -bg <color>	Background color (default white).\n\
  -reverse or -rv		Swap foreground and background.\n\
  -borderwidth or -bw <int>	Border width.\n\
  -title <string>		Window title.\n\
  -name <string>		Resource-manager class name (XDaliClock).\n\
", version, progname);
#ifdef BUILTIN_FONT
  fprintf (stderr, "\
  -font	or -fn <font>		Name of an X font to use, or the string\n\
				\"BUILTIN\", meaning to use the large builtin\n\
				font (this is the default).\n\
  -builtin			Same as -font BUILTIN.\n\
");
#else
  fprintf (stderr, "\
  -font	or -fn <font>		Name of an X font to use.\n\
");
#endif
  fprintf (stderr, "\
  -memory <high|medium|low>	Tune the memory versus bandwidth consumption;\n\
				default is \"low\".\n\
  -oink				Same as -memory medium.\n\
  -oink-oink			Same as -memory high.\n\
");
#ifdef SHAPE
  fprintf (stderr, "\
  -shape			Use the Shape extension if available.\n\
  -noshape			Don't.\n");
#endif
#if !defined (BUILTIN_FONT) || !defined (SHAPE)
  fprintf (stderr, "\n");
# ifndef BUILTIN_FONT
   fprintf (stderr, "This version has been compiled without the builtin font");
# endif
# ifndef SHAPE
#  ifndef BUILTIN_FONT
  fprintf (stderr, ", or support for the\nShape Extension.\n");
#  else
  fprintf (stderr, "\
This version has been compiled without support for the Shape Extension.\n");
#  endif
# else
  fprintf (stderr, ".\n");
# endif
#endif
  exit (-1);
}

char *
get_string (res_name, res_class)
     char *res_name, *res_class;
{
  XrmValue value;
  char	*type, *p;
  char full_name [1024], full_class [1024];
  strcpy (full_name, progname);
  strcat (full_name, ".");
  strcat (full_name, res_name);
  strcpy (full_class, app_class);
  strcat (full_class, ".");
  strcat (full_class, res_class);
  if (XrmGetResource (db, full_name, full_class, &type, &value))
    {
      char *str = (char *) malloc (value.size + 1);
      strncpy (str, (char *) value.addr, value.size);
      str [value.size] = 0;
      return str;
    }
  return 0;
}

int 
get_boolean (res_name, res_class)
     char *res_name, *res_class;
{
  char *tmp, buf [100];
  char *s = get_string (res_name, res_class);
  if (! s) return 0;
  for (tmp = buf; *s; s++)
    *tmp++ = isupper (*s) ? _tolower (*s) : *s;
  *tmp = 0;

  if (!strcmp (buf, "on") || !strcmp (buf, "true") || !strcmp (buf, "yes"))
    return 1;
  if (!strcmp (buf,"off") || !strcmp (buf, "false") || !strcmp (buf,"no"))
    return 0;
  fprintf (stderr, "%s: %s must be boolean, not %s.\n",
	   progname, res_name, buf);
  return 0;
}

void
get_piggedness ()
{
  char *tmp, buf [100];
  char *s = get_string ("memory", "Memory");
  be_a_pig = 0;
  if (! s) return;
  for (tmp = buf; *s; s++)
    *tmp++ = isupper (*s) ? _tolower (*s) : *s;
  *tmp = 0;

  if (!strcmp (buf, "high") || !strcmp (buf, "hi") || !strcmp (buf, "sleazy"))
    be_a_pig = 2;
  else if (!strcmp (buf, "medium") || !strcmp (buf, "med"))
    be_a_pig = 1;
  else if (!strcmp (buf, "low") || !strcmp (buf, "lo"))
    be_a_pig = 0;
  else
    fprintf (stderr,
      "%s: memory must be \"high\", \"medium\", or \"low\", not \"%s\".\n",
	     progname, buf);
}

int 
get_integer (res_name, res_class)
     char *res_name, *res_class;
{
  int val;
  char *s = get_string (res_name, res_class);
  if (!s) return 0;
  if (1 == sscanf (s, " %d %c", &val))
    return val;
  fprintf (stderr, "%s: %s must be an integer, not %s.\n",
	   progname, res_name, s);
  return 0;
}


int 
get_time_mode (res_name, res_class)
     char *res_name, *res_class;
{
  char *s = get_string (res_name, res_class);
  if (! s) return 1;
  if (!strcmp (s, "12")) return 1;
  if (!strcmp (s, "24")) return 0;
  fprintf (stderr, "%s: %s must be \"12\" or \"24\", not \"%s\".\n",
	   progname, res_name, s);
  return 1;
}


main (argc, argv)
     int argc;
     char **argv;
{
  char *display_name = 0;
  char *tmp, *font, *fg, *bg, *geom, *def_geom;
  int i, bw, rv;
  XrmDatabase cmdDB;
  char *getenv ();

  progname = argv [0];
  if (tmp = rindex (progname, '/'))
    progname = tmp+1;
  
  XrmParseCommand (&db, xdaliclock_options, num_options, progname, &argc, argv);
  if (argc > 1)
    {
      if (strcmp (argv [1], "-help"))
	fprintf (stderr, "%s: unknown option \"%s\"\n\n", progname, argv [1]);
      usage ();
    }

  app_class = "XDaliClock";
  tmp = get_string ("name", "Name");
  if (tmp) app_class = tmp;

  display_name = get_string ("display", "Display");
  if (! display_name) display_name = getenv ("DISPLAY");
  if (!display_name)  display_name = ":0";
  display = XOpenDisplay (display_name);
  if (! display)
    {
      fprintf(stderr, "%s: couldn't open display %s\n", progname, display_name);
      exit (-1);
    }

  geom = get_string ("geometry", "Geometry");
  cmdDB = db;
  db = XrmGetStringDatabase (xdaliclockDefaults);
  
  if (tmp = XResourceManagerString (display))
    XrmMergeDatabases (XrmGetStringDatabase (tmp), &db);

  if (getenv ("XENVIRONMENT") && access (getenv ("XENVIRONMENT")) == 0)
    XrmMergeDatabases (XrmGetFileDatabase (getenv ("XENVIRONMENT")), &db);
  else
    {
      char fn [1000];
      sprintf (fn, "%s/.Xdefaults", getenv("HOME"));
      if (access (fn, R_OK) == 0)
	XrmMergeDatabases (XrmGetFileDatabase (fn), &db);
    }
  XrmMergeDatabases (cmdDB, &db);

  fg	= get_string ("foreground", "Foreground");
  bg	= get_string ("background", "Background");
  bw	= get_integer ("borderWidth", "BorderWidth");
  rv	= get_boolean ("reverseVideo", "ReverseVideo");
  font	= get_string ("font", "Font");
  window_title	= get_string ("title", "Title");
  def_geom	= get_string ("geometry", "Geometry");
  do_cycle	= get_boolean ("cycle", "Cycle");
  minutes_only	= !get_boolean ("seconds", "Seconds");
  twelve_hour_time = get_time_mode ("mode", "Mode");
  get_piggedness ();
#ifdef BUILTIN_FONT
  use_builtin_font = (!font ||
		      !strcmp (font, "BUILTIN") ||
		      !strcmp (font, "builtin"));
#endif
#ifdef SHAPE
  do_shape = get_boolean ("shape", "Shape");
#endif
  if (rv) tmp = fg, fg = bg, bg = tmp;

  initialize (font);
  initialize_window (fg, bg, bw, geom, def_geom);
  run_clock ();
}
