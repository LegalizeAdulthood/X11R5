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
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/cmd/crxdic/RCS/crxdic.c,v 1.8 91/05/21 17:30:05 satoko Exp $";
#endif

#include	<string.h>
#include	<stdio.h>
#include	"RK.h"
#include	"RKintern.h"

struct wordrec	{
    unsigned char	*key;
    long		offset;
};

#define	MAXKEYCHUNK	5000
#define	MAXLINE		2048

char		fileName[256];	/* source file */
int		EndOfFile, lineNum;
unsigned char	*lastKey;
long		lastOffset;

unsigned	compress;
int		verbose;

/* statistics */
#define		MAXDEPTH	256
struct trie {
    int		count;
    int		width;
};
struct trie	trie[MAXDEPTH];

extern char	*malloc(), *mktemp();

void
alert(fmt, arg)
char	*fmt;
char	*arg;
{
    char	msg[256];
    (void)sprintf(msg, fmt, arg);
    (void)fprintf(stderr, "#line %d %s: %s\n", lineNum, fileName, msg);
}
void
fatal(fmt, arg)
char	*fmt;
char	*arg;
{
    char	msg[256];
    (void)sprintf(msg, fmt, arg);
    (void)fprintf(stderr, "#line %d %s: (FATAL) %s\n", lineNum, fileName, msg);
    exit(1);
}

static
unsigned
getWREC(gram, word, tmpwrd)
struct RkKxGram	*gram;
struct wordrec	*word;
FILE		*tmpwrd;
{
    unsigned char	line[MAXLINE], wrec[MAXLINE], *w;

    if ( lastKey ) {
	word->key = lastKey;
	word->offset = lastOffset;
	lastKey = (unsigned char *)0;
	return word->key[0];
    }
    else {
	while  ( fgets((char *)line, sizeof(line), stdin) ) {
	    lineNum++;
	    if ( strlen(line) <= 0 || line[0] <= ' ' || line[0] == '#' )
		continue;
	    line[strlen(line)-1] = 0;
	    if ( w = RkParseWrec(gram, line, wrec, compress) )  {
		word->key = (unsigned char *)malloc(strlen((char *)wrec) + 1);
		if ( !word->key ) 
		    fatal("Run out of memory", 0); 
		(void)strcpy((char *)word->key, (char *)wrec);
	    /* save the word record on the temporary file */
		word->offset = ftell(tmpwrd);
		(void)fwrite((char *)wrec, w - wrec, 1, tmpwrd);
		return (word->key[0]);
	    };
	    fatal("syntax error :%s", line); 
	};
	EndOfFile = 1;
	return 0;
    };
}

static
void
ungetWREC(word)
struct wordrec	*word;
{
    lastKey = word->key;
    lastOffset = word->offset;
    EndOfFile = 0;
    lineNum--;
}

static
void
writeTRIE(dir, diroff, direct, n)
FILE		*dir;
long		diroff;
unsigned long	direct[];
int		n;
{
    int			i;
    unsigned char	l4[4];

    (void)fseek(dir, diroff, 0);
    for ( i = 0; i < n; i++ ) {
	LTOL4(direct[i], l4);
	putc((char)l4[0], dir); putc((char)l4[1], dir);
	putc((char)l4[2], dir); putc((char)l4[3], dir);
    };
}

static
long	
createTRIE(tmpdir, diroff, words, s, e, d, count)
FILE		*tmpdir;
long		diroff;
struct wordrec	words[];
int		s, e;
int		d;
int		*count;
{
    unsigned long	direct[256];
    int			index[257];
    int			n, m, ndir, i;

/* At this point, all keys in [s, e) coincides its first d bytes 
 * Create the indices to identify the d+1-th byte
 */
    for ( n = 0; s < e; s = m) {
	register unsigned d1 = words[s].key[d + 1];

	index[n++] = s;
	for ( m = s; m < e; m++ ) 
	    if ( d1 != words[m].key[d + 1] )
		break;
    };
    index[n] = e;
    if ( count )
	*count = n;

    ndir = n;
    for ( i = 0; i < n; i++ ) {
	int	i0 = index[i + 0];
	int	i1 = index[i + 1];

	direct[i] = words[i0].key[d + 1]<<24;
	if ( (i1 - i0) == 1 ) 	/* word */
	    direct[i] |= ND_WORD|words[i0].offset;
	else {			/* direct */
	    direct[i] |= diroff + ND_NODSIZ*ndir;
	    ndir += createTRIE(tmpdir, diroff + ND_NODSIZ*ndir, words,
			    i0, i1, d + 1, (int *)0);
	};
    };
    direct[n - 1] |= ND_LAST;
    writeTRIE(tmpdir, diroff, direct, n);

/* compute some statistical measures */
    d++;
    if ( d >= MAXDEPTH )
	d = MAXDEPTH - 1;
    trie[d].count++;
    trie[d].width += n;
	
    return ndir;
}

static
int	
lexico(x, y)
struct wordrec	*x, *y;
{
    register unsigned char	*s = x->key;
    register unsigned char	*t = y->key;

    while ( *s == *t )
	if ( *s )
	    s++, t++;
	else
	    return(0);
    return ((int)*s) - ((int)*t);
}

static
void
copytmp(dst, file)
int	dst;
char	*file;
{
    int		src, n;
    char   	b[1024];

    if ( (src = open(file, 0)) < 0 ) 
	fatal("Cannot open the temporary file %s", file);
    (void)unlink(file);
    while ( (n = read(src, b, sizeof(b))) > 0 ) 
	    (void)write(dst, b, n);
    (void)close(src);
}

static
void
compile(gram)
struct RkKxGram	*gram;
{
    unsigned long	dispatch[ND_DSPENT];
    static struct wordrec	words[MAXKEYCHUNK];
    struct wordrec	wd;
    int			n, nwords, dirs, i;
    long		diroff, wrdoff;
    unsigned char	l4[4];
    char		dirname[256];	/* temporary directory file */
    char		wrdname[256];	/* temporary word record file */
    FILE		*tmpdir, *tmpwrd;

/* shokika */
    (void)strcpy(wrdname, mktemp("/tmp/wrdXXXXXX"));
    (void)strcpy(dirname, mktemp("/tmp/dirXXXXXX"));
    tmpdir = fopen(dirname, "w");
    tmpwrd = fopen(wrdname, "w");

    nwords = dirs = 0;
    diroff = ND_DSPENT*ND_NODSIZ;
    for ( i = 0; i < ND_DSPENT; i++ ) 
	dispatch[i] = ND_NULLOFF;
/* sentou moji ga onaji mono wo yomu */
    do {
	unsigned char	cc, c;
	int	count, ndir;

    /* read the next word */
	if ( !(cc = getWREC(gram, &words[0], tmpwrd)) )
	    break;
	if ( dispatch[cc] != ND_NULLOFF ) 
	    fatal("input is not sorted: %02x", cc);
    /* read the following words with the same first byte */
	for ( n = 1; c = getWREC(gram, &wd, tmpwrd); n++ ) 
	    if ( cc != c ) {
		ungetWREC(&wd);
		break;
	    }
	    else 
	    if ( n < MAXKEYCHUNK )
		 words[n] = wd;
	if ( verbose )
	    (void)fprintf(stderr, "<%02x> %d items\n", cc, n);
	if ( n >= MAXKEYCHUNK ) {
	     alert("more than %d words entered", MAXKEYCHUNK);
	     n = MAXKEYCHUNK;
	};
    /* Sort the words in the lexicographic order */
	qsort((char *)words, n, sizeof(struct wordrec), lexico);
    /* directory wo sakusei suru */
	ndir = createTRIE(tmpdir, diroff, words, 0, n, 0, &count);
	dispatch[cc] = (count<<24)|diroff;

	nwords += n;
	dirs += ndir;
	diroff += ND_NODSIZ*ndir;

	for ( i = 0; i < n; i++ )
	    free((char *)words[i].key);
    } while ( EndOfFile == 0 );

    writeTRIE(tmpdir, 0L, dispatch, ND_DSPENT);
    wrdoff = ftell(tmpwrd);
    (void)fclose(tmpdir);
    (void)fclose(tmpwrd);
/* directory part */
    LTOL4(diroff, l4);
    (void)write(1, (char *)l4, 4);
    copytmp(1, dirname);
/* word record part */
    LTOL4(wrdoff, l4);
    (void)write(1, (char *)l4, 4);
    copytmp(1, wrdname);

/* show statistics */
    if ( verbose ) {
	(void)fprintf(stderr, "**** statistics ****\n");
	(void)fprintf(stderr, "%d keys %d nodes SIZE %ld = %ld + %ld\n",
			nwords, dirs, diroff + wrdoff, diroff, wrdoff);
	for ( i = 0; i < MAXDEPTH; i++ )
	    if ( trie[i].count ) 
		(void)fprintf(stderr, "depth %d\toccurrence %d\t width %f\n",
			i, trie[i].count, (double)trie[i].width/trie[i].count);
    };
}

/*ARGSUSED*/
void
main(n, args)
int	n;
char	**args;
{
    struct RkKxGram	*gram = (struct RkKxGram *)0;
    int	flag = 0;
    int constx = 0;
    int dot = 0;

    if(n < 2) {
	 printf("usage: crxdic [-Kkh] -Dbunpou <file name>\n");
	 exit(0);
    }

    compress = 0xa4;
    while ( *++args ) {
	if ( args[0][0] == '-' ) {
	    switch(args[0][1]) {
	    case 'K':
		compress = 0x00; break;
	    case 'k':
		compress = 0xa5; break;
	    case 'h':
		compress = 0xa4; break;
	    case 'D':  
		gram = RkOpenGram(*args + 2, 0);
		if ( !gram ) 
		    alert("%s:Cannot open translation", *args + 2);
		break;
	    case 'v':
		verbose++; break;
            case 'm':
		constx = 1;
		break ;
	    };
	}
	else {
	    flag = 1;
	    if ( freopen(*args, "r", stdin) ) {
		unsigned char	header[ND_HDRSIZ];
		int	i;

		for ( i = 0; i < ND_HDRSIZ; i++ )
		    header[i] = 0;
		(void)strcpy(fileName, *args);

		if(constx) {
		  for ( dot = 0; fileName[dot] != '.' && fileName[dot]; dot++);
		  strcpy(fileName + dot, ".mwd");
		}

		header[0] = 'N'; header[1] = 'D'; header[2] = compress;

		(void)strcpy((char *)&header[3], fileName);

		(void)strcat((char *)&header[3], "\n");
		(void)write(1, (char *)header, ND_HDRSIZ);
		compile(gram);
	    }
	    else
		(void)fprintf(stderr, "%s: cannot open\n", *args);
	};
    };
    if(!flag)
	 printf("usage: crxdic [-Kkh] -Dbunpou <file name>\n");

    exit(0);
}

