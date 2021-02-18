/*
 * $Id: de_header.h,v 1.2 1991/09/16 21:31:16 ohm Exp $
 */
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/*	Version 4.0
 */
/*
	de_header.h
*/

#define	DEMON

#include "jslib.h"

#define FILENAME 128

#define CL_MAX 32
#define	ST_MAX 10
#define	MAX_ENV		256
#define	MAX_FILES	600
#define	MAX_DIC		1200

/* constants for one client */
#define USER_NAME_LEN 20
#define HOST_NAME_LEN 20
/* constants for one env. */
#define	FILE_NAME_L	100

/* value of file.localf */
#define	LOCAL	1
#define	REMOTE	0


/*
	Client
*/
struct cli{
	char user_name[USER_NAME_LEN];
	char host_name[HOST_NAME_LEN];
	int  env[WNN_MAX_ENV_OF_A_CLIENT];
};

typedef struct cli	CLIENT;

/*
	Environment for Conversion
 */
/*	jisho Table for an Environment		*/

/* ����ʸ������򤷤����� */

struct  GIJI {				/* ADD */
	int	eisuu;	/* Ⱦ��/���ѱѿ��� */
	int	number;	/* Ⱦ��/����/������ */
	int	kigou;	/* Ⱦ��/����(JIS/ASCII)���� */
};

struct cnv_env {
	char	env_name[WNN_ENVNAME_LEN];
	int	ref_count;
	int	sticky;		/* ���饤����Ȥ����ʤ��ʤäƤ⤽�δĶ���Ĥ� */
	int	nbun;		/* ��(��)ʸ����ϤΣ� */
	int	nshobun;	/* ��ʸ����ξ�ʸ��κ���� */
	int	hindoval;	/* ��������٤Υѥ�᡼�� */
	int	lenval;		/* ��ʸ��Ĺ�Υѥ�᡼�� */
	int	jirival;	/* ����Ĺ�Υѥ�᡼�� */
	int	flagval;	/* ���Ȥä���bit�Υѥ�᡼�� */
	int	jishoval;	/* ����Υѥ�᡼�� */
	int	sbn_val;	/* ��ʸ���ɾ���ͤΥѥ�᡼�� */
	int	dbn_len_val;	/* ��ʸ��Ĺ�Υѥ�᡼�� */
	int	sbn_cnt_val;	/* ��ʸ����Υѥ�᡼�� */

	int	suuji_val;	/* �����ʻ� ���������� */
	int	kana_val;	/* �����ʻ� ���ʤ����� */
	int	eisuu_val;	/* �����ʻ� �ѿ������� */
	int	kigou_val;	/* �����ʻ� ��������� */
	int	toji_kakko_val;	/* �����ʻ� �ĳ�̤����� */
	int	fuzokogo_val;	/* �����ʻ� ��°������� */
	int	kaikakko_val;	/* �����ʻ� ����̤����� */

	struct	GIJI	giji;	/* ADD */

	int	fzk_fid;

	int jishomax;
	int	jisho[WNN_MAX_JISHO_OF_AN_ENV];
	int	file[WNN_MAX_FILE_OF_AN_ENV];
};


/*
	structure of Dictionary
 */

struct wnn_dic {
	int	body;	/* fid */
	int	hindo;	/* fid */
	int	rw;
	int	hindo_rw;
	int	enablef;
	int	nice;
	int     rev;		/* reverse dict */
};

/*
	structure of File
 */

struct wnn_file {
	char	name[FILE_NAME_L];
	struct wnn_file_uniq f_uniq;
	struct wnn_file_uniq f_uniq_org;
	int	localf;
	int	file_type;
	int	ref_count;
	char	*area;
	char passwd[WNN_PASSWD_LEN];
};


/*
	external variables of demon
*/

extern CLIENT *client;	/* ask about Mr. Takeoka */
extern int max_client;
extern CLIENT *c_c;		/* this means current client */ 
extern int cur_client;
extern int clientp;

/*	env,dic,file	*/
extern struct cnv_env *c_env; 

extern	struct	cnv_env	*env[];
extern	struct	wnn_file	files[];
extern struct	wnn_dic	dic_table[];

extern int max_sticky_env;
extern struct cnv_env *s_env[];

extern int wnn_errorno;

extern int noisy;

extern char jserver_dir[];
extern char jserverrcfile[];

extern char SER_VERSION[];

extern char *hinsi_file_name;


