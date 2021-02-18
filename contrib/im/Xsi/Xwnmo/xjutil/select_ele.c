/*
 * $Id: select_ele.c,v 1.2 1991/09/16 21:35:25 ohm Exp $
 */
/*
 * Copyright 1989, 1990, 1991 by OMRON Corporation
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON and MIT not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON and MIT make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * OMRON, MIT AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL OMRON, MIT OR WNN CONSORTIUM BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author:	Seiji Kuwari	OMRON Corporation
 *				kuwa@omron.co.jp
 *				kuwa%omron.co.jp@uunet.uu.net
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
#define DD_MAX 512

#include <stdio.h>
#include "jslib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "xjutil.h"
#include "sxheader.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"
#include "xext.h"

#define printh(mm)\
  if(state == 0){\
    printf("%c.",((mm - dd[cc]) > 9)? mm - dd[cc] - 10  + 'A':mm - dd[cc] + '0');\
    printf(data[mm]);\
  }else{\
    printf("%c.",((mm > 9)? mm - 10  + 'A':mm + '0'));\
    printf(data[mm]);}
		    

#define kouho_len(x) (strlen(x) + 4)

static int cc; /* ima no gamen */
static int kosuu; /* kosuu of elements */
extern hilited_item;
extern max_line;

int
xw_select_one_element(c,kosuu1,init,title, bcnt,btable_t,b_tbl,key_tbl,comment)
char **c;
int kosuu1;
int init;
char *title;
int bcnt;
char *btable_t[];
int (**b_tbl)();
int (**key_tbl)();
char *comment;
{
  int d;
  unsigned int c1;
  int ret;

  kosuu = kosuu1;
  init_ichiran(c, kosuu1,init,title, comment);
  for(;;) {
    c1 = keyin() ;
    if (xjutil->sel_ret == -2) {
        end_ichiran();
	romkan_clear();
	return(-1);
    } else if (xjutil->sel_ret != -1) {
	d = find_hilite();
	xjutil->sel_ret = -1;
    } else {
	if ((c1 < 256) && (main_table[4][c1] != NULL)) {
	    if ((ret = (*main_table[4][c1])()) == 1) {
		d = find_hilite();
	    } else if (ret == -1) {
		end_ichiran();
		romkan_clear();
		return(-1);
	    } else {
		ring_bell();
		continue;
	    }
	} else {
	    ring_bell();
	    continue;
	}
    }
    if(d >= 0 && d < kosuu){
	romkan_clear();
	if(!bcnt) {
	    end_ichiran();
	    return(d);
	} else {
	    cc = d;
	    lock_ichiran();
	}
	init_keytable(bcnt, btable_t);
	for (;;) {
	    c1 = keyin() ;
	    if (xjutil->sel_ret == -2) {
		end_ichiran();
		end_keytable();
		romkan_clear();
		return(-1);
	    } else if (xjutil->sel_ret != -1) {
		d = xjutil->sel_ret;
		xjutil->sel_ret = -1;
		if((d < bcnt) && (b_tbl[d] != NULL)){
		    end_ichiran();
		    end_keytable();
		    (*b_tbl[d])();
    		    romkan_clear();
		    return(0);
		} else {
		    ring_bell();
		}
	    } else {
		if((c1 < 256) && (key_tbl[c1] != NULL)){
		    end_ichiran();
		    end_keytable();
		    (*key_tbl[c1])();
    		    romkan_clear();
		    return(0);
		} else {
		    ring_bell();
		    xjutil->sel_ret = -1;
		    continue;
		}
	    }
	}
    }
    romkan_clear();
  }
}

int
select_select()
{
    return(1); /* return mm from upper function */
}

int
quit_select()
{
    return(-1);
}

/*
 *
 *  Functions Called during select element (dictionaries).
 *
 */ 

/* jishodel for kensaku */
int kdicdel()
{
    char buf[512];

    sprintf(buf, "%s %s %s",
	    msg_get(cd, 77, default_message[77], xjutil->lang),
	    dicinfo[cc].fname,
	    msg_get(cd, 78, default_message[78], xjutil->lang));
    if(yes_or_no(buf) == 1){
	if(dic_delete_e(cur_env, dicinfo[cc].dic_no) == -1){
	    errorkeyin();
	    return(0);
	}
	return(1);
    }
    return(0);
}


int kdicuse()
{
    char buf[512];

    if(dicinfo[cc].enablef){
	sprintf(buf, "%s %s %s",
		msg_get(cd, 77, default_message[77], xjutil->lang),
		dicinfo[cc].fname,
		msg_get(cd, 79, default_message[89], xjutil->lang));
    }else{
	sprintf(buf, "%s %s %s",
		msg_get(cd, 77, default_message[77], xjutil->lang),
		dicinfo[cc].fname,
		msg_get(cd, 80, default_message[80], xjutil->lang));
    }
    if(yes_or_no(buf) == 1){
	if(js_dic_use(cur_env, dicinfo[cc].dic_no,!dicinfo[cc].enablef) == -1){
	    if_dead_disconnect(cur_env, -1);
	    errorkeyin();
	    return(0);
	}
	return(1);
    }
    return(0);
}

int kdiccom()
{
    w_char com[512];
    WNN_DIC_INFO dic;
    WNN_FILE_INFO_STRUCT file;

    com[0] = 0;
    init_input(msg_get(cd, 39, default_message[39], xjutil->lang),
	       msg_get(cd, 81, default_message[81], xjutil->lang),
	       msg_get(cd, 8, default_message[8], xjutil->lang));
    cur_text->max_pos = 127;
    clr_line_all();
    throw_col(0);
    kk_cursor_normal();
    if(kana_in(UNDER_LINE_MODE ,com,512) == -1) {
	end_input();
	return(0);
    }
    end_input();
    if(js_dic_info(cur_env,dicinfo[cc].dic_no,&dic) < 0) {
	if_dead_disconnect(cur_env, -1);
	return(0);
    }
    if (js_file_info(cur_env,dic.body,&file) < 0) {
	if_dead_disconnect(cur_env, -1);
	return(0);
    }
    if(js_file_comment_set(cur_env, file.fid, com) == -1){
	if_dead_disconnect(cur_env, -1);
	errorkeyin();
	return(0);
    }
    return(1);
}

/*
 *
 *  Functions Called during select element (dic_entries).
 *
 */ 

/* Defined in kensaku.c */
extern struct wnn_jdata *word_searched;

int
kworddel()
{
    char buf[512];
    int type;

    type = dicinfo[find_dic_by_no(word_searched[cc].dic_no)].type;
    if(type != WNN_UD_DICT && type != WNN_REV_DICT
#ifdef	CONVERT_with_SiSheng
		&& type != CWNN_REV_DICT
#endif	/* CONVERT_with_SiSheng */
       ){
	print_msg_getc(msg_get(cd, 82, default_message[82], xjutil->lang),
		       NULL, NULL, NULL);
	return(0);
    }
    if(dicinfo[find_dic_by_no(word_searched[cc].dic_no)].rw == WNN_DIC_RDONLY){
	print_msg_getc(msg_get(cd, 83, default_message[83], xjutil->lang),
		       NULL, NULL, NULL);
	return(0);
    }
	
    strcpy(buf, msg_get(cd, 84, default_message[84], xjutil->lang));
    sStrcpy(buf + strlen(buf), word_searched[cc].kanji);
    sprintf(buf + strlen(buf), "%s",
	    msg_get(cd, 85, default_message[85], xjutil->lang));
    if(yes_or_no(buf) == 1){
	if(js_word_delete(cur_env, word_searched[cc].dic_no,
			  word_searched[cc].serial) == -1){
	    if_dead_disconnect(cur_env, -1);
	    errorkeyin();
	    return(0);
	}
	return(1);
    }
    return(0);
}

int
kworduse()
{
    char buf[512];
    register struct wnn_jdata *jd;
    extern int wnn_word_use_initial_hindo;

    if(dicinfo[find_dic_by_no(word_searched[cc].dic_no)].hindo_rw == WNN_DIC_RDONLY){
	print_msg_getc(msg_get(cd, 86, default_message[86], xjutil->lang),
		       NULL, NULL, NULL);
	return(0);
    }
    strcpy(buf, msg_get(cd, 84, default_message[84], xjutil->lang));
    sStrcpy(buf + strlen(buf), word_searched[cc].kanji);
    if(word_searched[cc].hindo == -1 || 
       (word_searched[cc].int_hindo == -1 && !word_searched[cc].int_ima)){
	sprintf(buf + strlen(buf), "%s",
		msg_get(cd, 87, default_message[87], xjutil->lang));
    }else{
	sprintf(buf + strlen(buf), "%s",
		msg_get(cd, 88, default_message[88], xjutil->lang));
    }
    if(yes_or_no(buf) == 1){
	if(js_word_info(cur_env, word_searched[cc].dic_no,
			word_searched[cc].serial, &rb) == -1) {
	    if_dead_disconnect(cur_env, -1);
	    errorkeyin();
	    return(0);
	}
	jd = (struct wnn_jdata *)(rb.buf);
	if(jd->hindo != -1){
	    if(js_hindo_set(cur_env, word_searched[cc].dic_no,
			    word_searched[cc].serial, WNN_IMA_OFF,
			    WNN_ENTRY_NO_USE) == -1) {
		if_dead_disconnect(cur_env, -1);
		errorkeyin();
		return(0);
	    }
	} else {
	    if(js_hindo_set(cur_env, word_searched[cc].dic_no,
			    wnn_word_use_initial_hindo >> 7 ,
			    wnn_word_use_initial_hindo & 0x7f) == -1) {
		if_dead_disconnect(cur_env, -1);
		errorkeyin();
		return(0);
	    }
	}
	return(1);
    }
    return(0);
}

int kwordcom()
{
    w_char com[512];
    int type;

    type = dicinfo[find_dic_by_no(word_searched[cc].dic_no)].type;
    if(type != WNN_UD_DICT && type != WNN_REV_DICT
#ifdef	CONVERT_with_SiSheng
		&& type != CWNN_REV_DICT
#endif	/* CONVERT_with_SiSheng */
	){
	print_msg_getc(msg_get(cd, 89, default_message[89], xjutil->lang),
		       NULL, NULL, NULL);
	return(0);
    }
    if(dicinfo[find_dic_by_no(word_searched[cc].dic_no)].rw == WNN_DIC_RDONLY){
	print_msg_getc(msg_get(cd, 90, default_message[90], xjutil->lang),
		       NULL, NULL, NULL);
	return(0);
    }

    com[0] = 0;
    init_input(msg_get(cd, 39, default_message[39], xjutil->lang),
	       msg_get(cd, 81, default_message[81], xjutil->lang),
	       msg_get(cd, 8, default_message[8], xjutil->lang));
    cur_text->max_pos = 127;
    clr_line_all();
    throw_col(0);
    kk_cursor_normal();
    if(kana_in(UNDER_LINE_MODE ,com,512) == -1) {
	end_input();
	return(0);
    }
    end_input();
    if(js_word_comment_set(cur_env, word_searched[cc].dic_no,
			   word_searched[cc].serial, com) == -1){
	if_dead_disconnect(cur_env, -1);
	errorkeyin();
	return(0);
    }
    return(1);
}

/*
  Local Variables:
  eval: (setq kanji-flag t)
  eval: (setq kanji-fileio-code 0)
  eval: (mode-line-kanji-code-update)
  End:
*/
