#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunIMConf.c 1.4 91/08/10";
#endif
#endif

/******************************************************************

              Copyright 1990, 1991, by Sun Microsystems, Inc.

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sun Microsystems, Inc.
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.
Sun Microsystems, Inc. makes no representations about the suitability of
this software for any purpose.  It is provided "as is" without
express or implied warranty.

Sun Microsystems Inc. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL Sun Microsystems, Inc. BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

  Author: Hideki Hiura (hhiura@Sun.COM)
	  				     Sun Microsystems, Inc.
******************************************************************/

#include <stddef.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "XSunExt.h"
#include "XSunIMProt.h"
#include "XSunIMMMan.h"
#include "XSunIMPriv.h"
#include "XSunIMCore.h"
#include "XSunIMPub.h"

#define VERSION "X11R5 contrib version 0.1"
#define VERSION_NOTIFY

#define JLE103


typedef struct _iml_le_sw {
    int (*iml_constructor)();
} iml_le_sw_t ;

#ifdef JLE103
extern int     _iml_jle103_constructor() ;
#endif
#ifdef WNN403    
extern int    _iml_wnn403_constructor() ;
#endif
#ifdef CIO
extern int    _iml_cio_constructor() ;
#endif

static iml_le_sw_t iml_le_sw[]={
#ifdef JLE103
    _iml_jle103_constructor,
#endif
#ifdef WNN403
    _iml_wnn403_constructor,
#endif
#ifdef CIO
    _iml_cio_constructor,
#endif

};

#define HOW_MANY_LES   (sizeof(iml_le_sw) / sizeof(iml_le_sw_t))
Public int
_iml_configure(id, rv)
int id ;
void **rv;  /* actually it's iml_inst **rv, but this layer does not to know */
{
    if(id < 0 || id >= HOW_MANY_LES)
      return(-1);
    else {
#ifdef VERSION_NOTIFY
	static is_first_time = True ;

	if(is_first_time){
	    printf("SunXIM %s\n", VERSION);
	    printf("Attached language engine : ");
	    is_first_time = False ;
	}
#endif
    }
    return((int)(* iml_le_sw[id].iml_constructor)(id, rv));
}

