/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                 IIIII  M   M  PPPP    OOO   RRRR    TTTTT                   %
%                   I    MM MM  P   P  O   O  R   R     T                     %
%                   I    M M M  PPPP   O   O  RRRR      T                     %
%                   I    M   M  P      O   O  R R       T                     %
%                 IIIII  M   M  P       OOO   R  R      T                     %
%                                                                             %
%                                                                             %
%             Import X11 image to a machine independent format.               %
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
%  Import is an X Window System window dumping utility.  Import allows X
%  users to store window images in a specially formatted dump file.  This
%  file can then be read by the Display utility for redisplay, printing,
%  editing, formatting, archiving, image processing, etc.  The target
%  window can be specified by id or name or be selected by clicking the
%  mouse in the desired window.  The keyboard bell is rung once at the
%  beginning of the dump and twice when the dump is completed.
%
%  The import program command syntax is:
%
%  Usage: import [options ...] file
%
%  Where options include:
%    -border          include image borders in the output image
%    -display server  X server to contact
%    -frame           include window manager frame
%    -monochrome      transform image to black and white
%    -scene           image scene number
%    -screen          select image from root window
%    -window id       select window with this id or name
%
%  Change '-' to '+' in any option above to reverse its effect.
%  For example, +frame means do not window manager frame.
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
%   U s a g e                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure Usage displays the program usage;
%
%  The format of the Usage routine is:
%
%      Usage(message)
%
%  A description of each parameter follows:
%
%    message:Specifies a specific message to display to the user.
%
*/
static void Usage(message)
char
  *message;
{
  char
    **p;

  static char
    *options[]=
    {
      "-border          include image borders in the output image",
      "-display server  X server to contact",
      "-frame           include window manager frame",
      "-monochrome      transform image to black and white",
      "-scene           image scene number",
      "-screen          select image from root window",
      "-window id       select window with this id or name",
      (char *) NULL
    };
  if (message != (char *) NULL)
    (void) fprintf(stderr,"Can't continue, %s.\n\n",message);
  (void) fprintf(stderr,"Usage: %s [options ...] file\n",application_name);
  (void) fprintf(stderr,"\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) fprintf(stderr,"  %s\n",*p);
  (void) fprintf(stderr,
    "\nChange '-' to '+' in any option above to reverse its effect.\n");
  (void) fprintf(stderr,
    "For example, +frame means do not include window manager frame.\n");
  (void) fprintf(stderr,"\nSpecify 'file' as '-' for standard output.\n");
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
  *argv[];
{
  char
    *filename,
    *option,
    *resource_value,
    *server_name;

  Display
    *display;

  Image
    *image;

  int
    i;

  unsigned int
    borders,
    frame,
    j,
    monochrome,
    scene,
    screen;

  Window
    client_window,
    root_window,
    target_window;

  XrmDatabase
    resource_database,
    server_database;

  /*
    Display usage profile if there are no command line arguments.
  */
  application_name=(*argv);
  if (argc < 2)
    Usage((char *) NULL);
  /*
    Connect to X server.
  */
  server_name=(char *) NULL;
  for (i=1; i < argc; i++)
  {
    /*
      Check command line for server name.
    */
    option=argv[i];
    if ((strlen(option) > 1) && ((*option == '-') || (*option == '+')))
      if (strncmp("dis",option+1,3) == 0)
        {
          /*
            User specified server name.
          */
          i++;
          if (i == argc)
            Usage("missing server name on -display");
          server_name=argv[i];
          break;
        }
  }
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
  resource_value=XGetResource(resource_database,application_name,"borders",
    (char *) NULL,"False");
  borders=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,application_name,"frame",
    (char *) NULL,"False");
  frame=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,application_name,"monochrome",
    (char *) NULL,"False");
  monochrome=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,application_name,"scene",
    (char *) NULL,"False");
  scene=atoi(resource_value);
  resource_value=XGetResource(resource_database,application_name,"screen",
    (char *) NULL,"False");
  screen=IsTrue(resource_value);
  /*
    Check command syntax.
  */
  filename=(char *) NULL;
  root_window=XRootWindow(display,XDefaultScreen(display));
  target_window=(Window) NULL;
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) < 2) || ((*option != '-') && (*option != '+')))
      filename=argv[i];
    else
      switch(*(option+1))
      {
        case 'b':
        {
          borders=(*option == '-');
          break;
        }
        case 'd':
        {
          /*
            User specified server name.
          */
          i++;
          if (i == argc)
            Usage("missing server name on -display");
          server_name=argv[i];
          break;
        }
        case 'h':
        {
          Usage((char *) NULL);
          break;
        }
        case 'f':
        {
          frame=(*option == '-');
          break;
        }
        case 'm':
        {
          monochrome=(*option == '-');
          break;
        }
        case 's': 
        {
          if (strncmp("scene",option+1,4) == 0)
            {
              i++;
              if (i == argc)
                Usage("missing scene on -scene");
              scene=atoi(argv[i]);
            }
          else
            if (strncmp("screen",option+1,4) == 0)
              screen=(*option == '-');
            else
              Usage((char *) NULL);
          break;
        }
        case 'w':
        {
          i++;
          if (i == argc)
            Usage("missing id, name, or 'root' on -window");
          option=argv[i];
          if (strcmp("root",option) == 0)
            target_window=root_window;
          else
            {
              if (isdigit(*option))
                target_window=XWindowByID(display,root_window,
                  (Window) strtol(option,(char **) NULL,0));
              if (target_window == (Window) NULL)
                target_window=XWindowByName(display,root_window,option);
              if (target_window == (Window) NULL)
                Warning("No window with specified id exists",option);
            }
          break;
        }
        default:
          Usage((char *) NULL);
      }
  }
  if (filename == (char *) NULL)
    Usage("missing an image file name");
  /*
    If the window is not specified, let the user choose one with the mouse.
  */
  if (target_window == (Window) NULL)
    target_window=XSelectWindow(display,root_window);
  client_window=target_window;
  if (target_window != root_window)
    if (XGetGeometry(display,target_window,&root_window,&i,&i,&j,&j,&j,&j) != 0)
      {
        /*
          Get client window.
        */
        client_window=XClientWindow(display,target_window);
        if (!frame)
          target_window=client_window;
      }
  image=XReadImage(display,root_window,target_window,client_window,screen,
    borders);
  if (image == (Image *) NULL)
    exit(1);
  image->scene=scene;
  if (monochrome)
    {
      register unsigned int
        bit;

      /*
        Convert image to monochrome PseudoColor class.
      */
      (void) QuantizeImage(image,2,0,False,True);
      bit=Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
      image->colormap[bit].red=0;
      image->colormap[bit].green=0;
      image->colormap[bit].blue=0;
      image->colormap[!bit].red=255;
      image->colormap[!bit].green=255;
      image->colormap[!bit].blue=255;
    }
  (void) strcpy(image->filename,filename);
  (void) fprintf(stderr,"[%d] %s %dx%d\n",image->scene,image->filename,
    image->columns,image->rows);
  (void) WriteImage(image);
  DestroyImage(image);
  XrmDestroyDatabase(resource_database);
  XCloseDisplay(display);
  return(False);
}
