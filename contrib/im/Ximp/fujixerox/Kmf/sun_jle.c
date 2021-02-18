/*

Copyright 1991 by Fuji Xerox Co., Ltd., Tokyo, Japan.

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of M.I.T. not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission.  Fuji Xerox Co., Ltd. makes no representations
about the suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.

FUJI XEROX CO.,LTD. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJI XEROX CO.,LTD. BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

*/

#include "XIMServer.h"
#include "sun_jle.h"
#include <X11/keysym.h>

static Bool Create(), Destroy(), Begin(), End(), Reset(), Keyin();

PrivateMethodsRec private_method = {
    Create,
    Destroy,
    Begin,
    End,
    Reset,
    Keyin
};

static Ximp_Key key = {
	{ControlMask, ControlMask, XK_space}
};

Ximp_KeyList keys = {
	1,
	&key,
};

Ximp_KeyList *IMStartKeyList = &keys;

AllocInputMethodPrivate(ic)
InputContext ic;
{
    JLEPrivate private;

    ic->private = (MethodPrivate)XAllocFreeArea(sizeof(MethodPrivateRec));
    ic->private->methods = &private_method;
    private =
    ic->private->private = (JLEPrivate)XAllocFreeArea(sizeof(JLEPrivateRec));
}

static char buf[256];

SelectRegionDraw(ic, private)
InputContext ic;
JLEPrivate private;
{
    XIMText *text = XAlloc(XIMText);
    static char localbuf[1024];
    int i, j;

    sprintf(buf, "%d. %s", 1, private->select->candidates[private->select->disp_first]);
    for (i = 1; i < private->select->disp_num; i++) {
	j = (i + 1) % 10;
	sprintf(localbuf, "  %d. %s", j, private->select->candidates[private->select->disp_first + i]);
	strcat(buf, localbuf);
    }
    text->length = strlen(buf);
    text->feedback = (XIMFeedback *)NULL;
    text->encoding_is_wchar = False;
    text->string.multi_byte = Xmalloc(text->length + 1);
    strcpy(text->string.multi_byte, buf);
    (*ic->status->methods->draw)(ic, text);
}

ProcCMtoENV(ic, private, env, callReset)
InputContext ic;
JLEPrivate private;
struct cm_to_env *env;
Bool callReset;
{
    Atom atom;
    XIMText commit;
    static char *sel_commit_chars = "1234567890";

    for (; env; env = env->ce_next) {
	switch (env->ce_operation) {
	case ENV_COMMIT:
	    if (env->ce_string[env->ce_size - 1] == '\r') {
		env->ce_string[env->ce_size - 1] == '\n';
	    }
	    commit.encoding_is_wchar = False;
	    commit.length = strlen(env->ce_string);
	    commit.string.multi_byte = env->ce_string;
	    if (callReset) {
		(*ic->methods->reset_reply)(ic, &commit);
	    } else {
		(*ic->methods->send)(ic, &commit);
	    }
	    break;
	case ENV_SET_CURRENT_REGION:
	    private->region = env->ce_region;
	    if (private->region == 1) {
		XIMText *text = (XIMText *)Xmalloc(sizeof(XIMText));

		sprintf(buf, "%s%s%s",
			private->ce_name[private->ce_name_active].label,
			private->conv[private->conv_active].label,
			private->cm_state[private->cm_state_active].label);
		text->length = strlen(buf);
		text->feedback = (XIMFeedback *)NULL;
		text->encoding_is_wchar = False;
		text->string.multi_byte = Xmalloc(text->length + 1);
		strcpy(text->string.multi_byte, buf);
		(*ic->status->methods->draw)(ic, text);
	    }
	    break;
	case ENV_NOP:
	    break;
	case ENV_CM_OFF:
	    private->cmon = False;
	    (*ic->methods->proc_end)(ic);
	    break;
	case ENV_SETKEY_CM_ON:
	    private->cmon_key = env->ce_rtn_value;
	    break;
	case ENV_INTERM:
/*
 * ENV_INTERM((int)ce_cursor, (int)ce_cursor_type, (e_char *)ce_text,
 * (u_char *)ce_text_attr, (int)ce_v_pos, (int)ce_v_type) specifies 
 * the entire intermediate region.
 */
	    {
		XIMText *text = XAlloc(XIMText);
		int chars, i, len;

		text->encoding_is_wchar = False;
		len = strlen(env->ce_text);
		text->string.multi_byte = Xmalloc(len + 1);
		strcpy(text->string.multi_byte, env->ce_text);
		text->feedback = (XIMFeedback *)Xmalloc((len + 1) * sizeof(XIMFeedback));
		for (chars = i = 0; i < len; chars++, i += mblen(&env->ce_text[i], 4)) {
		    text->feedback[chars] = env->ce_text_attr[i];
		}
		text->length = chars;
		(*ic->preedit->methods->draw)(ic, text);
	    }
	    break;
	case ENV_SELECT:
	    private->select = (Select)Xmalloc(sizeof(SelectRec));
	    private->select->ncandidate = env->ce_ncandidate;
	    private->select->candidates = env->ce_candidate;
	    private->select->disp_first = 0;
	    private->select->disp_num = private->select->ncandidate - private->select->disp_first;
	    if (private->select->disp_num > 10) private->select->disp_num = 10;
	    SelectRegionDraw(ic, private);
	    break;
	case ENV_SELECT_END:
	    Xfree(private->select);
	    private->select = NULL;
	    break;
	case ENV_SELECT_NEXT:
	    if ((private->select->disp_first += private->select->disp_num) == private->select->ncandidate) private->select->disp_first = 0;
	    private->select->disp_num = private->select->ncandidate - private->select->disp_first;
	    if (private->select->disp_num > 10) private->select->disp_num = 10;
	    SelectRegionDraw(ic, private);
	    break;
	case ENV_SELECT_PREV:
	    if (private->select->disp_first == 0) {
		private->select->disp_num = private->select->ncandidate % 10;
		private->select->disp_first = private->select->ncandidate - private->select->disp_num;
	    } else {
		private->select->disp_num = 10;
		private->select->disp_first -= 10;
	    }
	    SelectRegionDraw(ic, private);
	    break;
	case ENV_SELECT_COMMIT:
	    {
		char *s;
		static struct env_to_cm selop;

		selop.ec_next = (struct env_to_cm *)NULL;
		if ((s = (char *)index(sel_commit_chars, *env->ce_sel_string)) &&
		    ((selop.ec_key = private->select->disp_first + ((int)(s - sel_commit_chars))) < private->select->ncandidate)) {
		    selop.ec_operation = CM_SELECT;
		} else {
		    selop.ec_operation = CM_SELECT_INVALID;
		    selop.ec_select_invalid = INVALID;
		}
		ProcCMtoENV(ic, private, cm_put(private->cm_id, private->cm_session, &selop), callReset);
	    }
	    break;
	case ENV_SET_MODE:
	    /**/
	    {
		XIMText *text = (XIMText *)Xmalloc(sizeof(XIMText));
		char *name = (char *)env->ce_mode_list, *key, *t;
		JLEmode *mode;
		int *mode_count;
		int *mode_active;
		int i;

		for (t = name; t = (char *)index(t, ')'); name = t) {
		    while (*name == '(') name++;
		    if (name == t) {
			t++;
			continue;
		    }
		    *t++ = '\0';
		    if (((key = (char *)index(name, ' ')) == NULL) &&
			((key = (char *)index(name, '\t')) == NULL)) continue;
		    *key++ = '\0';
		    while((*key == ' ') || (*key == '\t')) key++;
		    if (!strcmp(name, "ce_name")) {
			mode = &private->ce_name;
			mode_active = &private->ce_name_active;
			mode_count = &private->ce_name_count;
		    } else if (!strcmp(name, "conv")) {
			mode = &private->conv;
			mode_active = &private->conv_active;
			mode_count = &private->conv_count;
		    } else if (!strcmp(name, "cm_state")) {
			mode = &private->cm_state;
			mode_active = &private->cm_state_active;
			mode_count = &private->cm_state_count;
		    } else {
			continue;
		    }
		    for (i = 0; i < *mode_count; i++) {
			if (!strcmp((*mode)[i].key, key)) {
			    *mode_active = i;
			    break;
			}
		    }
		}
		sprintf(buf, "%s%s%s",
			private->ce_name[private->ce_name_active].label,
			private->conv[private->conv_active].label,
			private->cm_state[private->cm_state_active].label);
		text->length = strlen(buf);
		text->feedback = (XIMFeedback *)NULL;
		text->encoding_is_wchar = False;
		text->string.multi_byte = Xmalloc(text->length + 1);
		strcpy(text->string.multi_byte, buf);
		(*ic->status->methods->draw)(ic, text);
	    }
	    break;
	default:
	    break;
	}
    }
}

HandleCMEvent(ic, private, put)
InputContext ic;
JLEPrivate private;
struct env_to_cm *put;
{
    ProcCMtoENV(ic, private, cm_put(private->cm_id, private->cm_session, put), False);
}

static Bool
Create(ic)
InputContext ic;
{
    JLEPrivate private;
    static struct cm_initstruct init;
    struct cm_to_env *get;
    struct passwd *pwd;
    struct group *grp;
    static char host[128];
    static char buf[256];
    FILE *Modestrfile;
    char *s, *t;
    JLEmode *mode;
    int *mode_count;

    private = ic->private->private;
    sprintf(buf, "/usr/lib/mle/%s/se/sunview/mode.dat", ic->im->locale_name);
    Modestrfile = fopen(buf, "r");
    while (t = s = fgets(buf, 256, Modestrfile)) {
	t = (char *)index(t, ':');
	*t = '\0';
	if (!strcmp(s, "ce_name")) {
	    mode = &private->ce_name;
	    mode_count = &private->ce_name_count;
	} else if (!strcmp(s, "conv")) {
	    mode = &private->conv;
	    mode_count = &private->conv_count;
	} else if (!strcmp(s, "cm_state")) {
	    mode = &private->cm_state;
	    mode_count = &private->cm_state_count;
	} else {
	    continue;
	}
	t++;
	(*mode_count)++;
	*mode = (JLEmode) (*mode ?
		Xrealloc(*mode, sizeof(JLEmodeRec) * *mode_count) :
		Xmalloc(sizeof(JLEmodeRec) * *mode_count));
	(*mode_count)--;
	s = (char *)index(t, ':');
	*s ='\0';
	(*mode)[*mode_count].key = Xmalloc(strlen(t) + 1);
	strcpy((*mode)[*mode_count].key, t);
	s++;
	t = (char *)index(s, ':');
	*t ='\0';
	(*mode)[*mode_count].label = (u_char *)Xmalloc(strlen(s) + 1);
	strcpy((*mode)[*mode_count].label, s);
	(*mode_count)++;
    }
    fclose(Modestrfile);

    private->select = (Select)XAllocFreeArea(sizeof(SelectRec));
    private->cm_id = 0;
    init.env_value = 0;
    init.usr_auth_info.uid = getuid();
    pwd = getpwuid(init.usr_auth_info.uid);
    init.usr_auth_info.user_name = pwd->pw_name;
    init.usr_auth_info.gid = getgid();
    grp = getgrgid(init.usr_auth_info.gid);
    init.usr_auth_info.grp_name = grp ? grp->gr_name : "";
    init.usr_auth_info.hid = gethostid();
    gethostname(host, 128);
    init.usr_auth_info.host_name = host;
    private->cm_session = cm_open(private->cm_id, &init, &get);
    ProcCMtoENV(ic, private, get, False);
}

static Bool
Destroy(ic)
InputContext ic;
{
    JLEPrivate private = ic->private->private;

    if (private->select) Xfree(private->select);
    Xfree(private->ce_name);
    Xfree(private->conv);
    Xfree(private->cm_state);
    cm_close(private->cm_id, private->cm_session);
    Xfree(private);
    Xfree(ic->private);
}

static Bool
Begin(ic)
InputContext ic;
{
    JLEPrivate private = ic->private->private;
    static struct env_to_cm cmon;
    static Bool is_first_time = True;
    if (is_first_time) {
	cmon.ec_next = (struct env_to_cm *)NULL;
	cmon.ec_operation = CM_CMON;
    }
    HandleCMEvent(ic, private, &cmon);
}

static Bool
End(ic)
InputContext ic;
{
    JLEPrivate private = ic->private->private;
    static struct env_to_cm cmoff;
    static Bool is_first_time = True;

    if (is_first_time) {
	is_first_time = False;
	cmoff.ec_next = (struct env_to_cm *)NULL;
	cmoff.ec_operation = CM_SIMPLE_EVENT;
	cmoff.ec_key = private->cmon_key;
    }
    HandleCMEvent(ic, private, &cmoff);
}

static Bool
Reset(ic)
InputContext ic;
{
    JLEPrivate private = ic->private->private;
    static struct env_to_cm cmev[2];
    struct env_to_cm *p = &cmev[0];
    struct cm_to_env *get;

    if (private->region == SELECT_REGION) {
	p->ec_next = &cmev[1];
	p->ec_operation = CM_SELECT;
	p->ec_select = 0;
    }
    p->ec_next = (struct env_to_cm *)NULL;
    p->ec_operation = CM_RESET;
    HandleCMEvent(ic, private, &cmev[0]);
    get = (struct cm_to_env *)cm_put(private->cm_id, private->cm_session, &cmev[0]);
    ProcCMtoENV(ic, private, get, True);
}

#define TF(n) ((CM_TF_START) + (n))
#define LF(n) ((CM_LF_START) + (n))
#define BF(n) ((CM_BF_START) + (n))

#ifdef IsFunctionKey
#undef IsFunctionKey
#endif IsFunctionKey
#define IsFunctionKey(c)	(((c) & 0xff00) == 0xff00)
#define IsKanaKey(c)	(((c) & 0xff00) == 0x0400)
#define KanaKeyToWC16(k)	(0x8080 + (k & 0x7f))

u_int	KeysymtoCMKey[] = {
/* 0 */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/*   */	'\b',	'\t',	'\n',	NULL,	NULL,	'\r',	NULL,	NULL,
/* 1 */	NULL,	NULL,	NULL,	0x13,	NULL,	NULL,	NULL,	NULL,
/*   */	NULL,	NULL,	NULL,	0x1b,	NULL,	NULL,	NULL,	NULL,
/* 2 */	NULL,	BF(2),	BF(1),	BF(3),	BF(4),	BF(6),	BF(5),	NULL,
/*   */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	BF(2),
/* 3 */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/*   */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/* 4 */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/*   */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/* 5 */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/*   */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/* 6 */	NULL,	NULL,	BF(1),	NULL,	NULL,	NULL,	NULL,	NULL,
/*   */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/* 7 */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/*   */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	BF(4),	NULL,
/* 8 */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/*   */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/* 9 */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/*   */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/* a */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/*   */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/* b */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/*   */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	TF(1),	TF(2),
/* c */	TF(3),	TF(4),	TF(5),	TF(6),	TF(7),	TF(8),	TF(9),	TF(10),
/*   */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/* d */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/*   */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/* e */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/*   */	NULL,	NULL,	BF(4),	NULL,	NULL,	NULL,	NULL,	NULL,
/* f */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
/*   */	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	0x7f
};

static Bool
XKeyToCMKey(ic, key, ev)
InputContext ic;
XKeyEvent *key;
unsigned long *ev;
{
    KeySym keysym;
    static char buf[128];
    int n = 128;
    n = XLookupString(key, buf, n, &keysym, NULL);
    if (IsFunctionKey(keysym)) {
	*ev = KeysymtoCMKey[0xff & keysym];
	return(*ev != 0);
    }
    if (n == 1) {
	*ev = (unsigned long)buf[0];
	return(True);
    }
    if (IsKanaKey(keysym)) {
	*ev = KanaKeyToWC16(keysym);
	return(True);
    }
    return(False);
}

static Bool
Keyin(ic, key)
InputContext ic;
XKeyEvent *key;
{
    JLEPrivate private = ic->private->private;
    static struct env_to_cm cmev;
    static Bool is_first_time = True;
    unsigned long cmkey;

    if (is_first_time) {
	is_first_time = False;
	cmev.ec_next = (struct env_to_cm *)NULL;
	cmev.ec_operation = CM_SIMPLE_EVENT;
    }
    if (XKeyToCMKey(ic, key, &cmkey)) {
	cmev.ec_key = cmkey;
	HandleCMEvent(ic, private, &cmev);
    }
}
