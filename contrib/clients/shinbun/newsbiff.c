#ifdef sccs
static  char sccsid[] = "%W% %E%";
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
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <malloc.h>
#include <signal.h>
#include "shinbun.h"
#include "nntp.h"
#include "conf.h"

#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/icon.h>
#include <xview/textsw.h>
#include <xview/notify.h>
#include <xview/cursor.h>
#include "sb_item.h"
#include "sb_def.h"
#include "sb_tool.h"
#include "config.h"

#define MAXDATA 4096

FILE	*biff_ser_rd_fp;
FILE	*biff_ser_wr_fp;
extern struct timeval last_put_time;
extern struct timezone last_put_timezone;
extern char NEWSRC_FLIST_NAM[];
extern char NEWSBIFFRC[];

struct	sockaddr_in NB_sin;
char    *DATA[MAXDATA];
int	last_article_data[MAXDATA];
int	jxx = 0;
char	old_buf[15];
char	new_buf[15];
int	num_new;
int	num_of_nbiffgrp;
char	*newsbiffrc[MAX_OBSERVED_GROUPS+1];

Notify_value
nbiff()
{
	char command[100];
	int  i;
	char machine_name[50];
	extern int num_of_nbiffgrp;
	extern char *change_num_to_nbiffgrp();
	extern int sb_arrive_icon;
	int sb_news_biff_beep;
	int sb_news_biff_move;
	int	num_newarticle;

	strncpy(machine_name, (char *)getenv("NNTPSERVER"), 50);

	avoid_timeout_server();

        if((get_remote_time(machine_name, new_buf)) < 0){
		strcpy(new_buf, old_buf);
	}
	if(jxx == 0){
	    jxx = 1;
	    num_new = 0;
	    biff_ser_rd_fp = NULL;
	    biff_ser_wr_fp = NULL;
	    strcpy(old_buf, new_buf);
            return NOTIFY_DONE;
	} else if (jxx < 0){
		biff_ser_rd_fp = NULL;
                biff_ser_wr_fp = NULL;
                return NOTIFY_DONE;
	} else {
	    if(biff_ser_rd_fp != NULL || biff_ser_wr_fp != NULL){
		return NOTIFY_DONE;
	    }
            if(biff_connect_nntp_server() != 0){
		biff_ser_rd_fp = NULL;
		biff_ser_wr_fp = NULL;
		return NOTIFY_DONE;
	    }
	    sprintf(command,"NEWNEWS * %s GMT", old_buf);
	    num_newarticle = biff_nntp_put(command);
	    if(num_newarticle > 0){
	        output_newsinfo(num_newarticle);
		num_new = set_last_article_data(num_newarticle);
		mem_free(num_newarticle);
	    } else {
		num_new = 0;
	    }
            biff_nntp_quit();
	}
	strcpy(old_buf, new_buf);

#if OLD
#ifdef OW_I18N
	(void)frame_msg(sb_frame, gettext("Now newsbiff is running..."));
#else
	(void)frame_msg(sb_frame, "Now newsbiff is running...");
#endif
#endif OLD

	if(num_new == 0){
#if OLD
		(void)frame_msg(sb_frame,
#ifdef OW_I18N
			gettext("Now newsbiff is running... Done"));
#else
			"Now newsbiff is running... Done");
#endif
#endif OLD
		return NOTIFY_DONE;
	}

	if(((int)xv_get(sb_frame, FRAME_CLOSED) == TRUE) &&
		(sb_arrive_icon == FALSE)){
		xv_set(sb_frame_icon, 
			ICON_IMAGE,		sb_arrive_image,
			ICON_MASK_IMAGE,	sb_arrive_mask_image,
			ICON_TRANSPARENT,	TRUE,
#ifdef OW_I18N
			XV_LABEL,		gettext("Shinbun tool"),
#else
			XV_LABEL,		"Shinbun tool",
#endif
			NULL);

		xv_set(sb_frame, FRAME_ICON, sb_frame_icon, NULL);
		sb_arrive_icon = TRUE;

		return NOTIFY_DONE;
	}

	if((sb_news_biff_beep = (int)sb_get_props_value(Sb_biff_beep)) >0 ){
		for(i=0; i<sb_news_biff_beep; i++)
			XBell((Display *)xv_get(sb_frame, XV_DISPLAY), 0);
	}

	if(((sb_news_biff_move = (int)sb_get_props_value(Sb_biff_move)) >0 ) &&
		((int)xv_get(sb_frame, FRAME_CLOSED) == FALSE)){
		int w, h;
		
		w = (int)xv_get(sb_frame, XV_WIDTH);
		h = (int)xv_get(sb_frame, XV_HEIGHT);

		for(i=0; i<sb_news_biff_move; i++)
			sb_frame_move(w, h);
	}

	return NOTIFY_DONE;
}

insert_new_group_to_plist()
{
	register int i, j;
	char *name;

	list_all_delete(sb_newsbiff_panel);

	for(i=0, j=0; i<num_of_nbiffgrp; i++){
		if(is_newarticle(i) != 0){
			name = (char *)change_num_to_nbiffgrp(i);
			list_insert(sb_newsbiff_panel, name, j);
			j++;
		}
	}

}

avoid_timeout_server()
{
     struct timeval current_time;
     struct timezone current_timezone;

        gettimeofday(&current_time, &current_timezone);
	if((current_time.tv_sec - last_put_time.tv_sec) > 1800){
		read_server_status();
	}
}

mem_free(num)
int  num;
{
    int i;

    for(i = 0; i < num; i++){
	free(DATA[i]);
    }
}

set_last_article_data(num)
int  num;
{
    char            ser_line[NNTP_STRLEN];
    char	    *tmps;
    char	    command[50];
    int		    i, j, k=0;

    for(i = 0; i < num; i++){
	if(DATA[i][0] != '*'){
	    sprintf(command, "GROUP %s", DATA[i]);
            biff_put_server(command);
            (void) biff_get_server(ser_line, sizeof(ser_line));
	    /* EMPTY */
	    if(atoi(ser_line) != OK_GROUP){
	    } else {
		tmps = strtok(ser_line, " ");
		tmps = strtok(NULL, " ");
		tmps = strtok(NULL, " ");
		tmps = strtok(NULL, " ");
		if(tmps != NULL){
		    j = search_nbiffgrp(DATA[i]);
		    if(j >= 0){
		        last_article_data[j] = atoi(tmps);
			k++;
		    }
		} else {
			return (k);
		}
	    }
	}
    }
    return k;
}

output_newsinfo(num)
int  num;
{
    char            ser_line[NNTP_STRLEN];
    char            command[50];
    char	    *tmps;
    char	    hd_newsgroup[NNTP_STRLEN];
    int		    i, j, k;

    for(i = 0; i < num; i++){
	j = 0;
	while(DATA[i][j] != '\0' && DATA[i][j] != '\n')
	    j++;
	DATA[i][j] = '\0';
	sprintf(command,"HEAD %s", DATA[i]);

        biff_put_server(command);
        (void) biff_get_server(ser_line, sizeof(ser_line));
        if (atoi(ser_line) != OK_HEAD) {
	    DATA[i][0] = '*';
        } else {
	    tmps = (char *)strstr(ser_line, " ");
            while (biff_get_server(ser_line, sizeof(ser_line)) >= 0) {
                if (strcmp(ser_line, ".") == 0) {
                    break;
	        } else if (strncmp(ser_line, "Newsgroups", 10) == 0) {
		     tmps = (char *)strstr(ser_line, " ");
		     tmps++;
                     strcpy(hd_newsgroup, tmps);
		     k = search_nbiffgrp(tmps);
		    if(k >= 0){
		        DATA[i] = realloc(DATA[i], strlen(tmps) + 1);
			strcpy(DATA[i], tmps);
		    } else {
			DATA[i][0] = '*';
		    }
	        }
	    }
	}
    }
}

biff_nntp_put(command)
char *command;
{
    char ser_line[NNTP_STRLEN];

    biff_put_server(command);
    (void) biff_get_server(ser_line, sizeof(ser_line));
    if (atoi(ser_line) != OK_NEWNEWS) {
#ifdef DEBUG
        fprintf(stderr,
            "Couldn't get news\n%s", ser_line);
#endif DEBUG
        return 0;
    }
    return (biff_nntp_get());
}

biff_nntp_get()
{
    int   i=0;
    char  ser_line[NNTP_STRLEN];
    static char  old_line[NNTP_STRLEN];

    while (biff_get_server(ser_line, sizeof(ser_line)) >= 0) {  /* while */
        if (ser_line[0] == '.')
            break;
        if(i >= MAXDATA){
#ifdef DEBUG
            fprintf(stderr,"Error: Too much data\n");
#endif DEBUG
		;
	/* EMPTY */
	} else if(i == 0 && strcmp(ser_line, old_line) == 0){
	    ;
        } else {
            if((DATA[i]=malloc(sizeof(ser_line)+1))==NULL){
#ifdef DEBUG
                fprintf(stderr, "Error: Can not allocate memories\n");
#endif DEBUG
                return i;
            }
            strcpy(DATA[i], ser_line);
   	    strcpy(old_line, ser_line);
            i++;
	}
    }
    return i;
}

biff_nntp_quit()
{
	biff_close_server();
}

biff_connect_nntp_server()
{
    int             response;
    char            *NB_server;
    extern char    *biff_getserverbyfile();

    /* Connect with NNTP server */
    NB_server = (char *)biff_getserverbyfile();
    if (NB_server == NULL) {
        return(-1);
    }
    response = biff_server_init(NB_server);
    if (response < 0) {
#ifdef DEBUG
        fprintf(stderr,
            "Couldn't connect to %s news server. Try again later\n", NB_server);
#endif DEBUG
        return(-1);
    }
    if (handle_server_response(response, NB_server) < 0)
        return(-1);

    return 0;
}

get_remote_time(mname, buf)
char *mname, *buf;
{
    int count = 0;

    struct timeval timep;
    struct timeval timeout;

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    while(NB_get_socket(mname) == -1){
	count++;
	if(count > 20){
		return -1;
	}
#ifdef DEBUG
	fprintf(stderr,"Error: Can not connect to time server\n");
	fprintf(stderr,"Retry later...\n");
#endif DEBUG
	sleep(1);
    }
    count = 0;
    while(rtime(&NB_sin, &timep, &timeout) != 0){
	count++;
	if(count > 20){
		return -1;
	}
#ifdef DEBUG
	fprintf(stderr,"Error: Can not get remote time\n");
	fprintf(stderr,"Retry later...\n");
#endif DEBUG
	sleep(1);
    }
/*
 * Bug fixed.
 * If NNTP server and client are in different timezones, nbiff routine
 * don't work correctly.(e.g. server is in Japan, client is in US)
 *
 *   strftime(buf,15, "%y%m%d %H%M%S", (localtime(timep)));
 */
    strftime(buf,15, "%y%m%d %H%M%S", (gmtime(timep)));

    return 0;
}

NB_get_socket(machine)  
char    *machine;
{
    struct    servent *getservbyname(), *sp;
    struct    hostent *gethostbyname(), *hp;
    register  char **cp;

    if ((sp = getservbyname("time", "tcp")) ==  NULL) {
#ifdef DEBUG
        fprintf(stderr, "time/tcp: Unknown service.\n");
#endif DEBUG
        return (-1);
    }

    if ((hp = gethostbyname(machine)) == NULL) {
#ifdef DEBUG
        fprintf(stderr, "%s: Unknown host.\n", machine);
#endif DEBUG
        return (-1);
    }

    bzero((char *) &NB_sin, sizeof(NB_sin));
    NB_sin.sin_family = hp->h_addrtype;
    NB_sin.sin_port = sp->s_port;

    for (cp = hp->h_addr_list; cp && *cp; cp++) {
        bcopy(*cp, (char *)&NB_sin.sin_addr, hp->h_length);
    }

    return 0;
}

biff_server_init(machine)
char	*machine;
{
	int	sockt_rd, sockt_wr;
	char	line[256];
	char	*index();

	sockt_rd = get_tcp_socket(machine);

	if (sockt_rd < 0)
		return (-1);

	if ((biff_ser_rd_fp = fdopen(sockt_rd, "r")) == NULL) {
		perror("server_init: fdopen #1");
		return (-1);
	}

	sockt_wr = dup(sockt_rd);
	if ((biff_ser_wr_fp = fdopen(sockt_wr, "w")) == NULL) {
		perror("server_init: fdopen #2");
		biff_ser_rd_fp = NULL;		/* from above */
		return (-1);
	}

	/* Now get the server's signon message */

	(void) biff_get_server(line, sizeof(line));
	return (atoi(line));
}

biff_put_server(string)
char *string;
{
	fprintf(biff_ser_wr_fp, "%s\r\n", string);
	(void) fflush(biff_ser_wr_fp);
}

biff_get_server(string, size)
char	*string;
int	size;
{
	register char *cp;
	char	*index();

	if (fgets(string, size, biff_ser_rd_fp) == NULL)
		return (-1);

	if ((cp = index(string, '\r')) != NULL)
		*cp = '\0';
	else if ((cp = index(string, '\n')) != NULL)
		*cp = '\0';

	return (0);
}

biff_close_server()
{
	char	biff_ser_line[256];

	if (biff_ser_wr_fp == NULL || biff_ser_rd_fp == NULL)
		return;

	biff_put_server("QUIT");
	(void) biff_get_server(biff_ser_line, sizeof(biff_ser_line));

	(void) fclose(biff_ser_wr_fp);
	(void) fclose(biff_ser_rd_fp);
	biff_ser_wr_fp = NULL;
	biff_ser_rd_fp = NULL;
}

char *
biff_getserverbyfile()
{
        char   *cp;
        static char     buf[256];

        if (cp = (char *)getenv("NNTPSERVER")) {
                (void) strcpy(buf, cp);
                return (buf);
        } else {
		return NULL;
	}
}

change_grp_to_isnewarticle(gname)
char    *gname;
{
        int     i;
 
        i = search_nbiffgrp(gname);
 
        if (i < 0){
                return 0;
        } else {
                return(last_article_data[i]);
        }
}

is_newarticle(i)
int	i;
{
	return(last_article_data[i]);
}

char	*
change_num_to_nbiffgrp(i)
int	i;
{
	char	*tmp;
	static char ntmp[256];
	extern char *return_newsbiffrc_data();

	if(return_newsbiffrc_data(i) == NULL){
		return NULL;
	}

	strncpy(ntmp, return_newsbiffrc_data(i), 255);

	tmp = strtok(ntmp, ":");

	if(tmp != NULL){
		return tmp;
	} else {
		return NULL;
	}
}

reset_last_article_data(i)
int	i;
{
	last_article_data[i] = 0;
}

set_nbiff_as_rescan()
{
	int	i,j;

        for(i=0; i<num_of_nbiffgrp; i++){
                if((j = is_newarticle(i)) > 0){
        		last_article_data[i] = -j;
                }
        }
}

search_nbiffgrp(group)
char	*group;
{
        int             j = 0, len;
        char            buf[MAXSTRLEN];

        sprintf(buf, "%s:", group);

        while (j < num_of_nbiffgrp) {
                len = strlen(buf);
                if (strncmp(newsbiffrc[j], buf, len) == 0) {
                        return (j);
                }
                j++;
        }
        return (-1);
}

char *
return_newsbiffrc_data(i)
int	i;
{
	return newsbiffrc[i];
}

void
read_newsbiffrc()
{
        FILE           *fp;
        char            tmpbuf[512];
        char            *buf;
        int             i = 0;

        if(*NEWSBIFFRC == NULL)
                sprintf(NEWSBIFFRC, "%s/.newsbiff", getenv("HOME"));

        /* Read .newsbiff */
        if ((fp = fopen(NEWSBIFFRC, "r")) == NULL) {
/* Users will worry about this message.
 *                fprintf(stderr, "SHINBUN : Can't open .newsbiff file\n");
 */
/* Bug: make .newsbiff automatically using .newsrc */
/*
		if ((fp = fopen(NEWSRC_FLIST_NAM, "r")) == NULL) {
			exit(1);
		} else {
*/
/* Users will worry about this message.
 *			fprintf(stderr,"SHINBUN : Use .newsrc as .newsbiff\n");
 */
/*
			;
		}
*/
		num_of_nbiffgrp = 0;
		return;

        }
        while ((buf = fgets(tmpbuf, sizeof(tmpbuf), fp)) != NULL) {
		rm_return(tmpbuf, buf);
		newsbiffrc[i] = (char *) malloc(strlen(tmpbuf) + 1);
                strcpy(newsbiffrc[i], tmpbuf);
                i++;
                if (i == MAX_OBSERVED_GROUPS){
/* Users will worry about this message.
 *                 	fprintf(stderr, 
 *				"SHINBUN : Too much lines in .newsbiff\n");
 */
                        break;
		}
        }
        fclose(fp);
        num_of_nbiffgrp = i;
}

clear_newsbiffrc()
{
	int	i;

	for(i=0; i < num_of_nbiffgrp; i++){
		free(newsbiffrc[i]);
	}
}
