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

#define	SJ3_InternalError	-1

#define	SJ3_NormalEnd		0

#define	SJ3_ServerDown		1
#define	SJ3_OpenSocket		2
#define	SJ3_ConnectSocket	3
#define	SJ3_GetHostByName	4
#define	SJ3_NotOpened		5
#define	SJ3_NotEnoughMemory	6
#define	SJ3_IllegalCommand	7

#define	SJ3_DifferentVersion	11
#define	SJ3_NoHostName		12
#define	SJ3_NoUserName		13
#define	SJ3_NotAllowedUser	14
#define	SJ3_AlreadyConnected	15
#define	SJ3_NotConnected	16

#define	SJ3_TooLongParameter	21
#define	SJ3_IllegalParameter	22

#define	SJ3_BadDictID		31
#define	SJ3_IllegalDictFile	32
#define	SJ3_IllegalStdyFile	33
#define	SJ3_IncorrectPasswd	34
#define	SJ3_FileNotExist	35
#define	SJ3_CannotAccessFile	36
#define	SJ3_CannotOpenFile	37
#define	SJ3_CannotCreateFile	38
#define	SJ3_FileReadError	39
#define	SJ3_FileWriteError	40
#define	SJ3_FileSeekError	41

#define	SJ3_StdyAlreadyOpened	51
#define	SJ3_StdyFileNotOpened	52
#define	SJ3_TooSmallStdyArea	53

#define	SJ3_LockedByOther	61
#define	SJ3_NotLocked		62

#define	SJ3_NoSuchDict		71
#define	SJ3_ReadOnlyDict	72
#define	SJ3_DictLocked		73
#define	SJ3_BadYomiString	74
#define	SJ3_BadKanjiString	75
#define	SJ3_BadHinsiCode	76

#define	SJ3_AddDictFailed	81
#define	SJ3_AlreadyExistWord	82
#define	SJ3_NoMoreDouonWord	83
#define	SJ3_NoMoreUserDict	84
#define	SJ3_NoMoreIndexBlock	85

#define	SJ3_DelDictFailed	91
#define	SJ3_NoSuchWord		92

#define	SJ3_DirAlreadyExist	101
#define	SJ3_CannotCreateDir	102

#define	SJ3_NoMoreDictData	111

#define	SJ3_UserConnected	121

#define	SJ3_TooLongPasswd	131
#define	SJ3_TooLongComment	132

