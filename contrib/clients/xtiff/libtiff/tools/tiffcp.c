#ifndef lint
static char rcsid[] = "$Header: /usr/people/sam/tiff/tools/RCS/tiffcp.c,v 1.13 91/08/14 11:38:45 sam Exp $";
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
#include "tiffio.h"

typedef	unsigned char u_char;
typedef	unsigned short u_short;
typedef	unsigned long u_long;

#define	streq(a,b)	(strcmp(a,b) == 0)
#define	CopyField(tag, v) \
    if (TIFFGetField(in, tag, &v)) TIFFSetField(out, tag, v)
#define	CopyField2(tag, v1, v2) \
    if (TIFFGetField(in, tag, &v1, &v2)) TIFFSetField(out, tag, v1, v2)
#define	CopyField3(tag, v1, v2, v3) \
    if (TIFFGetField(in, tag, &v1, &v2, &v3)) TIFFSetField(out, tag, v1, v2, v3)

static	short config = -1;
static	u_short compression = -1;
static	u_short predictor = 0;
static	u_short fillorder = 0;
static	long rowsperstrip = -1;
static	long g3opts = 0;
static	int ignore = 0;			/* if true, ignore read errors */

static	int tiffcp();
static	int cpContig2Contig(), cpContig2Separate();
static	int cpSeparate2Contig(), cpSeparate2Separate();
static	void usage();

main(argc, argv)
	char *argv[];
{
	TIFF *in, *out;

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
		if (streq(argv[0], "-g3")) {
			compression = COMPRESSION_CCITTFAX3;
			continue;
		}
		if (streq(argv[0], "-g4")) {
			compression = COMPRESSION_CCITTFAX4;
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
		if (streq(argv[0], "-lsb2msb")) {
			fillorder = FILLORDER_LSB2MSB;
			continue;
		}
		if (streq(argv[0], "-msb2lsb")) {
			fillorder = FILLORDER_MSB2LSB;
			continue;
		}
		if (streq(argv[0], "-rowsperstrip")) {
			argc--, argv++;
			rowsperstrip = atoi(argv[0]);
			continue;
		}
		if (streq(argv[0], "-2d")) {
			g3opts = GROUP3OPT_2DENCODING;
			continue;
		}
		if (streq(argv[0], "-fill")) {
			g3opts |= GROUP3OPT_FILLBITS;
			continue;
		}
		if (streq(argv[0], "-predictor")) {
			argc--, argv++;
			predictor = atoi(argv[0]);
			continue;
		}
		if (streq(argv[0], "-ignore")) {
			ignore = 1;
			continue;
		}
		usage();
	}
	in = TIFFOpen(argv[0], "r");
	if (in == NULL)
		exit(-1);
	out = TIFFOpen(argv[1], "w");
	if (out == NULL)
		exit(-2);
	for (;;) {
		if (!tiffcp(in, out))
			goto bad;
		if (!TIFFReadDirectory(in))
			break;
		if (!TIFFWriteDirectory(out))
			goto bad;
	}
	(void) TIFFClose(in);
	(void) TIFFClose(out);
	exit(0);
bad:
	unlink(argv[1]);
	exit(1);
}

static int
tiffcp(in, out)
	TIFF *in, *out;
{
	short bitspersample, samplesperpixel, shortv;
	u_long w, l;
	float floatv;
	char *stringv;
	u_long longv;

	CopyField(TIFFTAG_SUBFILETYPE, longv);
	CopyField(TIFFTAG_IMAGEWIDTH, w);
	CopyField(TIFFTAG_IMAGELENGTH, l);
	CopyField(TIFFTAG_BITSPERSAMPLE, bitspersample);
	if (compression != (u_short)-1)
		TIFFSetField(out, TIFFTAG_COMPRESSION, compression);
	else
		CopyField(TIFFTAG_COMPRESSION, compression);
	if (predictor != 0)
		TIFFSetField(out, TIFFTAG_PREDICTOR, predictor);
	else
		CopyField(TIFFTAG_PREDICTOR, predictor);
	CopyField(TIFFTAG_PHOTOMETRIC, shortv);
	CopyField(TIFFTAG_THRESHHOLDING, shortv);
	if (fillorder != 0)
		TIFFSetField(out, TIFFTAG_FILLORDER, fillorder);
	else
		CopyField(TIFFTAG_FILLORDER, shortv);
	CopyField(TIFFTAG_ORIENTATION, shortv);
	CopyField(TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);
	CopyField(TIFFTAG_MINSAMPLEVALUE, shortv);
	CopyField(TIFFTAG_MAXSAMPLEVALUE, shortv);
	CopyField(TIFFTAG_XRESOLUTION, floatv);
	CopyField(TIFFTAG_YRESOLUTION, floatv);
	if (g3opts != 0)
		TIFFSetField(out, TIFFTAG_GROUP3OPTIONS, g3opts);
	else
		CopyField(TIFFTAG_GROUP3OPTIONS, g3opts);
	CopyField(TIFFTAG_GROUP4OPTIONS, longv);
	CopyField(TIFFTAG_RESOLUTIONUNIT, shortv);
	if (config != -1)
		TIFFSetField(out, TIFFTAG_PLANARCONFIG, config);
	else
		CopyField(TIFFTAG_PLANARCONFIG, config);
	if (rowsperstrip <= 0)
		rowsperstrip = (8*1024)/TIFFScanlineSize(out);
	TIFFSetField(out, TIFFTAG_ROWSPERSTRIP,
	    rowsperstrip == 0 ? 1L : rowsperstrip);
	CopyField(TIFFTAG_XPOSITION, floatv);
	CopyField(TIFFTAG_YPOSITION, floatv);
	CopyField(TIFFTAG_GRAYRESPONSEUNIT, shortv);
	{ u_short *graycurve;
	  CopyField(TIFFTAG_GRAYRESPONSECURVE, graycurve);
	}
	CopyField(TIFFTAG_COLORRESPONSEUNIT, shortv);
	{ u_short *red, *green, *blue;
	  CopyField3(TIFFTAG_COLORRESPONSECURVE, red, green, blue);
	}
	CopyField(TIFFTAG_MATTEING, shortv);
	{ u_short *red, *green, *blue;
	  CopyField3(TIFFTAG_COLORMAP, red, green, blue);
	}
	{ u_short shortv2;
	  CopyField2(TIFFTAG_PAGENUMBER, shortv, shortv2);
	}
	CopyField(TIFFTAG_ARTIST, stringv);
	CopyField(TIFFTAG_IMAGEDESCRIPTION, stringv);
	CopyField(TIFFTAG_MAKE, stringv);
	CopyField(TIFFTAG_MODEL, stringv);
	CopyField(TIFFTAG_SOFTWARE, stringv);
	CopyField(TIFFTAG_DATETIME, stringv);
	CopyField(TIFFTAG_HOSTCOMPUTER, stringv);
	CopyField(TIFFTAG_PAGENAME, stringv);
	CopyField(TIFFTAG_DOCUMENTNAME, stringv);
	(void) TIFFGetField(in, TIFFTAG_PLANARCONFIG, &shortv);
	if (shortv != config && bitspersample != 8 && samplesperpixel > 1) {
		fprintf(stderr,
"Can't handle different planar configuration w/ bits/sample != 8\n");
		return (0);
	}
#define	pack(a,b)	((a)<<8)|(b)
	switch (pack(shortv, config)) {
	case pack(PLANARCONFIG_CONTIG, PLANARCONFIG_CONTIG):
		return (cpContig2Contig(in, out, l));
	case pack(PLANARCONFIG_SEPARATE, PLANARCONFIG_SEPARATE):
		return (cpSeparate2Separate(in, out, l, samplesperpixel));
	case pack(PLANARCONFIG_CONTIG, PLANARCONFIG_SEPARATE):
		return (cpContig2Separate(in, out, l, samplesperpixel, w));
	case pack(PLANARCONFIG_SEPARATE, PLANARCONFIG_CONTIG):
		return (cpSeparate2Contig(in, out, l, samplesperpixel, w));
	default:
		fprintf(stderr,
		    "tiffcp: Unknown planar configuration (in=%d out=%d).\n",
		    shortv, config);
		return (0);
	}
	/*NOTREACHED*/
}

static int
cpContig2Contig(in, out, imagelength)
	TIFF *in, *out;
	u_long imagelength;
{
	u_char *buf = (u_char *)malloc(TIFFScanlineSize(in));
	u_long row;

	for (row = 0; row < imagelength; row++) {
		if (TIFFReadScanline(in, buf, row, 0) < 0 && !ignore)
			goto done;
		if (TIFFWriteScanline(out, buf, row, 0) < 0)
			goto bad;
	}
done:
	free(buf);
	return (1);
bad:
	free(buf);
	return (0);
}

static int
cpSeparate2Separate(in, out, imagelength, samplesperpixel)
	TIFF *in, *out;
	u_long imagelength;
	short samplesperpixel;
{
	u_char *buf = (u_char *)malloc(TIFFScanlineSize(in));
	u_long row;
	int s;

	for (s = 0; s < samplesperpixel; s++) {
		for (row = 0; row < imagelength; row++) {
			if (TIFFReadScanline(in, buf, row, s) < 0 && !ignore)
				goto done;
			if (TIFFWriteScanline(out, buf, row, s) < 0)
				goto bad;
		}
	}
done:
	free(buf);
	return (1);
bad:
	free(buf);
	return (0);
}

static int
cpContig2Separate(in, out, imagelength, samplesperpixel, imagewidth)
	TIFF *in, *out;
	u_long imagelength;
	short samplesperpixel;
	u_long imagewidth;
{
	u_char *inbuf = (u_char *)malloc(TIFFScanlineSize(in));
	u_char *outbuf = (u_char *)malloc(TIFFScanlineSize(out));
	register u_char *inp, *outp;
	register u_long n;
	u_long row;
	int s;

	/* unpack channels */
	for (s = 0; s < samplesperpixel; s++) {
		for (row = 0; row < imagelength; row++) {
			if (TIFFReadScanline(in, inbuf, row, 0) < 0 && !ignore)
				goto done;
			inp = inbuf + s;
			outp = outbuf;
			for (n = imagewidth; n-- > 0;) {
				*outp++ = *inp;
				inp += samplesperpixel;
			}
			if (TIFFWriteScanline(out, outbuf, row, s) < 0)
				goto bad;
		}
	}
done:
	if (inbuf) free(inbuf);
	if (outbuf) free(outbuf);
	return (1);
bad:
	if (inbuf) free(inbuf);
	if (outbuf) free(outbuf);
	return (0);
}

static int
cpSeparate2Contig(in, out, imagelength, samplesperpixel, imagewidth)
	TIFF *in, *out;
	u_long imagelength;
	int samplesperpixel;
	u_long imagewidth;
{
	u_char *inbuf = (u_char *)malloc(TIFFScanlineSize(in));
	u_char *outbuf = (u_char *)malloc(TIFFScanlineSize(out));
	register u_char *inp, *outp;
	register u_long n;
	u_long row;
	int s;

	for (row = 0; row < imagelength; row++) {
		/* merge channels */
		for (s = 0; s < samplesperpixel; s++) {
			if (TIFFReadScanline(in, inbuf, row, s) < 0 && !ignore)
				goto done;
			inp = inbuf;
			outp = outbuf + s;
			for (n = imagewidth; n-- > 0;) {
				*outp = *inp++;
				outp += samplesperpixel;
			}
		}
		if (TIFFWriteScanline(out, outbuf, row, 0) < 0)
			goto bad;
	}
done:
	if (inbuf) free(inbuf);
	if (outbuf) free(outbuf);
	return (1);
bad:
	if (inbuf) free(inbuf);
	if (outbuf) free(outbuf);
	return (0);
}

static void
usage()
{
	fprintf(stderr, "usage: tiffcp [options] input output\n");
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
	    " -g3\t\tcompress output with CCITT Group 3 encoding\n");
	fprintf(stderr,
	    " -g4\t\tcompress output with CCITT Group 4 encoding\n");
	fprintf(stderr,
	    " -none\t\tuse no compression algorithm on output\n");
	fprintf(stderr, "\n");
	fprintf(stderr,
	    " -2d\t\tuse 2d encoding when compressing with Group 3\n");
	fprintf(stderr,
	    " -fill\t\tzero-fill scanlines when compressing with Group 3\n");
	fprintf(stderr, "\n");
	fprintf(stderr,
	    " -rowsperstrip #\tmake each strip have no more than # rows\n");
	fprintf(stderr, " -ignore\tignore read errors\n");
	exit(-1);
}
