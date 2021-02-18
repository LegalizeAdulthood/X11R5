/*
 * $Id: jlib.h,v 1.2 1991/09/16 21:30:30 ohm Exp $
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
	Nihongo	Henkan	Library Header File
*/
/*
#include <setjmp.h>
*/


#define	w_char	unsigned short

#define JLIB

/* header file for dic_syurui */
#define NO_EXIST	-1
#define NOT_A_JISHO	0
#define USER_DIC	1
#define SYSTEM_DIC	3

extern	jmp_buf	jd_server_dead_env;	/* �����Ф�������������Ǥ���env */
extern	int	jd_server_dead_env_flg; /* jd_server_dead_env��ͭ�����ݤ�  */
extern	int	wnn_errorno;		/* Wnn�Υ��顼�Ϥ����ѿ�����𤵤�� */


struct	kouho_entry {
	int	s_ichi;		/*	���ʥХåե��ؤΥ��ե��å�	*/
	int	jl;		/*	��Ω���Ĺ��			*/
	int	fl;		/*	��°���Ĺ��			*/
	int	pl;		/*	�ǥ����ץ쥤����Ĺ��		*/
	int	jishono;	/*	��Ω��μ����ֹ�		*/
	int	serial;		/*	��Ω����ֹ�			*/
	w_char	*k_data;	/*	�����Хåե��ؤΥݥ���	*/
};
#define	KOUHO_ENT	struct	kouho_entry


struct	jikouho_entry {
	int	jl;		/*	��Ω���Ĺ��			*/
	int	jishono;	/*	��Ω��μ����ֹ�		*/
	int	serial;		/*	��Ω����ֹ�			*/
	w_char	*k_data;	/*	�����Хåե��ؤΥݥ���	*/
};
#define	JIKOUHO_ENT	struct	jikouho_entry


struct	bunjoho {
	w_char	*kana_buf;	/*	���ʥХåե�			*/
	int	kana_size;	/*	���ʥХåե���Ĺ��		*/
	KOUHO_ENT *klist;	/*	����ꥹ�ȡ��Хåե�		*/
	int	klist_size;	/*	����ꥹ�ȡ��Хåե���Ĺ��	*/
	w_char	*kanji_buf;	/*	�����Хåե�			*/
	int	kanji_buf_size;	/*	�����Хåե� ��Ĺ��		*/
};


struct	jikouhojoho{
	JIKOUHO_ENT *jlist;	/*	������Хåե�			*/
	int	jlist_size;	/*	������Хåե���Ĺ��		*/
	w_char	*kanji_buf;	/*	�����Хåե�			*/
	int	kanji_buf_size;	/*	�����Хåե���Ĺ��		*/
};
#define	JIKOUHOJOHO 	struct	jikouhojoho


struct	jishojoho{
	w_char	*k_data;	/*	�����Хåե��ؤΥݥ���	*/
	int	bumpo;		/*	ʸˡ����			*/
	int	hindo;		/*	����				*/
	int	jisho;		/*	�����ֹ�			*/
	int	serial;		/*	�����ֹ�			*/
};
#define	JISHOJOHO	struct	jishojoho


struct dicinfo{
	int	dic_no;		/*	�����ֹ�			*/
	int	dic_size;	/*	ñ���				*/
	int	ttl_hindo;	/*	�������			*/
	int	dic_type;	/*	1:�桼��,2:�����ƥ�		*/
	int	udp;		/*	0:������Ǥʤ�,1:������		*/
	int	prio;		/*	����ץ饤����ƥ�		*/
	int	rdonly;		/*	0:�饤�Ȳ�ǽ,1:�꡼�ɥ���꡼	*/
	char	*file_name;	/*	����ե�����̾			*/
	char	*h_file_name;	/*	���٥ե�����̾			*/
};
#define	DICINFO		struct	dicinfo


struct jwho {
    int sd;	  	/* socket discripter in jserver */
    char *user_name;	/* user name */
    char *host_name;	/* host name */
};
#define JWHO		struct jwho
