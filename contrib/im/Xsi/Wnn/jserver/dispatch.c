/*
 * $Id: dispatch.c,v 1.2 1991/09/16 21:31:21 ohm Exp $
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
/*	Version 4.0
 */
/*
 *	Command Dispatch routine
 */

#include <stdio.h>

#include "commonhd.h"
#include "demcom.h"
#include "de_header.h"

extern int cur_clp;

void expand_file_name();

void
do_command()
{
 register int command;

 wnn_errorno = 0;
 command = get4_cur();	/* get command */

#define CASE(X) case (X): error1("X(%d): cur_clp = %d\n", command, cur_clp);

 switch(command){
	CASE(JS_VERSION)
		put4_cur(JSERVER_VERSION);
		putc_purge();
		break;

	CASE(JS_OPEN)
		js_open();
		break;

	CASE(JS_CLOSE)
		js_close();
		break;
/* */
	CASE(JS_CONNECT)
		js_connect();
		break;

	CASE(JS_DISCONNECT)
		js_disconnect();
		break;

	CASE(JS_ENV_EXIST)
		js_env_exist();
		break;

	CASE(JS_ENV_UN_STICKY)
		js_env_un_sticky();
		break;

	CASE(JS_ENV_STICKY)
		js_env_sticky();
		break;
/* */
	CASE(JS_PARAM_SET)
		js_param_set();
		break;

	CASE(JS_PARAM_GET)
		js_param_get();
		break;
/* */
	CASE(JS_MKDIR)
		js_mkdir();
		break;

	CASE(JS_ACCESS)
		js_access();
		break;

	CASE(JS_FILE_STAT)
		js_file_stat();
		break;

	CASE(JS_FILE_INFO)
		js_file_info();
		break;

	CASE(JS_FILE_LIST_ALL)
		js_file_list_all();
		break;

	CASE(JS_FILE_LIST)
		js_file_list();
		break;

	CASE(JS_FILE_LOADED)
		js_file_loaded();
		break;

	CASE(JS_FILE_LOADED_LOCAL)
		js_file_loaded_local();
		break;

	CASE(JS_FILE_READ)
		js_file_read();
		break;

	CASE(JS_FILE_WRITE)
		js_file_write();
		break;

	CASE(JS_FILE_SEND)
		js_file_send();
		break;

	CASE(JS_FILE_RECEIVE)
		js_file_receive();
		break;

	CASE(JS_HINDO_FILE_CREATE)
		js_hindo_file_create();
		break;

	CASE(JS_DIC_FILE_CREATE)
		js_dic_file_create();
		break;

	CASE(JS_FILE_REMOVE)
		js_file_remove();
		break;

	CASE(JS_FILE_DISCARD)
		js_file_discard();
		break;
	CASE(JS_FILE_COMMENT_SET)
		js_file_comment_set();
		break;
	CASE(JS_FILE_PASSWORD_SET)
		js_file_password_set();
		break;

/* */
	CASE(JS_DIC_ADD)
		js_dic_add();
		break;

	CASE(JS_DIC_DELETE)
		js_dic_delete();
		break;

	CASE(JS_DIC_USE)
		js_dic_use();
		break;

	CASE(JS_DIC_LIST)
		js_dic_list();
		break;

	CASE(JS_DIC_LIST_ALL)
		js_dic_list_all();
		break;

	CASE(JS_DIC_INFO)
		js_dic_info();
		break;
/* */
	CASE(JS_FUZOKUGO_SET)
		js_fuzokugo_set();
		break;

	CASE(JS_FUZOKUGO_GET)
		js_fuzokugo_get();
		break;
/* */
	CASE(JS_WORD_ADD)
		js_word_add();
		break;

	CASE(JS_WORD_DELETE)
		js_word_delete();
		break;

	CASE(JS_WORD_SEARCH)
		js_word_search();
		break;

	CASE(JS_WORD_SEARCH_BY_ENV)
		js_word_search_by_env();
		break;

	CASE(JS_WORD_INFO)
		js_word_info();
		break;
	CASE(JS_WORD_COMMENT_SET)
		js_word_comment_set();
		break;
/* */
	CASE(JS_KANREN)
		do_kanren();
		putc_purge();
		break;

	CASE(JS_KANTAN_DAI)
		do_kantan_dai();
		putc_purge();
		break;

	CASE(JS_KANTAN_SHO)
		do_kantan_sho();
		putc_purge();
		break;

	CASE(JS_KANZEN_DAI)
		do_kanzen_dai();
		putc_purge();
		break;

	CASE(JS_KANZEN_SHO)
		do_kanzen_sho();
		putc_purge();
		break;

	CASE(JS_HINDO_SET)
		js_hindo_set();
		break;

/* */
	CASE(JS_WHO)
		js_who();
		break;

	CASE(JS_ENV_LIST)
		js_env_list();
		break;
	CASE(JS_KILL)
		js_kill();
		break;
	CASE(JS_HINDO_FILE_CREATE_CLIENT)
		js_hindo_file_create_client();
		break;
	CASE(JS_HINSI_LIST)
		js_hinsi_list();
		break;
	CASE(JS_HINSI_NAME)
		js_hinsi_name();
		break;
	CASE(JS_HINSI_NUMBER)
		js_hinsi_number();
		break;
	CASE(JS_HINSI_DICTS)
		js_hinsi_dicts();
		break;
	CASE(JS_HINSI_TABLE_SET)
		js_hinsi_table_set();
		break;
	default:
		error1("unknown command %x" , command);
    }
}

/*
	communication routine
 */

void
get_file_name(p) register char *p;
{
 gets_cur(p);
 if(p[0] == 0){
     return;
 }
 expand_file_name(p);
}

void
expand_file_name(p)
register char *p;
{
    char path[FILENAME];
    register char *q;
    if(*p != '/'){
	strcpy(path,jserver_dir);
/*	strcat(path,c_c->user_name);   */
	strcat(path,"/");
	strcat(path, p);
	strcpy(p,path);
    }
    for(q=p; *q++;) ;
    q -=2;
    for(;;){
	if(*q != '/') break;
	*q-- = '\0';
    }
/*
fprintf(stderr,"file_name=%s\n",p);
*/
}

void
error_ret()
{
  put4_cur(-1);
  if(wnn_errorno == 0){
      put4_cur(WNN_SOME_ERROR);
  }else{
      error1("Error %s(%d): cur_clp = %d\n", wnn_perror(),wnn_errorno, cur_clp);
      put4_cur(wnn_errorno);
  }
  putc_purge();
}
