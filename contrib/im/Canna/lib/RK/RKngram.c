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
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/lib/RK/RCS/RKngram.c,v 1.4 91/07/19 18:43:16 kubota Exp $";
#endif
/*LINTLIBRARY*/

#include        <stdio.h>
#include	"RK.h"
#include	"RKintern.h"

/* RkOpenGram
 *	bunpou file wo open suru
 */
struct RkKxGram	*
RkReadGram(dic)
int	dic;
{
    struct RkKxGram	*gram;
    unsigned char	l4[4];
    unsigned		sz;
    unsigned char	*conj;

    gram = (struct RkKxGram *)malloc(sizeof(struct RkKxGram));
    if ( gram ) {
    /* size wo yomu */
	(void)read(dic, (char *)l4, 4);
	sz = L4TOL(l4);
    /* row/col wo yomu */
	(void)read(dic, (char *)l4, 4);
	gram->ng_row = S2TOS(l4 + 0);
	gram->ng_col = S2TOS(l4 + 2);
	gram->ng_rowbyte = (gram->ng_col+7)/8;
	if ( conj = (unsigned char *)malloc(sz - 4) ) {
	    if ( read(dic, (char *)conj, (int)(sz - 4)) == (sz - 4) ) {
		gram->ng_conj = conj;
		gram->ng_strtab = conj + gram->ng_rowbyte*gram->ng_row;
		return gram; 
	    }
	    else {
		free((char *)conj);
	    };
	}
	free((char *)gram);
    };
    return (struct RkKxGram *)0;
}

/*ARGSUSED*/
struct RkKxGram	*
RkOpenGram(mydic, mode)
char	*mydic;		/* jisho mei */
int	mode;
{
    int	dic;
    unsigned char	header[ND_HDRSIZ];
    
    /* jisho ga aru ? */
    if ( (dic = open(mydic, 0)) == -1 ) 
	return (struct RkKxGram *)0;
    /* subete wo mount suru */
    while ( read(dic, (char *)header, ND_HDRSIZ) == ND_HDRSIZ ) {
	unsigned char		*h;
	
	if ( S2TOS(header) != ND_HDRMAG )
	    break;
	/* namae wo toru */
	h = header + 3;
	while ( *h != '\n' )	h++;
	*h++ = 0;
	if ( !strcmp((char *)(h - 4), "mwd") ||
	    !strcmp((char *)(h - 4), "swd") ||
	    !strcmp((char *)(h - 4), "pre") ||
	    !strcmp((char *)(h - 4), "suc") )  {
	    unsigned char	l4[4];
	    
	    /* directoru, word ryouiki wo yomitobasu */
	    (void)read(dic, (char *)l4, 4);
	    (void)lseek(dic, (long)(L4TOL(l4)), 1);
	    (void)read(dic, (char *)l4, 4);
	    (void)lseek(dic, (long)(L4TOL(l4)), 1);
	    /* swd ga open sarereba, fuzokugo mo yomikomu */
	    if ( !strcmp((char *)(h - 4), "swd") ) {
		struct RkKxGram	*gram;
		
		gram = RkReadGram(dic);
		(void)close(dic);
		return gram;
	    };
	}
	else
	    break;
    };
    /* nomikomenakatta */
    (void)close(dic);
    return (struct RkKxGram *)0;
}
/* RkCloseGram
 *	bunpou file wo close suru
 */
void	
RkCloseGram(gram)
struct RkKxGram	*gram;
{
    if ( gram ) {
	free((char *)gram->ng_conj);
	free((char *)gram);
    };
}

/* RkSetGramConj
 *	row to col wo setuzoku kankei wo henkou suru 
 */
int
RkSetGramConj(gram, r, c, mode)
struct RkKxGram	*gram;
int		r, c;
{
    if ( 0 <= c && c < gram->ng_col && 0 <= r && r < gram->ng_row ) {
	if ( mode )
	    gram->ng_conj[gram->ng_rowbyte*r + c/8] |= (0x80>>(c%8));
	else
	    gram->ng_conj[gram->ng_rowbyte*r + c/8] &= ~(0x80>>(c%8));
        return 0;
    }
    else
	return -1;
}

/* RkGetGramName
 *	(row, col) ni taiou suru namae wo sagasu
 */
unsigned char	*
RkGetGramName(gram, row, col)
struct RkKxGram	*gram;
int		row, col;
{
    if ( gram && gram->ng_strtab ) {
	if ( 0 <= row && row < gram->ng_row ) {
	    unsigned char	*p;

	    if ( p = gram->ng_strtab ) {
		while ( row-- > 0 )
		    p += 2 + strlen((char *)(p + 2)) + 1;
		if ( col == S2TOS(p) ) 
		    return p + 2;
	    };
	};
    };
    return (unsigned char *)0;
}
/* RkGetGramNum
 *
 */
int
RkGetGramNum(gram, name)
struct RkKxGram	*gram;
unsigned char	*name;
{
    int			row;
    unsigned char	*p;

    if ( gram && gram->ng_conj ) {
	p = gram->ng_conj + gram->ng_row*gram->ng_rowbyte; 
	for ( row = 0; row < gram->ng_row; row++ ) {
	    if ( !strcmp((char *)(p + 2), (char *)name) ) 
		return(crpair(row, ((p[0]<<8)|p[1])));
	    else
		p += 2 + strlen((char *)(p + 2)) + 1;
	};
    };
    return -1;
}
unsigned char	*
RkUparseGramNum(gram, row, col, s)
struct RkKxGram	*gram;
int		row, col;
unsigned char	*s;
{
    unsigned char	*name;

    if ( name = RkGetGramName(gram, row, col) )
	(void)sprintf((char *)s, "#%s", name);
    else
	(void)sprintf((char *)s, "#%d#%d", row, col);
    return s + strlen((char *)s);
}
unsigned char	*
RkParseGramNum(gram, s, row, col)
struct RkKxGram	*gram;
unsigned char	*s;
int		*row, *col;
{
    if ( *s++ != '#' )
	    return(0);
    *row = *col = 0;
    if ( '0' <= *s && *s <= '9'  ) {
	while ( '0' <= *s && *s <= '9'  )
	    *row = 10**row + (*s++) - '0';
	if ( *s == '#' ) {
	    s++;
	    while ( '0' <= *s && *s <= '9'  )
		*col = 10**col + (*s++) - '0';
	    return(s);
	}
    }
/* #code */
    else {
	unsigned char	code[256], *c;
	long	pair;

	c = code;
	while ( *s > ' ' && *s != '*' )
	    *c++ = *s++;
	*c = 0;
	pair = RkGetGramNum(gram, code);
	if ( pair > 0 ) {
	    *row = pair2row(pair);
	    *col = pair2col(pair);
	    return(s);
	};
    };
/* henkan shippai */
    return(0);
}
/* RkUparseWrec
 *	word record wo mojiretsu ni naosu
 */
unsigned char	*
RkUparseWrec(gram, wrec, s, pak)
struct RkKxGram	*gram;
unsigned char	*wrec;
unsigned char	*s;
unsigned	pak;
{
  unsigned char	*w, *base;
  int		nkouho;
  unsigned	oldrow, oldcol, oldfreq;

  base = s;
  w = wrec;
/* key */
  if ( pak ) {
    while ( *w )  {
      if ( *w & 0x80 )  
	*s++ = pak;
      *s++ = *w++;
    };
  }
  else
    while ( *w )  
      *s++ = *w++;

  *s = ' ';
  w++;
  /* kouho suu */
  nkouho = *w++;
  oldrow = oldcol = oldfreq = -1;
  while ( nkouho-- ) {
    unsigned	flags, row, col, freq;
    int		len;
    
    flags = *w++;	
    row   = *w++;
    col   = *w++;
    freq = *w++;
    if ( row256(flags) ) row += 256;
    if ( col256(flags) ) col += 256;
    if ( oldrow != row  || oldcol != col || oldfreq != freq ) {
      if (base > s -MAX_TEXT_LEN) {
	*s++ = ' ';
      }
      else {
	fprintf(stderr,"Length of the dic is too long!");
	return ++s;
      }
      s = RkUparseGramNum(gram, (int)row, (int)col, s);
      if ( freq ) {
	(void)sprintf((char *)s, "*%d", freq);
	s += strlen((char *)s);
      };
      oldrow = row;
      oldcol = col;
      oldfreq = freq;
    };
    if (base >  s -MAX_TEXT_LEN ) {
      *s++ = ' ';
    }
    else {
      fprintf(stderr,"Length of the dic is too long!");
      return ++s;
    }
    if ( len = candlen(flags) ) 
      while ( len-- )
	*s++ = *w++;
    else
      *s++ = '@';
  };
  if (base >  s -MAX_TEXT_LEN ) {
    *s++ = 0;
  }
  else {
    fprintf(stderr,"Length of the dic is too long!");
  }
  return(s);
}

/* RkParseWrec
 *	mojiretu wo word record ni naosu
 */
unsigned char	*
RkParseWrec(gram, s, wrec, pak)
struct RkKxGram	*gram;
unsigned char	*s;
unsigned char	*wrec;
unsigned	pak;
{
    unsigned char	*w;
    unsigned char	*nkouho;

    w = wrec;
/* key */
    while ( *s > ' ' ) 
	if ( *s & 0x80 ) {
	    if ( pak == s[0] ) 
		*w++ = s[1];
	    else {
		*w++ = s[0];
		*w++ = s[1];
	    };
	    s += 2;
	}
	else
	    *w++ = *s++;
/* key ga nai */
    if ( w == wrec )
	return((unsigned char *)0);
    *w++ = 0;
/* kouho suu */
    nkouho = w++;
    *nkouho = 0;
    while ( *s ) {
	int	flags, row, col, freq;

	while ( *s && *s != '\n' && *s != '#' )
	    s++;
	if ( *s != '#' )
		break;
    /* #row#col */
	s = RkParseGramNum(gram, s, &row, &col);
	flags = 0;
	if ( row >= 256 ) flags |= NW_R256, row -= 256;
	if ( col >= 256 ) flags |= NW_C256, col -= 256;
    /* *freq */
	if ( !s )
	    return((unsigned char *)0);
	freq = 0;
	if ( *s == '*' ) {
	    s++;
	    while ( '0' <= *s && *s <= '9'  )
		freq = 10*freq + (*s++) - '0';
	}
    /* kouho */
	while ( *s ) {
	    int	len;

	    while ( *s == ' ' || *s == '\t' ) s++;
	    if ( *s < ' ' || *s == '#' )
		    break;
	    w[1] = row; w[2] = col;	w[3] = freq;
	    for ( len = 0; *s > ' '; len++ ) 
		    w[NW_PREFIX + len] = *s++;

#define MAX_KOUHO_LEN 64
	    if (len >= MAX_KOUHO_LEN) return (unsigned char *)NULL;
	    if ( len == 1 && w[NW_PREFIX] == '@' )
		    len = 0;
	    w[0] = flags | len;
	    w += NW_PREFIX + len;
	    *nkouho += 1;
	};
    };
    return *nkouho ? w : (unsigned char *)0;
}
/* RkRemoveWrec: */
unsigned char	*
RkRemoveWrec(gram, wrec, type, kanji)
struct RkKxGram	*gram;
unsigned char	*wrec;
unsigned char	*type;
unsigned char	*kanji;
{
    int			row, col;
    unsigned char	*o, *p, *q;
    int			n, N;

    if ( !RkParseGramNum(gram, type, &row, &col) )
	return(0);
    p = wrec;
    while ( *p++ );
    o = p;
    n = *p++;
    N = 0;
    q = p;
    while ( n-- ) {
	unsigned	flags, r, c, len;
	
	flags = *p++;
	r = *p++; c = *p++;
	if ( row256(flags) ) r += 256;
	if ( col256(flags) ) c += 256;
	len = candlen(flags);
	if ( r == row && c == col &&
	    !strncmp((char *)p, (char *)kanji, (int)len))
		p += len;
	else {
	    len += NW_PREFIX; p -= NW_PREFIX;
	    while ( len-- ) *q++ = *p++;
	    N++;
	};
    };
    *o = N;
    return(q);
}


/* RkScanWcand: scans the whole wrec,
 *	        creates the index records for each candidates
 */
int
RkScanWcand(wrec, word)
unsigned char	*wrec;
struct RkWcand	*word;
{
    register unsigned char	*w;
    register int		i, nc;

/* skip the key part */
    for ( w = wrec; *w++;  );
    nc = *w++;
    for ( i = 0; i < nc; i++ ) {
	unsigned	flags;
	
	word->addr = w;
	flags = *w++;
	word->rnum = *w++; if ( row256(flags) ) word->rnum += 256;
	word->cnum = *w++; if ( col256(flags) ) word->cnum += 256;
	word->freq = *w++;
	word->klen = candlen(flags);
	w += candlen(flags);
	word++;
    };
    return nc;
}
/* RkUniqWcand: removes the duplicated entries from the RkWcand array
 *		prefers the first entry to others.
 */
int
RkUniqWcand(wc, nwc)
struct RkWcand	*wc;
int		nwc;
{
    int			i, j, nu;
    unsigned char	*a;
    unsigned 		k, r, c;

    nu = 0;
    for ( j = 0; j < nwc; j++ ) {
	k = wc[j].klen; r = wc[j].rnum; c = wc[j].cnum;
	a = wc[j].addr + NW_PREFIX;
	for ( i = 0; i < nu; i++ ) 
	    if ( wc[i].klen == k && wc[i].rnum == r && wc[i].cnum == c &&
		 !strncmp(wc[i].addr + NW_PREFIX, a, k) )
		break;
	if ( nu <= i ) 
	    wc[nu++] = wc[j];
    };
    return nu;
}

/* RkUnionWcand: creates the union of the two RkWcand arraies
 *		prefers wc1 to wc2.
 */
int
RkUnionWcand(wc1, nc1, wc2, nc2)
struct RkWcand	*wc1, *wc2;
int		nc1, nc2;
{
    int			i, j, nu;
    unsigned char	*a;
    unsigned 		k, r, c;

    nc1 = RkUniqWcand(wc1, nc1);
    nu = nc1;
    for ( j = 0; j < nc2; j++ ) {
    /* onaji kouho ga sakini aru ? */
	k = wc2[j].klen; r = wc2[j].rnum; c = wc2[j].cnum;
	a = wc2[j].addr + NW_PREFIX;
	for ( i = 0; i < nu; i++ ) 
	    if ( wc1[i].klen == k && wc1[i].rnum == r && wc1[i].cnum == c &&
		 !strncmp(wc1[i].addr + NW_PREFIX, a, k) )
		break;
	if ( nu <= i ) 
	    wc1[nu++] = wc2[j];
    };
    return nu;
}

/* RkSubtractWcand: creates the union of the two RkWcand arraies
 *		prefers wc1 to wc2.
 */
int
RkSubtractWcand(wc1, nc1, wc2, nc2)
struct RkWcand	*wc1, *wc2;
int		nc1, nc2;
{
    int			i, j, nu;
    unsigned char	*a;
    unsigned 		k, r, c;

    nc1 = RkUniqWcand(wc1, nc1);
    nu = 0;
    for ( i = 0; i < nc1; i++ ) {
    /* onaji kouho ga sakini aru ? */
	k = wc1[i].klen; r = wc1[i].rnum; c = wc1[i].cnum;
	a = wc1[i].addr + NW_PREFIX;
	for ( j = 0; j < nc2; j++ ) 
	    if ( wc2[j].klen == k && wc2[j].rnum == r && wc2[j].cnum == c &&
		 !strncmp((char *)wc2[j].addr + NW_PREFIX, (char *)a, k) )
		break;
	if ( nc2 <= j ) 
	    wc1[nu++] = wc1[i];
    };
    return nu;
}
/* RkWcand2Wrec: creates Wrec from the Wcand */
unsigned char	*
RkWcand2Wrec(key, wc, nc)
unsigned char	*key;
struct RkWcand	*wc;
int		nc;
{
    int			sz, i, j;
    unsigned char	*wrec, *d, *s, *a;
    unsigned 		k, r, c;

/* compute the size of the merged wrec */
    sz = strlen(key) + 2;
    for ( i = 0; i < nc; i++ )
	sz += wc[i].klen + NW_PREFIX;

    if ( wrec = (unsigned char *)malloc((unsigned)sz) ) {
    /* copy key part */
	d = wrec; s = key;
	while ( (*d++ = *s++) );
	*d++ = nc;
	for ( i = 0; i < nc; i++ ) {
	    k = wc[i].klen; r = wc[i].rnum; c = wc[i].cnum;
	    if ( r >= 256 ) k |= NW_R256, r -= 256;
	    if ( c >= 256 ) k |= NW_C256, c -= 256;
	    *d++ = k; *d++ = r; *d++ = c; *d++ = wc[i].freq;
	    a = wc[i].addr + NW_PREFIX;
	    for ( j = wc[i].klen; j--; )
		*d++ = *a++;
	};
    };
    return wrec;
}

/* RkGetWrecSize: computes the byte size of the wrec */
int
RkGetWrecSize(w)
register unsigned char	*w;
{
    register unsigned char	*p = w;
    register int		nc;

/* skip the key */
    while ( *p++ );
    for ( nc = *p++; nc--;  )
	p += wordlen(p[0]);
    return p - w;
}

/* RkCopyWrec: copies the contents of the wrec */
unsigned char	*
RkCopyWrec(wrec)
register unsigned char	*wrec;
{
    register unsigned char	*crec, *d, *s;
    register int		sz;

    sz = RkGetWrecSize(wrec);
    if ( crec = (unsigned char *)malloc((unsigned)sz) ) {
	for ( d = crec, s = wrec; sz--;  )
	    *d++ = *s++;
    };
    return crec;
}

/* RkUnionWrec: concatinates two word records. */
unsigned char	*
RkUnionWrec(wrec1, wrec2)
unsigned char	*wrec1, *wrec2;
{
    struct RkWcand	wc1[2*NW_MAXCAND], wc2[NW_MAXCAND];
    int			nc1, nc2, nc;

    if ( strcmp(wrec1, wrec2) )
	return (unsigned char *)0;
    nc1 = RkScanWcand(wrec1, wc1);
    nc2 = RkScanWcand(wrec2, wc2);
    nc = RkUnionWcand(wc1, nc1, wc2, nc2);
    if ( NW_MAXCAND < nc )
	return (unsigned char *)0;
    else
	return RkWcand2Wrec(wrec1, wc1, nc);
}

/* RkSubtractWrec: */
unsigned char	*
RkSubtractWrec(wrec1, wrec2)
unsigned char	*wrec1, *wrec2;
{
    struct RkWcand	wc1[NW_MAXCAND], wc2[NW_MAXCAND];
    int			nc1, nc2, nc;

    if ( strcmp(wrec1, wrec2) )
	return (unsigned char *)0;
    nc1 = RkScanWcand(wrec1, wc1);
    nc2 = RkScanWcand(wrec2, wc2);
    nc = RkSubtractWcand(wc1, nc1, wc2, nc2);
    if ( nc <= 0 )
	return (unsigned char *)0;
    else
	return RkWcand2Wrec(wrec1, wc1, nc);
}
