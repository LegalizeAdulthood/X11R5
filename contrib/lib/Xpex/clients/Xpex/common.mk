/* $Header: common.mk,v 1.1 91/09/13 12:59:32 sinyaw Exp $ */

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

#ifdef UseInstalled
#define  PexLibInc -I$(INCROOT)/X11/Xpex
#define	 PexLib -L$(USRLIBDIR) -lXpex
#define  DepLibs
#else
     PEXLIB_DIR = $(EXTENSIONSRC)/lib/PEX/Xpex/lib
#define  PexLibInc -I$(EXTENSIONSRC)/lib/PEX/Xpex/include
#define	 PexLib $(PEXLIB_DIR)/libXpex.a
#define  DepLibs $(PEXLIB)
#endif
       INCLUDES = PexLibInc \
                  -I$(BUILDINCROOT)/X11/extensions \
		  -I$(EXTENSIONSRC)/include/PEX
         PEXLIB = PexLib
