#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunIMWnn4.c 1.3 91/08/14";
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
/* $Id: Func.c,v 3.3 90/09/18 16:19:04 ishisone Rel $ */
/*
 * Copyright (c) 1989  Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 */

#include <stdio.h>
/*
 * According to ANSI C, wchar_t should be declared in stddef.h
 */
#include <stddef.h>
#include <stdlib.h>
#include <euc.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
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
#include <wnnerror.h>
#include <jslib.h>
#include "XSunJclib.h"
#include "XSunJilib.h"
#include "XSunCconv.h"
#include "XSunIMWnn4.h"
#include "XSunIMPub.h"

/*
 *
 */

Private functbl_t   functbls[] = {
"convert",	   convert,             selection_forward,   beep,
"convert-or-space",convert_sp,          selection_forward,   beep,
"convert-s",	   convert_small,       selection_forward,   beep,
"unconvert",	   unconvert,           beep,                beep,
"next",		   select_next,         selection_next,      symbol_next,
"next-s",	   select_next_small,   selection_next,      symbol_next,
"previous",	   select_prev,         selection_prev,      symbol_prev,
"previous-s",	   select_prev_small,   selection_prev,      symbol_prev,
"forward",	   move_forward,        selection_forward,   symbol_forward,
"backward",	   move_backward,       selection_backward,  symbol_backward,
"move-top",	   move_top,            selection_top,       symbol_top,
"move-bottom",	   move_bottom,         selection_bottom,    symbol_bottom,
"clear",	   clear_buffer,        clear_buffer,        clear_buffer,
"expand",	   expand_clause,       expand_clause,       beep,
"expand-s",	   expand_clause_small, expand_clause_small, beep,
"shrink",	   shrink_clause,       shrink_clause,       beep,
"shrink-s",	   shrink_clause_small, shrink_clause_small, beep,
"expand-noconv",   expand_clause2,      expand_clause2,      beep,
"expand-noconv-s", expand_clause2_small,expand_clause2_small,beep,
"shrink-noconv",   shrink_clause2,      shrink_clause2,      beep,
"shrink-noconv-s", shrink_clause2_small,shrink_clause2_small,beep,
"fix",		   commit,              commit,              commit,
"fix2",		   commit,              commit,              commit,
"fix-or-cr",	   commit_or_cr,        selection_select,    symbol_select,
"to-hankaku",	   hankaku,             hankaku,             beep,
"to-zenkaku",	   zenkaku,             zenkaku,             beep,
"to-hiragana",	   hiragana,            hiragana,            beep,
"to-katakana",	   katakana,            katakana,            beep,
"backspace",	   backspace,           backspace,           backspace,
"delete",	   delete,              delete,              delete,
"carriage-return", cr,                  selection_select,    symbol_select,
"fix-and-write",   fixwrite,            beep,                beep,
"beep",		   beep,                beep,                beep,
"jiscode-begin",   jiscode_begin,       beep,                beep,
"jiscode-end",	   jiscode_end,         beep,                beep,
"symbol-input",	   symbol_input,        beep,                symbol_abort,
"inspect",	   inspect,             beep,                beep,
"end-conversion",  convend,             convend,             convend,
};
#define FUNCTBLSIZE     (sizeof(functbls) / sizeof(functbl_t))
/*
 * $@JQ49%F!<%V%k(J
 */

/* $@A43Q$R$i$,$J(J/$@%+%?%+%J(J <-> $@H>3Q%+%JJQ49MQ%F!<%V%k(J */
#define DAKUON	(0xde<<8)
#define HANDAKUON	(0xdf<<8)
static unsigned short	hiratohan[] = {
	/* $@$"(J */
	0xa7, 0xb1, 0xa8, 0xb2, 0xa9, 0xb3, 0xaa, 0xb4, 0xab, 0xb5,
	/* $@$+(J */
	0xb6, 0xb6|DAKUON, 0xb7, 0xb7|DAKUON, 0xb8, 0xb8|DAKUON,
	0xb9, 0xb9|DAKUON, 0xba, 0xba|DAKUON,
	/* $@$5(J */
	0xbb, 0xbb|DAKUON, 0xbc, 0xbc|DAKUON, 0xbd, 0xbd|DAKUON,
	0xbe, 0xbe|DAKUON, 0xbf, 0xbf|DAKUON,
	/* $@$?(J */
	0xc0, 0xc0|DAKUON, 0xc1, 0xc1|DAKUON, 0xaf, 0xc2, 0xc2|DAKUON,
	0xc3, 0xc3|DAKUON, 0xc4, 0xc4|DAKUON,
	/* $@$J(J */
	0xc5, 0xc6, 0xc7, 0xc8, 0xc9,
	/* $@$O(J */
	0xca, 0xca|DAKUON, 0xca|HANDAKUON,
	0xcb, 0xcb|DAKUON, 0xcb|HANDAKUON,
	0xcc, 0xcc|DAKUON, 0xcc|HANDAKUON,
	0xcd, 0xcd|DAKUON, 0xcd|HANDAKUON,
	0xce, 0xce|DAKUON, 0xce|HANDAKUON,
	/* $@$^(J */
	0xcf, 0xd0, 0xd1, 0xd2, 0xd3,
	/* $@$d(J */
	0xac, 0xd4, 0xad, 0xd5, 0xae, 0xd6,
	/* $@$i(J */
	0xd7, 0xd8, 0xd9, 0xda, 0xdb,
	/* $@$o(J */
	0xdc, 0xdc, 0xb2, 0xb4, 0xa6,
	/* $@$s(J */
	0xdd,
};

/* $@A43Q5-9f(J $@"M(J $@H>3QBP1~I=(J */
static struct symzenhan {
	unsigned short	zen;
	unsigned char	han;
}	kigoutohan[] = {
	0xa1a1, 0x20,	0xa1a2, 0xa4,	0xa1a3, 0xa1,	0xa1a4, 0x2c,
	0xa1a5, 0x2e,	0xa1a6, 0xa5,	0xa1a7, 0x3a,	0xa1a8, 0x3b,
	0xa1a9, 0x3f,	0xa1aa, 0x21,	0xa1ab, 0xde,	0xa1ac, 0xdf,
	0xa1b0, 0x5e,	0xa1b2, 0x5f,	0xa1bc, 0xb0,	0xa1bf, 0x2f,
	0xa1c1, 0x7e,	0xa1c3, 0x7c,	0xa1c6, 0x60,	0xa1c7, 0x27,
	0xa1c8, 0x22,	0xa1c9, 0x22,	0xa1ca, 0x28,	0xa1cb, 0x29,
	0xa1cc, 0x5b,	0xa1cd, 0x5d,	0xa1ce, 0x5b,	0xa1cf, 0x5d,
	0xa1d0, 0x7b,	0xa1d1, 0x7d,	0xa1d6, 0xa2,	0xa1d7, 0xa3,
	0xa1dc, 0x2b,	0xa1dd, 0x2d,	0xa1e1, 0x3d,	0xa1e3, 0x3c,
	0xa1e4, 0x3e,	0xa1ef, 0x5c,	0xa1f0, 0x24,	0xa1f3, 0x25,
	0xa1f4, 0x23,	0xa1f5, 0x26,	0xa1f6, 0x2a,	0xa1f7, 0x40,
};
#define KIGOUSIZE	(sizeof(kigoutohan) / sizeof(struct symzenhan))

/* $@H>3Q(J $@"M(J $@A43QJQ49I=(J */
static wchar_t	hantozen[] = {
/* C0 */
0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f,
/* ASCII */
0xa1a1, 0xa1aa, 0xa1c9, 0xa1f4, 0xa1f0, 0xa1f3, 0xa1f5, 0xa1c7,
0xa1ca, 0xa1cb, 0xa1f6, 0xa1dc, 0xa1a4, 0xa1dd, 0xa1a5, 0xa1bf,
0xa3b0, 0xa3b1, 0xa3b2, 0xa3b3, 0xa3b4, 0xa3b5, 0xa3b6, 0xa3b7,
0xa3b8, 0xa3b9, 0xa1a7, 0xa1a8, 0xa1e3, 0xa1e1, 0xa1e4, 0xa1a9,
0xa1f7, 0xa3c1, 0xa3c2, 0xa3c3, 0xa3c4, 0xa3c5, 0xa3c6, 0xa3c7,
0xa3c8, 0xa3c9, 0xa3ca, 0xa3cb, 0xa3cc, 0xa3cd, 0xa3ce, 0xa3cf,
0xa3d0, 0xa3d1, 0xa3d2, 0xa3d3, 0xa3d4, 0xa3d5, 0xa3d6, 0xa3d7,
0xa3d8, 0xa3d9, 0xa3da, 0xa1ce, 0xa1ef, 0xa1cf, 0xa1b0, 0xa1b2,
0xa1c6, 0xa3e1, 0xa3e2, 0xa3e3, 0xa3e4, 0xa3e5, 0xa3e6, 0xa3e7,
0xa3e8, 0xa3e9, 0xa3ea, 0xa3eb, 0xa3ec, 0xa3ed, 0xa3ee, 0xa3ef,
0xa3f0, 0xa3f1, 0xa3f2, 0xa3f3, 0xa3f4, 0xa3f5, 0xa3f6, 0xa3f7,
0xa3f8, 0xa3f9, 0xa3fa, 0xa1d0, 0xa1c3, 0xa1d1, 0xa1c1, 0x007f,
/* C1 */
0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f,
0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f,
/* KANA */
0xa1a1, 0xa1a3, 0xa1d6, 0xa1d7, 0xa1a2, 0xa1a6, 0xa5f2, 0xa5a1,
0xa5a3, 0xa5a5, 0xa5a7, 0xa5a9, 0xa5e3, 0xa5e5, 0xa5e7, 0xa5c3,
0xa1bc, 0xa5a2, 0xa5a4, 0xa5a6, 0xa5a8, 0xa5aa, 0xa5ab, 0xa5ad,
0xa5af, 0xa5b1, 0xa5b3, 0xa5b5, 0xa5b7, 0xa5b9, 0xa5bb, 0xa5bd,
0xa5bf, 0xa5c1, 0xa5c4, 0xa5c6, 0xa5c8, 0xa5ca, 0xa5cb, 0xa5cc,
0xa5cd, 0xa5ce, 0xa5cf, 0xa5d2, 0xa5d5, 0xa5d8, 0xa5db, 0xa5de,
0xa5df, 0xa5e0, 0xa5e1, 0xa5e2, 0xa5e4, 0xa5e6, 0xa5e8, 0xa5e9,
0xa5ea, 0xa5eb, 0xa5ec, 0xa5ed, 0xa5ef, 0xa5f3, 0xa1ab, 0xa1ac,
/* undefined */
0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7,
0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff,
};

/* $@8uJdA*Br(J */
#define SEL_LEFT	0
#define SEL_RIGHT	1
#define SEL_NEXT	2
#define SEL_PREV	3
#define SEL_TOP		4
#define SEL_BOTTOM	5


Public int _iml_wnn403_constructor();

Public int
_iml_wnn403_constructor(id, rrv)
int id ;
iml_inst **rrv;
{
    return wnn403_open(id, rrv);
}


/*
 * jserver $@$H$N%3XSunIMWnn4.c/%7%g%s$d!"4D6-$N@_Dj$,L$$@$J$i$P!"$=$l$r9T$J$$!"(J
 * $@JQ49/home/sundae4/donation/si/SCCS_DIRECTORIES/usr.lib/libxim/SCCS/s.XSunIMWnn4.cC15:51:05!$r:n$k(J
 */
Private
wnn403_open(id, rrv)
int id ;
iml_inst **rrv;
{
    Private int construct_session(/* int id ,  iml_inst **rrv ,
			         iml_session_t *session */);
    extern int _iml_construct_session(/* int id , iml_inst **rrv ,
				      int (*constructor)() */);

    return _iml_construct_session(id,rrv,sizeof(iml_session_t),construct_session);
}
Private void
wnn403_close(s, rrv)
iml_session_t *s ;
iml_inst **rrv ;
{
    Private int destruct_session(/* iml_session_t *s, iml_inst **rrv */);
    extern int _iml_destruct_session(/* iml_session_t *s ,
				       iml_inst **rrv ,
				       int (*destructor)() */);
    _iml_destruct_session(s, rrv, destruct_session);

}
Private
defaultwnnConfirmFunc(type, file, client_data)
int type;
char *file;
caddr_t client_data;
{
}
Private
default_action(str, nbytes, client_data)
unsigned char *str;
int nbytes;
caddr_t client_data;
{
    beep();
}
Private int
destruct_session(s, rrv)
iml_session_t *s ;
iml_inst **rrv ;
{
    ccDestroyBuf(s->ccbuf);
    jcDestroyBuffer(s->jcconvbuf, 1);
}

Private 
construct_session(id, rrv, s, is_recycled)
int id ;
iml_inst **rrv;
iml_session_t *s ;
int is_recycled ;
{
    char *mode_path;
    char *getenv();    
    Private char *functbl[FUNCTBLSIZE];
    
    /*
     * jserver $@$H$N@\B3(J
     */
    if((s->jserver_id = jiOpenServer(NULL, /* get jservername from JSERVER */
				  30    /* timeout in sec */
				  )) == NULL){
	return(IMLERR_CANNOT_OPEN);
    }
    /*
     * $@4D6-$N:n@.(J
     */
    if((s->wnn_env = jiCreateEnv(s->jserver_id,
			 "",		/* envname */
			 "",		/* wnnrcfile */
			 NULL, 		/* errmsgfunc */
			 defaultwnnConfirmFunc,
			 NULL 		/* clientdata */
			 )
	) == NULL) {
	return(IMLERR_CANNOT_OPEN);
    }
    /*
     * $@%m!<%^;z2>L>JQ49/home/sundae4/donation/si/SCCS_DIRECTORIES/usr.lib/libxim/SCCS/s.XSunIMWnn4.cC15:51:05!$N:n@.(J: imlogic $@$G$O%;3287%g%s$H8F$V!#(J
     */

    if((s->ccrule = ccParseRule(NULL, NULL)) == NULL){
	return(IMLERR_CANNOT_OPEN);
    }
    if(!functbl[0]){
	register i = 0 ;
	for(; i< FUNCTBLSIZE ; i++){
	    functbl[i] = functbls[i].fname ;
	}
    }
    if((s->ccbuf = ccCreateBuf(s->ccrule,
				  16, /* context size */
				  functbl,	/* function table */
				  FUNCTBLSIZE,
				  default_action,
				  insert_char,
				  delete_char,
				  function_dispatch,
				  mode_notify,
				  s /* client data */
				  )) == NULL ){
	return(-1);
    }
    /*
     * $@2>L>4A;zJQ49/home/sundae4/donation/si/SCCS_DIRECTORIES/usr.lib/libxim/SCCS/s.XSunIMWnn4.cC15:51:05!$N:n@.(J: imlogic $@$G$O%;3547%g%s$H8F$V!#(J
     */

    if((s->jcconvbuf = jcCreateBuffer(s->wnn_env, 0, 0)) == NULL ){
	return(IMLERR_CANNOT_OPEN);
    }
    s->language_engine = id ;
    if(!is_recycled){
	s->pets = (wchar_t *)
	  calloc(sizeof(wchar_t),DEFAULTPreEditTextBufferSize);
	s->petssz = DEFAULTPreEditTextBufferSize ;
	s->petf = (XIMFeedback *)
	  calloc(sizeof(XIMFeedback),DEFAULTPreEditAttrBufferSize);
	s->petfsz = DEFAULTPreEditAttrBufferSize ;
	/*
	 * over ride member function
	 */
	s->iml_open = wnn403_open ;
	s->iml_close = wnn403_close ;
	s->iml_reset = wnn403_reset ;
	s->iml_send_event = wnn403_send_event ;
	s->iml_conversion_on = wnn403_conversion_on ;
	s->iml_send_results = wnn403_send_results ;
	s->iml_send_commit = wnn403_send_commit ;
    }
    s->session_id = (int) s->jcconvbuf ;
    return(s->session_id);
}
/*
 * $@%-!<%$s%H$,@8$:$k$?$S$K8F$P$l$k!#(J
 */
Private int
wnn403_send_event(s, rrv, event)
iml_session_t *s ;
iml_inst **rrv ;
XKeyEvent *event ;
{
    if(event->type != KeyPress){
	_iml_fake_nop_inst(s, rrv, event);
	return;
    }
    s->event = event ; /* save KeyEvent */
    if(s->remainder){
	*rrv = s->remainder ;
	s->remainder = NULL ;
	_iml_post_proc(s, rrv);
	return;
    }
    s->rrv = rrv ;
    ccConvchar(s->ccbuf, event);
    _iml_post_proc(s, rrv);
}

/* ARGSUSED */
Private void
function_dispatch(func, str, nbytes, s)
int func;
unsigned char *str;
int nbytes;
iml_session_t *s;
{
	switch (s->state) {
	case MODE_SELECTION_S:
	case MODE_SELECTION_L:
		(*functbls[func].func_selection)(s);
		break;
	case MODE_SYMBOL:
		(*functbls[func].func_symbol)(s);
		break;
	default:
		(*functbls[func].func_normal)(s);
		break;
	}
}

Private
wnn403_set_keyboard_state(session, rrv, key_buttons)
iml_session_t *session;
iml_inst **rrv;
IMKeyState *key_buttons;
{
    iml_generic_set_keyboard_state(session, rrv, key_buttons);
}

Private void
wnn403_conversion_on(session, rrv)
iml_session_t *session;
iml_inst **rrv ;
{
    if(session->remainder){
	*rrv = session->remainder ;
	session->remainder = NULL ;
	return;
    }
    make_conversion_on(session, rrv);
}
Private void
wnn403_send_results(s, rrv, results)
iml_session_t *s;
iml_inst **rrv ;
iml_inst *results ;
{
    XIMLookupStartCallbackStruct *ls;
    XIMLookupProcessCallbackStruct *lp;

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
	    wnn403_set_choice(s, rrv, lp->index_of_choice_selected,s->event);
	    return;
	  default:
	    break;
	}
	*rrv = (iml_inst *)_iml_new(s, sizeof(iml_inst));
	(*rrv)->opcode = IMM_NOP ;
	(*rrv)->next = NULL ;
	return ;

    }
}
Private void
make_conversion_on(session, rrv)
iml_session_t *session ;
iml_inst **rrv ;
{
    session->public_status |= IMLSTATUS_Henkan_Mode ;
}

Private void
wnn403_reset(session, rrv)
iml_session_t *session ;
iml_inst **rrv ;
{
    _iml_fake_nop_inst(session, rrv);
}
/*
 * Commit intermediate text.
 * This function is called from XmbReset();
 * IMLogic(,,IML_COMMIT,) and then iml_send_commit().
 *
 */
Private void
wnn403_send_commit(s, rrv)
iml_session_t *s;
iml_inst **rrv;
{
    if(s->remainder){
        *rrv = s->remainder;
        s->remainder = NULL;
        return;
    }

}

Private
wnn403_set_choice(s, rrv, index)
iml_session_t *s ;
iml_inst **rrv ;
int index ;
{
    _iml_delete2(s);
}


Private void
insert_char(c, s)
int c;
iml_session_t *s ;
{

	switch(s->state){
	case MODE_SELECTION_L:
	case MODE_SELECTION_S:
	    endSelection(s, 0);
	    break;
	case MODE_SYMBOL:
	    endSymbol(s, 0);
	    break ;
	default:
	    break;
	}
	if (jcIsConverted(s->jcconvbuf, s->jcconvbuf->curClause)) {
		(void)jcBottom(s->jcconvbuf);
	}
	(void)jcInsertChar(s->jcconvbuf, c);
}

Private void
deleate_char(s)
iml_session_t *s;
{
	switch(s->state){
	case MODE_SELECTION_L:
	case MODE_SELECTION_S:
	case MODE_SYMBOL:
	    beep(s);
	    return;
	    break ;
	default:
	    break;
	}
	ccContextDelete(s->ccbuf);
	jcDeleteChar(s->jcconvbuf, 1);
}

/* $Id: Func.c,v 3.3 90/09/18 16:19:04 ishisone Rel $ */
/*
 * Copyright (c) 1989  Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 */

Private void
convert(s)
iml_session_t *s ;
{
	jcConvBuf	*jcbuf = s->jcconvbuf;

	if (jcbuf->curClause == jcbuf->nClause) {
		(void)jcMove(jcbuf, 0, JC_BACKWARD);
	}

	if (jcIsConverted(jcbuf, jcbuf->curClause)) {
		beginSelection(s, 0);
		return;
	}

	if(jcConvert(jcbuf, 0, 0, 1)<0){
	    beep();
	}
	  
	ccContextClear(s->ccbuf);
}

Private void
convert_sp(s)
iml_session_t *s ;
{
	if (s->jcconvbuf->nClause == 0) {
		insert_char(' ', (caddr_t)s);
		commit(s);
	} else {
		convert(s);
	}
}

Private void
convert_small(s)
iml_session_t *s ;
{
	jcConvBuf	*jcbuf = s->jcconvbuf;

	if (jcbuf->curClause == jcbuf->nClause) {
		(void)jcMove(jcbuf, 1, JC_BACKWARD);
	}

	if (jcIsConverted(jcbuf, jcbuf->curClause)) {
		beginSelection(s, 1);
		return;
	}

	if(jcConvert(jcbuf, 1, 0, 1)<0){
	    beep();
	}
	ccContextClear(s->ccbuf);
}

Private void
unconvert(s)
iml_session_t *s ;
{
    if(jcUnconvert(s->jcconvbuf)<0){
	beep();
    }
    ccContextClear(s->ccbuf);
}

Private void
select_next(s)
iml_session_t *s ;
{
	if(jcNext(s->jcconvbuf, 0, JC_NEXT)<0){
	    beep();
	}
	ccContextClear(s->ccbuf);	
}

Private void
select_prev(s)
iml_session_t *s ;
{
	if(jcNext(s->jcconvbuf, 0, JC_PREV)<0){
	    beep();
	}
	ccContextClear(s->ccbuf);	
}
Private void
select_next_small(s)
iml_session_t *s ;
{
	if(jcNext(s->jcconvbuf, 1, JC_NEXT)<0){
	    beep();
	}
	ccContextClear(s->ccbuf);	
}

Private void
select_prev_small(s)
iml_session_t *s ;
{
	if(jcNext(s->jcconvbuf, 1, JC_PREV)<0){
	    beep();
	}
	ccContextClear(s->ccbuf);	
}

/*
 * $@%+!<%=%k0\F0(J
 */ 
Private void
move_top(s)
iml_session_t *s ;
{
	(void)jcTop(s->jcconvbuf);
	ccContextClear(s->ccbuf);
}

Private void
move_bottom(s)
iml_session_t *s ;
{
	(void)jcBottom(s->jcconvbuf);
	ccContextClear(s->ccbuf);
}

Private void
move_forward(s)
iml_session_t *s ;
{
	if(jcMove(s->jcconvbuf, 1, JC_FORWARD)){
	    beep();
	}
	ccContextClear(s->ccbuf);
}

Private void
move_backward(s)
iml_session_t *s ;
{
	if(jcMove(s->jcconvbuf, 1, JC_BACKWARD)<0){
	    beep();
	}
	ccContextClear(s->ccbuf);
}


/*
 * $@3NDj(J
 */

Private void
commit(s)
iml_session_t *s ;
{
    XIMPreeditDrawCallbackStruct *p;
    iml_inst *lp ;
    jcConvBuf	*jcbuf = s->jcconvbuf;
    int		len = jcbuf->displayEnd - jcbuf->displayBuf;
    
    switch(s->state){
    case MODE_SELECTION_L:
    case MODE_SELECTION_S:
	endSelection(s, 0);
	break;
    case MODE_SYMBOL:
	endSymbol(s, 0);
	break ;
    default:
	break;
    }
    if (jcFix(s->jcconvbuf) < 0) {
	beep(s);
	*(s->rrv) = _iml_make_nop_inst(s);
	return;
    }
    ccContextClear(s->ccbuf);
    
    /* $@3NDj$N=hM}(J */
    lp = _iml_make_preedit_erase_inst(s);
    *(s->rrv) = lp;
    lp = _iml_make_preedit_done_inst(s);
    _iml_link_inst_tail(s->rrv, lp);
    
    /*
     * $@3NDjJ8;zNs$H$7$F$O!"8=>u(J multi byte $@$N$_%5%]!<%H$7$F$$$k(J
     */
    _iml_make_commit_inst(s, &lp, len, jcbuf->displayBuf);
    _iml_link_inst_tail(s->rrv, lp);

    /* $@/home/sundae4/donation/si/SCCS_DIRECTORIES/usr.lib/libxim/SCCS/s.XSunIMWnn4.cC15:51:05!$r%/%j%"$9$k(J */
    jcClear(s->jcconvbuf);
    

}

Private void
commit_or_cr(s)
iml_session_t *s ;
{
	if (s->jcconvbuf->nClause == 0) {
		cr(s);
	} else {
		commit(s);
	}
}

/*
 * $@%/%j%"(J
 */

Private void
clear_buffer(s)
iml_session_t *s ;
{
	switch(s->state){
	case MODE_SELECTION_L:
	case MODE_SELECTION_S:
	    endSelection(s, 0);
	    break;
	case MODE_SYMBOL:
	    endSymbol(s, 0);
	    break ;
	default:
	    break;
	}
	if((jcClear(s->jcconvbuf)<0)){
	    beep();
	}
	ccContextClear(s->ccbuf);

}

/*
 * $@J8@a$N3HBg(J/$@=L>.(J
 */

Private void
expand_or_shrink(s, shrink, small, conv)
iml_session_t *s ;
int shrink;
int small;
int conv;
{
	switch(s->state){
	case MODE_SELECTION_L:
	case MODE_SELECTION_S:
	    endSelection(s, 0);
	    break;
	case MODE_SYMBOL:
	    endSymbol(s, 0);
	    break ;
	default:
	    break;
	}
	if (shrink) {
		if(jcShrink(s->jcconvbuf, small, conv)<0){
		    beep();
		}
	} else {
		if(jcExpand(s->jcconvbuf, small, conv)<0){
		    beep();
		}
	}
	ccContextClear(s->ccbuf);

}

Private void
expand_clause(s)
iml_session_t *s ;
{
	expand_or_shrink(s, 0, 0, 1);
}

Private void
expand_clause_small(s)
iml_session_t *s ;
{
	expand_or_shrink(s, 0, 1, 1);
}

Private void
expand_clause2(s)
iml_session_t *s ;
{
	expand_or_shrink(s, 0, 0, 0);
}

Private void
expand_clause2_small(s)
iml_session_t *s ;
{
	expand_or_shrink(s, 0, 1, 0);
}

Private void
shrink_clause(s)
iml_session_t *s ;
{
	expand_or_shrink(s, 1, 0, 1);
}

Private void
shrink_clause_small(s)
iml_session_t *s ;
{
	expand_or_shrink(s, 1, 1, 1);
}

Private void
shrink_clause2(s)
iml_session_t *s ;
{
	expand_or_shrink(s, 1, 0, 0);
}

Private void
shrink_clause2_small(s)
iml_session_t *s ;
{
	expand_or_shrink(s, 1, 1, 0);
}

/*
 * $@8uJdA*Br(J
 */

Private void
selection_top(s)
iml_session_t *s ;
{
	moveSelection(s, SEL_TOP);
}

Private void
selection_bottom(s)
iml_session_t *s ;
{
	moveSelection(s, SEL_BOTTOM);
}

Private void
selection_forward(s)
iml_session_t *s ;
{
	moveSelection(s, SEL_RIGHT);
}

Private void
selection_backward(s)
iml_session_t *s ;
{
	moveSelection(s, SEL_LEFT);
}

Private void
selection_next(s)
iml_session_t *s ;
{
	moveSelection(s, SEL_NEXT);
}

Private void
selection_prev(s)
iml_session_t *s ;
{
	moveSelection(s, SEL_PREV);
}

Private void
selection_select(s)
iml_session_t *s ;
{
	endSelection(s, 0);
	(void)jcMove(s->jcconvbuf, s->state == MODE_SELECTION_S, JC_FORWARD);

}

/*
 * $@5-9fF~NO(J
 */

Private void
symbol_input(s)
iml_session_t *s ;
{
	beginSymbol(s);
}

Private void
symbol_top(s)
iml_session_t *s ;
{
	moveSymbol(s, SEL_TOP);
}

Private void
symbol_bottom(s)
iml_session_t *s ;
{
	moveSymbol(s, SEL_BOTTOM);
}

Private void
symbol_forward(s)
iml_session_t *s ;
{
	moveSymbol(s, SEL_RIGHT);
}

Private void
symbol_backward(s)
iml_session_t *s ;
{
	moveSymbol(s, SEL_LEFT);
}

Private void
symbol_next(s)
iml_session_t *s ;
{
	moveSymbol(s, SEL_NEXT);
}

Private void
symbol_prev(s)
iml_session_t *s ;
{
	moveSymbol(s, SEL_PREV);
}

Private void
symbol_select(s)
iml_session_t *s ;
{
	endSymbol(s, 0);

}

Private void
symbol_abort(s)
iml_session_t *s ;
{
	endSymbol(s, 1);
}

/*
 * $@J8;z<o$NJQ49(J
 *	$@H>3Q(J <-> $@A43Q(J
 *	$@$R$i$,$J(J <-> $@%+%?%+%J(J
 */

Private void
hankaku(s)
iml_session_t *s ;
{
	wchar_t	han[256];
	wchar_t	*h;
	wchar_t	*ks, *ke;
	register int	c;
	jcConvBuf	*jcbuf = s->jcconvbuf;

	switch(s->state){
	case MODE_SELECTION_L:
	case MODE_SELECTION_S:
	    endSelection(s, 0);
	    break;
	case MODE_SYMBOL:
	    endSymbol(s, 0);
	    break ;
	default:
	    break;
	}
	if (jcbuf->curClause != jcbuf->nClause) {
		jcClause	*cinfo = jcbuf->clauseInfo;
		ks = cinfo[jcbuf->curLCStart].kanap;
		ke = cinfo[jcbuf->curLCEnd].kanap;
		h = han;
		while (ks < ke) {
			c = *ks++;
			if (0xa1a1 <= c && c <= 0xa1fe) {
				/* $@5-9f(J $@/home/sundae4/donation/si/SCCS_DIRECTORIES/usr.lib/libxim/SCCS/s.XSunIMWnn4.c$%J%j!&%5!<%A$9$k(J */
				register struct symzenhan	*hi, *lo, *m;
				register int	dif;

				lo = kigoutohan;
				hi = kigoutohan + KIGOUSIZE;

				while (lo <= hi) {
					m = lo + (hi - lo) / 2;
					dif = c - m->zen;
					if (dif < 0)
						hi = m - 1;
					else if (dif > 0)
						lo = m + 1;
					else 
						break;
				}
				if (lo > hi)
					*h++ = c;
				else
					*h++ = m->han;
			} else if (0xa3b0 <= c && c <= 0xa3b9) {
				*h++ = c - 0xa3b0 + '0';
			} else if (0xa3c1 <= c && c <= 0xa3da) {
				/* A-Z */
				*h++ = c - 0xa3c1 + 'A';
			} else if (0xa3e1 <= c && c <= 0xa3fa) {
				/* a-z */
				*h++ = c - 0xa3e1 + 'a';
			} else if (0xa4a1 <= c && c <= 0xa4f3) {
				/* $@$R$i$,$J(J */
				c = hiratohan[c - 0xa4a1];
				*h++ = c & 0xff;
				if (c & 0xff00)
					*h++ = c >> 8;
			} else if (0xa5a1 <= c && c <= 0xa5f3) {
				/* $@$+$?$+$J(J */
				c = hiratohan[c - 0xa5a1];
				*h++ = c & 0xff;
				if (c & 0xff00)
					*h++ = c >> 8;
			} else
				*h++ = c;
		}
		*h = 0;	/* NULL terminate */
		if(jcChangeClause(jcbuf, han)<0){
		    beep();
		}
	}
	ccContextClear(s->ccbuf);

}

Private void
zenkaku(s)
iml_session_t *s ;
{
	wchar_t	han[256];
	wchar_t	*ks, *ke;
	wchar_t	*h;
	register int	c;
	jcConvBuf	*jcbuf = s->jcconvbuf;

	switch(s->state){
	case MODE_SELECTION_L:
	case MODE_SELECTION_S:
	    endSelection(s, 0);
	    break;
	case MODE_SYMBOL:
	    endSymbol(s, 0);
	    break ;
	default:
	    break;
	}
	if (jcbuf->curClause != jcbuf->nClause) {
		jcClause	*cinfo = jcbuf->clauseInfo;
		ks = cinfo[jcbuf->curLCStart].kanap;
		ke = cinfo[jcbuf->curLCEnd].kanap;
		h = han;
		while (ks < ke) {
			c = *ks++;
			/* $@$A$J$_$K(J"$@$+!+(J"$@$H$$$C$?$b$N$r(J
			 * "$@$,(J"$@$K$9$k$h$&$J?F@Z$O$7$J$$(J
			 * $@$@$$$?$$!"H>3Q$r;H$*$&$C$FJ}$,(J
			 * $@4V0c$C$F$k$h$J(J -- by ishisone
			 * $@$^$C$?$/$=$N$H$&$j!*(J
			 * $@$"$^$d$+$5$J$$$[$&$,$$$$(J -- by hiura
			 */
			if ((0x20 <= c && c <= 0x7e) ||
			    (0xa1 <= c && c <= 0xdf))
				*h++ = hantozen[c];
			else
				*h++ = c;
		}
		*h = 0;	/* NULL terminate */
		if(jcChangeClause(jcbuf, han)<0){
		    beep();
		}
	}
	ccContextClear(s->ccbuf);

}

Private void
hiragana(s)
iml_session_t *s ;
{
	switch(s->state){
	case MODE_SELECTION_L:
	case MODE_SELECTION_S:
	    endSelection(s, 0);
	    break;
	case MODE_SYMBOL:
	    endSymbol(s, 0);
	    break ;
	default:
	    break;
	}
	if(jcKana(s->jcconvbuf, 0, JC_HIRAGANA)<0){
	    beep();
	}
	ccContextClear(s->ccbuf);

}

Private void
katakana(s)
iml_session_t *s ;
{
	switch(s->state){
	case MODE_SELECTION_L:
	case MODE_SELECTION_S:
	    endSelection(s, 0);
	    break;
	case MODE_SYMBOL:
	    endSymbol(s, 0);
	    break ;
	default:
	    break;
	}
	if(jcKana(s->jcconvbuf, 0, JC_KATAKANA)<0){
	    beep();
	}
	ccContextClear(s->ccbuf);

}

Private void
backspace(s)
iml_session_t *s ;
{
	switch(s->state){
	case MODE_SELECTION_L:
	case MODE_SELECTION_S:
	    endSelection(s, s->state == MODE_SELECTION_S);
	    break;
	case MODE_SYMBOL:
	    endSymbol(s, 0);
	    break ;
	default:
	    break;
	}
	ccContextDelete(s->ccbuf);
	if(jcDeleteChar(s->jcconvbuf, 1)<0){
	    beep();
	}

}

Private void
delete(s)
iml_session_t *s ;
{
	switch(s->state){
	case MODE_SELECTION_L:
	case MODE_SELECTION_S:
	    endSelection(s, 0);
	    break;
	case MODE_SYMBOL:
	    endSymbol(s, 0);
	    break ;
	default:
	    break;
	}
	if(jcDeleteChar(s->jcconvbuf, 0)<0){
	    beep();
	}
	ccContextClear(s->ccbuf);

}

Private void
carriageret(s)
iml_session_t *s ;
{
	insert_char('\r', s);
	commit(s);
}

Private void
fixwrite(s)
iml_session_t *s ;
{
	commit(s);
}

Private void
beep(s)
iml_session_t *s ;
{
	XBell(XtDisplay(s), 50);
}

/* ARGSUSED */
Private void
jiscode_begin(s)
iml_session_t *s ;
{
	/* NOT IMPLEMENTED */
}

/* ARGSUSED */
Private void
jiscode_end(s)
iml_session_t *s ;
{
	/* NOT IMPLEMENTED */
}

/* ARGSUSED */
Private void
inspect(s)
iml_session_t *s ;
{
/*	NOT IMPLEMENTED
	if (doInspection() < 0)
		beep(s);
*/
}

Private void
convend(s)
iml_session_t *s ;
{
	switch(s->state){
	case MODE_SELECTION_L:
	case MODE_SELECTION_S:
	    endSelection(s, 0);
	    break;
	case MODE_SYMBOL:
	    endSymbol(s, 0);
	    break ;
	default:
	    break;
	}
	commit(s);
/*	XtCallCallbacks((Widget)w, XtNendNotify, NULL); */
}
