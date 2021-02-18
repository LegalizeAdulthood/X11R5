#include "Vlib.h"
#include <string.h>

VColor *VAllocColor (name)
char *name; {

	VColor	*p=VColorList, *prev=0, **q;

/*
 *  Search for this color among those already allocated.
 */

	while (p != (VColor *) 0) {
		if (strcmp(p->color_name, name) == 0) {
			return p;
		}
		prev = p;
		p = p->next;
	}

/*
 *  The color was not in the list; allocate a new list element.
 */

	if (prev == (VColor *)0)
		q = &VColorList;
	else
		q = &(prev->next);

	*q = (VColor *) Vmalloc (sizeof(VColor));
	(*q)->color_name = strdup (name);
	(*q)->index = -1;
	(*q)->next = 0;

	return *q;
}
