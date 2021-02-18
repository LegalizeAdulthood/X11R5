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
#include <ctype.h>
#include <sys/types.h>
#include <locale.h>
#include "sjtool.h"

struct wordent {
	char	word_str[MAXWORD];
	};

extern char	home_dir[];
extern char	term_name[];

char	RCfile[LONGLENGTH];

static char	*rcfile = ".sjrc";

static int	user_euc = 0;
static int	file_code = SYS_SJIS;

int	set_dict(), set_server();

struct functbl {
	char *keyword;
	int (*func)();
} funcs[] = {
	"dictionary",	set_dict,
	"userdic",	set_dict,
	"server",	set_server,
	NULL,		NULL
};

#define TOLOWER(c) (isupper(c) ? tolower(c) : (c))

sjset_code()
{
	char	*s;

	s = setlocale (LC_CTYPE, "");
	if (strcmp (s, "ja_JP.EUC") == 0) {
		user_euc = 1;
		file_code = -1;
	} else
		user_euc = 0;
	setlocale (LC_CTYPE, "ja_JP.SJIS");
}

getsjrc ()
{
	FILE *fd, *fopen();
	register char *p;
	char *getenv();

	sjset_code();
	RCfile[0] = '\0';
	if ((p = getenv("SJRC")) != NULL && *p != '\0') {
		if (*p != '/') {
			strcpy(RCfile, home_dir);
			strcat(RCfile, "/");
		}
		strcat(RCfile, p);
		if ((fd = fopen (RCfile, "r")) != NULL) {
			setrc (RCfile, fd);
			fclose (fd);
			return TRUE;
		}
	}
	if (home_dir[0] != '\0') {
		strcpy(RCfile, home_dir);
		strcat(RCfile, "/");
		strcat(RCfile, rcfile);
		if ((fd = fopen (RCfile, "r")) != NULL) {
			setrc (RCfile, fd);
			fclose (fd);
			return TRUE;
		}
	}
	RCfile[0] = '\0';
	return FALSE;
}

setrc (file, fd)
char	*file;
FILE	*fd;
{
	char		line[MAXLINE];
	register int	w;
	struct wordent word[WORDN];
	register char	*p;
	struct functbl *functp;

	while ((p = fgets (line, MAXLINE, fd)) != NULL) {
		if ((w = getword(line, word)) <= 0)
			continue;
		functp = funcs;
		p = word[0].word_str;
		while (functp->keyword != NULL) {
			if (much(p, functp->keyword)) {
				if (functp->func) (*(functp->func))(word);
				break;;
			}
			functp++;
		}
	}
}

much(s1, s2)
char *s1, *s2;
{
	register char c1, c2;

	if (s1 == NULL)
		return 0;
	while (*s2 != '\0') {
		c1 = *s1++;
		c2 = *s2++;
		if (TOLOWER(c1) != TOLOWER(c2))
			return 0;
	}
	return 1;
}

getword (s, word)
register char	*s;
struct wordent	word[];
{
	register u_char	c, cc;
	register char *p;
	register int	i, wcount;

	if (IsDelimitor(*s)) {
		s++;
	} else {
		p = s;
		while(!IsDelimitor(*s)) {
			if (IsTerminator(*s) || isTerminator(*s))
				break;
			s++;
		}
		*s++ = '\0';
		if (much(p, term_name) == 0)
			return 0;
	}
	i = wcount = 0;
	p = word[wcount].word_str;
	while (*s != '\0') {
		c = *s++;
		if (file_code == SYS_SJIS) {
			if (issjis1(c) && issjis2(*s)) {
				if (i < MAXWORD - 2) {
					*p++ = c;
					*p++ = *s;
					i += 2;
				}
				s++;
				continue;
			}
		} else if (file_code == SYS_EUC) {
			if (iseuc(c) && iseuc(*s)) {
				if (i < MAXWORD - 2) {
					cc = (c << 8) + (*s & 0xff);
					cc = euc2sjis(cc);
					*p++ = (cc >> 8) & 0xff;
					*p++ = cc & 0xff;
					i += 2;
				}
				s++;
				continue;
			}
			if (iseuckana(c) && iskana2(*s))
				c = *s++;
		} else {
			if (issjis1(c) && issjis2(*s)) {
				file_code = SYS_SJIS;
				s--;
				continue;
			}
			if ((iseuc(c) && iseuc(*s)) ||
				(iseuckana(c) && iskana2(*s))) {
				file_code = SYS_EUC;
				s--;
				continue;
			}
		}
		if (isTerminator(c))
			break;
		if (IsDelimitor(c)) {
			if (i > 0) {
				if (++wcount >= WORDN - 1)
					break;
				*p = '\0';
				i = 0;
				p = word[wcount].word_str;
			}
			continue;
		}
		if (IsEscape(c))
			c = *s++;
		if (IsTerminator(c))
			break;
		if (i < MAXWORD - 1) {
			*p++ = c;
			i++;
		}
	}
	*p = '\0';
	if (word[wcount].word_str[0] != '\0')
		word[++wcount].word_str[0] = '\0';
	return wcount;
}

IsTerminator (c)
u_char	c;
{
	return (c == '\n') ? 1 : 0;
}

isTerminator (c)
u_char	c;
{
	return (c == '#') ? 1 : 0;
}

IsEscape (c)
u_char	c;
{
	return (c == '\\') ? 1 : 0;
}

IsDelimitor (c)
u_char	c;
{
	return (c == ' ' || c == '\t' || c == '.') ? 1 : 0;
}

set_dict (word)
struct wordent	word[];
{
	setdicname(word[1].word_str);
}

set_server(word)
struct wordent	word[];
{
	setsjserv(word[1].word_str);
}

