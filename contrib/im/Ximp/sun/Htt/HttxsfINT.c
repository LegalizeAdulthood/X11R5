/*
 *        %Z%%M% %I% %E%  
 */ 
/*
****************************************************************************

            Copyright 1991, by Sun Microsystems, Inc.
            Copyright 1991, by FUJITSU LIMITED.

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Sun Microsystems, Inc. or
FUJITSU LIMITED not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.  FUJITSU LIMITED and Sun Microsystems, Inc. make no
representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.

SUN MICROSYSTEMS, INC. AND FUJITSU LIMITED DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL FUJITSU LIMITED AND SUN
MICROSYSTEMS, INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

  Author: Hideki Hiura (hhiura@Sun.COM) Sun Microsystems, Inc.
	  Riki Kawaguchi, FUJITSU LIMITED
          Tomoki Kuriki, FUJITSU LIMITED

****************************************************************************
*/
#ifndef lint
static char     sccsid[] = "%Z%%M% %I% %E% Sun Microsystems Inc.";
#endif

#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <widec.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

#include <X11/keysymdef.h>

#include "XSunExt.h"
#include "Htt.h"
#include "XSunIMProt.h"
#include "XSunIMImpl.h"
#include "HttXsfSun.h"

static void		 Fwdwccopy() ;
static void		 Bakwccopy() ;
static wchar_t	*HttPreedit_alloc() ;

#ifdef	XSF_IMLOGIC_NOP
#define	IMLogic()	Dummy_IMLogic()
#endif				/* XSF_IMLOGIC_NOP */

#define CMSG
#ifdef DEBUG

#define Private static
#define Public			/**/

#include <stdio.h>
Private int     byte_of_line = 16;
Private void    fmtprt();

Public void
_iml_xdmem(top, len)
    char           *top;
    int             len;
{
    register char  *i, *j;
    register        print_len;

    j = top + len;
    i = top;
    while ((print_len = j - i) > 0) {
	if (print_len > byte_of_line) {
	    fmtprt(i, i, byte_of_line);
	    i += byte_of_line;
	} else {
	    fmtprt(i, i, print_len);
	    break;
	}
    }
}

Private void
fmtprt(data, offset, lsize)
    char           *data;
    long            offset;
    short           lsize;
{
    short           n;
    char            linebuf[512];
    char           *line = linebuf, *p;

    sprintf(line, "  %08lx   ", offset);
    p = line + 13;
    for (n = 0; n < byte_of_line; n++) {
	*p++ = ' ';
	if (n >= lsize) {
	    sprintf(p, "    ");
	    p += 4;
	    ++n;
	} else {
	    sprintf(p, "%02x", *((unsigned char *) data + n));
	    p += 2;
	    if ((++n) == lsize)
		sprintf(p, "  ");
	    else
		sprintf(p, "%02x", *((unsigned char *) data + n));
	    p += 2;
	}
    }
    sprintf(p, "     ");
    p += 5;
    for (n = 0; n < byte_of_line; n++) {
	if (n >= lsize)
	    *p++ = ' ';
	else if (*(data + n) < 0x20)
	    /* else if (!isprint(*((unsigned char *)data + n))) */
	    *p++ = '.';
	else
	    *p++ = *(data + n);
    }
    *p = '\0';
    dbg_printf("%s\n", line);
}
Private long
htoi(p)
    register char  *p;		/* hex mojiretsu e no pointer */
{
    register long   n = 0;

    if (upper(p) == -1)
	return (-1);
    while (*p >= '0' && *p <= '9' || *p >= 'A' && *p <= 'F')
	n = n * 16 + (*p++ >= 'A' ? *(p - 1) - 7 : *(p - 1)) - '0';
    return (n);
}

Private
upper(p)
    register char  *p;		/* hex mojiretsu e no pointer */
{
    register char  *pp;

    for (pp = p; *pp; ++pp) {
	*pp = 'a' <= *pp && 'z' >= *pp ? *pp + 'A' - 'a' : *pp;
	if (!isxdigit(*pp))
	    return (-1);
    }
}

Private char   *
itoa(i, a)
    int             i;		/* mojiretsu ni henkan suru binary (8-32 bit)  */
    register char  *a;		/* decimal mojiretsu o kakunou area no
				 * address */
{
    register int    c = 0;
    register int    b = 0;
    int             s;
    register char   d;

    i = (s = i < 0) ? (-i) : i;
    do {
	*(a + c++) = i % 10 + '0';
    } while ((i /= 10) > 0);
    *(a + c++) = s ? '-' : '\0';
    *(a + c++) = '\0';
    for (c = strlen(a) - 1; b < c; d = *(a + b), *(a + b++) = *(a + c), *(a + c--) = d);
    return (a);
}
#endif

extern void	xsf_textconv();
extern void	Display_ImlStatus_wcs() ;

static int	xsf_getmtow() ;
static void	xsf_rvdump();

static int      (*opsw[MAX_IMM_OPCODE]) ();

/*
 * This function expects to be invoked from X*LookupString() basically. The
 * function value is meaningless if it's not invoked from X*LookupString()
 * $@$5$i$K!"Cf$G8/27/91i%s%6%/%7%g%s$,F1$8$+0c$&$+$N15:01:59i%0$D$1$(JFIMLogic
 * $@$r8F$s$G$k(J
 * $@$1$I!"8/27/91i%s%6%/%7%g%s$,F1$8$+0c$&$+$O!"(JX*LookupString
 * $@$N;~$K$7$+!"0UL#$r(J $@$b$?$J$$$N$G!"(Jevent $@$K(J NULL
 * $@$rEO$;$P$h$5$=$&$M!#$H$m$&$+$J!#(J
 */
int
xsfinterpret(ic, rv, rb, rbsz, e)
    FXimp_Client   *ic;
    iml_inst       *rv;		/* return values from IMLogin() to be
				 * interpreted */
    char           *rb;
    int             rbsz;
    XKeyEvent      *e;		/* If not invoked from X*LooupString, then
				 * NULL */
{
    iml_inst       *i;
    iml_inst       *rrv = 0;
    XSFrrvCell     *cell;
    int             tr;
    int             r = XLookupNone;
    int             op;

#ifdef	DEBUG
    dbg_printf("** xsfinterpret Start **\n");
#endif				/* DEBUG */
    /* New rrv Link cell Create */
    cell = (XSFrrvCell *) malloc(sizeof(XSFrrvCell));
    bzero(cell, sizeof(XSFrrvCell));

    /* rrv Link */
    cell->rv = rv;

    /* Cell Link to IC */
    cell->nlink = ic->cell_link;
    ic->cell_link = cell;

    for (; ic->cell_link != 0;) {

	cell = ic->cell_link;

	for (i = cell->rv; i != NULL; i = i->next) {
	    cell->rv = i->next;
	    cell->called_rv = i;
#ifdef	DEBUG
	    xsf_rvdump(i);
#endif				/* DEBUG */
	    op = i->opcode & ~IMM_CB_RESULT_REQUIRED;
	    if ((op < 0) || (op >= MAX_IMM_OPCODE)) {
		break;
	    }
	    ic->callback_flg = 0;
	    tr = (*opsw[op]) (ic, i, e);
	    if (i->opcode & IMM_CB_RESULT_REQUIRED) {
		if (ic->callback_flg == 0) {
		    if ((int) IMLogic(ic->id, &rrv, IML_SEND_RESULTS, i, NULL) >= 0) {
#ifdef	DEBUG
			(void) dbg_printf("   xsfinterpret : Recall ( nest++ )\n");
#endif				/* DEBUG */
			tr = xsfinterpret(ic, rrv, e);
#ifdef	DEBUG
			(void) dbg_printf("   xsfinterpret : Recall-return ( nest-- )\n");
#endif				/* DEBUG */
			if (tr != XLookupNone)
			    r = tr;
#ifdef	DEBUG
			(void) dbg_printf("   xsfinterpret : Return By ????????\n");
#endif				/* DEBUG */
			return (r);
		    }
		} else {
		    if (tr != XLookupNone)
			r = tr;
#ifdef	DEBUG
		    (void) dbg_printf("   xsfinterpret : Cell Address : %8.8x\n", cell);
		    (void) dbg_printf("   xsfinterpret : rv Address : %8.8x\n", i);
		    (void) dbg_printf("   xsfinterpret : Return By Callback\n");
#endif				/* DEBUG */
		    return (r);
		}
	    }
	    if (tr != XLookupNone)
		r = tr;
	}

	ic->cell_link = cell->nlink;
	free(cell);

    }
#ifdef	DEBUG
    (void) dbg_printf("   xsfinterpret : Return All Cell Cleared\n");
#endif				/* DEBUG */
    return (r);
}

/*
 * static int members
 */

static int
xsf_nop()
{
    return (XLookupNone);
}
static int
xsf_set_status(ic, rv)
    FXimp_Client   *ic;
    iml_inst       *rv;
{
    int             prev_status = ic->iml_status;

    ic->iml_status = *(iml_status_t *) & (rv->operand);
    ic->henkan_mode = ic->iml_status ;

    if (((ic->iml_status & IMLSTATUS_Henkan_Mode) != IMLSTATUS_Henkan_Mode) &&
	((prev_status & IMLSTATUS_Henkan_Mode) == IMLSTATUS_Henkan_Mode)) {
	XimpCnversionEnd(ic);
    }
    return (XLookupNone);
}
static int
xsf_reset()
{
    /* NOT IMPLEMENTED */
    /* reset the ic */
    return (XLookupNone);
}
static int
xsf_commit(ic, rv, e)
    FXimp_Client   *ic;
    iml_inst       *rv;		/* return values from IMLogin() to be
				 * interpreted */
    XKeyEvent      *e;		/* If not invoked from X*LooupString, then
				 * NULL */
{
    /*
     * FIX ME! This routine can accept only multi byte.
     */
    committed_string_t *cs;
    int          ctlen;
    int		 mbslen;
    int		 wcslen;    
    wchar_t	*wbuf ;
    char        *ctbuf ;
    
    cs = (committed_string_t *) & rv->operand;
    mbslen = strlen(cs->string) ;
    wbuf = (wchar_t *)calloc(sizeof(wchar_t), mbslen + 1);
    /*
     * The following function is named as EUC_TO.. but according to the
     * source, this seems to mean EUC encoded wide char. UGAH!!
     * cs->string is mbs, so we must convert to wchar first!! UGAHHHHH!
     */
    wcslen = mbstowcs(wbuf, cs->string, mbslen);
    ctbuf = calloc(5, wcslen +1); /* 5 is maximam designation char length */
    ctlen = EUC_To_CompoundText(ctbuf, wbuf, wcslen);

#ifdef	DEBUG
    (void) dbg_printf("xsf_commit : ic=%x\n", ic);
    (void) dbg_printf("xsf_commit : mb cs->string\n");
    xsf_textdump2(cs->string);
    (void) dbg_printf("xsf_commit : wc converted string\n");
    xsf_textdump2(wbuf);
    (void) dbg_printf("xsf_commit : Convert string ( Compo )\n");
    xsf_textdump2(ctbuf);
    (void) dbg_printf("xsf_commit : ctlen=%d\n", ctlen);
#endif				/* DEBUG */

    /*
     * if this commited string is not composed but just returned,
     * KeySym must be returned.
     * Currently only controll char is needed.
     */
    if((wcslen == 1) && (wbuf[0] < 0x20)){
	int l ; /* length of Lookuped string */
	XComposeStatus NotSuppoted ;
	char XLookupBuf[sizeof(char)*2];
	KeySym keysym ;
	l = XLookupString(e,XLookupBuf,sizeof(char)*2,&keysym,&NotSuppoted);
	if((l == 1) && (cs->string[0] == XLookupBuf[0])){
	    XimpSendKeyEvent(e);
	    return(cs->status);
	}
    } 
    if (ctlen >= XSF_COMMIT_CMSG_MAX) {
	XimpCommitSendProp(ic, ctbuf, ctlen);
	ic->callback_flg = 1;
	return (cs->status);
    } else {
	XimpCommitSendCmsg(ic, ctbuf, ctlen);
	ic->callback_flg = 1;
	return (cs->status);
    }
}

static int
xsf_preedit_start(ic)
    FXimp_Client   *ic;
{
    int             (*p) () = ic->func.preedit_start;
    if (p) {
	(p) (ic);
    }
    return (XLookupNone);
}
static int
xsf_preedit_draw(ic, rv)
    FXimp_Client   *ic;
    iml_inst       *rv;
{
    int             (*p) () = ic->func.preedit_draw;
    if (p) {
	(p) (ic, rv);
    }
    return (XLookupNone);
}
static int
xsf_preedit_done(ic)
    FXimp_Client   *ic;
{
    int             (*p) () = ic->func.preedit_done;
    if (p) {
	(p) (ic);
    }
    return (XLookupNone);
}
static int
xsf_preedit_caret(ic, rv)
    FXimp_Client   *ic;
    iml_inst       *rv;
{
    int             (*p) () = ic->func.preedit_caret;
    if (p) {
	(p) (ic, rv);
    }
    return (XLookupNone);
}
static int
xsf_status_start(ic)
    FXimp_Client   *ic;
{
    int             (*p) () = ic->func.status_start;
    if (p) {
	(p) (ic);
    }
    return (XLookupNone);
}
static int
xsf_status_draw(ic, rv)
    FXimp_Client   *ic;
    iml_inst       *rv;
{
    int             (*p) () = ic->func.status_draw;
    if (p) {
	(p) (ic, rv);
    }
    return (XLookupNone);
}
static int
xsf_status_done(ic)
    FXimp_Client   *ic;
{
    int             (*p) () = ic->func.status_done;
    if (p) {
	(p) (ic);
    }
    return (XLookupNone);
}
static int
xsf_lookup_start(ic, rv, e)
    FXimp_Client   *ic;
    iml_inst       *rv;
    XKeyEvent      *e;
{
    int             (*p) () = ic->func.lookup_start;
    if (p) {
	(p) (ic, rv, e);
    }
    return (XLookupNone);
}
static int
xsf_lookup_draw(ic, rv)
    FXimp_Client   *ic;
    iml_inst       *rv;
{
    int             (*p) () = ic->func.lookup_draw;
    if (p) {
	(p) (ic, rv);
    }
    return (XLookupNone);
}
static int
xsf_lookup_process(ic, rv, e)
    FXimp_Client   *ic;
    iml_inst       *rv;
    XKeyEvent      *e;
{
    int             (*p) () = ic->func.lookup_process;
    if (p) {
	(p) (ic, rv, e);
    }
    return (XLookupNone);
}
static int
xsf_lookup_done(ic)
    FXimp_Client   *ic;
{
    int             (*p) () = ic->func.lookup_done;
    if (p) {
	(p) (ic);
    }
    return (XLookupNone);
}
static int
xsf_aux_start(ic)
    FXimp_Client   *ic;
{
    int             (*p) () = ic->func.aux_start;
    if (p) {
	(p) (ic);
    }
    return (XLookupNone);
}
static int
xsf_aux_draw(ic)
    FXimp_Client   *ic;
{
    int             (*p) () = ic->func.aux_draw;
    if (p) {
	(p) (ic);
    }
    return (XLookupNone);
}

static int
xsf_aux_done(ic)
    FXimp_Client   *ic;
{
    int             (*p) () = ic->func.aux_done;
    if (p) {
	(p) (ic);
    }
    return (XLookupNone);
}

/*
 * Call callback Functions...
 *	xsf_preedit_start_callback,	xsf_preedit_draw_callback,
 *	xsf_preedit_done_callback,	xsf_preedit_caret_callback,
 *	xsf_status_start_callback,	xsf_status_draw_callback,
 *	xsf_status_done_callback,	xsf_lookup_start_callback,
 *	xsf_lookup_draw_callback,	xsf_lookup_process_callback,
 *	xsf_lookup_done_callback
 */
static int
xsf_preedit_start_callback(ic)
    FXimp_Client   *ic;
{
#ifdef	DEBUG
    (void) dbg_printf("xsf_preedit_start_callback : ic=%x\n", ic);
#endif				/* DEBUG */
    XimpPreEditStartSend(ic);
    ic->callback_flg = 1;
    return (XLookupNone);
}

/* Pre-Edit Draw Callback Module */
static int
xsf_preedit_draw_callback(ic, rv)
    FXimp_Client   *ic;
    iml_inst       *rv;		/* return values from IMLogin() to be
				 * interpreted */
{
    XIMPreeditDrawCallbackStruct *pds =
    (XIMPreeditDrawCallbackStruct *) & rv->operand;
    int             pdslen;
    char            textarea[512];

#ifdef	DEBUG
    (void) dbg_printf("** PreEditDraw-Callback **\n");
    (void) dbg_printf("	ic = %x\n", ic);
    (void) dbg_printf("	pds = %x\n", pds);
    (void) xsf_XIMtextdump(pds->text);
#endif				/* DEBUG */

    bzero(textarea, sizeof(textarea));
    xsf_textconv(pds->text, textarea, &pdslen);

#ifdef	DEBUG
    (void) xsf_textdump2(textarea);
    dbg_printf("  Convert length : %d\n", pdslen);
#endif				/* DEBUG */

#ifndef CMSG
    XimpPreEditDrawSendProp(ic, pds, textarea, pdslen);
#else
    /*
     * Because of Ximp design bug, we need to use prop always
     * when text deletion occurs, so put (pds->text == NULL).
     */
    if ((pdslen >= XSF_COMMIT_CMSG_MAX)/* || (pds->text == NULL)*/){
	XimpPreEditDrawSendProp(ic, pds, textarea, pdslen);
    } else {
	XimpPreEditDrawSendCmsg(ic, pds, textarea, pdslen);
    }
#endif
    ic->callback_flg = 1;
    return (XLookupNone);
}
static int
xsf_preedit_done_callback(ic)
    FXimp_Client   *ic;
{
#ifdef	DEBUG
    (void) dbg_printf("xsf_preedit_done_callback : ic=%x\n", ic);
#endif				/* DEBUG */
    XimpPreEditDoneSend(ic);
    ic->callback_flg = 1;
    return (XLookupNone);
}
static int
xsf_preedit_caret_callback(ic, rv)
    FXimp_Client   *ic;
    iml_inst       *rv;
{
    XIMPreeditCaretCallbackStruct *pcs;

    pcs = (XIMPreeditCaretCallbackStruct *) & rv->operand;
#ifdef	DEBUG
    (void) dbg_printf("xsf_preedit_caret_callback : ic=%x\n", ic);
    (void) dbg_printf("xsf_preedit_caret_callback : pcs=%x\n", pcs);
#endif				/* DEBUG */
    XimpPreEditCaretSend(ic, pcs);
    ic->callback_flg = 1;
    return (XLookupNone);
}
static int
xsf_status_start_callback(ic)
    FXimp_Client   *ic;
{
#ifdef	DEBUG
    (void) dbg_printf("xsf_status_start_callback : ic=%x\n", ic);
#endif				/* DEBUG */
    XimpStatusStartSend(ic);
    ic->callback_flg = 1;
    return (XLookupNone);
}
static int
xsf_status_draw_callback(ic, rv)
    FXimp_Client   *ic;
    iml_inst       *rv;		/* return values from IMLogin() to be
				 * interpreted */
{
    XIMStatusDrawCallbackStruct *sds;
    int             sdslen;
    char            textarea[512];

    sds = (XIMStatusDrawCallbackStruct *) & rv->operand;

#ifdef	DEBUG
    dbg_printf("xsf_status_draw_callback : ic=%x\n", ic);
    dbg_printf("xsf_status_draw_callback : sds=%x\n", sds);
    xsf_XIMtextdump(sds->data.text);
#endif				/* DEBUG */

    bzero(textarea, sizeof(textarea));
    xsf_textconv(sds->data.text, textarea, &sdslen);

#ifdef	DEBUG
    xsf_textdump2(textarea);
    dbg_printf("  Convert length : %d\n", sdslen);
#endif				/* DEBUG */

#ifndef CMSG
    XimpStatusDrawSendProp(ic, sds, textarea, sdslen);
#else
    if (sdslen >= XSF_COMMIT_CMSG_MAX) {
	XimpStatusDrawSendProp(ic, sds, textarea, sdslen);
    } else {
	XimpStatusDrawSendCmsg(ic, sds, textarea, sdslen);
    }
#endif
    ic->callback_flg = 1;
    return (XLookupNone);
}
static int
xsf_status_done_callback(ic)
    FXimp_Client   *ic;
{
#ifdef	DEBUG
    (void) dbg_printf("xsf_status_done_callback : ic=%x\n", ic);
#endif				/* DEBUG */
    XimpStatusDoneSend(ic);
    ic->callback_flg = 1;
    return (XLookupNone);
}

static int
xsf_lookup_start_callback(ic, rv, e)
    FXimp_Client   *ic;
    iml_inst       *rv;		/* return values from IMLogin() to be
				 * interpreted */
    XKeyEvent      *e;
{
    XIMLookupStartCallbackStruct *lss;

    lss = (XIMLookupStartCallbackStruct *) & rv->operand;

#ifdef	DEBUG
    (void) dbg_printf("xsf_lookup_start_callback : ic=%x\n", ic);
    (void) dbg_printf("xsf_lookup_start_callback : lss=%x\n", lss);
    (void) dbg_printf("xsf_lookup_start_callback : e=%x\n", e);
#endif				/* DEBUG */

    XimpLookupStartSend(ic, lss, e);
    ic->callback_flg = 1;
    return (XLookupNone);
}
static int
xsf_lookup_draw_callback(ic, rv)
    FXimp_Client   *ic;
    iml_inst       *rv;		/* return values from IMLogin() to be
				 * interpreted */
{
    XIMLookupDrawCallbackStruct *lds;

    lds = (XIMLookupDrawCallbackStruct *) & rv->operand;
#ifdef	DEBUG
    dbg_printf("xsf_lookup_draw_callback : ic=%x\n", ic);
    dbg_printf("xsf_lookup_draw_callback : lds=%x\n", lds);
#endif				/* DEBUG */
    XimpLookupDrawSend(ic, lds, 1);
    ic->callback_flg = 1;
    return (XLookupNone);

}

static int
xsf_lookup_process_callback(ic, rv, e)
    FXimp_Client   *ic;
    iml_inst       *rv;		/* return values from IMLogin() to be
				 * interpreted */
    XKeyEvent      *e;
{
#ifdef	DEBUG
    (void) dbg_printf("xsf_lookup_process_callback : ic=%x\n", ic);
    (void) dbg_printf("xsf_lookup_process_callback : e=%x\n", e);
#endif				/* DEBUG */
    XimpLookupProcessSend(ic, e);
    ic->callback_flg = 1;
    return (XLookupNone);
}
static int
xsf_lookup_done_callback(ic)
    FXimp_Client   *ic;
{

#ifdef	DEBUG
    (void) dbg_printf("xsf_lookup_done_callback : ic=%x\n", ic);
#endif				/* DEBUG */
    XimpLookupDoneSend(ic);
    ic->callback_flg = 1;
    return (XLookupNone);
}

static int
xsf_key_info(ic, rv)
    FXimp_Client   *ic;
    iml_inst       *rv;		/* return values from IMLogin() to be
				 * interpreted */
{
#ifdef	DEBUG
    (void) dbg_printf("xsf_key_info CALL\n");
#endif				/* DEBUG */
    return (XLookupNone);
}
static int
xsf_put_queue(ic, rv, event)
    FXimp_Client   *ic;
    iml_inst       *rv;		/* return values from IMLogin() to be
				 * interpreted */
    XKeyEvent      *event;	/* If not invoked from X*LooupString, then
				 * NULL */
{
    /* only set ic->conv_on to ON in XmbLookupString */
    /*
     * PARANOIA: check event.
     */
#ifdef	DEBUG
    (void) dbg_printf("xsf_put_queue CALL\n");
#endif				/* DEBUG */
    if (event) {
	XPutBackEvent(display, event);
    }
    return (XLookupNone);
}

/*
 * Drawing Functions...
 *	xsf_preedit_draw_server,		xsf_preedit_caret_server,
 *	xsf_lookup_draw_server,			xsf_lookup_process_server,
 *	xsf_status_draw_server
 */
static int
xsf_preedit_draw_server( ic, rv )
    FXimp_Client	*ic ;
    iml_inst		*rv ;	/* Return values from IMLogic() to be
    						 *  interpreted */
{
    XIMPreeditDrawCallbackStruct	*pds ;
    wchar_t				*preedit_string ;
    int					 preedit_length ;

    pds = (XIMPreeditDrawCallbackStruct *)&rv->operand ;

#ifdef	DEBUG
    (void) dbg_printf("** PreEditDraw-Server **\n");
    (void) dbg_printf("	ic = %x\n", ic);
    (void) dbg_printf("	pds = %x\n", pds);
    (void) dbg_printf("	pds->caret = %d\n", pds->caret);
    (void) dbg_printf("	pds->chg_first = %d\n", pds->chg_first);
    (void) dbg_printf("	pds->chg_length = %d\n", pds->chg_length);
    (void) xsf_XIMtextdump(pds->text);
#endif				/* DEBUG */

    if ( ( !pds->caret )
      && ( !pds->chg_first )
      && ( !pds->chg_length )
      && ( !pds->text ) ) {
    	ClearTextBuff( ic ) ;
    	ic->last_colums = ic->current_colums = 0 ;
    	DisplayOutputBuff() ;
    	DisplayCursor() ;
    	return ( XLookupNone ) ;
    }

    if ( !pds->text ) {
    	ic->last_colums = HttPreedit_delete( ic->output_buff,
    			ic->last_colums, pds->chg_first, pds->chg_length ) ;
    	DisplayOutputBuff() ;
    	DisplayCursor() ;
    	return ( XLookupNone ) ;
    }

    if ( ( !pds->text->length )
      && ( !pds->text->feedback )
      && ( !pds->text->string.wide_char ) ) {
    	ClearTextBuff( ic ) ;
    	ic->last_colums = ic->current_colums = 0 ;
    	DisplayOutputBuff() ;
    	DisplayCursor() ;
    	return ( XLookupNone ) ;
    }

    /* Replace Preedit_attrib */
    if ( ic->preedit_attr )
    	free( ic->preedit_attr ) ;

    ic->preedit_attr = (XIMFeedback *)malloc(
    			sizeof( XIMFeedback ) * pds->text->length ) ;
    bcopy( pds->text->feedback, ic->preedit_attr,
    			sizeof( XIMFeedback ) * pds->text->length ) ;

    if ( !pds->text->string.wide_char )
    	return ( XLookupNone ) ;

    preedit_length = xsf_mbstowcs( pds->text, &preedit_string ) ;
    ic->current_colums = pds->caret + 1 ;
    ic->output_buff = HttPreedit_alloc( ic->output_buff, ic->last_colums,
    			pds->chg_length, preedit_length ) ;
    ic->last_colums = HttPreedit_delete( ic->output_buff, ic->last_colums,
    			pds->chg_first, pds->chg_length ) ;
    ic->last_colums = HttPreedit_insert( ic->output_buff, ic->last_colums,
    			preedit_string, pds->chg_first, preedit_length ) ;
    free( preedit_string ) ;
#ifdef	DEBUG
    (void) dbg_printf("	current_colums = %d\n", ic->current_colums);
    (void) dbg_printf("	last_colums = %d\n", ic->last_colums);
	_iml_xdmem( ic->output_buff, ic->last_colums * 2 ) ;
#endif	/* DEBUG */
    DisplayOutputBuff() ;
    DisplayCursor() ;
    return ( XLookupNone ) ;
}

static int
xsf_preedit_caret_server( ic, rv )
	FXimp_Client	*ic ;
	iml_inst		*rv ;	/* Return values from IMLogic() to be
							 *  interpreted */
{
	/*
	 * Not Yet Fixed
	 */
	return ( XLookupNone ) ;
}

static int
xsf_lookup_draw_server( ic, rv )
	FXimp_Client	*ic ;
	iml_inst		*rv ;	/* Return values from IMLogic() to be
							 *  interpreted */
{
	/*
	 * Not Yet Fixed
	 */
	return ( XLookupNone ) ;
}

static int
xsf_lookup_process_server( ic, rv )
	FXimp_Client	*ic ;
	iml_inst		*rv ;	/* Return values from IMLogic() to be
							 *  interpreted */
{
	/*
	 * Not Yet Fixed
	 */
	return ( XLookupNone ) ;
}

static int
xsf_status_draw_server( ic, rv )
	FXimp_Client	*ic ;
	iml_inst		*rv ;	/* Return values from IMLogic() to be
							 *  interpreted */
{
	XIMStatusDrawCallbackStruct		*sds ;

#ifdef	DEBUG
(void)dbg_printf( "** Status-Draw-Server **\n" ) ;
#endif	/* DEBUG */

	sds = (XIMStatusDrawCallbackStruct *)&rv->operand ;

	if ( ic->status_message ) {
		free( ic->status_message ) ;
		ic->status_message = NULL ;
	}

#ifdef	DEBUG
xsf_XIMtextdump( sds->data.text ) ;
#endif	/* DEBUG */

	ic->status_length = xsf_mbstowcs( sds->data.text, &ic->status_message ) ;
	DisplayStatus_wcs( ic ) ;
	return ( XLookupNone ) ;
}

void
xsfmgr_init()
{

    if (opsw[IMM_NOP])
	return;

    opsw[IMM_NOP] = xsf_nop;
    opsw[IMM_SET_STATUS] = xsf_set_status;
    opsw[IMM_RESET] = xsf_reset;
    opsw[IMM_COMMIT] = xsf_commit;
    opsw[IMM_KEY_INFO] = xsf_key_info;
    opsw[IMM_PUT_QUEUE] = xsf_put_queue;

    opsw[IMM_PREEDIT_START] = xsf_preedit_start;
    opsw[IMM_PREEDIT_DRAW] = xsf_preedit_draw;
    opsw[IMM_PREEDIT_DONE] = xsf_preedit_done;
    opsw[IMM_PREEDIT_CARET] = xsf_preedit_caret;

    opsw[IMM_STATUS_START] = xsf_status_start;
    opsw[IMM_STATUS_DRAW] = xsf_status_draw;
    opsw[IMM_STATUS_DONE] = xsf_status_done;

    opsw[IMM_LOOKUP_START] = xsf_lookup_start;
    opsw[IMM_LOOKUP_DRAW] = xsf_lookup_draw;
    opsw[IMM_LOOKUP_PROCESS] = xsf_lookup_process;
    opsw[IMM_LOOKUP_DONE] = xsf_lookup_done;

    opsw[IMM_AUX_START] = xsf_aux_start;
    opsw[IMM_AUX_DRAW] = xsf_aux_draw;
    opsw[IMM_AUX_DONE] = xsf_aux_done;
    return;

}

/* Client Function Pointer Records Initialize */
void
xsfmgr_client_init(ic)
    FXimp_Client   *ic;
{

    if ( ic->style & XIMPreeditNothing ) {
	ic->func.preedit_draw = xsf_preedit_draw_server ;
	ic->func.preedit_caret = xsf_preedit_caret_server ;
    }
    if ( ic->style & XIMPreeditPosition ) {
	ic->func.preedit_draw = xsf_preedit_draw_server ;
	ic->func.preedit_caret = xsf_preedit_caret_server ;
    }
    if ( ic->style & XIMPreeditCallbacks ) {
	ic->func.preedit_start = xsf_preedit_start_callback ;
	ic->func.preedit_draw = xsf_preedit_draw_callback ;
	ic->func.preedit_caret = xsf_preedit_caret_callback ;
	ic->func.preedit_done = xsf_preedit_done_callback ;
    }
    if ( ic->style & XIMStatusNothing ) {
	ic->func.status_draw = xsf_status_draw_server ;
    }
    if ( ic->style & XIMStatusArea ) {
	ic->func.status_draw = xsf_status_draw_server ;
    }
    if ( ic->style & XIMStatusCallbacks ) {
	ic->func.status_start = xsf_status_start_callback ;
	ic->func.status_draw = xsf_status_draw_callback ;
	ic->func.status_done = xsf_status_done_callback ;
    }

    /* Defaults Function Pointer ( Extent Records ) Setting */
    ic->func.lookup_draw = xsf_lookup_draw_server ;
    ic->func.lookup_process = xsf_lookup_process_server ;

    if ( ic->style & XIMLookupCallbacks ) {
	ic->func.lookup_start = xsf_lookup_start_callback ;
	ic->func.lookup_draw = xsf_lookup_draw_callback ;
	ic->func.lookup_process = xsf_lookup_process_callback ;
	ic->func.lookup_done = xsf_lookup_done_callback ;
    }
    return ;

}

/* Client Function Pointer ( Extent ) Records Initialize */
void
xsfmgr_client_ext_init(ic)
    FXimp_Client   *ic;
{

#ifdef	DEBUG
    dbg_printf("Xsfmgr extension Init\n");
#endif				/* DEBUG */
    ic->func.lookup_start = xsf_lookup_start_callback;
    ic->func.lookup_draw = xsf_lookup_draw_callback;
    ic->func.lookup_process = xsf_lookup_process_callback;
    ic->func.lookup_done = xsf_lookup_done_callback;
    return;

}

extern void
xsf_euc_textconv(ximtext, textarea, len)
    XIMText        *ximtext;
    char           *textarea;
    int            *len;	/* return */
{

    char           *mbtext;
    int             mbbl;

    if (!ximtext) {
	*len = 0;
	return;
    }
    if ((!ximtext->length)
	|| (!ximtext->string.multi_byte)) {
	*len = 0;
	return;
    }
    if (ximtext->encoding_is_wchar) {
	mbbl = (ximtext->length + 1) * MB_LEN_MAX;
	mbtext = (char *) calloc(1, mbbl);
	wcstombs(mbtext, ximtext->string.wide_char, mbbl);
	mbtext[mbbl] = NULL;
	*len = ximtext->length;
    } else {
	wchar_t        *wctext;

	mbbl = (ximtext->length + 1) * MB_LEN_MAX;
	mbtext = (char *) wctext = (wchar_t *) calloc(sizeof(wchar_t), mbbl);
	mbstowcs(wctext, ximtext->string.multi_byte, mbbl);
	*len = wslen(wctext);
	strncpy(ximtext->string.wide_char, mbtext, mbbl);
    }

    /*
     * The following function is named as EUC_TO.. but according to the
     * source, this seems to mean EUC encoded wide char. UGAH!!
     */
    *len = EUC_To_CompoundText(textarea, mbtext, *len);

    free(mbtext);
}

extern void
xsf_textconv(ximtext, textarea, len)
    XIMText        *ximtext;
    char           *textarea;
    int            *len;	/* return */
{
    if (!ximtext) {
	*len = 0;
	return;
    }
    if ((!ximtext->length)
	|| (!ximtext->string.multi_byte)) {
	*len = 0;
	return;
    }
    if (ximtext->encoding_is_wchar) {
	/*
	 * The following function is named as EUC_TO.. but according to the
	 * source, this seems to mean EUC encoded wide char. UGAH!!
	 */
	*len = EUC_To_CompoundText(textarea,
				   ximtext->string.wide_char,
				   ximtext->length);
#ifdef DEBUG
	dbg_printf("textconv: input wchar text |%ws|\n", ximtext->string.wide_char);
	_iml_xdmem(ximtext->string.wide_char,
		   ximtext->length * sizeof(wchar_t));
	dbg_printf("textconv: out put CT\n");
	_iml_xdmem(textarea, len + 2);
#endif
    } else {
	int             mbbl = (ximtext->length + 1) * MB_LEN_MAX;
	wchar_t        *wctext = (wchar_t *) calloc(sizeof(wchar_t), mbbl);

	mbstowcs(wctext, ximtext->string.multi_byte, mbbl);
	wctext[mbbl] = NULL;
	*len = wslen(wctext);
	/*
	 * The following function is named as EUC_TO.. but according to the
	 * source, this seems to mean EUC encoded wide char. UGAH!!
	 */
	*len = EUC_To_CompoundText(textarea, wctext, *len);
#ifdef DEBUG
	dbg_printf("textconv: input MB text |%s|\n", ximtext->string.multi_byte);
	_iml_xdmem(ximtext->string.multi_byte,
		   ximtext->length * MB_LEN_MAX);
	dbg_printf("textconv: converted WC text |%ws|\n", wctext);
	_iml_xdmem(wctext, strlen(wctext));
	dbg_printf("textconv: out put CT\n");
	_iml_xdmem(textarea, len + 2);
#endif
	free(wctext);

    }

}

/*
 * Convert mbs to wcs when XIMText is mbs.
 */
static int
xsf_mbstowcs( ximtext, textarea )
    XIMText        *ximtext;
    wchar_t        **textarea;
{
    *textarea = NULL ;

    if (!ximtext)
	return ( 0 ) ;

    if ((!ximtext->length)
	|| (!ximtext->string.multi_byte))
	return ( 0 ) ;

	if (ximtext->encoding_is_wchar) {
		int				wcbl = ( ximtext->length + 1 ) * sizeof(wchar_t) ;
		*textarea = (wchar_t *)malloc( wcbl ) ;
		(void)bzero( *textarea, wcbl ) ;
		(void)bcopy( (char *)ximtext->string.wide_char,
				(char *)*textarea, ximtext->length * sizeof(wchar_t) ) ;
		return ( ximtext->length ) ;
	} else {
		int             mbbl = (ximtext->length + 1) * MB_LEN_MAX;
		*textarea = (wchar_t *) calloc(sizeof(wchar_t), mbbl);

		mbstowcs(*textarea, ximtext->string.multi_byte, mbbl);
		textarea[mbbl] = NULL;
		return ( wslen( *textarea ) ) ;
	}
}

/*
 * Preedit strings delete
 *		return : new text character count
 */
int
HttPreedit_delete( text_area, org_len, first, length )
	wchar_t		*text_area ;
	int			 org_len ;
	int			 first ;
	int			 length ;
{
	wchar_t		*copy_orig_ptr ;
	wchar_t		*copy_dest_ptr ;

	if ( !text_area )
			return ( 0 ) ;

	if ( !length )
			return ( org_len ) ;
	copy_dest_ptr = text_area + first ;
	copy_orig_ptr = copy_dest_ptr + length ;
	Fwdwccopy( copy_orig_ptr, copy_dest_ptr, length ) ;
	return ( org_len - length ) ;
}

/*
 * Preedit strings insert
 *		return : new text character count
 */
int
HttPreedit_insert( text_area, org_len, ins_text, ins_pos, length )
    wchar_t		*text_area ;
    int			 org_len ;
    wchar_t		*ins_text ;
    int			 ins_pos ;
    int			 length ;
{
    wchar_t		*copy_orig_ptr = ins_text ;
    wchar_t		*copy_dest_ptr = text_area ;
    int			 ins_len ;

    if ( !text_area )
    		return ( 0 ) ;

    if ( !length )
    		return ( org_len ) ;
    if ( org_len ) {
	copy_orig_ptr = text_area + ins_pos ;
	copy_dest_ptr = copy_orig_ptr + length ;
	Bakwccopy( copy_orig_ptr, copy_dest_ptr, length ) ;
	copy_orig_ptr = ins_text ;
	copy_dest_ptr = text_area + ins_pos ;
    }
    Fwdwccopy( copy_orig_ptr, copy_dest_ptr, length ) ;
    return ( org_len + length ) ;
}

/*
 * Preedit text area allocate
 */
static wchar_t *
HttPreedit_alloc( text_area, org_len, del_len, ins_len )
	wchar_t		*text_area ;
	int			 org_len ;
	int			 del_len ;
	int			 ins_len ;
{
	if ( del_len >= ins_len ) {
		org_len += ins_len - del_len ;
		return ( text_area ) ;
	} else {
		wchar_t		*new_text_area ;
		int			 new_length = org_len + ins_len - del_len ;
		new_text_area =
				(wchar_t *)malloc( new_length * ( sizeof(wchar_t) + 1 ) ) ;
		Fwdwccopy( text_area, new_text_area, org_len ) ;
		if ( text_area )
			free( text_area ) ;
		return ( new_text_area ) ;
	}
}

/*
 * Preedit Area copy (1)
 */
static void
Fwdwccopy( src, dest, length )
	wchar_t	   *src ;
	wchar_t	   *dest ;
	int			length ;
{
	for ( ; length > 0; length--, src++, dest++ )
			*dest = *src ;
	return ;
}

/*
 * Preedit Area copy (2)
 */
static void
Bakwccopy( src, dest, length )
	wchar_t	   *src ;
	wchar_t	   *dest ;
	int			length ;
{
	src  += ( length - 1 ) ;
	dest += ( length - 1 ) ;
	for ( ; length > 0; length--, src--, dest-- )
			*dest = *src ;
	return ;
}

#ifdef	DEBUG
static int      rvtbl[] = {
     /* IMM_NOP */ 0,
     /* IMM_RESET */ 0,
     /* IMM_COMMIT */ 14,
     /* IMM_SET_STATUS */ sizeof(iml_status_t),
     /* IMM_PREEDIT_START */ 0,
     /* IMM_PREEDIT_DRAW */ 36,
     /* IMM_PREEDIT_CARET */ 0,
     /* IMM_PREEDIT_DONE */ 0,
     /* IMM_LOOKUP_START */ 0,
     /* IMM_LOOKUP_DRAW */ sizeof(XIMLookupDrawCallbackStruct),
     /* IMM_LOOKUP_PROCES */ sizeof(XIMLookupProcessCallbackStruct),
     /* IMM_LOOKUP_DONE */ 0,
     /* IMM_KEY_INFO */ 0,
     /* IMM_PUT_QUEUE */ 0,
     /* IMM_STATUS_START */ 0,
     /* IMM_STATUS_DRAW */ sizeof(XIMStatusDrawCallbackStruct),
     /* IMM_STATUS_DONE */ 0,
     /* IMM_AUX_START */ 0,
     /* IMM_AUX_DRAW */ 0,
     /* IMM_AUX_PROCESS */ 0,
     /* IMM_AUX_DONE */ 0
};

static void
xsf_rvdump(rv)
    iml_inst       *rv;
{

    int             i, op;
    unsigned char  *ptr;

    dbg_printf("===iml_inst Dump===\n");
    dbg_printf("	rv-addr   : %8.8x\n", rv);
    dbg_printf("	next-link : %8.8x\n", rv->next);
    dbg_printf("	op-size   : %d\n", rv->size_of_operand);
    op = rv->opcode & ~IMM_CB_RESULT_REQUIRED;
    dbg_printf("	Result    : %8.8x\n", rv->opcode & IMM_CB_RESULT_REQUIRED);
    dbg_printf("	opcode    : %d\n", op);
    dbg_printf("	oprand    : %8.8x\n", rv->operand);

    for (i = rvtbl[rv->opcode], ptr = (unsigned char *)&rv->operand;
			i > 0; i--, ptr++) {
	dbg_printf("%2.2x ", *ptr);
    }
    dbg_printf("\n", *ptr);

}

xsf_XIMtextdump(text)
    XIMText        *text;
{

    int             i;
    unsigned char  *ptr;

    dbg_printf("===XIMtext Dump===\n");
    dbg_printf("	XIMtext Addr	: %8.8x\n", text);
    if (!text)
	return;
    dbg_printf("	Length of Str.	: %d\n", text->length);
    dbg_printf("	Encoding      	: %d\n", text->encoding_is_wchar);
    if (text->feedback) {
	dbg_printf("	FeedBacks Addr	: %8.8x\n", text->feedback);
	for (i = text->length * 4, ptr = (unsigned char *)text->feedback;
	     i > 0; i--, ptr++) {
	    dbg_printf(" %2.2x", *ptr);
	}
    }
    if (text->string.multi_byte) {
	dbg_printf("\n	multi_byte Addr	: %8.8x\n", text->string.multi_byte);
	for (i = text->length * 2,
			ptr = (unsigned char *)text->string.multi_byte;
			i > 0; i--, ptr++) {
	    dbg_printf(" %2.2x", *ptr);
	}
    }
    dbg_printf("\n");
}


xsf_textdump2(text)
    char           *text;
{

    int             i;
    unsigned char  *ptr;

    dbg_printf("=== Convert Text ===\n");
    for (i = strlen(text), ptr = (unsigned char *)text; i > 0; i--, ptr++) {
	dbg_printf(" %2.2x", *ptr);
    }
    dbg_printf("\n");
}

#endif				/* DEBUG */
