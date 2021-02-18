#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunIM.c 1.7 91/08/24";
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
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <pwd.h>
#include <grp.h>
#include "XSunExt.h"
#include "XSunIMProt.h"
#include "XSunIMPriv.h"
#include "XSunIMMMan.h"
#include "XSunIMCore.h"
#include "XSunIMPub.h"

/*
 * 
 */

Public KeySym
IMLogic(id, rrv, opcode, operand1, operand2)
int id ;
iml_inst **rrv ;            /* buffer return */
IML_Opcode opcode ;
caddr_t operand1 ;
caddr_t operand2 ;
{
    *rrv = NULL ;
    if(opcode == IML_OPEN){
    	_iml_trace(id,"IML_OPEN",0);
	id =_iml_configure(id, rrv);
	return id ;
    } else {
	iml_session_t *s ;

	if((int)(s = _iml_get_session(id)) < 0 ){
	    /*
	     * Session $@$,8+$D$1$i$l$J$$%1!<%9$O!"(JIM $@$N=EEY$J/home/sundae4/donation/si/SCCS_DIRECTORIES/usr.lib/libxim/SCCS/s.XSunIM.c0$G$"$m$&!#(J
	     * X*LookupString $@$+$i8F$P$l$F$$$?$J$i!"$?$@4X?tCM$N$_(J
	     * $@$G=hM}$5$l$k$N$G!"(Jfake nop $@$OF~$i$J$$$,!"B>$+$i8F$P$l$?(J
	     * $@$H$-$N$?$a$K0l1~(J
	     */
	    static iml_inst XX_inst ;

	    *rrv = &XX_inst ;
	    (*rrv)->opcode = IMM_NOP ;
	    (*rrv)->next = NULL ;
	    return((KeySym)-1) ;
	}
	s->initial_public_status = s->public_status ;
	switch(opcode){
	case IML_CLOSE:
	    _iml_trace(id,"IML_CLOSE",0);
 	    (*s->iml_close)(s, rrv) ;
	    break;
	case IML_RESET:
	    _iml_trace(id,"IML_RESET",0);
	    (*s->iml_reset)(s, rrv) ;
	    break;
	case IML_GET_STATUS:
	    _iml_trace(id,"IML_GET_STATUS",0);
	    (*s->iml_get_status)(s, rrv);
	    break;
	case IML_GET_KEY_MAP:
	    _iml_trace(id,"IML_GET_KEY_MAP",0);
	    (*s->iml_get_key_map)(s, rrv, operand1, operand2);
	    break;
	case IML_SET_STATUS:
	    _iml_trace(id,"IML_SET_STATUS",0);
	    (*s->iml_set_status)(s, rrv, (iml_status_t)operand1);
	    break;
	case IML_SET_KEY_MAP:
	    _iml_trace(id,"IML_SET_KEY_MAP",0);
	    (*s->iml_set_key_map)(s, rrv, operand1, operand2);
	    break;
	case IML_SEND_EVENT:
	    /*
	     * $@$3$N(J opcode $@$O!"D>@\(J X*LookupString $@$+$i$N$_8F$P$l$k$?$a!"(J
	     * $@$3$N>l9g$N$_!"4X?tCM$O(J KeySym $@$K$J$j$&$k!#(J
	     */
	    _iml_trace(id,"IML_SEND_EVENT",((XKeyEvent*)operand1)->keycode);
	    if((*s->iml_send_event)(s, rrv, (XKeyEvent *)operand1)<0){
		return(-1);
	    } else {
		return(s->keysym);
	    }
	    break;
	case IML_SET_KEYBOARD_STATE:
	    _iml_trace(id,"IML_SET_KEYBOARD_STATE",0);
	    (*s->iml_set_keyboard_state)(s, rrv, operand1, operand2);
	    break;
	case IML_SEND_RESULTS:
	    _iml_trace(id,"IML_SEND_RESULTS",0);
	    if(s->remainder2){
		_iml_link_inst_tail(rrv, s->remainder2);
		s->remainder2 = NULL ;
	    }
	    (*s->iml_send_results)(s,rrv,(iml_inst *)operand1);
	    break ;
	case IML_COMMIT:
	    _iml_trace(id,"IML_COMMIT",0);
	    (*s->iml_send_commit)(s,rrv);
       default:
	    _iml_trace(id,"IML_UNKNOWN",0);
	    fprintf(stderr, "Warning: imlogic UNKNOWN OPCODE 0x%x\n",opcode);
	    return(IMLERR_UNKNOWN_OPCODE);
	    break;
	}
	return(s->keysym) ;
    }

}
