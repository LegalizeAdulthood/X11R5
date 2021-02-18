#include <stdio.h>

main () {

	int	pts, i, j, poly, tmp;
	int	count, idx;
	float	x, y, z;
	double	xd, yd, zd;

	scanf ("%d %d %d", &pts, &poly, &tmp);
	printf ("%d %d\n", pts, poly);

	for (i=0; i<pts; ++i) {
		scanf ("%g %g %g", &x, &y, &z);
		xd = y;
		yd = x;
		zd = -z;
		printf ("%d %g %g %g\n", i+1, xd, yd, zd);
	}

	for (i=0; i<poly; ++i) {
		scanf ("%d", &count);
		printf ("gray33 %d", count);
		for (j=0; j<count; ++j) {
			scanf ("%d", &idx);
			printf (" %d", idx);
		}
		printf ("\n");
	}

	exit (0);
}
