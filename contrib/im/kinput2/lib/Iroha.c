#ifndef lint
static char *rcsid = "$Id: Iroha.c,v 1.29 91/09/13 17:09:24 kon Exp $";
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

/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 * Author: Akira Kon, NEC Corporation.  (kon@d1.bs2.mt.nec.co.jp)
 */

/* 直さなければならないところ

 ・Destroy が呼ばれないので CloseUIContext できない。
 ・モード領域の大きさ。(これは他のファイルだろうな)

 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Atoms.h>
#include <X11/keysym.h>
#include "IrohaP.h"

#include <iroha/jrkanji.h>

static XtResource resources[] = {
#define offset(field) XtOffset(IrohaObject, iroha.field)
    { XtNirohahost, XtCIrohahost, XtRString, sizeof(String),
	offset(irohahost), XtRString, NULL },
    { XtNirohafile, XtCIrohafile, XtRString, sizeof(String),
	offset(irohafile), XtRString, NULL },
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
static void displayPreEdit();
#ifdef KC_SETLISTCALLBACK
static void listfunc();
static void openSelection();
#define SELECTION_SET 0 /* SelectionStart をしても良いと言う情報を設定する */
#define SELECTION_DO  1 /* 実際に SelectionStart を開始する */
#else /* !KC_SETLISTCALLBACK */
#define openSelection(x, y, z)
#endif /* !KC_SETLISTCALLBACK */

static ICString *GetAuxSegments();

IrohaClassRec irohaClassRec = {
  { /* object fields */
    /* superclass		*/	(WidgetClass) &inputConvClassRec,
    /* class_name		*/	"Iroha",
    /* widget_size		*/	sizeof(IrohaRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* obj1			*/	NULL,
    /* obj2			*/	NULL,
    /* obj3			*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* obj4			*/	FALSE,
    /* obj5			*/	FALSE,
    /* obj6			*/	FALSE,
    /* obj7			*/	FALSE,
    /* destroy			*/	Destroy,
    /* obj8			*/	NULL,
    /* obj9			*/	NULL,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* obj10			*/	NULL,
    /* get_values_hook		*/	NULL,
    /* obj11			*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* obj12			*/	NULL,
    /* obj13			*/	NULL,
    /* obj14			*/	NULL,
    /* extension		*/	NULL
  },
  { /* inputConv fields */
    /* InputEvent		*/	InputEvent,
    /* GetMode			*/	GetMode,
    /* CursorPos		*/	CursorPos,
    /* NumSegments		*/	NumSegments,
    /* GetSegment		*/	GetSegment,
    /* CompareSegment		*/	CompareSegment,
    /* GetItemList		*/	GetItemList,
    /* SelectItem		*/	SelectItem,
    /* GetConvertedString	*/	ConvertedString,
    /* ClearConversion		*/	ClearConversion,
    /* GetAuxSegments		*/	GetAuxSegments,
    /* SupportMultipleObjects	*/	True,
    /* NoMoreObjects		*/	False,
  },
  { /* iroha fields */
    /* foo			*/	0,
  }
};

WidgetClass irohaObjectClass = (WidgetClass)&irohaClassRec;

static void fix();

static ICString *SymbolList;
static int NumSymbols;

static void addObject();
static void deleteObject();

static int checkIfFunctionalChar();

static int ignoreListfunc = 0;

static Display *displaybell = (Display *)0;

static void
ClassInitialize()
{
  /* 何もしない */
}

static int
InputEvent(w, event)
Widget w;
XEvent *event;
{
    IrohaObject obj = (IrohaObject)w;
    char buf[1024];
    KeySym ks;
    XComposeStatus   compose_status;
    jrKanjiStatus    kanji_status;
    int len, nbytes, functionalChar;

    /* KeyPress以外は捨てる */
    if (event->type != KeyPress /*&& event->type != KeyRelease*/) return 0;

    obj->iroha.textchanged = False;

    /* 取りあえず文字に直してしまう */
    nbytes = XmuLookupKana (event, buf, 1024,
			    &ks, &compose_status);

    if (ks == XK_space && (event->xkey.state & ShiftMask)) {
      void convend();

      convend(obj);
      return 0;
    }

    /* 長さがゼロのものに関してはちょっと救済 */
    functionalChar = checkIfFunctionalChar(event, ks, buf, 1024);
    /* shift+→のような文字だったら１文字と数える */
    if ( !nbytes && functionalChar ) {
      nbytes = 1;
    }

    /* 要らない場合は無視する */
    if (nbytes == 0) return 0;

    /* ベルを鳴らすディスプレイの設定 */
 
    displaybell = XtDisplayOfObject((Widget)obj);

    /* かな漢字変換する */
    len = jrKanjiString(obj, buf[0], buf, 1024, &kanji_status);

    displayPreEdit(obj, len, buf, &kanji_status);
    return 0;
}

static void
displayPreEdit(obj, len, buf, ks)
IrohaObject obj;
int len;
char *buf;
jrKanjiStatus *ks;
{
    Widget w = (Widget)obj;

    if (len > 0) { /* 確定文字がある場合 */
      if (len == 1 && (ks->info & KanjiThroughInfo) &&
	  buf[0] < 0x20 && buf[0] != '\r' && buf[0] != '\t')
	/* XSendEventで送ってもらうような文字(何もしない) */;
      else {
	fixProcForIroha(obj, (unsigned char *)buf, len);
	fix(obj);
      }
    }

    changeTextForIroha(obj, ks);

    /* テキストの変化をチェックする */
    if (obj->iroha.textchanged) {
	XtCallCallbackList(w, obj->inputConv.textchangecallback,
			   (XtPointer)NULL);
	obj->iroha.textchanged = False;
    }

    /* 入力モードをチェックする */
    if (ks->info & KanjiModeInfo) {
      XtCallCallbackList(w, obj->inputConv.modechangecallback,
			 (XtPointer)NULL);
    }

    if (ks->info & KanjiGLineInfo) { /* 一覧行がある場合 */
      ICAuxControlArg arg;

      switch (obj->iroha.ibuf->candstat) {
      case IROHA_GLINE_Start:
	arg.command = ICAuxStart;
	XtCallCallbackList((Widget)obj, obj->inputConv.auxcallback,
			   (XtPointer)&arg);
	break;
      case IROHA_GLINE_End:
	arg.command = ICAuxEnd;
	XtCallCallbackList((Widget)obj, obj->inputConv.auxcallback,
			   (XtPointer)&arg);
	break;
      case IROHA_GLINE_Change:
	arg.command = ICAuxChange;
	XtCallCallbackList((Widget)obj, obj->inputConv.auxcallback,
			   (XtPointer)&arg);
	break;
      }
    }

    /* 終わりかどうかチェックする */
    if (ks->info & KanjiModeInfo) {
      char modeinfo[4];

      jrKanjiControl(obj, KC_SETMODEINFOSTYLE, 1);
      jrKanjiControl(obj, KC_QUERYMODE, modeinfo);
      jrKanjiControl(obj, KC_SETMODEINFOSTYLE, 0);
      if (modeinfo[0] - '@' == IROHA_MODE_AlphaMode) {
	toJapaneseMode(obj);
	convend(obj);
      }
    }
    openSelection(obj, SELECTION_DO, 0/* dummy */);
}

static ICString *
GetMode(w)
Widget w;
{
    IrohaObject obj = (IrohaObject)w;
    wchar *mode;
    static ICString icstr;

    icstr.nchars = obj->iroha.ibuf->modelen;
    if (icstr.nchars > 0) {
      mode = obj->iroha.ibuf->curmode;
      icstr.data = (char *)mode;
      icstr.nbytes = icstr.nchars * sizeof(wchar);
      icstr.attr = ICAttrNormalString;
    }
    return &icstr;
}

static int
CursorPos(w, nsegp, ncharp)
Widget w;
Cardinal *nsegp;
Cardinal *ncharp;
{
    IrohaObject obj = (IrohaObject)w;
    iBuf *ib = obj->iroha.ibuf;
    Cardinal nseg, nchar;
    int ret = 0;

    if (ib->curseg < ib->nseg) {
      nseg = ib->curseg;
      nchar = 0;
    }
    else {
      nseg = ib->offset;
      nchar = ib->len[0 + ib->offset];
      ret = 1;
    }
    if (nsegp) {
      *nsegp = nseg;
    }
    if (ncharp) {
      *ncharp = nchar;
    }

    return ret;
}

static int
NumSegments(w)
Widget w;
{
    IrohaObject obj = (IrohaObject)w;
    iBuf *ib = obj->iroha.ibuf;

    return ib->nseg;
}

static ICString *
GetSegment(w, n)
Widget w;
Cardinal n;
{
    IrohaObject obj = (IrohaObject)w;
    iBuf *ib = obj->iroha.ibuf;
    static ICString seg;

    seg.data = (char *)ib->str[n];
    seg.nchars = ib->len[n];
    seg.nbytes = seg.nchars * sizeof(wchar);
    if (ib->curseg == n) {
      seg.attr = ICAttrConverted | ICAttrCurrentSegment;
    }
    else if (n < ib->offset) {
      seg.attr = ICAttrConverted;
    }
    else {
      seg.attr = ICAttrNotConverted;
    }

    return &seg;
}

/* ARGSUSED */
static int
CompareSegment(w, seg1, seg2, n)
Widget w;
ICString *seg1;
ICString *seg2;
Cardinal *n;
{
    wchar *p, *q;
    int len, nsame;
    int result = 0;

    if (seg1->attr != seg2->attr) result |= ICAttrChanged;

    len = seg1->nchars > seg2->nchars ? seg2->nchars : seg1->nchars;
    nsame = 0;
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
Widget w;
Cardinal *n;
{
    IrohaObject obj = (IrohaObject)w;

    *n = obj->iroha.numcand;
    return obj->iroha.candlist;
}

#define SELECTBUFSIZE 1024

static int
SelectItem(w, n)
Widget w;
int n;
{
  IrohaObject obj = (IrohaObject)w;
  jrKanjiStatus ks;
  jrKanjiStatusWithValue ksv;
  char buf[SELECTBUFSIZE];

  if (n >= 0)
    *(obj->iroha.cur_addr) = n;

  ksv.ks = &ks;
  buf[0] = '\r';
  ksv.buffer = (unsigned char *)buf;
  ksv.bytes_buffer = SELECTBUFSIZE;
  ksv.val = IROHA_FN_Kakutei;
  {
    ignoreListfunc = 1;
    jrKanjiControl(obj, KC_DO, &ksv);
    ignoreListfunc = 0;
  }
  displayPreEdit(obj, ksv.val, buf, ksv.ks);
  return 0;
}

static int
ConvertedString(w, encoding, format, length, string)
Widget w;
Atom *encoding;
int *format;
int *length;
XtPointer *string;
{
    IrohaObject obj = (IrohaObject)w;
    iBuf *ib = obj->iroha.ibuf;
    wchar *wbuf, *wp;
    int len, wlen;
    extern int convJWStoCT();
    int offset = ib->offset;

    if (ib == NULL) return -1;

    if (ib->nseg == 0 || ib->offset == 0) return -1;

    wlen = ib->len[offset - 1];
    if (wlen == 0) return -1;

    wbuf = ib->str[offset - 1];

    /*
     * Iroha オブジェクトは COMPOUND_TEXT エンコーディングしかサポートしない
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

    shiftLeftAll(ib);

    return 0;
}

static int
ClearConversion(w)
Widget w;
{
  IrohaObject obj = (IrohaObject)w;

  jrKanjiStatus ks;
  jrKanjiStatusWithValue ksv;
  char buf[SELECTBUFSIZE];

  ksv.ks = &ks;
  ksv.buffer = (unsigned char *)buf;
  ksv.bytes_buffer = SELECTBUFSIZE;
  jrKanjiControl(obj, KC_KILL, &ksv);
  displayPreEdit(obj, ksv.val, buf, ksv.ks);
  return 0;
}

static ICString *
GetAuxSegments(w, n, ns, nc)
Widget w;
Cardinal *n, *ns, *nc;
{
  IrohaObject obj = (IrohaObject)w;
  iBuf *ib = obj->iroha.ibuf;
  Cardinal nseg, nchar;

  if (n) {
    *n = ib->ngseg;
  }

  if (ib->curgseg < ib->ngseg) {
    nseg = ib->curgseg;
    nchar = 0;
  }
  else {
    nseg = 0;
    nchar = ib->gllen[0];
  }
  if (ns) {
    *ns = nseg;
  }
  if (nc) {
    *nc = nchar;
  }

  return ib->ics;
}

/* ARGSUSED */
static void
Initialize(req, new, args, num_args)
Widget req;
Widget new;
ArgList args;
Cardinal *num_args;
{
    IrohaObject obj = (IrohaObject)new;

    obj->iroha.selectionending = False;
    obj->iroha.textchanged = False;
    obj->iroha.symbollist = SymbolList;
    obj->iroha.numsymbols = NumSymbols;
    obj->iroha.cursymbol = 0;
    obj->iroha.candlist = NULL;
    obj->iroha.candlistsize = 0;
    obj->iroha.numcand = 0;
    obj->iroha.lastTextLengthIsZero = False;
      
    ibufInitialize(obj);

    /* 変換の初期化 */
    initializeIrohaConnection(obj);

    addObject(obj);
}

static int
bell()
{
    XBell(displaybell, 0);
    return 0;
}

static int nIrohaContexts = 0;

static
initializeIrohaConnection(obj)
IrohaObject obj;
{
  char **warn = 0;
  extern (*jrBeepFunc)();

  if (nIrohaContexts == 0) {
#ifdef KC_SETSERVERNAME
    if (obj->iroha.irohahost != NULL) {
      jrKanjiControl(obj, KC_SETSERVERNAME, obj->iroha.irohahost);
    }
#endif /* KC_SETSERVERNAME */
#ifdef KC_SETINITFILENAME
    if (obj->iroha.irohafile != NULL) {
      jrKanjiControl(obj, KC_SETINITFILENAME, obj->iroha.irohafile);
    }
#endif /* KC_SETINITFILENAME */
  
    jrKanjiControl(obj, KC_INITIALIZE, &warn);
    if (warn) {
      char **p;
    
      for (p = warn; *p ; p++) {
	XtAppWarning(XtWidgetToApplicationContext((Widget)obj), *p);
      }
    }
  }
  nIrohaContexts++;

#ifdef DONT_USE_HANKAKU_KATAKANA
  /* 半角カタカナの禁止 */
  jrKanjiControl(obj, KC_INHIBITHANKAKUKANA, 1);
#endif
  
  /* 一覧行を７８コラムに設定 */
  jrKanjiControl(obj, KC_SETWIDTH, 78);

  /* 候補一覧コールバックを設定 */

#ifdef KC_SETLISTCALLBACK
  {
    jrListCallbackStruct lcs;

    lcs.client_data = (char *)obj;
    lcs.callback_func = listfunc;
    jrKanjiControl(obj, KC_SETLISTCALLBACK, (char *)&lcs);
  }
#endif /* KC_SETLISTCALLBACK */

  jrBeepFunc = bell;

  /* さらに日本語入力モードにしておく */
  toJapaneseMode(obj);
}


static void
Destroy(w)
Widget w;
{
    IrohaObject obj = (IrohaObject)w;
    jrKanjiStatusWithValue ksv;
    jrKanjiStatus ks;
    unsigned char buf[512];
    int i;
    
    /* バッファの解放 */

    freeIBuf(obj->iroha.ibuf);

    if (obj->iroha.candlist) {
      for (i = 0 ; i < obj->iroha.candlistsize ; i++) {
	if ((obj->iroha.candlist + i)->data) {
	  XtFree((obj->iroha.candlist + i)->data);
	}
      }

      XtFree((char *)obj->iroha.candlist);
    }

    /* 『いろは』の処理 */
    ksv.buffer = buf;
    ksv.bytes_buffer = 512;
    ksv.ks = &ks;

    jrKanjiControl(obj, KC_CLOSEUICONTEXT, &ksv);

    if (--nIrohaContexts == 0) {
      char **warn = 0;

      jrKanjiControl(0, KC_FINALIZE, &warn);
      if (warn) {
	char **p;
	
	for (p = warn; *p ; p++) {
	  XtAppWarning(XtWidgetToApplicationContext((Widget)obj), *p);
	}
      }
    }

    deleteObject(obj);
}

static Boolean
SetValues(cur, req, wid, args, num_args)
Widget cur;
Widget req;
Widget wid;
ArgList args;
Cardinal *num_args;
/* ARGSUSED */
{
    IrohaObject old = (IrohaObject)cur;
    IrohaObject new = (IrohaObject)wid;

    return False;	     
}

static void
fix(obj)
IrohaObject obj;
{
    /* 確定の処理 */
    XtCallCallbackList((Widget)obj, obj->inputConv.fixcallback,
		       (XtPointer)NULL);	/* ??? */
}

static void
convend(obj)
IrohaObject obj;
{
    XtCallCallbackList((Widget)obj, obj->inputConv.endcallback,
		       (XtPointer)NULL);
}

/*
 * keeping list of objects
 */
typedef struct _oblist_ {
    IrohaObject obj;
    struct _oblist_ *next;
} ObjRec;

static ObjRec *ObjList = NULL;

static void
addObject(obj)
IrohaObject obj;
{
    ObjRec *objp = XtNew(ObjRec);

    objp->obj = obj;
    objp->next = ObjList;
    ObjList = objp;
}

static void
deleteObject(obj)
IrohaObject obj;
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

/*
 * Operations to iroha.ibuf
 */

/* cfuncdef

   changeTextForIroha -- ibuf の内容を kanji_status を用いて書き換える。


   基本的には候補文字列と候補一覧行文字列に関して以下の処理を行う。

   (1) いずれの文字列もテキストが反転している部分とそうでない部分
       が存在し、反転している部分は１カ所しか存在しない。
   (2) したがっていずれの文字列も反転しているところとその両端に反転
       していない部分とが存在する場合に３部分に分かれることになり、
       反転している部分の位置や、反転している箇所がない場合などを
       合わせて考えても３部分以上に分かれることはない。
   (3) したがって、いずれの文字列も最大３つのセグメントに分けて表示
       ウィジェットに渡すようにする。
   (4) ついでにwchar型データに変更する。

 */

changeTextForIroha(cldata, ksp)
caddr_t cldata;
jrKanjiStatus *ksp;
{
    IrohaObject obj = (IrohaObject)cldata;
    iBuf *ibuf = obj->iroha.ibuf;
    int offset = ibuf->offset, i;

    if (ksp->length == 0) {
      ibuf->curseg = offset;
      ibuf->nseg = offset;
      ibuf->len[0 + offset] = ibuf->len[1 + offset] =
	ibuf->len[2 + offset] = 0;
      if (!obj->iroha.lastTextLengthIsZero) {
	obj->iroha.textchanged = True;
	obj->iroha.lastTextLengthIsZero = True;
      }
    }
    else if (ksp->length > 0) {
      obj->iroha.lastTextLengthIsZero = False;
      ibuf->len[1 + offset] = ibuf->len[2 + offset] = 0;
      if (ksp->revLen > 0) {
	if (ksp->revPos == 0) {
	  int remain = ksp->length - ksp->revLen;

	  copyInWchar(ksp->echoStr, ksp->revLen,
		      &(ibuf->str[0 + offset]), &(ibuf->size[0 + offset]),
		      &(ibuf->len[0 + offset]));
	  ibuf->curseg = offset;
	  ibuf->nseg = offset + 1;
	  if (remain) {
	    copyInWchar(ksp->echoStr + ksp->revLen, remain,
			&(ibuf->str[1 + offset]), &(ibuf->size[1 + offset]),
			&(ibuf->len[1 + offset]));
	    ibuf->nseg = offset + 2;
	  }
	}
	else {
	  int remain = ksp->length - ksp->revPos - ksp->revLen;

	  copyInWchar(ksp->echoStr, ksp->revPos,
		      &(ibuf->str[0 + offset]), &(ibuf->size[0 + offset]),
		      &(ibuf->len[0 + offset]));
	  copyInWchar(ksp->echoStr + ksp->revPos, ksp->revLen,
		      &(ibuf->str[1 + offset]), &(ibuf->size[1 + offset]),
		      &(ibuf->len[1 + offset]));
	  ibuf->curseg = offset + 1;
	  ibuf->nseg = offset + 2;
	  if (remain) {
	    copyInWchar(ksp->echoStr + ksp->revPos + ksp->revLen,
			remain,
			&(ibuf->str[2 + offset]), &(ibuf->size[2 + offset]),
			&(ibuf->len[2 + offset]));
	  ibuf->nseg = offset + 3;
	  }
	}
      }
      else {
	copyInWchar(ksp->echoStr, ksp->length,
		    &(ibuf->str[0 + offset]), &(ibuf->size[0 + offset]),
		    &(ibuf->len[0 + offset]));
	ibuf->len[1 + offset] = ibuf->len[2 + offset] = 0;
	ibuf->nseg = offset + 1;
	ibuf->curseg = offset + 1;
      }
      obj->iroha.textchanged = True;
    }
    /* モード */
    if (ksp->info & KanjiModeInfo) {
      copyInWchar(ksp->mode, strlen(ksp->mode),
		  &(ibuf->curmode), &(ibuf->modesize), &(ibuf->modelen));
    }
    /* 一覧行 */
    if (ksp->info & KanjiGLineInfo) {
      if (ksp->gline.length == 0) {
	switch (ibuf->candstat) {
	case IROHA_GLINE_Empty:
	case IROHA_GLINE_End:
	  ibuf->candstat = IROHA_GLINE_Empty;
	  break;
	case IROHA_GLINE_Start:
	case IROHA_GLINE_Change:
	  ibuf->candstat = IROHA_GLINE_End;
	  break;
	}
	ibuf->curgseg = 0;
	ibuf->ngseg = 0;
	ibuf->gllen[0] = ibuf->gllen[1] = ibuf->gllen[2] = 0;
      }
      else if (ksp->gline.length > 0) {
	switch (ibuf->candstat) {
	case IROHA_GLINE_Empty:
	case IROHA_GLINE_End:
	  ibuf->candstat = IROHA_GLINE_Start;
	  break;
	case IROHA_GLINE_Start:
	case IROHA_GLINE_Change:
	  ibuf->candstat = IROHA_GLINE_Change;
	  break;
	}
	ibuf->gllen[1] = ibuf->gllen[2] = 0;
	if (ksp->gline.revLen > 0) {
	  if (ksp->gline.revPos == 0) {
	    int remain = ksp->gline.length - ksp->gline.revLen;
	    
	    copyInWchar(ksp->gline.line, ksp->gline.revLen,
			&(ibuf->gline[0]), &(ibuf->glsize[0]),
			&(ibuf->gllen[0]));
	    ibuf->curgseg = 0;
	    ibuf->ngseg = 1;
	    if (remain) {
	      copyInWchar(ksp->gline.line + ksp->gline.revLen, remain,
			  &(ibuf->gline[1]), &(ibuf->glsize[1]),
			  &(ibuf->gllen[1]));
	      ibuf->ngseg = 2;
	    }
	  }
	  else {
	    int remain = ksp->gline.length
	      - ksp->gline.revPos - ksp->gline.revLen;
	    
	    copyInWchar(ksp->gline.line, ksp->gline.revPos,
			&(ibuf->gline[0]), &(ibuf->glsize[0]),
			&(ibuf->gllen[0]));
	    copyInWchar(ksp->gline.line + ksp->gline.revPos, ksp->gline.revLen,
			&(ibuf->gline[1]), &(ibuf->glsize[1]),
			&(ibuf->gllen[1]));
	    ibuf->curgseg = 1;
	    ibuf->ngseg = 2;
	    if (remain) {
	      copyInWchar(ksp->gline.line
			  + ksp->gline.revPos + ksp->gline.revLen,
			  remain,
			  &(ibuf->gline[2]), &(ibuf->glsize[2]),
			  &(ibuf->gllen[2]));
	      ibuf->ngseg = 3;
	    }
	  }
	}
	else {
	  copyInWchar(ksp->gline.line, ksp->gline.length,
		      &(ibuf->gline[0]), &(ibuf->glsize[0]),
		      &(ibuf->gllen[0]));
	  ibuf->gllen[1] = ibuf->gllen[2] = 0;
	  ibuf->ngseg = 1;
	  ibuf->curgseg = 1;
	}
      }
      for (i = 0 ; i < ibuf->ngseg ; i++) {
	ibuf->ics[i].data = (char *)ibuf->gline[i];
	ibuf->ics[i].nchars = ibuf->gllen[i];
	ibuf->ics[i].nbytes = ibuf->gllen[i] * sizeof(wchar);
	ibuf->ics[i].attr = ICAttrConverted;
      }
      if (ibuf->curgseg < ibuf->ngseg) {
	ibuf->ics[ibuf->curgseg].attr |= ICAttrCurrentSegment;
      }
    }
}

/* cfuncdef

   copyInWchar -- wchar にしてコピーする。

   mbs, mblen で示されたＥＵＣ文字列を wsbuf のポイント先のバッファに
   格納する。wsbuf のサイズは wssize のポイント先に格納されている値で
   指定されるが、それでは小さい時は copyInWchar 内で XtRealloc され、
   新たにアロケートされたバッファが wsbuf のポイント先に格納される。
   また、バッファの新たなサイズが wssize のポイント先に格納される。
   得られた文字数がwslen のポイント先に格納される。

 */

copyInWchar(mbs, mblen, wsbuf, wssize, wslen)
unsigned char *mbs;
int mblen;
wchar **wsbuf;
int *wssize, *wslen;
{
  wchar bigbuf[1024];
  int wlen, i;

  wlen = Iroha_mbstowcs(bigbuf, mbs, mblen);

  if (*wssize == 0) {
    if ((*wsbuf = (wchar *)XtMalloc((wlen + 1) * sizeof(wchar)))
	 == (wchar *)0) {
      /* エラーだ */
    }
    else {
      *wssize = wlen + 1;
    }
  }
  if (wlen + 1 > *wssize) {
    if ((*wsbuf = (wchar *)XtRealloc(*wsbuf, (wlen + 1) * sizeof(wchar)))
	 == (wchar *)0) {
      /* エラーだ。どうすれば良い？ */
    }
    else {
      *wssize = wlen + 1;
    }
  }
  *wslen = wlen;
  (void)bcopy(bigbuf, *wsbuf, wlen * sizeof(wchar));
  *(*wsbuf + wlen) = (wchar)0;

  return 0; /* ０は特に意味が無い */
}

/* cfuncdef

   Iroha_mbstowcs -- ＥＵＣコードをワイドキャラクタに変換する。

 */

int
Iroha_mbstowcs(wc, mb, n)
wchar *wc;
unsigned char *mb;
int n; /* 本当は size_t */
{
  int i, j;

  for (i = 0, j = 0 ; i < n ;) {
    if (mb[i] & 0x80) {
      if (mb[i] == 0x8e) {
	wc[j++] = mb[++i];
	i++;
      }
      else {
	wc[j++] = (mb[i] << 8) + mb[i + 1];
	i += 2;
      }
    }
    else {
      wc[j++] = mb[i++];
    }
  }
  return j;
}

/* cfuncdef

   fixProcForIroha -- 確定した文字の処理を行う

 */

fixProcForIroha(cldata, fixedstr, fixedlen)
caddr_t cldata;
unsigned char *fixedstr;
int fixedlen;
{
  IrohaObject obj = (IrohaObject)cldata;
  iBuf *ib = obj->iroha.ibuf;
  int offset = ib->offset;
  
  if (offset < NConvertedSegments) {
    shiftRight(ib);
    offset = ib->offset;
  }
  else {
    shiftLeft(ib);
  }
  copyInWchar(fixedstr, fixedlen,
	      &(ib->str[offset - 1]), &(ib->size[offset - 1]),
	      &(ib->len[offset - 1]));
}

/* cfuncdef

   shiftRight -- ibuf の文字バッファの offset 以降を右にシフトする

 */

shiftRight(ib)
iBuf *ib;
{
  int i;
  wchar *tmpbuf;
  int tmpsize, tmplen;
  int offset = ib->offset;

  tmpbuf = ib->str[ib->nseg];
  tmpsize = ib->size[ib->nseg];
  for (i = ib->nseg ; offset < i ; i--) {
    ib->str[i] = ib->str[i - 1];
    ib->size[i] = ib->size[i - 1];
    ib->len[i] = ib->len[i - 1];
  }
  ib->str[offset] = tmpbuf;
  ib->size[offset] = tmpsize;
  ib->offset++;
  ib->nseg++;
  ib->curseg++;
}

/* cfuncdef

   shiftLeft -- ibuf の文字バッファの offset 以前を左にシフトする

 */

shiftLeft(ib)
iBuf *ib;
{
  int i;
  wchar *tmpbuf;
  int tmpsize, tmplen;
  int offset = ib->offset;

  tmpbuf = ib->str[0];
  tmpsize = ib->size[0];
  for (i = 0 ; i < offset - 1 ; i++) {
    ib->str[i] = ib->str[i + 1];
    ib->size[i] = ib->size[i + 1];
    ib->len[i] = ib->len[i + 1];
  }
  ib->str[offset - 1] = tmpbuf;
  ib->size[offset - 1] = tmpsize;
}

/* cfuncdef

   shiftLeftAll -- ibuf の文字バッファ全体を左にシフトする

 */

shiftLeftAll(ib)
iBuf *ib;
{
  int i;
  wchar *tmpbuf;
  int tmpsize, tmplen;
  int nseg = ib->nseg;

  tmpbuf = ib->str[0];
  tmpsize = ib->size[0];
  for (i = 0 ; i < nseg - 1 ; i++) {
    ib->str[i] = ib->str[i + 1];
    ib->size[i] = ib->size[i + 1];
    ib->len[i] = ib->len[i + 1];
  }
  ib->str[nseg - 1] = tmpbuf;
  ib->size[nseg - 1] = tmpsize;
  ib->nseg--;
  ib->curseg--;
  ib->offset--;
}

/* cfuncdef

   ibufInitialize -- ibuf の初期化処理
 */

ibufInitialize(obj)
IrohaObject obj;
{
  int i;
  iBuf *ib;

  ib = obj->iroha.ibuf = (iBuf *)XtMalloc(sizeof(iBuf));
  if (ib == 0) {
    /* エラーだ。どうすべ */
  }
  for (i = 0 ; i < NConvertedSegments + 3 ; i++) {
    ib->size[i] = 0;
    ib->len[i] = 0;
  }
  ib->offset = ib->curseg = ib->nseg = 0;
  ib->candstat = IROHA_GLINE_Empty;
  for (i = 0 ; i < 3 ; i++) {
    ib->gline[i] = 0;
    ib->glsize[i] = ib->gllen[i] = 0;
  }
  ib->modesize = 0;
  ib->modelen = 0;
}

/* cfuncdef

   freeIBuf -- ibuf の free

 */

freeIBuf(ib)
iBuf *ib;
{
  int i;

  if (ib == 0) {
    return 0;
  }
  for (i = 0 ; i < NConvertedSegments + 3 ; i++) {
    if (ib->size[i] > 0) {
      XtFree(ib->str[i]);
    }
  }
  for (i = 0 ; i < 3 ; i++) {
    if (ib->glsize[i] > 0) {
      XtFree(ib->gline[i]);
    }
  }
  if (ib->modesize > 0) {
    XtFree(ib->curmode);
  }
  free(ib);
  return 0;
}

toJapaneseMode(obj)
IrohaObject obj;
{
  jrKanjiStatusWithValue ksv;
  jrKanjiStatus ks;
  unsigned char buf[1024];
  
  ksv.ks = &ks;
  ksv.buffer = buf;
  ksv.bytes_buffer = 1024;
  ksv.val = IROHA_MODE_HenkanMode;
  jrKanjiControl(obj, KC_CHANGEMODE,  &ksv);
  changeTextForIroha(obj, &ks);
}

/* checkIfFunctionalChar -- シフトキーのように無意味なキーかどうかの判別

  値:		１	意味がある
  		０	無意味(モディファイアキーなど)
*/

static int
checkIfFunctionalChar(event_struct, keysym, buffer_return, bytes_buffer)
XKeyEvent *event_struct;
KeySym keysym;
unsigned char *buffer_return;
int bytes_buffer;
{
  int functionalChar = 0;

  switch ((int)keysym)
    {
    case XK_KP_F1:
    case XK_KP_F2:
    case XK_KP_F3:
    case XK_KP_F4:
      *buffer_return = 	IROHA_KEY_PF1 + (int)keysym - (int)XK_KP_F1;
      functionalChar = 1;
      break;
    case XK_F1:
    case XK_F2:
    case XK_F3:
    case XK_F4:
    case XK_F5:
    case XK_F6:
    case XK_F7:
    case XK_F8:
    case XK_F9:
    case XK_F10:
    case XK_F11:
    case XK_F12:
    case XK_F13:
    case XK_F14:
    case XK_F15:
    case XK_F16:
      *buffer_return = IROHA_KEY_F1 + (int)keysym - (int)XK_F1;
      functionalChar = 1;
      break;
    case XK_Insert:
      *buffer_return = IROHA_KEY_Insert;
      functionalChar = 1;
      break;
    case XK_Prior:
      *buffer_return = IROHA_KEY_Rollup;
      functionalChar = 1;
      break;
    case XK_Next:
      *buffer_return = IROHA_KEY_Rolldown;
      functionalChar = 1;
      break;
    case XK_Muhenkan:
      if (event_struct->state & 4 /* control-shifted */)
	*buffer_return = IROHA_KEY_Cntrl_Nfer;
      else if (event_struct->state & 1 /* shifted */)
	*buffer_return = IROHA_KEY_Shift_Nfer;
      else
	*buffer_return = IROHA_KEY_Nfer;
      functionalChar = 1;
      break;
    case XK_Kanji:
      if (event_struct->state & 4 /* control-shifted */)
	*buffer_return = IROHA_KEY_Cntrl_Xfer;
      else if (event_struct->state & 1 /* shifted */)
	*buffer_return = IROHA_KEY_Shift_Xfer;
      else
	*buffer_return = IROHA_KEY_Xfer;
      functionalChar = 1;
      break;
    case XK_Up:
      if (event_struct->state & 4 /* control-shifted */)
	*buffer_return = IROHA_KEY_Cntrl_Up;
      else if (event_struct->state & 1 /* shifted */)
	*buffer_return = IROHA_KEY_Shift_Up;
      else
	*buffer_return = IROHA_KEY_Up;
      functionalChar = 1;
      break;
    case XK_Down:
      if (event_struct->state & 4 /* control-shifted */)
	*buffer_return = IROHA_KEY_Cntrl_Down;
      else if (event_struct->state & 1 /* shifted */)
	*buffer_return = IROHA_KEY_Shift_Down;
      else
	*buffer_return = IROHA_KEY_Down;
      functionalChar = 1;
      break;
    case XK_Right:
      if (event_struct->state & 4 /* control-shifted */)
	*buffer_return = IROHA_KEY_Cntrl_Right;
      else if (event_struct->state & 1 /* shifted */)
	*buffer_return = IROHA_KEY_Shift_Right;
      else
	*buffer_return = IROHA_KEY_Right;
      functionalChar = 1;
      break;
    case XK_Left:
      if (event_struct->state & 4 /* control-shifted */)
	*buffer_return = IROHA_KEY_Cntrl_Left;
      else if (event_struct->state & 1 /* shifted */)
	*buffer_return = IROHA_KEY_Shift_Left;
      else
	*buffer_return = IROHA_KEY_Left;
      functionalChar = 1;
      break;
    case XK_Help:
      *buffer_return = IROHA_KEY_Help;
      functionalChar = 1;
      break;
    case XK_Home:
      *buffer_return = IROHA_KEY_Home;
      functionalChar = 1;
      break;
    }
  return functionalChar;
}

#ifdef KC_SETLISTCALLBACK

static void
moveSelection(obj, dir)
IrohaObject obj;
int dir;
{
    ICSelectionControlArg arg;

    arg.command = ICSelectionMove;
    arg.u.dir = dir;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);
}

static int
insertSelection(obj, selected)
IrohaObject obj;
int selected;
{
    obj->iroha.curcand = selected;
    *(obj->iroha.cur_addr) = selected;

    return 0;
}

static int
endSelection(obj, abort)
IrohaObject obj;
int abort;
{
    ICSelectionControlArg arg;
    int selected;
    int ret = 0;

    if (ignoreListfunc) return; /* SelectItem で処理されるのでここは不要 */
    arg.command = ICSelectionEnd;
    arg.u.current_item = -1;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);

    if (!abort && (selected = arg.u.current_item) >= 0) {
	ret = insertSelection(obj, selected);
    }

    return ret;
}

static void
openSelection(obj, func, curitem)
IrohaObject obj;
int func; /* 一番上を見よ */
int curitem;
{
  ICSelectionControlArg arg;
  static int current = 0;
  static int doit = 0;

  if (func == SELECTION_SET) {
    current = curitem;
    doit = 1;
  }
  else if (func == SELECTION_DO && doit) {
    doit = 0;
    arg.command = ICSelectionStart;
    arg.u.selection_kind = ICSelectionCandidates;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);

    /* set current item */
    arg.command = ICSelectionSet;
    arg.u.current_item = current;
    XtCallCallbackList((Widget)obj, obj->inputConv.selectioncallback,
		       (XtPointer)&arg);
  }
}

static void getAllCandidates();

/* listfunc -- 候補一覧モードになった時に呼び出される関数
 *
 * obj      : KC_SETLISTCALLBACK で設定したクライアントデータ
 * func     : 候補一覧機能の固有の機能を表すＩＤ
 * items    : Start の時には候補一覧の全アイテムが入る
 * nitems   :      〃       全アイテムの数が入る
 * cur_item :      〃       カレントアイテム番号を格納するアドレスが入る
 */

static void
listfunc(obj, func, items, nitems, cur_item)
IrohaObject obj;
int  func;
unsigned char **items;
int nitems, *cur_item;
{
  ICSelectionControlArg arg;
  int i;
  unsigned char **p;

  switch (func) {
  case CANNA_LIST_Start:
    getAllCandidates(obj, nitems, items);
    obj->iroha.numcand = nitems;
    obj->iroha.curcand = *cur_item;
    obj->iroha.cur_addr = cur_item;
    openSelection(obj, SELECTION_SET, *cur_item);
    break;
  case CANNA_LIST_Select:
    endSelection(obj, False);
    break;
  case CANNA_LIST_Quit:
    endSelection(obj, True);
    break;
  case CANNA_LIST_Forward:
    moveSelection(obj, ICMoveRight);
    break;
  case CANNA_LIST_Backward:
    moveSelection(obj, ICMoveLeft);
    break;
  case CANNA_LIST_Next:
    moveSelection(obj, ICMoveDown);
    break;
  case CANNA_LIST_Prev:
    moveSelection(obj, ICMoveUp);
    break;
  case CANNA_LIST_BeginningOfLine:
    moveSelection(obj, ICMoveLeftMost);
    break;
  case CANNA_LIST_EndOfLine:
    moveSelection(obj, ICMoveRightMost);
    break;
  }
}

static void
allocCandlist(obj, n)
IrohaObject obj;
int n;
{
    ICString *p;
    int i;

    if (n <= obj->iroha.candlistsize) return;

    if (obj->iroha.candlistsize == 0) {
	p = (ICString *)XtMalloc(n * sizeof(ICString));
    } else {
	p = (ICString *)XtRealloc((char *)obj->iroha.candlist,
				  n * sizeof(ICString));
    }
    for (i = obj->iroha.candlistsize ; i < n ; i++) {
      p[i].nbytes = p[i].nchars = (unsigned short)0;
      p[i].data = (char *)0;
      p[i].attr = 0;
    }

    obj->iroha.candlist = p;
    obj->iroha.candlistsize = n;
}

static void
getAllCandidates(obj, ncand, items)
IrohaObject obj;
int ncand;
unsigned char **items;
{
    ICString *strp;
    unsigned char **p;
    int i, bytes, chars;

    allocCandlist(obj, ncand);

    for (i = 0, strp = obj->iroha.candlist, p = items ;
	 i < ncand; i++, strp++, p++) {

        bytes = strp->nbytes;
	chars = strp->nchars;
        copyInWchar(*p, strlen(*p), (wchar **)&(strp->data), &bytes, &chars);
	strp->nbytes = bytes;
	strp->nchars = chars;
	strp->attr = ICAttrNormalString;
    }
}
#endif /* KC_SETLISTCALLBACK */
