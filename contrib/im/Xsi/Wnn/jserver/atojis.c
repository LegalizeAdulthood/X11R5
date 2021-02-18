/*
 * $Id: atojis.c,v 1.2 1991/09/16 21:31:07 ohm Exp $
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
#include "ddefine.h"
#include "de_header.h"
#include "kaiseki.h"

static int	keta_4;

static w_char	_tatojis[] = {
#ifdef	CHINESE
	0xa1a1, 0xa3a1, 0xa3a2, 0xa3a3, 0xa3a4, 0xa3a5, 0xa3a6, 0xa3a7, 
	0xa3a8, 0xa3a9, 0xa3aa, 0xa3ab, 0xa3ac, 0xa3ad, 0xa3ae, 0xa3af, 
	0xa3b0, 0xa3b1, 0xa3b2, 0xa3b3, 0xa3b4, 0xa3b5, 0xa3b6, 0xa3b7, 
	0xa3b8, 0xa3b9, 0xa3ba, 0xa3bb, 0xa3bc, 0xa3bd, 0xa3be, 0xa3bf, 
	0xa3c0, 0xa3c1, 0xa3c2, 0xa3c3, 0xa3c4, 0xa3c5, 0xa3c6, 0xa3c7, 
	0xa3c8, 0xa3c9, 0xa3ca, 0xa3cb, 0xa3cc, 0xa3cd, 0xa3ce, 0xa3cf, 
	0xa3d0, 0xa3d1, 0xa3d2, 0xa3d3, 0xa3d4, 0xa3d5, 0xa3d6, 0xa3d7, 
	0xa3d8, 0xa3d9, 0xa3da, 0xa3db, 0xa3dc, 0xa3dd, 0xa3de, 0xa3df, 
	0xa3e0, 0xa3e1, 0xa3e2, 0xa3e3, 0xa3e4, 0xa3e5, 0xa3e6, 0xa3e7, 
	0xa3e8, 0xa3e9, 0xa3ea, 0xa3eb, 0xa3ec, 0xa3ed, 0xa3ee, 0xa3ef, 
	0xa3f0, 0xa3f1, 0xa3f2, 0xa3f3, 0xa3f4, 0xa3f5, 0xa3f6, 0xa3f7, 
	0xa3f8, 0xa3f9, 0xa3fa, 0xa3fb, 0xa3fc, 0xa3fd, 0xa3fe, 0x007f
#else	/* JAPANESE */
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
#endif	/* CHINESE */
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
#ifdef	CHINESE
	0xa1f0, 0xd2bb, 0xb6fe, 0xc8fd, 0xcbc4, /* ��һ������ */
	0xcee5, 0xc1f9, 0xc6df, 0xb0cb, 0xbec5	/* �����߰˾� */
#else	/* JAPANESE */
	0xa1bb, 0xb0ec, 0xc6f3, 0xbbb0, 0xbbcd, /* �����󻰻� */
	0xb8de, 0xcfbb, 0xbcb7, 0xc8ac, 0xb6e5	/* ��ϻ��Ȭ�� */
#endif	/* CHINESE */
};

static w_char	_tatokan_o[] = {
#ifdef	CHINESE
	0xc1e3, 0xd2bc, 0xb7a1, 0xc8fe, 0xcbc1,	/* ��Ҽ������ */
	0xcee9, 0xc2bd, 0xc6e2, 0xb0c6, 0xbec1	/* ��½��ƾ� */
#else	/* JAPANESE */
	0xceed, 0xb0ed, 0xc6f5, 0xbbb2, 0xbbcd, /* ��������� */
	0xb8e0, 0xcfbb, 0xbcb7, 0xc8ac, 0xb6e5	/* ��ϻ��Ȭ�� */
#endif	/* CHINESE */
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
#ifdef	CHINESE
	0xcaae, 0xb0d9, 0xc7a7,			/* ʮ��ǧ */
	0xcdf2, 0xd2da, 0xd5d7, 0xbea9,		/* �����׾� */
	0xdbf2,					/* �� */
#else	/* JAPANESE */
	0xbdbd, 0xc9b4, 0xc0e9,			/* ��ɴ�� */
	0xcbfc, 0xb2af, 0xc3fb, 0xb5fe,		/* �������� */
	0xd4b6, 				/* Զ    */
#endif	/* CHINESE */
	        0xa4b7, 0xbef7, 0xb9c2,		/*   ������ */
	0xb4c2, 0xc0b5, 0xbadc, 0xb6cb		/* �����ܶ� */
	/* ���Ϻ� */ /* ���ε� */ /* ��ͳ¾ */ /* �ԲĻ׵� */ /* ̵����� */
};

static w_char	_tatoket_o[] = {
#ifdef	CHINESE
	0xcab0, 0xb0db, 0xc7aa,			/* ʰ��Ǫ */
	0xcdf2					/* �� */
#else	/* JAPANESE */
	0xbda6, 0xeff9, 0xeff4,			/* ������ */
	0xe8df					/* �� */
#endif	/* CHINESE */
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
		kouho[(*col)++] = 0xb9b1;	/* ���Ϻ� */
	} else if (k / 4 == 14) {
		kouho[(*col)++] = 0xb5c0;
		kouho[(*col)++] = 0xc1ce;
		kouho[(*col)++] = 0xb0a4;	/* ���ε� */
	} else if (k / 4 == 15) {
		kouho[(*col)++] = 0xc2be;
		kouho[(*col)++] = 0xcdb3;
		kouho[(*col)++] = 0xc6e1;	/* ��ͳ¾ */
	} else if (k / 4 == 16) {
#ifdef CHINESE
		kouho[(*col)++] = 0xd2e5;
		kouho[(*col)++] = 0xcbbc;
		kouho[(*col)++] = 0xbfc9;
		kouho[(*col)++] = 0xb2bb;       /* ����˼�� */
#else
		kouho[(*col)++] = 0xb5c4;
		kouho[(*col)++] = 0xbbd7;
		kouho[(*col)++] = 0xb2c4;
		kouho[(*col)++] = 0xc9d4;	/* �ԲĻ׵� */
#endif
	} else if (k / 4 == 17) {
		kouho[(*col)++] = 0xbff4;
		kouho[(*col)++] = 0xc2e7;
		kouho[(*col)++] = 0xcecc;
		kouho[(*col)++] = 0xccb5;	/* ̵����� */
	} else {
		kouho[(*col)++] = 0xa1a9;	/* �� */
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
				kouho[(*col)++] = _tatoket_o[3];        /* �� */
			else
				getketa(k / 4 * 4, kouho, col);
		}
		kouho[(*col)++] = _tatoket_o[k % 4 - 1];
	} else if (k / 4 == 1) {
		kouho[(*col)++] = _tatoket_o[3];	/* �� */
	} else {
		getketa(k, kouho, col);
	}
	keta_4 = 0;
}

w_char *
get_giji_knj(j, i, t, kouho)
int	j;
int	i;
int	t;
w_char	*kouho;
{
	register int	k;
	w_char	revkouho[1000];
	int	col = 0, keta = 0;

	switch (t) {
	case	WNN_NUM_ZEN:	/* ���ѿ��� *//* ������ */
#ifdef	CONVERT_from_TOP
			for (k = i; k <=j ; k++) {
				if (k == i &&
#else
			for (k = j; k >=i ; k--) {
				if (k == j &&
#endif	/* CONVERT_from_TOP */
				    (isspace(bun[k]) || isjspace(bun[k])))
					*kouho++ = bun[k];
				else
					*kouho++ = atojis(bun[k]);
			}
			break;
	case	WNN_ALP_ZEN:	/* ���� */
	case	WNN_KIG_JIS:	
	case	WNN_KIG_ASC:
#ifdef	CONVERT_from_TOP
			for (k = i; k <=j ; k++)
#else
			for (k = j; k >=i ; k--)
#endif	/* CONVERT_from_TOP */
				*kouho++ = atojis(bun[k]);
			break;

	case	WNN_KATAKANA:	/* �������� */
#ifdef	CONVERT_from_TOP
			for (k = i; k <=j ; k++) {
#else
			for (k = j; k >=i ; k--) {
#endif	/* CONVERT_from_TOP */
				if ((bun[k] & 0xff00) == 0xa400)
					*kouho++ = bun[k] | 0x100;
				else 
					*kouho++ =bun[k];
			}
			break;

	case	WNN_NUM_KAN:	/* ������ *//* ���� */
#ifdef	CONVERT_from_TOP
			for (k = i; k <=j ; k++)
#else
			for (k = j; k >=i ; k--)
#endif	/* CONVERT_from_TOP */
				*kouho++ = atokan(bun[k]);
			break;
	case	WNN_NUM_HANCAN:	/* Ⱦ�ѿ��� *//* 1,234 */
#ifdef	CONVERT_from_TOP
			for (k = j; k >=i ; k--) {
				if (k == i &&
#else
			for (k = i; k <= j; k++) {
				if (k == j &&
#endif	/* CONVERT_from_TOP */
				    (isspace(bun[k]) || isjspace(bun[k]))) {
					revkouho[col++] = bun[k];
					break;
				}
				if ((keta != 0) && (keta % 3 == 0))
					revkouho[col++] = ',';	/* , */
				revkouho[col++] = bun[k];
				keta++;
			}

			while (--col >= 0) {
				*kouho++ = revkouho[col];
			}
			break;
	case	WNN_NUM_ZENCAN:	/* ���ѿ��� *//* ���������� */
#ifdef	CONVERT_from_TOP
			for (k = j; k >=i ; k--) {
				if (k == i &&
#else
			for (k = i; k <= j; k++) {
				if (k == j &&
#endif	/* CONVERT_from_TOP */
				    (isspace(bun[k]) || isjspace(bun[k]))) {
					revkouho[col++] = bun[k];
					break;
				}
				if ((keta != 0) && (keta % 3 == 0))
					revkouho[col++] = UJIS_comma; /* �� */
				revkouho[col++] = atojis(bun[k]);
				keta++;
			}

			while (--col >= 0) {
				*kouho++ = revkouho[col];
			}
			break;
	case	WNN_NUM_KANSUUJI:	/* ������ *//* ɴ�󽽻� */
			keta_4 = 0;
#ifdef	CONVERT_from_TOP
			for (k = j; k >=i ; k--) {
				if (k == i &&
#else
			for (k = i; k <= j; k++) {
				if (k == j &&
#endif	/* CONVERT_from_TOP */
				    (isspace(bun[k]) || isjspace(bun[k]))) {
					revkouho[col++] = bun[k];
					break;
				}
				if (bun[k] != '0') {
					if (keta != 0)
						getketa(keta, revkouho, &col);
					if (bun[k] != '1' ||
#ifdef	CHINESE
					    keta%4 !=1)
#else	/* JAPANESE */
					    keta % 4 == 0 || keta % 4 == 3)
					/* ����ɴ����*/
#endif	/* CHINESE */
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
			break;
	case	WNN_NUM_KANOLD:	/* ������ *//* ��ɴ������ */
			keta_4 = 0;
#ifdef	CONVERT_from_TOP
			for (k = j; k >=i ; k--) {
				if (k == i &&
#else
			for (k = i; k <= j; k++) {
				if (k == j &&
#endif	/* CONVERT_from_TOP */
				    (isspace(bun[k]) || isjspace(bun[k]))) {
					revkouho[col++] = bun[k];
					break;
				}
				if (bun[k] != '0') {
					if (keta != 0)
						getketa_o(keta, revkouho, &col);
					if (bun[k] != '1' ||
#ifdef	CHINESE
					    keta%4 !=1)
#else	/* JAPANESE */
					    keta % 4 == 0 || keta % 4 == 3)
					/* ����ɴ����*/
#endif	/* CHINESE */
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
			break;
	default:	/* �Ҥ餬�� */
#ifdef	CONVERT_from_TOP
			for (k = i; k <=j ; k++) {
#else
			for (k = j; k >=i ; k--) {
#endif	/* CONVERT_from_TOP */
				*kouho++ =bun[k];
			}
			break;
	}
	*kouho++ = 0;
	return (kouho);
}

void
giji_hindoup(t_jc)
int	t_jc;
{
	if (t_jc >= 0)
		return ;
	switch(t_jc) {
	case WNN_NUM_HAN:
	case WNN_NUM_ZEN:
	case WNN_NUM_KAN:
	case WNN_NUM_KANSUUJI:
	case WNN_NUM_KANOLD:
	case WNN_NUM_HANCAN:
	case WNN_NUM_ZENCAN:
			c_env->giji.number = t_jc;
			break;
	case WNN_ALP_HAN:
	case WNN_ALP_ZEN:
			c_env->giji.eisuu = t_jc;
			break;
	case WNN_KIG_HAN:
	case WNN_KIG_JIS:
	case WNN_KIG_ASC:
			c_env->giji.kigou = t_jc;
			break;
	default:
			return ;
	}
}
