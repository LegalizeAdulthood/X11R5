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
/*
 * Functions for dealing with the X fonts.
 */
#include "xproof.h"
#include <ctype.h>
#include <X11/Xatom.h>

#ifndef lint
static char rcsid[] = "$Header: /src/X11/uw/xproof/RCS/xfontinfo.c,v 3.9 1991/10/04 22:05:44 tim Exp $";
#endif

/* Font families included in the X11R3 distibuted fonts */
/* I really should use an enumeration here, but enums are such a pain! */
#define COURIER		0
#define HELVETICA	1
#define SCHOOLBOOK	2
#define SYMBOL		3
#define TIMES		4
/*#define CHARTER	5 */
#define DITHACKS    5
#define NONE -1
static char *fname[] = {
	"courier",
	"helvetica",
	"new century schoolbook",
	"symbol",
	"times",
	"dithacks",
	/* "charter",  not used */
};
#define FAMILIES (sizeof fname/sizeof *fname)

/* Weights */
enum _weight { MEDIUM, BOLD };
#define MEDIUM	0
#define BOLD	1
static char *wname[] = { "medium", "bold" };
#define WEIGHTS 2

/* Slants */
enum _slant { ROMAN, OBLIQUE, ITALIC };
#define ROMAN	0
#define OBLIQUE	1
#define	ITALIC	2
static char *sname[] = { "r", "o", "i" };
#define SLANTS 3

/* Mapping of troff one- or two-character font name to face/weight/slant
 * combination.  These mappings may need to be updated to cover additional
 * troff font names and/or to take advantage of additional X fonts.
 */
/* Updated, Aug 1989, to include various LaserWriter Plus fonts.
 * the values for the Garamond, Lubalin, Optima, and Souvenir families are
 * (currently) guesses.
 */
static struct _font_map {
	char *troffname;
	int family, weight, slant;
} font_map[] = {
"AG"	/* AvantGarde-DemiOblique */,		HELVETICA, BOLD, OBLIQUE,
"Ag"	/* AvantGarde-Demi */,				HELVETICA, BOLD, ROMAN,
"B"		/* Times-Bold */,					TIMES, BOLD, ROMAN,
"BI"	/* Times-BoldItalic */,				TIMES, BOLD, ITALIC,
"BO"	/* Bookman-DemiItalic */,			SCHOOLBOOK, BOLD, ITALIC,
"Bo"	/* Bookman-Demi */,					SCHOOLBOOK, BOLD, ROMAN,
"C"		/* Courier */,						COURIER, MEDIUM, ROMAN,
"CB"	/* Courier-Bold */,					COURIER, BOLD, ROMAN,
"CD"	/* Courier-BoldOblique */,			COURIER, BOLD, OBLIQUE,	
"CO"	/* Courier-Oblique */,				COURIER, MEDIUM, OBLIQUE,
"GA"	/* Garamond-BoldItalic */,			HELVETICA, BOLD, OBLIQUE,
"Ga"	/* Garamond-Bold */,				HELVETICA, BOLD, ROMAN,
"H"		/* Helvetica */,					HELVETICA, MEDIUM, ROMAN,
"HB"	/* Helvetica-Bold */,				HELVETICA, BOLD, ROMAN,
"HD"	/* Helvetica-BoldOblique */,		HELVETICA, BOLD, OBLIQUE,
"HN"	/* Helvetica-Narrow-BoldOblique */,	HELVETICA, BOLD, OBLIQUE,
"HO"	/* Helvetica-Oblique */,			HELVETICA, MEDIUM, OBLIQUE,
"Hn"	/* Helvetica-Narrow-Bold */,		HELVETICA, BOLD, ROMAN,
"I"		/* Times-Italic */,					TIMES, MEDIUM, ITALIC,
"LU"	/* LubalinGraph-DemiOblique */,		HELVETICA, BOLD, OBLIQUE,
"Lu"	/* LubalinGraph-Demi */,			HELVETICA, BOLD, ROMAN,
"NC"	/* NewCenturySchlbk-BoldItalic */,	SCHOOLBOOK, BOLD, ITALIC,
"Nc"	/* NewCenturySchlbk-Bold */,		SCHOOLBOOK, BOLD, ROMAN,
"OP"	/* Optima-BoldOblique */,			HELVETICA, BOLD, OBLIQUE,
"Op"	/* Optima-Bold */,					HELVETICA, BOLD, ROMAN,
"PA"	/* Palatino-Roman */,				TIMES, MEDIUM, ROMAN,
"PB"	/* Palatino-Bold */,				TIMES, BOLD, ROMAN,
"PI"	/* Palatino-Italic */,				TIMES, MEDIUM, ITALIC,
"PX"	/* Palatino-BoldItalic */,			TIMES, BOLD, ITALIC,
"R"		/* Times-Roman */,					TIMES, MEDIUM, ROMAN,
"S"		/* Symbol */,						SYMBOL, MEDIUM, ROMAN,
"SS"	/* DIThacks */,						DITHACKS, MEDIUM, ROMAN,
"SV"	/* Souvenir-DemiItalic */,			HELVETICA, BOLD, OBLIQUE,
"Sv"	/* Souvenir-Demi */,				HELVETICA, BOLD, ROMAN,
"TB"	/* Times-Bold */,					TIMES, BOLD, ROMAN,
"TD"	/* Times-BoldItalic */,				TIMES, BOLD, ITALIC,
"TI"	/* Times-Italic */,					TIMES, MEDIUM, ITALIC,
"TR"	/* Times-Roman */,					TIMES, MEDIUM, ROMAN,
"ZC"	/* ZapfChancery-MediumItalic */,	TIMES, MEDIUM, ITALIC,
"aG"	/* AvantGarde-BookOblique */,		HELVETICA, MEDIUM, OBLIQUE,
"ag"	/* AvantGarde-Book */,				HELVETICA, MEDIUM, ROMAN,
"bO"	/* Bookman-LightItalic */,			SCHOOLBOOK, MEDIUM, ITALIC,
"bo"	/* Bookman-Light */,				SCHOOLBOOK, MEDIUM, ROMAN,
"gA"	/* Garamond-LightItalic */,			HELVETICA, MEDIUM, ITALIC,
"ga"	/* Garamond-Light */,				HELVETICA, MEDIUM, ROMAN,
"hN"	/* Helvetica-Narrow-Oblique */,		HELVETICA, MEDIUM, OBLIQUE,
"hn"	/* Helvetica-Narrow */,				HELVETICA, MEDIUM, ROMAN,
"lU"	/* LubalinGraph-BookOblique */,		HELVETICA, MEDIUM, ITALIC,
"lu"	/* LubalinGraph-Book */,			HELVETICA, MEDIUM, ROMAN,
"nC"	/* NewCenturySchlbk-Italic */,		SCHOOLBOOK, MEDIUM, ITALIC,
"nc"	/* NewCenturySchlbk-Roman */,		SCHOOLBOOK, MEDIUM, ROMAN,
"oP"	/* Optima-Oblique */,				HELVETICA, MEDIUM, ITALIC,
"op"	/* Optima */,						HELVETICA, MEDIUM, ROMAN,
"sV"	/* Souvenir-LightItalic */,			HELVETICA, MEDIUM, ITALIC,
"sv"	/* Souvenir-Light */,				HELVETICA, MEDIUM, ROMAN,
0, NONE, NONE, NONE
};

/* List of available point sizes for each X font face */

/*
 * NB:  We really should query the server to get this information, but
 * it's much too slow (on a VAXstation 3200 qdss 4-plane, the following code
 * chews up about 20 seconds of cpu time in the server). Therefore, we
 * #define SLOW, run this program once, collect this output, and hand-edit
 * to create the following compiled-in tables.  Grumble!
 */

static int allsizes[] = { 8, 10, 12, 14, 18, 24, 0 };
static int nosizes[] = { 0 };
static int *sizes[FAMILIES][WEIGHTS][SLANTS] = {
	/* courier medium r */					allsizes,
	/* courier medium o */					allsizes,
	/* courier medium i */					nosizes,
	/* courier bold r */					allsizes,
	/* courier bold o */					allsizes,
	/* courier bold i */					nosizes,
	/* helvetica medium r */				allsizes,
	/* helvetica medium o */				allsizes,
	/* helvetica medium i */				nosizes,
	/* helvetica bold r */					allsizes,
	/* helvetica bold o */					allsizes,
	/* helvetica bold i */					nosizes,
	/* new century schoolbook medium r */	allsizes,
	/* new century schoolbook medium o */	nosizes,
	/* new century schoolbook medium i */	allsizes,
	/* new century schoolbook bold r */		allsizes,
	/* new century schoolbook bold o */		nosizes,
	/* new century schoolbook bold i */		allsizes,
	/* symbol medium r */					allsizes,
	/* symbol medium o */					nosizes,
	/* symbol medium i */					nosizes,
	/* symbol bold r */						nosizes,
	/* symbol bold o */						nosizes,
	/* symbol bold i */						nosizes,
	/* times medium r */					allsizes,
	/* times medium o */					nosizes,
	/* times medium i */					allsizes,
	/* times bold r */						allsizes,
	/* times bold o */						nosizes,
	/* times bold i */						allsizes,
	/* dithacks medium r */					allsizes,
	/* dithacks medium o */					nosizes,
	/* dithacks medium i */					nosizes,
	/* dithacks bold r */					nosizes,
	/* dithacks bold o */					nosizes,
	/* dithacks bold i */					nosizes,
};

static struct _loaded_fonts {
	int family, weight, slant, size;
	Font font_id;
} loaded_fonts[MAXFACES];
static int num_fonts;

/* Find out what fonts the X server has */
#ifdef SLOW
GetXfontInfo(widget)
ProofWidget widget;
{
	int f, w, s, count, i;
	char **names;
	XFontStruct *info;
	char pattern[1000];

	for (f=0; f<FAMILIES; f++) {
		for (w=0; w<WEIGHTS; w++) {
			for (s=0; s<SLANTS; s++) {
				(void)sprintf(pattern,
					"-adobe-%s-%s-%s-*--*-*-*-*-*-*-adobe-fontspecific",
					fname[f],wname[w],sname[s]);
				names = XListFontsWithInfo(
							XtDisplay((Widget)widget),
							pattern, 1000, &count, &info
						);

				sizes[f][w][s] =
					(int *)malloc((unsigned)(count+1)*sizeof (int *));
				bzero((char *)(sizes[f][w][s]), (count+1)*sizeof (int *));
				for (i=0; i<count; i++) {
					unsigned int points;
					if (!XGetFontProperty(&(info[i]),XA_POINT_SIZE,&points)) {
						printf("no point-size property for font %s!\n",
							names[i]);
						continue;
					}
					sizes[f][w][s][i] = (points+5)/10;
				}
				if (count>0) {
					XFreeFontInfo(names,info,count);
					/* If we were really going to use this info, we would
					 * have to sort sizes[f][w][s][0..count-1] into increasing
					 * order.
					 */
				}
			}
		}
	}
	printf("Fonts Available:\n");
	for (f=0; f<FAMILIES; f++) {
		for (w=0; w<WEIGHTS; w++) {
			for (s=0; s<SLANTS; s++) {
				printf("%s %s %s:", fname[f],wname[w],sname[s]);
				for (i=0; sizes[f][w][s][i]; i++)
					printf(" %d",sizes[f][w][s][i]);
				printf("\n");
			}
		}
	}
}
#else SLOW
/*ARGSUSED*/
GetXfontInfo(widget)
ProofWidget widget;
{}
#endif

static void BadFontMessage(name,message)
char *name, *message;
{
#	define MAXBADFONTS 100
	static char badfont[MAXBADFONTS][3];
	static nbadfonts = 0;
	int i;

	/* check to see if we've already printed a message about this one */
	for (i=0;i<nbadfonts;i++)
		if (strcmp(badfont[i],name)==0) return;
	if (nbadfonts >= MAXBADFONTS) return;
	(void)fprintf(stderr,"font %s: %s\n",name,message);
	(void)strcpy(badfont[nbadfonts++],name);
}

/* Find an available X font that is "close" to the face and size desired.
 * This procedure assumes that GetXFontInfo has been previously called
 * to set up sizes[].
 */
Font LoadXfont(widget, troffname, pointsize)
ProofWidget widget;
char *troffname;
int pointsize;
{
	char fontname[1000];
	struct _font_map *p;
	struct _loaded_fonts *q;
	int i;
	int *sizep;
	XFontStruct *result;

	/* Translate troff name to X family/weight/slant */
	for (p=font_map; p->troffname; p++)
		if (strcmp(p->troffname,troffname)==0)
			break;
	if (p->troffname==0) {
		BadFontMessage(troffname,"unknown");
		/* try to find an acceptable replacement (this is a hack) */
		if (troffname[1]==0) {
			/* one-character name */
			switch (troffname[0]) {
				case 'b': troffname = "B"; break;
				case 'i': troffname = "B"; break;
				default: troffname = "R"; break;
			}
		}
		else if (isalpha(troffname[0]) && isalpha(troffname[0])) {
			if (isupper(troffname[0])) {
				if (isupper(troffname[1])) troffname = "BI";
				else troffname = "B";
			}
			else {
				if (isupper(troffname[1])) troffname = "I";
				else troffname = "R";
			}
		}
		else troffname = "R";
		for (p=font_map; p->troffname; p++)
			if (strcmp(p->troffname,troffname)==0)
				break;
		if (p->troffname==0) return None;
	}
	if (p->family == NONE) {
		/* known but not supported */
		BadFontMessage(troffname,"not supported");
		return None;
	}
	/* Now look for an appropriate pointsize.
	 * First choice is smallest size >= 'pointsize'.  Otherwise, largest size
	 * <= 'pointsize'.
	 */
	sizep = sizes[p->family][p->weight][p->slant];
	while (*sizep && *sizep < pointsize) sizep++;
	if (*sizep == 0) {
		sizep--;
		while (sizep > sizes[p->family][p->weight][p->slant]
			&& *sizep > pointsize)
				sizep--;
	}
	if (*sizep == 0) {
		(void)fprintf(stderr,"No sizes for %s %s %s!\n",
				fname[p->family],
				wname[p->weight],
				sname[p->slant]);
		return None;
	}

	/*
	 * check to see if the font is already loaded, some servers
	 * can take a very long time to match the wild carded string.
	 */
	for (q=loaded_fonts, i=0; i<num_fonts; q++,i++)
		if ((q->family == p->family) && (q->weight == p->weight) &&
		    (q->slant == p->slant) && (q->size == *sizep))
			return (q->font_id);

	/* generate a name for the font */
	(void)sprintf(fontname, 
					"-adobe-%s-%s-%s-*--*-%d0-%d-%d-*-*-adobe-fontspecific",
					fname[p->family],
					wname[p->weight],
					sname[p->slant],
					*sizep, screen_res, screen_res);
	/* open it */
	/* NB:  We use XLoadQueryFont and throw away all but the Font id
	 * rather than XLoadFont, because the former returns an error indication
	 * whereas the latter raises an error, and I don't want to bother
	 * setting up a handler just to catch it.
	 */
	result = XLoadQueryFont(XtDisplay((Widget)widget), fontname);
	if (!result) {
		(void)fprintf(stderr,"cannot open %s\n",fontname);
		return None;
	}
	if (num_fonts == MAXFACES) { /* If font_id cache overflows, flush it. */
		(void)fprintf(stderr,"Warning: font_id cache flushed.\n");
		num_fonts = 0;
		q=loaded_fonts;
	}
	q->family = p->family;
	q->weight = p->weight;
	q->slant = p->slant;
	q->size = *sizep;
	q->font_id = result->fid;
	num_fonts++;
	return q->font_id;
}

/* Clear all allocated X fonts.  This procedure is called whenever the
 * magnification is changed, since the mapping from size number to X point
 * size changes.  Fonts are not unloaded here anymore.  It is better to
 * keep them loaded.  They are kept track of in the loaded_fonts array.
 */
void UnloadXFonts(widget)
ProofWidget widget;
{
	int i, j;
	for (i=0; i<nfaces; i++) {
		for (j=0; j<=nsizes; j++) {
			face[i]->xfont[j] = None;
		}
	}
}

