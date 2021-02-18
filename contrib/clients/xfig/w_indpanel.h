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

#define	I_ANGLEGEOM	0x00001
#define I_VALIGN	0x00002
#define I_HALIGN	0x00004
#define I_GRIDMODE	0x00008
#define I_POINTPOSN	0x00010
#define I_FILLAREA	0x00020
#define I_BOXRADIUS	0x00040
#define I_LINEWIDTH	0x00080
#define I_LINESTYLE	0x00100
#define I_ARROWMODE	0x00200
#define I_TEXTJUST	0x00400
#define I_FONTSIZE	0x00800
#define I_FONT		0x01000
#define I_TEXTSTEP	0x02000
#define I_ZOOM		0x04000
#define I_ROTNANGLE	0x08000
#define I_NUMSIDES	0x10000

#define I_NONE		0x00000
#define I_ALL		0x1ffff
#define I_MIN1		(I_GRIDMODE | I_ZOOM)
#define I_MIN2		(I_MIN1 | I_POINTPOSN)
#define I_TEXT0		(I_TEXTJUST | I_FONT | I_FONTSIZE | I_TEXTJUST)
#define I_TEXT		(I_MIN2 | I_TEXTSTEP | I_TEXT0)
#define I_LINE0		(I_FILLAREA | I_LINESTYLE | I_LINEWIDTH)
#define I_BOX		(I_MIN2 | I_LINE0)
#define I_ARC		(I_BOX | I_ARROWMODE)
#define I_REGPOLY	(I_BOX | I_NUMSIDES)
#define I_CLOSED	(I_BOX | I_ANGLEGEOM)
#define I_OPEN		(I_CLOSED | I_ARROWMODE)
#define I_ARCBOX	(I_BOX | I_BOXRADIUS)
#define I_OBJECT	(I_MIN1 | I_TEXT0 | I_LINE0 | I_ARROWMODE | I_BOXRADIUS)
#define I_ALIGN		(I_MIN1 | I_HALIGN | I_VALIGN)
#define I_ROTATE	(I_MIN1 | I_ROTNANGLE)

typedef struct choice_struct {
    int             value;
    PIXRECT         icon;
    Pixmap          normalPM;
}               choice_info;
