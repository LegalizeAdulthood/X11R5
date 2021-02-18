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
static char rcsid[]="@(#) 102.1 $Header: /imports/wink/work/nk.R3.1/lib/RK/RCS/RKcontext.c,v 1.7 91/09/13 12:04:15 kubota Exp $";
#endif
/*LINTLIBRARY*/
#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	"RK.h"
#include	"RKintern.h"

#define		STRCMP(d, s)	strcmp((char *)(d), (char *)(s))
static 
char	*
allocStr(s)
unsigned char	*s;
{
    char	*d;

    if ( s && (d = (char *)malloc((unsigned)(strlen((char *)s) + 1))) )
	(void)strcpy((char *)d, (char *)s);
    return d;
}
/*
 * DM
 */

struct DM	*
createDM(df, dicname, nickname)
struct DF	*df;
unsigned char	*dicname, *nickname;
{
    register struct DM	*dm;
    
    if ( dm = (struct DM *)calloc(1, sizeof(struct DM)) ) {
	dm->dm_next = dm->dm_prev = dm;
	dm->dm_file = df;

	dm->dm_dicname  = allocStr(dicname);
	dm->dm_nickname = allocStr(nickname);
	dm->dm_class = dm->dm_flags = dm->dm_packbyte = dm->dm_rcount = 0;
	dm->dm_gram = (struct RkKxGram *)0;
	dm->dm_extdata = (char *)0;
    };
    return dm;
}
void
freeDM(dm)
register struct DM	*dm;
{
    if ( dm ) {
	dm->dm_next->dm_prev = dm->dm_prev;
	dm->dm_prev->dm_next = dm->dm_next;
	if ( dm->dm_dicname )
	    (void)free((char *)dm->dm_dicname);
	if ( dm->dm_nickname )
	    (void)free((char *)dm->dm_nickname);
    /* 
	free member data 
     */
	(void)free((char *)dm);
    };
}

struct DM	*
allocDM(df, dicname, nickname)
struct DF	*df;
unsigned char	*dicname;
unsigned char	*nickname;
{
    struct DM	*m;
    struct DM	*mh = &df->df_members;

/* jisho wo sagasu */
    for ( m = mh->dm_next; m != mh; m = m->dm_next )
	if ( !STRCMP(m->dm_dicname,  dicname) )
	    break;
/* nakereba sakusei suru */
    if ( m == mh ) {
	if ( m = createDM(df, dicname, nickname) ) {
	    m->dm_next = mh;
	    m->dm_prev = mh->dm_prev;
	    mh->dm_prev = m;
	    m->dm_prev->dm_next = m;
#ifdef RK_DEBUG
	} else {
	    fprintf (stderr, "allocDM: cannot create DM %s\n", nickname);
#endif
	};
    };
    return m;
}
/*
 * DF
 */

struct DF	*
newDF(dd, link)
struct DD	*dd;
unsigned char	*link;
{
    register struct DF	*df;
    
    if ( df = (struct DF *)calloc(1, sizeof(struct DF)) ) {
	register struct DM	*dm = &df->df_members;

	df->df_next = df->df_prev = df;
	df->df_direct = dd;
	dm->dm_next = dm->dm_prev = dm;
	df->df_link = allocStr(link);
	df->df_rcount = 0;
	df->df_type  = 0 ;

	df->df_extdata = (char *)0;
    };
    return df;
}

void
freeDF(df)
struct DF	*df;
{
    struct DM	*m, *n;
    struct DM	*mh = &df->df_members;

    /*
    (void)printf("freeDF %s(%d): ", df->df_link, df->df_rcount);
    */
    for ( m = mh->dm_next; m != mh; m = n ) {
	n = m->dm_next;
	/*
	(void)printf(" %s(%d)", m->dm_nickname, m->dm_rcount);
	*/
	freeDM(m);
    };
    /*
    (void)printf("\n");
    */
    df->df_next->df_prev = df->df_prev;
    df->df_prev->df_next = df->df_next;
    (void)free((char *)df->df_link);
    (void)free((char *)df);
}

struct DF	*
allocDF(dd, link)
struct DD	*dd;
unsigned char	*link;
{
    struct DF	*f;
    struct DF	*fh = &dd->dd_files;

    for ( f = fh->df_next; f != fh; f = f->df_next ) {
	if ( !STRCMP(f->df_link,  link) ) {
	    break;
	}
    }
    if ( f == fh ) {
	if ( f = newDF(dd, link) ) {
	    f->df_next = fh;
	    f->df_prev = fh->df_prev;
	    fh->df_prev = f;
	    f->df_prev->df_next = f;
	};
    };
    return f;
}
/*
 * DD
 */
static
struct DD	*
newDD(path, name)
unsigned char	*path, *name;
{
    struct DD	*dd;
    
    if ( dd = (struct DD *)calloc(1, sizeof(struct DD)) ) {
	dd->dd_next = dd->dd_prev = dd;
#ifndef debug_Jan_26
	dd->dd_path = allocStr(path);
	dd->dd_name = allocStr(name);
#else
	if (!(dd->dd_path = allocStr(path))) {
	    fprintf(stderr, "newDD: cannot allocate %s\n", path);
	    (void)freeDD(dd);
	    return NULL;
	}
	if (!(dd->dd_name = allocStr(name))) {
	    fprintf(stderr, "newDD: cannot allocate %s\n", name);
	    (void)free((char *)dd->dd_path);
	    (void)freeDD(dd);
	    return NULL;
	}
#endif
	dd->dd_rcount = 0;
	dd->dd_files.df_next = dd->dd_files.df_prev = &dd->dd_files;
	dd->dd_flags = 0;
    };
    return dd;
}
static 
void
freeDD(dd)
struct DD	*dd;
{
    struct DF	*f, *g;
    struct DF	*fh = &dd->dd_files;

    /*
    (void)printf("freeDD %s(%d)\n", dd->dd_name, dd->dd_rcount);
    */
    for ( f = fh->df_next; f != fh; f = g ) {
	g = f->df_next;
	freeDF(f);
    };
    dd->dd_next->dd_prev = dd->dd_prev;
    dd->dd_prev->dd_next = dd->dd_next;
    (void)free((char *)dd->dd_path);
    (void)free((char *)dd->dd_name);
    (void)free((char *)dd);
}
static
struct DD	*
findDD(dd, name)
struct DD	*dd;
unsigned char	*name;
{
    struct DD	*d;

    for ( d = dd->dd_next; d != dd; d = d->dd_next )
	if ( !STRCMP(d->dd_name,  name) )
	    return d;
    return (struct DD *)0;
}
/* openDIR
 *	open the dictonaries according to the  dics.dir file 
 */
static
char	*
DF2File(df)
struct DF       *df;
{
    struct DD   *dd = df->df_direct;
    unsigned 	sz;
    char        *nm;

    sz = strlen(dd->dd_path) + strlen(df->df_link) + 2;
    if ( nm = (char *)malloc(sz) )  {
	(void)strcpy(nm, dd->dd_path);
	(void)strcat(nm, "/");
	(void)strcat(nm, df->df_link);
    };
    return nm;
}
static
struct DD	*
openDIR(path, name)
char	*path;
char	*name;
{
    int                 i;
    FILE		*fp;
    char		direct[512];
    char		file[512];
    char		*dics_dir = "/dics.dir";
    unsigned char	line[256*4];
    unsigned char	link[256], member[256], nickname[256];
    unsigned char	*s, *d, *t, par;
    struct DD		*dd;
    struct DD		*dh = &SX.dd;
    struct DF		*df;
    struct DM		*dm;

    if ( dd = findDD(dh, (unsigned char *)name) )
      return dd;
    dd = newDD((unsigned char *)path, (unsigned char *)name);
    if ( !dd )
      return dd;
 
   /* jisho table ga aruka ? */
    if ( strlen(path) + strlen(dics_dir) + 1 >= sizeof(direct) )
      return (struct DD *)0;
    (void)strcpy(direct, path);
    (void)strcat(direct, dics_dir);
    fp = fopen(direct, "r");
    if ( fp ) { /* openできなかったら次の処理は飛ばしてDDだけ作って終わる */

   /* read dics_dir lines */
    while ( fgets(s = line, sizeof(line), fp) ) {
      int	dftype = -1;
      int	dmclass = -1;
      
      if ( !isalpha(line[0]) )
	continue;
      /* parse line %s(%s)   -%s- */
      for ( d = link; *s && *s != '('; ) *d++ = *s++;
      *d = 0;
      if ( !*s++ ) continue;
      if ( !(t = (unsigned char *)strrchr(link, '.' )) )  continue;
      t++;
      if ( !STRCMP(t, "d") )  dftype = DF_PERMDIC;
      else
	if ( !STRCMP(t, "t") )  dftype = DF_TEMPDIC;
	else			
	  continue;
      for ( d = member; *s && *s != ')'; ) *d++ = *s++;
      *d = 0;
      if ( !*s++ ) continue;
      if ( !(t = (unsigned char *)strrchr(member, '.' )) )  continue;
      t++;
      if ( !STRCMP(t, "mwd") )   dmclass = ND_MWD;
      else
	if ( !STRCMP(t, "swd") )   dmclass = ND_SWD;
	else
	  if ( !STRCMP(t, "pre") )   dmclass = ND_PRE;
	  else
	    if ( !STRCMP(t, "suc") )   dmclass = ND_SUC;
	    else
	      continue;
      while ( *s && isspace(*s) ) s++;
      if ( !(par = *s++) )
	continue;
      if ((! *s) || (*s == par))
	continue;
      d = nickname;
      *d++ = *s++;
      for(i=1; (i < 256) && *s && (*s != par); i++ ) {
	*d++ = *s++;
      }
/*      while ( *s && (*s != par)) {
	*d++ = *s++;
      }
*/
      *d = 0;
      if ( !*s++ ) continue;
      
      if ( strlen((char *)path) + strlen((char *)link) + 1 >= sizeof(file) )
	continue;
      (void)strcpy(file, path);
      (void)strcat(file, "/");
      (void)strcat(file, link);
      if ( access(file, 0) )
	continue;

      if ( df = allocDF(dd, link) ) {
	df->df_type = dftype;
	if ( dm = allocDM(df, member, nickname) ) 
	  dm->dm_class = dmclass;
      };
    };
    fclose(fp);
  }
    dd->dd_next = dh;
    dd->dd_prev = dh->dd_prev;
    dh->dd_prev = dd;
    dd->dd_prev->dd_next = dd;
    return dd;
}
static
int
MountMD(cx, dm, mode)
struct RkContext	*cx;
struct DM		*dm;
int			mode;
{
    struct MD	*md = (struct MD *)calloc(1, sizeof(struct MD));

    if ( md ) {
	struct DF	*df = dm->dm_file;
	struct MD	*head;

    /* increment the reference counter */
	if ( dm->dm_rcount == 0 ) {	/* first mount */
	    char	*file = DF2File(df);
	    int		status;

	    if ( file ) {
		/*
		status = DST_OPEN(dm, file, (mode&MD_WRITE) ? DM_WRITABLE: 0);
		*/
		status = DST_OPEN(dm, file,  DM_WRITABLE);
		(void)free(file);
		if ( status ) {
		    (void)free((char *)md);
		    return -1;
		};
	    };
	};
	dm->dm_rcount++;
	/*
	(void)printf("MountMD %s(%s) #%d\n", 
		df->df_link, dm->dm_nickname, dm->dm_rcount);
	*/

    /* put it at the end of the mount list */
	head = cx->mount[dm->dm_class];
	md->md_next = head;
	md->md_prev = head->md_prev;
	head->md_prev = md;
	md->md_prev->md_next = md;
	md->md_dic = dm;
	md->md_flags = mode&MD_WRITE;
    /* wait for the translation to finish */
	if ( IS_XFERCTX(cx) ) 
	    md->md_flags |= MD_MPEND;
	return 0;
    };
    return -1;
}
/* ARGSUSED*/

void
UmountMD(cx, md)
struct RkContext	*cx;
struct MD		*md;
{
    struct DM	*dm = md->md_dic;
    struct DF	*df = dm->dm_file;
    struct DD	*dd = df->df_direct;

    if ( IS_XFERCTX(cx) ) 	/* await */
	md->md_flags |= MD_UPEND;
    else {
    /* remove MD from the mount list */
	md->md_prev->md_next = md->md_next;
	md->md_next->md_prev = md->md_prev;
	(void)free((char *)md);
    /* decrement the reference counter and check it */
	--dm->dm_rcount;
	/*
	(void)printf("UmountMD %s(%s)- #%d\n", 
		df->df_link, dm->dm_nickname, dm->dm_rcount);
	*/
	if ( dm->dm_rcount == 0 ) {	/* last mount */
	    char	*file = DF2File(df);

	    if ( file ) {
		(void)DST_CLOSE(dm, file);
		if ( df->df_rcount == 0 ) {
		    /*
		    (void)printf("CLOSE FILE %s dir #%d\n", file, dd->dd_rcount);
		    */

		    if ( --dd->dd_rcount == 0 )
			freeDD(dd);
		};
		(void)free(file);
	    };
	}
    };
}

/* RkInitialize: Renbunsetsu Henkan shokika
 *	subeteno Renbunsetsu henkan kannsuu wo siyou suru maeni
 *      itido dake call suru koto.
 * returns: -1/0
 */

static struct RkContext	CX[MAX_CONTEXT];
#define	IsCxNum(cn)	(0<=(cn)&&(cn)<MAX_CONTEXT)
#define	IsLiveCxNum(cn) (IsCxNum(cn) && IS_LIVECTX(&CX[cn]))
#define	IsXferCxNum(cn) (IsLiveCxNum(cn) && IS_XFERCTX(&CX[cn]))
int
_RkGetTick(mode)
int	mode;
{
    static time = 0;

    if ( mode )
	return time++;
    else
	return time;
}

int	
_RkInitialize(ddhome, numWord, numCache, heapByte)
char	*ddhome;
int	numWord;
int	numCache;
int	heapByte;
{
    int		i;
    struct RkParam	*sx = &SX;
    struct DD	*dd = &sx->dd;
    char	gramdic[256];

/* SX: set up system parameter  */
    sx->gramdic = (struct RkKxGram *)0;
    if ( strlen(ddhome) + strlen("/iroha/fuzokugo.d") + 1 < sizeof(gramdic) ) {
	sprintf(gramdic, "%s/iroha/fuzokugo.d", ddhome);
	sx->gramdic = RkOpenGram(gramdic, 0);
    };

    sx->word = (struct nword *)0;
    dd->dd_next = dd->dd_prev = dd;
    sx->ddhome = allocStr((unsigned char *)ddhome);

/* CX: set up per contex data */
    for( i = 0; i < MAX_CONTEXT; i++ )	
	CX[i].flags = 0;

/* set up memory allocators */
     if ( _RkInitializeWord(numWord) )
	return -1;
     if ( _RkInitializeCache(numCache) )
	return -1;
     if ( _RkInitializeHeap(heapByte) )
	return -1;
     if (  RkCreateContext() )
	return -1;
     return 0;
}

int	
RkInitialize(ddhome)
char	*ddhome;
{
    /*
     * Word:	????
     * Cache:	36B*512 	= 20KB
     * Heap:	30*1024B	= 30KB
     */
    return _RkInitialize(ddhome, 0, 512*10, 30*1024);
}

/* RkFinalize: Renbunsetu henkan shuuryou shori
 *
 */
void	
RkFinalize()
{
    struct RkParam	*sx = &SX;
    int	i;

/* houchi sareta context wo close */
    for( i = 0; i < MAX_CONTEXT; i++ )
	if ( IS_LIVECTX(&CX[i]) )
	    RkCloseContext(i);
/* sonohoka no shuuryou shori */
    _RkFinalizeWord();
    _RkFinalizeCache();
    _RkFinalizeHeap();
    if ( sx->ddhome ) 
	(void)free((char *)sx->ddhome);
    sx->ddhome = (char *)0;
    if ( sx->gramdic )
	RkCloseGram(sx->gramdic);
}

/* RkGetSystem: System heno pointer wo motomeru
 */
struct RkParam	*
RkGetSystem()
{
    return &SX;
}

/* RkGetContext: Context heno pointer wo motomeru
 *
 */
struct RkContext *
RkGetContext(cx_num)
int	cx_num;
{
     if ( IsLiveCxNum(cx_num) ) 
	 return &CX[cx_num];
     else {
#ifdef IROHA_DEBUG
	  (void)fprintf(stderr, "bad context number %d\n", cx_num);
#endif
	  return (struct RkContext *)0;
     };
}
struct RkContext *
RkGetXContext(cx_num)
int	cx_num;
{
     struct RkContext	*cx = RkGetContext(cx_num);

     if ( cx && !(cx->flags&CTX_XFER) ) {
#ifdef IROHA_DEBUG
	  (void)fprintf(stderr, "out of BgnBun %d\n", cx_num);
#endif
	  cx = (struct RkContext *)0;
     };
     return cx;
}

/*
 * Counterpart of the RkBgnbun/RkEndBun
 */
void	
_RkBgnBun(cx)
struct RkContext	*cx;
{
    cx->flags |= CTX_XFER;
}
static
int
isinDDP(ddp, dd)
struct DD	**ddp;
struct DD	*dd;
{
    while ( *ddp ) 
       if ( *ddp++ == dd )
	    return 1;
    return 0;
}
void	
_RkEndBun(cx)
struct RkContext	*cx;
{
    struct DD	**ddp = cx->ddpath;
    int		c;

    cx->flags &= ~CTX_XFER;
    for ( c = 0; c < 4; c++ ) {
	struct MD	*head, *md, *nd;

	head = cx->mount[c];
	for ( md = head->md_next; md != head; md = nd ) {
	    struct DM 	*dm = md->md_dic;
	    struct DF	*df = dm->dm_file;
	    struct DD	*dd = df->df_direct;

	    nd = md->md_next;
	    if ( md->md_flags & MD_MPEND ) 	/* release pending */
		md->md_flags &= ~MD_MPEND;
	    if ( md->md_flags & MD_UPEND ) 	/* unmount pending */
		UmountMD(cx, md);
	    else
	    if ( !isinDDP(ddp, dd) ) 		/* unreachable */
		UmountMD(cx, md);
	};
    };
}
/* RkSetDicPath
 *
 */
static
int
countDDP(ddp)
struct DD	**ddp;
{
    int	count = 0;

    if ( ddp )
	while ( ddp[count] )  count++;
    return count;
}
static
struct DD	**
copyDDP(ddp)
struct DD	**ddp;
{
    struct DD	**new = (struct DD **)0;
    int		i;
    struct DD	*dd;

    if ( ddp ) {
	int	count = countDDP(ddp);

	new = (struct DD **)calloc(count + 1, (unsigned)sizeof(struct DD *));
	if ( new ) 
	    for ( i = 0; dd = new[i] = ddp[i]; i++ ) 
		dd->dd_rcount++;
    };
    return new;
}
static
struct DD	**
appendDDP(ddp, dd)
struct DD	**ddp;
struct DD	*dd;
{
    struct DD	**new;
    int		i;
    int		count = countDDP(ddp);

    new = (struct DD **)calloc(count + 2, (unsigned)sizeof(struct DD *));
    if ( new ) {
	if ( ddp ) {
	    for ( i = 0; i < count; i++ ) new[i] = ddp[i];
	    (void)free((char *)ddp);
	};
	new[count++] = dd;
	new[count] = (struct DD *)0;
	dd->dd_rcount++;
    }
    else
	new = ddp;
    return new;
}
static
struct DD	**
createDDP(ddpath)
char	*ddpath;
{
    char		path[256], dir[256], *d, *s;
    struct DD		**ddp = (struct DD **)0;
    struct DD		*dd;

    for ( s = ddpath; *s;  ) {
	while ( *s && isspace(*s) ) s++;
	if ( !*s )
	    break;
	for ( d = dir; *d = *s; d++ ) 
	    if ( *s++ == ':' ) {
		*d = 0;
		break;
	    };
	(void)strcpy(path, SX.ddhome);
	(void)strcat(path, "/");
	(void)strcat(path, dir);
	if ( !(dd = openDIR(path, dir) ) )
	  fprintf(stderr, "error\n");
	else
	  ddp = appendDDP(ddp, dd);
    };
    return ddp;
}
static
void
freeDDP(ddp)
struct DD	**ddp;
{
    struct DD	*dd;
    int		i;

    if ( ddp ) {
	for ( i = 0; dd = ddp[i]; i++ ) 
	    if ( --dd->dd_rcount == 0 ) {	/* free this directory */
		freeDD(dd);
	    };
	(void)free((char *)ddp);
    };
}

struct DM	*
searchDDP(ddp, name)
struct DD	**ddp;
char		*name;
{
    struct DD	*dd;
    struct DF	*f, *fh;
    struct DM	*m, *mh;
    int		i;

    if ( ddp ) {
	for ( i = 0; dd = ddp[i]; i++ ) {
	    fh = &dd->dd_files;
	    for ( f = fh->df_next; f != fh; f = f->df_next ) {
		mh = &f->df_members;
		for ( m = mh->dm_next; m != mh; m = m->dm_next ) {
		    if ( !STRCMP(m->dm_nickname, name) ) 
			return m;
		};
	    };
	};
    };
    return (struct DM *)0;
}
int
RkSetDicPath(cx_num, path)
int	cx_num;
char	*path;
{
    struct RkContext	*cx = RkGetContext(cx_num);
    struct DD		**new;

    new = createDDP(path);
    if ( new ) {
	freeDDP(cx->ddpath);
	cx->ddpath = new;
	return 0;
    };
    return -1;
}
int	
RkCreateContext()
{
    int			cx_num, i;

    /* saisho no aki context wo mitsukeru */
    for( cx_num = 0; cx_num < MAX_CONTEXT ; cx_num++ )
	if( CX[cx_num].flags == 0 ) {
	    register struct RkContext	*cx = &CX[cx_num];

	/* create mount list headers */
	    for ( i = 0; i < 4; i++ ) {
	        struct MD	*mh = (struct MD *)calloc(1, sizeof(struct MD));

		if ( !mh )
		   return -1;
		mh->md_next = mh->md_prev = mh;
		mh->md_dic = (struct DM *)0;
		mh->md_flags = 0;
		cx->mount[i] = mh;
	    };
	    cx->ddpath = (struct DD **)0;
	    cx->flags = CTX_LIVE;
	    cx->kouhomode = 0;
	    cx->gwt_cx = -1; /* means no GetWordTextdic context is available */

	    if (!(cx->gwt_dicname = (unsigned char *)
		  malloc((unsigned)sizeof(unsigned char *)))) {
	        return -1;
	    }

	    cx->litmode = (unsigned long *)calloc(MAXLIT,sizeof(unsigned long));
	    if ( cx->litmode ) {
		for ( i = 0; i < MAXLIT; i++ )
		    cx->litmode[i] = 0x87654321;
		cx->litmode[LIT_NUM] = 0x8765432;
		cx->litmode[LIT_ALPHA] = 0x21;
		cx->litmode[LIT_HIRA] = 0x1;
	    }

	    return cx_num;
	};
    return -1;
}
int	
RkCloseContext(cx_num)
int	cx_num;
{
    register struct RkContext	*cx = RkGetContext(cx_num);
    int				i;

    if ( !cx )
	return -1;
/* close context for GetWordTextDic recursively */
    if ( cx->gwt_cx >= 0 ) {
      RkCloseContext(cx->gwt_cx);
      cx->gwt_cx = -1;
    }
    if (cx->gwt_dicname) {
      (void)free((unsigned char *)cx->gwt_dicname);
      cx->gwt_dicname = (unsigned char *)NULL;
    }
    freeTdn(cx);
/* terminate bunsetu henkan */
    if ( cx->flags & CTX_XFER )
	RkEndBun(cx_num, 0);
    freeDDP(cx->ddpath);
    cx->ddpath = (struct DD **)0;
/* subete no jisho wo MD suru */
    for ( i = 0; i < 4; i++ ) {
	struct MD	*mh, *m, *n;

    /* destroy mount list */
	mh = cx->mount[i];
	for ( m = mh->md_next; m != mh; m = n ) {
	     n = m->md_next;
	     (void)UmountMD(cx, m);
	};
	(void)free((char *)mh);
	cx->mount[i] = (struct MD *)0;
    };
/* convertion table */
    if ( cx->litmode )
	(void)free((char *)cx->litmode);
    cx->flags = 0;
    return 0;
}
/* RkDuplicateContext
 *	onaji naiyou no context wo sakuseisuru
 */
int
RkDuplicateContext(cx_num)
int cx_num;
{
    register struct RkContext	*sx = RkGetContext(cx_num);
    int				dup = -1;

    if ( sx ) {
	dup = RkCreateContext();
	if ( dup >= 0 ) {
	    int		i;
	    register struct RkContext	*dx = RkGetContext(dup);

	/* copy the mount list */
	    for ( i = 0; i < 4; i++ ) {
		struct MD	*mh, *md;

	    /* should mount dictionaries in reverse order */
		mh = sx->mount[i];
		for ( md = mh->md_prev; md != mh; md = md->md_prev ) 
		    (void)MountMD(dx, md->md_dic, md->md_flags&MD_WRITE);
	    };
	/* copy DDP */
	    dx->ddpath = copyDDP(sx->ddpath);
	/* copy lit */
	    if ( sx->litmode && dx->litmode ) 
		for ( i = 0; i < MAXLIT; i++ )
		    dx->litmode[i] = sx->litmode[i];
	};
    }
    return dup;
}
/* RkMountDic: append the specified dictionary at the end of the mount list */
int
RkMountDic(cx_num, name, mode)
int	cx_num;		/* context specified */
char	*name;		/* the name of dictonary */
int	mode;		/* mount mode */
{
    struct RkContext	*cx = RkGetContext(cx_num);

    if ( cx ) {
	struct DM	*dm = searchDDP(cx->ddpath, name);

	if ( dm ) {
	    struct MD	*mh = cx->mount[dm->dm_class];
	    struct MD	*md, *nd;
	    int		count = 0;

	/* search the dictionary */
	    for ( md = mh->md_next; md != mh; md = nd ) {
		nd = md->md_next;
		if ( md->md_dic == dm ) {	/* already mounted */
		/* cancel the previous unmount */
		    if ( md->md_flags & MD_UPEND )
			md->md_flags &= ~MD_UPEND;
		    count++;
		};
	    };
	    if ( count == 0 )
		return MountMD(cx, dm, mode);
	};
    };
    return -1;
}
/* RkUnmountDic: removes the specified dictionary from the mount list */
int
RkUnmountDic(cx_num, name)
int	cx_num;
char	*name;
{
    struct RkContext	*cx = RkGetContext(cx_num);
    int			i;

    if ( cx ) {
	for ( i = 0; i < 4; i++ )  {
	    struct MD	*mh = cx->mount[i];
	    struct MD	*md, *nd;

	    for ( md = mh->md_next; md != mh; md = nd ) {
		struct DM	*dm = md->md_dic;

		nd = md->md_next;
		if ( !STRCMP(dm->dm_nickname, name) ) 
		     UmountMD(cx, md);
	    };
	};
	return 0;
    };
    return -1;
}
/* RkRemountDic: relocate the specified dictionary among the mount list */
int
RkRemountDic(cx_num, name, mode)
int	cx_num;		/* context specified */
char	*name;		/* the name of dictonary */
int	mode;		/* mount mode */
{
    struct RkContext	*cx = RkGetContext(cx_num);
    int			i;

    if ( cx ) {
	for ( i = 0; i < 4; i++ ) {
	    struct MD	*mh = cx->mount[i];
	    struct MD	*md, *pd;

	/* do in reverse order */
	    for ( md = mh->md_prev; md != mh; md = pd ) {
		struct DM	*dm = md->md_dic;

		pd = md->md_prev;
		if ( !STRCMP(dm->dm_nickname, name) ) {
		/* remove from mount list */
		    md->md_prev->md_next = md->md_next;
		    md->md_next->md_prev = md->md_prev;
		/* insert according to the mode */
		    if ( !mode ) {    /* sentou he */
			md->md_next = mh->md_next;
			md->md_prev = mh;
			mh->md_next->md_prev = md;
			mh->md_next = md;
		    }
		    else {          /* saigo he */
			md->md_next = mh;
			md->md_prev = mh->md_prev;
			mh->md_prev->md_next = md;
			mh->md_prev = md;
		    };
		};
	    };
	};
	return 0;
    };
    return -1;
}
/*
 *
 */
/* RkGetDicList: collects the names of the mounted dictionaies */
int
RkGetMountList(cx_num, mdname, maxmdname)
int	cx_num;
char	*mdname;
int	maxmdname;
{
    struct RkContext	*cx = RkGetContext(cx_num);
    struct MD		*mh, *md;
    int			p, i, j;
    int			count = -1;

    if ( cx ) {
	i = count = 0;
	for ( p = 0; p < 4; p++ ) {
	    mh = cx->mount[p];
	    for ( md = mh->md_next; md != mh; md = md->md_next ) {
		struct DM	*dm = md->md_dic;

		if ( md->md_flags &(MD_MPEND|MD_UPEND) ) {
		    /*
		    (void)printf("PEND %d %s\n", md->md_flags, dm->dm_nickname);
		    */
		    continue;
		};
		j = i + strlen(dm->dm_nickname) + 1;
		if ( j + 1 < maxmdname ) {
		    (void)strcpy(mdname + i, dm->dm_nickname);
		    i = j;
		    count++;
		};
	    };
	};
	if ( i + 1 < maxmdname )
	    mdname[i++] = 0;
    };
    return count;
}
/* RkGetDicList: collects the names of dictionary */
int
RkGetDicList(cx_num, mdname, maxmdname)
int	cx_num;
char	*mdname;
int	maxmdname;
{
    struct RkContext	*cx = RkGetContext(cx_num);
    struct DD   	**ddp, *dd;
    struct DF   	*df, *fh;
    struct DM  		*dm, *mh;
    int			p, i, j;
    int			count = -1;

    if ( cx && (ddp = cx->ddpath) ) {
	i = count = 0;
	for ( p = 0; dd = ddp[p]; p++ ) {
	    fh = &dd->dd_files;
	    for ( df = fh->df_next; df != fh; df = df->df_next ) {
		mh = &df->df_members;
		for ( dm = mh->dm_next; dm != mh; dm = dm->dm_next ) {
		    j = i + strlen(dm->dm_nickname) + 1;
		    if ( j + 1 < maxmdname ) {
			(void)strcpy(mdname + i, dm->dm_nickname);
			i = j;
			count++;
		    };
		};
	    };
	};
	if ( i + 1 < maxmdname )
	    mdname[i++] = 0;
    };
    return count;
}

/* RkGetDirList: collects the names of directories */
int
RkGetDirList(cx_num, ddname, maxddname)
int	cx_num;
char	*ddname;
int	maxddname;
{
    struct RkContext	*cx = RkGetContext(cx_num);
    struct DD   	**ddp, *dd;
    int			p, i, j;
    int			count = -1;

    if ( cx && (ddp = cx->ddpath) ) {
	i = count = 0;
	for ( p = 0; dd = ddp[p]; p++ ) {
	    j = i + strlen(dd->dd_name) + 1;
	    if ( j + 1 < maxddname ) {
		(void)strcpy(ddname + i, dd->dd_name);
		i = j;
		count++;
	    };
	};
	if ( i + 1 < maxddname )
	    ddname[i++] = 0;
    };
    return count;
}
/* RkDefineDic
 *	mount the dictionary onto the specified context.
 */
int
RkDefineDic(cx_num, name, word)
int	cx_num;
char	*name;
char	*word;
{
    struct RkContext	*cx = RkGetContext(cx_num);

    if ( cx ) {
	struct DM	*dm = searchDDP(cx->ddpath, name);

	if ( dm ) 
	    return DST_CTL(dm, DST_DoDefine, word);
    };
    return -1;
}
/* RkDeleteDic
 *	mount the dictionary onto the specified context.
 */
int
RkDeleteDic(cx_num, name, word)
int	cx_num;
char	*name;
char	*word;
{
    struct RkContext	*cx = RkGetContext(cx_num);

    if ( cx ) {
	struct DM	*dm = searchDDP(cx->ddpath, name);

	if ( dm ) 
	    return DST_CTL(dm, DST_DoDelete, word);
    };
    return -1;
}
/* RkPrintDic
 */
int
RkPrintDic(cx_num, name)
int	cx_num;
char	*name;
{
    struct RkContext	*cx = RkGetContext(cx_num);

    if ( cx ) {
	struct DM	*dm = searchDDP(cx->ddpath, name);

	if ( dm ) 
	    return DST_CTL(dm, DST_DoPrint, 0);
    };
    return -1;
}
