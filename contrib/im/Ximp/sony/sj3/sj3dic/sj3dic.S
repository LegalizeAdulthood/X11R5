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
#include <strings.h>
#include <ctype.h>
#include <pwd.h>
#include "sjtool.h"
#include "sj3lib.h"

char	*getlogin(), *getenv();
struct	passwd	*getpwnam(), *getpwuid();

#ifndef	lint
static	char	rcsid_sony[] = "$Header: sj3dic.S,v 1.1 91/09/04 23:05:59 root Locked $ SONY;";
#endif

static	struct	option	{
	char	*str;
	int	code;
} options[] = {
	"text",		EXEC_TEXT,
	"dict",		EXEC_DICT,
	"init",		EXEC_INIT,
	"verbose",	EXEC_VERBOSE,
	"host",		EXEC_SERVER,
	"server",	EXEC_SERVER,
	"force",	EXEC_FORCE,
	0,		0
};

char	prog_name[LONGLENGTH];
char	home_dir[LONGLENGTH];
char	user_name[LONGLENGTH];
char	term_name[LONGLENGTH];
char	serv_name[LONGLENGTH];
char	file_name[LONGLENGTH];
char	dict_name[LONGLENGTH];
int	verbose_flag = 0;
int	init_flag = 0;
int	force_flag = 0;

main(argc, argv)
int	argc;
char	**argv;
{
	int	err;
	int	mode;

	init_code();
	init_env();
	mode = parsearg(argc, argv);
	setsjserv(getenv("SJ3SERV"));
	getsjrc();
	make_dicname();

	if (err = sj3_open(serv_name, user_name, prog_name)) _open_error(err);

	switch (mode) {
	case EXEC_TEXT:
		dictdisp(file_name);
		break;

	case EXEC_DICT:
		dictmake(file_name);
		break;

	default:
		break;
	}

	if (err = sj3_close()) _close_error(err);
}

init_env()
{
	char	*un, *hp, *tn;
	struct	passwd	*pwd;

	prog_name[0] =
	home_dir[0]  =
	user_name[0] =
	term_name[0] =
	serv_name[0] =
	file_name[0] =
	dict_name[0] = '\0';

	un = getlogin();
	hp = getenv("HOME");

	setpwent();
	pwd = (un == NULL || *un == '\0') ? getpwuid(getuid()) : getpwnam(un);
	if (pwd != NULL) {
		strcpy(user_name, pwd -> pw_name);
		if (hp == NULL) hp = pwd -> pw_dir;
		endpwent();
	}
	if (hp != NULL) strcpy(home_dir, hp);

	if ((tn = getenv("TERM")) == NULL) {
		fprintf(stderr, "Can't getenv TERM\n\r");
		perror("getenv");
		exit(1);
	}
	strcpy(term_name, tn);
}

usage(ret)
int	ret;
{
	fprintf(stderr,
		"Usage: %s -{text|dict} [-H host_name] [file_name]\n",
		prog_name);
	exit(ret);
}

parsearg(argc, argv)
int	argc;
char	*argv[];
{
	int	errflg = 0;
	int	i, j;
	char	*p, *q;
	struct	option	*opt;
	int	cmd;
	int	mode = 0;
	char	tmp[LONGLENGTH];

	strcpy(prog_name, (p = rindex(argv[0], '/')) ? p + 1 : argv[0]);

	for (i = 1 ; i < argc ; i++) {
		if (*argv[i] == '-') {
			for (p = argv[i]+1, q = tmp ; *p ; p++)
				*q++ = isupper(*p) ? tolower(*p) : *p;
			*q = 0;
			j = strlen(tmp);
			cmd = 0;
			for (opt = options ; opt -> code ; opt++) {
				if (strncmp(opt -> str, tmp, j))
					continue;
				else if (cmd) {
					errflg++; break;
				}
				else
					cmd = opt -> code;
			}
			if (errflg) break;
			if (cmd == 0) { errflg++; break; }
			switch (cmd) {
			case EXEC_TEXT:
			case EXEC_DICT:
				if (mode)
					errflg++;
				else
					mode = cmd;
				break;

			case EXEC_INIT:
				init_flag++; break;

			case EXEC_VERBOSE:
				verbose_flag++; break;

			case EXEC_SERVER:
				if (++i >= argc) { errflg++; break; }
				setsjserv(argv[i]);
				break;

			case EXEC_FORCE:
				force_flag++; break;

			default:
				errflg++; break;
			}
		}
		else if (file_name[0]) {
			errflg++; break;
		}
		else
			strcpy(file_name, argv[i]);
	}

	if (errflg || mode == 0) usage(1);

	return mode;
}

make_dicname()
{
	if (dict_name[0] != '\0') return;

	strcpy(dict_name, home_dir);
	strcat(dict_name, "/");
	strcat(dict_name, "sj2usr.dic");
}
setdicname(dictname)
char	*dictname;
{
	if (dict_name[0] != '\0') return;
	if (dictname == NULL) return;
	strcpy(dict_name, dictname);
}

setsjserv(hostname)
char	*hostname;
{
	if (serv_name[0] != '\0') return;
	if (hostname == NULL) return;
	strcpy(serv_name, hostname);
}

struct	errlist	{
	int	code;
	char	*msg;
	int	flg;
};

static	_error_and(err, list)
int	err;
struct	errlist	*list;
{
	int	flag = 0;

	while (list -> code) {
		if (err & list -> code) {
			error_out(list -> msg);
			err &= ~(list -> code);
			if (list -> flg) flag++;
		}
		list++;
	}
	if (flag) exit(1);
}

static	_open_error(err)
int	err;
{
	static	struct	errlist	err_msg[] = {
	SJ3_SERVER_DEAD,	"サーバが死んでいます",			1,
	SJ3_CONNECT_ERROR,	"サーバと接続できませんでした",		1,
	SJ3_ALREADY_CONNECTED,	"サーバと接続済です",			1,
	SJ3_CANNOT_OPEN_MDICT,	"メイン辞書がオープンできません",	0,
	SJ3_CANNOT_OPEN_UDICT,	"ユーザ辞書がオープンできません",	1,
	SJ3_CANNOT_OPEN_STUDY,	"学習ファイルがオープンできません",	0,
	SJ3_CANNOT_MAKE_UDIR,	"ユーザーディレクトリが作成できません",	1,
	SJ3_CANNOT_MAKE_UDICT,	"ユーザ辞書が作成できません",		1,
	SJ3_CANNOT_MAKE_STUDY,	"学習ファイルが作成できません",		0,
	-1,			"エラー",				1,
	0, 0, 0
	};

	_error_and(err, err_msg);
}

static	_close_error(err)
int	err;
{
	static	struct	errlist	err_msg[] = {
	SJ3_SERVER_DEAD,	"サーバーが死んでいます",		1,
	SJ3_DISCONNECT_ERROR,	"切り放しでエラーがありました",		1,
	SJ3_NOT_CONNECTED,	"サーバと接続されていません",		1,
	SJ3_NOT_OPENED_MDICT,	"メイン辞書はオープンされていません",	0,
	SJ3_NOT_OPENED_UDICT,	"ユーザ辞書はオープンされていません",	1,
	SJ3_NOT_OPENED_STUDY,	"学習ファイルはオープンされていません",	0,
	SJ3_CLOSE_MDICT_ERROR,	"メイン辞書がクローズできません",	0,
	SJ3_CLOSE_UDICT_ERROR,	"ユーザ辞書がクローズできません",	1,
	SJ3_CLOSE_STUDY_ERROR,	"学習ファイルがクローズできません",	0,
	-1,			"エラー",				1,
	0, 0, 0
	};

	_error_and(err, err_msg);
}

