/*
 * $Id: rk_modread.c,v 1.2 1991/09/16 21:34:47 ohm Exp $
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
			rk_modread.c
						87.11.17  改 正

	モード定義表の読み込みを担当するプログラム。
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
#include "rext.h"

#ifdef WNNDEFAULT
#	include "config.h"
 /* マクロLIBDIRの定義（のためだけ）。コンパイル時は、ヘッダファイルの
    サーチパスに、Wnnのインクルードファイルのありかを設定しておくこと。*/
#endif
#include <pwd.h>
#endif

#define xyint(X, Y) (((X) << 24) | (Y))
#define Terminator 0 /* intの列（naibu[]）の終止コード */

#ifdef OMRON_SPEC
#define mystrcmp(s1, s2)	strcmp(s1, s2)
#define mystrcpy(s1, s2)	strcpy(s1, s2)
#endif

static void cond_evl(), rd_bcksla(), rd_ctrl(), look_choose(), hyouse(),
  mystrcpy(), ERMOPN(), ERRMOD();
static int mystrcmp(), mod_evl(), fnmsrc_tourk(), pathsrc_tourk(),
  dspnamsrc_tourk(), modsrc_tourk(), chkchar_getc(), read1tm(),
  get_hmdir(), rd_string(), ctov(), scan1tm(), modnamchk(),
  look_cond(), evlcond();
static char codeeval(), *ename();

extern	char	*chrcat(), *strend();

#ifndef	MULTI
/* 88/05/31 V3.1 */
static	FILE	*modefile;	/* モード定義表の fp */

/* 88/06/03 V3.1 */
/* エラー処理のためのもの */
static	char	*mcurdir;	/* モード表のサーチパス */
static	char	*mcurfnm;	/* モード表のファイル名 */
static	char	*mcurread;	/* モード表の現在行bufへのポインタ */
#endif	/*!MULTI*/

/* キーワードとその内部表現の対応を与える構造体。内部表現を
	持たないものに対しては0が与えられている。*/
struct	kwdpair {
	char	*name;
	int	code;
};
static /* V3.1 */
struct	kwdpair modfn[] = {
	"defmode",	0,
	"if",		xyint(2, 0),
	"when",		xyint(2, 1),
	"path",		0,
	"search",	0,
	"on_dispmode",	xyint(5, 0),
	"off_dispmode",	xyint(5, 1),
	"on_unchg",	xyint(6, 0),
	"off_unchg",	xyint(6, 1),
	NULL
}; /* 下を見よ キーワード定義表はまだあと二つあるのだ */

static /* V3.1 */
struct	kwdpair	modcond[] = {
	"not",	xyint(3, 0),
	"and",	xyint(3, 1),
	"or",	xyint(3, 2),
	"true",	xyint(3, 3),
	"false",xyint(3, 4),
	"=",	xyint(3, 5),
	"!=",	xyint(3, 6),
	"<",	xyint(3, 7),
	">",	xyint(3, 8),
	NULL
};
static /* V3.1 */
int	condarg[] = {1, 2, 2, 0, 0, 2, 2, 2, 2}; /* 条件判断関数の引数の個数 */

static /* V3.1 */
struct	kwdpair	swstat[] = {
	"on",	0,
	"off",	0,
	NULL
};

 /** キーワード（if, andなど）が正当なものかチェックし、その番号を返す */
static /* V3.1 */
int
kwdsrc(hyou, wd)
struct	kwdpair	*hyou;	/* どのキーワード表を使うか */
char	*wd;		/* チェックされるキーワード */
{
	fast	int	i;	/* V3.1 */

	for(i = 0; hyou[i] . name != NULL; i++)
		if(!mystrcmp(hyou[i] . name, wd)) return(i);
	ERRMOD(9);
	 /*NOTREACHED*/
	return(-1);
}

 /** 与えられたファイルがディレクトリなら非0を返す */
static /* V3.1 */
int
isdir(fnm)
char	*fnm;
{
	struct	stat	statbuf;

	/* Bug!!! 88/07/20 H.HASHIMOTO */
	/* return(stat(fnm, &statbuf) == 0 && (statbuf.st_mode & S_IFDIR)); */
	return(stat(fnm, &statbuf) == 0 &&
			((statbuf.st_mode & S_IFMT) == S_IFDIR));
}

/* 88/06/10 V3.1 */
 /** モード表の読み込み */
#ifdef OMRON_LIB
static
#endif
int
readmode(filename)
char	*filename;	/* ユーザが指定したモード表の名 */
{
#ifndef	MULTI
	char	modefilename[REALFN + 5];	/* V3.1 */
	char	pathname[REALFN];		/* V3.1 */
#endif	/*!MULTI*/

	struct	stat	statbuf;		/* V3.1 */
	char	*buf, *bufp;			/* V3.1 */

#ifdef RKMODPATH
	char	*genv, *pathenv, *pathp;
	extern	char	*getenv();	/* V3.1 */
#endif

	/* 88/06/14 V3.1 */
	/* モード定義表のパス名の長さチェックを安全性のために、
							付け加えておきます。 */
	cur_rk_table->mcurdir = "";		/* エラー処理用 */	/* V3.1 */
	if (strlen(cur_rk_table->mcurfnm = filename) > (REALFN - 1))	/* V3.1 */
		ERMOPN(3); 				/* V3.1 */
	strcpy(cur_rk_table->modefilename, filename);
	if (*(cur_rk_table->modefilename) == '\0' || *(strend(cur_rk_table->modefilename)) == KUGIRI) {
		strcat(cur_rk_table->modefilename, "mode");
	} else if(isdir(cur_rk_table->modefilename)) {
		chrcat(cur_rk_table->modefilename, KUGIRI);
		strcat(cur_rk_table->modefilename, "mode");
	}
	/* 88/06/14 V3.1 */
	if (strlen(cur_rk_table->mcurfnm = cur_rk_table->modefilename) > (REALFN - 1))	/* V3.1 */
		ERMOPN(3); 					/* V3.1 */

#ifdef RKMODPATH
	/* 88/06/10 V3.1 */
	if (!fixednamep(cur_rk_table->modefilename) &&
	   (pathenv = genv = getenv(RKMODPATH)) != NULL && *genv != '\0') {
		 /* PATHに少なくとも一つのサーチパスがある場合 */
		for ( ; ;) {
			for(pathp = cur_rk_table->pathname; *genv != ':' && *genv; genv++)
				*pathp++ = *genv;
			*pathp = '\0';

			if(*(strend(cur_rk_table->pathname)) != KUGIRI) *pathp++ = KUGIRI;
			 /* pathの区切りはDG（MV）であっても'/' */

			strcpy(pathp, cur_rk_table->modefilename);
			if ((stat(cur_rk_table->pathname, &statbuf) == 0) &&
			    ((cur_rk_table->modefile = fopen(cur_rk_table->pathname, "r")) != NULL)) {
				 /* Now Mode-hyo found */
				 /*
				if (cur_rk->flags & RK_VERBOS) {
					fprintf(stderr,
					"romkan: using Mode-hyo %s ...\r\n",
								pathname);
				}
				*/
				cur_rk_table->mcurdir = cur_rk_table->pathname;
				 /* この時点ではファイル名込みだが、
				    あとでパス名だけになる */
				cur_rk_table->mcurfnm = ename(cur_rk_table->modefilename);	/* V3.1 */
				break;
			}

			if (*genv != ':') {
				 /* Mode-hyo not found */
				 /*
			 	if (flags & RK_VERBOS){
				 	fprintf(stderr,
						"no %s in ",cur_rk->modefilename);
					for(genv = pathenv; *genv; genv++){
						fputc((*genv == ':' ?
						      ' ' : *genv), stderr);
					}
					fprintf(stderr, ".\n");
				}
				*/
			 	ERMOPN(1);
			}
			 /* coutinues searching Mode-hyo */
			genv++; 
		}
	} else
#endif
	{
		/* 88/06/10 V3.1 */
		if (stat(cur_rk_table->modefilename, &statbuf) != 0) ERMOPN(1);
		if ((cur_rk_table->modefile = fopen(cur_rk_table->modefilename, "r")) == NULL) ERMOPN(2);
		/*
		if (cur_rk->flags & RK_VERBOS) {
			fprintf(stderr, "romkan: using Mode-hyo %s ...\r\n",
								cur_rk->modefilename);
		}
		*/
		strcpy(cur_rk_table->pathname, cur_rk_table->modefilename);
	}

	 /* サーチパスの先頭に、モード表のあるディレクトリを設定している。*/
	*(ename(cur_rk_table->pathname)) = '\0';			/* V3.1 */
	pathsrc_tourk(cur_rk_table->pathname);			/* V3.1 */

	/* 88/06/14 V3.1 */
	malloc_for_modenaibu(&(cur_rk_table->rk_modenaibu), statbuf.st_size + 1);
	*(cur_rk_table->rk_modenaibu.next) = Terminator;

	malloc_for_modebuf(&(cur_rk_table->rk_modebuf), statbuf.st_size + 1);
	cur_rk_table->mcurread = buf = cur_rk_table->rk_modebuf.org;	/* エラー処理用 */ /* V3.1 */

	 /* モード定義表の読み込み */
	while (bufp = buf, read1tm(&bufp, 0))	/* V3.1 */
		mod_evl(buf);			/* V3.1 */
	fclose(cur_rk_table->modefile);

	/* 88/06/14 V3.1 */
	free_for_modebuf(&(cur_rk_table->rk_modebuf));
	realloc_for_modenaibu(&(cur_rk_table->rk_modenaibu));

	return(cur_rk_table->rk_taiouhyo.count);	/* V3.1 */
}

 /**	モード表の一かたまり（リスト、ファイル名、モード表示文字列）を
	解釈する。返す値は、defmode,search及びpathの時0、それ以外なら1。*/
static /* V3.1 */
int
mod_evl(s)
char	*s; /* モード表の内部表現の列へのポインタ */
{
	char	md1[MDT1LN];			/* V3.1 */
	fast	char	*bgn, *end;		/* V3.1 */
	fast	int	num, retval = 1;	/* V3.1 */

	if(*s != '('){
		if(*s != '"'){
			num = fnmsrc_tourk(s);
			*(cur_rk_table->rk_modenaibu.next++) = xyint(4, num);
		} else {
			s++;
			if(*(end = strend(s)) != '"') ERRMOD(10);
			*end = '\0';
			num = dspnamsrc_tourk(s);
			*(cur_rk_table->rk_modenaibu.next++) = xyint(5, 0);
			*(cur_rk_table->rk_modenaibu.next++) = num;
		}
	} else {
		s++;
		scan1tm(&s, md1, 1);
		switch(num = kwdsrc(modfn, md1)){
			case 0: /* defmode */
				retval = 0;
				scan1tm(&s, md1, 1); /* modename */
				num = modsrc_tourk(md1, 0);
				if(scan1tm(&s, md1, 0)){ /* 初期on-off */
					switch(kwdsrc(swstat, md1)){
						case 0:
							cur_rk_table->rk_modesw.point[num]= 1;
							break;
						case 1:
							cur_rk_table->rk_modesw.point[num]= 0;
							break;
					}
					scan1tm(&s, md1, 2); /* あればerr */
				} else cur_rk_table->rk_modesw.point[num]= 0; /* defaultはoff */
				break;
			case 1: /* if */
			case 2: /* when */
				*(cur_rk_table->rk_modenaibu.next++) = modfn[num] . code;
				scan1tm(&s, md1, 1); /* condition */
				cond_evl(md1);
				while(scan1tm(&s, md1, 0)){
					if(mod_evl(md1) == 0) ERRMOD(8);
				}
				*(cur_rk_table->rk_modenaibu.next++) = Terminator;
				break;
			case 3: /* path */
				/* 88/06/09 V3.1 */
				cur_rk_table->rk_path.count = 1;		
				cur_rk_table->rk_path.next = cur_rk_table->rk_path.point[1];
			case 4: /* search */
				retval = 0;
				/* 88/06/29 V3.1 */
				if(cur_rk_table->rk_taiouhyo.point[0] != NULL) ERRMOD(11);
				 /* サーチパスの指定はファイル名の出現より
				    先行しなければならないとしておく。*/

				while(scan1tm(&s, md1, 0)){/* find pathname */
					pathsrc_tourk(md1);
				}
				break;
			case 5: /* on_dispmode */
			case 6:	/* off_dispmode */
				*(cur_rk_table->rk_modenaibu.next++) = modfn[num] . code;
				scan1tm(&s, md1, 1); /* dispmode string */

				if(*(bgn = md1) != '"') ERRMOD(12);
				bgn++;
				if(*(end = strend(bgn)) != '"') ERRMOD(10);
				*end = '\0';
				*(cur_rk_table->rk_modenaibu.next++) = dspnamsrc_tourk(bgn);
				scan1tm(&s, md1, 2); /* あればerr */
				break;
			case 7: /* on-unchg */
			case 8: /* off-unchg */
				*(cur_rk_table->rk_modenaibu.next++) = modfn[num] . code;
				scan1tm(&s, md1, 2); /* あればerr */
				break;
		}

	}
	*(cur_rk_table->rk_modenaibu.next) = Terminator;
	return(retval);
}

 /** 条件式（モード名 又はnot,andなどの式）一つを解釈 */
static /* V3.1 */
void
cond_evl(cod)
char	*cod; /* 条件式の内部表現の列へのポインタ */
{
	char	md1[MDT1LN];
	fast	int	num, i;		/* V3.1 */

	if(*cod != '('){
		num = modsrc_tourk(cod, 1);
		*(cur_rk_table->rk_modenaibu.next++) = xyint(1, num);
	} else {
		cod++;
		scan1tm(&cod, md1, 1); /* not;and;or */
		num = kwdsrc(modcond, md1);
		*(cur_rk_table->rk_modenaibu.next++) = xyint(3, num);
		for(i = condarg[num]; i; i--){
			scan1tm(&cod, md1, 0);
			cond_evl(md1);
		}
		scan1tm(&cod, md1, 2);
	}
	*(cur_rk_table->rk_modenaibu.next) = Terminator;
}

/* 88/06/09 V3.1 */
static int
serach_in_modetable_struct(ptr, s, np)
fast	modetable *ptr;
fast	char	*s;
fast	int	*np;
{
	fast	int	n;

	for (n = 0; n < ptr->count; n++) {
		if (!mystrcmp(ptr->point[n], s)) {
			*np = n;
			return(1);	/* found */
		}
	}
	*np = n;
	return(0);	/* not found */
}

/* 88/06/13 V3.1 */
static
void
entry_to_modetable_struct(ptr, n, s)
fast	modetable *ptr;
int	n;
char	*s;
{
	ptr->point[n] = ptr->next;
	mystrcpy(ptr->next, s);
	strtail(ptr->next);
	++(ptr->next);
}


/* 88/06/07 V3.1 */
 /**	sで指定されたファイル名が既登録か探し、なければ登録。但し、既登録か
	どうかのチェックは厳密ではないが（例えば、同じファイルでも、
	パス名付きと無しとでは、同じと見ない）、ファイル名が既登録かどうか
	チェックするのは、メモリ節約のために同じ表を読み込むのを防ぐため
	だけなので、それ以外には別に困る点はない。*/
static /* V3.1 */
int
fnmsrc_tourk(s)
char	*s;
{
	int	n;

	if (serach_in_modetable_struct(&(cur_rk_table->rk_taiouhyo), s, &n))
		return(n);

	check_and_realloc_for_modetable_struct(&(cur_rk_table->rk_taiouhyo), n, s,
				RK_TAIOUHYO_MAX_LOT, RK_TAIOUHYO_MEM_LOT);

	/* チェックのみ */
	if(!(filnamchk(s))) ERRMOD(3);

	entry_to_modetable_struct(&(cur_rk_table->rk_taiouhyo), n, s);
	return(n);
}

/* 88/06/07 V3.1 */
 /**	sで指定されたサーチパス名が既登録か探し、なければ登録。但し、fnmsrc_
	tourk()同様、既登録かどうかのチェックは厳密ではないが問題ない。*/
/* 88/06/07 V3.1 */
static /* V3.1 */
int
pathsrc_tourk(s)
char	*s;
{
	int	n;
	char	fnm_addsla[MDT1LN];

	mystrcpy(fnm_addsla, s);
	if( !(*fnm_addsla == '\0' || *(strend(fnm_addsla)) == KUGIRI))
		chrcat(fnm_addsla, KUGIRI);
	s = fnm_addsla;
	 /* パス名が'/'で終わってなければ、それを付加する。*/

	if (serach_in_modetable_struct(&(cur_rk_table->rk_path), s, &n))
		return(n);

	check_and_realloc_for_modetable_struct(&(cur_rk_table->rk_path), n, s,
				RK_PATH_MAX_LOT, RK_PATH_MEM_LOT);

	entry_to_modetable_struct(&(cur_rk_table->rk_path), n, s);
	return(n);
}

/* 88/06/07 V3.1 */
 /** sで指定されたモード表示文字列が既登録か探し、なければ登録 */
static /* V3.1 */
int
dspnamsrc_tourk(s)
char	*s;
{
	int	n;

	if (serach_in_modetable_struct(&(cur_rk_table->rk_dspmode), s, &n))
		return(n);

	check_and_realloc_for_modetable_struct(&(cur_rk_table->rk_dspmode), n, s,
				RK_DSPMODE_MAX_LOT, RK_DSPMODE_MEM_LOT);

	entry_to_modetable_struct(&(cur_rk_table->rk_dspmode), n, s);
	return(n);
}

/* 88/06/09 V3.1 */
 /**	sで指定されたモード名を探し、なければ登録。但し、flgが非0なら、
	見つからなければエラー */
static /* V3.1 */
int
modsrc_tourk(s, flg)
char	*s;
int	flg;
{
	int	n;

	if(!modnamchk(s)) ERRMOD(4);	/* V3.1 */

	if (serach_in_modetable_struct(&(cur_rk_table->rk_defmode), s, &n))
		return(n);

	if(flg) ERRMOD(5);

	check_and_realloc_for_modetable_struct(&(cur_rk_table->rk_defmode), n, s,
				RK_DEFMODE_MAX_LOT, RK_DEFMODE_MEM_LOT);

	check_and_realloc_for_modesw(&(cur_rk_table->rk_modesw), n,
				RK_DEFMODE_MAX_LOT, RK_DEFMODE_MEM_LOT);

	entry_to_modetable_struct(&(cur_rk_table->rk_defmode), n, s);
	return(n);
}

 /** ファイルから一文字読む（空白文字は飛ばす）。読んだ文字がEOFなら0を返す */
static /* V3.1 */
char	fspcpass()
{	
	fast	 int	c;	/* V3.1 */

	while(EOF != (c = chkchar_getc(cur_rk_table->modefile)) && is_nulsp(c));
	return(c == EOF ? '\0' : c);
}

 /**	モード表には空白文字以外のコントロール文字は生では混じらないものと
	する。混じっていた場合はチェックしつつ、getcを行う。*/
static /* V3.1 */
int
chkchar_getc(f)
FILE	*f;
{
	fast	 int	c;	/* V3.1 */

	c = getc(f);
	if(is_cntrl(c) && !isspace(c)){
		sprintf(cur_rk_table->mcurread, "\\%03o", c);
		ERRMOD(16);
	}
	return(c);
}	

 /**	モード表の一かたまり（リスト、ファイル名、モード表示文字列）を
	切り出す。その際、特殊な表記（'^','\'による）は、'\（8進）;' の
	形に直す。flgが非0なら、EOFでエラーを起こし、')'で0を返す。*/
static /* V3.1 */
int
read1tm(sptr, flg)
char	**sptr;	/* モード表の内部表現の列へのポインタへのポインタ。
		   rd_bcksla()、rd_ctrl()、codeeval()でも同様 */
int	flg;
{
	fast	int	c;	/* V3.1 */
	fast	char	*head;	/* V3.1 */
	int	retval = 1;	/* V3.1 */
	char	*s;		/* V3.1 */

	s = *sptr;

	while((c = fspcpass()) == ';'){
	  /* 注釈文を検出したら、行末までとばして再試行。*/
		while((c= chkchar_getc(cur_rk_table->modefile))!= '\n' && c != EOF);
	}

	switch(c){
		case '\0': /* EOFを表す */
			if(flg) ERRMOD(0);
			 else retval = 0;
			break;
		case ')':
			if(flg) retval = 0;
			 else ERRMOD(1);
			break;
		case '(':
			*s++ = c;
			*s++ = ' ';
			while(read1tm(&s, 1)) *s++ = ' ';
			*s++ = ')';
			break;
		case '"':
			*s++ = c;
			while((c = chkchar_getc(cur_rk_table->modefile)) != '"'){
				switch(c){
					case EOF: ERRMOD(0);
#ifndef OMRON_SPEC
					case '\\': rd_bcksla(&s); break;
					case '^': rd_ctrl(&s); break;
#endif
					default: *s++ = c;
				}
			}
			*s++ = '"';
			break;
		default:
		 /* 先頭が @ 又は ~ の時は、特殊処理。*/
			if(c == '@'){ /* @HOME, @MODEDIR, @LIBDIR */
				*s++ = c;
				head = s;
				rd_string(&s, 1);

				if(mystrcmp("HOME", head) == 0){
					s = --head;
					if(get_hmdir(&s, (char *)NULL) != 0){
						cur_rk_table->mcurread = head;
			  /* エラー表示の調整。抜けずにすぐにエラー表示
			     ルーチンに行くのでheadの中身は失われない。*/
						ERRMOD(13);
					}
				} else
				if(mystrcmp("MODEDIR", head) == 0){
					/* 88/06/09 V3.1 */
					strcpy(s = --head, cur_rk_table->rk_path.point[0]);
					if(KUGIRI== *(s= strend(s))) *s= '\0';
				} else
#ifdef WNNDEFAULT
				if(mystrcmp("LIBDIR", head) == 0){
					strcpy(s = --head, LIBDIR);
					strtail(s);
				} else
#endif
				{
					cur_rk_table->mcurread = --head;
					ERRMOD(14);
				}
			} else
			if(c == '~'){ /* ~user */
				*s++ = c;
				head = s;
				rd_string(&s, 1);

				mystrcpy(head, head);
				s = head - 1;
				if((get_hmdir(&s, (*head ? head : NULL)))!= 0){
					cur_rk_table->mcurread = --head;
					 /* エラー表示の調整 */
					ERRMOD(15);
				}
			} else {
				ungetc(c, cur_rk_table->modefile);
			}
			 /* 先頭の特殊処理 終わり */

			if((c = rd_string(&s, 0)) == EOF && flg) ERRMOD(0);
			if(c == ')' && !flg) ERRMOD(1);
	}

	*s = '\0';
	*sptr = s;
	return(retval);
}

/* 88/06/03 V3.1 */
 /**	socの名のユーザのログイン・ディレクトリ名をdestに入れ、*destにその末尾を
	指させる。但しsocが空列または、NULLなら自分のログイン・ディレクトリ名、
	いずれの場合も、不成功時は、何もしない。返値は、不成功時-1。*/
static /* V3.1 */
int
get_hmdir(dest, soc)
char	**dest, *soc;
{
	fast	struct	passwd	*usr;	/* V3.1 */
	extern	struct	passwd	*getpwnam(), *getpwuid();	/* V3.1 */

	/* 88/05/26 V3.1 */
	if(soc == NULL || *soc == '\0'){
		if(NULL == (usr = getpwuid(getuid()))) return(-1);
	} else {
		if(NULL == (usr = getpwnam(soc))) return(-1);
	}
	strcpy(*dest, usr -> pw_dir);
	strtail(*dest);
	return(0);
}



 /**	モード表・対応表中の、ファイル名の部分の読み込み。先頭が @ 又は ~ の
	時は、特殊処理を行う。引数は、一字読み込み・一字戻し・文字列取り出しの
	関数と、結果を入れるエリアの番地へのポインタ、次に読まれる文字を入れる
	ポインタ。返値は、正常終了時0、@HOMEでホーム・ディレクトリが取れない時
	1、@のあとに変なものが来たら2、~で自分のホーム・ディレクトリが取れない
	時3、~のあとに存在しないユーザ名が来たら4。*/
int
readfnm(readchar_func, unreadc_func, readstr_func, areap, lastcptr)
register int	(*readchar_func)(), (*unreadc_func)(), (*readstr_func)();
char	**areap;
int	*lastcptr;
{
	char	*head;
	register int	c;

	c = (*readchar_func)();
	if(c == '@'){ /* @HOME, @MODEDIR, @LIBDIR */
		*(*areap)++ = c;
		head = *areap;
		(*readstr_func)(areap, 1);

		if(mystrcmp("HOME", head) == 0){
			*areap = --head;
			if(get_hmdir(areap, (char *)NULL) != 0){
				*areap = head;
				return(1);
			}
		} else
		if(mystrcmp("MODEDIR", head) == 0){
			strcpy(*areap = --head, cur_rk_table->pathname);
			if(KUGIRI== *(*areap= strend(*areap))) **areap = '\0';
		} else
#ifdef WNNDEFAULT
		if(mystrcmp("LIBDIR", head) == 0){
			strcpy(*areap = --head, LIBDIR);
			strtail(*areap);
		} else
#endif
		{
			*areap = --head;
			return(2);
		}

	} else
	if(c == '~'){ /* ~user */
		int	err;
			
		*(*areap)++ = c;
		head = *areap;
		(*readstr_func)(areap, 1);

		mystrcpy(head, head);
		*areap = head - 1;
		if((err = get_hmdir(areap, (*head ? head : NULL)))!= 0){
			*areap = --head;
			return(err == -2 ? 3 : 4);
		}

	} else {
		(*unreadc_func)(c);
	}

	*lastcptr = (*readstr_func)(areap, 0);
	return(0);
}

 /**	モード表から一文字分取り出す作業を、空白・EOF・括弧のどれかが来るまで
	続ける。flgが非0なら、'/'が来ても終わる。返値は、次に読まれる文字。*/
static /* V3.1 */
int
rd_string(sptr, flg)
char	**sptr;
int	flg;
{
	fast	int	c;	/* V3.1 */

	while(c = chkchar_getc(cur_rk_table->modefile), !(is_nulsp(c) || c == EOF ||
	      c == '(' || c == ')' || (flg == 1 && c == KUGIRI))){
		switch(c){
#ifndef OMRON_SPEC
			case '\\': rd_bcksla(sptr); break;
			case '^': rd_ctrl(sptr); break;
#endif
			default: *(*sptr)++ = c;
		}
	}
	**sptr = '\0';
	return(ungetc(c, cur_rk_table->modefile));
}	

/* 88/06/02 V3.1 */
#ifndef OMRON_SPEC
 /**	モード表からバックスラッシュ形式の一文字分を取り出し、'\（8進）;' の
	形に直す。但し、先頭の'\\'は既に読まれたあと。*/
static /* V3.1 */
void
rd_bcksla(sptr)
char	**sptr;
{
	fast	int	c, code = 0, digflg = 0;	/* V3.1 */

	switch(c = chkchar_getc(cur_rk_table->modefile)){
		case 'n':
			code = '\n'; digflg = 1; break;
		case 't':
			code = '\t'; digflg = 1; break;
		case 'b':
			code = '\b'; digflg = 1; break;
		case 'r':
			code = '\r'; digflg = 1; break;
		case 'f':
			code = '\f'; digflg = 1; break;
		case 'e':
		case 'E':
			code = ESCCHR; digflg = 1; break;
		case 'o':
			while(c = chkchar_getc(cur_rk_table->modefile), is_octal(c)){
				code <<= 3;
				code += ctov(c);
				digflg = 1;
			}
			if(c != ';') ungetc(c, cur_rk_table->modefile);
			break;
		case 'd':
			while(c = chkchar_getc(cur_rk_table->modefile), is_digit(c)){
				code *= 10;
				code += ctov(c);
				digflg = 1;
			}
			if(c != ';') ungetc(c, cur_rk_table->modefile);
			break;
		case 'x':
			while(c = chkchar_getc(cur_rk_table->modefile), is_xdigit(c)){
				code <<= 4;
				code += ctov(c);
				digflg = 1;
			}
			if(c != ';') ungetc(c, cur_rk_table->modefile);
			break;
		default:
			if(is_octal(c)){
				digflg = 1;
				code = ctov(c);
				while(c= chkchar_getc(cur_rk_table->modefile), is_octal(c)){
					code <<= 3;
					code += ctov(c);
				}
				if(c != ';') ungetc(c, cur_rk_table->modefile);
			} else {
				code = c;
				digflg = 1;
			}
	}

	if(digflg == 0) ERRMOD(7);
	sprintf(*sptr, "\\%o;", code);
	strtail(*sptr);
}


 /**	モード表からコントロールコード形式の一文字分を取り出し、'\（8進）;' の
	形に直す。但し、先頭の'^'は既に読まれたあと。*/
static /* V3.1 */
void
rd_ctrl(sptr)
char	**sptr;
{
	fast	int	c;	/* V3.1 */

	if(!(' ' <= (c = chkchar_getc(cur_rk_table->modefile)) && c < '\177')) ERRMOD(7);
	if(c == '?') c = '\177'; else c &= 0x1f;

	sprintf(*sptr, "\\%o;", c);
	strtail(*sptr);
}
#endif

 /**	8・10・16進コード用のキャラクタを実際のコードに直す。入力のチェックは
	しない。*/
static
int
ctov(c)
char	c;
{
	if(is_upper(c)) return(c - 'A' + 10);
	if(is_lower(c)) return(c - 'a' + 10);
	return(c - '0');
}

 /**	リストの中身のscanに専用。')'で0を返す。EOLは来ないはず。
	flg == 1 のとき、取り出しに失敗したらエラー。
	flg == 2 のとき、取り出しに成功したらエラー。
	特殊なコード表記は既に全て '\（8進）;' の形に直っている筈。*/
static /* V3.1 */
int
scan1tm(socp, dest, flg)
char	**socp, *dest;
	 /* socpの指しているポインタが指している所から取り出してdestに入れる。
	    その後、socpが指しているポインタを進める。*/
int	flg;
{
	fast	char	c;		/* V3.1 */
	fast	int	retval = 1;	/* V3.1 */

	while(c = *(*socp)++, is_nulsp(c)) if(c == '\0') ERRMOD(6);
	switch(c){
		case ')':
			retval = 0;
			break;
		case '(':
			*dest++ = c;
			*dest++ = ' ';
			while(scan1tm(socp, dest, 0)){
				strtail(dest);
				*dest++ = ' ';
			}
			*dest++ = ')';
			break;
		case '"':
			*dest++ = c;
			while((c = *dest++ = *(*socp)++) != '"'){
				if(c == '\\'){ /* '\（8進）;'の解釈 */
					while(c = *dest++ = *(*socp)++,
					      is_octal(c));
				}
			}
			break;
		default:
			*dest++ = c;
			while(!is_nulsp(**socp)) *dest++ = *(*socp)++;
	}

	*dest = '\0';
	if(flg == 1 && retval == 0 || flg == 2 && retval == 1) ERRMOD(6);
	return(retval);
}

 /** モード名として正当かチェック。英数字からなっていればいい */
static /* V3.1 */
int
modnamchk(s)
fast	char	*s;	/* V3.1 */
{
	if(is_digit(*s)) return(0);
	for(; *s; s++) {
		if(!is_alnum(*s)) {
			if (*s != '_') {
				return(0);
			}
		}
	}
	return(1);
/*
	if(is_digit(*s)) return(0);
	for(; *s; s++) if(!is_alnum(*s) && *s != '_') return(0);
	return(1);
*/
}

 /** 変換対応表の選択を行う */
#ifdef OMRON_LIB
static
#endif
void
choosehyo()
{
	fast	int	i;	/* V3.1 */
	int	*naibup;	/* V3.1 */

	if (cur_rk->rk_usehyo.usehyo != NULL)
		cur_rk->rk_usehyo.usemaehyo[0] = cur_rk->rk_usehyo.usehyo[0] =
						cur_rk->rk_usehyo.useatohyo[0] = -1;
	for(i = 0; i < 2; i++){
		cur_rk->dspmod[1][i] = cur_rk->dspmod[0][i];
		cur_rk->dspmod[0][i] = NULL;
	}

	if ((naibup = cur_rk_table->rk_modenaibu.org) != NULL)
		look_choose(&naibup, 1);	/* V3.1 */
}

 /**	モード表の内部形式を順次見ていき、使用表の選択及びモード表示文字列の
	選択を行っていく。但しflgが0ならスキップするだけ */
static /* V3.1 */
void
look_choose(naibupp, flg)
int	**naibupp;   /* モード表の内部表現の列へのポインタへのポインタ。
			look_cond()、evlcond()でも同様 */
int	flg;
{
	fast	int	naibu1, naibu2, branch, lcrsl;	/* V3.1 */
	int	*naibup;				/* V3.1 */

	naibup = *naibupp;

	while((naibu1 = *naibup++) != Terminator){
		switch(SHUBET(naibu1)){
			case 4: /* 表名 */
				if(flg) hyouse(LWRMSK(naibu1));
				break;
			case 2: /* 条件式 */
				branch = LWRMSK(naibu1); /* if;when */
				lcrsl = look_cond(&naibup, flg);
				if(branch == 0 && lcrsl) flg = 0;
				break;
			case 5: /* romkanがon・off時それぞれの
				   モード表示文字列 */
				naibu2 = *naibup++;
				if(flg) cur_rk->dspmod[0][LWRMSK(naibu1)] =
					    cur_rk_table->rk_dspmode.point[naibu2];
				break;
			case 6: /* romkanがそれぞれon・off時のモード表示
				   文字列を前のままに */
				if(flg) cur_rk->dspmod[0][LWRMSK(naibu1)] = 
					    cur_rk->dspmod[1][LWRMSK(naibu1)];
				break;
			default:
				BUGreport(6);
		}
	}

	*naibupp = naibup;
}

 /**	*naibupp が、内部表現の列で条件式を表すところを指している筈なので、
	それを評価し、真ならその続きを解釈しにいく。偽なら読み飛ばす。
	返値は、最初に評価した条件式の真偽値。*/
static /* V3.1 */
int
look_cond(naibupp, flg)
int	**naibupp, flg;
{
	fast	int	condrsl;	/* V3.1 */
	int	*naibup;

	naibup = *naibupp;

	/* 88/06/06 V3.1 */
	condrsl = evlcond(&naibup); /* 必ず評価しないといけないため */
	look_choose(&naibup, flg &= condrsl);
	*naibupp = naibup;
	return(flg);
}

 /** 条件式の真偽値の評価 */
static /* V3.1 */
int
evlcond(naibupp)
int	**naibupp;
{
	int	*naibup, tmpval[ARGMAX];	/* V3.1 */
	fast int	naibu1, retval = 0, imax;	/* V3.1 */
	fast int	i;			/* V3.1 */

	naibup = *naibupp;

	naibu1 = *naibup++;
	switch(SHUBET(naibu1)){
		case 7: /* 数値 */
			 retval = *naibup++; break;
		case 1: /* モード名 */
			 retval = cur_rk->rk_modesw.point[LWRMSK(naibu1)]; break;
		case 3: /* if;when */
			imax =  condarg[LWRMSK(naibu1)];
			for(i = 0; i < imax; i++)
				tmpval[i] = evlcond(&naibup);
			switch(LWRMSK(naibu1)){
			 /* 上から順にtrue,false,not,and,or */
				case 0: retval = !tmpval[0]; break;
				case 1: retval = tmpval[0]&& tmpval[1]; break;
				case 2: retval = tmpval[0]|| tmpval[1]; break;
				case 3: retval = 1; break;
				case 4: retval = 0; break;
				case 5: retval = (tmpval[0] == tmpval[1]);
					break;
				case 6: retval = (tmpval[0] != tmpval[1]);
					break;
				case 7: retval = ((unsigned int)tmpval[0] <
						  (unsigned int)tmpval[1]);
					break;
				case 8: retval = ((unsigned int)tmpval[0] >
						  (unsigned int)tmpval[1]);
					break;

			}
	}

	*naibupp = naibup;
	return(retval);
}

 /** num番目の表を、使用するものとして登録する。前・本・後処理の区別もする */
static /* V3.1 */
void
hyouse(num)
int	num;
{
	fast	int	*ptr = NULL;	/* V3.1 */

	switch(cur_rk_table->rk_hyo.point[num].hyoshu){
		case 1: ptr = cur_rk->rk_usehyo.usemaehyo; break;
		case 2: ptr = cur_rk->rk_usehyo.usehyo; break;
		case 3: ptr = cur_rk->rk_usehyo.useatohyo; break;
		default: BUGreport(11);
	}
	if (ptr == NULL) return;
	for(; *ptr != -1; ptr++) if(*ptr == num) return;
	*ptr = num;
	*++ptr = -1;
}

/* 88/06/02 V3.1 */
#ifndef OMRON_SPEC
 /** strcmpと同等  但し、'\（8進）;'も解釈する。*/
static /* V3.1 */
int
mystrcmp(s1, s2)
char	*s1, *s2;
{
	fast	char	c1, c2;	/* V3.1 */

	while((c1 = codeeval(&s1)) == (c2 = codeeval(&s2)))
		if(c1 == '\0') return(0);
	return(c1 > c2 ? 1 : -1);
}

 /** strcpyと同等 但し'\（8進）;'も解釈する。s1 <= s2なら正常動作するはず */
static /* V3.1 */
void
mystrcpy(s1, s2)
char	*s1, *s2;
{
	while(*s1++ = codeeval(&s2));
}

 /**	一文字の解釈を行う。普通の文字はそのまま、'\（8進）;'は実際のコードに
	直す。その後、文字列へのポインタを一文字分進めておく（少なくとも
	１バイト分進むことが保証されるはず）。*/
static /* V3.1 */
char	codeeval(sptr)
fast	 char	**sptr;		/* V3.1 */
{
	fast	char	c;		/* V3.1 */
	fast	char	code = 0;	/* V3.1 */

	if((c = *(*sptr)++) != '\\') return(c);
	while(c = *(*sptr)++, is_octal(c)){
		code <<= 3;
		code += ctov(c);
	}
	if(c != ';') BUGreport(12);
	return(code);
}
#endif

 /** romkanがon時のモード表示文字列を返す関数。無指定であってRK_DSPNILフラグが
     立っている時は空文字列を返す。*/
char	*romkan_dispmode()
{
	return(cur_rk->dspmod[0][0] == NULL && (cur_rk_table->flags & RK_DSPNIL) ?
	       "" : cur_rk->dspmod[0][0]);	/* V3.1 */
}

 /** romkanがoff時のモード表示文字列を返す関数。無指定であってRK_DSPNILフラグ
     が立っている時は空文字列を返す。*/
char	*romkan_offmode()
{
	return(cur_rk->dspmod[0][1] == NULL && (cur_rk_table->flags & RK_DSPNIL) ?
	       "" : cur_rk->dspmod[0][1]);	/* V3.1 */
}

/* 88/06/02 V3.1 */
 /** ファイル名からパス名を除いた部分の先頭を返す。*/
static
char	*ename(s)
char	*s;
{
	fast	char	*p;	/* V3.1 */

	p = strrchr(s, KUGIRI);
	return(p == NULL ? s : p + 1);
}

 /**	ファイル名のチェック。先頭（パス名は除く）が'1'〜'3'でないといけない。
	正当なものなら1〜3（前・本・後処理表の区別を表す）を返し、そうでないと
	0を返す。*/
#ifdef OMRON_LIB
static
#endif
int
filnamchk(s)
char	*s;
{
	fast	int	c;	/* V3.1 */

	c = *(ename(s)) - '0';
	return((1 <= c && c <= 3) ? c : 0);
}

/* 88/06/02 V3.1 */
static
void
ERMOPN(n) /* モード定義表がオープンできない */
fast	unsigned int	n;	/* V3.1 */
{
	static	char	*ermes[] = {
 /*  0 */	"Unprintable error",
		"Mode-hyo does't exist",
		"Can't open Mode-hyo",
		"Too many long Mode-hyo filenames",
	};

	if(n >= numberof(ermes)) n = 0;

	fprintf(stderr, "\r\nromkan: Mode-hyo %s ---\r\n", cur_rk_table->mcurfnm);
	fprintf(stderr, "\tError No.%d: %s.\r\n", n, ermes[n]);
	longjmp(cur_rk_table->env0, 1);
}

/* 88/06/03 V3.1 */
static
void
ERRMOD(n) /* モード定義表のエラー */
fast	unsigned int	n;	/* V3.1 */
{
	static	char	*ermes[] = {
 /*  0 */	"Table incomplete",
		"')' mismatch",
		"Unprintable error",
		"Illegal filename",
		"Illegal modename",
 /*  5 */	"Undefined mode",
		"Illegal content(s) of list",
		"Illegal ^,\\o,\\x or \\d expression",
		"Illegal defmode",
		"Unrecognized keyword",
 /* 10 */	"Incomplete string",
		"Search path specified after filename",
		"Argument must be a string",
		"Can't get home directory",
		"Illegal @-keyword",
 /* 15 */	"User doesn't exist",
 		"Illegal character",
	};

	if(n >= numberof(ermes)) n = 2;

	fprintf(stderr, "\r\nromkan: Mode-hyo %s%s ---\r\n%s\r\n",
			cur_rk_table->mcurdir, cur_rk_table->mcurfnm, cur_rk_table->mcurread);
	fprintf(stderr, "\tError No.%d: %s.\r\n", n, ermes[n]);
	fclose(cur_rk_table->modefile);
	longjmp(cur_rk_table->env0, 1);
}
