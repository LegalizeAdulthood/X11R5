/*
 * $Id: rk_read.c,v 1.2 1991/09/16 21:34:50 ohm Exp $
 */
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/***********************************************************************
			rk_read.c
						87.12. 9  訂 補

	rk_main.c の部品。変換対応表の読み込みを担当。
***********************************************************************/
/*	Version 3.1	88/06/15	H.HASHIMOTO
 */
#ifndef OMRON_LIB
#include "rk_header.h"
#include "rk_extvars.h"
#ifdef	MULTI
#include "rk_multi.h"
#endif	/*MULTI*/
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include "rext.h"


/* 88/06/13 V3.1 */
#ifndef	MULTI
static	letter	*ltrbufbgn;	/* 使用中のletterのバッファの先頭を指す */
#endif	/*!MULTI*/
static	letter	*ltrbufptr;
static	letter	*term;
static	letter	*dummy;


static	dat	*datptr;	/* V3.1 */
static	letter	*memptr;		/* V3.1 */

static	letter	*hen_iki;		/* V3.1 */

/* エラーメッセージのためのもの */
#ifndef	MULTI
static	char	*tcurdir;	/* 対応表のサーチパス */
static	char	*tcurfnm;	/* 対応表のファイル名 */
static	char	*tcurread;	/* 対応表の現在行bufへのポインタ */
#endif	/*!MULTI*/

/* 88/06/02 V3.1 */
#define bit3(x, y, z) ((x) | ((y) << 1) | ((z) << 2))

/* 組み込み関数の性質を保持するstruct */
struct	funstr {
	char	*fnname;	/* 関数名 */
	char	appear;		/* 入力コード部・出力コード部・バッファ残り部
				   に書けるかどうかのフラグ３ビット */
	char	argnum;		/* 引数の個数 */
	char	fntype;		/* 結果のタイプ */
};

/* 関数の追加にはここと mchevl(), evlis() をいじる */
static	struct	funstr	func[] = {
/*			引数の数（-1…この値未使用）
	  名		    出現     ｜  タイプ…0:文字関数 1:文字列関数
	  ↓		     ↓	     ↓  ↓	 2:特殊関数 3:機能 4:宣言 */
	"defvar",	bit3(1,0,0), -1, 4, /*  0 */
	"include",	bit3(1,0,0), -1, 4,
	"toupper",	bit3(1,1,1),  1, 0,
	"tolower",	bit3(1,1,1),  1, 0,
/*	"error",	bit3(0,1,0),  0, 3,*/
	"off",		bit3(0,1,0), -1, 2,
	"on",		bit3(0,1,0), -1, 2, /*  5 */
	"switch",	bit3(0,1,0), -1, 2,
	"toupdown",	bit3(1,1,1),  1, 0,
	"tozenalpha",	bit3(1,1,1),  1, 0,
	"tohira",	bit3(1,1,1),  1, 0,
	"tokata",	bit3(1,1,1),  1, 0, /* 10 */
	"tohankata",	bit3(1,1,1),  1, 1,
	"tozenhira",	bit3(1,1,1),  1, 0,
	"tozenkata",	bit3(1,1,1),  1, 0,
	"+",		bit3(1,1,1),  2, 0,
	"-",		bit3(1,1,1),  2, 0, /* 15 */
	"*",		bit3(1,1,1),  2, 0,
	"/",		bit3(1,1,1),  2, 0,
	"%",		bit3(1,1,1),  2, 0,
	"last=",	bit3(1,0,0),  1, 2,  /* 直前のマッチコード==引数か? */
	"if",		bit3(1,0,0), -1, 2, /* 20 */
	"unless",	bit3(1,0,0), -1, 2,
	"restart",	bit3(0,1,0),  0, 3,
	"delchr",	bit3(1,1,1),  0, 0,  /* delchrを返す。隠しコマンド */
	"alloff",	bit3(0,1,0),  0, 2,  /* 全モードをいっぺんにOFF */
	"allon",	bit3(0,1,0),  0, 2, /* 25 */
	"bitand",	bit3(1,1,1),  2, 0,
	"bitor",	bit3(1,1,1),  2, 0,
	"bitnot",	bit3(1,1,1),  1, 0,
	"!",		bit3(0,1,0),  0, 2,  /* 裏バッファの明示的クリア */
	"atEOF",	bit3(1,0,0),  0, 2, /* 30 */
	"todigit",	bit3(1,1,1),  2, 1,
	"dakuadd",	bit3(1,1,1),  1, 1, /* 濁点の付加 */
	"handakuadd",	bit3(1,1,1),  1, 1, /* 半濁点の付加 */
	"value",	bit3(1,1,1),  1, 0,
	"error",	bit3(0,1,0),  0, 3, /* 35 */
	"defconst",	bit3(1,0,0), -1, 4,
	"setmode",	bit3(0,1,0), -1, 2,
	"mode+",	bit3(0,1,0), -1, 2,
	"mode-",	bit3(0,1,0), -1, 2,
	"mode=",	bit3(1,0,0), -1, 2, /* 40 */
	"mode!=",	bit3(1,0,0), -1, 2,
	"mode<",	bit3(1,0,0), -1, 2,
	"mode>",	bit3(1,0,0), -1, 2,
	"send",		bit3(0,1,0),  0, 1,  /* 試作 */
	NULL
};
	/* last=〜unless… 入力コードの位置にしか置けず、文字列関数扱いなので
			本処理表にしか書けない。*/
	/* +,-,*,/,bitand,bitor,bitnot… 3バイト分の演算を行う。*/
	/* atEOF… ファイル末尾での特別動作を指定するためのもの。但し、その
		動作は不完全。*/

static void readhyo(), rangeset(), vchk(), singleqscan(), doubleqscan(),
  listscan(), ERHOPN(), ERRLIN(), ERRHYO();
static int evalandcpy(), eval1cpy(), evlis(), hensrc_tourk(),
  serfun(), termsscan(), atermscan(), partscan(), blankpass(),
  modsrcL();
static letter onescan(), *rangekettei();

extern	letter	*ltrgrow(), letterpick(), *ltr1cut();
extern	char	*chrcat(), *strend();

#define IHENSU (1 << 24) /* 内部表現の上位１バイトで、変数を表す */
#define IKANSU (2 << 24) /*	      〃	      関数を表す */
#define	IMODNM (3 << 24) /*	      〃	      モード名を表す */

#define ENDOF_NestFileArea ((FILE *)~(NULL))

int tmp[64];

/* 88/06/01/ V3.1 */
 /** 一行分のポインタを進める。 */
static char * /* V3.1 */
readln_ustrtoltr(buf, ltrbuf)
char	*buf;
fast	letter	*ltrbuf;
{
	fast	int	l;

	if (*buf == '\0')
		return(NULL);
	while ((l = letterpick(&buf), is_space(l)))
		;
	if ((*ltrbuf++ = l) == EOLTTR)
		return(buf);
	while((*ltrbuf++ = letterpick(&buf)) != EOLTTR);
	return(cur_rk_table->tcurread = buf);
}

/* 88/06/01/ V3.1 */
/** 一対応表の読み込み。対応表を読みながら行数をチェックする。対応表には
    空白文字以外のコントロール文字は生では混じらないものとし、混じってい
    たらチェックする。*/
static int	/* V3.1 */
readbuf(fp, s)
fast	FILE	*fp;
fast	char	*s;
{
	fast	int	c, sv = '\0';
	fast	int	cnt = 0; 		/* 一対応表の行数 */

	cur_rk_table->tcurread = s;
	while ((c = getc(fp)) != EOF){
		if (is_cntrl(c) && !isspace(c)) {
			fclose(fp);
			sprintf(cur_rk_table->tcurread, "\\%03o", c);
			ERRLIN(21);
		} 
		if (c == '\n') {
			if (sv == '\0') continue;
			c = '\0';	/* '\n' --> '\0' */
			cnt++;
			cur_rk_table->tcurread = s + 1;
		}
		*s++ = sv = c;
	}
	if (*(s-1) != '\0') {
		cnt++;
		*s++ = '\0'; /* テキスト終端の改行記号欠如のファイルに対して有効 */
	}
	*s = '\0';
	return(cnt);
}

 /** 表全部の読み込み */
/* 88/06/03/ V3.1 */
#ifdef OMRON_LIB
static
#endif
void
readdata(filename)
char	*filename;	/* モード定義表の名又はそのパス名が入っている */
{
	fast	int	i;	/* V3.1 */
	fast	int	n;	/* V3.1 */

	/* 88/06/09 V3.1 */
	malloc_for_modetable_struct(&(cur_rk_table->rk_path),
				RK_PATH_MAX_DEF, RK_PATH_MEM_DEF);
	malloc_for_modetable_struct(&(cur_rk_table->rk_taiouhyo),
				RK_TAIOUHYO_MAX_DEF, RK_TAIOUHYO_MEM_DEF);
	malloc_for_modetable_struct(&(cur_rk_table->rk_defmode),
				RK_DEFMODE_MAX_DEF, RK_DEFMODE_MEM_DEF);
	malloc_for_modetable_struct(&(cur_rk_table->rk_dspmode),
				RK_DSPMODE_MAX_DEF, RK_DSPMODE_MEM_DEF);
	malloc_for_modesw(&(cur_rk_table->rk_modesw), RK_DEFMODE_MAX_DEF);

	/* モード定義表の読み込み */
	if ((n = readmode(filename)) != 0) {
		 /** 領域の獲得 */
		malloc_for_hyo(&(cur_rk_table->rk_hyo), n);
		malloc_for_usehyo(&(cur_rk_table->rk_usehyo), n);
		malloc_for_hensuudef(&(cur_rk_table->rk_hensuudef), RK_HENSUUDEF_MAX_DEF);

		malloc_for_hensuu(&(cur_rk_table->rk_hensuu),
					RK_HENSUU_MAX_DEF, RK_HENSUU_MEM_DEF);

		malloc_for_hyobuf(&(cur_rk_table->rk_hyobuf), RK_HYO_SIZE_DEF);
		malloc_for_heniki(&(cur_rk_table->rk_heniki), (RK_HYO_SIZE_DEF / 2) + 1);

		/* それぞれの対応表の読み込み */
		for(i = 0; i < n; i++){
			readhyo(i);
		}
		for(i = 0; i < n; i++){
			cur_rk_table->rk_hyo.point[i].hensudef =
				&cur_rk_table->rk_hensuudef.point[tmp[i]];
		}
	
		 /** 領域の解放 */
		free_for_heniki(&(cur_rk_table->rk_heniki));
		free_for_hyobuf(&(cur_rk_table->rk_hyobuf));

		free_for_hensuu(&(cur_rk_table->rk_hensuu));
	}

	/* 88/06/07 V3.1 */
	free_for_modetable_struct(&(cur_rk_table->rk_defmode));
	free_for_modetable_struct(&(cur_rk_table->rk_taiouhyo));
	free_for_modetable_struct(&(cur_rk_table->rk_path));

	/* 88/06/09 V3.1 */
	/*
	if ((n = cur_rk_table->rk_hensuudef.count) != 0)
		malloc_for_henmatch(&(cur_rk_table->rk_henmatch), n);

	choosehyo();
	*/
}

/* 88/06/02 V3.1 */
 /**	固定された（サーチパスを見る必要のない）ファイル名に対しては非0を
	返す。現在のところ、 / ./ ../ のどれかで始まるものとしているが（作者の
	独断）、適当に変えてよい。strchr(s,'/')!=NULL とする方が良いかも */
static
int
fixednamep(s)
char	*s;
{
	return(!strncmp("/",s,1)|| !strncmp("./",s,2)|| !strncmp("../",s,3));
}

 /**  nameで与えられた名の変換対応表をオープンする。固定名と見なされない
      名前（fixednamep()参照）に対しては、サーチパスを見る。*/
/* 88/06/09 V3.1 */
static FILE * /* V3.1 */
trytoopen(n, size)
int	n;	/* 対応表の番号 */
int	*size;
{
	fast	char	*name;	/* V3.1 */
	/*fast	char	**src;*/	/* V3.1 */
	fast	int	i;	/* V3.1 */
	/*fast	int	cnt;*/	/* V3.1 */

	char	taio_fnm[REALFN];
	struct	stat	statbuf;	/* V3.1 */
	FILE	*fp;	/* V3.1 */


	/* 88/06/10 V3.1 */
	if (fixednamep(cur_rk_table->tcurfnm = name = cur_rk_table->rk_taiouhyo.point[n])) {
		/* 88/06/09 V3.1 */
		cur_rk_table->tcurdir = "";	/* エラー処理用 */	/* V3.1 */
		if (stat(name, &statbuf) != 0) ERHOPN(1);
		if ((fp = fopen(name, "r")) == NULL) ERHOPN(2);
		/*
		if (cur_rk->flags & RK_VERBOS) {
			fprintf(stderr, "romkan: using Taio-hyo %s ...\r\n",
				name);
		}
		*/
		*size = statbuf.st_size;
		return(fp);
	} else {
		/* 88/06/09 V3.1 */
		for (i = 0; i < cur_rk_table->rk_path.count; i++){
			strcpy(taio_fnm, (cur_rk_table->tcurdir = cur_rk_table->rk_path.point[i]));
			strcat(taio_fnm, name);

			if (stat(taio_fnm, &statbuf) != 0 || 
			    (fp = fopen(taio_fnm, "r")) == NULL) continue;
			 /* Now Taio-hyo found */
			 /*
			if (cur_rk->flags & RK_VERBOS) {
				fprintf(stderr,
					"romkan: using Taio-hyo %s ...\r\n",
					taio_fnm);
			}
			*/
			*size = statbuf.st_size;
			return(fp);
		}
		/* Taio-hyo not found */
#ifdef	nodef
		if (cur_rk->flags & RK_VERBOS) {
			fast	char	*p, *q;		/* V3.1 */

			fprintf(stderr, "no %s in", name);
			for (i = 0; i < cur_rk_table->rk_path.count; i++) {	/* V3.1 */
				fputc(' ', stderr);
				p = q = cur_rk_table->rk_path.point[i];	/* V3.1 */
				if(*q == KUGIRI && *(q + 1) == '\0') q++;
				 else q = strend(q);
				for (; p < q; p++)		/* V3.1 */
					fputc(*p, stderr);
			}
			fprintf(stderr, ".\n");
		}
#endif
		cur_rk_table->tcurdir = "";	/* V3.1 */
		ERHOPN(1);	/* V3.1 */
	}
	return(NULL);
}

/* 88/06/02 V3.1 */
/******************************************************************************/
#ifndef OMRON
 /**	linbufに入っているunsigned charの列をletterの列にしてltrbufに入れる。
	エンドマークはEOLTTRになる。flgが非0の時は、先頭の空白文字は飛ばす。*/
static /* V3.1 */
void
ustrtoltr(linbuf, ltrbuf, flg)
uns_chr	*linbuf;
int	flg;
fast	letter	*ltrbuf;	/* V3.1 */
{
	fast	letter	l;	/* V3.1 */

	if(flg){
		while(l = letterpick(&linbuf), is_eolsp(l)){
			if(l == EOLTTR){
				*ltrbuf = EOLTTR;
				return;
			}
		}
		*ltrbuf++ = l;
	}
	while((*ltrbuf++ = letterpick(&linbuf)) != EOLTTR);
}
#endif
/******************************************************************************/

 /** letterを文字列にコンバート */
void
ltr1tostr(l, sptr)
char	**sptr;
letter	l;
{
	int	i;

	for(i = 0; i < 3 && 0 == (l & (0xff000000)); i++) l <<= 8;
	for(; i < 4; i++){
		*(*sptr)++ = (char)(l >> 24);
		l <<= 8;
	}
}

 /** 変数の「現在行既出フラグ」を全てクリア */
static /* V3.1 */
void
hen_useflgclr(hensu, n)
fast	hensuset *hensu;	/* V3.1 */
fast	int	n;			/* V3.1 */
{
	fast	int	i;	/* V3.1 */
	
	for (i = 0; i < n; i++)
		(hensu++) -> curlinflg = 0; /* 現在行に未出 */
}

 /** 対応表一つ読み込み */
static /* V3.1 */
void
readhyo(n)
fast	int	n;	/* V3.1 */
{
	fast	int	i, j;	/* V3.1 */
	fast	letter	*lp;	/* V3.1 */
	fast	int	m;	/* V3.1 */
	fast	hensuset *hensuptr;		/* V3.1 */
	letter	ltrbuf[LINSIZ];				/* V3.1 */
	letter	termbuf[TRMSIZ], dummybuf[TRMSIZ];	/* V3.1 */
	int	hyosw, rsltyp[3];

	FILE	*fp;			/* V3.1 */
	int	size, lines;		/* V3.1 */

	/* 88/06/11 V3.1 */
	/* 対応表のオープン */
	fp = trytoopen(n, &size);
	check_and_realloc_for_hyobuf(&(cur_rk_table->rk_hyobuf), size);
	lines = readbuf(fp, cur_rk_table->rk_hyobuf.next = cur_rk_table->rk_hyobuf.org);
	fclose(fp);

	/* 変換データの領域の獲得と初期化、及び、対応表の種別を求める */
	malloc_for_hyo_area(&(cur_rk_table->rk_hyo), n, lines, size, &datptr, &memptr);
	hyosw = cur_rk_table->rk_hyo.point[n].hyoshu = filnamchk(cur_rk_table->rk_taiouhyo.point[n]);
	/*
	cur_rk_table->rk_hyo.point[n].hensudef = &cur_rk_table->rk_hensuudef.point[cur_rk_table->rk_hensuudef.count];
	*/
	tmp[n] = cur_rk_table->rk_hensuudef.count;

	check_and_realloc_for_heniki(&(cur_rk_table->rk_heniki), (size / 2) + 1);
	*(hen_iki = cur_rk_table->rk_heniki.next = cur_rk_table->rk_heniki.org) = EOLTTR;

	/* 88/06/10 V3.1 */
	cur_rk_table->rk_hensuu.count = 0;
	*(cur_rk_table->rk_hensuu.next = cur_rk_table->rk_hensuu.org) = EOLTTR;

	cur_rk_table->ltrbufbgn = ltrbuf;
	dummy = dummybuf;

	/* 88/06/11/ V3.1 */
	cur_rk_table->rk_hyobuf.next = cur_rk_table->rk_hyobuf.org;
	while (cur_rk_table->rk_hyobuf.next =
	    readln_ustrtoltr(cur_rk_table->rk_hyobuf.next, (ltrbufptr = ltrbuf))) {
		datptr->code[0] = NULL;				/* V3.1 */
		/*hen_useflgclr(cur_rk_table->rk_hensuu.org, cur_rk_table->rk_hensuu.count);	 V3.1 */
		hen_useflgclr(cur_rk_table->rk_hensuu.point, cur_rk_table->rk_hensuu.count);	 /* V3.1 */

		for(m = 0; termsscan(&ltrbufptr, term = termbuf, 1); m++){
			 /* mは、何番目の項目を見ているかを表す */
			if(*term == ';') break; /* 注釈行 */
			if(m == 3) ERRLIN(15);
			if(m != 0 && rsltyp[0] == 4) ERRLIN(12);

			datptr -> code[m] = memptr;
			if((rsltyp[m] = evalandcpy(&term, m)) == 4){
				if(m) ERRLIN(14);
			 /* 宣言は最初の項目にしか来れない。
			    funstr[]のappearフラグでもチェックしているが
			    将来のために一応ここにもチェックを入れておく。*/
			} else {
			 /* 宣言の時には内部表現へのポインタは進めない */
				totail(memptr);
				memptr++;
			}
		}

		if(m != 0 && rsltyp[0] != 4){
			static	letter	nil[1] = {EOLTTR};	/* V3.1 */
			for( /* m=? */ ; m < 3; m++){
				datptr -> code[m] = nil;
				rsltyp[m] = -1; /* doesn't exist */
			}
			datptr++;

 /* rsltyp: 0=文字項 1=文字列連 2=データ連 3=機能項 4=宣言項 -1=存在せず */
			switch(hyosw){
		   /* 前・後処理は、表の内容に制限がある。それを検査 */
				case 1:
					if(!(rsltyp[0] == 0 &&
					     rsltyp[1] == 0 && 
					     rsltyp[2] == -1
					    ))
						ERRLIN(17);
					break;
				case 2:
					if(rsltyp[1] == 3 && rsltyp[2] != -1)
						ERRLIN(19);
					break;
				case 3:
					if(!(rsltyp[0] == 0 &&
					     (rsltyp[1] == 0 ||
					      rsltyp[1] == 1) &&
					     rsltyp[2] == -1
					    ))
						ERRLIN(18);
					break;
				default:
					BUGreport(10);
			}

		}
	}

	(datptr++) -> code[0] = NULL;

		/* 変数登録ここでまとめてする */
	for (lp = cur_rk_table->rk_heniki.org; lp < hen_iki; )
		*memptr++ = *lp++;

	for (i = 0, hensuptr = cur_rk_table->rk_hensuu.point; i < cur_rk_table->rk_hensuu.count;
							i++, hensuptr++) {
		if (hensuptr->regdflg == 0) ERRHYO(1);	/* V3.1 */

		check_and_realloc_for_hensuudef(&(cur_rk_table->rk_hensuudef),
				j = cur_rk_table->rk_hensuudef.count, RK_HENSUUDEF_MAX_LOT);
		cur_rk_table->rk_hensuudef.point[j] = memptr - (lp - hensuptr -> range);
	}
}

 /**	変換対応表の項目一つを、解釈して、内部形式のデータエリアにコピーする。
	返す値は、解釈した項目が文字項なら0、それ以外で文字列連なら1、それ以外
	でデータ連なら2、機能項なら3、宣言項なら4。それ以外はエラー。*/
static /* V3.1 */
int
evalandcpy(socp, m)
fast	letter	**socp;	/* 項目へのポインタ（へのポインタ）*/	/* V3.1 */
int	m;   /* 対応表の何番目の項目を見ているかを表す。入力コード部を
		見ているときは0、出力コード部なら1、バッファ残り部なら2 */
{
#define	TYPMAX	5

	char	exist[TYPMAX];	/* V3.1 */
	fast	char	total;	/* V3.1 */
	 /* existは、それぞれタイプ0〜4の項の出現のフラグ（eval1cpy()参照）。
	    totalは全体としての出現フラグ。どちらも、1=未出 2=一回出 その他の
	    時は下2ビット0 */
	fast	int	type;	/* V3.1 */

#define	TIME_0(flag) ((flag) == 1)
#define	TIME_1(flag) ((flag) == 2)
#define	TIME_n(flag) (((flag) & 3) == 0)

	total = 1;
	for(type = 0; type < TYPMAX; type++) exist[type] = 1;

	while(!is_eolsp(**socp)){
		if(!(0 <= (type = eval1cpy(socp, m, 0)) && type < TYPMAX))
			BUGreport(3);
		exist[type] <<= 1;
		total <<= 1;
	}

	if(TIME_0(total))
		BUGreport(13); /* 項目が空ならevalandcpyは実行しない筈 */
	if(!TIME_0(exist[3]) || !TIME_0(exist[4])){
		if(TIME_n(total)) ERRLIN(9);
		return(type); /* 3又は4。typeが値を保持している筈 */
	}
	if(TIME_1(total) && TIME_1(exist[0])) return(0);
	return(!TIME_0(exist[2]) ? 2 : 1);
}

 /**	対応表の項目の中の項一つを解釈し、内部形式のデータエリアにコピーし、
	それのタイプ（文字項=0 文字列項=1 特殊関数項=2 機能項=3 宣言項=4）を
	返す。flgが非0なら、再帰的に呼ばれたことを意味し、その場合、
	現在行に未出の変数を検出したらエラー。また、mが非0のとき（入力コード部
	以外の所を見ている時）も、現在行に未出の変数を検出したらエラー。*/
static /* V3.1 */
int
eval1cpy(socp, m, flg)
letter	**socp; /* flg以外の引数の意味はevalandcpyと同じ */
int	m, flg;
{
	letter	t1buf[TRMSIZ], *t1bufp;

	t1bufp = t1buf;

	*memptr = EOLTTR;
	switch(partscan(socp, t1bufp)){
		case 1:	/* 単文字 */
			memptr = ltrgrow(memptr, t1bufp);
			*memptr = EOLTTR;
			return(0);
		case 2: /* 引用文字 */
			t1bufp++;
			*memptr++ = onescan(&t1bufp, dummy);
			*memptr = EOLTTR;
			return(0);
		case 3: /* 引用文字列 */
			t1bufp++;
			while(*t1bufp != '"'){
				*memptr++ = onescan(&t1bufp, dummy);
			}
			*memptr = EOLTTR;
			return(1);
		case 0: /* リスト */
			return(evlis(m, &t1bufp, flg));
			 /* evlis内で *memptr = EOLTTR; をしている。*/
		default:
			BUGreport(4);
			 /*NOTREACHED*/
	}
	return(-1);
}

#define bitchk(x, n) ((x) & (1 << (n)))

#define get_ltr(lp) (*(lp)++)
#define unget_ltr(l, lp) (*--(lp) = (l))

 /** globalなポインタから指されているletter列から一文字取ってくる。*/
letter	get1ltr()
{
	return(get_ltr(lptr));
}

letter	unget1ltr(l)
letter	l;
{
	return(unget_ltr(l, lptr));
}

int int_get1ltr() {return((int)(get1ltr()));}
int int_unget1ltr(c) letter c; {return((int)(unget1ltr((letter) c)));}
 /** 汚いことこの上なし！なぜ関数の型のcastができないの？
    「(int ()) get1ltr」と書きたい！ */

 /**	includeファイル名のletter列をstringに取り出す作業を、letter列の終わり
	まで続ける。flg & 01が非0なら、'/'でも終了。*/
letter	getfrom_dblq(socp, destp, flg)
letter	**socp;
char	**destp;
int	flg;
{
	letter	l;

	while(**socp != EOLTTR && !(flg & 01 && **socp == KUGIRI)){
		if (**socp == '\\') *(*destp)++ = '\\';
		l = onescan(socp, dummy);
		ltr1tostr(l, destp);
	}
	*(*destp)++ = '\0';
	return(**socp);
}

int
getfrom_lptr(sptr, flg)
char	**sptr;
int	flg;
{
	return((int)getfrom_dblq(&lptr, sptr, flg));
}

 /**	リストを解釈して内部表現にする。返値は、そのリストのタイプを表す数。
	文字変数項又は文字関数項:0 文字列関数項:1 特殊関数項:2
	機能項:3 宣言項:4 */
static /* V3.1 */
int
evlis(m, socp, flg)
letter	**socp; /* 引数の意味はeval1cpyを参照 */
int	m, flg;
{
	fast	int	fnnum, hennum, i;	/* V3.1 */
	letter	t1buf[TRMSIZ];

	(*socp)++; /* '('をスキップ */
	atermscan(socp, t1buf, 3);

	fnnum = serfun(t1buf);
	if(fnnum != -1 && !bitchk(func[fnnum] . appear, m)) ERRLIN(14);
	 /* mの値によって、現れてはいけない所への出現かどうか見ている。*/

	switch(fnnum){ /* defaultの所以外は func[fnnum].argnumを使ってない */
		case -1: /* 変数 */
			vchk(t1buf);
			atermscan(socp, dummy, 2); /* あればERR */
			hennum = hensrc_tourk(t1buf, ((m==0 && !flg)? 0 : 1));
			*memptr++ = hennum | IHENSU;
			break;

		case 0: /* fn No.0 defvar */
			atermscan(socp, t1buf, 3);
			if(*t1buf == '('){
				letter	*soc2, t1buf2[TRMSIZ], t1buf3[TRMSIZ];
				letter	*common_hen;

				atermscan(socp, t1buf3, 3);

				soc2 = t1buf + 1; /* skip '(' */

				atermscan(&soc2, t1buf2, 3);
				vchk(t1buf2);
				if(-1 != serfun(t1buf2)) ERRLIN(11);
				hennum = hensrc_tourk(t1buf2, -1);
				common_hen = rangekettei(hennum, t1buf3);

				while(atermscan(&soc2, t1buf2, 0)){
					vchk(t1buf2);
					if(-1 != serfun(t1buf2)) ERRLIN(11);
					hennum = hensrc_tourk(t1buf2, -1);
					rangeset(hennum, common_hen);
				}
			} else {
				vchk(t1buf);
				if(-1 != serfun(t1buf)) ERRLIN(11);
				hennum = hensrc_tourk(t1buf, -1);
				  /* defvarの重複を避けるため */
				atermscan(socp, t1buf, 3);
				rangekettei(hennum, t1buf);
			}
			atermscan(socp, dummy, 2);
			break;

		case 36: /* fn No.36 defconst */

			atermscan(socp, t1buf, 3);
			vchk(t1buf);
			if(-1 != serfun(t1buf)) ERRLIN(11);

			hennum = hensrc_tourk(t1buf, 6);
			  /* defvar・defconstの変数名重複を避けるため */

			rangeset(hennum, hen_iki);

			blankpass(socp, 1);
			if(*(*socp)++ != '\'') ERRLIN(8);
			*hen_iki++ = onescan(socp, dummy);
			(*socp)++; /*「'」が閉じていることの検査は済んでいる*/
			*hen_iki++ = EOLTTR; /* needed? */
			*hen_iki = EOLTTR;

			atermscan(socp, dummy, 2);
			break;

		case 1: /* fn No.1 include */
			{
				char	fnmtmparea[REALFN], *s, *dirnamptr;
				int	dummyc = 0, err;

				blankpass(socp, 1);
				if(3 != partscan(socp, t1buf)) ERRLIN(22);
				atermscan(socp, dummy, 2);
				 /* 余分にあればERR */

				ltr1cut(lptr = t1buf + 1);
				*(s = fnmtmparea) = '\0';
				err = readfnm(int_get1ltr, int_unget1ltr,
					      getfrom_lptr, &s, &dummyc);

				if(err){
					cur_rk_table->hcurread = s;
					switch(err){
						case 1:
						case 3: ERRLIN(25);
						case 2: ERRLIN(26);
						case 4: ERRLIN(27);
					}
				}
				de_bcksla(fnmtmparea, fnmtmparea);

				if(*++(cur_rk_table->base) == ENDOF_NestFileArea){
					(cur_rk_table->base)--;
					ERRLIN(23);
				}
				*(cur_rk_table->base)= trytoopen(fnmtmparea, &dirnamptr,&err);
				if(err){
					switch(err){
						case 1:
						case 3:
						case 2:
						case 4: (cur_rk_table->base)--;
							ERRLIN(24);
					}
				}
			}
			break;

		 /* モード名一つを引数に取るもの */
		case 4: /* fn No.4〜6 off,on,switch */
		case 5:
		case 6:
		case 20: /* fn No.20,21 if,unless */
		case 21:
			*memptr++ = fnnum | IKANSU;
			atermscan(socp, t1buf, 3);
			*memptr++ = modsrcL(t1buf) | IMODNM;
			break;

		 /* モード名と文字 一つずつを引数に取るもの */
		case 37: /* fn No.37〜43 setmodeなど */
		case 38:
		case 39:
		case 40:
		case 41:
		case 42:
/*
		case 43:
			{
				int	err, n;
				modetyp	stat;

				*memptr++ = fnnum | IKANSU;
				atermscan(socp, t1buf, 3);
				*memptr++ = (n = modsrcL(t1buf))| IMODNM;
				atermscan(socp, t1buf, 3);
				err = chkL_get_int(t1buf, &stat,
							 modesw[n] . moderng);
				if(err != 0) ERRLIN(29);
				*memptr++ = stat;
				atermscan(socp, t1buf, 2);
				break;
			}
*/

		 /* 普通（引数を取らないものを含む） */
		default: /* toupper,tolower… */
			*memptr++ = fnnum | IKANSU;
			*memptr = EOLTTR;
			for(i = 0; i < func[fnnum] . argnum; i++){
				blankpass(socp, 1);
				if(eval1cpy(socp, m, 1) != 0) ERRLIN(13);
			}
			atermscan(socp, dummy, 2); /* 余分にあればERR */
			break;
	}
	*memptr = EOLTTR;
	return(fnnum == -1 ? 0 : func[fnnum] . fntype);
}

 /** 文字列中の「\」を抜く */
void
de_bcksla(s, r)
char	*s, *r;
{
	for(; *s; *r++ = *s++) if(*s == '\\') s++;
	*r = '\0';
}

 /**	defvarの第二引数（shikiに入る）を解釈して、その変数の変域を決定する。
	変数列の最後にエンドマークもちゃんと入る。返値は、変域を格納した
	所へのポインタ。*/
static /* V3.1 */
letter	*rangekettei(num, shiki)
letter	*shiki;
int	num; /* 変域を決定しつつある変数の内部番号 */
{
	letter	hyoukabuf[TRMSIZ];

	rangeset(num, hen_iki);
	*hen_iki = EOLTTR;

	if(*shiki++ != '(') ERRLIN(8);
	atermscan(&shiki, hyoukabuf, 1);

	if(!ltrstrcmp(hyoukabuf, "all")){
		*hen_iki++ = VARRNG;
		*hen_iki++ = 0;
		*hen_iki++ = LTRHUG; /* 変域は全文字 */
		*hen_iki++ = EOLTTR;
		*hen_iki = EOLTTR;
		atermscan(&shiki, dummy, 2); /* 余分にあればERR */
	} else 
	if(!ltrstrcmp(hyoukabuf, "between")){
		int	i;

		*hen_iki++ = VARRNG;
		while(blankpass(&shiki, 1), *shiki != ')'){
			for(i = 1; i <= 2; i++){
				switch(*shiki){
					case '\'':
						shiki++;
						*hen_iki++ = onescan(&shiki,
								     dummy);
						shiki++;
						break;
					case ')':
					case '"':
					case '(': ERRLIN(8);
					default:
						*hen_iki++ = *shiki++;
				}
				if(i < 2){
					if(!is_eolsp(*shiki)) ERRLIN(8);
					blankpass(&shiki, 1);
				}
			}
		}
		*hen_iki++ = EOLTTR;
		*hen_iki = EOLTTR;
	} else
	if(!ltrstrcmp(hyoukabuf, "list")){
		while(blankpass(&shiki, 1), *shiki != ')'){
			switch(*shiki){
				case '"':
				case '(': ERRLIN(8);
				case '\'':
					shiki++;
					*hen_iki++ = onescan(&shiki, dummy);
					shiki++; /* 本当に「'」が閉じているか
					 どうかの検査はもう済んでいる。*/
					break;
				default:
					*hen_iki++ = *shiki++;
			}
			if(!is_eolsp(*shiki)) ERRLIN(8);
		}
		*hen_iki++ = EOLTTR;
		*hen_iki = EOLTTR;
	} else {
		ERRLIN(8); /* 将来はこの他の構文も許す予定であった */
	}

	return(cur_rk_table->rk_hensuu.point[num].range);	/* V3.1 */
}

 /**	num番目の変数の変域を指すべきポインタの指し先を決定し、その変数を
	既登録状態にする。*/
static /* V3.1 */
void
rangeset(num, range)
letter	*range;	/* 変域の入る場所のポインタ */
int	num;
{
	cur_rk_table->rk_hensuu.point[num].range = range;	/* V3.1 */
	cur_rk_table->rk_hensuu.point[num].regdflg = 1;	/* V3.1 */
}


 /**	nameで指定された名の変数を探し、なければ登録。変数名の最後に
	エンドマークもちゃんと入る。
	flgが1の時、その名の変数が現在行に未出ならエラー。また、flgが-1の時、
	その名の変数が既定義ならエラー（defvarの重複チェック用）。*/
static /* V3.1 */
int
hensrc_tourk(name, flg)
letter	*name;
int	flg;
{
	fast	int	n;			/* V3.1 */
	fast	hensuset *hensuptr;	/* V3.1 */

	/* 88/06/06 V3.1 */
	for (n = 0; n < cur_rk_table->rk_hensuu.count; n++){
		if(ltrcmp(cur_rk_table->rk_hensuu.point[n].name, name)) continue;
		 /* found */
		if(flg == -1 && cur_rk_table->rk_hensuu.point[n].regdflg != 0) ERRLIN(10);
		if(flg == 1 && cur_rk_table->rk_hensuu.point[n].curlinflg == 0) ERRLIN(5);
		cur_rk_table->rk_hensuu.point[n].curlinflg = 1;
		return(n);
	}

	 /* ここへ来たということは、初出の変数と言うこと。当然、flgが1なら
	    エラー。*/
	if(flg == 1) ERRLIN(5);

	check_and_realloc_for_hensuu(&cur_rk_table->rk_hensuu, n, name,
					RK_HENSUU_MAX_LOT, RK_HENSUU_MEM_LOT);

	hensuptr = &cur_rk_table->rk_hensuu.point[n];
	hensuptr -> name =  cur_rk_table->rk_hensuu.next;
	hensuptr -> curlinflg = 1;
	hensuptr -> regdflg = 0; /* 初出だからrangeのdefは未だの筈 */
	cur_rk_table->rk_hensuu.next = ltrgrow(hensuptr->name, name);
	*(++(cur_rk_table->rk_hensuu.next)) = EOLTTR;
	return(n);
}

 /** 組み込み関数・機能名に対してはその番号を、そうでないものなら-1を返す */
static /* V3.1 */
int
serfun(lp)
fast	letter	*lp; /* 関数・機能名もしくは変数名 */	/* V3.1 */
{
	fast	int	i;	/* V3.1 */

	for(i = 0; func[i] . fnname != NULL; i++){
		if(! ltrstrcmp(lp, func[i] . fnname)) return(i);
	}
	return(-1);
}

 /** 変数の名前のチェック おかしいとエラー */
static /* V3.1 */
void
vchk(lp)
fast	letter	*lp;	/* V3.1 */
{
	if(is_digit(*lp)) ERRLIN(3);
	for(; *lp != EOLTTR; lp++){
	  /*	if(is_lower(*lp)) *lp = to_upper(*lp);  */
		if(!is_alnum(*lp) && *lp != '_') ERRLIN(3);
	}
}

 /**	一項目を取り出す。取り出しに成功すると1を返す。flgが非0の時は、')'が
	見つかるとエラー、';'はそれだけで一項目扱い。*/
static /* V3.1 */
int
termsscan(socp, dest, flg)
fast	letter	**socp, *dest;	/* V3.1 */
	 /* socpの指しているポインタが指している所から取り出してdestに入れる。
	    その後、socpが指しているポインタを進める。このファイルの **scan()
	    という関数は全てそうなってる。*/
int	flg;
{
	fast	letter	*bgn;	/* V3.1 */

	bgn = dest;

	if(blankpass(socp, 0) == 0){
		if(flg && **socp == ';'){
			*dest++ = *(*socp)++;
		} else
		while(!is_eolsp(**socp)){
			if(**socp == ')'){
				if(flg) ERRLIN(0);
				break;
			} else {
				partscan(socp, dest);
				totail(dest);
			}
		}
	}

	*dest = EOLTTR;
	return(bgn != dest);
}

 /**	リスト一つか、単純項の一まとまりを取り出す。成功したら1を返す。
	flgが1のとき、')'が見つかるとエラー。
	flgが2のとき、取り出しに成功したらエラー。
	flgが3のとき、取り出しに失敗したらエラー。*/
static /* V3.1 */
int
atermscan(socp, dest, flg)
fast	letter	**socp, *dest;	/* V3.1 */
int	flg;
{
	fast	letter	*bgn;	/* V3.1 */
	fast	int	found;	/* V3.1 */

	bgn = dest;

	if(blankpass(socp, 0) == 0){
		if(**socp == '('){
			listscan(socp, dest);
			totail(dest);
		} else {
			while(!is_eolsp(**socp) && **socp != '('){
				if(**socp == ')'){
					if(flg == 1) ERRLIN(0);
					break;
				} else {
					partscan(socp, dest);
					totail(dest);
				}
			}
		}
	}

	*dest = EOLTTR;

	found = (bgn != dest);
	if(!found && flg == 3 || found && flg == 2) ERRLIN(7);
	return(found);
}

 /**	項一つを取り出す。取り出したものがリストなら返値は0、単文字なら1、
	引用文字なら2、引用文字列なら3。*/
static /* V3.1 */
int
partscan(socp, dest)
fast	letter	**socp, *dest;	/* V3.1 */
{
	switch(**socp){
		case '(':
			listscan(socp, dest);
			return(0);
		case '\'':
			singleqscan(socp, dest);
			return(2);
		case '"':
			doubleqscan(socp, dest);
			return(3);
		default:
			*dest++ = *(*socp)++;
			*dest = EOLTTR;
			return(1);
	}
}

 /** シングルクォート表現一つを取り出す。*/
static /* V3.1 */
void
singleqscan(socp, dest)
fast	letter	**socp, *dest;	/* V3.1 */
{
	*dest++ = *(*socp)++;
	onescan(socp, dest);
	totail(dest);
	if((*dest++ = *(*socp)++) != '\'') ERRLIN(1);

	*dest = EOLTTR;
}

 /** ダブルクォート表現一つを取り出す。*/
static /* V3.1 */
void
doubleqscan(socp, dest)
fast	letter	**socp, *dest;	/* V3.1 */
{
	*dest++ = *(*socp)++;
	while(**socp != '"'){
		if(**socp == EOLTTR) ERRLIN(1);
		onescan(socp, dest);
		totail(dest);
	}
	*dest++ = *(*socp)++;

	*dest = EOLTTR;
}

 /**	8・10・16進コード用キャラクタを実際のコードに直す。入力のチェックは
	せず、英文字と数字以外の入力に対しては単に0を返す。*/
#ifdef OMRON_LIB
static
#endif
int
ltov(l)
fast	letter	l;	/* V3.1 */
{
	if(is_upper(l)) return(l - 'A' + 10);
	if(is_lower(l)) return(l - 'a' + 10);
	if(is_digit(l)) return(l - '0'); else return(0);
}

 /** ltovの逆 */
#ifdef OMRON_LIB
static
#endif
letter	vtol(l)
fast	letter	l;	/* V3.1 */
{
	if(BASEMX <= l) return('*');
	return(l + (l < 10 ? '0' : 'A' - 10));
}

 /**	シングル・ダブルクォートの中での一文字取り出し。
	「^」によるコントロールコード表現、「\」による8・10・16進表現にも
	対応。「\」表現は、「\［o又はd又はx］数字の並び［;］」である。*/
static /* V3.1 */
letter	onescan(socp, dest)
fast	letter	**socp, *dest;	/* V3.1 */
{
	fast	letter	l, realcode;	/* V3.1 */
	fast	int	digflg;		/* V3.1 */

	switch(realcode = *dest++ = *(*socp)++){
		case '^':
			if(!(' ' <= (l = *(*socp)++) && l < '\177'))ERRLIN(2);
			realcode = ((*dest++ = l) == '?' ? '\177' : l & 0x1f);
			break;
		case '\\':
			digflg = 0;
			switch(**socp){
				case 'n':
					*dest++ = *(*socp)++; realcode = '\n';
					break;
				case 't':
					*dest++ = *(*socp)++; realcode = '\t';
					break;
				case 'b':
					*dest++ = *(*socp)++; realcode = '\b';
					break;
				case 'r':
					*dest++ = *(*socp)++; realcode = '\r';
					break;
				case 'f':
					*dest++ = *(*socp)++; realcode = '\f';
					break;
				case 'e':
				case 'E':
					*dest++ = *(*socp)++; realcode=ESCCHR;
					break;
				case 'o':
					*dest++ = *(*socp)++;
					for(realcode = 0; is_octal(**socp);){
						digflg = 1;
						realcode <<= 3;
						realcode += ltov
						       (*dest++ = *(*socp)++);
					}
					if(!digflg) ERRLIN(2);
					if(**socp== ';') *dest++ = *(*socp)++;
					break;
				case 'x':
					*dest++ = *(*socp)++;
					for(realcode = 0; is_xdigit(**socp);){
						digflg = 1;
						realcode <<= 4;
						realcode += ltov
						       (*dest++ = *(*socp)++);
					}
					if(!digflg) ERRLIN(2);
					if(**socp== ';') *dest++ = *(*socp)++;
					break;
				case 'd':
					*dest++ = *(*socp)++;
					for(realcode = 0; is_digit(**socp);){
						digflg = 1;
						realcode *= 10;
						realcode += ltov
						       (*dest++ = *(*socp)++);
					}
					if(!digflg) ERRLIN(2);
					if(**socp== ';') *dest++ = *(*socp)++;
					break;
				default:
					if(is_octal(**socp)){
						for(realcode = 0;
						    is_octal(**socp);){
							realcode <<= 3;
							realcode += ltov(
							*dest++ = *(*socp)++);
						}
						if(**socp == ';')
							*dest++ = *(*socp)++;
					} else {
						realcode= *dest++= *(*socp)++;
					}
			}
			break;
		default: ;
	}

	*dest = EOLTTR;
	return(realcode);
}
	
 /**	letterの列の先頭にある空白をスキップする。
	行末に達したら、flgが0のときは1を返し、そうでないとエラー。*/
static /* V3.1 */
int
blankpass(pptr, flg)
fast	letter	**pptr;	/* V3.1 */
	 /* letterの列のポインタへのポインタ。これが指しているものを進める */
int	flg;
{
	while(is_eolsp(**pptr)){
		if(EOLTTR == **pptr){
			if(flg) ERRLIN(4);
			return(1);
		}
		(*pptr)++;
	}
	return(0);
}

 /** リスト一個取り出し */
static /* V3.1 */
void
listscan(socp, dest)
fast	letter	**socp, *dest;	/* V3.1 */
{
	fast	int	eofreach;	/* V3.1 */

	*dest++ = *(*socp)++; /* = '(' */
	*dest++ = ' ';
	
	while(eofreach = blankpass(socp, 0), **socp != ')'){
		if(eofreach){
			/* 88/06/02/ V3.1 */
			if((cur_rk_table->rk_hyobuf.next = readln_ustrtoltr(cur_rk_table->rk_hyobuf.next,
					(*socp = cur_rk_table->ltrbufbgn))) == NULL)
				ERRLIN(20);
			 /* listの中で行が切れている場合、一行追加読み込みを
			    する。uns_chr用のバッファも、letter用のものも、
			    以前の物を先頭から再利用しているので注意。また、
			    エラーが起きた場合、 エラーの位置にかかわらず、
			    現在行として表示されるのは、最後に読まれた行のみ*/
		} else {
			termsscan(socp, dest, 0);
			totail(dest);
			*dest++ = ' ';
		}
	}
	*dest++ = *(*socp)++; /* = ')' */
	*dest = EOLTTR;

}

 /** lpで指定されたモード名を探す。見つからないとエラー */
static /* V3.1 */
int
modsrcL(lp)
letter	*lp;
{
	fast	int	n;	/* V3.1 */

	/* 88/06/07 V3.1 */
	for(n = 0; n < cur_rk_table->rk_defmode.count; n++)
		if(!ltrstrcmp(lp, cur_rk_table->rk_defmode.point[n])) return(n);

	ERRLIN(16);
	 /*NOTREACHED*/
	return(0);
}

 /**	この先は、表読み込みでエラった時に警告するルーチン。nはエラーコード。
	ermesという配列が用意されてないものは、今の所エラーメッセージが
	一つしかないもの。その場合エラーコードも今の所0のみ。
	またこれらは全て、romkan_initのエラーコードとして、longjmp経由で
	1を返す。*/


static /* V3.1 */
void
ERHOPN(n) /* 変換対応表がオープンできない */
fast	unsigned int	n;	/* V3.1 */
{
	static	char	*ermes[] = {
 /*  0 */	"Unprintable error",
		"Taio-hyo doesn't exist",
		"Can't open Taio-hyo",
	};

	if(n >= numberof(ermes)) n = 0;

	fprintf(stderr, "\r\nromkan: Taio-hyo %s%s ---\r\n", cur_rk_table->tcurdir, cur_rk_table->tcurfnm);
	fprintf(stderr, "\tError No.%d: %s.\r\n", n, ermes[n]);
	longjmp(cur_rk_table->env0, 1);
}


static	/* V3.1 */
void
ERRLIN(n) /* 変換対応表のエラー */
fast	unsigned int	n;	/* V3.1 */
{
	static	char	*ermes[] = {
 /*  0 */	"')' mismatch",
		"Incomplete single-quote or double-quote expression",
		"Illegal ^,\\o,\\x or \\d expression",
		"Illegal variable name",
		"Incomplete line",
 /*  5 */	"Evaluation of unbound variable",
		"Unprintable error",
		"Too many or too few contents of list",
		"Illegal defvar",
		"Faculity or declaration joined other item(s)",
 /* 10 */	"Duplicate defvar",
		"Variable name conflicts with Function name",
		"A line contains both declaration and other output item(s)",
		"Argument isn't a letter",
		"Function, faculity or declaration in illegal place",
 /* 15 */	"More than 3 items",
		"Undefined mode",
		"Against the restriction of pre-transform table",
		"Against the restriction of after-transform table",
		"Item comes after faculity",
 /* 20 */	"Incomplete list",
 		"Illegal character",
	};

	if(n >= numberof(ermes)) n = 6;

	fprintf(stderr, "\r\nromkan: Taio-hyo %s%s ---\r\n%s\r\n",
		cur_rk_table->tcurdir, cur_rk_table->tcurfnm, cur_rk_table->tcurread);
	fprintf(stderr, "\tError No.%d: %s.\r\n", n, ermes[n]);
	longjmp(cur_rk_table->env0, 1);
}

/* 88/06/10 V3.1 */
static /* V3.1 */
void
ERRHYO(n) /* ERRLINと同様、対応表のエラーだが、一行だけの誤りでなく
             全体を見ないとわからない誤り。「未定義変数の出現」*/
fast	unsigned int	n;	/* V3.1 */
{
	static	char	*ermes[] = {
 /*  0 */	"Unprintable error",
		"Undefined variable was found",
	};

	if(n >= numberof(ermes)) n = 0;

	fprintf(stderr, "\r\nromkan: Taio-hyo %s%s ---\r\n", cur_rk_table->tcurdir, cur_rk_table->tcurfnm);
	fprintf(stderr, "Error No.%d: %s\r\n", n, ermes[n]);
	longjmp(cur_rk_table->env0, 1);
}
