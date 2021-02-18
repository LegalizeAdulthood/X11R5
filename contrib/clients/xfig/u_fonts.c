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

#include <X11/Xlib.h>
#include "u_fonts.h"

#define NULL 0

/* printer font names for indicator window */

struct _xfstruct x_fontinfo[NUM_X_FONTS] = {
    {"-adobe-times-medium-r-*--*", NULL},
    {"-adobe-times-medium-i-*--*", NULL},
    {"-adobe-times-bold-r-*--*", NULL},
    {"-adobe-times-bold-i-*--*", NULL},
    {"-schumacher-clean-medium-r-*--*", NULL},
    {"-schumacher-clean-medium-i-*--*", NULL},
    {"-schumacher-clean-bold-r-*--*", NULL},
    {"-schumacher-clean-bold-i-*--*", NULL},
    {"-adobe-courier-medium-r-*--*", NULL},
    {"-adobe-courier-medium-o-*--*", NULL},
    {"-adobe-courier-bold-r-*--*", NULL},
    {"-adobe-courier-bold-o-*--*", NULL},
    {"-adobe-helvetica-medium-r-*--*", NULL},
    {"-adobe-helvetica-medium-o-*--*", NULL},
    {"-adobe-helvetica-bold-r-*--*", NULL},
    {"-adobe-helvetica-bold-o-*--*", NULL},
    {"-abode-new century schoolbook-medium-r-*--*", NULL},
    {"-adobe-new century schoolbook-medium-i-*--*", NULL},
    {"-adobe-new century schoolbook-bold-r-*--*", NULL},
    {"-adobe-new century schoolbook-bold-i-*--*", NULL},
    {"-*-lucidabright-medium-r-*--*", NULL},
    {"-*-lucidabright-medium-i-*--*", NULL},
    {"-*-lucidabright-demibold-r-*--*", NULL},
    {"-*-lucidabright-demibold-i-*--*", NULL},
    {"*-symbol-medium-r-*--*", NULL},
    {"-*-zapfchancery-medium-i-*--*", NULL},
    {"-*-zapfdingbats-*-*-*--*", NULL},
};

struct _fstruct ps_fontinfo[NUM_PS_FONTS] = {
    {"Times-Roman", 0},
    {"Times-Italic", 1},
    {"Times-Bold", 2},
    {"Times-BoldItalic", 3},
    {"AvantGarde-Book", 4},
    {"AvantGarde-BookOblique", 5},
    {"AvantGarde-Demi", 6},
    {"AvantGarde-DemiOblique", 7},
    {"Bookman-Light", 0},
    {"Bookman-LightItalic", 0},
    {"Bookman-Demi", 0},
    {"Bookman-DemiItalic", 0},
    {"Courier", 8},
    {"Courier-Oblique", 9},
    {"Courier-Bold", 10},
    {"Courier-BoldOblique", 11},
    {"Helvetica", 12},
    {"Helvetica-Oblique", 13},
    {"Helvetica-Bold", 14},
    {"Helvetica-BoldOblique", 15},
    {"Helvetica-Narrow", 0},
    {"Helvetica-Narrow-Oblique", 0},
    {"Helvetica-Narrow-Bold", 0},
    {"Helvetica-Narrow-BoldOblique", 0},
    {"NewCenturySchlbk-Roman", 16},
    {"NewCenturySchlbk-Italic", 17},
    {"NewCenturySchlbk-Bold", 18},
    {"NewCenturySchlbk-BoldItalic", 19},
    {"Palatino-Roman", 20},
    {"Palatino-Italic", 21},
    {"Palatino-Bold", 22},
    {"Palatino-BoldItalic", 23},
    {"Symbol", 24},
    {"ZapfChancery-MediumItalic", 25},
    {"ZapfDingbats", 26},
};

struct _fstruct latex_fontinfo[NUM_LATEX_FONTS] = {
    {"Default", 0},
    {"Roman", 0},
    {"Bold", 2},
    {"Italic", 1},
    {"Modern", 12},
    {"Typewriter", 8},
};

x_fontnum(psflag, fnum)
    int             psflag, fnum;
{
    return (psflag ? ps_fontinfo[fnum].xfontnum :
	    latex_fontinfo[fnum].xfontnum);
}
