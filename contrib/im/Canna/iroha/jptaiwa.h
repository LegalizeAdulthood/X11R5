/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */
/* "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/jptaiwa.h,v 3.1 91/07/01 15:09:40 kon Exp $" */
/* "@(#)jptaiwa.h	R14.2.0.0 88/02/19 09:23:58 " */
/************************************************************************/
/*	Ϣʸ�ᥫ�ʴ����Ѵ��������Ѵ���ǽ�����				*/
/************************************************************************/

/************************************************************************/
/* 	���ý����⡼�� 							*/
/************************************************************************/

#define	DATAIN		0		/* ���ʴ����Ѵ�			*/
#define	TAIWAINIT	1		/* �Ѵ�����ν����		*/
#define	TAIWAAUTO	2		/* ���ý����γ���		*/
#define	TAIWAEND	3		/* ���ý����ν�λ		*/

/************************************************************************/
/*	���ϥ��ݥ��ݥɡ����業�ݡ�					*/
/************************************************************************/

#define KANJI		0x01		/* �������Ͻ�λ			*/
#define HEXCODE		0x02		/* �����ʥ��ݥ����ϳ���		*/
#define BUSHU		0x03		/* ���������Ѵ����ϡ���λ	*/
#define CHGROMA        	0x04		/* ��ݥ޻������������ڤ��ؤ�	*/
#define CHGHENKAN	0x05		/* �Ѵ���ݥ��ڤ��ؤ�		*/
#define CHGKATAKANA    	0x06		/* �������ʡ��Ҥ餬���ڤ��ؤ�	*/
#define CHGJISYO	0x07		/* �����ѹ�			*/
#define LEFT        	0x08		/* ��Сݥ�ɽ������ư�ʽ̾���	*/
#define RIGHT       	0x09		/* ��Сݥ�ɽ������ư�ʳ���)	*/
#define REVLEFT		0x0a		/* ��Сݥ�ʸ�Ẹ��ư		*/
#define MUHENKAN	0x0b		/* ��Сݥ�ʸ�ᱦ��ư��̵�Ѵ���	*/
#define HENKAN		0x0c		/* �����Ѵ�			*/
#define KAKUTEI    	0x0d		/* ��ʸ�����			*/
#define TORIKESI	0x0e		/* ���ä�			*/
#define TAB         	0x0f		/* ����				*/
#define BACKSPACE   	0x10		/* �Хå����ڡݥ�		*/
#define REDRAW      	0x11
#define TOHIRAGANA	0x12		/* ��Сݥ���ʬ�ΤҤ餬���Ѵ�	*/
#define TOKATAKANA	0x13		/* ��Сݥ���ʬ�Υ��������Ѵ�	*/

/************************************************************************/
/*	�����Ѵ���ǽ�ᥤ�󥹥ơݥ����ʽ�����ݥɤ�DATAIN�λ�������͡�	*/
/************************************************************************/

#define	MODE_CHG	1		/* ��ݥ��Ѳ�			*/
#define	MIDASI		2		/* ���ʸ��Ф�������		*/
#define RET_KEY		3		/* �ü쥭������			*/
#define	HEX_IN		4 		/* �����ʥ��ݥ�������		*/
#define	DOUON		5		/* Ʊ�����������		*/
#define	MUSI	        6 		/* ̵������			*/
#define	BUSHU_IN	7		/* �����Ѵ�������		*/
#define	KIGOU_IN	8		/* ���������			*/
#define	TAN_TOUROKU_IN	9		/* ñ����Ͽ��			*/
#define DIOERR          -1		/* ���񥢥����������		*/

/************************************************************************/
/* 	�ƥᥤ�󥹥ơݥ������Ф��륵�֥��ơݥ���  			*/
/************************************************************************/

/*	�ᥤ�󥹥ơݥ������ᡡMODE_CHG					*/
#define	KANA_M		1		/* �Ҥ餬�ʡ��������ʥ�ݥ��Ѳ�	*/
#define	HENKAN_M	2		/* �����Ѵ���ݥ��Ѳ�		*/
#define	HEX_M		3		/* �ʣɣӥ��ݥ����ϥ�ݥ�	*/
#define	BUSHU_M		4		/* �����Ѵ����ϥ�ݥ�		*/
#define	KANJI_M		5		/* ���ܸ����Ͻ�λ		*/
#define	ROMA_M		6		/* ��ݥ޻����������ϥ�ݥ��Ѳ�	*/
#define	JISHO_M		7		/* �����ڤ��ؤ�			*/
#define	ZENHIRAKAKU_M	8		/* ���ѤҤ餬�ʳ������ϥ⡼��   */
#define	ZENKATAKAKU_M	9		/* ���ѥ������ʳ������ϥ⡼��   */
#define	HANKATAKAKU_M	10		/* Ⱦ�ѥ������ʳ������ϥ⡼��   */
#define	ZENALPHAKAKU_M	11		/* ���ѥ���ե��٥åȳ������ϥ⡼�� */
#define	HANALPHAKAKU_M	12		/* Ⱦ�ѥ���ե��٥åȳ������ϥ⡼�� */
#define	KIGOU_M		13		/* �������ϥ⡼�� */
#define	TAN_TOUROKU_M	14		/* ñ����Ͽ�⡼�� */
#define	TAN_TOUROKU_HIN_M	15	/* ñ����Ͽ�ʻ�����⡼�� */
#define	TAN_TOUROKU_DIC_M	16	/* ñ����Ͽ��������⡼�� */

/*	�ᥤ�󥹥ơݥ������ᡡRET_KEY					*/
#define	KEY_KAKUTEI	0		/* ���ꥭ��			*/
#define	KEY_TAB		1		/* ���֥���			*/
#define	KEY_BS		2		/* �Хå����ڡݥ�����		*/
#define	KEY_HENKAN	3		/* �Ѵ�����			*/
#define KEY_REDRAW	4

/*	�ᥤ�󥹥ơݥ������ᡡHEX_IN					*/
#define	HEX_ERR		1		/* ���ϥ���ݡ����ä�		*/
#define	HEX_POS		2		/* �����ʿ�������		*/
#define	HEX_TRAN	3		/* �Ѵ������ʽ�λ��		*/
 
/*	�ᥤ�󥹥ơݥ������ᡡDOUON					*/
#define	D_HYOUJI	0		/* Ʊ�������ɽ��		*/
#define	D_END		1		/* Ʊ��������λ		*/

/*	�ᥤ�󥹥ơݥ������ᡡBUSHU_IN					*/
#define	BU_MIDASI	1		/* ���󸫽Ф�������		*/
#define	BU_KOUHO	2		/* ����ɽ��			*/
#define	BU_END		3		/* ���ϳ���ʽ�λ��		*/

/*	�ᥤ�󥹥ơݥ������ᡡKIGOU_IN					*/
#define	KI_KOUHO	1		/* �������ɽ����		*/
#define	KI_END		2		/* ���ϳ���ʽ�λ��		*/

/*	�ᥤ�󥹥ơݥ������ᡡTAN_TOUROKU_IN				*/
#define	ADD_INPUT	1		/* ������			*/
#define	ADD_END		2		/* ��Ͽ��λ			*/

/************************************************************************/
/* 	���ʴ����Ѵ��⡼�� 						*/
/************************************************************************/

#define	CHIKUJI		0		/* �༡��ư�Ѵ�			*/
#define	RENBUN		1		/* Ϣʸ���Ѵ�			*/
#define	TANBUN		2		/* ñʸ���Ѵ�			*/

/************************************************************************/
/* 	�Ҥ餬�ʡ��������ʥ⡼��					*/
/************************************************************************/

#define	IN_HIRAGANA	0		/* �Ҥ餬������(�Ѵ�����)	*/
#define	IN_KATAKANA	1		/* ������������(��������)	*/
#define	IN_ALPHA	2		/* ����ե��٥å�����(��������)	*/

/************************************************************************/
/* 	��ݥ޻����������ϥ�ݥ�					*/
/************************************************************************/

#define IN_ROMAJI	-1		/* ��ݥ޻�����			*/
#define IN_KANA		0		/* ��������			*/

/************************************************************************/
/* 	����								*/
/************************************************************************/

#include <jpdef.h>

/************************************************************************/
/* 	����¾								*/
/************************************************************************/

#define	KOUHO_SIZE	128 		/* Ʊ����ɽ������礭���κ���	*/
#define MIN_KOUHO_SIZE  34		/* Ʊ����ɽ������礭���κǾ�	*/
#define MIN_BUSHU_KOUHO_SIZE	4	/* �������ɽ������礭���κǾ�	*/

/************************************************************************/
/* 	Ϣʸ��饤�֥�����						*/
/************************************************************************/

#include <japanren.h>

/************************************************************************/
/* 	���ý�������ơݥ֥�						*/
/************************************************************************/

struct taiwaInf	{
	unsigned char	*message;	/* ��å��ݥ��Хåե�		*/
	int		messageLength;	/* ��å��ݥ�Ĺ			*/
	unsigned char	*kakutei;	/* ����ʸ����Хåե�		*/
	int		kakuteiLength;	/* ����ʸ����Ĺ			*/
	int		reversePosition;/* ��å��ݥ��Υ�Сݥ�ɽ������	*/
	int		reverseLength;	/* ��å��ݥ��Υ�Сݥ�ɽ��Ĺ	*/
	int		subStatus;	/* ���֥��ơݥ���		*/
	int		henkanMode;	/* ���ʴ����Ѵ���ݥ�		*/
	int		kanaMode;	/* �Ҥ餬�ʡ������������ϥ�ݥ�	*/
	int		romajiMode;	/* ��ݥ޻����������ϥ�ݥ�	*/
	int		jisyo;		/* �����ȼ���			*/
	int		kouhoSize;	/* Ʊ�������ɽ������礭��	*/
	unsigned char	*jrdic;		/* ���ѼԻ���Υ����ƥ༭��	*/
	unsigned char	*jgdic;		/* ���ѼԻ��������켭��	*/
	unsigned char	*judic;		/* ���ѼԻ���θĿͼ���		*/
};
