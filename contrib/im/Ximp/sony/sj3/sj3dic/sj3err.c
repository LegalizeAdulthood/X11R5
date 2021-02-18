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
	CASE(SJ3_InternalError,		"\203\103\203\223\203\136\201\133\203\151\203\213\203\107\203\211\201\133");
	CASE(SJ3_NormalEnd,		"\220\263\217\355\217\111\227\271");
	CASE(SJ3_ServerDown,		"\203\124\201\133\203\157\201\133\202\252\227\216\202\277\202\304\202\242\202\334\202\267");
	CASE(SJ3_OpenSocket,		"socket \202\314 open \202\311\216\270\224\163\202\265\202\334\202\265\202\275");
	CASE(SJ3_ConnectSocket,		"socket \202\314 connect \202\311\216\270\224\163\202\265\202\334\202\265\202\275");
	CASE(SJ3_GetHostByName,		"gethostbyname \202\311\216\270\224\163\202\265\202\334\202\265\202\275");
	CASE(SJ3_NotOpened,		"\203\111\201\133\203\166\203\223\202\263\202\352\202\304\202\242\202\334\202\271\202\361");
	CASE(SJ3_NotEnoughMemory,	"\203\201\203\202\203\212\202\252\221\253\202\350\202\334\202\271\202\361");
	CASE(SJ3_IllegalCommand,	"\203\122\203\175\203\223\203\150\202\252\222\350\213\140\202\263\202\352\202\304\202\242\202\334\202\271\202\361");
	CASE(SJ3_DifferentVersion,	"\203\157\201\133\203\127\203\207\203\223\202\252\210\341\202\242\202\334\202\267");
	CASE(SJ3_NoHostName,		"\203\172\203\130\203\147\226\274\202\252\202\240\202\350\202\334\202\271\202\361");
	CASE(SJ3_NoUserName,		"\203\206\201\133\203\125\226\274\202\252\202\240\202\350\202\334\202\271\202\361");
	CASE(SJ3_NotAllowedUser,	"\220\332\221\261\202\360\213\226\202\263\202\352\202\304\202\242\202\334\202\271\202\361");
	CASE(SJ3_AlreadyConnected,	"\220\332\221\261\215\317\202\305\202\267");
	CASE(SJ3_NotConnected,		"\220\332\221\261\202\263\202\352\202\304\202\242\202\334\202\271\202\361");
	CASE(SJ3_TooLongParameter,	"\203\160\203\211\203\201\201\133\203\136\202\252\222\267\202\267\202\254\202\334\202\267");
	CASE(SJ3_IllegalParameter,	"\203\160\203\211\203\201\201\133\203\136\202\252\210\331\217\355\202\305\202\267");
	CASE(SJ3_BadDictID,		"\202\273\202\314\202\346\202\244\202\310\216\253\217\221\202\315\202\240\202\350\202\334\202\271\202\361");
	CASE(SJ3_IllegalDictFile,	"\216\253\217\221\203\164\203\100\203\103\203\213\202\252\210\331\217\355\202\305\202\267");
	CASE(SJ3_IllegalStdyFile,	"\212\167\217\113\203\164\203\100\203\103\203\213\202\252\210\331\217\355\202\305\202\267");
	CASE(SJ3_IncorrectPasswd,	"\203\160\203\130\203\217\201\133\203\150\202\252\210\331\202\310\202\350\202\334\202\267");
	CASE(SJ3_FileNotExist,		"\203\164\203\100\203\103\203\213\202\252\221\266\215\335\202\265\202\334\202\271\202\361");
	CASE(SJ3_CannotAccessFile,	"\203\164\203\100\203\103\203\213\202\311\203\101\203\116\203\132\203\130\202\305\202\253\202\334\202\271\202\361");
	CASE(SJ3_CannotOpenFile,	"\203\164\203\100\203\103\203\213\202\252\203\111\201\133\203\166\203\223\202\305\202\253\202\334\202\271\202\361");
	CASE(SJ3_CannotCreateFile,	"\203\164\203\100\203\103\203\213\202\252\215\354\220\254\202\305\202\253\202\334\202\271\202\361");
	CASE(SJ3_FileReadError,		"\203\212\201\133\203\150\203\107\203\211\201\133\202\252\202\240\202\350\202\334\202\265\202\275");
	CASE(SJ3_FileWriteError,	"\203\211\203\103\203\147\203\107\203\211\201\133\202\252\202\240\202\350\202\334\202\265\202\275");
	CASE(SJ3_FileSeekError,		"\203\126\201\133\203\116\203\107\203\211\201\133\202\252\202\240\202\350\202\334\202\265\202\275");
	CASE(SJ3_StdyAlreadyOpened,	"\212\167\217\113\203\164\203\100\203\103\203\213\202\315\203\111\201\133\203\166\203\223\202\263\202\352\202\304\202\242\202\334\202\267");
	CASE(SJ3_StdyFileNotOpened,	"\212\167\217\113\203\164\203\100\203\103\203\213\202\252\203\111\201\133\203\166\203\223\202\263\202\352\202\304\202\242\202\334\202\271\202\361");
	CASE(SJ3_TooSmallStdyArea,	"\225\266\220\337\222\267\212\167\217\113\227\314\210\346\202\252\217\254\202\263\202\267\202\254\202\334\202\267");
	CASE(SJ3_LockedByOther,		"\221\274\202\314\203\116\203\211\203\103\203\101\203\223\203\147\202\311\203\215\203\142\203\116\202\263\202\352\202\304\202\334\202\267");
	CASE(SJ3_NotLocked,		"\203\215\203\142\203\116\202\263\202\352\202\304\202\242\202\334\202\271\202\361");
	CASE(SJ3_NoSuchDict,		"\202\273\202\314\202\346\202\244\202\310\216\253\217\221\202\315\202\240\202\350\202\334\202\271\202\361");
	CASE(SJ3_ReadOnlyDict,		"\217\221\202\253\215\236\202\335\202\305\202\253\202\310\202\242\216\253\217\221\202\305\202\267");
	CASE(SJ3_DictLocked,		"\216\253\217\221\202\252\203\215\203\142\203\116\202\263\202\352\202\304\202\242\202\334\202\267");
	CASE(SJ3_BadYomiString,		"\213\226\202\263\202\352\202\310\202\242\223\307\202\335\225\266\216\232\227\361\202\305\202\267");
	CASE(SJ3_BadKanjiString,	"\213\226\202\263\202\352\202\310\202\242\212\277\216\232\225\266\216\232\227\361\202\305\202\267");
	CASE(SJ3_BadHinsiCode,		"\225\151\216\214\203\122\201\133\203\150\210\331\217\355\202\305\202\267");
	CASE(SJ3_AddDictFailed,		"\216\253\217\221\223\157\230\136\202\360\216\270\224\163\202\265\202\334\202\265\202\275");
	CASE(SJ3_AlreadyExistWord,	"\202\267\202\305\202\311\221\266\215\335\202\265\202\304\202\242\202\334\202\267");
	CASE(SJ3_NoMoreDouonWord,	"\202\261\202\352\210\310\217\343\223\257\211\271\214\352\202\360\223\157\230\136\202\305\202\253\202\334\202\271\202\361");
	CASE(SJ3_NoMoreUserDict,	"\202\261\202\352\210\310\217\343\216\253\217\221\202\311\223\157\230\136\202\305\202\253\202\334\202\271\202\361");
	CASE(SJ3_NoMoreIndexBlock,	"\202\261\202\352\210\310\217\343\203\103\203\223\203\146\203\142\203\116\203\130\202\311\223\157\230\136\202\305\202\253\202\334\202\271\202\361");
	CASE(SJ3_DelDictFailed,		"\216\253\217\221\215\355\217\234\202\360\216\270\224\163\202\265\202\334\202\265\202\275");
	CASE(SJ3_NoSuchWord,		"\202\273\202\314\202\346\202\244\202\310\217\156\214\352\202\315\202\240\202\350\202\334\202\271\202\361");
	CASE(SJ3_DirAlreadyExist,	"\202\273\202\314\203\146\203\102\203\214\203\116\203\147\203\212\202\252\221\266\215\335\202\265\202\304\202\242\202\334\202\267");
	CASE(SJ3_CannotCreateDir,	"\203\146\203\102\203\214\203\116\203\147\203\212\202\252\215\354\220\254\202\305\202\253\202\334\202\271\202\361");
	CASE(SJ3_NoMoreDictData,	"\202\261\202\352\210\310\217\343\216\253\217\221\203\146\201\133\203\136\202\252\202\240\202\350\202\334\202\271\202\361");
	CASE(SJ3_UserConnected,		"\220\332\221\261\202\265\202\304\202\242\202\351\203\206\201\133\203\125\202\252\202\240\202\350\202\334\202\267");
	CASE(SJ3_TooLongPasswd,		"\203\160\203\130\203\217\201\133\203\150\202\252\222\267\202\267\202\254\202\334\202\267");
	CASE(SJ3_TooLongComment,	"\203\122\203\201\203\223\203\147\202\252\222\267\202\267\202\254\202\334\202\267");
#undef	CASE
	default:
		sprintf(tmp, "\226\242\222\350\213\140\202\314\203\107\203\211\201\133(%d)", code);
		printout(fp, tmp);
		break;
	}
	printout(fp, "\r\n");
}

