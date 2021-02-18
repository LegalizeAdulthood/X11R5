/*
 * $Id: dic_atojis.c,v 1.2 1991/09/16 21:29:16 ohm Exp $
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
#include "commonhd.h"
#include "jslib.h"
#include "jdata.h"
#include "wnn_string.h"

static int	keta_4;

static w_char	_tatojis[] = {
	0xa1a1, 0xa1aa, 0xa1c9, 0xa1f4, 0xa1f0, 0xa1f3, 0xa1f5, 0xa1c7, 
	0xa1ca, 0xa1cb, 0xa1f6, 0xa1dc, 0xa1a4, 0xa1dd, 0xa1a5, 0xa1bf, 
	0xa3b0, 0xa3b1, 0xa3b2, 0xa3b3, 0xa3b4, 0xa3b5, 0xa3b6, 0xa3b7, 
	0xa3b8, 0xa3b9, 0xa1a7, 0xa1a8, 0xa1e3, 0xa1e1, 0xa1e4, 0xa1a9, 
	0xa1f7, 0xa3c1, 0xa3c2, 0xa3c3, 0xa3c4, 0xa3c5, 0xa3c6, 0xa3c7, 
	0xa3c8, 0xa3c9, 0xa3ca, 0xa3cb, 0xa3cc, 0xa3cd, 0xa3ce, 0xa3cf, 
	0xa3d0, 0xa3d1, 0xa3d2, 0xa3d3, 0xa3d4, 0xa3d5, 0xa3d6, 0xa3d7, 
	0xa3d8, 0xa3d9, 0xa3da, 0xa1ce, 0xa1ef, 0xa1cf, 0xa1b0, 0xa1b2, 
	0xa1ae, 0xa3e1, 0xa3e2, 0xa3e3, 0xa3e4, 0xa3e5, 0xa3e6, 0xa3e7, 
	0xa3e8, 0xa3e9, 0xa3ea, 0xa3eb, 0xa3ec, 0xa3ed, 0xa3ee, 0xa3ef, 
	0xa3f0, 0xa3f1, 0xa3f2, 0xa3f3, 0xa3f4, 0xa3f5, 0xa3f6, 0xa3f7, 
	0xa3f8, 0xa3f9, 0xa3fa, 0xa1d0, 0xa1c3, 0xa1d1, 0xa1b1, 0x007f
};


static int
atojis(c)
register w_char c;
{
	if (c >= ' ' && c <= (unsigned char)'\376')
		c = _tatojis[c - ' '];
	return(c);
}

static w_char	_tatokan[] = {
	0xa1bb, 0xb0ec, 0xc6f3, 0xbbb0, 0xbbcd, /* ¡»°ìÆó»°»Í */
	0xb8de, 0xcfbb, 0xbcb7, 0xc8ac, 0xb6e5	/* ¸ÞÏ»¼·È¬¶å */
};

static w_char	_tatokan_o[] = {
	0xceed, 0xb0ed, 0xc6f5, 0xbbb2, 0xbbcd, /* Îí°íÆõ»²»Í */
	0xb8e0, 0xcfbb, 0xbcb7, 0xc8ac, 0xb6e5	/* ¸àÏ»¼·È¬¶å */
};

static w_char
atokan(c)
register w_char c;
{
	if (c >= '0' && c <= '9')
		c = _tatokan[c - '0'];
	return (c);
}

static w_char
atokan_o(c)
register w_char c;
{
	if (c >= '0' && c <= '9')
		c = _tatokan_o[c - '0'];
	return (c);
}

static w_char	_tatoket[] = {
	0xbdbd, 0xc9b4, 0xc0e9,			/* ½½É´Àé */
	0xcbfc, 0xb2af, 0xc3fb, 0xb5fe,		/* Ëü²¯Ãûµþ */
	0xd4b6, 0xa4b7, 0xbef7, 0xb9c2,		/* Ô¶¤·¾÷¹Â */
	0xb4c2, 0xc0b5, 0xbadc, 0xb6cb		/* ´ÂÀµºÜ¶Ë */
	/* ¹±²Ïº» */ /* °¤ÁÎµÀ */ /* ÆáÍ³Â¾ */ /* ÉÔ²Ä»×µÄ */ /* ÌµÎÌÂç¿ô */
};

static w_char	_tatoket_o[] = {
	0xbda6, 0xeff9, 0xeff4,			/* ½¦ïùïô */
	0xe8df					/* èß */
};

static void
getketa(k, kouho, col)
int	k;
w_char	kouho[];
int	*col;
{
	if (k % 4) {
		if (keta_4 != 0) {
			keta_4 = 0;
			getketa(k / 4 * 4, kouho, col);
		}
		kouho[(*col)++] = _tatoket[k % 4 - 1];
	} else if ((k / 4 > 0) && (k / 4 < 13)) {
		kouho[(*col)++] = _tatoket[k / 4 + 2];
	} else if (k / 4 == 13) {
		kouho[(*col)++] = 0xbabb;
		kouho[(*col)++] = 0xb2cf;
		kouho[(*col)++] = 0xb9b1;	/* ¹±²Ïº» */
	} else if (k / 4 == 14) {
		kouho[(*col)++] = 0xb5c0;
		kouho[(*col)++] = 0xc1ce;
		kouho[(*col)++] = 0xb0a4;	/* °¤ÁÎµÀ */
	} else if (k / 4 == 15) {
		kouho[(*col)++] = 0xc2be;
		kouho[(*col)++] = 0xcdb3;
		kouho[(*col)++] = 0xc6e1;	/* ÆáÍ³Â¾ */
	} else if (k / 4 == 16) {
		kouho[(*col)++] = 0xb5c4;
		kouho[(*col)++] = 0xbbd7;
		kouho[(*col)++] = 0xb2c4;
		kouho[(*col)++] = 0xc9d4;	/* ÉÔ²Ä»×µÄ */
	} else if (k / 4 == 17) {
		kouho[(*col)++] = 0xbff4;
		kouho[(*col)++] = 0xc2e7;
		kouho[(*col)++] = 0xcecc;
		kouho[(*col)++] = 0xccb5;	/* ÌµÎÌÂç¿ô */
	} else {
		kouho[(*col)++] = 0xa1a9;	/* ¡© */
	}
	keta_4 = 0;
}

static void
getketa_o(k, kouho, col)
int	k;
w_char	kouho[];
int	*col;
{
	if (k % 4) {
		if (keta_4 != 0) {
			keta_4 = 0;
			if (k / 4 == 1)
				kouho[(*col)++] = _tatoket_o[3];        /* èß */
			else
				getketa(k / 4 * 4, kouho, col);
		}
		kouho[(*col)++] = _tatoket_o[k % 4 - 1];
	} else if (k / 4 == 1) {
		kouho[(*col)++] = _tatoket_o[3];	/* èß */
	} else {
		getketa(k, kouho, col);
	}
	keta_4 = 0;
}

int
substr(c1, c2)
char *c1;
w_char *c2;
{
    for(;*c1;c1++, c2++){
	if(*c1 != *c2) break;
    }
    if(*c1) return(0);
    return(1);
}

w_char *
kanji_giji_str(bun, bunl, c, kouho)
w_char *bun;
int	bunl;
w_char *c;
w_char	*kouho;
{
    register int	k;
    w_char	revkouho[LENGTHYOMI];
    char tmp[LENGTHYOMI];
    int	col = 0, keta = 0;

    if(substr(DIC_HIRAGANA, c)){
	for (k = bunl - 1; k >=0 ; k--) {
	    *kouho++ =bun[k];
	}
    }else if(substr(DIC_KATAKANA, c)){	/* ¥«¥¿¥«¥Ê */
	for (k = bunl - 1; k >=0 ; k--){
	    if ((bun[k] & 0xff00) == 0xa400)
		*kouho++ = bun[k] | 0x100;
	    else 
		*kouho++ =bun[k];
	}
    }else if(substr(DIC_ZENKAKU, c)){	/* Á´³Ñ¿ô»ú *//* £±£²£³ */
	for (k = bunl - 1; k >=0 ; k--){
	    *kouho++ = atojis(bun[k]);
	}
    }else if(substr(DIC_NUM_KAN, c)){	/* ´Á¿ô»ú *//* °ìÆó»° */
	for (k = bunl - 1; k >=0 ; k--){
	    *kouho++ = atokan(bun[k]);
	}	    
    }else if(substr(DIC_NUM_HANCAN, c)){	/* È¾³Ñ¿ô»ú *//* 1,234 */
	for (k = 0; k < bunl ; k++){
	    if ((keta != 0) && (keta % 3 == 0))
		revkouho[col++] = ',';	/* , */
	    revkouho[col++] = bun[k];
	    keta++;
	}
	while (--col >= 0) {
	    *kouho++ = revkouho[col];
	}
    }else if(substr(DIC_NUM_ZENCAN, c)){	/* Á´³Ñ¿ô»ú *//* £±¡¤£²£³£´ */
	for (k = 0; k < bunl ; k++){
	    if ((keta != 0) && (keta % 3 == 0))
		revkouho[col++] = 0xa1a4; /* ¡¤ */
	    revkouho[col++] = atojis(bun[k]);
	    keta++;
	}
	while (--col >= 0) {
	    *kouho++ = revkouho[col];
	}
    }else if(substr(DIC_NUM_KANSUUJI, c)){	/* ´Á¿ô»ú *//* É´Æó½½»° */
	keta_4 = 0;
	for (k = 0; k < bunl ; k++){
	    if (bun[k] != '0') {
		if (keta != 0)
		    getketa(keta, revkouho, &col);
		if (bun[k] != '1' ||
		    keta % 4 == 0 || keta % 4 == 3)
		    revkouho[col++] = atokan(bun[k]);
	    } else if ((keta != 0) && (keta % 4 == 0)) {
		keta_4 = 1;
	    }
	    keta++;
	}
	if (col == 0)
	    *kouho++ = _tatokan[0];
	while (--col >= 0) {
	    *kouho++ = revkouho[col];
	}
    }else if(substr(DIC_NUM_KANOLD, c)){	/* ´Á¿ô»ú *//* °íÉ´Æõ½¦»² chao*/
	keta_4 = 0;
	for (k = 0; k < bunl; k++) {
	    if (bun[k] != '0') {
		if (keta != 0)
		    getketa_o(keta, revkouho, &col);
		if (bun[k] != '1' ||
		    keta % 4 == 0 || keta % 4 == 3)
		    revkouho[col++] = atokan_o(bun[k]);
	    } else if ((keta != 0) && (keta % 4 == 0)) {
		keta_4 = 1;
	    }
	    keta++;
	}
	if (col == 0)
	    *kouho++ = _tatokan_o[0];
	while (--col >= 0) {
	    *kouho++ = revkouho[col];
	}
    }else if(substr(DIC_ESC, c)){
	*kouho++ = '\\';
    }else if(substr(DIC_HEX, c) || substr(DIC_HEXc, c)){
	int num;
	sStrcpy(tmp, c + strlen(DIC_HEX));
	sscanf(tmp,"%x",&num);
	*kouho++ = num;
    }else if(substr(DIC_OCT, c)){
	int num;
	sStrcpy(tmp, c + strlen(DIC_OCT));
	sscanf(tmp,"%o",&num);
	*kouho++ = num;
    }else{
	for(;*c && *c != '\\';c++){
	    *kouho++ = *c;
	}
	if(*c == '\\') *kouho++ = *c;
    }
    *kouho = 0;
    return (kouho);
}


void
kanji_esc_str(w, oy, oyl)
w_char *w, *oy;
int oyl;
{
    register w_char *ret;
    register w_char *c = w;
    w_char tmp[LENGTHKANJI];

    for(;*c;c++){
	if(*c == '\\') break;
    }
    if(!*c) return;
    Strcpy(tmp, w);
    c = tmp;
    while(*c){
	if(*c == '\\' &&
	   (ret = kanji_giji_str(oy, oyl, c, w))!= NULL){
	    w = ret;
	    for(++c; *c; c++){
		if(*c == '\\'){
		    c++;
		    break;
		}
	    }
	}else{
	    *w++ = *c++;
	}
    }
    *w = 0;
}

void
Get_kanji(kptr, oy, oyl, kanji, yomi, comment)
UCHAR *kptr;
w_char *kanji, *comment, *yomi, *oy;
int oyl;
{
    extern void get_kanji_str();

    get_kanji_str(kptr, kanji, yomi, comment);
    if(kanji){
	kanji_esc_str(kanji, oy, oyl);
    }
    if(yomi){
	kanji_esc_str(yomi, oy, oyl);
    }
}


void
Get_knj1(kptr, oy, oyl,kanji2, kouho, yomi, comment)
UCHAR *kptr;
int kanji2;
w_char *kouho, *comment, *yomi, *oy;
int oyl;
{
	int tcnt;
	for (tcnt = 0; tcnt < kanji2; tcnt++){
		kptr += *kptr;
	}
	Get_kanji(kptr, oy, oyl, kouho, yomi, comment);
	return;
}

