/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */

/* @(#) 102.1 $Header: /work/nk.R3.1/lib/RK/RCS/RKdic.h,v 1.8 91/05/21 18:38:33 mako Exp $ */

#ifndef __RKdic_h
#define __RKdic_h

#define BUFLEN		1024
#define SUCCESS		0
#define NOTSUC		-1
#define ERR_VALUE	1

#define	NOENT	-2	/* No such file or directory		*/
#define	IO	-5	/* I/O error				*/
#define	NOTALC	-6	/* Cann't alloc. 	       		*/
#define	BADF	-9	/* 指定された辞書が間違っている	       	*/
#define	BADDR	-10	/* dics.dirに異常がある 	       	*/
#define	ACCES	-13	/* Permission denied 			*/
#define	NOMOUNT	-15	/* マウント失敗                 	*/
#define	MOUNT	-16	/* ファイルがある(マウント中)      	*/
#define	EXIST	-17	/* ファイルがある      			*/
#define	INVAL	-22	/* 辞書の種類が同じ			*/
#define	TXTBSY	-26	/* マウント中か使用中			*/
#define BADARG  -99	/* Bad Argment    			*/
#define BADCONT -100	/* Bad Context  			*/

#define KYOUSEI	0x01	/* kyouseiteki ni henkou wo okonau mode */

#define F_OK	0	/* does file exist */
#define X_OK	1	/* is it executable by caller */
#define W_OK	2	/* is it writable by caller */
#define R_OK	4	/* is it readable by caller */

#define RECSZ	256

#define	STRCMP(d, s)	strcmp((char *)(d), (char *)(s))

#define ND2RK(s)	((0x80 >> (int)(s)) & 255)
#define	Rk_MODE(i)	(((i) & (Rk_MWD | Rk_SWD | Rk_PRE | Rk_SUC)) & 255)

extern int errno;
extern char *DF2File();
struct DM *SearchUDDP();
struct DM *searchDDP();

unsigned char *TP();

struct RkContext	*RkGetContext();
struct DM *allocDM();
struct DF *allocDF();

int RkGetWordTextDic();
int RkCreateDic();

unsigned char *RkUparseWrec();
unsigned char *GetDicFilename();
unsigned char *CheckDicFile();

int MakeDicFile();
int RemoveDicFile();
int RenameDicFile();

void	Debug();

/* Extension Data in DM */
struct TD {
    struct TN   *td_node;	/* array of node TN */
    unsigned	td_n;		/* # effective nodes */
    unsigned	td_max;		/* # maximum nodes in the array */
};

struct TN {
    unsigned short	tn_flags;	/* type of node (see blow) */
    unsigned short	tn_key;
    union {
	struct TD	*tree;
	unsigned char	*word;
    }	tn_value;				/* type specific data */
/* syntax error */
#define tn_tree		tn_value.tree
#define tn_word		tn_value.word
};

/* flags  values */
#define	TN_WORD		1		/* is a word */
#define	TN_WDEF		2		/* wrec has been defined */
#define	TN_WDEL		4		/* wrec has been deleted */

#define	IsWordNode(tn)	((tn)->tn_flags&TN_WORD)

#endif	/* __RKdic_h */
