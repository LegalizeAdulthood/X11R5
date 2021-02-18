/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                 EEEEE  X   X  PPPP    OOO   RRRR    TTTTT                   %
%                 E       X X   P   P  O   O  R   R     T                     %
%                 EEE      X    PPPP   O   O  RRRR      T                     %
%                 E       X X   P      O   O  R R       T                     %
%                 EEEEE  X   X  P       OOO   R  R      T                     %
%                                                                             %
%                                                                             %
%                 Export MIFF image to a GRAY raster format.                  %
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
%  export image.miff image.gray
%
%  Specify 'image.miff' as '-' for standard input.  
%  Specify 'image.gray' as '-' for standard output.
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
static void Error(message,qualifier)
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
%    message:  Specifies a specific message to display to the user.
%
*/
static void Usage(message)
char
  *message;
{
  if (message != (char *) NULL)
    (void) fprintf(stderr,"Can't continue, %s\n\n",message);
  (void) fprintf(stderr,"Usage: %s image.miff image.gray\n\n",application_name);
  (void) fprintf(stderr,"Specify 'image.miff' as '-' for standard input.\n");
  (void) fprintf(stderr,"Specify 'image.gray' as '-' for standard output.\n");
  exit(1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e G R A Y I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteGRAYImage writes an image to a file on disk in red, green,
%  blue rasterfile format.
%
%  The format of the WriteGRAYImage routine is:
%
%      WriteGRAYImage(image)
%
%  A description of each parameter follows.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static void WriteGRAYImage(image)
Image
  *image;
{
  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *pixels;

  /*
    Open output image file.
  */
  if (*image->filename == '-')
    image->file=stdout;
  else
    if (strcmp(image->filename+strlen(image->filename)-2,".Z") != 0)
      image->file=fopen(image->filename,"w");
    else
      {
        char
          command[256];

        /*
          Image file is compressed-- uncompress it.
        */
        (void) sprintf(command,"compress -c > %s",image->filename);
        image->file=(FILE *) popen(command,"w");
      }
  if (image->file == (FILE *) NULL)
    Error("unable to open file",image->filename);
  /*
    Convert image to gray scale PseudoColor class.
  */
  pixels=(unsigned char *) 
    malloc(image->columns*image->rows*sizeof(unsigned char));
  if (pixels == (unsigned char *) NULL)
    Error("unable to allocate memory",(char *) NULL);
  (void) GrayImage(image);
  if (image->class == DirectClass)
    QuantizeImage(image,256,8,False,True);
  p=image->pixels;
  q=pixels;
  for (i=0; i < image->packets; i++)
  {
    for (j=0; j <= p->length; j++)
      *q++=p->red;
    p++;
  }
  (void) fwrite((char *) pixels,sizeof(unsigned char),
    (int) (image->columns*image->rows),image->file);
  (void) free((char *) pixels);
  if (image->file != stdin)
    if (strcmp(image->filename+strlen(image->filename)-2,".Z") != 0)
      (void) fclose(image->file);
    else
      (void) pclose(image->file);
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
  Image
    *image;

  application_name=argv[0];
  if (argc < 3)
    Usage((char *) NULL);
  image=ReadImage(argv[1]);
  if (image == (Image *) NULL)
    exit(1);
  (void) strcpy(image->filename,argv[2]);
  WriteGRAYImage(image);
  (void) fprintf(stderr,"%s=> %s  %dx%d\n",argv[1],argv[2],image->columns,
    image->rows);
  return(False);
}
