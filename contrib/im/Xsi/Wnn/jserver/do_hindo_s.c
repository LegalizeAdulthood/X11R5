/*
 * $Id: do_hindo_s.c,v 1.2 1991/09/16 21:31:30 ohm Exp $
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
#include "commonhd.h"
#include "jslib.h"
#include "kaiseki.h"
#include "ddefine.h"
#include "jdata.h"
#include "de_header.h"
    
int
js_hindo_set()
{
    int env_id,dic,entry,ima,hindo,x;
    int err = 0;
    if((env_id=envhandle()) == -1) err = -1;
    dic=get4_cur();
    entry=get4_cur();
    ima=get4_cur();
    hindo=get4_cur();
    if(err == -1){ error_ret(); return;}
    if(dic == -1){
	giji_hindoup(entry);
	put4_cur(0);
	putc_purge();
	return(-1);
    }
    if(find_dic_in_env(env_id,dic) == -1){ /* no such dic */
	wnn_errorno=WNN_DICT_NOT_USED; error_ret();
	return(-1);
    }
    if(dic_table[dic].hindo_rw){
	wnn_errorno = WNN_RDONLY_HINDO;error_ret();
	return(-1);
    }
    x=hindo_set(dic,entry,ima,hindo);
    if(x== -1){ error_ret();}
    else{ put4_cur(x); }
    putc_purge();
    return(0);
}

int
hindo_set(dic_no,entry,imaop,hinop)
int dic_no;
int entry;
int imaop;
int hinop;
{
#ifdef SRAND48
    double drand48();
#define RAND()  drand48()
#else
    double drand();
#define RAND()  ((double)rand() / (double)((1<<31) - 1))
#endif
    UCHAR *hst;
    int ima, hindo;

    if(dic_table[dic_no].hindo == -1){
	hst = ((struct JT *)(files[dic_table[dic_no].body].area))->hindo;
	((struct JT *)(files[dic_table[dic_no].body].area))->hdirty = 1;
	((struct JT *)(files[dic_table[dic_no].body].area))->dirty = 1;
    }else{
	hst = ((struct HJT *)(files[dic_table[dic_no].hindo].area))->hindo;
	((struct HJT *)(files[dic_table[dic_no].hindo].area))->hdirty = 1;
    }
    ima = hst[entry] & 0x80;
    hindo = hst[entry] & 0x7f;
    switch(imaop){
    case WNN_HINDO_NOP:
	break;
    case WNN_IMA_ON:
	ima = 0x80;
	break;
    case WNN_IMA_OFF:
	ima = 0x0;
	break;
    }
    switch(hinop){
    case WNN_HINDO_NOP:
	break;
    case WNN_HINDO_INC:
      if((hindo&0x7e) != 0x7e && ( RAND() < (double)1 / ((hindo >> 2) + 1)))
	  hindo++;
	break;
    case WNN_HINDO_DECL:
      if(hindo != 0 && ( RAND() < (double)1 / ((hindo >> 2) + 1)))
	  hindo--;
	hindo = 0x0;
	break;
    case WNN_ENTRY_NO_USE:
	hindo = 0x7f;
	break;
    default:
	hindo = asshuku(hinop);
    }
    hst[entry] = hindo | ima;
    return(0);
}

    
