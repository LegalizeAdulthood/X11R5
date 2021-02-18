/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                 RRRR    OOO   TTTTT   AAA   TTTTT   EEEEE                   %
%                 R   R  O   O    T    A   A    T     E                       %
%                 RRRR   O   O    T    AAAAA    T     EEE                     %
%                 R R    O   O    T    A   A    T     E                       %
%                 R  R    OOO     T    A   A    T     EEEEE                   %
%                                                                             %
%                                                                             %
%                Rotate a raster image by an arbitrary angle.                 %
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
%  Function RotateImage is based on the paper "A Fast Algorithm for General
%  Raster Rotatation" by Alan W. Paeth.  RotateImage is adapted from a similiar
%  routine based on the Paeth paper written by Michael Halle of the Spatial
%  Imaging Group, MIT Media Lab.
%
*/

/*
  Include declarations.
*/
#include "display.h"
#include "image.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o l u m n S h e a r                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure ColumnShear displaces a subcolumn of pixels a specified number of
%  pixels.
%
%  The format of the ColumnShear routine is:
%
%      ColumnShear(source_image,source_columns,column,y,length,displacement,
%        background)
%
%  A description of each parameter follows.
%
%    o source_image: A pointer to a ColorPacket structure which contains the
%      source image.
%
%    o source_columns: Specifies the number of columns in the source image.
%
%    o column: Specifies which column in the image to move.
%
%    o y: Specifies the offset in the source image.
%
%    o length: Specifies the number of pixels to move.
%
%    o displacement: Specifies the number of pixels to displace the column of
%      pixels.
%
%    o background: Specifies the background color.
%
%
*/
static void ColumnShear(source_image,source_columns,column,y,length,
  displacement,background)
ColorPacket
  *source_image;

register unsigned int
  source_columns;

unsigned int
  column,
  y,
  length;

double
  displacement;

ColorPacket
  background;
{
  ColorPacket
    last_pixel;

  double
    fractional_step;

  enum {UP,DOWN}
    direction;

  long int
    blue,
    green,
    int_fractional_step,
    red,
    step;

  register ColorPacket
    *p,
    *q;

  register int
    i;

  if (displacement == 0.0)
    return;
  else
    if (displacement > 0.0)
      direction=DOWN;
    else
      {
        displacement*=(-1.0);
        direction=UP;
      }
  step=(int) floor(displacement);
  fractional_step=displacement-(double) step;
  if (fractional_step == 0.0)
    {
      /*
        No fractional displacement-- just copy the pixels.
      */
      switch (direction)
      {
        case UP:
        {
          p=source_image+y*source_columns+column;
          q=p-step*source_columns;
          for (i=0; i < length; i++)
          {
            *q=(*p);
            q+=source_columns;
            p+=source_columns;
          }
          /*
            Set old column to background color.
          */
          for (i=0; i < step; i++)
          {
            *q=background;
            q+=source_columns;
          }
          break;
        }
        case DOWN:
        {
          p=source_image+(y+length)*source_columns+column;
          q=p+step*source_columns;
          for (i=0; i < length; i++)
          {
            q-=source_columns;
            p-=source_columns;
            *q=(*p);
          }
          /*
            Set old column to background color.
          */
          for (i=0; i < step; i++)
          {
            q-=source_columns;
            *q=background;
          }
          break;
        }
      }
      return;
    }
  /*
    Fractional displacment.
  */
  step++;
  int_fractional_step=(int) (((double) (1 << 14))*fractional_step);
  last_pixel=background;
  switch (direction)
  {
    case UP:
    {
      p=source_image+y*source_columns+column;
      q=p-step*source_columns;
      for (i=0; i < length; i++)
      {
        red=(last_pixel.red*((1 << 14)-int_fractional_step)+p->red*
          int_fractional_step) >> 14;
        if (red > MaxRgb)
          q->red=MaxRgb;
        else
          if (red < 0)
            q->red=0;
          else
            q->red=(unsigned char) red;
        green=(last_pixel.green*((1 << 14)-int_fractional_step)+p->green*
          int_fractional_step) >> 14;
        if (green > MaxRgb)
          q->green=MaxRgb;
        else
          if (green < 0)
            q->green=0;
          else
            q->green=(unsigned char) green;
        blue=(last_pixel.blue*((1 << 14)-int_fractional_step)+p->blue*
          int_fractional_step) >> 14;
        if (blue > MaxRgb)
          q->blue=MaxRgb;
        else
          if (blue < 0)
            q->blue=0;
          else
            q->blue=(unsigned char) blue;
        last_pixel=(*p);
        q+=source_columns;
        p+=source_columns;
      }
      /*
        Set old column to background color.
      */
      red=(last_pixel.red*((1 << 14)-int_fractional_step)+background.red*
        int_fractional_step) >> 14;
      if (red > MaxRgb)
        q->red=MaxRgb;
      else
        if (red < 0)
          q->red=0;
        else
          q->red=(unsigned char) red;
      green=(last_pixel.green*((1 << 14)-int_fractional_step)+background.green*
        int_fractional_step) >> 14;
      if (green > MaxRgb)
        q->green=MaxRgb;
      else
        if (green < 0)
          q->green=0;
        else
          q->green=(unsigned char) green;
      blue=(last_pixel.blue*((1 << 14)-int_fractional_step)+background.blue*
        int_fractional_step) >> 14;
      if (blue > MaxRgb)
        q->blue=MaxRgb;
      else
        if (blue < 0)
          q->blue=0;
        else
          q->blue=(unsigned char) blue;
      q+=source_columns;
      for (i=0; i < step-1; i++)
      {
        *q=background;
        q+=source_columns;
      }
      break;
    }
    case DOWN:
    {
      p=source_image+(y+length)*source_columns+column;
      q=p+step*source_columns;
      for (i=0; i < length; i++)
      {
        q-=source_columns;
        p-=source_columns;
        red=(last_pixel.red*((1 << 14)-int_fractional_step)+p->red*
          int_fractional_step) >> 14;
        if (red > MaxRgb)
          q->red=MaxRgb;
        else
          if (red < 0)
            q->red=0;
          else
            q->red=(unsigned char) red;
        green=(last_pixel.green*((1 << 14)-int_fractional_step)+p->green*
          int_fractional_step) >> 14;
        if (green > MaxRgb)
          q->green=MaxRgb;
        else
          if (green < 0)
            q->green=0;
          else
            q->green=(unsigned char) green;
        blue=(last_pixel.blue*((1 << 14)-int_fractional_step)+p->blue*
          int_fractional_step) >> 14;
        if (blue > MaxRgb)
          q->blue=MaxRgb;
        else
          if (blue < 0)
            q->blue=0;
          else
            q->blue=(unsigned char) blue;
        last_pixel=(*p);
      }
      /*
        Set old column to background color.
      */
      q-=source_columns;
      red=(last_pixel.red*((1 << 14)-int_fractional_step)+background.red*
        int_fractional_step) >> 14;
      if (red > MaxRgb)
        q->red=MaxRgb;
      else
        if (red < 0)
          q->red=0;
        else
          q->red=(unsigned char) red;
      green=(last_pixel.green*((1 << 14)-int_fractional_step)+background.green*
        int_fractional_step) >> 14;
      if (green > MaxRgb)
        q->green=MaxRgb;
      else
        if (green < 0)
          q->green=0;
        else
          q->green=(unsigned char) green;
      blue=(last_pixel.blue*((1 << 14)-int_fractional_step)+background.blue*
        int_fractional_step) >> 14;
      if (blue > MaxRgb)
        q->blue=MaxRgb;
      else
        if (blue < 0)
          q->blue=0;
        else
          q->blue=(unsigned char) blue;
      for (i=0; i < step-1; i++)
      {
        q-=source_columns;
        *q=background;
      }
      break;
    }
  }
  return;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n t e g r a l R o t a t i o n                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function IntegralRotation rotates the source image starting at location
%  (x,y) an integral of 90 degrees and copies the result to the rotated image
%  buffer.
%
%  The format of the IntegralRotation routine is:
%
%      IntegralRotation(image,source_columns,source_rows,rotated_image,
%        rotated_columns,x,y,rotations)
%
%  A description of each parameter follows.
%
%    o source_image: A pointer to a Image structure containing the source
%      image.
%
%    o source_columns: Specifies the number of columns of pixels in the
%      source image.
%
%    o source_rows: Specifies the number of rows of pixels in the source
%      image.
%
%    o rotated_image: A pointer to a ColorPacket structure containing the
%      rotated image.
%
%    o rotated_columns: Specifies the number of columns of pixels in the
%      rotated image.
%
%    o x: Specifies the x offset in the source image.
%
%    o y: Specifies the y offset in the source image.
%
%    o rotations: Specifies the number of 90 degree rotations.
%
%
*/
static void IntegralRotation(image,source_columns,source_rows,rotated_image,
  rotated_columns,x,y,rotations)
Image
  *image;

unsigned int
  source_columns,
  source_rows;

ColorPacket
  *rotated_image;

unsigned int
  rotated_columns,
  x,
  y,
  rotations;
{
  ColorPacket
    *q;

  register RunlengthPacket
    *p;

  register int
    i,
    j;

  /*
    Expand runlength packets into a rectangular array of pixels.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  switch (rotations)
  {
    case 0:
    {
      /*
        Rotate 0 degrees.
      */
      for (j=0; j < source_rows; j++)
      {
        q=rotated_image+rotated_columns*(y+j)+x;
        for (i=0; i < source_columns; i++)
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
          q++;
        }
      }
      break;
    }
    case 1:
    {
      /*
        Rotate 90 degrees.
      */
      for (j=source_columns-1; j >= 0; j--)
      {
        q=rotated_image+rotated_columns*y+x+j;
        for (i=0; i < source_rows; i++)
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
          q+=rotated_columns;
        }
      }
      break;
    }
    case 2:
    {
      /*
        Rotate 180 degrees.
      */
      q=rotated_image;
      for (j=source_rows-1; j >= 0; j--)
      {
        q=rotated_image+rotated_columns*(y+j)+x+source_columns;
        for (i=0; i < source_columns; i++)
        {
          if (image->runlength > 0)
            image->runlength--;
          else
            {
              p++;
              image->runlength=p->length;
            }
          q--;
          q->red=p->red;
          q->green=p->green;
          q->blue=p->blue;
          q->index=p->index;
        }
      }
      break;
    }
    case 3:
    {
      /*
        Rotate 270 degrees.
      */
      for (j=0; j < source_columns; j++)
      {
        q=rotated_image+rotated_columns*(y+source_rows)+x+j-rotated_columns;
        for (i=0; i < source_rows; i++)
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
          q-=rotated_columns;
        }
      }
      break;
    }
    default:
      break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R o w S h e a r                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure RowShear displaces a subrow of pixels a specified number of
%  pixels.
%
%  The format of the RowShear routine is:
%
%      RowShear(source_image,source_columns,row,y,length,displacement,
%        background)
%
%  A description of each parameter follows.
%
%    o source_image: A pointer to a ColorPacket structure.
%
%    o source_columns: Specifies the number of columns in the source image.
%
%    o row: Specifies which row in the image to move.
%
%    o y: Specifies the offset in the source image.
%
%    o length: Specifies the number of pixels to move.
%
%    o displacement: Specifies the number of pixels to displace the row of
%      pixels.
%
%    o background: Specifies the background color.
%
%
*/
static void RowShear(source_image,source_columns,row,x,length,displacement,
  background)
ColorPacket
  *source_image;

unsigned int
  source_columns,
  row,
  x,
  length;

double
  displacement;

ColorPacket
  background;
{
  ColorPacket
    last_pixel;

  double
    fractional_step;

  enum {LEFT,RIGHT}
    direction;

  long int
    blue,
    green,
    int_fractional_step,
    red,
    step;

  register ColorPacket
    *p,
    *q;

  register int
    i;

  if (displacement == 0.0)
    return;
  else
    if (displacement > 0.0)
      direction=RIGHT;
    else
      {
        displacement*=(-1.0);
        direction=LEFT;
      }
  step=(int) floor(displacement);
  fractional_step=displacement-(double)step;
  if (fractional_step == 0.0)
    {
      /*
        No fractional displacement-- just copy.
      */
      switch (direction)
      {
        case LEFT:
        {
          p=source_image+row*source_columns+x;
          q=p-step;
          for (i=0; i < length; i++)
          {
            *q=(*p);
            q++;
            p++;
          }
          /*
            Set old row to background color.
          */
          for (i=0; i < step; i++)
          {
            *q=background;
            q++;
          }
          break;
        }
        case RIGHT:
        {
          /*
            Right is the same as left except data is transferred backwards
            to prevent deleting data we need later.
          */
          p=source_image+row*source_columns+x+length;
          q=p+step;
          for (i=0; i < length; i++)
          {
            p--;
            q--;
            *q=(*p);
          }
          /*
            Set old row to background color.
          */
          for (i=0; i < step; i++)
          {
            q--;
            *q=background;
          }
          break;
        }
      }
      return;
    }
  /*
    Fractional displacement.
  */
  step++;
  int_fractional_step=(int) (((double) (1 << 14))*fractional_step);
  last_pixel=background;
  switch (direction)
  {
    case LEFT:
    {
      p=source_image+row*source_columns+x;
      q=p-step;
      for (i=0; i < length; i++)
      {
        red=(last_pixel.red*((1 << 14)-int_fractional_step)+p->red*
          int_fractional_step) >> 14;
        if (red > MaxRgb)
          q->red=MaxRgb;
        else
          if (red < 0)
            q->red=0;
          else
            q->red=(unsigned char) red;
        green=(last_pixel.green*((1 << 14)-int_fractional_step)+p->green*
          int_fractional_step) >> 14;
        if (green > MaxRgb)
          q->green=MaxRgb;
        else
          if (green < 0)
            q->green=0;
          else
            q->green=(unsigned char) green;
        blue=(last_pixel.blue*((1 << 14)-int_fractional_step)+p->blue*
          int_fractional_step) >> 14;
        if (blue > MaxRgb)
          q->blue=MaxRgb;
        else
          if (blue < 0)
            q->blue=0;
          else
            q->blue=(unsigned char) blue;
        last_pixel=(*p);
        p++;
        q++;
      }
      /*
        Set old row to background color.
      */
      red=(last_pixel.red*((1 << 14)-int_fractional_step)+background.red*
        int_fractional_step) >> 14;
      if (red > MaxRgb)
        q->red=MaxRgb;
      else
        if (red < 0)
          q->red=0;
        else
          q->red=(unsigned char) red;
      green=(last_pixel.green*((1 << 14)-int_fractional_step)+background.green*
        int_fractional_step) >> 14;
      if (green > MaxRgb)
        q->green=MaxRgb;
      else
        if (green < 0)
          q->green=0;
        else
          q->green=(unsigned ) green;
      blue=(last_pixel.blue*((1 << 14)-int_fractional_step)+background.blue*
        int_fractional_step) >> 14;
      if (blue > MaxRgb)
        q->blue=MaxRgb;
      else
        if (blue < 0)
          q->blue=0;
        else
          q->blue=(unsigned char) blue;
      q++;
      for (i=0; i < step-1; i++)
      {
        *q=background;
        q++;
      }
      break;
    }
    case RIGHT:
    {
      p=source_image+row*source_columns+x+length;
      q=p+step;
      for (i=0; i < length; i++)
      {
        p--;
        q--;
        red=(last_pixel.red*((1 << 14)-int_fractional_step)+p->red*
          int_fractional_step) >> 14;
        if (red > MaxRgb)
          q->red=MaxRgb;
        else
          if (red < 0)
            q->red=0;
          else
            q->red=(unsigned char) red;
        green=(last_pixel.green*((1 << 14)-int_fractional_step)+p->green*
          int_fractional_step) >> 14;
        if (green > MaxRgb)
          q->green=MaxRgb;
        else
          if (green < 0)
            q->green=0;
          else
            q->green=(unsigned char) green;
        blue=(last_pixel.blue*((1 << 14)-int_fractional_step)+p->blue*
          int_fractional_step) >> 14;
        if (blue > MaxRgb)
          blue=MaxRgb;
        else
          if (blue < 0)
            blue=0;
          else
            q->blue=(unsigned char) blue;
        last_pixel=(*p);
      }
      /*
        Set old row to background color.
      */
      q--;
      red=(last_pixel.red*((1 << 14)-int_fractional_step)+background.red*
        int_fractional_step) >> 14;
      if (red > MaxRgb)
        red=MaxRgb;
      else
        if (red < 0)
          red=0;
        else
          q->red=(unsigned char) red;
      green=(last_pixel.green*((1 << 14)-int_fractional_step)+background.green*
        int_fractional_step) >> 14;
      if (green > MaxRgb)
        green=MaxRgb;
      else
        if (green < 0)
          green=0;
        else
          q->green=(unsigned char) green;
      blue=(last_pixel.blue*((1 << 14)-int_fractional_step)+background.blue*
        int_fractional_step) >> 14;
      if (blue > MaxRgb)
        blue=MaxRgb;
      else
        if (blue < 0)
          blue=0;
        else
          q->blue=(unsigned char) blue;
      for (i=0; i < step-1; i++)
      {
        q--;
        *q=background;
      }
      break;
    }
  }
  return;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R o t a t e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function RotateImage creates a new image that is a rotated copy of an
%  existing one.  It allocates the memory necessary for the new Image structure %  and returns a pointer to the new image.
%
%  Function RotateImage is based on the paper "A Fast Algorithm for General
%  Raster Rotatation" by Alan W. Paeth.  RotateImage is adapted from a similiar
%  routine based on the Paeth paper written by Michael Halle of the Spatial
%  Imaging Group, MIT Media Lab.
%
%  The format of the RotateImage routine is:
%
%      RotateImage(image,degrees,clip)
%
%  A description of each parameter follows.
%
%    o status: Function RotateImage returns a pointer to the image after
%      rotating.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o degrees: Specifies the number of degrees to rotate the image.
%
%    o clip: A value other than zero clips the corners of the rotated
%      image and retains the original image size.
%
%
*/
Image *RotateImage(image,degrees,clip)
Image
  *image;

double
  degrees;

int
  clip;
{
#define DegreesToRadians(x) ((x)/180.0*3.14159265359)

  ColorPacket
    background,
    *rotated_pixels;

  double
    x_shear,
    y_shear;

  extern Image
    *CopyImage();

  extern void
    DestroyImage();

  Image
    *rotated_image;

  register ColorPacket
    *p;

  register int
    i,
    x,
    y;

  register RunlengthPacket
    *q;

  unsigned int
    number_rows,
    number_columns,
    rotations,
    x_offset,
    y_offset,
    y_width;

  /*
    Adjust rotation angle.
  */
  while (degrees < -45.0)
    degrees+=360.0;
  rotations=0;
  while (degrees > 45.0)
  {
    degrees-=90.0;
    rotations++;
  }
  rotations%=4;
  /*
    Calculate shear equations.
  */
  x_shear=(-tan(DegreesToRadians(degrees)/2.0));
  y_shear=sin(DegreesToRadians(degrees));
  if ((rotations == 1) || (rotations == 3))
    {
      /*
        Invert image size.
      */
      y_width=image->rows+
        (int) ceil(fabs(x_shear)*(double) (image->columns-1));
      number_columns=image->rows+2*
        (int) ceil(fabs(x_shear)*(double) (image->columns-1));
      number_rows=image->columns+
        (int) ceil(fabs(y_shear)*(double) (y_width-1));
      rotated_image=CopyImage(image,number_columns,number_rows,False);
      if (rotated_image == (Image *) NULL)
        {
          Warning("unable to rotate image","memory allocation failed");
          return((Image *) NULL);
        }
      rotated_image->columns=image->rows;
      rotated_image->rows=image->columns;
    }
  else
    {
      y_width=image->columns+
        (int) ceil(fabs(x_shear)*(double) (image->rows-1));
      number_columns=image->columns+2*
        (int) ceil(fabs(x_shear)*(double) (image->rows-1));
      number_rows=image->rows+
        (int) ceil(fabs(y_shear)*(double) (y_width-1));
      rotated_image=CopyImage(image,number_columns,number_rows,False);
      if (rotated_image == (Image *) NULL)
        {
          Warning("unable to rotate image","memory allocation failed");
          return((Image *) NULL);
        }
      rotated_image->columns=image->columns;
      rotated_image->rows=image->rows;
    }
  /*
    Initialize rotated image attributes.
  */
  rotated_pixels=(ColorPacket *)
    malloc(number_columns*(number_rows+2)*sizeof(ColorPacket));
  if (rotated_pixels == (ColorPacket *) NULL)
    {
      Warning("unable to rotate image","memory allocation failed");
      return((Image *) NULL);
    }
  if ((x_shear == 0.0) || (y_shear == 0.0))
    {
      /*
        No shearing required; do integral rotation.
      */
      x_offset=0;
      y_offset=0;
      IntegralRotation(image,rotated_image->columns,rotated_image->rows,
        rotated_pixels+number_columns,number_columns,x_offset,y_offset,
        rotations);
    }
  else
    {
      typedef struct Point
      {
        double
          x,
          y;
      } Point;

      double
        x_max,
        x_min,
        y_max,
        y_min;

      Point
        corners[4];

      unsigned int
        column,
        row;

      /*
        Initialize rotated image buffer to background color.
      */
      rotated_image->class=DirectClass;
      background.red=image->pixels[0].red;
      background.green=image->pixels[0].green;
      background.blue=image->pixels[0].blue;
      p=rotated_pixels;
      for (i=0; i < (number_columns*(number_rows+2)); i++)
      {
        *p=background;
        p++;
      }
      /*
        Perform an initial integral 90 degree rotation.
      */
      x_offset=(number_columns-rotated_image->columns)/2;
      y_offset=(number_rows-rotated_image->rows)/2;
      IntegralRotation(image,rotated_image->columns,rotated_image->rows,
        rotated_pixels+number_columns,number_columns,x_offset,y_offset,
        rotations);
      /*
        Perform a fractional rotation.  First, shear the image rows.
      */
      row=(number_rows-rotated_image->rows)/2;
      for (i=0; i < rotated_image->rows; i++)
      {
        RowShear(rotated_pixels+number_columns,number_columns,row,x_offset,
          rotated_image->columns,x_shear*
          (((double) i)-(rotated_image->rows-1)/2.0),background);
        row++;
      }
      /*
        Shear the image columns.
      */
      column=(number_columns-y_width)/2;
      for (i=0; i < y_width; i++)
      {
        ColumnShear(rotated_pixels+number_columns,number_columns,column,
          y_offset,rotated_image->rows,y_shear*(((double) i)-(y_width-1)/2.0),
          background);
        column++;
      }
      /*
        Shear the image rows again.
      */
      for (i=0; i < number_rows; i++)
        RowShear(rotated_pixels+number_columns,number_columns,(unsigned int) i,
          (number_columns-y_width)/2,y_width,x_shear*
          (((double) i)-(number_rows-1)/2.0),background);
      /*
        Calculate the rotated image size.
      */
      corners[0].x=(-((int) rotated_image->columns)/2.0);
      corners[0].y=(-((int) rotated_image->rows)/2.0);
      corners[1].x=((int) rotated_image->columns)/2.0;
      corners[1].y=(-((int) rotated_image->rows)/2.0);
      corners[2].x=(-((int) rotated_image->columns)/2.0);
      corners[2].y=((int) rotated_image->rows)/2.0;
      corners[3].x=((int) rotated_image->columns)/2.0;
      corners[3].y=((int) rotated_image->rows)/2.0;
      for (i=0; i < 4; i++)
      {
        corners[i].x+=x_shear*corners[i].y;
        corners[i].y+=y_shear*corners[i].x;
        corners[i].x+=x_shear*corners[i].y;
        corners[i].x+=(number_columns-1)/2.0;
        corners[i].y+=(number_rows-1)/2.0;
      }
      x_min=corners[0].x;
      y_min=corners[0].y;
      x_max=corners[0].x;
      y_max=corners[0].y;
      for (i=1; i < 4; i++)
      {
        if (x_min > corners[i].x)
          x_min=corners[i].x;
        if (y_min > corners[i].y)
          y_min=corners[i].y;
        if (x_max < corners[i].x)
          x_max=corners[i].x;
        if (y_max < corners[i].y)
          y_max=corners[i].y;
      }
      x_min=floor((double) x_min);
      x_max=ceil((double) x_max);
      y_min=floor((double) y_min);
      y_max=ceil((double) y_max);
      if (!clip)
        {
          /*
            Rotated image is not clipped.
          */
          rotated_image->columns=(unsigned int) (x_max-x_min);
          rotated_image->rows=(unsigned int) (y_max-y_min);
        }
      x_offset=(int) x_min+((int) (x_max-x_min)-rotated_image->columns)/2;
      y_offset=(int) y_min+((int) (y_max-y_min)-rotated_image->rows)/2;
    }
  /*
    Convert the rectangular array of pixels to runlength packets.
  */
  rotated_image->packets=rotated_image->columns*rotated_image->rows;
  q=rotated_image->pixels;
  for (y=0; y < rotated_image->rows; y++)
  {
    p=rotated_pixels+number_columns+(y+y_offset)*number_columns+x_offset;
    for (x=0; x < rotated_image->columns; x++)
    {
      q->red=p->red;
      q->green=p->green;
      q->blue=p->blue;
      q->index=p->index;
      q->length=0;
      q++;
      p++;
    }
  }
  (void) free((char *) rotated_pixels);
  return(rotated_image);
}
