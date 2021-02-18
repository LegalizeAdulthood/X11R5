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
%                  Export MIFF image to a SUN raster format.                  %
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
%  export image.miff image.sun
%
%  Specify 'image.miff' as '-' for standard input.  
%  Specify 'image.sun' as '-' for standard output.
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
%  M S B F i r s t O r d e r                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function MSBFirstOrder converts a least-significant byte first buffer of
%  integers to most-significant byte first.
%
%  The format of the MSBFirstOrder routine is:
%
%       MSBFirstOrder(p,length);
%
%  A description of each parameter follows.
%
%   o  p:  Specifies a pointer to a buffer of integers.
%
%   o  length:  Specifies the length of the buffer.
%
%
*/
MSBFirstOrder(p,length)
register char 
  *p;

register unsigned 
  length;
{
  register char 
    c,
    *q,
    *sp;

  q=p+length;
  while (p < q) 
  {
    sp=p+3;
    c=(*sp);
    *sp=(*p);
    *p++=c;
    sp=p+1;
    c=(*sp);
    *sp=(*p);
    *p++=c;
    p+=2;
  }
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
  (void) fprintf(stderr,"Usage: %s image.miff image.sun\n\n",application_name);
  (void) fprintf(stderr,"Specify 'image.miff' as '-' for standard input.\n");
  (void) fprintf(stderr,"Specify 'image.sun' as '-' for standard output.\n");
  exit(1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e R a s t e r I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteRasterImage writes an image to a file on disk in SUN 
%  rasterfile format.
%
%  The format of the WriteRasterImage routine is:
%
%      WriteRasterImage(image)
%
%  A description of each parameter follows.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static void WriteRasterImage(image)
Image
  *image;
{
#define RMT_EQUAL_RGB  1
#define RMT_NONE  0
#define RMT_RAW  2
#define RT_STANDARD  1
#define RT_FORMAT_RGB  3

  typedef struct Raster
  {
    int
      magic,
      width,
      height,
      depth,
      length,
      type,
      maptype,
      maplength;
  } Raster;

  Raster
    sun_header;

  register int
    i,
    j,
    x;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *sun_pixels;

  unsigned long
    lsb_first;

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
    Initialize raster file sun_header.
  */
  sun_header.magic=0x59a66a95;
  sun_header.width=image->columns;
  sun_header.height=image->rows;
  sun_header.depth=(image->class == DirectClass ? 24 : 8);
  sun_header.length=image->columns*image->rows*(sun_header.depth >> 3)+
    (image->columns % 2 ? image->rows : 0);
  sun_header.type=(image->class == DirectClass ? RT_FORMAT_RGB : RT_STANDARD);
  sun_header.maptype=(image->class == DirectClass ? RMT_NONE : RMT_EQUAL_RGB);
  sun_header.maplength=(image->class == DirectClass ? 0 : image->colors*3);
  /*
    Ensure the sun_header byte-order is most-significant byte first.
  */
  lsb_first=1;
  if ((*(char *) &lsb_first) == 0)
    (void) fwrite((char *) &sun_header,sizeof(sun_header),1,image->file);
  else
    {
      Raster
        reverse_header;

      reverse_header=sun_header;
      MSBFirstOrder((char *) &reverse_header,sizeof(reverse_header));
      (void) fwrite((char *) &reverse_header,sizeof(reverse_header),1,
        image->file);
    }
  if (image->class == PseudoClass)
    {
      unsigned char
        *sun_colormap;

      /*
         Dump colormap to file.
      */
      sun_colormap=(unsigned char *) 
        malloc((unsigned int) sun_header.maplength);
      if (sun_colormap == (unsigned char *) NULL)
        Error("unable to allocate memory",(char *) NULL);
      q=sun_colormap;
      for (i=0; i < image->colors; i++)
        *q++=image->colormap[i].red;
      for (i=0; i < image->colors; i++)
        *q++=image->colormap[i].green;
      for (i=0; i < image->colors; i++)
        *q++=image->colormap[i].blue;
      (void) fwrite((char *) sun_colormap,sizeof(char),sun_header.maplength,
        image->file);
      (void) free((char *) sun_colormap);
    }
  /*
    Convert MIFF to SUN raster pixels.
  */
  sun_pixels=(unsigned char *) malloc((unsigned int) sun_header.length);
  if (sun_pixels == (unsigned char *) NULL)
    Error("unable to allocate memory",(char *) NULL);
  p=image->pixels;
  q=sun_pixels;
  x=0;
  for (i=0; i < image->packets; i++)
  {
    for (j=0; j <= p->length; j++)
    {
      if (image->class == PseudoClass)
        *q++=p->index;
      else
        {
          *q++=p->red;
          *q++=p->green;
          *q++=p->blue;
        }
      if ((image->columns % 2) != 0)
        {
          x++;
          if ((x % image->columns) == 0)
            *q++;  /* pad scanline */
        }
    }
    p++;
  }
  (void) fwrite((char *) sun_pixels,sizeof(char),sun_header.length,image->file);
  (void) free((char *) sun_pixels);
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
  WriteRasterImage(image);
  (void) fprintf(stderr,"%s=> %s  %dx%d\n",argv[1],argv[2],image->columns,
    image->rows);
  return(False);
}
