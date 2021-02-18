#ifndef lint
static char rcsid[] = "$Header: /usr/people/sam/tiff/tools/RCS/tiffinfo.c,v 1.8 91/07/16 16:31:56 sam Exp $";
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

int	showdata = 0;			/* show data */
int	readdata = 0;			/* read data in file */

usage(code)
	int code;
{
	fprintf(stderr, "Usage: tiffinfo [-cdDjs] [-#] TIFF-files\n");
	fprintf(stderr, "-D    read data\n");
	fprintf(stderr, "-d    show data\n");
	fprintf(stderr, "-c    show color/gray response curves and colormap\n");
	fprintf(stderr, "-s    show data strip offsets and byte counts\n");
	fprintf(stderr, "-j    show JPEG tables\n");
	fprintf(stderr, "-#    show directory number #\n");
	exit(code);
}

main(argc, argv)
	char *argv[];
{
	int dirnum = -1, multiplefiles, c;
	char *cp;
	TIFF *tif;
	extern int optind;
	long flags = 0;

	while ((c = getopt(argc, argv, "cdDjs0123456789")) != -1)
		switch (c) {
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
		case '8': case '9':
			dirnum = atoi(argv[optind]);
			break;
		case 'd':
			showdata++;
			/* fall thru... */
		case 'D':
			readdata++;
			break;
		case 'c':
			flags |= TIFFPRINT_COLORMAP | TIFFPRINT_CURVES;
			break;
		case 's':
			flags |= TIFFPRINT_STRIPS;
			break;
		case 'j':
			flags |= TIFFPRINT_JPEGQTABLES |
				 TIFFPRINT_JPEGACTABLES |
				 TIFFPRINT_JPEGDCTABLES;
			break;
		case '?':
			usage(-1);
			/*NOTREACHED*/
		}
	if (optind >= argc)
		usage(-2);
	multiplefiles = (argc - optind > 1);
	for (; optind < argc; optind++) {
		if (multiplefiles)
			printf("%s:\n", argv[optind]);
		tif = TIFFOpen(argv[optind], "r");
		if (tif != NULL) {
			if (dirnum == -1) {
				do
					TIFFInfo(tif, flags);
				while (TIFFReadDirectory(tif));
			} else {
				if (TIFFSetDirectory(tif, dirnum))
					TIFFInfo(tif, flags);
			}
			TIFFClose(tif);
		}
	}
	exit(0);
}

static
Show(row, sample, pp, scanline)
	int row, sample;
	register u_char *pp;
	int scanline;
{
	register int cc;

	printf("[%3d", row);
	if (sample > 0)
		printf(",%d", sample);
	printf("]");
	for (cc = 0; cc < scanline; cc++) {
		printf(" %02x", *pp++);
		if (((cc+1) % 24) == 0)
			putchar('\n');
	}
	putchar('\n');
}

TIFFInfo(tif, flags)
	TIFF *tif;
	long flags;
{
	u_char *scanbuf;
	u_short config;
	u_long row, h;
	int s, scanline;

	TIFFPrintDirectory(tif, stdout, flags);
	if (!readdata)
		return;
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
	scanbuf = (u_char *)malloc(scanline = TIFFScanlineSize(tif));
	if (scanbuf == NULL) {
		fprintf(stderr, "No space for scanline buffer\n");
		return;
	}
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
	if (config == PLANARCONFIG_CONTIG) {
		for (row = 0; row < h; row++) {
			if (TIFFReadScanline(tif, scanbuf, row, 0) < 0)
				break;
			if (showdata)
				Show(row, -1, scanbuf, scanline);
		}
	} else {
		u_short samplesperpixel;

		TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
		for (s = 0; s < samplesperpixel; s++)
		for (row = 0; row < h; row++) {
			if (TIFFReadScanline(tif, scanbuf, row, s) < 0)
				goto done;
			if (showdata)
				Show(row, s, scanbuf, scanline);
		}
	done:
		;
	}
	free((char *)scanbuf);
}
