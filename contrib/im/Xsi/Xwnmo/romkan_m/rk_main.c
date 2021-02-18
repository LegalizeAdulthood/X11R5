/*
 * $Id: rk_main.c,v 1.2 1991/09/16 21:34:46 ohm Exp $
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
			rk_main.c
						87.12. 3  改 正

	ローマ字かな変換・その他、入力コードを変換するプログラム。
	ASCIIコード使用を、前提としている。
	このファイルは、変換のメインルーチン。
***********************************************************************/
/*	Version 3.1	88/06/14	H.HASHIMOTO
 */
#ifndef OMRON_LIB
#include "rk_header.h"
#include "rk_extvars.h"
#ifdef	MULTI
#include "rk_multi.h"
#endif	/*MULTI*/
#endif
#include "rext.h"

#ifdef	MULTI
#define	DISOUT ((cur_rk->flags & RK_NONISE) ? cur_rk->rk_output : cur_rk->disout)
 /* フラグの値はromkan_init3()の中で設定され、そのRK_NONISEのビットの値の
    標準は0。従ってDISOUTの標準値はdisout。これをrk_outputに変えると、偽コード
    （除：LTREOF）を一切出さない（但し、キー入力があれば必ず何かを返すように
    してある時に限り、EOLTTRをも返す）。*/

#else	/* !MULTI */
#ifdef KDSP
#	ifdef MVUX
letter	displine[DSPLIN]; /* デバッグ用  表示の一行分を記憶 */
#	endif
#endif

/* 88/06/02 V3.1 */
static	letter	nil[1] = {EOLTTR};
static	letter	*curdis;
static	int	codein_len;
static	letter	*codeout, *remainkbf;
static	letter	ungetc_buf = EOLTTR; /* romkan_ungetcが一文字退避しておくバッファ */
static	letter	unnext_buf = EOLTTR; /* romkan_unnextが		   〃		  */

static	letter	(*keyin_method)(); /* キーイン関数のアドレス */
static	int	(*bytcnt_method)(); /* バイトカウント関数のアドレス */
static	int	(*kbytcnt_method)(); /* キー入力解釈用バイトカウント関数のアドレス */
 
/* モード表名（又はそのパス名）を保存 */ /* 初期設定がいい加減! */
static	char	prv_modfnm[REALFN] = "\0";

#define	DISOUT ((flags & RK_NONISE) ? rk_output : disout)
 /* フラグの値はromkan_init3()の中で設定され、そのRK_NONISEのビットの値の
    標準は0。従ってDISOUTの標準値はdisout。これをrk_outputに変えると、偽コード
    （除：LTREOF）を一切出さない（但し、キー入力があれば必ず何かを返すように
    してある時に限り、EOLTTRをも返す）。*/

static /* V3.1 */
char	eofflg; /* romkan_next()からLTREOFが来たときに、romkan_henkan()内で1
		   にする。これの値が非0なら変換対応表の(error)を無視して、
		   本処理バッファ内の未確定コードも、強制的に変換させる。更に
		   LTREOFが来た時に特別にある出力を出すように指定されていれば
		   その処理も行う（但し、その処理は急ごしらえで不完全）。その
		   処理中はeofflgは2。*/

static	letter	evalbuf[2][2][OUTSIZ];
static	letter	delchr, delchr2 = EOLTTR, nisedl; /* DELのキャラクタは二個まで持てる*/

/* 88/05/31 V3.1 */
static	letter	rk_input;  /* 入力。3バイトまでは変更なしに対応可 */
static	letter	disout[OUTSIZ];  /* 本処理からの出力のバッファ */
static	letter	rk_output[OUTSIZ]; /* 後処理からの出力（最終出力）のバッファ */
static	letter	keybuf[KBFSIZ], urabuf[KBFSIZ];  /* 本処理バッファとその退避領域 */
static	int	lastoutlen, lastkbflen;
static	int	hyonum;
#endif	/* MULTI */

static void romkan_delete(), mchevl(), add_at_eof(), free_for_all_area(),
  ltrevlcpy(), set_rubout(), maeato_henkan(), codeout_chg();

/* letterを返す関数の定義及び宣言（一部にはcharのものも混じっている） */

static int	head_bytecount(), p_eq(), hen_ikisrc(), match(),
  henkan_ok(), romkan_restart();
static letter	mchedsrc();
letter	romkan_next(), romkan_unnext();
letter	romkan_getc(), romkan_ungetc(), *romkan_henkan();
letter	to_zenalpha(), to_zenhira(), to_zenkata();
 /* to_zenhira;kata は濁点を持つ文字を一まとめにしない。*/

static letter	*ltrcpy(lp1, lp2)
fast	letter	*lp1, *lp2;	/* V3.1 */
{
	fast	letter	*org;	/* V3.1 */

	org = lp1;
	while((*lp1++ = *lp2++) != EOLTTR);
	return(org);
}

static letter	*ltrncpy(lp1, lp2, n) /* 末尾に'EOLTTR'をセット */
letter	*lp1, *lp2;
int	n;	
{
	fast	letter	*org;

	org = lp1;
	for(; n; --n) if(EOLTTR == (*lp1++ = *lp2++)) return(org);
	*lp1 = EOLTTR;
	return(org);
}

 /** letterの列の末尾に一文字つなげる */
static /* V3.1 */
letter	*ltr1cat(lp, l)
fast	letter	*lp, l;	/* V3.1 */
{
	fast	letter	*org;	/* V3.1 */

	org = lp;
	totail(lp);
	*lp++ = l;
	*lp = EOLTTR;
	return(org);
}

 /** 文字列の末尾に一文字つなげる。ltr1catのchar版 */
#ifdef OMRON_LIB
static
#endif
char	*chrcat(s, c)
fast	char	*s, c;	/* V3.1 */
{
	fast	char	*org;	/* V3.1 */

	org = s;
	strtail(s);
	*s++ = c;
	*s = '\0';
	return(org);
}

static letter	*ltrcat(lp1, lp2)
fast	letter	*lp1, *lp2;	/* V3.1 */
{
	fast	letter	*org;	/* V3.1 */

	org = lp1;
	totail(lp1);
	ltrcpy(lp1, lp2);
	return(org);
}

 /** letterの列lp2の各要素の最上位ビットを立ててから、lp1の後ろにつなげる。*/
static /* V3.1 */
letter	*bitup_ltrcat(lp1, lp2)
fast	letter	*lp1, *lp2;	/* V3.1 */
{
	fast	letter	*org;	/* V3.1 */

	org = lp1;
	totail(lp1);

	while((*lp1 = *lp2++) != EOLTTR) *lp1++ |= HG1BIT;
	 /** lp2 の要素全ての最上位ビットを立てる。*lp2がNISEBP(rk_spclval.h
	     でdefine)などのときは最初から最上位が立ってるからよい */

	return(org);
}

 /** ltrcatしたあと、結果の文字列の末尾にポインタをもっていく。*/
#ifdef OMRON_LIB
static
#endif
letter	*ltrgrow(lp1, lp2)
fast	letter	*lp1, *lp2;	/* V3.1 */
{
	totail(lp1);
	while((*lp1 = *lp2++) != EOLTTR) lp1++;
	return(lp1);
}

int
ltrlen(lp)
fast	letter	*lp;	/* V3.1 */
{
	fast	letter	*org;	/* V3.1 */

	for(org = lp; *lp != EOLTTR; lp++);
	return(lp - org);
}

 /**	letterの列の最後の文字へのポインタを返す。但し空文字列に対しては
	そのまま返す。*/
static
letter	*ltrend(lp)
fast	letter	*lp;	/* V3.1 */
{
	return((*lp != EOLTTR) ? (lp + ltrlen(lp) - 1) : lp);
}

 /**	文字列の最後の文字へのポインタを返す。ltrend()のchar版。ltrend()と
	同様、空文字列のときは特例がある。*/
#ifdef OMRON_LIB
static
#endif
char	*strend(s)
fast	char	*s;	/* V3.1 */
{
	return(*s ? (s + strlen(s) - 1) : s);
}

int
ltrcmp(lp1, lp2)
fast	letter	*lp1, *lp2;	/* V3.1 */
{
	for(; *lp1 == *lp2; lp1++, lp2++) if(*lp1 == EOLTTR) return(0);
	return(*lp1 > *lp2 ? 1 : -1);
}

#ifdef nodef
int
ltrncmp(lp1, lp2, n)
letter	*lp1, *lp2;
int	n;	
{
	for(; n && *lp1 == *lp2; n--, lp1++, lp2++)
		if(*lp1 == EOLTTR) return(0);
	return(n == 0 ? 0 : (*lp1 > *lp2 ? 1 : -1));
}

/* 88/06/14 V3.1 */
/******************************************************************************/
#ifndef OMRON
letter	*ltr_index(lp, l)
letter	l, *lp;
{
	for(; *lp != EOLTTR; lp++) if(*lp == l) return(lp);
	return(NULL);
}
#endif
/******************************************************************************/
#endif

static /* V3.1 */
letter	*ltr_rindex(lp, l)
fast	letter	l, *lp;	/* V3.1 */
{
	fast	letter	*lp2;	/* V3.1 */

	for(lp += ltrlen(lp2 = lp); lp2 != lp; ) if(*--lp == l) return(lp);
	return(NULL);
}

 /** 文字列の末尾一文字をカットする。*/
/*static V3.1 */
letter	*ltr1cut(lp)
fast	letter	*lp;	/* V3.1 */
{
	fast	int	l;	/* V3.1 */

	if(0 == (l = ltrlen(lp))) BUGreport(0); /* Illegal POP */
	*(lp + --l) = EOLTTR;
	return(lp);
}

 /** letterの一文字を長さ１の文字列に変換する */
static /* V3.1 */
letter	*ltr_to_ltrseq(lp, l)
letter	*lp, l;
{
	*lp++ = l;
	*lp-- = EOLTTR;
	return(lp);
}

 /** charの列からletterを一つ取り出す。但し'\0'はEOLTTRに変える */
#ifdef OMRON_LIB
static
#endif
letter	letterpick(lbfpptr)
uns_chr	**lbfpptr;
{
	letter	l = 0;
	fast	int	i;	/* V3.1 */

	for(i = (*(cur_rk_table->bytcnt_method))(*lbfpptr); i; i--)
		l = (l << 8) + *(*lbfpptr)++;
	return(l == 0 ? EOLTTR : l);
}

 /**	入力されたコードを文字単位にまとめる。但しEOFはLTREOF（rk_spclval.h
	にて定義）に変える。*/
letter	romkan_next()
{
	letter	in;
	int	i, n;
	uns_chr	c;

	  /* unnextされている場合は、それを取り出す。*/
	if(cur_rk->unnext_buf != EOLTTR)
		return(in = cur_rk->unnext_buf, cur_rk->unnext_buf = EOLTTR, in);

	if( (letter)EOF == (in = (*(cur_rk->keyin_method))() )) return(LTREOF);
	c = (in &= 0xff);
	n = (*(cur_rk->kbytcnt_method))(&c);
	for(i = 1; i < n; i++) in = (in << 8) + ((*(cur_rk->keyin_method))() & 0xff);
	return(in);
}

 /*	letterの文字 l とcharの文字 c の比較。エンドマーク同士（letter列の
	エンドマークはEOLTTR、char列のは'\0'）も一致とみなす。*/
#define	ltrchreq(l, c) ((l) == (c) && (c) != 0 || (l) == EOLTTR && (c) == 0)

 /** letterの文字列とcharの文字列の比較 但し大小比較は符号なしとしてやる */
#ifdef OMRON_LIB
static
#endif
int
ltrstrcmp(l, s)
fast	 letter	*l;	/* V3.1 */
fast	 char	*s;	/* V3.1 */
{
	for(; ltrchreq(*l, *s); l++, s++) if(*s == 0) return(0);
	return((*l < (uns_chr)*s || *l == EOLTTR)? -1 : 1);
}

 /** 最後にマッチした文字 つまりurabufの最後。urabufが空だとEOLTTRを返す */
static /* V3.1 */
letter	lastmch()
{
	return(*(ltrend(cur_rk->urabuf)));
}

/* end of 'letter' functions */


 /** 変換バッファのクリア */
 /* 注意！ romkan_clear()を呼ぶ時は、必ずその前に、cur_rkをセットすること */
void	romkan_clear()
{
#ifdef KDSP
#	ifdef MVUX
	*displine = EOLTTR;
#	endif
#endif
	cur_rk->ungetc_buf = *(cur_rk->keybuf) = *(cur_rk->urabuf) = *(cur_rk->disout) = *(cur_rk->rk_output) = EOLTTR;
	cur_rk->curdis = DISOUT;
	cur_rk->lastoutlen = cur_rk->lastkbflen = 0;
}

 /**	WnnV4.0マルチクライアント用のRomkan構造体を初期化する **/
Romkan *
rk_buf_init()
{
	register Romkan	*p;
	register int i;

	if ((p = (Romkan *)malloc(sizeof(Romkan))) == NULL) {
		return(NULL);
	}
	malloc_for_henmatch(&(p->rk_henmatch),
			    cur_rk_table->rk_hensuudef.count);
	malloc_for_modesw(&(p->rk_modesw),
			  cur_rk_table->rk_modesw.max);
	p->rk_modesw.max = cur_rk_table->rk_modesw.max;
	p->rk_modesw.count = cur_rk_table->rk_modesw.count;
	for (i = 0; i < cur_rk_table->rk_modesw.max; i++) {
		p->rk_modesw.point[i] = cur_rk_table->rk_modesw.point[i];
	}
	malloc_for_usehyo(&(p->rk_usehyo), cur_rk_table->rk_usehyo.size);
	p->rk_usehyo.size = cur_rk_table->rk_usehyo.size;
	for (i = 0; i < (cur_rk_table->rk_usehyo.size + 1); i++) {
		p->rk_usehyo.usemaehyo[i] = cur_rk_table->rk_usehyo.usemaehyo[i];
		p->rk_usehyo.usehyo[i] = cur_rk_table->rk_usehyo.usehyo[i];
		p->rk_usehyo.useatohyo[i] = cur_rk_table->rk_usehyo.useatohyo[i];
	}
	p->rk_errstat = NULL;
	p->flags = 0;
	p->nil[0] = EOLTTR;
	p->ungetc_buf = EOLTTR;
	p->unnext_buf = EOLTTR;
	p->delchr2 = EOLTTR;
	p->ebf_sw = NULL;
	p->oneletter[0] = p->oneletter[1] = EOLTTR;
	p->keybuf[0] = p->urabuf[0] = p->disout[0] = p->rk_output[0] = EOLTTR;
	p->rk_table = (RomkanTable *)NULL;
	return(p);
}

RomkanTable *
rk_table_buf_init()
{
	RomkanTable	*p;
	if ((p = (RomkanTable *)malloc(sizeof(RomkanTable))) == NULL) {
		return(NULL);
	}
	p->flags = 0;
	p->prv_modfnm[0] = NULL;
	return(p);
}


void
rk_close(p)
Romkan	*p;
{
	/*free_for_all_area();*/	/* V3.1 */
	free_for_modesw(&(p->rk_modesw));
	free_for_henmatch(&(p->rk_henmatch));
	free_for_usehyo(&(p->rk_usehyo));
	free(p);
}

 /**	表を読み込んで変換の初期設定をする。（part 3）*/
 /* WnnV4.0マルチクライアントの場合
	romkan_init3は、Romkan構造体へのポインタを返します。
	その後、romkanをする場合は、グローバル変数cur_rkに
	そのポインタを入れなければならない。 
	但し、romkan_init3()は、内部で一時的にcur_rkをセットし、
	returnする時に元の値に戻します。 */

/* 88/06/13 V3.1 */
 /** 変換表のクリア */
static /* V3.1 */
void
romkan_reset()
{
	if (cur_rk_table->rk_modenaibu.org != NULL) {
		cur_rk_table->rk_modenaibu.org[0] = 0;	/* Terminator */
	}
	choosehyo();
	romkan_clear();
}


RomkanTable *
romkan_table_init(p, modhyo, bytcntfn, flags_)
RomkanTable *p;
char	*modhyo;	/* モード定義表の名又はそのパス名 */
int	(*bytcntfn)();	/* バイトカウント関数 */
int	flags_;
{
	int	head_bytecount(); /* デフォルトのバイトカウント関数 */
	int	errcod; /* 今の所1のみ */
	RomkanTable	*tmp_cur_rk_table;
	RomkanTable	*rk_table_buf;

	tmp_cur_rk_table = cur_rk_table;
	if (p == NULL) {
		if ((rk_table_buf = (RomkanTable *)rk_table_buf_init()) == NULL) {
			return(NULL);
		}
	} else {
		rk_table_buf = p;
	}
	cur_rk_table = rk_table_buf;
	cur_rk_table->bytcnt_method = (bytcntfn == NULL ? head_bytecount : bytcntfn);
	/* 88/05/30/ V3.1 */
	if (modhyo == NULL)
		modhyo = "";
	strcpy(cur_rk_table->prv_modfnm, modhyo);	/* V3.1 */
	if((errcod = setjmp(cur_rk_table->env0)) != 0){
		/* 88/06/13 V3.1 */
		free_for_all_area();	/* V3.1 */
		fprintf(stderr,"romkan_init failed.");
	} else {
		readdata(modhyo);	/* 88/06/08 V3.1 */
	}
	cur_rk_table = tmp_cur_rk_table;
	if (!errcod) {
		return(rk_table_buf);
	} else {
		free(rk_table_buf);
		return(NULL);
	}
}


 /**	表を読み込んで変換の初期設定をする。（part 3）*/

Romkan *
romkan_init3
(delchr_, nisedl_, delchr2_, keyinfn, bytcntfn, kbytcntfn, restartf,
 flags_)
letter	delchr_;	/* DELとして使うコード */
letter	nisedl_;	/* 偽DELとして使うコード */
letter	delchr2_;	/* DELとして使うコードが二つある場合そのもう一方 */
letter	(*keyinfn)();	/* キーイン関数 */
int	(*bytcntfn)();	/* バイトカウント関数 */
int	(*kbytcntfn)();	/* キーインの解釈用バイトカウント関数。NULL指定の時は
			   bytcntfnと同じものが使われる */
char	restartf;
 /*	romkan_restart()内から呼ばれた時に立つフラグ。これが立つと、バッファを
	クリアせずに済ます。この時はエラー検出時のメッセージも少し異なる */
int	flags_;
 /* 以下のフラグが利用可。これらはrk_spclval.hにて定義。
	RK_CHMOUT:モードチェンジを知らせるコードを返すか？
	RK_KEYACK:キーインに対し必ず何かを返すか
	RK_NONISE:偽コードを出さないようにするか
	RK_REDRAW:Wnn用特殊フラグ（redraw用のフラグを出すかどうか）
	RK_SIMPLD:deleteの動作を単純にするか
 */
{
	Romkan	*tmp_cur_rk;
	Romkan	*rk_buf;

	tmp_cur_rk = cur_rk;

	if(setjmp(cur_rk_table->env0) != 0){
		return(NULL);
	}

	if ((rk_buf = (Romkan *)rk_buf_init()) == NULL) {
		return(NULL);
	}
	cur_rk = rk_buf;
	 /* 偽コードを出すかの設定はromkan_clearで使うので、フラグの設定を
	    先にやっておかないといけない。*/
	cur_rk->flags = flags_;
	cur_rk->curdis = DISOUT;


	cur_rk->keyin_method = keyinfn;
	cur_rk->bytcnt_method = (bytcntfn == NULL ? head_bytecount : bytcntfn);
	cur_rk->kbytcnt_method = (kbytcntfn == NULL ? cur_rk->bytcnt_method : kbytcntfn);

	cur_rk->delchr = delchr_;
	cur_rk->nisedl = nisedl_;
	cur_rk->delchr2 = delchr2_;
 /**	実はDELのキャラクタを二個まで持てる。二個目：delchr2は、未設定の
	とき（値がEOLTTR）は無視される。それ以外は、delchr2が入力されると
	delchrが入力されたのと同じ振る舞いをする。*/

	choosehyo();
	cur_rk = tmp_cur_rk;
	return(rk_buf);
}

#define ifflg(a, b) ((a) ? (b) : 0)

 /**	表を読み込んで変換の初期設定をする。（part 2: キー入力に対し必ず何か
	返すようにするか、キーバッファをクリアするかどうかなども指定可）*/

Romkan *
romkan_init2
(delchr_, chmoutf, keyinfn, bytcntfn, keyackf, restartf, nonisecodf)
char	chmoutf, keyackf, restartf, nonisecodf;
letter	delchr_;
letter	(*keyinfn)();
int	(*bytcntfn)(); /* 引数の詳細はromkan_init3を参照 */
{
	return(romkan_init3(delchr_, toNISE(delchr_), EOLTTR,
			    keyinfn, bytcntfn, (int (*)())NULL,
			    restartf,
			    ifflg(chmoutf, RK_CHMOUT) |
			    ifflg(keyackf, RK_KEYACK) |
			    ifflg(nonisecodf, RK_NONISE)));
}

 /**	表を読み込んで変換の初期設定をする。（part 1）*/
Romkan *
romkan_init(delchr_, chmoutf, keyinfn, bytcntfn)
char	chmoutf;
letter	delchr_;
letter	(*keyinfn)();
int	(*bytcntfn)(); /* 引数の詳細はromkan_init3を参照 */
{
	return(romkan_init2(delchr_, chmoutf, keyinfn, bytcntfn,
			    0, 0, 0));
}


 /**	バイトカウント関数のデフォルト。sの指している所に入っているのが
	何バイトコードかを返す。
	romkan_nextに注意。そちらでは、バイトカウント関数の値は
	引数の一文字目にのみ依存すると仮定している。*/
static /* V3.1 */
int
head_bytecount(s)
uns_chr	*s;
{
#ifdef IKIS
	return((*s <= 0xa0 || *s == 0xff)? 1 : 2);
#else
	return((*s <= 0xa0 && *s != HNKAK1 || *s == 0xff)? 1 : 2);
#endif
}

 /** 変換された文字を順次返す */
letter	romkan_getc()
{
	fast	letter	l;	/* V3.1 */

	  /* ungetcされている場合は、それを取り出す。*/
	if(cur_rk->ungetc_buf != EOLTTR)
		return(l = cur_rk->ungetc_buf, cur_rk->ungetc_buf = EOLTTR, l);

	while(*(cur_rk->curdis) == EOLTTR){
		 /* romkan_next()の値がLTREOFの時も、そのまま送れば良い。*/
		cur_rk->curdis = romkan_henkan(romkan_next());

		 /* keyackflgが非0なら、キーインがあれば必ず何か返す。その
		    ため、返すべきものがない時はEOLTTRを返すことにする。*/
		if(cur_rk->flags & RK_KEYACK) break;
	}

	if(EOLTTR != (l = *(cur_rk->curdis))) cur_rk->curdis++;
	return(l);
	 /* 偽物の文字なら、HG1BITが立っている。*/
	 /* 特別なコード(例えば偽のBEEPとしてNISEBP)を返すときがある。それらは
	    rk_spclval.hに定義されている。*/
}

 /**	romkan_getcの下位関数として、入力を一文字受け取って
	変換結果の文字列を出力する。*/
letter	*romkan_henkan(mae_in)
letter	mae_in;
{
	letter	mae_out[2], *p;

	/* if(*(cur_rk->curdis) != EOLTTR){p = cur_rk->curdis; cur_rk->curdis = cur_rk->nil; return(p);} */
	/* ↑これではmae_inが無視される */
	cur_rk->curdis = cur_rk->nil;

	  /* 前・後処理への入力は常にただ一文字 */
	mae_out[0] = mae_out[1] = EOLTTR;

	cur_rk->eofflg = cur_rk->rk_errstat = 0;
	*(cur_rk->rk_output) = *(cur_rk->disout) = EOLTTR;

	maeato_henkan(mae_in, mae_out, cur_rk->rk_usehyo.usemaehyo);
	cur_rk->rk_input = *mae_out;

	if(cur_rk->rk_input == LTREOF){
		 /* LTREOFが来た場合、(error)を無視し、本処理バッファの末尾迄
		    強制変換する。そのためにeofflgを1にする。その後、結果の
		    末尾に、LTREOFが来た時の特別コード（指定されていれば）と、
		    LTREOFをつなぐ。*/
		cur_rk->eofflg = 1;
		match();

		cur_rk->eofflg = 2;
		add_at_eof(); /* codeoutに、LTREOFが来た時出すコードが入る。*/
		ltr1cat(cur_rk->codeout, LTREOF);

		ltrcat(cur_rk->disout, cur_rk->codeout);
		ltrcat(cur_rk->rk_output, cur_rk->codeout);
	} else if(cur_rk->rk_input == EOLTTR){
		 /* EOLTTRが来た場合も、上と同様の処理を行うが、LTREOFは
		    つながない。なお、これはromkan_getc()を呼んでいる時は
		    起こらない（romkan_next()がEOLTTRを返さないから）。*/
		cur_rk->eofflg = 1;
		match();
	} else if(cur_rk->rk_input == cur_rk->delchr
		|| cur_rk->rk_input == cur_rk->delchr2){
	    /* delchr2が未設定ならその値はEOLTTRなのでrk_inputと等しくない。*/
		romkan_delete(cur_rk->rk_input);	/* V3.1 */
	} else {
		ltr1cat(cur_rk->keybuf, cur_rk->rk_input);
		ltr1cat(cur_rk->disout, toNISE(cur_rk->rk_input));
		match();
	}

	if(!(cur_rk->flags & RK_CHMOUT)){
	   /* chmoutflgが0の時は、CHMSIGを出さない。*/
		for(p = DISOUT; ; p++){
			while(*p == CHMSIG) ltrcpy(p, p + 1);
			if(*p == EOLTTR) break;
		}
	}

	if((cur_rk->flags & RK_REDRAW)
	&& NULL != (p = ltr_rindex(cur_rk->disout, cur_rk->nisedl))){
		for(p++; *p != EOLTTR || (ltr1cat(cur_rk->disout, REDRAW), 0); p++)
			if(!isSPCL(*p)) break;
	}
	 /* wnnのredrawのフラグが立っていて、disoutがnisedl以後特殊コードのみ
	    で終わっていたら、REDRAWを出して、Wnnに変換行のredrawをさせる。*/

	return(DISOUT);
}


/******************************************************************************/
 /* デバッグ用関数 */
#ifdef	KDSP
#	ifdef	MVUX

pridbg2(a, b, c)
char	*a, *c;
letter	*b;
{
	printf("%s", a); dump_fordbg(b); printf("%s", c);
}

ltr_displen(l)
letter	l;
{
	while(l >= 256) l >>= 8;

	if(l == 0) return(0);
	if(l < ' ') return(0);
	if(168 <= l && l <= 170) return(1);
	return(l > 160 ? 2 : 1);
}

dump_fordbg(lp)
letter	*lp;
{
	while(*lp != EOLTTR) printf("%x/", *lp++);
}

print_fordbg(lp)
letter	*lp;
{
	while(*lp != EOLTTR) putletter(*lp++ & ~HG1BIT);
}

print_ltrseq(lp)
letter	*lp;
{
	while(*lp != EOLTTR) print_ltr(*lp++ & ~HG1BIT);
}

print_ltr(l)
letter	l;
{
	letter	*disptail;
	int	i;

	if(!isSPCL(l)) l &= ~HG1BIT;

	if(l == CHMSIG)
#ifdef CHMDSP
		printf("...mode=%s\n",romkan_dispmode())
#endif
		;
	else
#define BEEPCH	'\007'
#define NEWLIN	'\n'
	if(l == BEEPCH || l == NISEBP) putchar(BEEPCH);
	else
	if(l == NEWLIN){
		*(cur_rk->displine) = *(cur_rk->keybuf) = *(cur_rk->urabuf) = EOLTTR;
		cur_rk->lastoutlen = cur_rk->lastkbflen = 0;
		putchar(l);
	}
	else
	if(l == cur_rk->delchr || l == cur_rk->nisedl){
		if(*(cur_rk->displine) == EOLTTR) putchar(BEEPCH);
		 else {
			disptail = ltrend(cur_rk->displine);
			for(i = ltr_displen(*disptail); i; i--)
				printf("\031 \031");
			*disptail = EOLTTR;
		}
	}
	else {
		ltr1cat(cur_rk->displine, l);
		putletter(l);
	}
}

#	endif /* of #ifdef MVUX */
#endif
/* デバッグ用関数終わり */


/* 88/06/10 V3.1 */
 /** DELが入力されたときの処理をする */
static /* V3.1 */
void
romkan_delete(input_del)
letter	input_del;
{
	if(ltrlen(cur_rk->keybuf) > ((cur_rk->flags & RK_SIMPLD) ? 0 : cur_rk->lastkbflen)){
		ltr1cut(cur_rk->keybuf);
		set_rubout(cur_rk->disout, 1, cur_rk->nisedl);
	} else {
		if(*(cur_rk->urabuf) != EOLTTR && !(cur_rk->flags & RK_SIMPLD)){
			ltr1cut(ltrcpy(cur_rk->keybuf, cur_rk->urabuf));
			*(cur_rk->urabuf) = EOLTTR;

			set_rubout(cur_rk->rk_output, cur_rk->lastoutlen, input_del);

			set_rubout(cur_rk->disout, cur_rk->lastkbflen, cur_rk->nisedl);
			bitup_ltrcat(ltrcat(cur_rk->disout, cur_rk->rk_output), cur_rk->keybuf);

			cur_rk->lastkbflen = cur_rk->lastoutlen = 0;
		} else {
			set_rubout(cur_rk->disout, 1, input_del);
			set_rubout(cur_rk->rk_output, 1, input_del);
		}
	}
}

 /**	letterの列 lp1 と lp2 のマッチを試みる。返値は、lp1がlp2の頭部と
	一致の時 -1、lp1またはその頭部とlp2が一致のときは一致長（lp2が
	空文字列の時を含む。この場合返値は0）、それ以外は -2。
	lp2側に式が含まれていたら評価をする。lp1側には式を含んではだめ */
static /* V3.1 */
int
prefixp(lp1, lp2)
letter	*lp1, *lp2;
{
	/* 1行マッチさせるごとに、まずmatch情報をクリアしてから。
	   つまり、rk_henmatch.point[0].ltrmch = EOLTTR; としておく。*/

	fast	 int	mch_len = 0, d_len;	/* V3.1 */

	for(;;){
		if(*lp2 == EOLTTR) return(mch_len);
		if(*lp1 == EOLTTR) return(-1);
		if((d_len = p_eq(&lp2, &lp1)) < 0) return(d_len);
		mch_len += d_len;
	}
}

 /** num番目の変数が既にある文字とマッチしていると仮定して、その文字を返す */
static /* V3.1 */
letter	mchedsrc(num)
fast	int	num;	/* V3.1 */
{
	fast	matchpair	*pairptr;	/* V3.1 */

	if (cur_rk->rk_henmatch.size <= 0) return(0);
	for(pairptr = cur_rk->rk_henmatch.point; pairptr -> ltrmch != EOLTTR; pairptr++){
		if(num == pairptr -> hennum)
			return(pairptr -> ltrmch);
	}
	return(BUGreport(8), 0);
}

 /**	num番目の変数が文字 l とマッチするか調べる。その変数がunboundだった
	場合は l にbindする。マッチしたら(bindの時を含む)1、しないと0を返す */
static /* V3.1 */
int
mchsrc(num, l)
fast	int	num;	/* V3.1 */
fast	letter	l;	/* V3.1 */
{
	fast	matchpair	*pairptr;	/* V3.1 */

	if(hen_ikisrc(num, l) == 0) return(0);
	if (cur_rk->rk_henmatch.size <= 0) return(0);
	for(pairptr = cur_rk->rk_henmatch.point; pairptr -> ltrmch != EOLTTR; pairptr++){
		if(num == pairptr -> hennum)
			return(pairptr -> ltrmch == l);
	}

	pairptr -> ltrmch = l;
	pairptr -> hennum = num;
	(++pairptr) -> ltrmch = EOLTTR;
	return(1);
}

 /**	l1pから一単位を取って評価し、文字l2と一致するかどうかを返す。評価した
	結果が一文字にならなかったら、当然一致しない。*/
static /* V3.1 */
int
l_eq(l1p, l2)
letter	*l1p, l2;
{
	letter	evlrsl[RSLMAX];

	switch(SHUBET(*l1p)){
		case 0:	return(*l1p == l2);
		case 1:	return(mchsrc((int)LWRMSK(*l1p), l2));
		case 2:
			mchevl(&l1p, evlrsl);
			return(evlrsl[0] == l2 && evlrsl[1] == EOLTTR);
		default:
			return(BUGreport(1), 0);
	}
}

 /**	prefixp内で使用  但し引数の順序は逆、すなわち式が含まれうるのはl1pp側
	のみ。l1ppから一単位ぶん取って評価したものとl2ppのマッチを試みる。それ
	がl2ppまたはその頭部とマッチすれば一致長を返し（l1ppの評価結果が空文字
	列の時を含む。この場合返値は0）、逆にl1ppの評価結果の頭部とl2ppがマッ
	チした時は -1を返す。マッチが失敗したら返値は -2。*/
static /* V3.1 */
int
p_eq(l1pp, l2pp)
fast	 letter	**l1pp, **l2pp;	/* V3.1 */
{
	int	num;
	letter	evlrsl[RSLMAX], *rslptr;
	fast	 int	retval = -2;	/* V3.1 */

	 /*  l2pp側には式を含まない筈 */
	if(!is_HON(**l2pp)){
	    /*	if(is_HON(**l1pp)) retval = p_eq(l2pp, l1pp); else  */
		BUGreport(9);
	} else {
		switch(SHUBET(**l1pp)){
			case 0:	/* 文字同士 */
				retval = (*(*l1pp)++ == *(*l2pp)++ ? 1 : -2);
				break;
			case 1:	/* 変数と文字 */
				num = LWRMSK(*(*l1pp)++);
				retval = (mchsrc(num, *(*l2pp)++) ? 1 : -2);
				break;
			case 2:	/* 式と文字 */
				mchevl(l1pp, rslptr = evlrsl);
				for(retval = 0; *rslptr != EOLTTR; retval++){
					if(**l2pp == EOLTTR){
						retval = -1; break;
					} else if(*rslptr++ != *(*l2pp)++){
						retval = -2; break;
					}
				}
				break;
			default:
				BUGreport(2);
		}
	}

	return(retval);
}

 /** l1pから一単位評価してl2pに入れる */
#ifdef	MULTI


#define XY2INT(X, Y) (((X) << 24) | (Y)) /* これが１回しかYを評価しないことに
	依存して書いてある箇所がある  注意！ */

 /** l1pから一単位評価してl2pに入れる */
static
void
mchevl(l1pp, l2p)
letter	**l1pp, *l2p;
{
	letter	*l1p, tmpevl[RSLMAX];

	l1p = *l1pp;

	switch(SHUBET(*l1p)){
		case 0: *l2p++ = *l1p++; break;
		case 1: *l2p++ = mchedsrc((int)LWRMSK(*l1p++)); break;
		case 2:	 /* toupper, tolower, error, … */
			switch(LWRMSK(*l1p++)){
				case 2: mchevl(&l1p, tmpevl);
					*l2p++ = to_upper(*tmpevl);
					break;
				case 3: mchevl(&l1p, tmpevl);
					*l2p++ = to_lower(*tmpevl);
					break;
				case 4:
					*l2p++ = CHMSIG;
				        *l2p++ = *l1p++;
					*l2p++ = 0; break; /* EOLではない */
				case 5:
					*l2p++ = CHMSIG;
				        *l2p++ = *l1p++;
					*l2p++ = 1; break;
				case 6:
					*l2p++ = CHMSIG;
					*l2p++ = *l1p++;
					*l2p++ = 2; break;
				case 7:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_updown(*tmpevl);
					break;
				case 8:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_zenalpha(*tmpevl);
					break;
				case 9:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_hira(*tmpevl);
					break;
				case 10:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_kata(*tmpevl);
					break;
				case 11:
					mchevl(&l1p, tmpevl);
					to_hankata(*tmpevl, &l2p);
					break; /* 特殊 */
				case 12:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_zenhira(*tmpevl);
					break;
				case 13:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_zenkata(*tmpevl);
					break;
				case 14:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p += *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 15:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p -= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 16:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p *= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 17:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					if(! *tmpevl) *l2p = LTRHUG;
					 else *l2p /= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 18:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					if(! *tmpevl) *l2p = LTRHUG;
					 else *l2p %= *tmpevl;
					LWRCUT(*l2p++);
					break;
				 /* 19〜21・30は、条件を満たすと空文字列、
				    そうでないとUNUSDCを文字列として返す。*/
				case 19:
					mchevl(&l1p, tmpevl);
					if(lastmch() != *tmpevl)
						*l2p++ = UNUSDC;
					break;
				case 20:
					if(! (cur_rk->rk_modesw.point)[LWRMSK(*l1p++)])
						*l2p++ = UNUSDC;
					break;
				case 21:
					if(cur_rk->rk_modesw.point[LWRMSK(*l1p++)])
						*l2p++ = UNUSDC;
					break;
				case 22:
					*l2p++ = REASIG;
					break;
				case 23:
					*l2p++ = cur_rk->delchr;
					break;
				case 24:
					*l2p++ = CHMSIG;
					*l2p++ = 0; /* これで「all」を表す */
					*l2p++ = 0; break;
				case 25:
					*l2p++ = CHMSIG;
					*l2p++ = 0;
					*l2p++ = 1; break;
				case 26:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p &= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 27:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p |= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 28:
					mchevl(&l1p, tmpevl);
					*l2p = ~(*tmpevl);
					LWRCUT(*l2p++);
					break;
				case 29:
					*l2p++ = URBFCL;
					break;
				case 30:
					if(cur_rk->eofflg != 2 || *(cur_rk->keybuf) != EOLTTR)
						*l2p++ = UNUSDC;
					break;
				case 31:
					{
						letter	code, basenum;

						mchevl(&l1p, tmpevl);
						code = *tmpevl;
						mchevl(&l1p, tmpevl);
						if((basenum = *tmpevl) <= 1 ||
						   BASEMX < basenum)
							basenum = 10;
						to_digit(code, basenum, &l2p);
					}
					break;
				case 32:
					mchevl(&l1p, tmpevl);
					dakuadd(*tmpevl, &l2p);
					break; /* 特殊 */
				case 33:
					mchevl(&l1p, tmpevl);
					handakuadd(*tmpevl, &l2p);
					break; /* 特殊 */
				case 34:
					mchevl(&l1p, tmpevl);
					*l2p++ = ltov(*tmpevl);
					break;
				case 35:
					*l2p++ = ERRCOD;
					break;
			     /*	case 36: omitted */
				case 37:
					*l2p++ = CHMSIG;
/*
					*l2p++ = LWRMSK(*l1p++);
*/
				        *l2p++ = *l1p++;
					*l2p++ = *l1p++; break;
				case 38:
					*l2p++ = CHMSIG;
					*l2p++ = XY2INT(2, LWRMSK(*l1p++));
					*l2p++ = *l1p++; break;
				case 39:
					*l2p++ = CHMSIG;
					*l2p++ = XY2INT(3, LWRMSK(*l1p++));
					*l2p++ = *l1p++; break;
				case 40:
					{
						letter	modnum;

						modnum = LWRMSK(*l1p++);
						if(cur_rk->rk_modesw.point[modnum] !=
						   *l1p++)
							*l2p++ = UNUSDC;
						break;
					}
				case 41:
					{
						letter	modnum;

						modnum = LWRMSK(*l1p++);
						if(cur_rk->rk_modesw.point[modnum] ==
						   *l1p++)
							*l2p++ = UNUSDC;
						break;
					}
				case 42:
					{
						letter	modnum;

						modnum = LWRMSK(*l1p++);
						if(cur_rk->rk_modesw.point[modnum] >=
						   *l1p++)
							*l2p++ = UNUSDC;
						break;
					}
				case 43:
					{
						letter	modnum;

						modnum = LWRMSK(*l1p++);
						if(cur_rk->rk_modesw.point[modnum] <=
						   *l1p++)
							*l2p++ = UNUSDC;
						break;
					}
				case 44:
					mchevl(&l1p, tmpevl);
					*l2p++ = SENDCH;
					*l2p++ = *tmpevl;
					break;
				default:/* case 0及び上記以外 */;
					BUGreport(7);
			}
	}

	*l2p = EOLTTR;
	*l1pp = l1p;
}
#else	/*!MULTI*/
static /* V3.1 */
mchevl(l1pp, l2p)
letter	**l1pp, *l2p;
{
	letter	*l1p, tmpevl[RSLMAX];

	l1p = *l1pp;

	switch(SHUBET(*l1p)){
		case 0: *l2p++ = *l1p++; break;
		case 1:	*l2p++ = mchedsrc((int)LWRMSK(*l1p++)); break;
		case 2:  /* toupper, tolower, error, … */
			switch(LWRMSK(*l1p++)){
				case 1:	mchevl(&l1p, tmpevl);
					*l2p++ = to_upper(*tmpevl);
					break;
				case 2:	mchevl(&l1p, tmpevl);
					*l2p++ = to_lower(*tmpevl);
					break;
				case 3: *l2p++ = ERRCOD;
					break;
				case 4:
					*l2p++ = CHMSIG;
					*l2p++ = *l1p++;
					*l2p++ = 0; break; /* EOLではない */
				case 5:
					*l2p++ = CHMSIG;
					*l2p++ = *l1p++;
					*l2p++ = 1; break;
				case 6:
					*l2p++ = CHMSIG;
					*l2p++ = *l1p++;
					*l2p++ = 2; break;
				case 7:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_updown(*tmpevl);
					break;
				case 8:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_zenalpha(*tmpevl);
					break;
				case 9:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_hira(*tmpevl);
					break;
				case 10:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_kata(*tmpevl);
					break;
				case 11:
					mchevl(&l1p, tmpevl);
					to_hankata(*tmpevl, &l2p);
					break; /* 特殊 */
				case 12:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_zenhira(*tmpevl);
					break;
				case 13:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_zenkata(*tmpevl);
					break;
				case 14:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p += *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 15:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p -= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 16:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p *= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 17:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					if(! *tmpevl) *l2p = LTRHUG;
					 else *l2p /= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 18:
					mchevl(&l1p, tmpevl);
					*l2p++ = ltov(*tmpevl);
					break;
				 /* 19〜21・30は、条件を満たすと空文字列、
				    そうでないとUNUSDCを文字列として返す。*/
				case 19:
					mchevl(&l1p, tmpevl);
					if(lastmch() != *tmpevl)
						*l2p++ = UNUSDC;
					break;
				case 20:
					if(!(cur_rk->rk_modesw.point)[LWRMSK(*l1p++)])
						*l2p++ = UNUSDC;
					break;
				case 21:
					if(cur_rk->rk_modesw.point[LWRMSK(*l1p++)])
						*l2p++ = UNUSDC;
					break;
				case 22:
					*l2p++ = REASIG;
					break;
				case 23:
					*l2p++ = cur_rk->delchr;
					break;
				case 24:
					*l2p++ = CHMSIG;
					*l2p++ = 0; /* これで「all」を表す */
					*l2p++ = 0; break;
				case 25:
					*l2p++ = CHMSIG;
					*l2p++ = 0;
					*l2p++ = 1; break;
				case 26:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p &= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 27:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p |= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 28:
					mchevl(&l1p, tmpevl);
					*l2p = ~(*tmpevl);
					LWRCUT(*l2p++);
					break;
				case 29:
					*l2p++ = URBFCL;
					break;
				case 30:
					if(cur_rk->eofflg != 2 || *(cur_rk->keybuf) != EOLTTR)
						*l2p++ = UNUSDC;
					break;
				case 31:
					{
						letter	code, basenum;

						mchevl(&l1p, tmpevl);
						code = *tmpevl;
						mchevl(&l1p, tmpevl);
						if((basenum = *tmpevl) <= 1 ||
						   BASEMX < basenum)
							basenum = 10;
						to_digit(code, basenum, &l2p);
					}
					break;
				case 32:
					mchevl(&l1p, tmpevl);
					dakuadd(*tmpevl, &l2p);
					break; /* 特殊 */
				case 33:
					mchevl(&l1p, tmpevl);
					handakuadd(*tmpevl, &l2p);
					break; /* 特殊 */
				default:/* case 0及び上記以降 */;
					BUGreport(7);
			}
	}

	*l2p = EOLTTR;
	*l1pp = l1p;
}
#endif	/*!MULTI*/

 /** num番目の変数の変域が文字 l を含むかどうかを返す */
static /* V3.1 */
int
hen_ikisrc(num, l)
int	num;
letter	l;
{
	fast	letter	*defptr;	/* V3.1 */

	defptr = cur_rk_table->rk_hyo.point[cur_rk->hyonum].hensudef[num];	/* V3.1 */
	if(*defptr == VARRNG){
		for(defptr++; *defptr != EOLTTR; )
			if(*defptr++ <= l && l <= *defptr++) return(1);
		return(0);
	}

	for(; *defptr != EOLTTR; defptr++){
		if(l == *defptr) return(1);
	}
	return(0);
}

 /**	変換のメインルーチン。本処理を行うが、ついでに後処理もやっている。
	ちなみに前処理は、romkan_getcの下位関数romkan_henkanの中で、
	この関数を呼ぶ前にやっている。
	この関数は、romkan_nextから一文字来る度に呼ばれる。呼び出された直後は
	outputは空文字列、disoutには入力コード一文字が入っている。
	この関数で得られる文字の列が、romkan_henkanに渡り、
	romkan_getcは、それを文字ごとに分解して返す。
	(error)でエラーが引き起こされた場合は0を返し、正常終了時は1を返す */
static /* V3.1 */
int
match()
{
	int	henkanflg = 0, okcode = 0, chm_exist;
	letter	*q;	/* V3.1 */
	letter	urabufcreate[KBFSIZ], orgkeybuf[KBFSIZ];
	letter	*urabufjunbi, *outcutptr, *dis_end;
	/* 88/03/01/ V3.1 */
	letter	nokoribuf[KBFSIZ];

	if(*(cur_rk->keybuf) == EOLTTR){
		*(cur_rk->urabuf) = EOLTTR;
		return(1);
	}

	ltrcpy(urabufjunbi = orgkeybuf, cur_rk->keybuf);
	outcutptr = cur_rk->rk_output;

	while((okcode = henkan_ok()) > 0){
		henkanflg = 1;

		codeout_chg();
		ltrcat(cur_rk->rk_output, cur_rk->codeout);
		/* 88/03/01/ V3.1 */
		/*
		p = keybuf + codein_len;

		ltrcat(ltrcpy(keybuf, remainkbf), p);
		*/
		ltrcpy(nokoribuf, cur_rk->keybuf + cur_rk->codein_len);
		ltrcat(ltrcpy(cur_rk->keybuf, cur_rk->remainkbf), nokoribuf);

		if(okcode == 2){
			ltrcpy(urabufjunbi = urabufcreate, cur_rk->keybuf);
			totail(outcutptr);
		}
	}

	if(okcode == 0){
		ltr1cut(ltrcpy(cur_rk->keybuf, orgkeybuf));
		ltr_to_ltrseq(cur_rk->disout, NISEBP);
		*(cur_rk->rk_output) = EOLTTR;
		return(0);
	}
	if(henkanflg){
		ltrcpy(cur_rk->urabuf, urabufjunbi);

		set_rubout(cur_rk->disout, ltrlen(orgkeybuf) - 1, cur_rk->nisedl);

		dis_end = cur_rk->disout;
		totail(dis_end);
		ltrcpy(dis_end, cur_rk->rk_output);

		 /* モードチェンジを直ちに知らせるため CHMSIGを出力
		    （romkan_chmack(1)をやってないと、あとで一文字ずつに
		    分解する時点で、CHMSIGをカット）。
		    但し、rk_outputからは、CHMSIGを抜く。
		    また、CHMSIGは末尾に１回しか出力しない
		    （２回以上あっても、１回にまとめて、末尾に置く）。*/
		for(chm_exist = 0, q = cur_rk->rk_output; ; q++){
			while(*q == CHMSIG){
				chm_exist = 1;
				if(ltrcpy(q, q + 1) < outcutptr) outcutptr--;
			}
			if(*q == EOLTTR) break;
		}
		if(chm_exist){
		 /* CHMSIGを１つにまとめたものをdis_endにつなげ直す。
		    このif文をカットすれば、CHMSIGのとりまとめはやらない */
			ltr1cat(ltrcpy(dis_end, cur_rk->rk_output), CHMSIG);
		}

		bitup_ltrcat(cur_rk->disout, cur_rk->keybuf);
		cur_rk->lastoutlen = ltrlen(outcutptr);
		cur_rk->lastkbflen = ltrlen(cur_rk->keybuf);
	}
	return(1);
}

 /**	LTREOFが入ったときに、何か出すように指定されているか調べて、
	codeoutをその結果の文字列（指定がなかったら当然空）にポイントする。
	超急ごしらえで、特殊コード等は一切無視する。*/
static /* V3.1 */
void
add_at_eof()
{
	fast	dat	*datptr;	/* V3.1 */
	fast	int	i;			/* V3.1 */
	fast	int	hyoseq;			/* V3.1 */
	letter	*p;
	letter	evlrsl[RSLMAX];

	/* 88/06/14 V3.1 */
	for (hyoseq = 0;
	    cur_rk_table->rk_hyo.point != NULL && (cur_rk->hyonum = cur_rk->rk_usehyo.usehyo[hyoseq]) != -1;
								hyoseq++) {
		for (i = 0, datptr = cur_rk_table->rk_hyo.point[cur_rk->hyonum].data;
				    (p = datptr[i].code[0]) != NULL; i++) {
			if (cur_rk->rk_henmatch.size > 0) {
			cur_rk->rk_henmatch.point[0].ltrmch = EOLTTR;
			}
			while(*p != EOLTTR){
				switch(SHUBET(*p)){
					case 0:	/* 文字 */
					case 1:	/* 変数 */
					 /* これらがある場合は、NULLとは
					    マッチし得ない。*/
						goto Pass;
					case 2:	/* 式 */
						mchevl(&p, evlrsl);
						if(*evlrsl != EOLTTR)
							goto Pass;
					 /* 入力コード部に、評価すると
					    空文字列になるものが、他にはない
					    ことが前提。*/
				}
			}

			ltrevlcpy(cur_rk->codeout = p = cur_rk->evalbuf[0][0],
				  datptr[i] . code[1]);
			while(*p != EOLTTR){
				if(isSPCL(*p)) ltrcpy(p, p + 1);
				 else p++;
			}
			codeout_chg();
			return;

Pass:			;
		}
	}
	cur_rk->codeout = cur_rk->nil;
}

/* 88/06/02 V3.1 */
 /**	num番目のモードをチェンジし、変換表を選択し直す。引数 mod の値が0なら
	モードをoff、1ならon、2なら切り替える。旧modの値（0か1）を返す。*/
static
int
chgmod(num, mod)
int	num, mod;
{
	fast	int	oldmod;	/* V3.1 */

	oldmod = cur_rk->rk_modesw.point[num];
	cur_rk->rk_modesw.point[num] = (mod != 2 ? mod : !oldmod);
	choosehyo();
	return(oldmod);
}

 /** 全モードを、modが1ならon、0ならoffに切り換える。2ならswitchする。*/
static
void
allchgmod(mod)
int	mod;
{
	fast	int	i;	/* V3.1 */

	/* 88/06/07 V3.1 */
	for(i = 0; i < cur_rk->rk_modesw.count; i++)
		cur_rk->rk_modesw.point[i] = (mod != 2 ? mod : !(cur_rk->rk_modesw.point[i]));
	choosehyo();
}

 /**	一回マッチを試みる。返値は、マッチして確定した場合1（モードチェンジが
	混じっている場合は2）、マッチしたが未確定の時-1、マッチしなかったら0。
	実行中は、変数 l に、それまでに一致した長さの最高記録を入れており、
	より長く一致するものが見つかるごとに、これを更新する。lの値は、マッチ
	していても0になることもある。p_eq() 及び prefixp() の注釈文を参照。*/
static /* V3.1 */
int
henkan_ok()
{
	fast	dat	*datptr;	/* V3.1 */
	fast	int	i, k;			/* V3.1 */
	fast	int	l, j, hyoseq;		/* V3.1 */
	char	urabuf_clrf; /* モードチェンジなどで、urabufをクリアする必要が
				生じた場合はこれが立ち、その結果、henkan_ok()
				が1を返すべきところで2を返す。それを見て、
				match()がurabufなどの調整をする。*/
	fast	letter	*p;	/* V3.1 */
#ifndef	MULTI
	static	char	ebf_sw = 0;	/* V3.1 */
	static	letter	oneletter[2] = {EOLTTR, EOLTTR};	/* V3.1 */
#endif	/*!MULTI*/

	if(*(cur_rk->keybuf) == EOLTTR) return(-1);

	/* 88/06/14 V3.1 */
	for (l = -1, hyoseq = 0;
			cur_rk_table->rk_hyo.point != NULL && cur_rk->rk_usehyo.usehyo != NULL &&
			(cur_rk->hyonum = cur_rk->rk_usehyo.usehyo[hyoseq]) != -1; hyoseq++) {
		for (i = 0, datptr = cur_rk_table->rk_hyo.point[cur_rk->hyonum].data;
					(p = datptr[i].code[0]) != NULL; i++) {
			if (cur_rk->rk_henmatch.size > 0) {
			cur_rk->rk_henmatch.point[0].ltrmch = EOLTTR;
			}
			switch(k = prefixp(cur_rk->keybuf, p)){
				case -2: break;
				case -1:
					if(cur_rk->eofflg != 0) break;
				 /* eofflgが立っていたら、未確定の可能性は
				    捨てる。*/

					return(-1);
				default:
					if(k > l){
						cur_rk->ebf_sw = !cur_rk->ebf_sw;
						for(j = 1; j <= 2; j++){
							ltrevlcpy(
							 cur_rk->evalbuf[cur_rk->ebf_sw][j-1],
							 datptr[i] . code[j]);
						}

						l = k;
					}
			}
		}
	}

	if(l >= 0){
		cur_rk->codein_len = l;
		cur_rk->codeout = cur_rk->evalbuf[cur_rk->ebf_sw][0];
		cur_rk->remainkbf = cur_rk->evalbuf[cur_rk->ebf_sw][1];

		for(urabuf_clrf = 0, p = cur_rk->codeout; *p != EOLTTR; p++){
			switch(*p){
				case CHMSIG:
					 /* codeoutの1,2バイト目に
					    モード番号とスイッチが入ってる */
					if(*(p + 1) == 0)
						allchgmod((int)*(p + 2));
					 else chgmod((int)LWRMSK(*(p + 1)),
						     (int)*(p + 2));
					ltrcpy(p + 1, p + 3);
					 /* CHMSIGだけ残して1,2バイト目cut */

					urabuf_clrf = 1;
					break;
				case URBFCL:
				 /* urabufのクリアを明示的に指定する */
					urabuf_clrf = 1;
					ltrcpy(p, p + 1);
					p--; /* 一文字前にずれるため、
						for文の p++ を相殺して、
						同じ地点をもう一度見る。*/
					break;
			}
		}

		if(*(cur_rk->codeout) == ERRCOD){
			if(cur_rk->eofflg == 0){
				cur_rk->rk_errstat = 1;
				return(0);
			}

			 /* (error)であって、しかもeofflgが立ってたら、keybuf
			    の末尾まで、そのまま出す。*/
			cur_rk->codein_len = ltrlen(cur_rk->keybuf);
			cur_rk->codeout = ltrcpy(cur_rk->evalbuf[cur_rk->ebf_sw][0], cur_rk->keybuf);
			cur_rk->remainkbf = cur_rk->nil;
			cur_rk->rk_errstat = 2;

			return(1);
		}
	   /* (error)は単独でしか書けないので、エラー検出はこれで十分。*/

		if(*(cur_rk->codeout) == REASIG){
			*(cur_rk->codeout) = (romkan_restart() != 0 ? EOLTTR : CHMSIG);
			 /* 再readでエラったらモードチェンジの通知はしない */

			urabuf_clrf = 1;
		}
	   /* 表の再read。但し、これが起こったことを外に知らせるのはCHMSIGで
	      このコードそのものは外へ出ない。(restart)は、(error)同様、
	      単独でしか書けないので、検出はこれで十分。*/

		return(urabuf_clrf ? 2 : 1);
	}

	 /* 表に現れていないコードはそのまま返す */

	cur_rk->codein_len = 1;
	*(cur_rk->codeout = cur_rk->oneletter) = *(cur_rk->keybuf);
	cur_rk->remainkbf = cur_rk->nil;
	return(1);
}

static
void
free_for_all_area()
{
	fast	int	i;

	free_for_modetable_struct(&(cur_rk_table->rk_defmode));
	free_for_modetable_struct(&(cur_rk_table->rk_taiouhyo));
	free_for_modetable_struct(&(cur_rk_table->rk_path));
	free_for_modebuf(&(cur_rk_table->rk_modebuf));
	free_for_hyobuf(&(cur_rk_table->rk_hyobuf));
	free_for_heniki(&(cur_rk_table->rk_heniki));
	free_for_hensuu(&(cur_rk_table->rk_hensuu));

	free_for_modetable_struct(&(cur_rk_table->rk_dspmode));
	/* free_for_modesw(&(cur_rk_table->rk_modesw)); */
	free_for_modenaibu(&(cur_rk_table->rk_modenaibu));
	for (i = 0; i < cur_rk_table->rk_hyo.size; i++) {
		free_for_hyo_area(&(cur_rk_table->rk_hyo), i);
	}
	free_for_hyo(&(cur_rk_table->rk_hyo));
	/* free_for_usehyo(&(cur_rk_table->rk_usehyo)); */
	free_for_hensuudef(&(cur_rk_table->rk_hensuudef));
	/* free_for_henmatch(&(cur_rk_table->rk_henmatch)); */
}

/* 88/06/10 V3.1 */
 /* romkan_restart内で使うマクロ */
#define taihi(X, Y) {(X) = (Y);}
#define clear(X, N) {memset((char *)(&(X)), 0, (N));}
#define recov(X, Y) taihi((Y), (X))

 /**	表の動的再読み込みをする。現在の内部表現を全て退避し、前と同じ
	ディレクトリ（に、現在のところ限定）から表を読み込む。もし、
	読み込み中にエラーを検出すれば、もとの内部データを復活し非0を返す。*/
static /* V3.1 */
int
romkan_restart()
{
	fast	int	i/*, j*/;	/* V3.1 */

	modenaibutable	rk_modenaibu_sv;
	modeswtable	rk_modesw_sv;
	modetable	rk_dspmode_sv;
	hyotable	rk_hyo_sv;
	/* usehyotable	rk_usehyo_sv; */
	hensuudeftable	rk_hensuudef_sv;
	/* matchtable	rk_henmatch_sv; */

	/* char	*dspmod_sv[2][2]; */

	/* 待避 */
	taihi(rk_modenaibu_sv,		cur_rk_table->rk_modenaibu);
	taihi(rk_modesw_sv,		cur_rk_table->rk_modesw);
	taihi(rk_dspmode_sv,		cur_rk_table->rk_dspmode);
	taihi(rk_hyo_sv,		cur_rk_table->rk_hyo);
	/* taihi(rk_usehyo_sv,		cur_rk_table->rk_usehyo); */
	taihi(rk_hensuudef_sv,		cur_rk_table->rk_hensuudef);
	/* taihi(rk_henmatch_sv,		cur_rk_table->rk_henmatch);

	for(i = 0; i < 2; i++)
		for(j = 0; j < 2; j++)  dspmod_sv[i][j] = cur_rk_table->dspmod[i][j];
	*/

	/* 消去 */
	clear(cur_rk_table->rk_modenaibu,		sizeof(modenaibutable));
	clear(cur_rk_table->rk_modesw,		sizeof(modeswtable));
	clear(cur_rk_table->rk_dspmode,		sizeof(modetable));
	clear(cur_rk_table->rk_hyo,			sizeof(hyotable));
	/* clear(cur_rk_table->rk_usehyo,		sizeof(usehyotable)); */
	clear(cur_rk_table->rk_hensuudef,		sizeof(hensuudeftable));
	/* clear(cur_rk_table->rk_henmatch,		sizeof(matchtable)); */

	if (NULL != romkan_table_init(cur_rk_table, cur_rk_table->prv_modfnm, cur_rk_table->flags)) {
		/* 正常終了 */
		/* 88/06/10 V3.1 */
		/* 不要領域の削除 */
		free_for_modenaibu(&rk_modenaibu_sv);
		free_for_modesw(&rk_modesw_sv);
		free_for_modetable_struct(&rk_dspmode_sv);
		for (i = 0; i < rk_hyo_sv.size; i++) {
			free_for_hyo_area(&rk_hyo_sv, i);
		}
		free_for_hyo(&rk_hyo_sv);
		/* free_for_usehyo(&rk_usehyo_sv); */
		free_for_hensuudef(&rk_hensuudef_sv);
		/* free_for_henmatch(&rk_henmatch_sv); */

		return(0);
	}

	/* 異常終了 */
	/* 88/06/10 V3.1 */
	/* 不要領域の削除 */
	free_for_modenaibu(&(cur_rk_table->rk_modenaibu));
	free_for_modesw(&(cur_rk_table->rk_modesw));
	free_for_modetable_struct(&(cur_rk_table->rk_dspmode));
	for (i = 0; i < cur_rk_table->rk_hyo.size; i++) {
		free_for_hyo_area(&(cur_rk_table->rk_hyo), i);
	}
	free_for_hyo(&(cur_rk_table->rk_hyo));
	/* free_for_usehyo(&(cur_rk_table->rk_usehyo)); */
	free_for_hensuudef(&(cur_rk_table->rk_hensuudef));
	/* free_for_henmatch(&(cur_rk_table->rk_henmatch)); */

	/* 復帰 */
	recov(rk_modenaibu_sv,		cur_rk_table->rk_modenaibu);
	recov(rk_modesw_sv,		cur_rk_table->rk_modesw);
	recov(rk_dspmode_sv,		cur_rk_table->rk_dspmode);
	recov(rk_hyo_sv,		cur_rk_table->rk_hyo);
	/* recov(rk_usehyo_sv,		cur_rk_table->rk_usehyo); */
	recov(rk_hensuudef_sv,		cur_rk_table->rk_hensuudef);
	/* recov(rk_henmatch_sv,		cur_rk_table->rk_henmatch);

	for(i = 0; i < 2; i++)
		for(j = 0; j < 2; j++)  cur_rk_table->dspmod[i][j] = dspmod_sv[i][j];
	*/
	return(1);
}

 /** lp2から評価して得た文字列をlp1にコピー */
static /* V3.1 */
void
ltrevlcpy(lp1, lp2)
letter	*lp1, *lp2;
{
	while(*lp2 != EOLTTR){
		mchevl(&lp2, lp1);
		totail(lp1);
	}
	*lp1 = EOLTTR;
}

static /* V3.1 */
void
set_rubout(lp, n, del)
 /** lpに 「del」n個の列をセットする。ここに del は 'delchr'か'nisedl' */
fast	letter	*lp, del;	/* V3.1 */
fast	int	n;		/* V3.1 */
{
	for(; n; n--) *lp++ = del;
	*lp = EOLTTR;
}

 /** これが実行されたらバグ。但し実行はそのまま続く */
#ifdef OMRON_LIB
static
#endif
void
BUGreport(n)
int	n;
{
	fprintf(stderr, "\r\nromkan-Bug%d!!\r\n", n);
}

 /** 前処理(mae_in→mae_out)又は後処理(ato_in→ato_out)を行う。*/
static /* V3.1 */
void
maeato_henkan(in, outp, m_a_hyo)
letter	in;		/* 入力の一文字 */
letter	*outp;		/* 出力はここに入る */
int	*m_a_hyo;	/* どの前・後処理表が選択されているかの情報 */
{
	fast	dat	*datptr;	/* V3.1 */
	fast	int	i, hyoseq;		/* V3.1 */
	fast	letter	*curdat;		/* V3.1 */

	if(isSPCL(in)){
	 /* LTREOFやCHMSIGが来うるので、特殊コードはそのまま返すように
	    細工しておかないといけない。*/
		ltr_to_ltrseq(outp, in);
		return;
	}

	/* 88/06/14 V3.1 */
	for (hyoseq = 0;
	    m_a_hyo != NULL && (cur_rk->hyonum = m_a_hyo[hyoseq]) != -1; hyoseq++) {
		for(i = 0, datptr = cur_rk_table->rk_hyo.point[cur_rk->hyonum].data;
		    NULL != (curdat = datptr[i] . code[0]); i++){
			if (cur_rk->rk_henmatch.size > 0) {
			cur_rk->rk_henmatch.point[0].ltrmch = EOLTTR;
			}
			if(!l_eq(curdat, in)) continue;

			ltrevlcpy(outp, datptr[i] . code[1]);
			return;
		}
	}
	ltr_to_ltrseq(outp, in);
}

 /** 後処理 */
static /* V3.1 */
void
codeout_chg()
{
	fast	letter	*saishu_outp;	/* V3.1 */
	letter	saishu_out[OUTSIZ];	/* V3.1 */

	*(saishu_outp = saishu_out) = EOLTTR;

	for(; *(cur_rk->codeout) != EOLTTR; cur_rk->codeout++){
		maeato_henkan(*(cur_rk->codeout), saishu_outp, cur_rk->rk_usehyo.useatohyo);
		totail(saishu_outp);
	}

	cur_rk->codeout = saishu_out;
}

 /** 一文字プッシュ・バック */
letter	romkan_ungetc(l)
letter	l;
{
	return(cur_rk->ungetc_buf = l);
}

 /** romkan_nextに対し一文字プッシュ・バック */
letter	romkan_unnext(l)
letter	l;
{
	return(cur_rk->unnext_buf = l);
}

/* 88/06/02 V3.1 */
/******************************************************************************/
#ifndef OMRON
 /**	deleteとして使うキャラクタの設定（偽deleteも）。これを実行後は
	romkan_clearを実行しておかないと混乱のもとになります。（廃止予定）*/
void
romkan_setdel(delchr_, nisedl_)
letter	delchr_, nisedl_;
{
	cur_rk->delchr = delchr_;
	cur_rk->nisedl = nisedl_;
}
#endif
/******************************************************************************/
