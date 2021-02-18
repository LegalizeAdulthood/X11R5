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
 *	辞書アーカイブ:
 *
 *	dicar -t bindic1		[子辞書の表示]
 *	dicar -x bindic1 [bindic2 ...]	[子辞書のとりだし]
 *	dicar -r bindic1 bindic2 ...	[子辞書のマージ、bindic2をbindic1に]
 *	dicar -d bindic1 bindic2 ...	[子辞書の削除]
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


/* 出力状況のためのメッセージ */
void actmessage( msg, name)
char *msg;
char *name;
{
    printf("\t%s %s.\n", msg, name);
}


/* ヘッダ部の解析 */
int ckhder( fp, key )
FILE	*fp;
char	*key;				/* 子辞書のテキストファイル名 */
{
    char	hdr[ND_HDRSIZ], *h;	/* ヘッダ部 */
    char	*d;

    if( !fread((char *)hdr, ND_HDRSIZ, 1, fp) )    /* ヘッダ部の取り込み & 参照 */
	return( -1 );

    pak = (unsigned char)hdr[2];	/* パック形式の取り込み */
    h = hdr + 3;			/* ファイル名の取り込み */
    if( !strlen(h) )	return(-1);
    for ( d = key; (*d = *h++) != '\n'; d++ );
    *d = 0;
    return( 0 );
}


/* 各先頭４バイトの部分にディレクトリ部のサイズ、単語レコードのサイズが
 * 格納されているのでそれを読み込む
 */
long *readSize( fp )
FILE	*fp;
{
    unsigned char	l4[4];

    l4[0] = getc( fp );    l4[1] = getc( fp );
    l4[2] = getc( fp );    l4[3] = getc( fp );

    return( (long *)L4TOL( l4 ) );
}


/* 各サイズを格納し、ディレクトリ部を読み飛ばす */
void movedir( fp, dirsiz, wrdsiz, wrdoff)
FILE	*fp;
long	*dirsiz, *wrdsiz, *wrdoff;
{
    *dirsiz = (long)readSize( fp );
    fseek( fp, *dirsiz, 1 );
    *wrdsiz = (long)readSize( fp );
    *wrdoff = ftell( fp );
}


/* fp1のデータをfp2に書き出す */
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


/* 拡張子 ".d" のチェック */
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
 *      バイナリ辞書内の子辞書名とサイズを表示する
 */
void traceDic( fp )
FILE *fp;
{
    char        key[ND_HDRSIZ];
    long        dirsiz, wrdsiz, wrdoff;
    
    while( ckhder( fp, key ) != -1 ) {  /* ファイルの最後まで */
        movedir( fp, &dirsiz, &wrdsiz, &wrdoff );       /* サイズの読み込み */
        fseek( fp, wrdoff + wrdsiz, 0);
        /* テキスト辞書ファイル名 =                                     */
        /*              ディレクトリサイズ + ワードサイズ パック形式    */
        printf("%s.d = %ld + %ld pak %2x\n", key, dirsiz, wrdsiz, pak );
    }
}


/* -x: xgetDicAll, xgetDicOnly
 *      bindic1内の子辞書を取り出す。
 *
 *      xgetDicAll      子辞書名の指定がない時は、すべての子辞書を取り出す。
 *      xgetDicOnly     指定された子辞書のみ取り出す。
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
        actmessage( "extraced", key );  /* 出力メッセージ */
        movedir( fp1, &dirsiz, &wrdsiz, &wrdoff );/* サイズの読み込み */
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
char    *fname;         /* テキスト辞書名に .d がついたもの */
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
	if( !strcmp( tname, key ) ) {		/* 子辞書が同じ時 */
	    actmessage( "extraced", fname );	/* 出力メッセージ */
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
    return( -1 );			/* 指定された辞書が見つからなかった */
}


/* -r: remakedic
 *	bindic2中の子辞書をbindic1に追加する。
 *	もし、bindic1中に同じ子辞書がある場合は、上書きする。
 */
/* 子辞書の一覧を作成する */
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
    for( i = 0; ckhder( fp, key ) != -1 ; i++ ) {/* ファイルの最後まで */
	strcpy( childdic[i].key,  key);
	childdic[i].offset =  offset2;
	childdic[i].flag =  0;
	movedir( fp, &dirsiz, &wrdsiz, &wrdoff );	/* サイズの読み込み */
	fseek( fp, wrdoff + wrdsiz, 0);
	offset2 = ftell( fp );
    }
    fseek( fp, offset1, 0 );		/* 先頭に戻しておく */
    return( i );			/* 子辞書の個数を返す */
}


remakeDic(fp1, fp2, FP)
FILE	*fp1, *fp2, *FP;
{
    struct wordrec childdic1[256];		/* 子辞書の情報 */
    struct wordrec childdic2[256];		/* 子辞書の情報 */
    int		num1, num2;			/* 構造体の数 */
    int		i, j;

    num1 = mkchild( fp1, childdic1 );    	/* 子辞書情報の構造体の作成 */
    num2 = mkchild( fp2, childdic2 );
    for( i = 0; i < num1 ; i++ ) {		/* fp1 を基準に */
	for( j = 0; j < num2 ; j++ ) {
	    if( childdic2[j].flag == 0 ) {
		if( !strcmp( childdic1[i].key, childdic2[j].key ) ) {
		    childdic1[i].flag = 1;	/* fp1 は出力しない */
		    childdic2[j].flag = 1;
		    actmessage( "replaced", childdic2[j].key );	/* 出力メッセージ */
		    befwrite( fp2, FP, childdic2[j].offset );
		    break;
		}
	    }
	}
	if( childdic1[i].flag == 0 ) {		/* fp1 を出力する */
	    childdic1[i].flag = 1;
	    actmessage( "replaced", childdic1[i].key );	/* 出力メッセージ */
	    befwrite( fp1, FP, childdic1[i].offset );
	}
    }
    for( j = 0; j < num2 ; j++ ) {		/* 残っている fp2 を出力 */
	if( childdic2[j].flag == 0 ) {
	    childdic2[j].flag = 1;
	    actmessage( "replaced", childdic2[j].key );	/* 出力メッセージ */
	    befwrite(fp2, FP, childdic2[j].offset );
	}
    }
}


/* -d: deleteDic
 *	bindic1中の子辞書bindic2を削除する。
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
	if( strcmp( tname, key ) ) {		/* 子辞書がなければ出力 */
	    befwrite( fp1, fp2, offset );
	}
	else	actmessage( "deleted", fname );	/* 出力メッセージ */
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
    
    /* -t: 表示 */
    if( !strcmp( args[1], "-t" ) || !strcmp( args[1], "t" ) ){
	DebugPRINT("traceDic");
	if( !(FP1 = fopen( args[2], "r" )) ) {
	    fprintf( stderr,"dicar: cannot open %s\n", args[2] );
	    exit(1);
	}
	traceDic( FP1 );
	fclose( FP1 );
    }
    /* -x: 取り出す */
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
    /* -r: 追加する */
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
	    rename( fname, args[2] );		/* バックファイルを戻す */
	    fclose( FP1 );
	    fclose( FP2 );
	    fclose( FP );
	}
    }
    /* -d: 削除する */
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
