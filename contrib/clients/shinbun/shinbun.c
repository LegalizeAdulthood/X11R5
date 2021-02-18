#ifdef sccs
static char     sccsid[] = "@(#)shinbun.c	1.15 91/09/09";
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
#include "sb_def.h"

#ifdef EUCNEWS
#include <jcode.h>
#endif

int             num_of_ngrp;	/* Number of newsgroup written in .newsrc */
int             num_of_listall;
char           *newsrc[MAXNGRP];/* .newsrc data on memory */
char           *newslist_all[MAXNGRP];	/* All active group on memory */
int             news_fst_a[MAXNGRP];
int             news_lst_a[MAXNGRP];
char            ACTIVE_FLIST_NAM[50];	/* File name store active news list */
char            NEWSRC_FLIST_NAM[MAXSTRLEN];	/* File name of .newsrc */
char            gecos[100];	/* String of GCOS field in /etc/passwd */
int             sb_newsrc_status = 0;
int             internal_mode_status = 0;
NG_INFO         group_info;

int             NUM_INTERNAL_NGRP = 2;
char           *INTERNAL_GRPNAM[] = {"engj", "sun"};
int             sb_display_status = 0;
int             max_line = 300;
char            BRACKET[64];

/*
 * This function only get CGOS field in passwd file
 */
shinbun_init()
{
	int             i;
	FILE           *fp;
	char            command[MAXSTRLEN];

	strcpy(BRACKET, sb_get_props_char_value(Sb_indent_prefix));

	/* Get GCOS field for From field of news and mail */
	sprintf(command, GCOS, getlogin());
	if ((fp = popen(command, "r")) == 0)
		return 0;
	fgets(gecos, 100, fp);
	pclose(fp);
	i = 0;
	while (gecos[i] != '\n')
		i++;
	gecos[i] = '\0';

	return 1;
}

/*
 * Disconnect NNTP connection and delete a temporary file
 */
shinbun_quit()
{
	/* delete temp files for active newsgroup list */
	unlink(ACTIVE_FLIST_NAM);

	/* write current newsgroup data to newsrc (on memory) */
	write_newsrc_data();

	/* Cut NNTP connection with server */
	close_server();
}

/*
 * Internal news mode. This mode only access defined newsgroup for beginners.
 * .newsrc files are copyed to .newsrc-internal
 */
internal_news_mode()
{
	char            command[MAXSTRLEN];
	char            fname1[50];
	char            fname2[50];
	char            fname[MAXSTRLEN];
	int             i;
	FILE           *fp;

	fprintf(stderr, "Internal mode ");

	internal_mode_status = 1;
	sprintf(fname, "%s-internal", NEWSRC_FLIST_NAM);

	strcpy(fname1, mktemp("/tmp/NRinternal1.XXXXXX"));
	for (i = 0; i < NUM_INTERNAL_NGRP; i++) {
		sprintf(command, "/bin/grep \"^%s\" %s >> %s", INTERNAL_GRPNAM[i], ACTIVE_FLIST_NAM, fname1);
		system(command);
		fprintf(stderr, ". ");
	}
	sprintf(command, "/bin/cp %s %s", fname1, ACTIVE_FLIST_NAM);
	system(command);
	fprintf(stderr, ". ");
	unlink(fname1);

	if ((fp = fopen(fname, "r")) != NULL) {
		strcpy(NEWSRC_FLIST_NAM, fname);
		fprintf(stderr, "Done\n");
		fclose(fp);
		return;
	} else {
		strcpy(fname2, mktemp("/tmp/NRinternal1.XXXXXX"));
		for (i = 0; i < NUM_INTERNAL_NGRP; i++) {
			sprintf(command, "/bin/grep \"^%s\" %s >> %s",
			      INTERNAL_GRPNAM[i], NEWSRC_FLIST_NAM, fname2);
			system(command);
			fprintf(stderr, ". ");
		}
	}

	strcpy(NEWSRC_FLIST_NAM, fname);
	sprintf(command, "/bin/cp %s %s", fname2, NEWSRC_FLIST_NAM);
	system(command);
	fprintf(stderr, ". ");

	fprintf(stderr, "Done\n");
	unlink(fname2);
}

/*
 */
ck_ginfo(i)
	int             i;
{
	int             a_num;
	NA_INFO         na;

	na = (NA_INFO) group_info.first;
	if (na == 0) {
		return 0;
	}
	a_num = i;
	while (na->anum != a_num) {
		na = na->next;
		if (na == 0) {
			if (a_num > group_info.last_article) {
				return 0;
			} else {
				return 1;
			}
		}
	}
	if ((na->status & AT_READ) != 0) {
		return 1;
	} else {
		return 0;
	}
}

free_na_info(na)
	NA_INFO         na;
{
	NA_INFO         tmp;

	tmp = na;
	if (tmp->next) {
		free_na_info(tmp->next);
	}
	free(na->subject);
	if (na->sender != NULL) {
		free(na->sender);
		na->sender = NULL;
	}
	if (na->time != NULL) {
		free(na->time);
		na->time = NULL;
	}
	free(na);
	na = NULL;

	if (group_info.read_status) {
		free(group_info.read_status);
		group_info.read_status = NULL;
	}
}


/*
 * Search .newsrc and decide if the article is read/unread.
 */
isread_article(gname, anum)
	char           *gname, *anum;
{
	int             gnum, i = 0;

	/* Search if named news article is already read or not */
	if ((gnum = search_ngrp(gname)) < 0) {
		return (0);
	}
	while (newsrc[gnum][i] != ' ' && newsrc[gnum][i] != '\n' && newsrc[gnum][i] != '\0') {
		i++;
	}

	return (dec_isread(&newsrc[gnum][i], anum));
}

dec_isread(buf, num)
	char           *buf, *num;
{
	int             f_num = 0, l_num = 0, d_num = 0, l = 0, j = 0;
	char            cnum[20];

	d_num = atoi(num);

	do {
		switch (*buf) {
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '0':
			cnum[j++] = *buf;
			break;
		case '-':
			cnum[j] = '\0';
			if (j > 0) {
				f_num = atoi(cnum);
			} else {
				f_num = 0;
			}
			l = 1;
			j = 0;
			break;
		case ',':
		case '\n':
		case '\0':
			cnum[j] = '\0';
			if (j > 0) {
				l_num = atoi(cnum);
			} else {
				l_num = 0;
			}
			j = 0;
			if (l == 1) {
				if (f_num <= d_num && l_num >= d_num)
					return (1);
			} else {
				if (l_num == d_num)
					return (1);
			}
			l = 0;
			break;
		}
	} while (*buf++ != '\0');
	return (-1);
}

/*
 * Search if the groupname is already read on mamory or not.
 */
search_ngrp(gname)
	char           *gname;
{
	int             j = 0, len;
	char            buf[MAXSTRLEN];

	sprintf(buf, "%s:", gname);

	while (j < num_of_ngrp) {
		len = strlen(buf);
		if (strncmp(newsrc[j], buf, len) == 0) {
			return (j);
		}
		j++;
	}
	return (-1);
}

/*
 * Search if the groupname is already read on mamory or not.
 */
search_mkd_ngrp(gname)
	char           *gname;
{
	int             j = 0, len;
	char            buf[MAXSTRLEN];

	sprintf(buf, "%s!", gname);

	while (j < num_of_ngrp) {
		len = strlen(buf);
		if (strncmp(newsrc[j], buf, len) == 0) {
			return (j);
		}
		j++;
	}
	return (-1);
}

/*
 * Make a connection with NNTP server for newsreader.
 */
connect_nntp_server()
{
	int             response;
	char           *server;

	/* Connect with NNTP server */
	server = getserverbyfile(SERVER_FILE);
	if (server == NULL) {
		fprintf(stderr, "Couldn't get name of news server from %s\n",
			SERVER_FILE);
		fprintf(stderr,
			"Either fix this file, or put NNTPSERVER in your environment.\n");
		exit(1);
	}
	response = nntp_server_init(server);
	if (response < 0) {
		fprintf(stderr,
			"Couldn't connect to %s news server. Try again later\n", server);
		exit(1);
	}
	if (handle_server_response(response, server) < 0)
		exit(1);

	/* Download active newsgroup list */
	get_active_list_file();
}

/*
 * Quit the NNTP connection for newsreader.
 */
nntp_quit()
{
	/* quit procedure */
	shinbun_quit();
/*
 *	exit(0);
 */
}

/*
 * Change NNTP status to indicated newsgroup.
 */
char           *
nntp_group(groupname)
	char           *groupname;
{
	char            command[NNTP_STRLEN];
	char            ser_line[NNTP_STRLEN];
	char           *buf;
	int             i, j;

	j = search_ngrp(groupname);
	/* Change current group to <groupname> */
	sprintf(command, "GROUP %s", groupname);

	put_nntpserver(command);
	(void) get_server(ser_line, sizeof(ser_line));
	if (atoi(ser_line) == ERR_GOODBYE) {
		fprintf(stderr,
			"SHINBUN: Server Problem! Good-Bye\n");
		exit(1);
	}
	if (atoi(ser_line) != OK_GROUP) {
		/*
		 * fprintf(stderr, "Couldn't open group %s\n", groupname);
		 */
		return NULL;
	}
	/* Chack first,last article number and amount of articles */
	buf = ser_line;
	/* EMPTY */
	while (*buf++ != ' ');
	/* EMPTY */
	while (*buf++ != ' ');
	group_info.first_article = atoi(buf);
	/* EMPTY */
	while (*buf++ != ' ');
	group_info.last_article = atoi(buf);
	group_info.num_article = group_info.last_article - group_info.first_article + 1;
	group_info.read_status = malloc(group_info.num_article);
	if (group_info.last_article == 0)
		group_info.num_article = 0;

	if (group_info.groupname) {
		free(group_info.groupname);
		group_info.groupname = NULL;
	}
	group_info.groupname = malloc(strlen(groupname) + 1);
	i = strlen(groupname);
	if (groupname[i - 1] == '\n') {
		groupname[i - 1] = '\0';
	}
	strcpy(group_info.groupname, groupname);
	reset_last_article_data(search_nbiffgrp(groupname));
	set_initial_read();
	return groupname;
}

/*
 * Get subjects list of indicated newsgroup. The results are saved in NA_INFO
 * structures.
 */
nntp_get_subject()
{
	char            ser_line[NNTP_STRLEN];
	char           *buf;
	char           *tmp;
	char            anum[20];
	int             k = 0;
	int             nline = 0;
	NA_INFO         at_info, new_at;

	at_info = (NA_INFO) malloc(sizeof(NAX_INFO));
	at_info->time = 0;
	at_info->sender = 0;
	at_info->subject = 0;
	group_info.first = at_info;

	at_info->prev = 0;
	at_info->status = 0;

	while (get_server(ser_line, sizeof(ser_line)) >= 0) {

		at_info->next = 0;
		at_info->status = 0;

		k = 0;
		if (strcmp(ser_line, ".") == 0) {
			if (at_info->prev != 0) {
				at_info->prev->next = 0;
			}
			free(at_info);
			at_info = NULL;
			break;
		}
		while (ser_line[k] != ' ') {
			anum[k] = ser_line[k];
			ser_line[k] = ' ';
			k++;
		}
		anum[k] = '\0';
		if (search_isread(atoi(anum)) == 1) {
			at_info->status = (at_info->status | AT_READ);
		}
		nline++;

		buf = &ser_line[0];
		tmp = malloc(strlen(buf));
		while (*buf == ' ') {
			buf++;
		}
		strcpy(tmp, buf);
		at_info->line = nline;
		at_info->anum = atoi(anum);
		new_at = (NA_INFO) malloc(sizeof(NAX_INFO));
		new_at->time = 0;
		new_at->sender = 0;
		new_at->subject = 0;
		at_info->next = new_at;
		at_info->subject = tmp;
		new_at->prev = at_info;
		new_at->next = 0;
		new_at->status = 0;
		at_info = new_at;
	}
	return nline;
}

/*
 * Get subject lists and add current NA_INFO structures.
 */
nntp_add_subject()
{
	char            ser_line[NNTP_STRLEN];
	char           *buf;
	char           *tmp;
	char            anum[20];
	int             k = 0;
	int             nline = 0;
	NA_INFO         at_info, new_at, tmp_at;

	at_info = (NA_INFO) malloc(sizeof(NAX_INFO));
	at_info->time = 0;
	at_info->sender = 0;
	at_info->subject = 0;
	tmp_at = group_info.first;
	if (group_info.first == 0) {
		group_info.first = at_info;
		at_info->line = 0;
		at_info->prev = 0;
	} else {
		while (tmp_at->next != 0) {
			tmp_at = tmp_at->next;
		}
		tmp_at->next = at_info;
		at_info->prev = tmp_at;
		nline = tmp_at->line;
	}

	at_info->status = 0;

	while (get_server(ser_line, sizeof(ser_line)) >= 0) {

		at_info->next = 0;
		at_info->status = 0;

		k = 0;
		if (strcmp(ser_line, ".") == 0) {
			if (at_info->prev != 0) {
				at_info->prev->next = 0;
			}
			if (at_info == group_info.first) {
				group_info.first = 0;
			}
			free(at_info);
			at_info = NULL;
			break;
		}
		while (ser_line[k] != ' ') {
			anum[k] = ser_line[k];
			ser_line[k] = ' ';
			k++;
		}
		anum[k] = '\0';
		if (search_isread(atoi(anum)) == 1) {
			at_info->status = (at_info->status | AT_READ);
		}
		nline++;

		buf = &ser_line[0];
		tmp = malloc(strlen(buf));
		while (*buf == ' ') {
			buf++;
		}
		strcpy(tmp, buf);
		at_info->line = nline;
		at_info->anum = atoi(anum);
		new_at = (NA_INFO) malloc(sizeof(NAX_INFO));
		new_at->sender = 0;
		new_at->time = 0;
		new_at->subject = 0;
		at_info->next = new_at;
		at_info->subject = tmp;
		new_at->prev = at_info;
		new_at->next = 0;
		new_at->status = 0;
		at_info = new_at;
	}
	return nline;
}

/*
 * Get active newsgroup list
 */
nntp_get_list(fp)
	FILE           *fp;
{
	char            ser_line[NNTP_STRLEN];
	char           *buf;
	int             i = 0, j = 0;

	while (i < MAXNGRP) {
		if (newslist_all[i]) {
			free(newslist_all[i]);
			newslist_all[i] = NULL;
		}
		i++;
	}

	/* Get body of active newsgroup list */
	while (get_server(ser_line, sizeof(ser_line)) >= 0) {
		if (ser_line[0] == '.')
			break;
		buf = strtok(ser_line, " ");
		newslist_all[j] = strdup(buf);
		fprintf(fp, "%s:\n", buf);
		buf = strtok(NULL, " ");
		news_lst_a[j] = atoi(buf);
		buf = strtok(NULL, " ");
		news_fst_a[j] = atoi(buf);
		j++;
	}
	num_of_listall = j;
}

/*
 * Get current article
 */
nntp_get(fp)
	FILE           *fp;
{
	char            ser_line[NNTP_STRLEN];
	char            buf[NNTP_STRLEN];
	int             lineno = 0;

	/* Get body of article */
	while (get_server(ser_line, sizeof(ser_line)) >= 0) {
		if (strcmp(ser_line, ".") == 0)
			break;
#ifdef EUCNEWS
		jistouj(buf, ser_line);
#else
		strcpy(buf, ser_line);
#endif
		fprintf(fp, "%s\n", buf);
		lineno++;
	}
	return lineno;
}

/*
 * Get active newsgroup list
 */
get_active_list_file()
{
	FILE           *fp;
	char            ser_line[NNTP_STRLEN];


	strcpy(ACTIVE_FLIST_NAM, mktemp("/tmp/NRlist.XXXXXX"));
	if ((fp = fopen(ACTIVE_FLIST_NAM, "w")) == NULL) {
		fprintf(stderr, "Can't open work file\n");
		fclose(fp);
		return -1;
	}
	put_nntpserver("LIST");
	(void) get_server(ser_line, sizeof(ser_line));
	if (atoi(ser_line) == ERR_GOODBYE) {
		fprintf(stderr,
			"SHINBUN: Server Problem! Good-Bye\n");
		exit(1);
	}
	if (ser_line[0] != CHAR_OK) {
		fprintf(stderr, "Couldn't get list file\n");
		fprintf(stderr, "%s\n", ser_line);
		return -1;
	}
	nntp_get_list(fp);

	fclose(fp);
	return 0;
}

/*
 * Get all subject list
 */
get_subject_file(group)
	char           *group;
{
	char            ser_line[NNTP_STRLEN];
	int             j;
	char            command[MAXSTRLEN];

	if (sb_newsrc_status == 1) {
		write_newsrc_data();
	}
	if (nntp_group(group) == NULL) {
		sb_newsrc_status = 0;
		return -1;
	}
	sb_newsrc_status = 1;

	j = group_info.last_article - max_line;
	/*
	 * if(j <= 0) { return 0; }
	 */

	if (j < 0)
		j = group_info.first_article;

	sprintf(command, "XHDR subject %d-", j);
	put_nntpserver(command);
	(void) get_server(ser_line, sizeof(ser_line));
	if (atoi(ser_line) == ERR_GOODBYE) {
		fprintf(stderr,
			"SHINBUN: Server Problem! Good-Bye\n");
		exit(1);
	}
	if (atoi(ser_line) != OK_HEAD) {
		fprintf(stderr, "Couldn't list subject\n");
		return -1;
	}
	nntp_get_subject();
	group_info.num_unread = -1;
	if (group_info.num_article == 0) {
		free(group_info.first);
		group_info.first = NULL;
	}
	if (sb_display_status != 2)
		get_sender_file(j);
	if (sb_display_status != 2 && sb_display_status != 1)
		get_time_file(j);
	return (group_info.num_article);
}

/*
 * Get unread subject list.
 */
get_subject_file_unread(group)
	char           *group;
{
	char            ser_line[NNTP_STRLEN];
	char            anum[20], old_num[20];
	char            command[255];
	int             i, j, k;
	int             num_read = 0;

	if (sb_newsrc_status == 1) {
		write_newsrc_data();
	}
	if (nntp_group(group) == NULL) {
		sb_newsrc_status = 0;
		return -1;
	}
	sb_newsrc_status = 1;

	j = search_first_unread(max_line);
	if (j < 0) {
		return 0;
	}
	for (i = j; i <= (group_info.last_article + 1); i++) {
		sprintf(anum, "%d", i);
		if ((k = search_isread(i)) == 1) {
			num_read++;
		} else if (k == 0) {
			sprintf(command, "XHDR subject %d-", j);
			put_nntpserver(command);
			(void) get_server(ser_line, sizeof(ser_line));
			if (atoi(ser_line) == ERR_GOODBYE) {
				fprintf(stderr,
				     "SHINBUN: Server Problem! Good-Bye\n");
				exit(1);
			}
			if (atoi(ser_line) == OK_HEAD) {
				nntp_add_subject();
			}
			i = group_info.last_article + 1;
		} else if (i == (group_info.last_article + 1)) {
			sprintf(command, "XHDR subject %s-", anum);
			put_nntpserver(command);
			(void) get_server(ser_line, sizeof(ser_line));
			if (atoi(ser_line) == ERR_GOODBYE) {
				fprintf(stderr,
				     "SHINBUN: Server Problem! Good-Bye\n");
				exit(1);
			}
			if (atoi(ser_line) == OK_HEAD) {
				nntp_add_subject();
			}
		} else {
			strcpy(old_num, anum);
			while (search_isread(i) == -1 && i <= group_info.last_article) {
				i++;
				sprintf(anum, "%d", i);
			}
			i--;
			sprintf(anum, "%d", i);
			sprintf(command, "XHDR subject %s-%s", old_num, anum);
			put_nntpserver(command);
			(void) get_server(ser_line, sizeof(ser_line));
			if (atoi(ser_line) == ERR_GOODBYE) {
				fprintf(stderr,
				     "SHINBUN: Server Problem! Good-Bye\n");
				exit(1);
			}
			if (atoi(ser_line) == OK_HEAD) {
				nntp_add_subject();
			}
		}
	}
	if (j > group_info.first_article)
		group_info.num_unread = group_info.last_article - num_read - j + 1;
	else
		group_info.num_unread = group_info.num_article - num_read;

	if (group_info.num_unread < 0)
		group_info.num_unread = 0;
	if (group_info.num_unread == 0) {
		free(group_info.first);
		group_info.first = NULL;
	}
	if (sb_display_status != 2)
		get_sender_file_unread(j);
	if (sb_display_status == 0)
		get_time_file_unread(j);
	return (group_info.num_article - num_read);
}

get_view_file(gname, news_no, fname)
	char           *gname, *fname;
	int             news_no;
{
	char            command[NNTP_STRLEN];
	char            ser_line[NNTP_STRLEN];
	FILE           *fp;
	int             lineno;

	if (gname == NULL)
		return -1;

	strcpy(fname, mktemp("/tmp/NRarticle.XXXXXX"));
	if ((fp = fopen(fname, "w")) == NULL) {
		fprintf(stderr, "Can't open work file\n");
		return -1;
	}
#if 0				/* nabe */
	sprintf(command, "ARTICLE %s", news_no);
#else
	sprintf(command, "ARTICLE %d", news_no);
#endif
	put_nntpserver(command);
	(void) get_server(ser_line, sizeof(ser_line));
	if (atoi(ser_line) == ERR_GOODBYE) {
		fprintf(stderr,
			"SHINBUN: Server Problem! Good-Bye\n");
		exit(1);
	}
	if (atoi(ser_line) != OK_ARTICLE) {
#if 0				/* nabe */
		fprintf(stderr, "Couldn't list article No.%s\n", news_no);
#else
		fprintf(stderr, "Couldn't list article No.%d\n", news_no);
#endif
		return -1;
	}
#if 0				/* nabe */
	i = atoi(news_no);
	put_read_status(i, 1);
#else
	put_read_status(news_no, 1);
#endif
	lineno = nntp_get(fp);
	fclose(fp);
	return lineno;
}

get_view_file_MID(message_id, fname)
	char           *fname, *message_id;
{
	char            command[NNTP_STRLEN];
	char            ser_line[NNTP_STRLEN];
	FILE           *fp;
	int             lineno;

	strcpy(fname, mktemp("/tmp/NRarticle.XXXXXX"));
	if ((fp = fopen(fname, "w")) == NULL) {
		fprintf(stderr, "Can't open work file\n");
		return -1;
	}
	sprintf(command, "ARTICLE %s", message_id);
	put_nntpserver(command);
	(void) get_server(ser_line, sizeof(ser_line));
	if (atoi(ser_line) == ERR_GOODBYE) {
		fprintf(stderr,
			"SHINBUN: Server Problem! Good-Bye\n");
		exit(1);
	}
	if (atoi(ser_line) != OK_ARTICLE) {
/*
		fprintf(stderr, "Couldn't list article %s\n", message_id);
*/
		return -1;
	}
	lineno = nntp_get(fp);
	fclose(fp);
	return lineno;
}

put_read_status(i, st)
	int             i;
	int             st;
{
	NA_INFO         na;

	if (group_info.first == 0) {
		return -1;
	}
	na = group_info.first;

	while (na->anum != i) {
		na = na->next;
		if (na == 0) {
			fprintf(stderr, "No such article\n");
			return -1;
		}
	}
	if (st == 1) {
		na->status = (na->status | AT_READ);
	} else if (st == 0) {
		na->status = (na->status & ~AT_READ);
	} else {
		fprintf(stderr, "Wrong argument\n");
	}

	return 0;
}

read_server_status()
{
	char            ser_line[NNTP_STRLEN];

	put_nntpserver("STAT");
	get_server(ser_line, sizeof(ser_line));
#ifdef DEBUG
	printf("%s\n", ser_line);
#endif				/* DEBUG */
}

/*
 * Get sender list of indicated newsgroup. The results are saved in NA_INFO
 * structures.
 */
nntp_get_sender()
{
	char            ser_line[NNTP_STRLEN];
	char           *buf;
	char            anum[20];
	int             i, j, k = 0;
	NA_INFO         at_info;

	if (group_info.first == 0) {
		while (get_server(ser_line, sizeof(ser_line)) >= 0) {
			if (strcmp(ser_line, ".") == 0) {
				break;
			}
		}
		return;
	}
	while (get_server(ser_line, sizeof(ser_line)) >= 0) {
		at_info = group_info.first;
		k = 0;
		if (strcmp(ser_line, ".") == 0) {
			break;
		}
		while (ser_line[k] != ' ') {
			anum[k] = ser_line[k];
			k++;
		}
		anum[k] = '\0';

		buf = &ser_line[0];
		i = atoi(anum);
		while (at_info->anum != i) {
			at_info = at_info->next;
			if (at_info == 0) {
				break;
			}
		}

		if (at_info != 0) {
			j = 0;
			while (buf[j] != '\0' && buf[j] != '(') {
				j++;
			}
			buf[j] = '\0';
			at_info->sender = strdup(strstr(buf, " "));
		}
	}
}

/*
 * Get unread sender list.
 */
get_sender_file_unread(j)
	int             j;
{
	char            ser_line[NNTP_STRLEN];
	char            anum[20], old_num[20];
	char            command[255];
	int             i, k;
	int             num_read = 0;

	sb_newsrc_status = 1;

	for (i = j; i <= (group_info.last_article + 1); i++) {
		sprintf(anum, "%d", i);
		if ((k = search_isread(i)) == 1) {
			num_read++;
		} else if (k == 0) {
			sprintf(command, "XHDR from %d-", j);
			put_nntpserver(command);
			(void) get_server(ser_line, sizeof(ser_line));
			if (atoi(ser_line) == ERR_GOODBYE) {
				fprintf(stderr,
				     "SHINBUN: Server Problem! Good-Bye\n");
				exit(1);
			}
			if (atoi(ser_line) == OK_HEAD) {
				nntp_get_sender();
			}
			i = group_info.last_article + 1;
		} else if (i == (group_info.last_article + 1)) {
			sprintf(command, "XHDR from %s-", anum);
			put_nntpserver(command);
			(void) get_server(ser_line, sizeof(ser_line));
			if (atoi(ser_line) == ERR_GOODBYE) {
				fprintf(stderr,
				     "SHINBUN: Server Problem! Good-Bye\n");
				exit(1);
			}
			if (atoi(ser_line) == OK_HEAD) {
				nntp_get_sender();
			}
		} else {
			strcpy(old_num, anum);
			while (search_isread(i) == -1 && i <= group_info.last_article) {
				i++;
				sprintf(anum, "%d", i);
			}
			i--;
			sprintf(anum, "%d", i);
			sprintf(command, "XHDR from %s-%s", old_num, anum);
			put_nntpserver(command);
			(void) get_server(ser_line, sizeof(ser_line));
			if (atoi(ser_line) == ERR_GOODBYE) {
				fprintf(stderr,
				     "SHINBUN: Server Problem! Good-Bye\n");
				exit(1);
			}
			if (atoi(ser_line) == OK_HEAD) {
				nntp_get_sender();
			}
		}
	}
}

/*
 * Get all sender list
 */
get_sender_file(j)
	int             j;
{
	char            ser_line[NNTP_STRLEN];
	char            command[MAXSTRLEN];

	sb_newsrc_status = 1;

	sprintf(command, "XHDR from %d-", j);
	put_nntpserver(command);
	(void) get_server(ser_line, sizeof(ser_line));
	if (atoi(ser_line) == ERR_GOODBYE) {
		fprintf(stderr,
			"SHINBUN: Server Problem! Good-Bye\n");
		exit(1);
	}
	if (atoi(ser_line) != OK_HEAD) {
		fprintf(stderr, "Couldn't list subject\n");
		return -1;
	}
	nntp_get_sender();

	return 0;
}

/*
 * Get time list of indicated newsgroup. The results are saved in NA_INFO
 * structures.
 */
nntp_get_time()
{
	char            ser_line[NNTP_STRLEN];
	char           *buf;
	char            anum[20];
	int             i, j, k = 0;
	NA_INFO         at_info;

	if (group_info.first == 0) {
		while (get_server(ser_line, sizeof(ser_line)) >= 0) {
			if (strcmp(ser_line, ".") == 0) {
				break;
			}
		}
		return;
	}
	while (get_server(ser_line, sizeof(ser_line)) >= 0) {
		at_info = group_info.first;
		k = 0;
		if (strcmp(ser_line, ".") == 0) {
			break;
		}
		while (ser_line[k] != ' ') {
			anum[k] = ser_line[k];
			k++;
		}
		anum[k] = '\0';

		buf = &ser_line[0];
		i = atoi(anum);
		while (at_info->anum != i) {
			at_info = at_info->next;
			if (at_info == 0) {
				break;
			}
		}
		j = strlen(buf);
		while (buf[j] != ':') {
			j--;
		}
		buf[j] = '\0';
		while (*buf != ' ') {
			buf++;
		}

		if (at_info != 0) {
			at_info->time = strdup(buf);
		}
	}
}

/*
 * Get unread sender list.
 */
get_time_file_unread(j)
	int             j;
{
	char            ser_line[NNTP_STRLEN];
	char            anum[20], old_num[20];
	char            command[255];
	int             i, k;
	int             num_read = 0;

	sb_newsrc_status = 1;

	for (i = j; i <= (group_info.last_article + 1); i++) {
		sprintf(anum, "%d", i);
		if ((k = search_isread(i)) == 1) {
			num_read++;
		} else if (k == 0) {
			sprintf(command, "XHDR date %d-", j);
			put_nntpserver(command);
			(void) get_server(ser_line, sizeof(ser_line));
			if (atoi(ser_line) == ERR_GOODBYE) {
				fprintf(stderr,
				     "SHINBUN: Server Problem! Good-Bye\n");
				exit(1);
			}
			if (atoi(ser_line) == OK_HEAD) {
				nntp_get_time();
			}
			i = group_info.last_article + 1;
		} else if (i == (group_info.last_article + 1)) {
			sprintf(command, "XHDR date %s-", anum);
			put_nntpserver(command);
			(void) get_server(ser_line, sizeof(ser_line));
			if (atoi(ser_line) == ERR_GOODBYE) {
				fprintf(stderr,
				     "SHINBUN: Server Problem! Good-Bye\n");
				exit(1);
			}
			if (atoi(ser_line) == OK_HEAD) {
				nntp_get_time();
			}
		} else {
			strcpy(old_num, anum);
			while (search_isread(i) == -1 && i <= group_info.last_article) {
				i++;
				sprintf(anum, "%d", i);
			}
			i--;
			sprintf(anum, "%d", i);
			sprintf(command, "XHDR date %s-%s", old_num, anum);
			put_nntpserver(command);
			(void) get_server(ser_line, sizeof(ser_line));
			if (atoi(ser_line) == ERR_GOODBYE) {
				fprintf(stderr,
				     "SHINBUN: Server Problem! Good-Bye\n");
				exit(1);
			}
			if (atoi(ser_line) == OK_HEAD) {
				nntp_get_time();
			}
		}
	}
}

/*
 * Get all time list
 */
get_time_file(j)
	int             j;
{
	char            ser_line[NNTP_STRLEN];
	char            command[MAXSTRLEN];

	sb_newsrc_status = 1;

	sprintf(command, "XHDR date %d-", j);
	put_nntpserver(command);
	(void) get_server(ser_line, sizeof(ser_line));
	if (atoi(ser_line) == ERR_GOODBYE) {
		fprintf(stderr,
			"SHINBUN: Server Problem! Good-Bye\n");
		exit(1);
	}
	if (atoi(ser_line) != OK_HEAD) {
		fprintf(stderr, "Couldn't list subject\n");
		return -1;
	}
	nntp_get_time();

	return 0;
}

search_nlist(gname)
	char           *gname;
{
	int             j = 0;

	while (j < num_of_listall) {
		if (strcmp(newslist_all[j], gname) == 0) {
			return (j);
		}
		j++;
	}
	return (-1);
}

search_num_of_unread(gname)
	char           *gname;
{
	int             i, j, k = 0;
	char            buf[20];

	i = search_nlist(gname);

	if (i == -1) {
		return -1;
	}
	if (search_ngrp(gname) == -1) {
		return (news_lst_a[i] - news_fst_a[i]);
	}
	for (j = news_fst_a[i]; j <= news_lst_a[i]; j++) {
		sprintf(buf, "%d", j);
		if (isread_article(gname, buf)) {
			k++;
		}
	}
	return k;
}

get_fst_lst_num(groupname, fst, lst)
	char           *groupname;
	int            *fst, *lst;
{
	char            command[NNTP_STRLEN];
	char            ser_line[NNTP_STRLEN];
	char           *buf;

	sprintf(command, "GROUP %s", groupname);
	put_nntpserver(command);
	(void) get_server(ser_line, sizeof(ser_line));
	if (atoi(ser_line) == ERR_GOODBYE) {
		fprintf(stderr,
			"SHINBUN: Server Problem! Good-Bye\n");
		exit(1);
	}
	if (atoi(ser_line) != OK_GROUP) {
		fprintf(stderr,
			"Couldn't open group %s\n", groupname);
		return -1;
	}
	buf = ser_line;
	/* EMPTY */
	while (*buf++ != ' ');
	/* EMPTY */
	while (*buf++ != ' ');
	*fst = atoi(buf);
	/* EMPTY */
	while (*buf++ != ' ');
	*lst = atoi(buf);

	return 0;
}
