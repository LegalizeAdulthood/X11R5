/*
 * $Id: wnnrc_op.c,v 1.3 1991/09/16 21:37:30 ohm Exp $
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
/* uumrc operations */

#include "stdio.h"
#include <pwd.h>
#include "commonhd.h"
#include "config.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "ext.h"
#include "rk_spclval.h"
#include "rk_fundecl.h"

extern char     *getenv();
extern FILE	*fopen();

static char open_uumrc_filenm[1024];

static int
strmatch(s1, s2)
char	*s1, *s2;
{
    while(1) {
	if(*s1 == '\0') return(*s2 == '\0');
	if(*s1 != '*') {
	    if(*s1 != *s2) return(0);
	    s1++; s2++;
	    continue;
	}
	s1++;
	do{
	    if(strmatch(s1, s2)) return(1);
	} while(*s2++ != '\0');
	return(0);
    }
}

static void
err_expand(f, s)
char *f, *s;
{
    print_out2("In uumrc file \"%s\", I could not expand %s.", f, s);
}

static int
change_ascii_to_int(st,dp)
char *st;
int *dp;
{
    register int total,flag;

    total = 0;
    flag = 0;
    while(*st != '\0') {
	if (isdigit(*st)) {
	    total = total * 10 + (*st - '0');
	} else if (*st == '+') {
	    if (flag != 0) { return(-1); }
	    flag = 1;
	} else if (*st == '-') {
	    if (flag != 0) { return(-1); }
	    flag = -1;
	} else { return(-1); }
	st++;
    }
    if (flag == 0) {
	flag = 1;
    }
    *dp = total * flag;
    return(1);
}

/*
 * set parameter
 */
static void
uumrc_set_entry(data, lu, rk_pre_load)
char *data;
LangUumrcRec *lu;
int rk_pre_load;
{
    int num,d1;
    char code[256];
    char s[7][EXPAND_PATH_LENGTH];
    char tmp[1024];
    FILE *fp;
    char *s0or1;

    /*
     * get one list
     */
    num = sscanf(data,"%s %s %s %s %s %s %s %s",code,
    		 s[0],s[1],s[2],s[3],s[4],s[5],s[6]) ;
    if (num <= 0) { return; }

    if(code[0] == ';') {
	/*
	 * this means a comment list
	 */
	return;
    } else if (strcmp(code, "include") == 0) {
	if(expand_expr(s[0], lu->lang_name) != 0) {
	    err_expand(open_uumrc_filenm, s[0]);
	}
	if((fp = fopen(s[0], "r")) != NULL) {
	    while(fgets(tmp,1024,fp  ) != NULL) {
		uumrc_set_entry(tmp, lu, rk_pre_load);
	    }   
	    fclose(fp);
	}else{
	    err_expand(open_uumrc_filenm, s[0]);
	}
    } else if(strcmp(code , "setrkfile") == 0) {
	if((lu->rk_table == NULL) && (num > 1)) {
	    if(expand_expr(s[0], lu->lang_name) != 0) {
		err_expand(open_uumrc_filenm, s[0]);
	    }
	    lu->rkfile_name = alloc_and_copy(s[0]);
	}
    } else if (rk_pre_load == 0) {
	if(strcmp(code , "setuumkey") == 0) {
	    if(!uumkey_defined_by_option && (num > 1)) {
		if(expand_expr(s[0], lu->lang_name) != 0) {
		    err_expand(open_uumrc_filenm, s[0]);
		}
		uumkeyname = alloc_and_copy(s[0]);
	    }
	} else if(strcmp(code , "setconvenv") == 0) {
	    if(num > 1) {
		get_new_env(c_c, 0);
		if(num > 2) {
		    if(!(strcmp(s[num - 2], "sticky"))) {
			normal_sticky = 1;
			num--;
		    }
		}
		if (num == 2) s0or1 = s[0];
		else {
		    s0or1 = s[1];
		    if (def_servername && *def_servername) {
			servername = alloc_and_copy(def_servername);
		    } else {
			servername = alloc_and_copy(s[0]);
		    }
		}
		if(expand_expr(s0or1, lu->lang_name) != 0) {
		    err_expand(open_uumrc_filenm, s0or1);
		}
		envrcname = alloc_and_copy(s0or1);
	    }
	} else if(strcmp(code , "setkankanaenv") == 0) {
	    if(num > 1) {
		get_new_env(c_c, 1);
		if(num > 2) {
		    if(!(strcmp(s[num - 2], "sticky"))) {
			reverse_sticky = 1;
			num--;
		    }
		}
		if (num == 2) s0or1 = s[0];
		else {
		    s0or1 = s[1];
		    if (def_reverse_servername && *def_reverse_servername) {
			reverse_servername =
			  alloc_and_copy(def_reverse_servername);
		    } else {
			reverse_servername = alloc_and_copy(s[0]);
		    }
		}
		if(expand_expr(s0or1, lu->lang_name) != 0) {
		    err_expand(open_uumrc_filenm, s0or1);
		}
		reverse_envrcname = alloc_and_copy(s0or1);
	    }
	} else if(strcmp(code , "setenv") == 0) {
	    if(num > 2) {
		get_new_env(c_c, 0);
		if(!(strcmp(s[num - 3], "sticky"))) {
		    normal_sticky = 1;
		    num--;
		}
		if (num == 3) s0or1 = s[1];
		else {
		    s0or1 = s[2];
		    if (def_servername && *def_servername) {
			servername = alloc_and_copy(def_servername);
		    } else {
			servername = alloc_and_copy(s[1]);
		    }
		}
		if(expand_expr(s0or1, lu->lang_name) != 0) {
		    err_expand(open_uumrc_filenm, s0or1);
		}
		envrcname = alloc_and_copy(s0or1);
		strcpy(env_name, s[0]);
	    }
	} else if(strcmp(code , "setenv_R") == 0) {
	    if(num > 2) {
		get_new_env(c_c, 1);
		if(!(strcmp(s[num - 3], "sticky"))) {
		    reverse_sticky = 1;
		    num--;
		}
		if (num == 3) s0or1 = s[1];
		else {
		    s0or1 = s[2];
		    if (def_reverse_servername && *def_reverse_servername) {
			reverse_servername =
			  alloc_and_copy(def_reverse_servername);
		    } else {
			reverse_servername = alloc_and_copy(s[1]);
		    }
		}
		if(expand_expr(s0or1, lu->lang_name) != 0) {
		    err_expand(open_uumrc_filenm, s0or1);
		}
		reverse_envrcname = alloc_and_copy(s0or1);
		strcpy(reverse_env_name, s[0]);
	    }
	} else if(strcmp(code , "waking_up_in_henkan_mode") == 0) {
	    if(!(defined_by_option & OPT_WAKING_UP_MODE)) {
		henkan_off_def = 0;
	    }
	} else if(strcmp(code , "waking_up_no_henkan_mode") == 0) {
	    if(!(defined_by_option & OPT_WAKING_UP_MODE)) {
		henkan_off_def = 1;
	    }
	} else if(strcmp(code,"simple_delete")==0) {
	    excellent_delete = 0;
	} else if(strcmp(code,"excellent_delete")==0) {
	    excellent_delete = 1;
	} else if(strcmp(code , "send_ascii_char") == 0) {
	    send_ascii_char = send_ascii_char_def = 1;
	} else if(strcmp(code , "not_send_ascii_char") == 0) {
	    send_ascii_char = send_ascii_char_def = 0;
	} else if (strcmp(code, "setmaxchg") == 0) {
	    if (change_ascii_to_int(s[0],&d1) != -1) {
		maxchg = (d1 <= 0) ? maxchg:d1;
	    }
	} else if (strcmp(code, "setmaxbunsetsu") == 0) {
	    if (num >= 2) { 
		if (change_ascii_to_int(s[0],&d1) != -1) {
		    maxbunsetsu = (d1 <= 0) ? maxbunsetsu:d1;
		}
	    }
	} else if (strcmp(code, "setmaxhistory") == 0) {
	    if (num >= 2) { 
		if (change_ascii_to_int(s[0],&d1) != -1) {
		max_history = (d1 <= 0) ? max_history:d1+1;	
		}
	    }
	} else if (strcmp(code, "setjishopath") == 0) {
	} else if (strcmp(code, "sethindopath") == 0) {
	} else if (strcmp(code, "setfuzokugopath") == 0) {
	} else if (strcmp(code, "touroku_comment") == 0) {
	    touroku_comment = 1;
	} else if (strcmp(code, "touroku_no_comment") == 0) {
	    touroku_comment = 0;
	} else if (strcmp(code, "henkan_on_kuten") == 0) {
	    henkan_on_kuten = 1;
	} else if (strcmp(code, "henkan_off_kuten") == 0) {
	    henkan_on_kuten = 0;
	/*
	 * unnessesary in xim
	 */
	} else if(strcmp(code , "setkanaromenv") == 0) {
	} else if(strcmp(code , "flow_control_on") == 0) {
	} else if(strcmp(code , "flow_control_off") == 0) {
	} else if(strcmp(code , "convkey_always_on") == 0) {
	} else if(strcmp(code , "convkey_not_always_on") == 0) {
	} else if (strcmp(code, "setmaxichirankosu") == 0) {
	} else if(strcmp(code, "remove_cs") == 0) {
	} else if(strcmp(code, "not_remove_cs") == 0) {
	} else {
	}
    }
}

static int
lookLangname(s,l)
char *s,*l;
{
    return(*l == '\0' ? 0 : strmatch(s, l));
}

/*
 * read uumrc file 
 */ 
int
uumrc_get_entries(lang, rk_pre_load)
char *lang;
int rk_pre_load;
{
    FILE *fp = NULL;
    char data[1024];
    char *n;
    LangUumrcRec *lu, *prev;
    int err = 0;

    if (rk_pre_load == 0) {
	/*
	 * default setting
	 */
	maxchg = MAXCHG;
	maxbunsetsu = MAXBUNSETSU;
	max_history = MAX_HISTORY + 1;
    }


    for (lu = languumrc_list, prev = NULL; lu != NULL;
	 prev = lu, lu = lu->next) {
	  if (*(n = lu->lang_name) != '\0' && lookLangname(n, lang)) {
	    if (lu->uumrc_name && *lu->uumrc_name) {
		strcpy(open_uumrc_filenm, lu->uumrc_name);
		if((fp = fopen(open_uumrc_filenm, "r")) == NULL) {
		    err = 1;
		}
	    } else {
		err = 1;
	    }
	    break;
	}
    }
    if (fp == NULL) {
	if(strcpy(open_uumrc_filenm, LIBDIR),
	   strcat(open_uumrc_filenm, "/"),
	   strcat(open_uumrc_filenm, lang),
	   strcat(open_uumrc_filenm, RCFILE),
	   (fp = fopen(open_uumrc_filenm, "r")) != NULL) {
	    if (err) {
		lu->uumrc_name = alloc_and_copy(open_uumrc_filenm);
	    } else {
		lu = (LangUumrcRec *)Malloc(sizeof(LangUumrcRec));
		if (languumrc_list == NULL) {
		    languumrc_list = lu;
		} else {
		    prev->next = lu;
		}
	    }
	    lu->rkfile_name = NULL;
	    lu->rk_table = NULL;
	    lu->lang_name = alloc_and_copy(lang);
	    lu->uumrc_name = alloc_and_copy(open_uumrc_filenm);
	    lu->server_name = NULL;
#ifdef  USING_XJUTIL
	    lu->xjutil_act = 0;
	    lu->xjutil_id = 0;
	    lu->xjutil_pid = 0;
#endif  /* USING_XJUTIL */
	}
    }
    if (fp == NULL) {
	print_out2("%s %s",msg_get(cd, 28, default_message[28], lang),
		  open_uumrc_filenm);
	Free((char *)lu);
	return -1;
    }

    while(fgets(data, 1024, fp) != NULL) {
	uumrc_set_entry(data, lu, rk_pre_load);
    }
    fclose(fp);
    if (lu->rk_table == NULL) {
	if ((lu->rk_table = romkan_table_init(NULL, lu->rkfile_name, NULL, 0))
	    == NULL){
	    /*
	    Free((char *)lu);
	    */
	    return(-1);
	}
    }
    return(0);
}

int
read_default_rk()
{
    register ReadRkfileRec *rr;

    for (rr = read_rkfile_list; rr != NULL; rr = rr->next) {
	if (uumrc_get_entries(rr->name, 1) == -1) {
	    return(-1);
	}
    }
    return(0);
}
