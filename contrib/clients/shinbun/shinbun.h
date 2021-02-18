#ifdef sccs
static  char sccsid[] = "@(#)shinbun.h	1.6 91/07/24";
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


#define MAXNGRP 	4096
#define AT_READ 	0x00000001
#define AT_SELECT	0x00000010
#define AT_SELECT_MANY	0x00000020
#define AT_CANCEL	0x00000100
#define AT_NEW		0x00001000
#define AT_MKURD	0x00010000

typedef struct na_info *NA_INFO;
typedef struct na_info NAX_INFO;
typedef struct ng_info NG_INFO;
typedef struct hd_info HD_INFO;

typedef struct hd_info {    
	int     status;
	char    from[256];
	char    newsgroup[256];
	char    subject[256];
	char    reply_to[256];
	char    followup_to[256];
	char    message_id[256];
	char    distribution[256];
	char    reference[256];
};

struct na_info {
	int	line;
	int	anum;
	int	status;
	NA_INFO	next;
	NA_INFO	prev;
	char	*subject;
	char	*sender;
	char	*time;
};

typedef struct ng_info {
	int	first_article;
	int	last_article;
	int	num_article;
	int	num_unread;
	char    *groupname;
	char	*read_status;
	NA_INFO	first;
};

extern NG_INFO group_info;
extern char *get_current_newsgroup();
extern void write_to_newsrc();
extern char *nntp_group();
extern char *change_line_to_subject();
extern char *change_line_to_sender();
extern char *change_line_to_time();
extern char *change_line_to_header();
extern char *get_mail_address();
extern char *get_mail_path();
extern HD_INFO *nntp_get_header_info();
extern char *return_dir_of_newsrc();
extern char *return_file_of_newsrc();
extern char *get_a_line();
extern char *return_newsrc_data();
