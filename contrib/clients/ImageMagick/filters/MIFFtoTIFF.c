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
%                 Export MIFF image to a TIFF raster format.                  %
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
%  Permission to use, copy, modify,distribute , and sell this software and    %
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
%  export image.miff image.tiff
%
%  Specify 'image.miff' as '-' for standard input.  
%  Specify 'image.tiff' as '-' for standard output.
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
    (void) fprintf(stderr,"Can't continue,%s\n\n",message);
  (void) fprintf(stderr,"Usage: %s image.miff image.tiff\n\n",application_name);
  (void) fprintf(stderr,"Specify 'image.miff' as '-' for standard input.\n");
  (void) fprintf(stderr,"Specify 'image.tiff' as '-' for standard output.\n");
  exit(1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e T i f f I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteTiffImage writes a TIFF image to a file.
%
%  The format of the WriteTiffImage routine is:
%
%      status=WriteTiffImage(image,verbose)
%
%  A description of each parameter follows:
%
%    o status:  Function WriteTiffImage return True if the image is written.
%      False is returned is there is of a memory shortage or if the image
%      file cannot be opened for writing.
%
%    o image:  A pointer to a Image structure.
%
%    o  verbose:  A value greater than zero prints detailed information about
%       the image.
%
%
*/
static unsigned int WriteTiffImage(image,verbose)
Image
  *image;

unsigned int
  verbose;
{
#include "tiffio.h"

  register RunlengthPacket
    *p;

  register int
    i,
    j,
    x,
    y;

  register unsigned char
    *q;

  TIFF
    *file;

  unsigned char
    *scanline;

  unsigned short
    rows_per_strip;

  file=TIFFOpen(image->filename,"w");
  if (file == (TIFF *) NULL)
    exit(-1);
  /*
    Initialize TIFF fields.
  */
  TIFFSetField(file,TIFFTAG_BITSPERSAMPLE,8);
  TIFFSetField(file,TIFFTAG_COMPRESSION,COMPRESSION_NONE);
  TIFFSetField(file,TIFFTAG_DOCUMENTNAME,image->filename);
  TIFFSetField(file,TIFFTAG_IMAGEDESCRIPTION,"Converted from MIFF");
  TIFFSetField(file,TIFFTAG_IMAGELENGTH,image->rows);
  TIFFSetField(file,TIFFTAG_IMAGEWIDTH,image->columns);
  TIFFSetField(file,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
  TIFFSetField(file,TIFFTAG_PHOTOMETRIC,
    (image->class == DirectClass ? PHOTOMETRIC_RGB : PHOTOMETRIC_PALETTE));
  TIFFSetField(file,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
  TIFFSetField(file,TIFFTAG_SAMPLESPERPIXEL,
    (image->class == DirectClass ? 3 : 1));
  TIFFSetField(file,TIFFTAG_STRIPBYTECOUNTS,image->columns);
  rows_per_strip=8192/TIFFScanlineSize(file);
  if (rows_per_strip == 0)
    rows_per_strip=1;
  TIFFSetField(file,TIFFTAG_ROWSPERSTRIP,rows_per_strip);
  scanline=(unsigned char *) malloc((unsigned int) TIFFScanlineSize(file));
  if (scanline == (unsigned char *) NULL)
    Error("memory allocation error",(char *) NULL);
  if ((image->class == DirectClass) || (image->colors > 256))
    {
      /*
        Convert DirectClass packets to contiguous RGB scanlines.
      */
      p=image->pixels;
      q=scanline;
      x=0;
      y=0;
      for (i=0; i < image->packets; i++)
      {
        for (j=0; j <= p->length; j++)
        {
          *q++=p->red;
          *q++=p->green;
          *q++=p->blue;
          x++;
          if (x == image->columns)
            {
              if (TIFFWriteScanline(file,(char *) scanline,y) < 0)
                break;
              q=scanline;
              x=0;
              y++;
            }

        }
        p++;
      }
    }
  else
    {
      unsigned short
        blue[256],
        green[256],
        red[256];

      /*
        Initialize TIFF colormap.
      */
      for (i=0 ; i < image->colors; i++)
      {
        red[i]=(unsigned short) (image->colormap[i].red << 8);
        green[i]=(unsigned short) (image->colormap[i].green << 8);
        blue[i]=(unsigned short) (image->colormap[i].blue << 8);
      }
      TIFFSetField(file,TIFFTAG_COLORMAP,red,green,blue);
      /*
        Convert PseudoClass packets to contiguous colormap indexed scanlines.
      */
      p=image->pixels;
      q=scanline;
      x=0;
      y=0;
      for (i=0; i < image->packets; i++)
      {
        for (j=0; j <= p->length; j++)
        {
          *q++=(unsigned char) p->index;
          x++;
          if (x == image->columns)
            {
              if (TIFFWriteScanline(file,(char *) scanline,y) < 0)
                break;
              q=scanline;
              x=0;
              y++;
            }
        }
        p++;
      }
    }
  (void) free((char *) scanline);
  (void) TIFFFlushData(file);
  if (verbose)
    TIFFPrintDirectory(file,stderr,False,False,False);
  (void) TIFFClose(file);
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
  WriteTiffImage(image,True);
  DestroyImage(image);
}
