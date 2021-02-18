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

#include "sj_kcnv.h"
#include <setjmp.h>
#include <sys/file.h>
#include <errno.h>
#include "sj3cmd.h"
#include "sj3err.h"
#include "Dict.h"

extern	int	errno;

extern	Global	*work_base;
extern	STDY	*stdy_base;
extern	int	serv_errno;

extern	int	client_num;
extern	Client	client[];
extern	int	cur_client;
extern	int	client_fd;

extern	char	*dict_dir;
extern	int	dir_mode;
extern	int	file_mode;

static	Uchar	buf1[YomiBufSize];
static	Uchar	buf2[YomiBufSize];
static	Uchar	kbuf[KanjiBufSize];

static	jmp_buf	error_ret;
static	WorkArea *worklist = NULL;
static	Client	*cur_cli;

char	*malloc();
DictFile *opendict();
int	closedict();
StdyFile *openstdy();
int	closestdy();

Uchar	*put_ndata(), *put_string();

make_full_path(path)
char	*path;
{
	char	tmp[PathNameLen];
	int	i;

	if (*path == '/') return 0;
	i = strlen(dict_dir) + 1 + strlen(path) + 1;
	if (i > sizeof(tmp)) return ERROR;
	strcpy(tmp, dict_dir);
	strcat(tmp, "/");
	strcat(tmp, path);
	strcpy(path, tmp);
	return 0;
}

WorkArea *alloc_workarea()
{
	WorkArea *p;

	if (!(p = (WorkArea *)malloc(sizeof(WorkArea)))) return NULL;

	bzero(p, sizeof(WorkArea));
	p -> lock   = NotLocked;
	p -> refcnt = 1;

	p -> link = worklist;
	worklist = p;

	work_base = &(p -> global);
	initwork();

	return p;
}
free_workarea(p)
WorkArea *p;
{
	if (!p) return;
	if (--(p -> refcnt)) return;

	if (worklist == p)
		worklist = p -> link;
	else {
		register WorkArea *q;

		for (q = worklist ; q ; q = q -> link) {
			if (q -> link == p) {
				q -> link = p -> link;
				break;
			}
		}
	}

	if (p -> global.Jdictlist)
		close_dictlist(p -> global.Jdictlist);

	free((char *)p);
}

exec_connect()
{
	int	version;
	char	hostname[HostNameLen];
	char	username[UserNameLen];
	char	progname[ProgNameLen];
	WorkArea *wp;

	version = get_int();
	get_nstring(hostname, sizeof(hostname));
	get_nstring(username, sizeof(username));
	get_nstring(progname, sizeof(progname));

	if (cur_cli -> work) longjmp(error_ret, SJ3_AlreadyConnected);

	if (version != SJ3SERV_VERSION_NO)
		longjmp(error_ret, SJ3_DifferentVersion);

	if (hostname[0] == '\0')
		longjmp(error_ret, SJ3_NoHostName);

	if (username[0] == '\0')
		longjmp(error_ret, SJ3_NoUserName);

	if (!check_user(username, hostname))
		longjmp(error_ret, SJ3_NotAllowedUser);

	if (!(wp = alloc_workarea()))
		longjmp(error_ret, SJ3_NotEnoughMemory);

	strcpy(cur_cli -> host, hostname);
	strcpy(cur_cli -> user, username);
	strcpy(cur_cli -> prog, progname);
	cur_cli -> work = wp;

	put_int(SJ3_NormalEnd);

	logging_out("connection established on %d(%s, %s, %s)",
			cur_cli -> fd, hostname, username, progname);
}

exec_disconnect()
{
	if (cur_cli -> stdy) {
		closestdy(cur_cli -> stdy);
		cur_cli -> stdy = NULL;
	}
	if (cur_cli -> work) {
		free_workarea(cur_cli -> work);
		cur_cli -> work = NULL;
	}
	else
		longjmp(error_ret, SJ3_NotConnected);

	put_int(SJ3_NormalEnd);
}

exec_opendict()
{
	char	filename[PathNameLen];
	char	password[PassWordLen];
	DICT	*dict;
	DICTL	*dictl;

	get_nstring(filename, sizeof(filename));
	get_nstring(password, sizeof(password));

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	if (make_full_path(filename)) longjmp(error_ret, SJ3_TooLongParameter);

	if (!(dict = (DICT *)opendict(filename, password)))
		longjmp(error_ret, serv_errno);
	if (!(dictl = (DICTL *)malloc(sizeof *dictl))) {
		closedict((DictFile *)dict);
		longjmp(error_ret, SJ3_NotEnoughMemory);
	}

	dictl -> dict = dict;
	dictl -> next = dictlist;
	dictlist = dictl;

	if (cur_cli -> work -> lock == cur_cli -> fd) {
		if (dict -> maxunit != 0)
			lock_dict((DictFile *)dict, cur_cli -> fd);
	}

	put_int(SJ3_NormalEnd);
	put_int((int)dict->dicid);
}
exec_closedict()
{
	TypeDicID	id;
	DICTL	*dictl;

	id = get_int();

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);

	for (dictl = dictlist ; dictl ; dictl = dictl -> next)
		if (dictl -> dict -> dicid == id) break;
	if (!dictl) longjmp(error_ret, SJ3_BadDictID);

	unlock_dict((DictFile *)(dictl -> dict), cur_cli -> fd);

	closedict((DictFile *)(dictl -> dict));
	if (dictlist == dictl)
		dictlist = dictl -> next;
	else {
		DICTL	*p;

		for (p = dictlist ; p -> next ; p = p -> next) {
			if (p -> next == dictl) {
				p -> next = dictl -> next;
				break;
			}
		}
	}
	free((char *)dictl);

	put_int(SJ3_NormalEnd);
}
close_dictlist(dictl)
DICTL	*dictl;
{
	DICTL	*p;

	while (dictl) {
		unlock_dict((DictFile *)(dictl -> dict), cur_cli -> fd);
		closedict((DictFile *)(dictl -> dict));
		p = dictl -> next;
		free((char *)dictl);
		dictl = p;
	}
}

exec_openstdy()
{
	char	filename[PathNameLen];
	char	password[PassWordLen];
	StdyFile *stdy;

	get_nstring(filename, sizeof(filename));
	get_nstring(password, sizeof(password));

	if (cur_cli -> stdy) longjmp(error_ret, SJ3_StdyAlreadyOpened);
	if (make_full_path(filename)) longjmp(error_ret, SJ3_TooLongParameter);

	if (!(stdy = openstdy(filename, password)))
		longjmp(error_ret, serv_errno);
	cur_cli -> stdy = stdy;

	put_int(SJ3_NormalEnd);
}
exec_closestdy()
{
	if (!cur_cli -> stdy) longjmp(error_ret, SJ3_StdyFileNotOpened);

	closestdy(cur_cli -> stdy);
	cur_cli -> stdy = NULL;

	put_int(SJ3_NormalEnd);
}

exec_stdysize()
{
	put_int(SJ3_NormalEnd);
	put_int(sizeof(STDYOUT));
}

static	void	exec_lock()
{
	int	lock;
	DICTL	*dl;

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);

	lock = cur_cli -> work -> lock;
	if (lock != NotLocked && lock != cur_cli -> fd)
		longjmp(error_ret, SJ3_LockedByOther);
	cur_cli -> work -> lock = cur_cli -> fd;

	for (dl = dictlist ; dl ; dl = dl -> next) {
		if (dl -> dict -> maxunit == 0) continue;
		lock_dict((DictFile *)(dl -> dict), cur_cli -> fd);
	}

	put_int(SJ3_NormalEnd);
}
static	void	exec_unlock()
{
	int	lock;
	DICTL	*dl;

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);

	lock = cur_cli -> work -> lock;
	if (lock == NotLocked) longjmp(error_ret, SJ3_NotLocked);
	if (lock != cur_cli -> fd) longjmp(error_ret, SJ3_LockedByOther);
	cur_cli -> work -> lock = NotLocked;

	for (dl = dictlist ; dl ; dl = dl -> next) {
		if (dl -> dict -> maxunit == 0) continue;
		unlock_dict((DictFile *)(dl -> dict), cur_cli -> fd);
	}

	put_int(SJ3_NormalEnd);
}
static	void	lock_check_for_read()
{
	int	lock;

	lock = cur_cli -> work -> lock;
	if (lock == NotLocked)
#ifdef	SEVERE_LOCK
		longjmp(error_ret, SJ3_NotLocked);
#else
		;
#endif
	else if (lock != cur_cli -> fd)
		longjmp(error_ret, SJ3_LockedByOther);
}

exec_ph2knj()
{
	int	i;
	Uchar	*p;

	i = get_nstring(buf1, sizeof(buf1));

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	if (i) longjmp(error_ret, SJ3_TooLongParameter);
	lock_check_for_read();

	i = ph2knj(buf1, kbuf, sizeof(kbuf));

	put_int(SJ3_NormalEnd);
	put_int(i);
	p = kbuf;
	while (*p) {
		put_byte(*p); p++;
		p = put_ndata(p, sizeof(STDYOUT));
		p = put_string(p);
	}
	put_byte(0);
}

exec_cl2knj()
{
	int	len;
	int	i;
	Uchar	*ptr;

	len = get_int();
	i = get_nstring(buf1, sizeof(buf1));

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	if (i) longjmp(error_ret, SJ3_TooLongParameter);
	lock_check_for_read();

	i = cl2knj(buf1, len, kbuf);

	put_int(SJ3_NormalEnd);
	put_int(i);
	ptr = put_ndata(kbuf, sizeof(STDYOUT));
	put_string(ptr);
}

exec_nextcl()
{
	int	mode;
	int	i;
	Uchar	*ptr;

	mode = get_int();

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	lock_check_for_read();

	i = nextcl(kbuf, mode);

	put_int(SJ3_NormalEnd);
	put_int(i);
	ptr = put_ndata(kbuf, sizeof(STDYOUT));
	put_string(ptr);
}

exec_prevcl()
{
	int	mode;
	int	i;
	Uchar	*ptr;

	mode = get_int();

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	lock_check_for_read();

	i = prevcl(kbuf, mode);

	put_int(SJ3_NormalEnd);
	put_int(i);
	ptr = put_ndata(kbuf, sizeof(STDYOUT));
	put_string(ptr);
}

exec_cl2knj_cnt()
{
	int	len;
	int	i;
	int	cnt = 0;

	len = get_int();
	i = get_nstring(buf1, sizeof(buf1));

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	if (i) longjmp(error_ret, SJ3_TooLongParameter);
	lock_check_for_read();

	i = cl2knj(buf1, len, kbuf);
	if (i) {
		while (i == len) {
			cnt++;
			i = nextcl(kbuf, 0);
		}
	}

	put_int(SJ3_NormalEnd);
	put_int(cnt);
}

exec_cl2knj_all()
{
	int	len;
	int	i;
	Uchar	*p;

	len = get_int();
	i = get_nstring(buf1, sizeof(buf1));

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	if (i) longjmp(error_ret, SJ3_TooLongParameter);
	lock_check_for_read();

	put_int(SJ3_NormalEnd);

	i = cl2knj(buf1, len, kbuf);
	if (i) {
		while (i == len) {
			put_int(i);
			p = put_ndata(kbuf, sizeof(STDYOUT));
			put_string(p);

			i = nextcl(kbuf, 0);
		}
	}
	put_int(0);
}

exec_study()
{
	register int	err;
	STDYOUT	stdy;

	get_ndata(&stdy, sizeof(stdy));

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	if (!stdy_base) longjmp(error_ret, SJ3_StdyFileNotOpened);
	lock_check_for_read();

	err = study(&stdy);

	switch (err) {
	case StudyNoDict:	err = SJ3_StdyFileNotOpened; break;
	case StudyNormEnd:
	case StudyFullArea:
	case StudyNotStudy:	err = SJ3_NormalEnd; break;
	case StudyError:
	default:		err = SJ3_InternalError; break;
	}
	if (err != SJ3_NormalEnd) longjmp(error_ret, err);
	put_int(err);
}

exec_clstudy()
{
	register int	err;
	STDYOUT	stdy;

	err  = get_nstring(buf1, sizeof(buf1));
	err |= get_nstring(buf2, sizeof(buf2));
	get_ndata(&stdy, sizeof(stdy));

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	if (!stdy_base) longjmp(error_ret, SJ3_StdyFileNotOpened);
	if (err) longjmp(error_ret, SJ3_TooLongParameter);
	lock_check_for_read();

	err = clstudy(buf1, buf2, &stdy);

	switch (err) {
	case ClstudyNoDict:     err = SJ3_StdyFileNotOpened; break;
	case ClstudyNormEnd:	err = SJ3_NormalEnd; break;
	case ClstudyParaErr:
	case ClstudyYomiErr:	err = SJ3_IllegalParameter; break;
	case ClstudyNoArea:	err = SJ3_TooSmallStdyArea; break;
	default:		err = SJ3_InternalError; break;
	}
	if (err != SJ3_NormalEnd) longjmp(error_ret, err);
	put_int(err);
}

exec_adddict()
{
	TypeDicID	dicid;
	TypeGram	gram;
	register int	err;
	int		err1;
	DICTL		*dl;

	dicid = get_int();
	err  = get_nstring(buf1, sizeof(buf1));
	err1 = get_nstring(buf2, sizeof(buf2));
	gram = get_int();

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	if (err) longjmp(error_ret, SJ3_BadYomiString);
	if (err1) longjmp(error_ret, SJ3_BadKanjiString);

	for (dl = dictlist ; dl ; dl = dl -> next)
		if (dl -> dict -> dicid == dicid) break;
	if (!dl) longjmp(error_ret, SJ3_NoSuchDict);
	if (dl -> dict -> maxunit == 0)
		longjmp(error_ret, SJ3_ReadOnlyDict);
	if (is_dict_locked((DictFile *)(dl -> dict)))
		longjmp(error_ret, SJ3_DictLocked);

	debug_out(2, "adddic %s:%s:%d ", buf1, buf2, gram);
	seldict(dicid);
	err = adddic(buf1, buf2, gram);

	if (err & AD_NotUserDict)	err = SJ3_ReadOnlyDict;
	else if (err & AD_BadYomi)	err = SJ3_BadYomiString;
	else if (err & AD_BadKanji)	err = SJ3_BadKanjiString;
	else if (err & AD_BadHinsi)	err = SJ3_BadHinsiCode;
	else if (err & AD_ArExist)	err = SJ3_AlreadyExistWord;
	else if (err & AD_OvflwDouBlk)	err = SJ3_NoMoreDouonWord;
	else if (err & AD_OvflwDouNum)	err = SJ3_NoMoreDouonWord;
	else if (err & AD_OvflwUsrDic)	err = SJ3_NoMoreUserDict;
	else if (err & AD_OvflwIndex)	err = SJ3_NoMoreIndexBlock;
	else if (err)			err = SJ3_AddDictFailed;
	else				err = SJ3_NormalEnd;
	if (err != SJ3_NormalEnd) longjmp(error_ret, err);
	put_int(err);
}
exec_deldict()
{
	TypeDicID	dicid;
	TypeGram	gram;
	register int	err;
	int		err1;
	DICTL		*dl;

	dicid = get_int();
	err  = get_nstring(buf1, sizeof(buf1));
	err1 = get_nstring(buf2, sizeof(buf2));
	gram = get_int();

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	if (err) longjmp(error_ret, SJ3_BadYomiString);
	if (err1) longjmp(error_ret, SJ3_BadKanjiString);

	for (dl = dictlist ; dl ; dl = dl -> next)
		if (dl -> dict -> dicid == dicid) break;
	if (!dl) longjmp(error_ret, SJ3_NoSuchDict);
	if (dl -> dict -> maxunit == 0)
		longjmp(error_ret, SJ3_ReadOnlyDict);
	if (is_dict_locked((DictFile *)(dl -> dict)))
		longjmp(error_ret, SJ3_DictLocked);

	seldict(dicid);
	err = deldic(buf1, buf2, gram);

	if (err & AD_NotUserDict)	err = SJ3_ReadOnlyDict;
	else if (err & AD_BadYomi)	err = SJ3_BadYomiString;
	else if (err & AD_BadKanji)	err = SJ3_BadKanjiString;
	else if (err & AD_BadHinsi)	err = SJ3_BadHinsiCode;
	else if (err & AD_NoMidasi)	err = SJ3_NoSuchWord;
	else if (err & AD_NoHinsi)	err = SJ3_NoSuchWord;
	else if (err & AD_NoKanji)	err = SJ3_NoSuchWord;
	else if (err)			err = SJ3_DelDictFailed;
	else				err = SJ3_NormalEnd;
	if (err != SJ3_NormalEnd) longjmp(error_ret, err);
	put_int(err);
}

exec_getdict()
{
	TypeDicID	dicid;
	int		err;
	DICTL		*dl;
	Uchar		*p;

	dicid = get_int();

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);

	for (dl = dictlist ; dl ; dl = dl -> next)
		if (dl -> dict -> dicid == dicid) break;
	if (!dl) longjmp(error_ret, SJ3_NoSuchDict);
	lock_check_for_read();

	seldict(dicid);
	err = getusr(kbuf);

	if (err) {
		put_int(SJ3_NormalEnd);
		p = put_string(kbuf);
		p = put_string(p);
		put_int(*p);
	}
	else
		longjmp(error_ret, SJ3_NoMoreDictData);
}
exec_nextdict()
{
	TypeDicID	dicid;
	int		err;
	DICTL		*dl;
	Uchar		*p;

	dicid = get_int();

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);

	for (dl = dictlist ; dl ; dl = dl -> next)
		if (dl -> dict -> dicid == dicid) break;
	if (!dl) longjmp(error_ret, SJ3_NoSuchDict);
	lock_check_for_read();

	seldict(dicid);
	err = nextusr(kbuf);

	if (err) {
		put_int(SJ3_NormalEnd);
		p = put_string(kbuf);
		p = put_string(p);
		put_int(*p);
	}
	else
		longjmp(error_ret, SJ3_NoMoreDictData);
}
exec_prevdict()
{
	TypeDicID	dicid;
	int		err;
	DICTL		*dl;
	Uchar		*p;

	dicid = get_int();

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);

	for (dl = dictlist ; dl ; dl = dl -> next)
		if (dl -> dict -> dicid == dicid) break;
	if (!dl) longjmp(error_ret, SJ3_NoSuchDict);
	lock_check_for_read();

	seldict(dicid);
	err = prevusr(kbuf);

	if (err) {
		put_int(SJ3_NormalEnd);
		p = put_string(kbuf);
		p = put_string(p);
		put_int(*p);
	}
	else
		longjmp(error_ret, SJ3_NoMoreDictData);
}

exec_makedict()
{
	char	path[PathNameLen];
	int	err;
	int	ofslen;
	int	idxlen;
	int	seglen;
	int	segnum;

	err = get_nstring(path, sizeof(path));
	idxlen = get_int();
	seglen = get_int();
	segnum = get_int();
	if (err) longjmp(error_ret, SJ3_TooLongParameter);
	if (make_full_path(path)) longjmp(error_ret, SJ3_TooLongParameter);

	err = makedict(path, idxlen, seglen, segnum);
	if (err != SJ3_NormalEnd) longjmp(error_ret, err);
	chmod(path, (file_mode & MODE_MASK));
	put_int(err);
}

exec_makestdy()
{
	char	path[PathNameLen];
	int	err;
	int	stynum;
	int	clstep;
	int	cllen;

	err = get_nstring(path, sizeof(path));
	stynum = get_int();
	clstep = get_int();
	cllen = get_int();
	if (err) longjmp(error_ret, SJ3_TooLongParameter);
	if (make_full_path(path)) longjmp(error_ret, SJ3_TooLongParameter);

	err = makestdy(path, stynum, clstep, cllen);
	if (err != SJ3_NormalEnd) longjmp(error_ret, err);
	chmod(path, (file_mode & MODE_MASK));
	put_int(err);
}

exec_access()
{
	char	path[PathNameLen];
	int	mode;
	int	i;

	i = get_nstring(path, sizeof(path));
	mode = get_int();
	if (i) longjmp(error_ret, SJ3_TooLongParameter);
	if (make_full_path(path)) longjmp(error_ret, SJ3_TooLongParameter);

	put_int(access(path, mode));
}

exec_makedir()
{
	char	path[PathNameLen];
	int	i;

	i = get_nstring(path, sizeof(path));
	if (i) longjmp(error_ret, SJ3_TooLongParameter);
	if (make_full_path(path)) longjmp(error_ret, SJ3_TooLongParameter);

	if (mkdir(path, dir_mode)) {
		if (errno == EEXIST)
			longjmp(error_ret, SJ3_DirAlreadyExist);
		else
			longjmp(error_ret, SJ3_CannotCreateDir);
	}
	chmod(path, (dir_mode & MODE_MASK));

	put_int(SJ3_NormalEnd);
}

exec_who()
{
	register int	i;
	register Client	*cli;

	put_int(client_num);
	for (i = 0, cli = client ; i < client_num ; i++, cli++) {
		put_int(cli -> fd);
		put_string(cli -> host);
		put_string(cli -> user);
		put_string(cli -> prog);
	}
}

exec_kill()
{
	put_int(SJ3_NormalEnd);
	put_flush();

	server_terminate();
}
exec_quit()
{
	if (client_num > 1) longjmp(error_ret, SJ3_UserConnected);
	exec_kill();
}

exec_version()
{
	extern	char	*version_number, *time_stamp;
	static	char	*Version = "version : ";
	static	char	*TimeStamp = "times-stamp : ";

	put_int(SJ3_NormalEnd);
	put_ndata(Version, strlen(Version));
	put_string(version_number);
	put_ndata(TimeStamp, strlen(TimeStamp));
	put_string(time_stamp);
	put_string(NULL);
}

exec_dictpass()
{
	TypeDicID	dicid;
	char		buf[PasswdLen + 1];
	register int	err;
	DICTL		*dl;

	dicid = get_int();
	err  = get_nstring(buf, sizeof(buf));

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	if (err) longjmp(error_ret, SJ3_TooLongPasswd);

	for (dl = dictlist ; dl ; dl = dl -> next)
		if (dl -> dict -> dicid == dicid) break;
	if (!dl) longjmp(error_ret, SJ3_NoSuchDict);

	if (set_dictpass(dl -> dict, buf)) longjmp(error_ret, serv_errno);
	put_int(SJ3_NormalEnd);
}
exec_dictcmnt()
{
	TypeDicID	dicid;
	char		buf[CommentLength + 1];
	register int	err;
	DICTL		*dl;

	dicid = get_int();
	err  = get_nstring(buf, sizeof(buf));

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	if (err) longjmp(error_ret, SJ3_TooLongComment);

	for (dl = dictlist ; dl ; dl = dl -> next)
		if (dl -> dict -> dicid == dicid) break;
	if (!dl) longjmp(error_ret, SJ3_NoSuchDict);

	if (set_dictcmnt(dl -> dict, buf)) longjmp(error_ret, serv_errno);
	put_int(SJ3_NormalEnd);
}
exec_stdypass()
{
	char		buf[PasswdLen + 1];
	register int	err;

	err  = get_nstring(buf, sizeof(buf));

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	if (!stdy_base) longjmp(error_ret, SJ3_StdyFileNotOpened);
	if (err) longjmp(error_ret, SJ3_TooLongPasswd);

	if (set_stdypass(buf)) longjmp(error_ret, serv_errno);
	put_int(SJ3_NormalEnd);
}
exec_stdycmnt()
{
	char		buf[CommentLength + 1];
	register int	err;

	err  = get_nstring(buf, sizeof(buf));

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	if (!stdy_base) longjmp(error_ret, SJ3_StdyFileNotOpened);
	if (err) longjmp(error_ret, SJ3_TooLongComment);

	if (set_stdycmnt(buf)) longjmp(error_ret, serv_errno);
	put_int(SJ3_NormalEnd);
}

exec_stdypara()
{
	int	stynum, clstep, cllen;

	if (!work_base) longjmp(error_ret, SJ3_NotConnected);
	if (!stdy_base) longjmp(error_ret, SJ3_StdyFileNotOpened);

	get_stdysize(&stynum, &clstep, &cllen);
	put_int(SJ3_NormalEnd);
	put_int(stynum);
	put_int(clstep);
	put_int(cllen);
}

void	execute_cmd()
{
	int	i;

	if (i = setjmp(error_ret)) {
		debug_out(1, "%d: error code = %d\r\n", client_fd, i);
		put_int(i);
	}
	else {
		cur_cli = &client[cur_client];
		work_base = cur_cli->work ? &(cur_cli->work->global) : NULL;
		stdy_base = cur_cli->stdy ? &(cur_cli->stdy->stdy) : NULL;

#define	CASE(X)	case (X): debug_out(1, "%d: X\t", client_fd);
		switch (i = get_int()) {

		CASE(SJ3_CONNECT)	exec_connect(); break;
		CASE(SJ3_DISCONNECT)	exec_disconnect(); break;

		CASE(SJ3_OPENDICT)	exec_opendict(); break;
		CASE(SJ3_CLOSEDICT)	exec_closedict(); break;

		CASE(SJ3_OPENSTDY)	exec_openstdy(); break;
		CASE(SJ3_CLOSESTDY)	exec_closestdy(); break;
		CASE(SJ3_STDYSIZE)	exec_stdysize(); break;

		CASE(SJ3_LOCK)		exec_lock(); break;
		CASE(SJ3_UNLOCK)	exec_unlock(); break;

		CASE(SJ3_PH2KNJ)	exec_ph2knj(); break;

		CASE(SJ3_CL2KNJ)	exec_cl2knj(); break;
		CASE(SJ3_NEXTCL)	exec_nextcl(); break;
		CASE(SJ3_PREVCL)	exec_prevcl(); break;
		CASE(SJ3_CL2KNJ_CNT)	exec_cl2knj_cnt(); break;
		CASE(SJ3_CL2KNJ_ALL)	exec_cl2knj_all(); break;

		CASE(SJ3_STUDY)		exec_study(); break;
		CASE(SJ3_CLSTUDY)	exec_clstudy(); break;

		CASE(SJ3_ADDDICT)	exec_adddict(); break;
		CASE(SJ3_DELDICT)	exec_deldict(); break;

		CASE(SJ3_GETDICT)	exec_getdict(); break;
		CASE(SJ3_NEXTDICT)	exec_nextdict(); break;
		CASE(SJ3_PREVDICT)	exec_prevdict(); break;

		CASE(SJ3_MAKEDICT)	exec_makedict(); break;
		CASE(SJ3_MAKESTDY)	exec_makestdy(); break;
		CASE(SJ3_ACCESS)	exec_access(); break;
		CASE(SJ3_MAKEDIR)	exec_makedir(); break;

		CASE(SJ3_WHO)		exec_who(); break;
		CASE(SJ3_QUIT)		exec_quit(); break;
		CASE(SJ3_KILL)		exec_kill(); break;
		CASE(SJ3_VERSION)	exec_version(); break;

		CASE(SJ3_DICTPASS)	exec_dictpass(); break;
		CASE(SJ3_DICTCMNT)	exec_dictcmnt(); break;
		CASE(SJ3_STDYPASS)	exec_stdypass(); break;
		CASE(SJ3_STDYCMNT)	exec_stdycmnt(); break;

		default:		warning_out("Illegal command");
					longjmp(error_ret, SJ3_IllegalCommand);
					break;
		}
#undef	CASE
		debug_out(1, "cmd end\r\n");
	}

	put_flush();
}

