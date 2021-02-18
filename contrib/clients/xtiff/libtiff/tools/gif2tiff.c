#ifndef lint
static char rcsid[] = "$Header: /usr/people/sam/tiff/tools/RCS/gif2tiff.c,v 1.3 91/08/20 22:26:47 sam Exp $";
#endif

/*
 *	convert a GIF file into a TIFF file.
 *	based on Paul Haeberli's fromgif program which in turn is
 *	based on a GIF file reader by Marcel J.E. Mol March 23 1989 
 *
 *	if input is 320 by 200 pixel aspect is probably 1.2
 *	if input is 640 350 pixel aspect is probably 1.37
 *
 */
#include "tiffio.h"
#include <math.h>
#include <stdio.h>

#define	GIFGAMMA	(1.5)		/* smaller makes output img brighter */
#define	IMAX		((1<<16)-1)	/* max intensity value */
#define EXTRAFUDGE	128		/* some people write BAD .gif files */

short gamtab[256];

makegamtab(gam)
float gam;
{
    int i;

    for(i=0; i<256; i++) 
	gamtab[i] = IMAX*pow(i/255.0,gam)+0.5;
}

short rbuf[8192];
short gbuf[8192];
short bbuf[8192];

#define COLSIZE 256

unsigned char *stackp;
unsigned int prefix[4096];
unsigned char suffix[4096];
unsigned char stack[4096];
int datasize,codesize,codemask;     /* Decoder working variables */
int clear,eoi;                      /* Special code values */
int avail, oldcode;

FILE *infile;
int global;                        /* Is there a global color map? */
int globalbits;                     /* Number of bits of global colors */
unsigned char globalmap[COLSIZE][3];/* RGB values for global color map */
char bgcolor;                       /* background color */
unsigned char *raster;              /* Decoded image data */
unsigned int width, height;
unsigned short red[COLSIZE];
unsigned short green[COLSIZE];
unsigned short blue[COLSIZE];
char *filename, *imagename;

main(argc,argv)
int argc;
char *argv[];
{
    extern int optind;
    extern char *optarg;
    int flag;

    makegamtab(GIFGAMMA);
    if(argc<2) {
	fprintf(stderr,"usage: gif2tif image.gif image.tif\n");
	exit(1);
    }
    filename = argv[1];
    imagename = argv[2];
    if ((infile = fopen(filename,"r")) == NULL) {
	perror(filename);
	exit(1);
    }
    convert();
    fclose(infile);
    exit(0);
}

convert()
{
    char ch;

    if (checksignature())
        return;
    readscreen();
    while ((ch = getc(infile)) != ';' && ch != EOF) {
        switch (ch) {
            case '\0':  break;  /* this kludge for non-standard files */
            case ',':   if (readgifimage())
                           return;
                        break;
            case '!':   readextension();
                        break;
            default:    fprintf(stderr, "illegal GIF block type\n");
                        return;
                        break;
        }
    }
}

checksignature()
{
    char buf[6];

    fread(buf,1,6,infile);
    if (strncmp(buf,"GIF",3)) {
        fprintf(stderr, "file is not a GIF file\n");
        return 1;
    }
    if (strncmp(&buf[3],"87a",3)) {
        fprintf(stderr, "unknown GIF version number\n");
        return 1;
    }
    return 0;
}

/*
 * 	readscreen - 
 *		Get information which is global to all the images stored 
 *	in the file
 */
readscreen()
{
    unsigned char buf[7];
    unsigned int screenwidth;           /* The dimensions of the screen */
    unsigned int screenheight;          /*   (not those of the image)   */
    unsigned int rscreenwidth;          /* The dimensions of the raster */

    fread(buf,1,7,infile);
    screenwidth = buf[0] + (buf[1] << 8);
    rscreenwidth = screenwidth + screenwidth%2; /* compensate odd widths */
    screenheight = buf[2] + (buf[3] << 8);
    global = buf[4] & 0x80;
    if (global) {
        globalbits = (buf[4] & 0x07) + 1;
        fread(globalmap,3,1<<globalbits,infile);
    }
    bgcolor = buf[5];
}

readgifimage()
{
    unsigned char buf[9];
    int local, interleaved;
    char localmap[256][3];
    int localbits;
    unsigned int left,top;
    register row;
    register i;

    if (fread(buf, 1, 9, infile) == 0) {
        perror(filename);
        exit(1);
    }
    left = buf[0] + (buf[1] << 8);
    top = buf[2] + (buf[3] << 8);
    width = buf[4] + (buf[5] << 8);
    height = buf[6] + (buf[7] << 8);
    local = buf[8] & 0x80;
    interleaved = buf[8] & 0x40;

    if (local == 0 && global == 0) {
        fprintf(stderr, "no colormap present for image\n");
        return 1;
    }
    if ((raster = (unsigned char*) malloc(width*height+EXTRAFUDGE)) == NULL) {
        fprintf(stderr, "not enough memory for image\n");
        return 1;
    }
    if (readraster(width, height))
        return 1;
    if (local) {
        localbits = (buf[8] & 0x7) + 1;

        fprintf(stderr, "   local colors: %d\n", 1<<localbits);

        fread(localmap, 3, 1<<localbits, infile);
        initcolors(localmap, 1<<localbits, bgcolor);
    } else if (global) {
        initcolors(globalmap, 1<<globalbits, bgcolor);
    }
    rasterize(interleaved, raster);
    free(raster);
    return 0;
}

/*
 * 	readextension -
 *		Read a GIF extension block (and do nothing with it).
 *
 */
readextension()
{
    unsigned char code;
    int count;
    char buf[255];

    code = getc(infile);
    while (count = getc(infile))
        fread(buf, 1, count, infile);
}

/*
 * 	readraster -
 *		Decode a raster image
 *
 */
readraster(width, height)
unsigned width,height;
{
    unsigned char *fill = raster;
    unsigned char buf[255];
    register bits=0;
    register unsigned datum=0;
    register unsigned char *ch;
    register int count, code;

    datasize = getc(infile);
    clear = 1 << datasize;
    eoi = clear + 1;
    avail = clear + 2;
    oldcode = -1;
    codesize = datasize + 1;
    codemask = (1 << codesize) - 1;
    for (code = 0; code < clear; code++) {
	prefix[code] = 0;
	suffix[code] = code;
    }
    stackp = stack;
    for (count = getc(infile); count > 0; count = getc(infile)) {
	fread(buf,1,count,infile);
	for (ch=buf; count-- > 0; ch++) {
	    datum += *ch << bits;
	    bits += 8;
	    while (bits >= codesize) {
		code = datum & codemask;
		datum >>= codesize;
		bits -= codesize;
		if (code == eoi) {               /* This kludge put in */
#ifdef DEBUG
    fprintf(stderr, "found eoi code\n");
#endif
		    goto exitloop;               /* because some GIF files*/
		}                                /* aren't standard */
		if (process(code, &fill)) {
		    goto exitloop;
		}
	    }
	}
	if (fill >= raster + width*height) {
	    fprintf(stderr, "raster full before eoi code\n");
	    goto exitloop;
	}
    }
exitloop:
    if (fill != raster + width*height)  {
	fprintf(stderr, "warning: wrong rastersize: %ld bytes\n",
						      (long) (fill-raster));
	fprintf(stderr, "         instead of %ld bytes\n",
						      (long) width*height);
	return 0; /* can still draw a picture ... */
    }
    return 0;
}

/*
 * 	process - 
 *		Process a compression code.  "clear" resets the code table.  
 *	Otherwise make a new code table entry, and output the bytes 
 *	associated with the code.
 */
process(code, fill)
register code;
unsigned char **fill;
{
    int incode;
    static unsigned char firstchar;

    if (code == clear) {
	codesize = datasize + 1;
	codemask = (1 << codesize) - 1;
	avail = clear + 2;
	oldcode = -1;
	return 0;
    }

    if (oldcode == -1) {
	*(*fill)++ = suffix[code];
	firstchar = oldcode = code;
	return 0;
    }
    if (code > avail) {
	fprintf(stderr, "code % d to large for %d\n", code, avail);
	return 1; 
    }

    incode = code;
    if (code == avail) {      /* the first code is always < avail */
	*stackp++ = firstchar;
	code = oldcode;
    }
    while (code > clear) {
	*stackp++ = suffix[code];
	code = prefix[code];
    }

    *stackp++ = firstchar = suffix[code];
    prefix[avail] = oldcode;
    suffix[avail] = firstchar;
    avail++;

    if (((avail & codemask) == 0) && (avail < 4096)) {
	codesize++;
	codemask += avail;
    }
    oldcode = incode;
    do {
	*(*fill)++ = *--stackp;
    } while (stackp > stack);
    return 0;
}

/*
 * 	initcolors -
 *		Convert a color map (local or global) to arrays with R, G and B
 * 	values. 
 *
 */
initcolors(colormap, ncolors, bgcolor)
unsigned char colormap[COLSIZE][3];
int ncolors;
int bgcolor;
{
    register i, k;

    for (i = 0; i < ncolors; i++) {
        red[i]   = gamtab[colormap[i][0]];
        green[i] = gamtab[colormap[i][1]];
        blue[i]  = gamtab[colormap[i][2]];
    }
}

rasterize(interleaved, raster)
int interleaved;
register unsigned char *raster;
{
    register row, col;
    register unsigned char *rr;
    unsigned char *newras;
    TIFF *tif;
    
#define DRAWSEGMENT(offset, step)                       \
        for (row = offset; row < height; row += step) { \
            rr = newras + row*width;                   \
            bcopy(raster, rr, width);                   \
            raster += width;                            \
        }

    if ((newras = (unsigned char*) malloc(width*height+EXTRAFUDGE)) == NULL) {
        fprintf(stderr, "not enough memory for image\n");
        return 1;
    }
    rr = newras;
    if (interleaved) {
        DRAWSEGMENT(0, 8);
        DRAWSEGMENT(4, 8);
        DRAWSEGMENT(2, 4);
        DRAWSEGMENT(1, 2);
    }
    else 
        DRAWSEGMENT(0, 1);

    tif = TIFFOpen(imagename, "w");
    if (!tif) {
	TIFFError(imagename,"Can not open output image");
	exit(-1);
    }
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_PALETTE);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_COLORMAP, red, green, blue);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    for(row=0; row<height; row++)
	if (TIFFWriteScanline(tif, newras+row*width, row, 0) < 0)
	    break;
    TIFFClose(tif);

    free(newras);
} 
