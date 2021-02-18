/*
 *        %Z%%M% %I% %E%  
 * 
 * Derived from  @(#)ToUtil.c	2.1 91/08/13 11:28:39 FUJITSU LIMITED. */
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
static char     derived_from_sccs_id[] = "@(#)ToUtil.c	2.1 91/08/13 11:28:39 FUJITSU LIMITED.";
static char     sccsid[] = "%Z%%M% %I% %E% Sun Microsystems Inc.";
#endif

#ifdef	XSF_X11R5
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <widec.h>
#endif	/* XSF_X11R5 */

typedef unsigned short ushort;
typedef unsigned char uchar;


#define _NONE_	0x0000

static
ushort          jefA1toascii[] = {	/* special char. */
    _NONE_, 0x0020, 0x00A4, 0x00A1, 0x002C, 0x002E, 0x00A5, 0x003A,	/* A1A0-A1A7 */
    0x003B, 0x003F, 0x0021, 0x00DE, 0x00DF, _NONE_, 0x0060, _NONE_,	/* A1A8-A1AF */
    0x005E, _NONE_, 0x005F, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* A1B0-A1B7 */
    _NONE_, _NONE_, _NONE_, _NONE_, 0x00B0, _NONE_, _NONE_, 0x002F,	/* A1B8-A1BF */

    _NONE_, 0x007E, _NONE_, 0x007C, _NONE_, _NONE_, 0x0060, 0x0027,	/* A1C0-A1C7 */
    0x0022, 0x0022, 0x0028, 0x0029, 0x005B, 0x005D, 0x005B, 0x005D,	/* A1C8-A1CF */
    0x007B, 0x007D, _NONE_, _NONE_, _NONE_, _NONE_, 0x00A2, 0x00A3,	/* A1D0-A1D7 */
    _NONE_, _NONE_, _NONE_, _NONE_, 0x002B, 0x002D, _NONE_, _NONE_,	/* A1D8-A1DF */

    _NONE_, 0x003D, _NONE_, 0x003C, 0x003E, _NONE_, _NONE_, _NONE_,	/* A1E0-A1E7 */
    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, 0x005C,	/* A1E8-A1EF */
    0x0024, _NONE_, _NONE_, 0x0025, 0x0023, 0x0026, 0x002A, 0x0040,	/* A1F0-A1F7 */
    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* A1F8-A1FF */
};

static
ushort          jefA3toascii[] = {	/* ascii	 */
    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* A3A0-A3A7 */
    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* A3A8-A3AF */
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,	/* A3B0-A3B7 : 0-7 */
    0x0038, 0x0039, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* A3B8-A3BF : 8 9 */

    _NONE_, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,	/* A3C0-A3C7 : A-G */
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,	/* A3C8-A3CF : H-O */
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,	/* A3D0-A3D7 : P-W */
    0x0058, 0x0059, 0x005A, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* A3D8-A3DF : X-Z */

    _NONE_, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,	/* A3E0-A3E7 : a-g */
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,	/* A3E8-A3EF : h-o */
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,	/* A3F0-A3F7 : p-w */
    0x0078, 0x0079, 0x007A, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* A3F8-A3FF : x-z */
};

static
ushort          jefA4toascii[] = {	/* kana		 */
    _NONE_, 0x00A7, 0x00B1, 0x00A8, 0x00B2, 0x00A9, 0x00B3, 0x00AA,	/* A4A0-A4A7 */
    0x00B4, 0x00AB, 0x00B5, 0x00B6, 0xDEB6, 0x00B7, 0xDEB7, 0x00B8,	/* A4A8-A4AF */
    0xDEB8, 0x00B9, 0xDEB9, 0x00BA, 0xDEBA, 0x00BB, 0xDEBB, 0x00BC,	/* A4B0-A4B7 */
    0xDEBC, 0x00BD, 0xDEBD, 0x00BE, 0xDEBE, 0x00BF, 0xDEBF, 0x00C0,	/* A4B8-A4BF */

    0xDEC0, 0x00C1, 0xDEC1, 0x00AF, 0x00C2, 0xDEC2, 0x00C3, 0xDEC3,	/* A4C0-A4C7 */
    0x00C4, 0xDEC4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA,	/* A4C8-A4CF */
    0xDECA, 0xDFCA, 0x00CB, 0xDECB, 0xDFCB, 0x00CC, 0xDECC, 0xDFCC,	/* A4D0-A4D7 */
    0x00CD, 0xDECD, 0xDFCD, 0x00CE, 0xDECE, 0xDFCE, 0x00CF, 0x00D0,	/* A4D8-A4DF */

    0x00D1, 0x00D2, 0x00D3, 0x00AC, 0x00D4, 0x00AD, 0x00D5, 0x00AE,	/* A4E0-A4E7 */
    0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DC,	/* A4E8-A4EF */
    _NONE_, _NONE_, 0x00A6, 0x00DD, _NONE_, _NONE_, _NONE_, _NONE_,	/* A4F0-A4F7 */
    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* A4F8-A4FF */
};

static
ushort          jefA5toascii[] = {	/* katakana	 */
    _NONE_, 0x00A7, 0x00B1, 0x00A8, 0x00B2, 0x00A9, 0x00B3, 0x00AA,	/* A5A0-A5A7 */
    0x00B4, 0x00AB, 0x00B5, 0x00B6, 0xDEB6, 0x00B7, 0xDEB7, 0x00B8,	/* A5A8-A5AF */
    0xDEB8, 0x00B9, 0xDEB9, 0x00BA, 0xDEBA, 0x00BB, 0xDEBB, 0x00BC,	/* A5B0-A5B7 */
    0xDEBC, 0x00BD, 0xDEBD, 0x00BE, 0xDEBE, 0x00BF, 0xDEBF, 0x00C0,	/* A5B8-A5BF */

    0xDEC0, 0x00C1, 0xDEC1, 0x00AF, 0x00C2, 0xDEC2, 0x00C3, 0xDEC3,	/* A5C0-A5C7 */
    0x00C4, 0xDEC4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA,	/* A5C8-A5CF */
    0xDECA, 0xDFCA, 0x00CB, 0xDECB, 0xDFCB, 0x00CC, 0xDECC, 0xDFCC,	/* A5D0-A5D7 */
    0x00CD, 0xDECD, 0xDFCD, 0x00CE, 0xDECE, 0xDFCE, 0x00CF, 0x00D0,	/* A5D8-A5DF */

    0x00D1, 0x00D2, 0x00D3, 0x00AC, 0x00D4, 0x00AD, 0x00D5, 0x00AE,	/* A5E0-A5E7 */
    0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DC,	/* A5E8-A5EF */
    _NONE_, _NONE_, 0x00A6, 0x00DD, 0xDEB3, 0x00B6, 0x00B9, _NONE_,	/* A5F0-A5F7 */
    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* A5F8-A5FF */
};

static
ushort          asciitojef[] = {
    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* 00-07 */
    _NONE_, 0x0009, 0x000A, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* 08-0F */
    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* 10-07 */
    _NONE_, _NONE_, _NONE_, 0x001B, _NONE_, _NONE_, _NONE_, _NONE_,	/* 18-1F */

    0xA1A1, 0xA1AA, 0xA1C9, 0xA1F4, 0xA1F0, 0xA1F3, 0xA1F5, 0xA1C7,	/* 20-27 */
    0xA1CA, 0xA1CB, 0xA1F6, 0xA1DC, 0xA1A4, 0xA1DD, 0xA1A5, 0xA1BF,	/* 28-2F */
    0xA3B0, 0xA3B1, 0xA3B2, 0xA3B3, 0xA3B4, 0xA3B5, 0xA3B6, 0xA3B7,	/* 30-37 */
    0xA3B8, 0xA3B9, 0xA1A7, 0xA1A8, 0xA1E3, 0xA1E1, 0xA1E4, 0xA1A9,	/* 38-3F */

    0xA1F7, 0xA3C1, 0xA3C2, 0xA3C3, 0xA3C4, 0xA3C5, 0xA3C6, 0xA3C7,	/* 40-47 */
    0xA3C8, 0xA3C9, 0xA3CA, 0xA3CB, 0xA3CC, 0xA3CD, 0xA3CE, 0xA3CF,	/* 48-4F */
    0xA3D0, 0xA3D1, 0xA3D2, 0xA3D3, 0xA3D4, 0xA3D5, 0xA3D6, 0xA3D7,	/* 50-57 */
    0xA3D8, 0xA3D9, 0xA3DA, 0xA1CE, 0xA1EF, 0xA1CF, 0xA1B0, 0xA1B2,	/* 58-5F */

    0xA1C6, 0xA3E1, 0xA3E2, 0xA3E3, 0xA3E4, 0xA3E5, 0xA3E6, 0xA3E7,	/* 60-67 */
    0xA3E8, 0xA3E9, 0xA3EA, 0xA3EB, 0xA3EC, 0xA3ED, 0xA3EE, 0xA3EF,	/* 68-6F */
    0xA3F0, 0xA3F1, 0xA3F2, 0xA3F3, 0xA3F4, 0xA3F5, 0xA3F6, 0xA3F7,	/* 70-77 */
    0xA3F8, 0xA3F9, 0xA3FA, 0xA1D0, 0xA1C3, 0xA1D1, 0xA1C1, _NONE_,	/* 78-7F */

    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* 80-87 */
    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* 88-8F */
    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* 90-97 */
    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* 98-9F */

    0xA1A1, 0xA1A3, 0xA1D6, 0xA1D7, 0xA1A2, 0xA1A6, 0xA5F2, 0xA5A1,	/* A0-A7 */
    0xA5A3, 0xA5A5, 0xA5A7, 0xA5A9, 0xA5E3, 0xA5E5, 0xA5E7, 0xA5C3,	/* A8-AF */
    0xA1BC, 0xA5A2, 0xA5A4, 0xA5A6, 0xA5A8, 0xA5AA, 0xA5AB, 0xA5AD,	/* B0-B7 */
    0xA5AF, 0xA5B1, 0xA5B3, 0xA5B5, 0xA5B7, 0xA5B9, 0xA5BB, 0xA5BD,	/* B8-BF */

    0xA5BF, 0xA5C1, 0xA5C4, 0xA5C6, 0xA5C8, 0xA5CA, 0xA5CB, 0xA5CC,	/* C0-C7 */
    0xA5CD, 0xA5CE, 0xA5CF, 0xA5D2, 0xA5D5, 0xA5D8, 0xA5DB, 0xA5DE,	/* C8-CF */
    0xA5DF, 0xA5E0, 0xA5E1, 0xA5E2, 0xA5E4, 0xA5E6, 0xA5E8, 0xA5E9,	/* D0-D7 */
    0xA5EA, 0xA5EB, 0xA5EC, 0xA5ED, 0xA5EF, 0xA5F3, 0xA1AB, 0xA1AC,	/* D8-DF */

    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* E0-E7 */
    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* E8-EF */
    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* F0-F7 */
    _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_, _NONE_,	/* F8-FF */
};

static
unsigned char   jiskeybord_asciitokana[] = {
    0xa6, 0x21, 0x22, 0xa7, 0xa9, 0xaa, 0xab, 0xac,	/* 20   ! " #  $ % & ' */
    0xad, 0xae, 0xb9, 0xda, 0xc8, 0xce, 0xd9, 0xd2,	/* 28 ( ) * +  , - . / */
    0xdc, 0xc7, 0xcc, 0xb1, 0xb3, 0xb4, 0xb5, 0xd4,	/* 30 0 1 2 3  4 5 6 7 */
    0xd5, 0xd6, 0xb9, 0xda, 0xa4, 0xce, 0xa1, 0xa5,	/* 38 8 9 : ;  < = > ? */

    0xde, 0xc1, 0xba, 0xbf, 0xbc, 0xa8, 0xca, 0xb7,	/* 40 @ A B C  D E F G */
    0xb8, 0xc6, 0xcf, 0xc9, 0xd8, 0xd3, 0xd0, 0xd7,	/* 48 H I J K  L M N O */
    0xbe, 0xc0, 0xbd, 0xc4, 0xb6, 0xc5, 0xcb, 0xc3,	/* 50 P Q R S  T U V W */
    0xbb, 0xdd, 0xaf, 0xdf, 0xb0, 0xd1, 0xcd, 0xdb,	/* 58 X Y Z [  \ ] ^ _ */

    0xde, 0xc1, 0xba, 0xbf, 0xbc, 0xb2, 0xca, 0xb7,	/* 60 ` a b c  d e f g */
    0xb8, 0xc6, 0xcf, 0xc9, 0xd8, 0xd3, 0xd0, 0xd7,	/* 68 h i j k  l m n o */
    0xbe, 0xc0, 0xbd, 0xc4, 0xb6, 0xc5, 0xcb, 0xc3,	/* 70 p q r s  t u v w */
    0xbb, 0xdd, 0xc2, 0xa2, 0xb0, 0xa3, 0xcd, 0x7f,	/* 78 x y z {  | } ^   */
};

int
AsciiToKanaInJISkeybord(string, length)
    char           *string;
    int             length;
{
    for (; length > 0; length--, string++) {
	*string = jiskeybord_asciitokana[*string - 0x20];
    }
    return (0);
}

/*
 * a1a2a3a4a5a6a7a8a9aAaBaCaDaEaF b0b1b2b3b4b5b6b7b8b9bAbBbCbDbEbF
 * c0c1c2c3c4c5c6c7c8c9cAcBcCcDcEcF d0d1d2d3d4d5d6d7d8d9dAdBdCdDdEdF
 */
static ushort   kana_dakuten[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1,
    0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
    0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1,
0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
static ushort   kana_handakuten[] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1};

int
CheckKanaDakuten(kana, buff)
    ushort         *kana;
    ushort          buff;
{
    ushort          data = *kana;
    if (buff == 0xA1AB) {
	if (data >= 0xA4AB && data <= 0xA4DB) {
	    if (kana_dakuten[data - 0xA4A1]) {
		data += 1;
		*kana = data;
		return (0);
	    }
	} else if (data >= 0xA5AB && data <= 0xA5DB) {
	    if (kana_dakuten[data - 0xA5A1]) {
		data += 1;
		*kana = data;
		return (0);
	    }
	} else if (data == 0xA5A6) {
	    *kana = 0xA5F4;
	    return (0);
	}
    }
    if (buff == 0xA1AC) {
	if (data >= 0xA4CF && data <= 0xA4DB) {
	    if (kana_handakuten[data - 0xA4CF]) {
		data += 2;
		*kana = data;
		return (0);
	    }
	} else if (data >= 0xA5CF && data <= 0xA5DB) {
	    if (kana_handakuten[data - 0xA5CF]) {
		data += 2;
		*kana = data;
		return (0);
	    }
	}
    }
    return (-1);
}


ushort
Hankaku_To_ZenkakuAsciiHiragana(in)
    char            in;
{
    ushort          out;
    out = asciitojef[in & 0xff];
    if ((out & 0xff00) == 0xa500) {
	out -= 0xa500 - 0xa400;
    }
    if (out == 0)
	out = in;
    return (out);
}

int
ZenkakuAsciiKana_To_Hankaku(out, in, max_out_size, flag)
    ushort         *out, *in;
    int             max_out_size, flag;
{
    ushort         *top_of_out = out, *jeftoascii;
    int             data, ix, kana, less_out_size = max_out_size;
    ushort         *bp, buff[256];
    if (in == out) {
	bp = buff;
	while (*bp++ = *in++);
	in = buff;
    }
    while ((data = *in++) && (less_out_size > 0)) {
	less_out_size--;
	switch (data & 0xff00) {
	case 0xA100:
	    jeftoascii = jefA1toascii;
	    break;
	case 0xA300:
	    jeftoascii = jefA3toascii;
	    break;
	case 0xA400:
	    jeftoascii = flag ? jefA4toascii : (ushort *) 0;
	    break;
	case 0xA500:
	    jeftoascii = jefA5toascii;
	    break;
	default:
	    jeftoascii = (ushort *) 0;
	}
	ix = (data & 0x00ff) - 0x00A0;
	if ((jeftoascii != (ushort *) 0) && (ix > 0)) {
	    kana = jeftoascii[ix];
	    if (kana != 0) {
		*out++ = kana & 0x00ff;
		kana >>= 8;
		if ((kana != 0) && (less_out_size > 0)) {
		    less_out_size--;
		    *out++ = kana;
		}
	    } else
		*out++ = data;
	} else
	    *out++ = data;
    }
    if (less_out_size > 0)
	*out = 0;
    return (max_out_size - less_out_size);
}

int
Hiragana_To_Katakana(out, in, max_out_size)
    ushort         *out, *in;
    int             max_out_size;
{
    ushort         *top_of_out = out;
    int             offset = 0xa5a1 - 0xa4a1;
    int             data, less_out_size = max_out_size;
    while ((data = *in++) && (less_out_size > 0)) {
	if (0xa4a1 <= data && data <= 0xa4f3)
	    data += offset;
	*out++ = data;
	less_out_size--;
    }
    if (less_out_size > 0)
	*out = 0;
    return (max_out_size - less_out_size);
}

int
CopyUshort(out, in, max_out_size)
    ushort         *out, *in;
    int             max_out_size;
{
    int             data, less_out_size = max_out_size;
    while ((data = *in++) && (less_out_size > 0)) {
	less_out_size--;
	*out++ = data;
    }
    if (less_out_size > 0)
	*out = 0;
    return (max_out_size - less_out_size);
}


/*
 * ARGENT!! This function has wrong assumptions!!! only supports 2 bytes wide
 * char to Compound string. NEED rewrite NOW!!!
 */
int
EUC_To_CompoundText(out, in, wc_in_char_length)
    uchar          *out;
    wchar_t        *in;
    int             wc_in_char_length;	/* This is not a byte length */
{
    uchar          *top_of_out = out;
#define output(a)	     *out++ = (a)
#define setJISX0201L_ascii() output(0x1b),output(0x28),output(0x4a)
#define setJISX0201R_kana()  output(0x1b),output(0x29),output(0x49)
#define setJISX0208_kanji()  output(0x1b),output(0x24),output(0x29),output(0x42)
    enum _GL_code {
	ISO8859L_ascii, JISX0201L_ascii
    }
                    GL_code = ISO8859L_ascii;
    enum _GR_code {
	ISO_Lation1R, JISX0201R_kana, JISX0208_kanji
    }
                    GR_code = ISO_Lation1R;
    wchar_t         work;

    while ((work = *in++) != 0 && wc_in_char_length-- > 0) {
	if (work < 0x20) {
	    if (work == 0x0d){
		output(0x0a);
	    } else {
		output(work); /* We know this may break ICCCM */
	    }
	} else if (work < 0x80) {
	    if (GL_code != JISX0201L_ascii) {
		GL_code = JISX0201L_ascii;
		setJISX0201L_ascii();
	    }
	    output(work);
	} else if (work < 0x100) {
	    if (GR_code != JISX0201R_kana) {
		GR_code = JISX0201R_kana;
		setJISX0201R_kana();
	    }
	    output(work);
	} else {
	    if (GR_code != JISX0208_kanji) {
		GR_code = JISX0208_kanji;
		setJISX0208_kanji();
	    }
	    output((work & 0xff00) >> 8);
	    output(work & 0x00ff);
	}
    }
    *out = NULL;
    return ((int) (out - top_of_out));
}
