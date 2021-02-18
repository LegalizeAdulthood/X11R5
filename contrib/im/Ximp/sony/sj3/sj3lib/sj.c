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
#include <pwd.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/file.h>
#include "sj_const.h"
#include "Const.h"
#include "sj3err.h"
#include "sj3lib.h"
#include "sj3lowlib.h"
#include "sj_hinsi.h"

extern	int	sj3_error_number;

#ifndef	lint
static	char	rcsid_sony[] = "$Header: sj.c,v 1.1 91/09/04 23:06:20 root Locked $ SONY;";
#endif

char	*sj3_user_dir = "user";
static	char	*path_delimiter = "/";
static	SJ3_CLIENT_ENV  client = { -1, 0 };
static	long	mdicid = 0;
static	long	udicid = 0;

char	*getlogin(), *getenv();
long	sj3_open_dictionary();

static	make_dirs(path)
char	*path;
{
	char	tmp[PathNameLen];
	register char	*p;
	int	i;

	for (p = path ; *p ; p++) {
		if (*p != *path_delimiter) continue;

		strncpy(tmp, path, (i = p - path));
		tmp[i] = '\0';
		if (sj3_access(&client, tmp, F_OK) != ERROR) continue;
		if (sj3_error_number == SJ3_ServerDown) return ERROR;

		if (sj3_make_directory(&client, tmp) == ERROR) return ERROR;
	}
	return 0;
}

sj3_open(host, user)
char	*host;
char	*user;
{
	char	tmp[PathNameLen];
	char	*p;
	int	err = SJ3_NORMAL_END;

	if (client.fd != -1) return SJ3_ALREADY_CONNECTED;

	sprintf(tmp, "%d.sj3lib", getpid());
	if (sj3_make_connection(&client, host, user, tmp) == ERROR) {
		if (sj3_error_number == SJ3_ServerDown) goto server_dead;
		client.fd = -1; return SJ3_CONNECT_ERROR;
	}

	if (client.stdy_size > SJ3_WORD_ID_SIZE) {
		sj3_erase_connection(&client);
		return SJ3_CONNECT_ERROR;
	}

	mdicid = sj3_open_dictionary(&client, MainDictionary, NULL);
	if (mdicid == 0) {
		if (sj3_error_number == SJ3_ServerDown) goto server_dead;
		err |= SJ3_CANNOT_OPEN_MDICT;
	}

	strcpy(tmp, sj3_user_dir);
	if (tmp[strlen(tmp) - 1] != *path_delimiter)
		strcat(tmp, path_delimiter);
	strcat(tmp, user);
	strcat(tmp, path_delimiter);
	if (make_dirs(tmp) == ERROR) {
		if (sj3_error_number == SJ3_ServerDown) goto server_dead;
		return (err | SJ3_CANNOT_MAKE_UDIR);
	}

	for (p = tmp ; *p ; p++) ;
	strcpy(p, UserDictionary);
	if (sj3_access(&client, tmp, F_OK) == ERROR) {
		if (sj3_error_number == SJ3_ServerDown) goto server_dead;
		if (sj3_make_dict_file(&client, tmp, DefIdxLen,
				DefSegLen, DefSegNum) == ERROR) {
			if (sj3_error_number == SJ3_ServerDown)
				goto server_dead;
			err |= SJ3_CANNOT_MAKE_UDICT;
		}
	}
	udicid = sj3_open_dictionary(&client, tmp, "");
	if (udicid == 0) {
		if (sj3_error_number == SJ3_ServerDown) goto server_dead;
		err |= SJ3_CANNOT_OPEN_UDICT;
	}

	strcpy(p, StudyFile);
	if (sj3_access(&client, tmp, F_OK) == ERROR) {
		if (sj3_error_number == SJ3_ServerDown) goto server_dead;
		if (sj3_make_study_file(&client, tmp, DefStyNum, DefClStep,
				DefClLen) == ERROR) {
			if (sj3_error_number == SJ3_ServerDown)
				goto server_dead;
			err |= SJ3_CANNOT_MAKE_STUDY;
		}
	}
	if (sj3_open_study_file(&client, tmp, "") == ERROR) {
		if (sj3_error_number == SJ3_ServerDown) goto server_dead;
		err |= SJ3_CANNOT_OPEN_STUDY;
	}

	return err;

server_dead:
	mdicid = udicid = 0;
	return SJ3_SERVER_DEAD;
}
sj3_close()
{
	int	err = SJ3_NORMAL_END;

	if (client.fd == -1) return SJ3_NOT_CONNECTED;

	if (mdicid == 0)
		err |= SJ3_NOT_OPENED_MDICT;
	else if (sj3_close_dictionary(&client, mdicid) == ERROR) {
		if (sj3_error_number == SJ3_ServerDown) goto server_dead;
		err |= SJ3_CLOSE_MDICT_ERROR;
	}
	mdicid = 0;

	if (udicid == 0)
		err |= SJ3_NOT_OPENED_UDICT;
	else if (sj3_close_dictionary(&client, udicid) == ERROR) {
		if (sj3_error_number == SJ3_ServerDown) goto server_dead;
		err |= SJ3_CLOSE_UDICT_ERROR;
	}
	udicid = 0;

	if (sj3_close_study_file(&client) == ERROR) {
		switch (sj3_error_number) {
		case SJ3_ServerDown:
			goto server_dead;

		case SJ3_StdyFileNotOpened:
			err |= SJ3_NOT_OPENED_STUDY; break;

		default:
			err |= SJ3_CLOSE_STUDY_ERROR; break;
		}
	}

	if (sj3_erase_connection(&client)) {
		if (sj3_error_number == SJ3_ServerDown) goto server_dead;
		err |= SJ3_DISCONNECT_ERROR;
	}

	return err;

server_dead:
	mdicid = udicid = 0;
	return SJ3_SERVER_DEAD;
}

sj3_getkan(yomi, bun, knj, knjsiz)
u_char	*yomi;
SJ3_BUNSETU	*bun;
u_char	*knj;
int	knjsiz;
{
	u_char	*src;
	int	buncnt = 0;
	int	len;
	int	stysiz = client.stdy_size;

	if ((len = strlen(yomi)) > SJ3_IKKATU_YOMI) return 0;

	while (*yomi) {
		len = sj3_ikkatu_henkan(&client, yomi, knj, knjsiz);
		if (len == ERROR) {
			if (client.fd < 0) {
				mdicid = udicid = 0;
				return -1;
			}

			return 0;
		}
		else if (len == 0)
			break;

		src = knj;
		while (*src) {
			bun -> srclen = *src++;
			bcopy(src, &(bun -> dcid), stysiz);
			src += stysiz;
			bun -> destlen = strlen(src);
			bun -> srcstr = yomi;
			bun -> deststr = knj;
			while (*src) *knj++ = *src++;
			knjsiz -= bun -> destlen;
			src++;
			yomi += bun -> srclen;
			bun++;
			buncnt++;
		}
		*knj = 0;
	}

	if (*yomi) {
		bun -> srclen = strlen(yomi);
		bun -> srcstr = yomi;
		bun -> destlen = 0;
		bun -> deststr = NULL;
		bzero(&(bun -> dcid), sizeof(bun -> dcid));
		buncnt++;
	}

	return buncnt;
}

sj3_douoncnt(yomi)
u_char	*yomi;
{
	int	i;

	if ((i = strlen(yomi)) > SJ3_BUNSETU_YOMI) return 0;

	i = sj3_bunsetu_kouhosuu(&client, yomi, i);
	if (i == ERROR) {
		if (client.fd < 0) {
			mdicid = udicid = 0;
			return -1;
		}

		return 0;
	}

	return i;
}

sj3_getdouon(yomi, dou)
u_char	*yomi;
struct	sj3_douon	*dou;
{
	int	i;

	if ((i = strlen(yomi)) > SJ3_BUNSETU_YOMI) return 0;

	i = sj3_bunsetu_zenkouho(&client, yomi, i, dou);
	if (i == ERROR) {
		if (client.fd < 0) {
			mdicid = udicid = 0;
			return -1;
		}
		return 0;
	}

	return i;
}

sj3_gakusyuu(dcid)
SJ3_STUDYREC	*dcid;
{
	if (sj3_tango_gakusyuu(&client, dcid) == ERROR) {
		if (client.fd < 0) {
			mdicid = udicid = 0;
			return -1;
		}
		return 1;
	}
	return 0;
}

sj3_gakusyuu2(yomi1, yomi2, dcid)
u_char	*yomi1;
u_char	*yomi2;
SJ3_STUDYREC	*dcid;
{
	if (sj3_bunsetu_gakusyuu(&client, yomi1, yomi2, dcid) == ERROR) {
		if (client.fd < 0) {
			mdicid = udicid = 0;
			return -1;
		}
		return 1;
	}
	return 0;
}

sj3_touroku(yomi, kanji, code)
u_char	*yomi;
u_char	*kanji;
int	code;
{
	if (sj3_tango_touroku(&client, udicid, yomi, kanji, code)) {
		if (client.fd < 0) {
			mdicid = udicid = 0;
			return -1;
		}
		switch (sj3_error_number) {
		case SJ3_NoSuchDict:
		case SJ3_ReadOnlyDict:		return SJ3_DICT_ERROR;
		case SJ3_DictLocked:		return SJ3_DICT_LOCKED;
		case SJ3_BadYomiString:		return SJ3_BAD_YOMI_STR;
		case SJ3_BadKanjiString:	return SJ3_BAD_KANJI_STR;
		case SJ3_BadHinsiCode:		return SJ3_BAD_HINSI_CODE;
		case SJ3_AlreadyExistWord:	return SJ3_WORD_EXIST;
		case SJ3_NoMoreDouonWord:	return SJ3_DOUON_FULL;
		case SJ3_NoMoreUserDict:	return SJ3_DICT_FULL;
		case SJ3_NoMoreIndexBlock:	return SJ3_INDEX_FULL;
		default:			return SJ3_TOUROKU_FAILED;
		}
	}

	return 0;
}

sj3_syoukyo(yomi, kanji, code)
u_char	*yomi;
u_char	*kanji;
int	code;
{
	if (sj3_tango_sakujo(&client, udicid, yomi, kanji, code)) {
		if (client.fd < 0) {
			mdicid = udicid = 0;
			return -1;
		}
		switch (sj3_error_number) {
		case SJ3_NoSuchDict:
		case SJ3_ReadOnlyDict:		return SJ3_DICT_ERROR;
		case SJ3_DictLocked:		return SJ3_DICT_LOCKED;
		case SJ3_BadYomiString:		return SJ3_BAD_YOMI_STR;
		case SJ3_BadKanjiString:	return SJ3_BAD_KANJI_STR;
		case SJ3_BadHinsiCode:		return SJ3_BAD_HINSI_CODE;
		case SJ3_NoSuchWord:		return SJ3_WORD_NOT_EXIST;
		default:			return SJ3_SYOUKYO_FAILED;
		}
	}

	return 0;
}

sj3_getdict(buf)
u_char	*buf;
{
	if (sj3_tango_syutoku(&client, udicid, buf)) {
		if (client.fd < 0) {
			mdicid = udicid = 0;
			return -1;
		}
		return 1;
	}
	return 0;
}
sj3_nextdict(buf)
u_char	*buf;
{
	if (sj3_tango_jikouho(&client, udicid, buf)) {
		if (client.fd < 0) {
			mdicid = udicid = 0;
			return -1;
		}
		return 1;
	}
	return 0;
}
sj3_prevdict(buf)
u_char	*buf;
{
	if (sj3_tango_maekouho(&client, udicid, buf)) {
		if (client.fd < 0) {
			mdicid = udicid = 0;
			return -1;
		}
		return 1;
	}
	return 0;
}

sj3_lockserv()
{
	if (sj3_lock_server(&client) == ERROR) {
		if (client.fd < 0) {
			mdicid = udicid = 0;
			return -1;
		}
		return 1;
	}
	return 0;
}
sj3_unlockserv()
{
	if (sj3_unlock_server(&client)) {
		if (client.fd < 0) {
			mdicid = udicid = 0;
			return -1;
		}
		return 1;
	}
	return 0;
}

