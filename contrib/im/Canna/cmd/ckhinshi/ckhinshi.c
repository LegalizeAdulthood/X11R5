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
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/cmd/ckhinshi/RCS/ckhinshi.c,v 1.3 91/08/06 11:59:01 y-morika Exp $";
#endif
/* ckhinshi.c	�����ʸˡ�ˤ��������ʻ�ˤĤ�ľ��
 *	ckhinshi < [in-file-name] > [out-file-name]
 */
#include	<stdio.h>
#define		MAXKOUHO	10000
#define		MAXTANGO	256
#define		INFINITE	99999999
#define		NOLINENUM	(INFINITE-1)

typedef	unsigned char	uchar;
extern char *malloc(), *realloc(), *mktemp();
uchar *reigaifile;

struct kouho{
    int	num;
    uchar	*yomi;
    uchar	*hinshi;
    uchar	*tsuduri;
};

uchar	*salloc(s)
uchar	*s;
{
    uchar	*new;
    
    if ( new = (uchar *)malloc( (unsigned int)strlen( (char *)s ) + 1 ) )
	strcpy( (char *)new, (char *)s );
    else {
	fprintf(stderr, "No more memory\n");
	exit(1);
    }
    return( new );
}

uchar *setmoji( S, T )
uchar	*S, *T;
{
    free( (char *)S );
    return( salloc( T ) );
}

/* �ɤ߹��� */
enterkouho( block, nkouho, num, yomi, hinshi, tsuduri )
struct kouho	*block;
int	nkouho;
int	num;
uchar	*yomi, *hinshi,	*tsuduri;
{
    block[nkouho].num = num;
    block[nkouho].yomi = salloc( yomi );
    block[nkouho].hinshi = salloc( hinshi );
    block[nkouho].tsuduri = salloc( tsuduri );
}

/* �㳰�ơ��֥���ǡ��̤�Ƥ���Ĥ��ä��� */
int enterreigai( yomi, T_block, tkouho, R_block, rkouho )
uchar	*yomi;			/* ��ʸ���ơ��֥�Τ�����ɤ� */
struct kouho	*T_block;
struct kouho	*R_block;	/* �㳰�ѥơ��֥� */
int	tkouho, rkouho;
{
    int	i, maxnum;

    maxnum = -INFINITE;
    for( i = 0; i < tkouho ; i++ ) {
	if( maxnum < T_block[i].num )
	    maxnum = T_block[i].num;
    }
#ifdef	DEBUG
    fprintf(stderr, "err: rkouho, %d, maxnum = %d\n", rkouho, maxnum);
#endif
    for( i = 0; i < rkouho ; i++ ) {
	if( INFINITE == R_block[i].num ) {
	    if( !strncmp( (char *)yomi, (char *)R_block[i].yomi, 2 ) ) {
		if( strcmp((char *)R_block[i].hinshi, "#BATU") &&
		   strcmp((char *)R_block[i].hinshi, "#DEL") ) {
		    enterkouho( T_block, tkouho, maxnum, R_block[i].yomi,
			       R_block[i].hinshi, R_block[i].tsuduri );
		    tkouho++;
		}
	    }
	}
    }
    return( tkouho );
}

closekouho( block, nkouho )
struct kouho	*block;
int	nkouho;
{
    int	i;
    
    for( i = 0 ; i < nkouho ; i++ ) {
	free( (char *)block[i].yomi );
	free( (char *)block[i].hinshi );
	free( (char *)block[i].tsuduri );
    }
}

/* ���� */
putkouho( fp, block, s, e )
FILE	*fp;
struct kouho	*block;
int	s, e;
{
    while( s < e ) {
	if( strcmp( (char *)block[s].hinshi, (char *)"@" ) )
	    fprintf( fp, "%d %s %s %s\n", block[s].num, block[s].yomi, block[s].hinshi, block[s].tsuduri );
	s++;
    }
}

/* �ĤŤ�ˤĤ��ƾ���˥����Ȥ��� */
int	compare( p, q )
struct kouho	*p, *q;
{
    int	d;
    
    d = strcmp( (char *)p->tsuduri, (char *)q->tsuduri );
    if( d == 0 )
	d = strcmp( (char *)p->hinshi, (char *)q->hinshi );
    return( d );
}

int	y_compare( p, q )
struct kouho	*p, *q;
{
    int	d;
    
    d = strcmp( (char *)p->yomi, (char *)q->yomi );
    if( d == 0 ) {
	if( p->num < q->num )
	    d = -1;
	else if( p->num > q->num )
	    d = 1;
	else	d = 0;
    }
    return( d );
}

/* ��ʣ�Υ����å� */
cyoufuku( block, s, e )
struct kouho	*block;
int	s, e;
{
    while( s < e - 1) {
	if( !compare( &block[s], &block[s+1] ) )
	    block[s].hinshi = setmoji( block[s].hinshi, (uchar *)"@");
	s++;
    }
}

/* �Ѵ���§�Υ����å� */
int applyRule( block, s, e, lhs, rhs )
struct kouho	*block;
int	s, e;
uchar	*lhs, *rhs;
{
    uchar	*l, *r;
    int	i, j;
    
    for( l = lhs ; *l ; l += strlen((char *)l)+1 ) {
	for( i = s ; i < e ; i++ ) {
	    if( !strcmp( (char *)block[i].hinshi , (char *)l ) ) {
		if( block[i].hinshi[0] == '#' )
		    block[i].hinshi[0] = '!';
		break;
	    };
	};
	if ( e <= i ) {	/* �롼���Ŭ���Բ� */
	    for( i = s ; i < e ; i++ ) {	/* �ʻ���᤹ */
		if( block[i].hinshi[0] == '!' )
		    block[i].hinshi[0] = '#';
	    }
	    return ( -1 );
	}
    }
#ifdef	DEBUG
    fprintf(stderr, "MATCH: %s\t", block[s].tsuduri);
    for( l = lhs ; *l ; l += strlen((char *)l)+1 )
	fprintf(stderr, "%s", l);
    fprintf(stderr, "  :--> ");
#endif
    for( r = rhs; *r ; r += strlen((char *)r)+1 ) {
	int	min;

	j = -1;
	min = INFINITE;
	for ( i = s; i < e ; i++ ) {
	    if( block[i].hinshi[0] == '!' || block[i].hinshi[0] == '@' ) {
		if( min > block[i].num ) {
		    j = i;
		    min = block[i].num;
		};
	    };
	}
	if ( j >= 0 ) {
	    block[j].hinshi = setmoji( block[j].hinshi, r );
#ifdef	DEBUG
	    fprintf(stderr, "%s", r);
#endif
	}
    }
#ifdef	DEBUG
    fprintf(stderr, "\n");
#endif
    for( i = s ; i < e ; i++ ) {
	if( block[i].hinshi[0] == '!' )
	    block[i].hinshi = setmoji( block[i].hinshi, (uchar *)"@");
    }
    return(0);
}

/* �θ����ʻ��ư��Ϣ�ѷ��ν�ʣ�����å� */
rewritehinshi( block, s, e )
struct kouho	*block;
int	s, e;
{
    int	n;
    
    while( s < e ) {
	n = s;
	while( (n < e) && (!strcmp((char *)block[s].tsuduri, (char *)block[n].tsuduri)) )
	    n++;
	
	/* ������� */
	applyRule( block, s, n, (uchar *)"#BATU\0", (uchar *)"@\0" );
	applyRule( block, s, n, (uchar *)"#DEL\0#KJ\0", (uchar *)"@\0" );
	applyRule( block, s, n, (uchar *)"#DEL\0#T35\0", (uchar *)"@\0" );
	applyRule( block, s, n, (uchar *)"#KX\0", (uchar *)"@\0" );
	applyRule( block, s, n, (uchar *)"#DEL\0", (uchar *)"@\0" );
	
	/* �ʻ���ö�������ᤷ�Ƥ��� */
	applyRule( block, s, n, (uchar *)"#T05\0", (uchar *)"#KDna\0" );
	applyRule( block, s, n, (uchar *)"#T15\0", (uchar *)"#KDna\0" );
	applyRule( block, s, n, (uchar *)"#T16\0", (uchar *)"#KDna\0" );
	applyRule( block, s, n, (uchar *)"#T30\0", (uchar *)"#SN\0" );
	applyRule( block, s, n, (uchar *)"#T35\0", (uchar *)"#FN\0" );
	applyRule( block, s, n, (uchar *)"#F14\0", (uchar *)"#FK\0" );
	
	/* ��ͭ̾�� */
	applyRule( block, s, n, (uchar *)"#JN\0#CN\0", (uchar *)"#JN\0" );
	applyRule( block, s, n, (uchar *)"#JN\0#KK\0", (uchar *)"#JN\0" );
	applyRule( block, s, n, (uchar *)"#JN\0#KJ\0", (uchar *)"#JN\0" );
	
	applyRule( block, s, n, (uchar *)"#CN\0#KK\0", (uchar *)"#CN\0" );
	applyRule( block, s, n, (uchar *)"#CN\0#KJ\0", (uchar *)"#CN\0" );
	
	applyRule( block, s, n, (uchar *)"#KK\0#KJ\0", (uchar *)"#KK\0" );
	
	applyRule( block, s, n, (uchar *)"#FN\0#KJ\0", (uchar *)"#FN\0" );
	applyRule( block, s, n, (uchar *)"#FN\0#KK\0", (uchar *)"#FN\0" );
	
	/* ������ */
/*	  applyRule( block, s, n, (uchar *)"#KJ\0#SUC\0", (uchar *)"#SUC\0" );*/
	
	/* ����ư���� */
	applyRule( block, s, n, (uchar *)"#KDna\0#FN\0", (uchar *)"#KDna\0" );
	applyRule( block, s, n, (uchar *)"#KDna\0#FK\0", (uchar *)"#KDna\0" );
	
	applyRule( block, s, n, (uchar *)"#FN\0#FK\0", (uchar *)"#FN\0" );
	applyRule( block, s, n, (uchar *)"#FN\0#SN\0", (uchar *)"#SN\0" );
	
	applyRule( block, s, n, (uchar *)"#FK\0#SN\0", (uchar *)"#SN\0" );/* ? */
	
	/* �᤹ */
	applyRule( block, s, n, (uchar *)"#KDna\0#SN\0", (uchar *)"#T00\0" );
	applyRule( block, s, n, (uchar *)"#KDna\0", (uchar *)"#T05\0" );
	applyRule( block, s, n, (uchar *)"#SN\0", (uchar *)"#T30\0" );
	applyRule( block, s, n, (uchar *)"#FN\0", (uchar *)"#T35\0" );
	applyRule( block, s, n, (uchar *)"#FK\0", (uchar *)"#F14\0" );
	
	/* ���ƻ� */
	applyRule( block, s, n, (uchar *)"#KY\0#T35\0", (uchar *)"#KYT\0" );
	
	/* ����ư�� */
	applyRule( block, s, n, (uchar *)"#KS\0#T35\0", (uchar *)"#KSr\0" );
	
	/* ����ư�� */
	applyRule( block, s, n, (uchar *)"#SX\0#ZX\0", (uchar *)"#ZX\0" );
	
	s = n;
    }
}

/* ư���̾�����Ӥ��� */
ckdoushi( T_block, D_block, Tnum, Dnum, hinshi, tsuduri )
struct kouho	*T_block, *D_block;
int	Tnum, Dnum;
uchar	*hinshi, *tsuduri;
{
    int	i;
    
    for(i = 0; i < Dnum ; i++ ){
	if( !strcmp( (char *)D_block[i].tsuduri, (char *)tsuduri )) {
	    if( !strcmp( (char *)D_block[i].hinshi, (char *)hinshi ))
		ckrenyou( T_block, Tnum, hinshi, tsuduri );
	}
    }
}

ckrenyou( T_block, n, hinshi, tsuduri )
struct kouho	*T_block;
int	n;
uchar	*hinshi, *tsuduri;
{
    int	i;
    
    T_block[n].hinshi = setmoji( T_block[n].hinshi, (uchar *)"@" );
    /* �ʻ��Ϣ�ѷ�̾���ư��ˤĤ��Ѥ��� */
    for( i = 0 ; i < n ; i++ ) {
	if( !strcmp((char *)T_block[i].tsuduri, (char *)tsuduri) ) {
	    if( !strcmp((char *)T_block[i].hinshi, (char *)hinshi) ) {
		T_block[i].hinshi = (uchar *)realloc((char *)T_block[i].hinshi, (unsigned int)strlen( (char *)T_block[i].hinshi)+2 );
		strcat( (char *)T_block[i].hinshi, "r" );
#ifdef	DEBUG
		fprintf(stderr, "ckdoushi: [%s+T35 -> %s]\t%s\n", hinshi, T_block[i].hinshi, T_block[i].tsuduri);
#endif
	    }
	}
    }
}

uchar	*is5dan( gobi )
uchar	*gobi;
{
    while( (0x80 & *gobi) && strlen((char *)gobi) > 2 ){
	*gobi++;
	*gobi++;
    }
    if( !strcmp( (char *)gobi, "��" ) )
	return((uchar *)"#K5");
    else if( !strcmp( (char *)gobi, "��" ) )
	return((uchar *)"#G5");
    else if( !strcmp( (char *)gobi, "��" ) )
	return((uchar *)"#S5");
    else if( !strcmp( (char *)gobi, "��" ) )
	return((uchar *)"#T5");
    else if( !strcmp( (char *)gobi, "��" ) )
	return((uchar *)"#B5");
    else if( !strcmp( (char *)gobi, "��" ) )
	return((uchar *)"#M5");
    else if( !strcmp( (char *)gobi, "��" ) )
	return((uchar *)"#R5");
    else if( !strcmp( (char *)gobi, "��" ) )
	return((uchar *)"#W5");
    else	return((uchar *)"");
}

dopath1( fp, T_block, kazu, R_block, rkouho, num )
FILE	*fp;
struct kouho	*T_block, *R_block;
int	kazu, rkouho;
int	num;					/* Ʊ�֥�å���κǸ���ֹ� */
{
    if( kazu ) {
	int	i;
	
	for( i = 0 ; i < rkouho ; i++ ) {	/* Ʊ���ɤߤ�ȤäƤ��� */
	    if( !strcmp( R_block[i].yomi, T_block[0].yomi ) ) {
		R_block[i].num = num;
		enterkouho( T_block, kazu, R_block[i].num, R_block[i].yomi,
			   R_block[i].hinshi, R_block[i].tsuduri );
		kazu++;
	    }
	}
	
	qsort( T_block, (unsigned int)kazu, sizeof(struct kouho), compare );	/* �ĤŤ�Ǿ��祽���� */
	cyoufuku( T_block, 0, kazu );			/* ��ʣ����κ�� */
	rewritehinshi( T_block, 0, kazu );		/* �ĤŤ�����, �ʻ�θ�Ƥ */
	putkouho( fp, T_block, 0, kazu );		/* ���� */
	closekouho( T_block, kazu );
    }
}

dopath2( block, s, e )
struct kouho	*block;
int	s, e;
{
    qsort( block, (unsigned int)e, sizeof(struct kouho), y_compare );	/* �ɤߡ��ֹ�Ǿ��祽���� */
    putkouho( stdout, block, s, e );
    closekouho( block, e );
}

main( argv, args )
int	argv;
char	**args;
{
    struct kouho	T_block[MAXKOUHO], D_block[MAXKOUHO];
    struct kouho	R_block[MAXKOUHO];		/* �㳰�ե����� */
    int	num;
    uchar	*yomi;
    uchar	S[MAXTANGO], y[MAXTANGO], h[MAXTANGO], k[MAXTANGO], hin[MAXTANGO];
    int	nkouho, dkouho, rkouho;
    FILE	*fp, *Rfp;
    uchar	*filename;
    
    /* �㳰�ե�������ɤ߹��� */
    rkouho = 0;
    if( argv > 1 ) {
	int	i;
	
	for( i = 1; i < argv ; i++ ) {
	    if( Rfp =fopen( args[i], "r" ) ) {
		while( fgets( (char *)S, sizeof(S), Rfp ) ) {
		    if( 4 != sscanf( (char *)S, "%s %s %s %s", hin, y, h, k ) )
			continue;
		    else if( !strcmp( hin, "#" ) )	/* �оݳ��θ�λ� */
			continue;
		    else if( rkouho < MAXKOUHO ) {
			if( !strcmp( hin, "#GREEK" ) )
			    strcpy( hin, "#T35" );
			enterkouho( R_block, rkouho, INFINITE, y, hin, k );
			rkouho++;
		    }
		}
		fclose( Rfp );
	    }
	}
    }
    
    filename = salloc( (uchar *)mktemp("/tmp/nameXXXXXX") );
    fp = fopen( (char *)filename, "w+" );
    unlink( (char *)filename );
    
    /* ̾�졢����ư�졢����ư�� */
    nkouho = 0;
    yomi = salloc((uchar *)"");
    while( gets((char *)S) ) {
	if( 4 != sscanf( (char *)S, "%d %s %s %s", &num, y, h, k ) )
	    continue;
	else {
	    if( strcmp( (char *)yomi, (char *)y ) ) {
		dopath1( fp, T_block, nkouho, R_block, rkouho, T_block[nkouho-1].num );
		nkouho = 0;
		yomi = setmoji( yomi, y );
	    }
	    if( nkouho < MAXKOUHO ) {
		enterkouho( T_block, nkouho, num, y, h, k );
		nkouho++;
	    }
	}
    }
    /* �Ǹ�ι� */
    dopath1( fp, T_block, nkouho, R_block, rkouho, T_block[nkouho-1].num );
    
    /* ư��Ϣ�ѷ���̾������ */
    rewind(fp);
    yomi = setmoji( yomi, y );
    nkouho = dkouho = 0;
    
    while( fgets( (char *)S, sizeof(S), fp ) ) {
	if( 4 != sscanf( (char *)S, "%d %s %s %s", &num, y, h, k ) )
	    continue;
	else {
	    if( !strcmp( (char *)h, "#K5") ) {	/* �Ԥ� */
		if( !strcmp( (char *)y, "��" ) )
		    strcpy( (char *)h, "#C5r" );
	    }
	    
	    /* ��ʸ���ܤ��ɤߤ�Ʊ���֤��ʻ����Ӥ��� */
	    if( strncmp( (char *)yomi, (char *)y, 2 ) ) {	/* ��ʸ���ܤ��ɤߤ��㤦�� */
		/* �㳰�ơ��֥���ǡ��̤�Ƥ���Ĥ��ä��� */
		nkouho = enterreigai( yomi, T_block, nkouho, R_block, rkouho );
		dopath2( T_block, 0, nkouho );
		closekouho( D_block, dkouho );
		nkouho = dkouho = 0;
		yomi = setmoji( yomi, y );
	    }
	    if( nkouho < MAXKOUHO ) {
		/* ��ʸ��Ʊ���ơ��֥�κ��� */
		enterkouho( T_block, nkouho, num, y, h, k );
		/* ư��Υơ��֥�κ��� */
		if( (!strcmp((char *)h, (char *)"#K5"))
		   || (!strcmp((char *)h, (char *)"#C5"))
		   || (!strcmp((char *)h, (char *)"#G5"))
		   || (!strcmp((char *)h, (char *)"#S5"))
		   || (!strcmp((char *)h, (char *)"#T5"))
		   || (!strcmp((char *)h, (char *)"#B5"))
		   || (!strcmp((char *)h, (char *)"#M5"))
		   || (!strcmp((char *)h, (char *)"#R5"))
		   || (!strcmp((char *)h, (char *)"#W5"))) {
		    enterkouho( D_block, dkouho, num, y, h, k );
		    dkouho++;
		}
		else if( (!strcmp((char *)h, "#T35")) && (strlen((char *)k) >= 2) ) {
		    if( 0xa4 == k[strlen((char *)k) - 2] ) {
			uchar	*hinshi, *tsuduri;
			
			hinshi = salloc( is5dan(k) );
			tsuduri = salloc( k );
			tsuduri[strlen((char *)k)-2] = '\0';
			ckdoushi( T_block, D_block, nkouho, dkouho, hinshi, tsuduri );
			free( (char *)hinshi );
			free( (char *)tsuduri );
		    }
		}
		nkouho++;
	    }
	}
    }
    nkouho = enterreigai( yomi, T_block, nkouho, R_block, rkouho );
    dopath2( T_block, 0, nkouho );	/* �Ǹ�ιԤ���� */
    closekouho( D_block, dkouho );
    closekouho( R_block, rkouho );
    
    free( (char *)filename );
    fclose( fp );
    exit( 0 );
}
