/*

Copyright 1991 by Fuji Xerox Co., Ltd., Tokyo, Japan.

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of M.I.T. not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission.  Fuji Xerox Co., Ltd. makes no representations
about the suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.

FUJI XEROX CO.,LTD. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJI XEROX CO.,LTD. BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

*/

#include "XIMServer.h"
#include <X11/Xlocale.h>
#include <X11/Xresource.h>
#include <stdio.h>

#define DEFAULT_FONT	"-*-*-*-R-Normal--*-130-75-75-*-*"

static XrmOptionDescRec opTable[] = {
{"+rv",		"*reverseVideo", XrmoptionNoArg,	(void *) "off"},
{"+synchronous","*synchronous",	XrmoptionNoArg,		(void *) "off"},
{"-background",	"*background",	XrmoptionSepArg,	(void *) NULL},
{"-bd",		"*borderColor",	XrmoptionSepArg,	(void *) NULL},
{"-bg",		"*background",	XrmoptionSepArg,	(void *) NULL},
{"-bordercolor","*borderColor",	XrmoptionSepArg,	(void *) NULL},
{"-borderwidth",".borderWidth",	XrmoptionSepArg,	(void *) NULL},
{"-bw",		".borderWidth",	XrmoptionSepArg,	(void *) NULL},
{"-display",	".display",     XrmoptionSepArg,	(void *) NULL},
{"-fg",		"*foreground",	XrmoptionSepArg,	(void *) NULL},
{"-fn",		"*font",	XrmoptionSepArg,	(void *) NULL},
{"-font",	"*font",	XrmoptionSepArg,	(void *) NULL},
{"-foreground",	"*foreground",	XrmoptionSepArg,	(void *) NULL},
{"-geometry",	".geometry",	XrmoptionSepArg,	(void *) NULL},
{"-iconic",	".iconic",	XrmoptionNoArg,		(void *) "on"},
{"-name",	".name",	XrmoptionSepArg,	(void *) NULL},
{"-reverse",	"*reverseVideo", XrmoptionNoArg,	(void *) "on"},
{"-rv",		"*reverseVideo", XrmoptionNoArg,	(void *) "on"},
{"-selectionTimeout",
		".selectionTimeout", XrmoptionSepArg,	(void *) NULL},
{"-synchronous","*synchronous",	XrmoptionNoArg,		(void *) "on"},
{"-title",	".title",	XrmoptionSepArg,	(void *) NULL},
{"-xnllanguage",".xnlLanguage",	XrmoptionSepArg,	(void *) NULL},
{"-xrm",	NULL,		XrmoptionResArg,	(void *) NULL},
};

#define SizeOfArray(array) (sizeof(array) / sizeof(array[0]))
#define DEFAULT_DISTANCE (4)
#define DEFAULT_CHAR_SIZE (40)

static char *name = "ximserver";
static char *vendor = "fujixerox";
static char *Name = "Kmf(Sample IM Server)";

extern Ximp_KeyList *IMStartKeyList;

static
nullfunc() {}

main(argc, argv)
int argc;
char **argv;
{
    Display *display;
    XrmDatabase db = (XrmDatabase)NULL;
    Window shell, preedit, status;
    XFontSet font;
    XFontSetExtents *font_ext;
    int borderWidth;
    XSizeHints size_hint;
    unsigned long fore, back, border;

    XrmValue value;
    char *display_name = "";
    char *locale = setlocale(LC_ALL, "");
    char *font_name = DEFAULT_FONT;
    char **missing_list;
    int missing_count;
    char *def_string;
    char *geometry;
    char *type;
    char *tmp;

    XrmInitialize();
    XrmParseCommand(&db, opTable, SizeOfArray(opTable), name, &argc, argv);
    if (XrmGetResource(db, "ximserver.display", "XIMServer.Display", &type, &value))
		display_name = value.addr;
    if ((display = XOpenDisplay(display_name)) == (Display *)NULL) {
	fprintf(stderr, "%s: Cannot open display %s.\n", argv[0], display_name);
	exit(1);
    }
    XGetDefault(display, name, "font"); /* Dummy for InitDefaults */
    XrmMergeDatabases(XrmGetDatabase(display), &db);
    XrmSetDatabase(display, db);

    if (tmp = XGetDefault(display, name, "font")) font_name = tmp;
    font = XCreateFontSet(display, font_name,
		&missing_list, &missing_count, &def_string);
    font_ext = XExtentsOfFontSet(font);

    if (!font) {
	fprintf(stderr, "%s: Cannot open font %s.\n", argv[0], font_name);
	exit(1);
    }
    borderWidth = 1;
    if (tmp = XGetDefault(display, name, "borderWidth")) borderWidth = atol(tmp);

    {
	Colormap cmap = DefaultColormap(display, DefaultScreen(display));
	XColor color;

	tmp = XGetDefault(display, name, "foreground");
	if (tmp &&
	    XParseColor(display, cmap, tmp, &color) &&
	    XAllocColor(display, cmap, &color)) {
	    fore = color.pixel;
	} else {
	    fore = BlackPixel(display, DefaultScreen(display));
	}

	tmp = XGetDefault(display, name, "background");
	if (tmp &&
	    XParseColor(display, cmap, tmp, &color) &&
	    XAllocColor(display, cmap, &color)) {
	    back = color.pixel;
	} else {
	    back = WhitePixel(display, DefaultScreen(display));
	}

	tmp = XGetDefault(display, name, "borderColor");
	if (tmp &&
	    XParseColor(display, cmap, tmp, &color) &&
	    XAllocColor(display, cmap, &color)) {
	    border = color.pixel;
	} else {
	    border = BlackPixel(display, DefaultScreen(display));
	}
    }

    size_hint.min_width = 2 * (DEFAULT_DISTANCE + borderWidth);
    size_hint.width = font_ext->max_logical_extent.width * DEFAULT_CHAR_SIZE + size_hint.min_width;
    size_hint.min_height = 3 * DEFAULT_DISTANCE + 4 * borderWidth;
    size_hint.height = font_ext->max_logical_extent.height * 2 + size_hint.min_height;
    size_hint.flags = PSize;
    if (geometry = XGetDefault(display, name, "geometry")) {
	int result = XParseGeometry(geometry, &size_hint.x, &size_hint.y, &size_hint.width, &size_hint.height);
	if ((result & WidthValue) && (result & HeightValue)) {
	    size_hint.flags |= USSize;
	}
	if ((result & XValue) && (result & YValue)) {
	    size_hint.flags |= USPosition;
	}
    }

    shell = XCreateSimpleWindow(display, DefaultRootWindow(display),
	    size_hint.x, size_hint.y, size_hint.width, size_hint.height,
	    borderWidth, border, back);
    XSetStandardProperties(display, shell, "IMServer", "IMServer",
	    None, argv, argc, size_hint);
    XSelectInput(display, shell, PropertyChangeMask);

	XStoreName(display, shell, Name);
	;
    preedit = XCreateSimpleWindow(display, shell,
	/* x */	DEFAULT_DISTANCE,
	/* y */	DEFAULT_DISTANCE,
	/* w */	size_hint.width - 2 * (DEFAULT_DISTANCE + borderWidth),
	/* h */	font_ext->max_logical_extent.height,
		borderWidth, border, back);
    XSelectInput(display, preedit, ExposureMask);
    status = XCreateSimpleWindow(display, shell,
	/* x */	DEFAULT_DISTANCE,
	/* y */	2 * (DEFAULT_DISTANCE + borderWidth) + font_ext->max_logical_extent.height,
	/* w */	size_hint.width - 2 * DEFAULT_DISTANCE,
	/* h */	font_ext->max_logical_extent.height,
		0, border, back);
    XSelectInput(display, status, ExposureMask);
    if (InitializeProtocol(display, shell, preedit, status, fore, back, vendor, "0.9", "3.3", IMStartKeyList, font)) {
		fprintf(stderr, "%s: Another IM Server is alive.\n", argv[0]);
		exit(1);
	}
    XMapSubwindows(display, shell);
    XMapWindow(display, shell);

    MainLoop(display);
    fprintf(stderr, "%s: Another IM Server is involved.\n", argv[0]);
    exit(1);
}
