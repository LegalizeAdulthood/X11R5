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


#ifdef EUCNEWS
#include <jcode.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shinbun.h"
#include "config.h"

extern int		sb_newsrc_status;
extern int             num_of_ngrp;
extern char           *newsrc[];
extern char            ACTIVE_FLIST_NAM[];
extern char            NEWSRC_FLIST_NAM[];
extern NG_INFO         group_info;

/*
 * Save informations about each newsgroup to .newsrc (or some
 * other files)
 */
save_newsrc()
{
	FILE           *fp;
	int             i, j;

	write_to_newsrc();
	/* Write newsrc data to .newsrc file */
	if ((fp = fopen(NEWSRC_FLIST_NAM, "w")) == NULL) {
		/*
		 * fprintf(stderr, "Cannot write .newsrc\n");
		 */
		return -1;
	}
	j = num_of_ngrp;
	num_of_ngrp = 0;
	for (i = 0; i < j; i++) {
		fputs(newsrc[i], fp);
		if(newsrc[i]){
			free(newsrc[i]);
			newsrc[i] = NULL;
		}
	}
	fflush(fp);
	fclose(fp);
	sb_newsrc_status = FALSE;
	return 0;
}

/*
 * Free memory used for .newsrc information
 */
free_newsrc()
{
	int             i, j;

	j = num_of_ngrp;
	num_of_ngrp = 0;
	for (i = 0; i < j; i++) {
		if(newsrc[i]){
			free(newsrc[i]);
			newsrc[i] = NULL;
		}
	}
}

/*
 * Write current newsgroup read/unread data to .newsrc that
 * is on memory.
 */
void
write_to_newsrc()
{
	int             l, first, last;
	int             i;
	char            buf[4096];
	char            num[20];

	if ((l = search_ngrp(group_info.groupname)) == -1) {
		return;
	}
	buf[0] = '\0';
	sprintf(buf, "%s: ", group_info.groupname);
	for (i = group_info.first_article; i <= group_info.last_article; i++) {
		sprintf(num, "%d", i);
		if ((1 == ck_ginfo(i)) || (search_isread(atoi(num)) == 1)){
			first = i;
			last = cf_status_data(i);
			i = last + 1;
			if(first == group_info.first_article){
				first = 1;
			}
			if (first == last) {
				sprintf(buf, "%s%d,", buf, first);
			} else if (first < last) {
				sprintf(buf, "%s%d-%d,", buf, first, last);
			}
		}
	}
	if (buf[strlen(buf) - 1] == ',') {
		buf[strlen(buf) - 1] = '\n';
	} else {
		strncat(buf, " \n", 2);
	}
	newsrc[l] = realloc(newsrc[l], strlen(buf) + 1);
	strcpy(newsrc[l], buf);
}

/*
 * Call write_to_newsrc() and free memory held current
 * newsgroup read/unread data.
 */
write_newsrc_data()
{
	write_to_newsrc();
	if (group_info.first == 0) {
		return;
	}
	free_na_info(group_info.first);
	if(group_info.first->subject){
		free(group_info.first->subject);
		group_info.first->subject = NULL;
	}
	if(group_info.first->sender){
		free(group_info.first->sender);
		group_info.first->sender = NULL;
	}
	if(group_info.first->time){
		free(group_info.first->time);
		group_info.first->time = NULL;
	}
	if(group_info.first){
		free(group_info.first);
		group_info.first = NULL;
	}
}

/*
 * Read .newsrc file and hold it on memory
 * This is not used in shinbun.
 */
create_newsrc_file()
{
	char            command[MAXSTRLEN];
	FILE           *fp;

/*
	sprintf(NEWSRC_FLIST_NAM, "%s/.newsrc", getenv("HOME"));
*/
	if(*NEWSRC_FLIST_NAM == NULL)
		sprintf(NEWSRC_FLIST_NAM, "%s/.newsrc", getenv("HOME"));

	if ((fp = fopen(NEWSRC_FLIST_NAM, "r")) == NULL) {
		sprintf(command, "/bin/cp %s %s", ACTIVE_FLIST_NAM, NEWSRC_FLIST_NAM);
		system(command);
	} else {
		fclose(fp);
	}
	sb_newsrc_status = FALSE;
}

void
read_newsrc()
{
	FILE           *fp;
	char            tmpbuf[512];
	char            *buf;
	char            filename[MAXSTRLEN];
	int             i = 0;

	/* Read .newsrc */
	if ((fp = fopen(NEWSRC_FLIST_NAM, "r")) == NULL) {
		fprintf(stderr, "Can't open .newsrc file\n");
		exit(1);
	}
	while ((buf = fgets(tmpbuf, sizeof(tmpbuf), fp)) != NULL) {
		newsrc[i] = (char *) malloc(strlen(buf) + 1);
		strcpy(newsrc[i], buf);
		i++;
		if (i == MAXNGRP)
			exit(1);
	}
	fclose(fp);
	/* back up */
	sprintf(tmpbuf, "/usr/bin/cp %s %s/.newsrc.shinbun.backup", 
		NEWSRC_FLIST_NAM, (char *)getenv("HOME"));
	system(tmpbuf);
	num_of_ngrp = i;
	sb_newsrc_status = FALSE;
}

cf_status_data(ip)
        int             ip;
{
        int             i;
        char            num[20];

        sprintf(num, "%d", ip);

        if (ip == group_info.last_article) {
                if (change_num_to_mkurd(group_info.groupname, ip)>0){
                        return (ip - 1);
                } else if ((1 != ck_ginfo(ip)) &&
                        (search_isread(atoi(num)) != 1)) {
                        return (ip - 1);
                } else {
                        return group_info.last_article;
                }
        } else if (change_num_to_mkurd(group_info.groupname, ip)>0){
                return (ip - 1);
        } else if ((1 != ck_ginfo(ip)) && (search_isread(atoi(num)) != 1)) {
                return (ip - 1);
        } else {
                ip++;
                i = cf_status_data(ip);
                return i;
        }
}
 
char	*
return_newsrc_data(i)
int	i;
{
	return newsrc[i];
}

put_newsrc(i, buf)
int	i;
char	*buf;
{
	if(newsrc[i]){
		free(newsrc[i]);
		newsrc[i] = NULL;
	}
	newsrc[i] = strdup(buf);
}

search_isread(num)
int	num;
{
	if(group_info.read_status[num - group_info.first_article] == 'R')
		return 1;
	else
		return -1;
}

set_asread(num)
int	num;
{
	int	i;

	i = num - group_info.first_article;
	if (num > group_info.last_article)
		return -1;
	group_info.read_status[num - group_info.first_article] = 'R';
	return 0;
}

set_asunread(num)
int     num;
{
        int     i;
 
        i = num - group_info.first_article;
        if (num > group_info.last_article)
                return -1;
        group_info.read_status[num - group_info.first_article] = ' ';
        return 0;
}

set_allasread()
{
        NA_INFO         na;

        if (group_info.first == 0) {
                return -1;
        }
        na = group_info.first;

        while (na != 0) {
		if(na != 0){
		    group_info.read_status[na->anum - group_info.first_article] = 'R';
                } else {
                        return -1;
                }
                na = na->next;
        }
        return 0;
}

set_allasunread()
{
        NA_INFO         na;

        if (group_info.first == 0) {
                return -1;
        }
        na = group_info.first;

        while (na != 0) {
		if(na != 0){
		    group_info.read_status[na->anum - group_info.first_article] = ' ';
                } else {
                        return -1;
                }
                na = na->next;
        }
        return 0;
}

set_initial_read()
{
	int	i, j;
	char	anum[20];

	for(i=0; i<group_info.num_article; i++){
		sprintf(anum,"%d",i+group_info.first_article);
		if((isread_article(group_info.groupname, anum)) == 1){
                    group_info.read_status[i] = 'R';
		} else {
                    group_info.read_status[i] = ' ';
		}
	}
}

search_first_unread(i)
int	i;
{
	int j;

	if(group_info.last_article == 0){
		return -1;
	} else {
		j = group_info.last_article - i;
		return ((j > 0)? j : 0);
	}
}
