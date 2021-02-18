/*
 *  ibuild : build a fast linear interpolation table
 *
 *  ibuild is designed to read in a list of x and f(x) values and then build
 *  a table that can be used to interpolate f(x) values more quickly.  It does
 *  this by precomputing the equation of the line associated with each
 *  interval in the table.  These equations are output as C initialized
 *  structures that can be passed to interpolate() for processing.
 *
 *  Author:  Riley Rainey  riley@mips.com
 *
 */

#include <stdio.h>
#include <interpolate.h>

main (argc, argv)
int	argc;
char	*argv[]; {

	float	x[256], y[256];
	int	count, i;
	ITable	table;
	IEntry	entry[256];
	FILE	*f;

	if ((f = fopen(argv[1], "r")) == (FILE *) NULL) {
		perror ("can't open input file");
		exit (1);
	}

	fscanf (f, "%d", &count);

	fscanf (f, "%f%f", &x[0], &y[0]);
	table.minX = x[0];
	table.count = count-1;

	printf ("\n/* This interpolation table was built by ibuild */\n\n");
	printf ("static IEntry %s_e[] = {\n", argv[1]);

	for (i=1; i<count; ++i) {
		fscanf (f, "%f%f", &x[i], &y[i]);
		entry[i-1].m = (y[i] - y[i-1]) / (x[i] - x[i-1]);
		entry[i-1].b = y[i] - (x[i] * entry[i-1].m);
		printf ("\t{\t%g,\t%g,\t%g },\n", (double) x[i],
			(double) entry[i-1].m, (double) entry[i-1].b);
	}

	printf ("\t};\n");
	printf ("static ITable %s = { %d, %g, %s_e };\n", argv[1],
		table.count, (double) table.minX, argv[1]);

	exit (0);
}
