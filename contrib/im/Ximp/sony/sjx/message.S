/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Masaki Takeuchi, Sony Corporation
 */
/*
 *	message.c
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Sat May 30 14:47:48 JST 1987
 */
/*
 * $Header: message.S,v 1.1 91/09/04 23:08:29 root Locked $ Sony;
 */

char *mess_warning
	="warning: Please copy user dictionary to your home directory.\n\r";
char *mess_error1
	="error: Can't open user dictionary (%s).\n\r";
char *mess_error2
	="error: Can't open main dictionary (%s).\n\r";

char	*SJrun		= "ＳＪＸ　　";

char	*Mode_buff	= "";
char	*Mode_unbuff	= "";

char	*Mode_Zhira	= "全ひらがな";
char	*Mode_Zkata	= "全カタカナ";
char	*Mode_Hkata	= "半カタカナ";
char	*Mode_Zalpha	= "全アルファ";
char	*Mode_Halpha	= "半アルファ";

char	*Gmode
		= "  登録 消去 修正 　　 code  半ア 全ア 半カ 全カ かな";

char	*GEtc		= "その他　　";
char	*Getc
		= "  登録 消去 Help sjrc 修正  　　 　　 　　 　　 　　";

char	*GHelp		= "Help　Mode";
char	*MHelp		= "  ガイド・ライン %s になりました ";

/* guide code input	*/

char	*GCshift	= "ShiftJIS　";
char	*GCeuc		= "　ＥＵＣ　";
char	*GCjis		= "　ＪＩＳ　";
char	*GCkuten	= "　　区点　";

/* guide toroku		*/

char	*Gtoroku	= "登録";
char	*Mhani		= " 範囲を指定してください ";
char	*Myomi		= " 漢字 [%s] : 読み入力 ";
char	*TOUROKU_OK	= " 漢字 [%s] : 読み [%s] (y/n)? ";
char	*TOUROKU_FAIL	= " 登録できませんでした ";
char	*TOUROKU_SUCC	= " 登録しました ";

/* guide syoukyo	*/

char	*Gsyoukyo	= "消去";
char	*SYOUKYO_FAIL	= " 消去できませんでした ";
char	*SYOUKYO_SUCC	= " 消去しました ";

/* get sjrc		*/

char	*SUCCsjrc	= "を読み込みました";
char	*FAILsjrc	= "を読み込めませんでした";

/* guide hensyuu	*/

char	*Gedit		= "修正 ";
