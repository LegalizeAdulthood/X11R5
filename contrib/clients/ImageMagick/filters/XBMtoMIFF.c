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
%                  Import X bitmap image to a MIFF format.                    %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              July 1991                                      %
%                                                                             %
%                                                                             %
%  Copyright 1991 E. I. Dupont de Nemours & Company                           %
%                                                                             %
%  Permission to use, copy, modify, distribute, and sell this software and    %
%  its documentation for any purpose is hereby granted without fee,           %
%  provided that the above Copyright notice appear in all copies and that     %
%  both that Copyright notice and this permission notice appear in            %
%  supporting documentation, and that the name of E. I. Dupont de Nemours     %
%  & Company not be used in advertising or publicity pertaining to            %
%  distribution of the software without specific, written prior               %
%  permission.  E. I. Dupont de Nemours & Company makes no representations    %
%  about the suitability of this software for any purpose.  It is provided    %
%  "as is" without express or implied warranty.                               %
%                                                                             %
%  E. I. Dupont de Nemours & Company disclaims all warranties with regard     %
%  to this software, including all implied warranties of merchantability      %
%  and fitness, in no event shall E. I. Dupont de Nemours & Company be        %
%  liable for any special, indirect or consequential damages or any           %
%  damages whatsoever resulting from loss of use, data or profits, whether    %
%  in an action of contract, negligence or other tortious action, arising     %
%  out of or in connection with the use or performance of this software.      %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 
%  Command syntax:
%
%  import [-scene #] image.xbm image.miff
%  
%  Specify 'image.xbm' as '-' for standard input.  
%  Specify 'image.miff' as '-' for standard output.
%  
%
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
%    o message:  Specifies the message to display before terminating the
%      program.
%
%    o qualifier:  Specifies any qualifier to the message.
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
    (void) fprintf(stderr," %s",qualifier);
  (void) fprintf(stderr,".\n");
  exit(1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d X B M I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadImage reads an image file and returns it.  It allocates the 
%  memory necessary for the new Image structure and returns a pointer to the 
%  new image.
%
%  The format of the ReadImage routine is:
%
%      image=ReadImage(filename)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadImage returns a pointer to the image after 
%      reading.  A null image is returned if there is a a memory shortage or 
%      if the image cannot be read.
%
%    o filename:  Specifies the name of the image to read.
%
%
*/
static Image *ReadXBMImage(filename)
char
  *filename;
{
#define MaxLineLength  256

  char
    data[MaxLineLength];

  Image
    *image;

  int
    byte;

  register int
    x,
    y;

  register RunlengthPacket
    *p;

  register unsigned char
    bit;

  unsigned int
    value;

  /*
    Allocate image structure.
  */
  image=AllocateImage();
  if (image == (Image *) NULL)
    Error("memory allocation error",(char *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,filename);
  if (*image->filename == '-')
    image->file=stdin;
  else
    if (strcmp(image->filename+strlen(image->filename)-2,".Z") != 0)
      image->file=fopen(image->filename,"r");
    else
      {
        char
          command[256];

        /*
          Image file is compressed-- uncompress it.
        */
        (void) sprintf(command,"uncompress -c %s",image->filename);
        image->file=(FILE *) popen(command,"r");
      }
  if (image->file == (FILE *) NULL)
    Error("unable to open file",image->filename);
  /*
    Read X bitmap header.
  */
  while (!feof(image->file))
  {
    (void) fgets(data,MaxLineLength,image->file);
    if (sscanf(data,"#define %*32[a-zA-Z0-9]_width %d",&value) == 1)
      image->columns=value;
    if (sscanf(data,"#define %*32[a-zA-Z0-9]_height %d",&value) == 1)
      image->rows=value;
    if (sscanf(data,"%*[^#] char"))
      break;
  }
  if ((image->columns == 0) || (image->rows == 0))
    Error("XBM file is not in the correct format",image->filename);
  /*
    Create image.
  */
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *) 
    malloc(image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    Error("memory allocation error",(char *) NULL);
  /*
    Create colormap.
  */
  image->colors=2;
  image->colormap=(ColorPacket *) malloc(image->colors*sizeof(ColorPacket));
  if (image->colormap == (ColorPacket *) NULL)
    Error("memory allocation error",(char *) NULL);
  image->colormap[0].red=0;
  image->colormap[0].green=0;
  image->colormap[0].blue=0;
  image->colormap[1].red=255;
  image->colormap[1].green=255;
  image->colormap[1].blue=255;
  /*
    Initial image comment.
  */
  image->comments=(char *) malloc((unsigned int) strlen(image->filename)+256);
  if (image->comments == (char *) NULL)
    Error("unable to allocate memory",(char *) NULL);
  (void) sprintf(image->comments,"\n  Imported from X11 bitmap file:  %s\n\0",
    image->filename);
  /*
    Convert X bitmap image to runlength-encoded packets.
  */
  p=image->pixels;
  for (y=0; y < image->rows; y++)
  {
    bit=0;
    for (x=0; x < image->columns; x++)
    {
      if (bit == 0)
        (void) fscanf(image->file,"%i,",&byte);
      p->index=(byte & 0x01) ? 0 : 1;
      byte>>=1;
      p->red=image->colormap[p->index].red;
      p->green=image->colormap[p->index].green;
      p->blue=image->colormap[p->index].blue;
      p->length=0;
      p++;
      bit++;
      if (bit == 8)
        bit=0;
    }
  }
  if (image->file != stdin)
    if (strcmp(image->filename+strlen(image->filename)-2,".Z") != 0)
      (void) fclose(image->file);
    else
      (void) pclose(image->file);
  return(image);
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
%  Function Usage displays the program usage;
%
%  The format of the Usage routine is:
%
%      Usage(message)
%
%  A description of each parameter follows:
%
%    message:  Specifies a specific message to display to the user.
%
*/
static void Usage(message)
char
  *message;
{
  if (message != (char *) NULL)
    (void) fprintf(stderr,"Can't continue, %s\n\n",message);
  (void) fprintf(stderr,
    "Usage: %s [-scene] width height image.xbm image.miff\n",application_name);
  (void) fprintf(stderr,"\nSpecify 'image.xbm' as '-' for standard input.\n");
  (void) fprintf(stderr,"Specify 'image.miff' as '-' for standard output.\n");
  exit(1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M a i n                                                                    %
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
    filename[256];

  Image
    *image;

  int
    i;

  unsigned int
    scene;

  /*
    Initialize program variables.
  */
  application_name=argv[0];
  i=1;
  scene=0;
  if (argc < 3)
    Usage((char *) NULL);
  /*
    Read image and convert to MIFF format.
  */
  if (strncmp(argv[i],"-scene",2) == 0)
    {
      i++;
      scene=atoi(argv[i++]);
    }
  (void) strcpy(filename,argv[i++]);
  image=ReadXBMImage(filename);
  if (image == (Image *) NULL)
    exit(1);
  (void) strcpy(image->filename,argv[i++]);
  image->scene=scene;
  (void) WriteImage(image);
  (void) fprintf(stderr,"%s => %s  %dx%d\n",filename,image->filename,
    image->columns,image->rows);
  DestroyImage(image);
  return(False);
}
