/*
 * Copyright 1991 University of Wisconsin-Madison
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of Wisconsin-Madison not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  The University of
 * Wisconsin-Madison makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF WISCONSIN-MADISON DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF WISCONSIN-MADISON BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:   Marvin Solomon
 *           Computer Sciences Department
 *           University of Wisconsin, Madison WI
 *           solomon@cs.wisc.edu or seismo!uwvax!solomon
 *
 * Current Maintainer:
 *
 *           Tim Theisen           Systems Programmer
 * Internet: tim@cs.wisc.edu       Department of Computer Sciences
 *     UUCP: uwvax!tim             University of Wisconsin-Madison
 *    Phone: (608)262-0438         1210 West Dayton Street
 *      FAX: (608)262-9777         Madison, WI   53706
 */
#include <stdio.h>
#include "dev.h"
#include "xproof.h"
#ifndef lint
static char *rcsid="$Header: /src/X11/uw/xproof/RCS/getdevinfo.c,v 3.7 1991/10/04 22:05:44 tim Exp $";
#endif

char *malloc();
char *strcat();
char *strcpy();
long lseek();
#ifdef ultrix
void perror();
void bcopy();
void free();
#endif

char *device = DEVICE;

/*
 * Documentation about ditroff font information, gleaned from reading
 * various programs, and comments on how that information is used by this
 * program.
 * 
 * Marvin Solomon (solomon@cs.wisc.edu), Feb. 1987.  Revised Aug, 1988.
 *
 * (See also the routine printface() at the end of this file.)
 * 
 * A device is described by a "DESC" file (normally
 * ${DESCDIR}/dev${device}/DESC.out).  This file starts with a header of
 * 14 shorts ("struct dev" in dev.h) followed by three tables, which
 * describe various common characteristics of all fonts for this device.
 * 
 * The header, which contains such info as the size of the DESC file, the
 * resolution of the device, various dimensions, and the sizes of the
 * following tables, is read by GetDevInfo() into a local variable 'dev',
 * and the important parameters dev.nfonts, dev.nsizes, and dev.nchtab are
 * extracted (and converted from short to int) into the global variables
 * maxmount, nsizes, and nspecial, respectively.  Maxmount is (one less than)
 * the number of type faces that may be simultaneously "mounted" (see below).
 * Nsizes is (one less than) the number of point sizes available in each face.
 * Nspecial is the number of "extra" characters indicated in troff by
 * escapes like \(cc.  (Not all extra characters occur in all faces; this
 * is total number of distinct character names appearing anywhere).
 * GetDevInfo() then allocates space and reads in the next three tables in
 * one gulp.  The information in these tables is extracted into more
 * convenient formats, and the space is freed.
 * 
 * The first table consists of nsizes+1 shorts, indicating the available
 * point sizes on the device.  These values are copied into the array
 * 'sizes' of integers.  Internally, point sizes are usually recorded as
 * an index into sizes[] rather than the "actual" point size.
 * 
 * The second and third tables are used to create a correspondence between the
 * "extra characters" outside the regular ASCII printable set (32-127),
 * which are named in troff source by names like \(cc, and integers in the
 * range [0,nspecial-1].  The third table is an array of dev.lchname
 * characters, which is simply a concatenation of the (null-terminated)
 * names of all extra characters that appear in any font.  This string is
 * copied to malloc'ed storage.  The second table is a list of all the
 * names, coded as (short) offsets from the start of the array.  This
 * information is translated into the array fancy_char[] of pointers into
 * the malloc'ed space.  The result is that fancy_char[] is an array of
 * nspecial character names.  The device thus supports a total of
 * 96+nspecial characters: the 96 printable ASCII characters in the range
 * 32..127, and the nspecial extra characters.
 * 
 * Each "font" (actually, face) with name N (e.g. "R", "CB", etc) is
 * described by a file (normally DESCDIR/devDEV/N.out).  Copies of the
 * dev.nfonts most common of these files are concatenated on to the the end of
 * the DESC.out file.  A face description starts with a data structure of
 * 4 one-byte fields and two 2-char strings (declared as "struct font" in
 * dev.h).  Of these, only two fields are used by this program:  nwfont
 * (number of width entries--actually the number of distinct characters
 * present in the face) and namefont, the (two-character, null-terminated)
 * name.  Following the header are a width table, a code table, and a
 * font-inclusion table.  The width table contains 2*nwfont bytes, and the
 * other two contain nwfont bytes each.  Faces are "loaded" into a
 * specified position in the range 0..maxmount by the 'x f' command in the
 * input file.  Positions 1..maxmount are also loaded from the DESC.out file
 * in GetDevInfo().
 * 
 * The width table is simply a lists of shorts indicating the widths of
 * characters in this face, when the face is displayed in a "standard
 * size" (dev.unitwidth--normally 10, meaning the standard size is 10
 * points).  Widths are assumed to scale proportionally in other point
 * sizes.  This program ignores the width information.
 * 
 * The code table maps characters in this face to "code points" (physical
 * positions in the output device).  This program interprets code points
 * as indices into an X font.
 * 
 * The third table (the "font-inclusion table") maps characters to
 * positions in the width and code tables.  Not all characaters are
 * present in all faces.  For each face, the font-inclusion table maps
 * the  96+nspecial characters to integers in the range 1..nwfont-1, with
 * a value of 0 indicating that the character is not present in the font.
 * The first 96 entries of the font-inclusion table correspond to the
 * ASCII characters 32..127, while the remaining entries correspond to the
 * extra characters fancy_char[0]..fancy_char[nspecial-1].
 * 
 * Identical code in GetDevInfo() and MountFont() translates the code and
 * font-inclusion tables into a more convenient format:  a faceInfo
 * structure f, where f.pos[i]==codetab[fitab[i]] (the code point for
 * character i) if character i is present in the face, and f.pos[i]==-1
 * otherwise.  Each faceInfo structure also contains an array of X Font
 * id's, one for each of the nsizes+1 point sizes.  The X fonts are only
 * opened as they are needed (a value of f.xfont[i]==None indicates that
 * the corresponding X font hasn't been opened yet).
 * 
 * Troff thinks that there is only room for information about maxmount+1
 * faces, but we would rather trade space for time.  When a face is first
 * encountered, a faceInfo structure is allocated and an entry in face[]
 * is made to point to it.  The array face[] thus contains information
 * about all faces ever encountered by the program (up to MAXFACES), while
 * the array mounted[] points to faceInfo structures for those maxmount+1
 * faces that troff thinks are currently mounted.
 */

/* Read in all the device info about a device */
GetDevInfo()
{
	int fi;
	register int i, j, nchars;
	char fname[BUFSIZ], altfname[BUFSIZ];
	struct dev dev;
	char *file_body, *space;
	short *shortp;

	/* Open DESC file */

	(void) sprintf(fname,"%s/dev%s/DESC.out",DESCDIR,device);
	fi = open(fname,0);
	if (fi<0 && strcmp(device, DEVICE)) {
		/* Bad installation.  Try one other thing before giving up. */
		(void) sprintf(altfname,"%s/dev%s/DESC.out",DESCDIR,DEVICE);
		fi = open(altfname,0);
		if (fi<0) {
			perror(fname);
			return;
		}
		device = DEVICE;
	}

	/* Process header */

	if (read(fi, (char *) &dev, sizeof dev) != sizeof dev) {
		(void) strcat(fname, " (reading header)");
		perror(fname);
		return;
	}
	nfaces = maxmount = dev.nfonts;
	nsizes = dev.nsizes;
	nspecial = dev.nchtab;
	nchars = dev.nchtab + 96;
	file_body = malloc((unsigned)dev.filesize);
	if (!file_body) {
		(void)fprintf(stderr,"GetDevInfo: out of space!\n");
		return;
	}
	if (read(fi, (char *)file_body, (int) dev.filesize) != dev.filesize) {
		(void) strcat(fname, " (reading body)");
		perror(fname);
		return;
	}

	/* Point-size table */

	size = ALLOC(int, nsizes+1);
	shortp = (short *)file_body;
	for (i=0;i<=nsizes;i++)
		size[i] = *shortp++;

	/* Characater-mapping table */

	fancy_char = ALLOC(char *, dev.nchtab);
	space = ALLOC(char, dev.lchname);
	for (i=0; i<nspecial; i++)
		fancy_char[i] = &space[*shortp++];
	bcopy((char *)shortp, space, dev.lchname);
	shortp = (short *)((char *)shortp + dev.lchname);

	/* Included face tables */

	for (i=0; i<nfaces; i++) {
		struct faceInfo *f = XtNew(struct faceInfo);
		struct font *hdr = (struct font *)shortp;
		int nwidths = (unsigned char)(hdr->nwfont);
		unsigned char *pos = (unsigned char *)(hdr+1) + 2*nwidths;
		unsigned char *fitab = pos + nwidths;

		mounted[i+1] = face[i] = f;
		(void)strcpy(f->name, hdr->namefont);
		f->pos = ALLOC(int, nchars);
		for (j=0; j<nchars; j++) {
			int k = fitab[j];

			f->pos[j] = k ? pos[k] : -1;
		}
		f->xfont = ALLOC(Font, nsizes+1);
		for (j=0; j<=nsizes; j++)
			f->xfont[j] = None;
		shortp = (short *)(fitab + nchars);
	}
	/* Hack to get around a bug in troff:  Sometimes troff mounts fonts
	 * in position 0.  (This seems to happen when you do \f(xx and 'xx'
	 * is not one of the "standard" fonts;  the intent seems to be to
	 * avoid "unmounting" one of the other fonts.)  Unfortunately,
	 * the 'x font' commmand may be output too early: on the page preceding
	 * the 'f0' command to actually switch to the font.  In this case,
	 * if you skip directly to the page with the 'f0' command you will
	 * miss the call to MountFact and find the wrong thing (or nothing) mounted
	 * on position 0.  The following hack ensures that *something* is mounted
	 * there, even if it isn't the right thing, so the program won't core
	 * dump.  In fact, the chances are good that in this situation,
	 * font 'R' will be there, while the font that "should" be there
	 * is some weird unknown font, for what 'R' is the only reasonable
	 * substitute.
	 *
	 * In fact, the real problem is much worse:  If the document has lots
	 * of '.fp' requests, the "random address" feature of xproof won't
	 * really work right.  The only alternative I can think of is to
	 * scan the whole document for 'x font' commands while looking for
	 * 'p' commands and remember what pages they are on.  That was more
	 * trouble than I was willing to go through (especially since the
	 * 'x font' commands don't even necessarily come at the starts of lines).
	 * Most people only put '.fp' requests at the start of the document
	 * so in practice, this isn't a real problem.
	 */
	mounted[0] = mounted[1];
	free(file_body);
	(void)close(fi);
}

/* "Mount" typeface 'name' on position 'pos'.  If this face has never been
 * seen before, attempt to read in the info from a description file.
 * If the indicated font can't be found, try to find something "similar".
 */
MountFace(pos, name)
int pos;
char *name;
{
	int i, fin, nchars = nspecial+96, nwidths;
	struct faceInfo *f;
	char fname[BUFSIZ];
	unsigned char *codetab, *fitab;
	struct font hdr;

	/* See if this face is already known */
	for (i=0; i<nfaces; i++) {
		if (strcmp(face[i]->name, name) == 0) {
			mounted[pos] = face[i];
			return;
		}
	}
	if (nfaces >= MAXFACES) {
		(void)fprintf(stderr,"Too many type faces (max %d)\n",MAXFACES);
		return;
	}
	(void)sprintf(fname, "%s/dev%s/%s.out", DESCDIR, device, name);
	if ((fin = open(fname, 0)) < 0) {
		/* This face isn't defined for the preview device, although it
		 * was (presumably) defined for the target device.  Try to find
		 * a reasonable substitute.
		 */
		char c = name[strlen(name)-1];
		char alt_fname[BUFSIZ];

		if (c=='B') name = "B";
		else if (c=='I') name = "I";
		else name = "R";
		(void)sprintf(alt_fname, "%s/dev%s/%s.out", DESCDIR, device, name);
		if ((fin = open(alt_fname, 0)) < 0) {
			(void)fprintf(stderr,
				"No description file for typeface '%s'\n",name);
			return;
		}
	}
	if (read(fin, (char *)&hdr, sizeof hdr)!=sizeof hdr) {
		(void)strcat(fname," (header)");
		perror(fname);
		(void) close(fin);
		return;
	}
	f = mounted[pos] = face[nfaces++] = XtNew(struct faceInfo);
	(void)strcpy(f->name, hdr.namefont);

	nwidths = (unsigned char)(hdr.nwfont);
	/* skip width table (not needed) */
	if (lseek(fin, (long)nwidths*sizeof (short), 1) < 0) {
		(void)strcat(fname," (seek)");
		perror(fname);
		(void) close(fin);
		return;
	}

	/* "code" table */
	codetab = ALLOC(unsigned char, nwidths);
	if (read(fin, (char *)codetab, nwidths)!=nwidths) {
		(void)strcat(fname," (code table)");
		perror(fname);
		return;
	}

	/* "font inclusion" table */
	fitab = ALLOC(unsigned char, nchars);
	if (read(fin, (char *)fitab, nchars)!=nchars) {
		(void)strcat(fname," (inclusion table)");
		perror(fname);
		(void) close(fin);
		return;
	}
	f->pos = ALLOC(int, nchars);
	for (i=0; i<nchars; i++) {
		int j = fitab[i];

		f->pos[i] = j ? codetab[j] : -1;
	}
	f->xfont = ALLOC(Font, nsizes+1);
	for (i=0; i<=nsizes; i++)
		f->xfont[i] = None;
	free((char *)codetab);
	free((char *)fitab);
}

#ifdef notdef
/* debugging printout of characteristics of a face */
printface(f)
struct faceInfo *f;
{
	int i;
	int nchars=nspecial+96;

	printf("Face %s\n",f->name);
	for (i=0;i<96;i++)
		printf("%c: %d\n",i+32,f->pos[i]);
	for (i=0;i<nspecial;i++)
		printf("\\(%s: %d\n",fancy_char[i],f->pos[96+i]);
}
#endif notdef
