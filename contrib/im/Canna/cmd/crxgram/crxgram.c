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
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/cmd/crxgram/RCS/crxgram.c,v 1.1 91/05/21 17:31:30 satoko Exp $";
#endif

#include	<string.h>
#include	<stdio.h>
#include	"RKintern.h"

#define	MAXLINE		2048		/* maximum length of a line */
#define	MAXIDENT	1013		/* must be a PRIME number */
#define	MAXSTR		(MAXIDENT*10)	/* maximum string area */

extern char	*malloc(), *calloc();

struct ident {
    char		*name;
    short		rownum;
    short		colnum;
    short		hasconnect;
};
/* hash table */
static struct ident	**Row, **Column;
/* string table */
static char		Str[MAXSTR];
static char		*nextStr = Str;

static struct RkKxGram		gram;

/* error handling */
static char	fileName[256];
static int	lineNum;

/*VARARGS*/
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

unsigned char	*
readLine(s)
unsigned char	*s;
{
    while ( gets(s) ) 
	if ( s[0] == '#' ) 	/* syncronize the line information */
	    sscanf(s + 1, "%d %s", &lineNum, fileName);
	else {
	    lineNum++;
	    return s;
	};
    return (unsigned char *)0;
}

unsigned char	*
scanToken(s, token, maxtoken)
unsigned char	*s;
unsigned char	*token;
int		maxtoken;
{
    int		i;

/* skip the leading blanks */
    while ( *s && *s <= ' ' )
	s++;
    i = 0;
    while ( *s > ' ' )
	if ( i < maxtoken - 1 )
	    token[i++] = *s++;
    token[i] = 0;
    return s;
}

static int	probeHole;

struct ident	*
probeIdent(hid, name)
struct ident	**hid;
char		*name;
{
    char		*p;
    register unsigned	h, i, count;

/* compute the hash unsigned value */
    for ( h = 0, p = name; *p; )
	h = (h<<4) + *p++;
    h %= MAXIDENT;
    if ( !h ) h = *name;

    i = h;
    probeHole = -1;
    for ( count = MAXIDENT; count; count-- ) {
	struct ident	*id = hid[i];

	if ( id ) {	
	    if ( !strcmp(id->name, name) )
		return id;
	    i += h;
	    if ( i >= MAXIDENT )
		i -= MAXIDENT;
	}
	else  {
	    probeHole = i;
	    return (struct ident *)0;
	};
    };
    return (struct ident *)0;
}

struct ident	*
addIdent(hid, name, rownum, colnum)
struct ident	**hid;
char		*name;
int		rownum, colnum;
{
    struct ident	*id;

    if ( !probeIdent(hid, name) ) {
	if ( id = (struct ident *)malloc(sizeof(struct ident)) ) {
	    id->name = (char *)malloc((unsigned)strlen(name) + 1);
	    if ( !id->name )
		fatal("No more memory", 0);
	    /*NOTREACHED*/
	    (void)strcpy(id->name, name);
	    id->colnum = colnum;
	    id->rownum = rownum;
	    id->hasconnect = 0;
	    hid[probeHole] = id;
	    return id;
	};
	fatal("No more memory", 0);
	/*NOTREACHED*/
    };
    return (struct ident *)0;
}

enterIdent()
{
    unsigned char	S[MAXLINE], *s;
    FILE		*def, *swd, *mac;
    struct ident	*cc;	/* current column */

    gram.ng_row = gram.ng_col = 0;

    def = fopen("./cnj.h", "w");
    swd = fopen("./cnj.swd", "w");
    mac = fopen("./cnj.mac", "w");
    if ( !def ) fatal("Cannot create file %s", "cnj.h");
    if ( !swd ) fatal("Cannot create file %s", "cnj.swd");
    if ( !mac ) fatal("Cannot create file %s", "cnj.mac");

    cc = (struct ident *)0;
    while ( readLine(s = S) ) {
	unsigned char	tag[256], words[MAXLINE];
	unsigned char	cname[256], rname[256];
	unsigned char	*w;
	struct ident	*r;

	if ( s[0] == '%' ) 
	    break;

    /* parse line ( [cname] tag word ) */
	if ( s[0] > ' ' ) {
	    struct ident	*c;

	    s = scanToken(s, cname, sizeof(cname));
	    c = addIdent(Column, cname, gram.ng_row, gram.ng_col);
	    if ( !c ) {
		alert("column <%s> redefined", rname);
		continue;
	    };
	    cc = c;
	    gram.ng_col++;
	};

	s = scanToken(s, tag, sizeof(tag));
	if ( !tag[0]  ) 
	    continue;
        if ( !cc ) {
	    alert("no current column", 0);
	    continue;
	};

	(void)strcpy(rname, cc->name);
	if ( tag[0] != '_' ) 
	    (void)strcat(rname, tag);
	r = addIdent(Row, rname, gram.ng_row, cc->colnum);
	if ( !r ) {
	    alert("row <%s> redefined", rname);
	    continue;
	};
        gram.ng_row++;

    /* set the string table */
	if ( nextStr + 3 + strlen(rname) >= &Str[MAXSTR] ) 
	    fatal("string table overflow at <%s>", rname);
	*nextStr++ = (gram.ng_col-1)>>8;
	*nextStr++ = (gram.ng_col-1)&255;
	(void)strcpy(nextStr, rname);
	nextStr += strlen(rname) + 1;
    /* */
	(void)fprintf(mac, "#define\t%s\t%d#%d\n",
		r->name, r->rownum, r->colnum);
	(void)fprintf(def, "#define\tP_%s\tcrpair(%3d, %3d)\n",
		r->name, r->rownum, r->colnum);
	(void)fprintf(def, "#define\tR_%s\t%3d\n",
		r->name, r->rownum);

/* words wo dasu */
	s = scanToken(s, words, sizeof(words));
	if ( words[0] == '_' )
	    continue;
	w = words;
	while ( *w ) {
	    unsigned char	word[256], *p;
	    
	    p = word;
	    while ( *w ) {
		*p++ = *w++;
		if ( p[-1] == '/' ) {
		    p[-1] = 0;
		    break;
		};
	    };
	    *p = 0;
	    (void)fprintf(swd, "%s\t#%d#%d\t@\n", word, r->rownum, r->colnum);
	    (void)strcpy(word, w);
	};
    };
    (void)fclose(def);
    (void)fclose(swd);
    (void)fclose(mac);

    gram.ng_rowbyte = (gram.ng_col + 7)/8;
    gram.ng_conj = (unsigned char *)calloc(gram.ng_rowbyte, gram.ng_row);
    if ( !gram.ng_conj )
       fatal("No more memory", 0);
}

void
setVector(bits, s, op)
unsigned char	*bits;
unsigned char	*s;
int		op;
{
    for (;;) {
	unsigned char	name[256];
	int		i;
	struct ident	*r;
	struct ident	*c;

	s = scanToken(s, name, sizeof(name));
	if ( !name[0] )
	    return;
	    
	if ( name[0]=='@' ) {
	    if ( r = probeIdent(Row, name + 1) ) {
		unsigned char	*rbits = GetGramRow(&gram, r->rownum);

		if ( r->hasconnect == 0 )
		    alert("Undefined row vector is referred: %s", name);
		else {
		    switch(op) {
		    case '+':
			for ( i = 0; i < gram.ng_rowbyte; i++)
			    bits[i] |= rbits[i];
			break;
		    case '-':
			for ( i = 0; i < gram.ng_rowbyte; i++)
			    bits[i] &= ~rbits[i];
			break;
		    };
		};
	    }
	    else 
		alert("unknown row %s", name);
	}
	else {
	    if ( c = probeIdent(Column, name) ) 
		switch(op) {
		case '+':
		    bits[c->colnum/8] |= (0x80>>(c->colnum%8));
		    break;
		case '-':
		    bits[c->colnum/8] &= ~(0x80>>(c->colnum%8));
		    break;
		}
	    else 
		alert("unknown column %s", name);
	};
    };
}

void
enterMatrix()
{
    unsigned char	S[MAXLINE], *s;
    struct ident	*r;
    int			i;

/* try connection */
    while ( readLine(s = S) ) {
	char	row[256];
	int	op;

	if ( s[0] <= ' ' )
	    continue;
	s = scanToken(S, row, sizeof(row));
	op = row[strlen(row) - 1];
	row[strlen(row) - 1] = 0;
	if ( r = probeIdent(Row, row) ) {
	    switch(op) {
	    case	'+':	/* col... */
		r->hasconnect++;
	    case	'-':	/* col... */
		setVector(GetGramRow(&gram, r->rownum), s, op);
		break;
	    default:
		alert("unknown operation %c", op);
		break;
	    };
	}
	else 
	    alert("unknown row ? %s", row);
    };
/* emit the message */
    (void)fprintf(stderr, "Undefined row vectors:");
    for ( i = 0; i < MAXIDENT; i++ )
	if ( (r = Row[i]) && r->hasconnect == 0 ) 
	    (void)fprintf(stderr, " %s", r->name);
    (void)fprintf(stderr, "\n");
    (void)fprintf(stderr, "rows %d cols %d\n", gram.ng_row, gram.ng_col);
}

main()
{
    FILE		*cnj;
    unsigned long	pair;
    unsigned long	size;
    unsigned char	l4[4];

    Row    = (struct ident **)calloc(sizeof(struct ident *), MAXIDENT);
    Column = (struct ident **)calloc(sizeof(struct ident *), MAXIDENT);
    if ( !Row || !Column )
       fatal("No more memory", 0);

    enterIdent();
    enterMatrix();

/* write out the conjunction file */
    cnj = fopen("cnj.bits", "w");
    if ( !cnj )
       fatal("Cannot create file %s", "cnj.bits");
/* size */
    size = 4 + gram.ng_row*gram.ng_rowbyte + (nextStr - Str);
    LTOL4(size, l4);
    (void)fwrite((char *)l4, 1, 4, cnj);
/* row/col */
    pair = (gram.ng_row<<16)|gram.ng_col;
    LTOL4(pair, l4);
    (void)fwrite((char *)l4, 1, 4, cnj);
/* conjunction matrix */
    (void)fwrite((char *)gram.ng_conj, gram.ng_rowbyte, gram.ng_row, cnj);
/* string table */
    (void)fwrite((char *)Str, nextStr - Str, 1, cnj);
    (void)fclose(cnj);

    exit(0);
}
