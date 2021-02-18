/* $Header: eType.c,v 2.2 91/09/11 15:52:10 sinyaw Exp $ */
#include <stdio.h>
#include <X11/X.h>
#include "PEX.h"
#include "PEXprotost.h"
#include "PEXproto.h"

void
pexpr_edge_type( str, type)
	char *str;
	pexEnumTypeIndex type;
{
#define VALID_EDGE_TYPE(_M) \
(_M >= PEXSurfaceEdgeSolid && _M <= PEXSurfaceEdgeDashDot)

	static char *edge_type[5] = {
		NULL,
		"Solid",
		"Dashed",
		"Dotted",
		"DashDot"
	};

	if( VALID_EDGE_TYPE(type)) {
		(void) fprintf( stderr, "%s: %s\n", str, edge_type[type]);
	} else {
		(void) fprintf( stderr, "%s: Invalid\n", str);
	}
#undef VALID_EDGE_TYPE
}
