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

int remainder;
extern void ciAttr2IMTextAttr();
Ci_result       *get_rp();
iml_inst *      cio_make_preedit_start_inst();
iml_inst *      cio_make_preedit_done_inst();
iml_inst *      cio_make_preedit_draw_inst();
iml_inst *      cio_make_preedit_erase_inst();

Public void
iml_cio_interm(s, rrv, result,cmp)
iml_session_t *s ;
iml_inst **rrv;
Ci_result **result;
Cio_command *cmp;
{
	static int      first_interm_edit;
	int size_of_operand ;
	wchar_t * wc_text ;
	XIMPreeditDrawCallbackStruct *p ;
	iml_inst *lp ;
	Ci_result	*rp = get_rp(result);


	switch(cmp->data.interm.state){
	case CS_STATE_BUSY:
	    break;
	case CS_STATE_INIT:
	case CS_STATE_EDIT:
	case CS_STATE_CONV:
	    interm_id = cmp->data.interm.regid;
	    lp = cio_make_preedit_draw_inst(s, rp,cmp);
	    if(_iml_get_current_region(s) != PREEDIT ){
		*rrv = cio_make_preedit_start_inst(s);
		_iml_link_inst_tail(rrv, lp);
	    } else {
		*rrv = lp ;
	    }
	    _imm_trace("IMM_SET_REGION(pseudo PREEDIT)",0,0);

	    break;

	case CS_STATE_UNUSED:
	    cnvon = 0;
	    s->public_status &= ~IMLSTATUS_Henkan_Mode;
	case CS_STATE_KAKUTEI:
	    interm_id = cmp->data.interm.regid;

	    lp = cio_make_preedit_erase_inst(s);
	    *rrv = lp;
	    lp = cio_make_preedit_done_inst(s);
	    _iml_link_inst_tail(rrv, lp);

	    if(cmp->data.interm.itext.text.text_mb==0)
		break;
	    _iml_make_commit_inst(s, &lp,
		strlen(cmp->data.interm.itext.text.text_mb),
			cmp->data.interm.itext.text.text_mb);
	    _iml_link_inst_tail(rrv, lp);

	    break ;
	default:
		break;
	}
}

Private iml_inst *
cio_make_preedit_draw_inst(s, rp,cmp)
iml_session_t *s;
Ci_result *rp ;
Cio_command	*cmp;
{
    iml_inst *lp ;
    XIMPreeditDrawCallbackStruct *p ;
    wchar_t *wc_text ;



    wchar_t *pws = s->PreEditTextInfo.text->string.wide_char;/* prev str */
    int      pwl = s->PreEditTextInfo.text->length ;         /* prev len */
    XIMFeedback *pfb = s->PreEditTextInfo.text->feedback ;   /* prev fb */
    wchar_t *cws ;                                          /* current wstr */
    int      cwl ;                                          /* current len */
#ifdef notdef
    char    *cmbs = (char *) rp->ce_text ;
#endif
    char    *cmbs = (char *) cmp->data.interm.itext.text.text_mb;
							  /* current mbstr */
    int      cmbl = strlen(cmbs);                           /* current mblen */
    XIMFeedback *cfb ;                                      /* current fb */
    int si ;                                            /* str index */
    int sri ;                                           /* str reverse index */
    int fbi ;                                           /* fb index */
    int fbri ;                                          /* fb reverse index */
    int i ;                                             /* index */

    cws = (wchar_t*)_iml_new(s, (cmbl+1)*sizeof(wchar_t));
    mbstowcs(cws,cmbs,IML_MAX_SLOT_SIZE);
    cwl = wslen(cws);
    if(cwl <= 0){
        return(cio_make_preedit_erase_inst(s, rp));
    }   
    lp = (iml_inst*)_iml_new(s,sizeof(iml_inst)+sizeof(XIMPreeditDrawCallbackStruct));
    lp->opcode = IMM_PREEDIT_DRAW ;
    lp->next = NULL ;
    cfb = _iml_new(s, (cmbl + 1)*sizeof(XIMFeedback));
/*
    jle103_attr2feedback(cfb, rp->ce_text_attr, cwl, cmbs);
*/
    for(si = 0 ; si < pwl ; si++) if(pws[si] != cws[si]) break ;
    if(cwl && (pwl == cwl)){
        for(sri = pwl-1 ; sri >= 0 ; sri--) if(pws[sri] != cws[sri]) break ;
    }
    for(fbi = 0 ; fbi < pwl ; fbi++) if(pfb[fbi] != cfb[fbi]) break ;
    i = min(si, fbi);

    p = (XIMPreeditDrawCallbackStruct*)&(lp->operand);
    p->text = (XIMText *)_iml_new(s, sizeof(XIMText));
    p->text->encoding_is_wchar = True ;
    p->text->feedback = cfb + i ;
    if((p->text->length = cwl - i)<=0) {
        if(cwl < pwl ){
            p->text = NULL ;
        } else {
            p->text->string.wide_char = NULL ;
        }
    } else {
        p->text->string.wide_char = cws +i ;
    }

    p->chg_first = i ;
    p->chg_length = pwl - i ;
    p->caret = cwl - 1;

#ifdef notdef
    lp = (iml_inst*)_iml_new(s,sizeof(iml_inst)+sizeof(XIMPreeditDrawCallbackStruct));
    lp->opcode = IMM_PREEDIT_DRAW ;
    lp->next = NULL ;
    p = (XIMPreeditDrawCallbackStruct*)&(lp->operand);
    p->text = (XIMText *)_iml_new(s, sizeof(XIMText));
    p->text->length = strlen(cmp->data.interm.itext.text.text_mb);
    p->chg_first = 0 ;
    p->chg_length = s->PreEditTextInfo.text.length ;
    p->caret = 0 ;
    wc_text = (wchar_t*)_iml_new(s, (p->text.length+1)*sizeof(wchar_t));
    mbstowcs(wc_text,
	cmp->data.interm.itext.text.text_mb,IML_MAX_SLOT_SIZE);
    p->text.length = wslen(wc_text);
    p->text.feedback = (XIMFeedback *)
      _iml_new(s, (p->text.length + sizeof(XIMFeedback))*sizeof(XIMFeedback));
    p->text.string.wide_char = wc_text ;
    p->text.encoding_is_wchar = True ;

#endif

#if XlibSpecificationRelease != 5
#ifdef sun
    p->text.visible_pos =
      _iml_calc_visible_pos(cmp->data.interm.chgtop,
		cmp->data.interm.itext.text.text_mb, p->text.length);
#ifdef notdef
    switch(rp->ce_v_type){
    case POS_END:
        default:
        p->text.visible_type = XIMVisiblePosEnd ;
        break ;
    case POS_START:
        p->text.visible_type = XIMVisiblePosStart ;
        break ;
    case POS_MID:
        p->text.visible_type = XIMVisiblePosMid ;
        break ;
    }
#endif

#endif
#endif /* XlibSpecificationRelease */

/*
    ciAttr2IMTextAttr(p->text->feedback, 
			cmp->data.interm.itext.attr,
		p->text->length,cmp->data.interm.itext.text.text_mb);
    s->PreEditTextInfo.text.length =  p->text.length ;
*/

    /*
     * save current string as previous string
     */
    if((cwl + 1) > s->PreEditTextBufferSize){
        free(s->PreEditTextInfo.text->string.wide_char);
        free(s->PreEditTextInfo.text->feedback);
        s->PreEditTextInfo.text->string.multi_byte =
          calloc(sizeof(wchar_t),cwl);
        s->PreEditTextInfo.text->feedback =
          (XIMFeedback *)calloc(sizeof(XIMFeedback),cwl);
        s->PreEditTextBufferSize = cwl +1 ;
        s->PreEditAttrBufferSize = cwl +1 ;
    }
    s->PreEditTextInfo.text->length =  cwl ;
    wscpy(s->PreEditTextInfo.text->string.wide_char, cws) ;
    bcopy(cfb, s->PreEditTextInfo.text->feedback, cwl * sizeof(XIMFeedback));

    _imm_trace("IMM_PREEDIT_DRAW(Draw All)",wc_text,p->text->length);
    return lp;
}

Private iml_inst *
cio_make_preedit_start_inst(s)
iml_session_t *s;
{
    iml_inst *lp ;

    lp = (iml_inst*)_iml_new(s, sizeof(iml_inst));
    _imm_trace("IMM_PREEDIT_START",0,0);
    _iml_put_current_region(s, PREEDIT) ;
    lp->opcode = IMM_PREEDIT_START ;
    lp->next = NULL ;
    lp->size_of_operand = 0 ;
    return lp ;
}

Private iml_inst *
cio_make_preedit_done_inst(s)
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

/*
 * get Ci_result *result from Ci_result link chain and remove from the chain
 */

Ci_result *
get_rp(result)
Ci_result **result;
{
    Ci_result *rp = NULL ;

    if(result){
        rp = *result ;
        if (rp != NULL){
            *result = rp->next ;
        } else {
            *result = NULL ;
        }
        return rp ;
    }
    return NULL;
}

Private iml_inst *
cio_make_preedit_erase_inst(s)
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
    p->text = NULL ;
    p->chg_length = s->PreEditTextInfo.text->length ; /* previous length */
    s->PreEditTextInfo.text->length =  0 ;
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

