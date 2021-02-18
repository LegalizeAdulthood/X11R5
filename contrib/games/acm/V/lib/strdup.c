#include <string.h>
#include "Vlib.h"

#ifndef SYSV

char *strdup(string)
char *string;
{
     return(strcpy(Vmalloc(sizeof(char) * (strlen(string) + 1)),
		   string));
}

#endif
