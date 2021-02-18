/*
 * $Id: do_dic_env.c,v 1.3 1991/09/16 21:31:22 ohm Exp $
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
 *	Do Dic operation for Env
 */

#include <stdio.h>

#include "commonhd.h"
#include "demcom.h"
#include "de_header.h"
#include "jdata.h"


static void del_dic_from_env(), dic_delete();

/*
	Dic. OPE for Env
 */

/*	js_dic_add	*/
void
js_dic_add()
{
    int eid,fid,hfid,nice,rw,hrw,dic_no,d, rev;
    char pw[WNN_PASSWD_LEN], pw1[WNN_PASSWD_LEN];
    eid=get4_cur();
    fid=get4_cur();
    hfid=get4_cur();
    nice=get4_cur();
    rw=get4_cur();
    hrw=get4_cur();
    gets_cur(pw);
    gets_cur(pw1);
    rev=get4_cur();		/* rev is to add it as reverse dict */
    
    error1("dic_add: eid=%d,fid=%d,hfid=%d",eid,fid,hfid);
    c_env= env[eid];
    if(find_fid_in_env(eid,fid)==-1){
	wnn_errorno = WNN_FID_ERROR; error_ret(); putc_purge();
	return;
    }
    if(files[fid].file_type != WNN_FT_DICT_FILE){
	wnn_errorno = WNN_NOT_A_DICT; error_ret(); putc_purge();
	return;
    }
    if(rev &&
#ifdef	CONVERT_by_STROKE
       ((struct JT *)(files[fid].area))->syurui != BWNN_REV_DICT &&
#endif	/* CONVERT_by_STROKE */
#ifdef	CONVERT_with_SiSheng
       ((struct JT *)(files[fid].area))->syurui != CWNN_REV_DICT &&
#endif	/* CONVERT_with_SiSheng */
       ((struct JT *)(files[fid].area))->syurui != WNN_REV_DICT){
	wnn_errorno = WNN_NOT_A_REV;  error_ret(); putc_purge();
	return;
    }

    if(rw == WNN_DIC_RW && 
       ((struct JT *)(files[fid].area))->syurui != WNN_UD_DICT &&
#ifdef	CONVERT_by_STROKE
       ((struct JT *)(files[fid].area))->syurui != BWNN_REV_DICT &&
#endif	/* CONVERT_by_STROKE */
#ifdef	CONVERT_with_SiSheng
       ((struct JT *)(files[fid].area))->syurui != CWNN_REV_DICT &&
#endif	/* CONVERT_with_SiSheng */
       ((struct JT *)(files[fid].area))->syurui != WNN_REV_DICT){
	wnn_errorno = WNN_NOT_A_UD;  error_ret(); putc_purge();
	return;
    }
    if(hfid != -1){ /* hindo file selected */
	if(find_fid_in_env(eid,hfid)==-1){
	    wnn_errorno = WNN_FID_ERROR; error_ret(); putc_purge();
	    return;
	}
	if(files[hfid].file_type != WNN_FT_HINDO_FILE){
	    wnn_errorno = WNN_NOT_A_HINDO_FILE; error_ret(); putc_purge();
	    return;
	}	    
	if(!match_dic_and_hindo_p(&files[fid], &files[hfid])){
	    wnn_errorno = WNN_HINDO_NO_MATCH; error_ret(); putc_purge();
	    return;
	}
	if(hrw == WNN_DIC_RW && !check_pwd(pw1, files[hfid].passwd)){
	    wnn_errorno = WNN_INCORRECT_PASSWD; error_ret(); putc_purge();
	    return;
	}
    }
    if((d=chk_dic(eid,fid)) != -1){	/* already added that file */
	dic_table[d].enablef = 1;
	dic_table[d].rw = rw;
	
	if(hfid !=-1){ /* change hindo file */
	    dic_table[d].hindo=hfid;
	    hindo_file_size_justify(&files[fid], &files[hfid]);
	}else{
	    dic_table[d].hindo = -1;
	}
	dic_table[d].hindo_rw = hrw;
	dic_table[d].nice = nice;
	dic_table[d].rev = rev;
	put4_cur(d); putc_purge();
	return;
    }
    if(rw == WNN_DIC_RW && !check_pwd(pw, files[fid].passwd)){
	wnn_errorno = WNN_INCORRECT_PASSWD; error_ret(); putc_purge();
	return;
    }
    if(hfid == -1 && hrw == WNN_DIC_RW &&
       !check_pwd(pw1, ((struct JT *)(files[fid].area))->hpasswd)){
	wnn_errorno = WNN_INCORRECT_PASSWD; error_ret(); putc_purge();
	return;
    }
    
    /* add new dic */
    if((dic_no=get_new_dic()) == -1){ /* no more dic */ 
	wnn_errorno=WNN_JISHOTABLE_FULL; error_ret(); putc_purge();
	return;
    }
    if(add_dic_to_env(eid,dic_no)){ /* no more jisho */ 
	wnn_errorno=WNN_JISHOTABLE_FULL; error_ret(); putc_purge();
	return;
    }
    if(hfid != -1){
	hindo_file_size_justify(&files[fid], &files[hfid]);
    }
    dic_table[dic_no].body=fid;	dic_table[dic_no].hindo=hfid;
    dic_table[dic_no].rw = rw;	dic_table[dic_no].hindo_rw = hrw;
    dic_table[dic_no].enablef = 1;
    dic_table[dic_no].nice = nice;
    dic_table[dic_no].rev = rev;
    put4_cur(dic_no);
    putc_purge();
}

int
add_dic_to_env(eid,dic_no)
int eid, dic_no;
{
    int off_env;
    struct cnv_env *cenv;
    cenv= env[eid];
    off_env = cenv->jishomax;
    if(off_env >= WNN_MAX_JISHO_OF_AN_ENV){ /* no more jisho */ 
	wnn_errorno=WNN_JISHOTABLE_FULL;
	return -1;
    }
    (cenv->jisho)[off_env] = dic_no;
    cenv->jishomax = off_env+1;
    return 0;
}

int
chk_dic(eid,fid)
int eid, fid;
{
    int i,dic,max;
    max = env[eid]->jishomax;
    for(i=0;i<max;i++){
	dic=(env[eid]->jisho)[i];
	if(dic_table[dic].body==fid) return dic;
    }
    return -1;
}

int
chk_hindo(eid,fid)
int eid, fid;
{
    int i,dic,max;
    max = env[eid]->jishomax;
    for(i=0;i<max;i++){
	dic=(env[eid]->jisho)[i];
	if(dic_table[dic].hindo==fid) return dic;
    }
    return -1;
}

int
get_new_dic()
{
    int i;
    for(i=0;i<MAX_DIC;i++){
	if(dic_table[i].body==-1) return i;
    }
    return -1;
}

/*	search jisho of an env (dic_no == -1 then new dic) */
int
find_dic_in_env(eid,dic_no)
int eid, dic_no;
{
    int i,max;
    max = env[eid]->jishomax;
    for(i=0;i<max;i++){
	if((env[eid]->jisho)[i] == dic_no) return i;
    }
    return -1;
}

/*	js_dic_delete	*/
void
js_dic_delete()
{
    int eid,off_env,dic_no;
    eid=get4_cur();
    c_env= env[eid];
    dic_no=get4_cur();
    if((off_env=find_dic_in_env(eid,dic_no)) == -1){ /* no such dic */
	wnn_errorno=WNN_DICT_NOT_USED; error_ret(); putc_purge();
	return;
    }
    dic_table[dic_no].body= -1; dic_table[dic_no].hindo= -1;

    del_dic_from_env(eid,off_env);
    put4_cur(0);
    putc_purge();
}


static void
del_dic_from_env(eid,off_env)
int eid, off_env;
{
    int *jisho,max,i;
    struct cnv_env *cenv;

    cenv= env[eid];
    jisho= cenv->jisho; max= --(cenv->jishomax);
    for(i=off_env;i< max; i++){
	jisho[i]=jisho[i+1];
    }
}

/*	delete all Dic. in an Env , when disconnect	*/
void
del_all_dic_in_env(eid)
int eid;
{
    int *jisho,max,i;

    jisho= env[eid]->jisho; max= env[eid]->jishomax;
    for(i=0;i< max; i++){
	dic_table[jisho[i]].body= -1; dic_table[jisho[i]].hindo= -1;
    }
}


/*	js_dic_list_all	*/
void
js_dic_list_all()
{
    int c,i,dic_no;

    for(c=i=0;i<MAX_DIC;i++){
	if(dic_table[i].body==-1) continue;
	c++;
    }
    put4_cur(c);

    for(dic_no=0;dic_no<MAX_DIC;dic_no++){
	if(dic_table[dic_no].body==-1) continue;
	put_dic_info(dic_no);
    }
    putc_purge();
}

/*	js_dic_list	*/
void
js_dic_list()
{
    int eid,i,dic_no,max;
    eid=get4_cur(); c_env= env[eid];
    max=env[eid]->jishomax;
    put4_cur(max);
    for(i=0;i<max;i++){
	dic_no=(env[eid]->jisho)[i];
	put_dic_info(dic_no);
    }
    putc_purge();
}

/*	js_dic_use	*/
void
js_dic_use()
{
    int eid,dic_no,enable;
    eid=get4_cur();		c_env= env[eid];
    dic_no=get4_cur();
    enable=get4_cur();
    if(find_dic_in_env(eid,dic_no) == -1){ /* no such dic */
	wnn_errorno=WNN_DICT_NOT_USED; error_ret(); putc_purge();
	return;
    }
    dic_table[dic_no].enablef = enable;
    put4_cur(0);
    putc_purge();
}

/*	js_fuzokugo_set	*/
void
js_fuzokugo_set()
{
    int eid,fid;

    eid=get4_cur();		c_env= env[eid];
    fid=get4_cur();
    if(files[fid].file_type != WNN_FT_FUZOKUGO_FILE){
	wnn_errorno = WNN_NOT_A_FUZOKUGO_FILE; error_ret(); putc_purge();
	return;
    }	    
    if(find_fid_in_env(eid,fid)==-1){ /* H.T. */
	wnn_errorno = WNN_FID_ERROR; error_ret(); putc_purge();
	return;
    }
    env[eid]-> fzk_fid = fid;
    put4_cur(0);
    putc_purge();
}

/*	js_fuzokugo_get	*/
void
js_fuzokugo_get()
{
    int eid;

    eid=get4_cur();		c_env= env[eid];
    
    if(env[eid]-> fzk_fid == -1){
	wnn_errorno = WNN_FZK_FILE_NO_LOAD; error_ret(); putc_purge();
	return;
    }
    put4_cur(env[eid]-> fzk_fid);
    putc_purge();
}


/* Delete Used Dicts and Fuzokugo's H. T. */
void
used_dic_fuzoku_delete(eid, fid)
int eid;
int fid;
{
    int i, max;
    register int dic_no;
    max = env[eid]->jishomax;
    if(env[eid]->fzk_fid == fid){
	env[eid]->fzk_fid = -1;
	return;
    }
    for(i=0;i<max;i++){
	dic_no = (env[eid]->jisho)[i];
	if(dic_table[dic_no].body == fid || dic_table[dic_no].hindo == fid){
	    dic_delete(eid, dic_no, i);
	    return;
	}
    }
}    


static void
dic_delete(eid, dic_no, off_env)
int eid;
int dic_no;
int off_env;
{
    dic_table[dic_no].body= -1; dic_table[dic_no].hindo= -1;
    del_dic_from_env(eid,off_env);
}
