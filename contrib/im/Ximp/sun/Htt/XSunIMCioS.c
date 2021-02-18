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

Ci_result       *get_rp();
iml_inst *      cio_make_status_start_inst();


Public void
iml_cio_mode(s, rrv, result)
iml_session_t *s ;
iml_inst **rrv;
Ci_result **result;
{
	int size_of_operand ;
	wchar_t * wc_text ;
	static int simple=1;
	XIMStatusDrawCallbackStruct *q ;
	iml_inst *lp ;
	XIMText *t ;
        Ci_result       *rp = get_rp(result);	

    *rrv = cio_make_status_start_inst(s);
    size_of_operand = DEFAULTStatusStringRemainBufferSize ;
    wc_text = s->StatusStringRemainBuffer.buf ;
    lp = (iml_inst*)_iml_new(s,sizeof(iml_inst)+sizeof(XIMStatusDrawCallbackStruct));
    t = (XIMText *)_iml_new(s, sizeof(XIMText));
    lp->opcode = IMM_STATUS_DRAW ;
    lp->next = NULL ;
    q = (XIMStatusDrawCallbackStruct *)&lp->operand ;
	cio_make_status_string(s, wc_text, 
			(unsigned char *)rp->action._cmd.data.mode.modestr._mb,
				simple);
    /*
     * could be re-allocated in jle103_make_status_string
     */
    wc_text = s->StatusStringRemainBuffer.buf ;
    q->type = XIMTextType ;
    q->data.text = t ;
    t->length = wslen(wc_text);
    t->string.wide_char = wc_text ;
    t->encoding_is_wchar = True ;
    t->feedback = (XIMFeedback *)_iml_new(s,t->length*sizeof(XIMFeedback));
    _imm_trace("IMM_STATUS_DRAW",wc_text,t->length);
    _iml_link_inst_tail(rrv, lp);
 
}

Private iml_inst *
cio_make_status_start_inst(s)
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

