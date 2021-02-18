#include "Vlib.h"
#include <math.h>

int	pmap (vec, n)
unsigned long	*vec;
int		n; {

    static int itbl[] = {1, 2, 4, 8};
    register int i, r=0;

    for (i=0; i<4; ++i)
	if (itbl[i] & n)
	   r += vec[i];
    return r;
}

int VBindColors (v, background)
Viewport *v;
char * background; {

    register int i, j, k, n, c;
    static   int parseComplete = 0;
    unsigned int pixel;
    VColor 	*p;
    Display	*dpy;
    XColor	colorSet[MAXPCOLORS];
    unsigned long planemask[PLANES*2];
    unsigned long pixels[1];


    if (v->flags & VPMono)
	return 0;

    dpy = v->dpy;

/*
 *  If this is a color or greyscale pixmap, allocate color cells for each
 *  color, and then set up the Viewport for drawing.
 */

    if (v->flags & VPPixmap) {

	if (XParseColor(dpy, v->cmap, background, &colorSet[0]) == 0) {
		fprintf (stderr, "Can't parse color %s\n", background);
		return -1;
        }
	if (XAllocColor (dpy, v->cmap, &colorSet[0]) == 0) {
		fprintf (stderr, "Not enough pixel space for all colors\n");
		return -1;
	}
	v->aPixel[0] = colorSet[0].pixel;

	for ((i=0, p=VColorList); p != (VColor *) 0; i++) {

	    if (i > MAXPCOLORS) {
	        fprintf (stderr, "Too many colors selected.\n");
	        return -1;
	    }

	    if (XParseColor (dpy, v->cmap, p->color_name, &colorSet[i+1]) == 0) {
	        fprintf (stderr, "Can't parse color %s\n", p->color_name);
	        return -1;
	    }
	    p->xcolor = colorSet[i+1];

	    if (XAllocColor (dpy, v->cmap, &colorSet[i+1]) == 0) {
		fprintf (stderr, "Not enough pixel space for all colors\n");
		return -1;
	    }
	    v->aPixel[i+1] = colorSet[i+1].pixel;
	    p->index = i+1;
	    p = p->next;
	}

	v->colors = i+1;
	v->set = 0;
	v->pixel = &(v->aPixel[0]);
	v->mask = AllPlanes;

	return 0;
    }

    n = PLANES;
    c = 1 << n;


    if (XAllocColorCells (dpy, v->cmap, False, planemask, n*2, pixels, 1) == 0) {
        fprintf (stderr, "Cannot allocate color cells\n");
        return -1;
    }

/*
 *  Parse background color
 */

    if (/*parseComplete == 0*/ 1) {

	if (XParseColor(dpy, v->cmap, background, &colorSet[0]) == 0) {
	    fprintf (stderr, "Can't parse color %s\n", background);
            return -1;
        }

/*
 *  Parse each color defined in the V Color List
 */

	for ((i=0, p=VColorList); p != (VColor *) 0; i++) {
	    if (i > c) {
	        fprintf (stderr, "Too many colors selected.\n");
	        return -1;
	    }
	    if (XParseColor (dpy, v->cmap, p->color_name, &colorSet[i+1]) == 0) {
	        fprintf (stderr, "Can't parse color %s\n", p->color_name);
	        return -1;
	    }
	    p->xcolor = colorSet[i+1];
	    p->index = i+1;
	    p = p->next;
	}

	parseComplete = 1;
    }

    v->colors = i+1;

#ifdef DEBUG
    fprintf (stderr, "%d colors defined in the V color list.\n", i);
#endif

/*
 *  PAY ATTENTION!
 *
 *  We will now create a two lists of XColors. Each will expose a particular
 *  drawing buffer (there are two drawing buffers created here).
 *  A drawing is exposed by passing one of these lists to the XSetColors
 *  procedure.
 *  We create a list by iterating through each possible combination of
 *  pixel values, based on the values returned in pixel and planemask.
 *  The pixel value is determined using a function called pmap.  Each pixel
 *  value is assigned the appropriate XColor.
 */

    k = 0;
    for (i=0; i<v->colors; ++i) {
	pixel = v->aPixel[i] = pmap(&planemask[0], i) | pixels[0];
	for (j=0; j<v->colors; ++j) {
	    v->aColor[k] = colorSet[i];
	    v->aColor[k++].pixel = pixel | pmap (&planemask[n], j);
	}
    }

    v->aMask = pmap(&planemask[0], (c-1)) | pixels[0];

    k = 0;
    for (i=0; i<v->colors; ++i) {
	pixel = v->bPixel[i] = pmap(&planemask[n], i) | pixels[0];
	for (j=0; j<v->colors; ++j) {
	    v->bColor[k] = colorSet[i];
	    v->bColor[k++].pixel = pixel | pmap(&planemask[0], j);
	}
    }

    v->bMask = pmap(&planemask[n], (c-1)) | pixels[0];

    return 0;
}
