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
%                     Import GIF image to a MIFF format.                      %
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
%  import [-scene #] image.gif image.miff
%  
%  Specify 'image.gif' as '-' for standard input.  
%  Specify 'image.miff'  as '-' for standard output.
%  
*/

#include "display.h"
#include "image.h"
#include "X.h"
/*
  01/01/91

  Modified to convert GIF to MIFF image format.

  cristy@dupont.com
*/
/* +-------------------------------------------------------------------+ */
/* | Copyright 1990, David Koblas.                                     | */
/* |   Permission to use, copy, modify, and distribute this software   | */
/* |   and its documentation for any purpose and without fee is hereby | */
/* |   granted, provided that the above copyright notice appear in all | */
/* |   copies and that both that copyright notice and this permission  | */
/* |   notice appear in supporting documentation.  This software is    | */
/* |   provided "as is" without express or implied warranty.           | */
/* +-------------------------------------------------------------------+ */

#define  MAXCOLORMAPSIZE    256

#define  TRUE  1
#define  FALSE  0

#define CM_RED    0
#define CM_GREEN  1
#define CM_BLUE    2

#define  MAX_LWZ_BITS    12

#define INTERLACE    0x40
#define LOCALCOLORMAP  0x80
#define BitSet(byte, bit)  (((byte) & (bit)) == (bit))

#define  ReadOK(file,buffer,len)  (fread(buffer, len, 1, file) != 0)

#define LM_to_uint(a,b)      (((b)<<8)|(a))

struct {
  unsigned int  Width;
  unsigned int  Height;
  unsigned char  ColorMap[3][MAXCOLORMAPSIZE];
  unsigned int  BitPixel;
  unsigned int  ColorResolution;
  unsigned int  Background;
  unsigned int  AspectRatio;
} GifScreen;

struct {
  int  transparent;
  int  delayTime;
  int  inputFlag;
  int  disposal;
} Gif89 = { -1, -1, -1, 0 };

char
  *application_name,
  *input_filename,
  *output_filename;

int  scene;
int  verbose;
int  showComment;

static char  usage[] = 
  "[-verbose] [-comments] [-image N] [-scene N] GIFfile MIFFfile";

static void ReadGIF();
static void ReadGIFImage();
static int ReadColorMap();
static int DoExtension();
static int GetDataBlock();
static void Error();

void main(argc,argv)
int  argc;
char  **argv;
{
#define Usage(usage)  \
{  \
  (void) fprintf(stderr,"usage: %s\n",usage);  \
  exit(1);  \
}

  int    argn;
  FILE    *in;
  int    imageNumber;

  application_name=argv[0];

  argn = 1;
  imageNumber = 1;
  scene = 0;
  verbose = FALSE;
  showComment = FALSE;

  while (argn < argc && argv[argn][0] == '-' && argv[argn][1] != '\0') {
    if (strncmp(argv[argn], "-verbose", 2)) {
      verbose = TRUE;
      showComment = TRUE;
    } else if (strncmp(argv[argn], "-comments", 2)) {
      showComment = TRUE;
    } else if (strncmp(argv[argn], "-image", 2)) {
      ++argn;
      if (argn == argc || sscanf(argv[argn], "%d", &imageNumber) != 1)
        Usage(usage);
    } else if (strncmp(argv[argn], "-scene", 2)) {
      ++argn;
      if (argn == argc || sscanf(argv[argn], "%d", &scene) != 1)
        Usage(usage);
    } else {
      Usage(usage);
    }
    ++argn;
  }
  input_filename=argv[argn];
  if (input_filename == (char *) NULL ) 
    Usage(usage);
  if (*input_filename == '-') 
    in=stdin; 
  else 
    in=fopen(input_filename,"r");
  ++argn;
  output_filename=argv[argn];
  if (output_filename == (char *) NULL) 
     Usage(usage);
  ReadGIF(in, imageNumber);
  fclose(in);
  exit(0);
}

static void
ReadGIF(fd, imageNumber)
FILE  *fd;
int  imageNumber;
{
  unsigned char  buf[16];
  unsigned char  c;
  unsigned char  localColorMap[3][MAXCOLORMAPSIZE];
  int    useGlobalColormap;
  int    bitPixel;
  int    imageCount = 0;
  char    version[4];

  if (! ReadOK(fd,buf,6))
    Error("error reading magic number",(char *) NULL);

  if (strncmp(buf,"GIF",3) != 0)
    Error("not a GIF file",(char *) NULL);

  strncpy(version, buf + 3, 3);
  version[3] = '\0';

  if ((strcmp(version, "87a") != 0) && (strcmp(version, "89a") != 0))
    Error("bad version number, not '87a' or '89a'",(char *) NULL);

  if (! ReadOK(fd,buf,7))
    Error("failed to read screen descriptor",(char *) NULL);

  GifScreen.Width           = LM_to_uint(buf[0],buf[1]);
  GifScreen.Height          = LM_to_uint(buf[2],buf[3]);
  GifScreen.BitPixel        = 2<<(buf[4]&0x07);
  GifScreen.ColorResolution = (((buf[4]&0x70)>>3)+1);
  GifScreen.Background      = buf[5];
  GifScreen.AspectRatio     = buf[6];

  if (BitSet(buf[4], LOCALCOLORMAP)) {  /* Global Colormap */
    if (ReadColorMap(fd,GifScreen.BitPixel,GifScreen.ColorMap))
      Error("error reading global colormap",(char *) NULL);
  }

  if (GifScreen.AspectRatio != 0) {
    float  r;
    r = ( (float) GifScreen.AspectRatio + 15.0 ) / 64.0;
  }

  for (;;) {
    if (! ReadOK(fd,&c,1))
      Error("EOF / read error on image data",(char *) NULL);

    if (c == ';') {    /* GIF terminator */
      if (imageCount < imageNumber)
        Error("not enough images in the file",(char *) NULL);
      return;
    }

    if (c == '!') {   /* Extension */
      if (! ReadOK(fd,&c,1))
        Error("OF / read error on extention function code",(char *) NULL);
      DoExtension(fd, c);
      continue;
    }

    if (c != ',') {    /* Not a valid start character */
      continue;
    }

    ++imageCount;

    if (! ReadOK(fd,buf,9))
      Error("couldn't read left/top/width/height",(char *) NULL);

    useGlobalColormap = ! BitSet(buf[8], LOCALCOLORMAP);

    bitPixel = 1<<((buf[8]&0x07)+1);
    if (! useGlobalColormap) {
      if (ReadColorMap(fd, bitPixel, localColorMap))
        Error("error reading local colormap",(char *) NULL);
      ReadGIFImage(fd, LM_to_uint(buf[4],buf[5]),
          LM_to_uint(buf[6],buf[7]), localColorMap,
          BitSet(buf[8], INTERLACE), imageCount != imageNumber);
    } else {
      ReadGIFImage(fd, LM_to_uint(buf[4],buf[5]),
          LM_to_uint(buf[6],buf[7]), GifScreen.ColorMap,
          BitSet(buf[8], INTERLACE), imageCount != imageNumber);
    }

  }
}

static int
ReadColorMap(fd,number,buffer)
FILE    *fd;
int    number;
unsigned char  buffer[3][MAXCOLORMAPSIZE];
{
  int    i;
  unsigned char  rgb[3];

  for (i = 0; i < number; ++i) {
    if (! ReadOK(fd, rgb, sizeof(rgb)))
      Error("bad colormap",(char *) NULL);

    buffer[CM_RED][i] = rgb[0] ;
    buffer[CM_GREEN][i] = rgb[1] ;
    buffer[CM_BLUE][i] = rgb[2] ;
  }
  return FALSE;
}

static int
DoExtension(fd, label)
FILE  *fd;
int  label;
{
  static char  buf[256];
  char    *str;

  switch (label) {
  case 0x01:    /* Plain Text Extension */
    str = "Plain Text Extension";
#ifdef notdef
    if (GetDataBlock(fd, buf) == 0)
      ;

    lpos   = LM_to_uint(buf[0], buf[1]);
    tpos   = LM_to_uint(buf[2], buf[3]);
    width  = LM_to_uint(buf[4], buf[5]);
    height = LM_to_uint(buf[6], buf[7]);
    cellw  = buf[8];
    cellh  = buf[9];
    foreground = buf[10];
    background = buf[11];

    while (GetDataBlock(fd, buf) != 0) {
      PPM_ASSIGN(image[ypos][xpos],
          cmap[CM_RED][v],
          cmap[CM_GREEN][v],
          cmap[CM_BLUE][v]);
      ++index;
    }

    return FALSE;
#else
    break;
#endif
  case 0xff:    /* Application Extension */
    str = "Application Extension";
    break;
  case 0xfe:    /* Comment Extension */
    str = "Comment Extension";
    while (GetDataBlock(fd, buf) != 0) {
      if (showComment)
        printf("gif comment: %s", buf);
    }
    return FALSE;
  case 0xf9:    /* Graphic Control Extension */
    str = "Graphic Control Extension";
    (void) GetDataBlock(fd, buf);
    Gif89.disposal    = (buf[0] >> 2) & 0x7;
    Gif89.inputFlag   = (buf[0] >> 1) & 0x1;
    Gif89.delayTime   = LM_to_uint(buf[1],buf[2]);
    if ((buf[0] & 0x1) != 0)
      Gif89.transparent = buf[3];

    while (GetDataBlock(fd, buf) != 0)
      ;
    return FALSE;
  default:
    str = buf;
    sprintf(buf, "UNKNOWN (0x%02x)", label);
    break;
  }

  while (GetDataBlock(fd, buf) != 0)
    ;

  return FALSE;
}

int  ZeroDataBlock = FALSE;

static int
GetDataBlock(fd, buf)
FILE    *fd;
unsigned char   *buf;
{
  unsigned char  count;

  if (! ReadOK(fd,&count,1)) {
    Warning("error in getting DataBlock size",(char *) NULL);
    return -1;
  }

  ZeroDataBlock = count == 0;

  if ((count != 0) && (! ReadOK(fd, buf, count))) {
    Warning("error in reading DataBlock",(char *) NULL);
    return -1;
  }

  return count;
}

static int
GetCode(fd, code_size, flag)
FILE  *fd;
int  code_size;
int  flag;
{
  static unsigned char  buf[280];
  static int    curbit, lastbit, done, last_byte;
  int      i, j, ret;
  unsigned char    count;

  if (flag) {
    curbit = 0;
    lastbit = 0;
    done = FALSE;
    return 0;
  }

  if ( (curbit+code_size) >= lastbit) {
    if (done) {
      if (curbit >= lastbit)
        Error("ran off the end of my bits",(char *) NULL);
      return -1;
    }
    buf[0] = buf[last_byte-2];
    buf[1] = buf[last_byte-1];

    if ((count = GetDataBlock(fd, &buf[2])) == 0)
      done = TRUE;

    last_byte = 2 + count;
    curbit = (curbit - lastbit) + 16;
    lastbit = (2+count)*8 ;
  }

  ret = 0;
  for (i = curbit, j = 0; j < code_size; ++i, ++j)
    ret |= ((buf[ i / 8 ] & (1 << (i % 8))) != 0) << j;

  curbit += code_size;

  return ret;
}

static int
LWZReadByte(fd, flag, input_code_size)
FILE  *fd;
int  flag;
int  input_code_size;
{
  static int  fresh = FALSE;
  int    code, incode;
  static int  code_size, set_code_size;
  static int  max_code, max_code_size;
  static int  firstcode, oldcode;
  static int  clear_code, end_code;
  static int  table[2][(1<< MAX_LWZ_BITS)];
  static int  stack[(1<<(MAX_LWZ_BITS))*2], *sp;
  register int  i;

  if (flag) {
    set_code_size = input_code_size;
    code_size = set_code_size+1;
    clear_code = 1 << set_code_size ;
    end_code = clear_code + 1;
    max_code_size = 2*clear_code;
    max_code = clear_code+2;

    GetCode(fd, 0, TRUE);
    
    fresh = TRUE;

    for (i = 0; i < clear_code; ++i) {
      table[0][i] = 0;
      table[1][i] = i;
    }
    for (; i < (1<<MAX_LWZ_BITS); ++i)
      table[0][i] = table[1][0] = 0;

    sp = stack;

    return 0;
  } else if (fresh) {
    fresh = FALSE;
    do {
      firstcode = oldcode =
        GetCode(fd, code_size, FALSE);
    } while (firstcode == clear_code);
    return firstcode;
  }

  if (sp > stack)
    return *--sp;

  while ((code = GetCode(fd, code_size, FALSE)) >= 0) {
    if (code == clear_code) {
      for (i = 0; i < clear_code; ++i) {
        table[0][i] = 0;
        table[1][i] = i;
      }
      for (; i < (1<<MAX_LWZ_BITS); ++i)
        table[0][i] = table[1][i] = 0;
      code_size = set_code_size+1;
      max_code_size = 2*clear_code;
      max_code = clear_code+2;
      sp = stack;
      firstcode = oldcode =
          GetCode(fd, code_size, FALSE);
      return firstcode;
    } else if (code == end_code) {
      int    count;
      unsigned char  buf[260];

      if (ZeroDataBlock)
        return -2;

      while ((count = GetDataBlock(fd, buf)) > 0)
        ;

      if (count != 0)
        Warning("missing EOD in data stream (common occurance)",(char *) NULL);
      return -2;
    }

    incode = code;

    if (code >= max_code) {
      *sp++ = firstcode;
      code = oldcode;
    }

    while (code >= clear_code) {
      *sp++ = table[1][code];
      if (code == table[0][code])
        Error("circular table entry BIG ERROR",(char *) NULL);
      code = table[0][code];
    }

    *sp++ = firstcode = table[1][code];

    if ((code = max_code) <(1<<MAX_LWZ_BITS)) {
      table[0][code] = oldcode;
      table[1][code] = firstcode;
      ++max_code;
      if ((max_code >= max_code_size) &&
        (max_code_size < (1<<MAX_LWZ_BITS))) {
        max_code_size *= 2;
        ++code_size;
      }
    }

    oldcode = incode;

    if (sp > stack)
      return *--sp;
  }
  return code;
}

static void
ReadGIFImage(fd, len, height, cmap, interlace, ignore)
FILE  *fd;
int  len, height;
char  cmap[3][MAXCOLORMAPSIZE];
int  interlace, ignore;
{
  unsigned char  c;  
  int    v;
  int    xpos = 0, ypos = 0, pass = 0;
  unsigned char  
    *image,
    *p;

  /*
  **  Initialize the Compression routines
  */
  if (! ReadOK(fd,&c,1))
    Error("EOF / read error on image data",(char *) NULL);

  if (LWZReadByte(fd, TRUE, c) < 0)
    Error("error reading image",(char *) NULL);

  /*
  **  If this is an "uninteresting picture" ignore it.
  */
  if (ignore) {
    if (verbose)
      Warning("skipping image...",(char *) NULL);

    while (LWZReadByte(fd, FALSE, c) >= 0)
      ;
    return;
  }

  if ((image = (unsigned char *) malloc(len* height)) == NULL)
    Error("couldn't alloc space for image",(char *) NULL);
  p=image;
  while ((v = LWZReadByte(fd,FALSE,c)) >= 0 ) {
    image[ypos*len+xpos]=v;

    ++xpos;
    if (xpos == len) {
      xpos = 0;
      if (interlace) {
        switch (pass) {
        case 0:
        case 1:
          ypos += 8; break;
        case 2:
          ypos += 4; break;
        case 3:
          ypos += 2; break;
        }

        if (ypos >= height) {
          ++pass;
          switch (pass) {
          case 1:
            ypos = 4; break;
          case 2:
            ypos = 2; break;
          case 3:
            ypos = 1; break;
          }
        }
      } else {
        ++ypos;
      }
    }
  }

  if (verbose)
    Warning("writing output",(char *) NULL);
  GIFtoMIFF(cmap,image,len,height);
}

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
  Write MIFF format.
*/
GIFtoMIFF(colormap,pixels,width,height)
char  
  colormap[3][MAXCOLORMAPSIZE];

unsigned char
  *pixels;

unsigned int
  width,
  height;
{
  register int
    i;

  Image
    image;

  register RunlengthPacket
    *q;

  unsigned short
    index;

  /*
    Initialize image header.
  */
  if (strcmp(output_filename,"-") == 0)
    (void) strcpy(image.filename,"-");
  else
    (void) strcpy(image.filename,output_filename);
  image.class=PseudoClass;
  image.compression=RunlengthEncodedCompression;
  image.comments=(char *) NULL;
  image.scene=scene;
  image.columns=width;
  image.rows=height;
  image.colors=MAXCOLORMAPSIZE;
  /*
    Allocate memory.
  */
  image.comments=(char *) malloc((unsigned int) (strlen(image.filename)+256));
  image.colormap=(ColorPacket *) malloc(image.colors*sizeof(ColorPacket));
  image.packets=image.columns*image.rows;
  image.pixels=(RunlengthPacket *) 
    malloc(image.packets*sizeof(RunlengthPacket));
  if ((image.comments == (char *) NULL) ||
      (image.colormap == (ColorPacket *) NULL) ||
      (image.pixels == (RunlengthPacket *) NULL))
    {
      (void) fprintf(stderr,"Can't import GIF image, not enough memory.\n");
      exit(1);
    }
  /*
    Initialize comments, colormap, and pixel data.
  */
  (void) sprintf(image.comments,"\n  Imported from GIF image:  %s\n\0",
    input_filename);
  for (i=0; i < image.colors; i++)
  {
    image.colormap[i].red=colormap[CM_RED][i];
    image.colormap[i].green=colormap[CM_GREEN][i];
    image.colormap[i].blue=colormap[CM_BLUE][i];
  }
  q=image.pixels;
  for (i=0; i < (image.columns*image.rows); i++)
  {
    index=pixels[i];
    q->red=image.colormap[index].red;
    q->green=image.colormap[index].green;
    q->blue=image.colormap[index].blue;
    q->index=index;
    q->length=0;
    q++;
  }
  WriteImage(&image);
  (void) free((char *) image.pixels);
  (void) free((char *) image.colormap);
}
