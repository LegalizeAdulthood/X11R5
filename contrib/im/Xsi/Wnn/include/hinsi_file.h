/*
 * $Id: hinsi_file.h,v 1.2 1991/09/16 21:29:59 ohm Exp $
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
/*    品詞ファイルに関する、定義
*/
     

#define MAXHINSI 	256
#define MAXFUKUGOU 	256
#define MAXNODE 	256
#define FUKUGOU_START 	0xffff - RESERVE_FUKUGOU_USIRO
#define HEAP_LEN	(10 * (MAXHINSI + MAXFUKUGOU))
/* 10 bytes for each hinsi */
#define WHEAP_LEN	(10 * MAXFUKUGOU * sizeof(short))
/* 10 bytes for each fukugou hinsi */


#define RESERVE_FUKUGOU_USIRO 512   /* fukugou hinsi no usiro ni 
				       koredake reserve wo site oku */
/* hitoru ha delete sareta to iu jouhou no 
   ta me*/
#define TERMINATE 0xffff
#define ERR -2

#define WNN_NODE_SUFFIX '/'
#define NODE_CHAR '|'
#define COMMENT_CHAR ';'
#define DEVIDE_CHAR '$'
#define HINSI_SEPARATE_CHAR ':'
#define IGNORE_CHAR1 ' '
#define IGNORE_CHAR2 '\t'
#define YOYAKU_CHAR '@'
#define CONTINUE_CHAR '\\'

struct wnn_fukugou{
    w_char *name;	/* 複合品詞の名前 */
/* fukugou hinsi is held as a string */
    unsigned short *component;	/* 複合品詞の構成要素となる品詞の番号の配列
				   (最後は 0xffff で終る)
				 */
};

struct wnn_hinsi_node{
    w_char *name;	/* 品詞ノードの名前 */
    int kosuu;			/* 子どもの数 */
    w_char *son;	/* 最初の子どもへのポインタ */
};
