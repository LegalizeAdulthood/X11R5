/*
 * $Id: jbiki_b.c,v 1.2 1991/09/16 21:31:41 ohm Exp $
 */
/** 		Copyright OMRON Corporation. 1989, 1990, 1991
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose is hereby granted without fee, provided 
 * that all of the following conditions are satisfied:
 *
 * <1>	The above copyright notices appear in all copies
 * <2>	Both the copyright notices and this permission notice appear in 
 *	supporting documentation
 * <3>	The name of "cWnn" isn't changed unless substantial modifications 
 * 	are made
 * <4>	Following words followed by the above copyright notices appear
 *    	in all supporting documentation of software based on "cWnn":
 *
 * 	This software is based on the original version of cWnn developed 
 *	by OMRON Corporation and Wnn developed by Kyoto University Research 
 *	Institute for Mathematical Sciences (KURIMS), OMRON Corporation and 
 * 	ASTEC Inc."
 *
 * <5>	The names of OMRON may not be used in advertising or publicity 
 *	pertaining to distribution of the software without specific, written 
 *	prior permission
 *
 *  OMRON Corporation makes no representations about the suitability of this 
 *  software for any purpose.  It is provided "as is" without express or 
 *  implied warranty.
 *
 *  Wnn consortium is one of distributors of the official Wnn source code
 *  release.  Wnn consortium also makes no representations about the
 *  suitability of this software for any purpose.  It is provided "as is"
 *  without express or implied warranty.
 *
 *  OMRON AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 *  SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 *  IN NO EVENT SHALL OMRON OR WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL,
 *  INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 *  LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 *  OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *  PERFORMANCE OF THIS SOFTWARE.
 *
 *  This sofeware is developed by OMRON Corporation, based on the original 
 *  version of Wnn which was developed by Kyoto University Research Institute 
 *  for Mathematical Sciences (KURIMS), OMRON Corporation and ASTEC Inc. 
 *
 *  	Copyright Kyoto University Research Institute for Mathematical 
 *		Sciences 1987,1988,1989,1990,1991
 * 	Copyright OMRON Corporation 1987,1988,1989,1990,1991
 *	Copyright ASTEC Inc. 1987, 1988,1989,1990,1991
 *
 *  Contact:	Tel:   	+81-75-951-5111
 *		Fax:   	+81-75-955-2442
 *		E-mail: zhong@ari.ncl.omron.co.jp
**/
/**  cWnn  Version 1.1	 **/

/********************************************
	Modified by Zhong in Sept.1 1990
*********************************************
*/
#include <stdio.h>
#include "commonhd.h"

#include "de_header.h"
#include "jdata.h"
#include "kaiseki.h"

#ifdef CONVERT_by_STROKE

#ifndef min
#define min(a, b) ( ((a) > (b))? (b):(a) )
#define max(a, b) ( ((a) < (b))? (b):(a) )
#endif

#define MAX_B_KOHO	64
#define MIN_B_KOHO	5

static void rd_biki_b(), set_kohos(), koho_insert(), b_koho_put();
static int set_value();

static int      number;/** クライアントの辞書番号 */
static int      prior;
static unsigned short   *hinsistart;
static UCHAR    *hindoin;
static UCHAR    *hindostart;
static UCHAR    *hindo2start;

struct  b_koho  *p_kohos[MAX_B_KOHO];	/* BiXing koho Buffer' Index	*/
struct  b_koho  b_kohos[MAX_B_KOHO];	/* BiXing koho Buffer	*/
static 	struct  b_koho	*cur_koho;	/* */
static 	struct  b_koho	*last_koho;	/* */
int	cnt_k;				/* Current koho */
int	max_b_koho;			/* MAX num of b_koho */

int is_bwnn_rev_dict()
{
    int k;
    struct JT  *jt;

    for(k = 0; k < c_env->jishomax; k++){
	number = c_env->jisho[k];
	if(dic_table[c_env->jisho[k]].enablef){
	    jt = (struct JT *)(files[dic_table[c_env->jisho[k]].body].area);
	    if (jt->syurui == BWNN_REV_DICT) {
		return (1);
	    } else {
		return(0);
	    }
	}
    }
    return(0);
}

int jishobiki_b(b_bgn,b_end)		/* return number of b_koho */
int	b_bgn;
int	b_end;
{
    register struct JT 	*jt;
    int			b_ptr;		/* point to b_node */
    register int 	k;
   
    max_b_koho = min(MAX_B_KOHO, c_env->fuzokogo_val);
    max_b_koho = max(MIN_B_KOHO, max_b_koho);
    cnt_k = 0;
    cur_koho 	= b_kohos;		/* set init koho address */
    last_koho 	= NULL;

    for(k = 0; k < c_env->jishomax; k++){
	number = c_env->jisho[k];
	if(dic_table[c_env->jisho[k]].enablef){
	    jt = (struct JT *)(files[dic_table[c_env->jisho[k]].body].area);
	    prior = dic_table[c_env->jisho[k]].nice;

	    hindo2start = (dic_table[c_env->jisho[k]].hindo != -1)?
		jt->hindo:NULL;
	    hindostart = (dic_table[c_env->jisho[k]].hindo != -1)?
		((struct HJT *)(files[dic_table[c_env->jisho[k]].hindo].area))->hindo:jt->hindo;

	    hinsistart = jt->hinsi;
	    if(jt->syurui == BWNN_REV_DICT ){
		b_ptr = jt->bind[0].pter_son;
		rd_biki_b(jt,b_bgn, b_end-1, b_ptr);
	    }
	}
    }
    if (last_koho==NULL)
	return(-1);
    cnt_k = 0;
    b_koho_put(last_koho);	/*  put b_kohos in sequence  */
    return(cnt_k);
}
/****************************************
	"rd_biki_b()" is a recursive function, used for recursively search the 
	"b_index".  When it is first called by "jishobiki_b()", the second 
	param  "cur" is equel to one. " cur will become 2,3 .., when it is 
	called by itself for further deep levels.
					**************************************/
static void
rd_biki_b(jt, cur, bend, b_ptr)
struct	JT	*jt;			/* dic head	*/
int	cur;				/* Input:  input lenths  */
int	bend;
int 	b_ptr;			/* Input:  point to current b_node */
{
	w_char	*yo_kanji;	/*pointing to yomi or kanji of a tuple*/
	struct	b_node	*bind;
	int 	tmp_b_ptr;

	bind = jt->bind;

	if ( bun[cur]==Q_MARK) { 		/* Ignored w_char */
	    while( b_ptr != -1) {
		if ( cur==bend )
			set_kohos(jt, bind[b_ptr].pter);
		else 	rd_biki_b(jt,cur+1,bend,bind[b_ptr].pter_son);
		b_ptr = bind[b_ptr].pter_next;
	    }
	    return;
	}
	while( b_ptr != -1 ) { 
		tmp_b_ptr = b_ptr;
		while ( bind[tmp_b_ptr].pter == -1 ) 
			tmp_b_ptr = bind[tmp_b_ptr].pter_son;
		yo_kanji = KANJI_str(
		  jt->ri2[bind[tmp_b_ptr].pter].kanjipter+jt->kanji, 0);
		if ( bun[cur] > yo_kanji[cur-1] )   
	     		b_ptr = bind[b_ptr].pter_next;
		else  break;
	}

	if ( b_ptr == -1 )	
		return;				/* not searched */
	tmp_b_ptr = b_ptr;
	while ( bind[tmp_b_ptr].pter == -1 ) 
		tmp_b_ptr = bind[tmp_b_ptr].pter_son;
	yo_kanji = KANJI_str(jt->ri2[bind[tmp_b_ptr].pter].kanjipter+jt->kanji, 0);
	if ( bun[cur] < yo_kanji[cur-1] ) 
		return;				/* not searched */

	if ( bun[cur] == yo_kanji[cur-1] ){	/* searched */

		if ( cur==bend ) 		/* Set kohos */
			set_kohos(jt, bind[b_ptr].pter);
		else if ( bind[b_ptr].pter_son != -1 )
		   	rd_biki_b(jt,cur+1,bend,bind[b_ptr].pter_son);
	}
}

static void
set_kohos(jt, cur_off2)
struct 	JT  *jt;
int	cur_off2;
{
   for ( ; cur_off2 != -1; cur_off2 =jt->ri2[cur_off2].next[0] ) {


	if (  hindo2start && (*(hindo2start+ cur_off2)&0x7f) == 0x7f  || 
	     (*(hindostart + cur_off2)&0x7f) == 0x7f     ) 
		continue;			 /* Check masked word */

	if( cnt_k < max_b_koho)  	/* is not a full table */
		cnt_k++;
	else {
		cur_koho = last_koho;
		last_koho= last_koho->previou;
	}
	cur_koho->p_yomi  = KANJI_str
		(((jt->ri2)+cur_off2)->kanjipter+jt->kanji, 0);
	cur_koho->p_kanji = KANJI_str
		(((jt->ri2)+cur_off2)->kanjipter+jt->kanji, 1);
   	cur_koho->p_hinsi = hinsistart + cur_off2;
						/* cur_off2 is serial */
	cur_koho->p_hindo = hindostart + cur_off2;
	if(hindo2start)  
		hindoin = hindo2start + cur_off2;
	else  	hindoin = NULL;
	cur_koho->dic_no  = number;
	cur_koho->serial  = cur_off2;
	koho_insert(cur_koho); 
	cur_koho++; 
    }
} 

static void
koho_insert(c_koho)
register struct	b_koho	*c_koho;
{	
	register struct	b_koho	*t_koho1;
	register struct	b_koho	*t_koho2;

	if ( last_koho == NULL)  {		 /* for first koho  */
		c_koho->value = set_value(c_koho);
		last_koho = c_koho;
		c_koho->previou = NULL;
		return;
	}
	t_koho1 = last_koho;
	t_koho2 = last_koho;
	c_koho->value = set_value(c_koho);

	while( (t_koho1!=NULL) && ((t_koho1->value)<=(c_koho->value))){
		t_koho2 = t_koho1;
		t_koho1 = t_koho1->previou;
	}
	if (t_koho1==t_koho2)		/* c_koho is the last koho */
		last_koho = c_koho;
	else 	t_koho2->previou = c_koho;
	c_koho->previou = t_koho1;
}

static int set_value(koho)
struct b_koho	*koho;
{
   if (hindoin != NULL )
    	return( hyoka1(*(koho->p_hindo), *hindoin, 0, 0, prior) );
   else 
    	return( hyoka1(*(koho->p_hindo), 0, 0, 0, prior) );
}

static void
b_koho_put(last)	/*  put b_kohos in sequence  */
struct	b_koho  *last;
{
	if (last->previou!=NULL)
		b_koho_put(last->previou);
	p_kohos[cnt_k++] = last;
}
#endif /* CONVERT_by_STROKE */
