
/*
*     @(#)iml_cio.c 1.12 91/01/30
*/
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
iml_cio_callme(s, rrv, result)
iml_session_t *s ;
iml_inst **rrv;
Ci_result **result;
{
	static int      first_misc_init;
	int size_of_operand ;
	wchar_t * wc_text ;
	XIMPreeditDrawCallbackStruct *p ;
	Ci_result	*rp = get_rp(result);
	iml_inst *lp ;

if (rp->action._cmd.data.callme.flags == CS_CALLME_USECMD) {
switch(rp->action._cmd.data.callme.request.req_cmd.keyinfo.keycode->code){
#ifdef notdef
	  case CS_CNV_SWITCH:
	    lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
	    lp->opcode = IMM_CONVERSION_OFF ;
	    lp->next = NULL ;
	    s->engine_status &= ~CONVERSION_ON ;
	    _imm_trace("IMM_CONVERSION_OFF",0,0);
	    break;
	  case ENV_SETKEY_CM_ON:{
	      lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
	      lp->opcode = IMM_NOP ;
	      lp->next = NULL ;
	      lp->size_of_operand = 0 ;
	      i = mbtowc(((XKeyEvent *)&(lp->operand))->event ,
			 & rp->ce_rtn_value, 1);
	      ((XKeyEvent *)&(lp->operand))->event = NULL ;	    
	      /*
	       * This is for when called with IMM_GET_KEY .
	       * jle cm returns only 1 letter, so simulate it.
	       */
	      if(s->k){
		  free(s->k);
		  s->k = NULL ;
	      }
	      s->k = (XKeyEvent *)calloc(1, sizeof(wchar_t)*2 + sizeof(XKeyEvent)) ;
	      i = mbtowc(s->event->keycode , & rp->ce_rtn_value, 1);
	      s->event->keycode = NULL ;	/* toriaezu */
	      _imm_trace("IMM_NOP(CM_ON key is stored)",s->event->keycode,s->event->keycode);
	      break;
	  }
#endif
	  case CS_NOP:
	    _imm_trace("IMM_NOP(CS_NOP)",0,0);
	    lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
	    lp->opcode = IMM_NOP ;
	    lp->next = NULL ;
	    break;
	  case CS_RESET_CS:
	    _imm_trace("IMM_RESET",0,0);
	    lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
	    lp->opcode = IMM_RESET ;
	    lp->next = NULL ;
	    break;
          default :
	      _imm_trace("IMM_NOP(UNKNOWN)",0 ,0);
	    lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
	    lp->opcode = IMM_NOP ;
	    lp->next = NULL ;
	    break;
	}/* switch */
}/* CS_CALLME_USECMD */
else{
		_iml_fake_nop_inst(s,&lp);
}
	if(*rrv)
		_iml_link_inst_tail(rrv, lp);
	else
		*rrv = lp;
}
