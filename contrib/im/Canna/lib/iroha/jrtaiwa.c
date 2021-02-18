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
 */

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/jrtaiwa.c,v 3.1 91/06/28 18:09:41 kon Exp $";

#ifdef DEBUG
#include	<stdio.h>
#endif
#include	<iroha/jrkanji.h>
#include	<iroha/jptaiwa.h>

#define BUFSIZE 1024

static int initialized = 0;	    /* １回目によばれたか？ */
static int oldmode = 0;	            /* 現在のモード */
static int size;		            /* 候補一覧行表示行きの長さ */
static unsigned char kakutei_buf[BUFSIZE]; /* 確定文字列用バッファ */

static void ClearTaiwa();
static void EchoStrMessage();
static void GlineMessage();

static jrKanjiStatus ks;

extern iroha_debug;

jrTaiwaHenkan(mode, c, taiwa)
int	mode;
int	c;
struct taiwaInf	*taiwa;
{
  int newmode, rv;
  int context_id = 0;

#ifdef DEBUG
  if (iroha_debug) {
    printf( "jrTaiwaHenkanだよ。\n");
  }
#endif
  ClearTaiwa(taiwa);

  switch(mode) {
  case TAIWAAUTO:
#ifdef DEBUG
  if (iroha_debug) {
    printf( "TAIWAAUTO ");
  }
#endif
    if(!initialized) {
#ifdef DEBUG
  if (iroha_debug) {
    printf( "KC_INITIALIZE ");
  }
#endif
      if(jrKanjiControl(context_id, KC_INITIALIZE, 0) == -1)
	return(-1);
    }
    taiwa->kouhoSize = size = 60;
    if(jrKanjiControl(context_id, KC_SETWIDTH, (char *)60) == -1)
      return(-1);
    return(0);
    break;

  case TAIWAINIT:
#ifdef DEBUG
  if (iroha_debug) {
    printf( "TAIWAINIT ");
  }
#endif
    taiwa->kakutei = kakutei_buf;
    /* モードを数字で返してくださいね */
    if(jrKanjiControl(context_id, KC_SETMODEINFOSTYLE, 1) == -1)
      return(-1);
    /* 16進コード変換で、４桁目の数字が入力されたら確定してくださいね */
    if(jrKanjiControl(context_id, KC_SETHEXINPUTSTYLE, 1) == -1)
      return(-1);
    /* 同音語表示域が MIN_KOUHO_SIZE より小さかったら １にする */
    /* その場に候補を出すようにする */
    if(taiwa->kouhoSize != size) {
#ifdef DEBUG
  if (iroha_debug) {
    printf( "KC_SETWIDTH ");
  }
#endif
      if((size = taiwa->kouhoSize) < MIN_KOUHO_SIZE)
	size = 1;
      if(jrKanjiControl(context_id, KC_SETWIDTH, (char *)size) == -1)
	return(-1);
    }
    /* 前と違うモードが設定されていたらモードをかえる */
    if((taiwa->kanaMode) != oldmode) {
      jrKanjiStatusWithValue ksv;
      ksv.val = oldmode = taiwa->kanaMode;
      ksv.ks = &ks;
#ifdef DEBUG
  if (iroha_debug) {
    printf( "KC_CHANGEMODE[newmode(%d)oldmode(%d)] ",
	   taiwa->kanaMode, oldmode);
  }
#endif
      if(jrKanjiControl(context_id, KC_CHANGEMODE, &ksv) == -1)
	return(-1);
    }
    if(!initialized) {
#ifdef DEBUG
  if (iroha_debug) {
    printf( "KC_INITIALIZE ");
  }
#endif
      if(jrKanjiControl(context_id, KC_INITIALIZE, 0) == -1)
	return(-1);
    }
    return(0);
    break;

  case TAIWAEND:
#ifdef DEBUG
  if (iroha_debug) {
    printf( "TAIWAEND ");
  }
#endif
    return(jrKanjiControl(context_id, KC_FINALIZE, 0 ));
    break;

  case DATAIN:
#ifdef DEBUG
  if (iroha_debug) {
    printf( "DATAIN[c= 0x%x]\n", c);
  }
#endif
    rv = jrKanjiString(context_id, (unsigned char)c,
		       kakutei_buf, BUFSIZE, &ks);
#ifdef DEBUG
  if (iroha_debug) {
    satokodeb(&ks);
  }
#endif
    break;

  default:
#ifdef DEBUG
  if (iroha_debug) {
    printf( "jrTaiwaHenkan default\n");
  }
#endif
    return(DIOERR);
    break;
  }

  if(rv == -1) {
#ifdef DEBUG
  if (iroha_debug) {
    printf( "rv == -1 DIOERR\n");
  }
#endif
    return(DIOERR);
  } else if(rv) {
#ifdef DEBUG
  if (iroha_debug) {
    printf( "jrKanjiControlの戻り値が１以上だった rv = 0x%x\n", rv);
  }
#endif
    taiwa->kakuteiLength  = rv;
  }

  if(ks.info & KanjiModeInfo) { /* モードが変わった */
#ifdef DEBUG
    if (iroha_debug) {
      printf("モードが変わった (%d)\n", (ks.mode[0] - '@'));
    }
#endif
    newmode = ks.mode[0] - '@';
    if((newmode == IROHA_MODE_HenkanMode) && (ks.info & KanjiGLineInfo) &&
       (ks.gline.length > 0)) {
      newmode = IROHA_MODE_IchiranMode;
    }

    taiwa->kanaMode = newmode;

    switch(newmode) {
    case IROHA_MODE_AlphaMode : /* アルファベットモード */
      taiwa->subStatus = KANJI_M;
      break;

    case IROHA_MODE_HenkanMode : /* 読み入力がない状態 */
      if((ks.info & KanjiGLineInfo) && (ks.gline.length))
	GlineMessage(taiwa, &ks);
      else
	EchoStrMessage(taiwa, &ks);
      oldmode = newmode;
      switch(oldmode) {
      case IROHA_MODE_IchiranMode :
	taiwa->subStatus = D_END;
	return(DOUON);
	break;
      case IROHA_MODE_HexMode :
	taiwa->subStatus = HEX_TRAN;
	return(HEX_IN);
	break;
      case IROHA_MODE_BushuMode :
	taiwa->subStatus = BU_END;
	return(BUSHU_IN);
	break;
      case IROHA_MODE_KigoMode :
	taiwa->subStatus = KI_END;
	return(KIGOU_IN);
	break;
      case IROHA_MODE_TourokuMode : /* 単語登録の初期状態 */
      case IROHA_MODE_TourokuHinshiMode : /* 単語登録の品詞選択状態 */
      case IROHA_MODE_TourokuDicMode : /* 単語登録の辞書選択状態 */
	taiwa->subStatus = ADD_END;
	return(TAN_TOUROKU_IN);
	break;
      default :
	taiwa->subStatus = HENKAN_M;
	break;
      }
      break;

    case IROHA_MODE_YomiMode : /* 読み入力している状態 */
    case IROHA_MODE_TankouhoMode : /* 単一候補表示状態 */
    case IROHA_MODE_JishuMode : /* 文字種変換している状態 */
      taiwa->kanaMode = oldmode = newmode = IROHA_MODE_HenkanMode;
      if(ks.length == -1)
	return(MUSI);
      EchoStrMessage(taiwa, &ks);
      return(MIDASI);
      break;

    case IROHA_MODE_IchiranMode : /* 候補一覧表示状態 */
      taiwa->kanaMode = oldmode = newmode = IROHA_MODE_HenkanMode;
      if(ks.gline.length == -1)
	return(MUSI);
      taiwa->subStatus = D_HYOUJI;
      GlineMessage(taiwa, &ks);
      return(DOUON);
      break;

    case IROHA_MODE_HanAlphaKakuteiMode : /* 確定入力モード */
    case IROHA_MODE_HanKataKakuteiMode : /* 確定入力モード */
    case IROHA_MODE_ZenAlphaKakuteiMode : /* 確定入力モード */
    case IROHA_MODE_ZenHiraKakuteiMode : /* 確定入力モード */
    case IROHA_MODE_ZenKataKakuteiMode : /* 確定入力モード */
      taiwa->subStatus = newmode + 3;
      /* IROHA_MODE_HanAlphaKakuteiModeだったら、HANALPHAKAKU＿Mにする */
      /* IROHA_MODE_HanAlphaKakuteiModeに 3 たすと、HANALPHAKAKU＿Mになる */
      break;

    case IROHA_MODE_HexMode : /* １６進コード入力モード */
      oldmode = newmode;
      if(ks.gline.length == -1)
	return(MUSI);
      taiwa->subStatus = HEX_M;
      GlineMessage(taiwa, &ks);
      break;

    case IROHA_MODE_BushuMode : /* 部首の読みの入力状態 */
      taiwa->subStatus = BUSHU_M;
      taiwa->kanaMode = oldmode = newmode = IROHA_MODE_BushuMode;
      if(ks.gline.length == -1)
	return(MUSI);
      GlineMessage(taiwa, &ks);
      break;

    case IROHA_MODE_KigoMode : /* 記号一覧表示状態 */
      oldmode = newmode;
      if(ks.gline.length == -1)
	return(MUSI);
      taiwa->subStatus = KIGOU_M;
      GlineMessage(taiwa, &ks);
      break;

    case IROHA_MODE_TourokuMode : /* 単語登録の初期状態 */
      taiwa->kanaMode = oldmode = newmode = IROHA_MODE_TourokuMode;
      if(ks.gline.length == -1)
	return(MUSI);
      taiwa->subStatus = TAN_TOUROKU_M;
      GlineMessage(taiwa, &ks);
      break;

    case IROHA_MODE_TourokuHinshiMode : /* 単語登録の品詞選択状態 */
      taiwa->kanaMode = oldmode = newmode = IROHA_MODE_TourokuHinshiMode;
      if(ks.gline.length == -1)
	return(MUSI);
      taiwa->subStatus = TAN_TOUROKU_HIN_M;
      GlineMessage(taiwa, &ks);
      break;

    case IROHA_MODE_TourokuDicMode : /* 単語登録の辞書選択状態 */
      taiwa->kanaMode = oldmode = newmode = IROHA_MODE_TourokuDicMode;
      if(ks.gline.length == -1)
	return(MUSI);
      taiwa->subStatus = TAN_TOUROKU_DIC_M;
      GlineMessage(taiwa, &ks);
      break;

    default :
      oldmode = newmode;
      return(DIOERR);
      break;
    }

    oldmode = newmode; /* 現在のモードをセーブする */

    return(MODE_CHG);

  } else { /* モードは変わらなかった */
#ifdef DEBUG
  if (iroha_debug) {
    printf( "モードが変わらなかった\n");
  }
#endif

    switch(oldmode) {
    case IROHA_MODE_AlphaMode : /* アルファベットモード */
    case IROHA_MODE_HenkanMode : /* 読み入力がない状態 */
    case IROHA_MODE_YomiMode : /* 読み入力している状態 */
    case IROHA_MODE_TankouhoMode : /* 単一候補表示状態 */
    case IROHA_MODE_JishuMode : /* 文字種変換している状態 */
    case IROHA_MODE_HanAlphaKakuteiMode : /* 確定入力モード */
    case IROHA_MODE_HanKataKakuteiMode : /* 確定入力モード */
    case IROHA_MODE_ZenAlphaKakuteiMode : /* 確定入力モード */
    case IROHA_MODE_ZenHiraKakuteiMode : /* 確定入力モード */
    case IROHA_MODE_ZenKataKakuteiMode : /* 確定入力モード */
#ifdef DEBUG
  if (iroha_debug) {
    printf( "ふつうのもーど (0x%x)\n", (oldmode));
  }
#endif
      if(ks.length == -1)
	return(MUSI);
      if((ks.info & KanjiGLineInfo) && (ks.gline.length))
	GlineMessage(taiwa, &ks);
      else
	EchoStrMessage(taiwa, &ks);
#ifdef DEBUG
  if (iroha_debug) {
    printf( "ふつうのもーど return MIDASI\n");
  }
#endif
      return(MIDASI);
      break;

    case IROHA_MODE_IchiranMode : /* 候補一覧表示状態 */
      if(ks.gline.length == -1)
	return(MUSI);
      taiwa->subStatus = D_HYOUJI;
      GlineMessage(taiwa, &ks);
      return(DOUON);
      break;

    case IROHA_MODE_HexMode : /* １６進コード入力モード */
      if(ks.gline.length == -1)
	return(MUSI);
      taiwa->subStatus = HEX_POS;
      GlineMessage(taiwa, &ks);
      return(HEX_IN);
      break;

    case IROHA_MODE_BushuMode : /* 部首の読みの入力状態 */
      taiwa->subStatus = BU_MIDASI;
      if(ks.gline.length == -1)
	return(MUSI);
      GlineMessage(taiwa, &ks);
      return(BUSHU_IN);
      break;

    case IROHA_MODE_KigoMode : /* 記号一覧表示状態 */
      if(ks.gline.length == -1)
	return(MUSI);
      taiwa->subStatus = KI_KOUHO;
      GlineMessage(taiwa, &ks);
      return(KIGOU_IN);
      break;

    case IROHA_MODE_TourokuMode : /* 単語登録の初期状態 */
    case IROHA_MODE_TourokuHinshiMode : /* 単語登録の品詞選択状態 */
    case IROHA_MODE_TourokuDicMode : /* 単語登録の辞書選択状態 */
      if(ks.gline.length == -1)
	return(MUSI);
      taiwa->subStatus = ADD_INPUT;
      GlineMessage(taiwa, &ks);
      return(TAN_TOUROKU_IN);
      break;

    default :
      return(DIOERR);
      break;
    }
  }
}

static
void
ClearTaiwa(taiwa)
struct taiwaInf	*taiwa;
{

  taiwa->message 		= NULL;
  taiwa->messageLength 		= -1;
/*  taiwa->kakutei 		= NULL; 初期化しない */
  taiwa->kakuteiLength 		= -1;
  taiwa->reversePosition 	= 0;
  taiwa->reverseLength 		= 0;
  taiwa->subStatus 		= 0;
  taiwa->henkanMode 		= 0;
/*  taiwa->kanaMode		= 0; 初期化しない */
  taiwa->romajiMode		= 0;
  taiwa->jisyo			= 0;
/*  taiwa->kouhoSize		= 0; 初期化しない */
  taiwa->jrdic			= NULL;
  taiwa->jgdic			= NULL;
  taiwa->judic			= NULL;
}

static
void
EchoStrMessage(taiwa, ks)
struct taiwaInf	*taiwa;
jrKanjiStatus *ks;
{
  taiwa->message = ks->echoStr;
  taiwa->messageLength = ks->length;
  taiwa->reversePosition = ks->revPos;
  taiwa->reverseLength = ks->revLen;
#ifdef DEBUG
  if(iroha_debug) {
    printf( "☆echo☆Length(%d)Position(%d)pLen(%d)prevStr(%s)\n", 
	   taiwa->messageLength, taiwa->reversePosition,
	   taiwa->reverseLength, taiwa->message);
  }
#endif
}

static
void
GlineMessage(taiwa, ks)
struct taiwaInf	*taiwa;
jrKanjiStatus *ks;
{
  taiwa->message = ks->gline.line;
  taiwa->messageLength = ks->gline.length;
  taiwa->reversePosition = ks->gline.revPos;
  taiwa->reverseLength = ks->gline.revLen;
#ifdef DEBUG
  if(iroha_debug) {
    printf( "☆gline☆Length(%d)Position(%d)pLen(%d)prevStr(%s)\n", 
	   taiwa->messageLength, taiwa->reversePosition,
	   taiwa->reverseLength, taiwa->message);
  }
#endif
}

static
satokodeb(ks)
jrKanjiStatus *ks;
{
  char buf[1024];

  printf("\n(satokodeb)\n");
  if (ks->length > 0) {
    strncpy(buf, ks->echoStr, ks->length);
    buf[ks->length] = '\0';
    printf("ks.echoStr=(%s)", buf);
  }
  printf("echo_len=%d, ", ks->length);
  printf("revPos=%d, ", ks->revPos);
  printf("revLen=%d, ", ks->revLen);
  printf("info=%d, ", ks->info);
  printf("glinelen=%d, ", ks->gline.length);
  if ((ks->info & KanjiGLineInfo) && ks->gline.length > 0) {
    strncpy(buf, ks->gline.line, ks->gline.length);
    buf[ks->gline.length] = '\0';
    printf("\nks.gline.line=(%s)\n", buf);
  }
  if (ks->info & KanjiModeInfo) {
    printf("mode(%d)\n", (ks->mode[0] - '@'));
  }
}
