#ifdef sccs
static char     sccsid[] = "@(#)shinbun.c	1.13 91/03/20";
#endif
/*
****************************************************************************

              Copyright 1991, by Sun Microsystems, Inc.

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Sun Microsystems, Inc.
not be used in advertising or publicity pertaining to distribution of
the software without specific, written prior permission.  
Sun Microsystems, Inc. makes no representations about the suitability
of this software for any purpose.  It is provided "as is" without 
express or implied warranty.

SUN MICROSYSTEMS, INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL  SUN MICROSYSTEMS, INC. BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Auther: Hiroshi Watanabe	(nabe@japan.sun.com) Sun Microsystems, Inc.
	Tomonori Shioda		(shioda@japan.sun.com) Sun Microsystems, Inc.

****************************************************************************
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "nntp.h"
#include "conf.h"
#include "clientlib.h"
#include "shinbun.h"
#include "config.h"

#ifdef EUCNEWS
#include <jcode.h>
#endif

extern char            gecos[];
extern NG_INFO         group_info;

cancel_article(gname, anum)
	char           *gname, *anum;
{
	HD_INFO        *hd;
	int             i;
	FILE           *fp;
	char            fname[30];
	char           *buf;
	int		tmp;

	tmp = atoi(anum);
	hd = nntp_get_header_info(gname, tmp);
	if (hd->status == -1) {
		return -1;
	}
	buf = get_mail_address();
	i = 0;
	while (buf[i] != '(' && buf[i] != '\0') {
		i++;
	}
	buf[i] = '\0';
	if (strstr(hd->from, buf) == NULL) {
		return -1;
	}
	strcpy(fname, mktemp("/tmp/NRcancel.XXXXXX"));
	if ((fp = fopen(fname, "w")) == NULL) {
		return -1;
	}
	if (*(hd->from) == '\0') {
		fclose(fp);
		unlink(fname);
		return -1;
	}
	fprintf(fp, "From: %s\n", hd->from);
	if (*(hd->newsgroup) == '\0') {
		fclose(fp);
		unlink(fname);
		return -1;
	}
	fprintf(fp, "Newsgroups: %s\n", hd->newsgroup);
	if (*(hd->distribution) == '\0') {
		fclose(fp);
		unlink(fname);
		return -1;
	}
	fprintf(fp, "Distribution: %s\n", hd->distribution);
	if (*(hd->message_id) == '\0') {
		fclose(fp);
		unlink(fname);
		return -1;
	}
	fprintf(fp, "Subject: cmsg cancel %s\n", hd->message_id);
	fprintf(fp, "Organization: %s\n", ORGANIZATION);
	fclose(fp);
	i = nntp_post(fname, "");
	unlink(fname);
	change_anum_ascancel(gname, anum, 1);
	return i;
}

nntp_post(file, sign)
	char           *file, *sign;
{
	char            ser_line[MAXSTRLEN];
	char            buf[MAXSTRLEN];
	char            buf1[MAXSTRLEN];
	char            sig_file[MAXSTRLEN];
	int             xx = 0;
	FILE           *fp;

	if (strncmp(sign, ".signature", 10) == 0) {
		sprintf(sig_file, "%s/%s", getenv("HOME"), sign);
	} else {
		strncpy(sig_file, sign, 255);
	}

	if ((fp = fopen(file, "r")) == NULL) {
		fprintf(stderr, "Cannot Open file %s\n", file);
		return -1;
	}
	put_nntpserver("POST");
	(void) get_server(ser_line, sizeof(ser_line));
	if (atoi(ser_line) != 340) {
#ifdef DEBUG
		fprintf(stderr, "%s\n", ser_line);
#endif				/* DEBBUG */
		return -1;
	}
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		while (buf[xx] != '\n' && buf[xx] != '\0') {
			xx++;
		}
		buf[xx] = '\0';
		xx = 0;

		if (buf[0] == '.') {
			sprintf(buf1, ".%s", buf);
#ifdef EUCNEWS
			ujtojis(buf, buf1);
#else
			strncpy(buf, buf1, MAXSTRLEN);
#endif
			put_nntpserver(buf);
		} else {
#ifdef EUCNEWS
			ujtojis(buf1, buf);
#else
			strncpy(buf, buf1, MAXSTRLEN);
#endif
			put_nntpserver(buf1);
		}
	}
	fclose(fp);
	xx = 0;
	if ((fp = fopen(sig_file, "r")) != NULL) {
		while (fgets(buf, sizeof(buf), fp) != NULL) {
			while (buf[xx] != '\n' && buf[xx] != '\0') {
				xx++;
			}
			buf[xx] = '\0';
			xx = 0;
			if (buf[0] == '.') {
				sprintf(buf1, ".%s", buf);
				put_nntpserver(buf1);
			} else {
				put_nntpserver(buf);
			}
		}
		fclose(fp);
	}
	put_nntpserver("\n");
	put_nntpserver(".");
	(void) get_server(ser_line, sizeof(ser_line));
	if (atoi(ser_line) != 240) {
#ifdef DEBUG
		fprintf(stderr, "%s\n", ser_line);
#endif				/* DEBUG */
		return -1;
	}
	return 0;
}

char           *
get_mail_address()
{
	static char     m_address[MAXSTRLEN];
	char            hostname[64];
	int             i;

	hostname[0] = '\0';
	gethostname(hostname, 64);
	sprintf(m_address, "%s@%s.%s \(%s\)"
		,getlogin(), hostname, MAILADDRESS, gecos);

	return m_address;
}

char           *
get_mail_path()
{
	char            m_path[MAXSTRLEN];
	char            hostname[100];
	int             i;

	gethostname(hostname, i);
	sprintf(m_path, "%s!%s", hostname, getlogin());

	return m_path;
}

HD_INFO        *
nntp_get_header_info(newsgroup, newsno)
	char           *newsgroup;
	int		newsno;
{
	int             i = 0, j = 0;
	char            ser_line[NNTP_STRLEN];
	static HD_INFO  hd_info;
	char            command[50];
	char            buf[MAXSTRLEN];

	hd_info.from[0] = '\0';
	hd_info.newsgroup[0] = '\0';
	hd_info.subject[0] = '\0';
	hd_info.reply_to[0] = '\0';
	hd_info.followup_to[0] = '\0';
	hd_info.message_id[0] = '\0';
	hd_info.distribution[0] = '\0';
	hd_info.reference[0] = '\0';
	hd_info.status = 0;

	if (newsno <= 0 && newsgroup) {
		strcpy(hd_info.newsgroup, newsgroup);
		return &hd_info;
	}
	if (newsno <= 0 || newsgroup == NULL || strcmp(newsgroup, group_info.groupname)) {
		hd_info.status = -1;
		return &hd_info;
	}
	sprintf(command, "HEAD %d", newsno);

	put_nntpserver(command);
	(void) get_server(ser_line, sizeof(ser_line));
	if (atoi(ser_line) != OK_HEAD) {
		fprintf(stderr, "Couldn't get header.\n");
		hd_info.status = -1;
		return &hd_info;
	}
	while (get_server(ser_line, sizeof(ser_line)) >= 0) {
		if (strcmp(ser_line, ".") == 0) {
			break;
		} else if (strncmp(ser_line, "From", 4) == 0) {
			if (ser_line[5] == ' ')
				while (ser_line[j] != '(' && ser_line[j] != '\0')
					j++;
			ser_line[j] = '\0';
			strcpy(hd_info.from, strchr(ser_line, ' '));
		} else if (strncmp(ser_line, "Newsgroups", 10) == 0) {
			if (ser_line[11] == ' ')
				strcpy(hd_info.newsgroup, strchr(ser_line, ' '));
		} else if (strncmp(ser_line, "Subject", 7) == 0) {
			if (ser_line[8] == ' ') {
				if (strncmp(strchr(ser_line, ' '), " Re: ", 4) == 0) {
					strcpy(hd_info.subject, strchr(ser_line, ' '));
				} else {
					sprintf(hd_info.subject, "Re: %s", strchr(ser_line, ' '));
				}
			}
		} else if (strncmp(ser_line, "Reply-To", 8) == 0) {
			if (ser_line[9] == ' ')
				strcpy(hd_info.reply_to, strchr(ser_line, ' '));
		} else if (strncmp(ser_line, "Followup-To", 11) == 0) {
			if (ser_line[12] == ' ')
				strcpy(hd_info.followup_to, strchr(ser_line, ' '));
		} else if (strncmp(ser_line, "Message-ID", 10) == 0) {
			if (ser_line[11] == ' ')
				strcpy(hd_info.message_id, strchr(ser_line, ' '));
		} else if (strncmp(ser_line, "Distribution", 12) == 0) {
			if (ser_line[13] == ' ')
				strcpy(hd_info.distribution, strchr(ser_line, ' '));
		} else if (strncmp(ser_line, "References", 10) == 0) {
			if (ser_line[11] == ' ')
				strcpy(hd_info.reference, strchr(ser_line, ' '));
		}
	}
	return &hd_info;
}
