#ifndef lint
static char rcsid[] = "$Header: /usr/people/sam/tiff/tools/RCS/tiff2ps.c,v 1.11 91/07/16 16:31:51 sam Exp $";
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

#include <math.h>
#include <stdio.h>
#include "tiffio.h"

typedef	unsigned char u_char;
typedef	unsigned short u_short;
typedef unsigned long u_long;

main(argc, argv)
	int argc;
	char *argv[];
{
	int dirnum = -1;
	TIFF *tif;
	char *cp;
	float width = 0.0;
	float height = 0.0;

	for (argc--, argv++; argc > 0 && argv[0][0] == '-'; argc--, argv++) {
		if ('0' <= argv[0][1] && argv[0][1] <= '9') {
			dirnum = atoi(&argv[0][1]);
			continue;
		}
		if (argv[0][1] == 'h' || argv[0][1] == 'H') {
			argc--, argv++;
			height = atof(argv[0]);
			continue;
		}
		if (argv[0][1] == 'w' || argv[0][1] == 'W') {
			argc--, argv++;
			width = atof(argv[0]);
			continue;
		}
		usage(-1);
	}
	if (argc < 1)
		usage(-2);
	tif = TIFFOpen(argv[0], "r");
	if (tif != NULL) {
		if (dirnum != -1 && !TIFFSetDirectory(tif, dirnum))
			exit(-1);
		TIFF2PS(stdout, argv[0], tif, width, height);
	}
	exit(0);
}

usage(code)
	int code;
{
	fprintf(stderr,
	    "Usage: tiff2ps [-width inches] [-height inches] [-dirnum] file\n");
	exit(code);
}

#define MAXLINE		35
#define PS_UNIT_SIZE	72.0
#define	PSUNITS(npix,res)	((npix) * (PS_UNIT_SIZE / (res)))

static	u_short samplesperpixel;
static	u_short bitspersample;
static	u_short tf_bytesperrow;
static	u_short ps_bytesperrow;
static	u_short planarconfiguration;
static	u_short photometricinterpretation;
static	char *hex = "0123456789abcdef";

TIFF2PS(fd, file, tif, width, height)
	FILE* fd;
	char *file;
	TIFF *tif;
	float width;
	float height;
{
	u_long w, h;
	u_short res_tag;
	u_short res_unit;
	float ox, oy, xres, yres;
	long t;
	extern char *ctime();

	tf_bytesperrow = TIFFScanlineSize(tif);
	if (!TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitspersample))
		bitspersample = 1;
	switch (bitspersample) {
	case 1: case 2:
	case 4: case 8:
		break;
	default:
		fprintf(stderr, "%s: Can not image a %d-bit/sample image.\n",
			file, bitspersample);
		return;
	}
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planarconfiguration);
	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometricinterpretation);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
	if (!TIFFGetField(tif, TIFFTAG_XPOSITION, &ox))
		ox = 0;
	if (!TIFFGetField(tif, TIFFTAG_YPOSITION, &oy))
		oy = 0;
	if (!TIFFGetField(tif, TIFFTAG_RESOLUTIONUNIT, &res_unit))
		res_tag = RESUNIT_NONE;
	switch (res_tag) {
	case RESUNIT_CENTIMETER:
		res_unit = 2.54; /* centimeters per inch */
		break;
	case RESUNIT_INCH:
	case RESUNIT_NONE:
	default:
		res_unit = 1.0;
		break;
	}
	if (width == 0.0) {
		if (!TIFFGetField(tif, TIFFTAG_XRESOLUTION, &xres))
			xres = PS_UNIT_SIZE / res_unit;
	} else
		xres = (float) w / width;
	if (height == 0.0) {
		if (!TIFFGetField(tif, TIFFTAG_YRESOLUTION, &yres))
			yres = PS_UNIT_SIZE / res_unit;
	} else
		yres = (float) h / height;

	t = time(0);
	fprintf(fd, "%%!PS-Adobe-2.0 EPSF-2.0\n");
	fprintf(fd, "%%%%Creator: tiff2ps\n");
	fprintf(fd, "%%%%Title: %s\n", file);
	fprintf(fd, "%%%%CreationDate: %s", ctime(&t));
	fprintf(fd, "%%%%Origin: %g %g\n", ox, oy);
	fprintf(fd, "%%%%BoundingBox: 0 0 %f %f\n",
	    (float) PSUNITS(w,xres), (float) PSUNITS(h,yres));
	fprintf(fd, "%%%%EndComments\n");
	fprintf(fd, "10 dict begin\n");

	ps_bytesperrow = tf_bytesperrow;
	switch (photometricinterpretation) {
	case PHOTOMETRIC_RGB:
		if (planarconfiguration == PLANARCONFIG_CONTIG) {
			ps_bytesperrow = (u_short) 3 *
			    (tf_bytesperrow / samplesperpixel);
			fprintf(fd, "/scanLine %d string def\n", ps_bytesperrow);
			fprintf(fd, "%f %f scale\n",
			    (float) PSUNITS(w,xres), (float) PSUNITS(h,yres));
			fprintf(fd, "%lu %lu %d\n", w, h, bitspersample);
			fprintf(fd, "[ %lu 0 0 -%lu 0 %lu ]\n", w, h, h);
			fprintf(fd, "{ currentfile scanLine readhexstring pop } bind\n");
			fprintf(fd, "false 3 colorimage\n");
			PSDataRGBContig(fd, tif, w, h);
		} else { /* PLANARCONFIG_SEPARATE */
			fprintf(fd, "/redScanLine %d string def\n", ps_bytesperrow);
			fprintf(fd, "/greenScanLine %d string def\n", ps_bytesperrow);
			fprintf(fd, "/blueScanLine %d string def\n", ps_bytesperrow);
			fprintf(fd, "%f %f scale\n",
			    (float) PSUNITS(w,xres), (float) PSUNITS(h,yres));
			fprintf(fd, "%lu %lu %d\n", w, h, bitspersample);
			fprintf(fd, "[ %lu 0 0 -%lu 0 %lu] \n", w, h, h);
			fprintf(fd, "{ currentfile redScanLine readhexstring pop } bind\n");
			fprintf(fd, "{ currentfile greenScanLine readhexstring pop } bind\n");
			fprintf(fd, "{ currentfile blueScanLine readhexstring pop } bind\n");
			fprintf(fd, "true 3 colorimage\n");

			PSDataRGBSeparate(fd, tif, w, h);
		}
		break;
	case PHOTOMETRIC_PALETTE:
		fprintf(fd, "/scanLine %d string def\n", ps_bytesperrow);
		fprintf(fd, "%f %f scale\n",
		    (float) PSUNITS(w,xres), (float) PSUNITS(h,yres));
		fprintf(fd, "%lu %lu 8\n", w, h);
		fprintf(fd, "[ %lu 0 0 -%lu 0 %lu ]\n", w, h, h);
		fprintf(fd, "{ currentfile scanLine readhexstring pop } bind\n");
		fprintf(fd, "false 3 colorimage\n");
		PSDataPalette(fd, tif, w, h);
		break;
	case PHOTOMETRIC_MINISBLACK:
	case PHOTOMETRIC_MINISWHITE:
		fprintf(fd, "/scanLine %d string def\n", ps_bytesperrow);
		fprintf(fd, "%f %f scale\n",
		    (float) PSUNITS(w,xres), (float) PSUNITS(h,yres));
		fprintf(fd, "%lu %lu %d\n", w, h, bitspersample);
		fprintf(fd, "[ %lu 0 0 -%lu 0 %lu ]\n", w, h, h);
		fprintf(fd, "{ currentfile scanLine readhexstring pop } bind\n");
		fprintf(fd, "image\n");
		PSDataBW(fd, tif, w, h);
		break;
	default:
		fprintf(stderr,
		    "Can not handle \"PhotometricInterpretation\"=%d\n",
			photometricinterpretation);
		exit(0);
	}
	putc('\n', fd);
	fprintf(fd, "end\n");
	fprintf(fd, "showpage\n");
	fprintf(fd, "%%%%Trailer\n");
}

PSDataRGBContig(fd, tif, w, h)
	FILE* fd;
	TIFF* tif;
	u_long w, h;
{
	u_long row;
	int breaklen = MAXLINE, cc;
	u_char *tf_buf;
	u_char *cp, c;

	tf_buf = (u_char *) malloc(tf_bytesperrow);
	if (tf_buf == NULL) {
		fprintf(stderr, "No space for scanline buffer\n");
		return;
	}
	for (row = 0; row < h; row++) {
		if (TIFFReadScanline(tif, tf_buf, row, 0) < 0)
			break;
		for (cc = 0, cp = tf_buf; cc < tf_bytesperrow; cc += samplesperpixel) {
			if ((breaklen -= 3) <= 0) {
				putc('\n', fd);
				breaklen = MAXLINE;
			}
			putc(hex[*cp >> 4], fd);
			putc(hex[*cp++ & 0xf], fd);
			putc(hex[*cp >> 4], fd);
			putc(hex[*cp++ & 0xf], fd);
			putc(hex[*cp >> 4], fd);
			putc(hex[*cp++ & 0xf], fd);
			if (samplesperpixel == 4)	/* skip the fourth channel */
				cp++;
		}
	}
	free((char *) tf_buf);
}

PSDataRGBSeparate(fd, tif, w, h)
	FILE* fd;
	TIFF* tif;
	u_long w, h;
{
	u_long row;
	int breaklen = MAXLINE, cc, s;
	u_char *tf_buf;
	u_char *cp, c;

	tf_buf = (u_char *) malloc(tf_bytesperrow);
	if (tf_buf == NULL) {
		fprintf(stderr, "No space for scanline buffer\n");
		return;
	}
	for (row = 0; row < h; row++) {
		for (s = 0; s < samplesperpixel; s++) {
			if (s == 3) /* skip the fourth channel */
				continue;
			if (TIFFReadScanline(tif, tf_buf, row, s) < 0)
				break;
			for (cp = tf_buf, cc = 0; cc < tf_bytesperrow; cc++) {
				if (--breaklen == 0) {
					putc('\n', fd);
					breaklen = MAXLINE;
				}
				putc(hex[*cp >> 4], fd);
				putc(hex[*cp++ & 0xf], fd);
			}
		}
	}
	free((char *) tf_buf);
}

PSDataPalette(fd, tif, w, h)
	FILE* fd;
	TIFF* tif;
	u_long w, h;
{
	u_short *rmap, *gmap, *bmap;
	u_long row;
	int breaklen = MAXLINE, cc;
	u_char *tf_buf;
	u_char *cp, c;

	tf_buf = (u_char *) malloc(tf_bytesperrow);
	if (tf_buf == NULL) {
		fprintf(stderr, "No space for scanline buffer\n");
		return;
	}
	TIFFGetField(tif, TIFFTAG_COLORMAP, &rmap, &gmap, &bmap);
	for (row = 0; row < h; row++) {
		if (TIFFReadScanline(tif, tf_buf, row, 0) < 0)
			break;
		for (cp = tf_buf, cc = 0; cc < tf_bytesperrow; cc++) {
			if ((breaklen -= 3) <= 0) {
				putc('\n', fd);
				breaklen = MAXLINE;
			}
			switch (bitspersample) {
			case 8:
				c = *cp++;
				putc(hex[rmap[c] >> 12], fd);
				putc(hex[(rmap[c] >> 8) & 0xf], fd);
				putc(hex[gmap[c] >> 12], fd);
				putc(hex[(gmap[c] >> 8) & 0xf], fd);
				putc(hex[bmap[c] >> 12], fd);
				putc(hex[(bmap[c] >> 8) & 0xf], fd);
				break;
			case 4:
				c = *cp++;
				putc(hex[rmap[c & 0xf] >> 12], fd);
				putc(hex[(rmap[c & 0xf] >> 8) & 0xf], fd);
				putc(hex[gmap[c & 0xf] >> 12], fd);
				putc(hex[(gmap[c & 0xf] >> 8) & 0xf], fd);
				putc(hex[bmap[c & 0xf] >> 12], fd);
				putc(hex[(bmap[c & 0xf] >> 8) & 0xf], fd);
				c >>= 4;
				putc(hex[rmap[c] >> 12], fd);
				putc(hex[(rmap[c] >> 8) & 0xf], fd);
				putc(hex[gmap[c] >> 12], fd);
				putc(hex[(gmap[c] >> 8) & 0xf], fd);
				putc(hex[bmap[c] >> 12], fd);
				putc(hex[(bmap[c] >> 8) & 0xf], fd);
				break;
			case 2:
				c = *cp++;
				putc(hex[rmap[c & 3] >> 12], fd);
				putc(hex[(rmap[c & 3] >> 8) & 0xf], fd);
				putc(hex[gmap[c & 3] >> 12], fd);
				putc(hex[(gmap[c & 3] >> 8) & 0xf], fd);
				putc(hex[bmap[c & 3] >> 12], fd);
				putc(hex[(bmap[c & 3] >> 8) & 0xf], fd);
				c >>= 2;
				putc(hex[rmap[c & 3] >> 12], fd);
				putc(hex[(rmap[c & 3] >> 8) & 0xf], fd);
				putc(hex[gmap[c & 3] >> 12], fd);
				putc(hex[(gmap[c & 3] >> 8) & 0xf], fd);
				putc(hex[bmap[c & 3] >> 12], fd);
				putc(hex[(bmap[c & 3] >> 8) & 0xf], fd);
				c >>= 2;
				putc(hex[rmap[c & 3] >> 12], fd);
				putc(hex[(rmap[c & 3] >> 8) & 0xf], fd);
				putc(hex[gmap[c & 3] >> 12], fd);
				putc(hex[(gmap[c & 3] >> 8) & 0xf], fd);
				putc(hex[bmap[c & 3] >> 12], fd);
				putc(hex[(bmap[c & 3] >> 8) & 0xf], fd);
				c >>= 2;
				putc(hex[rmap[c] >> 12], fd);
				putc(hex[(rmap[c] >> 8) & 0xf], fd);
				putc(hex[gmap[c] >> 12], fd);
				putc(hex[(gmap[c] >> 8) & 0xf], fd);
				putc(hex[bmap[c] >> 12], fd);
				putc(hex[(bmap[c] >> 8) & 0xf], fd);
				break;
			default:
				fprintf(stderr, "depth %d not supported\n",
				    bitspersample);
				exit(0);
			}
		}
	}
	free((char *) tf_buf);
}

PSDataBW(fd, tif, w, h)
	FILE* fd;
	TIFF* tif;
	u_long w, h;
{
	u_long row;
	int breaklen = MAXLINE, cc;
	u_char *tf_buf;
	u_char *cp, c;

	tf_buf = (u_char *) malloc(tf_bytesperrow);
	if (tf_buf == NULL) {
		fprintf(stderr, "No space for scanline buffer\n");
		return;
	}
	for (row = 0; row < h; row++) {
		if (TIFFReadScanline(tif, tf_buf, row, 0) < 0)
			break;
		for (cp = tf_buf, cc = 0; cc < tf_bytesperrow; cc++) {
			if (--breaklen == 0) {
				putc('\n', fd);
				breaklen = MAXLINE;
			}
			if (photometricinterpretation == PHOTOMETRIC_MINISWHITE)
				*cp = ~(*cp);
			putc(hex[*cp >> 4], fd);
			putc(hex[*cp++ & 0xf], fd);
		}
	}
	free((char *) tf_buf);
}
