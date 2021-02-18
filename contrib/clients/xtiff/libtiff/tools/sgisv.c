#ifndef lint
static char rcsid[] = "$Header: /usr/people/sam/tiff/tools/RCS/sgisv.c,v 1.7 91/07/16 16:31:50 sam Exp $";
#endif

/*
 * Copyright (c) 1990, 1991 Sam Leffler
 * Copyright (c) 1991 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

#include <gl.h>
#include <stdio.h>
#include "tiffio.h"

#define	streq(a,b)	(strcmp(a,b) == 0)

typedef unsigned char u_char;
typedef unsigned long u_long;

u_long	rowsperstrip = (u_long) -1;
int	compression = COMPRESSION_LZW;
int	config = PLANARCONFIG_CONTIG;
int	xmaxscreen;
int	ymaxscreen;
short	photometric = PHOTOMETRIC_RGB;

main(argc, argv)
	int argc;
	char **argv;
{

	argc--, argv++;
	for (; argc > 0 && argv[0][0] == '-'; argc--, argv++) {
		if (streq(argv[0], "-none")) {
			compression = COMPRESSION_NONE;
			continue;
		}
		if (streq(argv[0], "-packbits")) {
			compression = COMPRESSION_PACKBITS;
			continue;
		}
		if (streq(argv[0], "-lzw")) {
			compression = COMPRESSION_LZW;
			continue;
		}
		if (streq(argv[0], "-contig")) {
			config = PLANARCONFIG_CONTIG;
			continue;
		}
		if (streq(argv[0], "-separate")) {
			config = PLANARCONFIG_SEPARATE;
			continue;
		}
		if (streq(argv[0], "-rowsperstrip")) {
			argc--, argv++;
			rowsperstrip = atoi(argv[0]);
			continue;
		}
		usage();
	}
	if (strcmp(argv[argc-1], "-b") == 0) {
		argc--;
		photometric = PHOTOMETRIC_MINISBLACK;
	}
	if (argc != 1 && argc != 5)
		usage();
	xmaxscreen = getgdesc(GD_XPMAX)-1;
	ymaxscreen = getgdesc(GD_YPMAX)-1;
	foreground();
	noport();
	winopen("tiffsv");
	if (argc == 5)
		tiffsv(argv[0],
		    atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
	else
		tiffsv(argv[0], 0, xmaxscreen, 0, ymaxscreen);
}

usage()
{

	fprintf(stderr, "usage: tiffsv [options] outimage [x1 x2 y1 y2] [-b]\n");
	fprintf(stderr, "where options are:\n");
	fprintf(stderr,
	    " -contig\tpack samples contiguously (e.g. RGBRGB...)\n");
	fprintf(stderr,
	    " -separate\tstore samples separately (e.g. RRR...GGG...BBB...)\n");
	fprintf(stderr, "\n");
	fprintf(stderr,
	    " -lzw\t\tcompress output with Lempel-Ziv & Welch encoding\n");
	fprintf(stderr,
	    " -packbits\tcompress output with packbits encoding\n");
	fprintf(stderr,
	    " -none\t\tuse no compression algorithm on output\n");
	fprintf(stderr, "\n");
	fprintf(stderr,
	    " -rowsperstrip #\tmake each strip have no more than # rows\n");
	exit(-1);
}

static
svRGBSeparate(tif, ss, xsize, ysize)
	TIFF *tif;
	register u_long *ss;
	int xsize, ysize;
{
	register int x, y;
	int scanline = TIFFScanlineSize(tif);
	char *rbuf = (char *)malloc(3*scanline);
	char *gbuf = rbuf + scanline;
	char *bbuf = gbuf + scanline;

	for (y = 0; y <= ysize; y++) {
		for (x = 0; x <= xsize; x++) {
			u_long v = ss[x];
			rbuf[x] = v;
			gbuf[x] = v >> 8;
			bbuf[x] = v >> 16;
		}
		if (TIFFWriteScanline(tif, rbuf, y, 0) < 0 ||
		    TIFFWriteScanline(tif, gbuf, y, 1) < 0 ||
		    TIFFWriteScanline(tif, bbuf, y, 2) < 0)
			break;
		ss += xsize+1;
	}
	free(rbuf);
}

static
svRGBContig(tif, ss, xsize, ysize)
	TIFF *tif;
	register u_long *ss;
	int xsize, ysize;
{
	register int x, y;
	char *scanline = (char *)malloc(TIFFScanlineSize(tif));

	for (y = 0; y <= ysize; y++) {
		register char *pp = scanline;

		for (x = 0; x <= xsize; x++) {
			u_long v = ss[x];
			pp[0] = v;
			pp[1] = v >> 8;
			pp[2] = v >> 16;
			pp += 3;
		}
		if (TIFFWriteScanline(tif, scanline, y) < 0)
			break;
		ss += xsize+1;
	}
	free(scanline);
}

#undef RED
#undef GREEN
#undef BLUE
#define	CVT(x)	(((x)*255)/100)
#define	RED	CVT(28)		/* 28% */
#define	GREEN	CVT(59)		/* 59% */
#define	BLUE	CVT(11)		/* 11% */

static
svGrey(tif, ss, xsize, ysize)
	TIFF *tif;
	register u_long *ss;
	int xsize, ysize;
{
	register int x, y;
	char *buf = (char *)malloc(TIFFScanlineSize(tif));

	for (y = 0; y <= ysize; y++) {
		for (x = 0; x <= xsize; x++) {
			u_char *cp = (u_char *)&ss[x];
			buf[x] = (RED*cp[3] + GREEN*cp[2] + BLUE*cp[1]) >> 8;
		}
		if (TIFFWriteScanline(tif, buf, y) < 0)
			break;
		ss += xsize+1;
	}
	free(buf);
}

#define	MIN(a,b)	((a)<(b)?(a):(b))
#define	ABS(x)		((x)<0?-(x):(x))

tiffsv(name, x1, x2, y1, y2)
	char *name;
	int x1, x2, y1, y2;
{
	TIFF *tif;
	int xsize, ysize;
	int xorg, yorg;
	int temp, y, i;
	int pos, togo, n;
	u_long *scrbuf, *ss;

	xorg = MIN(x1,x2);
	yorg = MIN(y1,y2);
	if (xorg<0)
		xorg = 0;
	if (yorg<0)
		yorg = 0;
	xsize = ABS(x2-x1);
	ysize = ABS(y2-y1);
	if (xorg+xsize > xmaxscreen)
		xsize = xmaxscreen-xorg;
	if (yorg+ysize > ymaxscreen)
		ysize = ymaxscreen-yorg;
	tif = TIFFOpen(name, "w");
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, (u_long) (xsize+1));
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, (u_long) (ysize+1));
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL,
	    photometric == PHOTOMETRIC_RGB ? 3 : 1);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, config);
	TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
	TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_BOTLEFT);
	if (rowsperstrip <= 0)
		rowsperstrip = (8*1024)/TIFFScanlineSize(tif);
	TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,
	    rowsperstrip == 0 ? 1L : rowsperstrip);
	scrbuf = (u_long *)malloc((xsize+1)*(ysize+1)*sizeof (u_long));
	readdisplay(xorg, yorg, xorg+xsize, yorg+ysize, scrbuf, RD_FREEZE);
	ss = scrbuf;
	if (photometric == PHOTOMETRIC_RGB) {
		if (config == PLANARCONFIG_SEPARATE)
			svRGBSeparate(tif, scrbuf, xsize, ysize);
		else
			svRGBContig(tif, scrbuf, xsize, ysize);
	} else
		svGrey(tif, scrbuf, xsize, ysize);
	(void) TIFFClose(tif);
	free((char *)scrbuf);
}
