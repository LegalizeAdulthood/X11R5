/* $Header: XpexMiscAttrs.c,v 2.3 91/09/11 16:07:04 sinyaw Exp $ */
/* 
 *	XpexMiscAttrs.c 
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

#include "Xpexlibint.h"
#include "Xpexlib.h"
#include <X11/extensions/Xext.h>
#include "extutil.h"
#include "Xpextutil.h"

void
XpexSetPickId(dpy, pickId)
	Display *dpy;
	long pickId;
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);
	XpexCheckLastReq(dpy,i);

	LockDisplay(dpy);
	{
		pexPickId *oc;
		XpexGetOutputCmd(PickId,dpy,i,oc);
		oc->pickId = (CARD32) pickId;
	}
	UnlockDisplay(dpy);
}

void
XpexSetHlhsrId(dpy, hlhsrID)
	Display *dpy;
	long hlhsrID;
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);
	XpexCheckLastReq(dpy,i);

	LockDisplay(dpy);
	{
		pexHlhsrIdentifier *oc;
		XpexGetOutputCmd(HlhsrIdentifier,dpy,i,oc);
		oc->hlhsrID = (CARD32) hlhsrID;
	}
	UnlockDisplay(dpy);
}

void
XpexAddToNameSet(dpy, names, count)
	Display *dpy;
	XpexName *names;
	int count;
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);
	XpexCheckLastReq(dpy,i);

	LockDisplay(dpy);
	{
		pexAddToNameSet *oc;
		int extra = count * sizeof(pexName);
		XpexGetOutputCmdExtra(AddToNameSet,dpy,i,extra,oc);
		bcopy((char *)names,(char *)(oc + 1),extra);
	}
	UnlockDisplay(dpy);
}

void
XpexRemoveFromNameSet(dpy, names, count)
	Display *dpy;
	XpexName *names;
	int count;
{
	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);
	XpexCheckLastReq(dpy,i);

	LockDisplay(dpy);
	{
		pexRemoveFromNameSet *oc;
		int extra = count * sizeof(pexName);
		XpexGetOutputCmdExtra(AddToNameSet,dpy,i,extra,oc);
		bcopy((char *)names,(char *)(oc + 1),extra);
	}
	UnlockDisplay(dpy);
}
