#ifdef sccs
static char     sccsid[] = "@(#)shinbun_cmd.c	1.12 91/07/24";
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


#ifdef EUCNEWS
#include <jcode.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "shinbun.h"
#include "config.h"
#include "sb_def.h"

extern NG_INFO  group_info;
extern int      sb_display_status;
extern int	sb_newsrc_status;
extern char     ACTIVE_FLIST_NAM[];
extern char     NEWSRC_FLIST_NAM[];
struct timeval last_put_time;
struct timezone last_put_timezone;

get_indent_header(file_name, msg_id, sender)
	char           *file_name, *msg_id, *sender;
{
	FILE           *fp;
	char            buf[MAXSTRLEN];
	int             i;

	msg_id[0] = '\0';
	sender[0] = '\0';

	if ((fp = fopen(file_name, "r")) == NULL) {
		fprintf(stderr, "Cannot Open file %s\n", file_name);
		exit(1);
	}
	while (fgets(buf, MAXSTRLEN, fp) != NULL) {
		if (strncmp(buf, "Message-ID", 10) == 0) {
			if (buf[11] == ' ')
				strcpy(msg_id, strchr(buf, ' '));
		} else if (strncmp(buf, "From", 4) == 0) {
			if (buf[5] == ' ')
				strcpy(sender, strchr(buf, ' '));
		}
	}
	fclose(fp);
	i = strlen(msg_id);
	if (i > 0) {
		msg_id[i - 1] = '\0';
	}
	i = strlen(sender);
	if (i > 0) {
		sender[i - 1] = '\0';
	}
}

/*
 * Add bracket at the first column of each line for News/Mail reply process.
 */
change_file_to_bracket(file_name)
	char           *file_name;
{
	FILE           *fp1, *fp2;
	char            buf[MAXSTRLEN];
	char            old_fname[MAXSTRLEN];

	strcpy(old_fname, file_name);
	strncat(file_name, "BK", 4);

	if ((fp1 = fopen(old_fname, "r")) == NULL) {
		fprintf(stderr, "Cannot Open file %s\n", old_fname);
		exit(1);
	}
	if ((fp2 = fopen(file_name, "w")) == NULL) {
		fprintf(stderr, "Cannot Open file %s\n", file_name);
		exit(1);
	}
	fprintf(fp2, "\n\n%s\n\n", BEGIN_INCLUDE);
	while (fgets(buf, MAXSTRLEN, fp1) != NULL) {
		fputs(buf, fp2);
	}
	fprintf(fp2, "\n\n%s\n", END_INCLUDE);
	fclose(fp1);
	fclose(fp2);

	unlink(old_fname);
}

/*
 * Add indicator at the first line and last line for News/Mail reply process.
 */
change_file_to_include(file_name)
	char           *file_name;
{
	FILE           *fp1, *fp2;
	char            buf[MAXSTRLEN];
	char            old_fname[MAXSTRLEN];
	char            msg_id[MAXSTRLEN];
	char            sender[MAXSTRLEN];
	char            xsender[MAXSTRLEN];
	char		bracket[MAXSTRLEN];

	get_indent_header(file_name, msg_id, sender);

	strcpy(old_fname, file_name);
	strncat(file_name, "BK", 4);

	if ((fp1 = fopen(old_fname, "r")) == NULL) {
		fprintf(stderr, "Cannot Open file %s\n", old_fname);
		exit(1);
	}
	if ((fp2 = fopen(file_name, "w")) == NULL) {
		fprintf(stderr, "Cannot Open file %s\n", file_name);
		exit(1);
	}
	if (msg_id[0] != '\0' && sender[0] != '\0') {
		strcpy(xsender, convert_madd_to_name(sender));
		while (fgets(buf, MAXSTRLEN, fp1) != NULL) {
			if (buf[0] == '\n')
				break;
		}
		fprintf(fp2, INDENT_INCLUDE_MSG, msg_id, sender);
	}
	if((int)sb_get_props_value(Sb_include_sender)){
		char *first_sender;

		first_sender = (char *)strtok(xsender, "@");
		if(first_sender == NULL){
                	first_sender = (char *)strtok(xsender, "@");
		}
		sprintf(bracket, "%s%s", first_sender,
			(char *) sb_get_props_char_value(Sb_indent_prefix));
	} else {
		strcpy(bracket,
			(char *) sb_get_props_char_value(Sb_indent_prefix));
	}
	while (fgets(buf, MAXSTRLEN, fp1) != NULL) {
		fprintf(fp2, "%s%s", bracket, buf);
	}
	fprintf(fp2, "\n");
	fclose(fp1);
	fclose(fp2);

	unlink(old_fname);
}

change_num_to_mkurd(group, num)
	int             num;
	char           *group;
{
	NA_INFO         na;

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return -1;
	}
	if (group_info.first == 0) {
		return -1;
	}
	na = group_info.first;

	while (na->anum != num) {
		na = na->next;
		if (na == 0) {
			return -1;
		}
	}
	return (na->status & AT_MKURD);
}

change_line_to_asmkurd(group, line, i)
	int             line, i;
	char           *group;
{
	NA_INFO         na;

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return -1;
	}
	if (group_info.first == 0) {
		return -1;
	}
	na = group_info.first;

	while (na->line != line) {
		na = na->next;
		if (na == 0) {
#ifdef DEBUG
			fprintf(stderr, "Something is wrong\n");
#endif				/* DEBUG */
			return -1;
		}
	}
	if (i == 1) {
		na->status = (na->status | AT_MKURD);
	} else if (i == 0) {
		na->status = (na->status & ~(AT_MKURD));
	} else {
		return -1;
	}
	return 0;
}

change_all_to_asmkurd(group, st)
	char           *group;
	int             st;
{
	NA_INFO         na;

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return -1;
	}
	if (group_info.first == 0) {
		return -1;
	}
	na = group_info.first;

	while (na != 0) {
		if (st == 1) {
			na->status = (na->status | AT_MKURD);
		} else if (st == 0) {
			na->status = (na->status & ~(AT_MKURD));
		} else {
			return -1;
		}
		na = na->next;
	}
	return 0;
}
/*
 * Change subject line number currently displayed to article number in order
 * to access articles.
 */
change_line_to_anum(group, line)
	int             line;
	char           *group;
{
	NA_INFO         na;

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return -1;
	}
	if (group_info.first == 0) {
		return -1;
	}
	na = group_info.first;

	while (na->line != line) {
		na = na->next;
		if (na == 0) {
#ifdef DEBUG
			fprintf(stderr, "Something is wrong\n");
#endif				/* DEBUG */
			return -1;
		}
	}
	return na->anum;
}


/*
 * Change line number currently displayed to Subject field.
 */
char           *
change_line_to_subject(group, line)
	int             line;
	char           *group;
{
	NA_INFO         na;

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return NULL;
	}
	if (group_info.first == 0) {
		return NULL;
	}
	na = group_info.first;

	while (na->line != line) {
		na = na->next;
		if (na == 0) {
			return NULL;
		}
	}
	return na->subject;
}

/*
 * Change line number currently displayed to sender field.
 */
char           *
change_line_to_sender(group, line)
	int             line;
	char           *group;
{
	NA_INFO         na;

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return NULL;
	}
	if (group_info.first == 0) {
		return NULL;
	}
	na = group_info.first;

	while (na->line != line) {
		na = na->next;
		if (na == 0) {
			return NULL;
		}
	}
	return na->sender;
}

/*
 * Change line number currently displayed to time field.
 */
char           *
change_line_to_time(group, line)
	int             line;
	char           *group;
{
	NA_INFO         na;

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return NULL;
	}
	if (group_info.first == 0) {
		return NULL;
	}
	na = group_info.first;

	while (na->line != line) {
		na = na->next;
		if (na == 0) {
			return NULL;
		}
	}
	return na->time;
}

/*
 * Change line number currently displayed to header field.
 */
char           *
change_line_to_header(group, line)
	int             line;
	char           *group;
{
	NA_INFO         na;
	static char     header[MAXSTRLEN];
	char            sender[MAXSTRLEN];
	int             i;

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return NULL;
	}
	if (group_info.first == 0) {
		return NULL;
	}
	na = group_info.first;

	while (na->line != line) {
		na = na->next;
		if (na == 0) {
			return NULL;
		}
	}
	if (sb_display_status == 1) {
		if (na->sender == 0) {
			strcpy(sender, "********************");
		} else {
			i = strlen(na->sender);
			strcpy(sender, na->sender);
			if (i > 25) {
				i = 25;
				while (sender[i] != '.' && sender[i] != '@') {
					i--;
					if (i == 0)
						break;
				}
				if (i != 0)
					sender[i] = '\0';
			}
		}
		sprintf(header, "%5d %-25s  %s", na->anum, sender, na->subject);
	} else if (sb_display_status == 2) {
		sprintf(header, "%5d  %s", na->anum, na->subject);
	} else {
		if (na->sender == 0) {
			strcpy(sender, "***************");
		} else {
			i = strlen(na->sender);
			strcpy(sender, na->sender);
			if (i > 20) {
				i = 20;
				while (sender[i] != '.' && sender[i] != '@') {
					i--;
					if (i == 0)
						break;
				}
				if (i != 0)
					sender[i] = '\0';
			}
		}
		if (na->time == 0) {
			sprintf(header, "%5d %-20s %16s  %s",
			  na->anum, sender, "***************", na->subject);
		} else {
			sprintf(header, "%5d %-20s %16s  %s",
				na->anum, sender, na->time, na->subject);
		}
	}
	return (&header[0]);
}

/*
 * Add or delete cancel flag to indicated article.
 */
change_anum_ascancel(group, anum, i)
	int             i;
	char           *group, *anum;
{
	NA_INFO         na;
	int             j;

	j = atoi(anum);

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return -1;
	}
	if (group_info.first == 0) {
		return -1;
	}
	na = group_info.first;

	while (na->anum != j) {
		na = na->next;
		if (na == 0) {
#ifdef DEBUG
			fprintf(stderr, "Something is wrong\n");
#endif				/* DEBUG */
			return -1;
		}
	}
	if (i == 1) {
		na->status = (na->status | AT_CANCEL);
	} else if (i == 0) {
		na->status = (na->status & ~(AT_CANCEL));
	} else {
		return -1;
	}
	return 0;
}

/*
 * return if the article is canceled or not.
 */
change_line_to_iscancel(group, line)
	int             line;
	char           *group;
{
	NA_INFO         na;

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return -1;
	}
	if (group_info.first == 0) {
		return -1;
	}
	na = group_info.first;

	while (na->line != line) {
		na = na->next;
		if (na == 0) {
#ifdef DEBUG
			fprintf(stderr, "Something is wrong\n");
#endif				/* DEBUG */
			return -1;
		}
	}
	return (na->status & AT_CANCEL);
}

change_line_to_isread(group, line)
	int             line;
	char           *group;
{
	NA_INFO         na;

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return -1;
	}
	if (group_info.first == 0) {
		return -1;
	}
	na = group_info.first;

	while (na->line != line) {
		na = na->next;
		if (na == 0) {
#ifdef DEBUG
			fprintf(stderr, "Something is wrong\n");
#endif				/* DEBUG */
			return -1;
		}
	}
	return (na->status & AT_READ);
}

change_line_to_asread(group, line, i)
	int             line, i;
	char           *group;
{
	NA_INFO         na;

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return -1;
	}
	if (group_info.first == 0) {
		return -1;
	}
	na = group_info.first;

	while (na->line != line) {
		na = na->next;
		if (na == 0) {
#ifdef DEBUG
			fprintf(stderr, "Something is wrong\n");
#endif				/* DEBUG */
			return -1;
		}
	}
	if (i == 1) {
		na->status = (na->status | AT_READ);
		set_asread(na->anum);
	} else if (i == 0) {
		na->status = (na->status & ~(AT_READ));
		set_asunread(na->anum);
	} else {
		return -1;
	}
	sb_newsrc_status = TRUE;
	return 0;
}

change_all_to_asread(group, st)
	char           *group;
	int             st;
{
	NA_INFO         na;

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return -1;
	}
	if (group_info.first == 0) {
		return -1;
	}
	na = group_info.first;

	while (na != 0) {
		if (st == 1) {
			na->status = (na->status | AT_READ);
			set_asread(na->anum);
		} else if (st == 0) {
			na->status = (na->status & ~AT_READ);
			set_asunread(na->anum);
		} else {
			return -1;
		}
		na = na->next;
	}
	sb_newsrc_status = TRUE;
	return 0;
}

change_line_to_isselect(group, line, st)
	int             line, st;
	char           *group;
{
	NA_INFO         na;

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return -1;
	}
	if (group_info.first == 0) {
		return -1;
	}
	na = group_info.first;

	while (na->line != line) {
		na = na->next;
		if (na == 0) {
#ifdef DEBUG
			fprintf(stderr, "Something is wrong\n");
#endif				/* DEBUG */
			return -1;
		}
	}
	return (na->status & st);
}

change_line_to_asselect(group, line, st, i)
	int             line, st, i;
	char           *group;
{
	NA_INFO         na;

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return -1;
	}
	if (group_info.first == 0) {
		return -1;
	}
	na = group_info.first;

	while (na->line != line) {
		na = na->next;
		if (na == 0) {
#ifdef DEBUG
			fprintf(stderr, "Something is wrong\n");
#endif				/* DEBUG */
			return -1;
		}
	}
	if (i == 1) {
		na->status = (na->status | st);
	} else if (i == 0) {
		na->status = (na->status & ~(st));
	} else {
		return -1;
	}
	return 0;
}

/*
 * return amount of articles in current newsgroup.
 */
return_num_article(group)
	char           *group;
{
	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return -1;
	}
	return group_info.num_article;
}

/*
 * return amount of unread articles in current newsgroup.
 */
return_num_unread(group)
	char           *group;
{
	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return -1;
	}
	return group_info.num_unread;
}

char           *
return_dir_of_newsrc()
{
	int             i;
	char            buf[256];

	i = strlen(NEWSRC_FLIST_NAM);
	strncpy(buf, NEWSRC_FLIST_NAM, i);

	while (buf[i] != '/' && i > 0) {
		i--;
	}
	buf[i] = '\0';

	return (buf);
}

char           *
return_file_of_newsrc()
{
	int             i, j, k;
	char            buf[256];

	j = i = strlen(NEWSRC_FLIST_NAM);

	while (NEWSRC_FLIST_NAM[j] != '/' && j > 0) {
		j--;
	}
	j++;

	for (k = 0; j <= i; k++, j++) {
		buf[k] = NEWSRC_FLIST_NAM[j];
	}

	return (buf);
}

/*
 * return current newsgroup name
 */
char           *
get_current_newsgroup()
{
	return group_info.groupname;
}

catchup_ngrp(groupname)
	char           *groupname;
{
	int             n, i = 0;
	char            buf1[MAXSTRLEN];
	char            buf2[MAXSTRLEN];

	if ((n = search_ngrp(groupname)) < 0) {
		return -1;
	}
	strcpy(buf1, return_newsrc_data(n));
	while (buf1[i] != ':') {
		i++;
	}
	buf1[i] = '\0';
	sprintf(buf2, "%s: \n", buf1);

	put_newsrc(n, buf2);

	return 0;
}

change_article_to_sender(group, article, str)
	char           *group;
	int           article;
	char           *str;
{
	NA_INFO         na;

	if (strcmp(group, group_info.groupname)) {
#ifdef DEBUG
		fprintf(stderr, "Wrong usage of this function. current ng is %s\n",
			group_info.groupname);
#endif DEBUG
		return -1;
	}
	if (group_info.first == 0) {
		return -1;
	}
	na = group_info.first;

	while (na->anum != article) {
		na = na->next;
		if (na == 0) {
#ifdef DEBUG
			fprintf(stderr, "Something is wrong\n");
#endif				/* DEBUG */
			return -1;
		}
	}
	while(*(na->sender) != '@'){
		*str++ = *(na->sender)++;
		if(*(na->sender) == '\0')
			break;
	}

	*str = '\0';

	return 0;
}

put_nntpserver(s)
char	*s;
{
	put_server(s);
	gettimeofday(&last_put_time, &last_put_timezone);
}
