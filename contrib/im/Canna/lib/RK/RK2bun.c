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
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/lib/RK/RCS/RK2bun.c,v 1.2 91/05/21 18:32:11 mako Exp $";
#endif
/* LINTLIBRARY */
#include	<stdio.h>
#include	<string.h>
#include	"RK.h"
#include	"RKintern.h"

/* RkBgnBun
 *	renbunsetu henkan wo kaishi surutameno shokisettei wo okonau
 * reuturns:
 *	# >=0	shoki bunsetsu no kosuu
 *	-1	shoki ka sippai
 */

static int
allocBunStorage(cx, n)
register struct RkContext	*cx;
int				n;
{
    cx->maxyomi = n;
    cx->yomi = (unsigned char *)malloc((unsigned)(cx->maxyomi + 1));
    if ( !cx->yomi )
	 return -1;
    cx->maxbunq  = n;
    cx->bunq = (struct nbun *)calloc((unsigned)(n+1), sizeof(struct nbun));
    if ( !cx->bunq )
	 return -1;
    cx->parse = (struct nword **)calloc((unsigned)(n+1),sizeof(struct nword *));
    if ( !cx->parse )
	return -1;
    cx->succ = (struct nword **)calloc((unsigned)(n+1),sizeof(struct nword *));
    if ( !cx->succ )
	return -1;
    return 0;
}

static void
freeBunStorage(cx)
register struct RkContext	*cx;
{
    if ( cx->yomi ) 
	(void)free((char *)cx->yomi);
    cx->yomi = (unsigned char *)0;
    if ( cx->bunq ) 
	(void)free((char *)cx->bunq);
    cx->bunq = (struct nbun *)0;
    if ( cx->parse ) 
	(void)free((char *)cx->parse);
    cx->parse = (struct nword **)0;
    if ( cx->succ ) 
	(void)free((char *)cx->succ);
    cx->succ = (struct nword **)0;
}

int	
RkBgnBun(cx_num, yomi, n, kouhomode)
int		cx_num;
unsigned char	*yomi;
int		n;
int		kouhomode;
{
    register struct RkContext	*cx = RkGetContext(cx_num);

    if ( !cx )
    	return -1;
    if ( cx->flags&CTX_XFER )
    	return -1;
    if ( !yomi || n <= 0 )
    	return -1;
/* allocate resources */
    if ( allocBunStorage(cx, n) )
	return -1;
/* context de siyousiteiru jisho wo subete lock suru */
    _RkBgnBun(cx);
    (void)strncpy((char *)cx->yomi, (char *)yomi, n);
    cx->yomi[n] = 0;
    cx->y = cx->yomi + n ;

/* bunsetsu kaiseki you sagyou ryouiki */
    cx->maxbun = cx->curbun = 0;
    cx->bunq[0].nb_key = cx->yomi;

/* other stuff */
    cx->kouhomode = kouhomode;
    return _RkRenbun2(cx, 0);
}

/* RkEndBun
 *	bunsetsu henkan wo shuuryou suru
 *	hituyou ni oujite, henkan kekka wo motoni gakushuu wo okonau 
 */
int
RkEndBun(cx_num, mode)
int	cx_num;
int	mode;
{
     register struct RkContext	*cx = RkGetXContext(cx_num);
     int	i;

     if ( cx ) {
	 for ( i = 0; i < cx->maxbun; i++ ) 
	    _RkLearnBun(cx, &cx->bunq[i], mode);
    /* jisho wo unlock suru */
	 _RkEndBun(cx);
	 freeBunStorage(cx);
    };
    return 0;
}

/* RkResize/RkEnlarge/RkShorten
 *	current bunsetsu no ookisa wo henkou
 */
int	
RkResize(cx_num, len)
int	cx_num;
int	len;
{
     register struct RkContext	*cx = RkGetXContext(cx_num);

     if ( cx ){
	 register struct nbun	*bun = &cx->bunq[cx->curbun];
	  if ( 0 < len && (bun->nb_key + len) <= cx->y )
	       return(_RkRenbun2(cx, len));
	  else
	       return(cx->maxbun);
     }
     return( 0 );
}
int	
RkEnlarge(cx_num)
int	cx_num;
{
     register struct RkContext	*cx = RkGetXContext(cx_num);

     if ( cx ) {
	 register struct nbun	*bun = &cx->bunq[cx->curbun];
	  if ( bun->nb_key[bun->nb_curlen] ) {
	       if ( bun->nb_key[bun->nb_curlen++] & 0x80 )
		    bun->nb_curlen++;
	       return(_RkRenbun2(cx, (int)bun->nb_curlen));
	  }
	  else
	       return(cx->maxbun);
     }
     return( 0 );
}
int	
RkShorten(cx_num)
int	cx_num;
{
     register struct RkContext	*cx = RkGetXContext(cx_num);

     if( cx ){
	  register struct nbun	*bun = &cx->bunq[cx->curbun];
	  int			newlen = (unsigned)bun->nb_curlen;

	  if ( newlen > 0 ) {
	        newlen -= (bun->nb_key[newlen - 1] & 0x80) ? 2 : 1;
	        if ( newlen > 0 ) {
		   bun->nb_curlen  = newlen;
		   return(_RkRenbun2(cx, (int)bun->nb_curlen));
		};
	   };
	   return(cx->maxbun);
     }
     return( 0 );
}
/* RkStoreYomi
 *	current bunsetu no yomi wo sitei sareta mono to okikaeru
 *	okikaeta noti, saihen kan suru
 */
int	
RkStoreYomi(cx_num, yomi, maxyomi)
int		cx_num;
unsigned char	*yomi;
int		maxyomi;
{
    int			nlen, olen;
    int			nmax, omax;
    unsigned char	*s, *d, *e;
    register struct RkContext	*cx = RkGetXContext(cx_num);

    if( cx && yomi && maxyomi >= 0 ) {
    	register struct nbun	*bun = &cx->bunq[cx->curbun];
	int			i, off;

     	nlen = maxyomi;
     	olen = bun->nb_curlen;
	nmax = cx->maxyomi + (nlen - olen);
	omax = cx->maxyomi;
     	if ( olen < nlen ) {	/* nobiru */
	    struct RkContext	new;

	/* copy yomi */
	    if ( allocBunStorage(&new, nmax) )
		return 0;
	    (void)strncpy((char *)new.yomi, (char *)cx->yomi,  omax);
	/* copy bunsetu queue */
	    for ( i = off = 0; i <= cx->maxbun; i++ ) {
		new.bunq[i] = cx->bunq[i];
		new.bunq[i].nb_key = new.yomi + off;
		off += new.bunq[i].nb_curlen;
	    };
	    freeBunStorage(cx);
	    cx->yomi = new.yomi;
	    cx->maxyomi = new.maxyomi;
	    cx->bunq = new.bunq;
	    cx->parse = new.parse;
	    cx->succ  = new.succ;

	    bun = &cx->bunq[cx->curbun];
	/* shift string */
	    s = new.yomi + omax;
	    d = new.yomi + nmax;
	    e = bun->nb_key + bun->nb_curlen;
	    while ( s > e ) *--d = *--s;
	}
	else
	if ( nlen < olen ) {	/* chizimu */
	    cx->maxyomi = nmax;
	    s = bun->nb_key + olen;
	    d = bun->nb_key + nlen;
	    e = cx->yomi + omax;
	    while ( s < e ) *d++ = *s++;
	};

	for ( i = cx->curbun + 1; i <= cx->maxbun; i++ ) 
	    cx->bunq[i].nb_key += (nlen - olen);

	(void)strncpy((char *)bun->nb_key, (char *)yomi, nlen);
	cx->yomi[nmax] = 0;
	cx->y = cx->yomi + nmax ;
	return(_RkRenbun2(cx, 0));
    };
    return 0;
}

/* RkGoTo/RkLeft/RkRight
 * 	current bunsetu no idou
 */
int	
RkGoTo(cx_num, bnum)
int	cx_num;
int	bnum;
{
    register struct RkContext	*cx = RkGetXContext(cx_num);

    if( cx ) {
	if ( 0 <= bnum && bnum < cx->maxbun )
	    cx->curbun = bnum;
	return(cx->curbun);
    };
    return 0;
}
int	
RkLeft(cx_num)
int	cx_num;
{
    register struct RkContext	*cx = RkGetXContext(cx_num);

    if( cx ) {
	if ( --cx->curbun < 0 )
	    cx->curbun = cx->maxbun - 1;
	return(cx->curbun);
    };
    return 0;
}
int	
RkRight(cx_num)
int	cx_num;
{
    register struct RkContext	*cx = RkGetXContext(cx_num);
     
    if( cx ) {
	if ( ++cx->curbun >= cx->maxbun )
	    cx->curbun = 0;
	return(cx->curbun);
    };
    return 0;
}

/* RkXfer/RkNfer/RkNext/RkPrev
 *	current kouho wo henkou
 */
static
int
countCand(cx)
struct RkContext	*cx;
{
    struct nbun		*bun = &cx->bunq[cx->curbun];
    int			maxcand = bun->nb_maxcand;
    unsigned		mask;

    for( mask = cx->kouhomode; mask; mask >>= RK_XFERBITS ) 
	maxcand++;
    return maxcand;
}

static
int
getXFER(cx, cnum)
struct RkContext	*cx;
int			cnum;
{
    struct nbun		*bun = &cx->bunq[cx->curbun];

    cnum -= ((int)bun->nb_maxcand);
    if ( cnum < 0 )
	return  RK_NFER;
    else 
	return  (cx->kouhomode>>(RK_XFERBITS*cnum))&RK_XFERMASK;
}

int	
RkXfer(cx_num, knum)
int	cx_num;
int  	knum;
{
    register struct RkContext	*cx = RkGetXContext(cx_num);
    register struct nbun	*bun;

    if( !cx )
	return 0;
    bun = &cx->bunq[cx->curbun];
    if ( 0 <= knum && knum < countCand(cx) )
	bun->nb_curcand = knum;
    return(bun->nb_curcand);
}
int	
RkNfer(cx_num)
int	cx_num;
{
    register struct RkContext	*cx = RkGetXContext(cx_num);
    register struct nbun	*bun;

    if( !cx )
	return 0;
    bun = &cx->bunq[cx->curbun];
    bun->nb_curcand = bun->nb_maxcand + 0;
    return(bun->nb_curcand);
}
int	
RkNext(cx_num)
int	cx_num;
{
    register struct RkContext	*cx = RkGetXContext(cx_num);
    register struct nbun	*bun;

    if( !cx )
	return 0;
    bun = &cx->bunq[cx->curbun];
    bun->nb_curcand++;
    if ( bun->nb_curcand >= countCand(cx) )
       bun->nb_curcand = 0;
    return(bun->nb_curcand);
}
int	
RkPrev(cx_num)
int	cx_num;
{
    register struct RkContext	*cx = RkGetXContext(cx_num);
    register struct nbun	*bun;

    if( !cx )
	return 0;
    bun = &cx->bunq[cx->curbun];
    if ( bun->nb_curcand )
    	bun->nb_curcand--;
    else
    	bun->nb_curcand = countCand(cx) - 1;
    return(bun->nb_curcand);
}
/* findBranch
 * 	shiteisareta kouho wo fukumu path wo motomeru
 */
static
struct nword	*
findBranch(cx, bnum, cnum)
struct RkContext	*cx;
int			bnum;
int			cnum;
{
     struct nbun	*bun = &cx->bunq[bnum];

     if ( 0 <= cnum && cnum < bun->nb_maxcand ) {
	 struct nword	*w;

	 for ( w = bun->nb_cand; w; w = w->nw_next )
	       if ( !purgeword(w->nw_flags) &&
		    bun->nb_curlen == w->nw_ylen ) {
		    if ( cnum-- <= 0 )
			 return w;
	       };
     };
     return (struct nword *)0;
}
/* RkGetStat
 */
int
RkGetStat(cx_num, st)
int	cx_num;
RkStat	*st;
{
    struct RkContext		*cx = RkGetXContext(cx_num);

/* set up void values */
    st->bunnum = st->candnum = 0;
    st->maxcand = st->diccand = 0;
    st->ylen = st->klen = st->tlen = 0;
    if( cx ) {	
	struct nbun	*bun = &cx->bunq[cx->curbun];
	struct nword	*cw, *lw;

	st->bunnum  = cx->curbun;
	st->candnum = bun->nb_curcand;

	st->maxcand = countCand(cx);
	st->diccand = bun->nb_maxcand;
	st->ylen    = bun->nb_curlen;
	st->klen    = bun->nb_curlen;
	st->tlen    = 1;
    /* look up the word node containing the current candidate */
    	if ( cw = findBranch(cx, cx->curbun, bun->nb_curcand) ) {
	    st->klen = st->tlen = 0;
	    for ( ; cw; cw = cw->nw_left ) {
		lw = cw->nw_left;
		if ( !lw )
		    break;
		if ( cw->nw_klen == lw->nw_klen )
		    st->klen += (cw->nw_ylen - lw->nw_ylen);
		else
		    st->klen += (cw->nw_klen - lw->nw_klen);
		st->tlen++;
	    };
	}
	else {
	    unsigned char	*yomi = bun->nb_key;

	    switch(getXFER(cx, bun->nb_curcand)) {
	    default:
	    case RK_XFER:
		st->klen = RkCvtHira((unsigned char *)0, 0, yomi, st->ylen);
		break;
	    case RK_KFER:
		st->klen = RkCvtKana((unsigned char *)0, 0, yomi, st->ylen);
		break;
	    case RK_HFER:
		st->klen = RkCvtHan((unsigned char *)0, 0, yomi, st->ylen);
		break;
	    case RK_ZFER:
		st->klen = RkCvtZen((unsigned char *)0, 0, yomi, st->ylen);
		break;
	    };
	};
	return 0;
    };
    return -1;
}
/* RkGetYomi
 *	current bunsetu no yomi wo toru
 */
static
int
addIt(cw, key, proc, dst, ind, maxdst)
register struct nword	*cw;
unsigned char	*key;
int		(*proc)();
char		*dst;
int		ind;
int		maxdst;
{
    register struct nword	*lw;

    if ( lw = cw->nw_left ) {
	register unsigned char	*y;
	RkLex			lex;

	ind = addIt(lw, key, proc, dst, ind, maxdst);

	y = key + lw->nw_ylen;
	lex.ylen = cw->nw_ylen - lw->nw_ylen;
	lex.klen = cw->nw_klen - lw->nw_klen;
	lex.rownum = cw->nw_row;
	lex.colnum = cw->nw_col;
	lex.dicnum = word2class(cw->nw_flags);
	ind = (*proc)(dst, ind, maxdst, y, _RkGetKanji(cw, y), &lex);
    };
    return ind;
}

static
int
getIt(cx, cnum, proc, dst, max)
struct RkContext	*cx;
int			cnum;
int			(*proc)();
unsigned char		*dst;
int			max;
{
    int			bnum = cx->curbun;
    struct nbun		*bun = &cx->bunq[bnum];
    struct nword	*w;

    if ( w = findBranch(cx, bnum, cnum) ) 
	 return addIt(w, bun->nb_key, proc, (char *)dst, 0, max);
    else
	 return -1;
}
/*ARGSUSED*/
static
int
addYomi(dst, ind, max, yomi, kanji, lex)
unsigned char	*dst;
int		ind;
int		max;
unsigned char	*yomi;
unsigned char	*kanji;
RkLex		*lex;
{
    int		i, ylen;

    ylen = lex->ylen;
    while ( ylen ) {
	int	bytes = (*yomi&0x80) ? 2 : 1;

	ylen -= bytes;
	if ( ind + bytes <= max ) {
	    if ( dst )
		for ( i = 0; i < bytes; i++ )
		    dst[ind + i] = yomi[i];
	    ind += bytes;
	};
	yomi += bytes;
    };
    return ind;
}
int
RkGetYomi(cx_num, yomi, maxyomi)
int		cx_num;
unsigned char	*yomi;
int		maxyomi;
{
    register struct RkContext	*cx = RkGetXContext(cx_num);

    if ( cx ) {
	struct nbun	*bun = &cx->bunq[cx->curbun];
	RkLex		lex;
	int		i;

	lex.ylen = bun->nb_curlen;
	i = addYomi(yomi, 0, maxyomi - 1, bun->nb_key, bun->nb_key, &lex);
	if ( yomi && i < maxyomi )
	    yomi[i] = 0;
	return i;
    }
    else
	return -1;
}
/* RkGetKanji
 *	current bunsetu no kanji tuduri wo toru
 */
/*ARGSUSED*/
static
int
addKanji(dst, ind, max, yomi, kanji, lex)
char		*dst;
int		ind;
int		max;
unsigned char	*yomi;
unsigned char	*kanji;
RkLex		*lex;
{
    int		i, klen;

    klen = lex->klen;
    while ( klen ) {
	int	bytes = (*kanji&0x80) ? 2 : 1;

	klen -= bytes;
	if ( ind + bytes <= max ) {
	    if ( dst ) {
		for ( i = 0; i < bytes; i++ )
		    dst[ind + i] = kanji[i];
	    };
	    ind += bytes;
	};
	kanji += bytes;
    };
    return ind;
}

static
int
getKanji(cx, cnum, dst, maxdst)
struct RkContext	*cx;
int			cnum;
unsigned char		*dst;
int			maxdst;
{
    struct nbun		*bun = &cx->bunq[cx->curbun];
    int			i;

    i = getIt(cx, cnum, addKanji, dst, maxdst - 1);
    if ( i < 0 ) {
	unsigned char	*yomi = bun->nb_key;
	int		ylen = bun->nb_curlen;

	switch(getXFER(cx, cnum)) {
	default:
	case RK_XFER:
	    i = RkCvtHira(dst, maxdst, yomi, ylen); break;
	case RK_KFER:
	    i = RkCvtKana(dst, maxdst, yomi, ylen); break;
	case RK_HFER:
	    i = RkCvtHan(dst, maxdst, yomi, ylen); break;
	case RK_ZFER:
	    i = RkCvtZen(dst, maxdst, yomi, ylen); break;
	};
    };
    if ( dst && i < maxdst )
	dst[i] = 0;
    return i;
}
int
RkGetKanji(cx_num, dst, maxdst)
int		cx_num;
unsigned char	*dst;
int		maxdst;
{
    register struct RkContext	*cx = RkGetXContext(cx_num);

    if ( cx ) {
	struct nbun	*bun = &cx->bunq[cx->curbun];
	int		i;
	
	i = getKanji(cx, bun->nb_curcand, dst, maxdst);
	if ( dst && i < maxdst )
	    dst[i] = 0;
	return i;
    };
    return -1;
}
/* RkGetKanjiList
 * 	genzai sentaku sareta kouho mojiretu wo toridasu
 */
int	
RkGetKanjiList(cx_num, dst, maxdst)
int		cx_num;
unsigned char	*dst;
int		maxdst;
{
    register struct RkContext	*cx = RkGetXContext(cx_num);

    if ( cx ) {
	int		i, len, ind, num;
	int		maxcand = countCand(cx);

	num = ind = 0;
	for ( i = 0; i < maxcand; i++ ) {
	    if ( dst )
		len = getKanji(cx, i, dst + ind, maxdst - ind - 1);
	    else
		len = getKanji(cx, i, dst, maxdst - ind - 1);
	    if ( 0 < len && ind + len + 1 < maxdst - 1 ) {
		if ( dst )
		    dst[ind + len] = 0;
		ind += len + 1;
		num++;
	    };
	};
	if ( dst && ind < maxdst )
	    dst[ind] = 0;
	return num;
    }
    else
	return -1;
}
/* RkGetLex
 *	current bunsetu no kanji tuduri wo toru
 */
/*ARGSUSED*/
static
int
addLex(dst, ind, max, yomi, kanji, lex)
RkLex		*dst;
int		ind;
int		max;
unsigned char	*yomi;
unsigned char	*kanji;
RkLex		*lex;
{
    if ( ind + 1 <= max ) {
	if ( dst )
	   dst[ind] = *lex;
	ind++;
    };
    return ind;
}
int
RkGetLex(cx_num, dst, maxdst)
int		cx_num;
RkLex		*dst;
int		maxdst;
{
    register struct RkContext	*cx = RkGetXContext(cx_num);
    int		i = -1;

    if ( cx ) {
	struct nbun	*bun = &cx->bunq[cx->curbun];
	
	i = getIt(cx, bun->nb_curcand, addLex, (char *)dst, maxdst - 1);
	if ( i < 0 ) {
	    if ( dst && 1 < maxdst ) {
		dst[0].ylen = bun->nb_curlen;
		dst[0].klen = bun->nb_curlen;
		dst[0].rownum  = pair2row(BUNSETU);
		dst[0].colnum  = pair2col(BUNSETU);
		dst[0].dicnum  = ND_EMP;
	    };
	    i = 1;
	};
    };
    return i;
}
