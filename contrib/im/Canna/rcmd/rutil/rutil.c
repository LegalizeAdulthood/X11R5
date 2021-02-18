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

/*
 * rutil.c
 */
#ifndef lint
static char rcs[] = "@(#) 102.1 $Header: /work/nk.R3.1/rcmd/rutil/RCS/rutil.c,v 1.27 91/05/21 19:55:11 mako Exp $";
#endif
#include <stdio.h>
#include <string.h>
#include "RK.h"
#include "RKintern.h"
#include "RKdic.h"

extern int RkGetWordTextDic();
void Message();
extern char init[];
extern int mode;

RkDefineLine(cx_num, name, line)
int cx_num;
char *name;
char *line;
{
  int linelen = strlen(line);
  int yomilen, yomihinshilen = 0;
  char *buf = (char *)malloc(linelen + 1), *sp, *dp;
  int res = 0;

  
  if ( !buf ) {
    return -1;
  }
  sp = line;
  while (*sp == ' ')
    sp++; /* 空白の読み飛ばし */

  if (!*sp || *sp == '#') /* コメント行 */
    goto endDefineLine;

  dp = buf;
  while (*sp && *sp != ' ') { /* 読みのとりだし */
    *dp++ = *sp++;
  }
  *dp++ = ' ';
  yomilen = dp - buf;

  while (*sp) {
    while (*sp == ' ')
      sp++; /* 空白の読み飛ばし */

    if (*sp) {
      if (*sp == '#') {
	dp = buf + yomilen;
	while (*sp && *sp != ' ') { /* 品詞と頻度のコピー */
	  *dp++ = *sp++;
	}
	*dp++ = ' ';
	yomihinshilen = dp - buf;

	while (*sp == ' ')
	  sp++; /* 空白の読み飛ばし */
      }
      if (yomihinshilen == 0) { /* ここまで品詞情報が出てこなかった。 */
	goto endDefineLine;
      }
      else if (*sp) { /* 候補があるのなら */
	dp = buf + yomihinshilen;
	while (*sp && *sp != ' ') { /* 候補のコピー */
	  *dp++ = *sp++;
	}
	*dp++ = '\0'; /* ヌル文字を最後に入れる */
	res = RkDefineDic(cx_num, name, buf);
	if (res < 0) {
	  goto endDefineLine;
	}
      }
    }
  }
 endDefineLine:
  (void)free(buf);
  return res;
}

#ifdef TEST_DEFINEDIC
RkDefineDic(cx_num, name, word)
int cx_num;
char *name;
char *word;
{
  printf("☆単語の定義(辞書:%s) \"%s\"\n", name, word);
  return 0;
}

main()
{
  char buf[2048], *p;
  int c;

  p = buf;
  c = getchar();
  while (c >= 0) {
    if (c == '\n') {
      *p++ = '\0';
      RkDefineLine(0, "tempdic", buf);
      p = buf;
    }
    else {
      *p++ = c;
    }
    c = getchar();
  }
}
#endif /* TEST_DEFINEDIC */

CopyDic(cx_num, dirname, dicname1, dicname2)
int            cx_num;
unsigned char  *dirname;
unsigned char  *dicname1;
unsigned char  *dicname2;
{
  register int i = 0;
  int ret;
  unsigned char buf[BUFLEN];
  unsigned char dic1_bk[BUFLEN];

  (void)strcpy((char *)dic1_bk, (char *)dicname1);

  do { /* RkGetWordTextDicの返り値が正の間ループする */
    if((ret = RkGetWordTextDic(cx_num, dirname, dic1_bk, buf, BUFLEN)) >= 0) {
      if(!ret)
	break;
      if(dic1_bk[0] != '\0') { /* １回目はマウントする */
	if(RkMountDic(cx_num, dicname2, 0)) {
	  
#ifdef DEBUG
	  (void)fprintf(stderr, "\nmode=%d\n", mode);
#endif
	  /* マウントに失敗した時の処理 */
	  if ((mode & KYOUSEI) == KYOUSEI) {
	    RkCreateDic(cx_num, dicname2, mode);
	  } else {
	    (void)rmDictionary(cx_num, (char *)dicname2);
	  }
	  (void)fprintf(stderr, "辞書\"%s\"のマウントに失敗しました。\n",
					dicname2);
	  return -1;
	}
      }
      /* １行登録する */
      if (RkDefineLine(cx_num, (char *)dicname2, (char *)buf) == -1) {
	(void)fprintf(stderr,"write error: \"%s\"\n", buf);
	continue;
      }
      (void)strcpy((char *)buf,"");
      (void)strcpy((char *)dic1_bk, "");
      i++;
      if ((i & 0x0f) == 0) {
	(void)fputs (".", stderr);
      }
    }
    else { /* RkGetWordTextDicの返り値が負の時 */
      RkUnmountDic(cx_num,dicname2);
      (void)fprintf(stderr,"\n");
      PrintMessage(ret, dic1_bk);
      if ((mode & KYOUSEI) == KYOUSEI) {
	RkCreateDic(cx_num, dicname2, mode);
      } else {
	(void)rmDictionary(cx_num, (char *)dicname2);
      }
      return -1; 
    }
  } while(ret >= 0);
  RkUnmountDic(cx_num,dicname2);
  return 0; 
}

PrintMessage(ret, dicname)
int            ret;
unsigned char  *dicname;
{
  switch (ret) {
  case 0 :
    break;
  case NOENT :
    (void)fprintf(stderr, "辞書\"%s\"が存在しません。\n", dicname);
    break;
  case NOTALC :
    (void)fprintf(stderr, "領域が確保できません。\n");
    break;
  case BADF :
    (void)fprintf(stderr, "指定された辞書\"%s\"はバイナリ辞書です。\n",
		  dicname);
    break;
  case BADDR :
    (void)fprintf(stderr, "dics.dirが異常です。\n",
		  dicname);
    break;
  case NOMOUNT :
    (void)fprintf(stderr, "辞書\"%s\"がマウントできません。\n", dicname);
    break;
  default:
    (void)fprintf(stderr, "かな漢字変換サーバ");
    if (init[0] != '/') {
	(void)fprintf(stderr, "\"%s\"", init);
    }
    (void)fprintf(stderr, "が辞書メンテナンスをサポートしていません\n");
    break;
  }
}

makeDictionary(cn, dicname)
int cn;
unsigned char *dicname;
{
  char ans[20];
  int ret = 0;

  switch ( RkCreateDic( cn, dicname, mode) ) {
  case 0 :
    (void)fprintf(stderr, "新しい辞書\"%s\"を作成します。\n", dicname);
    (void)fprintf(stderr, "カスタマイズファイルを書き換えて下さい。");
    ret = 0;
    break;
  case 1 :
    (void)fprintf(stderr, "辞書\"%s\"を上書きします。", dicname);
    ret = 0;
    break;
  case NOTALC :
    (void)fprintf(stderr, "メモリを確保できません。\n");
    ret = -1;
    break;
  case BADF :
    (void)fprintf(stderr, "辞書\"%s\"はバイナリ辞書です。上書きできません。\n",
		  dicname);
    ret = -1;
    break;
  case BADDR :
    (void)fprintf(stderr,
		  "dics.dir が異常です。辞書ファイルが作成できません。\n");
    ret = -1;
    break;
  case ACCES :
    (void)fprintf(stderr, "辞書ファイルが作成できません。\n");
    ret = -1;
    break;
  case EXIST :
   /*
    * check 'stdin' for uploaddic.
    */
    if (isatty(fileno(stdin)) != 0) {
	(void)fprintf(stderr,
		"指定された辞書\"%s\"はすでに存在します上書きしますか？(y/n)",
		      dicname);
	(void)gets(ans);
    } else {
	(void)fprintf(stderr,
		"指定された辞書\"%s\"はすでに存在します。",
		      dicname);
	(void)strcpy(ans,"n");
    }
    if ( ans[0] == 'y' ) {
	mode |= KYOUSEI;
	if((ret = makeDictionary(cn, dicname, mode)) != 0) {
#ifdef DEBUG
	    fprintf(stderr, "makeDictionary: cannot create %s by mode %d\n",
		    dicname, mode);
#endif
	}
    } else {
	(void)fprintf(stderr, "辞書\"%s\"は作成しません。\n", dicname);
	ret = -1;
    }
    break;
  case INVAL :
    (void)fprintf(stderr,
		  "既存の辞書と辞書\"%s\"の種類が違うので上書きできません。\n",
		  dicname);
    ret = -1;
    break;
  case MOUNT :
  case TXTBSY :
    (void)fprintf(stderr,
		  "辞書\"%s\"が使用中なので上書きできません。\n",
		  dicname);
    ret = -1;
    break;
  case BADARG :
    (void)fprintf(stderr,"モードの値が異常です。\n");
    ret = -1;
    break;
  case BADCONT :
    (void)fprintf(stderr,"コンテクストの値が異常です。\n");
    ret = -1;
    break;
  default:
    (void)fprintf(stderr, "かな漢字変換サーバ");
    if(init[0] != '/') {
      (void)fprintf(stderr, "\"%s\"", init);
    }
    (void)fprintf(stderr, "が辞書メンテナンスをサポートしていません\n");
    exit(ERR_VALUE);
    break;
  }
  return ret;
}

int
rmDictionary(cn, dicname)
int cn;
char *dicname;
{
  int ret = 0;
#ifdef DEBUG
  (void)fprintf(stderr,"RkRemoveDic(cn=%d,dicname=%s)\n", cn, dicname);
#endif
  switch (RkRemoveDic(cn, dicname)) {
  case 0:
    (void)fprintf(stderr, "辞書\"%s\"を削除します。\n", dicname );
    ret = 0;
    break;
  case NOENT :
    (void)fprintf(stderr, "辞書\"%s\"は存在しません。\n", dicname );
    ret = 0;
    break;
  case BADF :
    (void)fprintf(stderr,"辞書\"%s\"はバイナリ辞書です。削除できません。\n",
		  dicname);
    ret = 0;
    break;
  case ACCES :
    (void)fprintf(stderr,"辞書\"%s\"を削除出来ません。\n", dicname );
    ret = 0;
    break;
  case MOUNT:
  case TXTBSY:
    (void)fprintf(stderr,"辞書\"%s\"は使用中なので削除できません。\n",
		  dicname );
    ret = 0;
    break;
  case BADCONT :
    (void)fprintf(stderr,"コンテクストの値が異常です。\n");
    ret = -1;
    break;
  default:
    (void)fprintf(stderr, "かな漢字変換サーバ");
    if (init[0] != '/') {
	(void)fprintf(stderr, "\"%s\"", init);
    }
    (void)fprintf(stderr, "が辞書メンテナンスをサポートしていません\n");
    ret = -1;
    break;
  }
  return ret;
}

void
Message(fmt, a, b, c)
  char *fmt;
{
    char msg[RECSZ*4];
    (void)sprintf(msg, fmt, a, b, c);
    (void)fprintf(stderr,"%s\n", msg);
    (void)fflush(stderr);
}
