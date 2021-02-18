#ifndef lint
static char *rcs_id = "$Id: wdreg.c,v 1.2 1991/09/16 21:32:31 ohm Exp $";
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
/*
  UJIS ファイルを読みとって、単語登録を行なう。
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include "commonhd.h"
#include "config.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"

extern struct je **jeary;

char *def_server;
struct JT jt;
struct wnn_file_head file_head;
int dic_no = -1;
char *server_n;
char *env_n = "wdreg";
char *fname = NULL;
int fid;
int client = 0;			/* server site dict */

WNN_JSERVER_ID	*js;
struct wnn_env *env, *rev_env;
static struct wnn_ret_buf rb = {0, NULL};

static void
usage()
{
    fprintf(stderr, "wdreg [-D server_name] [-n env_name][-d dic_no][-L] [file_name]  < text dict\n");
    fprintf(stderr, "file_name or -d dic_no must be specified\n");
    fprintf(stderr, "default env_name = wdreg\n");
    fprintf(stderr, "default server_name = %s\n", def_server);
    fprintf(stderr, "L is to specify that the file at the client site.\n");
    exit(1);
}

static void
err()
{
    printf((char *)wnn_perror());
    printf("\n bye.\n");
    exit(1);
}

void
main(argc, argv)
int argc;
char **argv;
{
    extern int optind;
    extern char *optarg;
    struct wnn_dic_info *info;
    int c;
    int k;
    int num;
    extern char *getenv();

    if(getenv(WNN_JSERVER_ENV)){
	def_server = getenv(WNN_JSERVER_ENV);
    }else {
	def_server = "";
    }
    server_n = def_server;

    while ((c = getopt(argc,argv,"D:n:d:L")) != EOF) {
	switch(c){
	case 'D':
	    server_n = optarg;
	    break;
	case 'n':
	    env_n = optarg;
	    break;
	case 'd':
	    dic_no = atoi(optarg);
	    break;
	case 'L':
	    client = 1;
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
    if(argc > 1){
	fname = argv[1];
    }
    if(fname && dic_no != -1)usage();
    if(!fname && dic_no == -1)usage();

    rb.buf = (char *)malloc((unsigned)(rb.size = 0));

    init_heap(MAX_ENTRIES * HEAP_PER_LINE,
	      MAX_ENTRIES * YOMI_PER_LINE,
	      MAX_ENTRIES, MAX_ENTRIES, stdin);

#ifdef CHINESE
  { int which_dict;
  ujis_header(&which_dict);
  }
#else
    ujis_header();
#endif
    read_ujis(NORMAL, 0, 0);

    if((js = js_open(server_n, WNN_TIMEOUT)) == NULL) err();
    if((env=js_connect(js, env_n)) == NULL) err(); 
    if(fname){
	if(client){
	    if(access(fname, F_OK) == -1){
		if(js_dic_file_create_client(env, fname, WNN_REV_DICT, "", "", "") == -1){
		    err();
		}
	    }
	    if((fid = js_file_send(env, fname)) == -1){
		err();
	    }
	}else{
	    WNN_FILE_STAT s;
	    if(js_file_stat(env, fname, &s) == -1){
		if(js_dic_file_create(env, fname, WNN_REV_DICT, "", "", "") == -1){
		    err();
		}
	    }
	    if((fid = js_file_read(env, fname)) == -1) err();
	}
	if((dic_no = js_dic_add(env, fid, -1, 0, 0, WNN_DIC_RW, WNN_DIC_RW, "", "")) == -1)
	    err();
    }

    if((num = js_dic_list(env, &rb)) == -1) err();
    
    info = (struct wnn_dic_info *)(rb.buf);
    for(k = 0 ;k < num; k++){
	if(info[k].dic_no == dic_no) break;
    }
    if(k == num){
	fprintf(stderr, "指定された番号の辞書は、環境に存在しません。\n");
	exit(1);
    }
    if(info[k].type != WNN_UD_DICT && info[k].type != WNN_REV_DICT){
	fprintf(stderr, "指定された番号の辞書は、登録可能ではありません。\n");
	exit(1);
    }
    for(k = 0 ; k < jt.maxserial; k++){
	if(js_word_add(env, dic_no, jeary[k]->yomi, jeary[k]->kan,
		       jeary[k]->comm,jeary[k]->hinsi, jeary[k]->hindo) == -1){
	    err();
	}
    }
    if(fname){
	if(client){
	    if(js_file_receive(env, fid, "") == -1) err();
	}else{
	    if(js_file_write(env, fid, "") == -1) err();
	}
	fprintf(stderr, "Wrote the file back.\n");
    }
    exit(0);
}
