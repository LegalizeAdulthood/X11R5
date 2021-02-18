#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunIMSubr.c 1.28 91/09/29";
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
#include <stdlib.h>
#include <euc.h>
#include <widec.h>
#include <ctype.h>
#include <locale.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <mle/cm.h>
#include <mle/env.h>
#include <pwd.h>
#include <grp.h>
#include "XSunExt.h"
#include "XSunIMProt.h"
#include "XSunIMPriv.h"
#include "XSunIMMMan.h"
#include "XSunIMCore.h"
#include "XSunIMPub.h"

#define IMSERVER 
#define NO_PREEDIT_RESULTS

#ifdef IMLTRACE
#define SPECIAL_MALLOC 1
#define DEBUG 1
Private int imlogic_trace = 0x40 ;
#endif

/*
 * IMLogic_session link terminal
 */
Private iml_session_t *imlogic_session = 0 ;

Private void iml_generic_close() ;
Private void iml_generic_reset() ;
Private void iml_generic_get_status() ;
Private void iml_generic_get_key_map() ;
Private void iml_generic_set_status() ;
Private void iml_generic_set_key_map() ;
Private int  iml_generic_send_event() ;
Private void iml_generic_set_keyboard_state() ;
Private void iml_generic_conversion_on() ;
Private void iml_generic_send_results() ;

Private iml_session_t * iml_init_session();

/*
 * generic constructor of imlogic session struct
 */

Public int
_iml_construct_session(id, rrv, session_size, private_constructor)
    int id ;
    iml_inst **rrv;
    int session_size ;
    int (*private_constructor)();
{
    iml_session_t *s ;
    iml_session_t *ss = NULL ;
    int is_recycled = 0;
    iml_inst **rv ;
    int session_id ;

    s = iml_init_session(session_size);
    if(s->next != (void *)-1){
	while(s){
	    if(s->session_id == IML_SESSION_RECYCLE &&
	       s->language_engine == id) {
		is_recycled ++; 
		break;
	    }
	    ss = s ;
	    s = ss->next ;
	}
	if(!s){
	    s = (iml_session_t *)calloc(1,session_size);
	}
	if(ss)
	  ss->next = s ;
    }
    s->classname = 0 ;
    s->enginename = 0 ;
    s->locale = 0 ;
    s->session_id = 0 ;
    s->public_status = 0 ;
    s->initial_public_status = 0 ;
    s->private_status = 0 ;
    s->remainder = 0 ;
    s->rrv = 0 ;
    s->short_term_slot = 0 ;
    s->long_term_slot = 0 ;
    s->active_regions = 0 ;
    s->event = 0 ;
    s->keysym = 0 ;
    s->status= 0 ;
    s->WhoIsMaster = HasNotBeenNegotiated ;
    s->ld = 0 ;
    s->current_region.region = -1 ;
    s->current_region.next = 0 ;
    s->current_region.prev = 0 ;
    if(!is_recycled){
	s->next = NULL;
	s->StatusStringRemainBuffer.buf = (wchar_t *)
	  calloc(sizeof(wchar_t), DEFAULTStatusStringRemainBufferSize);
	s->StatusStringRemainBuffer.size = DEFAULTStatusStringRemainBufferSize;
	s->XLookupBuf = calloc(sizeof(char), DEFAULTXLookupStringBufferSize) ;
	s->XLookupBuflen = DEFAULTXLookupStringBufferSize ;
    }
    s->iml_open 		= (void *) NULL ; /* This should be private */
    s->iml_close 		= iml_generic_close ;
    s->iml_reset 		= iml_generic_reset ;
    s->iml_get_status 		= iml_generic_get_status ;
    s->iml_get_key_map 		= iml_generic_get_key_map ;
    s->iml_set_status 		= iml_generic_set_status ;
    s->iml_set_key_map 		= iml_generic_set_key_map ;
    s->iml_send_event 		= iml_generic_send_event ;
    s->iml_set_keyboard_state 	= iml_generic_set_keyboard_state ;
    s->iml_conversion_on 	= iml_generic_conversion_on ;
    s->iml_send_results 	= iml_generic_send_results ;
    
    session_id = (*private_constructor)(id, rrv, s, is_recycled);
    if(session_id < 0 ){
	if(ss)
	  ss->next = NULL ;
	free(s->StatusStringRemainBuffer.buf);
	free(s) ;
    } else if (!is_recycled){
#ifdef notdef /* before post public review */
	s->PreEditTextInfo.text.string.wide_char = (wchar_t *)
	  calloc(sizeof(wchar_t),DEFAULTPreEditTextBufferSize);
	s->PreEditTextInfo.text.feedback = (XIMFeedback *)
	  calloc(sizeof(XIMFeedback),DEFAULTPreEditAttrBufferSize);
#else         /* after post public review */
	s->PreEditTextInfo.text = (XIMText *)calloc(sizeof(XIMText), 1);
	s->PreEditTextInfo.text->string.wide_char = (wchar_t *)
	  calloc(sizeof(wchar_t),DEFAULTPreEditTextBufferSize);	
	s->PreEditTextInfo.text->feedback = (XIMFeedback *)
	  calloc(sizeof(XIMFeedback),DEFAULTPreEditAttrBufferSize);
#endif
        s->PreEditTextBufferSize = DEFAULTPreEditTextBufferSize ;    
	s->PreEditAttrBufferSize = DEFAULTPreEditAttrBufferSize ;
    }
    return session_id ;
}

/*
 * generic destructor of imlogic session struct
 */

Public void
_iml_destruct_session(s, rrv, private_destructor)
    iml_session_t *s ;
    iml_inst **rrv;
    int (*private_destructor)();
{
    _iml_fake_nop_inst(s, rrv);
    (*private_destructor)(s, rrv);
    _iml_delete(s); /* caution!! already released */
    _iml_delete2(s);
    s->session_id = IML_SESSION_RECYCLE ;
}

Public iml_session_t *
_iml_get_session(session_id)
int session_id ;
{
    register iml_session_t *session ;

    session = imlogic_session ;
    if(!session){
	return((void *)-1);
    }
    while(session){
	if(session->session_id == session_id){
	    return(session);
	} else if (session->next){
	    session = session->next;
	} else {
	    return((void *)-1);
	}
    }
    
}

/*
 * Japanese only
 * generate string into b (max len l ) from keysym
 */
Public int
_iml_lookupKanaString(b, l, keysym)
char	*b ; 
int	 l ;
KeySym   keysym ;
{
    int		index;

    return 0; /* temporary */
}


/*
 * 
 */ 

Private iml_session_t *
iml_init_session(session_size)
int session_size ;
{
    if(imlogic_session) return imlogic_session;
    imlogic_session=(iml_session_t *)calloc(1,session_size);
    imlogic_session->next = (void *)-1 ;
    return(imlogic_session);
}

/*
 * 第一引数のリンクリストの最後に第二引数を連結する
 */
Public iml_inst *
_iml_link_inst_tail(rrv, lp)
iml_inst **rrv ;
iml_inst  *lp ;
{
    register iml_inst *clp = *rrv ;
    
    if(!(*rrv)){
	*rrv = lp ;
    } else {
	if(clp){
	    while(clp->next){
		clp = clp->next ;
	    }
	    clp->next = lp ;
	} else {
	    clp = lp ;
	}
    }
    lp->next = NULL ;
    return *rrv ;
}
/*
 * public_status を set する iml_inst を作る
 */
Public iml_inst *
_iml_make_imm_set_status_inst(s)
iml_session_t *s ;
{
    iml_status_t *status ;
    iml_inst *lp ;
    
    lp=(iml_inst*)_iml_new(s, sizeof(iml_inst) + sizeof(iml_status_t));
    lp->opcode = IMM_SET_STATUS ;
    lp->next = NULL ;
    status =  (iml_status_t *) &(lp->operand);
    *status = s->public_status ;
    _imm_trace("IMM_SET_STATUS",*status,0);
    return lp ;
}
Public iml_inst *
_iml_make_preedit_start_inst(s)
iml_session_t *s;
{
    iml_inst *lp ;

    lp = (iml_inst*)_iml_new(s, sizeof(iml_inst)+sizeof(int));
    _imm_trace("IMM_PREEDIT_START",0,0);
    _iml_put_current_region(s, PREEDIT) ;
    lp->opcode = IMM_PREEDIT_START | IMM_CB_RESULT_REQUIRED;
    lp->next = NULL ;
    lp->size_of_operand = 0 ;
    return lp ;
}

Public iml_inst *
_iml_make_preedit_erase_inst(s)
iml_session_t *s;
{
    XIMPreeditDrawCallbackStruct *p ;
    iml_inst *lp;

    lp=(iml_inst*)_iml_new(s,sizeof(iml_inst)+sizeof(XIMPreeditDrawCallbackStruct));
    lp->opcode = IMM_PREEDIT_DRAW ;
    lp->next = NULL ;
    p = (XIMPreeditDrawCallbackStruct*)&(lp->operand);
    p->chg_first = 0 ;
#ifndef before_public_review_xim_compat
#ifdef nelson_no_tako
    p->text = (XIMText *)_iml_new(s, sizeof(XIMText));
    p->text->length = 0;
    p->text->feedback = (XIMFeedback *)NULL; 
    p->text->string.wide_char = (wchar_t *)NULL;
    p->text->encoding_is_wchar = True ;
    s->PreEditTextInfo.text->length =  p->text->length ; 
    p->chg_length = s->PreEditTextInfo.text->length ; 
#else
    p->text = NULL ;
    p->chg_length = s->PreEditTextInfo.text->length ; /* previous length */
    s->PreEditTextInfo.text->length =  0 ;
#endif
#else 
    p->chg_length = s->PreEditTextInfo.text.length ; 
    p->text.length = 0;
    p->text.feedback = (XIMFeedback *)NULL; 
    p->text.string.wide_char = (wchar_t *)NULL;
    p->text.encoding_is_wchar = True ;
    p->text.visible_pos = 0;
    s->PreEditTextInfo.text.length =  p->text.length ;
#endif 
    p->caret = 0 ;    
    _imm_trace("IMM_PREEDIT_DRAW(pseudo:erase)",0,p->chg_length);
    return lp ;
}

Public iml_inst *
_iml_make_preedit_done_inst(s)
iml_session_t *s;
{
    iml_inst *lp ;

    lp = (iml_inst*)_iml_new(s, sizeof(iml_inst));
    lp->opcode = IMM_PREEDIT_DONE ;
    lp->next = NULL ;
    lp->size_of_operand = 0 ;
    _imm_trace("IMM_PREEDIT_DONE(pseudo)",0,p->chg_length);
    _iml_pop_current_region(s);
    return lp ;
}
Public iml_inst *
_iml_make_nop_inst(s)
iml_session_t *s;
{
    iml_inst *lp ;

    lp = (iml_inst*)_iml_new(s, sizeof(iml_inst));
    lp->opcode = IMM_NOP ;
    lp->next = NULL ;
    lp->size_of_operand = 0 ;
    _imm_trace("IMM_NOP",0,0);
    return lp ;
}
Public iml_inst *
_iml_make_lookup_start_inst(s)
iml_session_t *s;
{
    iml_inst *lp ;
    XIMLookupStartCallbackStruct *ls;

    lp=(iml_inst *)_iml_new(s,sizeof(iml_inst)+
			    sizeof(XIMLookupStartCallbackStruct));	    
    lp->opcode = IMM_LOOKUP_START | IMM_CB_RESULT_REQUIRED;
    lp->next = NULL ;
    _iml_put_current_region(s, LOOKUP) ;
    ls = (XIMLookupStartCallbackStruct *)&lp->operand ;
    ls->event = s->event ;
    ls->WhoIsMaster = s->WhoIsMaster ;
    ls->XIMPreference = &s->XIMPreference ;
    ls->XIMPreference->WhoOwnsLabel = CBOwnsLabel ;
    ls->CBPreference = &s->CBPreference ;
    _imm_trace("IMM_SET_REGION(LOOKUP)",0,0);
    return lp ;
}
Public iml_inst *
_iml_make_lookup_process_inst(s, results_required, controll)
iml_session_t *s;
Bool results_required ;
int controll ; /* commit/next/prev: NOT IMPLEMENTED YET */
{
    iml_inst *lp ;

    if(s->WhoIsMaster == CBIsMaster){
	XIMLookupProcessCallbackStruct *lup ;
	lp = (iml_inst *)_iml_new(s,
		  sizeof(XIMLookupProcessCallbackStruct) + sizeof(iml_inst) );
	if(results_required){
	    lp->opcode = IMM_LOOKUP_PROCESS | IMM_CB_RESULT_REQUIRED;
	} else {
	    lp->opcode = IMM_LOOKUP_PROCESS ;
	}	    
	lp->next = NULL ;
	lup = (XIMLookupProcessCallbackStruct *)&lp->operand ;
	lup->event = s->event ;
	lp->size_of_operand =  sizeof(XIMLookupProcessCallbackStruct) ;
	_imm_trace("IMM_LOOKUP_PROCESS:CBM)",0,0);
    } else if(s->WhoIsMaster == XIMIsMaster){
	/*
	 * Not Implemented Yet.
	 * This code is same as CBIsMaster
	 */
	XIMLookupProcessCallbackStruct *lup ;
	lp = (iml_inst *)_iml_new(s,
		  sizeof(XIMLookupProcessCallbackStruct) + sizeof(iml_inst) );
	if(results_required){
	    lp->opcode = IMM_LOOKUP_PROCESS | IMM_CB_RESULT_REQUIRED;
	} else {
	    lp->opcode = IMM_LOOKUP_PROCESS ;
	}	    
	lp->next = NULL ;
	lup = (XIMLookupProcessCallbackStruct *)&lp->operand ;
	lup->event = s->event ;
	lp->size_of_operand =  sizeof(XIMLookupProcessCallbackStruct) ;
	_imm_trace("IMM_LOOKUP_PROCESS:IMM)",0,0);
    } else {
	/*
	 * This must be that the Lookup Callback is not registerd!!
	 */
	lp = _iml_make_nop_inst(s) ;
    }
    return lp ;
}

Public iml_inst *
_iml_make_lookup_done_inst(s)
iml_session_t *s;
{
    iml_inst *lp ;

    lp = (iml_inst*)_iml_new(s, sizeof(iml_inst));
    lp->opcode = IMM_LOOKUP_DONE ;
    lp->next = NULL ;
    lp->size_of_operand = 0 ;
    _imm_trace("IMM_LOOKUP_DONE",0,0);
    _iml_pop_current_region(s);
    return lp ;
}

Public iml_inst *
_iml_make_status_start_inst(s)
iml_session_t *s;
{
    iml_inst *lp ;

    lp=(iml_inst *)_iml_new(s,sizeof(IMM_Region)+sizeof(iml_inst));
    _imm_trace("IMM_SET_REGION(STATUS)",0,0);
    _iml_put_current_region(s, STATUS) ;
    lp->opcode = IMM_STATUS_START ;
    lp->next = NULL ;
    return lp ;
}
Public iml_inst *
_iml_make_status_done_inst(s)
iml_session_t *s;
{
    iml_inst *lp ;

    lp = (iml_inst*)_iml_new(s, sizeof(iml_inst));
    lp->opcode = IMM_STATUS_DONE ;
    lp->next = NULL ;
    lp->size_of_operand = 0 ;
    _imm_trace("IMM_STATUS_DONE",0,0);
    _iml_pop_current_region(s);
    return lp ;
}

/*
 * 確定文字列を返す iml_inst を作る
 * session の中に、確定文字列が格納されていることが前提。
 */
Public Status
_iml_make_commit_inst(s, rrv, l, mbcs)
iml_session_t *s ;
iml_inst **rrv ;
int l ; /* length of Lookuped string */
char *mbcs ; /* Multi byte committed string */
{
    iml_inst *rv ;
    int sz = l+sizeof(committed_string_t)+1;
    committed_string_t *cs ;
    
    if(l<0){
	return -1;
    }
    rv = *rrv = (iml_inst*)_iml_new(s,sizeof(iml_inst) + sz );
    rv->opcode = IMM_COMMIT ;
    rv->next = NULL ;
    rv->size_of_operand = sz ;
    cs = (committed_string_t *)&rv->operand ;
    if(l>0){
	if(s->keysym == NoSymbol){
	    cs->status = XLookupChars ;
	} else {
	    cs->status = XLookupBoth ;
	}
    } else if(l == 0){
	if(s->initial_public_status & IMLSTATUS_Henkan_Mode) {
	    s->keysym = cs->keysym = NoSymbol ;
	    cs->status = XLookupNone ;
	    _imm_trace("IMM_COMIT",cs->string,sz);
	    return 0;
	} else {
	    if(s->keysym == NoSymbol){
		cs->status = XLookupNone ;
	    } else {
		cs->status = XLookupKeySym ;
	    }
	}
    } else {
	cs->status = XLookupNone ;
    }
    cs->keysym = s->keysym ; 
    strncpy(cs->string, mbcs, l) ;
    cs->string[l] = '\0' ;
    _imm_trace("IMM_COMIT",cs->string,sz);

    return 0; 
}

/*
 * 何も immgr へ返すものが無い時の NOP inst を作る
 */
Public void
_iml_fake_nop_inst(s, rrv)
iml_session_t *s;
iml_inst **rrv ;
{
    *rrv = (iml_inst *)_iml_new(s, sizeof(iml_inst));
    (*rrv)->opcode = IMM_NOP ;
    (*rrv)->next = NULL ;
    (*rrv)->size_of_operand = 0 ;
    return ;
}

Public IMM_Region
_iml_get_current_region(s)
iml_session_t *s ;
{
    iml_region_stack_t *cr = & s->current_region ;
    
    while(cr->next){
	cr = cr->next ;
    }
    return(cr->region);
}
Public int
_iml_put_current_region(s,r)
iml_session_t *s ;
IMM_Region r ;
{
    iml_region_stack_t *cr = & s->current_region ;
    int depth = 0 ;
    if(cr->region < 0) {
	/*
	 * current region has not been set
	 */
	cr->region = r ;
	return(1);
    }
    while(cr->next){
	cr = cr->next ;
	depth ++ ;
    }
    if(cr->region == PREEDIT && r == LOOKUP ){
	cr->next = (iml_region_stack_t *)
	    calloc(1, sizeof(iml_region_stack_t));
	cr->next->prev = cr ;
	cr->next->region = r ;
	return(depth +1 ) ;
    } else if (cr->region != r){
	if(cr->prev != 0 && cr->prev->region == r){
	    depth = _iml_pop_current_region(s);
	} else {
	    cr->region = r ;
	    depth++ ;
	}
    }
    
    return (depth);
}
Public int
_iml_pop_current_region(s)
iml_session_t *s;
{
    iml_region_stack_t *cr = & s->current_region ;
    int depth = 0 ;
    if(cr->region < 0) {
	/*
	 * current region has not been set
	 */
	return(-1);
    }
    while(cr->next){
	cr = cr->next ;
	depth ++ ;
    }
    if(depth){
	cr->prev->next = 0 ;
	free(cr);
	return(depth -1);
    } else {
	cr->region = -1 ;
	cr->next = 0 ;
	cr->prev = 0 ;
	return( -1 );
    } 
}
/*
 * post processor of the packets which passes to immgr
 */
Public void
_iml_post_proc(s, rrv)
iml_session_t *s ;
iml_inst **rrv;
{
    iml_inst *p = *rrv ;
    iml_inst *pp = 0;
    iml_inst *lp = 0;
    iml_inst *status_draw = 0 ;
    int HowManyPacket = 0 ;
    Bool KeysymShouldBeReturned = False ;
    do {
	HowManyPacket ++ ;
	switch((p->opcode) & (~IMM_CB_RESULT_REQUIRED )){
 	  case IMM_NOP :
	    if(p->next){
		if(pp){
		    pp->next = p->next ;
		} else {
		    *rrv = p->next ;
		}
		p = p-> next ;
		continue ;
	    }
	    break ;
	  case IMM_RESET :
	    _iml_inst_trace(s->session_id," RESET");
	    break ;
	  case IMM_COMMIT :
	    _iml_inst_trace(s->session_id," COMMIT");
	    KeysymShouldBeReturned = True ;
#ifndef IMSERVER
	    if(p->next){
		s->remainder = p->next ;
		p->next = (iml_inst*)_iml_new(s, sizeof(iml_inst));
		p = p->next ;
		p->opcode = IMM_PUT_QUEUE ;
		p->next = NULL ;
		_imm_trace(" PUT_QUEUE",0 ,0);
		return ;
	    }
#endif	    
	    break ;
	  case IMM_SET_STATUS :
	    _iml_inst_trace(s->session_id," SET_STATUS");
	    break ;
/* Obsolete
 *	  case IMM_CONVERSION_OFF :
 *	    _iml_inst_trace(s->session_id," CONVERSION_OFF");
 *	    break ;
 */
/*
 * PREEDIT REGION POST PROCCESS
 */
	  case IMM_PREEDIT_START :
	    if(s->active_regions & PREEDIT_IS_ACTIVE){
		p->opcode = IMM_NOP ;
		break ;
	    } else {
		_iml_inst_trace(s->session_id," PREEDIT_START");
	    }
	    /*
	     * possible optimization
	     */
	    if(p->next){
		iml_inst *tp = p->next ;
		XIMPreeditDrawCallbackStruct *pdcs ;
		while(((tp->opcode)&(~IMM_CB_RESULT_REQUIRED)) == IMM_PREEDIT_DRAW){
		    if(!tp->next) goto OUT ;
		    tp = tp->next ;
		    pdcs = (XIMPreeditDrawCallbackStruct *)&tp->operand ;
		    if(pdcs->text) goto OUT ;
		}
		if(((tp->opcode)&(~IMM_CB_RESULT_REQUIRED)) == IMM_PREEDIT_DONE){
		    if(pp){
			if(tp->next){
			    pp->next = tp->next ;
			    p = tp->next ;
			} else {
			    tp->opcode = IMM_NOP ;
			    pp->next = p = tp ;
			}
			continue ;
		    } else {
			if(tp->next){
			    *rrv = p = tp->next ;
			    continue ;
			} else {
			    *rrv = p = tp ;
			    p->opcode = IMM_NOP ;
			}
			continue ;
		    } 
		}
   OUT:
		s->active_regions |= PREEDIT_IS_ACTIVE ;
#ifdef NO_PREEDIT_RESULTS
		p->opcode &= (~IMM_CB_RESULT_REQUIRED) ;
#else
#ifdef XXXXIMSERVER
		if(p->next){
		    s->remainder2 = p->next ;
		    p->next = NULL ;
		    _imm_trace(" RESULTS REQUIRED",0 ,0);
		    return ;
		}
#endif		
#endif		
	    }
	    break ;
	  case IMM_PREEDIT_CARET :
	    _iml_inst_trace(s->session_id," PREEDIT_CARET");
	    goto post_preedit;
	  case IMM_PREEDIT_DRAW :
	    _iml_inst_trace(s->session_id," PREEDIT_DRAW");
post_preedit:	    
	    if(!(s->active_regions & PREEDIT_IS_ACTIVE)){
		lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
		lp->operand = (caddr_t)IMM_PREEDIT_START ;
		lp->next = p ;
		if(pp){
		    pp->next = lp ;
		} else {
		    p = *rrv = lp ;
		}
		s->active_regions |= PREEDIT_IS_ACTIVE ;
	    }
	    break ;
	  case IMM_PREEDIT_DONE:
	    if(!(s->active_regions & PREEDIT_IS_ACTIVE)){
		p->opcode = IMM_NOP ;
	    } else {
		s->active_regions &= ~PREEDIT_IS_ACTIVE;
		_iml_inst_trace(s->session_id," PREEDIT_DONE");
	    }
	    break;
/*
 * AUX REGION POST PROCCESS
 */
	  case IMM_AUX_START :
	    if(s->active_regions & AUX_IS_ACTIVE){
		p->opcode = IMM_NOP ;
	    } else {
		_iml_inst_trace(s->session_id," AUX_START");
		s->active_regions |= AUX_IS_ACTIVE ;
	    }
	    break ;
	  case IMM_AUX_DRAW :
	    _iml_inst_trace(s->session_id," AUX_DRAW");
	    goto post_aux;
	    break ;
	  case IMM_AUX_PROCESS :
	    _iml_inst_trace(s->session_id," AUX_PROCESS");
post_aux:	    
	    if(!(s->active_regions & AUX_IS_ACTIVE)){
		_iml_inst_trace(s->session_id,"<It might be crash>");
		lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
		lp->operand = (caddr_t)IMM_AUX_START ;
		lp->next = p ;
		if(pp){
		    pp->next = lp ;
		} else {
		    p = *rrv = lp ;
		}
		s->active_regions |= AUX_IS_ACTIVE ;
	    }
	    break ;
	  case IMM_AUX_DONE :
	    if(!(s->active_regions & AUX_IS_ACTIVE)){
		p->opcode = IMM_NOP ;
	    } else {
		s->active_regions &= ~AUX_IS_ACTIVE;
		_iml_inst_trace(s->session_id," AUX_DONE");
	    }
	    break ;
/*
 * LOOKUP REGION POST PROCCESS
 */
	  case IMM_LOOKUP_START :
	    _iml_inst_trace(s->session_id," LOOKUP_START");
	    if(s->active_regions & LOOKUP_IS_ACTIVE){
               /* Nothing */
	    } else {
		s->active_regions |= LOOKUP_IS_ACTIVE ;
#ifdef XXXIMSERVER
		if(p->next){
		    s->remainder2 = p->next ;
		    p->next = NULL ;
		    _imm_trace(" RESULTS REQUIRED",0 ,0);
		    return ;
		}
#endif		
	    }
	    break ;
	  case IMM_LOOKUP_DRAW :
	    _iml_inst_trace(s->session_id," LOOKUP_DRAW");
	    goto post_lookup;
	    break ;
	  case IMM_LOOKUP_PROCESS :
	    _iml_inst_trace(s->session_id," LOOKUP_PROCESS");
post_lookup:
	    if(!(s->active_regions & LOOKUP_IS_ACTIVE)){
		XIMLookupStartCallbackStruct *ls;

		/*
		 * XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		 * XXXXXXX Assume always CBIsMaster XXXXXXXXXXX
		 */
		_iml_inst_trace(s->session_id,"<It might be crash>");
		lp=(iml_inst *)_iml_new(s,sizeof(iml_inst) +
			sizeof(XIMLookupStartCallbackStruct));
		lp->opcode = IMM_LOOKUP_START | IMM_CB_RESULT_REQUIRED;
		lp->next = NULL ;
		_iml_put_current_region(s, LOOKUP) ;
		ls = (XIMLookupStartCallbackStruct *)&lp->operand ;
		ls->event = s->event ;
		ls->WhoIsMaster = s->WhoIsMaster ;
		ls->XIMPreference = &s->XIMPreference ;
		ls->XIMPreference->WhoOwnsLabel = CBOwnsLabel ;
		ls->CBPreference = &s->CBPreference ;
		if(pp){
		    pp->next = lp ;
		} else {
		    p = *rrv = lp ;
		}
		s->active_regions |= LOOKUP_IS_ACTIVE ;
	    } 
#ifdef XXXIMSERVER
	    if(p->next){
		s->remainder2 = p->next ;
		p->next = NULL ;
		_imm_trace(" RESULTS REQUIRED",0 ,0);
		return ;
	    }
#endif	    
	    break ;
	  case IMM_LOOKUP_DONE :
	    if(!(s->active_regions & LOOKUP_IS_ACTIVE)){
		p->opcode = IMM_NOP ;
	    } else {
		_iml_inst_trace(s->session_id," LOOKUP_DONE");
		s->active_regions &= ~LOOKUP_IS_ACTIVE;
	    }
	    break ;
/*
 * STATUS REGION POST PROCCESS
 */
	  case IMM_STATUS_START :
	    if(s->active_regions & STATUS_IS_ACTIVE){
		p->opcode = IMM_NOP ;
	    } else {
		_iml_inst_trace(s->session_id," STATUS_START");
		s->active_regions |= STATUS_IS_ACTIVE ;
	    }
	    break ;
	  case IMM_STATUS_DRAW :
	    /*
	     * possible optimization
	     */
	    if(status_draw){
		status_draw->opcode = IMM_NOP ;
	    }
	    status_draw = p ;
	    _iml_inst_trace(s->session_id," STATUS_DRAW");
	    break ;
	  case IMM_STATUS_DONE :
	    if(!(s->active_regions & STATUS_IS_ACTIVE)){
		p->opcode = IMM_NOP ;
	    } else {
		_iml_inst_trace(s->session_id," STATUS_DONE");
		s->active_regions &= ~STATUS_IS_ACTIVE;
	    }
	    break ;
/*
 * MISCELLANEOUS POST PROCCESS
 */
	  case IMM_KEY_INFO :
	    _iml_inst_trace(s->session_id," KEY_INFO");
	    break ;
	  case IMM_PUT_QUEUE :
	    _iml_inst_trace(s->session_id," PUT_QUEUE");
	    break ;
	  default:
	    fprintf(stderr, "Warning: iml_post_proc: UNKOWN opcode 0X%x\n",
		    (p->opcode) & (~IMM_CB_RESULT_REQUIRED ) );
	    _iml_inst_trace(s->session_id,"UNKNOWN");
	    p->opcode = IMM_NOP ;
	    break ;
	}
	pp = p ;
	p = p->next ;
    }while(p);
    if(!KeysymShouldBeReturned)
      s->keysym = NoSymbol ;

    _iml_inst_trace(s->session_id,"\n");
}

Private int
iml_generic_send_event(s, rrv, event)
iml_session_t *s;
iml_inst **rrv;
XKeyEvent event ;
{

}
Private void
iml_generic_close(s, rrv)
iml_session_t *s;
iml_inst **rrv;
{

}
Private void
iml_generic_reset(s, rrv)
iml_session_t *s;
iml_inst **rrv;
{

}
Private void
iml_generic_send_results(s, rrv)
iml_session_t *s;
iml_inst **rrv;
{

}
Private void
iml_generic_conversion_on(s, rrv)
iml_session_t *s;
iml_inst **rrv;
{

}

Private void
iml_generic_get_status(s, rrv)
iml_session_t *s;
iml_inst **rrv;
{
    iml_status_t *status ;
    *rrv=(iml_inst*)_iml_new(s, sizeof(iml_inst) +
			     sizeof(iml_status_t));
    (*rrv)->opcode = IMM_SET_STATUS ;
    (*rrv)->next = NULL ;
    status =  (iml_status_t *) &((*rrv)->operand);
    *status = s->public_status ;
    _imm_trace("IMM_SET_STATUS",*status,0);

}
Private void
iml_generic_get_key_map(session, rrv)
iml_session_t *session;
iml_inst **rrv;
{

}

Private void
iml_generic_set_status(s, rrv, status)
iml_session_t *s;
iml_inst **rrv;
iml_status_t *status ;
{
    int prev_status = s->public_status ;
    s->public_status = *status ;
    if(((s->public_status & IMLSTATUS_Henkan_Mode) == IMLSTATUS_Henkan_Mode) &&
       ((prev_status & IMLSTATUS_Henkan_Mode) != IMLSTATUS_Henkan_Mode )){
	s->iml_conversion_on(s, rrv);
    }
}
Private void
iml_generic_set_key_map(s, rrv)
iml_session_t *s;
iml_inst **rrv;
{

}

Private void
iml_generic_set_keyboard_state(s, rrv, key_buttons)
iml_session_t *s ;
iml_inst **rrv;
IMKeyState *key_buttons;
{
    register i ;
    register iml_inst *lp, *clp;
    XIMStatusDrawCallbackStruct *q ;
    XIMText *p ;
    wchar_t *wc_text ;
    int	current_region ;

    _imm_trace("IMM_SET_KEYBOARD_STATE",0,key_buttons);
    current_region = _iml_get_current_region(s);
    if(*key_buttons == MODE_SWITCH_ON){
	
    } else {

    }

#ifdef ALWAYS_SET_REGION_REQUIRED
    *rrv = lp =(iml_inst *)_iml_new(s,
			      sizeof(IMM_Region)+sizeof(iml_inst));
    lp->opcode = IMM_SET_REGION ;
    _imm_trace("IMM_SET_REGION(pseudo STATUS)",0,0);
    lp->operand = (caddr_t)STATUS ;
    lp->next = (iml_inst*)_iml_new(s,
				   sizeof(iml_inst)+
				   sizeof(XIMStatusCBData));
    lp = lp->next ;
#else
    *rrv = lp = (iml_inst*)_iml_new(s,
				   sizeof(iml_inst)+
				   sizeof(XIMStatusDrawCallbackStruct));
#endif
    lp->next = NULL ;

    wc_text = s->StatusStringRemainBuffer.buf ;
    p = (XIMText *)_iml_new(s,
				 sizeof(XIMText));
    lp->opcode = IMM_STATUS_DRAW  ;
    q = (XIMStatusDrawCallbackStruct *)&lp->operand ;
    q->type = XIMTextType ;
    q->data.text = p ;
    p->length = wslen(wc_text);
    p->string.wide_char = wc_text ;
    p->encoding_is_wchar = True ;
    p->feedback = (XIMFeedback *)_iml_new(s,
			  p->length*sizeof(XIMFeedback));
    _imm_trace("IMM_STATUS_DRAW(UPDATE)",wc_text,p->length);

#ifdef  ALWAYS_SET_REGION_REQUIRED
    lp->next =(iml_inst *)_iml_new(s,
			      sizeof(IMM_Region)+sizeof(iml_inst));
    lp=lp->next ;
    lp->opcode = IMM_SET_REGION ;
    _imm_trace("IMM_SET_REGION(resume)",0,current_region);
    lp->operand = (caddr_t)current_region ;
#endif
    lp->next = 0 ;
}

Public void
_iml_error(s)
char * s;
{
    fprintf(stderr, "%s\n", s);
}

Public int
_iml_calc_visible_pos(bpos, mbs, maxlen)
int bpos;
char *mbs;
int maxlen;
{
    register i, j, l ;

    for(i=0, j=0; i<bpos && j <= (maxlen +1);i+=l, j++){
	l=mblen(mbs+i,4);
    }
    return(j);
}


/*
 * ------------------ TRACE ROUTINES -----------------------
 */
#ifdef IMLTRACE
Public void
_iml_inst_trace(id,s)
int id ;
char *s ;
{
    static neednl ;
    if(imlogic_trace & 0x40) {
	if(*s == '\n'){
	    if(neednl){
		fprintf(stderr,"%s",s);
	    }
	    neednl = 0 ;
	} else {
	    neednl++;
	    fprintf(stderr,"%s%d",s,id);
	}
    }
}

Public void
_iml_trace(id,s,s2)
int id ;
char *s ;
wchar_t *s2 ;
{
    if(imlogic_trace & 0x1) {
	fprintf(stderr,"M->L(%d): %s",id, s);
	if(s2){
	    fprintf(stderr," %ws - 0x%x ",s2, *s2);
	}
	fprintf(stderr,"\n");	
    }
#ifdef CALL_MALLOC_VERIFY
    if(imlogic_trace & 0x10 )
	if(!malloc_verify())
	    fprintf(stderr,"MALLOC_VERIFY RETURNS 0\n");
#endif
}
Public void
_imm_trace(s1,s2,d)
char *s1 ;
wchar_t *s2 ;
unsigned short d;
{
    if(imlogic_trace & 0x1)  {
	fprintf(stderr,"L->M: %s ",s1);
	if(s2 == NULL){
	    fprintf(stderr,"- ");
	} else {
	    fprintf(stderr,"'%ws' ",s2);
	}
	fprintf(stderr,"0x%x\n",d);
    }
#ifdef CALL_MALLOC_VERIFY
    if(imlogic_trace & 0x10 )
	if(!malloc_verify())
	    fprintf(stderr,"MALLOC_VERIFY RETURNS 0\n");

#endif
    
}

Public void
_iml_slot_trace(s, session, new, ptr, size, link_depth)
char *s ;
iml_session_t *session ;
iml_inst_slot_t *new ;
char *ptr ;
{

    if(imlogic_trace & 0x4) 
	fprintf(stderr,
		"%s session0x%x slot:0x%x adrress:0x%x size:0x%x link depth %d\n",
		s, session, new, ptr, size, link_depth
		);
#ifdef CALL_MALLOC_VERIFY
    if(imlogic_trace & 0x10 )
	if(!malloc_verify())
	    fprintf(stderr,"MALLOC_VERIFY RETURNS 0\n");

#endif    
}    

Private int byte_of_line = 16 ;
Private void fmtprt();

Public void
_iml_xdmem(top, len)
char *	top;
int	len;
{
    register	char *i,*j;
    register 	print_len ;
    
    if(!(imlogic_trace & 0x2)) return;
    j = top + len ;
    i = top ;
    while ( (print_len = j-i) >0 ) {
	if(print_len > byte_of_line){
	    fmtprt(i, i, byte_of_line);
	    i+=byte_of_line ;
	} else {
	    fmtprt(i, i, print_len);
	    break ;
	}
    }
    fflush(stderr);
}
Private void
fmtprt(data, offset, lsize)
char   *data;
long    offset;
short   lsize;
{
    short   n;
    char	linebuf[BUFSIZE];
    char	*line = linebuf, *p;
    
    sprintf(line, "  %08lx   ", offset);
    p = line + 13;
    for (n = 0; n < byte_of_line; n++) {
	*p++ = ' ';
	if (n >= lsize) {
	    sprintf(p, "    ");
	    p += 4;
	    ++n;
	} else {
	    sprintf(p, "%02x", *((unsigned char *)data + n));
	    p += 2;
	    if ((++n) == lsize)
		sprintf(p, "  ");
	    else
		sprintf(p, "%02x", *((unsigned char *)data + n));
	    p += 2;
	}
    }
    sprintf(p, "     ");
    p += 5;
    for (n = 0; n < byte_of_line; n++) {
	if (n >= lsize)
	    *p++ = ' ';
	else if (*(data + n) < 0x20 )
	    /*	else if (!isprint(*((unsigned char *)data + n)))*/
	    *p++ = '.';
	else
	    *p++ = *(data + n);
    }
    *p = '\0';
    fprintf(stderr,"%s\n", line);
}
Private long 
htoi(p)
register char *p;		/* hex mojiretsu e no pointer */
{
    register long n = 0;
    
    if (upper(p) == -1)
	return (-1);
    while (*p >= '0' && *p <= '9' || *p >= 'A' && *p <= 'F')
	n = n * 16 + (*p++ >= 'A' ? *(p - 1) - 7 : *(p - 1)) - '0';
    return (n);
}

Private
upper(p)
register char *p;		/* hex mojiretsu e no pointer */
{
    register char *pp;
    
    for (pp = p; *pp; ++pp) {
	*pp = 'a' <= *pp && 'z' >= *pp ? *pp + 'A' - 'a' : *pp;
	if (!isxdigit(*pp))
	    return (-1);
    }
}

Private char   *
itoa(i, a)
int     i;		/* mojiretsu ni henkan suru binary (8-32 bit)  */
register char *a;	/* decimal mojiretsu o kakunou area no address */
{
    register int c = 0;
    register int b = 0;
    int     s;
    register char d;
    
    i = (s = i < 0) ? (-i) : i;
    do {
	*(a + c++) = i % 10 + '0';
    } while ((i /= 10) > 0);
    *(a + c++) = s ? '-' : '\0';
    *(a + c++) = '\0';
    for (c = strlen(a) - 1; b < c; d = *(a + b), *(a + b++) = *(a + c), *(a + c--) = d);
    return (a);
}
#endif
