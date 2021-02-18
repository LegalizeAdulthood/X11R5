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

  このプログラムはエスケープシーケンスで入力される XFER、↑↓→←、F1、
  F2、PF1、PF2、HELP、などのキーを適当なＩＤに変換するためのプログラム
  である。表を準備しておいて initIS を呼べば、あとは identifySequence 
  を使うことによりエスケープシーケンスを認識することができる。

  詳細の使い方は main() を参考にして欲しい。このファイルは単体でコンパ
  イルし実行可能である。a.out は標準入力からエスケープシーケンスを入力
  し、『いろは』で用いるキーＩＤを表示する。単体でコンパイルするときは 
  USED_AS_LIBRARY の定義をコメントアウトすれば良い。

*/

static char rcs_id[] = "$Header: /work/nk.R3.1/misc/RCS/is.c,v 1.5 91/06/07 10:34:18 kon Exp $";

#define USED_AS_LIBRARY

#include <iroha/miscdefs.h>

typedef struct {
  int  id;
  char *seq;
} SeqToID;

static SeqToID generalSeqToID[] = {
  /* pcvt */
#ifdef SHRABETEKARATSUKAU
/* 以下のコードは未確認の為とりあえず追加しない。 */
  {IROHA_KEY_Xfer,	"OM"	}, /* pcvtex xfer 	*/
  {IROHA_KEY_Xfer,	"$@"	}, /* C-xfer 		*/
  {IROHA_KEY_Nfer,	"&Ub1"	}, /* C-nfer		*/
#endif
  {IROHA_KEY_Nfer,	"&U51"	}, /* nfer		*/
  {IROHA_KEY_PF1,	"&U52"	}, /* PF1		*/
  {IROHA_KEY_PF2,	"&U53"	}, /* PF2		*/
  {IROHA_KEY_PF3,	"&U54"	}, /* PF3		*/
  {IROHA_KEY_PF4,	"&U55"	}, /* PF4		*/
  
  {IROHA_KEY_Up,	"[A"	}, /* ↑		*/
  {IROHA_KEY_Down,	"[B"	}, /* ↓		*/
  {IROHA_KEY_Right,	"[C"	}, /* →		*/
  {IROHA_KEY_Left,	"[D"	}, /* ←		*/
  /* ddy880 */
  {IROHA_KEY_Up,	"OA"	}, /* ↑		*/
  {IROHA_KEY_Down,	"OB"	}, /* ↓		*/
  {IROHA_KEY_Right,	"OC"	}, /* →		*/
  {IROHA_KEY_Left,	"OD"	}, /* ←		*/
  {IROHA_KEY_PF1,	"OP"	}, /* PF1		*/
  {IROHA_KEY_PF2,	"OQ"	}, /* PF2		*/
  {IROHA_KEY_PF3,	"OR"	}, /* PF3		*/
  {IROHA_KEY_PF4,	"OS"	}, /* PF4		*/
  {IROHA_KEY_F1,	"[11~"	}, /* F1		*/
  {IROHA_KEY_F2,	"[12~"	}, /* F2		*/
  {IROHA_KEY_F3,	"[13~"	}, /* F3		*/
  {IROHA_KEY_F4,	"[14~"	}, /* F4		*/
  {IROHA_KEY_F5,	"[15~"	}, /* F5		*/
  {IROHA_KEY_F6,	"[17~"	}, /* F6		*/
  {IROHA_KEY_F7,	"[18~"	}, /* F7		*/
  {IROHA_KEY_F8,	"[19~"	}, /* F8		*/
  {IROHA_KEY_F9,	"[20~"	}, /* F9		*/
  {IROHA_KEY_F10,	"[21~"	}, /* F10		*/
  {IROHA_KEY_Nfer,	"[209z"	}, /* nfer		*/
  {IROHA_KEY_Xfer,	"[210z"	}, /* xfer		*/
  {IROHA_KEY_Insert,	"[2~"	}, /* insert		*/
  {IROHA_KEY_Insert,	"[193z"	}, /* insert		*/
  {IROHA_KEY_Help,	"[28~"	}, /* help		*/
  {IROHA_KEY_Help,	"[197z"	}, /* help		*/
  {IROHA_KEY_Rollup,	"[5~"	}, /* Roll Up		*/
  {IROHA_KEY_Rollup,	"[194z"	}, /* Roll Up		*/
  {IROHA_KEY_Rolldown,	"[6~"	}, /* Roll Down		*/
  {IROHA_KEY_Rolldown,	"[195z"	}, /* Roll Down		*/
  {0,			0	},
};

static SeqToID ddySeqToID[] = {
  /* pcvt */
  {IROHA_KEY_Up,	"[A"	}, /* ↑		*/
  {IROHA_KEY_Down,	"[B"	}, /* ↓		*/
  {IROHA_KEY_Right,	"[C"	}, /* →		*/
  {IROHA_KEY_Left,	"[D"	}, /* ←		*/
  /* ddy880 */
  {IROHA_KEY_Up,	"OA"	}, /* ↑		*/
  {IROHA_KEY_Down,	"OB"	}, /* ↓		*/
  {IROHA_KEY_Right,	"OC"	}, /* →		*/
  {IROHA_KEY_Left,	"OD"	}, /* ←		*/
  {IROHA_KEY_PF1,	"[234z"	}, /* PF1		*/
  {IROHA_KEY_PF2,	"[235z"	}, /* PF2		*/
  {IROHA_KEY_PF3,	"[236z"	}, /* PF3		*/
  {IROHA_KEY_PF4,	"[237z"	}, /* PF4		*/
  {IROHA_KEY_F1,	"OP"	}, /* F1		*/
  {IROHA_KEY_F2,	"OQ"	}, /* F2		*/
  {IROHA_KEY_F3,	"OR"	}, /* F3		*/
  {IROHA_KEY_F4,	"OS"	}, /* F4		*/
  {IROHA_KEY_F5,	"[228z"	}, /* F5		*/
  {IROHA_KEY_F6,	"[229z"	}, /* F6		*/
  {IROHA_KEY_F7,	"[230z"	}, /* F7		*/
  {IROHA_KEY_F8,	"[231z"	}, /* F8		*/
  {IROHA_KEY_F9,	"[232z"	}, /* F9		*/
  {IROHA_KEY_F10,	"[233z"	}, /* F10		*/
  {IROHA_KEY_Nfer,	"[209z"	}, /* nfer		*/
  {IROHA_KEY_Xfer,	"[210z"	}, /* xfer		*/
  {IROHA_KEY_Insert,	"[2~"	}, /* insert		*/
  {IROHA_KEY_Insert,	"[193z"	}, /* insert		*/
  {IROHA_KEY_Help,	"[28~"	}, /* help		*/
  {IROHA_KEY_Help,	"[197z"	}, /* help		*/
  {IROHA_KEY_Rollup,	"[5~"	}, /* Roll Up		*/
  {IROHA_KEY_Rollup,	"[194z"	}, /* Roll Up		*/
  {IROHA_KEY_Rolldown,	"[6~"	}, /* Roll Down		*/
  {IROHA_KEY_Rolldown,	"[195z"	}, /* Roll Down		*/
  {0,			0	},
};

static SeqToID *termTbls[3] = {
  generalSeqToID, 
  ddySeqToID,
  generalSeqToID,
};

/* cfuncdef

  initIS -- キーシーケンスを高速にスキャンするためのテーブルを作る

  高速テーブルは seqTbl のポインタとして作成される。また、キャラクタか
  ら数字への変換用のテーブルも作成される(charToNumTbl)。

  これはデータ SeqToID を基に初期化される。

  高速テーブルは状態と入力されたキーのコードから次の状態を得るためのテー
  ブルである。シーケンスが終わった時に、その時の状態で id メンバにその
  シーケンスに対応するキーの識別子が入っている。次の状態が定義されてい
  ない(０が入っている)ときはシーケンスが終わっている。そのときに id メ
  ンバ０が入っている時はイリーガルなシーケンスである。

 */

#define charToNum(c) charToNumTbl[(c) - ' ']

static int charToNumTbl['~' - ' ' + 1];

typedef struct {
  int id;
  int *tbl;
} seqlines;

static seqlines *seqTbl;	/* 内部の表(実際には表の表) */
static int ntbl = 0;		/* 状態の数。状態の数だけ表がある */

static int n = 1;

initIS(terminal)
int terminal;
{
  SeqToID *p;
  char *s;
  int i;
  seqlines seqTbls[1024];

  /* まず何文字使われているかを調べる。
     n は使われている文字数より１大きい値である */
  for (p = termTbls[terminal] ; p->id ; p++) {
    for (s = p->seq ; *s ; s++) {
      if ( !charToNumTbl[*s - ' '] ) {
	charToNumTbl[*s - ' '] = n; /* 各文字にシリアル番号を振る */
	n++;
      }
    }
  }
  seqTbls[ntbl++].tbl = (int *)calloc(n, sizeof(int)); /* 文字数分のテーブル */
  for (p = termTbls[terminal] ; p->id ; p++) {
    int line, nextline;

    line = 0;
    for (s = p->seq ; *s ; s++) {
      if (seqTbls[line].tbl == 0) { /* テーブルがない */
	seqTbls[line].tbl = (int *)calloc(n, sizeof(int));
      }
      nextline = seqTbls[line].tbl[charToNum(*s)];
      /* ちなみに、charToNum(*s) は絶対に０にならない */
      if ( nextline ) {
	line = nextline;
      }
      else { /* 最初にアクセスした */
	line = seqTbls[line].tbl[charToNum(*s)] = ntbl++;
      }
    }
    seqTbls[line].id = p->id;
  }
  seqTbl = (seqlines *)calloc(ntbl, sizeof(seqlines));
  for (i = 0 ; i < ntbl ; i++) {
    seqTbl[i].id  = seqTbls[i].id;
    seqTbl[i].tbl = seqTbls[i].tbl;
  }
}

finIS() /* identifySequence に用いたメモリ資源を開放する */
{
  int i;

  for (i = 0 ; i < ntbl ; i++) {
    if (seqTbl[i].tbl != 0) {
      free(seqTbl[i].tbl);
    }
  }
  free(seqTbl);
}

/* cvariable

  seqline: identifySequence での状態を保持する変数

 */

static int seqline = 0;

initSeqLine() /* 状態をリセットする */
{
  seqline = 0;
}

#define CONTINUE 1
#define END	 0

identifySequence(c, val)
char c;
int *val;
{
  int nextline;

  if (charToNum(c) &&
      (nextline = seqTbl[seqline].tbl[charToNum(c)]) ) {
    seqline = nextline;
    if (*val = seqTbl[seqline].id) {
      seqline = 0;
      return END;
    }
    else {
      return CONTINUE; /* continue */
    }
  }
  else {
    *val = -1;
    seqline = 0;
    return END;
  }
}

#ifndef USED_AS_LIBRARY
main()
{
  int c, code;

  initIS(2); /* 2 は VT コンパチターミナル */

  draw_table();

  c = getchar();

  while (c != -1) {
    if (c == 0x1b) {
      char foo[8];
      int n = 1;
      int res;

      foo[0] = 0x1b;
      do {
	foo[n++] = c = getchar();
	res = identifySequence(c, &code);
      } while (res == CONTINUE);
      if (code != -1) {
	printf("<0x%x>", code);
      }
      else {
	int i;

	for (i = 0 ; i < n ; i++) {
	  printchar(foo[i]);
	}
      }
    }
    else {
      printchar(c);
    }
    c = getchar();
  }
  finIS();
}

static
printchar(c)
char c;
{
  if (c < 0x20) {
    if (c == '\n' || c == '\t' || c == '\b') {
      putchar(c);
    }
    else if (c == 0x1b) {
      printf("\\e");
    }
    else {
      printf("^%c", c + '@');
    }
  }
  else {
    putchar(c);
  }
}

draw_table()
{
  int i, j;

  printf("         ");
  for (i = 1 ; i < n ; i++) {
    for (j = 0 ; j < '~' - ' ' + 1 ; j++) {
      if (charToNumTbl[j] == i) {
	printf("  %c", ' ' + j);
	break;
      }
    }
  }
  printf("\n");
  for (i = 0 ; i < ntbl ; i++) {
    printf("%2d(%02x)", i, seqTbl[i].id);
    if (seqTbl[i].tbl) {
      for (j = 0 ; j < n ; j++) {
	printf(" %2d", seqTbl[i].tbl[j]);
      }
    }
    printf("\n");
  }
}
#endif /* USED_AS_LIBRARY */
