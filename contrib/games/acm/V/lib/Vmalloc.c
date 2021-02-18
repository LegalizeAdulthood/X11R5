#include "Vlib.h"

extern char * malloc();

char *Vmalloc(size)
int	size; {

	char	*p;

	if ((p = malloc(size)) == (char *) NULL) {
		fprintf (stderr, "V package memory allocation error.\n");
		fprintf (stderr, "An error was encountered allocating\
 %d bytes.\n", size);
		exit (1);
	}
	return p;
}
