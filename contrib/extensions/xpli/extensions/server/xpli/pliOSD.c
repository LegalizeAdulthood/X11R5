/***********************************************************
		Copyright IBM Corporation 1987,1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliOSD.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliOSD.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliOSD.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
#endif

#define ARBITRARY_NUMBER  100
#ifdef BSDrt
#include <sys/file.h>
#include <machineons/buf_emul.h>
#else
#include <sys/ioctl.h>
#include <fcntl.h>
#endif ~BSDrt
#include "X.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"
#include "OScompiler.h"
#include "pliScreen.h"
#include "pliVideo.h"
#include "pliSave.h"

int
pliProbe()
{
	return ARBITRARY_NUMBER;
}
static struct pli_video_hardware_state VS_Start;
pliScreenInitHW( index )
register int index ;
{
	pli_set_graphics_mode(&VS_Start);
	return 1 ; /* SUCCESS !! */
}

void
pliCloseHW( index )
register int index ;
{
return ;
}
