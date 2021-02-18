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

#ifndef lint
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/lib/RK/RCS/RKroma.c,v 1.2 91/05/21 19:02:09 mako Exp $";
#endif

/* LINTLIBRARY */
#include	"RK.h"
#include	"RKintern.h"

struct RkRxDic	*
RkOpenRoma(romaji)
unsigned char	*romaji;
{
    struct RkRxDic	*rdic;

    if ( rdic = (struct RkRxDic *)malloc(sizeof(struct RkRxDic)) ) {
	int	dic;
	unsigned char	header[256];
	unsigned char	*s;
	int	i, sz;

	if ( (dic = open((char *)romaji, 0)) < 0 ) {
		free((char *)rdic);
		return(0);
	};
/* magic no shougou */
	if ( read(dic, (char *)header, 6) != 6 ||
	     strncmp((char *)header, "RD", 2) ) {
		(void)close(dic);
		free((char *)rdic);
		return(0);
	};
	rdic->nr_strsz = S2TOS(header + 2);
	rdic->nr_nkey  = S2TOS(header + 4);
	rdic->nr_string = (unsigned char *)malloc((unsigned int)rdic->nr_strsz);
	if ( !rdic->nr_string ) {
		(void)close(dic);
		free((char *)rdic);
		return(0);
	};
	sz = read(dic, (char *)rdic->nr_string, rdic->nr_strsz);
	(void)close(dic);
	if ( sz != rdic->nr_strsz ) {
	    free((char *)rdic->nr_string);
	    free((char *)rdic);
	    return(0);
	};

	rdic->nr_keyaddr =
	    (unsigned char **)calloc((unsigned)rdic->nr_nkey, sizeof(unsigned char *));
	if ( !rdic->nr_keyaddr ) {
	    free((char *)rdic->nr_string);
	    free((char *)rdic);
	    return(0);
	};

	s = rdic->nr_string;
	for ( i = 0; i < rdic->nr_nkey; i++ ) {
	    rdic->nr_keyaddr[i] = s;
	    while ( *s++ );
	    while ( *s++ );
	};
    };
    return((struct RkRxDic *)rdic);
}
/* RkCloseRoma
 *	romaji henkan table wo tojiru
 */
void	
RkCloseRoma(rdic)
struct RkRxDic	*rdic;
{
    if ( rdic ) {
	(void)free((char *)rdic->nr_string);
	(void)free((char *)rdic->nr_keyaddr);
	(void)free((char *)rdic);
    };
}

/* RkMapRoma
 *	key no sentou wo saichou itti hou ni yori,henkan suru
 */
#define	xkey(roma, line, n) 	((roma)->nr_keyaddr[line][n])

struct rstat {
    int	start, end;	/* match sury key no hanni */
};

static 
int	
findRoma(rdic, m, c, n)
struct RkRxDic	*rdic;
struct rstat	*m;
unsigned char	c;
int		n;
{
    register int	s, e;

    for(s = m->start; s < m->end; s++) 
	if( c == xkey(rdic, s, n) )
	    break;
    for(e = s; e < m->end; e++) 
	if( c != xkey(rdic, e, n) )
	    break;
    m->start	= s;
    m->end	= e;
    return e - s;
}
static 
unsigned char	*
getKana(rdic, p, flags)
struct RkRxDic	*rdic;
int		p;
int		flags;
{
    register unsigned char	*kana;
    int				klen;
    static unsigned  char	tmp[256];

    for ( kana = rdic->nr_keyaddr[p]; *kana++; );

    klen = strlen((char *)kana);
    switch(flags&RK_XFERMASK) {
    default: 
	(void)RkCvtNone(tmp, sizeof(tmp), kana, klen);
	return tmp;
    case RK_XFER:
	(void)RkCvtHira(tmp, sizeof(tmp), kana, klen);
	return tmp;
    case RK_HFER:
	(void)RkCvtHan(tmp, sizeof(tmp), kana, klen);
	return tmp;
    case RK_KFER:
	(void)RkCvtKana(tmp, sizeof(tmp), kana, klen);
	return tmp;
    case RK_ZFER:
	(void)RkCvtZen(tmp, sizeof(tmp), kana, klen); 
	return tmp;
    };
}
static 
unsigned char	*
getRoma(rdic, p)
struct RkRxDic	*rdic;
int		p;
{
    return rdic->nr_keyaddr[p];
}
/*ARGSUSED*/
static 
unsigned char	*
getTSU(rdic, flags)
struct RkRxDic	*rdic;
int		flags;
{
    static unsigned char  hira_tsu[] = {0xa4, 0xc3, 0};
    static unsigned char  kana_tsu[] = {0xa5, 0xc3, 0};
    static unsigned char  han_tsu[] =  {0x8e, 0xaf, 0};

    switch(flags&RK_XFERMASK) {
    default:	  return hira_tsu;
    case RK_HFER: return han_tsu;
    case RK_KFER: return kana_tsu;
    };
}

int	
RkMapRoma(rdic, dst, maxdst, src, maxsrc, flags, status)
struct RkRxDic	*rdic;
unsigned char	*dst;
int		maxdst;
unsigned char	*src;
int		maxsrc;
int		flags;
int		*status;
{
    struct rstat	match[256], *m;
    register int	i;
    unsigned char	*roma;
    unsigned char	*kana = src;
    int			count = 0;
    int			byte;
    int			found = 1;

    if ( rdic ) {
	m = match;
	m->start = 0;
	m->end = rdic->nr_nkey;
	for (i = 0; (flags & RK_FLUSH) || i < maxsrc;  i++) {
	    m[1] = m[0];
	    m++;
	    switch((i < maxsrc) ? findRoma(rdic, m, src[i], i) : 0) {
	    case	0:
		while ( --m > match && xkey(rdic, m->start, m - match) );
		if(m == match) { /* table ni nakatta tokino shori */
		    kana = src;
		    count = (maxsrc <= 0)? 0 : (*src & 0x80)? 2 : 1;
		    if( (flags & RK_SOKON) &&
			(match[1].start < rdic->nr_nkey) &&
			(2 <= maxsrc) &&
			(src[0] == src[1]) &&
			(i == 1)) {
			kana = getTSU(rdic, flags);
		    /* tsu ha jisho ni aru kao wo suru */
			byte = strlen((char *)kana);
		    }
		    else {
			static unsigned char	tmp[256];

			switch(flags&RK_XFERMASK) {
			default: 
			    byte = RkCvtNone(tmp, sizeof(tmp), src, count);
			    break;
			case RK_XFER:
			    byte = RkCvtHira(tmp, sizeof(tmp), src, count);
			    break;
			case RK_HFER:
			    byte = RkCvtHan(tmp, sizeof(tmp), src, count);
			    break;
			case RK_KFER:
			    byte = RkCvtKana(tmp, sizeof(tmp), src, count);
			    break;
			case RK_ZFER:
			    byte = RkCvtZen(tmp, sizeof(tmp), src, count); 
			    break;
			};
			kana = tmp;
			found = -1;
		    };
		}
		else {  /* 'n' nado no shori: saitan no monowo toru */
		    kana = getKana(rdic, m->start, flags);
		    byte = strlen((char *)kana);
		    count = m - match;
		}
		goto done;
	    case	1:	/* determined uniquely */
	    /* key no hou ga nagai baai */
		roma = getRoma(rdic, m->start);
		if ( roma[i + 1] ) 	/* waiting suffix */
		    continue;
		kana = getKana(rdic, m->start, flags);
		byte = strlen((char *)kana);
		count = i + 1;
		goto done;
	    };
	};
	byte = 0;
    }
    else 
	byte = (maxsrc <= 0) ? 0 : (*src & 0x80) ? 2 : 1;
done:
    *status = found*byte;
    if ( byte + 1 <= maxdst ) {
	if ( dst ) {
	    while ( byte-- )
		*dst++ = *kana++;
	    *dst = 0;
	};
    };
    return count;
}
/* RkCvtRoma
 */
int	
RkCvtRoma(rdic, dst, maxdst, src, maxsrc, flags)
struct RkRxDic	*rdic;
unsigned char	*dst;
int		maxdst;
unsigned char	*src;
int		maxsrc;
int	flags;
{
    register unsigned char	*d = dst;
    register unsigned char	*s = src;
    register unsigned char	*S = src + maxsrc;
    int				count;

    switch(flags&RK_XFERMASK) {
    case RK_XFER:	/* henkan */
    case RK_HFER:	/* hankaku */
    case RK_KFER:	/* katakana */
    case RK_ZFER:	/* zenkaku */
	if ( maxdst <= 0 || maxsrc < 0 )
	    return 0;
	count = 0;
	while ( s < S ) {
	    int			n, dstlen;

	    n = RkMapRoma(rdic, d, maxdst, s, maxsrc, flags, &dstlen);
	    if ( dstlen < 0 ) 
		dstlen = -dstlen;
	    if ( dstlen + 1 <= maxdst ) {
		maxdst -= dstlen; count += dstlen;
		if ( dst )
		    d += dstlen;
	    };
	    if ( !n )
		break;
	    s += n;
	    maxsrc -= n;
	};
	return count;
    default:
    case RK_NFER:	/* muhenkan */
	return RkCvtNone(dst, maxdst, src, maxsrc);
    };
}
