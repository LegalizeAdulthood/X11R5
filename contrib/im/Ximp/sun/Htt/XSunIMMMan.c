#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunIMMMan.c 1.2 91/07/15";
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

#include <stdio.h>
#include <euc.h>
#include <widec.h>
#include <locale.h>
#include <sys/types.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <pwd.h>
#include <grp.h>
#include "XSunExt.h"
#include "XSunIMProt.h"
#include "XSunIMMMan.h"
#include "XSunIMPriv.h"
#include "XSunIMCore.h"
#include "XSunIMPub.h"


Private void *slot_manager();

/*
 * IMLogic_session link terminal
 */

Private char *
im_memalign(align, size)
int align ;
int size  ;
{
	caddr_t p ;
	
	p = (caddr_t)malloc(size + align);
	p = (caddr_t)((((long)p+align-1)/align)*align) ;
	return((char*)p) ;
}

Public void *
_iml_new(session, size)
iml_session_t *session ;
int size ; 
{
    /* to avoid memory trash bug temporary */
    size = (((size + SLOTMINSIZE)/SLOTMINSIZE)*SLOTMINSIZE)  ;
    return(slot_manager(0, ALLOC_SLOT, session, size));
}
Public void *
_iml_new2(session, size)
iml_session_t *session ;
int size ; 
{
    /* to avoid memory trash bug temporary */
    size = (((size + SLOTMINSIZE)/SLOTMINSIZE)*SLOTMINSIZE)  ;
    return(slot_manager(1, ALLOC_SLOT, session, size));
}
Public void *
_iml_delete(session)
iml_session_t *session ;
{
    return(slot_manager(0, RELEASE_SLOT, session, 0));
}

Public void *
_iml_delete2(session)
iml_session_t *session ;
{
    return(slot_manager(1, RELEASE_SLOT, session, 0));
}

Private iml_inst_slot_t *free_slot_q1 ; /* general */
Private iml_inst_slot_t *free_slot_q2 ; /* lookup */

Private void *
slot_manager(luc, action, session, size)
int luc ;
slot_op_t action ;
register iml_session_t *session ;
int size ; /* available only in ALLOC_SLOT */
{
    iml_inst_slot_t *free_slot ;
    iml_inst_slot_t *new, *p ;
    register iml_inst_slot_t *next_slot ;
    int link_depth = 0;

    if(luc){
	next_slot = session->long_term_slot ;
	free_slot = free_slot_q2 ;
    } else {
	next_slot = session->short_term_slot ;    
	free_slot = free_slot_q1 ;
    }
    switch(action){
      case ALLOC_SLOT:
	if(free_slot){
	    p = new = free_slot ;
	    while(new && new->size < size ){
		_iml_slot_trace("search", session, new, new->ptr,
			       new->size, link_depth);
		p = new ;
		new = new->next ;
		link_depth++ ;
	    }
	    if (!new) {
		goto allocate_new ;
	    } else if (p != new ) {
		p->next = new->next ;
	    } else /* p == new means got top of free_slot */ {
		if (free_slot->next) {
		    free_slot = free_slot->next ;
		} else {
		    free_slot = 0 ;
		}
	    }
	    new->next = NULL ;
	} else {
allocate_new:
	    link_depth = -1 ;
	    new = (iml_inst_slot_t *)im_memalign( sizeof(iml_inst_slot_t *),sizeof(iml_inst_slot_t));
	    new->size = size ;
	    new->ptr = im_memalign(max(sizeof(int),sizeof(wchar_t)), size);
	    new->next = 0 ;
	}
    	new->busy = BUSY ;
	if(next_slot) {
	    while(next_slot->next){
		next_slot = next_slot->next ;
	    }
	    next_slot->next = new ;
	} else {
	    if(luc){
		next_slot = session->long_term_slot = new ;
	    } else {
		next_slot = session->short_term_slot = new ;
	    }
	}
	bzero(new->ptr, new->size); 
	_iml_slot_trace("alloc", session, new, new->ptr, new->size, link_depth);
	if(luc){
	    free_slot_q2 = free_slot ;
	} else {
	    free_slot_q1 = free_slot ;
	}
	return((void *)new->ptr) ;
	break ;

      case RELEASE_SLOT:
	if(free_slot){ 
	    p = free_slot ;
	    while(p && p->next){
		p = p->next ;
	    }
	    p->next = next_slot ;
	} else {
	    p=free_slot = next_slot ;
	}
	if(luc){
	    session->long_term_slot = NULL ;
	} else {
	    session->short_term_slot = NULL ;
	}
	while(p && p->next){
	    p = p->next ;
	    p->busy = FREE ;
	    _iml_slot_trace("release", session, p, p->ptr,
			   p->size, link_depth);
	    link_depth ++ ;
	}
	break ;
    }
    if(luc){
	free_slot_q2 = free_slot ;
    } else {
	free_slot_q1 = free_slot ;
    }
    return((void *)0);
}
