/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%             DDDD   IIIII  SSSSS  PPPP   L       AAA   Y   Y                 %
%             D   D    I    SS     P   P  L      A   A   Y Y                  %
%             D   D    I     SSS   PPPP   L      AAAAA    Y                   %
%             D   D    I       SS  P      L      A   A    Y                   %
%             DDDD   IIIII  SSSSS  P      LLLLL  A   A    Y                   %
%                                                                             %
%                                                                             %
%          Display Machine Independent File Format Image via X11.             %
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
%  Display is a machine architecture independent image processing
%  and display program.  It can display any image in the MIFF format on
%  any workstation display running X.  Display first determines the
%  hardware capabilities of the workstation.  If the number of unique
%  colors in the image is less than or equal to the number the workstation
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
%  The Display program command syntax is:
%
%  Usage: display [options ...] file [ [options ...] file ...]
%
%  Where options include:
%    -backdrop           display image centered on a backdrop
%    -clip geometry      preferred size and location of the clipped image
%    -colors value       preferred number of colors in the image
%    -compress type      compress image: RunlengthEncoded or QEncoded
%    -delay seconds      display the next image after pausing
%    -display server     display image to this X server
%    -dither             apply Floyd/Steinberg error diffusion to image
%    -enhance            apply a digital filter to enhance a noisy image
%    -gamma value        level of gamma correction
%    -geometry geometry  preferred size and location of the image window
%    -gray               transform image to gray scale colors
%    -inverse            apply color inversion to image
%    -magnify value      level of image magnification
%    -map type           display image using this Standard Colormap
%    -monochrome         transform image to black and white
%    -noise              reduce noise with a noise peak elimination filter
%    -normalize          tranform image to span the full the range of colors
%    -print file         write image as Postscript to a file
%    -reflect            reverse image scanlines
%    -rotate degrees     apply Paeth rotation to the image
%    -scale geometry     preferred size factors of the image
%    -scene number       image scene number
%    -treedepth value    depth of the color classification tree
%    -verbose            print detailed information about the image
%    -visual type        display image using this visual type
%    -window id          display image to background of this window
%    -write file         write image to a file
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
%    Control-1
%         press and drag to pan the image
%    1    press and drag to select a command from a pop-up menu
%    2    press and drag to define a region of the image to clip
%    3    press and drag to define a region of the image to magnify
%
%  Keys
%    i    press to display information about the image
%    w    press to write the image to a file
%    p    press to print the image to a file
%    a    press to annotate the image with text
%    r    press to reflect the image scanlines
%    /    press to rotate the image 90 degrees clockwise
%    \    press to rotate the image 90 degrees counter-clockwise
%    <    press to half the image size
%    >    press to double the image size
%    o    press to restore the image to its original size
%    n    press to display the next image
%    q    press to discard all images and exit program
%    1-9  press to change the level of magnification
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
#define ConstrainMagnifyFactor(image_window,magnify_window,magnify)  \
{  \
  while ((magnify*image_window->ximage->width) < magnify_window->width)  \
    magnify<<=1;  \
  while ((magnify*image_window->ximage->height) < magnify_window->height)  \
    magnify<<=1;  \
  if (magnify > magnify_window->width)  \
    magnify=magnify_window->width;  \
  if (magnify > magnify_window->height)  \
    magnify=magnify_window->height;  \
}
#define ConfigureWindowState  0x0001
#define ControlState  0x0002
#define DefaultState  0x0004
#define ExitState  0x0008
#define ImageMappedState  0x0010
#define InfoMappedState  0x0020
#define MagnifyState  0x0040
#define MagnifyMappedState  0x0080

/*
  Forward declarations.
*/
static Cursor
  XMakeCursor();

static unsigned int
  XAnnotateImageWindow(),
  XConfigureImageWindow(),
  XReflectImageWindow(),
  XRotateImageWindow();

static void
  XDisplayImageWindow(),
  XMagnifyImageWindow(),
  XMakeMagnifyImage(),
  XPanImageWindow();

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
%    o terminate: A value other than zero is returned if the program is to
%      terminate immediately.
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
      "Control-1",
      "     press and drag to pan the image",
      "1    press and drag to select a command from a pop-up menu",
      "2    press and drag to define a region of the image to clip",
      "3    press and drag to define a region of the image to magnify",
      (char *) NULL
    },
    *keys[]=
    {
      "i    press to display information about the image",
      "w    press to write the image to a file",
      "p    press to print the image to a file",
      "a    press to annotate the image with text",
      "r    press to reflect the image scanlines",
      "/    press to rotate the image 90 degrees clockwise",
      "\\    press to rotate the image 90 degrees counter-clockwise",
      "<    press to half the image size",
      ">    press to double the image size",
      "o    press to restore the image to its original size",
      "n    press to display the next image",
      "q    press to discard all images and exit program",
      "1-9  press to change the level of magnification",
      (char *) NULL
    },
    *options[]=
    {
      "-backdrop           display image centered on a backdrop",
      "-clip geometry      preferred size and location of the clipped image",
      "-colors value       preferred number of colors in the image",
      "-compress type      compress image: RunlengthEncoded or QEncoded",
      "-delay seconds      display the next image after pausing",
      "-display server     display image to this X server",
      "-dither             apply Floyd/Steinberg error diffusion to image",
      "-enhance            apply a digital filter to enhance a noisy image",
      "-gamma value        level of gamma correction",
      "-geometry geometry  preferred size and location of the image window",
      "-gray               transform image to gray scale colors",
      "-inverse            apply color inversion to image",
      "-magnify value      level of image magnification",
      "-map type           display image using this Standard Colormap",
      "-monochrome         transform image to black and white",
      "-noise              reduce noise with a noise peak elimination filter",
      "-normalize          tranform image to span the full range of colors",
      "-print file         write image as Postscript to a file",
      "-reflect            reflect the image scanlines",
      "-rotate degrees     apply Paeth rotation to the image",
      "-scale geometry     preferred size factors of the image",
      "-scene number       image scene number",
      "-treedepth value    depth of the color classification tree",
      "-verbose            print detailed information about the image",
      "-visual type        display image using this visual type",
      "-window id          display image to background of this window",
      "-write file         write image to a file",
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
%    UserCommand(display,resource_info,info_window,image_window,command,image,
%      timeout,state);
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o info_window: Specifies a pointer to a XWindowInfo structure.
%
%    o popup_window: Specifies a pointer to a XWindowInfo structure.
%
%    o image_window: Specifies a pointer to a XWindowInfo structure.
%
%    o command: Specifies a command to perform.
%
%    o image: Specifies a pointer to a Image structure;  UserCommand
%      may transform the image and return a new image pointer.
%
%    o timeout: Specifies an unsigned long;  UserCommand may modify this
%      value.
%
%    o state: Specifies an unsigned int;  UserCommand may return a
%      modified state.
%
%
*/
static void UserCommand(display,resource_info,info_window,popup_window,
  image_window,command,image,timeout,state)
Display
  *display;

XResourceInfo
  *resource_info;

XWindowInfo
  *info_window,
  *popup_window,
  *image_window;

char
  command;

Image
  **image;

unsigned long
  *timeout;

unsigned int
  *state;
{
  char
    text[1024];

  unsigned int
    status;

  XWindowInfo
    window;

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
    case '<':
    case '>':
    {
      /*
        Half or double the image size.
      */
      window=(*image_window);
      if (command == '>')
        {
          window.width=image_window->ximage->width << 1;
          window.height=image_window->ximage->height << 1;
        }
      else
        {
          window.width=image_window->ximage->width >> 1;
          window.height=image_window->ximage->height >> 1;
        }
      if ((window.width == 0) || (window.height == 0))
        break;
      *state|=ConfigureWindowState;
      break;
    }
    case '/':
    case '\\':
    {
      /*
        Rotate image 90 degrees to clockwise or counter-clockwise.
      */
      status=XRotateImageWindow(display,info_window,image_window,
        (unsigned int) (command == '/' ? 90 : 270),image);
      if (status == False)
        {
          XPopUpAlert(display,popup_window,"unable to rotate X image",
            image_window->name);
          break;
        }
      window=(*image_window);
      window.width=image_window->ximage->height;
      window.height=image_window->ximage->width;
      *state|=ConfigureWindowState;
      break;
    }
    case 'a':
    {
      /*
        Annotate the image with text.
      */
      status=XAnnotateImageWindow(display,resource_info,info_window,
        popup_window,image_window,*image);
      if (status == False)
        {
          XPopUpAlert(display,popup_window,"unable to annotate X image",
            image_window->name);
          break;
        }
      window=(*image_window);
      window.width=image_window->ximage->width;
      window.height=image_window->ximage->height;
      *state|=ConfigureWindowState;
      break;
    }
    case 'i':
    {
      /*
        Display information about the image in the info window.
      */
      (void) sprintf(text," [%d] %s %dx%d %s \0",(*image)->scene,
        (*image)->filename,image_window->ximage->width,
        image_window->ximage->height,
        XVisualClassName(info_window->visual_info));
      if ((*image)->colors > 0)
        (void) sprintf(text,"%s%dc \0",text,(*image)->colors);
      XSetWindowExtents(info_window,text,2);
      XMapWindow(display,info_window->id);
      XDrawImageString(display,info_window->id,info_window->graphic_context,2,
        info_window->font_info->ascent+2,text,strlen(text));
      break;
    }
    case 'n':
    {
      /*
        Display next image.
      */
      *timeout=0;
      break;
    }
    case 'o':
    {
      /*
        Restore image window to its original size.
      */
      if (image_window->clip_geometry != (char *) NULL)
        {
          unsigned int
            clip_height,
            clip_width;

          /*
            Retain the image clipping offsets;  discard geometry.
          */
          (void) XParseGeometry(image_window->clip_geometry,&image_window->x,
            &image_window->y,&clip_width,&clip_height);
          image_window->x=(-image_window->x);
          image_window->y=(-image_window->y);
          (void) free((char *) image_window->clip_geometry);
          image_window->clip_geometry=(char *) NULL;
        }
      window=(*image_window);
      window.width=(*image)->columns;
      window.height=(*image)->rows;
      *state|=ConfigureWindowState;
      break;
    }
    case 'p':
    case 'w':
    {
      char
        filename[1024];

      Image
        *output_image;

      /*
        Write or print image to file.
      */
      (void) strcpy(filename,(*image)->filename);
      if (command == 'p')
        {
          (void) strcpy(text," Printing image... ");
          if (resource_info->print_filename != (char *) NULL)
            (void) strcpy(filename,resource_info->print_filename);
        }
      else
        {
          (void) strcpy(text," Writing image... ");
          if (resource_info->write_filename != (char *) NULL)
            (void) strcpy(filename,resource_info->write_filename);
        }
      (void) XPopUpQuery(display,popup_window,"File name:",filename);
      if (*filename == (char) NULL)
        break;
      /*
        Alert user we are busy.
      */
      XDefineCursor(display,image_window->id,image_window->busy_cursor);
      XSetWindowExtents(info_window,text,2);
      XMapWindow(display,info_window->id);
      XDrawImageString(display,info_window->id,info_window->graphic_context,2,
        info_window->font_info->ascent+2,text,strlen(text));
      XFlush(display);
      /*
        Copy image before applying image transforms.
      */
      output_image=CopyImage(*image,(*image)->columns,(*image)->rows,True);
      if (output_image == (Image *) NULL)
        {
          XPopUpAlert(display,popup_window,"unable to output image",
            image_window->name);
          break;
        }
      if ((image_window->clip_geometry != (char *) NULL) ||
         (output_image->columns != image_window->ximage->width) ||
         (output_image->rows != image_window->ximage->height))
        {
          char
            image_geometry[1024];

          /*
            Clip and/or scale image.
          */
          (void) sprintf(image_geometry,"%dx%d\0",image_window->ximage->width,
            image_window->ximage->height);
          output_image=TransformImage(output_image,image_window->clip_geometry,
            image_geometry,(char *) NULL);
        }
      if (resource_info->number_colors > 0)
        if ((output_image->class == DirectClass) ||
            (output_image->colors > resource_info->number_colors))
          QuantizeImage(output_image,resource_info->number_colors,
            resource_info->tree_depth,resource_info->dither,True);
      (void) strcpy(output_image->filename,filename);
      if (command == 'p')
        PrintImage(output_image,resource_info->image_geometry);
      else
        WriteImage(output_image);
      DestroyImage(output_image);
      XDefineCursor(display,image_window->id,image_window->cursor);
      XWithdrawWindow(display,info_window->id,info_window->screen);
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
      /*
        Reflect image scanlines.
      */
      status=XReflectImageWindow(display,info_window,image_window,image);
      if (status == False)
        {
          XPopUpAlert(display,popup_window,"unable to reflect X image",
            image_window->name);
          break;
        }
      window=(*image_window);
      window.width=image_window->ximage->width;
      window.height=image_window->ximage->height;
      *state|=ConfigureWindowState;
      break;
    }
    default:
    {
      XBell(display,0);
      break;
    }
  }
  if (*state & ConfigureWindowState)
    {
      XWindowChanges
        window_changes;

      /*
        Update image window with new image.
      */
      status=XConfigureImageWindow(display,resource_info,info_window,&window,
        *image);
      if (status == False)
        XPopUpAlert(display,popup_window,"unable to configure X image",
          image_window->name);
      else
        image_window->ximage=window.ximage;
      window_changes.width=image_window->ximage->width;
      if (window_changes.width > XDisplayWidth(display,image_window->screen))
        window_changes.width=XDisplayWidth(display,image_window->screen);
      window_changes.height=image_window->ximage->height;
      if (window_changes.height > XDisplayHeight(display,image_window->screen))
        window_changes.height=XDisplayHeight(display,image_window->screen);
      if ((image_window->width != window_changes.width) ||
          (image_window->height != window_changes.height))
        XReconfigureWMWindow(display,image_window->id,image_window->screen,
          CWWidth | CWHeight,&window_changes);
      else
        {
          /*
            Image configuration has not changed-- just display the image.
          */
          XDisplayImageWindow(display,image_window,0,0,image_window->width,
            image_window->height);
          *state&=(~ConfigureWindowState);
        }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X A n n o t a t e I m a g e W i n d o w                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XAnnotateImageWindow annotates the image with text.
%
%  The format of the XAnnotateImageWindow routine is:
%
%    XAnnotateImageWindow(display,resource_info,info_window,popup_window,
%      image_window,image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o info_window: Specifies a pointer to a XWindowInfo structure.
%
%    o popup_window: Specifies a pointer to a XWindowInfo structure.
%
%    o image_window: Specifies a pointer to a XWindowInfo structure.
%
%    o image: Specifies a pointer to a Image structure; returned from
%      ReadImage.
%
*/
static unsigned int XAnnotateImageWindow(display,resource_info,info_window,
  popup_window,image_window,image)
Display
  *display;

XResourceInfo
  *resource_info;

XWindowInfo
  *info_window,
  *popup_window,
  *image_window;

Image
  *image;
{
  typedef struct _AnnotateInfo
  {
    int
      x,
      y;

    unsigned int
      width,
      height;

    char
      *text;

    struct _AnnotateInfo
      *previous,
      *next;
  } AnnotateInfo;

  AnnotateInfo
    *annotate;

  char
    annotate_geometry[1024],
    *p,
    text[1024];

  Cursor
    cursor;

  GC
    graphic_context;

  int
    i,
    x,
    y;

  static unsigned int
    font_id=0,
    pen_id=0;

  unsigned short
    index;

  unsigned int
    height,
    mask,
    state,
    status,
    width;

  unsigned long
    pixel,
    scale_factor;

  Window
    window;

  XColor
    pen_color;

  XFontStruct
    *font_info;

  XEvent
    event;

  XGCValues
    graphic_context_value;

  /*
    Map info window.
  */
  state=DefaultState;
  (void) sprintf(text," %+d%+d  \0",image_window->width,image_window->height);
  XSetWindowExtents(info_window,text,2);
  XMapWindow(display,info_window->id);
  state|=InfoMappedState;
  /*
    Track pointer until button 1 is pressed.
  */
  cursor=XCreateFontCursor(display,XC_pencil);
  XDefineCursor(display,image_window->id,cursor);
  XQueryPointer(display,image_window->id,&window,&window,&i,&i,&x,&y,&mask);
  XSelectInput(display,image_window->id,image_window->attributes.event_mask |
    PointerMotionMask);
  do
  {
    if (state & InfoMappedState)
      {
        /*
          Display pointer position.
        */
        (void) sprintf(text," %+d%+d \0",x-image_window->x,y-image_window->y);
        XClearWindow(display,info_window->id);
        XDrawImageString(display,info_window->id,info_window->graphic_context,2,
          info_window->font_info->ascent+2,text,strlen(text));
      }
    /*
      Wait for next event.
    */
    XWindowEvent(display,image_window->id,ButtonPressMask | ButtonReleaseMask |
      ExposureMask | KeyPressMask | PointerMotionMask,&event);
    switch (event.type)
    {
      case ButtonPress:
      {
        switch (event.xbutton.button)
        {
          case Button1:
          {
            /*
              Change to text entering mode.
            */
            x=event.xbutton.x;
            y=event.xbutton.y;
            state|=ExitState;
            break;
          }
          case Button2:
          {
            char
              font_name[1024],
              *MenuSelections[9];

            int
              font_number;

            /*
              Initialize menu selections.
            */
            for (i=0; i < 9; i++)
              MenuSelections[i]=resource_info->font_name[i];
            /*
              Select a font name from the pop-up menu.
            */
            font_number=XPopUpMenu(display,popup_window,event.xbutton.x_root,
              event.xbutton.y_root,"Pen Fonts",MenuSelections,9,font_name);
            if (*font_name != (char) NULL)
              {
                /*
                  Initialize font info.
                */
                font_info=XLoadQueryFont(display,font_name);
                if (font_info == (XFontStruct *) NULL)
                  XPopUpAlert(display,popup_window,"unable to load font",
                    font_name);
                else
                  {
                    font_id=font_number;
                    XFreeFont(display,font_info);
                  }
              }
            break;
          }
          case Button3:
          {
            char
              color_name[1024],
              *MenuSelections[9];

            int
              pen_number;

            /*
              Initialize menu selections.
            */
            for (i=0; i < 9; i++)
              MenuSelections[i]=resource_info->pen_color[i];
            /*
              Select a font color from the pop-up menu.
            */
            pen_number=XPopUpMenu(display,popup_window,event.xbutton.x_root,
              event.xbutton.y_root,"Pen Colors",MenuSelections,9,color_name);
            if (*color_name != (char) NULL)
              pen_id=pen_number;
            break;
          }
        }
      }
      case Expose:
      {
        /*
          Repaint image window.
        */
        if (event.xexpose.count == 0)
          XDisplayImageWindow(display,image_window,0,0,image_window->width,
            image_window->height);
        break;
      }
      case MotionNotify:
      {
        /*
          Discard pending pointer motion events.
        */
        while (XCheckMaskEvent(display,PointerMotionMask,&event));
        x=event.xmotion.x;
        y=event.xmotion.y;
        /*
          Map and unmap info window as text cursor crosses its boundaries.
        */
        if (state & InfoMappedState)
          {
            if ((x < (info_window->x+info_window->width)) &&
                (y < (info_window->y+info_window->height)))
              {
                XWithdrawWindow(display,info_window->id,info_window->screen);
                state&=(~InfoMappedState);
              }
          }
        else
          if ((x > (info_window->x+info_window->width)) ||
              (y > (info_window->y+info_window->height)))
            {
              XMapWindow(display,info_window->id);
              state|=InfoMappedState;
            }
        break;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  XSelectInput(display,image_window->id,image_window->attributes.event_mask);
  XDefineCursor(display,image_window->id,image_window->cursor);
  XFreeCursor(display,cursor);
  if (state & InfoMappedState)
    XWithdrawWindow(display,info_window->id,info_window->screen);
  XFlush(display);
  /*
    Set font info and pen color.
  */
  font_info=XLoadQueryFont(display,resource_info->font_name[font_id]);
  if (font_info == (XFontStruct *) NULL)
    {
      XPopUpAlert(display,popup_window,"unable to load font",
        resource_info->font_name[font_id]);
      font_info=image_window->font_info;
    }
  pen_color=image_window->pixel_info->pen_color[pen_id];
  /*
    Initialize graphic context.
  */
  pen_color=image_window->pixel_info->pen_color[pen_id];
  graphic_context_value.background=
    image_window->pixel_info->background_color.pixel;
  graphic_context_value.foreground=pen_color.pixel;
  graphic_context_value.font=font_info->fid;
  graphic_context_value.function=GXcopy;
  graphic_context_value.line_width=WindowBorderWidth;
  graphic_context_value.plane_mask=AllPlanes;
  graphic_context_value.subwindow_mode=IncludeInferiors;
  graphic_context=XCreateGC(display,image_window->id,GCBackground |
    GCFont | GCForeground | GCFunction | GCLineWidth | GCPlaneMask | 
    GCSubwindowMode,&graphic_context_value);
  if (graphic_context == (GC) NULL)
    return(False);
  /*
    Check boundary conditions.
  */
  if ((x+font_info->max_bounds.width) >= image_window->width)
    x=image_window->width-font_info->max_bounds.width;
  if (y < (font_info->ascent+font_info->descent))
    y=font_info->ascent+font_info->descent;
  if ((font_info->max_bounds.width > image_window->width) ||
      ((font_info->ascent+font_info->descent) >= image_window->height))
    return(False);
  /*
    Initialize annotate structure.
  */
  annotate=(AnnotateInfo *) malloc(sizeof(AnnotateInfo));
  if (annotate == (AnnotateInfo *) NULL)
    return(False);
  annotate->text=(char *)
    malloc(image_window->width/Max(font_info->min_bounds.width,1)+2);
  if (annotate->text == (char *) NULL)
    return(False);
  annotate->x=x;
  annotate->y=y;
  annotate->width=0;
  annotate->height=font_info->ascent+font_info->descent;
  annotate->previous=(AnnotateInfo *) NULL;
  annotate->next=(AnnotateInfo *) NULL;
  /*
    Begin annotating the image with text.
  */
  state=DefaultState;
  XDrawString(display,image_window->id,graphic_context,x,y,"_",1);
  p=annotate->text;
  *p=(char) NULL;
  do
  {
    /*
      Wait for next event.
    */
    XWindowEvent(display,image_window->id,ExposureMask | KeyPressMask,&event);
    switch (event.type)
    {
      case Expose:
      {
        if (event.xexpose.count == 0)
          {
            /*
              Repaint image window.
            */
            XDisplayImageWindow(display,image_window,0,0,image_window->width,
              image_window->height);
            XDrawString(display,image_window->id,graphic_context,annotate->x,
              annotate->y,annotate->text,strlen(annotate->text));
            XDrawString(display,image_window->id,graphic_context,x,y,"_",1);
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
          Erase text cursor.
        */
        XDisplayImageWindow(display,image_window,x,y-font_info->ascent,
          (unsigned int) font_info->max_bounds.width,annotate->height);
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
            if (p == annotate->text)
              if (annotate->previous == (AnnotateInfo *) NULL)
                break;
              else
                {
                  /*
                    Go to end of the previous line of text.
                  */
                  annotate=annotate->previous;
                  p=annotate->text;
                  x=annotate->x+annotate->width;
                  y=annotate->y;
                  if (annotate->width > 0)
                    p+=strlen(annotate->text);
                  break;
                }
            p--;
            x-=XTextWidth(font_info,p,1);
            XDisplayImageWindow(display,image_window,x,y-font_info->ascent,
              (unsigned int) font_info->max_bounds.width,annotate->height);
            break;
          }
          case XK_Delete:
          {
            /*
              Erase the entire line of text.
            */
            while (p != annotate->text)
            {
              p--;
              x-=XTextWidth(font_info,p,1);
              XDisplayImageWindow(display,image_window,x,y-font_info->ascent,
                (unsigned int) font_info->max_bounds.width,annotate->height);
            }
            break;
          }
          case XK_Escape:
          {
            /*
              Finished annotating.
            */
            annotate->width=
              XTextWidth(font_info,annotate->text,strlen(annotate->text));
            state|=ExitState;
            break;
          }
          default:
          {
            /*
              Draw a single character on the image window.
            */
            if (*command == (char) NULL)
              break;
            *p=(*command);
            XDrawString(display,image_window->id,graphic_context,x,y,p,1);
            x+=XTextWidth(font_info,p,1);
            p++;
            if ((x+font_info->max_bounds.width) < image_window->width)
              break;
          }
          case XK_Return:
          {
            /*
              Advance to the next line of text.
            */
            *p=(char) NULL;
            annotate->width=
              XTextWidth(font_info,annotate->text,strlen(annotate->text));
            if (annotate->next != (AnnotateInfo *) NULL)
              {
                /*
                  Line of text already exists.
                */
                annotate=annotate->next;
                x=annotate->x;
                y=annotate->y;
                p=annotate->text;
                break;
              }
            annotate->next=(AnnotateInfo *) malloc(sizeof(AnnotateInfo));
            if (annotate->next == (AnnotateInfo *) NULL)
              return(False);
            *annotate->next=(*annotate);
            annotate->next->previous=annotate;
            annotate=annotate->next;
            annotate->text=(char *)
              malloc(image_window->width/Max(font_info->min_bounds.width,1)+2);
            if (annotate->text == (char *) NULL)
              return(False);
            annotate->y+=annotate->height;
            if (annotate->y > image_window->height)
              annotate->y=annotate->height;
            annotate->next=(AnnotateInfo *) NULL;
            x=annotate->x;
            y=annotate->y;
            p=annotate->text;
            break;
          }
        }
        /*
          Display text cursor.
        */
        *p=(char) NULL;
        XDrawString(display,image_window->id,graphic_context,x,y,"_",1);
        break;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  /*
    Erase text cursor.
  */
  XDisplayImageWindow(display,image_window,x,y-font_info->max_bounds.ascent,
    (unsigned int) font_info->max_bounds.width,annotate->height);
  /*
    Annotation is relative to image configuration.
  */
  x=0;
  y=0;
  width=image->columns;
  height=image->rows;
  if (image_window->clip_geometry != (char *) NULL)
    (void) XParseGeometry(image_window->clip_geometry,&x,&y,&width,&height);
  /*
    Initialize annotated image.
  */
  XDefineCursor(display,image_window->id,image_window->busy_cursor);
  XFlush(display);
  while (annotate != (AnnotateInfo *) NULL)
  {
    if (annotate->width == 0)
      {
        /*
           No text on this line--  go to the next line of text.
        */
        annotate=annotate->previous;
        if (annotate != (AnnotateInfo *) NULL)
          (void) free((char *) annotate->next);
        continue;
      }
    /*
      Determine foreground pixel index for font color.
    */
    index=0;
    if (resource_info->monochrome)
      index=Intensity(pen_color) >= 128;
    else
      if (image_window->pixel_info->colors > 0)
        {
          for (index=0; index < image_window->pixel_info->colors; index++)
          {
            pixel=image_window->pixel_info->pixels[index];
            if (pixel == pen_color.pixel)
              break;
          }
          if (pixel != pen_color.pixel)
            {
              image_window->pixel_info->pixels[index]=pen_color.pixel;
              image_window->pixel_info->colors++;
            }
        }
    image_window->pixel_info->annotate_index=index;
    image_window->pixel_info->annotate_color=pen_color;
    /*
      Define the annotate geometry string.
    */
    scale_factor=(width << 14)/image_window->ximage->width;
    annotate->x-=image_window->x;
    annotate->x=(annotate->x*scale_factor+8191) >> 14;
    annotate->width=(annotate->width*scale_factor+8191) >> 14;
    scale_factor=(height << 14)/image_window->ximage->height;
    annotate->y-=(image_window->y+font_info->ascent);
    annotate->y=(annotate->y*scale_factor+8191) >> 14;
    annotate->height=(annotate->height*scale_factor+8191) >> 14;
    (void) sprintf(annotate_geometry,"%dx%d%+d%+d\0",annotate->width,
      annotate->height,annotate->x+x,annotate->y+y);
    /*
      Annotate image with text.
    */
    status=XAnnotateImage(display,font_info,image_window,annotate->text,
      annotate_geometry,False,image);
    if (status == 0)
      return(False);
    annotate=annotate->previous;
    if (annotate != (AnnotateInfo *) NULL)
      {
        /*
          Free up memory.
        */
        (void) free((char *) annotate->next->text);
        (void) free((char *) annotate->next);
      }
  }
  XDefineCursor(display,image_window->id,image_window->cursor);
  /*
    Free up memory.
  */
  if (font_info != image_window->font_info)
    XFreeFont(display,font_info);
  XFreeGC(display,graphic_context);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X C l i p I m a g e W i n d o w                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XClipImageWindow displays a rectangle whose sizes changes as
%  the pointer moves.  When the mouse button is released, the geometry of
%  the image region defined within the rectangle is returned.
%
%  The format of the XClipImageWindow routine is:
%
%    XClipImageWindow(display,info_window,image_window,image,x_offset,y_offset)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o info_window: Specifies a pointer to a XWindowInfo structure.
%
%    o image_window: Specifies a pointer to a XWindowInfo structure.
%
%    o image: Specifies a pointer to a Image structure; returned from
%      ReadImage.
%
%    o x_offset: Specifies the initial offset in the x-direction of the
%      rectangle.
%
%    o y_offset: Specifies the initial offset in the y-direction of the
%      rectangle.
%
%
*/
static void XClipImageWindow(display,info_window,image_window,image,x_offset,
  y_offset)
Display
  *display;

XWindowInfo
  *info_window,
  *image_window;

Image
  *image;

int
  x_offset,
  y_offset;
{
#define ClipRectangle(x,y,width,height)  \
{  \
  if ((width >= MinRectangle) && (height >= MinRectangle))  \
    {  \
      XSetFunction(display,image_window->graphic_context,GXinvert);  \
      XDrawRectangle(display,image_window->id,image_window->graphic_context,  \
        x,y,width-1,height-1);  \
      XSetFunction(display,image_window->graphic_context,GXcopy);  \
    }  \
}
#define MinRectangle  3

  char
    text[1024];

  register int
    x,
    y;

  register unsigned int
    height,
    width;

  unsigned int
    state;

  XEvent
    event;

  /*
    Set the width of info window.
  */
  state=DefaultState;
  (void) sprintf(text," %dx%d%+d%+d \0",image_window->width,
    image_window->height,image_window->width,image_window->height);
  XSetWindowExtents(info_window,text,2);
  /*
    Size rectangle as pointer moves until the mouse button is released.
  */
  x=x_offset;
  y=y_offset;
  width=0;
  height=0;
  do
  {
    if ((width >= MinRectangle) && (height >= MinRectangle))
      {
        /*
          Display info and draw clipping rectangle.
        */
        if (!(state & InfoMappedState))
          {
            XMapWindow(display,info_window->id);
            state|=InfoMappedState;
          }
        (void) sprintf(text," %dx%d%+d%+d\0",width,height,x,y);
        XClearWindow(display,info_window->id);
        XDrawImageString(display,info_window->id,info_window->graphic_context,2,
          info_window->font_info->ascent+2,text,strlen(text));
        ClipRectangle(x,y,width,height);
      }
    else
      if (state & InfoMappedState)
        {
          /*
            Clipping rectangle is too small;  withdraw info window.
          */
          XWithdrawWindow(display,info_window->id,info_window->screen);
          state&=(~InfoMappedState);
        }
    /*
      Wait for next event.
    */
    XWindowEvent(display,image_window->id,ButtonPressMask | Button2MotionMask |
      ButtonReleaseMask | ExposureMask,&event);
    ClipRectangle(x,y,width,height);
    switch (event.type)
    {
      case ButtonPress:
        break;
      case ButtonRelease:
      {
        /*
          User has committed to clipping rectangle.
        */
        if (event.xbutton.button != Button2)
          break;
        state|=ExitState;
        break;
      }
      case Expose:
      {
        /*
          Repaint image window.
        */
        if (event.xexpose.count == 0)
          XDisplayImageWindow(display,image_window,0,0,image_window->width,
            image_window->height);
        break;
      }
      case MotionNotify:
      {
        /*
          Discard pending button motion events.
        */
        while (XCheckMaskEvent(display,Button2MotionMask,&event));
        x=event.xmotion.x;
        y=event.xmotion.y;
        /*
          Check boundary conditions.
        */
        if (x < 0)
          x=0;
        else
          if (x > image_window->width)
            x=image_window->width-1;
        if (x < x_offset)
          width=(unsigned int) (x_offset-x);
        else
          {
            width=(unsigned int) (x-x_offset);
            x=x_offset;
          }
        if (y < 0)
          y=0;
        else
          if (y > image_window->height)
            y=image_window->height-1;
        if (y < y_offset)
          height=(unsigned int) (y_offset-y);
        else
          {
            height=(unsigned int) (y-y_offset);
            y=y_offset;
          }
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  if (state & InfoMappedState)
    XWithdrawWindow(display,info_window->id,info_window->screen);
  if ((width >= MinRectangle) && (height >= MinRectangle))
    {
      int
        clip_x,
        clip_y;

      unsigned int
        clip_height,
        clip_width;

      unsigned long
        scale_factor;

      XWindowChanges
        window_changes;

      /*
        Check boundary conditions.
      */
      if (((x+(int) width) < image_window->x) ||
          ((y+(int) height) < image_window->y) ||
          (x > (image_window->x+image_window->ximage->width)) ||
          (y > (image_window->y+image_window->ximage->height)))
        return;
      if (x < image_window->x)
        {
          width-=(unsigned int) (image_window->x-x);
          x=image_window->x;
        }
      if (y < image_window->y)
        {
          height-=(unsigned int) (image_window->y-y);
          y=image_window->y;
        }
      if ((x+(int) width) > (image_window->x+image_window->ximage->width))
        width=(unsigned int) (image_window->x+image_window->ximage->width);
      if ((y+(int) height) > (image_window->y+image_window->ximage->height))
        height=(unsigned int) (image_window->y+image_window->ximage->height);
      x-=image_window->x;
      y-=image_window->y;
      /*
        Clipping geometry is relative to any previous clip geometry.
      */
      clip_x=0;
      clip_y=0;
      clip_width=image->columns;
      clip_height=image->rows;
      if (image_window->clip_geometry != (char *) NULL)
        (void) XParseGeometry(image_window->clip_geometry,&clip_x,&clip_y,
          &clip_width,&clip_height);
      else
        {
          /*
            Allocate clip geometry string.
          */
          image_window->clip_geometry=(char *) malloc(256);
          if (image_window->clip_geometry == (char *) NULL)
            Error("unable to clip X image",image_window->name);
        }
      /*
        Define the clip geometry string from the clipping rectangle.
      */
      scale_factor=(clip_width << 14)/image_window->ximage->width;
      if (x > 0)
        clip_x+=(x*scale_factor+8191) >> 14;
      clip_width=(width*scale_factor+8191) >> 14;
      if (clip_width == 0)
        clip_width=1;
      scale_factor=(clip_height << 14)/image_window->ximage->height;
      if (y > 0)
        clip_y+=(y*scale_factor+8191) >> 14;
      clip_height=(height*scale_factor+8191) >> 14;
      if (clip_height == 0)
        clip_height=1;
      (void) sprintf(image_window->clip_geometry,"%dx%d%+d%+d\0",clip_width,
        clip_height,clip_x,clip_y);
      /*
        Reconfigure image window as defined by clipping rectangle.
      */
      window_changes.width=width;
      window_changes.height=height;
      XReconfigureWMWindow(display,image_window->id,image_window->screen,
        CWWidth | CWHeight,&window_changes);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X C o n f i g u r e I m a g e W i n d o w                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XConfigureImageWindow creates a new X image the same size as the
%  image window.
%
%  The format of the XConfigureImageWindow routine is:
%
%    status=XConfigureImageWindow(display,resource_info,info_window,
%      image_window,image)
%
%  A description of each parameter follows:
%
%    o status: Function XConfigureImageWindow return True if the window is
%      resized.  False is returned is there is a memory shortage or if the
%      window fails to resize.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o info_window: Specifies a pointer to a XWindowInfo structure.
%
%    o image_window: Specifies a pointer to a XWindowInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int XConfigureImageWindow(display,resource_info,info_window,
  image_window,image)
Display
  *display;

XResourceInfo
  *resource_info;

XWindowInfo
  *info_window,
  *image_window;

Image
  *image;
{
  char
    text[1024];

  unsigned int
    state;

  XImage
    *configured_ximage;

  state=DefaultState;
  if ((image_window->width*image_window->height) > MinInfoSize)
    {
      /*
        Map image window.
      */
      (void) strcpy(text," Configuring image... ");
      XSetWindowExtents(info_window,text,2);
      XMapWindow(display,info_window->id);
      XDrawImageString(display,info_window->id,info_window->graphic_context,2,
        info_window->font_info->ascent+2,text,strlen(text));
      state|=InfoMappedState;
    }
  /*
    Resize image to fit image window dimensions.
  */
  configured_ximage=XMakeImage(display,resource_info,image_window,image,
    image_window->width,image_window->height);
  if (configured_ximage == (XImage *) NULL)
    {
      if (state & InfoMappedState)
        XWithdrawWindow(display,info_window->id,info_window->screen);
      return(False);
    }
  XDestroyImage(image_window->ximage);
  image_window->ximage=configured_ximage;
  /*
    Restore cursor.
  */
  if (state & InfoMappedState)
    XWithdrawWindow(display,info_window->id,info_window->screen);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X D i s p l a y I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XDisplayImage displays an image via X11.
%
%  The format of the XDisplayImage routine is:
%
%      displayed_image=XDisplayImage(display,resource_info,argv,argc,image,
%        terminate)
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
%    o terminate: A value other than zero is returned if the user requests
%      the program to terminate immediately.
%
%
*/
static Image *XDisplayImage(display,resource_info,argv,argc,image,terminate)
Display
  *display;

XResourceInfo
  *resource_info;

char
  **argv;

int
  argc;

Image
  *image;

unsigned int
  *terminate;
{
#define MagnifySize  256  /* must be a power of 2 */

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

  unsigned int
    state;

  unsigned long
    timeout;

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
    pixel_info;

  XStandardColormap
    *map_info;

  XVisualInfo
    *visual_info;

  XWindowInfo
    backdrop_window,
    icon_window,
    image_window,
    info_window,
    magnify_window,
    popup_window,
    superclass_window;

  XWMHints
    *manager_hints;

  /*
    Discard outstanding X events.
  */
  while (XEventsQueued(display,QueuedAfterFlush) > 0)
    XNextEvent(display,&event);
  /*
    Get the best visual this server supports.
  */
  visual_info=XBestVisualInfo(display,resource_info->visual_type,
    resource_info->map_type,&map_info);
  if (visual_info == (XVisualInfo *) NULL)
    Error("unable to get visual",resource_info->visual_type);
  /*
    Initialize colormap.
  */
  if (resource_info->map_type == (char *) NULL)
    map_info=XMakeStandardColormap(display,visual_info,resource_info,
      &pixel_info,image);
  else
    {
      register int
        i;

      XGetPixelInfo(display,visual_info,map_info,resource_info,&pixel_info);
      if (image->class == PseudoClass)
        {
          /*
            Initialize pixel array for images of type PseudoClass.
          */
          pixel_info.colors=image->colors;
          pixel_info.pixels=(unsigned long *)
            malloc((unsigned int) image->colors*sizeof(unsigned long));
          if (pixel_info.pixels == (unsigned long *) NULL)
            Error("unable to create colormap","memory allocation failed");
          for (i=0; i < image->colors; i++)
            pixel_info.pixels[i]=XStandardPixel(map_info,image->colormap[i],8);
        }
      /*
        Define background/border/foreground/pen pixels.
      */
      pixel_info.background_color.pixel=
        XStandardPixel(map_info,pixel_info.background_color,16);
      pixel_info.border_color.pixel=
        XStandardPixel(map_info,pixel_info.border_color,16);
      pixel_info.foreground_color.pixel=
        XStandardPixel(map_info,pixel_info.foreground_color,16);
      for (i=0; i < 9; i++)
        pixel_info.pen_color[i].pixel=
          XStandardPixel(map_info,pixel_info.pen_color[i],16);
    }
  colormap=map_info->colormap;
  /*
    Initialize font info.
  */
  (void) sprintf(text," [%d] %s %dx%d %s \0",image->scene,image->filename,
    image->columns,image->rows,XVisualClassName(visual_info));
  if (image->colors > 0)
    (void) sprintf(text,"%s%dc \0",text,image->colors);
  font_info=XBestFont(display,resource_info,text,image->columns);
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
  superclass_window.map_info=map_info;
  superclass_window.pixel_info=(&pixel_info);
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
  superclass_window.border_width=2;
  superclass_window.immutable=True;
  superclass_window.attributes.background_pixel=
    pixel_info.background_color.pixel;
  superclass_window.attributes.background_pixmap=(Pixmap) NULL;
  superclass_window.attributes.backing_store=WhenMapped;
  superclass_window.attributes.bit_gravity=ForgetGravity;
  superclass_window.attributes.border_pixel=pixel_info.border_color.pixel;
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
  graphic_context_value.background=pixel_info.background_color.pixel;
  graphic_context_value.foreground=pixel_info.foreground_color.pixel;
  graphic_context_value.font=font_info->fid;
  graphic_context_value.function=GXcopy;
  graphic_context_value.line_width=2;
  graphic_context_value.plane_mask=AllPlanes;
  graphic_context_value.subwindow_mode=IncludeInferiors;
  graphic_context=XCreateGC(display,superclass_window.id,GCBackground |
    GCFont | GCForeground | GCFunction | GCLineWidth | GCPlaneMask |
    GCSubwindowMode,&graphic_context_value);
  if (graphic_context == (GC) NULL)
    Error("unable to create graphic context",(char *) NULL);
  superclass_window.graphic_context=graphic_context;
  graphic_context_value.background=pixel_info.foreground_color.pixel;
  graphic_context_value.foreground=pixel_info.background_color.pixel;
  graphic_context=XCreateGC(display,superclass_window.id,GCBackground |
    GCFont | GCForeground | GCFunction | GCLineWidth | GCPlaneMask |
    GCSubwindowMode,&graphic_context_value);
  if (graphic_context == (GC) NULL)
    Error("unable to create graphic context",(char *) NULL);
  superclass_window.highlight_context=graphic_context;
  /*
    Initialize icon window.
  */
  icon_window=superclass_window;
  icon_window.name="ImageMagick Icon";
  XBestIconSize(display,&icon_window,image);
  icon_window.attributes.event_mask=ExposureMask | StructureNotifyMask;
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=False;
  manager_hints->initial_state=IconicState;
  manager_hints->window_group=superclass_window.id;
  XMakeWindow(display,root_window,argv,argc,class_hint,manager_hints,
    delete_property,&icon_window);
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
      (void) strcpy(image_window.name,"ImageMagick: ");
      (void) strcat(image_window.name,image->filename);
    }
  image_window.geometry=resource_info->image_geometry;
  image_window.width=image->columns;
  if (image_window.width > XDisplayWidth(display,visual_info->screen))
    image_window.width=XDisplayWidth(display,visual_info->screen);
  image_window.height=image->rows;
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
  image_window.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | EnterWindowMask | ExposureMask | KeyPressMask |
    KeyReleaseMask | LeaveWindowMask | OwnerGrabButtonMask |
    StructureNotifyMask;
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
    Initialize image X image structure.
  */
  image_window.x=0;
  image_window.y=0;
  image_window.ximage=XMakeImage(display,resource_info,&image_window,image,
    image->columns,image->rows);
  if (image_window.ximage == (XImage *) NULL)
    Error("unable to create X image",(char *) NULL);
  /*
    Initialize magnify window and cursor.
  */
  magnify_window=superclass_window;
  magnify_window.cursor=XMakeCursor(display,image_window.id,colormap,
    resource_info->background_color,resource_info->foreground_color);
  if (magnify_window.cursor == (Cursor) NULL)
    Error("unable to create cursor",(char *) NULL);
  magnify_window.name="ImageMagick Magnifier";
  magnify_window.width=MagnifySize;
  magnify_window.height=MagnifySize;
  magnify_window.min_width=MagnifySize;
  magnify_window.min_height=MagnifySize;
  magnify_window.width_inc=MagnifySize;
  magnify_window.height_inc=MagnifySize;
  magnify_window.immutable=False;
  magnify_window.attributes.cursor=magnify_window.cursor;
  magnify_window.attributes.event_mask=
    ExposureMask | KeyPressMask | StructureNotifyMask;
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=False;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=superclass_window.id;
  XMakeWindow(display,root_window,argv,argc,class_hint,manager_hints,
    delete_property,&magnify_window);
  /*
    Initialize magnify window X image structure.
  */
  magnify_window.x=image->columns >> 1;
  magnify_window.y=image->rows >> 1;
  magnify_window.ximage=XMakeImage(display,resource_info,&magnify_window,
    (Image *) NULL,magnify_window.width,magnify_window.height);
  if (magnify_window.ximage == (XImage *) NULL)
    Error("unable to create magnify image",(char *) NULL);
  /*
    Initialize popup window.
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
    Respond to events.
  */
  state=DefaultState;
  timeout=(~0);
  do
  {
    /*
      Handle a window event.
    */
    if (resource_info->delay > 0)
      if (XEventsQueued(display,QueuedAfterFlush) == 0)
        {
          /*
            Block if delay > 0.
          */
          (void) sleep(1);
          continue;
        }
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
                *MenuCommand="iwpar/\\<>onq",
                *MenuSelections[]=
                {
                  "Image Info",
                  "Write Image",
                  "Print Image",
                  "Annotate Image",
                  "Reflect Image",
                  "Rotate Right",
                  "Rotate Left",
                  "Half Size",
                  "Double Size",
                  "Restore Image",
                  "Next Image",
                  "Quit"
                };

              static int
                command_number;

              if (state & ControlState)
                {
                  /*
                    User pressed the image pan button.
                  */
                  XPanImageWindow(display,&info_window,&image_window,
                    event.xbutton.x,event.xbutton.y);
                  break;
                }
              /*
                Select a command from the pop-up menu.
              */
              command_number=XPopUpMenu(display,&popup_window,
                event.xbutton.x_root,event.xbutton.y_root,"Commands",
                MenuSelections,sizeof(MenuSelections)/sizeof(MenuSelections[0]),
                command);
              if (*command != (char) NULL)
                UserCommand(display,resource_info,&info_window,&popup_window,
                  &image_window,MenuCommand[command_number],&image,&timeout,
                  &state);
              break;
            }
            case Button2:
            {
              /*
                User pressed the image clip button.
              */
              XClipImageWindow(display,&info_window,&image_window,image,
                event.xbutton.x,event.xbutton.y);
              break;
            }
            case Button3:
            {
              /*
                User pressed the image magnify button.
              */
              magnify_window.x=event.xbutton.x-image_window.x;
              magnify_window.y=event.xbutton.y-image_window.y;
              if (state & MagnifyMappedState)
                XRaiseWindow(display,magnify_window.id);
              else
                {
                  XMapRaised(display,magnify_window.id);
                  state|=MagnifyMappedState;
                  XWindowEvent(display,magnify_window.id,ExposureMask,&event);
                }
              XMagnifyImageWindow(display,resource_info,&info_window,
                &image_window,&magnify_window);
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
              XWithdrawWindow(display,event.xclient.window,
                visual_info->screen);
        break;
      }
      case ConfigureNotify:
      {
        if (event.xconfigure.window == image_window.id)
          {
            static unsigned long
              scale_factor;

            /*
              Image window has a new configuration.
            */
            image_window.x=0;
            image_window.y=0;
            if ((event.xconfigure.width == image_window.width) &&
                (event.xconfigure.height == image_window.height))
              break;
            /*
              Make magnify offset proportional to the window size.
            */
            scale_factor=(event.xconfigure.width << 14)/image_window.width;
            magnify_window.x=(magnify_window.x*scale_factor+8191) >> 14;
            scale_factor=(event.xconfigure.height << 14)/image_window.height;
            magnify_window.y=(magnify_window.y*scale_factor+8191) >> 14;
            image_window.width=event.xconfigure.width;
            image_window.height=event.xconfigure.height;
            if (!(state & ImageMappedState))
              break;
            /*
              Make font proportional to the window size.
            */
            (void) sprintf(text," [%d] %s %dx%d %s \0",image->scene,
              image->filename,image->columns,image->rows,
              XVisualClassName(image_window.visual_info));
            if (image->colors > 0)
              (void) sprintf(text,"%s%dc \0",text,image->colors);
            XFreeFont(display,font_info);
            font_info=XBestFont(display,resource_info,text,image_window.width);
            if (font_info == (XFontStruct *) NULL)
              Error("unable to load font",resource_info->font);
            XSetFont(display,superclass_window.graphic_context,font_info->fid);
            XSetFont(display,superclass_window.highlight_context,
              font_info->fid);
            superclass_window.font_info=font_info;
            icon_window.font_info=font_info;
            backdrop_window.font_info=font_info;
            image_window.font_info=font_info;
            magnify_window.font_info=font_info;
            popup_window.font_info=font_info;
            info_window.font_info=font_info;
            /*
              If ConfigureState, image is already resized.
            */
            if (state & ConfigureWindowState)
              state&=(~ConfigureWindowState);
            else
              {
                unsigned int
                  status;
                
                status=XConfigureImageWindow(display,resource_info,&info_window,
                  &image_window,image);
                if (status == False)
                  XPopUpAlert(display,&popup_window,
                    "unable to configure X image",image_window.name);
              }
            while
              (XCheckWindowEvent(display,image_window.id,ExposureMask,&event));
            XDisplayImageWindow(display,&image_window,0,0,image_window.width,
              image_window.height);
            break;
          }
        if (event.xconfigure.window == magnify_window.id)
          {
            /*
              Magnify window has a new configuration.
            */
            magnify_window.width=event.xconfigure.width;
            magnify_window.height=event.xconfigure.height;
            if (((magnify_window.width % magnify_window.width_inc) != 0) ||
                ((magnify_window.height % magnify_window.height_inc) != 0))
              break;
            XDestroyImage(magnify_window.ximage);
            magnify_window.ximage=XMakeImage(display,resource_info,
              &magnify_window,(Image *) NULL,magnify_window.width,
              magnify_window.height);
            if (magnify_window.ximage == (XImage *) NULL)
              Error("unable to create magnify image",(char *) NULL);
            ConstrainMagnifyFactor((&image_window),(&magnify_window),
              resource_info->magnify);
            XMakeMagnifyImage(&magnify_window,image_window.ximage,
              resource_info->magnify);
            XPutImage(display,magnify_window.id,magnify_window.graphic_context,
              magnify_window.ximage,0,0,0,0,magnify_window.width,
              magnify_window.height);
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
          {
            XDisplayImageWindow(display,&image_window,event.xexpose.x,
              event.xexpose.y,(unsigned int) event.xexpose.width,
              (unsigned int) event.xexpose.height);
            /*
              Reset timeout after expose.
            */
            if (resource_info->delay == 0)
              timeout=(~0);
            else
              timeout=time((long *) 0)+resource_info->delay;
            break;
          }
        if (event.xexpose.window == magnify_window.id)
          {
            XPutImage(display,magnify_window.id,magnify_window.graphic_context,
              magnify_window.ximage,event.xexpose.x,event.xexpose.y,
              event.xexpose.x,event.xexpose.y,
              (unsigned int) event.xexpose.width,
              (unsigned int) event.xexpose.height);
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
        if (key_symbol == XK_Control_L)
          state|=ControlState;
        else
          if (key_symbol == XK_Help)
            Usage((char *) NULL,False);
          else
            if (!IsCursorKey(key_symbol) && !isdigit(*command))
              UserCommand(display,resource_info,&info_window,&popup_window,
                &image_window,*command,&image,&timeout,&state);
            else
              {
                /*
                  User specified a magnify factor or position.
                */
                if (!(state & MagnifyMappedState))
                  break;
                if (key_symbol == XK_Home)
                  {
                    magnify_window.x=0;
                    magnify_window.y=0;
                  }
                if (key_symbol == XK_Left)
                  magnify_window.x--;
                if (key_symbol == XK_Up)
                  magnify_window.y--;
                if (key_symbol == XK_Right)
                  magnify_window.x++;
                if (key_symbol == XK_Down)
                  magnify_window.y++;
                if (isdigit(*command))
                  resource_info->magnify=1 << atoi(command);
                /*
                  Magnify image.
                */
                ConstrainMagnifyFactor((&image_window),(&magnify_window),
                  resource_info->magnify);
                XMakeMagnifyImage(&magnify_window,image_window.ximage,
                  resource_info->magnify);
                XPutImage(display,magnify_window.id,
                  magnify_window.graphic_context,magnify_window.ximage,0,0,0,0,
                  magnify_window.width,magnify_window.height);
              }
        break;
      }
      case KeyRelease:
      {
        static char
          command[1024];

        static KeySym
          key_symbol;

        /*
          Respond to a user key release.
        */
        *command=(char) NULL;
        XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        if (key_symbol == XK_Control_L)
          state&=(~ControlState);
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
            state|=ImageMappedState;
            break;
          }
        if (event.xmap.window == magnify_window.id)
          {
            state|=MagnifyMappedState;
            break;
          }
        if (event.xmap.window == info_window.id)
          {
            state|=InfoMappedState;
            break;
          }
        if (event.xmap.window == icon_window.id)
          {
            /*
              Create icon image.
            */
            icon_window.ximage=XMakeImage(display,resource_info,&icon_window,
              image,icon_window.width,icon_window.height);
            if (icon_window.ximage == (XImage *) NULL)
              Error("unable to create icon image",(char *) NULL);
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
        if (event.xunmap.window == image_window.id)
          {
            state&=(~ImageMappedState);
            /*
              Unmap magnify window.
            */
            if (state & MagnifyMappedState)
              XWithdrawWindow(display,magnify_window.id,magnify_window.screen);
            break;
          }
        if (event.xunmap.window == magnify_window.id)
          {
            /*
              Destroy magnify image.
            */
            state&=(~MagnifyMappedState);
            break;
          }
        if (event.xunmap.window == info_window.id)
          {
            state&=(~InfoMappedState);
            break;
          }
        if (event.xunmap.window == icon_window.id)
          {
            /*
              Destroy icon image.
            */
            XDestroyImage(icon_window.ximage);
            icon_window.ximage=(XImage *) NULL;
            break;
          }
        break;
      }
      default:
        break;
    }
  }
  while ((timeout > time((long *) 0)) && !(state & ExitState));
  /*
    Initialize function return values.
  */
  *terminate=state & ExitState;
  if ((resource_info->print_filename != (char *) NULL) ||
      (resource_info->write_filename != (char *) NULL))
    {
      /*
        Update image with user transforms.
      */
      XDefineCursor(display,image_window.id,image_window.busy_cursor);
      XFlush(display);
      if ((image_window.clip_geometry != (char *) NULL) ||
          (image->columns != image_window.ximage->width) ||
          (image->rows != image_window.ximage->height))
        {
          char
            image_geometry[1024];

          /*
            Clip and/or scale image.
          */
          (void) sprintf(image_geometry,"%dx%d\0",image_window.ximage->width,
            image_window.ximage->height);
          image=TransformImage(image,image_window.clip_geometry,image_geometry,
            (char *) NULL);
        }
      if (resource_info->number_colors > 0)
        if ((image->class == DirectClass) ||
            (image->colors > resource_info->number_colors))
          QuantizeImage(image,resource_info->number_colors,
            resource_info->tree_depth,resource_info->dither,True);
      XDefineCursor(display,image_window.id,image_window.cursor);
    }
  /*
    Free up memory.
  */
  XDestroyWindow(display,info_window.id);
  XDestroyWindow(display,popup_window.id);
  XDestroyWindow(display,magnify_window.id);
  XDestroyImage(magnify_window.ximage);
  XDestroyWindow(display,image_window.id);
  XDestroyImage(image_window.ximage);
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
  XFreeCursor(display,magnify_window.cursor);
  XFreeFont(display,font_info);
  /*
    Discard pending events before freeing colormap.
  */
  while (XEventsQueued(display,QueuedAfterFlush) > 0)
    XNextEvent(display,&event);
  if (resource_info->map_type == (char *) NULL)
    if (colormap != XDefaultColormap(display,visual_info->screen))
      XFreeColormap(display,colormap);
    else
      if ((visual_info->class == GrayScale) ||
          (visual_info->class == PseudoColor))
        XFreeColors(display,colormap,pixel_info.pixels,(int) image->colors,0);
  if (pixel_info.colors > 0)
    (void) free((char *) pixel_info.pixels);
  XFree((void *) map_info);
  XFree((void *) visual_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X D i s p l a y I m a g e W i n d o w                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XDisplayImageWindow displays an image in a X window.
%
%  The format of the XDisplayImageWindow routine is:
%
%      XDisplayImageWindow(display,image_window,x,y,width,height)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o image_window: Specifies a pointer to a XWindowInfo structure.
%
%    o x: Specifies the x coordinate relative to the image window.
%
%    o y: Specifies the y coordinate relative to the image window.
%
%    o width: Specifies the width in pixels of the rectangular area to
%      display.
%
%    o height: Specifies the height in pixels of the rectangular area to
%      display.
%
%
*/
static void XDisplayImageWindow(display,image_window,x,y,width,height)
Display
  *display;

XWindowInfo
  *image_window;

register int
  x,
  y;

register unsigned int
  height,
  width;
{
  /*
    Check boundary conditions.
  */
  if ((x+(int) width) > (int) image_window->width)
    width=(unsigned int) ((int) image_window->width-x);
  if ((y+(int) height) > (int) image_window->height)
    height=(unsigned int) ((int) image_window->height-y);
  if (x < image_window->x)
    {
      XClearArea(display,image_window->id,x,y,
        (unsigned int) (image_window->x-x),(unsigned int) (y+(int) height),
        False);
      width-=(unsigned int) (image_window->x-x);
      x=image_window->x;
    }
  if (y < image_window->y)
    {
      XClearArea(display,image_window->id,x,y,width,
        (unsigned int) (image_window->y-y),False);
      height-=(unsigned int) (image_window->y-y);
      y=image_window->y;
    }
  if ((x+(int) width) > (image_window->x+image_window->ximage->width))
    {
      width=(unsigned int) (image_window->x+image_window->ximage->width);
      XClearArea(display,image_window->id,(int) width,y,
        image_window->width-width,image_window->height,False);
    }
  if ((y+(int) height) > (image_window->y+image_window->ximage->height))
    {
      height=(unsigned int) (image_window->y+image_window->ximage->height);
      XClearArea(display,image_window->id,x,(int) height,width,
        image_window->height-height,False);
    }
  /*
    Display image.
  */
  XPutImage(display,image_window->id,image_window->graphic_context,
    image_window->ximage,x-image_window->x,y-image_window->y,x,y,width,height);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X D i s p l a y B a c k g r o u n d I m a g e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XDisplayBackgroundImage displays an image in the root window.
%
%  The format of the XDisplayBackgroundImage routine is:
%
%      XDisplayBackgroundImage(display,resource_info,window_id,image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window_id: Specifies a pointer to a string with a window id or name.
%
%    o image: Specifies a pointer to a Image structure; returned from
%      ReadImage.
%
%
*/
static void XDisplayBackgroundImage(display,resource_info,window_id,image)
Display
  *display;

XResourceInfo
  *resource_info;

char
  *window_id;

Image
  *image;
{
  Atom
    property,
    type;

  Colormap
    colormap;

  Cursor
    arrow_cursor,
    watch_cursor;

  int
    format,
    i;

  Pixmap
    pixmap;

  unsigned char
    *data;

  unsigned int
    height,
    number_children,
    width;

  unsigned long
    after,
    length;

  Window
    *children,
    parent,
    root,
    root_window,
    target_window;

  XGCValues
    graphic_context_value;

  XPixelInfo
    pixel_info;

  XStandardColormap
    *map_info;

  XWindowInfo
    image_window;

  XVisualInfo
    *visual_info;

  /*
    Initialize visual info.
  */
  if ((resource_info->visual_type != (char *) NULL) ||
      (resource_info->map_type != (char *) NULL))
    visual_info=XBestVisualInfo(display,resource_info->visual_type,
      resource_info->map_type,&map_info);
  else
    {
      int
        number_visuals;

      XVisualInfo
        visual_template;

      /*
        Get the default visual.
      */
      visual_template.visualid=
        XVisualIDFromVisual(XDefaultVisual(display,XDefaultScreen(display)));
      visual_info=XGetVisualInfo(display,VisualIDMask,&visual_template,
        &number_visuals);
    }
  if (visual_info == (XVisualInfo *) NULL)
    Error("unable to get visual",resource_info->visual_type);
  if (visual_info->visual != XDefaultVisual(display,visual_info->screen))
    Error("visual must be server default",resource_info->visual_type);
  /*
    Determine if root window is virtual.
  */
  root_window=XRootWindow(display,visual_info->screen);
  property=XInternAtom(display,"__SWM_VROOT",False);
  XQueryTree(display,root_window,&root,&parent,&children,&number_children);
  for (i=0; i < number_children; i++)
  {
    (void) XGetWindowProperty(display,children[i],property,0L,1L,False,
      XA_WINDOW,&type,&format,&length,&after,&data);
    if ((format == 32) && (length == 1) && (after == 0))
      {
        /*
          Root window is a virtual root.
        */
        root_window=(*((Window *) data));
        break;
      }
  }
  if (children != (Window *) NULL)
    XFree((void *) children);
  /*
    If there are previous resources on the root window, destroy them.
  */
  property=XInternAtom(display,"_XSETROOT_ID",False);
  if (property == (Atom) NULL)
    Error("unable to create X property","_XSETROOT_ID");
  (void) XGetWindowProperty(display,root_window,property,0L,1L,True,
    AnyPropertyType,&type,&format,&length,&after,&data);
  if ((type == XA_PIXMAP) && (format == 32) && (length == 1) &&
      (after == 0))
    {
      /*
        Free previous resources on the root window.
      */
      XKillClient(display,*((Pixmap *) data));
      XFree((void *) data);
    }
  /*
    Initialize colormap.
  */
  if (resource_info->map_type == (char *) NULL)
    map_info=XMakeStandardColormap(display,visual_info,resource_info,
      &pixel_info,image);
  else
    {
      if (image->class == PseudoClass)
        {
          /*
            Initialize pixel array for images of type PseudoClass.
          */
          pixel_info.pixels=(unsigned long *)
            malloc((unsigned int) image->colors*sizeof(unsigned long));
          if (pixel_info.pixels == (unsigned long *) NULL)
            Error("unable to create colormap","memory allocation failed");
          for (i=0; i < image->colors; i++)
            pixel_info.pixels[i]=XStandardPixel(map_info,image->colormap[i],8);
        }
      /*
        Define background/border/foreground pixels.
      */
      XGetPixelInfo(display,visual_info,map_info,resource_info,&pixel_info);
      pixel_info.background_color.pixel=
        XStandardPixel(map_info,pixel_info.background_color,16);
      pixel_info.border_color.pixel=
        XStandardPixel(map_info,pixel_info.border_color,16);
      pixel_info.foreground_color.pixel=
        XStandardPixel(map_info,pixel_info.foreground_color,16);
      for (i=0; i < 9; i++)
        pixel_info.pen_color[i].pixel=
          XStandardPixel(map_info,pixel_info.pen_color[i],16);
    }
  /*
    The colormap must be the default colormap.
  */
  colormap=map_info->colormap;
  if (colormap != XDefaultColormap(display,visual_info->screen))
    Error("unable to display X image on the root window","too many colors");
  /*
    Determine target window.
  */
  target_window=(Window) NULL;
  if (strcmp(window_id,"root") == 0)
    target_window=root_window;
  else
    {
      if (isdigit(*window_id))
        target_window=XWindowByID(display,root_window,
          (Window) strtol(window_id,(char **) NULL,0));
      if (target_window == (Window) NULL)
        target_window=XWindowByName(display,root_window,window_id);
      if (target_window == (Window) NULL)
        Error("No window with specified id exists",window_id);
    }
  /*
    Initialize cursor.
  */
  arrow_cursor=XCreateFontCursor(display,XC_arrow);
  watch_cursor=XCreateFontCursor(display,XC_watch);
  if ((arrow_cursor == (Cursor) NULL) || (watch_cursor == (Cursor) NULL))
    Error("unable to create cursor",(char *) NULL);
  /*
    Initialize image window attributes.
  */
  image_window.id=target_window;
  image_window.visual_info=visual_info;
  image_window.screen=visual_info->screen;
  image_window.depth=visual_info->depth;
  image_window.clip_geometry=(char *) NULL;
  image_window.map_info=map_info;
  image_window.pixel_info=(&pixel_info);
  image_window.cursor=arrow_cursor;
  image_window.busy_cursor=watch_cursor;
  image_window.x=0;
  image_window.y=0;
  image_window.width=image->columns;
  if (image_window.width > XDisplayWidth(display,visual_info->screen))
    image_window.width=XDisplayWidth(display,visual_info->screen);
  image_window.height=image->rows;
  if (image_window.height > XDisplayHeight(display,visual_info->screen))
    image_window.height=XDisplayHeight(display,visual_info->screen);
  image_window.border_width=resource_info->border_width;
  /*
    Graphic context superclass.
  */
  graphic_context_value.background=pixel_info.background_color.pixel;
  graphic_context_value.foreground=pixel_info.foreground_color.pixel;
  graphic_context_value.fill_style=FillSolid;
  graphic_context_value.function=GXcopy;
  graphic_context_value.plane_mask=AllPlanes;
  graphic_context_value.subwindow_mode=IncludeInferiors;
  image_window.graphic_context=XCreateGC(display,image_window.id,
    GCBackground | GCFillStyle | GCForeground | GCFunction | GCPlaneMask |
     GCSubwindowMode,&graphic_context_value);
  if (image_window.graphic_context == (GC) NULL)
    Error("unable to create graphic context",(char *) NULL);
  graphic_context_value.background=pixel_info.foreground_color.pixel;
  graphic_context_value.foreground=pixel_info.background_color.pixel;
  image_window.highlight_context=XCreateGC(display,image_window.id,
    GCBackground | GCForeground | GCFunction | GCPlaneMask | GCSubwindowMode,
    &graphic_context_value);
  if (image_window.highlight_context == (GC) NULL)
    Error("unable to create graphic context",(char *) NULL);
  /*
    Create the X image.
  */
  image_window.ximage=XMakeImage(display,resource_info,&image_window,image,
    image->columns,image->rows);
  if (image_window.ximage == (XImage *) NULL)
    Error("unable to create X image",(char *) NULL);
  /*
    Adjust image dimensions as specified by backdrop or geometry options.
  */
  width=image_window.width;
  height=image_window.height;
  if (resource_info->backdrop)
    {
      /*
        Center image on root window.
      */
      image_window.x=
        XDisplayWidth(display,visual_info->screen)/2-image->columns/2;
      image_window.y=
        XDisplayHeight(display,visual_info->screen)/2-image->rows/2;
      width=XDisplayWidth(display,visual_info->screen);
      height=XDisplayHeight(display,visual_info->screen);
    }
  if (resource_info->image_geometry != (char *) NULL)
    {
      char
        default_geometry[1024];

      int
        flags,
        gravity;

      XSizeHints
        *size_hints;

      /*
        User specified geometry.
      */
      size_hints=XAllocSizeHints();
      if (size_hints == (XSizeHints *) NULL)
        Error("unable to display on root","memory allocation failed");
      size_hints->flags=(long int) NULL;
      (void) sprintf(default_geometry,"%dx%d\0",width,height);
      flags=XWMGeometry(display,visual_info->screen,
        resource_info->image_geometry,default_geometry,
        image_window.border_width,size_hints,&image_window.x,&image_window.y,
        (int *) &width,(int *) &height,&gravity);
      if (flags & (XValue | YValue))
        {
          width=XDisplayWidth(display,visual_info->screen);
          height=XDisplayHeight(display,visual_info->screen);
        }
      XFree((void *) size_hints);
    }
  /*
    Create the root pixmap.
  */
  pixmap=XCreatePixmap(display,image_window.id,width,height,image_window.depth);
  if (pixmap == (Pixmap) NULL)
    Error("unable to create X pixmap",(char *) NULL);
  /*
    Display pixmap on the root window.
  */
  if ((width > image_window.width) || (height > image_window.height))
    XFillRectangle(display,pixmap,image_window.highlight_context,0,0,width,
      height);
  XPutImage(display,pixmap,image_window.graphic_context,image_window.ximage,
    0,0,image_window.x,image_window.y,image_window.width,image_window.height);
  XSetWindowBackgroundPixmap(display,image_window.id,pixmap);
  XClearWindow(display,image_window.id);
  /*
    Free resources.
  */
  if (pixel_info.colors > 0)
    (void) free((char *) pixel_info.pixels);
  XFreePixmap(display,pixmap);
  XDestroyImage(image_window.ximage);
  XFreeGC(display,image_window.graphic_context);
  XFreeGC(display,image_window.highlight_context);
  XFreeCursor(display,arrow_cursor);
  XFreeCursor(display,watch_cursor);
  XFree((void *) visual_info);
  XFree((void *) map_info);
  /*
    Put property on root window and set close-down mode to RetainPermanent.
  */
  pixmap=XCreatePixmap(display,root_window,1,1,1);
  if (pixmap == (Pixmap) NULL)
    Error("unable to create X pixmap",(char *) NULL);
  XChangeProperty(display,root_window,property,XA_PIXMAP,32,PropModeReplace,
    (unsigned char *) &pixmap,1);
  XSetCloseDownMode(display,RetainPermanent);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a g n i f y I m a g e W i n d o w                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMagnifyImageWindow magnifies portions of the image as indicated
%  by the pointer.  The magnified portion is displayed in a separate window.
%
%  The format of the XMagnifyImageWindow routine is:
%
%    XMagnifyImageWindow(display,resource_info,info_window,image_window,
%      magnify_window)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o info_window: Specifies a pointer to a XWindowInfo structure.
%
%    o image_window: Specifies a pointer to a XWindowInfo structure.
%
%    o magnify_window: Specifies a pointer to a XWindowInfo structure.
%
%
*/
static void XMagnifyImageWindow(display,resource_info,info_window,image_window,
  magnify_window)
Display
  *display;

XResourceInfo
  *resource_info;

XWindowInfo
  *info_window,
  *image_window,
  *magnify_window;
{
  char
    text[1024];

  register int
    x,
    y;

  unsigned int
    state;

  XColor
    color;

  XEvent
    event;

  /*
    Map info window.
  */
  state=DefaultState;
  (void) sprintf(text," %+d%+d #%02x%02x%02x \0",image_window->width,
    image_window->height,MaxRgb,MaxRgb,MaxRgb);
  XSetWindowExtents(info_window,text,2);
  XMapWindow(display,info_window->id);
  state|=InfoMappedState;
  /*
    Update magnified image until the mouse button is released.
  */
  XDefineCursor(display,image_window->id,magnify_window->cursor);
  ConstrainMagnifyFactor(image_window,magnify_window,resource_info->magnify);
  state|=MagnifyState;
  do
  {
    if (state & MagnifyState)
      {
        /*
          Display pointer position.
        */
        color.pixel=
          XGetPixel(image_window->ximage,magnify_window->x,magnify_window->y);
        XQueryColor(display,image_window->map_info->colormap,&color);
        (void) sprintf(text," %+d%+d #%02x%02x%02x \0",magnify_window->x,
          magnify_window->y,color.red >> 8,color.green >> 8,color.blue >> 8);
        XClearWindow(display,info_window->id);
        XDrawImageString(display,info_window->id,info_window->graphic_context,2,
          info_window->font_info->ascent+2,text,strlen(text));
        /*
          Display magnified image.
        */
        XMakeMagnifyImage(magnify_window,image_window->ximage,
          resource_info->magnify);
        XPutImage(display,magnify_window->id,magnify_window->graphic_context,
          magnify_window->ximage,0,0,0,0,magnify_window->width,
          magnify_window->height);
      }
    /*
      Wait for next event.
    */
    XWindowEvent(display,image_window->id,ButtonPressMask | Button3MotionMask |
      ButtonReleaseMask | ExposureMask | EnterWindowMask | LeaveWindowMask | 
      KeyPressMask,&event);
    switch (event.type)
    {
      case ButtonPress:
        break;
      case ButtonRelease:
      {
        /*
          User has finished magnifying image.
        */
        if (event.xbutton.button != Button3)
          break;
        state|=ExitState;
        break;
      }
      case Expose:
      {
        /*
          Repaint image window.
        */
        if (event.xexpose.count == 0)
          XDisplayImageWindow(display,image_window,0,0,image_window->width,
            image_window->height);
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
        *command=(char) NULL;
        XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        if (!isdigit(*command))
          XBell(display,0);
        else
          {
            /*
              User specified a new magnify factor.
            */
            resource_info->magnify=1 << atoi(command);
            ConstrainMagnifyFactor(image_window,magnify_window,
              resource_info->magnify);
            break;
          }
        break;
      }
      case EnterNotify:
      {
        x=event.xcrossing.x;
        y=event.xcrossing.y;
        /*
          Check boundary conditions.
        */
        if (x < 0)
          x=0;
        else
          if (x >= image_window->width)
            x=image_window->width-1;
        if (y < 0)
          y=0;
        else
         if (y >= image_window->height)
           y=image_window->height-1;
        magnify_window->x=x-image_window->x;
        magnify_window->y=y-image_window->y;
        state|=MagnifyState;
        break;
      }
      case LeaveNotify:
      {
        state&=(~MagnifyState);
        break;
      }
      case MotionNotify:
      {
        /*
          Discard pending button motion events.
        */
        while (XCheckMaskEvent(display,Button3MotionMask,&event));
        x=event.xmotion.x;
        y=event.xmotion.y;
        /*
          Map and unmap info window as text cursor crosses its boundaries.
        */
        if (state & InfoMappedState)
          {
            if ((x < (info_window->x+info_window->width)) &&
                (y < (info_window->y+info_window->height)))
              {
                XWithdrawWindow(display,info_window->id,info_window->screen);
                state&=(~InfoMappedState);
              }
          }
        else
          if ((x > (info_window->x+info_window->width)) ||
              (y > (info_window->y+info_window->height)))
            {
              XMapWindow(display,info_window->id);
              state|=InfoMappedState;
            }
        /*
          Check boundary conditions.
        */
        if (x < 0)
          x=0;
        else
          if (x >= image_window->width)
            x=image_window->width-1;
        if (y < 0)
          y=0;
        else
         if (y >= image_window->height)
           y=image_window->height-1;
        magnify_window->x=x-image_window->x;
        magnify_window->y=y-image_window->y;
        break;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  XDefineCursor(display,image_window->id,image_window->cursor);
  if (state & InfoMappedState)
    XWithdrawWindow(display,info_window->id,info_window->screen);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e C u r s o r                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeCursor creates a crosshairs X11 cursor.
%
%  The format of the XMakeCursor routine is:
%
%      XMakeCursor(display,window,colormap,background_color,foreground_color)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies the ID of the window for which the cursor is
%      assigned.
%
%    o colormap: Specifies the ID of the colormap from which the background
%      and foreground color will be retrieved.
%
%    o background_color: Specifies the color to use for the cursor background.
%
%    o foreground_color: Specifies the color to use for the cursor foreground.
%
%
*/
static Cursor XMakeCursor(display,window,colormap,background_color,
  foreground_color)
Display
  *display;

Colormap
  colormap;

Window
  window;

char
  *background_color,
  *foreground_color;
{
#define scope_height 17
#define scope_mask_width 17
#define scope_mask_height 17
#define scope_x_hot 8
#define scope_y_hot 8
#define scope_width 17

  static unsigned char scope_bit[] =
    {
      0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x7e, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x00, 0x00
    };

  static unsigned char scope_mask_bit[] =
    {
      0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00,
      0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x7f, 0xfc, 0x01,
      0x7f, 0xfc, 0x01, 0x7f, 0xfc, 0x01, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00,
      0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00,
      0x80, 0x03, 0x00
    };

  Cursor
    cursor;

  Pixmap
    mask,
    source;

  XColor
    background,
    foreground;

  source=XCreateBitmapFromData(display,window,(char *) scope_bit,scope_width,
    scope_height);
  mask=XCreateBitmapFromData(display,window,(char *) scope_mask_bit,
    scope_width,scope_height);
  if ((source == (Pixmap) NULL) || (mask == (Pixmap) NULL))
    Error("unable to create pixmap",(char *) NULL);
  XParseColor(display,colormap,background_color,&background);
  XParseColor(display,colormap,foreground_color,&foreground);
  cursor=XCreatePixmapCursor(display,source,mask,&foreground,&background,
    scope_x_hot,scope_y_hot);
  if (cursor == (Cursor) NULL)
    Error("unable to create cursor",(char *) NULL);
  XFreePixmap(display,source);
  XFreePixmap(display,mask);
  return(cursor);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e M a g n i f y I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeMagnifyImage magnifies a region of an X image and returns it.
%
%  The format of the XMakeMagnifyImage routine is:
%
%      XMakeMagnifyImage(magnify_window,ximage,magnify)
%
%  A description of each parameter follows:
%
%    o magnify_window: Specifies a pointer to a XWindowInfo structure.
%
%    o ximage: Specifies a pointer to a XImage structure;  returned from
%      XMakeImage.
%
%    o magnify: level of image magnification.
%
%
*/
static void XMakeMagnifyImage(magnify_window,ximage,magnify)
XWindowInfo
  *magnify_window;

XImage
  *ximage;

unsigned int
  magnify;
{
  int
    x,
    y;

  register unsigned char
    *p,
    *q;

  register unsigned int
    j,
    k,
    l;

  unsigned int
    i,
    scanline_pad;

  /*
    Check boundry conditions.
  */
  x=magnify_window->x-(magnify_window->ximage->width/magnify/2);
  if (x < 0)
    x=0;
  else
    if (x > (ximage->width-(magnify_window->ximage->width/magnify)))
      x=ximage->width-magnify_window->ximage->width/magnify;
  y=magnify_window->y-(magnify_window->ximage->width/magnify/2);
  if (y < 0)
    y=0;
  else
    if (y > (ximage->height-(magnify_window->ximage->height/magnify)))
      y=ximage->height-magnify_window->ximage->height/magnify;
  q=(unsigned char *) magnify_window->ximage->data;
  scanline_pad=magnify_window->ximage->bytes_per_line-
    ((magnify_window->ximage->width*
    magnify_window->ximage->bits_per_pixel) >> 3);
  if (ximage->bits_per_pixel < 8)
    {
      register unsigned char
        byte,
        p_bit,
        q_bit;

      register unsigned int
        plane;

      switch (ximage->bitmap_bit_order)
      {
        case LSBFirst:
        {
          /*
            Magnify little-endian bitmap.
          */
          for (i=0; i < magnify_window->ximage->height; i+=magnify)
          {
            /*
              Propogate pixel magnify rows.
            */
            for (j=0; j < magnify; j++)
            {
              p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
                ((x*ximage->bits_per_pixel) >> 3);
              p_bit=(x*ximage->bits_per_pixel) & 0x07;
              q_bit=0;
              byte=0;
              for (k=0; k < magnify_window->ximage->width; k+=magnify)
              {
                /*
                  Propogate pixel magnify columns.
                */
                for (l=0; l < magnify; l++)
                {
                  /*
                    Propogate each bit plane.
                  */
                  for (plane=0; plane < ximage->bits_per_pixel; plane++)
                  {
                    byte>>=1;
                    if (*p & (0x01 << (p_bit+plane)))
                      byte|=0x80;
                    q_bit++;
                    if (q_bit == 8)
                      {
                        *q++=byte;
                        q_bit=0;
                        byte=0;
                      }
                  }
                }
                p_bit+=ximage->bits_per_pixel;
                if (p_bit == 8)
                  {
                    p++;
                    p_bit=0;
                  }
                if (q_bit > 0)
                  *q=byte >> (8-q_bit);
                q+=scanline_pad;
              }
            }
            y++;
          }
          break;
        }
        case MSBFirst:
        default:
        {
          /*
            Magnify big-endian bitmap.
          */
          for (i=0; i < magnify_window->ximage->height; i+=magnify)
          {
            /*
              Propogate pixel magnify rows.
            */
            for (j=0; j < magnify; j++)
            {
              p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
                ((x*ximage->bits_per_pixel) >> 3);
              p_bit=(x*ximage->bits_per_pixel) & 0x07;
              q_bit=0;
              byte=0;
              for (k=0; k < magnify_window->ximage->width; k+=magnify)
              {
                /*
                  Propogate pixel magnify columns.
                */
                for (l=0; l < magnify; l++)
                {
                  /*
                    Propogate each bit plane.
                  */
                  for (plane=0; plane < ximage->bits_per_pixel; plane++)
                  {
                    byte<<=1;
                    if (*p & (0x80 >> (p_bit+plane)))
                      byte|=0x01;
                    q_bit++;
                    if (q_bit == 8)
                      {
                        *q++=byte;
                        q_bit=0;
                        byte=0;
                      }
                  }
                }
                p_bit+=ximage->bits_per_pixel;
                if (p_bit == 8)
                  {
                    p++;
                    p_bit=0;
                  }
                if (q_bit > 0)
                  *q=byte << (8-q_bit);
                q+=scanline_pad;
              }
            }
            y++;
          }
          break;
        }
      }
    }
  else
    switch (ximage->bits_per_pixel)
    {
      case 8:
      {
        /*
          Magnify 8 bit X image.
        */
        for (i=0; i < magnify_window->ximage->height; i+=magnify)
        {
          /*
            Propogate pixel magnify rows.
          */
          for (j=0; j < magnify; j++)
          {
            p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
              ((x*ximage->bits_per_pixel) >> 3);
            for (k=0; k < magnify_window->ximage->width; k+=magnify)
            {
              /*
                Propogate pixel magnify columns.
              */
              for (l=0; l < magnify; l++)
                *q++=(*p);
              p++;
            }
            q+=scanline_pad;
          }
          y++;
        }
        break;
      }
      default:
      {
        register unsigned int
          bytes_per_pixel,
          m;

        /*
          Magnify multi-byte X image.
        */
        bytes_per_pixel=ximage->bits_per_pixel >> 3;
        for (i=0; i < magnify_window->ximage->height; i+=magnify)
        {
          /*
            Propogate pixel magnify rows.
          */
          for (j=0; j < magnify; j++)
          {
            p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
              ((x*ximage->bits_per_pixel) >> 3);
            for (k=0; k < magnify_window->ximage->width; k+=magnify)
            {
              /*
                Propogate pixel magnify columns.
              */
              for (l=0; l < magnify; l++)
                for (m=0; m < bytes_per_pixel; m++)
                  *q++=(*(p+m));
              p+=bytes_per_pixel;
            }
            q+=scanline_pad;
          }
          y++;
        }
        break;
      }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X P a n I m a g e W i n d o w                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XPanImageWindow pans the image until the mouse button is released.
%
%  The format of the XPanImageWindow routine is:
%
%    XPanImageWindow(display,info_window,image_window,x_offset,y_offset)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o info_window: Specifies a pointer to a XWindowInfo structure.
%
%    o image_window: Specifies a pointer to a XWindowInfo structure.
%
%    o x_offset: Specifies the initial offset in the x-direction.
%
%    o y_offset: Specifies the initial offset in the y-direction.
%
%
*/
static void XPanImageWindow(display,info_window,image_window,x_offset,y_offset)
Display
  *display;

XWindowInfo
  *info_window,
  *image_window;

int
  x_offset,
  y_offset;
{
  char
    text[1024];

  Cursor
    cursor;

  unsigned int
    state;

  XEvent
    event;

  /*
    Map info window.
  */
  state=DefaultState;
  (void) sprintf(text," %dx%d%+d%+d  \0",image_window->ximage->width,
    image_window->ximage->height,image_window->ximage->width,
    image_window->ximage->height);
  XSetWindowExtents(info_window,text,2);
  XMapWindow(display,info_window->id);
  state|=InfoMappedState;
  /*
    Define cursor.
  */
  if ((image_window->ximage->width > image_window->width) &&
      (image_window->ximage->height > image_window->height))
    cursor=XCreateFontCursor(display,XC_fleur);
  else
    if (image_window->ximage->width > image_window->width)
      cursor=XCreateFontCursor(display,XC_sb_h_double_arrow);
    else
      if (image_window->ximage->height > image_window->height)
        cursor=XCreateFontCursor(display,XC_sb_v_double_arrow);
      else
        cursor=XCreateFontCursor(display,XC_arrow);
  if (cursor == (Cursor) NULL)
    Error("unable to create cursor",(char *) NULL);
  XDefineCursor(display,image_window->id,cursor);
  /*
    Pan image as pointer moves until the mouse button is released.
  */
  do
  {
    /*
      Display image pan offset.
    */
    (void) sprintf(text," %dx%d%+d%+d \0",image_window->ximage->width,
      image_window->ximage->height,image_window->x,image_window->y);
    XClearWindow(display,info_window->id);
    XDrawImageString(display,info_window->id,info_window->graphic_context,2,
      info_window->font_info->ascent+2,text,strlen(text));
    /*
      Display image window.
    */
    XDisplayImageWindow(display,image_window,0,0,image_window->width,
      image_window->height);
    /*
      Wait for next event.
    */
    XWindowEvent(display,image_window->id,ButtonPressMask | Button1MotionMask |
      ButtonReleaseMask,&event);
    switch (event.type)
    {
      case ButtonPress:
        break;
      case ButtonRelease:
      {
        /*
          User has finished panning the image.
        */
        if (event.xbutton.button != Button1)
          break;
        state|=ExitState;
        break;
      }
      case MotionNotify:
      {
        /*
          Discard pending button motion events.
        */
        while (XCheckMaskEvent(display,Button1MotionMask,&event));
        image_window->x-=(x_offset-event.xmotion.x);
        image_window->y-=(y_offset-event.xmotion.y);
        x_offset=event.xmotion.x;
        y_offset=event.xmotion.y;
        /*
          Check boundary conditions.
        */
        if (image_window->x < (-image_window->ximage->width))
          image_window->x=(-image_window->ximage->width);
        else
          if (image_window->x > image_window->ximage->width)
            image_window->x=image_window->ximage->width;
        if (image_window->y < (-image_window->ximage->height))
          image_window->y=(-image_window->ximage->height);
        else
          if (image_window->y > image_window->ximage->height)
            image_window->y=image_window->ximage->height;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  XDefineCursor(display,image_window->id,image_window->cursor);
  XFreeCursor(display,cursor);
  XWithdrawWindow(display,info_window->id,info_window->screen);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X R e f l e c t I m a g e W i n d o w                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XReflectImageWindow reflects the scanlines of an image.
%
%  The format of the XReflectImageWindow routine is:
%
%    status=XReflectImageWindow(display,info_window,image_window,image)
%
%  A description of each parameter follows:
%
%    o status: Function XReflectImageWindow return True if the window scanlines
%      reverse.  False is returned is there is a memory shortage or if the
%      window scanlines fails to reverse.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o info_window: Specifies a pointer to a XWindowInfo structure.
%
%    o image_window: Specifies a pointer to a XWindowInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int XReflectImageWindow(display,info_window,image_window,image)
Display
  *display;

XWindowInfo
  *info_window,
  *image_window;

Image
  **image;
{
  char
    text[1024];

  Image
    *reflected_image;

  unsigned int
    state;

  state=DefaultState;
  if (((*image)->columns*(*image)->rows) > MinInfoSize)
    {
      /*
        Map image window.
      */
      (void) strcpy(text," Reflecting image... ");
      XSetWindowExtents(info_window,text,2);
      XMapWindow(display,info_window->id);
      XDrawImageString(display,info_window->id,info_window->graphic_context,2,
        info_window->font_info->ascent+2,text,strlen(text));
      state|=InfoMappedState;
    }
  /*
    Reflect image scanlines.
  */
  XDefineCursor(display,image_window->id,image_window->busy_cursor);
  XFlush(display);
  reflected_image=ReflectImage(*image);
  XDefineCursor(display,image_window->id,image_window->cursor);
  if (state & InfoMappedState)
    XWithdrawWindow(display,info_window->id,info_window->screen);
  if (reflected_image == (Image *) NULL)
    return(False);
  DestroyImage(*image);
  *image=reflected_image;
  if (image_window->clip_geometry != (char *) NULL)
    {
      int
        x,
        y;

      unsigned int
        height,
        width;

      /*
        Reverse clip geometry.
      */
      (void) XParseGeometry(image_window->clip_geometry,&x,&y,&width,&height);
      (void) sprintf(image_window->clip_geometry,"%dx%d%+d%+d\0",width,height,
        (*image)->columns-width-x,y);
    }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X R o t a t e I m a g e W i n d o w                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XRotateImageWindow rotates the X image left or right 90 degrees.
%
%  The format of the XRotateImageWindow routine is:
%
%    status=XRotateImageWindow(display,info_window,image_window,degrees,image)
%
%  A description of each parameter follows:
%
%    o status: Function XRotateImageWindow return True if the window is
%      rotated.  False is returned is there is a memory shortage or if the
%      window fails to rotate.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o info_window: Specifies a pointer to a XWindowInfo structure.
%
%    o image_window: Specifies a pointer to a XWindowInfo structure.
%
%    o degrees: Specifies the number of degrees to rotate the image.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int XRotateImageWindow(display,info_window,image_window,
  degrees,image)
Display
  *display;

XWindowInfo
  *info_window,
  *image_window;

unsigned int
  degrees;

Image
  **image;
{
  char
    text[1024];

  int
    x,
    y;

  Image
    *rotated_image;

  unsigned int
    state;

  state=DefaultState;
  if (((*image)->columns*(*image)->rows) > MinInfoSize)
    {
      /*
        Map info window.
      */
      (void) strcpy(text," Rotating image... ");
      XSetWindowExtents(info_window,text,2);
      XMapWindow(display,info_window->id);
      XDrawImageString(display,info_window->id,info_window->graphic_context,2,
        info_window->font_info->ascent+2,text,strlen(text));
      state|=InfoMappedState;
    }
  /*
    Rotate image.
  */
  XDefineCursor(display,image_window->id,image_window->busy_cursor);
  XFlush(display);
  rotated_image=RotateImage(*image,(double) degrees,True);
  XDefineCursor(display,image_window->id,image_window->cursor);
  if (state & InfoMappedState)
    XWithdrawWindow(display,info_window->id,info_window->screen);
  if (rotated_image == (Image *) NULL)
    return(False);
  DestroyImage(*image);
  *image=rotated_image;
  if (image_window->clip_geometry != (char *) NULL)
    {
      unsigned int
        height,
        width;

      /*
        Rotate clip geometry.
      */
      (void) XParseGeometry(image_window->clip_geometry,&x,&y,&width,&height);
      if (degrees < 180.0)
        (void) sprintf(image_window->clip_geometry,"%dx%d%+d%+d\0",height,
          width,(*image)->columns-height-y,x);
      else
        (void) sprintf(image_window->clip_geometry,"%dx%d%+d%+d\0",height,
          width,y,(*image)->rows-width-x);
    }
  return(True);
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
    *image_geometry,
    *option,
    *print_filename,
    *scale_geometry,
    *server_name,
    *window_id,
    *write_filename;

  Display
    *display;

  double
    gamma;

  int
    degrees;

  register int
    i;

  unsigned int
    compression,
    display_image,
    dither,
    enhance,
    gray,
    image_number,
    inverse,
    monochrome,
    noise,
    normalize,
    number_colors,
    reflect,
    scene,
    tree_depth,
    verbose;

  XResourceInfo
    resource_info;

  XrmDatabase
    resource_database;

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
  compression=UnknownCompression;
  degrees=0;
  display=(Display *) NULL;
  display_image=True;
  dither=False;
  enhance=False;
  gamma=0.0;
  gray=False;
  image_geometry=(char *) NULL;
  inverse=False;
  monochrome=False;
  noise=False;
  normalize=False;
  number_colors=0;
  print_filename=(char *) NULL;
  reflect=False;
  resource_database=(XrmDatabase) NULL;
  scale_geometry=(char *) NULL;
  scene=0;
  server_name=(char *) NULL;
  tree_depth=0;
  verbose=False;
  window_id=(char *) NULL;
  write_filename=(char *) NULL;
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
          display_image=(*option == '-');
          if (display_image)
            {
              i++;
              if (i == argc)
                Usage("missing server name on -display",True);
              server_name=argv[i];
            }
          break;
        }
  }
  if (display_image)
    {
      char
        *resource_value;

      XrmDatabase
        server_database;

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
      XGetDefault(display,application_name,"dummy");
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
      clip_geometry=XGetResource(resource_database,application_name,
        "clipGeometry","ClipGeometry",(char *) NULL);
      resource_value=XGetResource(resource_database,application_name,"colors",
        (char *) NULL,"0");
      number_colors=atoi(resource_value);
      resource_value=XGetResource(resource_database,application_name,
        "compression",(char *) NULL,"UnknownCompression");
      if (*resource_value == 'R')
        compression=RunlengthEncodedCompression;
      else
        if (*resource_value == 'Q')
          compression=QEncodedCompression;
        else
           compression=UnknownCompression;
      resource_value=XGetResource(resource_database,application_name,"dither",
        (char *) NULL,"False");
      dither=IsTrue(resource_value);
      resource_value=XGetResource(resource_database,application_name,"enhance",
        (char *) NULL,"False");
      enhance=IsTrue(resource_value);
      resource_value=XGetResource(resource_database,application_name,"gamma",
        (char *) NULL,"0.0");
      gamma=atof(resource_value);
      resource_value=XGetResource(resource_database,application_name,"gray",
        (char *) NULL,"False");
      gray=IsTrue(resource_value);
      image_geometry=XGetResource(resource_database,application_name,
        "imageGeometry","ImageGeometry",(char *) NULL);
      resource_value=XGetResource(resource_database,application_name,"inverse",
        (char *) NULL,"False");
      inverse=IsTrue(resource_value);
      resource_value=XGetResource(resource_database,application_name,
        "monochrome",(char *) NULL,"False");
      monochrome=IsTrue(resource_value);
      resource_value=XGetResource(resource_database,application_name,"noise",
        (char *) NULL,"False");
      noise=IsTrue(resource_value);
      resource_value=XGetResource(resource_database,application_name,
        "normalize",(char *) NULL,"False");
      normalize=IsTrue(resource_value);
      print_filename=XGetResource(resource_database,application_name,
        "printFilename","PrintFilename",(char *) NULL);
      resource_value=XGetResource(resource_database,application_name,"reflect",
        (char *) NULL,"False");
      reflect=IsTrue(resource_value);
      resource_value=XGetResource(resource_database,application_name,"rotate",
        (char *) NULL,"0");
      degrees=atoi(resource_value);
      scale_geometry=XGetResource(resource_database,application_name,
        "scaleGeometry","ScaleGeometry",(char *) NULL);
      resource_value=XGetResource(resource_database,application_name,"scene",
        (char *) NULL,"0");
      scene=atoi(resource_value);
      resource_value=XGetResource(resource_database,application_name,
        "treeDepth","TreeDepth","0");
      tree_depth=atoi(resource_value);
      resource_value=XGetResource(resource_database,application_name,"verbose",
        (char *) NULL,"False");
      verbose=IsTrue(resource_value);
      window_id=XGetResource(resource_database,application_name,"windowId",
        "WindowId",(char *) NULL);
      write_filename=XGetResource(resource_database,application_name,
        "writeFilename","WriteFilename",(char *) NULL);
    }
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
            if (strncmp("color",option+1,3) == 0)
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
              if (strncmp("compress",option+1,3) == 0)
                {
                  compression=NoCompression;
                  if (*option == '-')
                    {
                      i++;
                      if (i == argc)
                        Usage("missing type on -compress",True);
                      if ((*argv[i] == 'R') || (*argv[i] == 'r'))
                        compression=RunlengthEncodedCompression;
                      else
                        if ((*argv[i] == 'Q') || (*argv[i] == 'q'))
                          compression=QEncodedCompression;
                        else
                          Usage("invalid compression type on -compress",True);
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
        case 'e':
        {
          enhance=(*option == '-');
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
                image_geometry=(char *) NULL;
                if (*option == '-')
                  {
                    i++;
                    if (i == argc)
                      Usage("missing geometry on -geometry",True);
                    image_geometry=argv[i];
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
              if (strncmp("inverse",option+1,2) == 0)
                inverse=(*option == '-');
              else
                Usage(option,True);
          break;
        }
        case 'm':
        {
          if (strncmp("magnify",option+1,3) == 0)
            {
              resource_info.magnify=2;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Usage("missing level on -magnify",True);
                  resource_info.magnify=Max(1 << atoi(argv[i]),2);
                }
            }
          else
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
                monochrome=(*option == '-');
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
            if (strncmp("noise",option+1,3) == 0)
              noise=(*option == '-');
            else
              if (strncmp("normalize",option+1,3) == 0)
                normalize=(*option == '-');
              else
                Usage(option,True);
          break;
        }
        case 'p':
        {
          if (strncmp("print",option+1,2) == 0)
            {
              print_filename=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Usage("missing file name on -print",True);
                  print_filename=argv[i];
                  if (access(print_filename,0) == 0)
                    {
                      char
                        answer[2];

                      (void) fprintf(stderr,"Overwrite %s? ",print_filename);
                      (void) gets(answer);
                      if (!((*answer == 'y') || (*answer == 'Y')))
                    exit(1);
                    }
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
            if (strncmp("rotate",option+1,2) == 0)
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
          if (strncmp("scale",option+1,3) == 0)
            {
              scale_geometry=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Usage("missing scale geometry on -scale",True);
                  scale_geometry=argv[i];
                }
            }
          else
            if (strncmp("scene",option+1,3) == 0)
              {
                scene=0;
                if (*option == '-')
                  {
                    i++;
                    if (i == argc)
                      Usage("missing scene number on -scene",True);
                    scene=atoi(argv[i]);
                  }
              }
            else
              Usage(option,True);
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
        case 'w':
        {
          if (strncmp("window",option+1,2) == 0)
            {
              window_id=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Usage("missing id, name, or 'root' on -window",True);
                  window_id=argv[i];
                }
            }
          else
            if (strncmp("write",option+1,2) == 0)
              {
                write_filename=(char *) NULL;
                if (*option == '-')
                  {
                    i++;
                    if (i == argc)
                      Usage("missing file name on -write",True);
                    write_filename=argv[i];
                    if (access(write_filename,0) == 0)
                      {
                        char
                          answer[2];

                        (void) fprintf(stderr,"Overwrite %s? ",write_filename);
                        (void) gets(answer);
                        if (!((*answer == 'y') || (*answer == 'Y')))
                          exit(1);
                      }
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
      do
      {
        Image
          *image,
          info_image;

        long
          start_time;

        unsigned int
          terminate;

        unsigned long
          total_colors;

        /*
          Option is a file name: begin by reading image from specified file.
        */
        terminate=window_id != (char *) NULL;
        image_number++;
        (void) strcpy(info_image.filename,option);
        start_time=time((long *) 0);
        image=ReadImage(info_image.filename);
        if (image == (Image *) NULL)
          if (*option == '-')
            break;
          else
            continue;
        info_image=(*image);
        if (scene > 0)
          image->scene=scene;
        /*
          Transform image as defined by the clip, image and scale geometries.
        */
        image=
          TransformImage(image,clip_geometry,image_geometry,scale_geometry);
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
        if (enhance)
          {
            Image
              *enhanced_image;

            /*
              Enhance image.
            */
            enhanced_image=EnhanceImage(image);
            if (enhanced_image != (Image *) NULL)
              {
                DestroyImage(image);
                image=enhanced_image;
              }
          }
        if (noise)
          {
            Image
              *noisy_image;

            /*
              Reduce noise in image.
            */
            noisy_image=NoisyImage(image);
            if (noisy_image != (Image *) NULL)
              {
                DestroyImage(image);
                image=noisy_image;
              }
          }
        if (gamma > 0.0)
          (void) GammaImage(image,gamma);
        if (inverse)
          (void) InverseImage(image);
        if (normalize)
          (void) NormalizeImage(image);
        if (verbose)
          {
            /*
              Initialize image error attributes.
            */
            if (image->class == DirectClass)
              image->colors=NumberColors(image);
            total_colors=image->colors;
          }
        if (gray)
          {
            /*
              Convert image to gray scale PseudoColor class.
            */
            (void) GrayImage(image);
            if (image->class == DirectClass)
              QuantizeImage(image,256,tree_depth,dither,True);
          }
        if (monochrome)
          {
            register unsigned int
              bit;

            /*
              Convert image to monochrome PseudoColor class.
            */
            (void) GrayImage(image);
            if ((image->class == DirectClass) || (image->colors > 2))
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
        if (display != (Display *) NULL)
          {
            /*
              Display image to X server.
            */
            if (compression != UnknownCompression)
              resource_info.compression=compression;
            else
              resource_info.compression=info_image.compression;
            resource_info.dither=dither;
            resource_info.monochrome=monochrome;
            resource_info.number_colors=number_colors;
            resource_info.image_geometry=image_geometry;
            resource_info.print_filename=print_filename;
            resource_info.tree_depth=tree_depth;
            resource_info.write_filename=write_filename;
            if (window_id != (char *) NULL)
              XDisplayBackgroundImage(display,&resource_info,window_id,image);
            else
              image=XDisplayImage(display,&resource_info,argv,argc,image,
                &terminate);
          }
        if (compression != UnknownCompression)
          image->compression=compression;
        else
          image->compression=info_image.compression;
        if (print_filename != (char *) NULL)
          {
            /*
              Print image as Encapsulated Postscript.
            */
            (void) strcpy(image->filename,print_filename);
            (void) PrintImage(image,image_geometry);
          }
        if (write_filename != (char *) NULL)
          {
            /*
              Write image in MIFF format.
            */
            (void) strcpy(image->filename,write_filename);
            (void) WriteImage(image);
          }
        if (verbose)
          {
            /*
              Display detailed info about the image.
            */
            (void) fprintf(stderr,"[%d] %s",(image->scene == 0 ? image_number :
              image->scene),info_image.filename);
            if (write_filename != (char *) NULL)
              (void) fprintf(stderr,"=>%s",write_filename);
            (void) fprintf(stderr," %dx%d",info_image.columns,info_image.rows);
            if ((info_image.columns != image->columns) ||
                (info_image.rows != image->rows))
              (void) fprintf(stderr,"=>%dx%d",image->columns,image->rows);
            if (image->class == DirectClass)
              (void) fprintf(stderr," DirectClass ");
            else
              (void) fprintf(stderr," PseudoClass ");
            if (total_colors != image->colors)
              (void) fprintf(stderr,"%d=>",total_colors);
            (void) fprintf(stderr,"%dc",image->colors);
            if ((number_colors > 0) || gray || monochrome)
              {
                double
                  normalized_maximum_error,
                  normalized_mean_error;

                unsigned int
                  mean_error_per_pixel;

                /*
                  Measure quantization error.
                */
                QuantizationError(image,&mean_error_per_pixel,
                  &normalized_mean_error,&normalized_maximum_error);
                (void) fprintf(stderr," %d/%.6f/%.6fe",mean_error_per_pixel,
                  normalized_mean_error,normalized_maximum_error);
              }
            (void) fprintf(stderr," %ds\n",time((long *) 0)-start_time+1);
          }
        /*
          Deallocate image resources.
        */
        DestroyImage(image);
        if (terminate)
          {
            /*
              User requests we quit.
            */
            if (resource_database != (XrmDatabase) NULL)
              XrmDestroyDatabase(resource_database);
            if (display != (Display *) NULL)
              XCloseDisplay(display);
            return(False);
          }
      }
      while (*option == '-');
    }
  if (image_number == 0)
    Usage("missing an image file name",True);
  if (resource_database != (XrmDatabase) NULL)
    XrmDestroyDatabase(resource_database);
  if (display != (Display *) NULL)
    XCloseDisplay(display);
  return(False);
}
