#ifndef lint
static char *rcs_id = "$Id: wnnstat.c,v 1.2 1991/09/16 21:32:33 ohm Exp $";
#endif /* lint */
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
/*	Version 4.0
 */
#include <stdio.h>
#include "jslib.h"
#include "commonhd.h"
#include "config.h"
#include "msg.h"

WNN_JSERVER_ID	*js;
struct wnn_ret_buf rb = {0, NULL};
#define WNNSTAT_ENV "This_is_wnnstat"

#define W_WHO 0
#define W_ENV 1
#define W_ALL 2
#define W_FILE 3
#define W_DIC 4
#define W_DIC_ALL 5
#define W_VERSION 6

static void printall(), printpat(), err(), usage(), who(),
  dic(), dic_all(), file(), wsttost(), out();

int ocode = TTY_KCODE;

int com = W_WHO;

struct msg_cat *cd;

void
main(argc,argv)
int argc;
char **argv;
{
    char *tmp;
    int c, i,j;
    WNN_ENV_INFO *w;
    static char lang[64] = { 0 };
    extern int optind;
    extern char *optarg;
    extern char *getenv();
    char nlspath[64];
    char *p;

    if ((p = getenv("LANG")) != NULL) {
	strcpy(lang, p);
        lang[5] = '\0';
    } else {
        lang[0] = '\0';
    }

    if (*lang == '\0')
#ifdef CHINESE
	strcpy(lang, "zh_CN");
#else
	strcpy(lang, WNN_DEFAULT_LANG);
#endif

    while ((c = getopt(argc,argv,"EewdDfFUSJVL:")) != EOF) {
	switch(c){
	case 'w':
	    com = W_WHO;
	    break;
	case 'E': 
	    com = W_ALL;
	    break;
	case 'e': 
	    com = W_ENV;
	    break;
	case 'd':
	    com = W_DIC;
	    break;
	case 'D':
	    com = W_DIC_ALL;
	    break;
	case 'f':
	case 'F':
	    com = W_FILE;
	    break;
	case 'V':
	    com = W_VERSION;
	    break;
	case 'U':
	    ocode = J_EUJIS;
	    break;
	case 'J':
	    ocode = J_JIS;
	    break;
	case 'S':
	    ocode = J_SJIS;
	    break;
	case 'L':
	    strcpy(lang, optarg);
	    break;
	default:
	    usage();
	    break;
	}
    }
    if (optind) {
	optind--;
	argc -= optind;
	argv += optind;
    }
    strcpy(nlspath, LIBDIR);
    strcat(nlspath, "/%L/%N");
    cd = msg_open("wnnstat.msg", nlspath, lang);

    rb.buf = (char *)malloc((unsigned)(rb.size = 0));
    if(argc > 1){
	tmp = argv[1];
    }else if(strcmp(lang, "zh_CN") && getenv(WNN_JSERVER_ENV)){
	tmp = getenv(WNN_JSERVER_ENV);
    }else if(!strcmp(lang, "zh_CN") && getenv(WNN_CSERVER_ENV)){
	tmp = getenv(WNN_CSERVER_ENV);
    }else {
	tmp = "";
    }

    if((js=js_open_lang(tmp, lang, WNN_TIMEOUT)) == NULL){
	out("wnnstat:");
	out(tmp);
	out(wnn_perror_lang(lang));
	putchar('\n');
/*	out(" jserver と接続出来ません。\n"); */
	exit(1);
    }
    switch (com) {
    case W_WHO:
	who(lang);
	break;
    case W_ALL:
	if((c = js_env_list(js, &rb)) == -1) err();
	w=(WNN_ENV_INFO *)rb.buf;
	printall(w, c);
	break;
    case W_ENV:
	if((c = js_env_list(js, &rb)) == -1) err();
	w=(WNN_ENV_INFO *)rb.buf;
	printpat(w, c);
	break;
    case W_DIC:
	dic();
	break;
    case W_DIC_ALL:
	dic_all();
	break;
    case W_FILE:
	file();
	break;
    case W_VERSION:
	js_version(js, &i, &j);
	printf("Jserver Version %x, Jlib Version %x\n", i, j);
    }
    exit (0);
}

static void
printall(w, c)
WNN_ENV_INFO *w;
int c;
{
    int i, j;
    extern char *msg_get();
    /*
    out("Env No.\t環境名\t参照数\t付属語\t辞書数(辞書No.)\t\tファイル\n");
    */
    out(msg_get(cd, 1, "Env No.\tEnv Name\tRefs\tGrammer\tDicts(Dict No.)\t\tFile\n", NULL));
    for(i = 0 ; i < c ; i++, w++){
	out("%d\t%s\t%d\t%d\t%d(", w->env_id, w->env_name, w->ref_count, w->fzk_fid,
	       w->jishomax);
	for(j = 0 ; j < w->jishomax; j++){
	    if(w->jisho[j] != -1)
		out("%d ", w->jisho[j]);
	}
	out(")\t");
	for(j = 0 ; j < WNN_MAX_FILE_OF_AN_ENV; j++){
	    if(w->file[j] != -1)
		out("%d ", w->file[j]);
	}
	putchar ('\n');
    }
}

static void
printpat(w, c)
WNN_ENV_INFO *w;
int c;
{
    int i;
    /*
    out("Env No.\t環境名\t参照数\n");
    */
    out(msg_get(cd, 2, "Env No.\tEnv Name\tRefs\n"));
    for(i = 0 ; i < c ; i++, w++){
	out("%d\t%s\t%d\n",w->env_id, w->env_name, w->ref_count);
    }
}

static void
err()
{
    out(wnn_perror());
    putchar('\n');
    exit(255);
}

static void
usage()
{
    fprintf(stderr, "wnnstat [-weEdDfFSUJL] [server_name]\n");
    exit(1);
}

static void
who(lang)
char *lang;
{
    WNN_JWHO *w;
    int c, k, j;
    struct wnn_env *env;

    if((env = js_connect_lang(js, WNNSTAT_ENV, lang)) == NULL) err();

    if((c = js_who(js, &rb)) == -1) err();
    w=(WNN_JWHO *)rb.buf;
    /*
    out("ユーザ名:ホスト名\t(ソケットNo.)\t環境番号\n");
    */
    out(msg_get(cd, 3, "User name:Host name\t(socket)\tEnv No.\n", NULL));
    for(k = 0 ; k < c ; k++, w++){
	if(w->env[0] == env->env_id) continue;
	out("%s:%s\t\t(%d)\t\t", w->user_name, w->host_name, w->sd);
	for(j = 0 ; j < WNN_MAX_ENV_OF_A_CLIENT; j++){
	    if(w->env[j] != -1){
		out("%d ", w->env[j]);
	    }
	}
	out("\n");
    }
    exit(c - 1);
}

static void
dic()
{
    char ch[256];
    WNN_DIC_INFO *w;
    int c, k;
    if((c = js_dic_list_all(js, &rb)) == -1) err();
    w=(WNN_DIC_INFO *)rb.buf;
    /*
    out("No.\t種類\tニックネーム\tファイル\t\n");
    */
    out(msg_get(cd, 4, "No.\tDict type\tName\tFile\n"));
    for(k = 0 ; k < c ; k++, w++){
	wsttost(ch, w->comment);
	out("%d:%s\t%s\t%d:%s", w->dic_no, 
#if	defined(CONVERT_by_STROKE) || defined(CONVERT_with_SiSheng)
	   w->type&0xff00 ? ((w->type&0xff00) == 0x100 ?
				bwnn_dic_types[0x00ff&w->type] :
				    cwnn_dic_types[0x00ff&w->type]) :
		wnn_dic_types[w->type],
#else
	    wnn_dic_types[w->type],
#endif
	       ch, w->body, w->fname);
	if(w->hindo != -1){
	    out("(%d)\n", w->hindo);
	}else{
	    out("\n");
	}
    }
}

static void
dic_all()
{
    char ch[256];
    WNN_DIC_INFO *w;
    int c, k;
    if((c = js_dic_list_all(js, &rb)) == -1) err();
    w=(WNN_DIC_INFO *)rb.buf;
    /*
    out("No.種類\t正/逆\t語数\t更新(頻)使用 プライオリティ\t[ニックネーム]\tファイル[(頻度)]\t[パスワード(頻度)]\n");
    */
    out(msg_get(cd, 5, "No.Dict type\tN/R\tWords\tR/W(F)Use Priorty\t[Name]\tFile[(Freq)]\t[Passwd(Freq)]\n", NULL));
    for(k = 0 ; k < c ; k++, w++){
	wsttost(ch, w->comment);
	out("%d:%s\t%s\t%d\t%s(%s)\t%s   %d\t",
	       w->dic_no, 
#ifdef CHINESE
	   w->type&0xff00 ? ((w->type&0xff00) == 0x100 ?
				bwnn_dic_types[0x00ff&w->type] :
				    cwnn_dic_types[0x00ff&w->type]) :
		wnn_dic_types[w->type],
#else
	    wnn_dic_types[w->type],
#endif
	    /*
	    w->rev ? "逆変換":"正変換",
	       w->gosuu, 
	       (w->rw)? "不":"可",
	       (w->hindo_rw)? "不":"可",
	       (w->enablef)? "中":"停止",
	       */
	    w->rev ? msg_get(cd, 6, "Rev", NULL) :
		     msg_get(cd, 7, "Norm", NULL),
	       w->gosuu, 
	       (w->rw)? msg_get(cd, 8, "Y", NULL):msg_get(cd, 9, "N", NULL),
	       (w->hindo_rw)? msg_get(cd, 8, "Y", NULL):msg_get(cd, 9, "N", NULL),
	       (w->enablef)? msg_get(cd, 10, "Y", NULL): msg_get(cd, 11, "N", NULL),

	       w->nice);
	if(*ch) out("%s\t", ch);
	out("%d:%s\t", w->body, w->fname);
	if(w->hindo != -1){
	    out("(%d:%s)\t",w->hindo, w->hfname);
	}
	if(w->passwd[0])out("%s", w->passwd);
	if(w->hpasswd[0])out("(%s)", w->hpasswd);
	out("\n");
    }
}


static void
file()
{
    char *d;
    WNN_FILE_INFO_STRUCT *w;
    int c, k;
    if((c = js_file_list_all(js, &rb)) == -1) err();
    w=(WNN_FILE_INFO_STRUCT *)rb.buf;
    /*
    out("Fid\t種類\t場所\t参照数\t\tファイル名\n");
    */
    out(msg_get(cd, 12, "Fid\tType\tL/R\tRefs\t\tFile\n", NULL));
    for(k = 0 ; k < c ; k++, w++){
	switch (w->type){
	case WNN_FT_DICT_FILE:
	    /*
	    d = "辞書";
	    */
	    d = msg_get(cd, 13, "Dict", NULL);
	    break;
	case WNN_FT_HINDO_FILE:
	    /*
	    d = "頻度";
	    */
	    d = "頻度";msg_get(cd, 14, "Freq", NULL);
	    break;
	case WNN_FT_FUZOKUGO_FILE:
	    /*
	    d = "付属語";
	    */
	    d = msg_get(cd, 15, "Gram", NULL);
	    break;
	}
	out("%d\t%s\t%s\t%d\t%s\n", w->fid, d, 
	       w->localf ? "LOCAL": "REMOTE",
	       w->ref_count, w->name);
    }
}


static void
wsttost(c, w)
char *c;
w_char *w;
{
    while(*w){
	if(*w & 0xff00)
	    *c++ = (*w & 0xff00) >> 8;
	*c++ = *w++ & 0x00ff;
    }
    *c = 0;
}

extern	int eujis_to_jis8(), eujis_to_sjis();

static void
out(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)
char *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10, *a11, *a12, *a13;
{
    int len;
    char buf[1024];
    char jbuf[1024];
    sprintf(buf, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);

    len = strlen(buf);
    switch(ocode){
    case J_EUJIS:
	strncpy(jbuf, buf, len + 1);
	break;
    case J_JIS:
	eujis_to_jis8(jbuf, buf, len + 1);
	break;
    case J_SJIS:
	eujis_to_sjis(jbuf, buf, len + 1);
	break;
    }
    printf("%s", jbuf);
}
