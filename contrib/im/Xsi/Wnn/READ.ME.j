/*
 * $Id: READ.ME.j,v 1.3 1991/09/18 05:36:28 proj Exp $
 */

��	�ף�̾�����Ѵ������ƥ� �С������4.1

�ף�̾�����Ѵ������ƥ�ϡ�������ؿ������ϸ���ꡢ�����������ҡ�
������ҥ����ƥå��Σ��Ԥˤ�äƳ�ȯ���줿�ͥåȥ����̾�����Ѵ�����
�ƥ�Ǥ���

����ϡ��ף�̾�����Ѵ������ƥ�ΥС������4.1�Ǥ���

����Υ�꡼���Ǥϡ��ʲ��ε�����ư���ǧ����Ƥ��ޤ���

SUN �� OS4.0.3/4.1.1

OMRON LUNA �� UniOS-B (BSD4.3)��UniOS-U (SystemV R2.1)��UniOS-Mach (MACH)




��	���󥹥ȡ���

 �������ե�����ˤϡ���ƥ��ʸ���󡢤���ӡ������Ȥ���ECU�����ɤǽ񤫤��
 ���ޤ�������ѥ���κݤˤϡ�8�ӥåȤ��̤륳��ѥ���ǡ�make ���Ʋ�������

 ��Imake�ǥ���ѥ���

	cd ..
	make World -f Makefile.ini

 ��Imake�ǥ��󥹥ȡ���

    Wnn�δĶ����٤Ƥ򥤥󥹥ȡ��뤹����
	make install

    Wnn�Υ�����¦�������󥹥ȡ��뤹����
	make instserver
    (jserver, jserverrc, hinsi.data, pubdic)

    Wnn�Υ��饤�����¦�������󥹥ȡ��뤹����
	make instclient
    (uum, jutil, configuration files)

    Wnn�Υ饤�֥��򥤥󥹥ȡ��뤹����
	make instlib
    (libwnn.a, include files)

 
 Project.tmpl ($(TOP)/contrib/im/Xsi/config�ˤ���) �� JWNNBINDIR��WNNWNNDIR��
 JWNNPUBDICDIR ������ޤ��� jserver, uum, ����桼�ƥ���ƥ��ʤɤΥ��ޥ��
 (JWNNBINDIR)������ե�����(WNNWNNDIR)���У������(JWNNPUBDICDIR)����
 ���󥹥ȡ��뤵���ǥ��쥯�ȥ꡼�Ǥ���
 ��������LIBDIR �ϡ�include/config.h �ˡ��ѥ�̾���񤫤�Ƥ��ޤ��Τǡ�
 ��������Ѥ��Ƥ�ư���ޤ���
 �ǥե���ȤǤϡ����줾�졢/usr/local/bin/Wnn4, /usr/local/lib/wnn/ja_JP,
 /usr/local/lib/wnn/ja_JP/dic/pubdic �ˤʤäƤ��ޤ���

 Project.tmpl ($(TOP)/contrib/im/Xsi/config�ˤ���) �� WNNOWNER ������ޤ���
 ����ϡ�Wnn �Υץ����(uum �����) ��ɤΥ桼���θ��¤�ư����������
 ���ޤ����С������3�ޤǤǤϡ�jserver �ϡ�root �θ��¤�ư�����Ƥ����Τ�
 ������root ��ư�����Τϴ������⤤���ᡢwnn �ѤΥ桼�����äơ�����
 ���¤� jserver ������ư������ �Ȥ�侩���ޤ���
 �ǥե���ȤǤϡ�wnn �ˤʤäƤ��ޤ���

 uum �ϡ������ʤ� root �ǡ� s�ӥåȤ�Ω�äƤ���ɬ�פ�����ޤ���

 ����Ǥϡ��������ѥ��ˡ�/usr/local/bin/Wnn4 ���ɲä��ơ�jserver ����ӡ�uum 
 ��ȤäƤߤƲ�������



��	manual �ʳ��Υǥ��쥯�ȥ�ϡ��ʲ����̤�Ǥ���

conv
	����С��ȡ�����(�ե��󥯥���󥭡����Ǥ����������Ŭ���ʥ���
	�ɤ��Ѵ�����)�Υ�������¸�ߤ��ޤ���
etc
	ʣ���Υǥ��쥯�ȥ�Ƕ�ͭ����륽�����ե����뤬¸�ߤ��ޤ���
fuzokugo
	��°��ե���������ץ����(atof)������ӡ�pubdic ����°��ե����뤬
	¸�ߤ��ޤ���
include 
	�إå����ե����뤬¸�ߤ��ޤ���
jd
	uum, jserver, ���޻���̾�Ѵ��ʤɤν�����ե����롢
	�ʻ�ե����뤬¸�ߤ��ޤ������Υǥ��쥯�ȥ�����Ƥϡ�
	/usr/local/lib/wnn/ja_JP �˥��󥹥ȡ��뤵��ޤ���

	    jserverrc - jserver ��Ω��������ե�����Ǥ���
	    uumrc	uum ��Ω��������ե�����Ǥ���
	    uumkey	uum �Υ����Х��������ե�����Ǥ���
	    wnnenvrc	uum ���ѻ��μ�����������ե�����Ǥ���
	jd �β��ˤϡ������������Τ�Τ��Ѱդ���Ƥ��ޤ���
	���ߤ˹�碌�Ƹ����Ѳ�������

jlib
	�饤�֥��Υ�������¸�ߤ��ޤ���
jlib.V3
	�С������3���Ф���ߴ��饤�֥��Υ�������¸�ߤ��ޤ���
jserver
	jserver �Υ�������¸�ߤ��ޤ���
jutil
	����桼�ƥ���ƥ��Υ�������¸�ߤ��ޤ���
		atod	 ����κ���
		dtoa	 ����Υƥ����Ȥؤ��ѹ�
		oldatonewa �С�����󣳤ޤǤμ���Υƥ����ȷ�����
			�С�����󣴤Υƥ����ȷ������ѹ�
		wnnstat	 jserver �ξ���(�桼�����Ķ����ե����롢����)��Ĵ�٤롣
		wnnkill	 jserver ��λ�����롣
		wnntouch ����ե�����ʤɤΥե������FID �����ե�����Υإå���
			���äƤ����ΤȰۤʤ���˰��פ����롣
		wddel	ñ������Хå�Ū�˹Ԥʤ�
		wdreg	ñ����Ͽ��Хå�Ū�˹Ԥʤ�
pubdic 			
	pubdic �μ���¸�ߤ��ޤ���
	����ϡ� �հ��������Ǻ�äƤ���ޤ���
romkan
	���޻���̾�Ѵ��Υ�������¸�ߤ��ޤ���
uum
	�ե��ȥ���ɡ��ץ��å� uum �Υ�������¸�ߤ��ޤ���

-----------------------------------------------------------------------
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
