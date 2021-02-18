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
 * Author: Noriyuki Hosoe, Sony Corporation
 */

#include <stdio.h>
#include "sj3err.h"

sj3error(fp, code)
FILE	*fp;
int	code;
{
	unsigned char	tmp[BUFSIZ];

	switch (code) {
#define	CASE(X, str)		case (X): printout(fp, str); break;
	CASE(SJ3_InternalError,		"インターナルエラー");
	CASE(SJ3_NormalEnd,		"正常終了");
	CASE(SJ3_ServerDown,		"サーバーが落ちています");
	CASE(SJ3_OpenSocket,		"socket の open に失敗しました");
	CASE(SJ3_ConnectSocket,		"socket の connect に失敗しました");
	CASE(SJ3_GetHostByName,		"gethostbyname に失敗しました");
	CASE(SJ3_NotOpened,		"オープンされていません");
	CASE(SJ3_NotEnoughMemory,	"メモリが足りません");
	CASE(SJ3_IllegalCommand,	"コマンドが定義されていません");
	CASE(SJ3_DifferentVersion,	"バージョンが違います");
	CASE(SJ3_NoHostName,		"ホスト名がありません");
	CASE(SJ3_NoUserName,		"ユーザ名がありません");
	CASE(SJ3_NotAllowedUser,	"接続を許されていません");
	CASE(SJ3_AlreadyConnected,	"接続済です");
	CASE(SJ3_NotConnected,		"接続されていません");
	CASE(SJ3_TooLongParameter,	"パラメータが長すぎます");
	CASE(SJ3_IllegalParameter,	"パラメータが異常です");
	CASE(SJ3_BadDictID,		"そのような辞書はありません");
	CASE(SJ3_IllegalDictFile,	"辞書ファイルが異常です");
	CASE(SJ3_IllegalStdyFile,	"学習ファイルが異常です");
	CASE(SJ3_IncorrectPasswd,	"パスワードが異なります");
	CASE(SJ3_FileNotExist,		"ファイルが存在しません");
	CASE(SJ3_CannotAccessFile,	"ファイルにアクセスできません");
	CASE(SJ3_CannotOpenFile,	"ファイルがオープンできません");
	CASE(SJ3_CannotCreateFile,	"ファイルが作成できません");
	CASE(SJ3_FileReadError,		"リードエラーがありました");
	CASE(SJ3_FileWriteError,	"ライトエラーがありました");
	CASE(SJ3_FileSeekError,		"シークエラーがありました");
	CASE(SJ3_StdyAlreadyOpened,	"学習ファイルはオープンされています");
	CASE(SJ3_StdyFileNotOpened,	"学習ファイルがオープンされていません");
	CASE(SJ3_TooSmallStdyArea,	"文節長学習領域が小さすぎます");
	CASE(SJ3_LockedByOther,		"他のクライアントにロックされてます");
	CASE(SJ3_NotLocked,		"ロックされていません");
	CASE(SJ3_NoSuchDict,		"そのような辞書はありません");
	CASE(SJ3_ReadOnlyDict,		"書き込みできない辞書です");
	CASE(SJ3_DictLocked,		"辞書がロックされています");
	CASE(SJ3_BadYomiString,		"許されない読み文字列です");
	CASE(SJ3_BadKanjiString,	"許されない漢字文字列です");
	CASE(SJ3_BadHinsiCode,		"品詞コード異常です");
	CASE(SJ3_AddDictFailed,		"辞書登録を失敗しました");
	CASE(SJ3_AlreadyExistWord,	"すでに存在しています");
	CASE(SJ3_NoMoreDouonWord,	"これ以上同音語を登録できません");
	CASE(SJ3_NoMoreUserDict,	"これ以上辞書に登録できません");
	CASE(SJ3_NoMoreIndexBlock,	"これ以上インデックスに登録できません");
	CASE(SJ3_DelDictFailed,		"辞書削除を失敗しました");
	CASE(SJ3_NoSuchWord,		"そのような熟語はありません");
	CASE(SJ3_DirAlreadyExist,	"そのディレクトリが存在しています");
	CASE(SJ3_CannotCreateDir,	"ディレクトリが作成できません");
	CASE(SJ3_NoMoreDictData,	"これ以上辞書データがありません");
	CASE(SJ3_UserConnected,		"接続しているユーザがあります");
	CASE(SJ3_TooLongPasswd,		"パスワードが長すぎます");
	CASE(SJ3_TooLongComment,	"コメントが長すぎます");
#undef	CASE
	default:
		sprintf(tmp, "未定義のエラー(%d)", code);
		printout(fp, tmp);
		break;
	}
	printout(fp, "\r\n");
}

