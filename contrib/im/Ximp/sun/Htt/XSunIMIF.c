#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunIMIF.c 1.15 91/08/27";
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
          Nelson Ng    (nng@Sun.COM)
	  				     Sun Microsystems, Inc.
******************************************************************/

#include <locale.h>
#include <stdio.h>
#include <widec.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

#define XK_KATAKANA
#define XK_LATIN1
#define XK_MISCELLANY

#include <X11/keysymdef.h>
#include <X11/Xlibint.h>
#include "XSunExt.h"
#include "XSunIMProt.h"
#include "XSunIMImpl.h"

extern char *malloc();
Private void immgr_init() ;

#ifndef NO_PLUGGIN
Private XIMMethodsRec im_methods = {
    _XSunCloseIM,
    _XSunGetIMValues,
    _XSunCreateIC
};
#endif NO_PLUGGIN

Public XIM
#ifndef NO_PLUGGIN
_XSunOpenIM(lcd, display, rdb, res_name, res_class)
    XLCd 	lcd;
#else /* NO_PLUGGIN */
XOpenIM(display, rdb, res_name, res_class)
#endif /* NO_PLUGGIN */
    Display	*display;
    XrmDatabase	rdb;
    XrmString	res_name;
    XrmString	res_class;
{
    Xim		im;
    char	*string;

    immgr_init();
    string = setlocale(LC_CTYPE,NULL);
    /*
     * FIX ME!!! Very Bad implementation!!
     * And we should take care of rdb/res_name/res_class here. 
     */
    if ((strcmp(string,"japanese")==0) ||
	(strncmp(string,"korean",6)==0)) {
	im = (Xim)malloc(sizeof(xim));
#ifndef NO_PLUGGIN    
        im->methods = (XIMMethods)&im_methods;
        im->core.lcd = lcd;
        im->core.ic_chain = NULL;
        im->core.display = display;
        im->core.rdb = rdb;
        im->core.res_name = res_name;
        im->core.res_class = res_class;
        /* 
         * Still need to set ic_resources, ic_num_resources, ic_attr_resources
         * and ic_num_attr_resources
         */
#endif /* NO_PLUGGIN */    	
	strcpy(im->locale, setlocale(LC_CTYPE,NULL));
	im->display = display;
	return ((XIM)im);
    } else {
	return (NULL);
    }
}

Public Status
#ifndef NO_PLUGGIN
_XSunCloseIM(im) 
#else /* NO_PLUGGIN */
XCloseIM(im) 
#endif /* NO_PLUGGIN */
    Xim		im;
{
	free(im);
}

Public char *
#ifndef NO_PLUGGIN
_XSunGetIMValues(im, args)
    Xim		im;
    XIMArg	*args;
#else /* NO_PLUGGIN */
XGetIMValues(im, va_alist)
    Xim		im;
    va_dcl  
#endif /* NO_PLUGGIN */
{
#ifdef NO_PLUGGIN
    va_list 		var;
    XIMArg 		*args = (XIMArg *)XVaCreateNestedList(var, va_alist);
#endif /* NO_PLUGGIN */
    XIMArg              *arg;
    caddr_t		next;
    XIMStyles		*p;
    XrmName            name;

    for (arg = args; arg->name && *(arg->name); arg++) {
        if (strcmp(arg->name,XNQueryInputStyle)==0) {
	        p = (XIMStyles *)Xmalloc(sizeof(XIMStyles));
                *(XIMStyles **)arg->value = p ;
#ifdef OLD_CODE                
		p->supported_styles = (XIMStyle*)Xmalloc(4*sizeof(XIMStyle));
		p->count_styles = 4;
		p->supported_styles[0] = XIMPreeditCallbacks;
		p->supported_styles[1] = XIMStatusCallbacks ;
		p->supported_styles[2] = XIMLookupCallbacks;
		p->supported_styles[3] = XIMAuxCallbacks;
#else /* OLD_CODE */
                p->count_styles = 1;
		p->supported_styles = (XIMStyle*)Xmalloc(p->count_styles
					* sizeof(XIMStyle));	
		p->supported_styles[0] = (XIMPreeditCallbacks | XIMStatusCallbacks | 
					  XIMLookupCallbacks  | XIMAuxCallbacks);
#endif /* OLD_CODE */	
        } else {
	    fprintf(stderr,
	      "XGetIMValues currently does not recognize this IMValue: %s\n",
	      (char *)next);
	}
    }

}    

Public char *
#ifndef NO_PLUGGIN
_XSunmbResetIC(ic)
#else /* NO_PLUGGIN */
XmbResetIC(ic)
#endif /* NO_PLUGGIN */
    Xic ic;
{
    struct iml_inst *rv;
    char *rb = Xmalloc(DEFAULTPreEditTextBufferSize);
    
    if(ic){
	if((int)IMLogic(ic->id,&rv,IML_COMMIT,NULL,NULL) < 0 ) {
	    return(NULL);
	}
	interpret_opcode(ic,rv,rb,DEFAULTPreEditTextBufferSize,NULL);
	return(rb);
    } 
    return(NULL);
}

Public wchar_t *
#ifndef NO_PLUGGIN
_XSunwcResetIC(ic)
#else /* NO_PLUGGIN */
XwcResetIC(ic)
#endif /* NO_PLUGGIN */
    Xic ic;
{
    struct iml_inst *rv;

    IMLogic(ic->id, &rv, IML_COMMIT, NULL, NULL); 
}

#ifdef NO_PLUGGIN 
Public Display *
XDisplayOfIM(im) 
    Xim	im;
{
    return (im->display);
}

Public XIM
XIMOfIC(ic) 
    Xic	ic;
{
    return ((XIM)ic->im);
}

Public char *
XLocaleOfIM(im)
    Xim im;
{
    return(im->locale);
}
#endif /* NO_PLUGGIN */

/*
 * replaces XLookupString, in charge of pre-editing
 * and calls the callbacks.
 */
Public int
#ifndef NO_PLUGGIN
_XSunmbLookupString(ic, e, rb, rbsz, rkeysym, rstatus)
#else /* NO_PLUGGIN */
XmbLookupString(ic, e, rb, rbsz, rkeysym, rstatus)
#endif /* NO_PLUGGIN */
    Xic ic;           
    XKeyEvent *e;     
    char      *rb;      /* return buffer */
    int        rbsz;    /* return buffer size */
    KeySym    *rkeysym; /* return keysym */
    int       *rstatus; /* return status */
{
    struct iml_inst *rv;

    if(ic){
	KeySym keysym;
	/*
	 * IMLogic $@$O!"(Jic $@0MB8$N(J XLookupString $@$NBeBX(J routine $@$H0LCV$E$1$k(J
	 */
	if((int)(keysym = IMLogic(ic->id,&rv,IML_SEND_EVENT,e,NULL)) < 0 ) {
	    *rstatus = XLookupNone;
	    return(0);
	}
	*rstatus = interpret_opcode(ic,rv,rb,rbsz,e,0);
	if(rkeysym) {
	    if(*rstatus == XLookupNone){
		if (keysym != NoSymbol) {
		    *rkeysym = keysym ;
		    *rstatus = XLookupKeySym ;
		}
	    } else {
		*rkeysym = keysym ;
	    } 
	}
	return(strlen(rb));
    } else {
	/*
	 * IC $@$J$7$G(J $@$+$J(J $@$r07$*$&$J$s$F#1#0G/$O$d$$!#(J
	 * $@$+$J$O(J $@L5;k(J $@$7$F$?$@(J XLookupString $@$9$k(J
	 * If this routine is invoked without valid ic,
	 * We just call XLookupString and return.
	 * We do not care about kana or some other
	 * special keysyns.
	 */
	XComposeStatus NotSupportedYet ;
	int l ;
	
	l = XLookupString(e, rb, rbsz, rkeysym, &NotSupportedYet);
	if (rkeysym && *rkeysym == NoSymbol){
	    *rstatus = XLookupNone ;
	} else if (l>0){
	    *rstatus = XLookupBoth ;
	} else {
	    *rstatus = XLookupKeySym ;
	}
	return(l);
    }
    /* NOT REACHED */
}   
Public int
#ifndef NO_PLUGGIN
_XSunwcLookupString(ic, e, rb, rbsz, rkeysym, rstatus)
#else /* NO_PLUGGIN */
XwcLookupString(ic, e, rb, rbsz, rkeysym, rstatus)
#endif /* NO_PLUGGIN */
    Xic ic;           
    XKeyEvent *e;     
    char      *rb;      /* return buffer */
    int        rbsz;    /* return buffer size */
    KeySym    *rkeysym; /* return keysym */
    int       *rstatus; /* return status */
{
}

Private (*opsw[MAX_IMM_OPCODE])() ;

/*
 * This function expects to be invoked from X*LookupString() basically.
 * The function value is meaningless if it's not invoked from X*LookupString()
 * $@$5$i$K!"Cf$G9/29/91i%s%6%/%7%g%s$,F1$8$+0c$&$+$N12:02:59i%0$D$1$F(JIMLogic $@$r8F$s$G$k(J
 * $@$1$I!"9/29/91i%s%6%/%7%g%s$,F1$8$+0c$&$+$O!"(JX*LookupString $@$N;~$K$7$+!"0UL#$r(J
 * $@$b$?$J$$$N$G!"(Jevent $@$K(J NULL $@$rEO$;$P$h$5$=$&$M!#$H$m$&$+$J!#(J
 */
Private int
interpret_opcode(ic, rv, rb, rbsz, e)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
    char	*rb;   /* return buffer */
    int		rbsz;  /* return buffer size */
    XKeyEvent	*e;    /* If not invoked from X*LooupString, then NULL */
{
    iml_inst 	*i ;
    iml_inst	*rrv = 0;
    int          tr ;
    int          r = XLookupNone ;
    int		 op ;
    
    for (i = rv ; i != NULL ; i = i->next) {
	op = i->opcode & ~IMM_CB_RESULT_REQUIRED ;
	if((op < 0) || (op >= MAX_IMM_OPCODE) ) {
	    break ;
	}
        tr = (*opsw[op])(ic,i,rb,rbsz,e);
	if (i->opcode & IMM_CB_RESULT_REQUIRED) {
	    if((int)IMLogic (ic->id, & rrv, IML_SEND_RESULTS, i, NULL) >= 0) {
		tr = interpret_opcode (ic, rrv, rb, rbsz, e);
	    }
	}
	if(tr != XLookupNone) r = tr ;
    }
    return(r);
}

/*
 * Private members
 */
 
Private 
imm_nop()
{
    return(XLookupNone);
}
Private
imm_set_status(ic, rv)
Xic ic;
iml_inst *rv ;
{
    ic->iml_status = *(iml_status_t *)&(rv->operand) ;
    return(XLookupNone);
}
Private
imm_reset()
{
    /* NOT IMPLEMENTED */
    /* reset the ic */
    return(XLookupNone);
}
Private
imm_commit(ic, rv, rb, rbsz, event)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
    char	*rb;   /* return buffer */
    int		rbsz;  /* return buffer size */
    XKeyEvent	*event;/* If not invoked from X*LooupString, then NULL */
{
    /*
     * FIX ME! This routine can accept only multi byte.
     */
    committed_string_t *cs ;
    int                cslen ;
    
    cs = (committed_string_t*)&rv->operand;
    cslen = strlen(cs->string);
    if (cslen >= rbsz) {
	*rb = '\0';
	return(XBufferOverflow);
    } else {
	strncpy(rb,cs->string,rbsz);
	return(cs->status) ;
    }
}
Private
imm_preedit_start(ic, rv)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
{
    XIMCallback *p = ic->text_start ;
    int (*pp)() ;
    int *i = (int *)&rv->operand;
    if (p) {
	pp = (int(*)())p->callback ;
       *i = (pp)(ic,p->client_data, NULL);
    }
    return(XLookupNone);
}
Private
imm_preedit_draw(ic, rv)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
{
    XIMCallback *p = ic->text_draw ;
    if (p) {
       (p->callback)(ic,p->client_data,&(rv->operand));
    }
    return(XLookupNone);
}
Private
imm_preedit_done(ic, rv)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
{
    XIMCallback *p = ic->text_done ;
    if (p) {
       (p->callback)(ic,p->client_data,&(rv->operand));
    }
    return(XLookupNone);
}
Private
imm_status_start(ic, rv)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
{
    XIMCallback *p = ic->status_start ;
    if (p) {
       (p->callback)(ic,p->client_data,&(rv->operand));
    }
    return(XLookupNone);
}
Private
imm_status_draw(ic, rv)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
{
    XIMCallback *p = ic->status_draw ;
    if (p) {
       (p->callback)(ic,p->client_data,&(rv->operand));
    }
    return(XLookupNone);
}
Private
imm_status_done(ic, rv)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
{
    XIMCallback *p = ic->status_done ;
    if (p) {
       (p->callback)(ic,p->client_data,&(rv->operand));
    }
    return(XLookupNone);
}

Private
imm_lookup_start(ic, rv)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
{
    XIMCallback *p = ic->lookup_start ;
    if (p) {
       (p->callback)(ic,p->client_data,&(rv->operand));
    }
    return(XLookupNone);
}
Private
imm_lookup_draw(ic, rv)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
{
    XIMCallback *p = ic->lookup_draw ;
    if (p) {
       (p->callback)(ic,p->client_data,&(rv->operand));
    }
    return(XLookupNone);
}
Private
imm_lookup_process(ic, rv)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
{
    XIMCallback *p = ic->lookup_process ;
    if (p) {
       (p->callback)(ic,p->client_data,&(rv->operand));
    }
    return(XLookupNone);
}
Private
imm_lookup_done(ic, rv)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
{
    XIMCallback *p = ic->lookup_done ;
    if (p) {
       (p->callback)(ic,p->client_data,&(rv->operand));
    }
    return(XLookupNone);
}
Private
imm_aux_start(ic, rv)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
{
    XIMCallback *p = ic->aux_start ;
    if (p) {
       (p->callback)(ic,p->client_data,&(rv->operand));
    }
    return(XLookupNone);
}
Private
imm_aux_draw(ic, rv)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
{
    XIMCallback *p = ic->aux_draw ;
    if (p) {
       (p->callback)(ic,p->client_data,&(rv->operand));
    }
    return(XLookupNone);
}
Private
imm_aux_done(ic, rv)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
{
    XIMCallback *p = ic->aux_done ;
    if (p) {
       (p->callback)(ic,p->client_data,&(rv->operand));
    }
    return(XLookupNone);
}
Private
imm_key_info(ic, rv)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
{
    keymaps_t *keymaps = (keymaps_t *)&rv->operand ;
    ic->keymaps = keymaps ;
    return(XLookupNone);
}
Private
imm_put_queue(ic, rv, rb, rbsz, event)
    Xic ic;
    iml_inst    *rv;   /* return values from IMLogin() to be interpreted */
    char	*rb;   /* return buffer */
    int		rbsz;  /* return buffer size */
    XKeyEvent	*event;/* If not invoked from X*LooupString, then NULL */
{
    /* only set ic->conv_on to ON in XmbLookupString */
    /*
     * PARANOIA: check event.
     */
    if(event){
	XPutBackEvent((Display *)(XDisplayOfIM((XIM)XIMOfIC(ic))), event);
    }
    return(XLookupNone);
}


Private void
immgr_init()
{
    if(opsw[IMM_NOP]) return ;
    
    opsw[IMM_NOP]		= imm_nop ;
    opsw[IMM_SET_STATUS]	= imm_set_status ;
    opsw[IMM_RESET]		= imm_reset ;
    opsw[IMM_COMMIT]		= imm_commit ;
    opsw[IMM_KEY_INFO]		= imm_key_info ;
    opsw[IMM_PUT_QUEUE]		= imm_put_queue ;
    
    opsw[IMM_PREEDIT_START]	= imm_preedit_start ;
    opsw[IMM_PREEDIT_DRAW]	= imm_preedit_draw ;
    opsw[IMM_PREEDIT_DONE]	= imm_preedit_done ;
    
    opsw[IMM_STATUS_START]	= imm_status_start ;
    opsw[IMM_STATUS_DRAW]	= imm_status_draw ;
    opsw[IMM_STATUS_DONE]	= imm_status_done ;
    
    opsw[IMM_LOOKUP_START]	= imm_lookup_start ;
    opsw[IMM_LOOKUP_DRAW]	= imm_lookup_draw ;
    opsw[IMM_LOOKUP_PROCESS]	= imm_lookup_process ;
    opsw[IMM_LOOKUP_DONE]	= imm_lookup_done ;
    
    opsw[IMM_AUX_START]		= imm_aux_start ;
    opsw[IMM_AUX_DRAW]		= imm_aux_draw ;
    opsw[IMM_AUX_DONE]		= imm_aux_done ;
}

#ifndef NO_PLUGGIN
static XICMethodsRec ic_methods = {
    _XSunDestroyIC,
    _XSunSetICFocus,
    _XSunUnsetICFocus,
    _XSunSetICValues,
    _XSunGetICValues,
    _XSunmbResetIC,
    _XSunwcResetIC,
    _XSunmbLookupString,
    _XSunwcLookupString
};
#endif /* NO_PLUGGIN */

Private Status
sun_ic_conversion(ic, op, henkan_mode)
Xic ic ;
XICOp_t op;
int henkan_mode ;
{
    iml_inst 	*rv;
    wchar_t      buffer_return[1024];
    iml_status_t status = ic->iml_status ;

    if(henkan_mode){
	status |= IMLSTATUS_Henkan_Mode ;
    } else {
	status &= ~IMLSTATUS_Henkan_Mode ;
    }
    
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	/*
	 * NOT Implemented Yet
	 */
	if(IMLogic(ic->id, &rv, IML_SET_STATUS, &status, NULL)<0){
	    return(-1);
	}
	interpret_opcode(ic, rv, buffer_return, 1024, NULL, 0);
	break ;
    case XICOpGet:
	/*
	 * NOT surely implemented Yet. Need attention.
	 */
	if(IMLogic(ic->id,&rv, IML_GET_STATUS, NULL, NULL)<0){
	    return(-1);
	}
	interpret_opcode(ic, rv, buffer_return, 1024, NULL, 0);
	*(Bool*)henkan_mode = ic->iml_status & IMLSTATUS_Henkan_Mode ;
	break;
    }
    return(0);
}

Private Status
ic_client_window(ic, op, p)
Xic ic ;
XICOp_t op;
caddr_t p ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	ic->client_window = (Window) p ;
	break;
    case XICOpGet:
	* (Window *)p = ic->client_window ;
	break;
    }
    return(0);
}
Private Status
ic_focus_window(ic, op, p)
Xic ic ;
XICOp_t op;
caddr_t p ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	ic->focus_window = (Window) p ;
	break;
    case XICOpGet:
	* (Window *)p = ic->focus_window ;
	break;
    }
    return(0);
}
Private Status
ic_input_style(ic, op, p)
Xic ic ;
XICOp_t op;
caddr_t p ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	ic->input_style = (unsigned short)p ;
	break;
    case XICOpGet:
	*(unsigned short *) p = ic->input_style ;
	break;
    }
    return(0);
}
Private Status
ic_preedit_start_callback(ic, op, cb)
Xic ic ;
XICOp_t op;
XIMCallback *cb ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	if(!ic->text_start)
	  ic->text_start = (XIMCallback *)malloc(sizeof(XIMCallback));
	ic->text_start->client_data =(XPointer)(cb->client_data);
	ic->text_start->callback=(XIMProc)(cb->callback);
	break ;
    case XICOpGet:
	*(XIMCallback **)cb = ic->text_start ;
	break ;
    }
    return(0);
}
Private Status
ic_preedit_draw_callback(ic, op, cb)
Xic ic ;
XICOp_t op;
XIMCallback *cb ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	if(!ic->text_draw)
	  ic->text_draw = (XIMCallback *)malloc(sizeof(XIMCallback));
	ic->text_draw->client_data =(XPointer)(cb->client_data);
	ic->text_draw->callback=(XIMProc)(cb->callback);
	break ;
    case XICOpGet:
	*(XIMCallback **)cb = ic->text_draw ;
	break ;
    }
    return(0);
}
Private Status
ic_preedit_done_callback(ic, op, cb)
Xic ic ;
XICOp_t op;
XIMCallback *cb ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	if(!ic->text_done)
	  ic->text_done = (XIMCallback *)malloc(sizeof(XIMCallback));
	ic->text_done->client_data =(XPointer)(cb->client_data);
	ic->text_done->callback=(XIMProc)(cb->callback);
	break ;
    case XICOpGet:
	*(XIMCallback **)cb = ic->text_done ;
	break ;
    }
    return(0);
}
Private Status
ic_preedit_caret_callback(ic, op, cb)
Xic ic ;
XICOp_t op;
XIMCallback *cb ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	if(!ic->text_caret)
	  ic->text_caret = (XIMCallback *)malloc(sizeof(XIMCallback));
	ic->text_caret->client_data =(XPointer)(cb->client_data);
	ic->text_caret->callback=(XIMProc)(cb->callback);
	break ;
    case XICOpGet:
	*(XIMCallback **)cb = ic->text_caret ;
	break ;
    }
    return(0);
}
Private Status
ic_status_start_callback(ic, op, cb)
Xic ic ;
XICOp_t op;
XIMCallback *cb ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	if(!ic->status_start)
	  ic->status_start = (XIMCallback *)malloc(sizeof(XIMCallback));
	ic->status_start->client_data =(XPointer)(cb->client_data);
	ic->status_start->callback=(XIMProc)(cb->callback);
	break ;
    case XICOpGet:
	*(XIMCallback **)cb = ic->status_start ;
	break ;
    }
    return(0);
}
Private Status
ic_status_draw_callback(ic, op, cb)
Xic ic ;
XICOp_t op;
XIMCallback *cb ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	if(!ic->status_draw)
	  ic->status_draw = (XIMCallback *)malloc(sizeof(XIMCallback));
	ic->status_draw->client_data =(XPointer)(cb->client_data);
	ic->status_draw->callback=(XIMProc)(cb->callback);
	break ;
    case XICOpGet:
	*(XIMCallback **)cb = ic->status_draw ;
	break ;
    }
    return(0);
}
Private Status
ic_status_done_callback(ic, op, cb)
Xic ic ;
XICOp_t op;
XIMCallback *cb ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	if(!ic->status_done)
	  ic->status_done = (XIMCallback *)malloc(sizeof(XIMCallback));
	ic->status_done->client_data =(XPointer)(cb->client_data);
	ic->status_done->callback=(XIMProc)(cb->callback);
	break ;
    case XICOpGet:
	*(XIMCallback **)cb = ic->status_done ;
	break ;
    }
    return(0);
}
Private Status
ic_lookup_start_callback(ic, op, cb)
Xic ic ;
XICOp_t op;
XIMCallback *cb ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	if(!ic->lookup_start)
	  ic->lookup_start = (XIMCallback *)malloc(sizeof(XIMCallback));
	ic->lookup_start->client_data =(XPointer)(cb->client_data);
	ic->lookup_start->callback=(XIMProc)(cb->callback);
	break ;
    case XICOpGet:
	*(XIMCallback **)cb = ic->lookup_start ;
	break ;
    }
    return(0);
}
Private Status
ic_lookup_draw_callback(ic, op, cb)
Xic ic ;
XICOp_t op;
XIMCallback *cb ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	if(!ic->lookup_draw)
	  ic->lookup_draw = (XIMCallback *)malloc(sizeof(XIMCallback));
	ic->lookup_draw->client_data =(XPointer)(cb->client_data);
	ic->lookup_draw->callback=(XIMProc)(cb->callback);
	break ;
    case XICOpGet:
	*(XIMCallback **)cb = ic->lookup_draw ;
	break ;
    }
    return(0);
}
Private Status
ic_lookup_done_callback(ic, op, cb)
Xic ic ;
XICOp_t op;
XIMCallback *cb ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	if(!ic->lookup_done)
	  ic->lookup_done = (XIMCallback *)malloc(sizeof(XIMCallback));
	ic->lookup_done->client_data =(XPointer)(cb->client_data);
	ic->lookup_done->callback=(XIMProc)(cb->callback);
	break ;
    case XICOpGet:
	*(XIMCallback **)cb = ic->lookup_done ;
	break ;
    }
    return(0);
}
Private Status
ic_lookup_process_callback(ic, op, cb)
Xic ic ;
XICOp_t op;
XIMCallback *cb ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	if(!ic->lookup_process)
	  ic->lookup_process = (XIMCallback *)malloc(sizeof(XIMCallback));
	ic->lookup_process->client_data =(XPointer)(cb->client_data);
	ic->lookup_process->callback=(XIMProc)(cb->callback);
	break ;
    case XICOpGet:
	*(XIMCallback **)cb = ic->lookup_process ;
	break ;
    }
    return(0);
}
Private Status
ic_aux_start_callback(ic, op, cb)
Xic ic ;
XICOp_t op;
XIMCallback *cb ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	if(!ic->aux_start)
	  ic->aux_start = (XIMCallback *)malloc(sizeof(XIMCallback));
	ic->aux_start->client_data =(XPointer)(cb->client_data);
	ic->aux_start->callback=(XIMProc)(cb->callback);
	break ;
    case XICOpGet:
	*(XIMCallback **)cb = ic->aux_start ;
	break ;
    }
    return(0);
}
Private Status
ic_aux_draw_callback(ic, op, cb)
Xic ic ;
XICOp_t op;
XIMCallback *cb ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	if(!ic->aux_draw)
	  ic->aux_draw = (XIMCallback *)malloc(sizeof(XIMCallback));
	ic->aux_draw->client_data =(XPointer)(cb->client_data);
	ic->aux_draw->callback=(XIMProc)(cb->callback);
	break ;
    case XICOpGet:
	*(XIMCallback **)cb = ic->aux_draw ;
	break ;
    }
    return(0);
}
Private Status
ic_aux_process_callback(ic, op, cb)
Xic ic ;
XICOp_t op;
XIMCallback *cb ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	if(!ic->aux_process)
	  ic->aux_process = (XIMCallback *)malloc(sizeof(XIMCallback));
	ic->aux_process->client_data =(XPointer)(cb->client_data);
	ic->aux_process->callback=(XIMProc)(cb->callback);
	break ;
    case XICOpGet:
	*(XIMCallback **)cb = ic->aux_process ;
	break ;
    }
    return(0);
}
Private Status
ic_aux_done_callback(ic, op, cb)
Xic ic ;
XICOp_t op;
XIMCallback *cb ;
{
    switch(op){
    case XICOpCreate:
    case XICOpSet:
	if(!ic->aux_done)
	  ic->aux_done = (XIMCallback *)malloc(sizeof(XIMCallback));
	ic->aux_done->client_data =(XPointer)(cb->client_data);
	ic->aux_done->callback=(XIMProc)(cb->callback);
	break ;
    case XICOpGet:
	*(XIMCallback **)cb = ic->aux_done ;
	break ;
    }
    return(0);
}
Private Status
ic_backfront(ic, op, backfront)
Xic ic ;
XICOp_t op;
Bool   backfront ;
{
    iml_inst 	*rv;
    wchar_t      buffer_return[1024];
    iml_status_t status = ic->iml_status ;

    switch(op){
    case XICOpCreate:
    case XICOpSet:
	/*
	 * frontend/backend $@$I$A$i$G(J IML $@$rF0$+$9$+$r;X<(!#(J
	 */
	if(backfront){
	    status |= IMLSTATUS_Backfront ;
	} else {
	    status &= ~IMLSTATUS_Backfront ;
	}
	if(IMLogic(ic->id, &rv, IML_SET_STATUS, &status, NULL)<0){
	    return(-1);
	}
	interpret_opcode(ic, rv, buffer_return, 1024, NULL, 0);
	break ;
    case XICOpGet:
	/*
	 * frontend/backend $@$I$A$i$r(J IML $@$,?d>)$9$k$+$rJ9$/!#(J
	 */
	if(IMLogic(ic->id,&rv, IML_GET_STATUS, NULL, NULL)<0){
	    return(-1);
	}
	interpret_opcode(ic, rv, buffer_return, 1024, NULL, 0);
	*(Bool *)backfront = IMLSTATUS_Backfront_Recommendation &
			       ic->iml_status  ;
	break;
    }
    return(0);
}

Private void
init_ic(ic)
Xic	ic;
{
    strcpy(ic->language,"");
    ic->client_window	= 0;
    ic->focus_window	= 0;
    ic->input_style	= 0;
    ic->text_start 	= NULL;
    ic->text_draw 	= NULL;
    ic->text_done 	= NULL;
    ic->text_caret 	= NULL;
    ic->status_start 	= NULL;
    ic->status_draw 	= NULL;
    ic->status_done 	= NULL;
    ic->lookup_start 	= NULL;
    ic->lookup_draw 	= NULL;
    ic->lookup_process 	= NULL;
    ic->lookup_done 	= NULL;
    ic->aux_start 	= NULL;
    ic->aux_draw 	= NULL;
    ic->aux_process 	= NULL;
    ic->aux_done 	= NULL;

    
}

Private Status nested_list();

Private
icop_t icoptbl[] = {
XNClientWindow			,ic_client_window,
XNFocusWindow			,ic_focus_window,
XNInputStyle			,ic_input_style,
XNPreeditStartCallback		,ic_preedit_start_callback,
XNPreeditDrawCallback		,ic_preedit_draw_callback,
XNPreeditDoneCallback		,ic_preedit_done_callback,
XNPreeditCaretCallback		,ic_preedit_caret_callback,
XNStatusStartCallback		,ic_status_start_callback,
XNStatusDrawCallback		,ic_status_draw_callback,
XNStatusDoneCallback		,ic_status_done_callback,
XNExtXimp_LookupStartCallback	,ic_lookup_start_callback,
XNExtXimp_LookupDrawCallback	,ic_lookup_draw_callback,
XNExtXimp_LookupDoneCallback	,ic_lookup_done_callback,
XNExtXimp_LookupProcessCallback	,ic_lookup_process_callback,
XNExtXimp_AuxStartCallback	,ic_aux_start_callback,
XNExtXimp_AuxDrawCallback	,ic_aux_draw_callback,
XNExtXimp_AuxProcessCallback	,ic_aux_process_callback,
XNExtXimp_AuxDoneCallback	,ic_aux_done_callback,
XNExtXimp_Conversion		,sun_ic_conversion,
XNExtXimp_Backfront             ,ic_backfront,
XNPreeditAttributes		,nested_list,
XNStatusAttributes		,nested_list,
XNExtXimp_LookupAttributes	,nested_list,
XNExtXimp_AuxAttributes		,nested_list,
XNVaNestedList			,nested_list,
0
};
#define ICOPTBLSIZE ((sizeof(icoptbl)/sizeof(icop_t)) - 1)

Private void
immgr_icop(ic, args_list, op, param, status)
Xic         ic;
XIMArg 	    *args_list;
XICOp_t	    op;
XIMArg      *param;
int	    *status;

{    
register i ;

    for(i = 0 ; i< ICOPTBLSIZE ; i++){
	if(param->name[0] ==  icoptbl[i].name[0]){ /* For faster comparison */
	    if(!strcmp(param->name + 1, icoptbl[i].name + 1)){
		*status = ((*(icoptbl[i].func))(ic, op, param->value));
		break ;
	    }
	}
    }    
    *status = 0;
}

Private Status
nested_list(ic, op, args)
Xic ic ;
XICOp_t op;
XIMArg *args ;
{
    register i ;
    int status ;
    register XIMArg *arg;

    for (arg = args; arg->name && *(arg->name); arg++) {
	immgr_icop(ic, args, op, arg, &status );
	if(status){
	    return (Status)arg ;
	}
    }
    return 0 ;
}  

Private void
immgr_icop_init(ic)
Xic	ic;
{
#ifndef NO_PLUGGIN
     ic->methods = (XICMethods *)&ic_methods;
#ifdef notdef     
     ic->core.next =   ;
     ic->core.client_window =  ;
     ic->core.input_style =   ;
     ic->core.focus_window =  ;
     ic->core.res_database =   ;
     ic->core.string_database =  ;
     ic->core.geometry_callback =   ;
     ic->core.preedit_state =  ;
     ic->core.filter_events =   ;
     ic->core.preedit_attr =  ;
     ic->core.status_attr =   ;
#endif     
#endif /* NO_PLUGGIN */
     ic->text_start = ic->text_draw = ic->text_done = ic->text_caret  = 
     ic->status_start = ic->status_draw = ic->status_done =  
     ic->lookup_start = ic->lookup_draw = ic->lookup_process = 
     ic->lookup_done  =  
     ic->aux_start = ic->aux_draw = ic->aux_process = ic->aux_done  =
      NULL;    
    /*
     * Make hash table for ic opration. Not implement yet.
     */
}

#ifndef NO_PLUGGIN
Public XIC
_XSunCreateIC(im, args)
Xim 	im;
XIMArg  *args;
#else /* NO_PLUGGIN */
Public XIC
XCreateIC(im, va_alist)
Xim 	im;
va_dcl
#endif /* NO_PLUGGIN */
{
#ifdef NO_PLUGGIN
    va_list 		var;
    XIMArg 		*args = (XIMArg *)XVaCreateNestedList(var, va_alist);
#endif /* NO_PLUGGIN */
    XIMArg              *arg;
    Xic         ic;
    iml_inst 	*rv;
    int		status;
    wchar_t	buffer_return[512];
    int		ret_status = NULL;
    
    ic = (Xic)malloc(sizeof(xic));
    immgr_icop_init(ic);
    ic->im = im;
    ic->iml_status = 0 ;
    strcpy(ic->language, im->locale);
#ifndef NO_PLUGGIN
     ic->core.im =  (XIM)im;
#endif /* NO_PLUGGIN */    

    if((status = IMLogic(NULL, &rv, IML_OPEN, NULL, NULL)) >= 0 ) {
        ic->id = status;
        interpret_opcode(ic, rv, buffer_return, 512, &ret_status, 0);
    } else {
	goto error;
    }
    if((status = IMLogic(ic->id, &rv, IML_GET_STATUS, NULL, NULL)) >= 0 ){
        interpret_opcode(ic, rv, buffer_return, 512, &ret_status, 0);
    }
    for (arg = args; arg->name && *(arg->name); arg++) {
	immgr_icop(ic, args, XICOpCreate, arg, &status );
	if(status){
	    goto error;
	}
    }
    if((!ic->input_style)||(!ic->client_window)){
	goto error;
    }
    return (XIC)ic;
error:
    free(ic);
    return NULL;
}

Public void
#ifndef NO_PLUGGIN
_XSunDestroyIC(ic)
#else
XDestroyIC(ic)
#endif
Xic	ic;
{
    iml_inst rv;
    int nbytes_returned;
    
    IMLogic(ic->id, &rv, IML_CLOSE, NULL, NULL);
    /*
     * Return values will be ignored
     */
#ifdef NO_PLUGGIN
    free(ic);
#endif    
}

Public char *
#ifndef NO_PLUGGIN
_XSunSetICValues(ic, args) 
Xic         ic;
XIMArg  *args;
#else /* NO_PLUGGIN */
XSetICValues(ic, va_alist) 
Xic         ic;
va_dcl
#endif /* NO_PLUGGIN */
{
#ifdef NO_PLUGGIN
    va_list 		var;
    XIMArg 		*args = (XIMArg *)XVaCreateNestedList(var, va_alist);
#endif /* NO_PLUGGIN */
    XIMArg              *arg;
    int 	status ;

    if (!ic) {
        fprintf(stderr,"Nil IC in XSetICValue!\n");
        return (arg->name);
    }
    for (arg = args; arg->name && *(arg->name); arg++) {
	immgr_icop(ic, args, XICOpSet, arg, &status) ;
	if(status){
	    return(arg->name);
	}
    }
    return(NULL);
}

Public char *
#ifndef NO_PLUGGIN
_XSunGetICValues(ic, args)
Xic         ic;
XIMArg      *args;
#else /* NO_PLUGGIN */
XGetICValue(ic, va_alist)
Xic         ic;
va_dcl
#endif /* NO_PLUGGIN */
{
#ifdef NO_PLUGGIN
    va_list 		var;
    XIMArg 		*args = (XIMArg *)XVaCreateNestedList(var, va_alist);
#endif /* NO_PLUGGIN */
    XIMArg              *arg;
    caddr_t	param;
    char	*ret_string=NULL;
    int 	status ;
    
    if (!ic) {
        fprintf(stderr,"Nil IC in XSetICValue!\n");
        return (arg->name);
    }
    for (arg = args; arg->name && *(arg->name); arg++) {
	immgr_icop(ic, args, XICOpGet, arg, &status) ;
	if(status){
	    return(arg->name);
	}
    }
    return(NULL);
}


#ifndef NO_PLUGGIN
void
_XSunUnsetICFocus(ic)
Xic ic;
{
}

#endif /* NO_PLUGGIN */

void
_XSunSetICFocus(ic)
Xic ic;
{
    int ret, status, root_x, root_y, win_x, win_y;
    Window root, child;
    unsigned int keys_buttons;
    iml_inst *rv;
    IMKeyState	keystate;
    
    if (!ic) {
	fprintf(stderr,"Nil IC in XSetICFocus!\n");
	return;
    }
    ret = XQueryPointer(ic->im->display, ic->client_window, &root, &child,
			&root_x, &root_y, &win_x, &win_y, &keys_buttons);
    if (keys_buttons & Mod3Mask) {
	keystate = MODE_SWITCH_ON;
    } else {
	keystate = MODE_SWITCH_OFF;
    }
    status = IMLogic(ic->id, &rv, IML_SET_KEYBOARD_STATE, &keystate, NULL);    
    if (status >=0) {
	/* Special case this IMM_STATUS_DRAW */
        if ((int)rv->opcode == IMM_STATUS_DRAW) {
	    if (ic->status_draw) {
		(ic->status_draw->callback)(ic, ic->status_draw->client_data, &(rv->operand));
	    }
    	}
    }
}

#if XlibSpecificationRelease != 5

#ifdef NO_PLUGGIN
Public Bool
XFilterEvent(){
    return False ;
}
#endif /* NO_PLUGGIN */

/*
 * For the compatibility between the version using pluggable layer interface
 * and not using pluggable layer interface.
 *
 * Following in partially taken from 
 * 	$XConsortium: XICWrap.c,v 11.7 91/05/30 13:10:36 rws Exp $
 */

/*
 * Copyright 1990, 1991 by OMRON Corporation, NTT Software Corporation,
 *                      and Nippon Telegraph and Telephone Corporation
 * Copyright 1991 by the Massachusetts Institute of Technology
 * Copyright 1991 by the Open Software Foundation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON, NTT Software, NTT, Open
 * Software Foundation and M.I.T. not be used in advertising or 
 * publicity pertaining to distribution of the software without specific, 
 * written prior permission. OMRON, NTT Software, NTT, Open Software
 * Foundation and M.I.T. make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * OMRON, NTT SOFTWARE, NTT, OPEN SOFTWARE FOUNDATION AND M.I.T. 
 * DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
 * SHALL OMRON, NTT SOFTWARE, NTT, OPEN SOFTWARE FOUNDATIONN OR M.I.T. BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 *	Authors: Li Yuhong		OMRON Corporation
 *		 Tatsuya Kato		NTT Software Corporation
 *		 Hiroshi Kuribayashi	OMRON Coproration
 *		 Muneiyoshi Suzuki	Nippon Telegraph and Telephone Co.
 * 
 *		 M. Collins		OSF  
 */				

#if NeedVarargsPrototypes
# include <stdarg.h>
# define Va_start(a,b) va_start(a,b)
#else
# include <varargs.h>
# define Va_start(a,b) va_start(a)
#endif

Private int
_XIMNestedListToNestedList(nlist, list)
    XIMArg *nlist;   /* This is the new list */
    XIMArg *list;    /* The original list */
{
    register XIMArg *ptr = list;

    while (ptr->name) {
	if (!strcmp(ptr->name, XNVaNestedList)) {
	    nlist += _XIMNestedListToNestedList(nlist, (XIMArg *)ptr->value);
	} else {
	    nlist->name = ptr->name;
	    nlist->value = ptr->value;
	    ptr++;
	    nlist++;
	}
    }
    return ptr - list;
}

Private void
_XIMCountNestedList(args, total_count)
    XIMArg *args;
    int *total_count;
{
    for (; args->name; args++) {
	if (!strcmp(args->name, XNVaNestedList))
	    _XIMCountNestedList((XIMArg *)args->value, total_count);
	else
	    ++(*total_count);
    }
}

#if NeedVarargsPrototypes
Private void
_XIMCountVaList(va_list var, int *total_count)
#else
Private void
_XIMCountVaList(var, total_count)
    va_list var;
    int *total_count;
#endif
{
    char *attr;

    *total_count = 0;

    for (attr = va_arg(var, char*); attr; attr = va_arg(var, char*)) {
	if (!strcmp(attr, XNVaNestedList)) {
	    _XIMCountNestedList(va_arg(var, XIMArg*), total_count);
	} else {
	    va_arg(var, XIMArg*);
	    ++(*total_count);
	}
    }
}

#if NeedVarargsPrototypes
Private void
_XIMVaToNestedList(va_list var, int max_count, XIMArg **args_return)
#else
Private void
_XIMVaToNestedList(var, max_count, args_return)
    va_list var;
    int max_count;
    XIMArg **args_return;
#endif
{
    XIMArg *args;
    char   *attr;

    if (max_count <= 0) {
	*args_return = (XIMArg *)NULL;
	return;
    }

    args = (XIMArg *)Xmalloc((unsigned)(max_count + 1) * sizeof(XIMArg));
    *args_return = args;
    if (!args) return;

    for (attr = va_arg(var, char*); attr; attr = va_arg(var, char*)) {
	if (!strcmp(attr, XNVaNestedList)) {
	    args += _XIMNestedListToNestedList(args, va_arg(var, XIMArg*));
	} else {
	    args->name = attr;
	    args->value = va_arg(var, XPointer);
	    args++;
	}
    }
    args->name = (char*)NULL;
}

/*ARGSUSED*/
Public
#if NeedVarargsPrototypes
XVaNestedList
XVaCreateNestedList(int dummy, ...)
#else
XVaNestedList
XVaCreateNestedList(dummy, va_alist)
    int dummy;
    va_dcl
#endif
{
    va_list		var;
    XIMArg		*args = NULL;
    int			total_count;

    Va_start(var, dummy);
    _XIMCountVaList(var, &total_count);
    va_end(var);

    Va_start(var, dummy);
    _XIMVaToNestedList(var, total_count, &args);
    va_end(var);

    return (XVaNestedList)args;
}

#endif /* XlibSpecificationRelease */
