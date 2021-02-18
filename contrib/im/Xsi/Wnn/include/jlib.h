/*
 * $Id: jlib.h,v 1.2 1991/09/16 21:30:04 ohm Exp $
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
/*	Version 3.0
 */
/*
	Nihongo	Henkan	Library Header File

	08/Jul/1987	(wnn_errorno)
	24/Apr/1987
	09/Mar/1987	(update server_dead)
	25/Feb/1987	(update wchar_t)
	18/Feb/1987	(update Comment)
	28/Nov/1986
	23/Oct/1986
*/
#include <setjmp.h>

#define JLIB

#define	S_BUF_SIZ	1024	/* NEVER change this */
#define	R_BUF_SIZ	1024	/* NEVER change this */



struct	kouho_entry {
	int	s_ichi;		/*	かなバッファへのオフセット	*/
	int	jl;		/*	自立語の長さ			*/
	int	fl;		/*	付属語の長さ			*/
	int	pl;		/*	ディスプレイ時の長さ		*/
	int	jishono;	/*	自立語の辞書番号		*/
	int	serial;		/*	自立語の番号			*/
	wchar_t	*k_data;	/*	漢字バッファへのポインタ	*/
};
#define	KOUHO_ENT	struct	kouho_entry


struct	jikouho_entry {
	int	jl;		/*	自立語の長さ			*/
	int	jishono;	/*	自立語の辞書番号		*/
	int	serial;		/*	自立語の番号			*/
	wchar_t	*k_data;	/*	漢字バッファへのポインタ	*/
};
#define	JIKOUHO_ENT	struct	jikouho_entry


struct	bunjoho {
	wchar_t	*kana_buf;	/*	かなバッファ			*/
	int	kana_size;	/*	かなバッファの長さ		*/
	KOUHO_ENT *klist;	/*	候補リスト・バッファ		*/
	int	klist_size;	/*	候補リスト・バッファの長さ	*/
	wchar_t	*kanji_buf;	/*	漢字バッファ			*/
	int	kanji_buf_size;	/*	漢字バッファ の長さ		*/
};


struct	jikouhojoho{
	JIKOUHO_ENT *jlist;	/*	次候補バッファ			*/
	int	jlist_size;	/*	次候補バッファの長さ		*/
	wchar_t	*kanji_buf;	/*	漢字バッファ			*/
	int	kanji_buf_size;	/*	漢字バッファの長さ		*/
};
#define	JIKOUHOJOHO		struct	jikouhojoho


struct	jishojoho{
	wchar_t	*k_data;	/*	漢字バッファへのポインタ	*/
	int	bumpo;		/*	文法情報			*/
	int	hindo;		/*	頻度				*/
	int	jisho;		/*	辞書番号			*/
	int	serial;		/*	項目番号			*/
};
#define	JISHOJOHO	struct	jishojoho


struct dicinfo{
	int	dic_no;		/*	辞書番号			*/
	int	dic_size;	/*	単語数				*/
	int	ttl_hindo;	/*	頻度総数			*/
	int	dic_type;	/*	1:ユーザ,2:システム		*/
	int	udp;		/*	0:使用中でない,1:使用中		*/
	int	prio;		/*	辞書プライオリティ		*/
	int	rdonly;		/*	0:ライト可能,1:リードオンリー	*/
	char	*file_name;	/*	辞書ファイル名			*/
	char	*h_file_name;	/*	頻度ファイル名			*/
};
#define	DICINFO	struct	dicinfo

#define	wchar	wchar_t

extern	jmp_buf	jd_server_dead_env;	/* サーバが死んだ時に飛んでいくenv */
extern	int	jd_server_dead_env_flg; /* jd_server_dead_envが有効か否か  */
extern	int	wnn_errorno;		/* Wnnのエラーはこの変数に報告される */

/* header file for dic_syurui */
#define NO_EXIST -1
#define NOT_A_JISHO 0
#define USER_DIC 1
#define SYSTEM_DIC 3

struct jwho {
    int sd;   /* socket discripter in jserver */
    char *user_name;   /* user name */
    char *host_name;   /* host name */
};

#define JWHO struct jwho

/*
  Local Variables:
  kanji-flag: t
  End:
*/
