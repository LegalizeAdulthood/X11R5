/*
 *        %Z%%M% %I% %E%  
 * 
 * Derived from @(#)XsfSun.h	1.0 91/08/13 FUJITSU LIMITED. 
 */
/****************************************************************************

              Copyright 1991, by FUJITSU LIMITED
              Copyright 1991, by Sun Microsystems, Inc.	      

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of FUJITSU LIMITED and Sun
Microsystems, Inc. not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.  FUJITSU LIMITED and Sun Microsystems, Inc. makes no
representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.

FUJITSU LIMITED AND SUN MICROSYSTEMS, INC. DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL FUJITSU LIMITED AND SUN
MICROSYSTEMS, INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Auther:	Riki	Kawaguchi	FUJITSU LIMITED
	Tomoki	Kuriki		FUJITSU LIMITED
	Hideki  Hiura           (hhiura@Sun.COM) Sun Microsystems, Inc.
	
****************************************************************************/
/* XsfSun : Input Server Sun IMlogic Interface			 */
/* : Version 1.0					 */

#ifndef _XSFSUN_H_
#define _XSFSUN_H_

#ifdef	XSF_X11R5
#include	"XSunIMProt.h"
#endif				/* XSF_X11R5 */

/* IMLogic operand Linked QUE Table Format */
typedef struct _XSFrrvCell {
    struct _XSFrrvCell *nlink;
    iml_inst       *called_rv;
    iml_inst       *rv;
}               XSFrrvCell;

#endif				/* _XSFSUN_H_	 */
