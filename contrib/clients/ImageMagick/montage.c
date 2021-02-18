/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%             M   M   OOO   N   N  TTTTT   AAA    GGGG  EEEEE                 %
%             MM MM  O   O  NN  N    T    A   A  G      E                     %
%             M M M  O   O  N N N    T    AAAAA  G  GG  EEE                   %
%             M   M  O   O  N  NN    T    A   A  G   G  E                     %
%             M   M   OOO   N   N    T    A   A   GGGG  EEEEE                 %
%                                                                             %
%                                                                             %
%          Montage Machine Independent File Format Image via X11.             %
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
%  Montage creates a composite image by combining several separate
%  images.  The images are tiled on the composite image with the name of
%  the image appearing just above the individual tile.
%
%  The composite image is constructed in the following manner.  First,
%  each image specified on the command line, except for the last, is
%  scaled to fit the maximum tile size.  The maximum tile size by default
%  is 256x256.  It can be modified with the -geometry command line
%  argument or X resource.  Note that the maximum tile size need not be a
%  square.  The original aspect ratio of each image is maintainted.
%
%  Next the composite image is initialized with the color specified by the
%  -bordercolor command line argument or X resource.  The width and height
%  of the composite image is determined by the maximum tile size, the
%  number of tiles per row, the border width and height, and the label
%  height.  The number of tiles per row specifies how many images are to
%  appear in each row of the composite image.  The default is to have an
%  equal number of images in each row and column of the composite.  This
%  value can be specified with -tiles_per_row.  The border width and
%  height defaults to the value of the X resource -borderwidth.  It can be
%  changed with the -borderwidth or -geometry command line argument or X
%  resource.  The label height is determined by the font you specify with
%  the -font command line argument or X resource.  If you do not specify a
%  font, a font is choosen that allows the name of the image to fit the
%  maximum width of a tiled area.  The label colors is determined by the
%  -background and -foreground command line argument or X resource.  Note,
%  that if the background and foreground colors are the same, labels will
%  not appear.
%
%  Finally, each image is set onto the composite image with its name
%  centered just above it.  The individual images are centered within the
%  width of the tiled area.  The final argument on the command line is the
%  name assigned to the composite image.  The image is written in the MIFF
%  format and may by viewed or printed with display.
%
%  The Montage program command syntax is:
%
%  Usage: montage [options ...] file [ [options ...] file ...] file
%
%  Where options include:
%    -clip geometry       preferred size and location of the clipped image
%    -colors value        preferred number of colors in the image
%    -compress type       compress image: RunlengthEncoded or QEncoded
%    -display server      query fonts from this X server
%    -dither              apply Floyd/Steinberg error diffusion to image
%    -gamma value         level of gamma correction
%    -geometry geometry   preferred tile and border sizes
%    -gray                transform image to gray scale colors
%    -monochrome          transform image to black and white
%    -reflect             reverse image scanlines
%    -rotate degrees      apply Paeth rotation to the image
%    -tiles_per_row value number of image tiles per row
%    -treedepth value     depth of the color classification tree
%    -verbose             print detailed information about the image
%
%  In addition to those listed above, you can specify these standard X
%  resources as command line options:  -background, -bordercolor -borderwidth,
%  -font, or -foreground.
%
%  Change '-' to '+' in any option above to reverse its effect.  For
%  example, specify +compress to store the image as uncompressed.
%
%  Specify 'file' as '-' for standard input or output.
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
%   M o n t a g e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function MontageImage creates a composite image by combining several
%  separate images.
%
%  The format of the MontageImage routine is:
%
%      MontageImage(display,resource_info,tiles_per_row,image,number_tiles)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o tiles_per_row: Specifies the number of arguments.
%
%    o image: Specifies a pointer to a Image structure; returned from
%      ReadImage.
%
%    o number_tiles: Specifies the number of tiles to tile.
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

static Image *MontageImage(display,resource_info,tiles_per_row,images,
  number_tiles)
Display
  *display;

XResourceInfo
  *resource_info;

unsigned int
  tiles_per_row;

Image
  **images;

unsigned int
  number_tiles;
{
  char
    annotate_geometry[1024],
    text[1024];

  Image
    *montage_image;

  int
    tile_border_width,
    tile_border_height;

  register int
    i,
    x,
    y;

  register RunlengthPacket
    *p,
    *q;

  unsigned int
    label_height,
    tile,
    tile_height,
    tile_width,
    x_offset,
    y_offset;

  XColor
    border_color;

  XWindowInfo
    image_window;

  /*
    Determine tile sizes.
  */
  tile_border_width=resource_info->border_width;
  tile_border_height=resource_info->border_width;
  tile_width=256;
  tile_height=256;
  if (resource_info->image_geometry != (char *) NULL)
    XParseGeometry(resource_info->image_geometry,&tile_border_width,
      &tile_border_height,&tile_width,&tile_height);
  if (tiles_per_row == 0)
    tiles_per_row=(unsigned int) (sqrt((double) number_tiles)+0.5);
  if (!display)
    {
      /*
        No X server-- no labels.
      */
      label_height=0;
      border_color.red=0;
      border_color.green=0;
      border_color.blue=0;
    }
  else
    {
      XFontStruct
        *font_info;

      XPixelInfo
        pixel_info;

      XVisualInfo
        *visual_info;

      /*
        Initialize visual info.
      */
      visual_info=XBestVisualInfo(display,"default",(char *) NULL,
        (XStandardColormap *) NULL);
      if (visual_info == (XVisualInfo *) NULL)
         Error("unable to get visual",resource_info->visual_type);
      /*
        Initialize font info.
      */
      tile=0;
      for (i=1; i < number_tiles; i++)
        if (strlen(images[i]->filename) > strlen(images[tile]->filename))
          tile=i;
      (void) sprintf(text," %s \0",images[tile]->filename);
      font_info=XBestFont(display,resource_info,text,tile_width);
      if (font_info == (XFontStruct *) NULL)
        Error("unable to load font",resource_info->font);
      label_height=font_info->ascent+font_info->descent;
      /*
        Determine background, border, and foreground colors.
      */
      XParseColor(display,XDefaultColormap(display,visual_info->screen),
        resource_info->background_color,&pixel_info.background_color);
      XParseColor(display,XDefaultColormap(display,visual_info->screen),
        resource_info->border_color,&pixel_info.border_color);
      border_color=pixel_info.border_color;
      XParseColor(display,XDefaultColormap(display,visual_info->screen),
        resource_info->foreground_color,&pixel_info.foreground_color);
      pixel_info.annotate_color=pixel_info.foreground_color;
      /*
        Window superclass.
      */
      image_window.id=XRootWindow(display,visual_info->screen);
      image_window.screen=visual_info->screen;
      image_window.depth=visual_info->depth;
      image_window.visual_info=visual_info;
      image_window.pixel_info=(&pixel_info);
      image_window.font_info=font_info;
    }
  /*
    Allocate image structure.
  */
  montage_image=(Image *) malloc(sizeof(Image));
  if (montage_image == (Image *) NULL)
    Error("memory allocation error",(char *) NULL);
  /*
    Initialize Image structure.
  */
  montage_image->class=DirectClass;
  montage_image->compression=NoCompression;
  montage_image->columns=(tile_width+tile_border_width*2)*tiles_per_row;
  montage_image->rows=(tile_height+tile_border_height*2+label_height)*
    (number_tiles/tiles_per_row+((number_tiles % tiles_per_row) != 0))+
    (tile_border_height >> 1);
  montage_image->packets=montage_image->columns*montage_image->rows;
  montage_image->colors=0;
  montage_image->scene=0;
  montage_image->colormap=(ColorPacket *) NULL;
  montage_image->pixels=(RunlengthPacket *) NULL;
  montage_image->comments=(char *) NULL;
  montage_image->pixels=(RunlengthPacket *)
    malloc(montage_image->packets*sizeof(RunlengthPacket));
  if (montage_image->pixels == (RunlengthPacket *) NULL)
    Error("memory allocation error",(char *) NULL);
  /*
    Initialize montage image to border color.
  */
  p=montage_image->pixels;
  for (i=0; i < montage_image->packets; i++)
  {
    p->red=border_color.red >> 8;
    p->green=border_color.green >> 8;
    p->blue=border_color.blue >> 8;
    p->index=0;
    p->length=0;
    p++;
  }
  /*
    Sort images by increasing tile number.
  */
  qsort((char *) images,(int) number_tiles,sizeof(Image *),LinearCompare);
  /*
    Copy tile images to the composite image.
  */
  for (tile=0; tile < number_tiles; tile++)
  {
    x_offset=(tile_width+tile_border_width*2)*(tile % tiles_per_row)+
      tile_border_width;
    y_offset=(tile_height+tile_border_width*2)*(tile/tiles_per_row)+
      (tile_border_height >> 1);
    if (display)
      {
        unsigned int
          label_height,
          label_width;

        /*
          Copy tile label to the composite image.
        */
        (void) sprintf(text," %s \0",images[tile]->filename);
        label_width=XTextWidth(image_window.font_info,text,strlen(text));
        label_height=image_window.font_info->ascent+
          image_window.font_info->descent;
        y_offset+=label_height*(tile/tiles_per_row);
        (void) sprintf(annotate_geometry,"%dx%d%+d%+d\0",label_width,
          label_height,x_offset+((tile_width-label_width) >> 1),y_offset);
        XAnnotateImage(display,image_window.font_info,&image_window,
          text,annotate_geometry,True,montage_image);
      }
    /*
      Copy this tile to the composite image.
    */
    p=images[tile]->pixels;
    images[tile]->runlength=p->length+1;
    y_offset+=tile_border_height+label_height;
    q=montage_image->pixels+(y_offset*montage_image->columns)+x_offset+
      ((tile_width-images[tile]->columns) >> 1);
    for (y=0; y < images[tile]->rows; y++)
    {
      for (x=0; x < images[tile]->columns; x++)
      {
        if (images[tile]->runlength > 0)
          images[tile]->runlength--;
        else
          {
            p++;
            images[tile]->runlength=p->length;
          }
        q->red=p->red;
        q->green=p->green;
        q->blue=p->blue;
        q++;
      }
      q+=montage_image->columns-images[tile]->columns;
    }
    DestroyImage(images[tile]);
  }
  if (display)
    {
      /*
        Free X resources.
      */
      XFreeFont(display,image_window.font_info);
      XFree((void *) image_window.visual_info);
    }
  return(montage_image);
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
    *options[]=
    {
      "-clip geometry       preferred size and location of the clipped image",
      "-colors value        preferred number of colors in the image",
      "-compress type       compress image: RunlengthEncoded or QEncoded",
      "-display server      query font from this X server",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-gamma value         level of gamma correction",
      "-geometry geometry   preferred size and location of the image window",
      "-gray                transform image to gray scale colors",
      "-monochrome          transform image to black and white",
      "-reflect             reflect the image scanlines",
      "-rotate degrees      apply Paeth rotation to the image",
      "-tiles_per_row value number of image tiles per row",
      "-treedepth value     depth of the color classification tree",
      "-verbose             print detailed information about the image",
      (char *) NULL
    };
  if (message != (char *) NULL)
    (void) fprintf(stderr,"Can't continue, %s\n\n",message);
  (void) fprintf(stderr,
    "Usage: %s [-options ...] file [ [-options ...] file ...] file\n",
    application_name);
  (void) fprintf(stderr,"\nWhere options include: \n");
  for (p=options; *p != (char *) NULL; p++)
    (void) fprintf(stderr,"  %s\n",*p);
  (void) fprintf(stderr,
    "\nIn addition to those listed above, you can specify these standard X\n");
  (void) fprintf(stderr,
    "resources as command line options:  -background, -bordercolor,\n");
  (void) fprintf(stderr,
    "-borderwidth, -font, or -foreground\n");
  (void) fprintf(stderr,
    "\nChange '-' to '+' in any option above to reverse its effect.  For\n");
  (void) fprintf(stderr,
    "example, specify +compress to store the image as uncompressed.\n");
  (void) fprintf(stderr,
    "\nSpecify 'file' as '-' for standard input or output.\n");
  if (terminate)
    exit(1);
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
    *server_name,
    *write_filename;

  Display
    *display;

  double
    gamma;

  Image
    **images,
    *montage_image;

  int
    degrees;

  register int
    i;

  unsigned int
    compression,
    display_image,
    dither,
    gray,
    image_number,
    number_colors,
    reflect,
    tiles_per_row,
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
  if (argc < 3)
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
  gamma=0.0;
  gray=False;
  number_colors=0;
  reflect=False;
  resource_database=(XrmDatabase) NULL;
  resource_info.border_width=0;
  resource_info.image_geometry=(char *) NULL;
  resource_info.monochrome=False;
  server_name=(char *) NULL;
  tiles_per_row=0;
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
      resource_value=XGetResource(resource_database,application_name,
        "treeDepth","TreeDepth","0");
      tree_depth=atoi(resource_value);
      resource_value=XGetResource(resource_database,application_name,"verbose",
        (char *) NULL,"False");
      verbose=IsTrue(resource_value);
    }
  /*
    Composite image is the last item on the command line.
  */
  write_filename=argv[argc-1];
  if (access(write_filename,0) == 0)
    {
      char
        answer[2];

      (void) fprintf(stderr,"Overwrite %s? ",write_filename);
      (void) gets(answer);
      if (!((*answer == 'y') || (*answer == 'Y')))
        exit(1);
    }
  /*
    Parse command line.
  */
  image_number=0;
  for (i=1; i < (argc-1); i++)
  {
    option=argv[i];
    if ((strlen(option) > 1) && ((*option == '-') || (*option == '+')))
      switch (*(option+1))
      {
        case 'b':
        {
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
        case 'm':
        {
          resource_info.monochrome=(*option == '-');
          break;
        }
        case 'r':
        {
          if (strncmp("reflect",option+1,2) == 0)
            reflect=(*option == '-');
          else
            if (strncmp("rotate",option+1,3) == 0)
              {
                degrees=0.0;
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
        case 't':
        {
          if (strncmp("tiles_per_row",option+1,2) == 0)
            {
              tiles_per_row=0;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Usage("missing value on -tiles_per_row",True);
                  tree_depth=atoi(argv[i]);
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
          verbose=(*option == '-');
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
        char
          geometry[1024];

        Image
          *image,
          info_image;

        unsigned int
          scale_factor,
          tile_height,
          tile_width;

        /*
          Option is a file name: begin by reading image from specified file.
        */
        image=ReadImage(option);
        if (image == (Image *) NULL)
          if (*option == '-')
            break;
          else
            continue;
        if (image->scene == 0)
          image->scene=image_number;
        info_image=(*image);
        /*
          Tile size maintains the aspect ratio of the image.
        */
        tile_width=256;
        tile_height=256;
        if (resource_info.image_geometry != (char *) NULL)
          {
            int
              x,
              y;

            XParseGeometry(resource_info.image_geometry,&x,&y,&tile_width,
              &tile_height);
            if ((tile_width == 0) || (tile_height == 0))
              Error("invalid geometry specifier",resource_info.image_geometry);
          }
        scale_factor=(tile_width << 14)/image->columns;
        if (scale_factor > ((tile_height << 14)/image->rows))
          scale_factor=(tile_height << 14)/image->rows;
        tile_width=(image->columns*scale_factor+8191) >> 14;
        tile_height=(image->rows*scale_factor+8191) >> 14;
        (void) sprintf(geometry,"%dx%d\0",tile_width,tile_height);
        image=TransformImage(image,clip_geometry,geometry,(char *) NULL);
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
            (void) fprintf(stderr,"\n");
          }
        images[image_number++]=image;
      }
      while (*option == '-');
    }
  if (image_number == 0)
    Usage("missing an image file name",True);
  else
    {
      montage_image=MontageImage(display,&resource_info,tiles_per_row,images,
        image_number);
      if (number_colors > 0)
        QuantizeImage(montage_image,number_colors,tree_depth,dither,True);
      if (compression != UnknownCompression)
        montage_image->compression=compression;
      strcpy(montage_image->filename,write_filename);
      (void) WriteImage(montage_image);
      (void) free((char *) images);
    }
  if (resource_database != (XrmDatabase) NULL)
    XrmDestroyDatabase(resource_database);
  if (display != (Display *) NULL)
    XCloseDisplay(display);
  return(False);
}
