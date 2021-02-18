/*
 *     (C) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *     pending in the U.S. and foreign countries. See LEGAL NOTICE
 *     file for terms of the license.
 * 
 *     @(#)iml_mman.h
 */

#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunIMMMan.h 1.3 91/07/15";
#endif
#endif
#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunIMMMan.h 1.3 91/07/15";
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

#ifndef xim_imlogic_mman_DEFIND
#define xim_imlogic_mman_DEFIND

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

char * memalign();
char * calloc();

typedef enum 
{
    RELEASE_SLOT = 0 ,
    ALLOC_SLOT = 1 
} slot_op_t;

typedef struct _iml_inst_slot {
    struct _iml_inst_slot *next ;
    enum { FREE, BUSY } busy ; 
    int size ;
    char *ptr ;
} iml_inst_slot_t;

#define IML_MAX_SLOT_SIZE 65535
#define IML_MIN_SLOT_SIZE 128

#endif  xim_imlogic_mman_DEFINED
