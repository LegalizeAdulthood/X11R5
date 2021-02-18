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
    selection_state,  /* $BJ8@a8uJdA*Br%b!<%I(J */
    symbol_state    /* $B5-9fF~NO%b!<%I(J */
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
    Sj3State    state;      /* $BJQ49$NFbIt>uBV(J */
    ICString    *symbollist;
    int         numsymbols;
    int         cursymbol;  /* $B5-9fA*Br%b!<%I$N;~!"8=:_A*Br$5$l$F$$$k5-9f(J */
    wchar       *sbdata;
    int         sbdatasize;
    ICString    *candlist;
    int         candlistsize;
    wchar       *strdata;
    int         strdatasize;
    int         numcand;    /* $BA*Br%b!<%I$N;~!"8uJd?t(J */
    int         curcand;    /* $B8uJdA*Br%b!<%I$N;~!"8=:_A*Br$5$l$F$$$k8uJd(J */
    Boolean     selectionending;    /* $BA*Br%b!<%I$r=*N;$7$h$&$H$7$F$$$k$+(J */
} Sj3Part;

typedef struct _Sj3Rec {
    ObjectPart          object;
    InputConvPart       inputConv;
    Sj3Part             sj3;
} Sj3Rec;

#endif
