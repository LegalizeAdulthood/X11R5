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

#ifndef	lint
static char rcsid[] = "@(#) 102.1 $Header: /work/nk.R3.1/cmd/splitwd/RCS/splitwd.c,v 1.5 91/07/29 13:02:44 y-morika Exp $";
#endif
    
#include	<stdio.h>
#include	<signal.h>

#define		SIZE			4192
#define		ISSPACE(c)		('\n' == c || ' ' == c || '\t' == c)

typedef unsigned char	uchar;

struct head{
    uchar	yomi[SIZE];
    struct tango	*next;
}word;

struct tango{
    uchar	*tsuduri;
    uchar	*hinshi;
    struct tango	*next;
};

uchar *getword(p,Word)
uchar	*p;
uchar	*Word;
{
    while( ISSPACE(*p) )	p++;

    while( !ISSPACE(*p) && '\0' != *p )	*Word++ = *p++;
    *Word = '\0';
    
    return(p);
}

struct tango *newtango(tsuduri,hinshi)
uchar   *tsuduri;
uchar   *hinshi;
{
    struct tango   *tp;
    uchar   *p;
	
    if( !(tp = (struct tango *)malloc(sizeof(struct tango))) )
	fprintf(stderr, "Cannnot malloc %d\n", sizeof(struct tango) );

    if( !(p = (uchar *)malloc(strlen(tsuduri) + 1)) )
	fprintf(stderr, "Cannnot malloc %d\n", strlen(tsuduri)+1 );
    
    tp->tsuduri = p;
    strcpy(p,tsuduri);

    if( !(p = (uchar *)malloc(strlen(hinshi) + 1)) )
	fprintf(stderr, "Cannnot malloc %d\n", strlen(hinshi)+1 );
    tp->hinshi = p;
    strcpy(p,hinshi);
    
    tp->next = NULL;
    
    return(tp);
}	

void savetango(tsuduri, hinshi)
uchar	*tsuduri;
uchar	*hinshi;
{
    struct tango	*tp;
    
    if( !word.next ){
	word.next = newtango(tsuduri,hinshi);
	return;
    }

    tp = word.next;
    while(tp->next)
	tp = tp->next;

    tp->next = newtango(tsuduri, hinshi);
}

void save_factor(line)
uchar	*line;
{
    uchar	*lp;
    uchar	hinshi[SIZE];
    uchar	tsuduri[SIZE];
    
    lp = line;
    
    lp = getword(lp,word.yomi);			/* head に読みを入れる */
    lp = getword(lp,hinshi);			/* hinshi を取り込む */
    
    next:
    while(1){ 		       			/* 表記読み込み loop */
	lp = getword(lp,tsuduri);
	if( '\0' == tsuduri[0] ) 		/* 1 行終わり */
	    break;
	if( '#' == tsuduri[0] ){ 		/* 品詞が読み込まれた */
	    strcpy(hinshi,tsuduri);
	    goto next;
	}
	savetango(tsuduri,hinshi);
    }
}


void disp_factor()
{
    struct tango	*tp;
    
    tp = word.next;
    while( tp ){
	if(!strcmp(word.yomi, tp->tsuduri))
	    printf("%s %s @\n", word.yomi, tp->hinshi);
	else
	    printf("%s %s %s\n", word.yomi, tp->hinshi, tp->tsuduri);
	tp = tp->next;
    }
}

void free_factor(tp)
struct tango	*tp;
{
    struct tango	*ttp,*btp;
    
    if( !(btp = tp) )	return;
    if( !(ttp = tp->next) )	return;
    else{
	free_factor(ttp);
	free(ttp->tsuduri);
	free(ttp->hinshi);
	free(ttp);
	btp->next = NULL;
    }
}	

catch(sig)
int sig;
{
  fprintf(stderr, "Dictionary format error.\n");
  exit(1);
}

main( argc, argv )
int	argc;
char	*argv[];
{
    FILE	*fp;
    struct tango	*tp;
    uchar	line[SIZE];
    int		i;

    signal(SIGSEGV, catch);
    signal(SIGBUS, catch);
    
    if( argc == 1 ) {		/* コマンドだけの時 */
	fprintf(stderr, "usage: %s [filename]\n", argv[0] );
	exit(1);
    }
    
    for( i = 1; i < argc ; i++ ){
	int	nline = 0;		/* 読み込む行数を数える */
	
	if( !(fp = fopen( argv[i], "r" )) )
	    fprintf(stderr, "Can't open file %s\n", argv[i] );
	
	while( fgets(line,sizeof(line),fp) ) {
	    uchar	*l;

	    nline++;
	    if( '\n' != line[strlen(line) - 1] )
		fprintf(stderr, "%s: %d行目の一行のデータが長すぎます。\n",
			argv[0], nline );
	    else	line[strlen(line) - 1] = '\0';
	    
	    save_factor( line );
	    disp_factor();
	    free_factor( word.next );
	    word.next = NULL;
	}
	fclose( fp );
    }
    exit(0);
}
