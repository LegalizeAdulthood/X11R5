/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                     IIIII  M   M   AAA   GGGG  EEEEE                        %
%                       I    MM MM  A   A G      E                            %
%                       I    M M M  AAAAA G  GG  EEE                          %
%                       I    M   M  A   A G   G  E                            %
%                     IIIII  M   M  A   A  GGGG  EEEEE                        %
%                                                                             %
%                                                                             %
%                    Utiltity Image Routines for Display                      %
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
%   A l l o c a t e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function AllocateImage allocates an Image structure and initializes each
%  field to a default value.
%
%  The format of the AllocateImage routine is:
%
%      allocated_image=AllocateImage()
%
%
*/
Image *AllocateImage()
{
  Image
    *allocated_image;

  /*
    Allocate image structure.
  */
  allocated_image=(Image *) malloc(sizeof(Image));
  if (allocated_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Initialize Image structure.
  */
  allocated_image->id=UnknownId;
  allocated_image->class=DirectClass;
  allocated_image->compression=RunlengthEncodedCompression;
  allocated_image->columns=0;
  allocated_image->rows=0;
  allocated_image->packets=0;
  allocated_image->colors=0;
  allocated_image->scene=0;
  allocated_image->colormap=(ColorPacket *) NULL;
  allocated_image->pixels=(RunlengthPacket *) NULL;
  allocated_image->comments=(char *) NULL;
  allocated_image->next=(Image *) NULL;
  return(allocated_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l i p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ClipImage creates a new image that is a subregion of an existing
%  one.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.  The pixels are copied from the source
%  image as defined by the region formed from x_offset, y_offset, width, and
%  height.
%
%  The format of the ClipImage routine is:
%
%      clipped_image=ClipImage(image,x_offset,y_offset,width,height)
%
%  A description of each parameter follows:
%
%    o clipped_image: Function ClipImage returns a pointer to the clipped
%      image.  A null image is returned if there is a a memory shortage or
%      if the image width or height is zero.
%
%    o image: The address of a structure of type Image.
%
%    o x_offset: An integer that specifies the column offset of the
%      clipped image.
%
%    o y_offset: An integer that specifies the row offset of the clipped
%      image.
%
%    o width: An integer that specifies the number of width in the clipped
%      image.
%
%    o height: An integer that specifies the number of height in the clipped
%      image.
%
%
*/
Image *ClipImage(image,x_offset,y_offset,width,height)
Image
  *image;

int
  x_offset,
  y_offset;

unsigned int
  width,
  height;
{
  Image
    *clipped_image,
    *CopyImage();

  register int
    x,
    y;

  register RunlengthPacket
    *p,
    *q;

  /*
    Check clip geometry.
  */
  if ((width*height) == 0)
    {
      Warning("unable to clip image","image size is zero");
      return((Image *) NULL);
    }
  if (((x_offset+(int) width) < 0) || ((y_offset+(int) height) < 0) ||
      (x_offset > (int) image->columns) || (y_offset > (int) image->rows))
    {
      Warning("unable to clip image","geometry does not contain image");
      return((Image *) NULL);
    }
  if ((x_offset+(int) width) > (int) image->columns)
    width=(unsigned int) ((int) image->columns-x_offset);
  if ((y_offset+(int) height) > (int) image->rows)
    height=(unsigned int) ((int) image->rows-y_offset);
  if (x_offset < 0)
    {
      width-=(unsigned int) (-x_offset);
      x_offset=0;
    }
  if (y_offset < 0)
    {
      height-=(unsigned int) (-y_offset);
      y_offset=0;
    }
  /*
    Initialize clipped image attributes.
  */
  clipped_image=CopyImage(image,width,height,False);
  if (clipped_image == (Image *) NULL)
    {
      Warning("unable to clip image","memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Skip pixels up to the clipped image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  for (x=0; x < (y_offset*image->columns+x_offset); x++)
    if (image->runlength > 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
  /*
    Extract clipped image.
  */
  q=clipped_image->pixels;
  for (y=0; y < clipped_image->rows; y++)
  {
    /*
      Transfer scanline.
    */
    for (x=0; x < clipped_image->columns; x++)
    {
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      q->red=p->red;
      q->green=p->green;
      q->blue=p->blue;
      q->index=p->index;
      q->length=0;
      q++;
    }
    /*
      Skip to next scanline.
    */
    for (x=0; x < (image->columns-clipped_image->columns); x++)
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
  }
  return(clipped_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p r e s s I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function CompressImage compresses an image to the minimum number of
%  runlength-encoded packets.
%
%  The format of the CompressImage routine is:
%
%      CompressImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
void CompressImage(image)
Image
  *image;
{
  register int
    i;

  register RunlengthPacket
    *p,
    *q;

  /*
    Compress image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  image->packets=0;
  q=image->pixels;
  q->length=MaxRunlength;
  for (i=0; i < (image->columns*image->rows); i++)
  {
    if (image->runlength > 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    if ((p->red == q->red) && (p->green == q->green) &&
        (p->blue == q->blue) && (q->length < MaxRunlength))
      q->length++;
    else
      {
        if (image->packets > 0)
          q++;
        image->packets++;
        q->red=p->red;
        q->green=p->green;
        q->blue=p->blue;
        q->index=p->index;
        q->length=0;
      }
  }
  image->pixels=(RunlengthPacket *)
    realloc((char *) image->pixels,image->packets*sizeof(RunlengthPacket));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o p y I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function CopyImage returns a copy of all fields of the input image.  The
%  the pixel memory is allocated but the pixel data is not copied.
%
%  The format of the CopyImage routine is:
%
%      copy_image=CopyImage(image,columns,rows,copy_pixels)
%
%  A description of each parameter follows:
%
%    o copy_image: Function CopyImage returns a pointer to the image after
%      copying.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o columns: An integer that specifies the number of columns in the copied
%      image.
%
%    o rows: An integer that specifies the number of rows in the copied
%      image.
%
%    o copy_pixels: Specifies whether the pixel data is copied.  Must be 
%      either True or False;
%
%
*/
Image *CopyImage(image,columns,rows,copy_pixels)
Image
  *image;

unsigned int
  columns,
  rows,
  copy_pixels;
{
  Image
    *copy_image;

  register int
    i;

  /*
    Allocate image structure.
  */
  copy_image=(Image *) malloc(sizeof(Image));
  if (copy_image == (Image *) NULL)
    return((Image *) NULL);
  *copy_image=(*image);
  copy_image->compression=NoCompression;
  copy_image->columns=columns;
  copy_image->rows=rows;
  if (image->colormap != (ColorPacket *) NULL)
    {
      /*
        Allocate and copy the image colormap.
      */
      copy_image->colormap=(ColorPacket *)
        malloc(image->colors*sizeof(ColorPacket));
      if (copy_image->colormap == (ColorPacket *) NULL)
        return((Image *) NULL);
      for (i=0; i < image->colors; i++)
        copy_image->colormap[i]=image->colormap[i];
    }
  /*
    Allocate the image pixels.
  */
  if (!copy_pixels)
    copy_image->packets=copy_image->columns*copy_image->rows;
  copy_image->pixels=(RunlengthPacket *)
    malloc(copy_image->packets*sizeof(RunlengthPacket));
  if (copy_image->pixels == (RunlengthPacket *) NULL)
    return((Image *) NULL);
  if (copy_pixels)
    {
      register RunlengthPacket
        *p,
        *q;

      if ((image->columns != columns) || (image->rows != rows))
        return((Image *) NULL);
      /*
        Copy the image pixels.
      */
      p=image->pixels;
      q=copy_image->pixels;
      for (i=0; i < image->packets; i++)
        *q++=(*p++);
    }
  if (image->comments != (char *) NULL)
    {
      /*
        Allocate and copy the image comments.
      */
      copy_image->comments=(char *)
        malloc((unsigned int) ((strlen(image->comments)+1)*sizeof(char)));
      if (copy_image->comments == (char *) NULL)
        return((Image *) NULL);
      (void) strcpy(copy_image->comments,image->comments);
    }
  return(copy_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function DestroyImage deallocates memory associated with an image.
%
%  The format of the DestroyImage routine is:
%
%      DestroyImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
void DestroyImage(image)
Image
  *image;
{
  /*
    Deallocate the image colormap.
  */
  if (image->colormap != (ColorPacket *) NULL)
    (void) free((char *) image->colormap);
  /*
    Deallocate the image pixels.
  */
  if (image->pixels != (RunlengthPacket *) NULL)
    (void) free((char *) image->pixels);
  /*
    Deallocate the image comments.
  */
  if (image->comments != (char *) NULL)
    (void) free((char *) image->comments);
  /*
    Deallocate the image structure.
  */
  (void) free((char *) image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     E n h a n c e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function EnhanceImage creates a new image that is a copy of an existing
%  one with the noise reduced.  It allocates the memory necessary for the new
%  Image structure and returns a pointer to the new image.
%
%  EnhanceImage does a weighted average of pixels in a 5x5 cell around each
%  target pixel.  Only pixels in the 5x5 cell that are within a RGB distance
%  threshold of the target pixel are averaged.
%
%  Weights assume that the importance of neighboring pixels is inversely
%  proportional to the square of their distance from the target pixel.
%
%  The scan only processes pixels that have a full set of neighbors.  Pixels
%  in the top, bottom, left, and right pairs of rows and columns are omitted
%  from the scan.
%
%  The format of the EnhanceImage routine is:
%
%      enhanced_image=EnhanceImage(image)
%
%  A description of each parameter follows:
%
%    o enhanced_image: Function EnhanceImage returns a pointer to the image
%      after it is enhanced.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Image *EnhanceImage(image)
Image
  *image;
{
#define Esum(weight) \
  red_distance=s->red-red; \
  green_distance=s->green-green; \
  blue_distance=s->blue-blue; \
  distance=red_distance*red_distance+green_distance*green_distance+ \
    blue_distance*blue_distance; \
  if (distance < Threshold) \
    { \
      total_red+=weight*(s->red); \
      total_green+=weight*(s->green); \
      total_blue+=weight*(s->blue); \
      total_weight+=weight; \
    } \
  s++;
#define Threshold  2500

  ColorPacket
    *scanline;

  Image
    *enhanced_image;

  int
    blue_distance,
    green_distance,
    red_distance;

  register ColorPacket
    *s,
    *s0,
    *s1,
    *s2,
    *s3,
    *s4;

  register RunlengthPacket
    *p,
    *q;

  register unsigned int
    x;

  unsigned char
    blue,
    green,
    red;

  unsigned int
    y;

  unsigned long
    distance,
    total_blue,
    total_green,
    total_red,
    total_weight;

  if ((image->columns < 5) || (image->rows < 5))
    {
      Warning("unable to enhance image","image size must exceed 4x4");
      return((Image *) NULL);
    }
  /*
    Initialize enhanced image attributes.
  */
  enhanced_image=CopyImage(image,image->columns,image->rows,False);
  if (enhanced_image == (Image *) NULL)
    {
      Warning("unable to enhance image","memory allocation failed");
      return((Image *) NULL);
    }
  enhanced_image->class=DirectClass;
  /*
    Allocate scan line buffer for 5 rows of the image.
  */
  scanline=(ColorPacket *) malloc(5*image->columns*sizeof(ColorPacket));
  if (scanline == (ColorPacket *) NULL)
    {
      Warning("unable to enhance image","memory allocation failed");
      DestroyImage(enhanced_image);
      return((Image *) NULL);
    }
  /*
    Read the first 4 rows of the image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < (image->columns*4); x++)
  {
    if (image->runlength > 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    s->red=p->red;
    s->green=p->green;
    s->blue=p->blue;
    s->index=p->index;
    s++;
  }
  /*
    Dump first 2 scanlines of image.
  */
  q=enhanced_image->pixels;
  s=scanline;
  for (x=0; x < (2*image->columns); x++)
  {
    q->red=s->red;
    q->green=s->green;
    q->blue=s->blue;
    q->index=s->index;
    q->length=0;
    q++;
    s++;
  }
  /*
    Enhance each row.
  */
  for (y=2; y < (image->rows-2); y++)
  {
    /*
      Initialize sliding window pointers.
    */
    s0=scanline+image->columns*((y-2) % 5);
    s1=scanline+image->columns*((y-1) % 5);
    s2=scanline+image->columns*(y % 5);
    s3=scanline+image->columns*((y+1) % 5);
    s4=scanline+image->columns*((y+2) % 5);
    /*
      Read another scan line.
    */
    s=s4;
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      s->red=p->red;
      s->green=p->green;
      s->blue=p->blue;
      s->index=p->index;
      s++;
    }
    /*
      Transfer first 2 pixels of the scanline.
    */
    s=s2;
    for (x=0; x < 2; x++)
    {
      q->red=0;
      q->green=0;
      q->blue=0;
      q->index=0;
      q->length=0;
      q++;
      s++;
    }
    for (x=2; x < (image->columns-2); x++)
    {
      /*
        Compute weighted average of target pixel color components.
      */
      total_red=0;
      total_green=0;
      total_blue=0;
      total_weight=0;
      s=s2+2;
      red=s->red;
      green=s->green;
      blue=s->blue;
      s=s0;
      Esum(5);  Esum(8);  Esum(10); Esum(8);  Esum(5);
      s=s1;
      Esum(8);  Esum(20); Esum(40); Esum(20); Esum(8);
      s=s2;
      Esum(10); Esum(40); Esum(80); Esum(40); Esum(10);
      s=s3;
      Esum(8);  Esum(20); Esum(40); Esum(20); Esum(8);
      s=s4;
      Esum(5);  Esum(8);  Esum(10); Esum(8);  Esum(5);
      q->red=(unsigned char) ((total_red+(total_weight >> 1)-1)/total_weight);
      q->green=
        (unsigned char) ((total_green+(total_weight >> 1)-1)/total_weight);
      q->blue=(unsigned char) ((total_blue+(total_weight >> 1)-1)/total_weight);
      q->index=0;
      q->length=0;
      q++;
      s0++;
      s1++;
      s2++;
      s3++;
      s4++;
    }
    /*
      Transfer last 2 pixels of the scanline.
    */
    s=s2;
    for (x=0; x < 2; x++)
    {
      q->red=s->red;
      q->green=s->green;
      q->blue=s->blue;
      q->index=s->index;
      q->length=0;
      q++;
      s++;
    }
  }
  /*
    Dump last 2 scanlines of pixels.
  */
  s=scanline+image->columns*(y % 3);
  for (x=0; x < (2*image->columns); x++)
  {
    q->red=s->red;
    q->green=s->green;
    q->blue=s->blue;
    q->index=s->index;
    q->length=0;
    q++;
    s++;
  }
  (void) free((char *) scanline);
  return(enhanced_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     G a m m a I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure GammaImage converts the reference image to gamma corrected colors.
%
%  The format of the GammaImage routine is:
%
%      GammaImage(image,gamma)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o gamma: A double precision value indicating the level of gamma
%      correction.
%
%
*/
void GammaImage(image,gamma)
Image
  *image;

double
  gamma;
{
  register int
    i;

  register RunlengthPacket
    *p;

  unsigned char
    gamma_map[MaxRgb+1];

  /*
    Initialize gamma table.
  */
  for (i=0; i <= MaxRgb; i++)
    gamma_map[i]=(unsigned char)
      ((pow((double) i/MaxRgb,1.0/gamma)*MaxRgb)+0.5);
  switch (image->class)
  {
    case DirectClass:
    {
      /*
        Gamma-correct DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        p->red=gamma_map[p->red];
        p->green=gamma_map[p->green];
        p->blue=gamma_map[p->blue];
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      register unsigned short
        index;

      /*
        Gamma-correct PseudoClass image.
      */
      for (i=0; i < image->colors; i++)
      {
        image->colormap[i].red=gamma_map[image->colormap[i].red];
        image->colormap[i].green=gamma_map[image->colormap[i].green];
        image->colormap[i].blue=gamma_map[image->colormap[i].blue];
      }
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        index=p->index;
        p->red=image->colormap[index].red;
        p->green=image->colormap[index].green;
        p->blue=image->colormap[index].blue;
        p++;
      }
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     G r a y I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure GrayImage converts the reference image to gray scale colors.
%
%  The format of the GrayImage routine is:
%
%      GrayImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
void GrayImage(image)
Image
  *image;
{
  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned char
    gray_value;

  switch (image->class)
  {
    case DirectClass:
    {
      /*
        Convert DirectClass packets to grayscale.
      */
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        gray_value=Intensity(*p);
        p->red=gray_value;
        p->green=gray_value;
        p->blue=gray_value;
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      register unsigned short
        index;

      /*
        Convert PseudoClass packets to grayscale.
      */
      for (i=0; i < image->colors; i++)
      {
        gray_value=Intensity(image->colormap[i]);
        image->colormap[i].red=gray_value;
        image->colormap[i].green=gray_value;
        image->colormap[i].blue=gray_value;
      }
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        index=p->index;
        p->red=image->colormap[index].red;
        p->green=image->colormap[index].green;
        p->blue=image->colormap[index].blue;
        p++;
      }
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I n v e r s e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure InverseImage inverses the colors in the reference image.
%
%  The format of the InverseImage routine is:
%
%      InverseImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
void InverseImage(image)
Image
  *image;
{
  register int
    i;

  register RunlengthPacket
    *p;

  switch (image->class)
  {
    case DirectClass:
    {
      /*
        Inverse DirectClass packets.
      */
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        p->red=(~p->red);
        p->green=(~p->green);
        p->blue=(~p->blue);
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      register unsigned short
        index;

      /*
        Inverse PseudoClass packets.
      */
      for (i=0; i < image->colors; i++)
      {
        image->colormap[i].red=(~image->colormap[i].red);
        image->colormap[i].green=(~image->colormap[i].green);
        image->colormap[i].blue=(~image->colormap[i].blue);
      }
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        index=p->index;
        p->red=image->colormap[index].red;
        p->green=image->colormap[index].green;
        p->blue=image->colormap[index].blue;
        p++;
      }
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L i n e a r C o l o r m a p                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function LinearColormap sorts the colormap of a PseudoClass image by
%  linearly increasing intensity.
%
%  The format of the LinearColormap routine is:
%
%      LinearColormap(image)
%
%  A description of each parameter follows:
%
%    o image: A pointer to a Image structure.
%
%
*/
static int LinearCompare(color_1,color_2)
register ColorPacket
  *color_1,
  *color_2;
{
  return((int) Intensity(*color_1)-(int) Intensity(*color_2));
}

void LinearColormap(image)
Image
  *image;
{
  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned short
    index;

  unsigned int
    *pixels;

  if (image->class == DirectClass)
    return;
  /*
    Allocate memory for pixel indexes.
  */
  pixels=(unsigned int *) malloc(image->colors*sizeof(unsigned int));
  if (pixels == (unsigned int *) NULL)
    {
      Warning("unable to sort colormap","memory allocation failed");
      return;
    }
  /*
    Assign index values to colormap entries.
  */
  for (i=0; i < image->colors; i++)
    image->colormap[i].index=i;
  /*
    Sort image colormap by increasing intensity.
  */
  qsort((char *) image->colormap,(int) image->colors,sizeof(ColorPacket),
    LinearCompare);
  /*
    Update image colormap indexes to sorted colormap order.
  */
  for (i=0; i < image->colors; i++)
    pixels[image->colormap[i].index]=i;
  p=image->pixels;
  for (i=0; i < image->packets; i++)
  {
    index=pixels[p->index];
    p->red=image->colormap[index].red;
    p->green=image->colormap[index].green;
    p->blue=image->colormap[index].blue;
    p->index=index;
    p++;
  }
  (void) free((char *) pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     N o i s y I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function NoisyImage creates a new image that is a copy of an existing
%  one with the noise reduced with a noise peak elimination filter.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The principal function of noise peak elimination filter is to smooth the
%  objects within an image without losing edge information and without
%  creating undesired structures.  The central idea of the algorithm is to
%  replace a pixel with its next neighbor in value within a 3 x 3 window,
%  if this pixel has been found to be noise.  A pixel is defined as noise
%  if and only if this pixel is a maximum or minimum within the 3 x 3
%  window.
%
%  The format of the NoisyImage routine is:
%
%      noisy_image=NoisyImage(image)
%
%  A description of each parameter follows:
%
%    o noisy_image: Function NoisyImage returns a pointer to the image after
%      the noise is reduced.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
static int NoisyCompare(color_1,color_2)
register ColorPacket
  *color_1,
  *color_2;
{
  return((int) Intensity(*color_1)-(int) Intensity(*color_2));
}

Image *NoisyImage(image)
Image
  *image;
{
  ColorPacket
    pixel,
    *scanline,
    window[9];

  Image
    *noisy_image;

  int
    i;

  register ColorPacket
    *s,
    *s0,
    *s1,
    *s2;

  register RunlengthPacket
    *p,
    *q;

  register unsigned int
    x;

  unsigned int
    y;

  if ((image->columns < 3) || (image->rows < 3))
    {
      Warning("unable to reduce noise","the image size must exceed 2x2");
      return((Image *) NULL);
    }
  /*
    Initialize noisy image attributes.
  */
  noisy_image=CopyImage(image,image->columns,image->rows,False);
  if (noisy_image == (Image *) NULL)
    {
      Warning("unable to reduce noise","memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Allocate scanline buffer for 3 rows of the image.
  */
  scanline=(ColorPacket *) malloc(3*image->columns*sizeof(ColorPacket));
  if (scanline == (ColorPacket *) NULL)
    {
      Warning("unable to reduce noise","memory allocation failed");
      DestroyImage(noisy_image);
      return((Image *) NULL);
    }
  /*
    Preload the first 2 rows of the image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < (2*image->columns); x++)
  {
    if (image->runlength > 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    s->red=p->red;
    s->green=p->green;
    s->blue=p->blue;
    s->index=p->index;
    s++;
  }
  /*
    Dump first scanline of image.
  */
  q=noisy_image->pixels;
  s=scanline;
  for (x=0; x < image->columns; x++)
  {
    q->red=s->red;
    q->green=s->green;
    q->blue=s->blue;
    q->index=s->index;
    q->length=0;
    q++;
    s++;
  }
  /*
    Reduce noise in each row.
  */
  for (y=1; y < (image->rows-1); y++)
  {
    /*
      Initialize sliding window pointers.
    */
    s0=scanline+image->columns*((y-1) % 3);
    s1=scanline+image->columns*(y % 3);
    s2=scanline+image->columns*((y+1) % 3);
    /*
      Read another scan line.
    */
    s=s2;
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      s->red=p->red;
      s->green=p->green;
      s->blue=p->blue;
      s->index=p->index;
      s++;
    }
    /*
      Transfer first pixel of the scanline.
    */
    s=s1;
    q->red=s->red;
    q->green=s->green;
    q->blue=s->blue;
    q->index=s->index;
    q->length=0;
    q++;
    for (x=1; x < (image->columns-1); x++)
    {
      /*
        Sort window pixels by increasing intensity.
      */
      s=s0;
      window[0]=(*s++);
      window[1]=(*s++);
      window[2]=(*s++);
      s=s1;
      window[3]=(*s++);
      window[4]=(*s++);
      window[5]=(*s++);
      s=s2;
      window[6]=(*s++);
      window[7]=(*s++);
      window[8]=(*s++);
      qsort((char *) window,9,sizeof(ColorPacket),NoisyCompare);
      pixel=(*(s1+1));
      if (Intensity(pixel) == Intensity(window[0]))
        {
          /*
            Pixel is minimum noise; replace with next neighbor in value.
          */
          for (i=1; i < 8; i++)
            if (Intensity(window[i]) != Intensity(window[0]))
              break;
          pixel=window[i];
        }
      else
        if (Intensity(pixel) == Intensity(window[8]))
          {
            /*
              Pixel is maximum noise; replace with next neighbor in value.
            */
            for (i=7; i > 0; i--)
              if (Intensity(window[i]) != Intensity(window[8]))
                break;
            pixel=window[i];
          }
      q->red=pixel.red;
      q->green=pixel.green;
      q->blue=pixel.blue;
      q->index=pixel.index;
      q->length=0;
      q++;
      s0++;
      s1++;
      s2++;
    }
    /*
      Transfer last pixel of the scanline.
    */
    s=s1;
    q->red=s->red;
    q->green=s->green;
    q->blue=s->blue;
    q->index=s->index;
    q->length=0;
    q++;
  }
  /*
    Dump last scanline of pixels.
  */
  s=scanline+image->columns*(y % 3);
  for (x=0; x < image->columns; x++)
  {
    q->red=s->red;
    q->green=s->green;
    q->blue=s->blue;
    q->index=s->index;
    q->length=0;
    q++;
    s++;
  }
  (void) free((char *) scanline);
  return(noisy_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     N o r m a l i z e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure NormalizeImage normalizes the pixel values to span the full
%  range of color values.
%
%  The format of the NormalizeImage routine is:
%
%      NormalizeImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
void NormalizeImage(image)
Image
  *image;
{
  double
    factor;

  register int
    i;

  register int
    max_intensity,
    min_intensity,
    intensity;

  register RunlengthPacket
    *p;

  unsigned char
    normalize_map[MaxRgb+1];

  switch (image->class)
  {
    case DirectClass:
    {
      /*
        Determine min and max intensity.
      */
      p=image->pixels;
      min_intensity=p->red;
      max_intensity=p->red;
      for (i=0; i < image->packets; i++)
      {
        if (p->red < min_intensity)
          min_intensity=p->red;
        else
          if (p->red > max_intensity)
            max_intensity=p->red;
        if (p->green < min_intensity)
          min_intensity=p->green;
        else
          if (p->green > max_intensity)
            max_intensity=p->green;
        if (p->blue < min_intensity)
          min_intensity=p->blue;
        else
          if (p->blue > max_intensity)
            max_intensity=p->blue;
        p++;
      }
      if ((min_intensity == 0) && (max_intensity == MaxRgb))
        break;
      /*
        Initialize normalize table.
      */
      factor=(double) MaxRgb/(double) (max_intensity-min_intensity);
      for (i=min_intensity; i <= max_intensity; i++)
      {
        intensity=(i-min_intensity)*factor;
        if (intensity < 0)
          intensity=0;
        else
          if (intensity > MaxRgb)
            intensity=MaxRgb;
        normalize_map[i]=(unsigned char) intensity;
      }
      /*
        Normalize DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        p->red=normalize_map[p->red];
        p->green=normalize_map[p->green];
        p->blue=normalize_map[p->blue];
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      register unsigned short
        index;

      /*
        Determine min and max intensity.
      */
      min_intensity=image->colormap[0].red;
      max_intensity=image->colormap[0].red;
      for (i=0; i < image->colors; i++)
      {
        if (image->colormap[i].red < min_intensity)
          min_intensity=image->colormap[i].red;
        else
          if (image->colormap[i].red > max_intensity)
            max_intensity=image->colormap[i].red;
        if (image->colormap[i].green < min_intensity)
          min_intensity=image->colormap[i].green;
        else
          if (image->colormap[i].green > max_intensity)
            max_intensity=image->colormap[i].green;
        if (image->colormap[i].blue < min_intensity)
          min_intensity=image->colormap[i].blue;
        else
          if (image->colormap[i].blue > max_intensity)
            max_intensity=image->colormap[i].blue;
      }
      if ((min_intensity == 0) && (max_intensity == MaxRgb))
        break;
      /*
        Initialize normalize table.
      */
      factor=(double) MaxRgb/(double) (max_intensity-min_intensity);
      for (i=min_intensity; i <= max_intensity; i++)
      {
        intensity=(i-min_intensity)*factor;
        if (intensity < 0)
          intensity=0;
        else
          if (intensity > MaxRgb)
            intensity=MaxRgb;
        normalize_map[i]=(unsigned char) intensity;
      }
      /*
        Normalize PseudoClass image.
      */
      for (i=0; i < image->colors; i++)
      {
        image->colormap[i].red=normalize_map[image->colormap[i].red];
        image->colormap[i].green=normalize_map[image->colormap[i].green];
        image->colormap[i].blue=normalize_map[image->colormap[i].blue];
      }
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        index=p->index;
        p->red=image->colormap[index].red;
        p->green=image->colormap[index].green;
        p->blue=image->colormap[index].blue;
        p++;
      }
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P r i n t I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function PrintImage translates a MIFF image to encapsulated Postscript for
%  printing.  If the supplied geometry is null, the image is centered on the
%  Postscript page.  Otherwise, the image is positioned as specified by the
%  geometry.
%
%  The format of the PrintImage routine is:
%
%      status=PrintImage(image,geometry)
%
%  A description of each parameter follows:
%
%    o status: Function PrintImage return True if the image is printed.
%      False is returned if the image file cannot be opened for printing.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o geometry: A pointer to a standard window geometry string.
%
%
*/
unsigned int PrintImage(image,geometry)
Image
  *image;

char
  *geometry;
{
#define PageSideMargin 16
#define PageTopMargin 92
#define PageWidth  612
#define PageHeight 792

  static char
    *Postscript[]=
    {
      "%",
      "% Display a color image.  The image is displayed in color on",
      "% Postscript viewers or printers that support color, otherwise",
      "% it is displayed as grayscale.",
      "%",
      "/buffer 512 string def",
      "/byte 1 string def",
      "/color_packet 3 string def",
      "/compression 1 string def",
      "/gray_packet 1 string def",
      "/pixels 768 string def",
      "",
      "/DirectClassPacket",
      "{",
      "  %",
      "  % Get a DirectClass packet.",
      "  %",
      "  % Parameters: ",
      "  %   red.",
      "  %   green.",
      "  %   blue.",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  currentfile color_packet readhexstring pop pop",
      "  compression 0 gt",
      "  {",
      "    /number_pixels 3 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add 3 mul def",
      "  } ifelse",
      "  0 3 number_pixels 1 sub",
      "  {",
      "    pixels exch color_packet putinterval",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/DirectClassImage",
      "{",
      "  %",
      "  % Display a DirectClass image.",
      "  %",
      "  systemdict /colorimage known",
      "  {",
      "    columns rows 8",
      "    [",
      "      columns 0 0",
      "      rows neg 0 rows",
      "    ]",
      "    { DirectClassPacket } false 3 colorimage",
      "  }",
      "  {",
      "    %",
      "    % No colorimage operator;  convert to grayscale.",
      "    %",
      "    columns rows 8",
      "    [",
      "      columns 0 0",
      "      rows neg 0 rows",
      "    ]",
      "    { GrayDirectClassPacket } image",
      "  } ifelse",
      "} bind def",
      "",
      "/GrayDirectClassPacket",
      "{",
      "  %",
      "  % Get a DirectClass packet;  convert to grayscale.",
      "  %",
      "  % Parameters: ",
      "  %   red",
      "  %   green",
      "  %   blue",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  currentfile color_packet readhexstring pop pop",
      "  color_packet 0 get 0.299 mul",
      "  color_packet 1 get 0.587 mul add",
      "  color_packet 2 get 0.114 mul add",
      "  cvi",
      "  /gray_packet exch def",
      "  compression 0 gt",
      "  {",
      "    /number_pixels 1 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add def",
      "  } ifelse",
      "  0 1 number_pixels 1 sub",
      "  {",
      "    pixels exch gray_packet put",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/GrayPseudoClassPacket",
      "{",
      "  %",
      "  % Get a PseudoClass packet;  convert to grayscale.",
      "  %",
      "  % Parameters: ",
      "  %   index: index into the colormap.",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  currentfile byte readhexstring pop 0 get",
      "  /offset exch 3 mul def",
      "  /color_packet colormap offset 3 getinterval def",
      "  color_packet 0 get 0.299 mul",
      "  color_packet 1 get 0.587 mul add",
      "  color_packet 2 get 0.114 mul add",
      "  cvi",
      "  /gray_packet exch def",
      "  compression 0 gt",
      "  {",
      "    /number_pixels 1 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add def",
      "  } ifelse",
      "  0 1 number_pixels 1 sub",
      "  {",
      "    pixels exch gray_packet put",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/PseudoClassPacket",
      "{",
      "  %",
      "  % Get a PseudoClass packet.",
      "  %",
      "  % Parameters: ",
      "  %   index: index into the colormap.",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  %",
      "  currentfile byte readhexstring pop 0 get",
      "  /offset exch 3 mul def",
      "  /color_packet colormap offset 3 getinterval def",
      "  compression 0 gt",
      "  {",
      "    /number_pixels 3 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add 3 mul def",
      "  } ifelse",
      "  0 3 number_pixels 1 sub",
      "  {",
      "    pixels exch color_packet putinterval",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/PseudoClassImage",
      "{",
      "  %",
      "  % Display a PseudoClass image.",
      "  %",
      "  % Parameters: ",
      "  %   colors: number of colors in the colormap.",
      "  %   colormap: red, green, blue color packets.",
      "  %",
      "  currentfile buffer readline pop",
      "  token pop /colors exch def pop",
      "  /colors colors 3 mul def",
      "  /colormap colors string def",
      "  currentfile colormap readhexstring pop pop",
      "  systemdict /colorimage known",
      "  {",
      "    columns rows 8",
      "    [",
      "      columns 0 0",
      "      rows neg 0 rows",
      "    ]",
      "    { PseudoClassPacket } false 3 colorimage",
      "  }",
      "  {",
      "    %",
      "    % No colorimage operator;  convert to grayscale.",
      "    %",
      "    columns rows 8",
      "    [",
      "      columns 0 0",
      "      rows neg 0 rows",
      "    ]",
      "    { GrayPseudoClassPacket } image",
      "  } ifelse",
      "} bind def",
      "",
      "/DisplayImage",
      "{",
      "  %",
      "  % Display a DirectClass or PseudoClass image.",
      "  %",
      "  % Parameters: ",
      "  %   x & y translation.",
      "  %   x & y scale.",
      "  %   image columns & rows.",
      "  %   class: 0-DirectClass or 1-PseudoClass.",
      "  %   compression: 0-RunlengthEncodedCompression or 1-NoCompression.",
      "  %   hex color packets.",
      "  %",
      "  gsave",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  x y translate",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  x y scale",
      "  currentfile buffer readline pop",
      "  token pop /columns exch def",
      "  token pop /rows exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /class exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /compression exch def pop",
      "  class 0 gt { PseudoClassImage } { DirectClassImage } ifelse",
      "  grestore",
      "  showpage",
      "} bind def",
      "",
      "DisplayImage",
      NULL
    };

  char
    **q;

  int
    x,
    y;

  register RunlengthPacket
    *p;

  register int
    i,
    j;

  unsigned int
    height,
    width;

  /*
    Open output image file.
  */
  if (*image->filename == '-')
    image->file=stdout;
  else
    image->file=fopen(image->filename,"w");
  if (image->file == (FILE *) NULL)
    {
      (void) fprintf(stderr,"%s: unable to print image, cannot open %s.\n",
        application_name,image->filename);
      return(False);
    }
  if (geometry != (char *) NULL)
    {
      /*
        User specified Postscript page position.
      */
      x=0;
      y=0;
      width=image->columns;
      height=image->rows;
      (void) XParseGeometry(geometry,&x,&y,&width,&height);
    }
  else
    {
      int
        delta_x,
        delta_y;

      unsigned long
        scale_factor;

      /*
        Scale image to size of Postscript page.
      */
      scale_factor=((PageWidth-(2*PageSideMargin)) << 14)/image->columns;
      if (scale_factor > (((PageHeight-(2*PageTopMargin)) << 14)/image->rows))
        scale_factor=((PageHeight-(2*PageTopMargin)) << 14)/image->rows;
      width=(image->columns*scale_factor+8191) >> 14;
      height=(image->rows*scale_factor+8191) >> 14;
      /*
        Center image on Postscript page.
      */
      delta_x=PageWidth-(width+(2*PageSideMargin));
      delta_y=PageHeight-(height+(2*PageTopMargin));
      if (delta_x >= 0)
        x=delta_x/2+PageSideMargin;
      else
        x=PageSideMargin;
      if (delta_y >= 0)
        y=delta_y/2+PageTopMargin;
      else
        y=PageTopMargin;
    }
  /*
    Output encapsulated Postscript header.
  */
  (void) fprintf(image->file,"%%!PS-Adobe-2.0 EPSF-2.0\n");
  (void) fprintf(image->file,"%%%%BoundingBox: %d %d %d %d\n",x,y,x+width,
    y+height);
  (void) fprintf(image->file,"%%%%Creator: ImageMagick\n");
  (void) fprintf(image->file,"%%%%Title: %s\n",image->filename);
  (void) fprintf(image->file,"%%%%EndComments\n");
  /*
    Output encapsulated Postscript commands.
  */
  for (q=Postscript; *q; q++)
    (void) fprintf(image->file,"%s\n",*q);
  /*
    Output image data.
  */
  x=0;
  p=image->pixels;
  switch (image->class)
  {
    case DirectClass:
    {
      if (image->compression == RunlengthEncodedCompression)
        {
          /*
            Compress the image.
          */
          CompressImage(image);
          if (image->packets > ((image->columns*image->rows*3) >> 2))
            image->compression=NoCompression;
        }
      (void) fprintf(image->file,"%d %d\n%d %d\n%d %d\n%d\n%d\n",x,y,width,
        height,image->columns,image->rows,(image->class == PseudoClass),
        image->compression == NoCompression);
      switch (image->compression)
      {
        case RunlengthEncodedCompression:
        default:
        {
          /*
            Dump runlength-encoded DirectColor packets.
          */
          for (i=0; i < image->packets; i++)
          {
            x++;
            (void) fprintf(image->file,"%02x%02x%02x%02x",p->red,p->green,
              p->blue,p->length);
            if (x == 9)
              {
                x=0;
                (void) fprintf(image->file,"\n");
              }
            p++;
          }
          break;
        }
        case NoCompression:
        {
          /*
            Dump uncompressed DirectColor packets.
          */
          for (i=0; i < image->packets; i++)
          {
            for (j=0; j <= p->length; j++)
            {
              x++;
              (void) fprintf(image->file,"%02x%02x%02x",p->red,p->green,
                p->blue);
              if (x == 12)
                {
                  x=0;
                  (void) fprintf(image->file,"\n");
                }
            }
            p++;
          }
          break;
        }
      }
      break;
    }
    case PseudoClass:
    {
      if (image->compression == RunlengthEncodedCompression)
        {
          /*
            Compress the image.
          */
          CompressImage(image);
          if (image->packets > ((image->columns*image->rows) >> 1))
            image->compression=NoCompression;
        }
      (void) fprintf(image->file,"%d %d\n%d %d\n%d %d\n%d\n%d\n",x,y,width,
        height,image->columns,image->rows,(image->class == PseudoClass),
        image->compression == NoCompression);
      /*
        Dump number of colors and colormap.
      */
      (void) fprintf(image->file,"%d\n",image->colors);
      for (i=0; i < image->colors; i++)
        (void) fprintf(image->file,"%02x%02x%02x\n",image->colormap[i].red,
          image->colormap[i].green,image->colormap[i].blue);
      switch (image->compression)
      {
        case RunlengthEncodedCompression:
        default:
        {
          /*
            Dump runlength-encoded PseudoColor packets.
          */
          for (i=0; i < image->packets; i++)
          {
            x++;
            (void) fprintf(image->file,"%02x%02x",p->index,p->length);
            if (x == 18)
              {
                x=0;
                (void) fprintf(image->file,"\n");
              }
            p++;
          }
          break;
        }
        case NoCompression:
        {
          /*
            Dump uncompressed PseudoColor packets.
          */
          for (i=0; i < image->packets; i++)
          {
            for (j=0; j <= p->length; j++)
            {
              x++;
              (void) fprintf(image->file,"%02x",p->index);
              if (x == 36)
                {
                  x=0;
                  (void) fprintf(image->file,"\n");
                }
            }
            p++;
          }
        }
        break;
      }
    }
  }
  (void) fprintf(image->file,"\n\n");
  (void) fprintf(image->file,"%%%%Trailer\n");
  if (image->file != stdin)
    (void) fclose(image->file);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d D a t a                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadData reads data from the image file and returns it.  If it
%  cannot read the requested number of items, False is returned indicating
%  an error.
%
%  The format of the ReadData routine is:
%
%      status=ReadData(data,size,number_items,file)
%
%  A description of each parameter follows:
%
%    o status:  Function ReadData returns True if all the data requested
%      is obtained without error, otherwise False.
%
%    o data:  Specifies an area to place the information reuested from
%      the file.
%
%    o size:  Specifies an integer representing the length of an
%      individual item to be read from the file.
%
%    o numer_items:  Specifies an integer representing the number of items
%      to read from the file.
%
%    o file:  Specifies a file to read the data.
%
%
*/
unsigned int ReadData(data,size,number_items,file)
char
  *data;

int
  size,
  number_items;

FILE
  *file;
{
  size*=number_items;
  while (size > 0)
  {
    number_items=fread(data,1,size,file);
    if (number_items <= 0)
      return(False);
    size-=number_items;
    data+=number_items;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I m a g e                                                         %
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
%    o image: Function ReadImage returns a pointer to the image after reading.
%      A null image is returned if there is a a memory shortage or if the
%      image cannot be read.
%
%    o filename: Specifies the name of the image to read.
%
%
*/
Image *ReadImage(filename)
char
  *filename;
{
#define MaxKeywordLength  256

  char
    keyword[MaxKeywordLength],
    value[MaxKeywordLength];

  Image
    *image;

  long int
    count;

  register int
    c,
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *pixels;

  unsigned int
    packets,
    packet_size,
    status;

  /*
    Allocate image structure.
  */
  image=AllocateImage();
  if (image == (Image *) NULL)
    return((Image *) NULL);
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
          command[1024];

        /*
          Image file is compressed-- uncompress it.
        */
        (void) sprintf(command,"uncompress -c %s",image->filename);
        image->file=(FILE *) popen(command,"r");
      }
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      (void) DestroyImage(image);
      return((Image *) NULL);
    }
  image->compression=NoCompression;
  /*
    Decode image header;  header terminates one character beyond a ':'.
  */
  do { c=fgetc(image->file); } while (isspace(c));
  while ((c > 0) && (c != ':'))
  {
    register char
      *p;

    if (c == '{')
      {
        register char
          *q;

        /*
          Comment.
        */
        if (image->comments == (char *) NULL)
          {
            /*
              Allocate initial memory for a comment.
            */
            image->comments=(char *) malloc(256);
            if (image->comments == (char *) NULL)
              {
                Warning("unable to read image","memory allocation failed");
                DestroyImage(image);
                return((Image *) NULL);
              }
            *image->comments=(char) NULL;
          }
        p=image->comments+strlen(image->comments);
        q=p+(255-(strlen(image->comments) % 256));
        c=fgetc(image->file);
        while ((c > 0) && (c != '}'))
        {
          if (p == q)
            {
              /*
                Allocate more memory for the comment.
              */
              image->comments=(char *) realloc((char *) image->comments,
                (unsigned int) (strlen(image->comments)+256));
              if (image->comments == (char *) NULL)
                {
                  Warning("unable to read image","memory allocation failed");
                  DestroyImage(image);
                  return((Image *) NULL);
                }
              q=p+255;
            }
          *p++=c;
          c=fgetc(image->file);
        }
        *p=(char) NULL;
        c=fgetc(image->file);
      }
    else
      if (isalnum(c))
        {
          /*
            Determine a keyword and its value.
          */
          p=keyword;
          do
          {
            if ((p-keyword) < (MaxKeywordLength-1))
              *p++=c;
            c=fgetc(image->file);
          } while (isalnum(c));
          *p=(char) NULL;
          while (isspace(c) || (c == '='))
            c=fgetc(image->file);
          p=value;
          while (isalnum(c))
          {
            if ((p-value) < (MaxKeywordLength-1))
              *p++=c;
            c=fgetc(image->file);
          }
          *p=(char) NULL;
          /*
            Assign a value to the specified keyword.
          */
          if (strcmp(keyword,"class") == 0)
            if (strcmp(value,"PseudoClass") == 0)
              image->class=PseudoClass;
            else
              if (strcmp(value,"DirectClass") == 0)
                image->class=DirectClass;
              else
                image->class=UnknownClass;
          if (strcmp(keyword,"compression") == 0)
            if (strcmp(value,"QEncoded") == 0)
              image->compression=QEncodedCompression;
            else
              if (strcmp(value,"RunlengthEncoded") == 0)
                image->compression=RunlengthEncodedCompression;
              else
                image->compression=UnknownCompression;
          if (strcmp(keyword,"colors") == 0)
            image->colors=(unsigned int) atoi(value);
          if (strcmp(keyword,"columns") == 0)
            image->columns=(unsigned int) atoi(value);
          if (strcmp(keyword,"id") == 0)
            if ((strcmp(value,"ImageMagick") == 0) ||
                (strcmp(value,"XImager") == 0))
              image->id=ImageMagickId;
            else
              image->id=UnknownId;
          if (strcmp(keyword,"packets") == 0)
            image->packets=(unsigned int) atoi(value);
          if (strcmp(keyword,"rows") == 0)
            image->rows=(unsigned int) atoi(value);
          if (strcmp(keyword,"scene") == 0)
            image->scene=(unsigned int) atoi(value);
        }
      else
        c=fgetc(image->file);
    while (isspace(c))
      c=fgetc(image->file);
  }
  (void) fgetc(image->file);
  /*
    Verify that required image information is valid.
  */
  if ((image->id == UnknownId) ||
      (image->class == UnknownClass) ||
      (image->compression == UnknownCompression) ||
      ((image->columns*image->rows) == 0))
    {
      Warning("incorrect image header in file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  if ((image->columns*image->rows) > MaxImageSize)
    {
      Warning("unable to read image","image size too large");
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Read image from disk and convert to runlength-encoded packets.
  */
  switch (image->class)
  {
    case DirectClass:
    {
      /*
        Transfer pixels to DirectClass image pixel buffer.
      */
      switch (image->compression)
      {
        case NoCompression:
        {
          /*
            Allocate image buffers and read image from disk.
          */
          image->packets=image->columns*image->rows;
          packet_size=3;
          pixels=(unsigned char *)
            malloc((unsigned int) image->packets*packet_size);
          image->pixels=(RunlengthPacket *)
            malloc(image->packets*sizeof(RunlengthPacket));
          if ((pixels == (unsigned char *) NULL) ||
              (image->pixels == (RunlengthPacket *) NULL))
            {
              Warning("unable to read image","memory allocation failed");
              DestroyImage(image);
              return((Image *) NULL);
            }
          status=ReadData((char *) pixels,(int) packet_size,(int)
            image->packets,image->file);
          if (status == False)
            {
              Warning("insufficient image data in file",image->filename);
              DestroyImage(image);
              return((Image *) NULL);
            }
          /*
            Convert to runlength-encoded DirectClass packets.
          */
          image->packets=image->columns*image->rows;
          p=pixels;
          q=image->pixels;
          for (i=0; i < image->packets; i++)
          {
            q->red=(*p++);
            q->green=(*p++);
            q->blue=(*p++);
            q->index=0;
            q->length=0;
            q++;
          }
          break;
        }
        case QEncodedCompression:
        {
          extern unsigned int
            QDecodeImage();

          unsigned char
            *compressed_pixels;

          /*
            Allocate image buffers and read image from disk.
          */
          packet_size=1;
          if (image->packets == 0)
            image->packets=image->columns*image->rows*3;
          pixels=(unsigned char *)
            malloc((unsigned int) image->packets*packet_size);
          image->pixels=(RunlengthPacket *)
            malloc(image->columns*image->rows*sizeof(RunlengthPacket));
          if ((pixels == (unsigned char *) NULL) ||
              (image->pixels == (RunlengthPacket *) NULL))
            {
              Warning("unable to read image","memory allocation failed");
              DestroyImage(image);
              return((Image *) NULL);
            }
          status=ReadData((char *) pixels,(int) packet_size,
            (int) image->packets,image->file);
          /*
            Uncompress image.
          */
          image->packets=image->columns*image->rows;
          packet_size=3;
          compressed_pixels=pixels;
          pixels=(unsigned char *)
            malloc((unsigned int) image->packets*packet_size);
          if (pixels == (unsigned char *) NULL)
            {
              Warning("unable to read image","memory allocation failed");
              DestroyImage(image);
              return((Image *) NULL);
            }
          packets=QDecodeImage(compressed_pixels,pixels,
            image->columns*(int) packet_size,image->rows);
          if (packets != (image->packets*packet_size))
            {
              Warning("insufficient image data in file",image->filename);
              DestroyImage(image);
              return((Image *) NULL);
            }
          (void) free((char *) compressed_pixels);
          /*
            Convert to runlength-encoded DirectClass packets.
          */
          p=pixels;
          q=image->pixels;
          for (i=0; i < (image->columns*image->rows); i++)
          {
            q->red=(*p++);
            q->green=(*p++);
            q->blue=(*p++);
            q->index=0;
            q->length=0;
            q++;
          }
          break;
        }
        case RunlengthEncodedCompression:
        {
          /*
            Allocate image buffers and read image from disk.
          */
          packet_size=4;
          if (image->packets == 0)
            {
              /*
                Number of packets is unspecified.
              */
              pixels=(unsigned char *)
                malloc((unsigned int) (image->columns*image->rows*packet_size));
              image->pixels=(RunlengthPacket *)
                malloc(image->columns*image->rows*sizeof(RunlengthPacket));
              if ((pixels == (unsigned char *) NULL) ||
                  (image->pixels == (RunlengthPacket *) NULL))
                {
                  Warning("unable to read image","memory allocation failed");
                  DestroyImage(image);
                  return((Image *) NULL);
                }
              count=0;
              image->packets=0;
              p=pixels;
              do
              {
                (void) ReadData((char *) p,(int) packet_size,1,image->file);
                image->packets++;
                p+=(packet_size-1);
                count+=(*p+1);
                p++;
              }
              while (count < (image->columns*image->rows));
              p=pixels;
            }
          else
            {
              pixels=(unsigned char *)
                malloc((unsigned int) image->packets*packet_size);
              image->pixels=(RunlengthPacket *)
                malloc(image->packets*sizeof(RunlengthPacket));
              if ((pixels == (unsigned char *) NULL) ||
                  (image->pixels == (RunlengthPacket *) NULL))
                {
                  Warning("unable to read image","memory allocation failed");
                  DestroyImage(image);
                  return((Image *) NULL);
                }
              (void) ReadData((char *) pixels,(int) packet_size,
                (int) image->packets,image->file);
            }
          count=0;
          p=pixels;
          q=image->pixels;
          for (i=0; i < image->packets; i++)
          {
            q->red=(*p++);
            q->green=(*p++);
            q->blue=(*p++);
            q->index=0;
            q->length=(*p++);
            count+=(q->length+1);
            q++;
          }
          /*
            Guarentee the correct number of runlength packets.
          */
          if (count > (image->columns*image->rows))
            {
              Warning("insufficient image data in file",image->filename);
              DestroyImage(image);
              return((Image *) NULL);
            }
          else
            if (count < (image->columns*image->rows))
              {
                Warning("too much image data in file",image->filename);
                DestroyImage(image);
                return((Image *) NULL);
              }
          break;
        }
      }
      break;
    }
    case PseudoClass:
    {
      register unsigned short
        index;

      if (image->colors == 0)
        {
          /*
            Create grayscale map.
          */
          image->colors=256;
          image->colormap=(ColorPacket *)
            malloc(image->colors*sizeof(ColorPacket));
          if (image->colormap == (ColorPacket *) NULL)
            {
              Warning("unable to read image","memory allocation failed");
              DestroyImage(image);
              return((Image *) NULL);
            }
          for (i=0; i < image->colors; i++)
          {
            image->colormap[i].red=(unsigned char) i;
            image->colormap[i].green=(unsigned char) i;
            image->colormap[i].blue=(unsigned char) i;
          }
        }
      else
        {
          unsigned char
            *colormap;

          /*
            Read colormap from disk.
          */
          packet_size=3;
          colormap=(unsigned char *)
            malloc((unsigned int) image->colors*packet_size);
          image->colormap=(ColorPacket *)
            malloc(image->colors*sizeof(ColorPacket));
          if ((colormap == (unsigned char *) NULL) ||
              (image->colormap == (ColorPacket *) NULL))
            {
              Warning("unable to read image","memory allocation failed");
              DestroyImage(image);
              return((Image *) NULL);
            }
          (void) ReadData((char *) colormap,(int) packet_size,
            (int) image->colors,image->file);
          p=colormap;
          for (i=0; i < image->colors; i++)
          {
            image->colormap[i].red=(*p++);
            image->colormap[i].green=(*p++);
            image->colormap[i].blue=(*p++);
          }
          (void) free((char *) colormap);
        }
      /*
        Transfer pixels to PseudoClass image pixel buffer.
      */
      switch (image->compression)
      {
        case NoCompression:
        {
          /*
            Allocate image buffers and read image from disk.
          */
          image->packets=image->columns*image->rows;
          packet_size=1;
          if (image->colors > 256)
            packet_size++;
          pixels=(unsigned char *)
            malloc((unsigned int) image->packets*packet_size);
          image->pixels=(RunlengthPacket *)
            malloc(image->packets*sizeof(RunlengthPacket));
          if ((pixels == (unsigned char *) NULL) ||
              (image->pixels == (RunlengthPacket *) NULL))
            {
              Warning("unable to read image","memory allocation failed");
              DestroyImage(image);
              return((Image *) NULL);
            }
          status=ReadData((char *) pixels,(int) packet_size,
            (int) (image->columns*image->rows),image->file);
          if (status == False)
            {
              Warning("insufficient image data in file",image->filename);
              DestroyImage(image);
              return((Image *) NULL);
            }
          /*
            Convert to runlength-encoded PseudoClass packets.
          */
          image->packets=image->columns*image->rows;
          p=pixels;
          q=image->pixels;
          if (image->colors <= 256)
            for (i=0; i < image->packets; i++)
            {
              index=(*p++);
              q->red=image->colormap[index].red;
              q->green=image->colormap[index].green;
              q->blue=image->colormap[index].blue;
              q->index=index;
              q->length=0;
              q++;
            }
          else
            for (i=0; i < image->packets; i++)
            {
              index=(*p++) << 8;
              index|=(*p++);
              q->red=image->colormap[index].red;
              q->green=image->colormap[index].green;
              q->blue=image->colormap[index].blue;
              q->index=index;
              q->length=0;
              q++;
            }
          break;
        }
        case QEncodedCompression:
        {
          extern unsigned int
            QDecodeImage();

          unsigned char
            *compressed_pixels;

          /*
            Allocate image buffers and read image from disk.
          */
          packet_size=1;
          if (image->packets == 0)
            image->packets=(image->columns*image->rows)*
              (image->colors > 256 ? 2 : 1);
          pixels=(unsigned char *)
            malloc((unsigned int) image->packets*packet_size);
          image->pixels=(RunlengthPacket *)
            malloc(image->columns*image->rows*sizeof(RunlengthPacket));
          if ((pixels == (unsigned char *) NULL) ||
              (image->pixels == (RunlengthPacket *) NULL))
            {
              Warning("unable to read image","memory allocation failed");
              DestroyImage(image);
              return((Image *) NULL);
            }
          status=ReadData((char *) pixels,(int) packet_size,
            (int) image->packets,image->file);
          /*
            Uncompress image.
          */
          image->packets=image->columns*image->rows;
          packet_size=1;
          if (image->colors > 256)
            packet_size++;
          compressed_pixels=pixels;
          pixels=(unsigned char *)
            malloc((unsigned int) image->packets*packet_size);
          if (pixels == (unsigned char *) NULL)
            {
              Warning("unable to read image","memory allocation failed");
              DestroyImage(image);
              return((Image *) NULL);
            }
          packets=QDecodeImage(compressed_pixels,pixels,image->columns*
            (int) packet_size,image->rows);
          if (packets != (image->packets*packet_size))
            {
              Warning("insufficient image data in file",image->filename);
              DestroyImage(image);
              return((Image *) NULL);
            }
          (void) free((char *) compressed_pixels);
          /*
            Convert to runlength-encoded PseudoClass packets.
          */
          p=pixels;
          q=image->pixels;
          if (image->colors <= 256)
            for (i=0; i < (image->columns*image->rows); i++)
            {
              index=(*p++);
              q->red=image->colormap[index].red;
              q->green=image->colormap[index].green;
              q->blue=image->colormap[index].blue;
              q->index=index;
              q->length=0;
              q++;
            }
          else
            for (i=0; i < (image->columns*image->rows); i++)
            {
              index=(*p++) << 8;
              index|=(*p++);
              q->red=image->colormap[index].red;
              q->green=image->colormap[index].green;
              q->blue=image->colormap[index].blue;
              q->index=index;
              q->length=0;
              q++;
            }
          break;
        }
        case RunlengthEncodedCompression:
        {
          packet_size=2;
          if (image->colors > 256)
            packet_size++;
          if (image->packets == 0)
            {
              /*
                Number of packets is unspecified.
              */
              pixels=(unsigned char *)
                malloc((unsigned int) (image->columns*image->rows*packet_size));
              image->pixels=(RunlengthPacket *)
                malloc((unsigned int) packets*sizeof(RunlengthPacket));
              if ((pixels == (unsigned char *) NULL) ||
                  (image->pixels == (RunlengthPacket *) NULL))
                {
                  Warning("unable to read image","memory allocation failed");
                  DestroyImage(image);
                  return((Image *) NULL);
                }
              count=0;
              image->packets=0;
              p=pixels;
              do
              {
                (void) ReadData((char *) p,(int) packet_size,1,image->file);
                image->packets++;
                p+=(packet_size-1);
                count+=(*p+1);
                p++;
              }
              while (count < (image->columns*image->rows));
            }
          else
            {
              pixels=(unsigned char *)
                malloc((unsigned int) image->packets*packet_size);
              image->pixels=(RunlengthPacket *)
                malloc(image->packets*sizeof(RunlengthPacket));
              if ((pixels == (unsigned char *) NULL) ||
                  (image->pixels == (RunlengthPacket *) NULL))
                {
                  Warning("unable to read image","memory allocation failed");
                  DestroyImage(image);
                  return((Image *) NULL);
                }
              (void) ReadData((char *) pixels,(int) packet_size,
                (int) image->packets,image->file);
            }
          count=0;
          p=pixels;
          q=image->pixels;
          if (image->colors <= 256)
            for (i=0; i < image->packets; i++)
            {
              index=(*p++);
              q->red=image->colormap[index].red;
              q->green=image->colormap[index].green;
              q->blue=image->colormap[index].blue;
              q->index=index;
              q->length=(*p++);
              count+=(q->length+1);
              q++;
            }
          else
            for (i=0; i < image->packets; i++)
            {
              index=(*p++) << 8;
              index|=(*p++);
              q->red=image->colormap[index].red;
              q->green=image->colormap[index].green;
              q->blue=image->colormap[index].blue;
              q->index=index;
              q->length=(*p++);
              count+=(q->length+1);
              q++;
            }
          /*
            Guarentee the correct number of runlength packets.
          */
          if (count > (image->columns*image->rows))
            {
              Warning("insufficient image data in file",image->filename);
              DestroyImage(image);
              return((Image *) NULL);
            }
          else
            if (count < (image->columns*image->rows))
              {
                Warning("too much image data in file",image->filename);
                DestroyImage(image);
                return((Image *) NULL);
              }
          break;
        }
      }
      break;
    }
  }
  (void) free((char *) pixels);
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
%   R e d u c e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReduceImage creates a new image that is a integral size less than
%  an existing one.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  ReduceImage scans the reference image to create a reduced image by computing
%  the weighted average of a 4x4 cell centered at each reference pixel.  The
%  target pixel requires two columns and two rows of the reference pixels.
%  Therefore the reduced image columns and rows become:
%
%    number_columns/2
%    number_rows/2
%
%  Weights assume that the importance of neighboring pixels is inversely
%  proportional to the square of their distance from the target pixel.
%
%  The scan only processes pixels that have a full set of neighbors.  Pixels
%  in the top, bottom, left, and right pairs of rows and columns are omitted
%  from the scan.
%
%  The format of the ReduceImage routine is:
%
%      reduced_image=ReduceImage(image)
%
%  A description of each parameter follows:
%
%    o reduced_image: Function ReduceImage returns a pointer to the image
%      after reducing.  A null image is returned if there is a a memory
%      shortage or if the image size is less than IconSize*2.
%
%    o image: The address of a structure of type Image.
%
%
*/
Image *ReduceImage(image)
Image
  *image;
{
#define Rsum(weight) \
  total_red+=weight*(s->red); \
  total_green+=weight*(s->green); \
  total_blue+=weight*(s->blue); \
  s++;

  ColorPacket
    *scanline;

  Image
    *reduced_image;

  register ColorPacket
    *s,
    *s0,
    *s1,
    *s2,
    *s3;

  register RunlengthPacket
    *p,
    *q;

  register unsigned int
    x;

  unsigned int
    y;

  unsigned long
    total_blue,
    total_green,
    total_red;

  if ((image->columns < 4) || (image->rows < 4))
    {
      Warning("unable to reduce image","image size must exceed 3x3");
      return((Image *) NULL);
    }
  /*
    Initialize reduced image attributes.
  */
  reduced_image=CopyImage(image,image->columns >> 1,image->rows >> 1,False);
  if (reduced_image == (Image *) NULL)
    {
      Warning("unable to reduce image","memory allocation failed");
      return((Image *) NULL);
    }
  reduced_image->class=DirectClass;
  /*
    Allocate image buffer and scanline buffer for 4 rows of the image.
  */
  scanline=(ColorPacket *) malloc(4*image->columns*sizeof(ColorPacket));
  if (scanline == (ColorPacket *) NULL)
    {
      Warning("unable to reduce image","memory allocation failed");
      DestroyImage(reduced_image);
      return((Image *) NULL);
    }
  /*
    Preload the first 2 rows of the image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < (2*image->columns); x++)
  {
    if (image->runlength > 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    s->red=p->red;
    s->green=p->green;
    s->blue=p->blue;
    s->index=p->index;
    s++;
  }
  /*
    Reduce each row.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  q=reduced_image->pixels;
  for (y=0; y < (image->rows-1); y+=2)
  {
    /*
      Initialize sliding window pointers.
    */
    s0=scanline+image->columns*((y+0) % 4);
    s1=scanline+image->columns*((y+1) % 4);
    s2=scanline+image->columns*((y+2) % 4);
    s3=scanline+image->columns*((y+3) % 4);
    /*
      Read another scan line.
    */
    s=s2;
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      s->red=p->red;
      s->green=p->green;
      s->blue=p->blue;
      s->index=p->index;
      s++;
    }
    /*
      Read another scan line.
    */
    s=s3;
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      s->red=p->red;
      s->green=p->green;
      s->blue=p->blue;
      s->index=p->index;
      s++;
    }
    for (x=0; x < (image->columns-1); x+=2)
    {
      /*
        Compute weighted average of target pixel color components.

        These particular coefficients total to 128.  Use 128/2-1 or 63 to
        insure correct round off.
      */
      total_red=0;
      total_green=0;
      total_blue=0;
      s=s0;
      Rsum(3); Rsum(7);  Rsum(7);  Rsum(3);
      s=s1;
      Rsum(7); Rsum(15); Rsum(15); Rsum(7);
      s=s2;
      Rsum(7); Rsum(15); Rsum(15); Rsum(7);
      s=s3;
      Rsum(3); Rsum(7);  Rsum(7);  Rsum(3);
      s0+=2;
      s1+=2;
      s2+=2;
      s3+=2;
      q->red=(total_red+63) >> 7;
      q->green=(total_green+63) >> 7;
      q->blue=(total_blue+63) >> 7;
      q->index=0;
      q->length=0;
      q++;
    }
  }
  (void) free((char *) scanline);
  return(reduced_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e f l e c t I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReflectImage creates a new image that refelects each scanline of an
%  existing one.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReflectImage routine is:
%
%      reflected_image=ReflectImage(image)
%
%  A description of each parameter follows:
%
%    o reflected_image: Function ReflectImage returns a pointer to the image
%      after reflecting.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image.
%
%
*/
Image *ReflectImage(image)
Image
  *image;
{
  ColorPacket
    *scanline;

  Image
    *reflected_image;

  register ColorPacket
    *s;

  register RunlengthPacket
    *p,
    *q;

  register unsigned int
    x,
    y;

  /*
    Initialize reflected image attributes.
  */
  reflected_image=CopyImage(image,image->columns,image->rows,False);
  if (reflected_image == (Image *) NULL)
    {
      Warning("unable to reflect image","memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Allocate scan line buffer and column offset buffers.
  */
  scanline=(ColorPacket *) malloc(image->columns*sizeof(ColorPacket));
  if (scanline == (ColorPacket *) NULL)
    {
      Warning("unable to reflect image","memory allocation failed");
      DestroyImage(reflected_image);
      return((Image *) NULL);
    }
  /*
    Reflect each row.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  q=reflected_image->pixels;
  for (y=0; y < reflected_image->rows; y++)
  {
    /*
      Read a scan line.
    */
    s=scanline;
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      s->red=p->red;
      s->green=p->green;
      s->blue=p->blue;
      s->index=p->index;
      s++;
    }
    /*
      Reflect each column.
    */
    s=scanline+image->columns;
    for (x=0; x < reflected_image->columns; x++)
    {
      s--;
      q->red=s->red;
      q->green=s->green;
      q->blue=s->blue;
      q->index=s->index;
      q->length=0;
      q++;
    }
  }
  (void) free((char *) scanline);
  return(reflected_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S c a l e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ScaleImage creates a new image that is a scaled size of an
%  existing one using pixel replication.  It allocates the memory necessary
%  for the new Image structure and returns a pointer to the new image.
%
%  The format of the ScaleImage routine is:
%
%      scaled_image=ScaleImage(image,columns,rows)
%
%  A description of each parameter follows:
%
%    o scaled_image: Function ScaleImage returns a pointer to the image after
%      scaling.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o columns: An integer that specifies the number of columns in the scaled
%      image.
%
%    o rows: An integer that specifies the number of rows in the scaled
%      image.
%
%
*/
Image *ScaleImage(image,columns,rows)
Image
  *image;

unsigned int
  columns,
  rows;
{
  ColorPacket
    *scanline;

  Image
    *scaled_image;

  register ColorPacket
    *s;

  register RunlengthPacket
    *p,
    *q;

  register unsigned int
    x;

  unsigned int
    *x_offset,
    y,
    *y_offset;

  unsigned long
    scale_factor;

  if ((columns*rows) == 0)
    {
      Warning("unable to scale image","image dimensions are zero");
      return((Image *) NULL);
    }
  if ((columns*rows) > MaxImageSize)
    {
      Warning("unable to scale image","image too large");
      return((Image *) NULL);
    }
  /*
    Initialize scaled image attributes.
  */
  scaled_image=CopyImage(image,columns,rows,False);
  if (scaled_image == (Image *) NULL)
    {
      Warning("unable to scale image","memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Allocate scan line buffer and column offset buffers.
  */
  scanline=(ColorPacket *) malloc(image->columns*sizeof(ColorPacket));
  x_offset=(unsigned int *) malloc(scaled_image->columns*sizeof(unsigned int));
  y_offset=(unsigned int *) malloc(scaled_image->rows*sizeof(unsigned int));
  if ((scanline == (ColorPacket *) NULL) ||
      (x_offset == (unsigned int *) NULL) ||
      (y_offset == (unsigned int *) NULL))
    {
      Warning("unable to scale image","memory allocation failed");
      DestroyImage(scaled_image);
      return((Image *) NULL);
    }
  /*
    Initialize column pixel offsets.
  */
  scale_factor=((image->columns-1) << 14)/scaled_image->columns;
  columns=0;
  for (x=0; x < scaled_image->columns; x++)
  {
    x_offset[x]=(((x+1)*scale_factor+8191) >> 14)-columns;
    columns+=x_offset[x];
  }
  /*
    Initialize row pixel offsets.
  */
  scale_factor=((image->rows-1) << 14)/scaled_image->rows;
  rows=0;
  for (y=0; y < scaled_image->rows; y++)
  {
    y_offset[y]=(((y+1)*scale_factor+8191) >> 14)-rows;
    rows+=y_offset[y];
  }
  /*
    Preload first scanline.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < image->columns; x++)
  {
    if (image->runlength > 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    s->red=p->red;
    s->green=p->green;
    s->blue=p->blue;
    s->index=p->index;
    s++;
  }
  /*
    Scale each row.
  */
  q=scaled_image->pixels;
  for (y=0; y < scaled_image->rows; y++)
  {
    /*
      Scale each column.
    */
    s=scanline;
    for (x=0; x < scaled_image->columns; x++)
    {
      q->red=s->red;
      q->green=s->green;
      q->blue=s->blue;
      q->index=s->index;
      q->length=0;
      q++;
      s+=x_offset[x];
    }
    if (y_offset[y] > 0)
      {
        /*
          Skip a scan line.
        */
        for (x=0; x < (image->columns*(y_offset[y]-1)); x++)
          if (image->runlength > 0)
            image->runlength--;
          else
            {
              p++;
              image->runlength=p->length;
            }
        /*
          Read a scan line.
        */
        s=scanline;
        for (x=0; x < image->columns; x++)
        {
          if (image->runlength > 0)
            image->runlength--;
          else
            {
              p++;
              image->runlength=p->length;
            }
          s->red=p->red;
          s->green=p->green;
          s->blue=p->blue;
          s->index=p->index;
          s++;
        }
      }
  }
  (void) free((char *) scanline);
  (void) free((char *) x_offset);
  (void) free((char *) y_offset);
  return(scaled_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t e r e o I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function StereoImage combines two images and produces a single image that
%  is the composite of a left and right image of a stereo pair.  The left
%  image is converted to grayscale and written to the red channel of the
%  stereo image.  The right image is converted to grayscale and written to the
%  blue channel of the stereo image.  View the composite image with red-blue
%  glasses to create a stereo effect.
%
%  The format of the StereoImage routine is:
%
%      stereo_image=StereoImage(left_image,right_image)
%
%  A description of each parameter follows:
%
%    o stereo_image: Function StereoImage returns a pointer to the stereo
%      image.  A null image is returned if there is a memory shortage.
%
%    o left_image: The address of a structure of type Image.
%
%    o right_image: The address of a structure of type Image.
%
%
*/
Image *StereoImage(left_image,right_image)
Image
  *left_image,
  *right_image;
{
  Image
    *stereo_image;

  register int
    i;

  register RunlengthPacket
    *p,
    *q,
    *r;

  if ((left_image->columns != right_image->columns) ||
      (left_image->rows != right_image->rows))
    {
      Warning("unable to create stereo image",
        "left and right image sizes differ");
      return((Image *) NULL);
    }
  /*
    Initialize stereo image attributes.
  */
  stereo_image=CopyImage(left_image,left_image->columns,left_image->rows,False);
  if (stereo_image == (Image *) NULL)
    {
      Warning("unable to create stereo image","memory allocation failed");
      return((Image *) NULL);
    }
  stereo_image->class=DirectClass;
  /*
    Copy left image to red channel and right image to blue channel.
  */
  GrayImage(left_image);
  p=left_image->pixels;
  left_image->runlength=p->length+1;
  GrayImage(right_image);
  q=right_image->pixels;
  right_image->runlength=q->length+1;
  r=stereo_image->pixels;
  for (i=0; i < (stereo_image->columns*stereo_image->rows); i++)
  {
    if (left_image->runlength > 0)
      left_image->runlength--;
    else
      {
        p++;
        left_image->runlength=p->length;
      }
    if (right_image->runlength > 0)
      right_image->runlength--;
    else
      {
        q++;
        right_image->runlength=q->length;
      }
    r->red=(p->red*12) >> 4;
    r->green=0;
    r->blue=q->blue;
    r->index=0;
    r->length=0;
    r++;
  }
  return(stereo_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T r a n s f o r m I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function TransformImage creates a new image that is a transformed size of
%  of existing one as specified by the clip, image and scale geometries.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  If a clip geometry is specified a subregion of the image is obtained.
%  If the specified image size, as defined by the image and scale geometries,
%  is smaller than the actual image size, the image is first reduced to an
%  integral of the specified image size with an antialias digital filter.  The
%  image is then scaled to the exact specified image size with pixel
%  replication.  If the specified image size is greater than the actual image
%  size, the image is first enlarged to an integral of the specified image
%  size with bilinear interpolation.  The image is then scaled to the exact
%  specified image size with pixel replication.
%
%  The format of the TransformImage routine is:
%
%      transformed_image=TransformImage(image,clip_geometry,image_geometry,
%        scale_geometry)
%
%  A description of each parameter follows:
%
%    o transformed_image: Function TransformImage returns a pointer to the
%      image after clipping and scaling it.  The original image is returned if
%      there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o clip_geometry: Specifies a pointer to a clip geometry string.
%      This geometry defined a subregion of the image.
%
%    o image_geometry: Specifies a pointer to a image geometry string.
%      The specified width and height of this geometry string are absolute.
%
%    o scale_geometry: Specifies a pointer to a scale geometry string.
%      The specified width and height of this geometry string are relative.
%
%
*/
Image *TransformImage(image,clip_geometry,image_geometry,scale_geometry)
Image
  *image;

char
  *clip_geometry,
  *image_geometry,
  *scale_geometry;
{
  int
    flags,
    x,
    y;

  unsigned int
    height,
    width;

  if (clip_geometry != (char *) NULL)
    {
      Image
        *clipped_image;

      /*
        Clip image to a user specified size.
      */
      x=0;
      y=0;
      flags=XParseGeometry(clip_geometry,&x,&y,&width,&height);
      if ((flags & WidthValue) == 0)
        width=(unsigned int) ((int) image->columns-x);
      if ((flags & HeightValue) == 0)
        height=(unsigned int) ((int) image->rows-y);
      clipped_image=ClipImage(image,x,y,width,height);
      if (clipped_image != (Image *) NULL)
        {
          DestroyImage(image);
          image=clipped_image;
        }
    }
  /*
    Scale image to a user specified size.
  */
  width=image->columns;
  height=image->rows;
  if (scale_geometry != (char *) NULL)
    {
      float
        scale_height,
        scale_width;

      scale_width=0.0;
      scale_height=0.0;
      (void) sscanf(scale_geometry,"%fx%f",&scale_width,&scale_height);
      if (scale_height == 0.0)
        scale_height=scale_width;
      width*=scale_width;
      height*=scale_height;
    }
  if (image_geometry != (char *) NULL)
    (void) XParseGeometry(image_geometry,&x,&y,&width,&height);
  while ((image->columns >= (width << 1)) && (image->rows >= (height << 1)))
  {
    Image
      *reduced_image;

    /*
      Reduce image with a antialias digital filter.
     */
    reduced_image=ReduceImage(image);
    if (reduced_image == (Image *) NULL)
      break;
    DestroyImage(image);
    image=reduced_image;
  }
  while ((image->columns <= (width >> 1)) && (image->rows <= (height >> 1)))
  {
    Image
      *zoomed_image;

    /*
      Zoom image with bilinear interpolation.
    */
    zoomed_image=ZoomImage(image);
    if (zoomed_image == (Image *) NULL)
      break;
    DestroyImage(image);
    image=zoomed_image;
  }
  if ((image->columns != width) || (image->rows != height))
    {
      Image
        *scaled_image;

      /*
        Scale image with pixel replication.
      */
      scaled_image=ScaleImage(image,width,height);
      if (scaled_image != (Image *) NULL)
        {
          DestroyImage(image);
          image=scaled_image;
        }
    }
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteImage writes an image to a file on disk.
%
%  The format of the WriteImage routine is:
%
%      status=WriteImage(image)
%
%  A description of each parameter follows:
%
%    o status: Function WriteImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image: A pointer to a Image structure.
%
%
*/
unsigned int WriteImage(image)
Image
  *image;
{
  register int
    i,
    j;

  register long int
    count;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *colormap,
    *pixels;

  unsigned int
    packets,
    packet_size;

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
          command[1024];

        /*
          Image file is compressed-- uncompress it.
        */
        (void) sprintf(command,"compress -c > %s",image->filename);
        image->file=(FILE *) popen(command,"w");
      }
  if (image->file == (FILE *) NULL)
    {
      (void) fprintf(stderr,"%s: unable to write image, cannot open %s.\n",
        application_name,image->filename);
      return(False);
    }
  /*
    Allocate pixel buffer.
  */
  count=0;
  switch (image->class)
  {
    case DirectClass:
    {
      if (image->compression == RunlengthEncodedCompression)
        {
          /*
            Compress the image.
          */
          CompressImage(image);
          if (image->packets > ((image->columns*image->rows*3) >> 2))
            image->compression=NoCompression;
        }
      /*
        Transfer pixels to DirectClass image pixel buffer.
      */
      switch (image->compression)
      {
        case NoCompression:
        {
          packet_size=3;
          packets=image->columns*image->rows;
          pixels=(unsigned char *) malloc((unsigned int) packets*packet_size);
          if (pixels == (unsigned char *) NULL)
            {
              Warning("unable to write image","memory allocation failed");
              return(False);
            }
          p=image->pixels;
          q=pixels;
          for (i=0; i < image->packets; i++)
          {
            for (j=0; j <= p->length; j++)
            {
              *q++=p->red;
              *q++=p->green;
              *q++=p->blue;
            }
            count+=(p->length+1);
            p++;
          }
          break;
        }
        case QEncodedCompression:
        {
          extern unsigned int
            QEncodeImage();

          unsigned char
            *compressed_pixels;

          packet_size=3;
          pixels=(unsigned char *)
            malloc((unsigned int) (image->columns*image->rows*packet_size));
          if (pixels == (unsigned char *) NULL)
            {
              Warning("unable to write image","memory allocation failed");
              return(False);
            }
          p=image->pixels;
          q=pixels;
          for (i=0; i < image->packets; i++)
          {
            for (j=0; j <= p->length; j++)
            {
              *q++=p->red;
              *q++=p->green;
              *q++=p->blue;
            }
            count+=(p->length+1);
            p++;
          }
          compressed_pixels=(unsigned char *)
            malloc((unsigned int) (image->columns*image->rows*packet_size));
          if (compressed_pixels == (unsigned char *) NULL)
            {
              Warning("unable to write image","memory allocation failed");
              return(False);
            }
          packets=QEncodeImage(pixels,compressed_pixels,
            image->columns*(int) packet_size,image->rows);
          packet_size=1;
          (void) free((char *) pixels);
          pixels=compressed_pixels;
          break;
        }
        case RunlengthEncodedCompression:
        {
          packet_size=4;
          packets=image->packets;
          pixels=(unsigned char *) malloc((unsigned int) packets*packet_size);
          if (pixels == (unsigned char *) NULL)
            {
              Warning("unable to write image","memory allocation failed");
              return(False);
            }
          p=image->pixels;
          q=pixels;
          for (i=0; i < image->packets; i++)
          {
            *q++=p->red;
            *q++=p->green;
            *q++=p->blue;
            *q++=p->length;
            count+=(p->length+1);
            p++;
          }
          break;
        }
        default:
        {
          Warning("unable to write image","unknown compression type");
          return(False);
        }
      }
      break;
    }
    case PseudoClass:
    {
      if (image->compression == RunlengthEncodedCompression)
        {
          /*
            Compress the image.
          */
          CompressImage(image);
          if (image->packets > ((image->columns*image->rows) >> 1))
            image->compression=NoCompression;
        }
      /*
        Transfer pixels from PseudoClass image pixel buffer.
      */
      switch (image->compression)
      {
        case NoCompression:
        {
          packet_size=1;
          if (image->colors > 256)
            packet_size++;
          packets=image->columns*image->rows;
          pixels=(unsigned char *) malloc((unsigned int) packets*packet_size);
          if (pixels == (unsigned char *) NULL)
            {
              Warning("unable to write image","memory allocation failed");
              return(False);
            }
          p=image->pixels;
          q=pixels;
          if (image->colors <= 256)
            for (i=0; i < image->packets; i++)
            {
              for (j=0; j <= p->length; j++)
                *q++=(unsigned char) p->index;
              count+=(p->length+1);
              p++;
            }
          else
            {
              register unsigned char
                xff00,
                xff;

              for (i=0; i < image->packets; i++)
              {
                xff00=(unsigned char) (p->index >> 8);
                xff=(unsigned char) p->index;
                for (j=0; j <= p->length; j++)
                {
                  *q++=xff00;
                  *q++=xff;
                }
                count+=(p->length+1);
                p++;
              }
            }
          break;
        }
        case QEncodedCompression:
        {
          extern unsigned int
            QEncodeImage();

          unsigned char
            *compressed_pixels;

          /*
            Sort image colormap by increasing intensity.
          */
          LinearColormap(image);
          packet_size=1;
          if (image->colors > 256)
            packet_size++;
          pixels=(unsigned char *)
            malloc((unsigned int) (image->columns*image->rows*packet_size));
          if (pixels == (unsigned char *) NULL)
            {
              Warning("unable to write image","memory allocation failed");
              return(False);
            }
          p=image->pixels;
          q=pixels;
          if (image->colors <= 256)
            for (i=0; i < image->packets; i++)
            {
              for (j=0; j <= p->length; j++)
                *q++=(unsigned char) p->index;
              count+=(p->length+1);
              p++;
            }
          else
            {
              register unsigned char
                xff00,
                xff;

              for (i=0; i < image->packets; i++)
              {
                xff00=(unsigned char) (p->index >> 8);
                xff=(unsigned char) p->index;
                for (j=0; j <= p->length; j++)
                {
                  *q++=xff00;
                  *q++=xff;
                }
                count+=(p->length+1);
                p++;
              }
            }
          compressed_pixels=(unsigned char *)
            malloc((unsigned int) (image->columns*image->rows*packet_size));
          if (compressed_pixels == (unsigned char *) NULL)
            {
              Warning("unable to write image","memory allocation failed");
              return(False);
            }
          packets=QEncodeImage(pixels,compressed_pixels,
            image->columns*(int) packet_size,image->rows);
          packet_size=1;
          (void) free((char *) pixels);
          pixels=compressed_pixels;
          break;
        }
        case RunlengthEncodedCompression:
        {
          packet_size=2;
          if (image->colors > 256)
            packet_size++;
          packets=image->packets;
          pixels=(unsigned char *) malloc((unsigned int) packets*packet_size);
          if (pixels == (unsigned char *) NULL)
            {
              Warning("unable to write image","memory allocation failed");
              return(False);
            }
          p=image->pixels;
          q=pixels;
          if (image->colors <= 256)
            for (i=0; i < image->packets; i++)
            {
              *q++=(unsigned char) p->index;
              *q++=p->length;
              count+=(p->length+1);
              p++;
            }
          else
            for (i=0; i < image->packets; i++)
            {
              *q++=(unsigned char) (p->index >> 8);
              *q++=(unsigned char) p->index;
              *q++=p->length;
              count+=(p->length+1);
              p++;
            }
          break;
        }
        default:
        {
          Warning("unable to write image","unknown compression type");
          return(False);
        }
      }
      /*
        Allocate colormap.
      */
      colormap=(unsigned char *) malloc((unsigned int) image->colors*3);
      if (colormap == (unsigned char *) NULL)
        {
          Warning("unable to write image","memory allocation failed");
          return(False);
        }
      q=colormap;
      for (i=0; i < image->colors; i++)
      {
        *q++=image->colormap[i].red;
        *q++=image->colormap[i].green;
        *q++=image->colormap[i].blue;
      }
      break;
    }
    default:
    {
      Warning("unable to write image","unknown image class");
      return(False);
    }
  }
  /*
    Guarentee the correct number of runlength packets.
  */
  if (count < (image->columns*image->rows))
    {
      Warning("insufficient image data in",image->filename);
      return(False);
    }
  else
    if (count > (image->columns*image->rows))
      {
        Warning("too much image data in",image->filename);
        return(False);
      }
  /*
    Write header and runlength encoded image to disk.
  */
  if (image->comments != (char *) NULL)
    (void) fprintf(image->file,"{%s}\n",image->comments);
  (void) fprintf(image->file,"id=ImageMagick\n");
  if (image->class == DirectClass)
    (void) fprintf(image->file,"class=DirectClass\n");
  else
    (void) fprintf(image->file,"class=PseudoClass  colors=%d\n",image->colors);
  if (image->compression == RunlengthEncodedCompression)
    (void) fprintf(image->file,"compression=RunlengthEncoded  packets=%d\n",
      packets);
  else
    if (image->compression == QEncodedCompression)
      (void) fprintf(image->file,"compression=QEncoded  packets=%d\n",packets);
  (void) fprintf(image->file,"columns=%d  rows=%d\n",image->columns,
    image->rows);
  if (image->scene > 0)
    (void) fprintf(image->file,"scene=%d\n",image->scene);
  (void) fprintf(image->file,"\f\n:\n");
  if (image->class == PseudoClass)
    {
      count=fwrite((char *) colormap,sizeof(unsigned char),
        (int) image->colors*3,image->file);
      if (count != (image->colors*3))
        {
          Warning("unable to write image","file write error");
          return(False);
        }
      (void) free((char *) colormap);
    }
  count=fwrite((char *) pixels,(int) packet_size,(int) packets,image->file);
  if (count != packets)
    {
      Warning("unable to write image","file write error");
      return(False);
    }
  (void) free((char *) pixels);
  if (image->file != stdin)
    if (strcmp(image->filename+strlen(image->filename)-2,".Z") != 0)
      (void) fclose(image->file);
    else
      (void) pclose(image->file);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n C o m p r e s s I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function UncompressImage compresses an image to the minimum number of
%  runlength-encoded packets.  
%
%  The format of the UncompressImage routine is:
%
%      status=UncompressImage(image)
%
%  A description of each parameter follows:
%
%    o status: Function UncompressImage returns True if the image is 
%      uncompressed otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/
unsigned int UncompressImage(image)
Image
  *image;
{
  register int
    i,
    j;

  register RunlengthPacket
    *p,
    *q;

  RunlengthPacket
    *uncompressed_pixels;

  /*
    Uncompress runlength-encoded packets.
  */
  uncompressed_pixels=(RunlengthPacket *)
    malloc(image->columns*image->rows*sizeof(RunlengthPacket));
  if (uncompressed_pixels == (RunlengthPacket *) NULL)
    return(False);
  p=image->pixels;
  q=uncompressed_pixels;
  for (i=0; i < image->packets; i++)
  {
    for (j=0; j <= p->length; j++)
    {
      q->red=p->red;
      q->green=p->green;
      q->blue=p->blue;
      q->index=p->index;
      q->length=0;
      q++;
    }
    p++;
  }
  (void) free((char *) image->pixels);
  image->pixels=uncompressed_pixels;
  image->packets=image->columns*image->rows;
  image->compression=NoCompression;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Z o o m I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ZoomImage creates a new image that is a integral size greater
%  than an existing one.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  ZoomImage scans the reference image to create a zoomed image by bilinear
%  interpolation.  The zoomed image columns and rows become:
%
%    number_columns << 1
%    number_rows << 1
%
%  The format of the ZoomImage routine is:
%
%      zoomed_image=ZoomImage(image)
%
%  A description of each parameter follows:
%
%    o zoomed_image: Function ZoomImage returns a pointer to the image after
%      zooming.  A null image is returned if there is a a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%
*/
Image *ZoomImage(image)
Image
  *image;
{
  ColorPacket
    *scanline;

  Image
    *zoomed_image;

  register ColorPacket
    *cs,
    *ns,
    *s;

  register RunlengthPacket
    *p,
    *q;

  register unsigned int
    x;

  unsigned int
    y;

  if (((image->columns*image->rows) << 1) > MaxImageSize)
    {
      Warning("unable to zoom image","image size too large");
      return((Image *) NULL);
    }
  /*
    Initialize scaled image attributes.
  */
  zoomed_image=CopyImage(image,image->columns << 1,image->rows << 1,False);
  if (zoomed_image == (Image *) NULL)
    {
      Warning("unable to zoom image","memory allocation failed");
      return((Image *) NULL);
    }
  zoomed_image->class=DirectClass;
  /*
    Allocate scan line buffer.
  */
  scanline=(ColorPacket *) malloc(4*image->columns*sizeof(ColorPacket));
  if (scanline == (ColorPacket *) NULL)
    {
      Warning("unable to zoom image","memory allocation failed");
      DestroyImage(zoomed_image);
      return((Image *) NULL);
    }
  /*
    Preload a scan line and interpolate.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < image->columns; x++)
  {
    s->red=p->red;
    s->green=p->green;
    s->blue=p->blue;
    s++;
    if (image->runlength > 0)
      {
        image->runlength--;
        s->red=p->red;
        s->green=p->green;
        s->blue=p->blue;
      }
    else
      {
        p++;
        image->runlength=p->length;
        s->red=(p->red+(s-1)->red) >> 1;
        s->green=(p->green+(s-1)->green) >> 1;
        s->blue=(p->blue+(s-1)->blue) >> 1;
      }
    s++;
  }
  /*
    Zoom each row.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  q=zoomed_image->pixels;
  for (y=0; y < image->rows; y++)
  {
    cs=scanline+image->columns*((y+0) % 2)*2;
    ns=scanline+image->columns*((y+1) % 2)*2;
    /*
      Read a scan line and interpolate.
    */
    s=ns;
    for (x=0; x < image->columns; x++)
    {
      s->red=p->red;
      s->green=p->green;
      s->blue=p->blue;
      s++;
      if (image->runlength > 0)
        {
          image->runlength--;
          s->red=p->red;
          s->green=p->green;
          s->blue=p->blue;
        }
      else
        {
          p++;
          image->runlength=p->length;
          s->red=(p->red+(s-1)->red) >> 1;
          s->green=(p->green+(s-1)->green) >> 1;
          s->blue=(p->blue+(s-1)->blue) >> 1;
        }
      s++;
    }
    /*
      Dump column interpolation values.
    */
    s=cs;
    for (x=0; x < zoomed_image->columns; x++)
    {
      q->red=s->red;
      q->green=s->green;
      q->blue=s->blue;
      q->index=0;
      q->length=0;
      q++;
      s++;
    }
    /*
      Dump row interpolation values.
    */
    for (x=0; x < zoomed_image->columns; x++)
    {
      q->red=(cs->red+ns->red) >> 1;
      q->green=(cs->green+ns->green) >> 1;
      q->blue=(cs->blue+ns->blue) >> 1;
      q->index=0;
      q->length=0;
      q++;
      cs++;
      ns++;
    }
  }
  (void) free((char *) scanline);
  return(zoomed_image);
}
