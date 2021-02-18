/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%           CCCC   OOO   M   M  PPPP   RRRR   EEEEE   SSSSS  SSSSS            %
%          C      O   O  MM MM  P   P  R   R  E       SS     SS               %
%          C      O   O  M M M  PPPP   RRRR   EEE      SSS    SSS             %
%          C      O   O  M   M  P      R R    E          SS     SS            %
%           CCCC   OOO   M   M  P      R  R   EEEEE   SSSSS  SSSSS            %
%                                                                             %
%                                                                             %
%              Predictive Lossless Image Compression Algorithm                %
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
%  Procedures QEncodeImage and QDecodeImage is based on the document
%  "JPEG-9-R6 Working Draft for Development of JPEG CD", January 1991.
%  These routines do not yet fully implement the lossless JPEG compression,
%  but will some time in the future.
%
%  Images are compressed using a simple predictive method.  The predictor 
%  combines three neighboring samples (A, B, and C) to form a prediction
%  of the sample X:
%
%    C B
%    A X
%
%  The prediction formula is A + B - C.  The prediction is subtracted from
%  from the actual sample X and the difference is encoded by an arithmetic
%  entropy coding method.
%
%
*/

/*
  Include declarations.
*/
#include "display.h"
/*
  Define declarations.
*/
#define LowerBound  0
#define MaxContextStates  121  
#define MinimumIntervalD  (unsigned short) 0xf000  /* ~-0.75 */
#define MinimumIntervalE  (unsigned short) 0x1000  /* ~0.75 */
#define No  0
#define UpperBound  2
#define Yes  1
/*
  State classification.
*/
#define ZeroState  0
#define SmallPostitiveState  1
#define SmallNegativeState  2
#define LargePostitiveState  3
#define LargeNegativeState  4
/*
  Typedef declarations.
*/
typedef struct _ScanlinePacket
{
  unsigned char
    pixel;

  int
    state;
} ScanlinePacket;
/*
  Initialized declarations.
*/
int decrement_less_probable[]=
{
  0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 
  2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 3, 2, 3, 2
};

int increment_more_probable[]=
{
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0
};

int more_probable_exchange[]=
{
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int statistics[][5]=
{
   0,  4,  8, 12, 16, 
  20, 24, 28, 32, 36, 
  40, 44, 48, 52, 56, 
  60, 64, 68, 72, 76, 
  80, 84, 88, 92, 96,
};

unsigned short probability[]=
{
  0x0ac1, 0x0a81, 0x0a01, 0x0901, 0x0701, 0x0681,
  0x0601, 0x0501, 0x0481, 0x0441, 0x0381, 0x0301,
  0x02c1, 0x0281, 0x0241, 0x0181, 0x0121, 0x00e1,
  0x00a1, 0x0071, 0x0059, 0x0053, 0x0027, 0x0017,
  0x0013, 0x000b, 0x0007, 0x0005, 0x0003, 0x0001
};
/*
  Declarations and initializations for predictive arithimetic coder.
*/
int
  code,
  less_probable[MaxContextStates],
  more_probable[MaxContextStates],
  probability_estimate[MaxContextStates];

unsigned char
  *q;  

unsigned short
  interval;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e c o d e                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Decode uncompresses a string.
%
%  The format of the Decode routine is:
%
%      (void) Decode(state,decision)
%
%  A description of each parameter follows:
%
%    o state:  An integer value representing the current state.
%
%    o decision:  A pointer to an integer.  The output of the binary
%      decision (Yes/No) is returned in this value.
%
%
*/
void Decode(state,decision)
register int
  state,
  *decision;
{
  interval+=probability[probability_estimate[state]];
  if (((code >> 16) & 0xffff) < interval)
    {
      code-=(interval << 16);
      interval=(-probability[probability_estimate[state]]);
      *decision=less_probable[state];
    }
  else
    {
      *decision=more_probable[state];
      if (interval <= MinimumIntervalD)
        return;
    }
  do
  {
    if ((code & 0xff) == 0)
      {
        code&=0xffff0000;
        if ((*q++) == 0xff)
          code+=((int) (*q) << 9)+0x02;
        else
          code+=((int) (*q) << 8)+0x01;
      }
    interval<<=1;
    code<<=1;
  } while (interval > MinimumIntervalD);
  /*
    Update probability estimates
  */
  if (*decision == more_probable[state])
    probability_estimate[state]+=
      increment_more_probable[probability_estimate[state]];
  else
    probability_estimate[state]-=
      decrement_less_probable[probability_estimate[state]];
  if (more_probable_exchange[probability_estimate[state]] != 0)
    {
      /*
        Exchange sense of most probable and least probable.
      */
      less_probable[state]=more_probable[state];
      more_probable[state]=1-more_probable[state];
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E n c o d e                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Encode generate compressed data string by encoding yes-no decision
%  given state s.
%
%  The format of the Encode routine is:
%
%      (void) Encode(state,decision)
%
%  A description of each parameter follows:
%
%    o state:  An integer value representing the current state.
%
%    o decision:  An integer value representing a binary decision.
%
%
*/
void Encode(state,decision)
register int
  state,
  decision;
{
  /*
    Test on "most-probable-symbol" for state s(more_probable[state])
  */
  interval-=probability[probability_estimate[state]];
  if (more_probable[state] != decision)
    {
      code-=interval;
      interval=probability[probability_estimate[state]];
    }
  else
    if (interval >= MinimumIntervalE)
      return;
  /*
    Encoder renormalization.
  */
  do
  {
    interval<<=1;
    if (code >= 0)
      code<<=1;
    else
      {
        /*
          Shift unsigned char of data from Code register to compressed string.
        */
        code<<=1;
        if (code > 0)
          {
            /*
              Add eight bits from Code register to compressed data string.
            */
            (*q++)--;
            *q=(unsigned char) (code >> 16);
            code&=0x0000ffff;
            code|=0x01800000;
          }
        else
          {
            code&=0x01ffffff;
            if (interval > code)
              {
                /*
                  Add eight bits from Code register to compressed data string.
                */
                (*q++)--;
                *q=0xff;
                code|=0x01810000;
              }
            else
              if ((*q++) == 0xff)
                {
                  /*
                    Add seven bits from Code register plus one stuffed bit to
                    compressed data string.
                  */
                  *q=(unsigned char) (code >> 17);
                  code&=0x0001ffff;
                  code|=0x03000000;
                }
              else
                {
                  /*
                    Add eight bits from Code register to compressed data string.
                  */
                  *q=(unsigned char) (code >> 16);
                  code&=0x0000ffff;
                  code|=0x01800000;
                }
          }
      }
  } while (interval < MinimumIntervalE);
  /*
    Update probability estimates
  */
  if (decision == more_probable[state])
    probability_estimate[state]+=
      increment_more_probable[probability_estimate[state]];
  else
    probability_estimate[state]-=
      decrement_less_probable[probability_estimate[state]];
  if (more_probable_exchange[probability_estimate[state]] != 0)
    more_probable[state]=1-more_probable[state];
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F l u s h                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Flush flushes the final bits of data from the Code register to the
%  compressed data string.
%
%  The format of the Flush routine is:
%
%      (void) Flush()
%
%
*/
void Flush()
{
  register int
    extra_bits;

  code-=interval;
  extra_bits=24;
  extra_bits--;
  while (code >= 0)
  {
    code<<=1;
    extra_bits--;
  }
  code<<=1;
  if (code > 0)
    (*q)--;
  /*
    Add the final compressed data unsigned chars to the compressed data string.
  */
  do
  {
    if ((*q++) == 0xff)
      {
        /*
          Add seven bits of data plus one stuffed bit to the compressed data
          string during final Flush of Code register.
        */
        *q=(unsigned char) (code >> 17);
        code&=0x0001ffff;
        code<<=7;
        extra_bits-=7;
      }
    else
      {
        /*
           Add eight bits of data to the compressed data string during final
           flush of Code register.
        */
        *q=(unsigned char) (code >> 16);
        code&=0x0000ffff;
        code<<=8;
        extra_bits-=8;
      }
  } while (extra_bits > 0);
  q++;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q D e c o d e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function QDecodeImage uncompresses an image via Q-coding.
%
%  The format of the QDecodeImage routine is:
%
%      count=QDecodeImage(compressed_pixels,pixels,number_columns,number_rows)
%
%  A description of each parameter follows:
%
%    o count:  The QDecodeImage routine returns this integer value.  It is
%      the actual number of pixels created by the decompression process. 
%
%    o compressed_pixels:  The address of a byte (8 bits) array of compressed
%      pixel data.
%
%    o pixels:  The address of a byte (8 bits) array of pixel data created by 
%      the uncompression process.  The number of bytes in this array
%      must be at least equal to the number columns times the number of rows
%      of the source pixels.
%
%    o number_columns:  An integer value that is the number of columns or
%      width in pixels of your source image.
%
%    o number_rows:  An integer value that is the number of rows or
%      heigth in pixels of your source image.
%
%
*/
unsigned int QDecodeImage(compressed_pixels,pixels,number_columns,number_rows)
unsigned char
  *compressed_pixels,
  *pixels;

unsigned int
  number_columns,
  number_rows;
{
  int
    decision,
    i,
    prediction,
    row;

  register int
    column,
    magnitude,
    sign,
    state,
    value;

  register ScanlinePacket
    *cs,
    *ls;

  register unsigned char
    *p;

  ScanlinePacket
    *scanline;

  for (i=0; i < MaxContextStates; i++)
  {
    probability_estimate[i]=0;
    more_probable[i]=0;
    less_probable[i]=1;
  }
  /*
    Allocate scanline for row values and states
  */
  scanline=(ScanlinePacket *)
    malloc((unsigned int) (2*(number_columns+1)*sizeof(ScanlinePacket)));
  if (scanline == (ScanlinePacket *) NULL)
    {
      Warning("unable to compress image, unable to allocate memory",
        (char *) NULL);
      exit(1);
    }
  cs=scanline;
  for (i=0; i < 2*(number_columns+1); i++)
  {
    cs->pixel=0;
    cs->state=ZeroState;
    cs++;
  }
  interval=MinimumIntervalD;
  p=pixels;
  q=compressed_pixels+1;
  /*
    Add a new unsigned char of compressed data to the Code register.
  */
  code=(int) (*q) << 16;
  if ((*q++) == 0xff)
    code+=((int) (*q) << 9)+0x02;
  else
    code+=((*q) << 8)+0x01;
  code<<=4;
  code+=(interval << 16);
  /*
    Decode each image scanline.
  */
  for (row=0; row < number_rows; row++)
  {
    ls=scanline+(number_columns+1)*((row+0) % 2);
    cs=scanline+(number_columns+1)*((row+1) % 2);
    for (column=0; column < number_columns; column++)
    {
      prediction=(int) cs->pixel-(int) ls->pixel;
      ls++;
      prediction+=(int) ls->pixel;
      state=statistics[cs->state][ls->state];
      cs++;
      cs->state=ZeroState;
      /*
        Branch for zero code value
      */
      (void) Decode(state,&decision);
      if (decision == No)
        value=0;
      else
        {
          /*
            Decode sign information
          */
          state++;
          (void) Decode(state,&decision);
          if (decision == Yes)
            sign=(-1);
          else
            {
              sign=1;
              state++;
            }
          state++;
          /*
            Branch for value=+-1
          */
          (void) Decode(state,&decision);
          if (decision == No)
            value=1;
          else
            {
              /*
                Establish magnitude of value.
              */
              magnitude=2;
              state=100;
              (void) Decode(state,&decision);
              while (decision != No)
              {
                if (state < 107)
                  state++;
                magnitude<<=1;
                (void) Decode(state,&decision);
              }
              /*
                Code remaining bits.
              */
              state+=7;
              value=1;
              magnitude>>=2;
              if (magnitude != 0)
                {
                  (void) Decode(state,&decision);
                  state+=6;
                  value=(value << 1) | decision;
                  magnitude>>=1;
                  while (magnitude != 0)
                  {
                    (void) Decode(state,&decision);
                    value=(value << 1) | decision;
                    magnitude>>=1;
                  }
                }
              value++;
            }
          if (value > LowerBound)
            if (value <= UpperBound)
              cs->state=
                (sign < ZeroState ? SmallPostitiveState : SmallNegativeState);
            else
              cs->state=
                (sign < ZeroState ? LargePostitiveState : LargeNegativeState);
          if (sign < 0)
            value=(-value);
        }
      cs->pixel=(unsigned char) (value+prediction);
      *p++=cs->pixel;
    }
  }
  (void) free((char *) scanline);
  return((unsigned int) (p-pixels));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q E n c o d e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function QEncodeImage compresses an image via q-coding.
%
%  The format of the QEncodeImage routine is:
%
%      count=QEncodeImage(pixels,compressed_pixels,number_columns,
%        number_rows)
%
%  A description of each parameter follows:
%
%    o count:  The QEncodeImage routine returns this integer value.  It is
%      the actual number of compressed pixels created by the compression
%      process. 
%
%    o pixels:  The address of a byte (8 bits) array of pixel data.
%
%    o compressed_pixels:  The address of a byte (8 bits) array of pixel data
%      created by the compression process.  The number of bytes in this array
%      must be at least equal to the number columns times the number of rows
%      of the source pixels to allow for the possibility that no compression
%      is possible.  The actual number of bytes used is reflected by the
%      count parameter.
%
%    o number_columns:  An integer value that is the number of columns or
%      width in pixels of your source image.
%
%    o number_rows:  An integer value that is the number of rows or
%      heigth in pixels of your source image.
%
%
%
*/
unsigned int QEncodeImage(pixels,compressed_pixels,number_columns,number_rows)
unsigned char
  *pixels,
  *compressed_pixels;

unsigned int
  number_columns,
  number_rows;
{
  int
    i,
    prediction,
    row;

  register int
    column,
    magnitude,
    sign,
    state,
    value;

  register ScanlinePacket
    *cs,
    *ls;

  register unsigned char
    *p;

  ScanlinePacket
    *scanline;

  void
    Encode(),
    Flush();

  for (i=0; i < MaxContextStates; i++)
  {
    probability_estimate[i]=0;
    more_probable[i]=0;
  }
  /*
    Allocate scanline for row values and states.
  */
  scanline=(ScanlinePacket *)
    malloc((unsigned int) (2*(number_columns+1)*sizeof(ScanlinePacket)));
  if (scanline == (ScanlinePacket *) NULL)
    {
      Warning("unable to compress image, unable to allocate memory",
        (char *) NULL);
      exit(1);
    }
  cs=scanline;
  for (i=0; i < 2*(number_columns+1); i++)
  {
    cs->pixel=0;
    cs->state=ZeroState;
    cs++;
  }
  interval=MinimumIntervalE;
  p=pixels;
  q=compressed_pixels;
  (*q)++;
  code=0x00180000;
  /*
    Encode each scanline.
  */
  for (row=0; row < number_rows; row++)
  {
    ls=scanline+(number_columns+1)*((row+0) % 2);
    cs=scanline+(number_columns+1)*((row+1) % 2);
    for (column=0; column < number_columns; column++)
    {
      prediction=(int) cs->pixel-(int) ls->pixel;
      ls++;
      prediction+=(int) ls->pixel;
      state=statistics[cs->state][ls->state];
      cs++;
      cs->pixel=(*p++);
      cs->state=ZeroState;
      value=(int) cs->pixel-prediction;
      Encode(state,(value == 0 ? No : Yes));
      if (value != 0)
        {
          /*
            Code sign information
          */
          state++;
          sign=(value < 0 ? -1 : 1);
          Encode(state,(sign >= 0 ? No : Yes));
          if (sign < 0)
            value=(-value);
          else
            state++;
          state++;
          value--;
          /*
            Branch for code=+-1
          */
          Encode(state,(value == 0 ? No : Yes));
          if (value != 0)
            {
              /*
                Establish magnitude of value.
              */
              state=100;
              magnitude=2;
              while (value >= magnitude)
              {
                (void) Encode(state,Yes);
                if (state < 107)
                  state++;
                magnitude<<=1;
              }
              (void) Encode(state,No);
              /*
                Code remaining bits
              */
              state+=7;
              magnitude>>=2;
              if (magnitude != 0)
                {
                  (void) Encode(state,((magnitude & value) == 0 ? No : Yes));
                  state+=6;
                  magnitude>>=1;
                  while (magnitude != 0)
                  {
                    (void) Encode(state,((magnitude & value) == 0 ? No : Yes));
                    magnitude>>=1;
                  }
                }
            }
          if (value >= LowerBound)
            if (value < UpperBound)
              cs->state=
                (sign < ZeroState ? SmallPostitiveState : SmallNegativeState);
            else
              cs->state=
                (sign < ZeroState ? LargePostitiveState : LargeNegativeState);
        }
    }
  }
  (void) Flush();
  (void) free((char *) scanline);
  return((unsigned int) (q-compressed_pixels));
}
