#include	<stdio.h>
#include	"RK.h"
#include	"RKintern.h"

void
printList(s, c)
unsigned char	*s;
int		c;
{
    while ( *s ) {
	while ( *s ) putchar(*s++);
	putchar(c);
	s++;
    };
}

void
printContext(ctx)
{
   char			names[256];
   int			n1;
   struct RkContext	*cx = RkGetContext(ctx);
   struct DD		**ddp = cx->ddpath;
   struct DD		*dd;
   struct DF		*f, *fh;
   struct DM		*m, *mh;
   int			i;

   printf("*** CONTEXT %d ***\n", ctx);

   n1 = RkGetDirList(ctx, names, sizeof(names));
   printf("DIR#%d:\t", n1);
   printList(names, ',');
   putchar(10);

   if ( ddp ) 
       printf("DDP:\n");
       for ( i = 0; dd = ddp[i]; i++ ) {
	    (void)printf("(%d)  %s#%d [", i, dd->dd_name, dd->dd_rcount);
	    fh = &dd->dd_files;
	    for ( f = fh->df_next; f != fh; f = f->df_next ) {
		(void)printf("  %s#%d(", f->df_link, f->df_rcount);
		mh = &f->df_members;
		for ( m = mh->dm_next; m != mh; m = m->dm_next ) 
		    (void)printf(" %s#%d", m->dm_nickname, m->dm_rcount);
		(void)printf(")");
	    };
	    (void)printf("]\n");
	   
       };
   n1 = RkGetDicList(ctx, names, sizeof(names));
   printf("DIC#%d:\t", n1);
   printList(names, ',');
   putchar(10);
   n1 = RkGetMountList(ctx, names, sizeof(names));
   printf("MNT#%d:\t", n1);
   printList(names, ',');
   putchar(10);
}

henkan(ctx, s)
{
    char	kanji[1000], yomi[1000];
    int		i, j, n;
    int		k, y;
    RkStat	st;
    int		mode;
    int		n1, n2, l1;
    RkLex	lex[256];

    putchar(10);

    mode = 0;
    mode = (mode<<RK_XFERBITS)|RK_ZFER;
    mode = (mode<<RK_XFERBITS)|RK_KFER;
    mode = (mode<<RK_XFERBITS)|RK_XFER;
    n = RkBgnBun(ctx, s, strlen(s), mode);
    for ( i = 0; i < n; i++ ) {
	RkGoTo(ctx, i);
	RkGetKanji(ctx, kanji, 10000);
	RkGetYomi( ctx,  yomi, 10000);
	RkGetStat( ctx, &st);	
	l1 = RkGetLex(ctx, lex, 100);
	k = y = 0;
	for ( j = 0; j < l1; j++ ) {
	    char	kk[100], yy[100], ll[100];

	    sprintf(ll, "#%d#%d", lex[j].rownum, lex[j].colnum);
	    strncpy(kk, kanji + k, lex[j].klen);
	    kk[lex[j].klen] = 0;
	    k += lex[j].klen;
	    strncpy(yy, yomi + y, lex[j].ylen);
	    yy[lex[j].ylen] = 0;
	    y += lex[j].ylen;
	    printf("%s(%s C%d %s)", kk, yy, lex[j].dicnum, ll);
	};
/* stat */
	printf(";  (Y%d K%d T%d #%d/%d)\n", 
		st.ylen, st.klen, st.tlen, st.diccand, st.maxcand);
    };
    RkEndBun(ctx, 1);
}

main(argc, args)
int	argc;
char	*args[];
{
    int		c;
    char		*file, *term;
    char		*user;
    char		src[2560];
    int			n1, n2;
    char		names[256];
    int			ctx = 0;
    char		dic[256];
    char		kana[2560];
    int			flags = RK_XFER|RK_SOKON|RK_FLUSH;
    struct RkKxRoma	*rdic;
    unsigned		data[2], nword;
    struct nword	*w;
    char		*home = getenv("iroha");

   data[0] = sbrk(0);
/* initialization
 *	for more minute set-up, use _RkInitlialize
 */
   _RkInitialize(home,
	/*
	0, 2*1024, 60*1024);
	0, 2*1024, 4*1024);
	*/
	0, 256, 10*1024);

   ctx = 0;
   rdic = RkOpenRoma("romaji.rdic");
   while ( gets(src) ) {
	int	status = 0;

	switch(src[0] ) {
	case '#':
	   puts(src);
	   break;
    /* henakan */
	case 'X': 
	    n1 = RkCvtRoma(rdic, kana, 100, src + 2, strlen(src + 2), flags);
	    printf("ROMA <%s>\n", src + 2);
	    n1 = RkCvtRoma(rdic,  kana , 100, src + 2, strlen(src + 2), 
		RK_KFER|RK_SOKON|RK_FLUSH);
	    printf("KATA <%s> len %d\n", kana, n1);
	    n1 = RkCvtRoma(rdic,  kana , 100, src + 2, strlen(src + 2), 
		RK_HFER|RK_SOKON|RK_FLUSH);
	    printf("HFER <%s> len %d\n", kana, n1);
	    n1 = RkCvtRoma(rdic,  kana , 100, src + 2, strlen(src + 2), 
		RK_XFER|RK_SOKON|RK_FLUSH);
	    printf("HIRA <%s> len %d\n", kana, n1);
	    henkan(ctx, kana);
	    break;
    /* directory path */
	case 'P':
	   RkSetDicPath(ctx, src + 2);
	   break;
    /* context */
	case 'D':
	   ctx = RkDuplicateContext(ctx);
	   printf("CONTEXT = %d\n", ctx);
	   break;
	case '=':
	   ctx = atoi(src + 2);
	   printf("CONTEXT = %d\n", ctx);
	   break;
	case '?':
	   printContext(ctx);
	   break;
    /* mount */
	case 'M':
	   strcpy(dic, src + 2);
	   status = RkMountDic(ctx, dic, 0);
	   break;
	case 'U':
	   strcpy(dic, src + 2);
	   status = RkUnmountDic(ctx, dic, 0);
	   break;
	case '<':	/* sentou ni mount */
	   strcpy(dic, src + 2);
	   status = RkRemountDic(ctx, dic, 0);
	   break;
	case '>':	/* saigo ni mount */
	   strcpy(dic, src + 2);
	   status = RkRemountDic(ctx, dic, 1);
	   break;
    /* tempdic */
	case '+':
	   status = RkDefineDic(ctx, dic, src + 2);
	   break;
	case '-':
	   status = RkDeleteDic(ctx, dic, src + 2);
	   break;
	case '!':
	   status = RkPrintDic(ctx, dic);
	   break;
	};

	if ( status )
	     printf("???\n");
        data[1] = sbrk(0);
	nword = 0;
	for ( w = SX.word; w; w = w->nw_next )	nword++;
	printf("DATA %d - %d = %d\tWORD %d\n",
		data[1], data[0], data[1]-data[0], nword);
    };
   RkCloseRoma(rdic);
/* shuuryou shori */
   RkFinalize();
}
