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
%               Import X11 XWD raster image to a MIFF format.                 %
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
%  import [-scene #] image.xwd image.miff
%
%  Specify 'image.xwd' as '-' for standard input.
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
%  R e a d X W D I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadXWDImage reads an image file and returns it.  It allocates the 
%  memory necessary for the new Image structure and returns a pointer to the 
%  new image.
%
%  The format of the ReadXWDImage routine is:
%
%      image=ReadXWDImage(filename)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadXWDImage returns a pointer to the image after 
%      reading.  A null image is returned if there is a a memory shortage or 
%      if the image cannot be read.
%
%    o filename:  Specifies the name of the image to read.
%
%
*/
static Image *ReadXWDImage(filename)
char
  *filename;
{
#include <X11/XWDFile.h>

  char
    *window_name;

  Display
    display;

  Image
    *image;

  int
    status,
    x,
    y;

  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned long
    pixel;

  unsigned int
    packets;

  unsigned long
    lsb_first;

  ScreenFormat 
    screen_format;

  XColor
    *colors;

  XImage
    *ximage;

  XWDFileHeader
    header;

  /*
    Allocate image structure.
  */
  image=AllocateImage();
  if (image == (Image *) NULL)
    Error("unable to allocate memory",(char *) NULL);
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
     Read in header information.
  */
  status=ReadData((char *) &header,sizeof(header),1,image->file);
  if (status == False)
    Error("Unable to read dump file header",image->filename);
  /*
    Ensure the header byte-order is most-significant byte first.
  */
  lsb_first=1;
  if (*(char *) &lsb_first)
    MSBFirstOrderLong((char *) &header,sizeof(header));
  /*
    Check to see if the dump file is in the proper format.
  */
  if (header.file_version != XWD_FILE_VERSION)
    Error("XWD file format version mismatch",image->filename);
  if (header.header_size < sizeof(header))
    Error("XWD header size is too small",image->filename);
  packets=(header.header_size-sizeof(header));
  window_name=(char *) malloc((unsigned int) packets);
  if (window_name == (char *) NULL)
    Error("unable to allocate memory",(char *) NULL);
  status=ReadData((char *) window_name,sizeof(char),(int) packets,image->file);
  if (status == False)
    Error("unable to read window name from dump file",image->filename);
  /*
    Initialize the X image.
  */
  display.byte_order=header.byte_order;
  display.bitmap_unit=header.bitmap_unit;
  display.bitmap_bit_order=header.bitmap_bit_order;
  display.pixmap_format=(&screen_format);
  display.nformats=1;
  screen_format.depth=header.pixmap_depth;
  screen_format.bits_per_pixel=(int) header.bits_per_pixel;
  ximage=XCreateImage(&display,(Visual *) NULL,header.pixmap_depth,
    header.pixmap_format,header.xoffset,0,header.pixmap_width,
    header.pixmap_height,header.bitmap_pad,header.bytes_per_line);
  ximage->red_mask=header.red_mask;
  ximage->green_mask=header.green_mask;
  ximage->blue_mask=header.blue_mask;
  /*
    Read colormap.
  */
  if (header.ncolors > 0)
    {
      colors=(XColor *) malloc((unsigned) header.ncolors*sizeof(XColor));
      if (colors == (XColor *) NULL)
        Error("unable to allocate memory",(char *) NULL);
      status=ReadData((char *) colors,sizeof(XColor),(int) header.ncolors,
	image->file);
      if (status == False)
        Error("unable to read color map from dump file",image->filename);
      /*
        Ensure the header byte-order is most-significant byte first.
      */
      lsb_first=1;
      if (*(char *) &lsb_first)
        for (i=0; i < header.ncolors; i++)
        {
          MSBFirstOrderLong((char *) &colors[i].pixel,sizeof(unsigned long));
          MSBFirstOrderShort((char *) &colors[i].red,3*sizeof(unsigned short));
        }
    }
  /*
    Allocate the pixel buffer.
  */
  if (ximage->format == ZPixmap)
    packets=ximage->bytes_per_line*ximage->height;
  else
    packets=ximage->bytes_per_line*ximage->height*ximage->depth;
  ximage->data=(char *) malloc((unsigned int) packets);
  if (ximage->data == (char *) NULL)
    Error("unable to allocate memory",(char *) NULL);
  status=ReadData(ximage->data,sizeof(char),(int) packets,image->file);
  if (status == False)
    Error("unable to read dump pixmap",image->filename);
  if (*image->filename != '-')
    (void) fclose(image->file);
  /*
    Convert image to MIFF format.
  */
  image->columns=ximage->width;
  image->rows=ximage->height;
  /*
    Initial image comment.
  */
  image->comments=(char *) 
    malloc((unsigned int) (strlen(image->filename)+256));
  if (image->comments == (char *) NULL)
    Error("unable to allocate memory",(char *) NULL);
  (void) sprintf(image->comments,"\n  Imported from X11 dump file:  %s\n\0",
    image->filename);
  if ((ximage->red_mask > 0) || (ximage->green_mask > 0) ||
      (ximage->blue_mask > 0))
    image->class=DirectClass;
  else
    image->class=PseudoClass;
  image->colors=header.ncolors;
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    malloc(image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    Error("unable to allocate memory",(char *) NULL);
  p=image->pixels;
  switch (image->class)
  {
    case DirectClass:
    {
      register unsigned long
        color;

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
      red_mask=ximage->red_mask;
      red_shift=0;
      while ((red_mask & 0x01) == 0)
      {
        red_mask>>=1;
        red_shift++;
      }
      green_mask=ximage->green_mask;
      green_shift=0;
      while ((green_mask & 0x01) == 0)
      {
        green_mask>>=1;
        green_shift++;
      }
      blue_mask=ximage->blue_mask;
      blue_shift=0;
      while ((blue_mask & 0x01) == 0)
      {
        blue_mask>>=1;
        blue_shift++;
      }
      /*
        Convert X image to DirectClass packets.
      */
      if (image->colors > 0)
        for (y=0; y < image->rows; y++)
        {
          for (x=0; x < image->columns; x++)
          {
            pixel=XGetPixel(ximage,x,y);
            p->index=(pixel >> red_shift) & red_mask;
            p->red=(unsigned char) (colors[p->index].red >> 8);
            p->index=(pixel >> green_shift) & green_mask;
            p->green=(unsigned char) (colors[p->index].green >> 8);
            p->index=(pixel >> blue_shift) & blue_mask;
            p->blue=(unsigned char) (colors[p->index].blue >> 8);
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
        Error("unable to allocate memory",(char *) NULL);
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
  (void) free((char *) window_name);
  if (header.ncolors > 0)
    (void) free((char *) colors);
  XDestroyImage(ximage);
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
  (void) fprintf(stderr,"Usage: %s [-scene #] image.xwd image.miff\n\n",
    application_name);
  (void) fprintf(stderr,"Specify 'image.xwd' as '-' for standard input.\n");
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
  image=ReadXWDImage(filename);
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
