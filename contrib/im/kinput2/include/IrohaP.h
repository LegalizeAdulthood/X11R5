/* $Id: IrohaP.h,v 1.11 91/09/04 17:53:17 kon Exp $ */
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
 * Author: Akira Kon, NEC Corporation. (kon@d1.bs2.mt.nec.co.jp)
 */


#ifndef _IrohaP_h
#define _IrohaP_h

#include "InputConvP.h"

#include "WStr.h"
#include "Iroha.h"

typedef struct {
    int foo;
} IrohaClassPart;

typedef struct _IrohaClassRec {
    ObjectClassPart object_class;
    InputConvClassPart inputConv_class;
    IrohaClassPart iroha_class;
} IrohaClassRec;

#define NConvertedSegments 3

typedef struct {
  wchar *str[NConvertedSegments + 3];
  int size[NConvertedSegments + 3];
  int len[NConvertedSegments + 3];
  int offset;
  int curseg;
  int nseg;
  int candstat;	/* 候補一覧行の状態。以下を見よ */
  ICString ics[3];
  wchar *gline[3];
  int glsize[3], gllen[3];
  int curgseg, ngseg;
  wchar *curmode;
  int modesize, modelen;
} iBuf;

#define IROHA_GLINE_Empty	0
#define IROHA_GLINE_Start	1
#define IROHA_GLINE_End		2
#define IROHA_GLINE_Change	3

typedef struct {
    /* resources */
    String	irohahost;
    String	irohafile;
    /* private data */
    iBuf	*ibuf;
    Boolean	textchanged;	/* 変換テキストが変わったか */
    Boolean	selectionending;/* 選択モードを終了しようとしているか */
    ICString	*symbollist;
    int		numsymbols;
    int		cursymbol;	/* 記号選択時、現在選択されている記号 */
    ICString	*candlist;
    int		candlistsize;
    int		numcand;	/* 選択モードの時、候補数 */
    int		curcand;	/* 候補選択時、現在選択されている候補 */
    int		*cur_addr;	/* 候補選択時候補番号を入れるアドレス */
    Boolean	lastTextLengthIsZero;
} IrohaPart;

typedef struct _IrohaRec {
    ObjectPart  object;
    InputConvPart inputConv;
    IrohaPart iroha;
} IrohaRec;

#endif
