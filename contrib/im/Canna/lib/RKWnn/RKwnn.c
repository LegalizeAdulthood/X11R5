/*
 * COPYRIGHT (C) NEC Corporation
 *           and NEC Microcomputer Technology, Ltd 1991
 * All Rights Reserved
 */
/*
 * RkWnn.c
 *	RK ��Ϣʸ���Ѵ��饤�֥����б�������
 *
 *	1991.9: y-morika
 */
#ifndef lint
static char rcsid[]="$Header: /work/nk.R3.1/lib/RKWnn/RCS/RKwnn.c,v 1.7 91/09/18 10:45:22 kon Exp $";
#endif

#include	<iroha/RK.h>
#include	<stdio.h>
#include	<signal.h>
#include	"jclib.h"
#include	"jilib.h"
#define	uchar	unsigned char

#ifndef RK_DEFAULT_JSERVER
#define RK_DEFAULT_JSERVER	"quincy"
#endif

#define ISSK1(c) \
    (((c) >= 0x81 && (c) <= 0x9f) || ((c) >= 0xe0 && (c) <= 0xfc))

#define CKCONV(cn)  if (!ccx[(cn)].converting) return -1
#define CKCN(cn) if (cn < 0 || MAXCONTEXT <= cn || !ccx[(cn)].occupied) \
  return -1

WNN_JSERVER_ID	*server;
WNN_ENV		*wnn_env;

#define MAXCONTEXT 512

typedef struct _CannaContext {
  int           occupied;	/* ���Υ���ƥ����Ȥ��Ȥ��Ƥ��� */
  jcConvBuf	*buf;
  int		converting;	/* �Ѵ��� */
  int		tan;		/* Ϣʸ���Ѵ����� */
  int		small;		/* small 0 ��ʸ��ñ�̤ǰ�ư */
} CannaContext;			/* ����ʳ� ��ʸ��ñ�̤ǰ�ư */

CannaContext ccx[MAXCONTEXT];

static wchar_t *
euc2wchar(echar, wc, n)			/* EUC�����ɤˤʤ��� */
uchar 	*echar;
wchar_t	*wc;
int n;
{
   wchar_t    *w = wc;
   int nn = n;

   while ( *echar ) {
       if ( *echar & 0x80 ) {
	   *w++ = (echar[0]<<8)|echar[1];
	   echar += 2;
	   nn -= 2;
       }
       else {
	   *w++ = *echar++;
	   nn --;
       }
       if (n && nn <= 0)
	   break;
   }
   *w = 0;
   return( wc );
}

static void
w2c(wp, cp, len)
wchar_t	*wp;
char	*cp;
int	len;	/* length of go */
{
    wchar_t	wc;

    while(wc = *wp++) {
	if(wc & 0x8000)
	    *cp++ = (wc & 0xff00) >> 8;
	*cp++ = wc & 0x00ff;
	if(len && --len == 0)
	    break;
    }
    *cp = 0;
}

static
putwchar(s, n)				/* ʸ����ν��� */
wchar_t	*s;
int	n;
{
    int	kanji = 0;

    while ( n-- ) {
	/* 2�Х��ȥ����� */
	if ( *s & 0xff00 ) {
	    /* "\033$@"����Ϥ��Ƥ��ʤ���н��� */
	    if( !kanji ){
		kanji = 1;
		putchar('\033');
		putchar('$');
		putchar('@');
	    }
	    putchar((char)(*s>>8));
	}
	/* 1�Х��ȥ����ɤ�"\033$@"�����Ϥ��줿���֤ˤ��� */
	else if( kanji ){
	    putchar('\033');
	    putchar('(');
	    putchar('J');
	}
	putchar((char )(*s&255));
	s++;
    };
    /* ����� */
    if( kanji ){
	putchar('\033');
	putchar('(');
	putchar('J');
    }
}


/* ���ʥХåե��ȥǥ����ץ쥤�Хåե����ʸ�᤺�Ľ��Ϥ��� */
static
putKana( cn, bnum, n )
int     cn;			/* ����ƥ������ֹ� */
int	bnum;			/* ���Ϥ���ʸ���ֹ�*/
int	n;			/* 1 �λ������ʥХåե����� */
{				/* 0 �λ����ǥ����ץ쥤�Хåե����� */
    wchar_t	*s, *e;
    int	kanji = 0;
    struct jcClause	*Info;
    jcConvBuf *buf;

    CKCN(cn);
    buf = ccx[cn].buf;
    
    if( n ) {
	s = buf->clauseInfo[bnum].kanap;
	e = buf->clauseInfo[bnum].kanap+1;
    }
    else {
	s = buf->clauseInfo[bnum].dispp;
	e = buf->clauseInfo[bnum+1].dispp+1;
    }

    while( s < e ) {
	/* 2�Х��ȥ����� */
	if ( *s & 0xff00 ) {
	    /* "\033$@"����Ϥ��Ƥ��ʤ���н��� */
	    if( ! kanji ){
		kanji = 1;
		putchar('\033');
		putchar('$');
		putchar('@');
	    }
	    putchar((char)(*s>>8));
	}
	/* 1�Х��ȥ����ɤ�"\033$@"�����Ϥ��줿���֤ˤ��� */
	else if( kanji ){
	    putchar('\033');
	    putchar('(');
	    putchar('J');
	}
	putchar((char)(*s&255));
	s++;
    }
    /* ����� */
    if( kanji ){
	putchar('\033');
	putchar('(');
	putchar('J');
    }
}


/* ���ʥХåե����ǥ����ץ쥤�Хåե��ν��Ϥ򤹤� */
static
hyouji(cn)
int cn; /* ����ƥ������ֹ� */
{
    int	bnum;
    int ncand, curcand;
    jcConvBuf *buf;

    CKCN(cn);
    buf = ccx[cn].buf;
    
    for( bnum = 0 ; bnum < buf->nClause ; bnum++ ) {
	if( bnum == buf->curClause )
	    putchar('_');
	putKana( cn, bnum, 0 );	/* ���� 0 �λ��ǥ����ץ쥤�Хåե�ɽ�� */
	if( bnum == buf->curClause )
	    putchar('_');
    }
    jcCandidateInfo(buf, ccx[cn].small, &ncand, &curcand);
    putchar('\t');
    putchar('#');
    printf("%d", curcand );
    putchar(':');
    putKana( cn, buf->curClause, 1 );	/* ���� 1 �λ����ʥХåե�ɽ�� */
    putchar('\n');
    putchar('\r');
}

static void
initCannaContexts()
{
  int i;

  for (i = 0 ; i < MAXCONTEXT ; i++) {
    ccx[i].occupied = 0;
    ccx[i].buf = (jcConvBuf *)0;
  }
}


/* ����� */
int	RkInitialize(home)
char	*home;				/* ����ǥ��쥯�ȥ�(����Ϥλ�ͭ��) */
{
    char *jserver_name;
    extern char *getenv();

    initCannaContexts();
    if ((jserver_name = getenv("JSERVER")) == NULL)
	jserver_name = RK_DEFAULT_JSERVER;	/* very site dependent !!!! */
    server = jiOpenServer(jserver_name, 30);
    
    if( !server ) {
	fprintf(stderr, "RkInitialize: cannot connect to jserver(%s).", jserver_name);
	return ( -1 );
    }
    wnn_env = jiCreateEnv(server,
			  ""/* envname */, 0/* override */,
			  ""/* wnnrcname */,
			  NULL/* callback */, NULL/* callback */,
			  0/* client data */);

    return RkCreateContext();
}


/* ��λ���� */
void	RkFinalize()
{
    int i;

    for (i = 0 ; i < MAXCONTEXT ; i++) {
      RkCloseContext(i);
    }
    jiCloseServer( server );
}


/* ����ե�����Υ����ץ� */
/*ARGSUSED*/
int	RkMountDic(ctx, filename, mode)
int	ctx;
char	*filename;
int	mode;
{
    return( 1 );
}


/* �Ѵ��γ��� */
int RkBgnBun( ctx, yomi, n, mode )
int	ctx;
uchar	*yomi;
int	n;
int	mode;
{
    jcConvBuf *buf;
    wchar_t	wChar[1024];

    CKCN(ctx);
    buf = ccx[ctx].buf;
    jcClear(buf);
    
    /* �Хåե���ʸ������������� */
    if( jcChangeClause(buf, euc2wchar(yomi, wChar, n)) == -1 )
	return( -1 );
    
    /* ���ʴ����Ѵ���Ԥʤ� */
    if( jcConvert(buf, 0, ccx[ctx].tan, 0) == -1 )
	return( -1 );

    ccx[ctx].converting = 1;
    return( buf->nClause );
}


/* �Ѵ��ν�λ */
/*ARGSUSED*/  
int RkEndBun(ctx,  mode)
int	ctx;
int	mode;			/* �ؽ�����Υե饰 */
{
    jcConvBuf *buf;

    CKCN(ctx);
    buf = ccx[ctx].buf;

    jcFix(buf);
/*    jcSaveDic(buf);		/* �ؽ�����Ƥ�����֤Ǥ��� */
    ccx[ctx].converting = 0;
    return 0;
}


/* ������ʸ����ѹ� */
int RkGoTo(ctx, bnum)
int	ctx;
int	bnum;
{
    jcConvBuf *buf;

    CKCN(ctx);
    CKCONV(ctx);
    buf = ccx[ctx].buf;
    if( 0 <= bnum && bnum < buf->nClause ) {
	while (bnum != buf->curClause)
	    jcMove( buf, ccx[ctx].small, 
		   buf->curClause < bnum ? JC_FORWARD : JC_BACKWARD );
	
	return( buf->curClause );
    }
    else
	return( -1 );
}


/* �������ʸ��κ���ư */
int RkLeft(ctx)
int	ctx;
{
    jcConvBuf *buf;

    CKCN(ctx);
    CKCONV(ctx);
    buf = ccx[ctx].buf;
    if( buf->curClause == 0 )
	jcBottom(buf);

    jcMove( buf, ccx[ctx].small, JC_BACKWARD );
    return( buf->curClause );
}


/* ������ʸ��α���ư */
int RkRight(ctx)
int	ctx;
{
    jcConvBuf *buf;

    CKCN(ctx);
    CKCONV(ctx);
    buf = ccx[ctx].buf;
    if( buf->curClause == buf->nClause - 1 )
	jcTop(buf);
    else
	jcMove( buf, ccx[ctx].small, JC_FORWARD);
    return( buf->curClause );

}


/* ������ʸ��򼡸�����֤������� */
int RkNext(ctx)
int	ctx;
{
    jcConvBuf *buf;
    int		ncand, curcand;
    
    CKCN(ctx);
    CKCONV(ctx);
    buf = ccx[ctx].buf;
    jcCandidateInfo(buf, ccx[ctx].small, &ncand, &curcand);
    if( curcand == ncand - 1 )	curcand = 0;
    else	curcand++;

    jcSelect( buf, curcand );
    return( curcand );
}


/* ������ʸ���ľ���θ�����֤������� */
int RkPrev(ctx)
int	ctx;
{
    jcConvBuf *buf;
    int		ncand, curcand;

    CKCN(ctx);
    CKCONV(ctx);
    buf = ccx[ctx].buf;
    jcCandidateInfo(buf, ccx[ctx].small, &ncand, &curcand);
    if( 0 == curcand )	curcand = ncand - 1;
    else	curcand--;
    jcSelect(buf, curcand);
    
    return( curcand );
}


/* ������������ */
int	RkGetKanjiList(ctx, kouho, bufsize)
int	ctx;
uchar *kouho;
int     bufsize;
{
    jcConvBuf *buf;
    int		ncand, curcand;
    int		j, i;
    wchar_t 	wChar[1024];
    
    CKCN(ctx);
    CKCONV(ctx);
    buf = ccx[ctx].buf;
    jcCandidateInfo(buf, ccx[ctx].small, &ncand, &curcand);
    for ( i = 0, j = 0; j < ncand; j++ ) {	/* ʸ���ɽ�� */
	jcGetCandidate(buf, j, wChar);
	
	w2c(wChar, kouho + i, bufsize - i);
	i += strlen(kouho + i) + 1;
    }
/*    jcSelect( buf, curcand ); */
    return( ncand );
}


/* ���ꤵ�줿�����ֹ�򥫥��ȸ���ˤ��� */
int RkXfer(ctx, knum)
int	ctx;
int knum;
{
    jcConvBuf *buf;
    int		ncand, curcand;

    CKCN(ctx);
    CKCONV(ctx);
    buf = ccx[ctx].buf;
    jcCandidateInfo(buf, ccx[ctx].small, &ncand, &curcand);
    if( 0 <= knum && knum < ncand ) {
	jcSelect( buf, knum );
/*	putKana( ctx, buf->curClause, 0);
	putchar('/'); */
    }
    else 
	return( -1 );

    return( knum );
}


/* ������ʸ����ɤߤ��ʤ�Ĺ�� len �ˤ��� */
int RkResize(ctx, len)
int	ctx;
int	len;		/* len �ϡ�EUC�����ɤǤΥХ��ȿ���Ϳ���� */
{
    int		len_kanap;
    int		n, nwords, nbytes;
    wchar_t	*wp;
    jcConvBuf   *buf;

    CKCN(ctx);
    buf = ccx[ctx].buf;
    for (nwords = 0, nbytes = 0, wp = buf->clauseInfo[buf->curClause].kanap ;
	 nbytes < len && wp < buf->kanaEnd ; wp++, nwords++) {
      if (*wp & 0x8080) { /* ���ܸ���ͭ�����ǥ��� */
	nbytes += 2;
      }
      else {
	nbytes ++;
      }
    }

    len_kanap = buf->clauseInfo[buf->curClause + 1].kanap
      - buf->clauseInfo[buf->curClause].kanap;
    
    if( nwords < len_kanap ) {
	n = len_kanap - nwords;
	while( n-- )
	    RkShorten(ctx);
    }
    else if( len > len_kanap ) {
	n = nwords - len_kanap;
	while( n-- )
	    RkEnlarge(ctx);
    }
    
    return( buf->nClause );
}


/* ������ʸ���Ĺ����̤�� */
int RkShorten(ctx)
int	ctx;
{
    CKCN(ctx);
    CKCONV(ctx);
    jcShrink(ccx[ctx].buf, ccx[ctx].small, 1);
    return( ccx[ctx].buf->nClause );
}


/* ������ʸ���Ĺ���򿭤Ф� */
int RkEnlarge(ctx)
int	ctx;
{
    CKCN(ctx);
    CKCONV(ctx);
    jcExpand(ccx[ctx].buf, ccx[ctx].small, 1);
    return( ccx[ctx].buf->nClause );
}


/* ������ʸ���̵�Ѵ��ξ��֤��᤹ */
int RkNfer(ctx)
int	ctx;
{
    int		ncand, curcand;
    
    CKCN(ctx);
    CKCONV(ctx);
/*    if( jcUnconvert(ccx[ctx].buf) == -1 ) */
    if (jcKana(ccx[ctx].buf, ccx[ctx].small, JC_HIRAGANA) == -1)
	return( -1 );

    jcCandidateInfo( ccx[ctx].buf, ccx[ctx].small, &ncand, &curcand );
    return( curcand );
}



/* ������ʸ����б������ɤߤ��ʤ��ΰ�yomi�����ꤷ�����Υ��ɥ쥹���֤�	*/
int	RkGetYomi(ctx, yomi, MAX)
int	ctx;
uchar *yomi;
int	MAX;
{
    jcConvBuf *buf;
    int n;
    int len;

    CKCN(ctx);
    CKCONV(ctx);
    buf = ccx[ctx].buf;
    n = buf->curClause;
    len = buf->clauseInfo[n+1].kanap - buf->clauseInfo[n].kanap;
    w2c(buf->clauseInfo[n].kanap, (char *)yomi,
	(MAX < len) ? MAX : len);

    return( strlen(yomi) );
}


/* ������ʸ����б��������������ΰ�kanji�����ꤷ�����Υ��ɥ쥹���֤� */
int	RkGetKanji(ctx, kanji, MAX)
int	ctx;
uchar *kanji;
int	MAX;
{
    jcConvBuf *buf;
    int n;
    int len;

    CKCN(ctx);
    CKCONV(ctx);
    buf = ccx[ctx].buf;
    n = buf->curClause;
    len = buf->clauseInfo[n+1].dispp - buf->clauseInfo[n].dispp;
    w2c(buf->clauseInfo[n].dispp, (char *)kanji, (MAX < len) ? MAX : len);

    return( strlen(kanji) );
}


RkCreateContext()
{
  int i;

  for (i = 0 ; i < MAXCONTEXT && ccx[i].occupied ; i++)
    ;
  if (i < MAXCONTEXT) {
    ccx[i].occupied = 1;
    ccx[i].small = 1;
    ccx[i].converting = 0;
    ccx[i].tan = 0;
    ccx[i].buf = jcCreateBuffer(wnn_env, 0, 0);	/* �Ѵ��Хåե��κ��� */
    return i;
  }
  else {
    return -1;
  }
}

RkDuplicateContext(cn)
int cn;
/* ARGSUSED */
{
  return RkCreateContext();
}

RkCloseContext(cn)
int cn;
{
  CKCN(cn);
  if (ccx[cn].occupied) {
    jcDestroyBuffer(ccx[cn].buf, 1);
    ccx[cn].buf = (jcConvBuf *)0;
    ccx[cn].occupied = 0;
    return 0;
  }
  else {
    return -1;
  }
}

RkSetDicPath(cn, path)
int cn;
char *path;
/* ARGSUSED */
{
    return 0;
}

RkGetStat(cn, stat)
int cn;
RkStat *stat;
{
  jcConvBuf *buf;
  wchar_t *wp;
  int n;

  CKCN(cn);
  CKCONV(cn);
  buf = ccx[cn].buf;
  stat->bunnum = buf->curClause;
  stat->candnum = buf->curCand;
  stat->maxcand = buf->nCand;
  stat->diccand = buf->nCand;
  for (n = 0, wp = buf->clauseInfo[buf->curClause].kanap ;
       wp < buf->clauseInfo[buf->curClause + 1].kanap ; wp++) {
    if (*wp & 0x8080) {
      n += 2;
    }
    else {
      n ++;
    }
  }
  stat->ylen = n;
  for (n = 0, wp = buf->clauseInfo[buf->curClause].dispp ;
       wp < buf->clauseInfo[buf->curClause + 1].dispp ; wp++) {
    if (*wp & 0x8080) {
      n += 2;
    }
    else {
      n ++;
    }
  }
  stat->klen = n;
  stat->tlen = 1;/* ����Ǥ� */
  return 0;
}

/*
 * ��ʸ���ñ��Ȥ��������ʲ���򤷤Ƥ��롣����Ͼ���Ū�ˤ�ľ��������
 */

RkGetLex(cn, lex, maxlex)
int cn, maxlex;
RkLex *lex;
{
  RkStat stat;

  if (RkGetStat(cn, &stat) < 0) return -1;

  if (maxlex < 1) return 0;

  lex->ylen = stat.ylen;
  lex->klen = stat.klen;
  lex->rownum = 0;
  lex->colnum = 0;
  return 1;
}

/* �������鲼�϶��äݤǤ� */
RkCreateDic(cn, dicname, mode)
int cn, mode;
unsigned char *dicname;
{
    return -1;
}

RkGetDicList(cn, dicnames_return, MAX)
int cn;
char *dicnames_return;
int MAX;
{
  return 0;
}

RkGetMountList(cn, dicnames_return, MAX)
int cn;
char *dicnames_return;
int MAX;
{
  return 0;
}

RkUnmountDic(cn, dicname)
int cn;
unsigned char *dicname;
{
  return 0;
}

RkRemountDic(cn, dicname, where)
int cn, where;
unsigned char *dicname;
{
  return 0;
}

RkStoreYomi(cn, yomi, MAX) /* ����Ϥ����Ǥʤ��Ρ�*/
int cn, MAX;
char *yomi;
{
  return -1;
}

RkDefineDic(cn, dicname, wordrec)
int cn;
unsigned char *dicname, *wordrec;
{
  return -1;
}

RkDeleteDic(cn, dicname, wordrec)
int cn;
unsigned char *dicname, *wordrec;
{
  return -1;
}
