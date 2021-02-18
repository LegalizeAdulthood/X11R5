/*
 *	convert.c -- code converters for kterm
 */

/*
 * Copyright (c) 1989  Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 */

#ifndef lint
static char kterm_id[] = "$Header: /work/nk.R3.1/lib/Xnw/RCS/Convert.c,v 1.2 91/09/12 20:58:00 mako Exp $";
#endif /* lint */

typedef unsigned short	wchar_t;

#define NULL	0

/* WS -> JIS */
#define ASCII	0
#define KANJI	1
#define KANA	2

int
_Xnw_convWStoJIS(ws, js)
wchar_t		*ws;
unsigned char	*js;
{
	int	c;
	int	kanji = ASCII;
	int	n = 0;

	while (c = *ws++) {
		switch (c & 0x8080) {
		case 0:
			if (kanji != ASCII) {
				if (js) {
					*js++ = '\033';
					*js++ = '(';
					*js++ = 'J';
				}
				n += 3;
			}
			if (js)
				*js++ = c & 0x7f;
			n++;
			kanji = ASCII;
			break;
		case 0x80:
			if (kanji != KANA) {
				if (js) {
					*js++ = '\033';
					*js++ = '(';
					*js++ = 'I';
				}
				n += 3;
			}
			if (js)
				*js++ = c & 0x7f;
			n++;
			kanji = KANA;
			break;
		case 0x8080:
			if (kanji != KANJI) {
				if (js) {
					*js++ = '\033';
					*js++ = '$';
					*js++ = 'B';
				}
				n += 3;
			}
			if (js) {
				*js++ = (c >> 8) & 0x7f;
				*js++ = c & 0x7f;
			}
			n += 2;
			kanji = KANJI;
			break;
		}
	}
	if (kanji != ASCII) {
		if (js) {
			*js++ = '\033';
			*js++ = '(';
			*js++ = 'J';
		}
		n += 3;
	}
	if (js)
		*js = '\0';

	return n;
}

/* WS -> EUC */
int
_Xnw_convWStoEUC(ws, es)
wchar_t		*ws;
unsigned char	*es;
{
	int	c;
	int	n = 0;

	while (c = *ws++) {
		switch (c & 0x8080) {
		case 0:
			if (es)
				*es++ = c & 0x7f;
			n++;
			break;
		case 0x80:
			if (es) {
				*es++ = 0x8e;	/* SS2 */
				*es++ = c & 0xff;
			}
			n += 2;
			break;
		case 0x8000:
			if (es) {
				*es++ = 0x8f;	/* SS3 */
				*es++ = (c >> 8) | 0x80;
				*es++ = (c & 0xff) | 0x80;
			}
			n += 3;
			break;
		case 0x8080:
			if (es) {
				*es++ = c >> 8;
				*es++ = c & 0xff;
			}
			n += 2;
			break;
		}
	}
	if (es)
		*es = '\0';

	return n;
}

/* WS -> SJIS */
int
_Xnw_convWStoSJIS(ws, ss)
wchar_t		*ws;
unsigned char	*ss;
{
	int	c1, c2;
	int	n = 0;

	while (c1 = *ws++) {
		switch (c1 & 0x8080) {
		case 0:
		case 0x80:
			if (ss)
				*ss++ = c1 & 0xff;
			n++;
			break;
		case 0x8080:
			c2 = c1 & 0x7f;
			c1 = (c1 >> 8) & 0x7f;
			if (ss) {
				*ss++ = (c1 - 0x21) / 2 +
				    ((c1 <= 0x5e) ? 0x81 : 0xc1);
				if (c1 & 1)	/* odd */
				    *ss++ = c2 + ((c2 <= 0x5f) ? 0x1f : 0x20);
				else
				    *ss++ = c2 + 0x7e;
			}
			n += 2;
			break;
		}
	}
	if (ss)
		*ss = '\0';

	return n;
}

/* WS -> ISO Latin-1 */
int
_Xnw_convJWStoLatin1(ws, ls)
wchar_t *ws;
unsigned char *ls;
{
    int	n = 0;
    int c;

    if (ls == NULL) {
	while (c = *ws++) {
	    if ((c & 0x8080) == 0)
	        n++;
	}
    } else {
	while (c = *ws++) {
	    if ((c & 0x8080) == 0) {
		*ls++ = c & 0x7f;
		n++;
	    }
	}
    }
    return n;
}

/******************************************************************************
COMPOUND_TEXT Summary
  (based on Comopund Text Encoding Version 1 -- MIT X Consortium Standard)
(1) Only G0 and G1 are used. G2 and G3 are not.
(2) G0 is invoked into GL and G1 into GR. These invocation are not changed.
	(In other words, Locking Shift and Single Shift are not used)
(3) In initial state, ISO Latin-1 is designated into G0/G1.
(4) To designate MBCS into G0, ESC-$-F is not used but ESC-$-(-F.
(5) In C0, only HT, NL, and ESC are used.
(6) In C1, only CSI is used.
(7) Text direction can be indecated.
	begin left-to-right string
	begin right-to-left string
	end of string
******************************************************************************/

#define CS96	0x100	/* 96chars CS */
#define MBCS	0x200	/* Multibyte CS */

/* _Xnw_convJWStoCT -- Japanese Wide Character String -> COMPOUND_TEXT */
int
_Xnw_convJWStoCT(wstr, xstr, jisroman)
register wchar_t *wstr;
register unsigned char *xstr;
int jisroman;	/* if true, use JIS X0201 for G0. (else use ASCII) */
/* Convert Wide Character String wstr to COMPOUND_TEXT xstr, return
 * length of xstr in bytes (not including the terminating null character).
 * If xstr is NULL, no conversion is done, but return length of xstr.
 */
{
	register int	c;
	register int	g0, g1;
	register int	n = 0;
	int		g0cs;
	
	g0cs = jisroman ? 'J' : 'B';

	g0 = 'B';
	g1 = CS96|'A';
	
	/*
	 * G0 and G1 are used for following character sets
	 *  G0: ASCII / JIS-ROMAN (if jisroman is true)
	 *  G1: KANJI / KANA
	 */
	
	while (c = *wstr++) {
		switch (c & 0x8080) {
		case 0:		/* ASCII or C0 */
			if (c < ' ') {
				/* C0 */
				if (c == '\t' || c == '\n') {
					if (xstr) *xstr++ = c;
					n++;
				}
				break;
			}
			if (g0 != g0cs) {
				if (xstr) {
					*xstr++ = '\033';
					*xstr++ = '(';
					*xstr++ = g0cs;
				}
				n += 3;
				g0 = g0cs;
			}
			if (xstr) *xstr++ = c & 0x7f;
			n++;
			break;
		case 0x80:	/* KANA */
			if (g1 != 'I') {
				if (xstr) {
					*xstr++ = '\033';
					*xstr++ = ')';
					*xstr++ = 'I';
				}
				n += 3;
				g1 = 'I';
			}
			if (xstr) *xstr++ = c & 0xff;
			n++;
			break;
		case 0x8080:	/* KANJI */
			if (g1 != (MBCS|'B')) {
				if (xstr) {
					*xstr++ = '\033';
					*xstr++ = '$';
					*xstr++ = ')';
					*xstr++ = 'B';
				}
				n += 4;
				g1 = MBCS|'B';
			}
			if (xstr) {
				*xstr++ = (c >> 8) & 0xff;
				*xstr++ = c & 0xff;
			}
			n += 2;
			break;
		default:
			/* ignored */
			break;
		}
	}
	if (xstr) *xstr = '\0';
	return n;
}

static unsigned char *
getesc(str, len)
unsigned char *str;
int len;
{
	register int	c;

	/* Find intermediate characters and final character
	 * following the escape character in an escape sequence.
	 */
	/* The intermediate character is 02/00 to 02/15 */
	while (len > 0) {
		c = *str;
		if (c < 0x20 || 0x2f < c)
			break;
		len--, str++;
	}
	/* The final character is 03/00 to 07/14 */
	if (--len < 0 || (c = *str++) < 0x30 || 0x7e < c)
		return (unsigned char *)NULL;

	return str;
}

static unsigned char *
getcsi(str, len)
unsigned char *str;
int len;
{
	register int	c;

	/* Find parameter characters, intermediate characters
	 * and final character following the CSI character
	 * in a CSI sequence.
	 */
	/* The parameter characters is 03/00 to 03/15 */
	while (len > 0) {
		c = *str;
		if (c < 0x30 || 0x3f < c)
			break;
		len--, str++;
	}
	/* The intermediate character is 02/00 to 02/15 */
	while (len > 0) {
		c = *str;
		if (c < 0x20 || 0x2f < c)
			break;
		len--, str++;
	}
	/* The final character is 04/00 to 07/14 */
	if (--len < 0 || (c = *str++) < 0x40 || 0x7e < c)
		return (unsigned char *)NULL;

	return str;
}

/* _Xnw_convCTtoJWS -- COMPOUND_TEXT -> Japanese Wide Character String */
int
_Xnw_convCTtoJWS(xstr, len, wstr)
register unsigned char *xstr;
int len;
wchar_t *wstr;
/* Convert COMPOUND_TEXT xstr to Wide Character String wstr, return
 * length of wstr in characters (not including the terminating null character).
 * If wstr is NULL, no conversion is done, but return length of wstr.
 */
{
	register int	c;
	int	nskip;
	int	n = 0;
	int	g0, g1, gs;
	unsigned char	*xstr1;

	/*
	 * Compound Text can include null octet. Therefore the length
	 * of xstr is able to be specified by parameter len.
	 * But if len is zero or negative, get length by strlen() assuming
	 * that no null octet exists.
	 */
	if (len <= 0) {
		len = strlen((char *)xstr);
	}

	/* In initial state, ISO 8859/1 is designated into G0/G1 */
	g0 = 'B';	/* ASCII -> G0 */
	g1 = CS96|'A';	/* Latin/1 right hand part -> G1 */

	while (len-- > 0) {
		switch (c = *xstr++) {
		case '\n':	/* NEWLINE */
		case '\t':	/* TAB */
			if (wstr) *wstr++ = c;
			n++;
			break;
		case 0x9b:	/* CSI */
			/*
			 * CSI sequence is generally in following form:
			 *	CSI {P} {I} F
			 *        P : 03/00 to 03/15
			 *        I : 02/00 to 02/15
			 *        F : 04/00 to 07/14
			 */
			/*
			 * Currently only directionality is definde
			 * as following:
			 *	CSI-1-]		begin left-to-right text
			 *	CSI-2-]		begin right-to-left text
			 *	CSI-]		end of string
			 * But this implementation ignores them.
			 */
			xstr1 = getcsi(xstr, len);
			if (xstr1 == NULL)
				return -1;
			len -= xstr1 - xstr;
			xstr = xstr1;
			break;
		case '\033':	/* ESC */
			/*
			 * ESC sequence is generally in following form:
			 *	ESC {I} F
			 *        I : 02/00 to 02/15
			 *        F : 03/00 to 07/14
			 */
			/*
			 * Currently, following functions are defined:
			 *   Standard character set
			 *	ESC-(-F
			 *	ESC-$-(-F
			 *	ESC-)-F
			 *	ESC---F
			 *	ESC-$-)-F
			 *   Non standard character set
			 *	ESC-%-/-[0123]
			 * Standard character set must be accepted correctly.
			 * Non standard one is ignored but must be parsed
			 * for skipping data.
			 */
			xstr1 = getesc(xstr, len);
			if (xstr1 == NULL)
				return -1;
			len -= xstr1 - xstr;
			switch (xstr1 - xstr) {
			case 2:		/* ESC - I - F */
				switch (*xstr++) {
				case '(':	/* 94chars CS -> G0 */
					g0 = *xstr;
					break;
				case ')':	/* 94chars CS -> G1 */
					g1 = *xstr;
					break;
				case '-':	/* 96chars CS -> G1 */
					g1 = *xstr | CS96;
					break;
				default:	/* ignore */
					break;
				}
				break;
			case 3:		/* ESC - I - I - F */
				switch (*xstr++) {
				case '$':
					switch (*xstr++) {
					case '(':	/* 94chars MBCS -> G0 */
						g0 = *xstr | MBCS;
						break;
					case ')':	/* 94chars MBCS -> G1 */
						g1 = *xstr | MBCS;
						break;
					case '-':	/* 96chars MBCS -> G1 */
						g1 = *xstr | CS96 | MBCS;
						break;
					default:	/* ignore */
						break;
					}
					break;
				case '%':
					if (*xstr++ != '/') {
						/* unknown sequence */
						break;
					}
					/*
					 * Private encoding is ignored.
					 * But following data must be skipped.
					 *	ESC-%-/-F-M-L
					 */
					len -= 2;
					if (len < 0)
						return -1;
					nskip = (*xstr1 & 0x7f) * 128 +
					    (*(xstr1 + 1) & 0x7f);
					if ((len -= nskip) < 0)
						return -1;
					xstr1 += nskip + 2;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			xstr = xstr1;
			break;
		default:
			if (!(c & 0x60)) {
				/*
				 * Non NL/TAB/ESC/CSI character in C0 or C1
				 * is an obvious error.
				 */
				return -1;
			}
			gs = (c & 0x80) ? g1 : g0;
			c &= 0x7f;
			if (gs & MBCS) {
				switch (gs & 0x70) {
				case 0x70:	/* 4byte/char */
					if (--len < 0) return -1;
					c = (c << 8) | (*xstr++ & 0x7f);
				case 0x60:	/* 3byte/char */
					if (--len < 0) return -1;
					c = (c << 8) | (*xstr++ & 0x7f);
				case 0x50:	/* 2byte/char */
				case 0x40:	/* 2byte/char */
					if (--len < 0) return -1;
					c = (c << 8) | (*xstr++ & 0x7f);
					break;
				default:
				return -1;
				}
			}
			if (wstr) {
				switch (gs) {
				case 'B':
				case 'J':
					*wstr++ = c;
					n++;
					break;
				case 'I':
					*wstr++ = 0x80 | c;
					n++;
					break;
				case MBCS|'B':
					*wstr++ = 0x8080 | c;
					n++;
					break;
				}
			} else {
				switch (gs) {
				case 'B':
				case 'J':
				case 'I':
					n++;
					break;
				case MBCS|'B':
					n++;
					break;
				}
			}
			break;
		}
	}
	if (wstr) *wstr = 0;
	return n;
}
