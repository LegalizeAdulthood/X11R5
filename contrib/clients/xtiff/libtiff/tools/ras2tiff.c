#ifndef lint
static char rcsid[] = "$Header: /usr/people/sam/tiff/tools/RCS/ras2tiff.c,v 1.6 91/07/16 16:31:46 sam Exp $";
#endif

/*
 * Copyright (c) 1988, 1989, 1990, 1991 Sam Leffler
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

#include <stdio.h>
#include "rasterfile.h"
#include "tiffio.h"

typedef	unsigned char u_char;
typedef	unsigned short u_short;
typedef	unsigned long u_long;

#define	howmany(x, y)	(((x)+((y)-1))/(y))
#define	streq(a,b)	(strcmp(a,b) == 0)

u_short	config = PLANARCONFIG_CONTIG;
u_short	compression = -1;
u_long	rowsperstrip = (u_long) -1;

main(argc, argv)
	char *argv[];
{
	u_char *buf;
	int row, linebytes;
	TIFF *out;
	FILE *in;
	struct rasterfile h;

	argc--, argv++;
	if (argc < 2)
		usage();
	for (; argc > 2 && argv[0][0] == '-'; argc--, argv++) {
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
		if (streq(argv[0], "-rowsperstrip")) {
			argc--, argv++;
			rowsperstrip = atoi(argv[0]);
			continue;
		}
		usage();
	}
	in = fopen(argv[0], "r");
	if (in == NULL) {
		fprintf(stderr, "%s: Can not open.\n", argv[0]);
		exit(-1);
	}
	if (fread(&h, sizeof (h), 1, in) != 1) {
		fprintf(stderr, "%s: Can not read header.\n", argv[0]);
		exit(-2);
	}
	if (h.ras_magic != RAS_MAGIC) {
		fprintf(stderr, "%s: Not a rasterfile.\n", argv[0]);
		exit(-3);
	}
	out = TIFFOpen(argv[1], "w");
	if (out == NULL)
		exit(-4);
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, (u_long) h.ras_width);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, (u_long) h.ras_height);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, h.ras_depth > 8 ? 3 : 1);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, h.ras_depth > 1 ? 8 : 1);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, config);
	if (h.ras_maptype != RMT_NONE) {
		register u_short *red;
		register int i, j;
		int mapsize;

		buf = (u_char *)malloc(h.ras_maplength);
		if (buf == NULL) {
			fprintf(stderr, "No space to read in colormap.\n");
			exit(-5);
		}
		if (fread(buf, h.ras_maplength, 1, in) != 1) {
			fprintf(stderr, "%s: Read error on colormap.\n",
			    argv[0]);
			exit(-6);
		}
		mapsize = 1<<h.ras_depth; 
		if (h.ras_maplength > mapsize*3) {
			fprintf(stderr,
			    "%s: Huh, %d colormap entries, should be %d?\n",
			    argv[0], h.ras_maplength, mapsize*3);
			exit(-7);
		}
		red = (u_short *)malloc(mapsize * 3 * sizeof (u_short));
		if (red == NULL) {
			fprintf(stderr, "No space for colormap.\n");
			exit(-8);
		}
		for (j = 0; j < 3; j++) {
#define	SCALE(x)	(((x)*((1L<<16)-1))/255)
			for (i = h.ras_maplength/3; i-- > 0;)
				*red++ = SCALE(*buf++);
			if ((i = h.ras_maplength/3) < mapsize) {
				i = mapsize - i;
				bzero(red, i*sizeof (u_short));
				red += i;
			}
		}
		TIFFSetField(out, TIFFTAG_COLORMAP,
		     red, red + mapsize, red + 2*mapsize);
		TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_PALETTE);
		if (compression == (u_short)-1)
			compression = COMPRESSION_PACKBITS;
		TIFFSetField(out, TIFFTAG_COMPRESSION, compression);
	} else {
		/* XXX this is bogus... */
		TIFFSetField(out, TIFFTAG_PHOTOMETRIC, h.ras_depth == 24 ?
		    PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK);
		if (compression == (u_short)-1)
			compression = COMPRESSION_LZW;
		TIFFSetField(out, TIFFTAG_COMPRESSION, compression);
	}
	linebytes = ((h.ras_depth*h.ras_width+15) >> 3) &~ 1;
	if (TIFFScanlineSize(out) > linebytes)
		buf = (u_char *)malloc(linebytes);
	else
		buf = (u_char *)malloc(TIFFScanlineSize(out));
	if (rowsperstrip != (u_long)-1)
		rowsperstrip = (8*1024)/linebytes;
	TIFFSetField(out, TIFFTAG_ROWSPERSTRIP,
	    rowsperstrip == 0 ? 1L : rowsperstrip);
	for (row = 0; row < h.ras_height; row++) {
		if (fread(buf, linebytes, 1, in) != 1) {
			fprintf(stderr, "%s: scanline %d: Read error.\n",
			    argv[0], row);
			break;
		}
		if (TIFFWriteScanline(out, buf, row, 0) < 0)
			break;
	}
	(void) TIFFClose(out);
}

usage()
{
	fprintf(stderr, "usage: ras2tif [options] input output\n");
	fprintf(stderr, "where options are:\n");
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
