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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/kakutei.c,v 3.2 91/07/19 22:38:56 kon Exp $";
     
#include "iroha.h"

static int kakuteiKanaInsert();

/***********************************************************
 * 各 確定入力モード 共通	(map から呼ばれるもの)	   *
 ***********************************************************/

#define  doubleByteP(x) ((x) & 0x80)

#define  SENTOU        1
#define  HENKANSUMI    2
#define  WARIKOMIMOJI  4
#define  SHIRIKIRE     8
#define  ATAMAKIRE    16

#define  kanaReplace(where, insert, insertlen, mask) \
generalReplace(yc->kana_buffer, yc->kAttr, \
&yc->kRStartp, &yc->kCurs, &yc->kEndp,\
where, insert, insertlen, mask)

#define  romajiReplace(where, insert, insertlen, mask) \
generalReplace(yc->romaji_buffer, yc->rAttr, \
&yc->rStartp, &yc->rCurs, &yc->rEndp,\
where, insert, insertlen, mask)

/*
 * 確定入力  カーソルの左の文字の削除
 */
static
KakuteiDeletePrevious(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  if(yc->rCurs) { /* 未確定ローマ字がある */
    int i;

#ifdef DEBUG
    if (iroha_debug) {
      printf("KakuteiDeleteP() 未確定ローマ字がある\n");
    }
#endif

    /* 未確定ローマ字の最後の１文字を削除する */

    romajiReplace (doubleByteP(yc->romaji_buffer[yc->rCurs - 1]) ? -2 : -1,
		   NULL, 0, 0);
		    
    /* kanji_status_return を作る */
    d->kanji_status_return->echoStr = yc->romaji_buffer;
    d->kanji_status_return->length  = yc->rCurs;
    d->kanji_status_return->revLen  = 0;

    if (yc->kCurs > 0) {
      for (i = yc->kCurs - 1 ; i > 0 && !(yc->kAttr[i] & SENTOU); i--)
	;
      kanaReplace (i - yc->kCurs, NULL, 0, 0);
    }
    else {
      d->kanji_status_return->info |= KanjiEmptyInfo;
    }
    
    return(0);
  }
  
  /* バックスペースキーがそのまま渡る */
  d->kanji_status_return->info |= KanjiThroughInfo | KanjiEmptyInfo;
  return(d->nbytes);
}

/*
 * 未確定ローマ字の取り消し
 */
static
KakuteiRomQuit(d)
uiContext d;
{
  RomajiClearYomi(d);
  
  d->kanji_status_return->length = 0;
  d->kanji_status_return->revLen = 0;
  d->kanji_status_return->info |= KanjiEmptyInfo;

  return(0);
}

/* 
 * 次の一字がどのような文字でもスルーで通す
 */
static
KakuteiQuotedInsert(d)
uiContext d;
{
  /* どうしたら良いのでしょうか？ */
  return(0);
}
  
/*
 * 確定入力モードの入力
 */
static
KakuteiEmptySelfInsert(d)
uiContext d;
{
  yomiContext yc = (yomiContext)d->modec;

  if(!yc->rCurs) {
    d->kanji_status_return->info |= KanjiThroughInfo | KanjiEmptyInfo;
    return d->nbytes;
  }
#ifdef DEBUG
  if (iroha_debug) {
    fprintf(stderr, "KakuteiEmptySelfInsert なにもしない\n");
  }
#endif
  d->kanji_status_return->length = -1;
  
  return(0);
}

/***********************************************************
 * 各 確定入力モード 共通	(内部から呼ばれるもの)	   *
 ***********************************************************/

/*
 * 確定入力モードのローマ字変換
 *
 * 引き数	uiContext
 *		flag	半角カタカナ確定入力モードから呼ばれた HANKATA
 *			全角ひらがな確定入力モードから呼ばれた ZENHIRA
 *			全角カタカナ確定入力モードから呼ばれた ZENKATA
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
kakuteiInsert(d, flag)
uiContext d;
int         flag;
{
  yomiContext yc = (yomiContext)d->modec;
  unsigned char char1_buffer[10];		/* 確定入力用のバッファ */
  unsigned char char2_buffer[10];		/* 確定入力用のバッファ */
  int len, rlen, total_len = 0;
  int i, j;
  int preConvertedStrings = 0;

  if(d->ch > 127)
    return(kakuteiKanaInsert(d, flag));

  if (yc->rCurs && (yc->romaji_buffer[yc->rCurs - 1] & 0x80)) {
    /* カナがローマ字バッファに入っている。 */
    switch (flag)
      {
      case HANKATA:
	strncpy(d->buffer_return, yc->romaji_buffer, yc->rCurs);
	preConvertedStrings = yc->rCurs;
	break;
      case ZENKATA:
	preConvertedStrings = 
	  RkCvtZen(d->buffer_return, d->bytes_buffer, 
		   yc->romaji_buffer, yc->rCurs);
	break;
      case ZENHIRA:
	preConvertedStrings = 
	  RkCvtZen(char1_buffer, 10, yc->romaji_buffer, yc->rCurs);
	preConvertedStrings =
	  RkCvtHira(d->buffer_return, d->bytes_buffer, 
		    char1_buffer, preConvertedStrings);
	break;
      }
    romajiReplace(-yc->rCurs, NULL, 0, 0);
  }

  yc->romaji_buffer[yc->rCurs++] = d->ch;
  yc->romaji_buffer[yc->rCurs]   = (unsigned char)NULL;

  if (romajidic != NULL) {
    rlen = RkMapRoma(romajidic, yc->kana_buffer, 1024,
		     yc->romaji_buffer, yc->rCurs,
		     RK_SOKON | RK_XFER, &len);
  }
  else {
    len = -(rlen = yc->rCurs);
    strncpy(yc->kana_buffer, yc->romaji_buffer, rlen);
  }

#ifdef DEBUG
  if (iroha_debug) {
    char aho[1024];
    
    strncpy(aho, yc->romaji_buffer, yc->rCurs);
    aho[yc->rCurs] = 0;
    fprintf(stderr, "romaji(%s)のうちの%d文字が、", aho, rlen);
    strncpy(aho, yc->kana_buffer, (len < 0) ? -len : len);
    aho[(len < 0) ? -len : len] = 0;
    fprintf(stderr, "kana(%s)%d文字に変換された\n", aho, len);
  }
#endif /* DEBUG */

  if(len < 0) {
    len = -len;
  }

  /* ローマ字変換に成功したら */
  while (rlen) {
    /* ローマ字に変換された部分を取り除く */
    for (i = 0, j = rlen ; j < yc->rCurs ; i++, j++)
      yc->romaji_buffer[i] = yc->romaji_buffer[j];
    yc->romaji_buffer[i] = (unsigned char)NULL;
    yc->rCurs = i;
    /* 変換されたかなを適宜変換する */
    if((flag == HANKATA) ||     /* 半角カタカナ */
       (flag == ZENKATA)) { /* 全角カタカナ */
      /* ひらがな → カタカナ */
      len = RkCvtKana(char1_buffer, 10, yc->kana_buffer + total_len, len);
      strncpy(yc->kana_buffer + total_len, char1_buffer, len);
      if(flag == HANKATA) {
	/* 全角 → 半角 */
	len = RkCvtHan(char1_buffer, 10, yc->kana_buffer + total_len, len);
	strncpy(yc->kana_buffer + total_len, char1_buffer, len);
      }
    }
    total_len += len;
    if (romajidic != NULL) {
      rlen = 
	RkMapRoma(romajidic, yc->kana_buffer + total_len, 1024 - total_len,
		       yc->romaji_buffer, yc->rCurs, RK_SOKON | RK_XFER, &len);
    }
    else {
      len = -(rlen = yc->rCurs);
      strncpy(yc->kana_buffer, yc->romaji_buffer, rlen);
    }
#ifdef DEBUG
    if (iroha_debug) {
      char aho[1024];

      strncpy(aho, yc->romaji_buffer, yc->rCurs);
      aho[yc->rCurs] = 0;
      fprintf(stderr, "romaji(%s)のうちの%d文字が、", aho, rlen);
      strncpy(aho, yc->kana_buffer + total_len, (len < 0) ? -len : len);
      aho[(len < 0) ? -len : len] = 0;
      fprintf(stderr, "kana(%s)%d文字に変換された\n", aho, len);
    }
#endif /* DEBUG */
    if (len < 0) 
      len = -len;
  }
  yc->kana_buffer[total_len] = (unsigned char)NULL;
  
  /* kanji_status_return を作る */
  d->kanji_status_return->echoStr = yc->romaji_buffer;
  d->kanji_status_return->length = yc->rCurs;
  d->kanji_status_return->revLen = 0;
  if (yc->rCurs == 0) {
    d->kanji_status_return->info |= KanjiEmptyInfo;
  }
  strncpy(d->buffer_return + preConvertedStrings, 
	  yc->kana_buffer, total_len);

  /* カナバッファをクリアする。 */
  yc->kRStartp = 0;
  yc->kCurs = 0;
  yc->kEndp = 0;
  yc->kana_buffer[0] = (unsigned char)NULL;
  yc->kAttr[0] = SENTOU;
  
  return(total_len + preConvertedStrings);
}

/*
 * 未確定ローマ字があればそれを確定する
 * なければ確定キーをそのまま返す
 *
 * 引き数	uiContext
 *		flag	半角カタカナ確定入力モードから呼ばれた HANKATA
 *			全角ひらがな確定入力モードから呼ばれた ZENHIRA
 *			全角カタカナ確定入力モードから呼ばれた ZENKATA
 * 戻り値	正常終了時 0	異常終了時 -1
 */
kakuteiKakutei(d, flag)
uiContext d;
int	    flag;
{
  yomiContext yc = (yomiContext)d->modec;
  int n, m, rst, kst;
  unsigned char char1_buffer[10];

  n = m = rst = kst = 0;

  d->kanji_status_return->info |= KanjiEmptyInfo;
  if(yc->rCurs) {  /* 未確定ローマ字がある */
#ifdef DEBUG
    if (iroha_debug) {
      printf("Kakutei() 未確定ローマ字がある\n");
    }
#endif

    /* kanji_status_return を作る */
    d->kanji_status_return->length = 0;
    d->kanji_status_return->revLen = 0;
    if(!(yc->romaji_buffer[0] & 0x80)) { /* ローマ字かな変換 */
      while(rst < yc->rCurs) {
	if (romajidic != NULL) {
	  n = RkMapRoma(romajidic, yc->kana_buffer + kst, 1024, 
			yc->romaji_buffer + rst, yc->rCurs - rst,
			RK_SOKON | RK_FLUSH, &m);
	}
	else {
	  m = -(n = yc->rCurs - rst);
	  strncpy(yc->kana_buffer + kst, yc->romaji_buffer + rst, n);
	}
	if(n < 0) {
		      /* ローマ字をカナに変換し切っていない部分が
	 	      あったら、それを変換してやれ */
	  necKanjiError = "RkMapRoma returns -1";
	  return -1;
	}
	if (m < 0)
	  m = -m;
	rst += n;
	kst += m;
      }

      /* 変換されたかなを適宜変換する */
      if((flag == HANKATA) ||     /* 半角カタカナ */
	 (flag == ZENKATA)) { /* 全角カタカナ */
	/* ひらがな → カタカナ */
	kst = RkCvtKana(char1_buffer, 10, yc->kana_buffer, kst);
	strncpy(yc->kana_buffer, char1_buffer, kst);
	if(flag == HANKATA) {
	  /* 全角 → 半角 */
	  kst = RkCvtHan(char1_buffer, 10, yc->kana_buffer, kst);
	  strncpy(yc->kana_buffer, char1_buffer, kst);
	}
      }
    }
    d->nbytes = strlen(yc->kana_buffer);
    strncpy(d->buffer_return, yc->kana_buffer, d->nbytes);

    RomajiClearYomi(d);
    
    return(d->nbytes);
  }
  
  /* リターンキーがそのまま渡る */
  return(d->nbytes);
}

#define KANAYOMIINSERT_BUFLEN 10
#define DAKUON		0xde
#define HANDAKUON	0xdf

/*
 * カナロックキーが押された状態での入力
 *
 * 引き数	uiContext
 *		flag	半角カタカナ確定入力モードから呼ばれた HANKATA
 *			全角ひらがな確定入力モードから呼ばれた ZENHIRA
 *			全角カタカナ確定入力モードから呼ばれた ZENKATA
 * 戻り値	正常終了時 0	異常終了時 -1
 */
static
kakuteiKanaInsert(d, flag)
uiContext d;
int	    flag;
{
  yomiContext yc = (yomiContext)d->modec;
  static unsigned char kana[] = {' ', ' ', 0x8e, ' '}, *kanap;
  unsigned char buf1[KANAYOMIINSERT_BUFLEN], buf2[KANAYOMIINSERT_BUFLEN];
  int i, j, len, replacelen, dakuten = 0;

  kanap = kana + 2; len = 2; replacelen = 0;
  kana[3] = d->ch;
  romajiReplace(0, kanap, len, 0);
  if (d->ch == DAKUON || d->ch == HANDAKUON) { /* 濁音記号だったりしたら */
    if (yc->rCurs >= 4) { /* 前に文字がある場合には次の処理をする */
      kana[0] = yc->romaji_buffer[yc->rCurs - 4];
      kana[1] = yc->romaji_buffer[yc->rCurs - 3];
      if ((kana[0] & 0x80) && (kana[1] & 0x80)) { /* 前の文字がカナか？ */
	kanap = kana; len = 4; replacelen = -2;
	dakuten = 1;
      }
    }
  }

  if((flag == ZENKATA) || (flag == ZENHIRA)) {
    len = RkCvtZen(buf1, KANAYOMIINSERT_BUFLEN, kanap, len);
    if(flag == ZENHIRA) 
      len = RkCvtHira(buf2, KANAYOMIINSERT_BUFLEN, buf1, len);
    else
      strcpy(buf2, buf1, len);
  } else { /* HANKATA */
    /* そのまま確定文字列として渡す */
    d->nbytes = yc->rCurs;
    strncpy(d->buffer_return, yc->romaji_buffer, d->nbytes);
    
    /* 読みをクリアする */
    RomajiClearYomi(d);

    d->kanji_status_return->info |= KanjiEmptyInfo;
    
    return(d->nbytes);
  }

  if (dakuten && len == 2) { /* うまくいった場合には濁点は先頭ではなくなる */
    yc->rAttr[yc->rCurs - 2] &= ~SENTOU;
  }
  kanaReplace(replacelen, buf2, len, HENKANSUMI);
  yc->kRStartp = yc->kCurs;

  /* 濁点が続く可能性がある文字の処理 (う、か〜と、は〜ほ) */
  if((dakuten == 0) && (yc->romaji_buffer[0] & 0x8e)
     && (((yc->romaji_buffer[1] > 181) && (yc->romaji_buffer[1] < 197)) ||
         ((yc->romaji_buffer[1] > 201) && (yc->romaji_buffer[1] < 207)) ||
	 ((flag == ZENKATA) && (yc->romaji_buffer[1] == 0xb3)))) {
    /* 濁点が続く可能性がある文字の次に濁点が続く可能性がある文字がきた */
    /* 例: かき (「か」を確定して「き」を未確定にする) */
    if(yc->rCurs > 2) {
      if(((yc->romaji_buffer[3] > 181) && (yc->romaji_buffer[3] < 197)) ||
	 ((yc->romaji_buffer[3] > 201) && (yc->romaji_buffer[3] < 207)) ||
	 ((flag == ZENKATA) && (yc->romaji_buffer[3] == 0xb3))) {
	d->nbytes = 2;
	strncpy(d->buffer_return, yc->kana_buffer, d->nbytes);
	for(i=0, j=2; j<yc->rCurs; i++, j++)
	  yc->romaji_buffer[i] = yc->romaji_buffer[j];
	yc->romaji_buffer[i] = (unsigned char)NULL; yc->rCurs = i;
	for(i=0, j=2; j<yc->kCurs; i++, j++)
	  yc->kana_buffer[i] = yc->kana_buffer[j];
	yc->kana_buffer[i] = (unsigned char)NULL; yc->kCurs = i;
	
	d->kanji_status_return->echoStr = yc->kana_buffer;
	d->kanji_status_return->length = yc->kCurs;
	d->kanji_status_return->revLen = 0;

	if (yc->kCurs == 0) {
	  d->kanji_status_return->info |= KanjiEmptyInfo;
	}

	return(d->nbytes);
      } else { 
	/* 濁点が続く可能性がある文字の次に濁点が続かない文字がきた */
	/* 例: かに (「かに」を確定する) */
	goto futsuu;
      }
    }

    d->kanji_status_return->echoStr = yc->kana_buffer;
    d->kanji_status_return->length = yc->kCurs;
    d->kanji_status_return->revLen = 0;
    if (yc->kCurs == 0) {
      d->kanji_status_return->info |= KanjiEmptyInfo;
    }
    
    return(0);
  }

  /* 濁点が続かない文字の処理 */
 futsuu:

  d->nbytes = yc->kCurs;
  strncpy(d->buffer_return, yc->kana_buffer, d->nbytes);

  /* 読みをクリアする */
  RomajiClearYomi(d);

  d->kanji_status_return->info |= KanjiEmptyInfo;

  return(d->nbytes);
}

#undef KANAYOMIINSERT_BUFLEN

/***********************************************************
 * 半角アルファベット確定入力モード			   *
 ***********************************************************/

/*
 * 半角アルファベット確定入力モードの入力
 */
static
KakuteiHanAlphaInsert(d)
uiContext d;
{
  if(d->ch > 127) {
    return(kakuteiKanaInsert(d, HANKATA));
  }

  d->buffer_return[0] = d->ch;
  d->buffer_return[1] = '\0';
  d->kanji_status_return->length = 0;
  d->kanji_status_return->info |= KanjiEmptyInfo;

  return(1);
}

/***********************************************************
 * 半角カタカナ確定入力モード				   *
 ***********************************************************/

/*
 * 半角カタカナ確定入力モードの入力
 */
static
KakuteiHanKataInsert(d)
uiContext d;
{
  return(kakuteiInsert(d, HANKATA));
}

/*
 * 半角カタカナ確定入力モードの確定
 */
static
KakuteiHanKataKakutei(d)
uiContext d;
{
  return(kakuteiKakutei(d, HANKATA));
}

/***********************************************************
 * 全角アルファベット確定入力モード			   *
 ***********************************************************/

/*
 * 全角アルファベット確定入力モードの入力 (ａｉｕｅｏ)
 */
static
KakuteiZenAlphaInsert(d)
uiContext d;
{
  unsigned char char1_buffer[10];		/* 確定入力用のバッファ */
  unsigned char char2_buffer[10];		/* 確定入力用のバッファ */

  if(d->ch > 127)
    return(kakuteiKanaInsert(d, ZENHIRA));

  char1_buffer[0] = d->ch;
  char1_buffer[1] = (unsigned char)NULL;

  /* 全角にして、kanji_status_return を作る */
  if (d->ch == ' ') { /* 空白は例外 */
    strncpy(d->buffer_return, " ", 1);
    d->nbytes = 1;
  }
  else {
    d->nbytes = RkCvtZen(char2_buffer, 10, char1_buffer, 1);
    strncpy(d->buffer_return, char2_buffer, d->nbytes);
  }

  /* エコーしないようにする */
  d->kanji_status_return->length = 0;
  d->kanji_status_return->revLen = 0;

  d->kanji_status_return->info |= KanjiEmptyInfo;

  return(d->nbytes);
}

/***********************************************************
 * 全角ひらがな確定入力モード				   *
 ***********************************************************/

/*
 * 全角ひらがな確定入力モードの入力
 */
static
KakuteiZenHiraInsert(d)
uiContext d;
{
  return(kakuteiInsert(d, ZENHIRA));
}

/*
 * 全角ひらがな確定入力モードの確定
 */
static
KakuteiZenHiraKakutei(d)
uiContext d;
{
  return(kakuteiKakutei(d, ZENHIRA));
}

/***********************************************************
 * 全角カタカナ確定入力モード				   *
 ***********************************************************/

/*
 * 全角カタカナ確定入力モードの入力
 */
static
KakuteiZenKataInsert(d)
uiContext d;
{
  return(kakuteiInsert(d, ZENKATA));
}

/*
 * 全角カタカナ確定入力モードの確定
 */
static
KakuteiZenKataKakutei(d)
uiContext d;
{
  return(kakuteiKakutei(d, ZENKATA));
}

/*
 *  {ひら、カタ}カナ確定入力モードからアルファベットモードへ移行
 */

static
KakuteiZenHiraAlphaMode(d)
uiContext d;
{
  int res = 0;
  yomiContext yc = (yomiContext)d->modec;

  if(yc->rCurs)
    res = KakuteiZenHiraKakutei(d);

  AlphaMode(d);
  return res;
}

static
KakuteiZenKataAlphaMode(d)
uiContext d;
{
  int res = 0;
  yomiContext yc = (yomiContext)d->modec;

  if(yc->rCurs)
    res = KakuteiZenKataKakutei(d);

  AlphaMode(d);
  return res;
}

static
KakuteiHanKataAlphaMode(d)
uiContext d;
{
  int res = 0;
  yomiContext yc = (yomiContext)d->modec;

  if(yc->rCurs)
    res = KakuteiHanKataKakutei(d);

  AlphaMode(d);
  return res;
}

#include	"kakuteimap.c"
