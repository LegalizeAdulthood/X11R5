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
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/lib/RK/RCS/RKtempdic.c,v 1.4 91/05/21 19:33:29 mako Exp $";
#endif
/*LINTLIBRARY*/

#include	<stdio.h>
#include	"RK.h"
#include	"RKintern.h"

/* Extension Data in DM */
struct TD {
    struct TN   *td_node;	/* array of node TN */
    unsigned	td_n;		/* # effective nodes */
    unsigned	td_max;		/* # maximum nodes in the array */
};

struct TN {
    unsigned short	tn_flags;	/* type of node (see blow) */
    unsigned short	tn_key;
    union {
	struct TD	*tree;
	unsigned char	*word;
    }	tn_value;				/* type specific data */
/* syntax sugar */
#define tn_tree		tn_value.tree
#define tn_word		tn_value.word
};
/* flags  values */
#define	TN_WORD		1		/* is a word */
#define	TN_WDEF		2		/* wrec has been defined */
#define	TN_WDEL		4		/* wrec has been deleted */

#define	IsWordNode(tn)	((tn)->tn_flags&TN_WORD)

/*
 * PRINT
 */
static
void
printTD(tab, n)
struct TD	*tab;
{
    register struct TN	*tn = tab->td_node;
    int		i, j, k;
    
    for ( i = 0; i < tab->td_n; i++, tn++ ) {
	for ( j = n; j ; j-- )	(void)printf("  ");
	(void)printf("%08x:", tn->tn_key);
	if ( IsWordNode(tn) ) {
	    struct RkWcand 	 wc[NW_MAXCAND], *w;
	    int			nc;

	    (void)printf("%s\t", tn->tn_word);
	    nc = RkScanWcand(tn->tn_word, wc);
	    for ( j = 0; j < nc; j++ ) {
		w = &wc[j];
		(void)printf("  #%d#%d", w->rnum, w->cnum);
		(void)putchar('(');
		for ( k = 0; k < w->klen; k++ )
		    (void)putchar(w->addr[NW_PREFIX + k]);
		(void)putchar(')');
	    };

	    if ( tn->tn_flags & TN_WDEF ) (void)printf("\tDEFINED");
	    if ( tn->tn_flags & TN_WDEL ) (void)printf("\tDELETED");
	    (void)putchar('\n');
	}
	else {
	    (void)printf("\n");
	    printTD(tn->tn_tree, n + 1);
	};
    };
}

/* newTD: allocates a fresh node */

struct TD	*
newTD()
{
    register struct TD	*td;
    
    if ( td = (struct TD *)malloc(sizeof(struct TD)) ) {
	td->td_n    = 0;
	td->td_max  = 1;
	td->td_node  = (struct TN *)calloc(td->td_max, sizeof(struct TN));
	if ( !td->td_node ) {
	    (void)free((char *)td);
	    td = (struct TD *)0;
	};
    };
    return td;
}
/*
 * INSERT
 */
static
struct TN	*
extendTD(tdic, key, wrec)
struct TD		*tdic;
unsigned		key;
unsigned char		*wrec;
{
    int			i, j;
    struct TN		*tp;
    unsigned char	*wp;
    
    if ( !(wp = RkCopyWrec(wrec)) )
	return (struct TN *)0;

/* relocate the contents if neccesary */
    tp = tdic->td_node;
    if ( tdic->td_n >= tdic->td_max ) {
	tp = (struct TN *)calloc(tdic->td_max + 1, sizeof(struct TN));
	if ( !tp ) 
	    return (struct TN *)0;
	for ( i = 0; i < tdic->td_n; i++ )
	    tp[i] = tdic->td_node[i];
	(void)free((char *)tdic->td_node);
	tdic->td_max++;
	tdic->td_node = tp;
    };
/* find where the key is */
    for ( i = 0; i < tdic->td_n; i++ ) 
	if ( key < tp[i].tn_key )
	    break;
/* create single hole */
    for ( j = tdic->td_n; j > i; j-- )
	tp[j] = tp[j - 1];
/* teigi */
    tp[i].tn_flags = TN_WORD|TN_WDEF;
    tp[i].tn_key = key;
    tp[i].tn_word = wp;
    tdic->td_n++;
    return tp + i;
}

static
struct TN	*
defineTD(dm, tab, n, newW)
struct DM		*dm;
struct TD		*tab;
int			n;
unsigned char		*newW;
{
    int 	i;
    unsigned	key;
    struct TN	*tn;

/* tugino moji wo yomikomu */
    key = newW[n++];
    if ( key & 0x80 ) {
	key <<= 8;
	key |= newW[n++];
    };
    tn = tab->td_node;
    for ( i = 0; i < tab->td_n && tn->tn_key <= key; i++, tn++ ) {
	if ( key == tn->tn_key  ) {
	    if ( IsWordNode(tn) ) {
		struct TD	*td;
		unsigned char	*oldW = tn->tn_word;

		if ( key == 0 ||
		     !strcmp((char *)newW+n, (char *)oldW+n) ) { /* merge 2 */
		    struct ncache	*cache = _RkFindCache(dm, oldW);

		    if ( !cache || cache->nc_count <= 0 ) {
			unsigned char	*mergeW = RkUnionWrec(newW, oldW);
			
			if ( mergeW ) {
			    (void)free((char *)oldW);
			    tn->tn_word = mergeW;
			    tn->tn_flags |= TN_WDEF;
			    if ( cache ) {
				_RkRehashCache(cache, mergeW);
				cache->nc_word = mergeW;
			    };
			    return tn;
			}
		    };
		    return (struct TN *)0;
		};
	    /* extend the branch */
		td = newTD();
		if ( !td )
		    return (struct TN *)0;
		td->td_n = 1;
		key = oldW[n + 0];
		if ( key & 0x80 ) {
		    key <<= 8;
		    key |= oldW[n + 1];
		};
		td->td_node[0].tn_key = key;
		td->td_node[0].tn_flags |= TN_WORD;
		td->td_node[0].tn_word = oldW;
		tn->tn_flags &= ~TN_WORD;
		tn->tn_tree = td;
	    };
	    return defineTD(dm, tn->tn_tree, n, newW);
	};
    };
/* sinnki */
    return extendTD(tab, key, newW);
}
static
int	
enterTD(dm, td, gram, word)
struct DM	*dm;
struct TD	*td;
struct RkKxGram	*gram;
unsigned char	*word;
{
    unsigned char	wrec[1024];
    
    if ( word[0] != '#' ) {
	if ( RkParseWrec(gram, word, wrec, 0) ) {
	    struct TN	*tn = defineTD(dm, td, 0, wrec);

   	    if ( tn ) 
		tn->tn_flags &= ~(TN_WDEF|TN_WDEL);
	    return tn ? 0 : -1;
	};
    };
    return -1;
}

/*
 * DELETE 
 */
static
shrinkTD(td, key)
struct TD	*td;
unsigned	key;
{
    int		i;
    struct TN	*tn = td->td_node;

/* sounyuu ichi wo motomeru */
    for ( i = 0; i < td->td_n; i++ ) 
	if ( key == tn[i].tn_key ) {
	    while ( ++i < td->td_n ) tn[i - 1] = tn[i];
	    td->td_n--;
	    break;
	};
}
static
int
deleteTD(dm, tab, n, newW)
struct DM	*dm;
struct TD	**tab;
int		n;
unsigned char	*newW;
{
    struct TD	*td = *tab;
    int		i;
    unsigned	key;
    
    key = newW[n++];
    if ( key & 0x80 ) {
	key <<= 8;
	key |= newW[n++];
    };
    for ( i = 0; i < td->td_n; i++ ) {
	register struct TN	*tn = &td->td_node[i];
	
	if ( key == tn->tn_key  ) {
	    if ( IsWordNode(tn) ) {
		unsigned char	*oldW = tn->tn_word;

		if ( key == 0 ||
		     !strcmp((char *)newW+n, (char *)oldW+n) ) {
		    struct ncache	*cache = _RkFindCache(dm, oldW);

		/* delete word if and only if the cache is free */
		    if ( !cache || cache->nc_count <= 0 ) {
			unsigned char	*subW = RkSubtractWrec(oldW, newW);

			(void)free((char *)oldW);
			if ( subW ) {	/* some candidates are left */
			    tn->tn_word = subW;
			    tn->tn_flags |= TN_WDEL;
			    if ( cache ) {
				_RkRehashCache(cache, subW);
				cache->nc_word = subW;
			    };
			    return 0;
			}
			else {		 /*  this node is useless */
			    if ( cache ) 
				_RkPurgeCache(cache);
			    shrinkTD(td, key);
			};
		    };
		};
	    }
	    else {
		if ( deleteTD(dm, &tn->tn_tree, n, newW) )
		    shrinkTD(td, key);
	    };
	/* if all child has removed, */
	    if ( td->td_n <= 0 ) {
		(void)free((char *)td->td_node);
		(void)free((char *)td);
		*tab = (struct TD *)0;
		return 1;	/* signal to the caller */
	    }
	    else
		return 0;
	};
    };
/* not found */
    return 0;
}
/* 
 * OPEN
 */
/*ARGSUSED*/
int	_Rktopen(dm, file, mode)
struct DM	*dm;
char		*file;
int		mode;
{
    struct DF		*df = dm->dm_file;
    struct DD		*dd = df->df_direct;
    struct TD		*xdm;
    FILE		*f;
    char		line[MAX_TEXT_LEN];

/* saisho no dm wo open suru toki, df mo shokika suru */
    if ( df->df_rcount == 0 ) {
	f = fopen(file, "r");
	if ( !f ) 
	    return -1;
	/*
	(void)printf("TEMP OPEN FILE %s\n", file);
	*/
	df->df_flags |= DF_EXIST;
	dm->dm_flags |= DM_EXIST;
	if ( !close(open(file, 2)) )
	    dm->dm_flags |= DM_WRITABLE;
	dd->dd_rcount++;
    };
    if ( !(dm->dm_flags & DM_EXIST) ) 
	return -1;
    df->df_rcount++;

/* read words from file */
    xdm = newTD();
    dm->dm_extdata = (char *)xdm;
    while ( fgets((char *)line, sizeof(line), f) ) 
	(void)enterTD(dm, xdm, SX.gramdic, (unsigned char *)line);
    (void)fclose(f);
/* lock file wo tukuru */
    /*
    (void)printf("TEMP OPEN %s\n", dm->dm_nickname);
    */
    return 0;
}
/*
 * CLOSE
 */
static
void
writeTD(td, gram, fp)
struct TD	*td;
struct RkKxGram	*gram;
FILE		*fp;
{
    int	i;

    for ( i = 0; i < td->td_n; i++ ) {
	struct TN	*tn = &td->td_node[i];
	unsigned char	line[MAX_TEXT_LEN];

	if ( IsWordNode(tn) ) {
	    if ( RkUparseWrec(gram, tn->tn_word, line, 0) ) {
	    /*
		switch(tn->tn_flags&(TN_WDEF|TN_WDEL)) {
		case TN_WDEF:
			(void)fprintf(fp, "#*DEFINED\n"); break;
		case TN_WDEL:
			(void)fprintf(fp, "#*DELETED\n"); break;
		case TN_WDEF|TN_WDEL:
			(void)fprintf(fp, "#*MODIFIED\n"); break;
		};
	    */
		(void)fprintf(fp, "%s\n", line);
	    };
	}
	else 
	    writeTD(tn->tn_tree, gram, fp);
    };
}

void
freeTD(td)
struct TD	*td;
{
    int	i;
    
    for ( i = 0; i < td->td_n; i++ ) {
	struct TN	*tn = &td->td_node[i];
	
	if ( IsWordNode(tn) ) 
	    (void)free((char *)tn->tn_word);
	else
	    freeTD(tn->tn_tree);
    };
    (void)free((char *)td->td_node);
    (void)free((char *)td);
}
int	
_Rktclose(dm, file)
struct DM	*dm;
char		*file;
{
    struct DF		*df = dm->dm_file;
    struct TD		*xdm = (struct TD *)dm->dm_extdata;
    FILE		*fp;

    /*
    (void)printf("TEMPDIC CLOSE %s(%s)\n", file, dm->dm_nickname);
    */
/* cache wo kaihou suru */
    _RkKillCache(dm);
#ifdef RK_PUT_ASIDE
/* save old file */
    if ( !access(file, 0) ) {
	char		oldfile[1024], *t;

	t = file + strlen(file) - 1;
	while ( t > file && *t != '/' )
		t--;
	if ( t > file ) {
	    t++;
	    strncpy(oldfile, file, t - file);
	    oldfile[t - file] = 0;
	}
	else
	    strcpy(oldfile, "./");
	strcat(oldfile, "#");
	strcat(oldfile, t);
	unlink(oldfile);
	link(file, oldfile);
	unlink(file);
    };
#endif
/* save the contents of the dictionary */
    if ( fp = fopen(file, "w") ) {
    /* prologue */
	(void)fprintf(fp, "#*DIC %s\n", dm->dm_nickname);
	writeTD(xdm, SX.gramdic, fp);
	(void)fclose(fp);
    };
    freeTD((struct TD *)xdm);
    --df->df_rcount;
    return 0;
}

/*
 * SEARCH
 */
int 
_Rktsearch(dm, key, n, ncache, maxcache)
struct 	DM	*dm;
unsigned char	*key;
int		n;
struct ncache	*ncache[];
int		maxcache;
{
    struct TD	*xdm = (struct TD *)dm->dm_extdata;
    int		nc = 0;
    int		i, j;
    
    for ( j = 0; j < n; ) {
	unsigned	k = key[j++];
	struct TN	*tn;
	

	if ( k & 0x80 ) {
	    k <<= 8;
	    k |= key[j++];
	};
	tn = xdm->td_node;
	for ( i = 0; i < xdm->td_n && tn->tn_key <= k; i++, tn++ ) {

	    if ( k == tn->tn_key || (k == 0xa1bc && tn->tn_key == '-')
		|| ((k & 0xff00) == 0xa300 && (k & 0x007f) == tn->tn_key) ) {
		if ( IsWordNode(tn) ) {
		    if ( nc < maxcache ) {
			ncache[nc] = _RkReadCache(dm, (long)tn->tn_word);
			if ( ncache[nc] ) 
			    nc++;
		    };
		    return nc;
		}
		else {
		    struct TD	*ct = tn->tn_tree;
		    struct TN	*n0 = &ct->td_node[0];

		    if ( ct->td_n && !n0->tn_key ) {
			if ( nc < maxcache) {
			    ncache[nc] = _RkReadCache(dm, (long)n0->tn_word);
			    if ( ncache[nc] ) 
				nc++;
			};
		    };
		    xdm = ct;
		    goto	cont;
		};
	    };
	};
	break;
    cont:;
    };
    return nc;
}
/*
 * IO
 */
/*ARGSUSED*/
int	
_Rktio(dm, cp, io)
struct DM	*dm;
struct ncache	*cp;
int		io;
{
    if ( !cp )
	return -1;
    if ( io == 0 ) {
	cp->nc_word = (unsigned char *)cp->nc_address;
	cp->nc_flags |= NC_NHEAP;
    };
    return 0;
}
/*
 * CTL
 */
int	
_Rktctl(dm, what, arg)
struct DM	*dm;
int		what;
int		arg;
{
    struct TD	*xdm = (struct TD *)dm->dm_extdata;
    int		status = 0;
    unsigned char	wrec[2048];

    switch(what) {
    case DST_DoDefine:
	if ( !(dm->dm_flags&DM_WRITABLE) )
	    status = -1;
	else
	if ( !RkParseWrec(SX.gramdic, (unsigned char *)arg, wrec, 0) )
	    status = -1;
	else {
	    status = defineTD(dm, xdm, 0, wrec) ? 0 : -1;
	};
	break;
    case DST_DoDelete:
	if ( !(dm->dm_flags&DM_WRITABLE) )
	    status = -1;
	else
	if ( !RkParseWrec(SX.gramdic, (unsigned char *)arg, wrec, 0) )
	    status = -1;
	else
	if ( deleteTD(dm, &xdm, 0, wrec) ) {
	    xdm = newTD();
	    dm->dm_extdata = (char *)xdm;
	};
	break;
    case DST_DoPrint:		/* debug purpose only */
	printTD(xdm, 0);
	break;
    };
    return status;
}
