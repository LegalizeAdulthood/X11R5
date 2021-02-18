/* $Header: Sj3P.h,v 1.1 91/09/28 10:23:57 nao Locked $ */

#ifndef _Sj3P_h
#define _Sj3P_h

#include "InputConvP.h"

#include "WStr.h"
#include "sj3def.h"
#include "Sj3.h"
#include "sj3func.h"
#ifdef NEWS_OS4
#include    <sj3lib.h>
#else
#include    "sj3lib.h"
#endif

typedef struct {
    int foo;
} Sj3ClassPart;

typedef struct _Sj3ClassRec {
    ObjectClassPart     object_class;
    InputConvClassPart  inputConv_class;
    Sj3ClassPart        sj3_class;
} Sj3ClassRec;

typedef enum {
    normal_state,
    selection_state,  /* 文節候補選択モード */
    symbol_state    /* 記号入力モード */
} Sj3State;

typedef struct {
    /* resources */
    String      sj3serv;
    String      sj3serv2;
    String      sj3user;
    String      rcfile;
    String      rkfile;
    String      hkfile;
    String      zhfile;
    String      sbfile;
    String      home;
    String      sj3locale;
    int         usr_code;
    /* private data */
    sj3Buf      sj3buf;
    Sj3State    state;      /* 変換の内部状態 */
    ICString    *symbollist;
    int         numsymbols;
    int         cursymbol;  /* 記号選択モードの時、現在選択されている記号 */
    wchar       *sbdata;
    int         sbdatasize;
    ICString    *candlist;
    int         candlistsize;
    wchar       *strdata;
    int         strdatasize;
    int         numcand;    /* 選択モードの時、候補数 */
    int         curcand;    /* 候補選択モードの時、現在選択されている候補 */
    Boolean     selectionending;    /* 選択モードを終了しようとしているか */
} Sj3Part;

typedef struct _Sj3Rec {
    ObjectPart          object;
    InputConvPart       inputConv;
    Sj3Part             sj3;
} Sj3Rec;

#endif
