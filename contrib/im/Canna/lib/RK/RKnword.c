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
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/lib/RK/RCS/RKnword.c,v 3.1 91/06/25 10:09:17 kon Exp $";
#endif
/*LINTLIBRARY*/
#include	<ctype.h>
#include	"RK.h"
#include	"RKintern.h"
#include	<stdio.h>

/* 
 *  WORD
 */
/* _RkInitializeWord
 */
/* ARGSUSED */
int
_RkInitializeWord(numWord)
int	numWord;
{
    SX.word = (struct nword *)0;
    return 0;
}

#define	clearWord(p) \
{\
	(p)->nw_cache = (struct ncache *)0;\
	(p)->nw_row = pair2row(BUNSETU);\
	(p)->nw_col = pair2col(BUNSETU);\
	(p)->nw_ylen = (p)->nw_klen = 0;\
	(p)->nw_flags = ND_EMP;\
	(p)->nw_lit = 0;\
	(p)->nw_prio = 0;\
	(p)->nw_left = (p)->nw_next = (struct nword *)0;\
	(p)->nw_kanji = (unsigned char *)0;\
}
static
struct nword	*
allocWord()
{
    register struct nword	*p;

    if ( p = SX.word )  {		/* recycle word */
	SX.word = p->nw_next;
	RkDebug("recycled word %d", p, 0, 0);
    }
    else {
	p = (struct nword *)malloc(sizeof(struct nword));
    };
/* shorika */
    if ( p ) 
	clearWord(p);
    return p;
}
/* _RkFinalizeWord
 */
void
_RkFinalizeWord()
{
    register struct nword	*w, *t;
    
    for ( w = SX.word; w; w = t ) {
	t = w->nw_next;
	(void)free((char *)w);
    };
    SX.word = (struct nword *)0;
}

/* concWord
 *	tango p, q wo setuzoku sita tango pq wo tukuru 
 * returns:
 */
static
struct nword	*
concWord(p, q)
register struct nword	*p, *q;
{
    register struct nword	*pq = (struct nword *)0;

/* nagasa no check */
    if ( p->nw_ylen + q->nw_ylen > 255 )
	return pq;
    if ( p->nw_klen + q->nw_klen > 255 )
	return pq;

/* free list kara hitotu toru */
    pq = allocWord();
    if ( !pq )
	return pq;
    *pq = *q;
    pq->nw_ylen  += p->nw_ylen;
    pq->nw_klen  += p->nw_klen;
    pq->nw_flags = ((p->nw_flags)&(Rk_PRE|Rk_MWD|Rk_SUC|Rk_SWD))
			    |word2class(q->nw_flags);
    pq->nw_prio = p->nw_prio;
    pq->nw_left = p;
    pq->nw_next = (struct nword *)0;
/* cache no sanshoudo wo kousinn suru */
    if ( pq->nw_cache )
	(void)_RkEnrefCache(pq->nw_cache);

    switch(word2class(q->nw_flags)) {
/* kakko, kutouten ha setuzoku kankei ni eikyou sinai */
    case	ND_OPN:
    case	ND_CLS:
    case	ND_PUN:
	pq->nw_row = p->nw_row;
	pq->nw_col = p->nw_col;
	if ( word2class(p->nw_flags) != ND_EMP )
	    pq->nw_flags = p->nw_flags;
	else
	    pq->nw_flags = q->nw_flags;
	break;
    case	ND_MWD:
	pq->nw_flags |= Rk_MWD;
	pq->nw_prio = q->nw_prio;
	break;
    case	ND_SWD:
	if ( !(pq->nw_flags&Rk_SWD) ) 
	    pq->nw_flags |= Rk_SWD;
	break;
    case	ND_PRE:
	pq->nw_flags |= Rk_PRE;
	break;
    case	ND_SUC:
	pq->nw_flags |= Rk_SUC;
	break;
    };
    return pq;
}

/* freeWord
 *	word wo kaihou suru
 */
static
void
derefWord(word)
struct nword	*word;
{
    register struct nword	*p;
    
    RkDebug("DEREF{", 0, 0, 0);
    for ( p = word; p; p = p->nw_next ) 
	if ( p->nw_cache )
	    (void)_RkDerefCache(p->nw_cache);
    RkDebug("DEREF}", 0, 0, 0);
}

static
void	
killWord(word)
struct nword	*word;
{
    register struct nword	*p, *q;
    
    if ( word ) {
	for ( p = q = word; p; q = p, p = p->nw_next ) {
		if ( !p->nw_cache && p->nw_kanji ) 
		    (void)free((char *)p->nw_kanji);
	};
	q->nw_next = SX.word;
	SX.word = word;
    };
}

static
void	
freeWord(word)
struct nword	*word;
{
    derefWord(word);
    killWord(word);
}
/* createWord
 *	jisho ni nai katakana, suuji, tokushu moji wo tango to minasu
 */
static
void
setWord(w, rc, lit, yomi, ylen, kanji, klen)
register struct nword	*w;
int			rc;
int			lit;
unsigned char		*yomi;
int			ylen;
unsigned char		*kanji;
int			klen;
{
    clearWord(w);
    w->nw_row = pair2row(rc);
    w->nw_col = pair2col(rc);
    w->nw_ylen = ylen;
    w->nw_klen = klen;
    w->nw_flags = Rk_MWD;
    w->nw_lit = lit;
    w->nw_kanji = kanji;
}

/*
 * Literal
 */	
static
int
cvtNum(dst, maxdst, src, maxsrc, format)
unsigned char		*dst;
int			maxdst;
unsigned char		*src;
int			maxsrc;
int			format;
{
    return RkCvtSuuji(dst, maxdst, src, maxsrc, format - 1);
}
static
int
cvtAlpha(dst, maxdst, src, maxsrc, format)
unsigned char		*dst;
int			maxdst;
unsigned char		*src;
int			maxsrc;
int			format;
{
    switch(format) {
#ifdef ALPHA_CONVERSION
    case 1: 	return RkCvtZen(dst, maxdst, src, maxsrc);
    case 2: 	return RkCvtHan(dst, maxdst, src, maxsrc);
    case 3: 	return -1;
#else
    case 1: 	return RkCvtNone(dst, maxdst, src, maxsrc);
    case 2: 	return -1;
#endif
    default: 	return 0;
    }
}
static
int
cvtHira(dst, maxdst, src, maxsrc, format)
unsigned char		*dst;
int			maxdst;
unsigned char		*src;
int			maxsrc;
int			format;
{
    switch(format) {
    case 1: 	return RkCvtHira(dst, maxdst, src, maxsrc);
    case 2: 	return RkCvtKana(dst, maxdst, src, maxsrc);
    default: 	return 0;
    }
}

static
int
cvtLit(dst, maxdst, src, maxsrc, format)
unsigned char		*dst;
int			maxdst;
unsigned char		*src;
int			maxsrc;
int			format;
{
    switch(format>>4) {
    case LIT_NUM: 	return cvtNum(dst, maxdst, src, maxsrc, format&15);
    case LIT_ALPHA: 	return cvtAlpha(dst, maxdst, src, maxsrc, format&15);
    case LIT_HIRA: 	return cvtHira(dst, maxdst, src, maxsrc, format&15);
    default:		return 0;
    };
}

static
int
setLit(cx, word, maxword, rc, src, srclen, format)
struct RkContext	*cx;
struct nword		*word;
int			maxword;
int			rc;
unsigned char		*src;
int			srclen;
int			format;
{
    struct nword	*w = word;
    unsigned char	*dst;
    int 		dstlen;
    unsigned long	mode;

    if ( !cx->litmode )
	return 0;
    for ( mode = cx->litmode[format]; mode; mode >>= RK_XFERBITS )
	if ( w < word + maxword ) {
	    int	code = MAKELIT(format, mode&RK_XFERMASK);

	    dstlen = cvtLit((unsigned char *)0, 9999, src, srclen, code);
	    if ( 0 < dstlen  && dstlen < 256 ) 
		setWord(w++, rc, code, src, srclen, (unsigned char *)0, dstlen);
	    if ( dstlen < 0 ) 
		setWord(w++, rc, code, src, srclen, (unsigned char *)0, srclen);
	};
    return w - word;
}

static
struct nword	*
createWord(cx, class, key, ys, ye, word, maxword)
struct RkContext		*cx;
int				class;
register unsigned char		*key;
int				ys, ye;
struct nword			*word;
int				maxword;
{
    register unsigned char	*k, *z;
    register struct nword	*w = word;
    unsigned			c, d;
    int				hinshi;

    if ( ye <= 0 || class != ND_MWD ) 
	return w;
    k = key;
    z = key + ye;
/* sentou moji wo yomu */
    if ( (c = *k++) & 0x80 ) 
	c = (c<<8)|*(k++); 
/* hiragana ga kurumade yomitobasu */
    if ( c < 128 ) {	 	/* ascii string */
	if ( isdigit(c) ) {
	    for ( ; k < z && isascii(*k); k++ ) 
		if ( !isdigit(*k) ) break;
	    if ( ys < (k - key) )
		w += setLit(cx, w, maxword, P_NN, key, k - key, LIT_NUM);
	}
	else {
#ifndef PINYIN_BASED_CONVERSION
	    for ( ; k < z && isascii(*k); k++ )
		 continue;
#endif /* PINYIN_BASED_CONVERSION */
	    if ( ys < (k - key) )
		w += setLit(cx, w, maxword, P_T35, key, k - key, LIT_ALPHA);
	};
	return w;
    }
    else
    if ( 0xb000 <= c ) {	/* kanji string */
	hinshi = P_T00;
	for ( ; k < z && *k >= 0xb0 ; k += 2 );
    }
    else {			/* kana string */
	switch(c>>8) {
	case	0xa3:		/* ei/suuji */
	    if ( 0xa3b0 <= c && c <= 0xa3b9 ) {
		for ( ; k < z && *k == 0xa3 ; k += 2 ) 
		    if ( !(0xb0 <= k[1] && k[1] <= 0xb9) || k - key > 64) 
			break;
		if ( ys < (k - key) )
		    w += setLit(cx, w, maxword, P_NN, key, k - key, LIT_NUM);
	    }
	    else {
		for ( ; k < z && *k == 0xa3 ; k += 2 );
		if ( ys < (k - key) )
		    w += setLit(cx, w, maxword, P_T35, key, k - key, LIT_ALPHA);
	    };
	    return w;
	case	0x8e:		/* hankaku katakana */
	    for ( ; k < z && *k == 0x8e ; k += 2 );
	    hinshi = P_T30;
	    break;
	case	0xa5:	/* katakana */
	    while ( k < z ) 
		if ( (d = *(k++)) & 0x80 ) {
		    if ( d != 0xa1 && d != 0xa5 ) {
			k--;
			break;
		    };
		    k++;
		}
		else 
		if ( d != '-' ) {
		    k--;
		    break;
		};
	    hinshi = P_T30;
	    break;
	case	0xa4: 	/* hiragana */
	    hinshi = P_T35;
	    while ( k < z ) 
		if ( (d = *(k++)) & 0x80 ) {
		    if ( d == 0xa4 ) {
			switch(*k++) {
		/* la li lu le lo */
			case 0xa1: case 0xa3: case 0xa5: case 0xa7: case 0xa9:
		/* lya lyu lyo */
			case 0xe3: case	0xe5: case 0xe7:
			case	0xc3:	/* t */
			case	0xf3:	/* n */
			    continue;
			default:
			    k -= 2;
			    goto	hira;
			};
		    }
		    else
		    if ( d == 0xa1 ) {
			switch(*k++) {
			case 0xab: case 0xac: 
			case 0xb3: case 0xb4: case 0xb5: case 0xb6: case 0xbc:
			    continue;
			default:
			    k -= 2;
			    goto hira;
			};
		    };
		/* fail */
		    k--;
		    goto hira;
		}
		else 
		if ( d == '-' ) 
		    continue;
		else {
		    k--;
		    goto hira;
		};
	hira:
	    if ( ys < (k - key) )
		w += setLit(cx, w, maxword, hinshi, key, k - key, LIT_HIRA);
	    return w;

	default:
	    hinshi = P_T35;
	    break;
	};
    };
    if ( w < word + maxword && ys < (k - key) ) 
	setWord(w++, hinshi, 0, key, k - key, 0, k - key);
    return w;
}
/* isKuten
 *	kutouten
 */
static
int	
isKuten(key, n)
unsigned char	*key;
int	n;
{
    if ( n < 2 || *key != 0xa1 )
	return(0);
    switch(key[1]) {
    case	0xa2:	/* ten */
    case	0xa3:	/* maru */
    case	0xa4:	/* , */
    case	0xa5:	/* . */
    case	0xa6:	/* nakaten */
    case	0xa7:	/* : */
    case	0xa8:	/* ; */
    case	0xa9:	/* ? */
    case	0xaa:	/* ! */
	return(ND_PUN);
    case	0xc6:	 case	0xc7:
    case	0xc8:	 case	0xc9:
    case	0xca:	 case	0xcb:
    case	0xcc:	 case	0xcd:
    case	0xce:	 case	0xcf:
    case	0xd0:	 case	0xd1:
    case	0xd2:	 case	0xd3:
    case	0xd4:	 case	0xd5:
    case	0xd6:	 case	0xd7:
    case	0xd8:	 case	0xd9:
    case	0xda:	 case	0xdb:
	return((key[1]&1) ? ND_CLS : ND_OPN);
    default:
	return(0);
    };
}

/*
 * the Hardcore of the Saichou 2bunsetu Icchi Method
 */

/* classmask
 *	bunsetu bunpou wo teigi 
 */
static unsigned	classmask[] = {
	8+2,		/* MWD --> SUC | SWD */
	2,		/* SWD --> SWD */
	1+2,		/* PRE --> MWD | SWD */
	2,		/* SUC --> SWD */
	1+2+4,		/* EMP --> MWD | SWD | PRE  */
};
/* readWord
 *	shitei sareta class no jisho kara tango wo hiku
 *	key de hajimaru tango ga nai baai ni kagiri,
 *	katte ni tango wo katei suru
 */

static
struct nword	*
readWord(cx, class, key, ys, ye, nword, maxword, douniq)
struct RkContext	*cx;
int			class;		/* word class */
unsigned char		*key;		/* prefix key */
int			ys, ye;		/* key no saidai chouu */
struct nword		*nword;		/* match suru word */
int			maxword;
int			douniq;		/* hinshi wo uniq ni suru */
{
    register struct nword	*w;
    struct ncache		*ncache[64];
    int				maxcache;
    int				maxlen;
    struct MD			*head, *md;

/* mazu jisho kara kensaku suru */
    w = nword;
/* jisho kara word wo yomu */
    maxcache = sizeof(ncache)/sizeof(struct ncache *);
    maxlen = 0;
    head = cx->mount[class];
    for ( md = head->md_next; md != head; md = md->md_next ) {
	struct DM	*dm = md->md_dic;
	int		c, nc, num;

	if ( !dm )
	    continue;
	if ( maxword <=  0 )
	     break;
	nc = DST_SEARCH(dm, key, ye, ncache, maxcache);
	for ( c = 0; c < nc; c++ ) {
	    int	len, nk;
	    register  unsigned char	*K, Key, *C, pak;

	    C = ncache[c]->nc_word;
	/* key to cache ga match surumono dake wo nokosu */
	    K = key;
	    if ( pak = dm->dm_packbyte ) {
		for ( len = 0;len < ye; C++ ) 
		    if ( (Key = (*K++)) & 0x80 ) {
			if ( (Key == pak) && (*C == *K) ) {
			    len += 2; K++;
			}
			else if ( Key == 0xa1 && (*C == '-') && *K == 0xbc ) {
			    len += 2; K++;
			}
			else if ( Key == 0xa3 && *C == (*K & 0x7f)) {
			  /* Hankaku Suuji, by KON 1990.9.13 */
			  len += 2; K++;
			}
		        else
			    break;
		    }
		    else {
			if ( *C == Key ) 
			    len++;
			else
			    break;
		    };
	   }
	   else {
	       for ( len = 0; len < ye; C++ ) {
		    if ( *C == (Key = (*K++)) )
			len++;
		    else
		    if ( *C == '-' && Key == 0xa1 && K[0] ==0xbc ) {
			K++; len += 2;
		    }
		    else if ( Key == 0xa3 && *C == (*K & 0x7f)) {
		      /* Hankaku Suuji, by KON 1990.9.13 */
                      len += 2; K++;
		    }
		    else
			break;
	       }
	   };

	/* full match */
	   if ( *C++ == 0 && (ys < len && len <= ye) )  {
		struct nword	*pp, *qq;
	       if ( maxlen < len )
		    maxlen = len;
	   /* word record wo hinshi tanni ni bunkai suru */
	       nk = *C++;
	       pp = w;
	       for ( num = 0; num < nk; num++ ) {
		    unsigned	flags, freq;


		    if ( maxword <=  0 )
			 break;
		    clearWord(w);
		    w->nw_kanji = C;
		    flags = *C++;
		    w->nw_row = *C++;
		    w->nw_col = *C++;
		    freq = *C++;
		    if ( row256(flags) ) w->nw_row += 256;
		    if ( col256(flags) ) w->nw_col += 256;
		    C += candlen(flags);
		/* remove dup */
		    if ( douniq ) {
			for ( qq = pp; qq < w; qq++ )
			    if ( qq->nw_row == w->nw_row &&
				 qq->nw_col == w->nw_col )
				 break;
			if ( qq < w )
			    continue;
		    };
		/* atarashii word */
		    w->nw_cache = ncache[c];
		    _RkEnrefCache(ncache[c]);
		    w->nw_ylen = len;
		    w->nw_klen = candlen(flags);
		    if ( w->nw_klen == 0  )
			w->nw_klen = len;
		    w->nw_flags = class;
		    w->nw_prio  = 0;
		    if ( class == ND_MWD ) {
		    /* set log2(freq) */
			for ( w->nw_prio = 0; freq; freq >>= 1 ) 
			   w->nw_prio++;
			if ( num == 0  ) {
			    if ( w->nw_prio )
				w->nw_prio |= 0x0008;
			    if ( ncache[c]->nc_utime ) {
				long	t;
				
				t = _RkGetTick(0)- ncache[c]->nc_utime;
				if ( 0 <= t && t < 0x400 )
				    t = (0x400 - t);
				else				/* katute */
				    t = 0x1;
				w->nw_prio |= (t<<4);
			    };
			};
		    };
		    w++;
		    maxword--;
		};
	    };
	/* release cache */
	    _RkDerefCache(ncache[c]);
	};
	maxcache -= nc;
    };
    return(w);
}

/* concDicWord
 *	setuzoku kanou na tango wo jisho kara yomidasu
 */
static
void		
concDicWord(cx, class, key, ys, ye, length, maxlen, douniq)
struct RkContext	*cx;
int			class;
unsigned char		*key;
int			ys, ye;
struct nword		*length[];	/* length table */
int			*maxlen;	/* mottomo nagai yomi no nagasa */
int			douniq;
{
    int			l, minword;
    struct RkKxGram	*gram = SX.gramdic;

/*
 * yomi no nagasa ga mijikai monokara setuzoku wo kokoromiru
 * maxlen ha loop naide kousinn sareru.
 */
    for ( l = 0; l <= *maxlen && l < ye; l += minword ) {
	struct nword		*tail[256];
	struct nword		right[64*16];
	int			nt, t;
	register struct nword	*p, *q, *r;
	int			pun;
	int			ys1, ye1;

	minword = (key[l] & 0x80) ? 2 : 1;
    /* nagasa l no word de, setuzoku dekiru mono wo tail he atumeru */
	for ( p = length[l], nt = 0; p; p = p->nw_next ) 
	    if ( classmask[word2class(p->nw_flags)] & (1<<class) )
		if ( nt < sizeof(tail)/sizeof(tail[0]) )
		    tail[nt++] = p;
	if ( !nt )
	    continue;
    /* key + l kara hajimaru word wo right he atumeru */
	ys1 = ys - l;  if( ys1 < 0 )  ys1 = 0;
	ye1 = ye - l;
	r = readWord(cx, class, key + l, ys1, ye1,
			right, sizeof(right)/sizeof(right[0])-1, douniq);
    /* kutouten no shori */
	if( pun = isKuten(key + l, ye) ) {
	    clearWord(r);
	    r->nw_flags = pun;
	    r->nw_ylen = r->nw_klen = minword;
	    r++;
	};
    /* mitourokugo shori */
	if ( class == ND_MWD ) {
	/* hiragana no tokiha, MWD ga arawareru iti made yomitobasu beki !! */
	    r = createWord(cx, class, key + l, ys1, ye1,
			    r, sizeof(right)/sizeof(right[0])-1 - (r - right));
	};

    /* tail[t] to tunageru */
	for ( t = 0; t < nt; t++ ) {
	    unsigned char	*cj;

        /* setuzoku kanou na tango dake wo tukuru */
	    p = tail[t];
	    cj = gram ? GetGramRow(gram, p->nw_row) : 0;
	    for ( q = right; q < r; q++ )  {
		if ( word2class(q->nw_flags) >= ND_OPN ||
		     !gram || 
		     TestGram(cj, q->nw_col) )  {
		     struct nword	*pq = concWord(p, q);

		     if ( pq ) {
			 int	ylen = pq->nw_ylen;
			 struct nword	*s;

		     /* bunsetu shuutann to nariuruka ? */
			 if ( gram && !IsShuutan(gram, pq->nw_row) )
			    pq->nw_flags |= NW_PURGE;
		    /* sakusei jun ni naraberu */
			 if ( *maxlen < ylen ) 
			      length[*maxlen = ylen] = pq;
			 else
			 if ( s = length[ylen] ) {
			    while ( s->nw_next )
				s = s->nw_next;
			    s->nw_next = pq;
			 }
			 else 
			     length[ylen] = pq;
			pq->nw_next = (struct nword *)0;
		    };
		};
	    };
	};	
    /* setuzoku dekinakattamonoha free list ni kaesu */
	for ( q = right; q < r; q++ ) 
	    if ( q->nw_cache )
		_RkDerefCache(q->nw_cache);
    /* kutouren rui ha 1 kai de owaru */
	switch(pun) {
	case	ND_OPN:
	    if ( l >  0 )	return;
	    break;
	case	ND_CLS:
	    if ( l <= 0 )	return;
	    break;
	case	ND_PUN:
	    return;
	};
    /* bunpou ga nakereba ikkai de owari */
	if ( !gram )
	    return;
    };	/* end of length list */
}

/* detSuc
 *	sentou bunsetu no nagasa wo kettei suru
 */
struct sucRec {
    int	u2;
    short	l2;
    short	count;		/* */
};

static
void
evalSuc(suc, ul)
struct nword	*suc;
struct sucRec	*ul;
{
      register struct nword	*p, *q;
      register int		l2, u2, count;

      l2 = u2 = count = 0;
      for ( p = suc; p; p = p->nw_next )  {
	    if ( purgeword(p->nw_flags) || (p->nw_flags & Rk_SUC) )
		continue;
	    if ( l2 < p->nw_ylen ) 
		l2 = p->nw_ylen;
	    if ( u2 < p->nw_prio )
		u2 = p->nw_prio;
	    if (word2class(p->nw_flags) == ND_MWD && p->nw_cache ) 
		count++;
      };
     ul->l2 = l2;
     ul->u2 = u2;
     ul->count = count;
}

static
int	
detSuc(y, top, suc, maxlen, flush)
unsigned char	*y;
struct nword	*top;
struct nword	*suc[];
int		maxlen;
int		flush;
{
     int			L, L1, L2;
     int			U1, U2;
     register struct nword	*w;
     int			minl;
     struct sucRec		ul2[256];
     int			i, max;
	
     minl = (*y & 0x80) ? 2: 1;
     L = (L1 = minl)+ (L2 = 0);
     U1 = U2 = 0;

     max = sizeof(ul2)/sizeof(ul2[0]) - 1;
     if ( max > maxlen )
	max = maxlen;
     for ( i = 0; i <= max; i++ )
	ul2[i].l2 = ul2[i].u2 = 0;

     for ( w = top; w; w = w->nw_next ) {
	  int				l, l1;
	  int				u1;
	  struct sucRec			ul;
	  register struct nword		*p;

	  if ( purgeword(w->nw_flags) ) 
	       continue;
	/* mijikasugiru/bunsetumatu ni narenai  monoha hazusu */
	  l1 = w->nw_ylen;
	  u1 = w->nw_prio;
	  if ( l1 <= 0 )
	      continue;
	/* ichido ni 2tu tukomono ha yameru */
	  if ( (w->nw_flags & Rk_PRE) && (w->nw_flags & Rk_SUC) )  
		continue;
	/* shuujoushi ha bun no tochuu deha tukanai */
	  if ( flush && !y[l1] ) {
	       L1 = l1;
	       break;
	  };
	/* migi donari no bunsetsu wo kaiseki */
	  if ( l1 <= max ) {
	      if ( !ul2[l1].l2 ) 
		  evalSuc(suc[l1], &ul2[l1]);
	      ul =ul2[l1];
	  }
	  else 
	      evalSuc(suc[l1], &ul);
	/* hikaku */
	  l = l1 + ul.l2;
	  if (((L < l)) ||
	      ((L == l) && (U2 < ul.u2)) ||
	      ((L == l) && (U2 == ul.u2) && (L2 < ul.l2)) ){
		L = l;
		L1 = l1;
		L2 = ul.l2;
		U2 = ul.u2;
	    }
    };
    return(L1);
}

/* parseBun
 *	key yori hajimaru bunsetsu kouzou wo kaiseki suru
 */
/* getKanji
 *	get kanji in reverse order 
 */
unsigned char	*
_RkGetKanji(cw, key)
struct nword    *cw;
unsigned char   *key;
{
    register unsigned char  	*str;

    if ( cw->nw_cache ) {
	if ( candlen(cw->nw_kanji[0]) )	/* kanji kouho */
	    str = cw->nw_kanji + NW_PREFIX;
	else
	    str = key;			/* hiragana kouho */
    }
    else 
    if ( cw->nw_kanji ) 		/* user kouho */
	str = cw->nw_kanji;
    else 
    if ( cw->nw_lit ) {			/* literal kouho */
	unsigned		klen, ylen;
	static unsigned char 	tmp[256];
	struct nword		*lw = cw->nw_left;

	klen = (cw->nw_klen - lw->nw_klen);
	ylen = (cw->nw_ylen - lw->nw_ylen);
	if ( cvtLit(tmp, klen+1, key, ylen, cw->nw_lit) > 0 )
	    str = tmp;
	else
	    str = key;
    }
    else 				/* miteigi */
	str = key;
    return str;
}

static
int
getKanji(w, key, d)
struct nword    *w;
unsigned char   *key;
unsigned char   *d;
{
    register struct nword	*cw, *lw;
    int				hash, klen;

    hash = 0;
    for ( cw = w; cw; cw = lw ) {
	register unsigned char  	*s;
	register unsigned char  	*t;

	lw = cw->nw_left;
	if ( !lw )
	    continue;
	klen = (cw->nw_klen - lw->nw_klen);
	s = _RkGetKanji(cw, key + lw->nw_ylen);
	t = s + klen;
    /* copy */
	while ( s < t ) {
	    *d++ = *--t;
	    if ( *t & 0x80 )  {
		*d++ = *--t;
		hash += *t;
	    }
	    else
		hash += *t;
	};
    };
    return hash;
}

/* uniqWord
 *	unique word list
 */
static
uniqWord(key, words, ylen)
unsigned char	*key;
struct nword	*words;
unsigned	ylen;
{
    register struct nword	*p;
    int				uniq[16], heap[512];
    int				hp = 0;

    if ( !key || ylen <= 0 )
	return;
/* clear hash table */
    uniq[ 0] = uniq[ 1] = uniq[ 2] = uniq[ 3] = 
    uniq[ 4] = uniq[ 5] = uniq[ 6] = uniq[ 7] = 
    uniq[ 8] = uniq[ 9] = uniq[10] = uniq[11] = 
    uniq[12] = uniq[13] = uniq[14] = uniq[15] = -1;
    for ( p = words; p; p = p->nw_next ) 
	if ( !purgeword(p->nw_flags) && p->nw_ylen == ylen ) {
	    int			wsize;

	/* compute word size */
	    wsize = (p->nw_klen + sizeof(heap[0])-1)/sizeof(heap[0]);
	    if ( hp + 1 + wsize < sizeof(heap)/sizeof(heap[0]) ) {
		register int	hno, h;

	    /* put kanji string without EOS */
		heap[hp + wsize] = 0;
	    	hno = getKanji(p, key, (unsigned char *)&heap[hp + 1])&15;
	    /* search on the hash list */
		for ( h = uniq[hno]; h >= 0; h = heap[h&0xffff] ) 
		    if ( (h>>16) == p->nw_klen ) { /* same length */
			register int	*p1 = &heap[(h&0xffff) + 1];
			register int	*p2 = &heap[hp + 1];
			int		i;

		    /* compare by word */
			switch(wsize) {
			case 3:	if ( *p1++ != *p2++ ) goto next;
			case 2:	if ( *p1++ != *p2++ ) goto next;
			case 1:	if ( *p1++ != *p2++ ) goto next;
			case 0:	break;
			default:
			    i = wsize;
			    while ( i-- ) 
				if ( *p1++ != *p2++ ) goto next;
			};
		    /* match */
			p->nw_flags |= NW_PURGE;
			goto  done;
		next:
			continue;
		    };
	    /* enter new entry */
		heap[hp + 0] = uniq[hno];
		uniq[hno] = (p->nw_klen<<16)|hp;
		hp += 1 + wsize;
	   };
    done:
	    continue;
	};
}

/* sortWord
 *	word list wo sort suru
 */
struct compRec {
    struct nword	*word;
    int			prio;
};
static
int
compword(x, y)
register struct compRec *x, *y;
{
    register int	d = y->word->nw_prio - x->word->nw_prio;

    if ( d )
	return d;
    else
	return  -(y->prio - x->prio);
}

static
struct nword	*
sortWord(words)
struct nword	*words;
{
    unsigned 			nwords, pos, neg;
    int				i, p, n;
    register struct compRec	*wptr;
    register struct nword	*w;

/* count number of words */
    pos = neg = 0;
    for ( w = words; w; w = w->nw_next )
	if ( w->nw_prio > 0 ) 
	    pos++;
	else
	    neg++;
    nwords = pos + neg;
    if ( nwords <= 0 )
	return words;
/* allocate work space */
    wptr = (struct compRec *)malloc(sizeof(struct compRec)*nwords);
    if ( wptr ) {
	p = 0;
	n = pos;
    /* store pointers */
	for ( w = words; w; w = w->nw_next )
	    if ( w->nw_prio > 0 ) {	/* positive list */
		wptr[p].word = w;
		wptr[p].prio = p;
		p++;
	    }
	    else {			/* negative list && null word */
		wptr[n].word = w;
		n++;
	    };
    /* positive list no sakusei */
	if ( pos > 1 )
	    qsort((char *)wptr, (int)pos, sizeof(struct compRec), compword);
	for ( i = 1; i < nwords; i++ ) 
	    wptr[i - 1].word->nw_next = wptr[i].word;
	words = wptr[0].word;
	(void)free((char *)(wptr));
    };
    return words;
}

static
struct nword	*
parseBun(cx, key, ys, ye, douniq)
struct RkContext	*cx;
unsigned char		*key;
int			ys, ye;
int			douniq;
{
    struct nword	**parse = cx->parse;
    int			maxlen, i;
    struct nword	*e, *p, *head, *tail;
    
/* length table no shokika */
    e = allocWord();
    if ( !e )
	return e;
    parse[0] = e;
    for ( i = 1; i <= ye; i++ )
	parse[i] = (struct nword *)0;
    maxlen = 0;
/* bunsetu wo tukuru */
    concDicWord(cx, ND_PRE, key, ys, ye, parse, &maxlen, douniq);
    concDicWord(cx, ND_MWD, key, ys, ye, parse, &maxlen, douniq);
    concDicWord(cx, ND_SUC, key, ys, ye, parse, &maxlen, douniq);
    concDicWord(cx, ND_SWD, key, ys, ye, parse, &maxlen, douniq);
/* kouho wo list ni naosu */
    tail = (struct nword *)0;
    for ( i = 1; i <= maxlen; i++ ) 
	if ( parse[i] ) {
	    for ( p = parse[i]; p->nw_next; p = p->nw_next );
	    if ( tail )
		tail->nw_next = parse[i];
	    else
		head = parse[i];
	    tail = p;
	};
    if ( tail )
	tail->nw_next = e;
    else
	head = e;
    return head;
}
/* _RkRenbun2
 *	saichou 2 bunsetsu itti hou ni yori,
 *	current bunsetsu kara migi wo saihenkan suru 
 */
int
_RkRenbun2(cx, firstlen)
register struct RkContext	*cx;
int	firstlen;		/* sentou bunsetsu chou sitei(nakereba 0) */
{
     unsigned char	*yomi;			/* yomigana */
     int		ys, ye;			/* yomi no nagasa */
     unsigned char	*y;
     int		L1;		/* setnou bunsetsu chou */
     int		oldcurbun;
     int		maxlen;
     struct nword	*next;
     int		douniq = 1;

      yomi = cx->bunq[oldcurbun = cx->curbun].nb_key;
      y = yomi;
      ys = 0;
      ye = strlen((char *)yomi);
      next = (struct nword *)0;
      while ( ye > 0 ) {
	   register struct nbun		*bun;
	   register struct nword	*w;

      /* sedeni kaiseki zumi naraba sore wo tukasu */
	   if ( y != yomi ) {
		int	b, c;

		for ( b = cx->curbun; b < cx->maxbun; b++ )
		     if ( cx->bunq[b].nb_key == y ) {
			  for ( c = cx->curbun; c < b; c++ )
			       freeWord(cx->bunq[c].nb_cand);
			  while ( b < cx->maxbun ) 
			       cx->bunq[cx->curbun++] = cx->bunq[b++];
			  goto	exit;
		     };
	   };
      /* sentou bunsetu wo kaiseki suru */
	   if ( cx->curbun < cx->maxbun )
		freeWord(cx->bunq[cx->curbun].nb_cand);
	   bun = &(cx->bunq[cx->curbun++]);
	   bun->nb_key = y;
	   bun->nb_maxcand = bun->nb_curcand = 0;
	   if ( next ) {
	   	bun->nb_cand = sortWord(next);
		next = (struct nword *)0;
	   }
	   else
		bun->nb_cand = sortWord(parseBun(cx, y, ys, ye, 0));
       /* maxlen wo motomeru */
 	   maxlen = 0;
 	   for ( w = bun->nb_cand; w; w = w->nw_next )
		if ( maxlen < w->nw_ylen )
		    maxlen = w->nw_ylen;
	   bun->nb_maxlen = bun->nb_curlen = maxlen;
      /* bunsetu ga overflow nara ika wo kaiseki sinai */
	   if ( cx->curbun >= cx->maxbunq || !bun->nb_maxlen  ) 
		L1 = ye;
	   else if ( firstlen ) 	/* sentouchou sitei ga aru */
		L1 = firstlen;
	   else {
		register struct nword	**succ = cx->succ;
		int			i, len;

	    /* kouzoku bunsetu table wo sakusei */
		for ( i = 0; i <= maxlen; i++ )	
		    succ[i] = (struct nword *)0;
		for ( w = bun->nb_cand; w; w = w->nw_next ) 
		     if ( purgeword(w->nw_flags) )
			  continue;
		     else if ( !succ[len = w->nw_ylen] && len > 0 ) {
			 int		ys1, ye1;

			 ys1 = ys - len; if ( ys1 <= 0 ) ys1 = 0;
			 ye1 = ye - len;
			 succ[len] = parseBun(cx, y + len, ys1, ye1, douniq);
		     };

	    /* bunsetu chou wo kettei */
		L1 = detSuc(y, bun->nb_cand, succ, maxlen, 1);
	    /* fuyou na mono wo henkyaku */
		for ( i = 0; i <= maxlen; i++ )
		     if ( succ[i] && L1 != i ) {
			  freeWord(succ[i]);
			  succ[i] = 0;
		     };
	   /* next mo kaihou */
		if ( douniq ) {
		    if ( succ[L1] ) {
			freeWord(succ[L1]);
			succ[L1] = (struct nword *)0;
		    };
		    next = (struct nword *)0;
		}
		else
		    next = succ[L1];
	   };
	/* */
	   bun->nb_curlen = L1;
	   uniqWord(bun->nb_key, bun->nb_cand, bun->nb_curlen);
	   bun->nb_maxcand = 0;
	   for ( w = bun->nb_cand; w; w = w->nw_next )
		if ( !purgeword(w->nw_flags) && w->nw_ylen == L1 ) 
		     bun->nb_maxcand++;
	   firstlen = 0;

	   y += L1;
	   ys -= L1; if ( ys < 0 )  ys = 0;
	   ye -= L1;
      };
      while ( cx->maxbun > cx->curbun ) 
	   freeWord( cx->bunq[--cx->maxbun].nb_cand );
 exit:
      if ( next )
	   freeWord(next);
      cx->maxbun = cx->curbun;
      cx->curbun = oldcurbun;
      cx->bunq[cx->maxbun].nb_key = y;
      return(cx->maxbun);
}

/* _RkRenbun2
 *	saichou 2 bunsetsu itti hou ni yori,
 *	current bunsetsu kara migi wo saihenkan suru 
 */
static
void	blkcpy(d, s, e)
register unsigned char	*d;
register unsigned char	*s, *e;
{	while ( s < e )	*d++ = *s++;	}

/* _RkLearnBun
 *	bunsetu jouho wo motoni gakushuu suru 
 *	sarani, word wo kaihou suru
 */
static
void	
doLearn(cx, cw)
register struct RkContext	*cx;
register struct nword		*cw;
{
    struct nword	*lw;

    for ( ; lw = cw->nw_left; cw = lw ) {
	struct DM	*dm;
	struct ncache	*cache;
	unsigned char	*c, *t, *l;

/* cache ga nai */
	if ( !(cache = cw->nw_cache) ) {
	/* literal no gakushuu */
	    if ( cw->nw_lit && cx->litmode ) {
		unsigned long	old, new;
		int			c;

	    /* update literal table */
		new = LIT2MEM(cw->nw_lit)<<(32 - RK_XFERBITS);
		old = cx->litmode[LIT2GRP(cw->nw_lit)];
		for( c = 0; c < 32; c += RK_XFERBITS ) {
		    if ( (old&RK_XFERMASK) != LIT2MEM(cw->nw_lit) ) {
			new >>= RK_XFERBITS;
			new |= (old&RK_XFERMASK)<<(32 - RK_XFERBITS);
		    };
		    old >>= RK_XFERBITS;
		};
		cx->litmode[LIT2GRP(cw->nw_lit)] = new;
	    };
	    continue;
	};
	if ( cache->nc_flags&NC_ERROR ) 
	    continue;

/* cache ga fukusuu no word de kyouyuu sareteiru */
	if ( cache->nc_count ) {
	    RkDebug("REFCOUNT %d\n", cache->nc_count, 0, 0);
	    continue;
	};
/* write dekinai */
	dm = cache->nc_dic;
	if ( !(dm->dm_flags & DM_WRITABLE) ) 
	    continue;
/*
 *	********--------********
 *	t	l	c
 */
	for ( t = cache->nc_word; *t++; );
	t++;
	l = cw->nw_kanji;
	c = l + NW_PREFIX + candlen(*l);
    /* hinndo wo kousinn suru */
	if ( l[3] < 255 ) {
	    l[3]++;
	    cache->nc_flags |= NC_DIRTY;
	};
    /* sentou he idou suru */
	if ( t < l ) {
	    unsigned char	tmp[1024];
	    blkcpy(tmp, t, l);
	    blkcpy(t, l, c);
	    blkcpy(t + (c - l), tmp, tmp + (l - t));
	    cache->nc_flags |= NC_DIRTY;
	};
    };
}

void
_RkLearnBun(cx, bun, mode)
register struct RkContext	*cx;
struct nbun			*bun;
int				mode;
{
    register struct nword	*w, *t;
    int				count = bun->nb_curcand;

/* decrease the reference counters */
   derefWord(bun->nb_cand);
/* learn */
    if ( mode ) {
	for ( w = bun->nb_cand; w; w = w->nw_next ) {
	    if ( !purgeword(w->nw_flags) && w->nw_ylen == bun->nb_curlen ) {
		if ( count-- <= 0 ) {
		/* shuffle the order of candidates in the word record */
		    doLearn(cx, w);
		/* put the time stamp on the used word records */
		    for ( t = w; t; t = t->nw_left )
			if ( t->nw_cache ) {
			    cx->time = _RkGetTick(1);
			    t->nw_cache->nc_utime = cx->time;
			};
		    break;
		};
	    };
	};
    };
/* free words */
    killWord(bun->nb_cand);
}
