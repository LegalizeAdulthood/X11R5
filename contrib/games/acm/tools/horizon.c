#include <stdio.h>
#include <math.h>
#include <Vlib.h>

#define DEGtoRAD(a)	(a * M_PI / 180.0)

#define P_i	DEGtoRAD(5.0)
#define P_o	DEGtoRAD(15.0)
#define P_ha	DEGtoRAD(20.0)

main () {

	double	c, cp2, sp2, dpitch, pitch;
	double	xi, yi, xo, yo;
	VPoint	inner, inner_p;
	VMatrix	m1;

	inner.x = cos (P_i / 2.0);
	inner.y = sin (P_i / 2.0);
	inner.z = 0.0;

	for (dpitch=0.0; dpitch < 81.0; dpitch += 10.0) {

		pitch = DEGtoRAD (dpitch);

		VIdentMatrix (&m1);
		VRotate (&m1, YRotation, pitch);
		VTransform (&inner, &m1, &inner_p);

		xi = inner_p.x;
		yi = inner_p.y;

		c = cos((P_o - P_i) / 2.0);
		cp2 = cos (pitch);
		cp2 = cp2 * cp2;
		sp2 = sin (pitch);
		sp2 = sp2 * sp2;

		xo = (xi * c - yi * sqrt (xi * xi * cp2 + yi * yi * cp2 -
			c * c + 2.0 * c * sp2 - sp2 * sp2) - xi * sp2) /
			(xi * xi + yi * yi);

		yo = sqrt (cp2 - (xo * xo));

		printf ("%f\t%f\t%f\t%f\t", dpitch, xi, yi, inner_p.z);
		printf ("%f\t%f\t%f\n\n", xo, yo, sin(pitch));
	}

	exit (0);

}
