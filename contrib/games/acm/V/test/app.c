#include "../lib/Vlib.h"
#include <X11/Xutil.h>
#include <math.h>

#define VIEW1

extern Display  *dpy;
extern Window   win;
extern GC       curGC;
extern XSizeHints xsh;
extern int	mono;

VPolygon *poly[1024];

void placeObject (obj, loc, roll, pitch, yaw, poly, cnt)
VObject *obj;
VPoint  loc;
double roll;
double pitch;
double yaw;
VPolygon **poly;
int	 *cnt; {

	int	 i, j, k;
	VPoint	 *q, tmp;
	VMatrix	 mtx;

	j = *cnt;

	VIdentMatrix (&mtx);
	if (roll != 0.0)
		VRotate (&mtx, XRotation, roll);
	if (pitch != 0.0)
		VRotate (&mtx, YRotation, pitch);
	if (yaw != 0.0)
		VRotate (&mtx, ZRotation, yaw);
	VTranslatePoint (&mtx, loc);

	for (i=0; i<obj->numPolys; ++i) {

		if (poly[j] != (VPolygon *) NULL)
			VDestroyPolygon (poly[j]);

		poly[j] = VCopyPolygon(obj->polygon[i]);
		for ((k=0, q=poly[j]->vertex); k<poly[j]->numVtces; (++k, ++q)) {
			VTransform(q, &mtx, &tmp);
			*q = tmp;
		}
		++j;
	}

	*cnt = j;
}

app(background)
char *background; {

	int	i, cnt, black;
	unsigned int j, curPixel = 0;
	char	*str;
	FILE	*file;
	VObject	*obj, *ftr, *mig;
	VPoint	viewPt, centerInt, up, ftrLoc, ftrLoc2, ftrLoc3, migLoc1, migLoc2;
	Viewport *v;
	double	dist, p, migV, v1, a, migRoll;
	double	updateRate = 5.0;
	Colormap cmap;
	long	screen;

	str = "fullrwy";

	if ((file = fopen(str, "r")) == (FILE *) NULL) {
		perror ("Cannot open object file");
		exit (1);
	}

	if ((obj = VReadObject(file)) == (VObject *) NULL) {
		fprintf (stderr, "Error reading the object definition.\n");
		exit (1);
	}

	fclose (file);

	str = "f16";

	if ((file = fopen(str, "r")) == (FILE *) NULL) {
		perror ("Cannot open object file");
		exit (1);
	}

	if ((ftr = VReadObject(file)) == (VObject *) NULL) {
		fprintf (stderr, "Error reading the object definition.\n");
		exit (1);
	}

	fclose (file);
	

	str = "mig23";

	if ((file = fopen(str, "r")) == (FILE *) NULL) {
		perror ("Cannot open object file");
		exit (1);
	}

	if ((mig = VReadObject(file)) == (VObject *) NULL) {
		fprintf (stderr, "Error reading the object definition.\n");
		exit (1);
	}

	fclose (file);
	
	usePixmaps = 0;

	screen = DefaultScreen (dpy);
	cmap = DefaultColormap (dpy, screen);

	v = VOpenViewport (dpy, screen, win, cmap, 12 * 25.4 / 1000.0,
		1.5, 1.0, xsh.width, xsh.height);

	if (VBindColors (v, background) < 0) {
		fprintf (stderr, "Error in binding colors.\n");
		exit (1);
	}
	VExposeBuffer (v, curGC);


	ftrLoc.x = 80.0;
	ftrLoc.y = -110.0;
	ftrLoc.z = -7.0;

	ftrLoc2.x = 475.0;
	ftrLoc2.y = 3.7;
	ftrLoc2.z = -7.0;

	migLoc1.z = -310.0;
	migLoc2.z = -320.0;
	migV = 370.0 / 3600.0 * 5280.0 / updateRate;	/* mig speed in fps */
	migV = migV / (2500.0 * 3.14 * 2.0); /* mig speed in rad/update */
	migRoll = 28.0 * 3.14 / 180.0;
	a = 90.0 * 3.14 / 180.0;

	v1 = 130.0 / 3600.0 * 5280.0 / updateRate;

	centerInt.x = 80.0;
	centerInt.y = -90.0;
	centerInt.z = -6.0;

	dist = -10000.0; p = 0;

	while (1) {

		ftrLoc3.x = dist+150.0;
		ftrLoc3.y = -15.0;
		ftrLoc3.z = (dist+150.0) * 50.0 / 1000.0 - 40.0;
		if (ftrLoc3.z > -13.0)
			ftrLoc3.z = -13.0;
		viewPt.x = dist;
		viewPt.y = 15.0;
		viewPt.z = dist * 50.0 / 1000.0 - 50.0;
		if (viewPt.z > -13.0)
			viewPt.z = -13.0;
		up = viewPt;
		up.z = up.z - 1.0;
		if (viewPt.z < -14.0)
			up.y = up.y + 0.1 * sin(p);
		p = p + 0.03;
		dist = dist + v1;
		if (dist > 10000.0)
			exit (1);

		migLoc1.x = 0.0 + 2200.0 * cos (a);
		migLoc1.y = -800.0 + 2200.0 * sin (a);
		migLoc2.x = 0.0 + 2225.0 * cos (a-0.04);
		migLoc2.y = -800.0 + 2225.0 * sin (a-0.04);
		a = a + migV;

/*
 *  Now create a vector containing all polygons from the objects.
 */

	for (i=0; i<obj->numPolys; ++i) {
		if (poly[i] != (VPolygon *) NULL)
			VDestroyPolygon(poly[i]);
		poly[i] = VCopyPolygon(obj->polygon[i]);
	}
	cnt = obj->numPolys;
#ifndef VIEW3
	placeObject (ftr, ftrLoc, 0.0, 0.0, 90.0*3.14/180.0, &poly[0], &cnt);
#endif
	placeObject (ftr, ftrLoc2, 0.0, 0.0, -3.0*3.14/180.0, &poly[0], &cnt);
	placeObject (ftr, ftrLoc3, 0.0, -11.0*3.14/180.0, 0.0, &poly[0], &cnt);
#ifndef VIEW1
	placeObject (ftr, viewPt, 0.0, -11.0*3.14/180.0, 0.0, &poly[0], &cnt);
#endif
	placeObject (mig, migLoc1, migRoll, 0.0, a+90.0*3.14/180.0, &poly[0], &cnt);
#ifndef VIEW2
	placeObject (mig, migLoc2, migRoll, 0.0, a+90.0*3.14/180.0, &poly[0], &cnt);
#endif

/*
 *  Calculate eye space information based on our current viewpoint.
 */

#ifdef VIEW1
	VGetEyeSpace (v, viewPt, centerInt, up);
#endif
#ifdef VIEW2
	up = migLoc2;
	up.z = up.z - 1.0;
	VGetEyeSpace (v, migLoc2, viewPt, up);
#endif
#ifdef VIEW3
	up = ftrLoc;
	up.z = up.z - 1.0;
	VGetEyeSpace (v, ftrLoc, viewPt, up);
#endif

	black = BlackPixel(dpy, 0);


/*
 *  First clip, then draw each polygon.
 */

	for (i=0; i<cnt; ++i) {

		if (mono)
			XSetForeground (dpy, curGC, black);

		VTransformPolygon (poly[i], &(v->eyeSpace));
		poly[i] = VClipPolygon(poly[i], v->clipPoly);
		if (poly[i]) {
		    if (mono == 0 && curPixel != (j=v->pixel[poly[i]->color->index])) {
			XSetForeground (dpy, curGC, j);
			curPixel = j;
		    }
		    if (mono)
		        VDrawPolygon (v, win, curGC, poly[i]);
		    else
		        VFillPolygon (v, win, curGC, poly[i]);
		}

	}

	VExposeBuffer (v, curGC);

/*
 * Erase the un-displayed planes.
 */

	if (mono == 0) {
		curPixel = *(v->pixel);
        	XSetForeground (dpy, curGC, curPixel);
		if (usePixmaps == 1)
        	XFillRectangle (dpy, v->monoPixmap, curGC, 0, 0, xsh.width, xsh.height);
		else
        	XFillRectangle (dpy, win, curGC, 0, 0, xsh.width, xsh.height);
	}

        }

}
