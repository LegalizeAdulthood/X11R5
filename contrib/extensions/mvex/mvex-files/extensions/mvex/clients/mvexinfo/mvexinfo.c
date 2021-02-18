/*
 * Copyright 1989, 1990 by Tektronix, Inc., Beaverton, Oregon, and
 * the Massachusetts Institute of Technology, Cambridge, Massachusetts.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Tektronix or M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Tektronix and M.I.T. make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * TEKTRONIX AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL TEKTRONIX OR M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Contains parts of xdpyinfo, writen by Jim Fulton, MIT X Consortium
 */

#ifndef LINT
static char *copy_notice = "Copyright 1990 Tektronix, Inc. and M.I.T.";
#ifdef RCS_ID
static char *rcsid=  "$Header: /n/lacey/usr/local/src/video/MVEX/clients/mvexinfo/RCS/mvexinfo.c,v 1.25 1991/09/26 06:18:20 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */
/*
 * xvexinfo - print information about X video connecton
 */

#include <stdio.h>
#include <math.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/MVEX/MVEXlib.h>

#define NUM_ELEMENTS_START	3

char *ProgramName;


static void usage ()
{
    fprintf (stderr, "usage:  %s [-display displayname]\n",
	     ProgramName);
    exit (1);
}

main (argc, argv)
    int argc;
    char *argv[];
{
    Display *dpy;			/* X connection */
    char *displayname = NULL;		/* server to contact */
    int i;				/* temp variable:  iterator */

    ProgramName = argv[0];

    for (i = 1; i < argc; i++) {
	char *arg = argv[i];

	if (arg[0] == '-') {
	    switch (arg[1]) {
	      case 'd':
		if (++i >= argc) usage ();
		displayname = argv[i];
		continue;
	      default:
		usage ();
	    }
	} else
	  usage ();
    }

    dpy = XOpenDisplay (displayname);
    if (!dpy) {
	fprintf (stderr, "%s:  unable to open display \"%s\".\n",
		 ProgramName, XDisplayName (displayname));
	exit (1);
    }

    PrintExtensionInfo (dpy);

    for (i = 0; i < ScreenCount (dpy); i++) {
	PrintVexInfo (dpy, i);
    }

    XCloseDisplay (dpy);
    exit (0);
}


PrintExtensionInfo (dpy)
    Display *dpy;
{
    int n = 0;
    char **extlist = XListExtensions (dpy, &n);

    printf ("Number of extensions:    %d\n", n);

    if (extlist) {
	register int i;

	for (i = 0; i < n; i++) {
	    printf ("      %s\n", extlist[i]);
	}
	XFreeExtensionList (extlist);
    }
    return;
}

typedef struct _vismap {
    char *className;
    int visualid;
} VisualMap;
static int NVisuals;
static VisualMap *VisualMapList;

static char *VisualClassName(id)
    int id;
{
    int i;

    for (i = 0; i < NVisuals; i++)
	if (id == VisualMapList[ i ].visualid)
	    return (VisualMapList[ i ].className);
    return ("unknown");
}

static void AddVisualMap(id, class)
    int id;
    int class;
{
    int i;
    char *className;

    for (i = 0; i < NVisuals; i++)
	if (id == VisualMapList[ i ].visualid)
	    return;

    switch (class) {
      case StaticGray:     className = "StaticGray"; break;
      case GrayScale:      className = "GrayScale"; break;
      case StaticColor:    className = "StaticColor"; break;
      case PseudoColor:    className = "PseudoColor"; break;
      case TrueColor:      className = "TrueColor"; break;
      case DirectColor:    className = "DirectColor"; break;
      case MVEXVideoColor: className = "VideoColor"; break;
      case MVEXVideoGray:  className = "VideoGray"; break;
      default: return;
    }

    i = NVisuals++;
    if (VisualMapList == NULL)
	VisualMapList = (VisualMap *)malloc(sizeof(VisualMap));
    else
	VisualMapList = (VisualMap *)realloc(VisualMapList,
					     sizeof(VisualMap) * NVisuals);
    VisualMapList[ i ].className = className;
    VisualMapList[ i ].visualid = id;
}

PrintVexInfo (dpy, scr)
    Display *dpy;
    int scr;
{
    Screen *s = ScreenOfDisplay (dpy, scr);  /* opaque structure */
    XVisualInfo viproto;		/* template */
    XVisualInfo *vip;			/* retured info */
    int nvi;				/* number of elements returned */
    int nscr;				/* number of screens */
    int i,j;				/* temp variable: iterator */
    char eventbuf[80];			/* want 79 chars per line + nul */
    MVEXVin *pXvin;
    int nvins;
    MVEXVout *pXvout;
    int nvouts;
    XRectangle *clip_size;
    Bool input_overlap, capture_overlap, io_overlap;
    char *nameString;
    int majOpcode, firstEvent, firstError;


    if (!XQueryExtension(dpy, MVEX_NAME, &majOpcode, &firstEvent, &firstError)){
	printf("No MVEX extension on display (screen %d)\n", scr);
	return;
    }
    if (scr == 0) {
	printf("MVEX major opcode %d\n", majOpcode);
	printf("     base event %d\n", firstEvent);
	printf("     base error %d\n", firstError);
	printf("     major version %d\n", MVEXMajorVersion(dpy));
	printf("     minor version %d\n", MVEXMinorVersion(dpy));
    }

    printf ("\n");
    printf ("screen #%d:\n", scr);

    /*
     * Get the visuals listed by MVEX
     */
    nvi = 0;
    vip = MVEXGetVisualInfo (dpy, VisualNoMask, &viproto, &nvi, False);
    printf ("  Number of unique video visuals:    %d\n", nvi);
    for (i = 0; i < nvi; i++) {
	PrintVisualInfo (dpy, vip+i);
    }
    if (vip) XFree ((char *) vip);

    /*
     * Get all the core visuals, too, so we know their names
     */
    nvi = 0;
    vip = XGetVisualInfo (dpy, VisualNoMask, &viproto, &nvi);
    for (i = 0; i < nvi; i++) {
	AddVisualMap(vip[i].visualid, vip[i].class);
    }
    if (vip) XFree ((char *) vip);

    MVEXOverlapsOfScreen(dpy, scr,
	&input_overlap, &capture_overlap, &io_overlap);
    printf ("  MVEXOverlapping capabilities:\n");
    printf ("    InputMVEXOverlap: %s\n",
	input_overlap ? "Permitted" : "Not permitted");
    printf ("    CaptureMVEXOverlap: %s\n",
	capture_overlap ? "Permitted" : "Not permitted");
    printf ("    InputOutputMVEXOverlap: %s\n",
	io_overlap ? "Permitted" : "Not permitted");

    pXvin = MVEXGetVins( dpy, scr, &nvins);
    pXvout = MVEXGetVouts( dpy, scr, &nvouts);

    printf ("  Number of video inputs = %d\n",nvins);
    for (i=0; i<nvins; i++){
	printf ("    input VideoReference 0x%x:\n", pXvin[i].vref);
	PrintGeometry(&pXvin[i].geometry);
	clip_size = &pXvin[i].clip_size;
	printf ("      ClipSize: x=%d, y=%d, width=%d, height=%d\n", 
		clip_size->x, clip_size->y, 
		clip_size->width, clip_size->height);
	printf ("      %d ports: \n", pXvin[i].n_ports);
	PrintPorts(dpy, &pXvin[i]);
	printf ("      Video visuals: \n");
	PrintVideoVisuals(&pXvin[i], True);
	printf ("      Video abilities: \n");
	PrintVideoAbilities(&pXvin[i]);
    }

    printf ("  Number of video outputs = %d\n",nvouts);
    for (i=0; i<nvouts; i++){
	printf ("  output VideoReference 0x%x:\n", pXvout[i].vref);
	PrintGeometry(&pXvout[i].geometry);
	printf ("      %d ports: \n", pXvout[i].n_ports);
	PrintPorts(dpy, &pXvout[i]);
	printf ("      Video visuals: \n");
	PrintVideoVisuals(&pXvout[i], False);
	printf ("      Video abilities: \n");
	PrintVideoAbilities(&pXvout[i]);
    }

    XFree(pXvin);
    XFree(pXvout);
    return;
}

PrintVisualInfo (dpy, vip)
    Display *dpy;
    XVisualInfo *vip;
{
    char *class = NULL;			/* for printing */

    AddVisualMap(vip->visualid, vip->class);
    printf ("  visual:\n");
    printf ("    visual id:    0x%lx\n", vip->visualid);
    printf ("    class:    %s\n", class);
    printf ("    depth:    %d plane%s\n", vip->depth, 
	    vip->depth == 1 ? "" : "s");
    printf ("    size of colormap:    %d entries\n", vip->colormap_size);
    printf ("    red, green, blue masks:    0x%lx, 0x%lx, 0x%lx\n",
	    vip->red_mask, vip->green_mask, vip->blue_mask);
    printf ("    significant bits in color specification:    %d bits\n",
	    vip->bits_per_rgb);

    return;
}

PrintGeometry(geom)
    MVEXGeometry  *geom;
{
    int j;
    char *indent = "      ";

    printf ("%sframe rate = %g\n", indent,
	(double)geom->frame_rate.numerator /
	(double)geom->frame_rate.denominator);
    printf ("%sfield rate = %g\n", indent,
	(double)geom->field_rate.numerator /
	(double)geom->field_rate.denominator);
    printf ("%swidth = %d\n", indent, geom->width);
    printf ("%sheight = %d\n", indent, geom->height);
    printf ("%sconcurrent use = %d\n", indent, geom->concurrent_use);
    printf ("%spriority steps = %d\n", indent, geom->priority_steps);
    printf ("%snumber of placements = %d:%s\n",
	indent, geom->nplacement,
	geom->nplacement ? "" : " any placement acceptable");
    for (j=0; j<geom->nplacement; j++) {
	printf ("%s  placement #%d:\n", indent, j+1);
	printf ("%s    frame rate = %g\n", indent,
	    (double)geom->placement[ j ].frame_rate.numerator/
	    (double)geom->placement[ j ].frame_rate.denominator);
	printf ("%s    src rectangle values:\n", indent);
	PrintRectangleRange(&geom->placement[ j ].src);
	printf ("%s    dest rectangle values:\n", indent);
	PrintRectangleRange(&geom->placement[ j ].dest);
	if (geom->placement[ j ].identity_aspect)
	    printf ("%s    x&y scale values: ", indent);
	else
	    printf ("%s    xscale values: ", indent);
	PrintFractionRange(&geom->placement[ j ].x_scale);
	if (! geom->placement[ j ].identity_aspect) {
	    printf ("%s    yscale values: ", indent);
	    PrintFractionRange(&geom->placement[ j ].y_scale);
	}
    }
}

PrintFraction(pFrac)
    MVEXFraction *pFrac;
{
    printf("%d/%d = %g\n",
	pFrac->numerator, pFrac->denominator,
	(float)pFrac->numerator  / pFrac->denominator);
}

PrintPorts(dpy, vio)
    Display *dpy;
    MVEXVin *vio;
{
    char *indent = "        ";
    char *portName;
    int i;

    for (i = 0; i < vio->n_ports; i++) {
	portName = XGetAtomName(dpy, vio->ports[ i ]);
	if (portName == NULL)
	    portName = "unknown??";
	printf("%sport atom 0x%x = \"%s\"\n", indent,
	    vio->ports[ i ], portName);
    }
}

PrintVideoAbilities(vio)
    MVEXVin *vio;
{
    MVEXVideoAbility *pAble = &vio->ability;
    char *indent = "          ";

    if (pAble->n_saturation) {
	printf("%snormal saturation: ", indent);
	PrintFraction(&pAble->normal_saturation);
	printf("%ssaturation values: ", indent);
	PrintFractionRange(pAble->saturation);
    }

    if (pAble->n_contrast) {
	printf("%snormal contrast:   ", indent);
	PrintFraction(&pAble->normal_contrast);
	printf("%scontrast values:   ", indent);
	PrintFractionRange(pAble->contrast);
    }

    if (pAble->n_hue) {
	printf("%snormal hue:        ", indent);
	PrintFraction(&pAble->normal_hue);
	printf("%shue values:        ", indent);
	PrintFractionRange(pAble->hue);
    }

    if (pAble->n_bright) {
	printf("%snormal bright:     ", indent);
	PrintFraction(&pAble->normal_bright);
	printf("%sbright values:     ", indent);
	PrintFractionRange(pAble->bright);
    }
}

PrintVideoVisuals(vio, isVin)
    MVEXVin *vio;
    Bool isVin;
{
    MVEXRenderModel *model = vio->model_list;
    char *indent = "          ";
    int j, k;

    for (j=0; j < vio->n_models; j++) {
	printf ("        depth: %d\n", model[j].depth);
	if (isVin) {
	    if (model[j].opaque)
		printf ("%sopaque (pixels can't be interpreted)\n", indent);
	    else
		printf ("%snon-opaque (pixels rendered in %s visual type)\n",
		    indent, VisualClassName(model[j].visualid));
	} else {
	    if (model[j].model_mask & MVEXCompositeModelMask)
		printf ("%scomposite (%s)\n", indent,
		    "IncludeInferiors subwindow-mode forced on CaptureGraphics");
	    else
		printf ("%snot composite (%s)\n", indent,
		    "subwindow-mode may be any value on CaptureGraphics");
	}

	switch (model[j].model_mask
	      & (MVEXWindowModelMask|MVEXPixmapModelMask)) {
	case MVEXWindowModelMask | MVEXPixmapModelMask:
	    printf ("%sWindows and Pixmaps supported\n", indent);
	    break;
	case MVEXWindowModelMask:
	    printf ("%sOnly windows supported\n", indent);
	    break;
	case MVEXPixmapModelMask:
	    printf ("%sOnly pixmaps supported\n", indent);
	    break;
	}
	if (model[j].visualid)
	    printf ("%svisual ID: 0x%lx\n", indent, model[j].visualid);
	else
	    printf ("%sno visuals supported\n", indent);
	if (model[j].model_mask & MVEXPixmapModelMask)
	    printf ("%sred, green, blue masks:    0x%lx, 0x%lx, 0x%lx\n",
		    indent,
		    model[j].red_mask, model[j].green_mask, model[j].blue_mask);
    }
}

#define EPSILON	0.001
PrintFractionRange(frac_range)
    MVEXFractionRange *frac_range;
{

    double num_base, num_increment, num_limit;
    double denom_base, denom_increment, denom_limit;
    double base, increment, limit;
    double lastval, curval;
    Bool print_last_val;
    int i, k;
    int type;
    int nElements;

    num_base = (double)frac_range->num_base;
    num_increment = (double)frac_range->num_increment;
    num_limit = (double)frac_range->num_limit;
    denom_base = (double)frac_range->denom_base;
    denom_increment = (double)frac_range->denom_increment;
    denom_limit = (double)frac_range->denom_limit;

    print_last_val = False;
    for (k=0; k < 2; k++) {
	if (k == 0) {
	    base = num_base;
	    limit = num_limit;
	    increment = num_increment;
	    type = frac_range->num_type;
	} else {
	    base = denom_base;
	    limit = denom_limit;
	    increment = denom_increment;
	    type = frac_range->denom_type;
	}
	if (type == MVEXLinearRange)
	    nElements = (limit - base) / increment + 1;
	else
	    nElements = limit - increment;
	if (nElements <= NUM_ELEMENTS_START){
	    print_last_val = False;
	} else {
	    print_last_val = True;
	    nElements = NUM_ELEMENTS_START;
	}
	if (frac_range->denom_type == MVEXLinearRange) {
	    lastval = limit;
	    for (curval=base, i=0; i < nElements; i++, curval += increment) {
		printf ("%s%g", i ? ", " : "", curval);
	    }
	} else {
	    lastval = pow(base, limit);
	    for (i=0; i < nElements; i++) {
		printf ("%s%g", i ? ", " : "", pow(base, i + increment));
	    }
	}
	if (k == 0)
	    if (print_last_val)
		printf (", ..., %g / ", lastval);
	    else
		printf (" / ");
    }

    if (print_last_val)
	printf ("..., %g\n", lastval);
    else
	printf ("\n");
}

PrintRectangleRange(rect_range)
    MVEXRectangleRange *rect_range;
{
    char *indent = "            ";

    printf ("%sx = ", indent);
    PrintRectangeSet((long) rect_range->base.x, (long) rect_range->x_inc, 
	    (long) rect_range->limit.x, rect_range->type);
    printf ("%sy = ", indent);
    PrintRectangeSet((long) rect_range->base.y, (long) rect_range->y_inc, 
	    (long) rect_range->limit.y, rect_range->type);
    printf ("%swidth = ", indent);
    PrintRectangeSet((long)rect_range->base.width, 
	    (long)rect_range->width_inc, (long)rect_range->limit.width, 
	    rect_range->type);
    printf ("%sheight = ", indent);
    PrintRectangeSet((long)rect_range->base.height,
	    (long)rect_range->height_inc, (long)rect_range->limit.height, 
	    rect_range->type);
    
}

PrintRectangeSet(base, increment, limit, type)
    long base;
    long increment;
    long limit;
    char type;
{
    double lastval, curval;
    int i;

    if (type == MVEXLinearRange) {
	lastval = limit;
	for (i = 0; i<NUM_ELEMENTS_START; i++) {
	    curval = base + i*increment;
	    if (curval >= lastval) {
		break;
	    }
	    printf ("%g, ", curval);
	}
    } else {
	lastval = pow((double)increment, (double) limit);
	for (i = 0; i<NUM_ELEMENTS_START; i++) {
	    curval = pow((double)increment, (double)base+i);
	    if (curval >= lastval - EPSILON) {
		break;
	    }
	    printf ("%g, ", curval);
	}
    }

    if (i != 0)
	printf ("..., ");
    printf ("%g\n", lastval);

}
