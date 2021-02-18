/* $Header: XpexFree.c,v 2.2 91/09/11 15:51:32 sinyaw Exp $ */
/*
 *	XpexFree.c
 */

/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

#include "Xpexlib.h"

void
XpexFreeEnumTypeInfo(format, info_list, count)
	XpexBitmask format;
	XpexListOfEnumTypeDesc *info_list;
	int count;
{
	register int i, j;

	switch ((int) format) {
	case 0x0:
		XFree((char *)info_list);
		break;
	case PEXETIndex:
		for (i = 0; i < count; i++) {
			XFree((char *)info_list[i].desc);
		}
		XFree((char *)info_list);
		break;
	case PEXETMnemonic:
		for (i = 0; i < count; i++) {
			for (j = 0; j < info_list[i].count; j++) {
				XFree((char *)info_list[i].desc[j].name);
			}
			XFree((char *)info_list[i].desc);
		}
		XFree((char *)info_list);
		break;
	case PEXETBoth:
		for (i = 0; i < count; i++) {
			for (j = 0; j < info_list[i].count; j++) {
				XFree((char *)info_list[i].desc[j].name);
			}
			XFree((char *)info_list[i].desc);
		}
		XFree((char *)info_list);
		break;
	default:
		break;
	}
}
