#ifndef lint
static char *rcs_id = "$Id: kankana.c,v 1.2 1991/09/16 21:30:25 ohm Exp $";
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
/*#include "commonhd.h" */

#define FZK_FILE "/usr/local/lib/wnn/dic/pubdic/full.fzk"
#define KIHON_DIC "/usr/local/lib/wnn/dic/pubdic/pd-kihon.sys"
#define SETTO_DIC "/usr/local/lib/wnn/dic/pubdic/pd-setsuji.sys"
#define REV_DIC "/usr/local/lib/wnn/dic/pubdic/pd-kihon.sysR"
#define REV_DIC_SETTO "/usr/local/lib/wnn/dic/pubdic/pd-setsuji.sysR"

WNN_JSERVER_ID	*js;
struct wnn_env *env, *rev_env;
static struct wnn_ret_buf rb = {0, NULL};

int	dcnt,scnt;

char yomi[1024];
char kanji[5000];

int kihon_file, setto_file, rev_file, fzk_file, rev_setto_file;
int kihon_dic_no, setto_dic_no, rev_dic_no, rev_setto_dic_no;

main(argc, argv)
int argc;
char **argv;
{
    char *mname = "";
    rb.buf = (char *)malloc((unsigned)(rb.size = 0));
    if(argc > 1) mname = argv[1];

    if((js=js_open(mname)) == NULL) err();

    if((env=js_connect(js, "kana")) == NULL) err(); 
    if((fzk_file = js_file_read(env,FZK_FILE)) == -1) err();
    if((kihon_file = js_file_read(env,KIHON_DIC)) == -1) err();
    if((setto_file = js_file_read(env,SETTO_DIC)) == -1) err();

    if(js_fuzokugo_set(env,fzk_file) == -1) err();
    if((kihon_dic_no = js_dic_add(env,kihon_file,-1,1,WNN_DIC_RDONLY, WNN_DIC_RDONLY, NULL, NULL)) == -1) err();
    if((setto_dic_no=js_dic_add(env,setto_file,-1,1,WNN_DIC_RDONLY, WNN_DIC_RDONLY, NULL, NULL)) == -1) err();
    p_set(env);

    if((rev_env=js_connect(js, "kanji")) == NULL) err();

    if((fzk_file = js_file_read(rev_env,FZK_FILE)) == -1) err();
    if((rev_file = js_file_read(rev_env,REV_DIC)) == -1) err();
    if((rev_setto_file = js_file_read(rev_env,REV_DIC_SETTO)) == -1) err();

    if(js_fuzokugo_set(rev_env,fzk_file) == -1) err();
    if((rev_dic_no = js_dic_add(rev_env,rev_file,-1,1,WNN_DIC_RDONLY, WNN_DIC_RDONLY, NULL, NULL)) == -1) err();
    if((rev_setto_dic_no = js_dic_add(rev_env,rev_setto_file,-1,1,WNN_DIC_RDONLY, WNN_DIC_RDONLY, NULL, NULL)) == -1) err();
    p_set(rev_env);

#ifdef DEBUG
printf("Now discard file push any key\n");
getchar();
    js_file_discard(rev_env, rev_file);
printf("Now discard file\n");
#endif

    henkan();
    js_close(js);	
}

henkan()
{
    wchar u[1024];
    struct wnn_env *c_env = env;
    struct wnn_dai_bunsetsu *dp;

    for(;;){
	if(c_env == env){printf("yomi> ");fflush(stdout);}
	else {printf("kanji> ");fflush(stdout);}
	if(gets(yomi) == NULL)return;
	if(yomi[0] == '!')return;
	if(yomi[0] == '@'){
	    c_env = (c_env == env)? rev_env:env;
	    continue;
	}
	strtows(u,yomi);
	dcnt =js_kanren(c_env,u,WNN_BUN_SENTOU, NULL, WNN_VECT_KANREN, WNN_VECT_NO,&rb);
	dp = (struct wnn_dai_bunsetsu *)rb.buf;
	print_kanji(dp,dcnt );
    }
}

print_kanji(dlist, cnt)
struct wnn_dai_bunsetsu	*dlist;
int	cnt;
{
    int i;
    struct wnn_sho_bunsetsu  *sbn;

    if (dlist == 0) return;
    putchar('\n');
    for ( ; cnt > 0; dlist++, cnt --) {
	sbn = dlist->sbn;
	for (i = dlist->sbncnt; i > 0; i--) {
	    putws(sbn->kanji); printf("-"); 
	    putws(sbn->fuzoku); printf(" ");
	    sbn++;
	}
	printf("|"); 
    }
    putchar('\n');
    fflush(stdout);
}

p_set(env1)
struct wnn_env *env1;
{
struct wnn_param pa;
	pa.n= 2;		/* n_bun */
	pa.nsho = 10;	/* nshobun */
	pa.p1 = 2;	/* hindoval */
	pa.p2 = 40;	/* lenval */
	pa.p3 = 0;	/* jirival */
	pa.p4 = 100;	/* flagval */
	pa.p5 = 5;	/* jishoval */
	pa.p6 = 1;	/* sbn_val */
	pa.p7 = 15;	/* dbn_len_val */
	pa.p8 = -20;	/* sbn_cnt_val */
	pa.p9 = 0;	/* kan_len_val */

 js_param_set(env1,&pa);
}


putwchar(x)
unsigned short x;
{
    putchar( x >> 8);
    putchar( x );
}

putws(s)
unsigned short *s;
{
    while(*s) putwchar(*s++);
}

strtows(u,e)
wchar *u;
unsigned char *e;
{int x;
 for(;*e;){
	x= *e++;
	if(x & 0x80) x = (x << 8)  | *e++;
	*u++= x;
 }
 *u=0;
}

err()
{
    printf(wnn_perror());
    printf("\n bye.\n");
    exit(1);
}
