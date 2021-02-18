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
 *	sjrc.c
 *	get .sjrc file
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Mon Jul 13 02:27:13 JST 1987
 */
/*
 * $Header: sjrc.c,v 1.1 91/04/29 17:56:10 masaki Exp $ SONY;
 */

#include <locale.h>
#include "common.h"
#include "key.h"

int		vflag = 1;
int		shell_flag = 0;
int		fork_flag = 0;

char		home[LONGLENGTH];
char		user_name[SHORTLENGTH];
char		RCfile[LONGLENGTH];
char		dictpath[LONGLENGTH];
char		mdictpath[LONGLENGTH];
char		term_name[SHORTLENGTH];
char		shellprog[LONGLENGTH];

static char	*init_mode[5] = {
			"halpha",	"zalpha",	"hkata",
			"zkata",	"hira"
		};

static int	mode_value[5] = {
			MODE_HALPHA,	MODE_ZALPHA,	MODE_HKATA,
			MODE_ZKATA,	MODE_ZHIRA
		};

#define GUIDELEN	80

char	guide_sj2[GUIDELEN];
char	guide_buffer[GUIDELEN];
char	guide_unbuffer[GUIDELEN];
char	guide_hira[GUIDELEN];
char	guide_zkata[GUIDELEN];
char	guide_hkata[GUIDELEN];
char	guide_halpha[GUIDELEN];
char	guide_zalpha[GUIDELEN];
char	guide_shift[GUIDELEN];
char	guide_jis[GUIDELEN];
char	guide_kuten[GUIDELEN];
char	guide_etc[GUIDELEN];
char	guide_etcfunc[GUIDELEN];
char	guide_function[GUIDELEN];

static char	*rcfile = ".sjrc";

static int	intr_code = 0x03;
static char	goto_code[LONGLENGTH];
static int	goto_num = 0;
static char	trap_code[LONGLENGTH];
static int	trap_num = 0;
static int	def_muhenkan	= KEY_HALPHA;
static int	def_muedit	= KEY_ZHIRA;
static int	muhenkan_toggle = 1;
static int	bstudy = 1;

/* re initialize	*/
static int	user_euc = 0;

sjrc_init ()
{
	int	user_code;
	char	*s;

	s = setlocale (LC_CTYPE, "");
	if (strcmp (s, "ja_JP.EUC") == 0)
		user_euc = 1;
	else
		user_euc = 0;
	setlocale (LC_CTYPE, "ja_JP.SJIS");
	clear_bsdel ();
	clear_key ();
	clear_Utable ();
	intr_code = 0x03;
	goto_num = 0;
	trap_num = 0;
	def_muhenkan = KEY_HALPHA;
	def_muedit = KEY_ZHIRA;
	muhenkan_toggle = 1;
	bstudy = 1;
	flush_conversion = 0;
}

/* set user name	*/

setusrname ()
{
	struct passwd  *pwd, *getpwnam(), *getpwuid();
	char		*uname, *getlogin ();

        uname = getlogin ();
        if( uname == (char *)NULL || *uname == NULL ) {
		setpwent ();
		if ((pwd = getpwuid(getuid())) == NULL)
			return (FALSE);
		endpwent ();
		uname = pwd -> pw_name;
        }
	else {
		setpwent ();
		if ((pwd = getpwnam(uname)) == NULL)
			return (FALSE);
		endpwent ();
	}
        strcpy (user_name, uname);
	return (TRUE);
}

/* set user's home directory	*/

sethome (s)
char	s[];
{
	struct passwd	*pwd, *getpwnam ();
	char		*uname;

	if (setusrname ())
		return (FALSE);
	uname = user_name;
	setpwent ();
	pwd = getpwnam (uname);
	endpwent ();
	if (vflag > 1) {
		aprintf ("user name is %s\n", uname);
		aprintf ("user id is %d\n", pwd->pw_uid);
		aprintf ("home directory is %s\n", pwd->pw_dir);
	}
	strcpy (s, pwd->pw_dir);
	return (TRUE);
}

gethome (s)
char	*s;
{
	char	*buff, *getenv ();

	if ((buff = getenv ("HOME")) == NULL)
		return (FALSE);
	else
		strcpy (s, buff);
	if (vflag > 1)
		aprintf ("home directory is %s\n\r", s);
	return (TRUE);
}

/* get sjrc file		*/

getsjrc ()
{
	FILE	*fd, *fopen ();

	if (gethome (home)) {
		if (sethome (home)) {
			aprintf ("getlogin: no user name\n");
			exit (1);
		}
	}
	else
		setusrname ();

	strcpy (RCfile, rcfile);
	if ((fd = fopen (RCfile, "r")) != NULL) {
		setrc (RCfile, fd);
		fclose (fd);
		return (TRUE);
	}

	strcpy (RCfile, home);
	strcat (RCfile, "/");
	strcat (RCfile, rcfile);
	if ((fd = fopen (RCfile, "r")) != NULL) {
		setrc (RCfile, fd);
		fclose (fd);
		return (TRUE);
	}

	return (FALSE);
}

/* set rc file		*/

setrc (file, fd)
char	*file;
FILE	*fd;
{
	char		line[MAXLINE];
	register int	w;
	struct wordent	word[WORDN];

	if (vflag > 1)
		aprintf ("sjrc file is %s\n", file);

	sjrc_init ();
	while (fgetline (fd, line, MAXLINE) > 0) {
		w = getword (line, word);
		if (w == 0)
			continue;
		if (vflag > 2)
			aprintf ("\n");
		setword (word, w);
	}
	set_sjRtable ();
	if (vflag > 2)
		print_Utable ();
}

setword (word, w)
struct wordent	word[];
register int	w;
{
	extern int	fork_flag;

	if (mutch (word[0].word_str, "key"))
		set_comkey (word);
	else if (mutch (word[0].word_str, "escape"))
		set_func (word);
	else if (mutch (word[0].word_str, "sjxkey"))
		set_func (word);
	else if (mutch (word[0].word_str, "etckey"))
		set_etckeys (word);
	else if (mutch (word[0].word_str, "intr"))
		set_intr (word);
	else if (mutch (word[0].word_str, "bskey"))
		set_bskey (word);
	else if (mutch (word[0].word_str, "delkey"))
		set_delkey (word);
	else if (mutch (word[0].word_str, "setnormal"))
		set_goto (word);
	else if (mutch (word[0].word_str, "throughnext"))
		set_trap (word);
	else if (mutch (word[0].word_str, "initialmode"))
		set_init_mode (word);
	else if (mutch (word[0].word_str, "helpmode"))
		set_helpmenu (word);
	else if (mutch (word[0].word_str, "defaultcode"))
		set_defcode (word);
	else if (mutch (word[0].word_str, "muhenkanmode"))
		set_muhenkan (word);
	else if (mutch (word[0].word_str, "muhenkaninedit"))
		set_muedit (word);
	else if (mutch (word[0].word_str, "muhenkantoggle"))
		set_m_toggle (word);
	else if (mutch (word[0].word_str, "guide"))
		set_guide (word);
	else if ((!fork_flag) && mutch (word[0].word_str, "forkshell"))
		set_forkshell (word);
	else if ((!fork_flag) && mutch (word[0].word_str, "execshell"))
		set_forkshell (word);
	else if (mutch (word[0].word_str, "dictionary"))
		set_dict (word);
	else if (mutch (word[0].word_str, "userdic"))
		set_dict (word);
	else if (mutch (word[0].word_str, "maindic"))
		set_mdict (word);
	else if (mutch (word[0].word_str, "romaji"))
		set_romaji (word);
	else if (mutch (word[0].word_str, "bstudy"))
		set_bstudy (word);
	else if (mutch (word[0].word_str, "silent"))
		set_silent (word);
	else if (mutch (word[0].word_str, "flushafterconversion"))
		set_flush_conversion (word);
}

/* string mutch, with lower char	*/

mutch (obj, src)
char	*obj, *src;
{
	char	tmpo[MAXWORD], tmps[MAXWORD];

	/* tolowerstr is in SJ_romaji.c	*/

	tolowerstr (obj, tmpo);
	tolowerstr (src, tmps);
	return (Mutch (tmpo, tmps));
}

Mutch (obj, src)
register char	*obj, *src;
{
	register char	c;

	while ((c = *(src ++)) != '\0') {
		if (*(obj ++) != c)
			return (0);
	}
	return (1);
}

/* getword returns a number of words	*/

getword (s, word)
register u_char	*s;
struct wordent	word[];
{
	extern char	term_name[];
	register u_char	c;
	register int	i, wcount;
	register int	kanji;
	int		terminal;

	if (IsDelimitor (*s)) {
		s ++;
		terminal = 0;
	}
	else
		terminal = 1;
	kanji = 0;
	i = wcount = 0;
	while ((c = *(s ++)) != '\0') {
		if (kanji == 2)
			kanji = 0;
		else if (kanji == 1)
			kanji ++;
		if ((kanji == 0) && iskanji (c))
			kanji = 1;
		if ((!kanji) && isTerminator (c)) {
			word[wcount].word_str[i] = '\0';
			if (vflag > 2 && word[wcount].word_str[0] != '\0')
				aprintf (" \"%s\"", word[wcount].word_str);
			break;
		}
		if ((!kanji) && IsDelimitor (c)) {
			if (i != 0) {
				word[wcount].word_str[i] = '\0';
				if (wcount == 0 && terminal) {
					if (!mutch
					    (word[0].word_str, term_name)) {
						word[0].word_str[0] = '\0';
						break;
					}
					terminal = 0;
					if (vflag > 2)
						aprintf (" \"%s\"",
						word[wcount].word_str);
				}
				else {
					if (vflag > 2)
						aprintf (" \"%s\"",
						word[wcount].word_str);
					wcount ++;
				}
				i = 0;
				if (wcount >= WORDN - 1)
					break;
			}
			continue;
		}
		if ((!kanji) && IsEscape (c))
			c = *(s ++);
		if (IsTerminator (c)) {
			word[wcount].word_str[i] = '\0';
			if (vflag > 2 && word[wcount].word_str[0] != '\0')
				aprintf (" \"%s\"", word[wcount].word_str);
			break;
		}
		if (i < MAXWORD - 1) {
			word[wcount].word_str[i++] = c;
		}
	}
	if (word[wcount].word_str[0] != '\0') {
		wcount ++;
		word[wcount].word_str[0] = '\0';
	}
	return (wcount);
}

IsTerminator (c)
u_char	c;
{
	if (c == '\n')
		return (1);
	else
		return (0);
}

isTerminator (c)
u_char	c;
{
	if (c == '#')
		return (1);
	else
		return (0);
}

IsEscape (c)
u_char	c;
{
	if (c == '\\')
		return (1);
	else
		return (0);
}

IsDelimitor (c)
u_char	c;
{
	if (c == ' ' || c == '\t' || c == '.')
		return (1);
	else
		return (0);
}

/* fgetline returns a number of input characters	*/

fgetline (fd, s, lim)
FILE		*fd;
register u_char	*s;
register int	lim;
{
	register int	c, i;

	i = 0;
	if (user_euc) {
		while (--lim > 0 && (c = getc (fd)) != EOF && c != '\n') {
			if (iseuc (c)) {
				int	cc;

				cc = c << 8;
				cc |= getc (fd);
				cc = euc2sjis (cc);
				s[i++] = (cc >> 8) & 0xff;
				s[i++] = cc & 0xff;
			}
			else if (iseuckana (c)) {
				s[i++] = getc (fd);
			}
			else {
				s[i++] = c;
			}
		}
	}
	else {
		while (--lim > 0 && (c = getc (fd)) != EOF && c != '\n')
			s[i++] = c;
	}
	if (c == '\n')
		s[i++] = c;
	s[i] = '\0';
	return (i);
}

set_forkshell (word)
struct wordent	word[];
{
	if (word[1].word_str[0] != '\0') {
		strcpy (shellprog, word[1].word_str);
		shell_flag++;
	}
}

set_intr (word)
struct wordent	word[];
{
	int	c;

	if (word[1].word_str[0] != '\0')
		if ((c = eval_key (word[1].word_str)) != -1)
			intr_code = c;
}

set_bskey (word)
struct wordent	word[];
{
	int	c;

	if (word[1].word_str[0] != '\0')
		if ((c = eval_key (word[1].word_str)) != -1)
			set_bs (c);
}

set_delkey (word)
struct wordent	word[];
{
	int	c;

	if (word[1].word_str[0] != '\0')
		if ((c = eval_key (word[1].word_str)) != -1)
			set_del (c);
}

isintr (c)
u_char	c;
{
	if (c == intr_code)
		return (1);
	else
		return (0);
}

set_goto (word)
struct wordent	word[];
{
	int	c;

	if (word[1].word_str[0] != '\0')
		if ((c = eval_key (word[1].word_str)) != -1)
			goto_code[goto_num ++] = c;
}

IsGoto (c)
register u_char	c;
{
	register int	i;

	for (i = 0 ; i < goto_num ; i ++) {
		if (c == goto_code[i])
			return (1);
	}
	return (0);
}

set_trap (word)
struct wordent	word[];
{
	int	c;

	if (word[1].word_str[0] != '\0')
		if ((c = eval_key (word[1].word_str)) != -1)
			trap_code[trap_num ++] = c;
}

IsTrap (c)
register u_char	c;
{
	register int	i;

	for (i = 0 ; i < trap_num ; i ++) {
		if (c == trap_code[i])
			return (1);
	}
	return (0);
}

mode_key (mode)
register int mode;
{
	switch (mode) {
		case MODE_HALPHA:
			return (KEY_HALPHA);
			break;
		case MODE_ZALPHA:
			return (KEY_ZALPHA);
			break;
		case MODE_HKATA:
			return (KEY_HKATA);
			break;
		case MODE_ZKATA:
			return (KEY_ZKATA);
			break;
		case MODE_ZHIRA:
		default:
			return (KEY_ZHIRA);
			break;
	}
}

int	Direct = 0;

set_init_mode (word)
struct wordent	word[];
{
	register Conversion	*cv;
	register int	i, j;

	cv = GetConversion ();
	j = 1;
	while (*word[j].word_str != '\0') {
		if (mutch (word[j].word_str, "buff"))
			cv->BuffMode = BUFF;
		else if (mutch (word[j].word_str, "unbuff"))
			cv->BuffMode = UNBUFF;
		else if (mutch (word[j].word_str, "direct"))
			Direct = 1;
		else {
			for (i = 0 ; i < 5 ; i ++) {
				if (mutch (word[j].word_str, init_mode[i])) {
					cv->Imode = mode_value[i];
					break;
				}
			}
		}
		j ++;
	}
}

set_helpmenu (word)
struct wordent	word[];
{
	if (mutch (word[1].word_str, "off"))
		HHlevel = Hlevel = 1;
	else if (mutch (word[1].word_str, "on"))
		HHlevel = Hlevel = 2;
}

set_defcode (word)
struct wordent	word[];
{
	extern int	def_code;

	if (mutch (word[1].word_str, "shiftjis"))
		def_code = MODE_SCODE;
	else if (mutch (word[1].word_str, "sjis"))
		def_code = MODE_SCODE;
	else if (mutch (word[1].word_str, "euc"))
		def_code = MODE_ECODE;
	else if (mutch (word[1].word_str, "jis"))
		def_code = MODE_JCODE;
	else if (mutch (word[1].word_str, "kuten"))
		def_code = MODE_KCODE;
}

set_muhenkan (word)
struct wordent	word[];
{
	register int	i;

	for (i = 0 ; i < 5 ; i ++) {
		if (mutch (word[1].word_str, init_mode[i])) {
			def_muhenkan = mode_key (mode_value[i]);
			break;
		}
	}
}

set_muedit (word)
struct wordent	word[];
{
	register int	i;

	for (i = 0 ; i < 5 ; i ++) {
		if (mutch (word[1].word_str, init_mode[i])) {
			def_muedit = mode_key (mode_value[i]);
			break;
		}
	}
}

set_m_toggle (word)
struct wordent	word[];
{
	if (mutch (word[1].word_str, "on"))
		muhenkan_toggle = 1;
	if (mutch (word[1].word_str, "off"))
		muhenkan_toggle = 0;
}

set_silent (word)
struct wordent	word[];
{
	vflag = 0;
}

set_bstudy (word)
struct wordent	word[];
{
	if (mutch (word[1].word_str, "on"))
		bstudy = 1;
	if (mutch (word[1].word_str, "off"))
		bstudy = 0;
}

is_bstudy ()
{
	return (bstudy);
}

eval_muhenkan ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	if (muhenkan_toggle && (KEY_ZHIRA != mode_key (cv->Imode)))
		return (KEY_ZHIRA);
	else
		return (def_muhenkan);
}

value_muhenkan ()
{
	return (def_muedit);
}

set_guide (word)
struct wordent	word[];
{
	extern char	*SJrun, *Mode_buff, *Mode_unbuff,
			*Mode_Zhira, *Mode_Zkata, *Mode_Hkata,
			*Mode_Zalpha, *Mode_Halpha, *Gmode,
			*GEtc, *Getc, *GCshift, *GCjis, *GCkuten;
	register int	i;

	if (*word[2].word_str == '\0') {
		return;
	}
	else if (mutch (word[1].word_str, "sj2")) {
		strcpy (guide_sj2, word[2].word_str);
		SJrun = guide_sj2;
	}
	else if (mutch (word[1].word_str, "buffer")) {
		strcpy (guide_buffer, word[2].word_str);
		Mode_buff = guide_buffer;
	}
	else if (mutch (word[1].word_str, "unbuffer")) {
		strcpy (guide_unbuffer, word[2].word_str);
		Mode_unbuff = guide_unbuffer;
	}
	else if (mutch (word[1].word_str, "hira")) {
		strcpy (guide_hira, word[2].word_str);
		Mode_Zhira = guide_hira;
	}
	else if (mutch (word[1].word_str, "zkata")) {
		strcpy (guide_zkata, word[2].word_str);
		Mode_Zkata = guide_zkata;
	}
	else if (mutch (word[1].word_str, "hkata")) {
		strcpy (guide_hkata, word[2].word_str);
		Mode_Hkata = guide_hkata;
	}
	else if (mutch (word[1].word_str, "halpha")) {
		strcpy (guide_halpha, word[2].word_str);
		Mode_Halpha = guide_halpha;
	}
	else if (mutch (word[1].word_str, "zalpha")) {
		strcpy (guide_zalpha, word[2].word_str);
		Mode_Zalpha = guide_zalpha;
	}
	else if (mutch (word[1].word_str, "shift")) {
		strcpy (guide_shift, word[2].word_str);
		GCshift = guide_shift;
	}
	else if (mutch (word[1].word_str, "jis")) {
		strcpy (guide_jis, word[2].word_str);
		GCjis = guide_jis;
	}
	else if (mutch (word[1].word_str, "kuten")) {
		strcpy (guide_kuten, word[2].word_str);
		GCkuten = guide_kuten;
	}
	else if (mutch (word[1].word_str, "etc")) {
		strcpy (guide_etc, word[2].word_str);
		GEtc = guide_etc;
	}
	else if (mutch (word[1].word_str, "funcetc")) {
		i = 2;
		*guide_etcfunc = '\0';
		while (*word[i].word_str != '\0') {
			if (i == 2)
				strcat (guide_etcfunc, "  ");
			else
				strcat (guide_etcfunc, " ");
			strcat (guide_etcfunc, word[i].word_str);
			i ++;
		}
		Getc = guide_etcfunc;
	}
	else if (mutch (word[1].word_str, "function")) {
		i = 2;
		*guide_function = '\0';
		while (*word[i].word_str != '\0') {
			if (i == 2)
				strcat (guide_function, "  ");
			else
				strcat (guide_function, " ");
			strcat (guide_function, word[i].word_str);
			i ++;
		}
		Gmode = guide_function;
	}
}

set_dict (word)
struct wordent	word[];
{
	if (word[1].word_str[0] != '\0')
		strcpy (dictpath, word[1].word_str);
}

set_mdict (word)
struct wordent	word[];
{
	if (word[1].word_str[0] != '\0')
		strcpy (mdictpath, word[1].word_str);
}

set_romaji (word)
struct wordent	word[];
{
	if (word[1].word_str[0] != '\0')
	   if (set_Utable (word[1].word_str, word[2].word_str))
		aprintf ("can't alloc romaji area for %s %s\n\r",
		word[1].word_str, word[2].word_str);
}

set_flush_conversion (word)
struct wordent	word[];
{
	if (mutch (word[1].word_str, "on"))
		flush_conversion = 1;
	if (mutch (word[1].word_str, "off"))
		flush_conversion = 0;
}
