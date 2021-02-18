/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 *
 * "Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty."
 *
 */

#define	MIN_P_SIZE 6
#define	MAX_P_SIZE 30

#define DEF_FONTSIZE 12		/* default font size in pts */
#define	PS_FONTPANE_WD 290
#define	LATEX_FONTPANE_WD 112
#define	PS_FONTPANE_HT 20
#define	LATEX_FONTPANE_HT 20
#define	NUM_X_FONTS 27
#define	NUM_PS_FONTS 35
#define NUM_LATEX_FONTS 6

/* element of linked list for each font
   The head of list is for the different font NAMES,
   and the elements of this list are for each different
   point size of that font */

struct xfont {
    int             size;	/* size in points */
    Font            fid;	/* X font id */
    XFontStruct    *fstruct;	/* X font structure */
    char           *fname;	/* actual name of X font found */
    struct xfont   *next;	/* next in the list */
};

struct _fstruct {
    char           *name;	/* Postscript font name */
    int             xfontnum;	/* template for locating X fonts */
};

struct _xfstruct {
    char           *template;	/* template for locating X fonts */
    struct xfont   *xfontlist;	/* linked list of X fonts for different point
				 * sizes */
};

int             x_fontnum();
