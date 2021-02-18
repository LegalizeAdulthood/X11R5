/* 
 * $Header: pexdpyinfo.c,v 1.1 91/09/18 13:50:08 sinyaw Exp $
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

static XpexString cmd_line_options[] = {
	"[-display display_name]",
	"\n\t[-synchronous]",
	"\n\t[-extensionInfo]",
	"\n\t[-enumTypeInfo]",
	"\n\t[-impDepConstants]",
	"\n\t[-lookupTables]",
	"\n\t[-pipelineContext]",
	"\n\t[-renderer]",
	"\n\t[-searchContext]",
	"\n\t[-workstation]",
	"\n\t[-all]",
	"\n",
	NULL
};

static Display *dpy;
static char *dpyname = NULL;
static XpexExtensionInfo extension_info;
static XExtCodes *pex_codes;

/* flags */
static int display_ext_info = 0;
static int display_enum_types = 0;
static int display_imp_dep = 0;
static int display_lut_info = 0;
static int display_pc_info = 0;
static int display_rdr_info = 0;
static int display_sc_info = 0;
static int display_wks_info = 0;
static int synchronous = 0;

int
main(argc, argv)
	int argc;
	char *argv[];
{
	handle_args(argc, argv);
	InitX(argv[0]);
	InitPEX();
	pexdpyinfo();
	exit(0);
}

handle_args(argc, argv)
	int argc;
	char *argv[];
{
	register int i;

	if (argc < 2) {
		usage(argv[0]);
		
	}
	for (i = 1; i < argc; i++) {
		if (!strcmp("-display",argv[i])) {
			dpyname = argv[++i];
		} else if (!strcmp("-synchronous",argv[i])) {
			synchronous = 1;
		} else if (!strcmp("-extensionInfo",argv[i])) {
			display_ext_info = 1;
		} else if (!strcmp("-enumTypeInfo",argv[i])) {
			display_enum_types = 1;
		} else if (!strcmp("-impDepConstants",argv[i])) {
			display_imp_dep = 1;
		} else if (!strcmp("-lookupTables",argv[i])) {
			display_lut_info = 1;
		} else if (!strcmp("-pipelineContext",argv[i])) {
			display_pc_info = 1;
		} else if (!strcmp("-renderer",argv[i])) {
			display_rdr_info = 1;
		} else if (!strcmp("-searchContext",argv[i])) {
			display_sc_info = 1;
		} else if (!strcmp("-workstation",argv[i])) {
			display_wks_info = 1;
		} else if (!strcmp("-all",argv[i])) {
			display_ext_info = display_imp_dep = 
			display_lut_info = display_pc_info = 
			display_rdr_info = display_sc_info = display_wks_info = 1;
		} else {
			(void) printf(
			"%s: bad command option \"%s\"\n\n",
			argv[0], argv[i]);

			usage(argv[0]);
			exit(-1);
		}
	} /* end-for */
}

InitX(name) 
	char *name;
{
	dpy = XOpenDisplay(dpyname);

	if (!dpy) {
		(void) fprintf(stderr, 
		"%s: can't connect to display %s\n",
		name, dpyname);
		exit(-1);
	}

	XSynchronize(dpy, synchronous); 
}

InitPEX() 
{
	pex_codes = XpexInitialize(dpy);

	if (!pex_codes) {
		exit(-1);
	}
	XpexSetFloatingPointFormat(dpy, Xpex_Ieee_754_32);
	XpexGetExtensionInfo(dpy, 5, 0, &extension_info);
}

usage(name)
	char *name;
{
	register int i = 0;

	(void) printf("usage: %s ", name);
	for (; cmd_line_options[i] != (XpexString) NULL; i++) {
		(void) printf("%s", cmd_line_options[i]);
	}
}

pexdpyinfo()
{
	static Window openWindow();

	Drawable drawable = openWindow(dpy);

	if (display_ext_info) {
		dpy_ext_info(dpy, pex_codes, &extension_info);
	}
	if (display_enum_types) {
		dpy_enum_types(dpy);
	}
	if (display_imp_dep) {
		dpy_imp_dep(dpy);
	}
	if (display_lut_info) {
		dpy_lut_info(dpy);
	}
	if (display_pc_info) {
		dpy_pc_info(dpy);
	}
	if (display_rdr_info) {
		dpy_rdr_info(dpy,drawable);
	}
	if (display_sc_info) {
		dpy_sc_info(dpy);
	}
	if (display_wks_info) {
		dpy_wks_info(dpy,drawable); 
	}
}

static Window
openWindow(dpy)
	Display *dpy;
{
	int screen = DefaultScreen(dpy);

	return XCreateSimpleWindow(dpy, 
		RootWindow(dpy,screen), 
		0, 0,  /* x, y */
		10, 10, /* width, height */
		2, /* border width */
		BlackPixel(dpy,screen),
		WhitePixel(dpy,screen));
}
