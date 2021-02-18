/*
 *
 * Copyright 1991 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	xcrtca.c
 *
 *	Get measurements from a colorimeter.
 */

#include	<stdio.h>
#include	<termio.h>
#include	<signal.h>
/* #include	<sys/filio.h> */
#include	<fcntl.h>
#include	<ctype.h>
#include	<sys/time.h>
#include	<X11/Intrinsic.h>
#include	<X11/StringDefs.h>
#include	<X11/Shell.h>
#include	<X11/Xaw/Cardinals.h>

#include        <X11/Xlib.h>
#include        <X11/Xutil.h>

#include	<sys/ioctl.h>
#include        "nap.h"

#define DEFAULT_DELAY	4
#define DEFAULT_BAUD_FLAG	B9600
#define DEFAULT_BAUD		9600
int	ttyspeed = DEFAULT_BAUD_FLAG;
int	sDelay = DEFAULT_DELAY;

FILE	*ttywrite;
FILE	*fileread;
FILE	*filewrite;
FILE    *rawWrite;
int	ttyreadfd;
int	incommand;

static XrmOptionDescRec options [] = {
{"-device",	".device",	XrmoptionSepArg,    NULL},
{"-format",	".format",	XrmoptionSepArg,    NULL},
{"-file",	".file",	XrmoptionSepArg,    NULL},
{"-input",	".input",	XrmoptionSepArg,    NULL},
{"-name",       ".name",        XrmoptionSepArg,    NULL},
{"-model",      ".model",       XrmoptionSepArg,    NULL},
{"-baud",       ".baud",        XrmoptionSepArg,    NULL},
{"-delay",      ".delay",       XrmoptionSepArg,    NULL},
};

typedef struct {
    char    *device;
    char    *format;
    char    *file;
    char    *input;
    char    *name;
    char    *model;
    int	    baud;
    int	    delay;
} ResourceData, *ResourceDataPtr;

static ResourceData applicationData;

#define offset(field) XtOffsetOf(ResourceData,field)

XtResource resources[] = {
    {"device",	"Device", XtRString, sizeof (char *),
	offset(device), XtRString, "ca100"},
    {"format",	"Format", XtRString, sizeof (char *),
	offset(format), XtRString, "xyY"},
    {"file",	"File",	XtRString, sizeof (char *),
	offset(file), XtRString, NULL },
    {"input",	"Input",    XtRString, sizeof (char *),
	offset(input), XtRString, "/dev/ttya"},
    {"name",	"Name",    XtRString, sizeof (char *),
	offset(name), XtRString, "Monitor Name"},
    {"model",	"Model",    XtRString, sizeof (char *),
	offset(model), XtRString, "Monitor Model"},
    {"baud",	"Baud",    XtRInt, sizeof (int),
	offset(baud), XtRImmediate, (caddr_t)DEFAULT_BAUD},
    {"delay",	"Delay",    XtRInt, sizeof (int),
	offset(delay), XtRImmediate, (caddr_t)DEFAULT_DELAY},
};

#undef offset

static char ttybuf[1024];
static int  ttyused;
static int  ttycount;
static XtAppContext	xtcontext;

static void
readtty (closure, sourcep, inputid)
    XtPointer	closure;
    int		*sourcep;
    XtInputId	*inputid;
{
    int	size;
    int	fd;
    int	count;

    fd = *sourcep;
    size = sizeof (ttybuf) - ttycount;
    if (size <= 0)
    {
	write (1, "\007", 1);
	return;
    }
    count = read (*sourcep, ttybuf + ttycount, size);
    if (count > 0)
	ttycount += count;
}

static unsigned short
baudflag(baudrate)
    int baudrate;
{
    switch (baudrate) {
      case 50:
	return(B50);
      case 75:
	return(B75);
      case 110:
	return(B110);
      case 134:
	return(B134);
      case 150:
	return(B150);
      case 200:
	return(B200);
      case 300:
	return(B300);
      case 600:
	return(B600);
      case 1200:
	return(B1200);
      case 1800:
	return(B1800);
      case 2400:
	return(B2400);
      case 4800:
	return(B4800);
      case 9600:
	return(B9600);
      case 19200:
	return(B19200);
      case 38400:
	return(B38400);
      default:
	return(0);
    }
}

main (argc, argv)
char	**argv;
{
  Display    *dpy;		/* X server connection */
  XColor colorCell;
  unsigned long plane_mask[1];
  unsigned long pixels[1];
  int i;
  Widget    toplevel;
  Colormap  cmap;

  int	takereading();
  int	temp;
  char    ret;
    Arg	args[10];
    XEvent  event;

    i = 0;
    XtSetArg(args[i], XtNoverrideRedirect, TRUE); i++;
    toplevel = XtAppInitialize (&xtcontext, "Ca100", options, XtNumber(options),
		     &argc, argv, NULL, args, i);

    XtGetApplicationResources (toplevel, (XtPointer) &applicationData, resources,
				XtNumber(resources), NULL, ZERO);
				
    dpy = XtDisplay (toplevel);
  
    cmap = DefaultColormap (dpy, DefaultScreen (dpy));

    XAllocColorCells(dpy, cmap, False, plane_mask, 0, pixels, 1);
    
    i = 0;
    XtSetArg (args[i], XtNbackground, pixels[0]); i++;
    XtSetArg (args[i], XtNx, 0); i++;
    XtSetArg (args[i], XtNy, 0); i++;
    XtSetArg (args[i], XtNwidth, DisplayWidth (dpy, DefaultScreen(dpy))); i++;
    XtSetArg (args[i], XtNheight, DisplayHeight (dpy, DefaultScreen(dpy))); i++;
    XtSetValues (toplevel, args, i);

    ttyreadfd = open (applicationData.input, 0);
    ttyspeed = baudflag(applicationData.baud);
    sDelay = applicationData.delay;
    settty (ttyreadfd);
    XtAppAddInput (xtcontext, ttyreadfd, (XtPointer) XtInputReadMask, readtty, NULL);

    fprintf(stderr, "Place the probe, then hit return:");
    scanf("%c", &ret);

    colorCell.pixel = pixels[0];
    colorCell.flags = DoRed | DoGreen | DoBlue;
    colorCell.red = 0;
    colorCell.green = 0;
    colorCell.blue = 0;
    XStoreColor (dpy, cmap, &colorCell);

    XtRealizeWidget (toplevel);

    XFlush (dpy);

    if (strcmp(applicationData.device, "ca100") == 0) {
	ttywrite = fopen (applicationData.input, "w");
	setbuf (ttywrite, NULL);
    }

    if (applicationData.file)
	rawWrite = fopen (applicationData.file, "w" );
    else
	rawWrite = stdout;

    fprintf(rawWrite, "NAME %s\n", applicationData.name);
    fprintf(rawWrite, "MODEL %s\n", applicationData.model);
    fprintf(rawWrite, "FORMAT %s\n", applicationData.format);
  
    if (strcmp(applicationData.device, "ca100") == 0) {
	fputs("F1\r", ttywrite); /* turn on remote */
    }

    for (i=0;i<=0xff;i++) {	/* red */
	colorCell.red = i << 8;
	colorCell.green = 0x00;
	colorCell.blue = 0x00;
	XForceScreenSaver(dpy, ScreenSaverReset);
	XStoreColor(dpy, cmap, &colorCell);
	XSync(dpy, 0);
	if (i==0) sleep(sDelay);
	takereading(i);
    }
    for (i=0;i<=0xff;i++) {	/* green */
	colorCell.red = 0x00;	
	colorCell.green = i<<8;
	colorCell.blue = 0x00;
	XForceScreenSaver(dpy, ScreenSaverReset);
	XStoreColor(dpy, cmap, &colorCell);
	XSync(dpy, 0);
	if (i==0) sleep(sDelay);
	takereading(i);
    }
    for (i=0;i<=0xff;i++) {	/* blue */
	colorCell.red = 0x00;
	colorCell.green = 0x00;
	colorCell.blue = i<<8;
	XForceScreenSaver(dpy, ScreenSaverReset);
	XStoreColor(dpy, cmap, &colorCell);
	XSync(dpy, 0);
	if (i==0) sleep(sDelay);
	takereading(i);
    }

    if (strcmp(applicationData.device, "ca100") == 0) {
	fputs ("F0\r", ttywrite);
    }

    if (rawWrite)
	fclose (rawWrite);
}

takereading(index)
     int index;
{    
    int i,j, intty, count;
    char outbuf[18];
    float parsedx, parsedy, parsedY;
    XEvent    event;
    XtInputMask	mask;
    char	inputline[1024];
    char	format[8];
    double  a, b, c;
    int	attempts;

    nap (30);
    for (;;) {
	mask = XtAppPending (xtcontext);
	if (!(mask & XtIMAlternateInput))
	    break;
	XtAppProcessEvent (xtcontext, XtIMAll);
    }
    ttycount = 0;
    ttyused = 0;

    attempts = 0;
    for(;;) {
    	count = 0;
    	do {
    	    while (ttycount == 0) {
	    	XtAppProcessEvent (xtcontext, XtIMAll);
    	    }
	    while (ttyused < ttycount) {
	    	inputline[count++] = ttybuf[ttyused++];
	    	if (inputline[count-1] == '\r')
		    break;
	    }
	    if (ttyused == ttycount)
	    	ttyused = ttycount = 0;
    	} while (count == 0 || inputline[count-1] != '\r');
    
    	inputline[count-1] = '\0';

	if (count == 1) continue;

	if (strcmp(applicationData.device, "ca100") == 0) {
	    /* CA-100 measurement in xyY format */
	    if (sscanf (inputline + 3, "%lf;%lf;%lf ", &a, &b, &c) == 3) {
		/* CA-100 scales x and y up by 1000 */
		a /= 1000;
		b /= 1000;
		break;
	    }
	} else if (strcmp(applicationData.device, "j17") == 0) {
	    if (sscanf(inputline, "%s%lf,%lf,%lf ", format, &a, &b, &c) == 4) {
		if (strcmp(format, applicationData.format) != 0) {
		    if (attempts == 4) {
		        fprintf (stderr, "Format mismatch: expected %s\n",
			    applicationData.format);
		        fprintf (stderr, "\t got %s\n", format);
		    exit (1);
		    }
		} else {
		    break;
		}
	    }
	} else {
	    fprintf (stderr, "Invalid device %s\n", applicationData.device);
	    exit (1);
	}

	if (++attempts == 5)
	{
	    fprintf (stderr, "Can't get reading at %d\n", index);
	    exit (1);
	    return;
	}
    }
    fprintf (rawWrite, "%d %lf;%lf;%lf\n", index, a, b, c);
    fflush (rawWrite);
}

struct termio origtermio, newtermio;

struct termio ttytermio = { 
    IGNBRK | IXON | IXANY | ISTRIP,		    /* c_iflag */
    0,						    /* c_oflag */
    CS7 | CSTOPB | CREAD | PARENB | CLOCAL,	    /* c_cflag */
    0,						    /* c_lflag */
    0,						    /* c_line */
    {0},					    /* c_cc */
};

settty (fd)
{
    ttytermio.c_cflag |= ttyspeed;
    ttytermio.c_cc[VMIN] = 6;
    ttytermio.c_cc[VTIME] = 1;
    ioctl (fd, TCSETA, &ttytermio);
}
