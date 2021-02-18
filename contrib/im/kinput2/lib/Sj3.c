#ifndef lint
static char *rcsid = "$Header: Sj3.c,v 1.1 91/09/28 10:18:55 nao Locked $";
#endif
/*
 * Copyright (c) 1990  Software Research Associates, Inc.
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
/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/*
 * Author: Naoshi Suzuki, SONY Corporation.  (nao@sm.sony.co.jp)
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Atoms.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include "Sj3P.h"

static XtResource resources[] = {
#define offset(field) XtOffset(Sj3Object, sj3.field)
    { XtNsj3serv, XtCSj3serv, XtRString, sizeof(String),
    offset(sj3serv), XtRString, NULL },
    { XtNsj3serv2, XtCSj3serv2, XtRString, sizeof(String),
    offset(sj3serv2), XtRString, NULL },
    { XtNsj3user, XtCSj3user, XtRString, sizeof(String),
    offset(sj3user), XtRString, NULL },
    { XtNrcfile, XtCRcfile, XtRString, sizeof(String),
    offset(rcfile), XtRString, NULL },
    { XtNsbfile, XtCSbfile, XtRString, sizeof(String),
    offset(sbfile), XtRString, NULL },
    { XtNrkfile, XtCRkfile, XtRString, sizeof(String),
    offset(rkfile), XtRString, NULL },
#ifndef USE_HANTOZEN
    { XtNhkfile, XtCHkfile, XtRString, sizeof(String),
    offset(hkfile), XtRString, NULL },
    { XtNzhfile, XtCZhfile, XtRString, sizeof(String),
    offset(zhfile), XtRString, NULL },
#endif /* USE_HANTOZEN */
#undef offset
};

static void ClassInitialize();
static void Initialize(), Destroy();
static Boolean SetValues();
static int InputEvent();
static ICString *GetMode();
static int CursorPos();
static int NumSegments();
static ICString *GetSegment();
static int CompareSegment();
static ICString *GetItemList();
static int SelectItem();
static int ConvertedString();
static int ClearConversion();
static ICString *GetAuxSegments();

Sj3ClassRec sj3ClassRec = {
  { /* object fields */
    /* superclass       */          (WidgetClass) &inputConvClassRec,
    /* class_name       */          "Sj3",
    /* widget_size      */          sizeof(Sj3Rec),
    /* class_initialize */          ClassInitialize,
    /* class_part_initialize    */  NULL,
    /* class_inited     */          FALSE,
    /* initialize       */          Initialize,
    /* initialize_hook  */          NULL,
    /* obj1             */          NULL,
    /* obj2             */          NULL,
    /* obj3             */          0,
    /* resources        */          resources,
    /* num_resources    */          XtNumber(resources),
    /* xrm_class        */          NULLQUARK,
    /* obj4             */          FALSE,
    /* obj5             */          FALSE,
    /* obj6             */          FALSE,
    /* obj7             */          FALSE,
    /* destroy          */          Destroy,
    /* obj8             */          NULL,
    /* obj9             */          NULL,
    /* set_values       */          SetValues,
    /* set_values_hook  */          NULL,
    /* obj10            */          NULL,
    /* get_values_hook  */          NULL,
    /* obj11            */          NULL,
    /* version          */          XtVersion,
    /* callback_private */          NULL,
    /* obj12            */          NULL,
    /* obj13            */          NULL,
    /* obj14            */          NULL,
    /* extension        */          NULL
  },
  { /* inputConv fields */
    /* InputEvent       */          InputEvent,
    /* GetMode          */          GetMode,
    /* CursorPos        */          CursorPos,
    /* NumSegments      */          NumSegments,
    /* GetSegment       */          GetSegment,
    /* CompareSegment   */          CompareSegment,
    /* GetItemList      */          GetItemList,
    /* SelectItem       */          SelectItem,
    /* GetConvertedString   */      ConvertedString,
    /* ClearConversion  */          ClearConversion,
    /* GetAuxSegments   */          GetAuxSegments,
    /* SupportMultipleObjects   */  True,
    /* NoMoreObjects    */          False,
  },
  { /* sj3 fields */
    /* foo              */          NULL,
  }
};

WidgetClass sj3ObjectClass = (WidgetClass)&sj3ClassRec;

static void startSelection();
static void startSymbol();
static void moveSelection();
static int  endSelection();
static int  insertSelection();
static void symbolInit();
static void allocCandlist();
static void allocSymbollist();
static void allocStrdata();
static void allocSbdata();

static void addObject();
static void deleteObject();

static void bell();

static void
ClassInitialize()
{
    
}

static int
InputEvent(w, event)
    Widget w;
    XEvent *event;
{
    Sj3Object               obj =   (Sj3Object)w;
    sj3Buf                  buf = obj->sj3.sj3buf;
    int                     ret = 0;
    register int            len;
    register SJ3_EVENT      value,      select;

    /* KeyPress以外は捨てる */
    if (event->type != KeyPress)
        return ret;
    
    value = sj3_conv_key(obj->sj3.sj3buf, (XKeyPressedEvent *)event);

    if (select = (value & KEY_SELECT)) {
        switch (select) {
        case KEY_SELECT_START:
            startSelection(obj);
            break;
        case KEY_SYMBOL_START:
            startSymbol(obj);
            break;
        case KEY_SELECT_SYMBOL:
            /* Not Supported Yet */
            break;
        case KEY_SELECT_RIGHT:
            moveSelection(obj, ICMoveRight);
            break;
        case KEY_SELECT_LEFT:
            moveSelection(obj, ICMoveLeft);
            break;
        case KEY_SELECT_UP:
            moveSelection(obj, ICMoveUp);
            break;
        case KEY_SELECT_DOWN:
            moveSelection(obj, ICMoveDown);
            break;
        case KEY_SELECT_FIRST:
            moveSelection(obj, ICMoveFirst);
            break;
        case KEY_SELECT_LAST:
            moveSelection(obj, ICMoveLast);
            break;
        case KEY_SELECT_NEXTP:
            moveSelection(obj, ICMoveNextPage);
            break;
        case KEY_SELECT_PREVP:
            moveSelection(obj, ICMovePrevPage);
            break;
        case KEY_SELECT_RIGHTMOST:
            moveSelection(obj, ICMoveRightMost);
            break;
        case KEY_SELECT_LEFTMOST:
            moveSelection(obj, ICMoveLeftMost);
            break;
        case KEY_SELECT_END:
            endSelection(obj, False);
            break;
        case KEY_SELECT_ABORT:
            endSelection(obj, True);
            break;
        default:
            break;
        }
    }
    if (value & KEY_CHANGE){
        if (value & KEY_MODE_CHANGE) {
        /* モード変更 */
            XtCallCallbackList(w, obj->inputConv.modechangecallback,
               (XtPointer)NULL);
        }
        if (value & KEY_TEXT_FIXED) {
        /* テキスト確定 */
            XtCallCallbackList(w, obj->inputConv.fixcallback,
               (XtPointer)NULL);
            sj3_clear_buf(buf);
        } else if (value & KEY_TEXT_FLUSH) {
        /* テキストフラッシュ   */
            XtCallCallbackList(w, obj->inputConv.fixcallback,
               (XtPointer)NULL);
            sj3_flush_buf(buf);
        }
        if (value & KEY_TEXT_CHANGE) {
        /* テキスト変更 */
            XtCallCallbackList(w, obj->inputConv.textchangecallback,
               (XtPointer)NULL);
        }
        if (value & KEY_HENKAN_END) {
        /* 変換終了 */
            XtCallCallbackList((Widget)obj, obj->inputConv.endcallback,
                   (XtPointer)NULL);
        }
        if  (value & KEY_BELL){
            bell(obj);
        }
        return ret;
    } else if (value & KEY_TEXT_NO_CHANGE) {
        /* Nothing to do    */
    } 
    if  (value & KEY_BELL){
        bell(obj);
        return ret;
    } else if(value & KEY_RECONNECT) {
        XtCallCallbackList(w, obj->inputConv.textchangecallback,
           (XtPointer)NULL);
        sj3_reconnect(obj->sj3.sj3buf, obj->sj3.sj3serv, obj->sj3.sj3serv2,
            obj->sj3.sj3user);
        return ret;
/**** No Supported *************************
        KEY_HENKAN_START
********************************************/
    } else if (value == KEY_NULL) {
        /* Nothing to do    */
        return ret;
    } else {
#ifdef DEBUG
        XtAppWarning(XtWidgetToApplicationContext(w),
             "sj3 Object: Unknown control event is coming.");
#endif
    }
    return ret;
}

static ICString *
GetMode(w)
    Widget w;
{
    Sj3Object obj = (Sj3Object)w;
    wchar  *mode;
    static ICString icstr;

    mode = sj3_get_mode_str(obj->sj3.sj3buf);
    icstr.data = (char *)mode;
    icstr.nchars = wstrlen(mode);
    icstr.nbytes = icstr.nchars * sizeof(wchar);
    icstr.attr = ICAttrNormalString;
    return &icstr;
}

static int
CursorPos(w, nsegp, ncharp)
    Widget w;
    Cardinal *nsegp;
    Cardinal *ncharp;
{
    Sj3Object obj = (Sj3Object)w;

    return(sj3_get_position(obj->sj3.sj3buf, nsegp, ncharp));
}

static int
NumSegments(w)
Widget w;
{
    Sj3Object obj = (Sj3Object)w;

    return (sj3_get_segnum(obj->sj3.sj3buf));
}

static ICString *
GetSegment(w, n)
    Widget      w;
    Cardinal    n;
{
    Sj3Object   obj = (Sj3Object)w;
    sj3Buf      buf = obj->sj3.sj3buf;
    static      ICString seg;
    int         attr;

    seg.data = (char *)sj3_get_seg(obj->sj3.sj3buf, n, &attr);
    seg.nchars = wstrlen(seg.data);
    seg.nbytes = seg.nchars * sizeof(wchar);
    seg.attr = attr;

    return &seg;
}

/* ARGSUSED */
static int
CompareSegment(w, seg1, seg2, n)
    Widget      w;
    ICString    *seg1;
    ICString    *seg2;
    Cardinal    *n;
{
    register wchar      *p,     *q;
    int                 len,    nsame = 0;
    int                 result = ICSame;

    if (seg1->attr != seg2->attr) result |= ICAttrChanged;

    len = seg1->nchars > seg2->nchars ? seg2->nchars : seg1->nchars;
    p = (wchar *)seg1->data;
    q = (wchar *)seg2->data;
    while (nsame < len && *p++ == *q++) nsame++;

    if (nsame != len || len != seg1->nchars || len != seg2->nchars)
    result |= ICStringChanged;

    if (n) *n = nsame;

    return result;

}

static ICString *
GetItemList(w, n)
    Widget      w;
    Cardinal    *n;
{
    Sj3Object       obj = (Sj3Object)w;
    sj3Buf          buf = obj->sj3.sj3buf;
    
    switch (obj->sj3.state) {
    case selection_state:
        *n = obj->sj3.numcand;
        return obj->sj3.candlist;
    case symbol_state:
        *n = obj->sj3.numsymbols;
        return obj->sj3.symbollist;
    default:
        *n = 0;
        return NULL;    /* no item available */
    }
    /* NOTREACHED */

}

static int
SelectItem(w, n)
    Widget      w;
    int         n;
{
    Sj3Object   obj = (Sj3Object)w;
    sj3Buf      buf = obj->sj3.sj3buf;
    int         ret = 0,    changed;

    if (obj->sj3.state == normal_state )
        return -1;
    if (obj->sj3.candlist == NULL && obj->sj3.symbollist) {
        ret = -1;
    } else if (n >= 0) {
        ret = insertSelection(obj, n, &changed);
    }
    
    obj->sj3.state = normal_state;
    if (changed) {
        XtCallCallbackList((Widget)obj,
                   obj->inputConv.textchangecallback,
                   (XtPointer)NULL);
    }
    return ret;

}

static int
ConvertedString(w, encoding, format, length, string)
    Widget      w;
    Atom        *encoding;
    int         *format;
    int         *length;
    XtPointer   *string;
{
    Sj3Object   obj = (Sj3Object)w;
    sj3Buf      buf = obj->sj3.sj3buf;
    wchar       *wbuf, *wp;
    wchar       *data;
    int         len, wlen;
    extern int  convJWStoCT();

    if ((data = sj3_get_converted(obj->sj3.sj3buf)) == NULL) {
        XtAppWarning(XtWidgetToApplicationContext(w),
             "sj3 Object: Could not get converted string");
        return -1;
    }
    wlen = wstrlen(data);
    wbuf = (wchar *)XtMalloc((wlen + 1) * sizeof(wchar));
    (void)bcopy((char *)data, (char *)wbuf,
        sizeof(wchar) * wlen);
    wbuf[wlen] = 0;

    /*
     * Sj3 オブジェクトは COMPOUND_TEXT エンコーディングしかサポートしない
     * COMPOUND_TEXT に変換する
     */
    *encoding = XA_COMPOUND_TEXT(XtDisplayOfObject((Widget)obj));
    *format = 8;

    /* COMPOUND_TEXT は \r が送れないので \n に変換しておく */
    for (wp = wbuf; *wp != 0; wp++) {
    if (*wp == '\r') *wp = '\n';
    }

    *length = len = convJWStoCT(wbuf, (unsigned char *)NULL, 0);
    *string = XtMalloc(len + 1);
    (void)convJWStoCT(wbuf, (unsigned char *)*string, 0);

    /* wbuf を free しておく */
    XtFree((char *)wbuf);
    
    return 0;
}

static int
ClearConversion(w)
    Widget w;
{
    Sj3Object   obj = (Sj3Object)w;

    sj3_clear_buf(obj->sj3.sj3buf);
    XtCallCallbackList(w, obj->inputConv.textchangecallback, (XtPointer)NULL);
    return 0;
}

static ICString *
GetAuxSegments(w, n, ns, nc)
    Widget w;
    Cardinal *n, *ns, *nc;
{
    /* Sj3 doesn't use AuxPanel */
    XtAppWarning(XtWidgetToApplicationContext(w),
         "sj3 Object: GetAuxSegments shouldn't be called");
    return NULL;
}

/* ARGSUSED */
static void
Initialize(req, new, args, num_args)
    Widget req;
    Widget new;
    ArgList args;
    Cardinal *num_args;
{
    Sj3Object   obj = (Sj3Object)new;
    char        home[256];
    char        uname[32];
    char        locale[32];

    obj->sj3.numcand = 0;
    obj->sj3.curcand = 0;
    obj->sj3.strdatasize = 0;
    obj->sj3.sbdatasize = 0;
    obj->sj3.candlistsize = 0;
    obj->sj3.state = normal_state;
    obj->sj3.selectionending = False;

    sj3_set_locale(locale, &obj->sj3.usr_code);  
    obj->sj3.sj3locale = locale;

    if (!obj->sj3.sj3user || *obj->sj3.sj3user == '\0') {
        uname[0] = '\0';
        sj3_set_user(uname, home);  
        obj->sj3.sj3user = uname;
    } else
        sj3_set_user(obj->sj3.sj3user, home);   
                    /* user and dictionary difinition   */
    obj->sj3.sj3buf = sj3_set_sjrc(obj->sj3.rcfile, home, obj->sj3.usr_code);
                    /* user resource difinition */
    if (!obj->sj3.sj3buf)
        XtAppError(XtWidgetToApplicationContext(new),
             "sj3 Object: Allocating buffer failed");

    sj3_set_sjsb(obj->sj3.sj3buf, obj->sj3.sbfile, home, obj->sj3.usr_code);
    obj->sj3.numsymbols = sj3_get_symbolnum(obj->sj3.sj3buf);
    obj->sj3.symbollist = (ICString *)XtMalloc(obj->sj3.numsymbols
                                                    * sizeof(ICString));
    symbolInit(obj);

    sj3_set_sjrk(obj->sj3.sj3buf, obj->sj3.rkfile, home, obj->sj3.usr_code);
                    /* roman kana conversion difinition */
#ifndef USE_HANTOZEN
    sj3_set_sjhk(obj->sj3.sj3buf, obj->sj3.hkfile, home, obj->sj3.usr_code);
    sj3_set_sjzh(obj->sj3.sj3buf, obj->sj3.zhfile, home, obj->sj3.usr_code);
#endif /* USE_HANTOZEN */

    if ((sj3_initialize(obj->sj3.sj3buf, obj->sj3.sj3serv,
                obj->sj3.sj3user, False, False)) != CONNECT_OK) {
        XtAppWarning(XtWidgetToApplicationContext(new),
     "sj3 Object: Failed to connect first server, then try to second server");
        if ((sj3_initialize(obj->sj3.sj3buf, obj->sj3.sj3serv2,
                obj->sj3.sj3user, False, True)) != CONNECT_OK) {
            XtAppError(XtWidgetToApplicationContext(new),
                 "sj3 Object: Failed to connect to second server");
        }
    };
    
    addObject(obj);
}

static void
Destroy(w)
    Widget w;
{
    Sj3Object obj = (Sj3Object)w;
    
    sj3_free_dispbuf(obj->sj3.sj3buf);
    sj3_down(False);
    free(obj->sj3.sj3buf);
    deleteObject(obj);
}

static void
symbolInit(obj)
    Sj3Object obj;
{
    sj3Buf                  buf = obj->sj3.sj3buf;
    ICSelectionControlArg   arg;
    ICString                *strp;
    register int            i;
    Cardinal                nchars;
    wchar                   *p;
    register wchar          symbol[RBUFSIZ];

    nchars = 0;
    for (i = 0, strp = obj->sj3.symbollist;
            i < obj->sj3.numsymbols; i++, strp++) {
        (void)sj3_get_symbol(buf, i, symbol);
        strp->nchars = wstrlen(symbol);
        strp->nbytes = strp->nchars * sizeof(wchar);
        strp->attr = ICAttrNormalString;
        allocSbdata(obj, nchars + strp->nchars);
        (void)bcopy((char *)symbol, (char *)(obj->sj3.sbdata + nchars),
                strp->nbytes);
        nchars += strp->nchars;
    }

    p = obj->sj3.sbdata;
    for (i = 0, strp = obj->sj3.symbollist;
            i < obj->sj3.numsymbols; i++, strp++) {
        strp->data = (char *)p;
        p += strp->nchars;
    }
}

static void
startSymbol(obj)
    Sj3Object obj;
{
    sj3Buf                  buf = obj->sj3.sj3buf;
    ICSelectionControlArg   arg;
    ICString                *strp;
    Cardinal                nchars;
    
    obj->sj3.state = symbol_state;

    arg.command = ICSelectionStart;
    arg.u.selection_kind = ICSelectionCandidates;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
               (XtPointer)&arg);

    /* set current item */
    arg.command = ICSelectionSet;
    arg.u.current_item = sj3_get_cursymbol(buf);
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
               (XtPointer)&arg);
}

static void
startSelection(obj)
    Sj3Object obj;
{
    sj3Buf                  buf = obj->sj3.sj3buf;
    ICSelectionControlArg   arg;
    ICString                *strp;
    int                     ncand,  curcand;
    register int            i;
    Cardinal                nchars;
    wchar                   *p;
    register wchar          item[KANJIBUFSIZ];
    
    sj3_get_itemnum(buf, &ncand, &curcand);

    if (ncand <= 0) {
        bell(obj);
        return;
    }

    obj->sj3.numcand = ncand;
    obj->sj3.curcand = curcand;

    allocCandlist(obj, obj->sj3.numcand);
    
    (void)sj3_init_items(buf);
    nchars = 0;
    for (i = 0, strp = obj->sj3.candlist;
            i < obj->sj3.numcand; i++, strp++) {
        (void)sj3_get_item(buf, i, item);
        strp->nchars = wstrlen(item);
        strp->nbytes = strp->nchars * sizeof(wchar);
        strp->attr = ICAttrNormalString;
        allocStrdata(obj, nchars + strp->nchars);
        (void)bcopy((char *)item, (char *)(obj->sj3.strdata + nchars),
                strp->nbytes);
        nchars += strp->nchars;
    }

    p = obj->sj3.strdata;
    for (i = 0, strp = obj->sj3.candlist;
            i < obj->sj3.numcand; i++, strp++) {
        strp->data = (char *)p;
        p += strp->nchars;
    }
    obj->sj3.state = selection_state;

    arg.command = ICSelectionStart;
    arg.u.selection_kind = ICSelectionCandidates;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
               (XtPointer)&arg);

    /* set current item */
    arg.command = ICSelectionSet;
    arg.u.current_item = curcand;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
               (XtPointer)&arg);
}

static void
moveSelection(obj, dir)
    Sj3Object obj;
    int dir;
{
    ICSelectionControlArg arg;

    if (obj->sj3.state == normal_state) return;
    arg.command = ICSelectionMove;
    arg.u.dir = dir;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
               (XtPointer)&arg);
}

static int
endSelection(obj, abort)
    Sj3Object obj;
    int abort;
{
    ICSelectionControlArg   arg;
    int                     selected;
    int                     ret = 0,    changed = False;
    sj3Buf                  buf = obj->sj3.sj3buf;

    if (obj->sj3.selectionending)
        return 0;

    if (obj->sj3.state == normal_state)
        return -1;

    arg.command = ICSelectionEnd;
    arg.u.current_item = -1;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
               (XtPointer)&arg);

    if (!abort && (selected = arg.u.current_item) >= 0) {
        ret = insertSelection(obj, selected, &changed);
    }

    switch (obj->sj3.state) {
    case selection_state:
        sj3_item_end(buf);
        break;
    case symbol_state:
        sj3_symbol_end(buf);
        break;
    }
    obj->sj3.state = normal_state;

    if (changed) {
        XtCallCallbackList((Widget)obj,
               obj->inputConv.textchangecallback,
               (XtPointer)NULL);
    }

    return ret;
}

/* ARGSUSED */
static Boolean
SetValues(cur, req, wid, args, num_args)
    Widget cur;
    Widget req;
    Widget wid;
    ArgList args;
    Cardinal *num_args;
{
    return False;        
}

static int
insertSelection(obj, selected, changed)
    Sj3Object   obj;
    int         selected;
    int         *changed;
{
    sj3Buf      buf = obj->sj3.sj3buf;
    int         ret = 0;

    obj->sj3.selectionending = True;
    if (obj->sj3.state == symbol_state) {
        obj->sj3.cursymbol = selected;
        ret = sj3_set_symbol(buf, selected, changed);
    } else {
        obj->sj3.curcand = selected;
        ret = sj3_set_item(buf, selected, changed);
    }
    obj->sj3.selectionending = False;

    return ret;
}

static void
allocCandlist(obj, n)
    Sj3Object   obj;
    int         n;
{
    ICString *p;

    if (n <= obj->sj3.candlistsize) return;

    if (obj->sj3.candlistsize == 0) {
    p = (ICString *)XtMalloc(n * sizeof(ICString));
    } else {
    p = (ICString *)XtRealloc((char *)obj->sj3.candlist,
                  n * sizeof(ICString));
    }

    obj->sj3.candlist = p;
    obj->sj3.candlistsize = n;
}

static void
allocStrdata(obj, nchars)
    Sj3Object   obj;
    Cardinal    nchars;
{
    wchar *p;

    if (nchars <= obj->sj3.strdatasize) return;

    if (obj->sj3.strdatasize == 0) {
        if (nchars < 256) nchars = 256;
            p = (wchar *)XtMalloc(nchars * sizeof(wchar));
    } else {
        if (nchars - obj->sj3.strdatasize < 256)
            nchars = obj->sj3.strdatasize + 256;
        p = (wchar *)XtRealloc((char *)obj->sj3.strdata,
                     nchars * sizeof(wchar));
    }

    obj->sj3.strdata = p;
    obj->sj3.strdatasize = nchars;
}

static void
allocSbdata(obj, nchars)
    Sj3Object   obj;
    Cardinal    nchars;
{
    wchar *p;

    if (nchars <= obj->sj3.sbdatasize) return;

    if (obj->sj3.sbdatasize == 0) {
        if (nchars < 256) nchars = 256;
            p = (wchar *)XtMalloc(nchars * sizeof(wchar));
    } else {
        if (nchars - obj->sj3.sbdatasize < 256)
            nchars = obj->sj3.sbdatasize + 256;
        p = (wchar *)XtRealloc((char *)obj->sj3.sbdata,
                     nchars * sizeof(wchar));
    }

    obj->sj3.sbdata = p;
    obj->sj3.sbdatasize = nchars;
}

/*
 * keeping list of objects
 */
typedef struct _oblist_ {
    Sj3Object       obj;
    struct _oblist_ *next;
} ObjRec;

static ObjRec *ObjList = NULL;

static void
addObject(obj)
    Sj3Object obj;
{
    ObjRec *objp = XtNew(ObjRec);

    objp->obj = obj;
    objp->next = ObjList;
    ObjList = objp;
}

static void
deleteObject(obj)
    Sj3Object obj;
{
    ObjRec *objp, *objp0;

    for (objp0 = NULL, objp = ObjList;
            objp != NULL;
            objp0 = objp, objp = objp->next) {
        if (objp->obj == obj) {
            if (objp0 == NULL) {
                ObjList = objp->next;
            } else {
                objp0->next = objp->next;
            }
            XtFree((char *)objp);
            return;
        }
    }
}

static void
bell(obj)
    Sj3Object obj;
{
    XBell(XtDisplayOfObject((Widget)obj), 0);
}
