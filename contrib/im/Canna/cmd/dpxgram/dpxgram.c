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
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/cmd/dpxgram/RCS/dpxgram.c,v 1.5 91/07/26 15:36:56 y-morika Exp $";
#endif

# include	<stdio.h>
# include	"RK.h"
# include	"RKintern.h"

unsigned	Pak;
int	expand, invert;
int	used, tree;
unsigned char	Pat[256], *P;

int	matchpat(text)
unsigned char	*text;
{
    unsigned char	*pat;

    for ( pat = Pat; *pat; pat += strlen((char *)pat) + 1 ) {
	int	plen, tlen;

	plen = strlen((char *)(pat + 1));
	tlen = strlen((char *)text);
	if ( tlen < plen )
	    continue;

	switch(pat[0]) {
	case	'P':
	    if ( !strncmp((char *)text, (char *)&pat[1], plen) )
		    return(1);
	    break;
	case	'S':
	    if ( !strncmp((char *)&text[tlen-plen], (char *)&pat[1], plen) )
		    return(1);
	    break;
	case	'I': {
	    unsigned char	*t;
	    int	w;

	    for ( t = text; tlen >= plen; t += w, tlen -= w ) {
		w = (*t&0x80) ? 2 : 1;
		if ( !strncmp((char *)t, (char *)&pat[1], plen) )
		    return(1);
	    };
	    break;
	};
	};
    };
    return(0);
}

readWrec(dic, wrec)
FILE		*dic;
unsigned char	*wrec;
{
    unsigned char	*w;
    int			nk, flags, len;

    for ( w = wrec; *w++ = getc(dic);  );
    *w++ = nk = getc(dic);	
    while ( nk-- > 0 ) {
	flags = *w++ = getc(dic);
	*w++ = getc(dic);
	*w++ = getc(dic);
	*w++ = getc(dic);	
	for ( len = candlen(flags); len--; )
	    *w++ = getc(dic);
    };
    return w - wrec;
}

int	Gomi;
int	Addr;
int	Page = 1;
int	List;
int	Word;
int	Old, New;

int
dumpWord(dic, gram)
FILE	*dic;
struct RkKxGram	*gram;
{
    unsigned char	wrec[1024], *w, c;
    unsigned char	yomi[1024], *y;
    struct RkWcand	word[NW_MAXCAND];
    int		i, nk, sz;
    unsigned	oldrow, oldcol, oldlen, len;
    unsigned char	*k;
    char		pair[256];
    int			count;

    sz = readWrec(dic, wrec);
List++;
if ( Addr + sz >= 1024 ) {
     Gomi += 1024 - Addr;
     Addr = 0;
     Page++;
};
Addr += sz;

    y = yomi;
    w = wrec;
    do {
	if ( (c = *w++) & 0x80 )
	    if ( Pak )
		*y++ = Pak;
	*y++ = c;
    } while ( c );

    if ( Pat[0] && !matchpat(yomi) ) 
	return sz;

    nk = RkScanWcand(wrec, word);
Word += nk;
    oldrow = oldcol = oldlen = -1;
    for ( i = 0; i < nk; i++ ) {
	if ( oldrow!=word[i].rnum||oldcol!=word[i].cnum||oldlen!=word[i].klen) {
	    oldrow = word[i].rnum;
	    oldcol = word[i].cnum;
	    oldlen = word[i].klen;
	    Old += 4 + oldlen;
	    New += 2 + oldlen;
	    count = 0;
	}
	else {
	    Old += 4 + oldlen;
	    New += oldlen;
	    if ( !count++ )
		New += 2;
	};

    };
    oldrow = oldcol = -1;
    for ( i = 0; i < nk; i++ ) {
	if ( oldrow != word[i].rnum  || oldcol != word[i].cnum ) {
	    RkUparseGramNum(gram, word[i].rnum, word[i].cnum, pair);
	    oldrow = word[i].rnum;
	    oldcol = word[i].cnum;
	};
	if ( word[i].freq >= used ) {
	    if ( !invert ) {
		printf("%s %s", yomi, pair);
		if ( word[i].freq )
		    printf("*%d", word[i].freq);
		putchar(' ');

		if ( len = word[i].klen ) 
		    for ( k = word[i].addr + 4; len--;  )
			putchar(*k++);
		else 
		if ( expand ) 
		    printf("%s", yomi);
		else
		    putchar('@');
		putchar('\n');
	    }
	    else {
		if ( len = word[i].klen ) 
		    for ( k = word[i].addr + 4; len--;  )
			putchar(*k++);
		else
			printf("%s", yomi);
		printf(" %s", pair);
		if ( word[i].freq )
		    printf("*%d", word[i].freq);
		printf(" %s\n", yomi);
	    };
	};
    };
    return sz;
}

int	COUNT;
dumpWrec(dic, gram, off, depth)
FILE	*dic;
long	off;
{
    unsigned char	wrec[1024], *w, c;
    struct RkWcand	word[NW_MAXCAND];
    char		pair[256];
    int			i, nk, len, oldrow, oldcol;
    unsigned char	*k;
    int			sz;

    fseek(dic, off, 0);
    sz = readWrec(dic, w = wrec);
    if ( sz > 500 )
	fprintf(stderr, "more than %d\n", sz);
    COUNT += depth;
    if ( wrec[depth] == 0 )	COUNT++;
    do {
	if ( (c = *w++) & 0x80 )
	    if ( Pak ) 
		putchar(Pak);
	putchar(c);
	if ( depth-- == 0 )
	    putchar('_');
    } while ( c );
    nk = RkScanWcand(wrec, word);
    oldrow = oldcol = -1;
    for ( i = 0; i < nk; i++ ) {
	if ( oldrow != word[i].rnum || oldcol != word[i].cnum ) {
	    RkUparseGramNum(gram, word[i].rnum, word[i].cnum, pair);
	    printf(" %s", pair);
	    oldrow = word[i].rnum;
	    oldcol = word[i].cnum;
	};
	putchar(' ');
	if ( len = word[i].klen ) 
	    for ( k = word[i].addr + 4; len--;  )
		putchar(*k++);
	else 
	    putchar('@');
    };
    putchar('\n');
}

int
dumpDir(dic, gram, dir, d, wrdoff, depth)
FILE	*dic;
struct RkKxGram	*gram;
unsigned long	*dir;
unsigned long	d;
long		wrdoff;
int		depth;
{
    int			i, j;
    unsigned long	node, off;
    unsigned char	key;

    i = d/4;
    do {
	node = dir[i++];
	key = NOD2KEY(node);
	off = NOD2OFF(node);

	if ( off == ND_NULLOFF )
	    continue;
    /* key */
	for ( j = 2*depth; j; j-- )
	    putchar(' ');
	if ( ISLASTNOD(node) ) {	/* L_ */
	    putchar('|'); putchar('_');
	}
	else {			/* |- */
	    putchar('|'); putchar('-');
	};
	if ( depth == 0 ) {
	    key = i - 1;
	    if ( Pak ) {
		if ( key&0x80 ) putchar(Pak);
		putchar(key);
	    }
	    else
		printf("%02x", key);
	}
	else 
	if ( key ) {
	    if ( Pak ) {
		if ( key&0x80 ) putchar(Pak);
		putchar(key);
	    }
	    else
		printf("%02x", key);
	}
	else {
	    putchar('-');
	    putchar('-');
	    COUNT++;
	};
    /* word */
	if ( ISWORDNOD(node) ) {
	    putchar(' ');
	    putchar(' ');
	    putchar(' ');
	    putchar(' ');
	    dumpWrec(dic, gram, wrdoff + off, depth);
	}
	else {
	    putchar('\n');
	    dumpDir(dic, gram, dir, off, wrdoff, depth + 1);
	};
    } while ( !ISLASTNOD(node) );
}
/* 
 *	dumpdic	[-u] [-x]
 */
main(n, args)
int	n;
char	**args;
{
    struct RkKxGram	*gram = (struct RkKxGram *)0;
    struct RkRxDic	*roma = (struct RkRxDic *)0;
    FILE		*dic;

    if ( n < 2 ) {
	printf("usage: dpxgram <file name>\n");
	exit(1);
    }

    expand = invert = used = 0;
    P = Pat;
    used = 0;
    tree = 0;
    while ( args[1][0] == '-' ) {
	switch(args[1][1]) {
	case	'i':	invert = 1; break;
	case	'x':	expand = 1; break;
	case	'u':
	    used = atoi(args[1] + 2);
	    if ( used == 0 )
		used = 1;
	    break;
	case	't':		/* tree format */
	    tree++; break;
	case	'D':		/* dictionary shitei */
	    gram = RkOpenGram(args[1] + 2, 0);
	    break;
#ifdef ROMAJI
	case	'R':		/* romaji shitei */
	    roma = RkOpenRoma(args[1] + 2, 0);
	    break;
	case	'P':	case	'S':	case	'I':
	    *P++ = args[1][1];
	    RkCvtRoma(roma, args[1] + 2, P, 0);
	    P += strlen((char *)P) + 1;
	    break;
#endif
	};
	args++;
	n--;
    };
    *P = 0;

    if ( !(dic = fopen(args[1], "r")) )
	    exit(1);
    for (;;) {
	unsigned char	hdr[ND_HDRSIZ], *h;
	unsigned char	key[256], *d;
	unsigned char	l4[4];
	unsigned long	*dir;
	long		dirsiz, wrdsiz, wrdoff, cnjsiz, count;
	int		i;

	if ( !fread((char *)hdr, 1, ND_HDRSIZ, dic) )
	    break;
	if ( S2TOS(hdr) != ND_HDRMAG )
	    break;
	Pak = hdr[2];
	h = hdr + 3;
	for ( d = key; (*d = *h++) != '\n'; d++ );
	*d = 0;

	l4[0] = getc(dic); l4[1] = getc(dic);
	l4[2] = getc(dic); l4[3] = getc(dic);
	dirsiz = L4TOL(l4);
	if ( dir = (unsigned long *)malloc(dirsiz) ) 
	    fread((char *)dir, dirsiz, 1, dic);
	else
	    fseek(dic, dirsiz, 1);
	l4[0] = getc(dic); l4[1] = getc(dic);
	l4[2] = getc(dic); l4[3] = getc(dic);
	wrdsiz = L4TOL(l4);
	wrdoff = ftell(dic);
	for (  i = 2; args[i]; i++ )
	    if ( !strcmp((char *)key, args[i]) )  {
		if ( tree ) 
		    dumpDir(dic, gram, dir, 0, wrdoff, 0);
		else 
		    for ( count = wrdsiz; count > 0; ) 
			count -= dumpWord(dic, gram);
		cnjsiz = 0;
		goto end;
	    };
	if ( dir )
	    free((char *)dir);
	cnjsiz = 0;
	fseek(dic, wrdoff + wrdsiz, 0);
/* swd naraba, setuzoku jouhou mo yomitobasu */
	if ( !strcmp((char *)&d[-3], "swd") ) {
	    char	*gram;

	    l4[0] = getc(dic); l4[1] = getc(dic);
	    l4[2] = getc(dic); l4[3] = getc(dic);
	    cnjsiz = L4TOL(l4);
	    gram = (char *)malloc(cnjsiz);
	    if ( !gram )
		continue;
	    fread(gram, cnjsiz, 1, dic);
	    dumpGram(gram);
	    free((char *)gram);
	};
    end:
	fprintf(stderr, "%s = %ld + %ld + %ld pak %02x  SAVE %d\n",
		key, dirsiz, wrdsiz, cnjsiz, Pak, Gomi);
	fprintf(stderr, "List %d Word %d Gomi %d\n", List, Word, Gomi);
	fprintf(stderr, "Old %d New %d\n", Old, New);
    };
    fclose(dic);
    exit(0);
}


struct {
    char	*name;
    int		colnum;
};

dumpGram(gram)
unsigned char	*gram;
{
    unsigned		rowcol, row, col, rbytes;
    unsigned char	*g;
    int			r, c, b;
    int			rn, cn;
    char		*rowname[1024];
    char		*colname[1024];
    int			colnum[1024];

    rowcol = L4TOL(gram);
    row = rowcol>>16; col = rowcol&0xffff; rbytes = (col + 7)/8;

    g = gram + 4 + row*rbytes;
    for ( r = 0; r < row; r++ ) {
	colnum[r] = 256*g[0] + g[1];
	rowname[r] = (char *)(g + 2);
	g += 2 + strlen(g + 2) + 1;
    };
    colnum[r] = 0;

    rn = cn = 0;
    while ( rn < row ) {
	char	prefix[200], suffix[200];
	int		i, j;

	for ( c = rn; colnum[rn] == colnum[c]; c++ );

	for ( i = 0;; i++) {
	    for ( j = rn; j < c; j++ ) 
		if ( !rowname[j][i] || rowname[rn][i] != rowname[j][i] )
		    goto end;
	};
    end:
	colname[cn] = malloc(i + 1);
	strncpy(colname[cn], rowname[rn], i);
	colname[cn][i] = 0;
	cn++;
	for ( j = rn; j < c; j++ ) {
	    strcpy(prefix, rowname[j]);
	    prefix[i] = 0;
	    if ( rn < j )
		strcpy(prefix, " ");
	    strcpy(suffix, rowname[j] + i);
	    if ( !suffix[0] )
		strcpy(suffix, "_");
	    printf("%s\t%s\t_\t\n", prefix, suffix);
	};
	rn = c;
    };
    
    printf("\n%%\n\n");
/* setuzoku jouhou wo dasu */
    g = gram + 4;
    for ( r = 0; r < row; r++ ) {
	c = 0;
	printf("# %s\t(%d, %d)\n", rowname[r], r, colnum[r]);
	for ( b = 0; b < rbytes; b++ ) {
	    unsigned	mask = 0x80;

	    for ( mask = 0x80; mask; mask >>= 1 )
		if ( c++ < col && g[b] & mask )   
		    printf("%s+	%s\n", rowname[r], colname[c-1]);
        };
	g += rbytes;
    };
}
