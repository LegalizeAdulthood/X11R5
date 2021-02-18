/*
 * $Header: pexlsfonts.c,v 1.5 91/09/11 17:40:24 sinyaw Exp $
 */

/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

#include <stdio.h>
#include "Xpexlib.h"

static Display *dpy;
static char *dpyname = (char *) NULL;
static XExtCodes *pex_codes;
static XpexString *font_names; 
static int actual_count;

static int query_font = 0;
static int synchronous = 0;

pexlsfonts()
{
	int i;

	dpy = XOpenDisplay(dpyname);

	if (!dpy) {
		printf("can't connect to server\n");
		exit(-1);
	}
	XSynchronize(dpy, synchronous);

	pex_codes = XpexInitialize(dpy);

	if (!pex_codes) {
		printf("PEX extension not supported\n");
		exit(-1);
	}
	XpexSetFloatingPointFormat(dpy, Xpex_Ieee_754_32);

	font_names = XpexListFonts(dpy, "*", 10, &actual_count);

	for (i = 0; i < actual_count; i++) {
		printf("%s\n",font_names[i]);
	}
}

QueryFont()
{
	XpexFont font_id;
	XpexFontInfo *font_info;

	font_id = XpexOpenFont(dpy, font_names[0]);

	font_info = XpexQueryFont(dpy, font_id);

	if (font_info) {
		printf("first_glyph: %d\n",font_info->first_glyph);
		printf("last_glyph: %d\n",font_info->last_glyph);
		printf("default_glyph: %d\n",font_info->default_glyph);
		printf("all_exist: %d\n",font_info->all_exist);
		printf("stroke_font: %d\n",font_info->stroke_font);
		printf("n_properties: %d\n",font_info->n_properties);
	}

	XpexCloseFont(dpy, font_id);
}

usage(name)
	char *name;
{
	static XpexString cmd_line_options[] = {
		"[-display display_name]",
		"\n\t[-synchronous]",
		"\n\t[-queryFont]",
		"\n",
		NULL
	};
	register int i = 0;

	(void) printf("usage: %s ", name);

	for (; cmd_line_options[i] != (XpexString) NULL; i++) {
		(void) printf("%s", cmd_line_options[i]);
	}
}

handle_args(argc,argv)
	int argc;
	char *argv[];
{
	register int i;

	if (argc < 2) {
		return;
	}
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-synchronous")) {
			synchronous = 1;
		} else if (!strcmp("-display",argv[i])) {
			dpyname = argv[++i];
		} else if (!strcmp("-queryFont",argv[i])) {
			query_font = 1;
		} else {
			(void) printf(
			"%s: bad command option \"%s\"\n\n",
			argv[0], argv[i]);

			usage(argv[0]);
			exit(-1);
		}
	} /* end-for */
}

int
main(argc, argv)
	int argc;
	char *argv[];
{	
	handle_args(argc,argv);

	pexlsfonts();

	if (query_font) {
		QueryFont();
	}
}
