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
%                 Import SUN raster image to a MIFF format.                   %
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
%  import [-scene #] image.sun image.miff
%  
%  Specify 'image.sun' as '-' for standard input.  
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
%  R e a d S U N I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadSUNImage reads an image file and returns it.  It allocates 
%  the memory necessary for the new Image structure and returns a pointer to 
%  the new image.
%
%  The format of the ReadSUNImage routine is:
%
%      image=ReadSUNImage(filename)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadSUNImage returns a pointer to the image after 
%      reading.  A null image is returned if there is a a memory shortage or 
%      if the image cannot be read.
%
%    o filename:  Specifies the name of the image to read.
%
%
*/
static Image *ReadSUNImage(filename)
char
  *filename;
{
#define RMT_EQUAL_RGB  1
#define RMT_NONE  0
#define RMT_RAW  2
#define RT_STANDARD  1
#define RT_ENCODED  2
#define RT_FORMAT_RGB  3

  typedef struct _Rasterfile
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
  } Rasterfile;

  Image
    *image;

  Rasterfile
    sun_header;

  register int
    bit,
    i,
    x,
    y;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *sun_data,
    *sun_pixels;

  unsigned long 
    lsb_first;

  unsigned short
    index;

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
    Read raster image.
  */
  (void) ReadData((char *) &sun_header,1,sizeof(Rasterfile),image->file);
  /*
    Ensure the header byte-order is most-significant byte first.
  */
  lsb_first=1;
  if (*(char *) &lsb_first)
    MSBFirstOrder((char *) &sun_header,sizeof(sun_header));
  if (sun_header.magic != 0x59a66a95)
    Error("not a SUN raster,",image->filename);
printf("%d\n",sun_header.length,sun_header.depth);
printf("%d\n",sun_header.maplength,sun_header.depth);
  switch (sun_header.maptype)
  {
    case RMT_NONE:
    {
puts("none");
      if (sun_header.depth < 24)
        {
          /*
            Create linear color ramp.
          */
          image->colors=1 << sun_header.depth;
          image->colormap=(ColorPacket *) 
            malloc(image->colors*sizeof(ColorPacket));
          if (image->colormap == (ColorPacket *) NULL)
            Error("memory allocation error",(char *) NULL);
          for (i=0; i < image->colors; i++)
          {
            image->colormap[i].red=(255*i)/(image->colors-1);
            image->colormap[i].green=(255*i)/(image->colors-1);
            image->colormap[i].blue=(255*i)/(image->colors-1);
          }
        }
      break;
    }
    case RMT_EQUAL_RGB:
    {
      unsigned char
        *sun_colormap;

puts("rgb");
      /*
        Read Sun raster colormap.
      */
      image->colors=sun_header.maplength/3;
      image->colormap=
        (ColorPacket *) malloc(image->colors*sizeof(ColorPacket));
      sun_colormap=(unsigned char *) 
        malloc(image->colors*sizeof(unsigned char));
      if ((image->colormap == (ColorPacket *) NULL) ||
          (sun_colormap == (unsigned char *) NULL))
        Error("memory allocation error",(char *) NULL);
      (void) ReadData((char *) sun_colormap,1,(int) image->colors,image->file);
      for (i=0; i < image->colors; i++)
        image->colormap[i].red=sun_colormap[i];
      (void) ReadData((char *) sun_colormap,1,(int) image->colors,image->file);
      for (i=0; i < image->colors; i++)
        image->colormap[i].green=sun_colormap[i];
      (void) ReadData((char *) sun_colormap,1,(int) image->colors,image->file);
      for (i=0; i < image->colors; i++)
        image->colormap[i].blue=sun_colormap[i];
      (void) free((char *) sun_colormap);
      break;
    }
    case RMT_RAW:
    {
      unsigned char
        *sun_colormap;

      /*
        Read Sun raster colormap.
      */
puts("raw");
      sun_colormap=(unsigned char *) 
        malloc((unsigned int) sun_header.maplength*sizeof(unsigned char));
      if (sun_colormap == (unsigned char *) NULL)
        Error("memory allocation error",(char *) NULL);
      (void) ReadData((char *) sun_colormap,1,sun_header.maplength,
        image->file);
      (void) free((char *) sun_colormap);
      break;
    }
    default:
    {
      Error("colormap type is not supported",image->filename);
      break;
    }
  }
  sun_data=(unsigned char *) 
    malloc((unsigned int) sun_header.length*sizeof(unsigned char));
  if (sun_data == (unsigned char *) NULL)
    Error("memory allocation error",(char *) NULL);
  (void) ReadData((char *) sun_data,1,sun_header.length,image->file);
  sun_pixels=sun_data;
printf("%d\n",sun_header.type);
  if (sun_header.type == RT_ENCODED) 
    {
      register int
        count,
        number_pixels;

      register unsigned char
        byte,
        *q;

      /*
        Read run-length encoded raster pixels.
      */
      number_pixels=(sun_header.width+(sun_header.width % 2))*
        sun_header.height*(((sun_header.depth-1) >> 3)+1);
      sun_pixels=(unsigned char *) 
        malloc((unsigned int) number_pixels*sizeof(unsigned char));
      if (sun_pixels == (unsigned char *) NULL)
        Error("memory allocation error",(char *) NULL);
      p=sun_data;
      q=sun_pixels;
      while ((q-sun_pixels) <= number_pixels)
      {
        byte=(*p++);
        if (byte != 128)
          *q++=byte;
        else
          {
            /*
              Runlength-encoded packet: <count><byte>
            */
            count=(*p++);
            if (count > 0)
              byte=(*p++);
            while (count >= 0)
            {
              *q++=byte;
              count--;
            }
         }
      }
    (void) free((char *) sun_data);
  }
  /*
    Create image.
  */
  image->comments=(char *) malloc((unsigned int) (strlen(image->filename)+256));
  if (image->comments == (char *) NULL)
    Error("memory allocation error",(char *) NULL);
  (void) sprintf(image->comments,"\n  Imported from Sun raster image:  %s\n\0",
    image->filename);
  image->class=(sun_header.depth < 24 ? PseudoClass : DirectClass);
  image->columns=sun_header.width;
  image->rows=sun_header.height;
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *) 
    malloc(image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    Error("memory allocation error",(char *) NULL);
  /*
    Convert Sun raster image to runlength-encoded packets.
  */
  p=sun_pixels;
  q=image->pixels;
  if (sun_header.depth == 1)
    for (y=0; y < image->rows; y++)
    {
      /*
        Convert bitmap scanline to runlength-encoded color packets.
      */
      for (x=0; x < (image->columns >> 3); x++) 
      {
        for (bit=7; bit >= 0; bit--)
        {
          index=((*p) & (0x01 << bit) ? 0x00 : 0x01);
          q->red=image->colormap[index].red;
          q->green=image->colormap[index].green;
          q->blue=image->colormap[index].blue;
          q->index=index;
          q->length=0;
          q++;
        }
        p++;
      }
      if ((image->columns % 8) != 0)
        {
          for (bit=7; bit >= (8-(image->columns % 8)); bit--)
          {
            index=((*p) & (0x01 << bit) ? 0x00 : 0x01);
            q->red=image->colormap[index].red;
            q->green=image->colormap[index].green;
            q->blue=image->colormap[index].blue;
            q->index=index;
            q->length=0;
            q++;
          }
          p++;
        }
      if ((((image->columns/8)+(image->columns % 8 ? 1 : 0)) % 2) != 0)
        p++;
    }
  else
    if (image->class == PseudoClass)
      for (y=0; y < image->rows; y++)
      {
        /*
          Convert PseudoColor scanline to runlength-encoded color packets.
        */
        for (x=0; x < image->columns; x++) 
        {
          index=(*p++);
          q->red=image->colormap[index].red;
          q->green=image->colormap[index].green;
          q->blue=image->colormap[index].blue;
          q->index=index;
          q->length=0;
          q++;
        }
        if ((image->columns % 2) != 0)
          p++;
      }
    else
      for (y=0; y < image->rows; y++)
      {
        /*
          Convert DirectColor scanline to runlength-encoded color packets.
        */
        for (x=0; x < image->columns; x++) 
        {
          if (sun_header.type == RT_STANDARD)
            {
              q->blue=(*p++);
              q->green=(*p++);
              q->red=(*p++);
            }
          else
            {
              q->red=(*p++);
              q->green=(*p++);
              q->blue=(*p++);
            }
          if (image->colors > 0)
            {
              q->red=image->colormap[q->red].red;
              q->green=image->colormap[q->green].green;
              q->blue=image->colormap[q->blue].blue;
            }
          q->index=0;
          q->length=0;
          q++;
        }
        if ((image->columns % 2) != 0)
          p++;
      }
  (void) free((char *) sun_pixels);
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
  (void) fprintf(stderr,"Usage: %s [-scene #] image.sun image.miff\n\n",
    application_name);
  (void) fprintf(stderr,"Specify 'image.sun' as '-' for standard input.\n");
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
  image=ReadSUNImage(filename);
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
