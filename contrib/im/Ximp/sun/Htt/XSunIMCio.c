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
#include <X11/Xutil.h>

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

#ifndef ENV_SET_SIMPLE_MODE
/*
 * This is for ALE support. JLE header file does not contains this
 * declaration.
 * ENV_SET_SIMPLE_MODE((e_char *)data.interm.itext.text.text_mb) sets the mode to data.interm.itext.text.text_mb,
 * ENV will display the corresponding string for the mode.
 */
#	define ENV_SET_SIMPLE_MODE     117
#endif
/*
 * cio session public member functions
 */

Public int _iml_cio_constructor();
Private void cio_make_conversion_on();
Private iml_inst ** cio_eval_it();
Public void iml_cio_cmd();

Public int
_iml_cio_constructor(id, rrv)
int id ;
iml_inst **rrv;
{
int	ret;
	
	ret = cio_open(id, rrv);
	return(ret);
}

Private int
cio_send_event(s, rrv, key_event)
iml_session_t	*s ;
iml_inst	**rrv ;
XKeyEvent	*key_event ;
{
    char		localename[LOCALENAMELEN];
    Private Ci_result	result ;
    Private Ci_result	*rp = &result ;
    Private Ci_request	request ;
    int            	keyfunc,keycnt,onoffkey_pushed,i;
    Ci_keycode     	*ci_key,*setkey();
    int			l;
    XComposeStatus	NotSuppoted ;
	static u_short	attr = 0;


    if(key_event && key_event->type != KeyPress){
	return ;
    }	

    if(s->remainder){
        *rrv = s->remainder ;
        s->remainder = NULL ;
        _iml_post_proc(s, rrv);
        return ;
    } else {
        _iml_delete(s);
    }
    s->event = key_event ; /* save KeyEvent */
	
	l = XLookupString(s->event,s->XLookupBuf,s->XLookupBuflen,
						&s->keysym,&NotSuppoted);
    if(l<=0) {
        l=_iml_lookupKanaString(s->XLookupBuf,s->XLookupBuflen,&s->keysym);
    }      
    if(! (s->public_status & IMLSTATUS_Henkan_Mode)){
        iml_inst *rv = *rrv ;
        if(s->keysym == XK_Henkan || checkonoff( s->keysym )){
            cio_make_conversion_on(s, rrv);
            return 0 ;
        }

        if(l<=0) {
            _iml_fake_nop_inst(s, rrv, s->event);
            return -1;
        }
        _imm_trace("IMM_COMIT(off)",rrv,*rrv);
        return  _iml_make_commit_inst(s, rrv, l, s->XLookupBuf) ;
    }

    switch(s->keysym){
	/*
	 * IMLogic doesn't want to lookup X keysym but immgr is not 
	 * enough smart so IMLogic have to lookup X keysym.
	 */
      case XK_Control_L:
      case XK_Control_R:
      case XK_Shift_L:
      case XK_Shift_R:
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
	_iml_fake_nop_inst(s, rrv, s->event);
	return;
      default:	
		ci_key = setkey( s->keysym );
		if(key_event->state & ControlMask){
			if(ci_key->code >= 0x<61 && ci_key->code <= 0x7e ){
				ci_key->code -= 0x60;
			}else if(ci_key->code >= 0x<41 && ci_key->code <= 0x5e){
				ci_key->code -= 0x40;
			}
			if(ci_key->code == 0xfffd) 
				ci_key->code = 0x1d;
		}
		keyfunc = CS_CHAR_EVENT;
		keycnt = 1;
		onoffkey_pushed = 0;
		break;
	}

	if(cnvon == 0 && onoffkey_pushed == 1){
		request.operation._cmd.active_regtype = CI_REGION_UNKNOWN ;
	} else if(cnvon == 1 && selon == 0) {
		request.operation._cmd.active_regtype = CI_REGION_INTERM ;
	} else if(cnvon == 1 && selon == 1) {
		request.operation._cmd.active_regtype = CI_REGION_SELECT ;
	} else if(cnvon == 1 && miscon == 1) {
		request.operation._cmd.active_regtype = CI_REGION_MISC ;
	} else {
#ifdef DEBUG
		if(ci_key && keycnt){
			fprintf(stdout,"[%wc]%c%c",ci_key->keycode,0x0a,0x0d);                        }
#else
		if(ci_key)
/*
			dispstr(keycnt,ci_key);
*/
			cio_eval_it(s, rp, rrv);
#endif
		return;
	}
	request.next = NULL ;
	request.type = CI_REQ_COMMAND ;
	request.operation._cmd.next = NULL ;
	if (selon)
		request.operation._cmd.active_region = select_id ;
	else if (miscon)
		request.operation._cmd.active_region = misc_id ;
	else if(cnvon == 0 && onoffkey_pushed == 1)
		request.operation._cmd.active_region = 0 ;
	else{
		request.operation._cmd.active_region = interm_id ;
	}

	request.operation._cmd.select_info = selreg.sel_info ;

	if ( miscon == 0){                                 
		request.operation._cmd.active_fldtype = CI_FIELD_UNUSED ;
		request.operation._cmd.active_field = 0 ;
	}else{
	    switch(active_f){
	    case 2:
		request.operation._cmd.active_fldtype = CI_FIELD_SELECT ;
		request.operation._cmd.active_field = 2 ;
		request.operation._cmd.select_info = m_sel1reg.sel_info ;
		break;
	    case 3:
		request.operation._cmd.active_fldtype = CI_FIELD_INTERM ;
		request.operation._cmd.active_field = 3 ;
		break;
	    case 4:
		request.operation._cmd.active_fldtype = CI_FIELD_INTERM ;
		request.operation._cmd.active_field = 4 ;
		break;
	    case 5:
		request.operation._cmd.active_fldtype = CI_FIELD_SELECT ;
		request.operation._cmd.active_field = 5 ;
		request.operation._cmd.select_info = m_sel2reg.sel_info ;
		break;
	    }    
	}  

	request.operation._cmd.keyinfo.func = keyfunc ;
	request.operation._cmd.keyinfo.count = keycnt ;
	request.operation._cmd.keyinfo.keycode = ci_key ;
	request.operation._cmd.keyinfo.arg_is_wchar = 0 ;
	request.operation._cmd.keyinfo.argum.char_str = NULL ;
	request.operation._cmd.select_info = selreg.sel_info ;
	request.operation._cmd.user_etcsize = 0 ;
	request.operation._cmd.user_etc = NULL ;
#ifdef DEBUG
if(ci_key)
fprintf(stderr,"region[%d]key[0x%x]func[%d]keycnt[%d]%c%c",
        request.operation._cmd.active_regtype,ci_key->keycode,keyfunc,keycnt,0x0a,0x0d);
#endif

	
	if(s->remainder){
	    *rrv = s->remainder ;
	    s->remainder = NULL ;
	    _iml_post_proc(s, rrv);
	    return;
	}
	rp = ci_put(s->language_engine, s->session_id, &request);
	if(cnvon == 0 && onoffkey_pushed == 1){
		cnvon = 1;
	} else if(cnvon == 1 && onoffkey_pushed == 1){
		cnvon = 0;
	}
	cio_eval_it(s, rp, rrv);
	_iml_post_proc(s, rrv);
	return;
/*
    _iml_fake_nop_inst(s, rrv);
*/
}


Private void
cio_conversion_on(s, rrv)
iml_session_t *s;
iml_inst **rrv ;
{
    if(s->remainder){
	*rrv = s->remainder ;
	s->remainder = NULL ;
	return;
    }
    cio_make_conversion_on(s, rrv);
}

Private void
cio_send_results(s, rrv, inst_rp)
iml_session_t *s;
iml_inst **rrv ;
iml_inst *inst_rp ;
{
    XIMLookupStartCallbackStruct *ls;
    XIMLookupProcessCallbackStruct *lp;
    
    if(inst_rp->opcode & IMM_CB_RESULT_REQUIRED){
	inst_rp->opcode &= ~IMM_CB_RESULT_REQUIRED ;
	switch(inst_rp->opcode){
	  case IMM_LOOKUP_START:
	    ls = (XIMLookupStartCallbackStruct *)& inst_rp->operand ;
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
	    lp = (XIMLookupProcessCallbackStruct *)& inst_rp->operand ;
		if ((lp->index_of_choice_selected == XIM_UNDETERMINED) ||
                (lp->index_of_choice_selected == XIM_UNKNOWN_KEYSYM)) {
                break ;
            }
	    cio_iml_set_choice(s, rrv, lp->index_of_choice_selected,s->event);
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

Private int
cio_open(id, rrv)
int id ;
iml_inst **rrv;
{
int	ret;
    Private int cio_create_session(/* int id ,  iml_inst **rrv ,
			         iml_session_t *s */);
    extern int _iml_construct_session(/* int id , iml_inst **rrv ,
				      int (*constructor)() */);

	ret =    _iml_construct_session(id, rrv, sizeof(iml_session_t),cio_create_session);

	return ret;
}

Private void
cio_make_conversion_on(s, rrv)
iml_session_t *s ;
iml_inst **rrv ;
{
    Ci_result *rp ;
    Ci_request *requestp ;
    

    s->public_status |= IMLSTATUS_Henkan_Mode ;
    cnvon = 1 ;
    requestp = (Ci_request *)_iml_new(s, sizeof(Ci_request));
    bzero(requestp, sizeof(Ci_request));
	requestp->operation._cmd.active_regtype = CI_REGION_UNKNOWN;
	requestp->next = NULL;
	requestp->type = CI_REQ_COMMAND;
	requestp->operation._cmd.keyinfo.func = CS_CNV_SWITCH;
	requestp->operation._cmd.keyinfo.count = 0;
	requestp->operation._cmd.keyinfo.keycode = NULL;
	requestp->operation._cmd.keyinfo.arg_is_wchar = 0 ;
	requestp->operation._cmd.keyinfo.argum.char_str = NULL ;
	requestp->operation._cmd.select_info = selreg.sel_info ;
	requestp->operation._cmd.user_etcsize = 0 ;
	requestp->operation._cmd.user_etc = NULL ;
    rp = ci_put(s->language_engine, s->session_id, requestp);
    eval_ci_to_cio_packet(s, rp, rrv, 0);
    
}

Private
cio_create_session(cs_id, rrv, s)
int cs_id ;
iml_inst **rrv;
iml_session_t *s ;
{
	int		session_id ;
	char		*mode_path,*getenv();
	Ci_request	request;
	Ci_result	*rp ;
	Private int	construct_ci_initial_data();

	construct_ci_initial_data(&clntinfo);
        /* session open */
        cs_id = (getenv("CI_ID") ? atoi(getenv("CI_ID")) : 0);
        session_id = ci_open(cs_id, &clntinfo, &csinfo);
                                                /* creae resions */
	if(session_id == -1 || session_id == 0) {
		return(IMLERR_CANNOT_OPEN);
	}
	s->language_engine = cs_id ;
/*	s->session_id = session_id - 1;  CUE */
	s->session_id = session_id ;

        if(csinfo){
                csinfo_save = *csinfo ;         /* save CS information  */
                free(csinfo) ;
        }
                                            
        /* session active       */          
        request.next = NULL ;                   
        request.type = CI_REQ_CONTROL ;
        request.operation._ctl.opcode = CS_ACTIVE ;
        request.operation._ctl.operand = NULL ;
        request.operation._ctl.opdlen = 0 ;
        rp = ci_put(s->language_engine,s->session_id,&request) ;
 
        /* get CS status        */
        free(rp) ;
        request.next = NULL ;
        request.type = CI_REQ_CONTROL ;
        request.operation._ctl.opcode = CS_STATUS_GET ;
        request.operation._ctl.operand = NULL ;
        request.operation._ctl.opdlen = 0 ;
        rp = ci_put(s->language_engine,s->session_id,&request) ;

	/* 
	* constructing member functions
	*/

	s->iml_open = cio_open ;
	s->iml_close = cio_close ;
	s->iml_reset = cio_reset ;
	s->iml_send_event = cio_send_event ;
	s->iml_conversion_on = cio_conversion_on ;
	s->iml_send_results = cio_send_results ;

	cio_init_status_string(s,"./modefile");
	if(s->CurrentStatusString[s->StatusString[0].order - 1].string == NULL){
		mbstowcs(
		s->CurrentStatusString[s->StatusString[0].order - 1].string ,
		((Cs_mode *)(rp->action._ctl.result))->modestr._mb ,
		sizeof( ((Cs_mode *)(rp->action._ctl.result))->modestr._mb ) );
	}
	eval_ci_to_cio_packet(s, rp, rrv, s->event);
	return(s->session_id);
}

Private int
construct_ci_initial_data(ci_clntinfo_p)
Ci_clntinfo     *ci_clntinfo_p;
{
        struct group   *gp;
        struct passwd  *pp;
        static char     host_name[64];

        ci_clntinfo_p->env_value = (caddr_t *) NULL;
        ci_clntinfo_p->uid = getuid();
        ci_clntinfo_p->gid = getgid();
        ci_clntinfo_p->hid = gethostid();

        pp = (struct passwd  *)getpwuid(getuid());
        if (pp != (struct passwd *)NULL)
            ci_clntinfo_p->user_name = pp->pw_name;
        else
            ci_clntinfo_p->user_name = (char *)NULL;

        gp = (struct group   *)getgrgid(getgid());
        if (gp != (struct group *)NULL)
            ci_clntinfo_p->grp_name = gp->gr_name;
        else
            ci_clntinfo_p->grp_name = (char *)NULL;

        gethostname(host_name, sizeof(host_name));
        ci_clntinfo_p->host_name = host_name;

}


Private iml_inst **
cio_eval_it(s, result, rrv)
iml_session_t *s ;
Ci_result *result;
iml_inst **rrv;
{
    int		HowManyPacket = FALES;
    Ci_result	*rp ;
    iml_inst	*lp = 0, *clp = 0;
    int		size_of_operand ;
    int		i ;
    wchar_t	* wc_text ;
    int		remainder = 0 ;
    int		simple = 0 ;
    Cio_command *cmp;
    

	*rrv = 0;
	rp = result;
    while( rp ) {
	if(rp->type == CIO_RES_COMMAND){
	    cmp = &rp->action._cmd;
	    while(cmp){
		switch (cmp->type) {
		    case CS_RESULT_INTERM :
			iml_cio_interm(s, &lp, &rp,cmp);
			break;
		    case CS_RESULT_SELECT :
			iml_cio_select(s, &lp, &rp,cmp);
			break;
		    case CS_RESULT_MODE :
			iml_cio_mode(s, &lp, &rp);
			break;
		    case CS_RESULT_MISC :
			iml_cio_misc(s, &lp, &rp,cmp);
			break;
		    case CS_RESULT_CALLME :
			iml_cio_callme(s, &lp, &rp);
			break;
		    case CS_RESULT_TEXT :
/*
			if(result->data.text.encoding)
				longtomb(cp,result->data.text.text.text_wc,
							result->data.text.len);
			else
				cp = result->data.text.text.text_mb;
			catr = result->data.text.attr;
			lp = cio_make_preedit_done_inst(s);
			_iml_link_inst_tail(rrv, lp);

			_iml_make_commit_inst(s, &lp,
			strlen(cmp->data.interm.itext.text.text_mb),
				cmp->data.interm.itext.text.text_mb);
			_iml_link_inst_tail(rrv, lp);
*/
/**/
			_iml_fake_nop_inst(s, &lp);
/**/
			break;
		    default:
			fprintf(stderr,"cio_eval_it error\n");
			break;

		}
		cmp = cmp->next;
		if(rp == NULL){
			;
		}
		else if(rp->next){
			rp = rp->next;
		}
		else{
			free(rp);
			rp = NULL;
		}
		
        	if(*rrv){
            	iml_inst *plp;
			
			plp = *rrv;
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
	}
	else if(s->event){
		unsigned char	string[2];

                size_of_operand = sizeof(char) + 1 ;
                lp = (iml_inst *)_iml_new(s, size_of_operand +
                                                        sizeof(iml_inst) );
		string[0] = (unsigned char)(s->keysym);
		string[1] = NULL;
		_iml_make_commit_inst(s, &lp, size_of_operand -1 , string);
/*
		_iml_link_inst_tail(rrv, lp);
                strncpy(lp->operand,s->event->keycode, size_of_operand) ;
                lp->opcode = IMM_COMMIT ;
                lp->next = NULL ;
                lp->size_of_operand = size_of_operand;
                _imm_trace("IMM_COMIT",&lp->operand,size_of_operand);
*/
                interm_id = 0;
	}
	else if(rp->type == CIO_RES_CONTROL){
		_imm_trace("IMM_NOP(ENV_NOP)",0,0);
		lp=(iml_inst*)_iml_new(s, sizeof(iml_inst));
		lp->opcode = IMM_NOP ;
		lp->operand = NULL;
                lp->size_of_operand = 0;
		lp->next = NULL ;
		free(rp->action._ctl.result);
		if(rp == NULL){
			;
		}
		else if(rp->next){
			rp = rp->next;
		}
		else{
			free(rp);
			rp = NULL;
		}
        	if(*rrv){
            	iml_inst *plp;
			
			plp = *rrv;
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
	else{
		/* error ? */
	}


    	}/* while */

    return(&lp->next); /* iml_inst lin last point */
}

void
cio_make_status_string(s,ws,s1,simple)
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
Private
cio_init_status_string(s, path)
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
    int j, k;
    
    
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

Private
eval_ci_to_cio_packet(s, rp, rrv)
iml_session_t *s ;
Ci_result *rp;
iml_inst **rrv;
{
    cio_eval_it(s, rp, rrv);
    _iml_post_proc(s, rrv);
}

Private void
cio_close(s, rrv)
iml_session_t *s ;
iml_inst **rrv ;
{
    Private int cio_destruct_session(/* iml_session_t *s, iml_inst **rrv */);
    extern void _iml_destruct_session(/* iml_session_t *s ,
				       iml_inst **rrv ,
				       int (*destructor)() */);
    _iml_destruct_session(s, rrv, cio_destruct_session);

}

Private void
cio_destruct_session(s, rrv)
iml_session_t *s ;
iml_inst **rrv ;
{
    ci_close(s->language_engine, s->session_id);
     /* free CS information  */
        for(;csinfo_save.count_ci_onoff;csinfo_save.count_ci_onoff--)
                free(*csinfo_save.key_ci_onoff);
        free(csinfo_save.key_ci_onoff);
        for(;csinfo_save.count_ci_on;csinfo_save.count_ci_on--)
                free(*csinfo_save.key_ci_on);
        free(csinfo_save.key_ci_on);
        if(csinfo_save.key_misc_top)
                free(csinfo_save.key_misc_top);
        if(csinfo_save.key_misc_last)
                free(csinfo_save.key_misc_last);
        if(csinfo_save.key_misc_next)
                free(csinfo_save.key_misc_next);
        if(csinfo_save.key_misc_prev)
                free(csinfo_save.key_misc_prev);
}

Private void
cio_reset(s, rrv)
iml_session_t *s ;
iml_inst **rrv ;
{
    _iml_fake_nop_inst(s, rrv);
}

Private void
cio_iml_set_choice(s, rrv, index)
iml_session_t *s ;
iml_inst **rrv ;
int index ;
{
    Private Ci_result result ;
    Private Ci_result *rp = &result ;
    Private Ci_request request ;
    

    request.type = CI_REQ_COMMAND ;
    request.next = NULL ;
    request.operation._cmd.next = NULL ;
    request.operation._cmd.active_region = select_id ;
    request.operation._cmd.active_regtype = CI_REGION_SELECT ;
    request.operation._cmd.select_info.idx_focus = index ;
    rp = ci_put(s->language_engine, s->session_id, &request);
    eval_ci_to_cio_packet(s, rp, rrv);
    _iml_delete2(s);
}

void
ciAttr2IMTextAttr(p, q, len, e)
char *e ;
XIMFeedback *p ;
unsigned char *q ; 
int len ;
{
     int i,j,l;
     char r ;
    
    iml_xdmem(q,len*sizeof(wchar_t));
    
    for(i=0,j=0;j<len;i+=l,j++){
#ifdef NEXT_GENERATION_OF_JLE_ATTRIBUTE_COMPATIBLE
	p[i] = q[j] ;
#else
	l = mblen(e+i,4);
	r = q[i];
	p[j]=0;

	if(r & 0x0040)
	  p[j] |= XIMReverse ;
	else
	  p[j] |= XIMUnderline ; /* because of cm bug */

#endif
    }
    
    iml_xdmem(p,len*sizeof(XIMFeedback)) ;
}

/*
pop_region()
{
	active_regions = 
}
*/
/*
 *      check convert on/off
 */

checkonoff(key)
int     key;
{
        int     i ;

        for(i=0;i<csinfo_save.count_ci_onoff;i++){
                if(csinfo_save.key_ci_onoff[i]->code == key)
                        return(1);
        }
        return(0);
}

static void
cio_set_keyboard_state(session, rrv, key_buttons)
iml_session_t *session;
iml_inst **rrv;
IMKeyState *key_buttons;
{
    iml_generic_set_keyboard_state(session, rrv, key_buttons);
}

/*
 *      make key code data for command request
 */
Ci_keycode *
setkey(key)
int     key ;
{
        Ci_keycode      *wp ;

        wp = (Ci_keycode *)malloc(sizeof(Ci_keycode)) ;
        wp->code = key ;
        wp->attr = 0 ;
        return(wp);
}

