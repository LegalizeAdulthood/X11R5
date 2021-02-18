/* $Header: dbpex.c,v 2.2 91/09/11 15:52:07 sinyaw Exp $ */
#include <stdio.h>

/* HlhsrMode */

static char *pex_hlhsr_mode[] = {
    NULL,
    "Off",
    "ZBuffer",
    "Painters",
    "Scanline",
    "HiddenLineOnly" 
};

void dbpexHlhsrMode();

void
dbpexHlhsrMode(index)
unsigned short index;
{
    (void) fprintf( stderr, 
        "mode: %s\n", pex_hlhsr_mode[index] );
}
