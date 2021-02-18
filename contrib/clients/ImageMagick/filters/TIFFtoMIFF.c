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
%                    Import TIFF image to a MIFF format.                      %
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
%  import [-scene #] image.tiff image.miff
%  
%  Specify 'image.tiff' as '-' for standard input.  
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
%  R e a d T i f f I m a g e                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadTIFFImage reads an image file and returns it.  It allocates the 
%  memory necessary for the new Image structure and returns a pointer to the 
%  new image.
%
%  The format of the ReadTIFFImage routine is:
%
%      image=ReadTIFFImage(filename)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadTIFFImage returns a pointer to the image after 
%      reading.  A null image is returned if there is a a memory shortage or 
%      if the image cannot be read.
%
%    o filename:  Specifies the name of the image to read.
%
%    o verbose:  A value greater than zero prints detailed information about 
%      the image.
%
%
*/
static Image *ReadTIFFImage(filename,verbose)
char
  *filename;

unsigned int
  verbose;
{
#include <tiff.h>
#include <tiffio.h>

  Image
    *image;

  register int
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  register unsigned int 
    x,
    y;

  TIFF
    *file;

  unsigned char
    *scanline;

  unsigned short
    bits_per_sample,
    fill_order,
    photometric,
    planar_configuration,
    samples_per_pixel;

  unsigned long
    image_length,
    image_width;

  /*
    Allocate image structure.
  */
  image=AllocateImage();
  if (image == (Image *) NULL)
    Error("unable to allocate memory",(char *) NULL);
  /*
    Open TIFF image file.
  */
  (void) strcpy(image->filename,filename);
  file=TIFFOpen(image->filename,"r"); 
  if (file == (TIFF *) NULL)
    exit(1);
  if (verbose)
    TIFFPrintDirectory(file,stderr,False,False,False);
  /*
    Verify this image is acceptable.
  */
  if (!TIFFGetField(file,TIFFTAG_BITSPERSAMPLE,&bits_per_sample))
    bits_per_sample=1;
  if (!TIFFGetField(file,TIFFTAG_PHOTOMETRIC,&photometric))
    photometric=PHOTOMETRIC_PALETTE;
  if ((bits_per_sample > 8) && (photometric != PHOTOMETRIC_RGB))
    Error("image is more than 8-bits per sample",(char *) NULL);
  if (!TIFFGetField(file,TIFFTAG_SAMPLESPERPIXEL,&samples_per_pixel))
    samples_per_pixel=1;
  if ((samples_per_pixel != 1) && (samples_per_pixel != 3) &&
      (samples_per_pixel != 4))
    Error("samples per pixel must be 1, 3, or 4",(char *) NULL);
  /*
    Initialize image attributes.
  */
  if (!TIFFGetField(file,TIFFTAG_PHOTOMETRIC,&photometric))
    photometric=PHOTOMETRIC_PALETTE;
  TIFFGetField(file,TIFFTAG_IMAGEWIDTH,&image_width);
  TIFFGetField(file,TIFFTAG_IMAGELENGTH,&image_length);
  image->comments=(char *) malloc((unsigned int) (strlen(image->filename)+256));
  if (image->comments == (char *) NULL)
    Error("unable to allocate memory",(char *) NULL);
  (void) sprintf(image->comments,"\n  Imported from TIFF image %s.\n\0",
    image->filename);
  image->class=(photometric == PHOTOMETRIC_RGB ? DirectClass : PseudoClass);
  image->columns=image_width;
  image->rows=image_length;
  image->colors=1 << bits_per_sample;
  if (image->class == PseudoClass)
    {
      /*
        Prepare for pseudo-color image.
      */
      image->colormap=(ColorPacket *) malloc(image->colors*sizeof(ColorPacket));
      if (image->colormap == (ColorPacket *) NULL)
        Error("unable to allocate memory",(char *) NULL);
      /*
        Create colormap.
      */
      switch (photometric)
      {
        case PHOTOMETRIC_MINISBLACK:
        {
          for (i=0; i < image->colors; i++)
          {
            image->colormap[i].red=(255*i)/(image->colors-1);
            image->colormap[i].green=(255*i)/(image->colors-1);
            image->colormap[i].blue=(255*i)/(image->colors-1);
          }
          break;
        }
        case PHOTOMETRIC_MINISWHITE:
        {
          for (i=0; i < image->colors; i++)
          {
            image->colormap[i].red=255-(255*i)/(image->colors-1);
            image->colormap[i].green=255-(255*i)/(image->colors-1);
            image->colormap[i].blue=255-(255*i)/(image->colors-1);
          }
          break;
        }
        case PHOTOMETRIC_PALETTE:
        {
          unsigned short
            *blue_colormap,
            *green_colormap,
            *red_colormap;

          TIFFGetField(file,TIFFTAG_COLORMAP,&red_colormap,&green_colormap,
            &blue_colormap);
          for (i=0; i < image->colors; i++)
          {
            image->colormap[i].red=red_colormap[i] >> 8;
            image->colormap[i].green=green_colormap[i] >> 8;
            image->colormap[i].blue=blue_colormap[i] >> 8;
          }
          break;
        }
        default:
          break;
      }
    }
  /*
    Allocate memory for the image and scanline buffer.
  */
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *) 
    malloc(image->packets*sizeof(RunlengthPacket));
  TIFFGetField(file,TIFFTAG_PLANARCONFIG,&planar_configuration);
  if (planar_configuration == PLANARCONFIG_CONTIG)
    scanline=(unsigned char *) malloc((unsigned int) TIFFScanlineSize(file));
  else
    scanline=(unsigned char *) 
      malloc((unsigned int) samples_per_pixel*TIFFScanlineSize(file));
  if ((image->pixels == (RunlengthPacket *) NULL) ||
      (scanline == (unsigned char *) NULL))
    Error("unable to allocate memory",(char *) NULL);
  /*
    Read the TIFF image and write to image pixel buffer.
  */
  if (!TIFFGetField(file,TIFFTAG_FILLORDER,&fill_order))
    fill_order=FILLORDER_MSB2LSB;
  q=image->pixels;
  for (y=0; y < image->rows; y++)
  {
    p=scanline;
    if (TIFFReadScanline(file,p,y,0) < 0)
      Error("unable to read TIFF scanline",(char *) NULL);
    if (planar_configuration != PLANARCONFIG_CONTIG)
      for (i=1; i < samples_per_pixel; i++)
        if (TIFFReadScanline(file,p+i*TIFFScanlineSize(file),y,i) < 0)
          Error("unable to read TIFF scanline",(char *) NULL);
    switch (photometric)
    {
      case PHOTOMETRIC_RGB:
      {
        switch (bits_per_sample)
        {
          case 8:
          default:
          {
            /*
              Assumes Max Sample Value is 255.
            */
            for (x=0; x < image->columns; x++)
            {
              if (planar_configuration == PLANARCONFIG_CONTIG)
                {
                  q->red=(*p++);
                  q->green=(*p++);
                  q->blue=(*p++);
                }
              else
                {
                  q->red=(*p);
                  q->green=(*(p+TIFFScanlineSize(file)));
                  q->blue=(*(p+2*TIFFScanlineSize(file)));
                  p++;
                }
              q->index=0;
              q->length=0;
              q++;
              if (samples_per_pixel == 4)
                p++;
            }
            break;
          }
          case 16:
          {
            register unsigned short
              *p;

            /*
              Assumes Max Sample Value is 2047.
            */
            p=(unsigned short *) scanline;
            for (x=0; x < image->columns; x++)
            {
              if (planar_configuration == PLANARCONFIG_CONTIG)
                {
                  q->red=(*p++ >> 3);
                  q->green=(*p++ >> 3);
                  q->blue=(*p++ >> 3);
                }
              else
                {
                  q->red=(*p >> 3);
                  q->green=(*(p+TIFFScanlineSize(file)) >> 3);
                  q->blue=(*(p+2*TIFFScanlineSize(file)) >> 3);
                  p++;
                }
              q->index=0;
              q->length=0;
              q++;
              if (samples_per_pixel == 4)
                p++;
            }
            break;
          }
        }
        break;
      }
      case PHOTOMETRIC_MINISBLACK:
      case PHOTOMETRIC_MINISWHITE:
      {
        switch (bits_per_sample)
        {
          case 1:
          {
            register int
              bit;

            for (x=0; x < image->columns; x+=8)
            {
              for (bit=7; bit >= 0; bit--)
              {
                q->index=((*p) & (0x01 << bit) ? 0x01 : 0x00);
                q->red=image->colormap[q->index].red;
                q->green=image->colormap[q->index].green;
                q->blue=image->colormap[q->index].blue;
                q->length=0;
                q++;
              }
              p++;
            }
            if ((image->columns % 8) != 0)
              {
                for (bit=7; bit >= (8-(image->columns % 8)); bit--)
                {
                  q->index=((*p) & (0x01 << bit) ? 0x00 : 0x01);
                  q->red=image->colormap[q->index].red;
                  q->green=image->colormap[q->index].green;
                  q->blue=image->colormap[q->index].blue;
                  q->length=0;
                  q++;
                }
                p++;
              }
            break;
          }
          case 2:
          {
            for (x=0; x < image->columns; x+=4)
            {
              q->index=(*p >> 6) & 0x3;
              q->red=image->colormap[q->index].red;
              q->green=image->colormap[q->index].green;
              q->blue=image->colormap[q->index].blue;
              q->length=0;
              q++;
              q->index=(*p >> 4) & 0x3;
              q->red=image->colormap[q->index].red;
              q->green=image->colormap[q->index].green;
              q->blue=image->colormap[q->index].blue;
              q->length=0;
              q++;
              q->index=(*p >> 2) & 0x3;
              q->red=image->colormap[q->index].red;
              q->green=image->colormap[q->index].green;
              q->blue=image->colormap[q->index].blue;
              q->length=0;
              q++;
              q->index=(*p) & 0x3;
              q->red=image->colormap[q->index].red;
              q->green=image->colormap[q->index].green;
              q->blue=image->colormap[q->index].blue;
              q->length=0;
              q++;
              p++;
            }
            break;
          }
          case 4:
          {
            for (x=0; x < image->columns; x+=2)
            {
              q->index=(*p >> 4) & 0xf;
              q->red=image->colormap[q->index].red;
              q->green=image->colormap[q->index].green;
              q->blue=image->colormap[q->index].blue;
              q->length=0;
              q++;
              q->index=(*p) & 0xf;
              q->red=image->colormap[q->index].red;
              q->green=image->colormap[q->index].green;
              q->blue=image->colormap[q->index].blue;
              q->length=0;
              q++;
              p++;
            }
            break;
          }
          case 8:
          {
            for (x=0; x < image->columns; x++)
            {
              q->index=(*p);
              q->red=image->colormap[q->index].red;
              q->green=image->colormap[q->index].green;
              q->blue=image->colormap[q->index].blue;
              q->length=0;
              q++;
              p++;
            }
            break;
          }
          default:
            break;
        }
        break;
      }
      case PHOTOMETRIC_PALETTE:
      {
        for (x=0; x < image->columns; x++)
        {
          q->index=(*p++);
          q->red=image->colormap[q->index].red;
          q->green=image->colormap[q->index].green;
          q->blue=image->colormap[q->index].blue;
          q->length=0;
          q++;
        }
        break;
      }
      default:
        break;
    }
  }
  free((char *) scanline);
  TIFFClose(file);
  if (image->class == PseudoClass)
    QuantizeImage(image,image->colors,8,False,True);
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
%    program_name:  Specifies the name of this program.
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
  (void) fprintf(stderr,"Usage: %s [-scene #] image.tiff image.miff\n\n",
    application_name);
  (void) fprintf(stderr,"Specify 'image.tiff' as '-' for standard input.\n");
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
  image=ReadTIFFImage(filename,True);
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
