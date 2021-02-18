#undef False
#undef True
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#undef index
/*
  Define declarations.
*/
#define WindowBorderWidth  2
#define XSetWindowExtents(window,text,border)  \
  window->width=XTextWidth(window->font_info,text,strlen(text))+  \
    border*window->font_info->max_bounds.width;  \
  window->height=window->font_info->ascent+window->font_info->descent+4;  \
  XResizeWindow(display,window->id,window->width,window->height);
#define XStandardPixel(map,color,shift)  (unsigned long) (map->base_pixel+  \
  ((color.red*map->red_max+(1 << (shift-1))) >> shift)*map->red_mult+  \
  ((color.green*map->green_max+(1 << (shift-1))) >> shift)*map->green_mult+  \
  ((color.blue*map->blue_max+(1 << (shift-1))) >> shift)*map->blue_mult)

/*
  Typedef declarations.
*/
typedef struct _XPixelInfo
{
  unsigned int
    colors;

  unsigned long
    *pixels;

  XColor
    background_color,
    border_color,
    foreground_color,
    pen_color[9],
    annotate_color;

  unsigned short
    background_index,
    annotate_index;
} XPixelInfo;

typedef struct _XResourceInfo
{
  unsigned int
    backdrop;

  char
    *background_color;

  char
    *border_color;

  unsigned int
    border_width,
    compression,
    delay,
    dither;

  char
    *font,
    *font_name[9],
    *foreground_color,
    *icon_geometry;

  unsigned int
    iconic;

  char
    *image_geometry;

  unsigned int
    magnify;

  char
    *map_type;

  unsigned int
    monochrome;

  char
    *name;

  unsigned int
    number_colors;

  char
    *pen_color[9];

  char
    *print_filename,
    *title;

  unsigned int
    tree_depth;

  char
    *visual_type,
    *write_filename;
} XResourceInfo;

typedef struct _XWindowInfo
{
  Window
    id;

  int
    screen,
    depth;

  XVisualInfo
    *visual_info;

  XStandardColormap
    *map_info;

  XPixelInfo
    *pixel_info;

  XFontStruct
    *font_info;

  GC
    graphic_context,
    highlight_context;

  Cursor
    cursor,
    busy_cursor;

  char
    *name,
    *geometry,
    *icon_geometry,
    *clip_geometry;

  unsigned long
    flags;

  int
    x,
    y;

  unsigned int
    width,
    height,
    min_width,
    min_height,
    width_inc,
    height_inc,
    border_width,
    immutable;

  XImage
    *ximage;

  Pixmap
    *pixmaps;

  XSetWindowAttributes
    attributes;
} XWindowInfo;

/*
  X utilities routines.
*/
extern char 
  *XGetResource(),
  *XVisualClassName();

extern Image 
  *XReadImage();

extern XStandardColormap 
  *XMakeStandardColormap();

extern int
  XError();

extern unsigned int 
  IsTrue(),
  XAnnotateImage(),
  XPopUpMenu(),
  XReadColormap();

extern unsigned long 
  XBestPixel();

extern XVisualInfo 
  *XBestVisualInfo();

extern void 
  Latin1Upper(),
  XBestIconSize(),
  XGetPixelInfo(),
  XGetResourceInfo(),
  XMakeImageLSBFirst(),
  XMakeImageMSBFirst(),
  XMakeWindow(),
  XPopUpAlert(),
  XPopUpQuery();

extern Window 
  XClientWindow(),
  XSelectWindow(),
  XWindowByID(),
  XWindowByName(),
  XWindowByProperty();

extern XFontStruct 
  *XBestFont();

extern XImage 
  *XMakeImage();

/*
  Invoke pre-X11R5 ICCCM routines if XlibSpecificationRelease is not defined.
*/
#ifndef XlibSpecificationRelease
#define PRE_R5_ICCCM
#endif
/*
  Invoke pre-X11R4 ICCCM routines if PWinGravity is not defined.
*/
#ifndef PWinGravity
#define PRE_R4_ICCCM
#endif
#include "PreRvIcccm.h"
