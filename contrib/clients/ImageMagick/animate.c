/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%              AAA   N   N  IIIII  M   M   AAA   TTTTT  EEEEE                 %
%             A   A  NN  N    I    MM MM  A   A    T    E                     %
%             AAAAA  N N N    I    M M M  AAAAA    T    EEE                   %
%             A   A  N  NN    I    M   M  A   A    T    E                     %
%             A   A  N   N  IIIII  M   M  A   A    T    EEEEE                 %
%                                                                             %
%                                                                             %
%          Animate Machine Independent File Format Image via X11.             %
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
%  Animate displays a sequence of images in the MIFF format on any
%  workstation display running an X server.  Animate first determines the
%  hardware capabilities of the workstation.  If the number of unique
%  colors in an image is less than or equal to the number the workstation
%  can support, the image is displayed in an X window.  Otherwise the
%  number of colors in the image is first reduced to match the color
%  resolution of the workstation before it is displayed.
%
%  This means that a continuous-tone 24 bits/pixel image can display on a
%  8 bit pseudo-color device or monochrome device.  In most instances the
%  reduced color image closely resembles the original.  Alternatively, a
%  monochrome or pseudo-color image can display on a continuous-tone 24
%  bits/pixels device.
%
%  The Animate program command syntax is:
%
%  Usage: animate [options ...] file [ [options ...] file ...]
%
%  Where options include:
%    -backdrop            display image centered on a backdrop
%    -clip geometry       preferred size and location of the clipped image
%    -colors value        preferred number of colors in the image
%    -delay milliseconds  display the next image after pausing
%    -display server      display image to this X server
%    -dither              apply Floyd/Steinberg error diffusion to image
%    -gamma value         level of gamma correction
%    -geometry geometry   preferred size and location of the image window
%    -gray                transform image to gray scale colors
%    -map type            display image using this Standard Colormap
%    -monochrome          transform image to black and white
%    -reflect             reverse image scanlines
%    -rotate degrees      apply Paeth rotation to the image
%    -scale geometry      preferred size factors of the image
%    -treedepth value     depth of the color classification tree
%    -verbose             print detailed information about the image
%    -visual type         display image using this visual type
%
%  In addition to those listed above, you can specify these standard X
%  resources as command line options:  -background, -bordercolor,
%  -borderwidth, -font, -foreground, -iconGeometry, -iconic, -name, or
%  -title.
%
%  Change '-' to '+' in any option above to reverse its effect.  For
%  example, specify +compress to store the image as uncompressed.
%
%  Specify 'file' as '-' for standard input or output.
%
%  Buttons
%    1    press and drag to select a command from a pop-up menu
%
%  Keys
%    0-9  press to change the level of delay
%    p    press to animate the sequence of images
%    s    press to display the next image in the sequence
%    .    press to continually display the sequence of images
%    a    press to automatically reverse the sequence of images
%    f    press to animate in the forward direction
%    r    press to animate in the reverse direction
%    i    press to display information about the image
%    q    press to discard all images and exit program
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
  Define declarations.
*/
#define AutoReverseAnimationState 0x0001
#define ConfigureWindowState  0x0002
#define DefaultState  0x0004
#define ExitState  0x0008
#define ForwardAnimationState 0x0010
#define HighlightState  0x0020
#define InfoMappedState  0x0040
#define PlayAnimationState 0x0080
#define RepeatAnimationState 0x0100
#define StepAnimationState 0x0200

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e l a y                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Delay suspends animation for the number of milliseconds specified.
%
%  The format of the Delay routine is:
%
%      Delay(milliseconds)
%
%  A description of each parameter follows:
%
%    o milliseconds: Specifies the number of milliseconds to delay before
%      returning.
%
%
*/
void Timer()
{
}

void Delay(milliseconds)
unsigned long
  milliseconds;
{
#ifdef unix
#ifdef SYSV
#include <poll.h>
  if (milliseconds == 0)
    return;
  poll((struct pollfd *) NULL,(unsigned long) NULL,(int) (milliseconds/1000));
#else
  struct timeval
    timer;

  if (milliseconds == 0)
    return;
  timer.tv_sec=milliseconds/1000;
  timer.tv_usec=(milliseconds % 1000)*1000;
  (void) select(0,(void *) NULL,(void *) NULL,(void *) NULL,timer);
#endif
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E r r o r                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Error displays an error message and then terminates the program.
%
%  The format of the Error routine is:
%
%      Error(message,qualifier)
%
%  A description of each parameter follows:
%
%    o message: Specifies the message to display before terminating the
%      program.
%
%    o qualifier: Specifies any qualifier to the message.
%
%
*/
void Error(message,qualifier)
char
  *message,
  *qualifier;
{
  (void) fprintf(stderr,"%s: %s",application_name,message);
  if (qualifier != (char *) NULL)
    (void) fprintf(stderr," (%s)",qualifier);
  (void) fprintf(stderr,".\n");
  exit(1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U s a g e                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Usage displays the program command syntax.
%
%  The format of the Usage routine is:
%
%      Usage(message,terminate)
%
%  A description of each parameter follows:
%
%    o message: Specifies a specific message to display to the user.
%
%    o terminate: The program will exit if the value is not zero.
%
%
*/
static void Usage(message,terminate)
char
  *message;

unsigned int
  terminate;
{
  char
    **p;

  static char
    *buttons[]=
    {
      "1    press and drag to select a command from a pop-up menu",
      (char *) NULL
    },
    *keys[]=
    {
      "0-9  press to change the level of delay",
      "p    press to animate the sequence of images",
      "s    press to display the next image in the sequence",
      ".    press to continually display the sequence of images",
      "a    press to automatically reverse the sequence of images",
      "f    press to animate in the forward direction",
      "r    press to animate in the reverse direction",
      "i    press to display information about the image",
      "q    press to discard all images and exit program",
      (char *) NULL
    },
    *options[]=
    {
      "-backdrop            display image centered on a backdrop",
      "-clip geometry       preferred size and location of the clipped image",
      "-colors value        preferred number of colors in the image",
      "-delay milliseconds  display the next image after pausing",
      "-display server      display image to this X server",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-gamma value         level of gamma correction",
      "-geometry geometry   preferred size and location of the image window",
      "-gray                transform image to gray scale colors",
      "-map type            display image using this Standard Colormap",
      "-monochrome          transform image to black and white",
      "-reflect             reflect the image scanlines",
      "-rotate degrees      apply Paeth rotation to the image",
      "-scale geometry      preferred size factors of the image",
      "-treedepth value     depth of the color classification tree",
      "-verbose             print detailed information about the image",
      "-visual type         display image using this visual type",
      (char *) NULL
    };
  if (message != (char *) NULL)
    (void) fprintf(stderr,"Can't continue, %s\n\n",message);
  (void) fprintf(stderr,
    "Usage: %s [-options ...] file [ [-options ...] file ...]\n",
    application_name);
  (void) fprintf(stderr,"\nWhere options include: \n");
  for (p=options; *p != (char *) NULL; p++)
    (void) fprintf(stderr,"  %s\n",*p);
  (void) fprintf(stderr,
    "\nIn addition to those listed above, you can specify these standard X\n");
  (void) fprintf(stderr,
    "resources as command line options:  -background, -bordercolor,\n");
  (void) fprintf(stderr,
    "-borderwidth, -font, -foreground, -iconGeometry, -iconic, -name, or\n");
  (void) fprintf(stderr,"-title.\n");
  (void) fprintf(stderr,
    "\nChange '-' to '+' in any option above to reverse its effect.  For\n");
  (void) fprintf(stderr,
    "example, specify +compress to store the image as uncompressed.\n");
  (void) fprintf(stderr,
    "\nSpecify 'file' as '-' for standard input or output.\n");
  (void) fprintf(stderr,"\nButtons: \n");
  for (p=buttons; *p != (char *) NULL; p++)
    (void) fprintf(stderr,"  %s\n",*p);
  (void) fprintf(stderr,"\nKeys: \n");
  for (p=keys; *p != (char *) NULL; p++)
    (void) fprintf(stderr,"  %s\n",*p);
  if (terminate)
    exit(1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U s e r C o m m a n d                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function UserCommand makes a transform to the image or image window as
%  specified by a user menu button or keyboard command.
%
%  The format of the UserCommand routine is:
%
%    UserCommand(display,info_window,image_window,image,command,state);
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o info_window: Specifies a pointer to a XWindowInfo structure.
%
%    o image_window: Specifies a pointer to a XWindowInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  UserCommand
%      may transform the image and return a new image pointer.
%
%    o state: Specifies an unsigned int;  UserCommand may return a
%      modified state.
%
%
*/
static void UserCommand(display,info_window,image_window,command,image,state)
Display
  *display;

XWindowInfo
  *info_window,
  *image_window;

char
  command;

Image
  **image;

unsigned int
  *state;
{
  if (*state & InfoMappedState)
    XWithdrawWindow(display,info_window->id,info_window->screen);
  /*
    Process user command.
  */
  switch (command)
  {
    case ' ':
    case '\0':
      break;
    case '.':
    {
      *state|=RepeatAnimationState;
      *state&=(~AutoReverseAnimationState);
      *state|=PlayAnimationState;
      break;
    }
    case 'a':
    {
      *state|=AutoReverseAnimationState;
      *state&=(~RepeatAnimationState);
      *state|=PlayAnimationState;
      break;
    }
    case 'f':
    {
      *state=ForwardAnimationState;
      *state&=(~AutoReverseAnimationState);
      break;
    }
    case 'i':
    {
      char
        text[1024];

      /*
        Display information about the image in the info window.
      */
      (void) sprintf(text," [%d] %s %dx%d \0",(*image)->scene,
        (*image)->filename,image_window->width,image_window->height);
      if ((*image)->colors > 0)
        (void) sprintf(text,"%s%dc \0",text,(*image)->colors);
      XSetWindowExtents(info_window,text,2);
      XMapWindow(display,info_window->id);
      XDrawImageString(display,info_window->id,info_window->graphic_context,2,
        info_window->font_info->ascent+2,text,(unsigned int) strlen(text));
      break;
    }
    case 'p':
    {
      *state|=PlayAnimationState;
      *state&=(~AutoReverseAnimationState);
      break;
    }
    case 's':
    {
      *state|=StepAnimationState;
      *state&=(~PlayAnimationState);
      break;
    }
    case 'q':
    {
      /*
        Exit program
      */
      *state|=ExitState;  /* exit program */
      break;
    }
    case 'r':
    {
      *state&=(~ForwardAnimationState);
      *state&=(~AutoReverseAnimationState);
      break;
    }
    default:
    {
      XBell(display,0);
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X A n i m a t e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XAnimateImage displays an image via X11.
%
%  The format of the XAnimateImage routine is:
%
%      XAnimateImage(display,resource_info,argv,argc,image,number_scenes)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o argv: Specifies the application's argument list.
%
%    o argc: Specifies the number of arguments.
%
%    o image: Specifies a pointer to a Image structure; returned from
%      ReadImage.
%
%    o number_scenes: Specifies the number of scenes to animate.
%
%
*/
static int LinearCompare(image_1,image_2)
Image
  **image_1,
  **image_2;
{
  return((int) (*image_1)->scene-(int) (*image_2)->scene);
}

static void XAnimateImage(display,resource_info,argv,argc,images,number_scenes)
Display
  *display;

XResourceInfo
  *resource_info;

char
  **argv;

int
  argc;

Image
  **images;

unsigned int
  number_scenes;
{
  Atom
    delete_property,
    protocols_property;

  Colormap
    colormap;

  char
    text[1024];

  Cursor
    arrow_cursor,
    watch_cursor;

  GC
    graphic_context;

  int
    scene;

  unsigned int
    state;

  Window
    root_window;

  XClassHint
    *class_hint;

  XEvent
    event;

  XFontStruct
    *font_info;

  XGCValues
    graphic_context_value;

  XPixelInfo
    *pixel_info;

  XStandardColormap
    **map_info;

  XVisualInfo
    *visual_info;

  XWindowInfo
    backdrop_window,
    icon_window,
    image_window,
    info_window,
    popup_window,
    superclass_window;

  XWMHints
    *manager_hints;

  /*
    Sort images by increasing scene number.
  */
  qsort((char *) images,(int) number_scenes,sizeof(Image *),LinearCompare);
  /*
    Allocate pixel and map info.
  */
  pixel_info=(XPixelInfo *) malloc(number_scenes*sizeof(XPixelInfo));
  map_info=(XStandardColormap **)
    malloc(number_scenes*sizeof(XStandardColormap *));
  if ((pixel_info == (XPixelInfo *) NULL) ||
      (map_info == (XStandardColormap **) NULL))
    Error("unable to animate images","memory allocation failed");
  /*
    Get the best visual this server supports.
  */
  visual_info=XBestVisualInfo(display,resource_info->visual_type,
    resource_info->map_type,&map_info[0]);
  if (visual_info == (XVisualInfo *) NULL)
    Error("unable to get visual",resource_info->visual_type);
  /*
    Initialize colormap.
  */
  if (resource_info->map_type == (char *) NULL)
    map_info[0]=XMakeStandardColormap(display,visual_info,resource_info,
      &pixel_info[0],images[0]);
  else
    {
      register int
        i;

      XGetPixelInfo(display,visual_info,map_info[0],resource_info,
        &pixel_info[0]);
      if (images[0]->class == PseudoClass)
        {
          /*
            Initialize pixel array for images of type PseudoClass.
          */
          pixel_info[0].colors=images[0]->colors;
          pixel_info[0].pixels=(unsigned long *)
            malloc((unsigned int) images[0]->colors*sizeof(unsigned long));
          if (pixel_info[0].pixels == (unsigned long *) NULL)
            Error("unable to create colormap","memory allocation failed");
          for (i=0; i < images[0]->colors; i++)
            pixel_info[0].pixels[i]=
              XStandardPixel(map_info[0],images[0]->colormap[i],8);
        }
      /*
        Define background/border/foreground pixels.
      */
      pixel_info[0].background_color.pixel=
        XStandardPixel(map_info[0],pixel_info->background_color,16);
      pixel_info[0].border_color.pixel=
        XStandardPixel(map_info[0],pixel_info->border_color,16);
      pixel_info[0].foreground_color.pixel=
        XStandardPixel(map_info[0],pixel_info->foreground_color,16);
    }
  colormap=map_info[0]->colormap;
  /*
    Initialize font info.
  */
  (void) sprintf(text," [%d] %s %dx%d \0",images[0]->scene,images[0]->filename,
    images[0]->columns,images[0]->rows);
  if (images[0]->colors > 0)
    (void) sprintf(text,"%s%dc \0",text,images[0]->colors);
  font_info=XBestFont(display,resource_info,text,images[0]->columns);
  if (font_info == (XFontStruct *) NULL)
    Error("unable to load font",resource_info->font);
  /*
    Initialize cursor.
  */
  arrow_cursor=XCreateFontCursor(display,XC_arrow);
  watch_cursor=XCreateFontCursor(display,XC_watch);
  if ((arrow_cursor == (Cursor) NULL) || (watch_cursor == (Cursor) NULL))
    Error("unable to create cursor",(char *) NULL);
  /*
    Initialize atoms.
  */
  protocols_property=XInternAtom(display,"WM_PROTOCOLS",False);
  delete_property=XInternAtom(display,"WM_DELETE_WINDOW",False);
  if ((protocols_property == (Atom) NULL) || (delete_property == (Atom) NULL))
    Error("unable to create property",(char *) NULL);
  /*
    Initialize class and manager hints.
  */
  class_hint=XAllocClassHint();
  manager_hints=XAllocWMHints();
  if ((class_hint == (XClassHint *) NULL) ||
      (manager_hints == (XWMHints *) NULL))
    Error("unable to allocate X hints",(char *) NULL);
  if (resource_info->name == (char *) NULL)
    class_hint->res_name=application_name;
  else
    class_hint->res_name=resource_info->name;
  class_hint->res_class=(char *) "ImageMagick";
  manager_hints->flags=InputHint | StateHint;
  manager_hints->input=False;
  manager_hints->initial_state=NormalState;
  /*
    Window superclass.
  */
  superclass_window.screen=visual_info->screen;
  superclass_window.depth=visual_info->depth;
  superclass_window.visual_info=visual_info;
  superclass_window.map_info=map_info[0];
  superclass_window.pixel_info=(&pixel_info[0]);
  superclass_window.font_info=font_info;
  superclass_window.cursor=arrow_cursor;
  superclass_window.busy_cursor=watch_cursor;
  superclass_window.name="ImageMagick SuperClass";
  superclass_window.geometry=(char *) NULL;
  superclass_window.icon_geometry=resource_info->icon_geometry;
  superclass_window.clip_geometry=(char *) NULL;
  superclass_window.flags=PSize;
  superclass_window.x=0;
  superclass_window.y=0;
  superclass_window.width=1;
  superclass_window.height=1;
  superclass_window.min_width=0;
  superclass_window.min_height=0;
  superclass_window.width_inc=1;
  superclass_window.height_inc=1;
  superclass_window.border_width=WindowBorderWidth;
  superclass_window.immutable=True;
  superclass_window.attributes.background_pixel=
    pixel_info[0].background_color.pixel;
  superclass_window.attributes.background_pixmap=(Pixmap) NULL;
  superclass_window.attributes.backing_store=WhenMapped;
  superclass_window.attributes.bit_gravity=ForgetGravity;
  superclass_window.attributes.border_pixel=pixel_info[0].border_color.pixel;
  superclass_window.attributes.colormap=colormap;
  superclass_window.attributes.cursor=arrow_cursor;
  superclass_window.attributes.do_not_propagate_mask=NoEventMask;
  superclass_window.attributes.event_mask=NoEventMask;
  superclass_window.attributes.override_redirect=False;
  superclass_window.attributes.save_under=False;
  superclass_window.attributes.win_gravity=NorthWestGravity;
  superclass_window.graphic_context=(GC) NULL;
  superclass_window.ximage=(XImage *) NULL;
  /*
    Initialize graphic context.
  */
  root_window=XRootWindow(display,visual_info->screen);
  XMakeWindow(display,root_window,argv,argc,class_hint,manager_hints,
    delete_property,&superclass_window);
  graphic_context_value.background=pixel_info[0].background_color.pixel;
  graphic_context_value.foreground=pixel_info[0].foreground_color.pixel;
  graphic_context_value.font=font_info->fid;
  graphic_context_value.function=GXcopy;
  graphic_context_value.line_width=2;
  graphic_context_value.graphics_exposures=False;
  graphic_context_value.plane_mask=AllPlanes;
  graphic_context_value.subwindow_mode=IncludeInferiors;
  graphic_context=XCreateGC(display,superclass_window.id,GCBackground |
    GCFont | GCForeground | GCFunction | GCGraphicsExposures | GCLineWidth |
    GCPlaneMask | GCSubwindowMode,&graphic_context_value);
  if (graphic_context == (GC) NULL)
    Error("unable to create graphic context",(char *) NULL);
  superclass_window.graphic_context=graphic_context;
  graphic_context_value.background=pixel_info[0].foreground_color.pixel;
  graphic_context_value.foreground=pixel_info[0].background_color.pixel;
  graphic_context=XCreateGC(display,superclass_window.id,GCBackground |
    GCFont | GCForeground | GCFunction | GCLineWidth | GCPlaneMask,
    &graphic_context_value);
  if (graphic_context == (GC) NULL)
    Error("unable to create graphic context",(char *) NULL);
  superclass_window.highlight_context=graphic_context;
  /*
    Initialize icon window.
  */
  icon_window=superclass_window;
  icon_window.name="ImageMagick Icon";
  XBestIconSize(display,&icon_window,images[0]);
  icon_window.attributes.event_mask=ExposureMask | StructureNotifyMask;
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=False;
  manager_hints->initial_state=IconicState;
  manager_hints->window_group=superclass_window.id;
  XMakeWindow(display,root_window,argv,argc,class_hint,manager_hints,
    delete_property,&icon_window);
  /*
    Create icon image.
  */
  icon_window.ximage=XMakeImage(display,resource_info,&icon_window,images[0],
    icon_window.width,icon_window.height);
  if (icon_window.ximage == (XImage *) NULL)
    Error("unable to create icon image",(char *) NULL);
  /*
    Initialize image window.
  */
  image_window=superclass_window;
  image_window.name=(char *) malloc(256);
  if (image_window.name == NULL)
    Error("unable to create image window","memory allocation failed");
  if (resource_info->title != (char *) NULL)
    (void) strcpy(image_window.name,resource_info->title);
  else
    {
      register char
        *p;

      (void) strcpy(image_window.name,"ImageMagick: ");
      (void) strcat(image_window.name,images[0]->filename);
      p=image_window.name;
      while (*p != (char) NULL)
      {
        if (*p == '.')
          {
            *p=(char) NULL;
            break;
          }
        p++;
      }
    }
  image_window.geometry=resource_info->image_geometry;
  image_window.width=images[0]->columns;
  if (image_window.width > XDisplayWidth(display,visual_info->screen))
    image_window.width=XDisplayWidth(display,visual_info->screen);
  image_window.height=images[0]->rows;
  if (image_window.height > XDisplayHeight(display,visual_info->screen))
    image_window.height=XDisplayHeight(display,visual_info->screen);
  image_window.border_width=resource_info->border_width;
  backdrop_window=superclass_window;
  if (resource_info->backdrop)
    {
      /*
        Initialize backdrop window.
      */
      backdrop_window.name="ImageMagick Background";
      backdrop_window.flags=USSize | USPosition;
      backdrop_window.width=XDisplayWidth(display,visual_info->screen);
      backdrop_window.height=XDisplayHeight(display,visual_info->screen);
      backdrop_window.border_width=0;
      backdrop_window.immutable=True;
      backdrop_window.attributes.do_not_propagate_mask=
        ButtonPressMask | ButtonReleaseMask;
      manager_hints->flags=IconWindowHint | InputHint | StateHint |
        WindowGroupHint;
      manager_hints->icon_window=icon_window.id;
      manager_hints->input=True;
      manager_hints->initial_state=
        resource_info->iconic ? IconicState : NormalState;
      manager_hints->window_group=superclass_window.id;
      XMakeWindow(display,root_window,argv,argc,class_hint,manager_hints,
        delete_property,&backdrop_window);
      XSetTransientForHint(display,backdrop_window.id,backdrop_window.id);
      XMapWindow(display,backdrop_window.id);
      /*
        Position image in the center the backdrop.
      */
      image_window.flags|=USPosition;
      image_window.x=XDisplayWidth(display,visual_info->screen)/2-
        (image_window.width+image_window.border_width)/2;
      image_window.y=XDisplayHeight(display,visual_info->screen)/2-
        (image_window.height+image_window.border_width)/2;
    }
  image_window.immutable=False;
  image_window.attributes.cursor=image_window.cursor;
  image_window.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | EnterWindowMask | ExposureMask | KeyPressMask |
    LeaveWindowMask | OwnerGrabButtonMask | StructureNotifyMask;
  manager_hints->flags=
    IconWindowHint | InputHint | StateHint | WindowGroupHint;
  manager_hints->icon_window=icon_window.id;
  manager_hints->input=True;
  manager_hints->initial_state=
    resource_info->iconic ? IconicState : NormalState;
  manager_hints->window_group=superclass_window.id;
  XMakeWindow(display,(resource_info->backdrop ? backdrop_window.id :
    root_window),argv,argc,class_hint,manager_hints,delete_property,
    &image_window);
  XMapWindow(display,image_window.id);
  /*
    Create X image.
  */
  image_window.ximage=XMakeImage(display,resource_info,&image_window,images[0],
    images[0]->columns,images[0]->rows);
  if (image_window.ximage == (XImage *) NULL)
    Error("unable to create X image",(char *) NULL);
  /*
    Create X pixmap.
  */
  image_window.pixmaps=(Pixmap *) malloc(number_scenes*sizeof(Pixmap));
  if (image_window.pixmaps == (Pixmap *) NULL)
    Error("unable to animate images","memory allocation failed");
  image_window.pixmaps[0]=XCreatePixmap(display,image_window.id,
    images[0]->columns,images[0]->rows,image_window.depth);
  XPutImage(display,image_window.pixmaps[0],image_window.graphic_context,
    image_window.ximage,0,0,0,0,images[0]->columns,images[0]->rows);
  XDestroyImage(image_window.ximage);
  (void) free((char *) images[0]->pixels);
  images[0]->pixels=(RunlengthPacket *) NULL;
  /*
    Initialize menu window.
  */
  popup_window=superclass_window;
  popup_window.name="ImageMagick Popup";
  popup_window.flags=PSize | PPosition;
  popup_window.attributes.override_redirect=True;
  popup_window.attributes.save_under=True;
  popup_window.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | EnterWindowMask | ExposureMask | LeaveWindowMask |
    OwnerGrabButtonMask;
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=False;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=superclass_window.id;
  XMakeWindow(display,root_window,argv,argc,class_hint,manager_hints,
    delete_property,&popup_window);
  XSetTransientForHint(display,popup_window.id,image_window.id);
  /*
    Initialize info window.
  */
  info_window=superclass_window;
  info_window.name="ImageMagick Info";
  info_window.flags=PSize | PPosition;
  info_window.x=2;
  info_window.y=2;
  info_window.attributes.event_mask=StructureNotifyMask;
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=False;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=superclass_window.id;
  XMakeWindow(display,image_window.id,argv,argc,class_hint,manager_hints,
    delete_property,&info_window);
  /*
    Initialize image pixmaps structure.
  */
  XDefineCursor(display,image_window.id,image_window.busy_cursor);
  XMapWindow(display,info_window.id);
  for (scene=1; scene < number_scenes; scene++)
  {
    /*
      Display information about the image in the info window.
    */
    (void) sprintf(text," [%d] %s %dx%d \0",images[scene]->scene,
      images[scene]->filename,image_window.width,image_window.height);
    if (images[scene]->colors > 0)
      (void) sprintf(text,"%s%dc \0",text,images[scene]->colors);
    XSetWindowExtents((&info_window),text,2);
    XDrawImageString(display,info_window.id,info_window.graphic_context,2,
      info_window.font_info->ascent+2,text,(unsigned int) strlen(text));
    /*
      Initialize colormap.
    */
    if (resource_info->map_type == (char *) NULL)
      map_info[scene]=XMakeStandardColormap(display,visual_info,resource_info,
        &pixel_info[scene],images[scene]);
    else
      {
        /*
          Propogate identical Standard Colormap to each scene.
        */
        map_info[scene]=map_info[0];
        XGetPixelInfo(display,visual_info,map_info[scene],resource_info,
          &pixel_info[scene]);
        if (images[scene]->class == PseudoClass)
          {
            register int
              i;

            /*
              Initialize pixel array for images of type PseudoClass.
            */
            pixel_info[scene].colors=images[scene]->colors;
            pixel_info[scene].pixels=(unsigned long *) malloc((unsigned int)
              images[scene]->colors*sizeof(unsigned long));
            if (pixel_info[scene].pixels == (unsigned long *) NULL)
              Error("unable to create colormap","memory allocation failed");
            for (i=0; i < images[scene]->colors; i++)
              pixel_info[scene].pixels[i]=
                XStandardPixel(map_info[scene],images[scene]->colormap[i],8);
          }
        pixel_info[scene].background_color=pixel_info[0].background_color;
        pixel_info[scene].border_color=pixel_info[0].border_color;
        pixel_info[scene].foreground_color=pixel_info[0].foreground_color;
      }
    /*
      Create X image.
    */
    image_window.map_info=map_info[scene];
    image_window.pixel_info=(&pixel_info[scene]);
    image_window.ximage=XMakeImage(display,resource_info,&image_window,
      images[scene],images[scene]->columns,images[scene]->rows);
    if (image_window.ximage == (XImage *) NULL)
      Error("unable to create X image",(char *) NULL);
    /*
      Create X pixmap.
    */
    image_window.pixmaps[scene]=XCreatePixmap(display,image_window.id,
      images[scene]->columns,images[scene]->rows,image_window.depth);
    XPutImage(display,image_window.pixmaps[scene],image_window.graphic_context,
      image_window.ximage,0,0,0,0,image_window.width,image_window.height);
    XDestroyImage(image_window.ximage);
    (void) free((char *) images[scene]->pixels);
    images[scene]->pixels=(RunlengthPacket *) NULL;
    if (colormap != map_info[scene]->colormap)
      {
        /*
          Set image window colormap.
        */
        colormap=map_info[scene]->colormap;
        XSetWindowColormap(display,image_window.id,colormap);
        XInductColormap(display,colormap);
      }
    /*
      Copy X pixmap to image window.
    */
    XCopyArea(display,image_window.pixmaps[scene],image_window.id,
      image_window.graphic_context,0,0,image_window.width,image_window.height,
      0,0);
    XSync(display,False);
  }
  XWithdrawWindow(display,info_window.id,info_window.screen);
  XDefineCursor(display,image_window.id,image_window.cursor);
  /*
    Respond to events.
  */
  state=DefaultState;
  scene=0;
  do
  {
    if (XEventsQueued(display,QueuedAfterFlush) == 0)
      if ((state & PlayAnimationState) || (state & StepAnimationState))
        {
          if (state & InfoMappedState)
            XWithdrawWindow(display,info_window.id,info_window.screen);
          if (colormap != map_info[scene]->colormap)
            {
              /*
                Set image window colormap.
              */
              colormap=map_info[scene]->colormap;
              XSetWindowColormap(display,image_window.id,colormap);
              XInductColormap(display,colormap);
            }
          /*
            Copy X pixmap to image window.
          */
          XCopyArea(display,image_window.pixmaps[scene],image_window.id,
            image_window.graphic_context,0,0,image_window.width,
            image_window.height,0,0);
          XSync(display,False);
          if (state & StepAnimationState)
            {
              state&=(~StepAnimationState);
              UserCommand(display,&info_window,&image_window,'i',&images[scene],                &state);
            }
          if (resource_info->delay > 0)
            Delay((unsigned long) resource_info->delay);
          if (state & ForwardAnimationState)
            {
              /*
                Forward animation:  increment scene number.
              */
              scene++;
              if (scene == number_scenes)
                if (state & AutoReverseAnimationState)
                  {
                    state&=(~ForwardAnimationState);
                    scene--;
                  }
                else
                  {
                    if (!(state & RepeatAnimationState))
                      state&=(~PlayAnimationState);
                    scene=0;
                  }
            }
          else
            {
              /*
                Reverse animation:  decrement scene number.
              */
              scene--;
              if (scene < 0)
                if (state & AutoReverseAnimationState)
                  {
                    state|=ForwardAnimationState;
                    scene=0;
                  }
                else
                  {
                    if (!(state & RepeatAnimationState))
                      state&=(~PlayAnimationState);
                    scene=number_scenes-1;
                  }
            }
          continue;
        }
    /*
      Handle a window event.
    */
    XNextEvent(display,&event);
    switch (event.type)
    {
      case ButtonPress:
      {
        if (event.xbutton.window == image_window.id)
          switch (event.xbutton.button)
          {
            case Button1:
            {
              static char
                command[1024],
                *MenuCommand="ips.afrq",
                *MenuSelections[]=
                {
                  "Image Info",
                  "Play",
                  "Step",
                  "Repeat",
                  "Auto Reverse",
                  "Forward",
                  "Reverse",
                  "Quit"
                };

              static int
                command_number;

              /*
                Select a command from the pop-up menu.
              */
              command_number=XPopUpMenu(display,&popup_window,
                event.xbutton.x_root,event.xbutton.y_root,"Commands",
                MenuSelections,sizeof(MenuSelections)/sizeof(char *),command);
              if (*command != (char) NULL)
                UserCommand(display,&info_window,&image_window,
                  MenuCommand[command_number],&images[scene],&state);
              break;
            }
            default:
              break;
          }
        break;
      }
      case ClientMessage:
      {
        /*
          If client window delete message, exit.
        */
        if (event.xclient.message_type == protocols_property)
          if (*event.xclient.data.l == delete_property)
            if (event.xclient.window == image_window.id)
              state|=ExitState;
            else
              XWithdrawWindow(display,event.xclient.window,visual_info->screen);
        break;
      }
      case ConfigureNotify:
      {
        if (event.xconfigure.window == image_window.id)
          {
            /*
              Image window has a new configuration.
            */
            image_window.width=event.xconfigure.width;
            image_window.height=event.xconfigure.height;
            break;
          }
        if (event.xconfigure.window == icon_window.id)
          {
            /*
              Icon window has a new configuration.
            */
            icon_window.width=event.xconfigure.width;
            icon_window.height=event.xconfigure.height;
            break;
          }
      }
      case EnterNotify:
      {
        /*
          Selectively install colormap.
        */
        if (colormap != XDefaultColormap(display,visual_info->screen))
          if (event.xcrossing.mode != NotifyUngrab)
            XInductColormap(display,colormap);
        break;
      }
      case Expose:
      {
        /*
          Repaint windows that are now exposed.
        */
        if (event.xexpose.window == image_window.id)
          if (event.xexpose.count == 0)
            {
              XCopyArea(display,image_window.pixmaps[scene],image_window.id,
                image_window.graphic_context,0,0,image_window.width,
                image_window.height,0,0);
              break;
            }
        if (event.xexpose.window == icon_window.id)
          {
            XPutImage(display,icon_window.id,icon_window.graphic_context,
              icon_window.ximage,event.xexpose.x,event.xexpose.y,
              event.xexpose.x,event.xexpose.y,
              (unsigned int) event.xexpose.width,
              (unsigned int) event.xexpose.height);
            break;
          }
        break;
      }
      case KeyPress:
      {
        static char
          command[1024];

        static KeySym
          key_symbol;

        /*
          Respond to a user key press.
        */
        if (state & ConfigureWindowState)
          {
            XBell(display,0);
            break;
          }
        *command=(char) NULL;
        XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        if (key_symbol == XK_Help)
          Usage((char *) NULL,False);
        else
          if (!IsCursorKey(key_symbol) && !isdigit(*command))
            UserCommand(display,&info_window,&image_window,*command,
              &images[scene],&state);
          else
            if (isdigit(*command))
              resource_info->delay=(2 << atoi(command));
        break;
      }
      case LeaveNotify:
      {
        /*
          Selectively uninstall colormap.
        */
        if (colormap != XDefaultColormap(display,visual_info->screen))
          if (event.xcrossing.mode != NotifyUngrab)
            XUninductColormap(display,colormap);
        break;
      }
      case MapNotify:
      {
        if (event.xmap.window == image_window.id)
          {
            state=ForwardAnimationState | PlayAnimationState;
            break;
          }
        if (event.xmap.window == info_window.id)
          {
            state|=InfoMappedState;
            break;
          }
        break;
      }
      case MappingNotify:
      {
        XRefreshKeyboardMapping(&event.xmapping);
        break;
      }
      case UnmapNotify:
      {
        if (event.xunmap.window == info_window.id)
          {
            state&=(~InfoMappedState);
            break;
          }
        break;
      }
      default:
        break;
    }
  }
  while (!(state & ExitState));
  /*
    Free up memory.
  */
  XDestroyWindow(display,info_window.id);
  XDestroyWindow(display,popup_window.id);
  XDestroyWindow(display,image_window.id);
  (void) free((char *) image_window.name);
  if (resource_info->backdrop)
    XDestroyWindow(display,backdrop_window.id);
  XDestroyWindow(display,icon_window.id);
  if (icon_window.ximage != (XImage *) NULL)
    XDestroyImage(icon_window.ximage);
  XDestroyWindow(display,superclass_window.id);
  XFreeGC(display,superclass_window.graphic_context);
  XFreeGC(display,superclass_window.highlight_context);
  XFree((void *) class_hint);
  XFree((void *) manager_hints);
  XFreeCursor(display,arrow_cursor);
  XFreeCursor(display,watch_cursor);
  XFreeFont(display,font_info);
  /*
    Discard pending events before freeing colormap.
  */
  while (XEventsQueued(display,QueuedAfterFlush) > 0)
    XNextEvent(display,&event);
  for (scene=0; scene < number_scenes; scene++)
  {
    XFreePixmap(display,image_window.pixmaps[scene]);
    DestroyImage(images[scene]);
    if (resource_info->map_type == (char *) NULL)
      {
        colormap=map_info[scene]->colormap;
        if (colormap != XDefaultColormap(display,visual_info->screen))
          XFreeColormap(display,colormap);
        else
          if ((visual_info->class == GrayScale) ||
              (visual_info->class == PseudoColor))
            XFreeColors(display,colormap,pixel_info[scene].pixels,
              (int) images[scene]->colors,0);
      }
    if (pixel_info[scene].colors > 0)
      (void) free((char *) pixel_info[scene].pixels);
    XFree((void *) map_info[scene]);
  }
  (void) free((char *) pixel_info);
  (void) free((char *) map_info);
  XFree((void *) visual_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    M a i n                                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/
int main(argc,argv)
int
  argc;

char
  **argv;
{
  char
    *clip_geometry,
    *option,
    *resource_value,
    *scale_geometry,
    *server_name;

  Display
    *display;

  double
    gamma;

  Image
    **images;

  int
    degrees;

  register int
    i;

  unsigned int
    dither,
    gray,
    image_number,
    number_colors,
    reflect,
    tree_depth,
    verbose;

  XResourceInfo
    resource_info;

  XrmDatabase
    resource_database,
    server_database;

  /*
    Display usage profile if there are no command line arguments.
  */
  application_name=(*argv);
  if (argc < 2)
    Usage((char *) NULL,True);
  /*
    Set defaults.
  */
  clip_geometry=(char *) NULL;
  degrees=0;
  display=(Display *) NULL;
  dither=False;
  gamma=0.0;
  gray=False;
  number_colors=0;
  reflect=False;
  scale_geometry=(char *) NULL;
  server_name=(char *) NULL;
  tree_depth=0;
  verbose=False;
  images=(Image **) malloc((unsigned int) argc*sizeof(Image *));
  if (images == (Image **) NULL)
    Error("unable to animate images","memory allocation failed");
  /*
    Check for server name specified on the command line.
  */
  for (i=1; i < argc; i++)
  {
    /*
      Check command line for server name.
    */
    option=argv[i];
    if ((strlen(option) > 1) && ((*option == '-') || (*option == '+')))
      if (strncmp("display",option+1,3) == 0)
        {
          /*
            User specified server name.
          */
          if (*option == '-')
            {
              i++;
              if (i == argc)
                Usage("missing server name on -display",True);
              server_name=argv[i];
              break;
            }
        }
  }
  /*
    Open X server connection.
  */
  display=XOpenDisplay(server_name);
  if (display == (Display *) NULL)
    Error("unable to connect to X server",XDisplayName(server_name));
  /*
    Set our forgiving error handler.
  */
  XSetErrorHandler(XError);
  /*
    Initialize resource database.
  */
  XrmInitialize();
  resource_database=XrmGetDatabase(display);
  resource_value=XResourceManagerString(display);
  if (resource_value == (char *) NULL)
    resource_value="";
  server_database=XrmGetStringDatabase(resource_value);
  XrmMergeDatabases(server_database,&resource_database);
  /*
    Get user defaults from X resource database.
  */
  XGetResourceInfo(resource_database,application_name,&resource_info);
  clip_geometry=XGetResource(resource_database,application_name,"clipGeometry",
    "ClipGeometry",(char *) NULL);
  resource_value=XGetResource(resource_database,application_name,"colors",
    (char *) NULL,"0");
  number_colors=atoi(resource_value);
  resource_value=XGetResource(resource_database,application_name,"dither",
    (char *) NULL,"False");
  dither=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,application_name,"gamma",
    (char *) NULL,"0.0");
  gamma=atof(resource_value);
  resource_value=XGetResource(resource_database,application_name,"gray",
    (char *) NULL,"False");
  gray=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,application_name,"reflect",
    (char *) NULL,"False");
  reflect=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,application_name,"rotate",
    (char *) NULL,"0");
  degrees=atoi(resource_value);
  scale_geometry=XGetResource(resource_database,application_name,
    "scaleGeometry","ScaleGeometry",(char *) NULL);
  resource_value=XGetResource(resource_database,application_name,
    "treeDepth","TreeDepth","0");
  tree_depth=atoi(resource_value);
  resource_value=XGetResource(resource_database,application_name,"verbose",
    (char *) NULL,"False");
  verbose=IsTrue(resource_value);
  /*
    Parse command line.
  */
  image_number=0;
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) > 1) && ((*option == '-') || (*option == '+')))
      switch (*(option+1))
      {
        case 'b':
        {
          if (strncmp("backdrop",option+1,5) == 0)
            resource_info.backdrop=(*option == '-');
          else
            if (strncmp("background",option+1,5) == 0)
              {
                resource_info.background_color=(char *) NULL;
                if (*option == '-')
                  {
                    i++;
                    if (i == argc)
                      Usage("missing color on -background",True);
                    resource_info.background_color=argv[i];
                  }
              }
            else
              if (strncmp("bordercolor",option+1,7) == 0)
                {
                  resource_info.border_color=(char *) NULL;
                  if (*option == '-')
                    {
                      i++;
                      if (i == argc)
                        Usage("missing color on -bordercolor",True);
                      resource_info.border_color=argv[i];
                    }
                }
              else
                if (strncmp("borderwidth",option+1,7) == 0)
                  {
                    resource_info.border_width=0;
                    if (*option == '-')
                      {
                        i++;
                        if (i == argc)
                          Usage("missing width on -borderwidth",True);
                        resource_info.border_width=atoi(argv[i]);
                      }
                  }
                else
                  Usage(option,True);
          break;
        }
        case 'c':
        {
          if (strncmp("clip",option+1,2) == 0)
            {
              clip_geometry=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Usage("missing geometry on -clip",True);
                  clip_geometry=argv[i];
                }
            }
          else
            if (strncmp("colors",option+1,3) == 0)
              {
                number_colors=0;
                if (*option == '-')
                  {
                    i++;
                    if (i == argc)
                      Usage("missing colors on -colors",True);
                    number_colors=atoi(argv[i]);
                  }
              }
            else
              Usage(option,True);
          break;
        }
        case 'd':
        {
          if (strncmp("delay",option+1,2) == 0)
            {
              resource_info.delay=0;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Usage("missing seconds on -delay",True);
                  resource_info.delay=atoi(argv[i]);
                }
            }
          else
            if (strncmp("display",option+1,3) == 0)
              {
                server_name=(char *) NULL;
                if (*option == '-')
                  {
                    i++;
                    if (i == argc)
                      Usage("missing server name on -display",True);
                    server_name=argv[i];
                  }
              }
            else
              if (strncmp("dither",option+1,3) == 0)
                dither=(*option == '-');
              else
                Usage(option,True);
          break;
        }
        case 'f':
        {
          if (strncmp("font",option+1,3) == 0)
            {
              resource_info.font=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Usage("missing font name on -font",True);
                  resource_info.font=argv[i];
                }
            }
          else
            if (strncmp("foreground",option+1,3) == 0)
              {
                resource_info.foreground_color=(char *) NULL;
                if (*option == '-')
                  {
                    i++;
                    if (i == argc)
                      Usage("missing foreground on -foreground",True);
                    resource_info.foreground_color=argv[i];
                  }
              }
            else
              Usage(option,True);
          break;
        }
        case 'g':
        {
          if (strncmp("gamma",option+1,2) == 0)
            {
              gamma=0.0;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Usage("missing gamma on -gamma",True);
                  gamma=atof(argv[i]);
                }
            }
          else
            if (strncmp("geometry",option+1,2) == 0)
              {
                resource_info.image_geometry=(char *) NULL;
                if (*option == '-')
                  {
                    i++;
                    if (i == argc)
                      Usage("missing geometry on -geometry",True);
                    resource_info.image_geometry=argv[i];
                  }
              }
            else
              if (strncmp("gray",option+1,2) == 0)
                gray=(*option == '-');
              else
                Usage(option,True);
          break;
        }
        case 'h':
        {
          Usage((char *) NULL,True);
          break;
        }
        case 'i':
        {
          if (strncmp("iconGeometry",option+1,5) == 0)
            {
              resource_info.icon_geometry=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Usage("missing geometry on -iconGeometry",True);
                  resource_info.icon_geometry=argv[i];
                }
            }
          else
            if (strncmp("iconic",option+1,5) == 0)
              resource_info.iconic=(*option == '-');
            else
              Usage(option,True);
          break;
        }
        case 'm':
        {
          if (strncmp("map",option+1,3) == 0)
            {
              resource_info.map_type=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Usage("missing map type on -map",True);
                  resource_info.map_type=argv[i];
                }
            }
          else
            if (strncmp("monochrome",option+1,2) == 0)
              resource_info.monochrome=(*option == '-');
            else
              Usage(option,True);
          break;
        }
        case 'n':
        {
          if (strncmp("name",option+1,2) == 0)
            {
              resource_info.name=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Usage("missing name on -name",True);
                  resource_info.name=argv[i];
                }
            }
          else
            Usage(option,True);
          break;
        }
        case 'r':
        {
          if (strncmp("reflect",option+1,2) == 0)
            reflect=(*option == '-');
          else
            if (strncmp("rotate",option+1,3) == 0)
              {
                degrees=0;
                if (*option == '-')
                  {
                    i++;
                    if (i == argc)
                      Usage("missing degrees on -rotate",True);
                    degrees=atoi(argv[i]);
                  }
              }
            else
              Usage(option,True);
          break;
        }
        case 's':
        {
          scale_geometry=(char *) NULL;
          if (*option == '-')
            {
              i++;
              if (i == argc)
                Usage("missing scale geometry on -scale",True);
              scale_geometry=argv[i];
            }
          break;
        }
        case 't':
        {
          if (strncmp("title",option+1,2) == 0)
            {
              resource_info.title=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Usage("missing title on -title",True);
                  resource_info.title=argv[i];
                }
            }
          else
            if (strncmp("treedepth",option+1,2) == 0)
              {
                tree_depth=0;
                if (*option == '-')
                  {
                    i++;
                    if (i == argc)
                      Usage("missing depth on -treedepth",True);
                    tree_depth=atoi(argv[i]);
                  }
              }
            else
              Usage(option,True);
          break;
        }
        case 'v':
        {
          if (strncmp("verbose",option+1,2) == 0)
            verbose=(*option == '-');
          else
            if (strncmp("visual",option+1,2) == 0)
              {
                resource_info.visual_type=(char *) NULL;
                if (*option == '-')
                  {
                    i++;
                    if (i == argc)
                      Usage("missing visual class on -visual",True);
                    resource_info.visual_type=argv[i];
                  }
              }
            else
              Usage(option,True);
          break;
        }
        default:
        {
          Usage((char *) NULL,True);
          break;
        }
      }
    else
      {
        Image
          *image,
          info_image;

        /*
          Option is a file name: begin by reading image from specified file.
        */
        image=ReadImage(option);
        if (image == (Image *) NULL)
          continue;
        if (image->scene == 0)
          image->scene=image_number;
        info_image=(*image);
        /*
          Transform image as defined by the clip, image and scale geometries.
        */
        image=TransformImage(image,clip_geometry,resource_info.image_geometry,
          scale_geometry);
        if (reflect)
          {
            Image
              *reflected_image;

            /*
              Reverse image scanlines.
            */
            reflected_image=ReflectImage(image);
            if (reflected_image != (Image *) NULL)
              {
                DestroyImage(image);
                image=reflected_image;
              }
          }
        if ((degrees % 360) != 0)
          {
            Image
              *rotated_image;

            /*
              Rotate image.
            */
            rotated_image=RotateImage(image,(double) degrees,False);
            if (rotated_image != (Image *) NULL)
              {
                DestroyImage(image);
                image=rotated_image;
              }
          }
        if (gamma > 0.0)
          (void) GammaImage(image,gamma);
        if (gray)
          {
            /*
              Convert image to gray scale PseudoColor class.
            */
            (void) GrayImage(image);
            if (image->class == DirectClass)
              QuantizeImage(image,256,tree_depth,dither,True);
          }
        if (resource_info.monochrome)
          {
            register unsigned int
              bit;

            /*
              Convert image to resource_info.monochrome PseudoColor class.
            */
            (void) GrayImage(image);
            (void) QuantizeImage(image,2,tree_depth,dither,True);
            bit=Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
            image->colormap[bit].red=0;
            image->colormap[bit].green=0;
            image->colormap[bit].blue=0;
            image->colormap[!bit].red=255;
            image->colormap[!bit].green=255;
            image->colormap[!bit].blue=255;
          }
        if (number_colors > 0)
          if ((image->class == DirectClass) || (image->colors > number_colors))
            QuantizeImage(image,number_colors,tree_depth,dither,True);
        if (verbose)
          {
            /*
              Display detailed info about the image.
            */
            (void) fprintf(stderr,"[%d] %s",(image->scene == 0 ? image_number :
              image->scene),image->filename);
            (void) fprintf(stderr," %dx%d",info_image.columns,info_image.rows);
            if ((info_image.columns != image->columns) ||
                (info_image.rows != image->rows))
              (void) fprintf(stderr,"=>%dx%d",image->columns,image->rows);
            if (image->class == DirectClass)
              (void) fprintf(stderr," DirectClass ");
            else
              (void) fprintf(stderr," PseudoClass %dc",image->colors);
            (void) fprintf(stderr,"                                        \r");
          }
        images[image_number++]=image;
      }
    }
  if (verbose)
    (void) fprintf(stderr,"\n");
  if (image_number == 0)
    Usage("missing an image file name",True);
  else
    XAnimateImage(display,&resource_info,argv,argc,images,image_number);
  (void) free((char *) images);
  if (resource_database != (XrmDatabase) NULL)
    XrmDestroyDatabase(resource_database);
  XCloseDisplay(display);
  return(False);
}
