#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include "Clock.h"
#include <stdio.h> 

extern void exit();

/* Command line options table.  Only resources are entered here...there is a
   pass over the remaining options after XtParseCommand is let loose. */

/* Exit with message describing command line format */

void usage()
{
    fprintf(stderr,
"usage: oclock\n");
    fprintf (stderr, 
"       [-geometry [{width}][x{height}][{+-}{xoff}[{+-}{yoff}]]] [-display [{host}]:[{vs}]]\n");
    fprintf(stderr,
"       [-fg {color}] [-bg {color}] [-bd {color}] [-bw {pixels}]\n");
    fprintf(stderr,
"       [-minute {color}] [-hour {color}]\n");
    fprintf(stderr,
"       [-backing {backing-store}]\n");
    exit(1);
}

static XrmOptionDescRec options[] = {
{"-bg",		"*Background",		XrmoptionSepArg,	NULL},
{"-foreground",	"*Foreground",		XrmoptionSepArg,	NULL},
{"-background",	"*Background",		XrmoptionSepArg,	NULL},
{"-minute",	"*clock.minute",	XrmoptionSepArg,	NULL},
{"-hour",	"*clock.hour",		XrmoptionSepArg,	NULL},
{"-backing",	"*clock.backingStore",	XrmoptionSepArg,	NULL},
};

void main(argc, argv)
    int argc;
    char **argv;
{
    Widget toplevel;
    Widget clock;
    
    toplevel = XtInitialize(NULL, "LogoClock", options, XtNumber (options),
				    &argc, argv);
      
    if (argc != 1) usage();

    clock = XtCreateManagedWidget ("lclock", clockWidgetClass, toplevel, 
				NULL, 0);
    XtRealizeWidget (toplevel);
    XtMainLoop();
}
