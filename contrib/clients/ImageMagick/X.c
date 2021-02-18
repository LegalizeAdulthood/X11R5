/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%                                  X   X                                      %
%                                   X X                                       %
%                                    X                                        %
%                                   X X                                       %
%                                  X   X                                      %
%                                                                             %
%                       X11 Utility Routines for Display.                     %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              July 1991                                      %
%                                                                             %
%                                                                             %
%  Copyright 1991 E. I. du Pont de Nemours & Company                          %
%                                                                             %
%  Permission to use, copy, modify, distribute, and sell this software and    %
%  its documentation for any purpose is hereby granted without fee,           %
%  provided that the above Copyright notice appear in all copies and that     %
%  both that Copyright notice and this permission notice appear in            %
%  supporting documentation, and that the name of E. I. du Pont de Nemours    %
%  & Company not be used in advertising or publicity pertaining to            %
%  distribution of the software without specific, written prior               %
%  permission.  E. I. du Pont de Nemours & Company makes no representations   %
%  about the suitability of this software for any purpose.  It is provided    %
%  "as is" without express or implied warranty.                               %
%                                                                             %
%  E. I. du Pont de Nemours & Company disclaims all warranties with regard    %
%  to this software, including all implied warranties of merchantability      %
%  and fitness, in no event shall E. I. du Pont de Nemours & Company be       %
%  liable for any special, indirect or consequential damages or any           %
%  damages whatsoever resulting from loss of use, data or profits, whether    %
%  in an action of contract, negligence or other tortious action, arising     %
%  out of or in connection with the use or performance of this software.      %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "display.h"
#include "image.h"
#include "X.h"
/*
  External declarations.
*/
extern void
  Error();

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s T r u e                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function IsTrue returns True if the boolean is "true", "on", "yes" or "1".
%
%  The format of the IsTrue routine is:
%
%      option=IsTrue(boolean)
%
%  A description of each parameter follows:
%
%    o option: either True or False depending on the boolean parameter.
%
%    o boolean: Specifies a pointer to a character array.
%
%
*/
unsigned int IsTrue(boolean)
char
  *boolean;
{
  char
    c,
    *p;

  if (boolean == (char *) NULL)
    return(False);
  for (p=boolean; *p != (char) NULL; p++)
  {
    /*
      Convert to lower case.
    */
    c=(*p);
    if (isascii(c) && isupper(c))
      *p=tolower(c);
  }
  if (strcmp(boolean,"true") == 0)
    return(True);
  if (strcmp(boolean,"on") == 0)
    return(True);
  if (strcmp(boolean,"yes") == 0)
    return(True);
  if (strcmp(boolean,"1") == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L a t i n 1 U p p e r                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Latin1Upper converts a string to upper-case Latin1.
%
%  The format of the Latin1Upper routine is:
%
%      Latin1Upper(string)
%
%  A description of each parameter follows:
%
%    o string: A pointer to the string to convert to upper-case Latin1.
%
%
*/
void Latin1Upper(string)
unsigned char
  *string;
{
  unsigned char
    c;

  c=(*string);
  while (c != (char) NULL)
  {
    if ((c >= XK_a) && (c <= XK_z))
      *string=c-(XK_a-XK_A);
    else
      if ((c >= XK_agrave) && (c <= XK_odiaeresis))
        *string=c-(XK_agrave-XK_Agrave);
      else
        if ((c >= XK_oslash) && (c <= XK_thorn))
          *string=c-(XK_oslash-XK_Ooblique);
    string++;
    c=(*string);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X A n n o t a t e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XAnnotateImage annotates the image with text.
%
%  The format of the XAnnotateImage routine is:
%
%    status=XAnnotateImage(display,font_info,image_window,annotate_text,
%      annotate_geometry,background,image)
%
%  A description of each parameter follows:
%
%    o status: Function XAnnotateImage returns True if the image is
%      successfully annotated with text.  False is returned is there is a
%      memory shortage.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o font_info: Specifies a pointer to a XFontStruct structure.
%
%    o image_window: Specifies a pointer to a XWindowInfo structure.
%
%    o text: Specifies the text to annotate the image.
%
%    o annotate_geometry: Specifies the size and location of the annotated 
%      image.
%
%    o background: Specifies whether the background color is included in
%      the annotation.  Must be either True or False;
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
unsigned int XAnnotateImage(display,font_info,image_window,annotate_text,
  annotate_geometry,background,image)
Display
  *display;

XFontStruct
  *font_info;

XWindowInfo
  *image_window;

char
  *annotate_text;

char
  *annotate_geometry;

unsigned int
  background;

Image
  *image;
{
  GC
    graphic_context;

  Image
    *annotated_image;

  int
    annotate_x,
    annotate_y;

  Pixmap
    annotated_pixmap;

  register int
    x,
    y;

  register RunlengthPacket
    *p,
    *q;

  unsigned int
    annotate_height,
    annotate_width,
    height,
    width;

  XGCValues
    graphic_context_value;

  XImage
    *annotated_ximage;

  XPixelInfo
    *pixel_info;

  /*
    Initialize annotated image.
  */
  if (image->compression != NoCompression)
    if (!UncompressImage(image))
      return(False);
  /*
    Initialize annotated pixmap.
  */
  width=XTextWidth(font_info,annotate_text,strlen(annotate_text));
  height=font_info->ascent+font_info->descent;
  annotated_pixmap=XCreatePixmap(display,image_window->id,width,height,
    image_window->depth);
  if (annotated_pixmap == (Pixmap) NULL)
    return(False);
  /*
    Initialize graphics info.
  */
  graphic_context_value.background=0;
  graphic_context_value.foreground=1;
  graphic_context_value.font=font_info->fid;
  graphic_context=XCreateGC(display,image_window->id,GCBackground | GCFont |
    GCForeground,&graphic_context_value);
  if (graphic_context == (GC) NULL)
    return(False);
  /*
    Draw text to pixmap.
  */
  XDrawImageString(display,annotated_pixmap,graphic_context,0,
    font_info->ascent,annotate_text,strlen(annotate_text));
  XFreeGC(display,graphic_context);
  /*
    Initialize annotated X image.
  */
  annotated_ximage=XGetImage(display,annotated_pixmap,0,0,width,height,
    AllPlanes,ZPixmap);
  if (annotated_ximage == (XImage *) NULL)
    return(False);
  XFreePixmap(display,annotated_pixmap);
  /*
    Initialize annotated  image.
  */
  annotated_image=CopyImage(image,width,height,False);
  if (annotated_image == (Image *) NULL)
    return(False);
  /*
    Transfer annotated X image to image.
  */
  q=annotated_image->pixels;
  for (y=0; y < height; y++)
  {
    for (x=0; x < width; x++)
    {
      q->index=XGetPixel(annotated_ximage,x,y);
      q->length=0;
      q++;
    }
  }
  XDestroyImage(annotated_ximage);
  /*
    Determine annotate geometry.
  */
  (void) XParseGeometry(annotate_geometry,&annotate_x,&annotate_y,
    &annotate_width,&annotate_height);
  if ((annotate_height != height) || (annotate_height != height))
    {
      Image
        *scaled_image;

      /*
        Scale image.
      */
      scaled_image=ScaleImage(annotated_image,annotate_width,annotate_height);
      if (scaled_image == (Image *) NULL)
        return(False);
      if (annotated_image != image)
        DestroyImage(annotated_image);
      annotated_image=scaled_image;
    }
  /*
    Paste annotated image to image.
  */
  pixel_info=image_window->pixel_info;
  image->class=DirectClass;
  p=annotated_image->pixels;
  q=image->pixels+annotate_y*image->columns+annotate_x;
  for (y=0; y < annotated_image->rows; y++)
  {
    for (x=0; x < annotated_image->columns; x++)
    {
      if (p->index == 1)
        {
          /*
            Set this pixel to the pen color.
          */
          q->red=pixel_info->annotate_color.red >> 8;
          q->green=pixel_info->annotate_color.green >> 8;
          q->blue=pixel_info->annotate_color.blue >> 8;
          q->index=pixel_info->annotate_index;
        }
      else
        if (background)
          {
            /*
              Set this pixel to the pen color.
            */
            q->red=pixel_info->background_color.red >> 8;
            q->green=pixel_info->background_color.green >> 8;
            q->blue=pixel_info->background_color.blue >> 8;
            q->index=pixel_info->background_index;
          }
      p++;
      q++;
    }
    q+=image->columns-annotated_image->columns;
  }
  DestroyImage(annotated_image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t F o n t                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XBestFont returns the "best" font.  "Best" is defined as a font
%  specified in the X resource database or a font such that the text width
%  displayed with the font does not exceed the specified maximum width.
%
%  The format of the XBestFont routine is:
%
%      font=XBestFont(display,resource_info,text,maximum_width)
%
%  A description of each parameter follows:
%
%    o font: XBestFont returns a pointer to a XFontStruct structure.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o text: Specifies the text whose width is compared to the maximum.
%
%    o maximum_width: Specifies the maximum width in pixels of the text.
%
%
*/
XFontStruct *XBestFont(display,resource_info,text,maximum_width)
Display
  *display;

XResourceInfo
  *resource_info;

char
  *text;

unsigned int
  maximum_width;
{
  static char
    *fonts[]=
    {
      "fixed",
      "6x10",
      "5x8",
      (char *) NULL
    };

  char
    *font_name,
    **p;

  unsigned int
    width;

  XFontStruct
    *font_info;

  font_info=(XFontStruct *) NULL;
  font_name=resource_info->font;
  if (font_name != (char *) NULL)
    {
      /*
        Load preferred font specified in the X resource database.
      */
      font_info=XLoadQueryFont(display,font_name);
      if (font_info == (XFontStruct *) NULL)
        Warning("unable to load font",font_name);
    }
  /*
    Load a font that does not exceed the text width.
  */
  for (p=fonts; *p != (char *) NULL; p++)
  {
    if (font_info != (XFontStruct *) NULL)
      {
        width=XTextWidth(font_info,text,strlen(text))+
          2*font_info->max_bounds.width+2*WindowBorderWidth;
        if (width < maximum_width)
          break;
        font_name=(*p);
        XFreeFont(display,font_info);
      }
    font_info=XLoadQueryFont(display,*p);
  }
  if (font_info == (XFontStruct *) NULL)
    font_info=XLoadQueryFont(display,"fixed");  /* backup font */
  if (font_info == (XFontStruct *) NULL)
    font_info=XLoadQueryFont(display,"variable");  /* backup font */
  return(font_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t I c o n S i z e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XBestIconSize returns the "best" icon size.  "Best" is defined as
%  an icon size that maintains the aspect ratio of the image.  If the window
%  manager has preferred icon sizes, one of the preferred sizes is used.
%
%  The format of the XBestIconSize routine is:
%
%      XBestIconSize(display,icon_window,image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
void XBestIconSize(display,icon_window,image)
Display
  *display;

XWindowInfo
  *icon_window;

Image
  *image;
{
#define MaxIconSize  64

  int
    number_sizes;

  unsigned long
    icon_height,
    icon_width,
    scale_factor;

  Window
    root_window;

  XIconSize
    *icon_size,
    *size_list;

  /*
    Determine if the window manager has specified preferred icon sizes.
  */
  icon_size=(XIconSize *) NULL;
  number_sizes=0;
  root_window=XRootWindow(display,icon_window->screen);
  if (XGetIconSizes(display,root_window,&size_list,&number_sizes) != 0)
    if ((number_sizes > 0) && (size_list != (XIconSize *) NULL))
      icon_size=size_list;
  if (icon_size == (XIconSize *) NULL)
    {
      /*
        Window manager does not restrict icon size.
      */
      icon_size=XAllocIconSize();
      if (icon_size == (XIconSize *) NULL)
        Error("unable to create icon","memory allocation failed");
      icon_size->min_width=1;
      icon_size->max_width=MaxIconSize;
      icon_size->min_height=1;
      icon_size->max_height=MaxIconSize;
      icon_size->width_inc=1;
      icon_size->height_inc=1;
    }
  /*
    Look for an icon size that maintains the aspect ratio of image.
  */
  scale_factor=Min((icon_size->max_width << 14)/image->columns,
    (icon_size->max_height << 14)/image->rows);
  icon_width=icon_size->min_width;
  while (icon_width < icon_size->max_width)
  {
    if (icon_width >= ((image->columns*scale_factor+8191) >> 14))
      break;
    icon_width+=icon_size->width_inc;
  }
  icon_height=icon_size->min_height;
  while (icon_height < icon_size->max_height)
  {
    if (icon_height >= ((image->rows*scale_factor+8191) >> 14))
      break;
    icon_height+=icon_size->height_inc;
  }
  XFree((void *) icon_size);
  icon_window->width=icon_width;
  icon_window->height=icon_height;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t P i x e l                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XBestPixel returns a pixel from an array of pixels that is closest
%  to the requested color.
%
%  The format of the XBestPixel routine is:
%
%      pixel=XBestPixel(colors,number_colors,color)
%
%  A description of each parameter follows:
%
%    o pixel: XBestPixel returns the pixel value closest to the requested
%      color.
%
%    o colors: Specifies an array of XColor structures.
%
%    o number_colors: Specifies the number of XColor structures in the
%      color definition array.
%
%    o color: Specifies the desired RGB value to find in the colors array.
%
%
*/
unsigned long XBestPixel(colors,number_colors,color)
XColor
  *colors;

unsigned int
  number_colors;

XColor
  color;
{
  register int
    blue_distance,
    green_distance,
    i,
    red_distance;

  register unsigned long
    distance,
    min_distance,
    pixel;

  /*
    Find closest representation for the requested RGB color.
  */
  pixel=0;
  min_distance=(~0);
  for (i=0; i < number_colors; i++)
  {
    red_distance=(colors[i].red >> 8)-(color.red >> 8);
    green_distance=(colors[i].green >> 8)-(color.green >> 8);
    blue_distance=(colors[i].blue >> 8)-(color.blue >> 8);
    distance=red_distance*red_distance+green_distance*green_distance+
      blue_distance*blue_distance;
    if (distance < min_distance)
      {
        min_distance=distance;
        pixel=colors[i].pixel;
      }
  }
  return(pixel);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t V i s u a l I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XBestVisualInfo returns visual information for a visual that is
%  the "best" the server supports.  "Best" is defined as:
%
%    1. Restrict the visual list to those supported by the default screen.
%
%    2. If a visual type is specified, restrict the visual list to those of
%       that type.
%
%    3. If a map type is specified, choose the visual that matches the id
%       specified by the Standard Colormap.
%
%    4  From the list of visuals, choose one that can display the most
%       simultaneous colors.  If more than one visual can display the same
%       number of simultaneous colors, one is choosen based on a rank.
%
%  The format of the XBestVisualInfo routine is:
%
%      visual_info=XBestVisualInfo(display,visual_type,map_type,map_info)
%
%  A description of each parameter follows:
%
%    o visual_info: XBestVisualInfo returns a pointer to a X11 XVisualInfo
%      structure.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o visual_type: Specifies the desired visual type.
%
%    o map_type: Specifies the desired Standard Colormap type.
%
%    o map_info: If map_type is specified, this structure is initialized
%      with info from the Standard Colormap.
%
%
*/
XVisualInfo *XBestVisualInfo(display,visual_type,map_type,map_info)
Display
  *display;

char
  *visual_type,
  *map_type;

XStandardColormap
  **map_info;
{
#define XVisualColormapSize(visual_info) \
  ((visual_info->class == TrueColor) || (visual_info->class == DirectColor) ? \
    visual_info->red_mask | visual_info->green_mask | visual_info->blue_mask : \
    visual_info->colormap_size)

  int
    number_visuals;

  register int
    i;

  unsigned int
    visual_mask;

  XVisualInfo
    *visual_info,
    *visual_list,
    visual_template;

  /*
    Restrict visual search by screen number.
  */
  visual_mask=VisualScreenMask;
  visual_template.screen=XDefaultScreen(display);
  if (visual_type != (char *) NULL)
    {
      register char
        *p;

      register int
        c;

      /*
        Restrict visual search by class or visual id.
      */
      p=visual_type;
      for ( ; *p != (char) NULL; p++)
      {
        /*
          Convert to lower case.
        */
        c=(*p);
        if (isascii(c) && isupper(c))
          *p=tolower(c);
      }
      if (strcmp("staticgray",visual_type) == 0)
        {
          visual_mask|=VisualClassMask;
          visual_template.class=StaticGray;
        }
      else
        if (strcmp("grayscale",visual_type) == 0)
          {
            visual_mask|=VisualClassMask;
            visual_template.class=GrayScale;
          }
        else
          if (strcmp("staticcolor",visual_type) == 0)
            {
              visual_mask|=VisualClassMask;
              visual_template.class=StaticColor;
            }
          else
            if (strcmp("pseudocolor",visual_type) == 0)
              {
                visual_mask|=VisualClassMask;
                visual_template.class=PseudoColor;
              }
            else
              if (strcmp("truecolor",visual_type) == 0)
                {
                  visual_mask|=VisualClassMask;
                  visual_template.class=TrueColor;
                }
              else
                if (strcmp("directcolor",visual_type) == 0)
                  {
                    visual_mask|=VisualClassMask;
                    visual_template.class=DirectColor;
                  }
                else
                  if (strcmp("default",visual_type) == 0)
                    {
                      visual_mask|=VisualIDMask;
                      visual_template.visualid=XVisualIDFromVisual(
                        XDefaultVisual(display,XDefaultScreen(display)));
                    }
                  else
                    if (isdigit(*visual_type))
                      {
                        visual_mask|=VisualIDMask;
                        visual_template.visualid=
                          strtol(visual_type,(char **) NULL,0);
                      }
                    else
                      Error("invalid visual specifier",visual_type);
    }
  /*
    Get all visuals that meet our criteria so far.
  */
  number_visuals=0;
  visual_list=XGetVisualInfo(display,visual_mask,&visual_template,
    &number_visuals);
  visual_mask=VisualScreenMask | VisualIDMask;
  if ((number_visuals == 0) || (visual_list == (XVisualInfo *) NULL))
    {
      /*
        Failed to get visual;  try using the default visual.
      */
      Warning("unable to get visual",visual_type);
      visual_template.visualid=
        XVisualIDFromVisual(XDefaultVisual(display,XDefaultScreen(display)));
      visual_list=XGetVisualInfo(display,visual_mask,&visual_template,
        &number_visuals);
      if ((number_visuals == 0) || (visual_list == (XVisualInfo *) NULL))
        return((XVisualInfo *) NULL);
      Warning("using default visual",XVisualClassName(visual_list));
    }
  if (map_type != (char *) NULL)
    {
      Atom
        map_property;

      int
        j,
        number_maps,
        status;

      unsigned char
        map_name[1024];

      Window
        root_window;

      XStandardColormap
        *map_list;

      /*
        Restrict visual search by Standard Colormap visual id.
      */
      (void) sprintf((char *) map_name,"RGB_%s_MAP",map_type);
      Latin1Upper(map_name);
      map_property=XInternAtom(display,(char *) map_name,True);
      if (map_property == (Atom) NULL)
        Error("unable to get Standard Colormap",map_type);
      root_window=XRootWindow(display,XDefaultScreen(display));
      status=XGetRGBColormaps(display,root_window,&map_list,&number_maps,
        map_property);
      if (status == 0)
        Error("unable to get Standard Colormap",map_type);
      /*
        Search all Standard Colormaps and visuals for ids that match.
      */
      *map_info=XAllocStandardColormap();
      if (*map_info == (XStandardColormap *) NULL)
        Error("unable to create Standard Colormap","memory allocation failed");
      **map_info=map_list[0];
#ifndef PRE_R4_ICCCM
      visual_template.visualid=XVisualIDFromVisual(visual_list[0].visual);
      for (i=0; i < number_maps; i++)
        for (j=0; j < number_visuals; j++)
          if (map_list[i].visualid ==
              XVisualIDFromVisual(visual_list[j].visual))
            {
              **map_info=map_list[i];
              visual_template.visualid=
                XVisualIDFromVisual(visual_list[j].visual);
              break;
            }
      if ((*map_info)->visualid != visual_template.visualid)
        Error("unable to match visual to Standard Colormap",map_type);
#endif
      if ((*map_info)->colormap == (Colormap) NULL)
        Error("Standard Colormap is not initialized",map_type);
      XFree((void *) map_list);
    }
  else
    {
      static unsigned int
        rank[]=
          {
            StaticGray,
            GrayScale,
            StaticColor,
            DirectColor,
            TrueColor,
            PseudoColor
          };

      XVisualInfo
        *p;

      /*
        Pick one visual that displays the most simultaneous colors.
      */
      visual_info=visual_list;
      p=visual_list;
      for (i=1; i < number_visuals; i++)
      {
        p++;
        if (XVisualColormapSize(p) > XVisualColormapSize(visual_info))
          visual_info=p;
        else
          if (XVisualColormapSize(p) == XVisualColormapSize(visual_info))
            if (rank[p->class] > rank[visual_info->class])
              visual_info=p;
      }
      visual_template.visualid=XVisualIDFromVisual(visual_info->visual);
    }
  XFree((void *) visual_list);
  /*
    Retrieve only one visual by its screen & id number.
  */
  visual_info=XGetVisualInfo(display,visual_mask,&visual_template,
    &number_visuals);
  if ((number_visuals == 0) || (visual_info == (XVisualInfo *) NULL))
    return((XVisualInfo *) NULL);
  return(visual_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X C l i e n t W i n d o w                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XClientWindow finds a window, at or below the specified window,
%  which has a WM_STATE property.  If such a window is found, it is returned,
%  otherwise the argument window is returned.
%
%  The format of the XClientWindow function is:
%
%      client_window=XClientWindow(display,target_window)
%
%  A description of each parameter follows:
%
%    o client_window: XClientWindow returns a window, at or below the specified
%      window, which has a WM_STATE property otherwise the argument
%      target_window is returned.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o target_window: Specifies the window to find a WM_STATE property.
%
%
*/
Window XClientWindow(display,target_window)
Display
  *display;

Window
  target_window;
{
  Atom
    state,
    type;

  int
    format;

  unsigned char
    *data;

  unsigned long
    after,
    number_items;

  Window
    client_window;

  state=XInternAtom(display,"WM_STATE",True);
  if (state == (Atom) NULL)
    return(target_window);
  type=(Atom) NULL;
  (void) XGetWindowProperty(display,target_window,state,0L,0L,False,
    AnyPropertyType,&type,&format,&number_items,&after,&data);
  if (type != (Atom) NULL)
    return(target_window);
  client_window=XWindowByProperty(display,target_window,state);
  if (client_window == (Window) NULL)
    return(target_window);
  return(client_window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X E r r o r                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XError ignores BadWindow errors for XQueryTree and 
%  XGetWindowAttributes, and ignores BadDrawable errors for XGetGeometry, and
%  ignores BadValue errors for XQueryColor.  It returns False in those cases.  
%  Otherwise it returns True.
%
%  The format of the XError function is:
%
%      XError(display,error)
%
%  A description of each parameter follows:
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o error: Specifies the error event.
%
%
*/
int XError(display,error)
Display 
  *display;

XErrorEvent 
  *error;
{
  switch (error->request_code) 
  {
    case X_GetGeometry:
    {
      if (error->error_code == BadDrawable) 
        return(False);
      break;
    }
    case X_GetWindowAttributes:
    case X_QueryTree:
    {
      if (error->error_code == BadWindow) 
        return(False);
      break;
    }
    case X_QueryColors:
    {
      if (error->error_code == BadValue) 
        return(False);
      break;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeImage creates an X11 image.  If the image size differs from
%  the X11 image size, the image is first resized.
%
%  The format of the XMakeImage routine is:
%
%      ximage=XMakeImage(display,resource_info,image_window,image,columns,
%        height)
%
%  A description of each parameter follows:
%
%    o ximage: Specifies a pointer to a XImage structure;  returned from
%      XMakeImage.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o image_window: Specifies a pointer to a XWindowInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o width: Specifies the width in pixels of the rectangular area to
%      display.
%
%    o height: Specifies the height in pixels of the rectangular area to
%      display.
%
%
*/
XImage *XMakeImage(display,resource_info,image_window,image,width,height)
Display
  *display;

XResourceInfo
  *resource_info;

XWindowInfo
  *image_window;

Image
  *image;

unsigned int
  width,
  height;
{
  Image
    *transformed_image;

  XImage
    *ximage;

  /*
    Display busy cursor.
  */
  XDefineCursor(display,image_window->id,image_window->busy_cursor);
  XFlush(display);
  /*
    Create X image.
  */
  ximage=XCreateImage(display,image_window->visual_info->visual,
    (resource_info->monochrome ? 1 : image_window->depth),
    (((image_window->depth == 1) || resource_info->monochrome) ? XYBitmap :
    ZPixmap),0,(char *) NULL,width,height,XBitmapPad(display),0);
  if (ximage == (XImage *) NULL)
    {
      /*
        Unable to X image.
      */
      XDefineCursor(display,image_window->id,image_window->cursor);
      return((XImage *) NULL);
    }
  /*
    Allocate X image pixel data.
  */
  if (ximage->format == XYBitmap)
    ximage->data=(char *) malloc((unsigned int)
      ximage->bytes_per_line*ximage->height*ximage->depth);
  else
    ximage->data=(char *)
      malloc((unsigned int) ximage->bytes_per_line*ximage->height);
  if (ximage->data == (char *) NULL)
    {
      /*
        Unable to allocate pixel data.
      */
      XDestroyImage(ximage);
      XDefineCursor(display,image_window->id,image_window->cursor);
      return((XImage *) NULL);
    }
  if (image == (Image *) NULL)
    {
      XDefineCursor(display,image_window->id,image_window->cursor);
      return(ximage);
    }
  transformed_image=image;
  if (image_window->clip_geometry)
    {
      Image
        *clipped_image;

      int
        clip_x,
        clip_y;

      unsigned
        clip_height,
        clip_width;

      /*
        Clip image.
      */
      (void) XParseGeometry(image_window->clip_geometry,&clip_x,&clip_y,
        &clip_width,&clip_height);
      clipped_image=
        ClipImage(transformed_image,clip_x,clip_y,clip_width,clip_height);
      if (clipped_image != (Image *) NULL)
        {
          if (transformed_image != image)
            DestroyImage(transformed_image);
          transformed_image=clipped_image;
        }
    }
  if ((ximage->width != transformed_image->columns) ||
      (ximage->height != transformed_image->rows))
    {
      Image
        *scaled_image;

      /*
        Scale image.
      */
      scaled_image=ScaleImage(transformed_image,(unsigned int) ximage->width,
        (unsigned int) ximage->height);
      if (scaled_image != (Image *) NULL)
        {
          if (transformed_image != image)
            DestroyImage(transformed_image);
          transformed_image=scaled_image;
        }
    }
  /*
    Convert runlength-encoded pixels to X image data.
  */
  if ((ximage->byte_order == LSBFirst) ||
      ((ximage->format == XYBitmap) && (ximage->bitmap_bit_order == LSBFirst)))
    XMakeImageLSBFirst(image_window->map_info,image_window->pixel_info,
      transformed_image,ximage);
  else
    XMakeImageMSBFirst(image_window->map_info,image_window->pixel_info,
      transformed_image,ximage);
  if (transformed_image != image)
    DestroyImage(transformed_image);
  XDefineCursor(display,image_window->id,image_window->cursor);
  return(ximage);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e I m a g e L S B F i r s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeImageLSBFirst initializes the pixel data of an X11 Image.
%  The X image pixels are copied in least-significant bit and byte first
%  order.  The server's scanline pad is respected.  Rather than using one or
%  two general cases, many special cases are found here to help speed up the
%  image conversion.
%
%  The format of the XMakeImageLSBFirst routine is:
%
%      XMakeImageLSBFirst(map_info,pixel_info,image,ximage)
%
%  A description of each parameter follows:
%
%    o map_info: Specifies a pointer to a XStandardColormap structure.
%
%    o pixel_info: Specifies a pointer to a XPixelInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o ximage: Specifies a pointer to a XImage structure;  returned from
%      XCreateImage.
%
%
*/
void XMakeImageLSBFirst(map_info,pixel_info,image,ximage)
XStandardColormap
  *map_info;

XPixelInfo
  *pixel_info;

Image
  *image;

XImage
  *ximage;
{
  register int
    i,
    j,
    x;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  register unsigned long
    pixel;

  unsigned int
    scanline_pad;

  unsigned long
    *pixels;

  pixels=pixel_info->pixels;
  p=image->pixels;
  q=(unsigned char *) ximage->data;
  x=0;
  if (ximage->format == XYBitmap)
    {
      register unsigned char
        bit,
        byte;

      register unsigned char
        foreground_pixel;

      /*
        Convert image to big-endian bitmap.
      */
      foreground_pixel=(Intensity(image->colormap[0]) >
        Intensity(image->colormap[1]) ? 0 : 1);
      scanline_pad=ximage->bytes_per_line-(ximage->width >> 3);
      bit=0;
      byte=0;
      for (i=0; i < image->packets; i++)
      {
        for (j=0; j <= p->length; j++)
        {
          byte>>=1;
          if (p->index == foreground_pixel)
            byte|=0x80;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
          x++;
          if (x == ximage->width)
            {
              /*
                Advance to the next scanline.
              */
              if (bit > 0)
                *q=byte >> (8-bit);
              q+=scanline_pad;
              bit=0;
              byte=0;
              x=0;
            }
        }
        p++;
      }
    }
  else
    {
      /*
        Convert image to little-endian color-mapped X image.
      */
      scanline_pad=ximage->bytes_per_line-
        ((ximage->width*ximage->bits_per_pixel) >> 3);
      if (pixel_info->colors > 0)
        switch (ximage->bits_per_pixel)
        {
          case 2:
          {
            register unsigned int
              nibble;

            /*
              Convert to 2 bit color-mapped X image.
            */
            nibble=0;
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index] & 0xf;
              for (j=0; j <= p->length; j++)
              {
                switch (nibble)
                {
                  case 0:
                  {
                    *q=(unsigned char) pixel;
                    nibble++;
                    break;
                  }
                  case 1:
                  {
                    *q|=(unsigned char) (pixel << 2);
                    nibble++;
                    break;
                  }
                  case 2:
                  {
                    *q|=(unsigned char) (pixel << 4);
                    nibble++;
                    break;
                  }
                  case 3:
                  {
                    *q|=(unsigned char) (pixel << 6);
                    q++;
                    nibble=0;
                    break;
                  }
                }
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    nibble=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
          case 4:
          {
            register unsigned int
              nibble;

            /*
              Convert to 4 bit color-mapped X image.
            */
            nibble=0;
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index] & 0xf;
              for (j=0; j <= p->length; j++)
              {
                switch (nibble)
                {
                  case 0:
                  {
                    *q=(unsigned char) pixel;
                    nibble++;
                    break;
                  }
                  case 1:
                  {
                    *q|=(unsigned char) (pixel << 4);
                    q++;
                    nibble=0;
                    break;
                  }
                }
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    nibble=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
          case 6:
          case 8:
          {
            /*
              Convert to 8 bit color-mapped X image.
            */
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index];
              for (j=0; j <= p->length; j++)
              {
                *q++=(unsigned char) pixel;
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
          default:
          {
            register int
              k;

            register unsigned int
              bytes_per_pixel;

            unsigned char
              channel[sizeof(unsigned long)];

            /*
              Convert to multi-byte color-mapped X image.
            */
            bytes_per_pixel=ximage->bits_per_pixel >> 3;
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index];
              for (k=0; k < bytes_per_pixel; k++)
              {
                channel[k]=(unsigned char) pixel;
                pixel>>=8;
              }
              for (j=0; j <= p->length; j++)
              {
                for (k=0; k < bytes_per_pixel; k++)
                  *q++=channel[k];
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
        }
      else
        {
          /*
            Convert image to little-endian continuous-tone X image.
          */
          switch (ximage->bits_per_pixel)
          {
            case 2:
            {
              register unsigned int
                nibble;

              /*
                Convert to contiguous 2 bit continuous-tone X image.
              */
              nibble=0;
              for (i=0; i < image->packets; i++)
              {
                pixel=XStandardPixel(map_info,(*p),8);
                pixel&=0xf;
                for (j=0; j <= p->length; j++)
                {
                  switch (nibble)
                  {
                    case 0:
                    {
                      *q=(unsigned char) pixel;
                      nibble++;
                      break;
                    }
                    case 1:
                    {
                      *q|=(unsigned char) (pixel << 2);
                      nibble++;
                      break;
                    }
                    case 2:
                    {
                      *q|=(unsigned char) (pixel << 4);
                      nibble++;
                      break;
                    }
                    case 3:
                    {
                      *q|=(unsigned char) (pixel << 6);
                      q++;
                      nibble=0;
                      break;
                    }
                  }
                  x++;
                  if (x == ximage->width)
                    {
                      x=0;
                      nibble=0;
                      q+=scanline_pad;
                    }
                }
                p++;
              }
              break;
            }
            case 4:
            {
              register unsigned int
                nibble;

              /*
                Convert to contiguous 4 bit continuous-tone X image.
              */
              nibble=0;
              for (i=0; i < image->packets; i++)
              {
                pixel=XStandardPixel(map_info,(*p),8);
                pixel&=0xf;
                for (j=0; j <= p->length; j++)
                {
                  switch (nibble)
                  {
                    case 0:
                    {
                      *q=(unsigned char) pixel;
                      nibble++;
                      break;
                    }
                    case 1:
                    {
                      *q|=(unsigned char) (pixel << 4);
                      q++;
                      nibble=0;
                      break;
                    }
                  }
                  x++;
                  if (x == ximage->width)
                    {
                      x=0;
                      nibble=0;
                      q+=scanline_pad;
                    }
                }
                p++;
              }
              break;
            }
            case 6:
            case 8:
            {
              /*
                Convert to contiguous 8 bit continuous-tone X image.
              */
              for (i=0; i < image->packets; i++)
              {
                pixel=XStandardPixel(map_info,(*p),8);
                for (j=0; j <= p->length; j++)
                {
                  *q++=(unsigned char) pixel;
                  x++;
                  if (x == ximage->width)
                    {
                      x=0;
                      q+=scanline_pad;
                    }
                }
                p++;
              }
              break;
            }
            default:
            {
              if ((ximage->bits_per_pixel == 32) &&
                  (map_info->red_max == 255) &&
                  (map_info->green_max == 255) &&
                  (map_info->blue_max == 255) &&
                  (map_info->red_mult == 65536) &&
                  (map_info->green_mult == 256) &&
                  (map_info->blue_mult == 1))
                {
                  /*
                    Convert to 32 bit continuous-tone X image.
                  */
                  for (i=0; i < image->packets; i++)
                  {
                    for (j=0; j <= p->length; j++)
                    {
                      *q++=p->blue;
                      *q++=p->green;
                      *q++=p->red;
                      *q++=0;
                    }
                    p++;
                  }
                }
              else
                {
                  register int
                    k;

                  register unsigned int
                    bytes_per_pixel;

                  unsigned char
                    channel[sizeof(unsigned long)];

                  /*
                    Convert to multi-byte continuous-tone X image.
                  */
                  bytes_per_pixel=ximage->bits_per_pixel >> 3;
                  for (i=0; i < image->packets; i++)
                  {
                    pixel=XStandardPixel(map_info,(*p),8);
                    for (k=0; k < bytes_per_pixel; k++)
                    {
                      channel[k]=(unsigned char) pixel;
                      pixel>>=8;
                    }
                    for (j=0; j <= p->length; j++)
                    {
                      for (k=0; k < bytes_per_pixel; k++)
                        *q++=channel[k];
                      x++;
                      if (x == ximage->width)
                        {
                          x=0;
                          q+=scanline_pad;
                        }
                    }
                    p++;
                  }
                }
              break;
            }
          }
        }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e I m a g e M S B F i r s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeImageMSBFirst initializes the pixel data of an X11 Image.
%  The X image pixels are copied in most-significant bit and byte first order.
%  The server's scanline pad is also resprected. Rather than using one or two
%  general cases, many special cases are found here to help speed up the image
%  conversion.
%
%  The format of the XMakeImageMSBFirst routine is:
%
%      XMakeImageMSBFirst(map_info,pixel_info,image,ximage)
%
%  A description of each parameter follows:
%
%    o map_info: Specifies a pointer to a XStandardColormap structure.
%
%    o pixel_info: Specifies a pointer to a XPixelInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o ximage: Specifies a pointer to a XImage structure;  returned from
%      XCreateImage.
%
%
*/
void XMakeImageMSBFirst(map_info,pixel_info,image,ximage)
XStandardColormap
  *map_info;

XPixelInfo
  *pixel_info;

Image
  *image;

XImage
  *ximage;
{
  register int
    i,
    j,
    x;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  register unsigned long
    pixel;

  unsigned int
    scanline_pad;

  unsigned long
    *pixels;

  pixels=pixel_info->pixels;
  p=image->pixels;
  q=(unsigned char *) ximage->data;
  x=0;
  if (ximage->format == XYBitmap)
    {
      register unsigned char
        bit,
        byte;

      register unsigned char
        foreground_pixel;

      /*
        Convert image to big-endian bitmap.
      */
      foreground_pixel=(Intensity(image->colormap[0]) >
        Intensity(image->colormap[1]) ? 0 : 1);
      scanline_pad=ximage->bytes_per_line-(ximage->width >> 3);
      bit=0;
      byte=0;
      for (i=0; i < image->packets; i++)
      {
        for (j=0; j <= p->length; j++)
        {
          byte<<=1;
          if (p->index == foreground_pixel)
            byte|=0x01;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
          x++;
          if (x == ximage->width)
            {
              /*
                Advance to the next scanline.
              */
              if (bit > 0)
                *q=byte << (8-bit);
              q+=scanline_pad;
              bit=0;
              byte=0;
              x=0;
            }
        }
        p++;
      }
    }
  else
    {
      /*
        Convert image to big-endian X image.
      */
      scanline_pad=ximage->bytes_per_line-
        ((ximage->width*ximage->bits_per_pixel) >> 3);
      if (pixel_info->colors > 0)
        switch (ximage->bits_per_pixel)
        {
          case 2:
          {
            register unsigned int
              nibble;

            /*
              Convert to 2 bit color-mapped X image.
            */
            nibble=0;
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index] & 0xf;
              for (j=0; j <= p->length; j++)
              {
                switch (nibble)
                {
                  case 0:
                  {
                    *q=(unsigned char) (pixel << 6);
                    nibble++;
                    break;
                  }
                  case 1:
                  {
                    *q|=(unsigned char) (pixel << 4);
                    nibble++;
                    break;
                  }
                  case 2:
                  {
                    *q|=(unsigned char) (pixel << 2);
                    nibble++;
                    break;
                  }
                  case 3:
                  {
                    *q|=(unsigned char) pixel;
                    q++;
                    nibble=0;
                    break;
                  }
                }
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    nibble=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
          case 4:
          {
            register unsigned int
              nibble;

            /*
              Convert to 4 bit color-mapped X image.
            */
            nibble=0;
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index] & 0xf;
              for (j=0; j <= p->length; j++)
              {
                switch (nibble)
                {
                  case 0:
                  {
                    *q=(unsigned char) (pixel << 4);
                    nibble++;
                    break;
                  }
                  case 1:
                  {
                    *q|=(unsigned char) pixel;
                    q++;
                    nibble=0;
                    break;
                  }
                }
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    nibble=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
          case 8:
          {
            /*
              Convert to 8 bit color-mapped X image.
            */
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index];
              for (j=0; j <= p->length; j++)
              {
                *q++=(unsigned char) pixel;
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
          default:
          {
            register int
              k;

            register unsigned int
              bytes_per_pixel;

            unsigned char
              channel[sizeof(unsigned long)];

            /*
              Convert to 8 bit color-mapped X image.
            */
            bytes_per_pixel=ximage->bits_per_pixel >> 3;
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index];
              for (k=bytes_per_pixel-1; k >= 0; k--)
              {
                channel[k]=(unsigned char) pixel;
                pixel>>=8;
              }
              for (j=0; j <= p->length; j++)
              {
                for (k=0; k < bytes_per_pixel; k++)
                  *q++=channel[k];
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
        }
      else
        {
          /*
            Convert to big-endian continuous-tone X image.
          */
          switch (ximage->bits_per_pixel)
          {
            case 2:
            {
              register unsigned int
                nibble;

              /*
                Convert to 4 bit continuous-tone X image.
              */
              nibble=0;
              for (i=0; i < image->packets; i++)
              {
                pixel=XStandardPixel(map_info,(*p),8);
                pixel&=0xf;
                for (j=0; j <= p->length; j++)
                {
                  switch (nibble)
                  {
                    case 0:
                    {
                      *q=(unsigned char) (pixel << 6);
                      nibble++;
                      break;
                    }
                    case 1:
                    {
                      *q|=(unsigned char) (pixel << 4);
                      nibble++;
                      break;
                    }
                    case 2:
                    {
                      *q|=(unsigned char) (pixel << 2);
                      nibble++;
                      break;
                    }
                    case 3:
                    {
                      *q|=(unsigned char) pixel;
                      q++;
                      nibble=0;
                      break;
                    }
                  }
                  x++;
                  if (x == ximage->width)
                    {
                      x=0;
                      nibble=0;
                      q+=scanline_pad;
                    }
                }
                p++;
              }
              break;
            }
            case 4:
            {
              register unsigned int
                nibble;

              /*
                Convert to 4 bit continuous-tone X image.
              */
              nibble=0;
              for (i=0; i < image->packets; i++)
              {
                pixel=XStandardPixel(map_info,(*p),8);
                pixel&=0xf;
                for (j=0; j <= p->length; j++)
                {
                  switch (nibble)
                  {
                    case 0:
                    {
                      *q=(unsigned char) (pixel << 4);
                      nibble++;
                      break;
                    }
                    case 1:
                    {
                      *q|=(unsigned char) pixel;
                      q++;
                      nibble=0;
                      break;
                    }
                  }
                  x++;
                  if (x == ximage->width)
                    {
                      x=0;
                      nibble=0;
                      q+=scanline_pad;
                    }
                }
                p++;
              }
              break;
            }
            case 8:
            {
              /*
                Convert to 8 bit continuous-tone X image.
              */
              for (i=0; i < image->packets; i++)
              {
                pixel=XStandardPixel(map_info,(*p),8);
                for (j=0; j <= p->length; j++)
                {
                  *q++=(unsigned char) pixel;
                  x++;
                  if (x == ximage->width)
                    {
                      x=0;
                      q+=scanline_pad;
                    }
                }
                p++;
              }
              break;
            }
            default:
            {
              if ((ximage->bits_per_pixel == 32) &&
                  (map_info->red_max == 255) &&
                  (map_info->green_max == 255) &&
                  (map_info->blue_max == 255) &&
                  (map_info->red_mult == 65536) &&
                  (map_info->green_mult == 256) &&
                  (map_info->blue_mult == 1))
                {
                  /*
                    Convert to 32 bit continuous-tone X image.
                  */
                  for (i=0; i < image->packets; i++)
                  {
                    for (j=0; j <= p->length; j++)
                    {
                      *q++=0;
                      *q++=p->red;
                      *q++=p->green;
                      *q++=p->blue;
                    }
                    p++;
                  }
                }
              else
                {
                  register int
                    k;

                  register unsigned int
                    bytes_per_pixel;

                  unsigned char
                    channel[sizeof(unsigned long)];

                  /*
                    Convert to multi-byte continuous-tone X image.
                  */
                  bytes_per_pixel=ximage->bits_per_pixel >> 3;
                  for (i=0; i < image->packets; i++)
                  {
                    pixel=XStandardPixel(map_info,(*p),8);
                    for (k=bytes_per_pixel-1; k >= 0; k--)
                    {
                      channel[k]=(unsigned char) pixel;
                      pixel>>=8;
                    }
                    for (j=0; j <= p->length; j++)
                    {
                      for (k=0; k < bytes_per_pixel; k++)
                        *q++=channel[k];
                      x++;
                      if (x == ximage->width)
                        {
                          x=0;
                          q+=scanline_pad;
                        }
                    }
                    p++;
                  }
                }
              break;
            }
          }
        }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e S t a n d a r d C o l o r m a p                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeStandardColormap creates an X11 colormap.  If the visual
%  class is TrueColor or DirectColor a Standard Colormap is also created.
%
%  The format of the XMakeStandardColormap routine is:
%
%      XMakeStandardColormap(display,visual_info,resource_info,pixel_info,
%        image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o pixel_info: Specifies a pointer to a XPixelInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
XStandardColormap *XMakeStandardColormap(display,visual_info,resource_info,
  pixel_info,image)
Display
  *display;

XVisualInfo
  *visual_info;

XResourceInfo
  *resource_info;

XPixelInfo
  *pixel_info;

Image
  *image;
{
  Colormap
    colormap;

  int
    status;

  register int
    i;

  XColor
    color;

  XStandardColormap
    *map_info;

  Window
    root_window;

  /*
    Initialize colormap.
  */
  root_window=XRootWindow(display,visual_info->screen);
  if (visual_info->visual == XDefaultVisual(display,visual_info->screen))
    colormap=XDefaultColormap(display,visual_info->screen);
  else
    if ((visual_info->class == StaticGray) ||
        (visual_info->class == StaticColor) ||
        (visual_info->class == TrueColor))
      colormap=
        XCreateColormap(display,root_window,visual_info->visual,AllocNone);
    else
      colormap=
        XCreateColormap(display,root_window,visual_info->visual,AllocAll);
  if (colormap == (Colormap) NULL)
    Error("unable to create colormap",(char *) NULL);
  /*
    Initialize map info.
  */
  map_info=XAllocStandardColormap();
  if (map_info == (XStandardColormap *) NULL)
    Error("unable to create colormap","memory allocation failed");
  map_info->colormap=colormap;
  map_info->red_max=0;
  map_info->green_max=0;
  map_info->blue_max=0;
  map_info->red_mult=0;
  map_info->green_mult=0;
  map_info->blue_mult=0;
  map_info->base_pixel=0;
  /*
    Initialize pixel info.
  */
  XGetPixelInfo(display,visual_info,map_info,resource_info,pixel_info);
  if ((visual_info->class != TrueColor) && (visual_info->class != DirectColor))
    {
      if ((image->class == DirectClass) ||
          (image->colors > visual_info->colormap_size))
        {
          unsigned int
            number_colors;

          /*
            Prevent colormap "flashing"; reserve some colormap entries.
          */
          number_colors=visual_info->colormap_size;
          if ((visual_info->class == GrayScale) ||
              (visual_info->class == PseudoColor))
            if (visual_info->visual ==
                XDefaultVisual(display,visual_info->screen))
              if ((visual_info->depth > 1) && !resource_info->backdrop)
                number_colors=(number_colors*61) >> 6;
          QuantizeImage(image,number_colors,0,False,False);
          image->class=DirectClass;
        }
      /*
        Allocate pixels array.
      */
      pixel_info->colors=image->colors;
      pixel_info->pixels=(unsigned long *)
        malloc((unsigned int) visual_info->colormap_size*sizeof(unsigned long));
      if (pixel_info->pixels == (unsigned long *) NULL)
        Error("unable to create colormap","memory allocation failed");
      /*
        Colormap creation is dictated by the server visual class.
      */
      switch (visual_info->class)
      {
        case StaticGray:
        case StaticColor:
        {
          /*
            Immutable colormap;  allocate shared colors.
          */
          color.flags=DoRed | DoGreen | DoBlue;
          for (i=0; i < image->colors; i++)
          {
            color.red=(unsigned short) (image->colormap[i].red << 8);
            color.green=(unsigned short) (image->colormap[i].green << 8);
            color.blue=(unsigned short) (image->colormap[i].blue << 8);
            status=XAllocColor(display,colormap,&color);
            if (status == 0)
              {
                colormap=XCopyColormapAndFree(display,colormap);
                XAllocColor(display,colormap,&color);
              }
            pixel_info->pixels[i]=color.pixel;
          }
          /*
            Set background/border/foreground/pen pixels.
          */
          XAllocColor(display,colormap,&pixel_info->background_color);
          XAllocColor(display,colormap,&pixel_info->border_color);
          XAllocColor(display,colormap,&pixel_info->foreground_color);
          for (i=0; i < 9; i++)
            XAllocColor(display,colormap,&pixel_info->pen_color[i]);
          break;
        }
        case GrayScale:
        case PseudoColor:
        {
          XColor
            *colors;

          /*
            Read/write colormap;  allocate private colors.
          */
          colors=(XColor *)
            malloc((unsigned int) visual_info->colormap_size*sizeof(XColor));
          if (colors == (XColor *) NULL)
            Error("unable to create colormap","memory allocation failed");
          status=XAllocColorCells(display,colormap,False,
            (unsigned long *) NULL,0,pixel_info->pixels,image->colors);
          if (status == 0)
            {
              unsigned int
                retain_colors;

              /*
                Create a new colormap.
              */
              colormap=XCreateColormap(display,root_window,visual_info->visual,
                AllocNone);
              if (colormap == (Colormap) NULL)
                Error("unable to create colormap",(char *) NULL);
              map_info->colormap=colormap;
              retain_colors=visual_info->colormap_size-image->colors;
              if (retain_colors > 0)
                {
                  /*
                    Not enough colormap entries in the default colormap;
                    create a new colormap and retain as many colors from the
                    default colormap as possible.
                  */
                  if (retain_colors > 144)
                    retain_colors=144;
                  for (i=0; i < retain_colors; i++)
                    colors[i].pixel=i;
                  XQueryColors(display,XDefaultColormap(display,
                    visual_info->screen),colors,(int) retain_colors);
                  XAllocColorCells(display,colormap,False,
                    (unsigned long *) NULL,0,pixel_info->pixels,retain_colors);
                  XStoreColors(display,colormap,colors,retain_colors);
                }
              XAllocColorCells(display,colormap,False,(unsigned long *) NULL,0,
                pixel_info->pixels,image->colors);
            }
          /*
            Store the image colormap.
          */
          if (visual_info->class != GrayScale)
            for (i=0; i < image->colors; i++)
            {
              colors[i].flags=DoRed | DoGreen | DoBlue;
              colors[i].red=(unsigned short) (image->colormap[i].red << 8);
              colors[i].green=(unsigned short) (image->colormap[i].green << 8);
              colors[i].blue=(unsigned short) (image->colormap[i].blue << 8);
              colors[i].pixel=pixel_info->pixels[i];
            }
          else
            {
              register unsigned char
                gray_value;

              /*
                Convert PseudoClass packets to grayscale.
              */
              for (i=0; i < image->colors; i++)
              {
                colors[i].flags=DoRed | DoGreen | DoBlue;
                gray_value=Intensity(image->colormap[i]);
                colors[i].red=(unsigned short) (gray_value << 8);
                colors[i].green=(unsigned short) (gray_value << 8);
                colors[i].blue=(unsigned short) (gray_value << 8);
                colors[i].pixel=pixel_info->pixels[i];
              }
            }
          XStoreColors(display,colormap,colors,image->colors);
          /*
            Set background/border/foreground/pen pixels.
          */
          status=XAllocColor(display,colormap,&pixel_info->background_color);
          if (status == 0)
            pixel_info->background_color.pixel=
              XBestPixel(colors,image->colors,pixel_info->background_color);
          status=XAllocColor(display,colormap,&pixel_info->border_color);
          if (status == 0)
            pixel_info->border_color.pixel=
              XBestPixel(colors,image->colors,pixel_info->border_color);
          status=XAllocColor(display,colormap,&pixel_info->foreground_color);
          if (status == 0)
            pixel_info->foreground_color.pixel=
              XBestPixel(colors,image->colors,pixel_info->foreground_color);
          for (i=0; i < 9; i++)
          {
            status=XAllocColor(display,colormap,&pixel_info->pen_color[i]);
            if (status == 0)
              pixel_info->pen_color[i].pixel=
                XBestPixel(colors,image->colors,pixel_info->pen_color[i]);
          }
          (void) free((char *) colors);
          break;
        }
      }
    }
  else
    {
      unsigned int
        linear_colormap,
        number_colors;

      XColor
        *colors;

      /*
        Initialize the Standard Colormap attributes.
      */
      if (visual_info->red_mask > 0)
        {
          map_info->red_max=visual_info->red_mask;
          map_info->red_mult=1;
          while ((map_info->red_max & 0x01) == 0)
          {
            map_info->red_max>>=1;
            map_info->red_mult<<=1;
          }
        }
      if (visual_info->green_mask > 0)
        {
          map_info->green_max=visual_info->green_mask;
          map_info->green_mult=1;
          while ((map_info->green_max & 0x01) == 0)
          {
            map_info->green_max>>=1;
            map_info->green_mult<<=1;
          }
        }
      if (visual_info->blue_mask > 0)
        {
          map_info->blue_max=visual_info->blue_mask;
          map_info->blue_mult=1;
          while ((map_info->blue_max & 0x01) == 0)
          {
            map_info->blue_max>>=1;
            map_info->blue_mult<<=1;
          }
        }
      /*
        Determine the number of colors this server supports.
      */
      linear_colormap=
        ((map_info->red_max+1) == visual_info->colormap_size) &&
        ((map_info->green_max+1) == visual_info->colormap_size) &&
        ((map_info->blue_max+1) == visual_info->colormap_size);
      if (linear_colormap)
        number_colors=visual_info->colormap_size;
      else
        number_colors=(map_info->red_max*map_info->red_mult)+
          (map_info->green_max*map_info->green_mult)+
          (map_info->blue_max*map_info->blue_mult)+1;
      /*
        Allocate color array.
      */
      colors=(XColor *) malloc((unsigned int) number_colors*sizeof(XColor));
      if (colors == (XColor *) NULL)
        Error("unable to create colormap","memory allocation failed");
      /*
        Initialize linear color ramp.
      */
      if (linear_colormap)
        for (i=0; i < number_colors; i++)
        {
          colors[i].flags=DoRed | DoGreen | DoBlue;
          if (map_info->blue_max == 0)
            colors[i].blue=(unsigned short) 0;
          else
            colors[i].blue=(unsigned short)
              (((i % map_info->green_mult)*65535)/map_info->blue_max);
          colors[i].red=colors[i].blue;
          colors[i].green=colors[i].blue;
          colors[i].pixel=XStandardPixel(map_info,colors[i],16);
        }
      else
        for (i=0; i < number_colors; i++)
        {
          colors[i].flags=DoRed | DoGreen | DoBlue;
          if (map_info->red_max == 0)
            colors[i].red=(unsigned short) 0;
          else
            colors[i].red=(unsigned short)
              (((i/map_info->red_mult)*65535)/map_info->red_max);
          if (map_info->green_max == 0)
            colors[i].green=(unsigned short) 0;
          else
            colors[i].green=(unsigned short) ((((i/map_info->green_mult) %
              (map_info->green_max+1))*65535)/map_info->green_max);
          if (map_info->blue_max == 0)
            colors[i].blue=(unsigned short) 0;
          else
            colors[i].blue=(unsigned short)
              (((i % map_info->green_mult)*65535)/map_info->blue_max);
          colors[i].pixel=XStandardPixel(map_info,colors[i],16);
        }
      if ((visual_info->class == DirectColor) &&
          (colormap != XDefaultColormap(display,visual_info->screen)))
        XStoreColors(display,colormap,colors,number_colors);
      else
        for (i=0; i < image->colors; i++)
          XAllocColor(display,colormap,&colors[i]);
      (void) free((char *) colors);
      if (image->class == PseudoClass)
        {
          /*
            Initialize pixel array for images of type PseudoClass.
          */
          pixel_info->colors=image->colors;
          pixel_info->pixels=(unsigned long *)
            malloc((unsigned int) image->colors*sizeof(unsigned long));
          if (pixel_info->pixels == (unsigned long *) NULL)
            Error("unable to create colormap","memory allocation failed");
          for (i=0; i < image->colors; i++)
            pixel_info->pixels[i]=XStandardPixel(map_info,image->colormap[i],8);
        }
      /*
        Define background/border/foreground/pen pixels.
      */
      pixel_info->background_color.pixel=
        XStandardPixel(map_info,pixel_info->background_color,16);
      pixel_info->border_color.pixel=
        XStandardPixel(map_info,pixel_info->border_color,16);
      pixel_info->foreground_color.pixel=
        XStandardPixel(map_info,pixel_info->foreground_color,16);
      for (i=0; i < 9; i++)
        pixel_info->pen_color[i].pixel=
          XStandardPixel(map_info,pixel_info->pen_color[i],16);
    }
  return(map_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e W i n d o w                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeWindow creates an X11 window.
%
%  The format of the XMakeWindow routine is:
%
%      XMakeWindow(display,parent,argv,argc,class_hint,manager_hints,property,
%        window_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o parent: Specifies the parent window.
%
%    o argv: Specifies the application's argument list.
%
%    o argc: Specifies the number of arguments.
%
%    o class_hint: Specifies a pointer to a X11 XClassHint structure.
%
%    o manager_hints: Specifies a pointer to a X11 XWMHints structure.
%
%    o property: A property to define on the window.
%
%    o window_info: Specifies a pointer to a X11 XWindowInfo structure.
%
%
*/
void XMakeWindow(display,parent,argv,argc,class_hint,manager_hints,property,
  window_info)
Display
  *display;

Window
  parent;

char
  **argv;

int
  argc;

XClassHint
  *class_hint;

XWMHints
  *manager_hints;

Atom
  property;

XWindowInfo
  *window_info;
{
#define MinWindowSize  64

  XSizeHints
    *size_hints;

  XTextProperty
    name;

  /*
    Set window hints.
  */
  size_hints=XAllocSizeHints();
  if (size_hints == (XSizeHints *) NULL)
    Error("unable to make window","memory allocation failed");
  size_hints->flags=window_info->flags;
  size_hints->x=window_info->x;
  size_hints->y=window_info->y;
  size_hints->width=window_info->width;
  size_hints->height=window_info->height;
  size_hints->flags|=PMinSize | PMaxSize;
  if (!window_info->immutable)
    {
      /*
        Window size can be changed.
      */
      size_hints->min_width=window_info->min_width;
      size_hints->min_height=window_info->min_height;
      size_hints->max_width=XDisplayWidth(display,window_info->screen);
      size_hints->max_height=XDisplayHeight(display,window_info->screen);
    }
  else
    {
      /*
        Window size cannot be changed.
      */
      size_hints->min_width=window_info->width;
      size_hints->min_height=window_info->height;
      size_hints->max_width=window_info->width;
      size_hints->max_height=window_info->height;
    }
  size_hints->flags|=PResizeInc;
  size_hints->width_inc=window_info->width_inc;
  size_hints->height_inc=window_info->height_inc;
#ifndef PRE_R4_ICCCM
  size_hints->flags|=PBaseSize;
  size_hints->base_width=size_hints->min_width;
  size_hints->base_height=size_hints->min_height;
#endif
  if (window_info->geometry != (char *) NULL)
    {
      char
        default_geometry[1024];

      int
        flags,
        gravity;

      /*
        User specified geometry.
      */
      (void) sprintf(default_geometry,"%dx%d\0",size_hints->width,
        size_hints->height);
      flags=XWMGeometry(display,window_info->screen,window_info->geometry,
        default_geometry,window_info->border_width,size_hints,&size_hints->x,
        &size_hints->y,&size_hints->width,&size_hints->height,&gravity);
      window_info->x=size_hints->x;
      window_info->y=size_hints->y;
      if ((flags & WidthValue) && (flags & HeightValue))
        size_hints->flags|=USSize;
      if ((flags & XValue) && (flags & YValue))
        size_hints->flags|=USPosition;
#ifndef PRE_R4_ICCCM
      size_hints->win_gravity=gravity;
      size_hints->flags|=PWinGravity;
#endif
    }
  window_info->id=XCreateWindow(display,parent,window_info->x,window_info->y,
    window_info->width,window_info->height,window_info->border_width,
    window_info->depth,InputOutput,window_info->visual_info->visual,
    CWBackingStore | CWBackPixel | CWBackPixmap | CWBitGravity |
    CWBorderPixel | CWColormap | CWCursor | CWDontPropagate | CWEventMask |
    CWOverrideRedirect | CWSaveUnder | CWWinGravity,&window_info->attributes);
  if (window_info->id == (Window) NULL)
    Error("unable to create window",window_info->name);
  name.value=(unsigned char *) window_info->name;
  name.encoding=XA_STRING;
  name.format=8;
  name.nitems=strlen(window_info->name);
  if (window_info->icon_geometry != (char *) NULL)
    {
      int
        flags,
        gravity,
        height,
        width;

      /*
        User specified icon geometry.
      */
      size_hints->flags|=USPosition;
      flags=XWMGeometry(display,window_info->screen,window_info->icon_geometry,
        (char *) NULL,0,size_hints,&manager_hints->icon_x,
        &manager_hints->icon_y,&width,&height,&gravity);
      if ((flags & XValue) && (flags & YValue))
        manager_hints->flags|=IconPositionHint;
    }
  XSetWMProperties(display,window_info->id,&name,&name,argv,argc,size_hints,
    manager_hints,class_hint);
  XSetWMProtocols(display,window_info->id,&property,1);
  XFree((void *) size_hints);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X P o p U p A l e r t                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XPopUpAlert displays a popup window with an alert to the user.
%  The function returns when the user presses a button or key.
%
%  The format of the XPopUpAlert routine is:
%
%    XPopUpAlert(display,popup_window,message,qualifier)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o popup_window: Specifies a pointer to a XWindowInfo structure.
%
%    o message: Specifies the message to display before terminating the
%      program.
%
%    o qualifier: Specifies any qualifier to the message.
%
%
*/
void XPopUpAlert(display,popup_window,message,qualifier)
Display
  *display;

XWindowInfo
  *popup_window;

char
  *message,
  *qualifier;
{
  char
    text[1024];

  int
    i,
    x,
    y;

  unsigned int
    mask;

  Window
    window;

  XEvent
    event;

  /*
    Position and map popup window.
  */
  (void) sprintf(text,"%s\0",message);
  if (qualifier != (char *) NULL)
    {
      (void) strcat(text," (");
      (void) strcat(text,qualifier);
      (void) strcat(text,")");
    }
  popup_window->width=XTextWidth(popup_window->font_info,text,strlen(text))+
    4*popup_window->font_info->max_bounds.width;
  popup_window->height=
    popup_window->font_info->ascent+popup_window->font_info->descent+8;
  XQueryPointer(display,XRootWindow(display,popup_window->screen),&window,
    &window,&i,&i,&popup_window->x,&popup_window->y,&mask);
  x=Min(popup_window->x+2,XDisplayWidth(display,popup_window->screen)-
    popup_window->width);
  y=Min(popup_window->y+2,XDisplayHeight(display,popup_window->screen)-
    popup_window->height);
  XMoveResizeWindow(display,popup_window->id,x,y,popup_window->width,
    popup_window->height);
  XMapRaised(display,popup_window->id);
  /*
    Display message in popup window.
  */
  x=2*popup_window->font_info->max_bounds.width;
  y=popup_window->font_info->ascent+4;
  XDrawString(display,popup_window->id,popup_window->graphic_context,x,y,text,
    strlen(text));
  XBell(display,0);
  /*
    Wait for a key press.
  */
  XMaskEvent(display,KeyPressMask,&event);
  XWithdrawWindow(display,popup_window->id,popup_window->screen);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X P o p U p M e n u                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XPopUpMenu maps a menu and returns the command pointed to by the
%  user when the button is released.
%
%  The format of the XPopUpMenu routine is:
%
%    selection_number=XPopUpMenu(display,popup_window,x,y,menu_title,
%      menu_selections,number_selections,item)
%
%  A description of each parameter follows:
%
%    o selection_number: Specifies the number of the selection that the
%      user choose.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o popup_window: Specifies a pointer to a XWindowInfo structure.
%
%    o x: Specifies an unsigned integer representing the root offset in the
%      x-direction.
%
%    o y: Specifies an unsigned integer representing the root offset in the
%      x-direction.
%
%    o menu_title: Specifies a character string that describes the menu
%      selections.
%
%    o menu_selections: Specifies a pointer to one or more strings that
%      make up the choices in the menu.
%
%    o number_selections: Specifies the number of choices in the menu.
%
%    o item: Specifies a character array.  The item selected from the menu
%      is returned here.
%
%
*/
unsigned int XPopUpMenu(display,popup_window,x,y,menu_title,menu_selections,
  number_selections,item)
Display
  *display;

XWindowInfo
  *popup_window;

int
  x,
  y;

char
  *menu_title,
  **menu_selections;

unsigned int
  number_selections;

char
  *item;
{
  typedef struct _Selection
  {
    int
      id,
      x,
      y;

    unsigned int
      height;
  } Selection;

  int
    id;

  Selection
    selection;

  unsigned int
    height,
    exit_state,
    title_height,
    width;

  XEvent
    event;

  /*
    Size and position menu window under current pointer location and map.
  */
  popup_window->width=
    XTextWidth(popup_window->font_info,menu_title,strlen(menu_title));
  for (selection.id=0; selection.id < number_selections; selection.id++)
  {
    width=XTextWidth(popup_window->font_info,menu_selections[selection.id],
      strlen(menu_selections[selection.id]));
    if (width > popup_window->width)
      popup_window->width=width;
  }
  popup_window->width+=4*popup_window->font_info->max_bounds.width;
  title_height=
    (popup_window->font_info->descent+popup_window->font_info->ascent)*2;
  popup_window->height=title_height+number_selections*
    (popup_window->font_info->ascent+popup_window->font_info->descent+4);
  width=popup_window->width+2*popup_window->border_width;
  popup_window->x=x-(int) width/2;
  if (popup_window->x < 0)
    popup_window->x=0;
  else
    if (popup_window->x > (XDisplayWidth(display,popup_window->screen)-width))
      popup_window->x=XDisplayWidth(display,popup_window->screen)-width;
  height=popup_window->height+2*popup_window->border_width;
  popup_window->y=y-(int) popup_window->border_width;
  if (popup_window->y < 0)
    popup_window->y=0;
  else
    if (popup_window->y > (XDisplayHeight(display,popup_window->screen)-height))
      popup_window->y=XDisplayHeight(display,popup_window->screen)-height;
  XMoveResizeWindow(display,popup_window->id,popup_window->x,popup_window->y,
    popup_window->width,popup_window->height);
  XMapRaised(display,popup_window->id);
  XWindowEvent(display,popup_window->id,ExposureMask,&event);
  /*
    Draw title.
  */
  width=XTextWidth(popup_window->font_info,menu_title,strlen(menu_title));
  XDrawString(display,popup_window->id,popup_window->graphic_context,
    (popup_window->width-width) >> 1,(popup_window->font_info->ascent*3) >> 1,
    menu_title,strlen(menu_title));
  XDrawLine(display,popup_window->id,popup_window->graphic_context,0,
    title_height-1,popup_window->width,title_height-1);
  /*
    Draw menu selections.
  */
  selection.x=2*popup_window->font_info->max_bounds.width;
  selection.y=title_height+popup_window->font_info->ascent+2;
  selection.height=popup_window->font_info->ascent+
    popup_window->font_info->descent+4;
  for (selection.id=0; selection.id < number_selections; selection.id++)
  {
    XDrawString(display,popup_window->id,popup_window->graphic_context,
      selection.x,selection.y,menu_selections[selection.id],
      strlen(menu_selections[selection.id]));
    selection.y+=(int) selection.height;
  }
  /*
    Highlight menu as pointer moves;  return item on button release.
  */
  selection.id=(-1);
  exit_state=False;
  do
  {
    /*
      Wait for next event.
    */
    XMaskEvent(display,ButtonPressMask | ButtonMotionMask | ButtonReleaseMask |
	 EnterWindowMask | LeaveWindowMask,&event);
    switch (event.type)
    {
      case ButtonPress:
        break;
      case ButtonRelease:
      {
        /*
          Exit menu.
        */
        *item=(char) NULL;
        exit_state=True;
        break;
      }
      case EnterNotify:
      {
        if (event.xcrossing.window != popup_window->id)
          break;
        id=((event.xcrossing.y-title_height)/(int) selection.height);
        if ((id < 0) || (id >= number_selections))
          break;
        /*
          Highlight this selection.
        */
        selection.id=id;
        selection.y=title_height+
          popup_window->font_info->ascent+2+selection.id*selection.height;
        XFillRectangle(display,popup_window->id,popup_window->graphic_context,0,
          selection.y-popup_window->font_info->ascent,popup_window->width,
          selection.height);
        XDrawString(display,popup_window->id,popup_window->highlight_context,
          selection.x,selection.y,menu_selections[selection.id],
          strlen(menu_selections[selection.id]));
        break;
      }
      case LeaveNotify:
      {
        if (event.xcrossing.window != popup_window->id)
          break;
        if ((selection.id >= 0) && (selection.id < number_selections))
          {
            /*
              Unhighlight last selection.
            */
            XClearArea(display,popup_window->id,0,selection.y-
              popup_window->font_info->ascent,popup_window->width,
              selection.height,False);
            XDrawString(display,popup_window->id,popup_window->graphic_context,
              selection.x,selection.y,menu_selections[selection.id],
              strlen(menu_selections[selection.id]));
          }
        selection.id=(-1);
        break;
      }
      case MotionNotify:
      {
        if (event.xmotion.window != popup_window->id)
          break;
        /*
          Determine if pointer has moved to a new selection.
        */
        id=(event.xmotion.y-title_height)/(int) selection.height;
        if ((selection.id >= 0) && (selection.id < number_selections))
          {
            /*
              Unhighlight last selection.
            */
            if (id == selection.id)
              break;
            XClearArea(display,popup_window->id,0,selection.y-
              popup_window->font_info->ascent,popup_window->width,
              selection.height,False);
            XDrawString(display,popup_window->id,popup_window->graphic_context,
              selection.x,selection.y,menu_selections[selection.id],
              strlen(menu_selections[selection.id]));
          }
        selection.id=id;
        if ((id < 0) || (id >= number_selections))
          break;
        /*
          Highlight this selection.
        */
        selection.y=title_height+
          popup_window->font_info->ascent+2+selection.id*selection.height;
        XFillRectangle(display,popup_window->id,popup_window->graphic_context,0,
          selection.y-popup_window->font_info->ascent,popup_window->width,
          selection.height);
        XDrawString(display,popup_window->id,popup_window->highlight_context,
          selection.x,selection.y,menu_selections[selection.id],
          strlen(menu_selections[selection.id]));
        break;
      }
      default:
        break;
    }
  } while (!exit_state);
  XWithdrawWindow(display,popup_window->id,popup_window->screen);
  if ((selection.id >= 0) && (selection.id < number_selections))
    (void) strcpy(item,menu_selections[selection.id]);
  return((unsigned int) selection.id);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X P o p U p Q u e r y                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XPopUpQuery displays a popup window with a query to the user.  The
%  user keys their reply and presses return to exit.  The typed text is
%  returned as the reply function parameter.
%
%  The format of the XPopUpQuery routine is:
%
%    XPopUpQuery(display,popup_window,query,reply)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o popup_window: Specifies a pointer to a XWindowInfo structure.
%
%    o query: Specifies a pointer to the query to present to the user.
%
%    o reply: The response from the user is returned in this parameter.
%
%
*/
void XPopUpQuery(display,popup_window,query,reply)
Display
  *display;

XWindowInfo
  *popup_window;

char
  *query;

char
  *reply;
{
  char
    *p,
    text[1024];

  GC
    graphic_context;

  int
    i,
    x,
    y;

  unsigned int
    exit_state,
    height,
    mask;

  Window
    window;

  XFontStruct
    *font_info;

  XEvent
    event;

  /*
    Position and map popup window.
  */
  (void) sprintf(text,"%s %s\0",query,reply);
  popup_window->width=XTextWidth(popup_window->font_info,text,strlen(text))+
    22*popup_window->font_info->max_bounds.width;
  popup_window->height=
    popup_window->font_info->ascent+popup_window->font_info->descent+8;
  XQueryPointer(display,XRootWindow(display,popup_window->screen),&window,
    &window,&i,&i,&popup_window->x,&popup_window->y,&mask);
  x=Min(popup_window->x+2,XDisplayWidth(display,popup_window->screen)-
    popup_window->width);
  y=Min(popup_window->y+2,XDisplayHeight(display,popup_window->screen)-
    popup_window->height);
  XMoveResizeWindow(display,popup_window->id,x,y,popup_window->width,
    popup_window->height);
  XMapRaised(display,popup_window->id);
  /*
    Display query in popup window.
  */
  font_info=popup_window->font_info;
  graphic_context=popup_window->graphic_context;
  x=2*font_info->max_bounds.width;
  y=font_info->ascent+4;
  height=font_info->ascent+font_info->descent;
  XDrawString(display,popup_window->id,graphic_context,x,y,query,strlen(query));
  x+=XTextWidth(font_info,query,strlen(query))+font_info->max_bounds.width;
  /*
    Display reply in popup window.
  */
  XDrawString(display,popup_window->id,graphic_context,x,y,reply,strlen(reply));
  x+=XTextWidth(font_info,reply,strlen(reply));
  /*
    Begin entering a reply.
  */
  exit_state=False;
  p=reply+strlen(reply);
  XDrawString(display,popup_window->id,graphic_context,x,y,"_",1);
  do
  {
    /*
      Wait for next event.
    */
    XMaskEvent(display,KeyPressMask,&event);
    switch (event.type)
    {
      case KeyPress:
      {
        static char
          command[1024];

        static KeySym
          key_symbol;

        /*
          Erase text cursor.
        */
        XClearArea(display,popup_window->id,x,y-font_info->ascent,
          font_info->max_bounds.width,height,False);
        /*
          Respond to a user key press.
        */
        *command=(char) NULL;
        XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        switch (key_symbol)
        {
          case XK_BackSpace:
          {
            /*
              Erase one character.
            */
            if (p == reply)
              break;
            p--;
            x-=XTextWidth(font_info,p,1);
            XClearArea(display,popup_window->id,x,y-font_info->ascent,
              font_info->max_bounds.width,height,False);
            break;
          }
          case XK_Delete:
          {
            /*
              Erase the entire line of text.
            */
            while (p != reply)
            {
              p--;
              x-=XTextWidth(font_info,p,1);
              XClearArea(display,popup_window->id,x,y-font_info->ascent,
                font_info->max_bounds.width,height,False);
            }
            break;
          }
          case XK_Escape:
          case XK_Return:
          {
            /*
              Finished annotating.
            */
            exit_state=True;
            break;
          }
          default:
          {
            /*
              Draw a single character on the popup window.
            */
            if (*command == (char) NULL)
              break;
            if ((x+font_info->max_bounds.width) >= popup_window->width)
              {
                /*
                  Resize popup window.
                */
                (void) sprintf(text,"%s %s\0",query,reply);
                popup_window->width=
                  XTextWidth(popup_window->font_info,text,strlen(text))+
                  22*popup_window->font_info->max_bounds.width;
                XResizeWindow(display,popup_window->id,popup_window->width,
                  popup_window->height);
                /*
                  Display reply in popup window.
                */
                x=2*font_info->max_bounds.width;
                XDrawString(display,popup_window->id,graphic_context,x,y,query,
                  strlen(query));
                x+=XTextWidth(font_info,query,strlen(query))+
                  font_info->max_bounds.width;
                XDrawString(display,popup_window->id,graphic_context,x,y,reply,
                  strlen(reply));
                x+=XTextWidth(font_info,reply,strlen(reply));
              }
            *p=(*command);
            XDrawString(display,popup_window->id,graphic_context,x,y,p,1);
            x+=XTextWidth(font_info,p,1);
            p++;
            break;
          }
        }
        /*
          Display text cursor.
        */
        *p=(char) NULL;
        XDrawString(display,popup_window->id,graphic_context,x,y,"_",1);
        break;
      }
      default:
        break;
    }
  } while (!exit_state);
  XWithdrawWindow(display,popup_window->id,popup_window->screen);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X R e a d C o l o r m a p                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XReadColormap returns the red, green, and blue colormap of a window.
%  Additionally, the number of colors in the colormap is returned.
%
%  The format of the XReadColormap function is:
%
%      number_colors=XReadColormap(display,window_attributes,colors)
%
%  A description of each parameter follows:
%
%    o number_colors: XReadColormap returns the number of colors in the
%      colormap.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o window_attributes: Specifies a pointer to the window attributes
%      structure;  returned from XGetWindowAttributes.
%
%    o colors: Specifies a an array of XColor structures.  The colormap
%      red, green, and blue are returned.
%
%
*/
unsigned int XReadColormap(display,window_attributes,colors)
Display
  *display;

XWindowAttributes
  *window_attributes;

XColor
  **colors;
{
  unsigned int
    number_colors;

  register int
    i;

  if (window_attributes->colormap == (Colormap) NULL)
    return(0);
  number_colors=window_attributes->visual->map_entries;
  *colors=(XColor *) malloc((unsigned) (number_colors*sizeof(XColor)));
  if (*colors == (XColor *) NULL)
    {
      Warning("unable to read colormap","memory allocation failed");
      return(0);
    }
  if ((window_attributes->visual->class != DirectColor) &&
      (window_attributes->visual->class != TrueColor))
    for (i=0; i < number_colors; i++)
    {
      (*colors)[i].pixel=i;
      (*colors)[i].pad=0;
    }
  else
    {
      unsigned long
        blue,
        blue_bit,
        green,
        green_bit,
        red,
        red_bit;

       /*
         DirectColor or TrueColor visual.
       */
       red=0;
       green=0;
       blue=0;
       red_bit=window_attributes->visual->red_mask &
         (~(window_attributes->visual->red_mask)+1);
       green_bit=window_attributes->visual->green_mask &
         (~(window_attributes->visual->green_mask)+1);
       blue_bit=window_attributes->visual->blue_mask &
         (~(window_attributes->visual->blue_mask)+1);
       for (i=0; i < number_colors; i++)
       {
         (*colors)[i].pixel=red | green | blue;
         (*colors)[i].pad=0;
         red+=red_bit;
         if (red > window_attributes->visual->red_mask)
           red=0;
         green+=green_bit;
         if (green > window_attributes->visual->green_mask)
           green=0;
         blue+=blue_bit;
         if (blue > window_attributes->visual->blue_mask)
           blue=0;
       }
     }
  XQueryColors(display,window_attributes->colormap,*colors,number_colors);
  return(number_colors);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X R e a d I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure XReadImage reads an image from an X window.
%
%  The format of the XReadImage routine is:
%
%      image=XReadImage(display,root_window,target_window,client_window,
%        screen,borders)
%
%  A description of each parameter follows:
%
%    o image: Specifies a pointer to the Image structure.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o root_window: Specifies the id of the root window.
%
%    o target_window: Specifies the id of the target window.
%
%    o client_window: Specifies the id of the client of the target window.
%
%    o borders: Specifies whether borders pixels are to be saved with
%      the image.
%
%
*/
Image *XReadImage(display,root_window,target_window,client_window,screen,
  borders)
Display
  *display;

Window
  root_window,
  target_window,
  client_window;

unsigned int
  screen,
  borders;
{
  Image
    *image;

  int
    display_width,
    display_height,
    x,
    y;

  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned long
    pixel;

  Window
    child;

  XColor
    *colors;

  XImage
    *ximage;

  XTextProperty
    window_name;

  XWindowAttributes
    client_attributes,
    target_attributes;

  /*
    Inform the user not to alter the screen.
  */
  XBell(display,0);
  /*
    Get the attributes of the window being dumped.
  */
  if ((XGetWindowAttributes(display,target_window,&target_attributes) == 0) ||
      (XGetWindowAttributes(display,client_window,&client_attributes) == 0))
    {
      Warning("unable to get target window attributes",(char *) NULL);
      return((Image *) NULL);
    }
  XTranslateCoordinates(display,target_window,root_window,0,0,&x,&y,&child);
  target_attributes.x=x;
  target_attributes.y=y;
  /*
    Allocate image structure.
  */
  image=AllocateImage();
  if (image == (Image *) NULL)
    {
      Warning("unable to read image","memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Initialize Image structure.
  */
  image->columns=target_attributes.width;
  image->rows=target_attributes.height;
  if (borders)
    {
      /*
        Do not include border in image.
      */
      x-=target_attributes.border_width;
      y-=target_attributes.border_width;
      image->columns+=2*target_attributes.border_width;
      image->rows+=2*target_attributes.border_width;
    }
  if (screen)
  /*
    clip to window
  */
  if (x < 0)
    {
      image->columns+=x;
      x=0;
    }
  if (y < 0)
    {
      image->rows+=y;
      y=0;
    }
  display_width=DisplayWidth(display,XDefaultScreen(display));
  display_height=DisplayHeight(display,XDefaultScreen(display));
  if ((x+(int) image->columns) > display_width)
    image->columns=display_width-x;
  if ((y+(int) image->rows) > display_height)
    image->rows=display_height-y;
  /*
    Get image from window with XGetImage.
  */
  if (screen)
    ximage=XGetImage(display,root_window,x,y,image->columns,image->rows,
      AllPlanes,ZPixmap);
  else
    {
      x-=target_attributes.x;
      y-=target_attributes.y;
      ximage=XGetImage(display,target_window,x,y,image->columns,image->rows,
        AllPlanes,ZPixmap);
    }
  if (ximage == (XImage *) NULL)
    {
      Warning("unable to read image",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Obtain the window colormap from the client of the target window.
  */
  image->colors=XReadColormap(display,&client_attributes,&colors);
  XBell(display,0);
  XBell(display,0);
  XFlush(display);
  /*
    Convert image to MIFF format.
  */
  image->comments=(char *) NULL;
  if (XGetWMName(display,target_window,&window_name) != 0)
    {
      /*
        Initial image comment.
      */
      image->comments=(char *)
        malloc((unsigned int) (strlen((char *) window_name.value)+256));
      if (image->comments == (char *) NULL)
        {
          Warning("unable to read image","memory allocation failed");
          DestroyImage(image);
          return((Image *) NULL);
        }
      (void) sprintf(image->comments,"\n  Imported from X11 window:  %s\n\0",
        window_name.value);
    }
  if ((target_attributes.visual->class == TrueColor) ||
      (target_attributes.visual->class == DirectColor))
    image->class=DirectClass;
  else
    image->class=PseudoClass;
  image->compression=RunlengthEncodedCompression;
  image->scene=0;
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    malloc(image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    {
      Warning("unable to read image","memory allocation failed");
      DestroyImage(image);
      return((Image *) NULL);
    }
  p=image->pixels;
  switch (image->class)
  {
    case DirectClass:
    {
      register unsigned long
        color,
        index;

      unsigned long
        blue_mask,
        blue_shift,
        green_mask,
        green_shift,
        red_mask,
        red_shift;

      /*
        Determine shift and mask for red, green, and blue.
      */
      red_mask=target_attributes.visual->red_mask;
      red_shift=0;
      while ((red_mask & 0x01) == 0)
      {
        red_mask>>=1;
        red_shift++;
      }
      green_mask=target_attributes.visual->green_mask;
      green_shift=0;
      while ((green_mask & 0x01) == 0)
      {
        green_mask>>=1;
        green_shift++;
      }
      blue_mask=target_attributes.visual->blue_mask;
      blue_shift=0;
      while ((blue_mask & 0x01) == 0)
      {
        blue_mask>>=1;
        blue_shift++;
      }
      /*
        Convert X image to DirectClass packets.
      */
      if ((image->colors > 0) &&
          (target_attributes.visual->class == DirectColor))
        for (y=0; y < image->rows; y++)
        {
          for (x=0; x < image->columns; x++)
          {
            pixel=XGetPixel(ximage,x,y);
            index=(pixel >> red_shift) & red_mask;
            p->red=(unsigned char) (colors[index].red >> 8);
            index=(pixel >> green_shift) & green_mask;
            p->green=(unsigned char) (colors[index].green >> 8);
            index=(pixel >> blue_shift) & blue_mask;
            p->blue=(unsigned char) (colors[index].blue >> 8);
            p->index=0;
            p->length=0;
            p++;
          }
        }
      else
        for (y=0; y < image->rows; y++)
          for (x=0; x < image->columns; x++)
          {
            pixel=XGetPixel(ximage,x,y);
            color=(pixel >> red_shift) & red_mask;
            p->red=(unsigned char)
              ((((unsigned long) color*65535)/red_mask) >> 8);
            color=(pixel >> green_shift) & green_mask;
            p->green=(unsigned char)
              ((((unsigned long) color*65535)/green_mask) >> 8);
            color=(pixel >> blue_shift) & blue_mask;
            p->blue=(unsigned char)
              ((((unsigned long) color*65535)/blue_mask) >> 8);
            p->index=0;
            p->length=0;
            p++;
          }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert X image to PseudoClass packets.
      */
      image->colormap=(ColorPacket *)
        malloc((unsigned) (image->colors*sizeof(ColorPacket)));
      if (image->colormap == (ColorPacket *) NULL)
        {
          Warning("unable to read image","memory allocation failed");
          DestroyImage(image);
          return((Image *) NULL);
        }
      for (i=0; i < image->colors; i++)
      {
        image->colormap[i].red=colors[i].red >> 8;
        image->colormap[i].green=colors[i].green >> 8;
        image->colormap[i].blue=colors[i].blue >> 8;
      }
      for (y=0; y < image->rows; y++)
        for (x=0; x < image->columns; x++)
        {
          pixel=XGetPixel(ximage,x,y);
          p->red=(unsigned char) (colors[pixel].red >> 8);
          p->green=(unsigned char) (colors[pixel].green >> 8);
          p->blue=(unsigned char) (colors[pixel].blue >> 8);
          p->index=(unsigned short) pixel;
          p->length=0;
          p++;
        }
      break;
    }
  }
  /*
    Free image and colormap.
  */
  if (image->colors > 0)
    (void) free((char *) colors);
  XDestroyImage(ximage);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t P i x e l I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetPixelInfo initializes the PixelInfo structure.
%
%  The format of the XMakeStandardColormap routine is:
%
%      XGetPixelInfo(display,visual_info,map_info,resource_info,pixel_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o map_info: If map_type is specified, this structure is initialized
%      with info from the Standard Colormap.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o colormap: Specifies the ID of the colormap from which the color
%      information is retrieved.
%
%    o pixel_info: Specifies a pointer to a XPixelInfo structure.
%
%
*/
void XGetPixelInfo(display,visual_info,map_info,resource_info,pixel_info)
Display
  *display;

XVisualInfo
  *visual_info;

XStandardColormap
  *map_info;

XResourceInfo
  *resource_info;

XPixelInfo
  *pixel_info;
{
  register int
    i,
    status;

  pixel_info->colors=0;
  pixel_info->pixels=(unsigned long *) NULL;
  status=XParseColor(display,map_info->colormap,resource_info->background_color,
    &pixel_info->background_color);
  if (status == 0)
    Warning("color is not known to X server",resource_info->background_color);
  pixel_info->background_color.pixel=XBlackPixel(display,visual_info->screen);
  status=XParseColor(display,map_info->colormap,resource_info->border_color,
    &pixel_info->border_color);
  if (status == 0)
    Warning("color is not known to X server",resource_info->border_color);
  pixel_info->border_color.pixel=XWhitePixel(display,visual_info->screen);
  status=XParseColor(display,map_info->colormap,resource_info->foreground_color,
    &pixel_info->foreground_color);
  if (status == 0)
    Warning("color is not known to X server",resource_info->foreground_color);
  pixel_info->foreground_color.pixel=XWhitePixel(display,visual_info->screen);
  for (i=0; i < 9; i++)
  {
    status=XParseColor(display,map_info->colormap,resource_info->pen_color[i],
      &pixel_info->pen_color[i]);
    if (status == 0)
      Warning("color is not known to X server",resource_info->pen_color[i]);
    pixel_info->pen_color[i].pixel=XWhitePixel(display,visual_info->screen);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t R e s o u r c e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetResource queries the X server for the specified resource name 
%  or class.  If the resource name or class is not defined in the database, the
%  supplied default value is returned.
%
%  The format of the XGetResource routine is:
%
%      value=XGetResource(database,application_name,name,class,resource_default)
%
%  A description of each parameter follows:
%
%    o value: Function XGetResource returns the resource value associated with 
%      the name or class.  If none is found, the supplied default value is 
%      returned.
%
%    o database: Specifies a resource database; returned from
%      XrmGetStringDatabase.
%
%    o application_name:  Specifies the application name used to retrieve
%      resource info from the X server database.
%
%    o name: Specifies the name of the value being retrieved.
%
%    o class: Specifies the class of the value being retrieved.
%
%    o resource_default: Specifies the default value to return if the query 
%      fails to find the specified name/class.
%
%
*/
char *XGetResource(database,application_name,name,class,resource_default)
XrmDatabase
  database;

char
  *application_name,
  *name,
  *class,
  *resource_default;
{
  char
    *resource_type,
    resource_class[1024],
    resource_name[1024];

  int
    status;

  XrmValue
    resource_value;

  if (database == (XrmDatabase) NULL)
    return(resource_default);
  *resource_name=(char) NULL;
  if (name != (char *) NULL)
    (void) sprintf(resource_name,"%s.%s\0",application_name,name);
  *resource_class=(char) NULL;
  if (class != (char *) NULL)
    (void) sprintf(resource_class,"%s.%s\0",application_name,class);
  status=XrmGetResource(database,resource_name,resource_class,&resource_type,
    &resource_value);
  if (status == False)
    return(resource_default);
  return(resource_value.addr);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t R e s o u r c e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetResourceInfo initializes the ResourceInfo structure.
%
%  The format of the XGetResourceInfo routine is:
%
%      XGetResourceInfo(resource_database,application_name,resource_info)
%
%  A description of each parameter follows:
%
%    o resource_database: Specifies a resource database; returned from
%      XrmGetStringDatabase.
%
%    o application_name:  Specifies the application name used to retrieve
%      resource info from the X server database.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%
*/
void XGetResourceInfo(resource_database,application_name,resource_info)
XrmDatabase
  resource_database;

char
  *application_name;

XResourceInfo
  *resource_info;
{
  char
    *resource_value;

  /*
    Initialize resource info fields.
  */
  resource_value=XGetResource(resource_database,application_name,"backdrop",
    (char *) NULL,"False");
  resource_info->backdrop=IsTrue(resource_value);
  resource_info->background_color=XGetResource(resource_database,
    application_name,"background","Background","black");
  resource_info->border_color=XGetResource(resource_database,application_name,
    "borderColor","BorderColor","white");
  resource_value=XGetResource(resource_database,application_name,"borderWidth",
    "BorderWidth","2");
  resource_info->border_width=atoi(resource_value);
  resource_value=XGetResource(resource_database,application_name,"delay",
    "Delay","0");
  resource_info->delay=atoi(resource_value);
  resource_info->font=XGetResource(resource_database,application_name,
    "font","Font",(char *) NULL);
  resource_info->font_name[0]=XGetResource(resource_database,application_name,
    "font1","Font1","fixed");
  resource_info->font_name[1]=XGetResource(resource_database,application_name,
    "font2","Font2","variable");
  resource_info->font_name[2]=XGetResource(resource_database,application_name,
    "font3","Font3","5x8");
  resource_info->font_name[3]=XGetResource(resource_database,application_name,
    "font4","Font4","6x10");
  resource_info->font_name[4]=XGetResource(resource_database,application_name,
    "font5","Font5","7x13bold");
  resource_info->font_name[5]=XGetResource(resource_database,application_name,
    "font6","Font6","8x13bold");
  resource_info->font_name[6]=XGetResource(resource_database,application_name,
    "font7","Font7","9x15bold");
  resource_info->font_name[7]=XGetResource(resource_database,application_name,
    "font8","Font8","10x20");
  resource_info->font_name[8]=XGetResource(resource_database,application_name,
    "font9","Font9","12x24");
  resource_info->foreground_color=XGetResource(resource_database,
    application_name,"foreground","Foreground","white");
  resource_info->icon_geometry=XGetResource(resource_database,
    application_name,"iconGeometry","IconGeometry",(char *) NULL);
  resource_value=XGetResource(resource_database,application_name,"iconic",
    "Iconic","False");
  resource_info->iconic=IsTrue(resource_value);
  resource_info->image_geometry=XGetResource(resource_database,
    application_name,"imageGeometry","ImageGeometry",(char *) NULL);
  resource_value=XGetResource(resource_database,application_name,"magnify",
    (char *) NULL,"2");
  resource_info->magnify=atoi(resource_value);
  resource_info->map_type=XGetResource(resource_database,application_name,"map",
    "Map",(char *) NULL);
  resource_value=XGetResource(resource_database,application_name,"monochrome",
    (char *) NULL,"False");
  resource_info->monochrome=IsTrue(resource_value);
  resource_info->name=XGetResource(resource_database,application_name,"name",
    "Name",(char *) NULL);
  resource_info->pen_color[0]=XGetResource(resource_database,application_name,
    "pen1","Pen1","black");
  resource_info->pen_color[1]=XGetResource(resource_database,application_name,
    "pen2","Pen2","blue");
  resource_info->pen_color[2]=XGetResource(resource_database,application_name,
    "pen3","Pen3","cyan");
  resource_info->pen_color[3]=XGetResource(resource_database,application_name,
    "pen4","Pen4","green");
  resource_info->pen_color[4]=XGetResource(resource_database,application_name,
    "pen5","Pen5","gray");
  resource_info->pen_color[5]=XGetResource(resource_database,application_name,
    "pen6","Pen6","red");
  resource_info->pen_color[6]=XGetResource(resource_database,application_name,
    "pen7","Pen7","magenta");
  resource_info->pen_color[7]=XGetResource(resource_database,application_name,
    "pen8","Pen8","yellow");
  resource_info->pen_color[8]=XGetResource(resource_database,application_name,
    "pen9","Pen9","white");
  resource_info->title=XGetResource(resource_database,application_name,"title",
    "Title",(char *) NULL);
  resource_info->visual_type=XGetResource(resource_database,application_name,
    "visual","Visual",(char *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X S e l e c t W i n d o w                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XSelectWindow allows a user to select a window using the mouse.
%
%  The format of the XSelectWindow function is:
%
%      target_window=XSelectWindow(display,root_window)
%
%  A description of each parameter follows:
%
%    o window: XSelectWindow returns the window id.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o root_window: Specifies the window id of the root window.
%
%
*/
Window XSelectWindow(display,root_window)
Display
  *display;

Window
  root_window;
{
  Cursor
    crosshair_cursor;

  int
    presses,
    status;

  Window
    target_window;

  XEvent
    event;

  /*
    Make the target cursor.
  */
  crosshair_cursor=XCreateFontCursor(display,XC_crosshair);
  /*
    Grab the pointer using target cursor.
  */
  status=XGrabPointer(display,root_window,False,ButtonPressMask |
    ButtonReleaseMask,GrabModeSync,GrabModeAsync,root_window,crosshair_cursor,
    CurrentTime);
  if (status != GrabSuccess)
    Error("unable to grab the mouse",(char *) NULL);
  /*
    Select a window.
  */
  target_window=(Window) NULL;
  presses=0;
  do
  {
    /*
      Allow another event.
    */
    XAllowEvents(display,SyncPointer,CurrentTime);
    XWindowEvent(display,root_window,ButtonPressMask | ButtonReleaseMask,
      &event);
    switch (event.type)
    {
      case ButtonPress:
      {
        if (target_window == (Window) NULL)
          {
            target_window=event.xbutton.subwindow;
            if (target_window == (Window) NULL)
              target_window=root_window;
          }
        presses++;
        break;
      }
      case ButtonRelease:
      {
        presses--;
        break;
      }
      default:
        break;
    }
  }
  while ((target_window == (Window) NULL) || (presses > 0));
  XUngrabPointer(display,CurrentTime);
  XFreeCursor(display,crosshair_cursor);
  return(target_window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X V i s u a l C l a s s N a m e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XVisualClassName returns the visual class name as a character
%  string.
%
%  The format of the XVisualClassName routine is:
%
%      visual_type=XVisualClassName(visual_info)
%
%  A description of each parameter follows:
%
%    o visual_type: XVisualClassName returns the visual class as a character
%      string.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%
*/
char *XVisualClassName(visual_info)
XVisualInfo
  *visual_info;
{
  if (visual_info == (XVisualInfo *) NULL)
    return((char *) NULL);
  switch (visual_info->class)
  {
    case StaticGray: return("StaticGray");
    case GrayScale: return("GrayScale");
    case StaticColor: return("StaticColor");
    case PseudoColor: return("PseudoColor");
    case TrueColor: return("TrueColor");
    case DirectColor: return("DirectColor");
  }
  return("unknown visual class");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X W i n d o w B y I D                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XWindowByID locates a child window with a given ID.  If not window
%  with the given name is found, 0 is returned.   Only the window specified
%  and its subwindows are searched.
%
%  The format of the XWindowByID function is:
%
%      child=XWindowByID(display,window,id)
%
%  A description of each parameter follows:
%
%    o child: XWindowByProperty returns the window with the specified
%      id.  If no windows are found, XWindowByProperty returns 0.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o id: Specifies the id of the window to locate.
%
%
*/
Window XWindowByID(display,root_window,id)
Display
  *display;

Window
  root_window;

unsigned long
  id;
{
  register int
    i;

  unsigned int
    number_children;

  Window
    *children,
    child,
    window;

  if (root_window == id)
    return(id);
  if (!XQueryTree(display,root_window,&child,&child,&children,&number_children))
    return((Window) NULL);
  window=(Window) NULL;
  for (i=0; i < number_children; i++)
  {
    /*
      Search each child and their children.
    */
    window=XWindowByID(display,children[i],id);
    if (window != (Window) NULL)
      break;
  }
  if (children != (Window *) NULL)
    XFree((void *) children);
  return(window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X W i n d o w B y N a m e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XWindowByName locates a window with a given name on a display.
%  If no window with the given name is found, 0 is returned. If more than
%  one window has the given name, the first one is returned.  Only root and
%  its children are searched.
%
%  The format of the XWindowByName function is:
%
%      window=XWindowByName(display,root_window,name)
%
%  A description of each parameter follows:
%
%    o window: XWindowByName returns the window id.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o root_window: Specifies the id of the root window.
%
%    o name: Specifies the name of the window to locate.
%
%
*/
Window XWindowByName(display,root_window,name)
Display
  *display;

Window
  root_window;

char
  *name;
{
  register int
    i;

  unsigned int
    number_children;

  Window
    *children,
    child,
    window;

  XTextProperty
    window_name;

  if (XGetWMName(display,root_window,&window_name) != 0)
    if (strcmp((char *) window_name.value,name) == 0)
      return(root_window);
  if (!XQueryTree(display,root_window,&child,&child,&children,&number_children))
    return((Window) NULL);
  window=(Window) NULL;
  for (i=0; i < number_children; i++)
  {
    /*
      Search each child and their children.
    */
    window=XWindowByName(display,children[i],name);
    if (window != (Window) NULL)
      break;
  }
  if (children != (Window *) NULL)
    XFree((void *) children);
  return(window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X W i n d o w B y P r o p e r y                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XWindowByProperty locates a child window with a given property.
%  If no window with the given name is found, 0 is returned.  If more than
%  one window has the given property, the first one is returned.  Only the
%  window specified and its subwindows are searched.
%
%  The format of the XWindowByProperty function is:
%
%      child=XWindowByProperty(display,window,property)
%
%  A description of each parameter follows:
%
%    o child: XWindowByProperty returns the window id with the specified
%      property.  If no windows are found, XWindowByProperty returns 0.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o property: Specifies the property of the window to locate.
%
%
*/
Window XWindowByProperty(display,window,property)
Display
  *display;

Window
  window;

Atom
  property;
{
  Atom
    type;

  int
    format;

  unsigned char
    *data;

  unsigned int
    i,
    number_children;

  unsigned long
    after,
    number_items;

  Window
    *children,
    child,
    parent,
    root;

  if (XQueryTree(display,window,&root,&parent,&children,&number_children) == 0)
    return((Window) NULL);
  type=(Atom) NULL;
  child=(Window) NULL;
  for (i=0; (i < number_children) && (child == (Window) NULL); i++)
  {
    (void) XGetWindowProperty(display,children[i],property,0L,0L,False,
      AnyPropertyType,&type,&format,&number_items,&after,&data);
    if (type != (Atom) NULL)
      child=children[i];
  }
  for (i=0; (i < number_children) && (child == (Window) NULL); i++)
    child=XWindowByProperty(display,children[i],property);
  if (children != (Window *) NULL)
    XFree((void *) children);
  return(child);
}
