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
%                  Export MIFF image to a XWD raster format.                  %
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
%  export image.miff image.xwd
%
%  Specify 'image.miff' as '-' for standard input.  
%  Specify 'image.xwd' as '-' for standard output.
%
%
*/

#include "display.h"
#include "image.h"
#include "X.h"
#include "X11/XWDFile.h"

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
%  M S B F i r s t O r d e r L o n g                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function MSBFirstOrderLong converts a least-significant byte first buffer
%  of long integers to most-significant byte first.
%
%  The format of the MSBFirstOrderLong routine is:
%
%       MSBFirstOrderLong(p,length);
%
%  A description of each parameter follows.
%
%   o  p:  Specifies a pointer to a buffer of integers.
%
%   o  length:  Specifies the length of the buffer.
%
%
*/
MSBFirstOrderLong(p,length)
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
%  M S B F i r s t O r d e r S h o r t                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function MSBFirstOrderShort converts a least-significant byte first buffer
%  of short integers to most-significant byte first.
%
%  The format of the MSBFirstOrderShort routine is:
%
%       MSBFirstOrderShort(p,length);
%
%  A description of each parameter follows.
%
%   o  p:  Specifies a pointer to a buffer of integers.
%
%   o  length:  Specifies the length of the buffer.
%
%
*/
MSBFirstOrderShort(p,length)
register char
  *p;

register unsigned
  length;
{
  register char
    c,
    *q;

  q=p+length;
  while (p < q)
  {
    c=(*p);
    *p=(*(p+1));
    p++;
    *p++=c;
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
  (void) fprintf(stderr,"Usage: %s image.miff image.xwd\n\n",application_name);
  (void) fprintf(stderr,"Specify 'image.miff' as '-' for standard input.\n");
  (void) fprintf(stderr,"Specify 'image.xwd' as '-' for standard output.\n");
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
%  Procedure WriteXWDImage writes an image to a file on disk in XWD 
%  rasterfile format.
%
%  The format of the WriteXWDImage routine is:
%
%      WriteXWDImage(image)
%
%  A description of each parameter follows.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static void WriteXWDImage(image)
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
    *xwd_pixels;

  unsigned int
    number_pixels;

  unsigned long
    lsb_first;

  XWDFileHeader
    xwd_header;

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
    Initialize XWD file header.
  */
  xwd_header.header_size=sizeof(xwd_header)+strlen(image->filename)+1;
  xwd_header.file_version=(unsigned long) XWD_FILE_VERSION;
  xwd_header.pixmap_format=(unsigned long) ZPixmap;
  xwd_header.pixmap_depth=(unsigned long) 
    (image->class == DirectClass ? 24 : 8);
  xwd_header.pixmap_width=(unsigned long) image->columns;
  xwd_header.pixmap_height=(unsigned long) image->rows;
  xwd_header.xoffset=(unsigned long) 0;
  xwd_header.byte_order=(unsigned long) MSBFirst;
  xwd_header.bitmap_unit=(unsigned long) 
    (image->class == DirectClass ? 24 : 8);
  xwd_header.bitmap_bit_order=(unsigned long) MSBFirst;
  xwd_header.bitmap_pad=(unsigned long) 
    (image->class == DirectClass ? 24 : 8);
  xwd_header.bits_per_pixel=(unsigned long) 
    (image->class == DirectClass ? 24 : 8);
  xwd_header.bytes_per_line=(unsigned long) image->columns*
    (image->class == DirectClass ? 3 : 1);
  xwd_header.visual_class=(unsigned long) 
    (image->class == DirectClass ? DirectColor : PseudoColor);
  xwd_header.red_mask=(unsigned long) 
    (image->class == DirectClass ? 0xff0000 : 0);
  xwd_header.green_mask=(unsigned long) 
    (image->class == DirectClass ? 0xff00 : 0);
  xwd_header.blue_mask=(unsigned long) 
    (image->class == DirectClass ? 0xff : 0);
  xwd_header.bits_per_rgb=(unsigned long) 
    (image->class == DirectClass ? 24 : 8);
  xwd_header.colormap_entries=(unsigned long) 
    (image->class == DirectClass ? 256 : image->colors);
  xwd_header.ncolors=(image->class == DirectClass ? 0 : image->colors);
  xwd_header.window_width=(unsigned long) image->columns;
  xwd_header.window_height=(unsigned long) image->rows;
  xwd_header.window_x=0;
  xwd_header.window_y=0;
  xwd_header.window_bdrwidth=(unsigned long) 0;
  number_pixels=xwd_header.bytes_per_line*xwd_header.pixmap_height;
  /*
    Ensure the xwd_header byte-order is most-significant byte first.
  */
  lsb_first=1;
  if (*(char *) &lsb_first)
    MSBFirstOrderLong((char *) &xwd_header,sizeof(xwd_header));
  (void) fwrite((char *) &xwd_header,sizeof(xwd_header),1,image->file);
  (void) fwrite((char *) image->filename,1,strlen(image->filename)+1,
    image->file);
  if (image->class == PseudoClass)
    {
      XColor
        *colors;

      /*
         Dump colormap to file.
      */
      colors=(XColor *) malloc((unsigned int) image->colors*sizeof(XColor));
      if (colors == (XColor *) NULL)
        Error("unable to allocate memory",(char *) NULL);
      for (i=0; i < image->colors; i++)
      {
        colors[i].pixel=i;
        colors[i].red=image->colormap[i].red << 8;
        colors[i].green=image->colormap[i].green << 8;
        colors[i].blue=image->colormap[i].blue << 8;
        colors[i].flags=DoRed | DoGreen | DoBlue;
        colors[i].pad=0;
        if (*(char *) &lsb_first)
          {
            MSBFirstOrderLong((char *) &colors[i].pixel,sizeof(long));
            MSBFirstOrderShort((char *) &colors[i].red,3*sizeof(short));
          }
      }
      (void) fwrite((char *) colors,sizeof(XColor),(int) image->colors,
        image->file);
      (void) free((char *) colors);
    }
  /*
    Convert MIFF to XWD raster pixels.
  */
  xwd_pixels=(unsigned char *) malloc(number_pixels);
  if (xwd_pixels == (unsigned char *) NULL)
    Error("unable to allocate memory",(char *) NULL);
  p=image->pixels;
  q=xwd_pixels;
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
    }
    p++;
  }
  /*
    Dump pixels to file.
  */
  (void) fwrite((char *) xwd_pixels,sizeof(char),(int) number_pixels,
    image->file);
  (void) free((char *) xwd_pixels);
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
  WriteXWDImage(image);
  (void) fprintf(stderr,"%s=> %s  %dx%d\n",argv[1],argv[2],image->columns,
    image->rows);
  return(False);
}
