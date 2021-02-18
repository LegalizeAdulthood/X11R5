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
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/cmd/hindoup/RCS/hindoup.c,v 1.5 91/07/25 10:19:41 y-morika Exp $";
#endif
/*
 *	���ޥ�� -new [newdic] -old [olddic] -w [writedic]
 *	[0]      [1]  [2]      [3]  [4]      [5] [6]
 *	���ޥ�� -n   [newdic] -o   [olddic] -w [writedic]
 *
 */

# include	<stdio.h>
# include	"RK.h"
# include	"RKintern.h"


extern char	*malloc();


/* ���Ϥ��뤿��Υե������������� */
FILE *mkWriteF( name, fname )
char	*name;
char	*fname;
{
    FILE	*fp;
    int		i;

    for( i = strlen( fname ) ; *name ; ) {
	if( !strncmp( name, "/", 1 ) )
	    i = strlen( fname );
	else	fname[i++] = *name;
	name++;
    }
    if( !(fp = fopen( fname, "w" )) ) {
	fprintf(stderr, "Can't open %s\n", fname);
	return( 0 );
    }
    return( fp );
}


/* ñ��쥳������ʬ���ɤ߹��� */
int readWrec(dic, wrec)
FILE		*dic;
char	*wrec;
{
    char	*w;
    int			nk, flags, len;

    for ( w = wrec; *w++ = getc(dic);  );	/* �ɤ� */
    *w++ = nk = getc(dic);			/* ����� */
    while ( nk-- > 0 ) {
	flags = *w++ = getc(dic);		/* �ե饰 */
	*w++ = getc(dic);			/* ���ֹ� */
	*w++ = getc(dic);			/* ���ֹ� */
	*w++ = getc(dic);			/* ���� */
	for ( len = candlen(flags); len--; )	/* ����ʸ���� */
	    *w++ = getc(dic);
    };
    return( w - wrec );				/* �ɤߤ��Ф���ǡ�����Ĺ�� */
}


/* �ѥå������򸵤��᤹ */
void bakpac( wrec, yomi, Pak )
char	*wrec;
char	*yomi;
char		*Pak;
{
    char	*w, c;

    w = wrec;
    do{
	if ( (c = *w++) & 0x80 )
	    if ( Pak )	strcpy( yomi++, Pak );
	*yomi++ = c;
    }while ( c );
}


/* wrec�ϡ�ñ��쥳���ɤ���Ƭ�ݥ��� */
void mergWord( Nwrec, Owrec)
char	*Nwrec, *Owrec;
{
    struct RkWcand	Nword[NW_MAXCAND];
    struct RkWcand	Oword[NW_MAXCAND];	/* word��RkWcand��¤�ΤؤΥݥ��� */
    int		Nnk, Onk;		/* nk �ϡ������ */
    char	Nlen, Olen;
    char	Nkanji[256], Okanji[256];
    char	*Nk, *Ok;
    int		i, ii;
    				
    Nnk = RkScanWcand(Nwrec, Nword);
    Onk = RkScanWcand(Owrec, Oword);
    
    for( i = 0; i < Nnk; i++ ) {
	int	j;

	if( Nlen = Nword[i].klen ) {
	    for( j = 0, Nk = (char *)Nword[i].addr + 4; Nlen--; j++ )
		Nkanji[j] = *Nk++;
	    Nkanji[j+1] = 0;
	}
	else	strcpy( Nkanji, "@" );
	
	for ( ii = 0; ii < Onk; ii++ ) {
	    int	jj;
	    
	    if( !strcmp( Nword[i].rnum, Oword[ii].rnum )
	       && !strcmp( Nword[i].cnum, Oword[ii].cnum ) ) {

		char	*w;
		int	flags, len, p;

		if( Olen = Oword[ii].klen ) {
		    for ( jj = 0, Ok = (char *)Oword[ii].addr + 4; Olen--; jj++ )
			Okanji[jj] = *Ok++;
		    Okanji[jj+1] = 0;
		}
		else	strcpy( Okanji, "@" );
		
		if( !strcmp( Nkanji, Okanji ) ) {
		    Nword[i].freq += Oword[ii].freq;
		    for ( w = Nwrec; *w++ ;  );
		    w++;
		    for( p = 0 ; p < i ; p++ ) {
			flags = *w++;
			w += 3;
			for ( len = candlen(flags); len--; w++);
		    }
		    *(w+3) = Nword[i].freq;
		    w++;
		    break;
		}
	    }
	}
    }
}


/* �إå����β��� */
char *ckhder( dic, key )
FILE	*dic;
char *key;
{
    char	hdr[ND_HDRSIZ], *h;	/* �إå��� */
    char	*d;			/* �ե�����̾ */

    /* �إå����μ����� & ���� */
    if ( !fread((char *)hdr, 1, ND_HDRSIZ, dic) )
	return( 0 );
    if ( S2TOS(hdr) != ND_HDRMAG )
	return( 0 );

    h = hdr + 3;			/* �ե�����̾�μ����� */
    for ( d = key; (*d = *h++) != '\n'; d++ );
    *d = 0;
    
    return( &hdr[2] );		/* �ѥå������Υ����å� */
}


/* ��Ƭ���Х��Ȥ���ʬ�˥ǥ��쥯�ȥ����Υ�������ñ��쥳���ɤΥ�������
 * ��Ǽ����Ƥ���Τ��ɤ߹���
 */
long *readSize( dic )
FILE	*dic;
{
    unsigned char	l4[4];

    l4[0] = getc(dic);    l4[1] = getc(dic);
    l4[2] = getc(dic);    l4[3] = getc(dic);

    return( (long *)L4TOL( l4 ) );
}


/* �ǥ��쥯�ȥ������ɤ����Ф� */
void movedir( dic, dirsiz, wrdsiz, wrdoff)
FILE	*dic;
long	*dirsiz, *wrdsiz, *wrdoff;
{
    *dirsiz = (long)readSize( dic );
    fseek( dic, *dirsiz, 1 );
    *wrdsiz = (long)readSize( dic );
    *wrdoff = ftell( dic );
}


main(argn, args)
int	argn;
char	**args;
{
    FILE		*newdic, *olddic, *FP;
    char		*nd, *od;
    extern int		getopt();
    extern int		optind;
    extern char		*optarg;
    int			c;
    int			wflag = 0;	/* ����������˾�񤭤��뤫�ɤ��� */
					/* 1 -- ��񤭲� or �̥ե�����	*/
					/* 0 -- /tmp ���˽�		*/
    char		putname[256];

    if( argn < 4 ) {
	fprintf(stderr, "usage: hindoup -n newdicfile -o olddicfile [-w] [outputfile] \n");
	exit(1);
    }
    
    while( -1 != (c = getopt(argn, args, "n:o:w")) ){
	switch(c){
	    case	'n':	nd = optarg;	break;
	    case	'o':	od = optarg;	break;
	    case	'w':	wflag = 1;	break;
	}
    }
    if( !nd || !od ){	/* �ޡ������뼭��̾�����ꤵ��Ƥ��ʤ� */
	fprintf(stderr, "usage: hindoup -n newdicfile -o olddicfile [-w] [outputfile] \n");
	exit(1);
    }
    if( !(newdic = fopen(nd, "r")) ){	/* ����̵���μ��� */
	fprintf(stderr, "Can't open %s\n", nd);
	exit(1);
    }
    if( !(olddic = fopen(od, "r")) ){	/* ����ͭ��μ��� */
	fprintf(stderr, "Can't open %s\n", od);
	fclose(newdic);
	exit(1);
    }
    if( wflag ) {
	if( argn == 7 ) {
	    if( !(FP = fopen(args[6], "w")) ){
		fprintf(stderr, "Can't open %s\n", args[6]);
		fclose(newdic);
		fclose(olddic);
		exit(1);
	    }
	    wflag = 2;
	    printf("write dic %s\n", args[6]);
	}
	else {
	    strcpy( putname, "#" );
	    if( !(FP = mkWriteF( nd, putname ))) {
		fclose(newdic);
		fclose(olddic);
		exit(1);
	    }
	    printf("write dic %s\n", nd );
	}
    }
    else {
	strcpy( putname, "/tmp/" );
	if( !(FP = mkWriteF( nd, putname )) ) {
	    fclose(newdic);
	    fclose(olddic);
	    exit(1);
	}
	printf("write dic %s\n", putname);
    }

    for(;;){
	char	Nkey[256], Okey[256];	/* �Ҽ���̾ */
	long	Ndirsiz, Odirsiz;	/* �ǥ��쥯�ȥ��������� */
	long	Nwrdsiz, Owrdsiz;	/* ����������� */
	long	Nwrdoff, Owrdoff;	/* ������Υ��ե��å� */
	long	Ncount, Ocount;
	long	offset, Noffset;	/* �ե�����ݥ��󥿤ΰ��֤�Ф��Ƥ��� */
	char	Nwrec[1024], Owrec[1024];/* ??? unsigned ��ɬ�פ��� */
	char	Nyomi[1024], Oyomi[1024];
	int	flag;
	char	NPak, OPak;		/* �ѥå��������Ǽ */
	long	*Box;

	offset = ftell( newdic );
	NPak = (char)ckhder( newdic, Nkey );	/* �ѥå��Υ����å��� */
	OPak = (char)ckhder( olddic, Okey );	/* �Ҽ���̾�μ����� */
	if( !NPak || !OPak )	break;
	
	movedir( newdic, &Ndirsiz, &Nwrdsiz, &Nwrdoff );
	movedir( olddic, &Odirsiz, &Owrdsiz, &Owrdoff );

	/* ���ϥե�����˰�ö�񤭽Ф��Ƥ��� */
	fseek(newdic, offset, 0);
	if( !(Box = (long *)malloc( ND_HDRSIZ+Ndirsiz+Nwrdsiz+4*2 )) ) {
	    fprintf(stderr, "cannot malloc %ld siz.", ND_HDRSIZ+Ndirsiz+Nwrdsiz+2);
	    break;
	}
	fread((char *)Box, ND_HDRSIZ+Ndirsiz+Nwrdsiz+4*2, 1, newdic);
	fwrite((char *)Box, ND_HDRSIZ+Ndirsiz+Nwrdsiz+4*2, 1, FP);
	free( Box );
	fseek(newdic, Nwrdoff, 0);
	
	Ncount = Nwrdsiz;
	Ocount = Owrdsiz;
	Ocount -= readWrec(olddic, Owrec);
	bakpac( Owrec, Oyomi, &OPak );
	
	while( Ncount > 0 ) {
	    long	count;
	    
	    count = Ncount;
	    offset = ftell( newdic );
	    Ncount -= readWrec(newdic, Nwrec);
	    bakpac( Nwrec, Nyomi, &NPak );		/* �ɤߤ��᤹ */
	    count -=Ncount;
	    Noffset = ftell( newdic );
	    flag = 1;

	    do {
		if( strcmp( Nyomi, Oyomi ) == 0 ) {
		    mergWord(Nwrec, Owrec);
		    fseek(FP, offset, 0);
		    fwrite(Nwrec, count, 1, FP);
		    fseek(FP, Noffset, 0);
		    break;
		}
		else if( strcmp( Nyomi, Oyomi ) < 0 ) {
		    flag = 0;
		    break;
		}
		Ocount -= readWrec(olddic, Owrec);
		bakpac( Owrec, Oyomi, &OPak );
	    }while( Ocount > 0 );
	    
	    if( flag && Ocount != 0 ) {
		Ocount -= readWrec(olddic, Owrec);
		bakpac( Owrec, Oyomi, &OPak );
	    }
	}
	fseek(newdic, Nwrdoff + Nwrdsiz, 0);
	fseek(olddic, Owrdoff + Owrdsiz, 0);
	fseek(FP, Nwrdoff + Nwrdsiz, 0);
    }
    if( wflag == 1 )	rename( putname, nd );
    fclose(newdic);
    fclose(olddic);
    fclose(FP);
    exit(0);
}
