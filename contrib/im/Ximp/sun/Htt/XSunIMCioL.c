#ifndef lint
#ifdef sccs
static char    sccsid[] = "%Z%%M% %I% %E%";
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

  Author: Masayuki Seino (seino@Japan.Sun.COM)
                                             Sun Microsystems, Inc.
******************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <euc.h>
#include <widec.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#if XlibSpecificationRelease != 5
#ifdef sun
#include "Sunkeysym.h"
#endif
#endif /* XlibSpecificationRelease */

#include "mle/cs.h"
#include "mle/ci.h"
#include "mle/cics_macro.h"

#include "XSunExt.h"
#include "XSunIMProt.h"
#include "XSunIMPriv.h"
#include "XSunIMMMan.h"
#include "XSunIMCio.h"
#include "XSunIMPub.h"

Ci_result	*get_rp();
iml_inst *	cio_make_lookup_process_inst();
iml_inst *	cio_make_lookup_draw_inst();
iml_inst *	cio_make_lookup_start_inst();

Public void
iml_cio_select(s, rrv, result,cmp)
iml_session_t *s ;
iml_inst **rrv;
Ci_result **result;
Cio_command *cmp;
{
	static int      first_select_init;
	int size_of_operand ;
	wchar_t * wc_text ;
	int		i;
	XIMPreeditDrawCallbackStruct *p ;
	iml_inst *lp ;
	Ci_result *rp = get_rp(result);


switch(cmp->data.select.state){
	case CS_STATE_UNUSED :
	case CS_STATE_BUSY :
			break;
	case CS_STATE_INIT :
		switch(cmp->data.select.operation){
			case CS_SEL_INIT:

			    lp = cio_make_lookup_draw_inst(s, rp, cmp);
			    if(_iml_get_current_region(s) != LOOKUP ){
				*rrv = cio_make_lookup_start_inst(s);
				_iml_link_inst_tail(rrv, lp);
			    } else {
				*rrv = lp ;
			    }
			    _imm_trace("PSEUDO LOOKUP",0,0);
			    select_id = cmp->data.select.regid ;
			    selon = 1;
			    selreg = cmp->data.select ;
			    break;
/*************************************************
			selreg = cmp->data.select ;
			selreg.sel_info.select_attr = 0x00021123 ;
			selreg.sel_info.page_items = 15 ;
			selreg.sel_info.line_items = 5 ;
			selreg.sel_info.idx_top = 1 ;
			if(selreg.sel_info.total_items < 15)
				selreg.sel_info.idx_end = 
					selreg.sel_info.total_items ;
			else
				selreg.sel_info.idx_end = 15 ;
			selreg.sel_info.idx_focus = 1 ;
			selcnt = cmp->data.select.count ;
			sellen = getsellen(selcnt,
				cmp->data.select.items.text.text_mb);
			selbuf = (char *)malloc(sellen);
			memcpy(selbuf,cmp->data.select.items.text.text_mb,sellen);
			dispsel();
			selon = 1;
*************************************************/
		}
		break;
	case CS_STATE_CHOICE:
		switch(cmp->data.select.operation){
			case CS_SEL_UPDATE:/* change next page */
		if(_iml_get_current_region(s) != LOOKUP){
			XIMLookupStartCallbackStruct *ls;
	 
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
			_imm_trace("PSEUDO LOOKUP",0,0);
			_iml_put_current_region(s, LOOKUP) ;
		    } else {
			if(s->WhoIsMaster == CBIsMaster){
			    XIMLookupProcessCallbackStruct *lup ;
			    lp = (iml_inst *)_iml_new(s,
					 sizeof(XIMLookupProcessCallbackStruct)
							+ sizeof(iml_inst) );
			    lp->opcode = IMM_LOOKUP_PROCESS ;
			    lp->next = NULL ;
			    lup = (XIMLookupProcessCallbackStruct *)&lp->operand ;
			    lup->event = (XKeyEvent *)s->event ;
			    lp->size_of_operand = size_of_operand ;
		    _imm_trace("IMM_LOOKUP_PROCESSNEXT)",0,0);
			    break;
			} else if(s->WhoIsMaster == XIMIsMaster){
			    /* Not Yet */
			}
		    }
			default:
				break;
		}
	case CS_STATE_KAKUTEI:{
		switch(cmp->data.select.operation){
			case CS_SEL_END:
				lp = cio_make_lookup_process_inst(s, rp);
				if(_iml_get_current_region(s) != LOOKUP ){
					*rrv = cio_make_lookup_start_inst(s);
					_iml_link_inst_tail(rrv, lp, False);
				} else {
					*rrv = lp ;
				}

/*
			       size_of_operand = 
(strlen( cmp->data.interm.itext.text.text_mb)+1)*sizeof(wchar_t) ;
		       lp = (iml_inst *)_iml_new(s,  size_of_operand +
                  sizeof(XIMLookupProcessCallbackStruct) + sizeof(iml_inst) );
		       lp->opcode = IMM_LOOKUP_PROCESS |IMM_CB_RESULT_REQUIRED;
		       lp->next = NULL ;
		       lup = (XIMLookupProcessCallbackStruct *)&lp->operand ;
		       lup->event = (XKeyEvent *)s->event ;
		       lp->size_of_operand = size_of_operand ;
		       _imm_trace("IMM_LOOKUP_PROCESS",0,s->event->keycode);
*/
		       break;
			default:/* ENV_SELECT_END */
				_imm_trace("IMM_LOOKUP_DONE",0,0);
				lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
				lp->opcode = IMM_LOOKUP_DONE ;
				lp->next = NULL ;
				_iml_pop_current_region(s);
				    *rrv = lp ;
				break;
		}/* switch */
	}/* case */
	default:
		selreg = cmp->data.select ;
/*
		dispsel();
*/
		select_id = cmp->data.select.regid ;
		break ;
	}
}

/*
	CI-CS returns string as candidates , but XIC needs array of strings.
	The candidates are separated by NULL of 1 byte.
*/

Private int
cio_string_to_array(array,string,count)
u_char	*array[256];
u_char	*string;
int	count;
{
	int	i,j,k;
	int	start,end;
	u_char	*candidate;

	start = 0;
	for(i=0;i<count;i++){
		for(j=0;j<256;j++)
			if(string[start+j] == '\0')
				break;
		if (j==0)
			return(j);
		array[i] = (u_char *)malloc(j+1); 
		memcpy(array[i],&string[start],j+1);
		start += (j+1);
	}
	return(i);
#if 0
	j = k = 0;
	start = end = 0;
	for(i=0; j <= count;i = i + 2){
		if(string[i+1] == '\0'){
			break;
		}

		if((string[i] == '\0')){
			end = i;
			candidate = (u_char *)malloc(sizeof((end-start+1)));
			for(k=0;(start+k) < end;k++)
				candidate[k] = string[start+k];
			candidate[end-start+1] = '\0';
			array[j++] = candidate;
			start = end + 1;
		}
		else{
		}
	}
	return (j);
#endif
}
Private iml_inst *
cio_make_lookup_start_inst(s)
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

Private iml_inst *
cio_make_lookup_draw_inst(s, rp, cmp)
iml_session_t *s;
Ci_result *rp ;
Cio_command	*cmp;
{
    iml_inst *lp ;
    register XIMLookupDrawCallbackStruct *ld ;
    register XIMText *t ;
    register wchar_t *w ;
    register u_char **e ;
    register XIMFeedback *a ;
    register l ;
    register i ;
    u_char	*E[256];


    switch(s->WhoIsMaster){
    case HasNotBeenNegotiated:
        /* Not yet */
    case XIMIsMaster:
        /* Not Yet */
    case CBIsMaster:
        lp=(iml_inst*)_iml_new2(s,
                        sizeof(iml_inst)+sizeof(XIMLookupDrawCallbackStruct));
        lp->opcode = IMM_LOOKUP_DRAW ;
        lp->next = NULL;
        ld = (XIMLookupDrawCallbackStruct*) &lp->operand;
	ld->n_choices = cmp->data.select.sel_info.total_items ;
        ld->index_of_first_candidate = 0 ;
        ld->index_of_last_candidate = ld->n_choices ;

        ld->choices = (XIMChoiceObject *)_iml_new2(s,
                           ld->n_choices * sizeof(XIMChoiceObject)) ;

	e = (u_char **)&(cmp->data.select.items.text.text_mb );




	cio_string_to_array(E, cmp->data.select.items.text.text_mb,
                                        cmp->data.select.count);

        ld->max_len = 0 ;
        for(i = 0 ; i < ld->n_choices ; i++){
            l = (strlen(E[i])+sizeof(char))*sizeof(wchar_t) ;
            w = (wchar_t *)_iml_new2(s, l);
            mbstowcs(w, E[i], l);
            t = (ld->choices)[i].value = (XIMText *)
              _iml_new2(s, sizeof(XIMText));
            (ld->choices)[i].label = NULL ;
            t->length = wslen(w);
            if(t->length > ld->max_len) {
                ld->max_len = t->length ;
            }
            t->feedback = (XIMFeedback*)_iml_new2(s,
                                        t->length * sizeof(XIMFeedback));
            t->encoding_is_wchar = True ;
            t->string.wide_char = w ;
        }
        s->ld = ld ;
        break;
    }
    _imm_trace("IMM_LOOKUP_DRAW",0,ld->n_choices);
    return lp;
}

Private
cio_select_end(s, rrv, result)
iml_session_t *s;
iml_inst **rrv;
Ci_result **result;
{
    iml_inst *lp ;
    Ci_result *rp = get_rp(result);

    _imm_trace("IMM_LOOKUP_DONE",0,0);
    lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
    lp->opcode = IMM_LOOKUP_DONE ;
    lp->next = NULL ;
    _iml_pop_current_region(s);
    *rrv = lp ;
}

Private iml_inst *
cio_make_lookup_process_inst(s, rp, results_required, controll)
iml_session_t *s;
Ci_result *rp ;
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
        _imm_trace("IMM_LOOKUP_PROCESSNEXT)",0,0);
    } else if(s->WhoIsMaster == XIMIsMaster){
        /* Not Yet */
    }
    return lp ;
}

Private
cio_select_next(s, rrv, result)
iml_session_t *s;
iml_inst **rrv;
Ci_result **result;
{
    iml_inst *lp ;
    Ci_result *rp = get_rp(result);

    lp = cio_make_lookup_process_inst(s, rp);
    if(_iml_get_current_region(s) != LOOKUP ){
        *rrv = cio_make_lookup_start_inst(s);
        _iml_link_inst_tail(rrv, lp, False);
    } else {
        *rrv = lp ;
    }
}

Private
cio_select_prev(s, rrv, result)
iml_session_t *s;
iml_inst **rrv;
Ci_result **result;
{
    iml_inst *lp ;
    Ci_result *rp = get_rp(result);

    lp = cio_make_lookup_process_inst(s, rp);
    if(_iml_get_current_region(s) != LOOKUP ){
        *rrv = cio_make_lookup_start_inst(s);
        _iml_link_inst_tail(rrv, lp, False);
    } else {
        *rrv = lp ;
    }
}

