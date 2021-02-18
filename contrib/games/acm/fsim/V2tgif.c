#include <stdio.h>
#include <Vlib.h>

#define INCHES	128

int	horg = 4 * INCHES;
int	vorg = 5 * INCHES;
double	scale = ((double) INCHES / 4.0);	/* 4 feet to the inch */

#define	XAXIS	1
#define YAXIS	2
#define ZAXIS	3
int	axis = XAXIS;

extern char * optarg;

main (argc, argv)
int	argc;
char 	*argv[]; {

	VObject	*object;
	char	*name;
	FILE	*f;
	int	c, i, j, k, n;
	int	v, h;
	VPoint	*q, tmp;
	VMatrix	 mtx;
	VPolygon **p;

	while ((c = getopt (argc, argv, "f:xyz")) != EOF) {

		switch (c) {

		case 'f':
			name = optarg;
			break;

		case 'x':
			axis = XAXIS;
			break;

		case 'y':
			axis = YAXIS;
			break;

		case 'z':
			axis = ZAXIS;
			break;
		}
	}

	f = fopen (name, "r");
	object = VReadObject(f);
	fclose (f);

	printf ("state(0,6,0,0,0,16,1,4,1,1,0,0,1,0,1,0,1,0,4).\n");

	n = object->numPolys;
	p = object->polygon;
	for (i=0; i<n; ++i) {
		printf ("polygon(yellow,%d,[", p[i]->numVtces+1);
		for ((k=0, q=p[i]->vertex); k<p[i]->numVtces; (++k, ++q)) {
			transform (q, &v, &h);
			printf ("%d,%d,", v, h);	
		}
		transform (p[i]->vertex, &v, &h);
		printf ("%d,%d],0,0,1,0).\n", v, h);
		++j;
	}

	exit (0);
}

transform (p, y, x)
VPoint	*p;
int	*y, *x; {

	double	dx, dy;

	if (axis == XAXIS) {
		dx = p->y;
		dy = p->z;
	}
	else if (axis == YAXIS) {
		dx = p->x;
		dy = p->z;
	}
	else if (axis == ZAXIS) {
		dx = p->x;
		dy = p->y;
	}

	*x = (int) (dx * scale + 0.5) + vorg;
	*y = (int) (dy * scale + 0.5) + horg;
}
	
