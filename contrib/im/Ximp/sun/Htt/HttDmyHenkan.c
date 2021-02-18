/*
 *        %Z%%M% %I% %E%  
 * 
 * Derived from @(#)DmyHenkan.c	2.1 91/08/13 11:28:26 FUJITSU LIMITED.
 */
/*
****************************************************************************

              Copyright 1991, by FUJITSU LIMITED

              Copyright 1991, by Sun Microsystems, Inc.	      

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of FUJITSU LIMITED and Sun
Microsystems, Inc. not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.  FUJITSU LIMITED and Sun Microsystems, Inc. makes no
representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.

FUJITSU LIMITED AND SUN MICROSYSTEMS, INC. DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL FUJITSU LIMITED AND SUN
MICROSYSTEMS, INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Auther: Takashi Fujiwara	FUJITSU LIMITED
				fujiwara@a80.tech.yk.fujitsu.co.jp
	Kiyoaki oya		FUJITSU LIMITED
	Riki	Kawaguchi	FUJITSU LIMITED
	Tomoki	Kuriki		FUJITSU LIMITED
	Hideki  Hiura           (hhiura@Sun.COM) Sun Microsystems, Inc.

****************************************************************************
*/

#ifndef lint
static char     derived_from_sccs_id[] = "@(#)DmyHenkan.c	2.1 91/08/13 11:28:26 FUJITSU LIMITED.";
static char     sccsid[] = "%Z%%M% %I% %E% Sun Microsystems Inc.";
#endif

#include "HttKanjiTbl.h"
#include "HttHenkan.h"
typedef unsigned short ushort;
typedef unsigned char uchar;

#ifdef DEBUG
#define Static			/**/
#else
#define Static static
#endif

#define nop	0

int             Dmy_Henkan(),
                Dmy_Zenkouho(),
                Dmy_HenkanCancel();

DictionaryFuncRec dmy_dictionary_func = {
    (char *) 0,
    nop,
    nop,
    nop,
    nop,
    Dmy_Henkan,
    Dmy_Zenkouho,
    Dmy_HenkanCancel
};

Static int      _mu_henkan_count = 0;
Static ushort  *_dic_old_buff = 0;

Dmy_Henkan(cnvrt, count, yomi, yomi_len, out_buff, out_size)
    uchar           cnvrt;
    int             count;
    ushort         *yomi;
    int             yomi_len;
    ushort         *out_buff;
    int             out_size;
{
    int             num;
    if (count == 0) {
	if (cnvrt == FKK_MAE) {
	    return (0);
	}
	if (_dic_old_buff != 0) {
	    Dic_HenkanCancel();
	}
	_mu_henkan_count = 0;
    }
    _dic_old_buff = yomi;
    num = _Dmy_Muhenkan(yomi, yomi_len, out_buff, out_size);
    return (num);
}

int
Dmy_Zenkouho(type, yomi, yomi_len)
    int             type;
    ushort         *yomi;
    int             yomi_len;
{
    int             i, ret, num, out_size = 60, no, r_len;
    ushort          out_buff[60];

    InitialKanjiItem();
    if (_dic_old_buff != yomi) {
	Dic_HenkanCancel();
	_dic_old_buff = yomi;
    }
    for (i = 3; i > 0; i--) {
	num = _Dmy_Muhenkan(yomi, yomi_len, out_buff, out_size);
	ret = AddKanjiItem(num, out_buff);
	if (ret == -1)
	    return (-1);
    }
    return (select_kanji_item_no);
}

int
Dmy_HenkanCancel()
{
    if (_dic_old_buff != (ushort *) 0) {
	_dic_old_buff = (ushort *) 0;
	_mu_henkan_count = 0;
    }
}

_Dmy_Muhenkan(yomi, yomi_length, out_buff, out_size)
    ushort         *yomi, *out_buff;
    int             yomi_length, out_size;
{
    int             num = 0;
    ushort          save;
    save = yomi[yomi_length];
    yomi[yomi_length] = 0;
    if (_mu_henkan_count == 0) {
	_mu_henkan_count = 1;
	num = CopyUshort(out_buff, yomi, out_size);
    } else if (_mu_henkan_count == 1) {
	_mu_henkan_count = 2;
	num = Hiragana_To_Katakana(out_buff, yomi, out_size);
    } else {
	_mu_henkan_count = 0;
	num = ZenkakuAsciiKana_To_Hankaku(out_buff, yomi, out_size, 1);
    }
    yomi[yomi_length] = save;
    return (num);
}
