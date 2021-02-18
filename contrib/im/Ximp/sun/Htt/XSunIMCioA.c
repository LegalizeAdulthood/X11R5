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

Public void
iml_cio_misc(s, rrv, result,cmp)
iml_session_t *s ;
iml_inst **rrv;
Ci_result **result;
Cio_command *cmp;
{
	static int      first_misc_init;
	int size_of_operand ;
	wchar_t * wc_text ;
	XIMPreeditDrawCallbackStruct *p ;
	XIMAuxStartCallbackStruct *q;
	XIMText *t;
        iml_inst *lp ;
	Ci_result	*rp = get_rp(result);

	switch(rp->action._cmd.data.misc.operation){
	case CS_MISC_INIT : /* AUX - START & DRAW */
	    /* env_m_label */
            lp=(iml_inst*)_iml_new(s, sizeof(iml_inst)+
                                     sizeof(XIMAuxStartCallbackStruct));
            lp->opcode = IMM_AUX_START ;
            lp->next = NULL ;
            q = (XIMAuxStartCallbackStruct *)&lp->operand ;
/*
            size_of_operand = strlen(rp->ce_misc_text)*sizeof(wchar_t);
*/
            q->event = (XKeyEvent *)s->event ;
            t = q->label = _iml_new(s, sizeof(XIMText));
            t->encoding_is_wchar = True ;
            t->string.wide_char = _iml_new(s, size_of_operand);
/*
            t->length = mbstowcs(t->string.wide_char, rp->ce_misc_text,
                                 size_of_operand) * sizeof(wchar_t) ;
*/
            t->feedback = _iml_new(s, t->length);
            bzero(t->feedback, t->length);
            t = q->text = _iml_new(s, sizeof(XIMText));
            t->length = 0 ; 
	    *rrv = lp;
	    /* env_m_interactive */
/*
	    lp = make_preedit_draw_inst(s,rp);
*/
	    lp->opcode = IMM_AUX_DRAW ;
	    (*rrv)->next = lp;


            /*
		lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
            lp->opcode = IMM_AUX_DONE ;
            lp->next = NULL ;
		*/
            break;
	case CS_MISC_UPDATE :
#ifdef notdef
			case ENV_M_LABEL:{
              XIMAuxStartCallbackStruct *q ;
              XIMText *t;
              _imm_trace("IMM_AUX_START(ENV_M_LABEL)",0,0);
              _iml_put_current_region(s, AUX) ;
              lp=(iml_inst*)_iml_new(s, sizeof(iml_inst)+
                                       sizeof(XIMAuxStartCallbackStruct));
              lp->opcode = IMM_AUX_START ;
              lp->next = NULL ;
              q = (XIMAuxStartCallbackStruct *)&lp->operand ;
              size_of_operand = strlen(rp->ce_misc_text)*sizeof(wchar_t);
              q->event = (XKeyEvent *)s->event ;
              t = q->label = _iml_new(s, sizeof(XIMText));
              t->encoding_is_wchar = True ;
              t->string.wide_char = _iml_new(s, size_of_operand);
              t->length = mbstowcs(t->string.wide_char, rp->ce_misc_text,
                                   size_of_operand) * sizeof(wchar_t) ;
              t->feedback = _iml_new(s, t->length);
              bzero(t->feedback, t->length);
              t = q->text = _iml_new(s, sizeof(XIMText));
              t->length = 0 ;
              break;
          }
          case ENV_M_MESS:
            _imm_trace("IMM_NOP(ENV_M_MESS)",0,0);
            lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
            lp->opcode = IMM_NOP ;
            lp->next = NULL ;
            break;
#endif
          default :
		  _imm_trace("IMM_NOP(UNKNOWN)",0 ,0);
            lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
            lp->opcode = IMM_NOP ;
            lp->next = NULL ;
            break;
        }

	if(*rrv)
		_iml_link_inst_tail(rrv, lp);
	else
		*rrv = lp;
}
