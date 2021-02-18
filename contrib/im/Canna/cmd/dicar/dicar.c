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
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/cmd/dicar/RCS/dicar.c,v 1.5 91/07/25 10:22:58 y-morika Exp $";
#endif
/*
 *	���񥢡�������:
 *
 *	dicar -t bindic1		[�Ҽ����ɽ��]
 *	dicar -x bindic1 [bindic2 ...]	[�Ҽ���ΤȤ����]
 *	dicar -r bindic1 bindic2 ...	[�Ҽ���Υޡ�����bindic2��bindic1��]
 *	dicar -d bindic1 bindic2 ...	[�Ҽ���κ��]
 *
 */

# include	<stdio.h>
# include	"RK.h"
# include	"RKintern.h"


#ifdef DEBUG
#define DebugPRINT(n)	dbgprint(n)
dbgprint(n)
char	*n;
{
    printf("%s\n", n);
}
#else
#define DebugPRINT(n)
#endif


extern char	*malloc();
unsigned	pak;

struct wordrec {
    char	key[ND_HDRSIZ];
    long	offset;
    int		flag;
};


void umessage()
{
    fprintf( stderr,"usage: dicar -t bindic1\n       dicar -x bindic1 [bindic2 ...]\n       dicar -r bindic1 bindic2 ...\n       dicar -d bindic1 bindic2 ...\n");
}


/* ���Ͼ����Τ���Υ�å����� */
void actmessage( msg, name)
char *msg;
char *name;
{
    printf("\t%s %s.\n", msg, name);
}


/* �إå����β��� */
int ckhder( fp, key )
FILE	*fp;
char	*key;				/* �Ҽ���Υƥ����ȥե�����̾ */
{
    char	hdr[ND_HDRSIZ], *h;	/* �إå��� */
    char	*d;

    if( !fread((char *)hdr, ND_HDRSIZ, 1, fp) )    /* �إå����μ����� & ���� */
	return( -1 );

    pak = (unsigned char)hdr[2];	/* �ѥå������μ����� */
    h = hdr + 3;			/* �ե�����̾�μ����� */
    if( !strlen(h) )	return(-1);
    for ( d = key; (*d = *h++) != '\n'; d++ );
    *d = 0;
    return( 0 );
}


/* ����Ƭ���Х��Ȥ���ʬ�˥ǥ��쥯�ȥ����Υ�������ñ��쥳���ɤΥ�������
 * ��Ǽ����Ƥ���ΤǤ�����ɤ߹���
 */
long *readSize( fp )
FILE	*fp;
{
    unsigned char	l4[4];

    l4[0] = getc( fp );    l4[1] = getc( fp );
    l4[2] = getc( fp );    l4[3] = getc( fp );

    return( (long *)L4TOL( l4 ) );
}


/* �ƥ��������Ǽ�����ǥ��쥯�ȥ������ɤ����Ф� */
void movedir( fp, dirsiz, wrdsiz, wrdoff)
FILE	*fp;
long	*dirsiz, *wrdsiz, *wrdoff;
{
    *dirsiz = (long)readSize( fp );
    fseek( fp, *dirsiz, 1 );
    *wrdsiz = (long)readSize( fp );
    *wrdoff = ftell( fp );
}


/* fp1�Υǡ�����fp2�˽񤭽Ф� */
int putwriteDic(fp1, fp2, siz)
FILE	*fp1, *fp2;
long	*siz;
{
    unsigned long	*baf;

    if( !(baf = (unsigned long *)malloc( siz ))) {
        fprintf(stderr, "cannot malloc %ld siz.\n", siz);
        return( -1 );
    }
    fread((char *)baf, siz, 1, fp1 );
    fwrite((char *)baf, siz, 1, fp2 );
    free( baf );
    return( 0 );
}


void befwrite(fp1, fp2, offset)
FILE    *fp1, *fp2;
long    offset;
{
    long        dirsiz, wrdsiz, wrdoff;
    long        offset2;

    offset2 = ftell( fp2 );
    fseek( fp1, offset + ND_HDRSIZ, 0 );
    movedir( fp1, &dirsiz, &wrdsiz, &wrdoff );
    fseek( fp1, offset, 0 );
    putwriteDic(fp1, fp2, ND_HDRSIZ + dirsiz + wrdsiz + 4*2);
    fseek( fp2, offset2 + (wrdoff - offset) + wrdsiz, 0);
}


/* ��ĥ�� ".d" �Υ����å� */
char *ckname( name )
char *name;
{
    static char new[ND_HDRSIZ];
    int i;
    
    strcpy( new, name );
    for( i = 0 ; new[i] ; i++ );
    if( strcmp( &new[i-2], ".d" ) ) {
        fprintf(stderr, "Please change the filename suffix to \".d\" and then re-execute.\n");
        return( 0 );
    }
    new[i-2] = '\0';
    return( new );
}


/* -t: traceDic
 *      �Х��ʥ꼭����λҼ���̾�ȥ�������ɽ������
 */
void traceDic( fp )
FILE *fp;
{
    char        key[ND_HDRSIZ];
    long        dirsiz, wrdsiz, wrdoff;
    
    while( ckhder( fp, key ) != -1 ) {  /* �ե�����κǸ�ޤ� */
        movedir( fp, &dirsiz, &wrdsiz, &wrdoff );       /* ���������ɤ߹��� */
        fseek( fp, wrdoff + wrdsiz, 0);
        /* �ƥ����ȼ���ե�����̾ =                                     */
        /*              �ǥ��쥯�ȥꥵ���� + ��ɥ����� �ѥå�����    */
        printf("%s.d = %ld + %ld pak %2x\n", key, dirsiz, wrdsiz, pak );
    }
}


/* -x: xgetDicAll, xgetDicOnly
 *      bindic1��λҼ������Ф���
 *
 *      xgetDicAll      �Ҽ���̾�λ��꤬�ʤ����ϡ����٤ƤλҼ������Ф���
 *      xgetDicOnly     ���ꤵ�줿�Ҽ���Τ߼��Ф���
 */
int xgetDicAll( fp1 )
FILE    *fp1;
{
    FILE        *fp2;
    char        key[ND_HDRSIZ];
    long        dirsiz, wrdsiz, wrdoff;
    long        offset;
    
    offset = ftell( fp1 );
    while( ckhder( fp1, key ) != -1 ) {
        strcat( key, ".d" );
        actmessage( "extraced", key );  /* ���ϥ�å����� */
        movedir( fp1, &dirsiz, &wrdsiz, &wrdoff );/* ���������ɤ߹��� */
        if( !(fp2 = fopen( key, "w" )) )
            fprintf( stderr,"dicar: cannot open %s\n", key );
        else {
            fseek( fp1, offset, 0 );
            putwriteDic(fp1, fp2, ND_HDRSIZ + dirsiz + wrdsiz + 4*2);
            fclose( fp2 );
        }
        fseek(fp1, wrdoff + wrdsiz, 0);
        offset = ftell( fp1 );
    }
    return( 0 );
}


int xgetDicOnly( fp1, fname )
FILE    *fp1;
char    *fname;         /* �ƥ����ȼ���̾�� .d ���Ĥ������ */
{
    FILE        *fp2;
    char        key[ND_HDRSIZ];
    char        tname[ND_HDRSIZ];
    long        dirsiz, wrdsiz, wrdoff;
    long        offset;
    int         i;

    strcpy( tname, ckname( fname ) );
    if( !tname )        return( -1 );

    offset = ftell( fp1 );
    while( ckhder( fp1, key ) != -1 ) {
	movedir( fp1, &dirsiz, &wrdsiz, &wrdoff );
	if( !strcmp( tname, key ) ) {		/* �Ҽ���Ʊ���� */
	    actmessage( "extraced", fname );	/* ���ϥ�å����� */
	    if( !(fp2 = fopen( fname, "w" )) ) {
		fprintf( stderr,"dicar: cannot open %s\n", fname );
		return( -1 );
	    }
	    fseek( fp1, offset, 0 );
	    putwriteDic(fp1, fp2, ND_HDRSIZ + dirsiz + wrdsiz + 4*2);
	    fclose( fp2 );
	    return( 0 );
	}
	fseek( fp1, wrdoff + wrdsiz, 0);
	offset = ftell( fp1 );
    }
    fprintf(stderr, "dicar: %s not found %s\n", fname );
    return( -1 );			/* ���ꤵ�줿���񤬸��Ĥ���ʤ��ä� */
}


/* -r: remakedic
 *	bindic2��λҼ����bindic1���ɲä��롣
 *	�⤷��bindic1���Ʊ���Ҽ��񤬤�����ϡ���񤭤��롣
 */
/* �Ҽ���ΰ������������ */
int mkchild( fp, childdic )
FILE		*fp;
struct wordrec	*childdic;
{
    char	key[ND_HDRSIZ];
    long	dirsiz, wrdsiz, wrdoff;
    long	offset1, offset2;
    int		i;

    offset1 = ftell( fp );
    offset2 = ftell( fp );
    for( i = 0; ckhder( fp, key ) != -1 ; i++ ) {/* �ե�����κǸ�ޤ� */
	strcpy( childdic[i].key,  key);
	childdic[i].offset =  offset2;
	childdic[i].flag =  0;
	movedir( fp, &dirsiz, &wrdsiz, &wrdoff );	/* ���������ɤ߹��� */
	fseek( fp, wrdoff + wrdsiz, 0);
	offset2 = ftell( fp );
    }
    fseek( fp, offset1, 0 );		/* ��Ƭ���ᤷ�Ƥ��� */
    return( i );			/* �Ҽ���θĿ����֤� */
}


remakeDic(fp1, fp2, FP)
FILE	*fp1, *fp2, *FP;
{
    struct wordrec childdic1[256];		/* �Ҽ���ξ��� */
    struct wordrec childdic2[256];		/* �Ҽ���ξ��� */
    int		num1, num2;			/* ��¤�Το� */
    int		i, j;

    num1 = mkchild( fp1, childdic1 );    	/* �Ҽ������ι�¤�Τκ��� */
    num2 = mkchild( fp2, childdic2 );
    for( i = 0; i < num1 ; i++ ) {		/* fp1 ����� */
	for( j = 0; j < num2 ; j++ ) {
	    if( childdic2[j].flag == 0 ) {
		if( !strcmp( childdic1[i].key, childdic2[j].key ) ) {
		    childdic1[i].flag = 1;	/* fp1 �Ͻ��Ϥ��ʤ� */
		    childdic2[j].flag = 1;
		    actmessage( "replaced", childdic2[j].key );	/* ���ϥ�å����� */
		    befwrite( fp2, FP, childdic2[j].offset );
		    break;
		}
	    }
	}
	if( childdic1[i].flag == 0 ) {		/* fp1 ����Ϥ��� */
	    childdic1[i].flag = 1;
	    actmessage( "replaced", childdic1[i].key );	/* ���ϥ�å����� */
	    befwrite( fp1, FP, childdic1[i].offset );
	}
    }
    for( j = 0; j < num2 ; j++ ) {		/* �ĤäƤ��� fp2 ����� */
	if( childdic2[j].flag == 0 ) {
	    childdic2[j].flag = 1;
	    actmessage( "replaced", childdic2[j].key );	/* ���ϥ�å����� */
	    befwrite(fp2, FP, childdic2[j].offset );
	}
    }
}


/* -d: deleteDic
 *	bindic1��λҼ���bindic2�������롣
 */
int deleteDic( fp1, fp2, fname )
FILE	*fp1, *fp2;
char	*fname;
{
    char	tname[ND_HDRSIZ];
    char	key[ND_HDRSIZ];
    long	dirsiz, wrdsiz, wrdoff;
    long	offset, offset2;

    strcpy( tname, ckname( fname ) );
    if( !tname )	return( -1 );
    offset = ftell( fp1 );
    while( ckhder( fp1, key ) != -1 ) {
	movedir( fp1, &dirsiz, &wrdsiz, &wrdoff );
	if( strcmp( tname, key ) ) {		/* �Ҽ��񤬤ʤ���н��� */
	    befwrite( fp1, fp2, offset );
	}
	else	actmessage( "deleted", fname );	/* ���ϥ�å����� */
	fseek( fp1, wrdoff + wrdsiz, 0);
	offset = ftell( fp1 );
    }
}


main(argn, args)
int	argn;
char	**args;
{
    FILE	*FP1, *FP2;
    FILE	*FP;
    char	fname[ND_HDRSIZ];
    int		i;

    if( argn < 3 ) {
	umessage();
	exit(1);
    }
    
    /* -t: ɽ�� */
    if( !strcmp( args[1], "-t" ) || !strcmp( args[1], "t" ) ){
	DebugPRINT("traceDic");
	if( !(FP1 = fopen( args[2], "r" )) ) {
	    fprintf( stderr,"dicar: cannot open %s\n", args[2] );
	    exit(1);
	}
	traceDic( FP1 );
	fclose( FP1 );
    }
    /* -x: ���Ф� */
    else if( !strcmp( args[1], "-x" ) || !strcmp( args[1], "x" ) ){
	DebugPRINT("xgetDIc");
	if( !(FP1 = fopen( args[2], "r" )) ) {
	    fprintf( stderr,"dicar: cannot open %s\n", args[2] );
	    exit(1);
	}
	if( argn == 3 )
	    xgetDicAll( FP1 );
	else {
	    for( i = 3 ; i < argn ; i++ )
		xgetDicOnly( FP1, args[i] );
	}
	fclose( FP1 );
    }
    /* -r: �ɲä��� */
    else if( !strcmp( args[1], "-r" ) || !strcmp( args[1], "r" ) ){
	DebugPRINT("remakeDic");
	for( i = 3 ; i < argn ; i++ ) {
	    if( !(FP1 = fopen( args[2], "r" )) ) {
		fprintf( stderr,"dicar: cannot open %s\n", args[2] );
		exit(1);
	    }
	    if( !(FP2 = fopen( args[i], "r" )) ) {
		fprintf( stderr,"dicar: cannot open %s\n", args[i] );
		fclose( FP1 );
		exit(1);
	    }
	    strcpy( fname, "#" );
	    strcat( fname, args[2] );
	    if( !(FP = fopen( fname, "w" )) ) {
		fprintf( stderr,"dicar: cannot open %s\n", fname );
		fclose( FP1 );
		fclose( FP2 );
		exit(1);
	    }
	    remakeDic( FP1, FP2, FP );
	    rename( fname, args[2] );		/* �Хå��ե�������᤹ */
	    fclose( FP1 );
	    fclose( FP2 );
	    fclose( FP );
	}
    }
    /* -d: ������� */
    else if( (!strcmp( args[1], "-d" ) || !strcmp( args[1], "d" ) )
	    && argn > 3 ){
	DebugPRINT("deleteDic");
	for( i = 3 ; i < argn ; i++ ) {
	    if( !(FP1 = fopen( args[2], "r" )) ) {
		fprintf( stderr,"dicar: cannot open %s\n", args[2] );
		exit(1);
	    }
	    strcpy( fname, "#" );
	    strcat( fname, args[2] );
	    if( !(FP = fopen( fname, "w" )) ) {
		fprintf( stderr,"dicar: cannot open %s\n", fname );
		fclose( FP1 );
		exit(1);
	    }
	    deleteDic( FP1, FP, args[i] );
	    rename( fname, args[2] );
	    fclose( FP1 );
	    fclose( FP );
	}
    }
    else {
	umessage();
	exit(1);
    }
    exit(0);
}
