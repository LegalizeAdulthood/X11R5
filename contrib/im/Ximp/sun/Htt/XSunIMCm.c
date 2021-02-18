#ifndef lint
#ifdef sccs
static char    sccsid[] = "@(#)XSunIMCm.c 1.15 91/09/29";
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
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>

#if XlibSpecificationRelease != 5
#ifdef sun
#include "Sunkeysym.h"
#endif
#endif /* XlibSpecificationRelease */

#include <mle/cm.h>
#include <mle/env.h>
#include <mle/cmkeys.h>
#include <pwd.h>
#include <grp.h>
#include "XSunExt.h"
#include "XSunIMProt.h"
#include "XSunIMPriv.h"
#include "XSunIMMMan.h"
#include "XSunIMCm.h"
#include "XSunIMPub.h"

/*#define PREEDITTRACE*/

#define ATTACHEDMSG
#ifdef ATTACHEDMSG
Private int attachedMsgCount ;
#endif


#ifndef ENV_SET_SIMPLE_MODE
/*
 * This is for ALE support. JLE header file does not contains this
 * declaration.
 * ENV_SET_SIMPLE_MODE((e_char *)ce_string) sets the mode to ce_string,
 * ENV will display the corresponding string for the mode.
 */
#	define ENV_SET_SIMPLE_MODE     117
#endif
/*
 * jle103 session public member functions
 */

Public int _iml_jle103_constructor();

Public int
_iml_jle103_constructor(id, rrv)
int id ;
iml_inst **rrv;
{
    jle103_opsw_init();
    return jle103_open(id, rrv);
}

#define jle103_is_conv_on_key(s,l) ((l)>0 && \
				((s)->conversion_on_key == *((s)->XLookupBuf)))
#define functionKey(keysym,s) 
Private int
jle103_send_event(s, rrv, e)
iml_session_t *s ;
iml_inst **rrv ;
XKeyEvent *e;  /* If this is NULL, this event must be pseudo event */
{
    char localename[LOCALENAMELEN];
    Private struct cm_to_env cte ;
    Private struct cm_to_env *ctep = &cte ;
    Private struct env_to_cm etc ;
    int l ; /* length of Lookuped string */
    XComposeStatus NotSuppoted ;
    
#ifdef ATTACHEDMSG
    if(! attachedMsgCount){
	printf("\n");
	attachedMsgCount++ ;
    }
#endif
    /*
     * jle103 engine は、KeyPress Event しかいらない
     */
    if(e && e->type != KeyPress){
	if(e->type != KeyRelease ){
	    return -1 ;
	}
	/*
	 * しかし無変換の場合は、Keysym くらい返してあげよう。
	 */
	if(! (s->public_status & IMLSTATUS_Henkan_Mode)){
	    l=XLookupString(e,s->XLookupBuf,s->XLookupBuflen,&s->keysym,&NotSuppoted);
	    if(l<=0) {
		if(s->keysym == NoSymbol){
		    return -1;
		} else {
		    return 0;
		}
	    } else {
		/*
		 * 文字列が見つかるとやっかいなので無視
		 */
		return -1 ;
	    }
	} else {
	    return -1 ;
	}
    }
    if(s->remainder){
	*rrv = s->remainder ;
	s->remainder = NULL ;
	_iml_post_proc(s, rrv);
	return 0;
    } else {
	_iml_delete(s);
    }
    if(e){
	s->event = e ; /* save KeyEvent */
    } else {
	e = s->event ; /* May NOT be needed: restore KeyEvent */
    }
    l=XLookupString(e,s->XLookupBuf,s->XLookupBuflen,&s->keysym,&NotSuppoted);
    if(l<=0) {
	/*
	 * XLookupString で looup できないなら、仮名かもしれない。
	 */
	int tl=_iml_lookupKanaString(s->XLookupBuf,s->XLookupBuflen,&s->keysym);
	if(tl>0)l=tl;
    }
    if(! (s->public_status & IMLSTATUS_Henkan_Mode)){
	/*
	 * 無変換の場合
	 */
	iml_inst *rv = *rrv ;
	if((s->keysym == XK_Henkan) || jle103_is_conv_on_key(s,l)){
	    jle103_make_conversion_on(s, rrv);
	    s->keysym = NoSymbol ;
	    return 0 ;
	} 
	if(l<=0) {
	    /*
	     * _iml_lookupKanaString でも string にならず、さらに変換キーなど
	     * の日本語特殊キーでもなく、変換 on などのエンジン固有のキーでも
	     * ないのなら、もうあきらめて nop にする。
	     */
	    _iml_fake_nop_inst(s, rrv, e);
	    if(s->keysym == NoSymbol){
		return -1;
	    } else {
		return 0;
	    }
	}
	/*
	 * すべて、確定文字列として返す
	 */
	_imm_trace("IMM_COMIT(off)",rrv,*rrv);
	return 	_iml_make_commit_inst(s, rrv, l, s->XLookupBuf) ;
    }
    /*
     * 変換 ON の時
     */
    switch(s->keysym){
      case XK_Shift_L:
      case XK_Shift_R:
      case XK_Control_L:
      case XK_Control_R:
      case XK_Caps_Lock:
      case XK_Shift_Lock:
      case XK_Meta_L:
      case XK_Meta_R:
      case XK_Alt_L:
      case XK_Alt_R:
      case XK_Super_L:
      case XK_Super_R:
      case XK_Hyper_L:
      case XK_Hyper_R:
	/*
	 * Only modifier key is coming..
	 */
	_iml_fake_nop_inst(s, rrv, e);
	return -1;
	/*
	 * Function Key Operations
	 */
      case XK_Multi_key: /* 0xFF20  Multi-key character compose */
	functionKey( XK_Multi_key, "XK_Multi_key");
	break;
      case XK_Kanji: /* 0xFF21	Kanji, Kanji convert */
	functionKey( XK_Kanji, "XK_Kanji");
	etc.ec_key = 0xb002 | CM_BF_START ;
	goto VALID;
	break;
      case XK_Muhenkan: /* 0xFF22   Cancel Conversion */
	functionKey( XK_Muhenkan, "XK_Muhenkan");
	break;
      case XK_Henkan_Mode: /* 0xFF23  Start/Stop Conversion */
	functionKey( XK_Henkan_Mode, "XK_Henkan_Mode");
	etc.ec_key = 0xb003 | CM_BF_START ;
	goto VALID;
	break;
      case XK_Romaji: /* 0xFF24   to Romaji */
	functionKey( XK_Romaji, "XK_Romaji");
	break;
      case XK_Hiragana: /* 0xFF25   to Hiragana */
	functionKey( XK_Hiragana, "XK_Hiragana");
	break;
      case XK_Katakana: /* 0xFF26   to Katakana */
	functionKey( XK_Katakana, "XK_Katakana");
	break;
      case XK_Hiragana_Katakana: /* 0xFF27  Hiragana/Katakana toggle */
	functionKey( XK_Hiragana_Katakana, "XK_Hiragana_Katakana");
	break;
      case XK_Zenkaku: /* 0xFF28   to Zenkaku */
	functionKey( XK_Zenkaku, "XK_Zenkaku");
	break;
      case XK_Hankaku: /* 0xFF29   to Hankaku */
	functionKey( XK_Hankaku, "XK_Hankaku");
	break;
      case XK_Zenkaku_Hankaku: /* 0xFF2A   Zenkaku/Hankaku toggle */
	functionKey( XK_Zenkaku_Hankaku, "XK_Zenkaku_Hankaku");
	break;
      case XK_Touroku: /* 0xFF2B   Add to Dictionary */
	functionKey( XK_Touroku, "XK_Touroku");
	break;
      case XK_Massyo: /* 0xFF2C   Delete from Dictionary */
	functionKey( XK_Massyo, "XK_Massyo");
	break;
      case XK_Kana_Lock: /* 0xFF2D  Kana Lock */
	functionKey( XK_Kana_Lock, "XK_Kana_Lock");
	break;
      case XK_Kana_Shift: /* 0xFF2E  Kana Shift */
	functionKey( XK_Kana_Shift, "XK_Kana_Shift");
	break;
      case XK_Eisu_Shift: /* 0xFF2F  Alphanumeric Shift */
	functionKey( XK_Eisu_Shift, "XK_Eisu_Shift");
	break;
      case XK_Eisu_toggle: /* 0xFF30  Alphanumeric toggle */
	functionKey( XK_Eisu_toggle, "XK_Eisu_toggle");
	break;
      case XK_Home: /* 	0xFF50 */
	functionKey( XK_Home, "XK_Home");
	break;
      case XK_Left: /* 	0xFF51 Move left, left arrow */
	functionKey( XK_Left, "XK_Left");
	break;
      case XK_Up: /* 	0xFF52	/* Move up, up arrow */
	functionKey( XK_Up, "XK_Up");
	break;
      case XK_Right: /* 0xFF53	/* Move right, right arrow */
	functionKey( XK_Right, "XK_Right");
	break;
      case XK_Down: /* 	0xFF54	/* Move down, down arrow */
	functionKey( XK_Down, "XK_Down");
	break;
      case XK_Prior: /* 0xFF55	/* Prior, previous */
	functionKey( XK_Prior, "XK_Prior");
	break;
      case XK_Next: /* 	0xFF56	/* Next */
	functionKey( XK_Next, "XK_Next");
	break;
      case XK_End: /* 	0xFF57	/* EOL */
	functionKey( XK_End, "XK_End");
	break;
      case XK_Begin: /* 0xFF58	/* BOL */
	functionKey( XK_Begin, "XK_Begin");
	break;
      case XK_Select: /* 0xFF60	 Select, mark */
	functionKey( XK_Select, "XK_Select");
	break;
      case XK_Print: /* 0xFF61 */
	functionKey( XK_Print, "XK_Print");
	break;
      case XK_Execute: /* 0xFF62  Execute, run, do */
	functionKey( XK_Execute, "XK_Execute");
	etc.ec_key = 0xb001 | CM_BF_START ;
	goto VALID;
	break;
      case XK_Insert: /* 0xFF63	 Insert, insert here */
	functionKey( XK_Insert, "XK_Insert");
	break;
      case XK_Undo: /* 	0xFF65	Undo, oops */
	functionKey( XK_Undo, "XK_Undo");
	break;
      case XK_Redo: /* 	0xFF66	redo, again */
	functionKey( XK_Redo, "XK_Redo");
	break;
      case XK_Menu: /* 	0xFF67 */
	functionKey( XK_Menu, "XK_Menu");
	break;
      case XK_Find: /* 	0xFF68	 Find, search */
	functionKey( XK_Find, "XK_Find");
	break;
      case XK_Cancel: /* 0xFF69	Cancel, stop, abort, exit */
	functionKey( XK_Cancel, "XK_Cancel");
	break;
      case XK_Help: /* 	0xFF6A	Help, ? */
	functionKey( XK_Help, "XK_Help");
	break;
      case XK_Break: /* 0xFF6B */
	functionKey( XK_Break, "XK_Break");
	break;
      case XK_Mode_switch: /* 0xFF7E Character set switch */
	/* case XK_script_switch: 0xFF7E Alias for mode_switch */
	functionKey( XK_Mode_switch, "XK_Mode_switch");
	/*
	 * We should not pass this to CM.
	 */
        strcpy(localename, setlocale(LC_CTYPE, (char *)NULL));
	if ( strncmp(localename, "korean", 6) == 0) {
	    etc.ec_key = 0xb004 | CM_BF_START ;
	    goto VALID;
	}

#ifdef notdef
	etc.ec_key = 0xb003 | CM_BF_START ;
	goto VALID;
#endif
	break;
      case XK_Num_Lock: /* 0xFF7F */
	functionKey( XK_Num_Lock, "XK_Num_Lock");
	break;
      case XK_KP_Space: /* 0xFF80	 space */
	functionKey( XK_KP_Space, "XK_KP_Space");
	break;
      case XK_KP_Tab: /* 0xFF89 */
	functionKey( XK_KP_Tab, "XK_KP_Tab");
	break;
      case XK_KP_Enter: /* 0xFF8D enter */
	functionKey( XK_KP_Enter, "XK_KP_Enter");
	break;
      case XK_KP_F1: /* 0xFF91	PF1, KP_A, ... */
	functionKey( XK_KP_F1, "XK_KP_F1");
	break;
      case XK_KP_F2: /* 0xFF92 */
	functionKey( XK_KP_F2, "XK_KP_F2");
	break;
      case XK_KP_F3: /* 0xFF93 */
	functionKey( XK_KP_F3, "XK_KP_F3");
	break;
      case XK_KP_F4: /* 0xFF94 */
	functionKey( XK_KP_F4, "XK_KP_F4");
	break;
      case XK_KP_Equal: /* 0xFFBD  equals */
	functionKey( XK_KP_Equal, "XK_KP_Equal");
	break;
      case XK_KP_Multiply: /* 0xFFAA */
	functionKey( XK_KP_Multiply, "XK_KP_Multiply");
	break;
      case XK_KP_Add: /* 0xFFAB */
	functionKey( XK_KP_Add, "XK_KP_Add");
	break;
      case XK_KP_Separator: /* 0xFFAC separator, often comma */
	functionKey( XK_KP_Separator, "XK_KP_Separator");
	break;
      case XK_KP_Subtract: /* 0xFFAD */
	functionKey( XK_KP_Subtract, "XK_KP_Subtract");
	break;
      case XK_KP_Decimal: /* 0xFFAE */
	functionKey( XK_KP_Decimal, "XK_KP_Decimal");
	break;
      case XK_KP_Divide: /* 0xFFAF */
	functionKey( XK_KP_Divide, "XK_KP_Divide");
	break;
      case XK_KP_0: /* 	0xFFB0 */
	functionKey( XK_KP_0, "XK_KP_0");
	break;
      case XK_KP_1: /* 	0xFFB1 */
	functionKey( XK_KP_1, "XK_KP_1");
	break;
      case XK_KP_2: /* 	0xFFB2 */
	functionKey( XK_KP_2, "XK_KP_2");
	break;
      case XK_KP_3: /* 	0xFFB3 */
	functionKey( XK_KP_3, "XK_KP_3");
	break;
      case XK_KP_4: /* 	0xFFB4 */
	functionKey( XK_KP_4, "XK_KP_4");
	break;
      case XK_KP_5: /* 	0xFFB5 */
	functionKey( XK_KP_5, "XK_KP_5");
	break;
      case XK_KP_6: /* 	0xFFB6 */
	functionKey( XK_KP_6, "XK_KP_6");
	break;
      case XK_KP_7: /* 	0xFFB7 */
	functionKey( XK_KP_7, "XK_KP_7");
	break;
      case XK_KP_8: /* 	0xFFB8 */
	functionKey( XK_KP_8, "XK_KP_8");
	break;
      case XK_KP_9: /*  0xFFB9 */
	functionKey( XK_KP_9, "XK_KP_9");
	break;
      case XK_F1:
	functionKey(XK_F1,"XK_F1");
	etc.ec_key = 0x1 | CM_TF_START ;
	goto VALID;
	break;
      case XK_F2:
	functionKey(XK_F2,"XK_F2");
	etc.ec_key = 0x2 | CM_TF_START ;
	goto VALID;
	break;
      case XK_F3:
	functionKey(XK_F3,"XK_F3");
	etc.ec_key = 0x3 | CM_TF_START ;
	goto VALID;
	break;
      case XK_F4:
	functionKey(XK_F4,"XK_F4");
	etc.ec_key = 0x4 | CM_TF_START ;
	goto VALID;
	break;
      case XK_F5:
	functionKey(XK_F5,"XK_F5");
	etc.ec_key = 0x5 | CM_TF_START ;
	goto VALID;
	break;
      case XK_F6:
	functionKey(XK_F6,"XK_F6");
	etc.ec_key = 0x6 | CM_TF_START ;
	goto VALID;
	break;
      case XK_F7:
	functionKey(XK_F7,"XK_F7");
	etc.ec_key = 0x7 | CM_TF_START ;
	goto VALID;
	break;
      case XK_F8:
	functionKey(XK_F8,"XK_F8");
	etc.ec_key = 0x8 | CM_TF_START ;
	goto VALID;
	break;
      case XK_F9:
	functionKey(XK_F9,"XK_F9");
	etc.ec_key = 0x9 | CM_TF_START ;
	goto VALID;
	break;
      case XK_F10:
	functionKey(XK_F10,"XK_F10");
	etc.ec_key = 0xa | CM_TF_START ;
	goto VALID;
	break;
#if XlibSpecificationRelease != 5
#ifdef sun
      case XK_SunF36: 
#endif /* sun */
#endif /* XlibSpecificationRelease */
      case XK_F11: /* XK_L1 */
	functionKey(XK_F11,"XK_F11");
	etc.ec_key = 0xb | CM_TF_START ;
	goto VALID;
	break;
#if XlibSpecificationRelease != 5
#ifdef sun
      case XK_SunF37: 
#endif /* sun */
#endif /* XlibSpecificationRelease */
      case XK_F12: /* XK_L2 */
	functionKey(XK_F12,"XK_F12");
	goto VALID;
	break;
      case XK_F13: /* XK_L3 */
	functionKey(XK_F13,"XK_F13");
	break;
      case XK_F14: /* XK_L4 */
	functionKey(XK_F14,"XK_F14");
	break;
      case XK_F15: /* XK_L5 */
	functionKey(XK_F15,"XK_F15");
	break;
      case XK_F16: /* XK_L6 */
	functionKey(XK_F16,"XK_F16");
	break;
      case XK_F17: /* XK_L7 */
	functionKey(XK_F17,"XK_F17");
	break;
      case XK_F18: /* XK_L8 */
	functionKey(XK_F18,"XK_F18");
	break;
      case XK_F19: /* XK_L9 */
	functionKey(XK_F19,"XK_F19");
	break;
      case XK_F20: /* XK_L10 */
	functionKey(XK_F20,"XK_F20");
	break;
      case XK_F21: /* XK_R1 */
	functionKey(XK_F21,"XK_F21");
	break;
      case XK_F22: /* XK_R2 */
	functionKey(XK_F22,"XK_F22");
	break;
      case XK_F23: /* XK_R3 */
	functionKey(XK_F23,"XK_F23");
	break;
      case XK_F24: /* XK_R4 */
	functionKey(XK_F24,"XK_F24");
	break;
      case XK_F25: /* XK_R5 */
	functionKey(XK_F25,"XK_F25");
	break;
      case XK_F26: /* XK_R6 */
	functionKey(XK_F26,"XK_F26");
	break;
      case XK_F27: /* XK_R7 */
	functionKey(XK_F27,"XK_F27");
	break;
      case XK_F28: /* XK_R8 */
	functionKey(XK_F28,"XK_F28");
	break;
      case XK_F29: /* XK_R9 */
	functionKey(XK_F29,"XK_F29");
	break;
      case XK_F30: /* XK_R10 */
	functionKey(XK_F30,"XK_F30");
	break;
      case XK_F31: /* XK_R11 */
	functionKey(XK_F31,"XK_F31");
	break;
      case XK_F32: /* XK_R12 */
	functionKey(XK_F32,"XK_F32");
	break;
      case XK_R13: /* XK_F33 */
	functionKey(XK_R13,"XK_R13");
	break;
      case XK_F34: /* XK_R14 */
	functionKey(XK_F34,"XK_F34");
	break;
      case XK_F35: /* XK_R15 */
	functionKey(XK_F35,"XK_F35");
	break;
      default:	
	etc.ec_key = s->XLookupBuf[0]; /* どうして cm はこんな仕様なんだ */
VALID:
	etc.ec_operation = CM_SIMPLE_EVENT ;
	etc.ec_next = NULL ;
	ctep = cm_put(s->language_engine, s->session_id, &etc);
	eval_cm_to_env_packet(s, ctep, rrv, e);
	return 0;
    }
    _iml_fake_nop_inst(s, rrv, e);
    return -1 ;
}

Private void
jle103_conversion_on(s, rrv)
iml_session_t *s;
iml_inst **rrv ;
{
    if(s->remainder){
	*rrv = s->remainder ;
	s->remainder = NULL ;
	return;
    }
    jle103_make_conversion_on(s, rrv);
}

Private void
jle103_send_results(s, rrv, results)
iml_session_t *s;
iml_inst **rrv ;
iml_inst *results ;
{
    XIMLookupStartCallbackStruct *ls;
    XIMLookupProcessCallbackStruct *lp;
    Private struct cm_to_env cte ;
    int *i ;
    
    if(results->opcode & IMM_CB_RESULT_REQUIRED){
	results->opcode &= ~IMM_CB_RESULT_REQUIRED ;
	switch(results->opcode){
	  case IMM_LOOKUP_START:
	    ls = (XIMLookupStartCallbackStruct *)& results->operand ;
	    if(s->WhoIsMaster == HasNotBeenNegotiated) {
	      s->WhoIsMaster =  ls->WhoIsMaster ;
	    } else {
		break;
	    }
	    /*
	     * Obey CBPreferrence
	     */
	    s->XIMPreference = s->CBPreference ;
	    break;
	  case IMM_LOOKUP_PROCESS:
	    lp = (XIMLookupProcessCallbackStruct *)& results->operand ;
	    if ((lp->index_of_choice_selected == XIM_UNDETERMINED) ||
		(lp->index_of_choice_selected == XIM_UNKNOWN_KEYSYM)) {
		break ;
	    }
	    jle103_set_choice(s, rrv, lp->index_of_choice_selected);
	    return;
	case IMM_PREEDIT_START:
	    s->PreEditBufferSizeInCallback = *(int *)&results->operand ;
	    break;
	  default:
	    break;
	}
	*rrv = _iml_make_nop_inst(s);
	return ;
    }
}
Private int
jle103_open(id, rrv)
int id ;
iml_inst **rrv;
{
    Private int jle103_create_session(/* int id ,  iml_inst **rrv ,
			         iml_session_t *s, Bool is_recycled */);
    extern int _iml_construct_session(/* int id , iml_inst **rrv ,int size,
				      int (*constructor)() */);

    return _iml_construct_session(id,rrv,sizeof(iml_session_t),jle103_create_session);
}

Private void
jle103_make_conversion_on(s, rrv)
iml_session_t *s ;
iml_inst **rrv ;
{
    struct cm_to_env *ctep ;
    struct env_to_cm *etcp ;
    iml_inst *lp ;
    
    s->public_status |= IMLSTATUS_Henkan_Mode ;
    etcp = (struct env_to_cm *)_iml_new(s,
					  sizeof(struct env_to_cm));
    bzero(etcp, sizeof(struct env_to_cm));
    etcp->ec_operation = CM_CMON ;
    etcp->ec_next = NULL ;
    ctep = cm_put(s->language_engine, s->session_id, etcp);
    
    lp = _iml_make_imm_set_status_inst(s);
    _iml_link_inst_tail(rrv, lp);
    eval_cm_to_env_packet(s, ctep, rrv, 0);
}
Private
jle103_create_session(cm_id, rrv, s, is_recycled)
int cm_id ;
iml_inst **rrv;
iml_session_t *s ;
Bool is_recycled ;
{
    struct cm_initstruct cm_initvalue ;
    struct cm_to_env *cte ;
    int session_id ;
    char buf[100];
    char *mode_path,*getenv();
    construct_cm_initial_data(& cm_initvalue);
    session_id = cm_open(cm_id, & cm_initvalue, &cte);
    if(session_id == -1 ) {
	return(-1);
    }
    s->language_engine = cm_id ;
    s->session_id = session_id ;
 
    if ((mode_path = getenv("MLEPATH")) != (char *)0) {
        struct stat statbuf;
        strcpy(&buf[0], mode_path);
	/*
	 * FIX_ME: "japanese" is hard coded!
	 */
        strcat(&buf[0], "/japanese/se/sunview/mode.dat");
        if (stat(&buf[0], &statbuf) < 0) {
            goto DEFALT_MODE_DOT_DAT;
	}
    } else {
	    /*
	     * FIX_ME: File name is hardcoded which include the
	     * "japanese".
	     */
DEFALT_MODE_DOT_DAT:
            strcpy(buf, (unsigned char *)JLE103_STATUS_STRING_FILE);
    }
    /*
     * constructing member functions
     */
    s->iml_open = jle103_open ;
    s->iml_close = jle103_close ;
    s->iml_reset = jle103_reset ;
    s->iml_send_event = jle103_send_event ;
    s->iml_conversion_on = jle103_conversion_on ;
    s->iml_send_results = jle103_send_results ;
    s->iml_send_commit = jle103_send_commit ;
    
    init_status_string(s,&buf[0]);
    eval_cm_to_env_packet(s, cte, rrv, s->event);
    return(s->session_id);
}

Private int
construct_cm_initial_data(cm_initp)
struct cm_initstruct *cm_initp;
{
    struct group   *gp;
    struct passwd  *pp;
    Private char     host_name[64];
    
    cm_initp->env_value = (e_char *) NULL;
    cm_initp->usr_auth_info.uid = getuid();
    cm_initp->usr_auth_info.gid = getgid();
    cm_initp->usr_auth_info.hid = gethostid();
    pp = getpwuid(getuid());
    if (pp != (struct passwd *)NULL)
      cm_initp->usr_auth_info.user_name = pp->pw_name;
    else
      cm_initp->usr_auth_info.user_name = (char *)NULL;
    gp = getgrgid(getgid());
    if (gp != (struct group *)NULL)
      cm_initp->usr_auth_info.grp_name = gp->gr_name;
    else
      cm_initp->usr_auth_info.grp_name = (char *)NULL;
    gethostname(host_name, sizeof(host_name));
    cm_initp->usr_auth_info.host_name = host_name;
}

Private 
env_commit(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);
    
    lp = make_preedit_erase_inst(s);
    *rrv = lp;
    lp = make_preedit_done_inst(s);
    _iml_link_inst_tail(rrv, lp);
    
    /*
     * 確定文字列としては、現状 multi byte のみサポートしている
     */
    _iml_make_commit_inst(s, &lp, strlen(rp->ce_string), rp->ce_string);
    _iml_link_inst_tail(rrv, lp);
}
Private iml_inst *
make_preedit_erase_inst(s)
iml_session_t *s;
{
    return _iml_make_preedit_erase_inst(s);
}

Private iml_inst *
make_preedit_done_inst(s)
iml_session_t *s;
{
    return _iml_make_preedit_done_inst(s);
}
Private iml_inst *
make_preedit_start_inst(s)
iml_session_t *s;
{
    return _iml_make_preedit_start_inst(s);
}
Private iml_inst *
make_nop_inst(s)
iml_session_t *s;
{
    return _iml_make_nop_inst(s);
}
Private iml_inst *
make_lookup_start_inst(s)
iml_session_t *s;
{
    return _iml_make_lookup_start_inst(s);
}
Private iml_inst *
make_status_start_inst(s)
iml_session_t *s;
{
    return _iml_make_status_start_inst(s);
}
Private 
env_set_current_region(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);
    int i = _iml_get_current_region(s);
    
    switch(rp->ce_region){
    case INTERM_REGION:
	if(i == PREEDIT ){
	    lp = make_nop_inst(s);
	} else {
	    lp = make_preedit_start_inst(s);
	}
	break;
    case MISC_REGION:
	/*
	 * postpone start.
	 */
	_imm_trace("IMM_SET_REGION(AUX)changed to NOP",0,0);
	lp = make_nop_inst(s);
	break ;
    case SELECT_REGION:
	if(i == LOOKUP ) {
	    lp = make_nop_inst(s);
	} else {
	    lp = make_lookup_start_inst(s);
	}
	break ;
    case MODE_REGION:
	if(i == STATUS ) {
	    lp = make_nop_inst(s);
	} else {
	    lp = make_status_start_inst(s);
	}
	break;
    }
    lp->next = NULL ;
    *rrv = lp;
}

Private 
env_cm_off(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);
    iml_status_t *status ;
    
    lp = (iml_inst*)_iml_new(s, sizeof(iml_inst) + sizeof(iml_status_t));
    lp->opcode = IMM_SET_STATUS ;
    lp->next = NULL ;
    s->public_status &= ~IMLSTATUS_Henkan_Mode;
    status =  (iml_status_t *) &(lp->operand);
    *status = s->public_status ;
    *rrv = lp ;
    _imm_trace("IMM_SET_STATUS",0,*status);
}
Private 
env_setkey_cm_on(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);
    _iml_fake_nop_inst(s,&lp); 
    /*
     * Since every thing is done inside imlogic, IMM_GET_KEY is
     * now obsolete. conv_on_key is stored as mbchar in session.
     */
    s->conversion_on_key = rp->ce_rtn_value;/* In multi byte form */
    _imm_trace("IMM_NOP(ON key is stored)",s->conversion_on_key,0);
}
Private 
env_nop(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);

    _imm_trace("IMM_NOP(ENV_NOP)",0,0);
    lp = make_nop_inst(s);
    *rrv = lp ;
}
Private 
env_interm_reset(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);
/*
 * NOT IMPLEMENTED YET
 */
    _imm_trace("IMM_RESET",0,0);
    lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
    lp->opcode = IMM_RESET ;
    lp->next = NULL ;
    *rrv = lp ;
}    
Private iml_inst *
make_preedit_draw_inst(s, rp)
iml_session_t *s;
struct cm_to_env *rp ;
{
    iml_inst *lp ;
    XIMPreeditDrawCallbackStruct *p ;

#ifdef notdef /* before post public review draft */
    wchar_t *pws = s->PreEditTextInfo.text.string.wide_char;/* prev str */
    int      pwl = s->PreEditTextInfo.text.length ;         /* prev len */
    XIMFeedback *pfb = s->PreEditTextInfo.text.feedback ;   /* prev fb */
#endif  
    wchar_t *pws = s->PreEditTextInfo.text->string.wide_char;/* prev str */
    int      pwl = s->PreEditTextInfo.text->length ;         /* prev len */
    XIMFeedback *pfb = s->PreEditTextInfo.text->feedback ;   /* prev fb */
    wchar_t *cws ;                                          /* current wstr */
    int      cwl ;                                          /* current len */
    char    *cmbs = (char *) rp->ce_text ;                  /* current mbstr */
    int      cmbl = strlen(cmbs);                           /* current mblen */
    XIMFeedback *cfb ;                                      /* current fb */
    int si ;                                            /* str index */
    int sri ;                                           /* str reverse index */
    int fbi ;                                           /* fb index */
    int fbri ;                                          /* fb reverse index */
    int i ;                                             /* index */
    int ri;						/* reverse index */

    cws = (wchar_t*)_iml_new(s, (cmbl+1)*sizeof(wchar_t));
    mbstowcs(cws,cmbs,IML_MAX_SLOT_SIZE);
    cwl = wslen(cws);
    if(cwl <= 0){
	return(make_preedit_erase_inst(s, rp));
    }
    lp = (iml_inst*)_iml_new(s,sizeof(iml_inst)+sizeof(XIMPreeditDrawCallbackStruct));
    lp->opcode = IMM_PREEDIT_DRAW ;
    lp->next = NULL ;
    cfb = _iml_new(s, (cmbl + 1)*sizeof(XIMFeedback));
    jle103_attr2feedback(cfb, rp->ce_text_attr, cwl, cmbs);
    for(si = 0 ; si < pwl ; si++) if(pws[si] != cws[si]) break ;
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
#ifdef notdef /* before post public review draft */
    p->text.encoding_is_wchar = True ;
    p->text.feedback = cfb + i ;
    p->text.string.wide_char = cws +i ;
    p->text.length = cwl - i;
#endif  
    
    p->chg_first = i ;
    p->chg_length = pwl - i ;
    p->caret = cwl - 1;

#ifdef REVERSEOPTIMIZE
    /*
     * Not sure it works perfectly
     */
    if(cwl && (pwl == cwl) && p->text){
	register gain ;
	for(sri = pwl-1 ; sri > 0 ; sri--) if(pws[sri] != cws[sri]) break ;
	for(fbri = pwl-1 ; fbri > 0 ; fbri--) if(pfb[fbri] != cfb[fbri]) break ;
	ri = max(sri, fbri);
	gain = pwl - 1 - ri ;
	if((gain > 0) && (p->chg_length > gain) && (p->text->length > gain)){
	    p->chg_length -= gain ;
	    p->text->length -= gain ;
	}
    }
#endif    

#if XlibSpecificationRelease != 5
#ifdef sun
    p->text.visible_pos = _iml_calc_visible_pos(rp->ce_v_pos, cmbs, cwl);
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
#endif /* sun */
#endif /* XlibSpecificationRelease */

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
    
    _imm_trace("IMM_PREEDIT_DRAW(Draw All)",ws,p->text->length);


#ifdef PREEDITTRACE
    fprintf(stderr,"caret: %d ", p->caret);
    fprintf(stderr,"chg_first: %d ", p->chg_first);
    fprintf(stderr,"chg_length: %d\n", p->chg_length);
    if(p->text){
	fprintf(stderr,"text->length: %d \n", p->text->length);
	if(p->text->feedback){
	    fprintf(stderr,"text->feedback:\n");
	    _iml_xdmem(p->text->feedback, sizeof(XIMFeedback)*(p->text->length));
	} else {
	    fprintf(stderr,"text->feedback is NULL\n");
	}
	fprintf(stderr,"text->encoding_is_wchar: %d\n", p->text->encoding_is_wchar);
	fprintf(stderr,"text->string: in wcs|%ws|\n", p->text->string.wide_char);
	fprintf(stderr,"text->string: in mbs|%s|\n", p->text->string.wide_char);
	_iml_xdmem(p->text.string.wide_char, length);
    }
#endif    

    return lp;
}

Private 
env_interm(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);
    
    lp = make_preedit_draw_inst(s, rp);
    if(_iml_get_current_region(s) != PREEDIT ){
	*rrv = make_preedit_start_inst(s);
	_iml_link_inst_tail(rrv, lp);
    } else {
	*rrv = lp ;
    }
    _imm_trace("IMM_SET_REGION(pseudo PREEDIT)",0,0);
}
Private 
env_m_interactive(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);
    
    lp = make_preedit_draw_inst(s, rp);
    lp->opcode = IMM_AUX_DRAW ;
    _imm_trace("IMM_PREEDIT_DRAW(Draw All)",wc_text,p->text.length);
    *rrv = lp ;
}
Private 
env_select_reset(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);
/*
 * NOT IMPLEMENTED YET
 */
    _imm_trace("IMM_RESET",0,0);
    lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
    lp->opcode = IMM_RESET ;
    lp->next = NULL ;
    *rrv = lp ;
}    
Private iml_inst *
make_lookup_draw_inst(s, rp)
iml_session_t *s;
struct cm_to_env *rp ;
{
    iml_inst *lp ;
    register XIMLookupDrawCallbackStruct *ld ;
    register XIMText *t ;
    register wchar_t *w ;
    register e_char **e ;
    register XIMFeedback *a ;
    register l ;
    register i ;
    
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
	ld->n_choices = rp->ce_ncandidate ;
	ld->index_of_first_candidate = 0 ;
	ld->index_of_last_candidate = ld->n_choices ;
	ld->choices = (XIMChoiceObject *)_iml_new2(s,
			   ld->n_choices * sizeof(XIMChoiceObject)) ;
	e = rp->ce_candidate ;
	ld->max_len = 0 ;
	for(i = 0 ; i < ld->n_choices ; i++){
	    l = (strlen(e[i])+sizeof(char))*sizeof(wchar_t) ;
	    w = (wchar_t *)_iml_new2(s, l);
	    mbstowcs(w, e[i], l);
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
env_select(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);
    
    lp = make_lookup_draw_inst(s, rp);
    if(_iml_get_current_region(s) != LOOKUP ){
	*rrv = make_lookup_start_inst(s);
	_iml_link_inst_tail(rrv, lp);
    } else {
	*rrv = lp ;
    }
    _imm_trace("PSEUDO LOOKUP",0,0);
}

Private 
env_select_end(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);

    lp = _iml_make_lookup_done_inst(s);
    _iml_pop_current_region(s);
    *rrv = lp ;
}
Private iml_inst *
make_lookup_process_inst(s, rp, results_required, controll)
iml_session_t *s;
struct cm_to_env *rp ;
Bool results_required ;
int controll ; /* commit/next/prev: NOT IMPLEMENTED YET */
{
    return _iml_make_lookup_process_inst(s, results_required, controll);
}
Private 
env_select_next(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);

    lp = make_lookup_process_inst(s, rp, False);
    if(_iml_get_current_region(s) != LOOKUP ){
	*rrv = make_lookup_start_inst(s);
	_iml_link_inst_tail(rrv, lp, False);
    } else {
	*rrv = lp ;
    }
}
Private 
env_select_prev(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);

    lp = make_lookup_process_inst(s, rp);
    if(_iml_get_current_region(s) != LOOKUP ){
	*rrv = make_lookup_start_inst(s);
	_iml_link_inst_tail(rrv, lp, False);
    } else {
	*rrv = lp ;
    }
}
Private 
env_select_commit(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);
    
    lp = make_lookup_process_inst(s, rp, True);
    _imm_trace("IMM_LOOKUP_PROCESS",0,0);
    *rrv = lp ;
}
Private 
_env_set_mode(s, rrv, ctep, simple)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
Bool simple;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);
    XIMStatusDrawCallbackStruct *q ;
    XIMText *t ;
    wchar_t *wc_text ;
    int size_of_operand;
    
    *rrv = make_status_start_inst(s);
    size_of_operand = DEFAULTStatusStringRemainBufferSize ;
    wc_text = s->StatusStringRemainBuffer.buf ;
    lp = (iml_inst*)_iml_new(s,sizeof(iml_inst)+sizeof(XIMStatusDrawCallbackStruct));
    t = (XIMText *)_iml_new(s, sizeof(XIMText));
    lp->opcode = IMM_STATUS_DRAW ;
    lp->next = NULL ;
    q = (XIMStatusDrawCallbackStruct *)&lp->operand ;
    jle103_make_status_string(s, wc_text, (unsigned char *)rp->ce_mode_list, simple);
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
Private
env_set_mode(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    _env_set_mode(s, rrv, ctep, 0);
}
Private
env_set_smode(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    _env_set_mode(s, rrv, ctep, 1);
}
Private 
env_misc_reset(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);
/*
 * NOT IMPLEMENTED YET
 */
    _imm_trace("IMM_NOP(ENV_MISC_RESET)",0,0);
    lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
    lp->opcode = IMM_AUX_DONE ;
    lp->next = NULL ;
    *rrv = lp ;
}    
Private 
env_m_label(s, rrv, ctep)
iml_session_t *s;
iml_inst **rrv;
struct cm_to_env **ctep;
{
    iml_inst *lp ;
    struct cm_to_env *rp = get_cte(ctep);
    XIMAuxStartCallbackStruct *q ;
    XIMText *t;
    int l ;

    _imm_trace("IMM_AUX_START(ENV_M_LABEL)",0,0);
    _iml_put_current_region(s, AUX) ;
    lp=(iml_inst*)_iml_new(s, sizeof(iml_inst)+
			   sizeof(XIMAuxStartCallbackStruct));
    lp->opcode = IMM_AUX_START ;
    lp->next = NULL ;
    q = (XIMAuxStartCallbackStruct *)&lp->operand ;
    l = strlen(rp->ce_misc_text)*sizeof(wchar_t);
    q->event = s->event ;
    t = q->label = _iml_new(s, sizeof(XIMText));
    t->encoding_is_wchar = True ;
    t->string.wide_char = _iml_new(s, l);
    t->length=mbstowcs(t->string.wide_char,rp->ce_misc_text,l)*sizeof(wchar_t);
    t->feedback = _iml_new(s, t->length);
    bzero(t->feedback, t->length);
    t = q->text = _iml_new(s, sizeof(XIMText));	      
    t->length = 0 ;
    *rrv = lp ;
}



/*
 * get cm_to_env *cte from cm_to_env link chain and remove from the chain
 */
Private struct cm_to_env *
get_cte(ctep)
struct cm_to_env **ctep;
{
    struct cm_to_env *rp = NULL ;
    if(ctep){
	rp = *ctep ;
	if (rp != NULL){
	    *ctep = rp->ce_next ;
	} else {
	    *ctep = NULL ;
	}
	return rp ;
    } 
    return NULL;
}

Private (*opsw[MAX_ENV_OPCODE])();

Private iml_inst **
eval_it(s, cte, rrv)
iml_session_t *s ;
register struct cm_to_env *cte;
iml_inst **rrv;
{
    struct cm_to_env *rp = cte ;
    iml_inst *lp = 0;
    int op ; 

    while( rp ) {
	op = rp->ce_operation;
	(*opsw[op])(s, &lp , &rp);
	if(*rrv){
	    register iml_inst *plp = *rrv ;
	    
	    while(plp->next){
		plp = plp->next ;
	    }
	    plp->next = lp ;
	    while(plp->next){
		plp = plp->next ;
	    }
	} else {
	    *rrv = lp ;
	}
    }
    return(&lp->next); /* iml_inst lin last point */
}
Private void
jle103_make_status_string(s,ws,s1,simple)
iml_session_t *s ; 
wchar_t *ws ; /* This sould be s->StatusStringRemainBuffer.buf */
unsigned char *s1 ;
int simple ;
{
    wchar_t	*list[MAXSTATUS];
    wchar_t	*name[MAXSTATUS];
    wchar_t	*value[MAXSTATUS];
    wchar_t	*string[MAXSTATUS];
    int         order[MAXSTATUS];
    int             i = 0, k = 0, j = 0;
    int             num = 0;
    wchar_t     *p ;
    char           *strtok();
    char            s2[BUFSIZE];
    wchar_t	w[2];
    
    if(simple){
	mbstowcs(ws, s1, MAXSTATUS) ; /* assume s < MAXSTATUS */
    } else {    
	mbstowcs(s2, s1, BUFSIZE);
	strtows(w,"(");
	list[i] = (wchar_t *) wstok(s2, w);
	for (i = 1, num = 0;
	     ((list[i] =  wstok(NULL, w)) != NULL);
	     i++, num++);
	strtows(w,")");
	for (i = 0; ((wstok(list[i], w)) != NULL); i++);
	
	strtows(w," ");
	for (i = 0; i <= num; i++) {
	    name[i] =  wstok(list[i], w);
	    value[i] = wstok(NULL, w);
	}
	for (i = 0; i <= num; i++) {
	    if (!((name[i]) && (value[i])))	continue;
	    for (k = 0; k <= s->NStatusInFile; k++) {
		if (((wscmp(name[i], s->StatusString[k].name)) == 0) &&
		    (wscmp(value[i], s->StatusString[k].value) == 0)) {
		    value[i] = s->StatusString[k].value;
		    string[i] = s->StatusString[k].string;
                    order[i] = s->StatusString[k].order;
		    break;
		}
	    }
	}
	for (j = 0; j <= num; j++) {
	    for (i = 0; i <= s->NStatus; i++) {
		if (s->CurrentStatusString[i].name &&
		    (order[j] == i + 1)){
		    s->CurrentStatusString[i].value = value[j];
		    s->CurrentStatusString[i].string = string[j];
		    break;
		}
	    }
	}
	p = ws ;
	for (i = 0; i <= s->NStatus; i++)
	  if (s->CurrentStatusString[i].string) {
	      wscpy(p, s->CurrentStatusString[i].string);
	      p += wslen(s->CurrentStatusString[i].string);
	  }
	/*
	 * We don't check the status string length is shorter than 
	 * s->StatusStringRemainBuffer.size. This is sort of bug.
	 * but currently this buffer size is enough big for status string
	 */
    }
}
Private void
init_status_string(s, path)
iml_session_t *s ;
unsigned char         *path;
{
    FILE           *status_file;
    char           *name;
    char           *value;
    char           *p1, *p2;
    char            buf[BUFSIZE];
    wchar_t	   buf2[BUFSIZE];
    char           *index();
    register j, k;
    
    
    s->NStatusInFile = -1 ;
    if ((status_file = fopen(path, "r")) != NULL) {
	while ((fgets(buf, BUFSIZE, status_file)) != NULL) {
	    if (buf[0] == '#')
	      continue;
	    s->NStatusInFile++;
	    p1 = buf;
	    p2 = index(p1, ':');
	    *p2 = '\0';
	    mbstowcs(s->StatusString[s->NStatusInFile].name,
		     p1, p2 - p1);
	    p1 = p2 + 1;
	    p2 = index(p1, ':');
	    *p2 = '\0';
	    mbstowcs(s->StatusString[s->NStatusInFile].value,
		     p1, p2 - p1);
	    p1 = p2 + 1;
	    p2 = index(p1, ':');
	    *p2 = '\0';
	    mbstowcs(s->StatusString[s->NStatusInFile].string,
		     p1, p2 - p1);
	    s->StatusString[s->NStatusInFile].order =
	      atoi(p2 + 1);
	    
	}
	k = s->StatusString[0].order - 1;
	s->CurrentStatusString[k].name = s->StatusString[0].name;
	s->CurrentStatusString[k].value = s->StatusString[0].value;
	s->CurrentStatusString[k].string = s->StatusString[0].string;
	
	for (j = 1, s->NStatus = 0; j <= s->NStatusInFile; j++) {
	    if ((!s->CurrentStatusString[k].name) ||
		(!s->StatusString[j].name))
	      continue;
	    if ((wscmp(s->CurrentStatusString[k].name,
		       s->StatusString[j].name))
		!= 0) {
		k = s->StatusString[j].order - 1;
		s->CurrentStatusString[k].name =
		  s->StatusString[j].name;
		s->CurrentStatusString[k].value =
		  s->StatusString[j].value;
		s->CurrentStatusString[k].string =
		  s->StatusString[j].string;
		s->NStatus++;
	    }
	}
	fclose(status_file);
    } else {
	/* assume using SIMPLE MODE */
    }
}

Private void
eval_cm_to_env_packet(s, cte, rrv)
iml_session_t *s ;
register struct cm_to_env *cte;
iml_inst **rrv;
{
    eval_it( s, cte, rrv);
    _iml_post_proc(s, rrv);
}
Private void
jle103_close(s, rrv)
iml_session_t *s ;
iml_inst **rrv ;
{
    Private int destruct_session(/* iml_session_t *s, iml_inst **rrv */);
    extern int _iml_destruct_session(/* iml_session_t *s ,
				       iml_inst **rrv ,
				       int (*destructor)() */);
    _iml_destruct_session(s, rrv, destruct_session);

}
Private void
destruct_session(s, rrv)
iml_session_t *s ;
iml_inst **rrv ;
{
    cm_close(s->language_engine, s->session_id);
}

Private void
jle103_reset(s, rrv)
iml_session_t *s ;
iml_inst **rrv ;
{
    _iml_fake_nop_inst(s, rrv);
}
/*
 * Commit intermediate text by sending CM_RESET to Language engine.
 * This function is called from XmbReset();
 * IMLogic(,,IML_COMMIT,) and then iml_send_commit().
 * Assignment of jle103_send_commit() to function entity of
 * iml_send_commit() is done in jle103configure().
 *
 */
Private void
jle103_send_commit(s, rrv)
iml_session_t *s;
iml_inst **rrv;
{
    struct cm_to_env *ctep;
    static struct env_to_cm etc;

    if(s->remainder){
        *rrv = s->remainder;
        s->remainder = NULL;
        return;
    }
    etc.ec_operation = CM_RESET;
    etc.ec_next = NULL ;
    ctep = cm_put(s->language_engine, s->session_id, &etc);
    eval_cm_to_env_packet(s, ctep, rrv, NULL, NULL);
}

Private void
jle103_set_choice(s, rrv, index)
iml_session_t *s ;
iml_inst **rrv ;
int index ;
{
    Private struct cm_to_env cte ;
    Private struct cm_to_env *ctep = &cte ;
    Private struct env_to_cm etc ;
    
    etc.ec_operation = CM_SELECT ;
    etc.ec_select = index ;
    etc.ec_next = NULL ;
    ctep = cm_put(s->language_engine, s->session_id, &etc);
    eval_cm_to_env_packet(s, ctep, rrv, NULL, NULL);
    _iml_delete2(s);
}

Private void
jle103_attr2feedback(p, q, len, e)
char *e ;
XIMFeedback *p ;
unsigned char *q ; 
int len ;
{
    register i,j,l;
    register char r ;
    
    _iml_xdmem(q,len*sizeof(wchar_t));
    
    for(i=0,j=0;j<len;i+=l,j++){
#ifdef NEXT_GENERATION_OF_JLE_ATTRIBUTE_COMPATIBLE
	p[i] = q[j] ;
#else
	l = mblen(e+i,4);
	r = q[i];
	p[j]=0;

	if(r & ATTR_REVERSE)
	  p[j] |= XIMReverse ;
	else
	  p[j] |= XIMUnderline ; /* because of cm bug */

#endif
    }
    
    _iml_xdmem(p,len*sizeof(XIMFeedback)) ;
}
Private void
jle103_opsw_init()
{
    register i ;
    
    if(opsw[ENV_NOP]) return;

#ifdef ATTACHEDMSG
    if(! attachedMsgCount){
	printf("JleCm ");
    }
#endif

    for(i = 0 ; i < MAX_ENV_OPCODE ; i++){
	opsw[i] = env_nop ;
    }
    opsw[ENV_NOP] = env_nop ;
    opsw[ENV_COMMIT] = env_commit ;
    opsw[ENV_SET_CURRENT_REGION] = env_set_current_region ;
    opsw[ENV_CM_OFF] = env_cm_off ;
    opsw[ENV_SETKEY_CM_ON] = env_setkey_cm_on ;
    opsw[ENV_INTERM_RESET] = env_interm_reset ;
    opsw[ENV_INTERM] = env_interm ;
    opsw[ENV_M_INTERACTIVE] = env_m_interactive ;
    opsw[ENV_SELECT_RESET] = env_select_reset ;
    opsw[ENV_SELECT] = env_select ;
    opsw[ENV_SELECT_END] = env_select_end ;
    opsw[ENV_SELECT_NEXT] = env_select_next ;
    opsw[ENV_SELECT_PREV] = env_select_prev ;
    opsw[ENV_SELECT_COMMIT] = env_select_commit ;
    opsw[ENV_SELECT_TEST] = env_nop ;
    opsw[ENV_SET_SIMPLE_MODE] =  env_set_smode ;
    opsw[ENV_SET_MODE] = env_set_mode ;
    opsw[ENV_MISC_RESET] = env_misc_reset ;
    opsw[ENV_M_LABEL] = env_m_label ;
    opsw[ENV_M_MESS] = env_nop ;

}
