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

/*
 * If you rewrite this code,
 * condisional compiled line by TEMPLATE must be alterd.
 *
 * For drawing preedit/status and returning XIMFunction,
 * you must use functions bellow.
 *
 * (*ic->methods->send_key)(InputContext, XKeyEvent *)
 *   send not used KeyEvent in InputMethod
 *   and X{wc,mb}LookupString() returns with XIMKeysym
 *
 * (*ic->methods->send)(InputContext, XIMText *)
 *   send Convert/Compose -ed string
 *   and X{wc,mb}LookupString() returns with XIMChars
 *   rooms for XIMText * is freed if needed.
 *
 * (*ic->methods->reset_reply)(InputContext, XIMText *)
 *   this function must be called in Reset()
 *   and send Convert/Compose -ing string
 *   and X{wc,mb}ResetIC() returns string
 *   rooms for XIMText * is freed if needed.
 *
 * (*ic->methods->proc_begin)(InputContext)
 *   if your implementation is always handle keyevent,
 *   this function must be called in Create()
 *
 * (*ic->methods->proc_end)(InputContext)
 *   if Convert/Compose -ion is turned off by InputMethod,
 *   this function must be called.
 *   and lator KeyPress Event is not intercepted.
 *   X{mb,wc}LookupString() rolls like XLookupString()
 *   until Registered Convert/Compose Start Key is Pressed.
 *
 * (*ic->preedit->methods->draw)(InputContext, XIMText *)
 *   draw preedit string
 *   XIMText * must be allocate every time,
 *   and NOT free.
 *   rooms are freed by callee when context is not need.
 *   if you clear preedit, call with length field 0.
 *
 * (*ic->status->methods->draw)(InputContext, XIMText *)
 *   draw status string
 *   XIMText * must be allocate every time,
 *   and NOT free.
 *   rooms are freed by callee when context is not need.
 *   if you clear status, call with length field 0.
 *
 */

#include "XIMServer.h"
#ifdef TEMPLATE
#include <X11/keysym.h>

#define STATUS_LABEL "[Compose]"
#define BUF_SIZE (64)
#endif /* TEMPLATE */

static Bool Create(), Destroy(), Begin(), End(), Reset(), Keyin();

PrivateMethodsRec private_method = {
    Create,
    Destroy,
    Begin,
    End,
    Reset,
    Keyin
};

#ifdef TEMPLATE
static Ximp_Key key = {
	{0, 0, XK_Multi_key}
};

static Ximp_KeyList keys = {
	1,
	&key,
};
#endif /* TEMPLATE */

/*
 * Define Conversion/Compose Start Key(s).
 *
 *   If key sequence is always handled by IMServer,
 *   this value should be NULL and call (*ic->methods->proc_begin)(ic);
 *   in Create().
 */
Ximp_KeyList *IMStartKeyList = &keys;

/*
 * Allocate rooms for InputMethod,
 * and pre-processing on XCreateIC().
 */
AllocInputMethodPrivate(ic)
InputContext ic;
{
#ifdef TEMPLATE
    char *private;
#endif /* TEMPLATE */

    ic->private = (MethodPrivate)XAllocFreeArea(sizeof(MethodPrivateRec));
    ic->private->methods = &private_method;
#ifdef TEMPLATE
    private =
    ic->private->private = Xmalloc(4);
    *private = '\0';
#endif /* TEMPLATE */
}

/*
 * post-processing on XCreateIC().
 */
static Bool
Create(ic)
InputContext ic;
{
#ifdef TEMPLATE
    char *private = ic->private->private;
/*
 * If IMServer handle whole key sequence,
 *    (*ic->methods->proc_begin)(ic);
 * is here.
 */
#endif /* TEMPLATE */
}

/*
 * XDestroyIC() or XCloseIM() or Client exit
 * Free InputMethod private rooms.
 */
static Bool
Destroy(ic)
InputContext ic;
{
#ifdef TEMPLATE
    char *private = ic->private->private;

    Xfree(private);
#endif /* TEMPLATE */
    Xfree(ic->private);
}

/*
 * One of Conversion/Compose Start Keys has Pressed.
 * Starting Conversion/Compose Sequence
 */
static Bool
Begin(ic)
InputContext ic;
{
#ifdef TEMPLATE
    char *private = ic->private->private;
    XIMText *text = XAlloc(XIMText);

    text->length = strlen(STATUS_LABEL) + 1;
    text->string.multi_byte = Xmalloc(text->length);
    text->feedback = (XIMFeedback *)NULL;
    text->encoding_is_wchar = False;
    strcpy(text->string.multi_byte, STATUS_LABEL);
    (*ic->status->methods->draw)(ic, text);
    *private = '\0';
#endif /* TEMPLATE */
}

/*
 * Conversion/Compose Sequence is terminated by client
 * Now not happened
 */
static Bool
End(ic)
InputContext ic;
{
#ifdef TEMPLATE
    char *private = ic->private->private;
    XIMText *text = XAlloc(XIMText);

    text->string.multi_byte = Xmalloc(text->length = strlen(1));
    text->string.multi_byte[0] = '\0';
    text->feedback = (XIMFeedback *)NULL;
    text->encoding_is_wchar = False;
    (*ic->status->methods->draw)(ic, text);
#endif /* TEMPLATE */
}

/*
 * X{mb,wc}ResetIC() is called.
 * Must call (*ic->methods->reset_reply)(InputContext, XIMText *)
 * for return value.
 */
static Bool
Reset(ic)
InputContext ic;
{
#ifdef TEMPLATE
    XIMText *text;
    XIMText composed;

    text = XAlloc(XIMText);
    text->length = 0;
    text->encoding_is_wchar = False;
    text->feedback = (XIMFeedback *)NULL;
    text->string.multi_byte = Xmalloc(1);
    text->string.multi_byte[0] = '\0';
    (*ic->preedit->methods->draw)(ic, text);

    composed.length = 0;
    composed.encoding_is_wchar = False;
    composed.feedback = (XIMFeedback *)NULL;
    composed.string.multi_byte = "";
    (*ic->methods->reset_reply)(ic, &composed);

    text = XAlloc(XIMText);
    text->string.multi_byte = Xmalloc(text->length = strlen(1));
    text->string.multi_byte[0] = '\0';
    text->feedback = (XIMFeedback *)NULL;
    text->encoding_is_wchar = False;
    (*ic->status->methods->draw)(ic, text);
    (*ic->methods->proc_end)(ic);
#endif /* TEMPLATE */
}

#ifdef TEMPLATE
struct comp_rec {
    char *comp_seq;
    char *composed_char;
};

static struct comp_rec comp_data[] = {
{ "  ", "\240"},
{ "!!", "\241"},
{ "c/", "\242"}, { "c/", "\242"}, { "c/", "\242"}, { "c/", "\242"},
{ "l/", "\243"}, { "l/", "\243"}, { "l/", "\243"}, { "l/", "\243"},
{ "ox", "\244"}, { "ox", "\244"}, { "ox", "\244"}, { "ox", "\244"},
{ "y-", "\245"}, { "y-", "\245"}, { "y-", "\245"}, { "y-", "\245"},
{ "||", "\246"},
{ "so", "\247"}, { "so", "\247"}, { "so", "\247"}, { "so", "\247"},
{ "!!", "\250"},
{ "co", "\251"},
{ "!!", "\252"},
{ "<<", "\253"},
{ "!!", "\254"},
{ "!!", "\255"},
{ "!!", "\256"},
{ "!!", "\257"},
{ "0^", "\260"},
{ "+_", "\261"},
{ "2^", "\262"},
{ "3^", "\263"},
{ "!!", "\264"},
{ "/u", "\265"},
{ "p!", "\266"},
{ ".^", "\267"},
{ "!!", "\270"},
{ "1^", "\271"},
{ "o_", "\272"},
{ ">>", "\273"},
{ "14", "\274"},
{ "12", "\275"},
{ "34", "\276"},
{ "??", "\277"},
{ "A`", "\300"},
{ "A'", "\301"},
{ "A^", "\302"},
{ "A~", "\303"},
{ "A\"", "\304"},
{ "A*", "\305"},
{ "AE", "\306"},
{ "C,", "\307"},
{ "E`", "\310"},
{ "E'", "\311"},
{ "E^", "\312"},
{ "E\"", "\313"},
{ "I`", "\314"},
{ "I'", "\315"},
{ "I^", "\316"},
{ "I\"", "\317"},
{ "D-", "\320"},
{ "N~", "\321"},
{ "O`", "\322"},
{ "O'", "\323"},
{ "O^", "\324"},
{ "O~", "\325"},
{ "O\"", "\326"},
{ "XX", "\327"},
{ "O/", "\330"},
{ "U`", "\331"},
{ "U'", "\332"},
{ "U^", "\333"},
{ "U\"", "\334"},
{ "Y'", "\335"},
{ "!!", "\336"},
{ "ss", "\337"},
{ "a`", "\340"},
{ "a'", "\341"},
{ "a^", "\342"},
{ "a~", "\343"},
{ "a\"", "\344"},
{ "a*", "\345"},
{ "ae", "\346"},
{ "c,", "\347"},
{ "e`", "\350"},
{ "e'", "\351"},
{ "e^", "\352"},
{ "e\"", "\353"},
{ "i`", "\354"},
{ "i'", "\355"},
{ "i^", "\356"},
{ "i\"", "\357"},
{ "d!", "\360"},
{ "n~", "\361"},
{ "o`", "\362"},
{ "o'", "\363"},
{ "o^", "\364"},
{ "o~", "\365"},
{ "o\"", "\366"},
{ "!!", "\367"},
{ "o/", "\370"},
{ "u`", "\371"},
{ "u'", "\372"},
{ "u^", "\373"},
{ "u\"", "\374"},
{ "y'", "\375"},
{ "!!", "\376"},
{ "!!", "\377"}
};

static int num_comp;

static char *
Compose(from)
char *from;
{
    static Bool first_time = True;
    int i;

    if (first_time) {
	num_comp = sizeof(comp_data) / sizeof(comp_data[0]);
    }
    for (i = 0; i < num_comp; i++) {
	if (!strcmp(from, comp_data[i].comp_seq)) {
	    return(comp_data[i].composed_char);
	}
    }
    return("");
}
#endif /* TEMPLATE */

/*
 * Handling Conversion/Compose -ing Key Sequence
 */
static Bool
Keyin(ic, key)
InputContext ic;
XKeyEvent *key;
{
    char *private = ic->private->private;
    static char buf[BUF_SIZE];
    int n, len;
    KeySym keysym;
    XIMText *text;
    XIMText composed;

    n = XLookupString(key, buf, BUF_SIZE, &keysym, NULL);
    buf[n] = '\0';
    if (n == 1) {
	strcat(private, buf);
	len = strlen(private);

	switch (len) {
	case 0:
	    break;
	case 1:
	    text = XAlloc(XIMText);
	    text->length = 1;
	    text->encoding_is_wchar = False;
	    text->feedback = (XIMFeedback *)NULL;
	    text->string.multi_byte = Xmalloc(2);
	    strcpy(text->string.multi_byte, private);
	    (*ic->preedit->methods->draw)(ic, text);
	    break;
	default:
	    text = XAlloc(XIMText);
	    text->length = 0;
	    text->encoding_is_wchar = False;
	    text->feedback = (XIMFeedback *)NULL;
	    text->string.multi_byte = Xmalloc(1);
	    text->string.multi_byte[0] = '\0';
	    (*ic->preedit->methods->draw)(ic, text);

	    if (len == 2) {
		composed.encoding_is_wchar = False;
		composed.feedback = (XIMFeedback *)NULL;
		composed.string.multi_byte = Compose(private);
		composed.length = strlen(composed.string.multi_byte);
		(*ic->methods->send)(ic, &composed);
	    }

	    text = XAlloc(XIMText);
	    text->string.multi_byte = Xmalloc((text->length = 0) + 1);
	    text->string.multi_byte[0] = '\0';
	    text->feedback = (XIMFeedback *)NULL;
	    text->encoding_is_wchar = False;
	    (*ic->status->methods->draw)(ic, text);
	    (*ic->methods->proc_end)(ic);
	    break;
	}
    }
}
