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
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/lib/RK/RCS/RKpermdic.c,v 1.3 91/05/21 19:01:19 mako Exp $";
#endif
/*LINTLIBRARY*/

/* permdic.c
 *	hyoujunn jisho kansuu
 */
#include	"RK.h"
#include	"RKintern.h"

/* perm
 *	PERMDIC jouhou
 */

/*  Extension Data in DM */
struct xdm {
    unsigned		ubyte;
    long		dispoff, dispsiz;
    long		wordoff, wordsiz;
    long		gramoff, gramsiz;
    unsigned char	*dispptr;
    unsigned char	*wordptr;
    unsigned long	lastpage, pageio, realio;
};
/* Extension Data in DF */


/*
 * OPEN
 */
static
int	
openDF(df, file)
struct DF	*df;
char		*file;
{
    int			fdes;
    long		off;
    long		dispoff, dispsiz;
    long		wordoff, wordsiz;
    long		gramoff, gramsiz;
    unsigned char	hd[ND_HDRSIZ];
    unsigned char	l4[4], *h;
    struct xdm		*xdm;
    int			count = 0;
    
/* jisho ga aru ? */
    if ( (fdes = open(file, 2)) == -1 ) {
	if ( (fdes = open(file, 0)) == -1 ) 
	    return -1;
    }
    else
	df->df_flags |= DF_WRITABLE;
    df->df_flags |= DF_EXIST;

    /*
    (void)printf("PERM OPEN FILE %d\n", fdes);
     */
/* subete wo mount suru */
    off = 0;
    while  ( read(fdes, (char *)hd, ND_HDRSIZ) == ND_HDRSIZ ) {
	struct DM		*dm, *dmh;
	
	off += ND_HDRSIZ;
	if ( S2TOS(hd) != ND_HDRMAG )
	    break;
    /* namae wo toru */
	for ( h = hd + 3; *h != '\n'; h++ );
	*h = 0;
    /* direct */
	(void)read(fdes, (char *)l4, 4);
	dispsiz = L4TOL(l4);	dispoff = (off += 4); off += dispsiz;
	(void)lseek(fdes, off, 0);
    /* word */
	(void)read(fdes, (char *)l4, 4);
	wordsiz = L4TOL(l4);	wordoff = (off += 4); off += wordsiz;
	(void)lseek(fdes, off, 0);
    /* gram */
	if ( !strcmp(".swd", (char *)(h - 4)) ) {
	    (void)read(fdes, (char *)l4, 4);
	    gramsiz = L4TOL(l4); gramoff = (off += 4);  off += gramsiz;
	    (void)lseek(fdes, off, 0);
	};
	/*
	(void)printf("PERM OPEN SCAN %s(%s)\n", file, hd + 3); 
	*/
    /* jiso wo sagasu */
	dmh = &df->df_members;
	for ( dm = dmh->dm_next; dm != dmh; dm = dm->dm_next )
	    if ( !strcmp(dm->dm_dicname, hd + 3) ) {	/* found */
		dm->dm_flags |= DM_EXIST;
		dm->dm_packbyte = hd[2];
		xdm = (struct xdm *)malloc(sizeof(struct xdm));
		if ( !xdm )
		    break;
		dm->dm_extdata = (char *)xdm;
		xdm->ubyte   = hd[2];
		xdm->dispoff = dispoff; xdm->dispsiz = dispsiz;
		xdm->wordoff = wordoff; xdm->wordsiz = wordsiz;
		xdm->gramoff = gramoff; xdm->gramsiz = gramsiz;
		xdm->dispptr = (unsigned char *)0;
		xdm->wordptr = (unsigned char *)0;
		xdm->lastpage = 0;
		xdm->pageio = xdm->realio  = 0;
		count++;
		break;
	    };
    };
    if ( !count ) {
	(void)close(fdes);
	return -1;
    };
    df->df_extdata = (char *)fdes;
    return 0;
}
int	
_Rkpopen(dm, file, mode)
struct DM	*dm;
char		*file;
int		mode;
{
    struct DF		*df = dm->dm_file;
    struct DD		*dd = df->df_direct;
    struct xdm		*xdm;
    int			fd;

/* saisho no dm wo open suru toki, df mo shokika suru */
    if ( df->df_rcount == 0 ) {
	if ( openDF(df, file) ) 
	    return -1;
	fd = (int)df->df_extdata;
	dd->dd_rcount++;
	df->df_extdata = (char *)fd;
    };
    if ( !(dm->dm_flags & DM_EXIST) ) 
	return -1;
    df->df_rcount++;
    xdm = (struct xdm *)dm->dm_extdata;
    fd = (int)df->df_extdata;
    /*
    (void)printf("PERM OPEN %s D %d#%d W %d#%d\n", 
    dm->dm_nickname, xdm->dispoff, xdm->dispsiz, xdm->wordoff, xdm->wordsiz);  
    */

/* dispatch */
    (void)lseek(fd, xdm->dispoff, 0);
    xdm->dispptr = (unsigned char *)malloc((unsigned)xdm->dispsiz);
    if ( !xdm->dispptr )
	return -1;
    (void)read(fd, (char *)xdm->dispptr, (int)xdm->dispsiz);
/* word record */
    xdm->wordptr = (unsigned char *)0;
    mode |= DM_LOADWORD;
    if ( ( mode&DM_LOADWORD) && (dm->dm_class != ND_MWD) ) {
	(void)lseek(fd, xdm->wordoff, 0);
	xdm->wordptr = (unsigned char *)malloc((unsigned)xdm->wordsiz);
	if ( xdm->wordptr ) 
	    (void)read(fd, (char *)xdm->wordptr, (int)xdm->wordsiz);
    };
/* swd ga open sarereba, fuzokugo mo yomikomu */
    if ( dm->dm_class == ND_SWD ) {
	(void)lseek(fd, xdm->gramoff - 4, 0);
	dm->dm_gram = RkReadGram(fd);
    };
/* */
    if ( (mode&DM_WRITABLE) && (df->df_flags&DF_WRITABLE) )
	dm->dm_flags |= DM_WRITABLE;
    return 0;
}

/*
 * CLOSE
 */
/*ARGSUSED*/
int	
_Rkpclose(dm, file)
struct DM	*dm;
char		*file;
{
    struct DF		*df = dm->dm_file;
    struct xdm		*xdm = (struct xdm *)dm->dm_extdata;

/* cache wo kaihou suru */
    _RkKillCache(dm);
/* free memories */
    if ( dm->dm_gram )
	(void)RkCloseGram(dm->dm_gram);
    if ( xdm->wordptr )
	(void)free((char *)xdm->wordptr);
    if ( xdm->dispptr )
	(void)free((char *)xdm->dispptr);

    if ( --df->df_rcount == 0 )  {
	int		fdes = (int)df->df_extdata;
	struct DM	*dmh, *dm;

	(void)close(fdes);
	dmh = &df->df_members;
	for ( dm = dmh->dm_next; dm != dmh; dm = dm->dm_next ) {
	    xdm = (struct xdm *)dm->dm_extdata;
	    if ( xdm ) {	/* BUG 77. MS */
		RkDebug("PAGEIO <%s> %d/%d",
		    dm->dm_nickname, xdm->pageio, xdm->realio);
		(void)free((char *)xdm);
		dm->dm_extdata = (char *)0;
	    };
	};
    };
}
/* permsearch
 *	jisho kara key no prefix to itti suru word wo cache he yomikomu
 * note:
 *	[key, key + n) ha euc hiragana dake kara naru
 */
int		
_Rkpsearch(dm, key, n, ncache, maxcache)
struct DM	*dm;
unsigned char	*key;		/* yomigana */
int		n;		/* yomi no nagasa */
struct ncache	*ncache[];	/* cache heno pointer */
int		maxcache;	/* cache no saidai suu */
{
    struct xdm		*xdm = (struct xdm *)dm->dm_extdata;
    unsigned char	*k;
    unsigned char	*root;
    register long	node, off;
    int	i, dpt;
    int	count;
    int	nc;
    
    k = key;
    nc = 0;
    root = xdm->dispptr;
    for ( i = dpt = 0; i < n; dpt++ ) {
	register unsigned char	*next;
	unsigned char		Key;

	if ( dm->dm_packbyte ) {	/* pack sareta euc */
	    if ( (Key = *k++) & 0x80 ) {
		if ( Key == dm->dm_packbyte ) {
		    Key = *k++;
		    i += 2;
		}
		else	/* chouon wo '-' ni suru */
		if ( Key == 0xa1 && k[0] == 0xbc ) {
		    Key = '-'; k++;
		    i += 2;
		}
		else if ( Key == 0xa3 ) { /* Hankaku Suuji, by KON 1990.9.13 */
		  Key = (*k++ & 0x7f);
		  i += 2;
		}
		else
		    goto	end;
	    }
	    else  			/* ascii */
		i++;
	}
	else {
	    Key = *k++;
	    i++;
	};
/* root: dispatch no kaishi iti */
	root = xdm->dispptr;
	switch(dpt) {
	case	0:	/* 1 dan me: dispatch table niyoru hyoubiki */
	    next = root + ND_NODSIZ*Key;
	    node = readnode(next);
	    if ( (off = NOD2OFF(node)) == ND_NULLOFF )
		goto	end;
	    break;
	case	1:	/* 2 dan me: gozonji binary search */
	/* entry ga sukunai baai niha linear no houga hayai */
	    if ( (count = NOD2CNT(node)) > 8 ) {
		register long	s, e, m;

		s = off/ND_NODSIZ;
		e = s + count;
	    /* Key ga areba, [s, e) no aida ni kanarazu iru */
		for(;;) {
		/* m ni yori kukan wo bunkatu
		 * 	[s, e) = [s, m) + [m, m] + (m, e)
		 */
		    m = (s + e)>>1;
		    next = root + ND_NODSIZ*m;
		    node = readnode(next);
		/* in [s, m) ? */
		    if ( Key < NOD2KEY(node) ) {
			if ( e == m )
			    goto end;
			e = m;
		    }
		    else
		/* in (m, e) ? */
		    if ( Key > NOD2KEY(node) ) {
			if ( s == m )
			    goto end;
			s = m;
		    }
		/* just m  */
		    else  
			goto found;
		};
	    };
	default:	/* 3 dan me ikou: linear search */
	    next = root + off;
	    for (;;) {
		register unsigned char	kb;

		node = readnode(next);
		kb = NOD2KEY(node);
		if ( kb == Key ) goto found;
		if ( kb > Key || ISLASTNOD(node) ) goto end;
		next += ND_NODSIZ;

		node = readnode(next);
		kb = NOD2KEY(node);
		if ( kb == Key ) goto found;
		if ( kb > Key || ISLASTNOD(node) ) goto end;
		next += ND_NODSIZ;

		node = readnode(next);
		kb = NOD2KEY(node);
		if ( kb == Key ) goto found;
		if ( kb > Key || ISLASTNOD(node) ) goto end;
		next += ND_NODSIZ;

		node = readnode(next);
		kb = NOD2KEY(node);
		if ( kb == Key ) goto found;
		if ( kb > Key || ISLASTNOD(node) ) goto end;
		next += ND_NODSIZ;
	    };
	};
found:
	off = NOD2OFF(node);
	if ( ISWORDNOD(node) ) {		/* word wo sasu */
	    if ( nc < maxcache ) {
		ncache[nc] = _RkReadCache(dm, NOD2OFF(node));
		if ( ncache[nc] )
		    nc++;
	    };
	    goto	end;
	}
	else {				/* directory wo sasu */
	    long	ahead;

    /* tugi no node wo nozokimiru */
	    next = xdm->dispptr + off;
	    ahead = readnode(next);
	    if ( !NOD2KEY(ahead) )	/* kittemo yoi */
		if ( nc < maxcache ) {
		    ncache[nc] = _RkReadCache(dm, NOD2OFF(ahead));
		    if ( ncache[nc] )
			nc++;
		    else
			goto end;
		};
	};
    };
end:
    return(nc);
}
/*
 * IO
 */
int	
_Rkpio(dm, cp, mode)
struct DM	*dm;
struct ncache	*cp;
{
    struct xdm		*xdm = (struct xdm *)dm->dm_extdata;
    int			fdes = (int)dm->dm_file->df_extdata;

    if ( mode == 0 ) {	/* READ */
    /* chokusetu sasu */
	if ( xdm->wordptr ) {
	    cp->nc_word = xdm->wordptr + cp->nc_address;
	    cp->nc_flags |= NC_NHEAP;
	}
	else {
	    unsigned char	wrec[NW_MAXWREC];
	    unsigned	sz;

	    (void)lseek(fdes, xdm->wordoff + cp->nc_address, 0);
	    (void)read(fdes, (char *)wrec, NW_MAXWREC);
	    sz = RkGetWrecSize(wrec);
	    if ( cp->nc_word = _RkNewHeap(sz) ) {
		register unsigned char	*w, *c;

		c = cp->nc_word;
		w = wrec;
		while ( sz-- ) *c++ = *w++;
	    }
	    else {
		cp->nc_flags |= NC_ERROR;
		return -1;
	    };
	};
    }
    else {		/* WRITE */
	if ( 1 || !xdm->wordptr ) {
	    (void)lseek(fdes, xdm->wordoff + cp->nc_address, 0);
	    (void)write(fdes, (char *)cp->nc_word, RkGetWrecSize(cp->nc_word));
	};
    };

    if ( (cp->nc_address>>12) != xdm->lastpage ) {
	xdm->lastpage = cp->nc_address>>12;
	xdm->pageio++;
    };
    xdm->realio++;

    return 0;
}
/*
 * CTL
 */
/*ARGSUSED*/
int	
_Rkpctl(dm, what, arg)
struct DM	*dm;
int		what;
{
    return -1;
}
