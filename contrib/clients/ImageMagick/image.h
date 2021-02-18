/*
  Image define declarations.
*/
#define Intensity(color)  \
  (((color).red*77+(color).green*150+(color).blue*29) >> 8)
#define MaxColormapSize  65535
#define MaxImageSize  (4096*4096)
#define MaxRgb  255
#define MaxRunlength  255
#ifdef vms
#define pclose(file)  exit(1)
#define popen(command,mode)  exit(1)
#endif

/*
  Image Id's
*/
#define UnknownId  0
#define ImageMagickId  1
/*
  Image classes:
*/
#define UnknownClass  0
#define DirectClass  1
#define PseudoClass  2
/*
  Image compression algorithms:
*/
#define UnknownCompression  0
#define NoCompression  1
#define RunlengthEncodedCompression  2
#define QEncodedCompression  3

/*
  Typedef declarations for the Display program.
*/
typedef struct _ColorPacket
{
  unsigned char
    red,
    green,
    blue;

  unsigned short
    index;
} ColorPacket;

typedef struct _RunlengthPacket
{
  unsigned char
    red,
    green,
    blue,
    length;

  unsigned short
    index;
} RunlengthPacket;

typedef struct _Image
{
  FILE
    *file;

  char
    filename[256];

  char
    *comments;

  unsigned int
    id,
    class,
    compression,
    columns,
    rows;

  unsigned int
    colors;

  ColorPacket
    *colormap;

  unsigned int
    packets,
    runlength;

  RunlengthPacket
    *pixels;

  unsigned int
    scene;

  struct _Image
    *next;
} Image;

/*
  Image utilities routines.
*/
extern Image
  *AllocateImage(),
  *CopyImage(),
  *ClipImage(),
  *EnhanceImage(),
  *NoisyImage(),
  *ReadImage(),
  *ReduceImage(),
  *ReflectImage(),
  *RotateImage(),
  *ScaleImage(),
  *StereoImage(),
  *TransformImage(),
  *ZoomImage();

extern unsigned int
  NumberColors(),
  PrintImage(),
  ReadData(),
  UncompressImage(),
  WriteImage();

extern void
  CompressImage(),
  DestroyImage(),
  GammaImage(),
  GrayImage(),
  InverseImage(),
  NormalizeImage(),
  QuantizationError(),
  QuantizeImage();
