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
%                  Export MIFF image to a GIF raster format.                  %
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
%  export image.miff image.gif
%
%  Specify 'image.miff' as '-' for standard input.  
%  Specify 'image.gif' as '-' for standard output.
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
  (void) fprintf(stderr,"Usage: %s image.miff image.gif\n\n",application_name);
  (void) fprintf(stderr,"Specify 'image.miff' as '-' for standard input.\n");
  (void) fprintf(stderr,"Specify 'image.gif' as '-' for standard output.\n");
  exit(1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e G I F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteGIFImage writes an image to a file on disk in GIF 
%  rasterfile format.
%
%  The format of the WriteGIFImage routine is:
%
%      WriteGIFImage(image)
%
%  A description of each parameter follows.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static void WriteGIFImage(image)
Image
  *image;
{
  register int
    i,
    j,
    x;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    Blue[256],
    Green[256],
    *pixels,
    Red[256];

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
    Image must be less than or equal to 256 colors.
  */
  if ((image->class == DirectClass) || (image->colors > 256))
    QuantizeImage(image,256,8,False,True);
  for (i=0; i < image->colors; i++)
  {
    Red[i]=image->colormap[i].red;
    Green[i]=image->colormap[i].green;
    Blue[i]=image->colormap[i].blue;
  }
 /*
    Convert runlength packets to a rectangular array of colormap indexes.
  */
  pixels=(unsigned char *) 
    malloc(image->columns*image->rows*sizeof(unsigned char));
  if (pixels == (unsigned char *) NULL)
    Error("unable to allocate memory",(char *) NULL);
  p=image->pixels;
  q=pixels;
  for (i=0; i < image->packets; i++)
  {
    for (j=0; j <= p->length; j++)
      *q++=p->index;
    p++;
  }
 /*
    Convert MIFF to GIF raster pixels.
  */
  WriteGIF(image->file,pixels,image->columns,image->rows,Red,Green,Blue,
    image->colors,image->colors == 2 ? 2 : 0);
  (void) free((char *) pixels);
  if (image->file != stdin)
    if (strcmp(image->filename+strlen(image->filename)-2,".Z") != 0)
      (void) fclose(image->file);
    else
      (void) pclose(image->file);
}

/*****************************************************************
 * Portions of this code Copyright (C) 1989 by Michael Mauldin.
 * Permission is granted to use this file in whole or in part provided
 * that you do not sell it for profit and that this copyright notice
 * and the names of all authors are retained unchanged.
 *
 * Authors:  Michael Mauldin (mlm@cs.cmu.edu)
 *           David Rowley (mgardi@watdcsu.waterloo.edu)
 *
 * Based on: compress.c - File compression ala IEEE Computer,June 1984.
 *
 *  Spencer W. Thomas       (decvax!harpo!utah-cs!utah-gr!thomas)
 *  Jim McKie               (decvax!mcvax!jim)
 *  Steve Davies            (decvax!vax135!petsd!peora!srd)
 *  Ken Turkowski           (decvax!decwrl!turtlevax!ken)
 *  James A. Woods          (decvax!ihnp4!ames!jaw)
 *  Joe Orost               (decvax!vax135!petsd!joe)
 *****************************************************************/

typedef long int        count_int;

static int  Width,Height;
static int  curx,cury;
static long CountDown;
static int  Interlace;
static unsigned char bw[2]={0,0xff};

static void putword(),compress(),output(),cl_block(),cl_hash();
static void char_init(),char_out(),flush_char();

/*************************************************************/
int WriteGIF(fp,pic,w,h,rmap,gmap,bmap,numcols,colorstyle)
FILE *fp;
unsigned char *pic;
int   w,h;
unsigned char *rmap,*gmap,*bmap;
int   numcols,colorstyle;
{
  int RWidth,RHeight;
  int LeftOfs,TopOfs;
  int Resolution,ColorMapSize,InitCodeSize,Background,BitsPerPixel;
  int i,j;


 /* if writing B/W stipple... */
  if (colorstyle==2) {
    rmap=gmap=bmap=bw;
    numcols=2;
  }

  Interlace=0;
  Background=0;

 /* figure out 'BitsPerPixel' */
  for (i=1; i<8; i++)
    if ((1<<i) >= numcols) break;
  
  BitsPerPixel=i;

  ColorMapSize=1 << BitsPerPixel;
  
  RWidth =Width =w;
  RHeight=Height=h;
  LeftOfs=TopOfs=0;
  
  Resolution=BitsPerPixel;

  CountDown=w * h;   /* # of pixels we'll be doing */

  if (BitsPerPixel <= 1) InitCodeSize=2;
                    else InitCodeSize=BitsPerPixel;

  curx=cury=0;

  if (!fp) {
    fprintf(stderr, "WriteGIF: file not open for writing\n");
    return (1);
  }

  fwrite("GIF87a",1,6,fp);   /* the GIF magic number */

  putword(RWidth,fp);          /* screen descriptor */
  putword(RHeight,fp);

  i=0x80;                  /* Yes,there is a color map */
  i |= (8-1)<<4;                /* OR in the color resolution (hardwired 8) */
  i |= (BitsPerPixel - 1);      /* OR in the # of bits per pixel */
  fputc(i,fp);          

  fputc(Background,fp);        /* background color */

  fputc(0,fp);                 /* future expansion unsigned char */


  if (colorstyle == 1) {        /* greyscale */
    for (i=0; i<ColorMapSize; i++) {
      j=(rmap[i]*77+gmap[i]*150+bmap[i]*29) >> 8;
      fputc(j,fp);
      fputc(j,fp);
      fputc(j,fp);
    }
  }
  else {
    for (i=0; i<ColorMapSize; i++) {      /* write out Global colormap */
      fputc(rmap[i],fp);
      fputc(gmap[i],fp);
      fputc(bmap[i],fp);
    }
  }

  fputc(',',fp);             /* image separator */

 /* Write the Image header */
  putword(LeftOfs,fp);
  putword(TopOfs, fp);
  putword(Width,  fp);
  putword(Height, fp);
  if (Interlace) fputc(0x40,fp);  /* Use Global Colormap,maybe Interlace */
            else fputc(0x00,fp);

  fputc(InitCodeSize,fp);
  compress(InitCodeSize+1,fp,pic,w*h);

  fputc(0,fp);                     /* Write out a Zero-length packet (EOF) */
  fputc(';',fp);                   /* Write GIF file terminator */

  return (0);
}

/******************************/
static void putword(w,fp)
int w;
FILE *fp;
{
 /* writes a 16-bit integer in GIF order (LSB first) */
  fputc(w & 0xff,fp);
  fputc((w>>8)&0xff,fp);
}




/***********************************************************************/


static unsigned long cur_accum=0;
static int           cur_bits=0;


#define min(a,b)        ((a>b) ? b : a)

#define BITS  12
#define MSDOS  1

#define HSIZE  5003           /* 80% occupancy */

typedef unsigned char   char_type;


static int n_bits;                  /* number of bits/code */
static int maxbits=BITS;          /* user settable max # bits/code */
static int maxcode;                 /* maximum code,given n_bits */
static int maxmaxcode=1 << BITS;  /* NEVER generate this */

#define MAXCODE(n_bits)     ((1 << (n_bits)) - 1)

static  count_int      htab [HSIZE];
static  unsigned short codetab [HSIZE];
#define HashTabOf(i)   htab[i]
#define CodeTabOf(i)   codetab[i]

static int hsize=HSIZE;           /* for dynamic table sizing */

/*
 * To save much memory,we overlay the table used by compress() with those
 * used by decompress().  The tab_prefix table is the same size and type
 * as the codetab.  The tab_suffix table needs 2**BITS characters.  We
 * get this from the beginning of htab.  The output stack uses the rest
 * of htab,and contains characters.  There is plenty of room for any
 * possible stack (stack used to be 8000 characters).
 */

#define tab_prefixof(i) CodeTabOf(i)
#define tab_suffixof(i)        ((char_type *)(htab))[i]
#define de_stack               ((char_type *)&tab_suffixof(1<<BITS))

static int free_ent=0;                 /* first unused entry */

/*
 * block compression parameters -- after all codes are used up,
 * and compression rate changes,start over.
 */
static int clear_flg=0;

static long int in_count=1;           /* length of input */
static long int out_count=0;          /* # of codes output (for debugging) */

/*
 * compress stdin to stdout
 *
 * Algorithm:  use open addressing double hashing (no chaining) on the 
 * prefix code/g next character combination.  We do a variant of Knuth's
 * algorithm D (vol. 3,sec. 6.4) along with G. Knott's relatively-prime
 * secondary probe.  Here,the modular division first probe is gives way
 * to a faster exclusive-or manipulation.  Also do block compression with
 * an adaptive reset,whereby the code table is cleared when the compression
 * ratio decreases,but after the table fills.  The variable-length output
 * codes are re-sized at this point,and a special CLEAR code is generated
 * for the decompressor.  Late addition:  construct the table according to
 * file size for noticeable speed improvement on small files.  Please direct
 * questions about this implementation to ames!jaw.
 */

static int g_init_bits;
static FILE *g_outfile;

static int ClearCode;
static int EOFCode;


/********************************************************/
static void compress(init_bits,outfile,data,len)
int   init_bits;
FILE *outfile;
unsigned char *data;
int   len;
{
  register long fcode;
  register int i=0;
  register int c;
  register int ent;
  register int disp;
  register int hsize_reg;
  register int hshift;

 /*
   * Set up the globals:  g_init_bits - initial number of bits
   *                      g_outfile   - pointer to output file
   */
  g_init_bits=init_bits;
  g_outfile  =outfile;

 /* initialize 'compress' globals */
  maxbits=BITS;
  maxmaxcode=1<<BITS;
  memset((char *) htab,0,sizeof(htab));
  memset((char *) codetab,0,sizeof(codetab));
  hsize=HSIZE;
  free_ent=0;
  clear_flg=0;
  in_count=1;
  out_count=0;
  cur_accum=0;
  cur_bits=0;

 /*
   * Set up the necessary values
   */
  out_count=0;
  clear_flg=0;
  in_count=1;
  maxcode=MAXCODE(n_bits=g_init_bits);

  ClearCode=(1 << (init_bits - 1));
  EOFCode=ClearCode+1;
  free_ent=ClearCode+2;

  char_init();
  ent=(*data++);  len--;

  hshift=0;
  for (fcode=(long) hsize;  fcode < 65536L; fcode *= 2L)
    hshift++;
  hshift=8 - hshift;               /* set hash code range bound */

  hsize_reg=hsize;
  cl_hash((count_int) hsize_reg);           /* clear hash table */

  output(ClearCode);
    
  while (len) {
    c=(*data++);  len--;
    in_count++;

    fcode=(long) (((long) c << maxbits)+ent);
    i=(((int) c << hshift) ^ ent);   /* xor hashing */

    if (HashTabOf (i) == fcode) {
      ent=CodeTabOf (i);
      continue;
    }

    else if ((long)HashTabOf (i) < 0)     /* empty slot */
      goto nomatch;

    disp=hsize_reg - i;          /* secondary hash (after G. Knott) */
    if (i == 0)
      disp=1;

probe:
    if ((i -= disp) < 0)
      i += hsize_reg;

    if (HashTabOf (i) == fcode) {
      ent=CodeTabOf (i);
      continue;
    }

    if ((long)HashTabOf (i) > 0) 
      goto probe;

nomatch:
    output(ent);
    out_count++;
    ent=c;

    if (free_ent < maxmaxcode) {
      CodeTabOf (i)=free_ent++;/* code -> hashtable */
      HashTabOf (i)=fcode;
    }
    else
      cl_block();
  }

 /* Put out the final code */
  output(ent);
  out_count++;
  output(EOFCode);
}


/*****************************************************************
 * TAG(output)
 *
 * Output the given code.
 * Inputs:
 *      code:   A n_bits-bit integer.  If == -1,then EOF.  This assumes
 *              that n_bits =< (long)wordsize - 1.
 * Outputs:
 *      Outputs code to the file.
 * Assumptions:
 *      Chars are 8 bits long.
 * Algorithm:
 *      Maintain a BITS character long buffer (so that 8 codes will
 * fit in it exactly).  Use the VAX insv instruction to insert each
 * code in turn.  When the buffer fills up empty it and start over.
 */

static
unsigned long masks[]={ 0x0000,0x0001,0x0003,0x0007,0x000F,
                                  0x001F,0x003F,0x007F,0x00FF,
                                  0x01FF,0x03FF,0x07FF,0x0FFF,
                                  0x1FFF,0x3FFF,0x7FFF,0xFFFF };

static void output(code)
int code;
{
  cur_accum &= masks[cur_bits];

  if (cur_bits > 0)
    cur_accum |= ((long)code << cur_bits);
  else
    cur_accum=code;
  
  cur_bits += n_bits;

  while(cur_bits >= 8) {
    char_out((unsigned int) (cur_accum & 0xff));
    cur_accum >>= 8;
    cur_bits -= 8;
  }

 /*
   * If the next entry is going to be too big for the code size,
   * then increase it,if possible.
   */

  if (free_ent > maxcode || clear_flg) {

    if(clear_flg) {
      maxcode=MAXCODE (n_bits=g_init_bits);
      clear_flg=0;
    }
    else {
      n_bits++;
      if (n_bits == maxbits)
  maxcode=maxmaxcode;
      else
  maxcode=MAXCODE(n_bits);
    }
  }
  
  if(code == EOFCode) {
   /* At EOF,write the rest of the buffer */
    while(cur_bits > 0) {
      char_out((unsigned int)(cur_accum & 0xff));
      cur_accum >>= 8;
      cur_bits -= 8;
    }

    flush_char();
  
    fflush(g_outfile);

    if(ferror(g_outfile))
      Error("unable to write GIF file",(char *) NULL);
  }
}


/********************************/
static void cl_block ()            /* table clear for block compress */
{
 /* Clear out the hash table */

  cl_hash ((count_int) hsize);
  free_ent=ClearCode+2;
  clear_flg=1;

  output(ClearCode);
}


/********************************/
static void cl_hash(hsize)         /* reset code table */
register count_int hsize;
{
  register count_int *htab_p=htab+hsize;
  register long i;
  register long m1=(-1);

  i=hsize - 16;
  do {                           /* might use Sys V memset(3) here */
    *(htab_p-16)=m1;
    *(htab_p-15)=m1;
    *(htab_p-14)=m1;
    *(htab_p-13)=m1;
    *(htab_p-12)=m1;
    *(htab_p-11)=m1;
    *(htab_p-10)=m1;
    *(htab_p-9)=m1;
    *(htab_p-8)=m1;
    *(htab_p-7)=m1;
    *(htab_p-6)=m1;
    *(htab_p-5)=m1;
    *(htab_p-4)=m1;
    *(htab_p-3)=m1;
    *(htab_p-2)=m1;
    *(htab_p-1)=m1;
    htab_p -= 16;
  } while ((i -= 16) >= 0);

  for (i += 16; i > 0; i--)
    *--htab_p=m1;
}


/******************************************************************************
 *
 * GIF Specific routines
 *
 ******************************************************************************/

/*
 * Number of characters so far in this 'packet'
 */
static int a_count;

/*
 * Set up the 'unsigned char output' routine
 */
static void char_init()
{
  a_count=0;
}

/*
 * Define the storage for the packet accumulator
 */
static char accum[256];

/*
 * Add a character to the end of the current packet,and if it is 254
 * characters,flush the packet to disk.
 */
static void char_out(c)
int c;
{
  accum[a_count++]=c;
  if(a_count >= 254) 
    flush_char();
}

/*
 * Flush the packet to disk,and reset the accumulator
 */
static void flush_char()
{
  if(a_count > 0) {
    fputc(a_count,g_outfile);
    fwrite(accum,1,a_count,g_outfile);
    a_count=0;
  }
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
  WriteGIFImage(image);
  (void) fprintf(stderr,"%s=> %s  %dx%d\n",argv[1],argv[2],image->columns,
    image->rows);
  return(False);
}
