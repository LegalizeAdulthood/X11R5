/*
 * $Id: buffer.h,v 1.2 1991/09/16 21:33:25 ohm Exp $
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
/* buffer��ɽ���Τ������ϡ�hanten�ˤ�äƷ��ꤵ��롣
ȿž���뤫���������֤ˤ��뤫�����Ω�ˡ�
�Хåե�������ǡ�ʸƬ����mark�ޤǡ�mark����cursor�ޤǡ�
cursor����buffer�κǸ�ޤǤξ��֤���Ω�����ꤹ�뤳�Ȥ��Ǥ��롣
	 when mark is set
        	hanten bit0(0x01),bit1(0x02):      --- mark
		bit2(0x04),bit3(0x08):        mark --- cursor
		bit4(0x10),bit5(0x20):      cursor --- 
	  when mark is not set(-1)
		bit4,bit5:      
		bit6(0x40):	t_b_st --- t_b_end �ܡ����ɽ��
    first one....hanten
    second one...under_line
*/
struct buf{
  w_char  *buffer;	/* ��Ͽ�ѥХåե� */
  char *buf_msg; /** ��̾�����Ϥ�����ΥХåե���������ɽ������ʸ���� */
  int start_col; /** ��̾���ϻ��β��̾�Υ����Υ������Ȱ��� */
  int t_c_p; /** ��̾�����ѥХåե�����ǤΥ���������� */
  int hanten; /** ��̾�����ѥХåե�����ȿžɽ�������Ƥ��뤫�ɤ��� */
  int maxlen; /**��̾�����ѥХåե�������ˤϤ��äƤ���ʸ���� */
  int buflen; /** length of the buffer */
  int t_m_start;  	/* -1 : mark�����ꤵ��Ƥ��ʤ���*/
			/* 0 �ʾ� : �ޡ����ΰ���*/
  int t_b_st;	/**�ܡ����ɽ���γ��ϰ��� */
  int t_b_end;	/**�ܡ����ɽ���ν�λ���� */
  int vlen;    /*�Хåե�����ǥ����꡼���ɽ���������*/
  int duplicate;/* �����꡼�󤬥������뤹����ˡ���ʣ����ɽ������ʸ����*/

  int vst;    /* buffer����ǲ��̤�ɽ������Ƥ���ǽ�ΰ���*/
		/*����ϡ�����˾�Υ롼����Ǥ�����ʤ�����˾�ޤ�����*/
  int (*key_in_fun)(); /*�����˴ؿ������ꤵ��Ƥ���ȡ�key_table�˥Х���ɤ�
  �ʤ�ʸ�������Ϥ�
  ���������˥Хåե����ˤ���ʸ�������줺������ˤ��δؿ����ƤФ��*/
  int (*redraw_fun)();/* romkan kara redraw ga kaette kitatoki */
  int (*ctrl_code_fun)();/*�����˴ؿ������ꤵ��Ƥ���ȡ�key_table�˥Х���ɤ�
  �ʤ�����ȥ���ʸ�������Ϥ���������˥٥���Ĥ餹�����
  ���δؿ����ƤФ��*/
  int (**key_table)();	/*�����Х���ɤΥơ��֥�*/
  char *rk_clear_tbl;	/* romakn_clear �� hituyouka */
};


extern int *bunsetsu;
extern int *bunsetsuend;
extern struct wnn_env **bunsetsu_env;	/* ʸ����Ѵ��˻Ȥä� env */
extern int *touroku_bnst;
/* extern int bunsetsucnt; */
extern int touroku_bnst_cnt;

extern struct buf *c_b; /* �����Ѥ����Ƥ���buf�򼨤������Х��ѿ�*/
extern w_char *kill_buffer; /* ����Хåե����礭���ϡ�maxchg�ȤʤäƤ���*/
extern int kill_buffer_offset;/* ����Хåե����Ȥ��Ƥ���Ĺ��*/
extern w_char *remember_buf; /* ����Х��Хåե��礭���ϡ�maxchg�ȤʤäƤ���*/

