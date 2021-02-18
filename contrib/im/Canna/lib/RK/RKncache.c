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
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/lib/RK/RCS/RKncache.c,v 1.2 91/05/21 18:44:26 mako Exp $";
#endif
/* LINTLIBRARY */
#include	"RK.h"
#include	"RKintern.h"

#define	NCHASH		101
#define	hash(x)		((int)((x)%NCHASH))

/* hash list header */
static struct ncache	Nchash[NCHASH];
/* free list
 *	free list niha siyou kanou na cache ga tunagareteiru.
 *	nc_count ha tuneni 0 tonaru.
 *	cache ga hituyou to natta baai, tuneni sentou kara torareru.
 *
 *	cache ga fuyou to natta baai, hennkyaku iti ha sentou ka saigo dearu.
 *	sentou ni kaesareru baai ha jisho wo close sita toki nado de,
 *	yuusenn teki ni sai siyou sareru.
 *
 *	saigo ni kaesareru baai ha cache ha mada hash list ni tunagatteiru.
 *	cache no saisiyou ha kanou na siyou hinndo ga takaimono hodo 
 *	ushiro he idou suru.
 */
static struct ncache	Ncfree;

#define	aremove(p)	{\
p->nc_anext->nc_aprev = p->nc_aprev; p->nc_aprev->nc_anext = p->nc_anext;\
p->nc_anext = p->nc_aprev = p;\
}

#define	hremove(p)	{\
p->nc_hnext->nc_hprev = p->nc_hprev; p->nc_hprev->nc_hnext = p->nc_hnext;\
p->nc_hnext = p->nc_hprev = p;\
}

#ifdef RK_DEBUG
static
PrintCache(msg, c)
char		*msg;
struct ncache	*c;
{
    struct DM	*dm = c->nc_dic;
    char	fmt[1024];

    strcpy(fmt, "C#");
    strcat(fmt, msg);
    strcpy(fmt, ": <%s> %d#%d");
    RkDebug(fmt, dm ? dm->dm_dicname : "-", c->nc_address, c->nc_count);
}
#else
#define PrintCache(msg, c)	
#endif

/* _RkInitCache
 *	cache/free list wo shokika suru
 */
int	
_RkInitializeCache(size)
int	size;
{
    register struct RkParam	*sx = &SX;
    int				i;

/* free list wo sakusei suru */
    sx->maxcache = size;
    sx->cache = (struct ncache *)calloc((unsigned)size, sizeof(struct ncache));
    if ( !sx->cache ) 
	return -1;
    for ( i = 0; i < sx->maxcache; i++ ) {
	sx->cache[i].nc_anext = &sx->cache[i + 1];
	sx->cache[i].nc_aprev = &sx->cache[i - 1];
	sx->cache[i].nc_hnext = sx->cache[i].nc_hprev = &sx->cache[i];
	sx->cache[i].nc_count = 0;
    };
    Ncfree.nc_anext = &sx->cache[0];
    sx->cache[sx->maxcache - 1].nc_anext = &Ncfree;
    Ncfree.nc_aprev = &sx->cache[sx->maxcache - 1];
    sx->cache[0].nc_aprev = &Ncfree;
/* cache head wo kuu ni settei */
    for ( i = 0; i < NCHASH; i++ ) 
	Nchash[i].nc_hnext = Nchash[i].nc_hprev = &Nchash[i];
    return 0;
}
/* _RkFinalizeCache
 */
void
_RkFinalizeCache()
{
    register struct RkParam	*sx = &SX;

    if ( sx->cache ) 
	(void)free((char *)sx->cache);
    sx->cache = (struct ncache *)0;
}

/* flushCache 
 *	cache wo jisho he kakimodosu
 */
static
int	flushCache(dm, cache)
struct DM	*dm;
struct ncache	*cache;
{
    if ( cache->nc_word ) {
    /* jisho wo kousinn suru */
	if ( dm && (cache->nc_flags&NC_DIRTY) ) {
	    PrintCache("FLUSH", cache);
	    DST_WRITE(dm, cache);
	};
	cache->nc_flags &= ~NC_DIRTY;
	return 0;
    };
    return -1;
}
/* newCache
 *	free list no sentou kara cache wo hazusite wariateru
 *	cache ha hash/free izureno list nimo zokusanai 
 */
static
struct ncache	*newCache(ndm, address)
register struct DM	*ndm;
register long		address;
{
    register struct ncache	*new;

/* get a cache from the front of the free list */
    if ( (new = Ncfree.nc_anext) != &Ncfree ) {
	PrintCache("NEW_CACHE",  new);
    /* kousinn sareteireba jisho he kakikomu */
	(void)flushCache(new->nc_dic, new);
    /* heap wo kaihou suru */
	if ( new->nc_word && !(new->nc_flags&NC_NHEAP) )
	    _RkFreeHeap(new->nc_word);
    /* hash/free list kara hazusu */
	aremove(new);
	hremove(new);
    /* shokisetteiwo okonau */
	new->nc_dic = ndm;
	new->nc_utime  = 0;
	new->nc_word = (unsigned char *)0;
	new->nc_flags  = 0;
	new->nc_address = address;
	new->nc_count = 0;
	return(new);
    };
    return (struct ncache *)0;
}

/* RkRelease
 *   free list ni iru cache no heap ryouiki wo kaihou suru.
 *   cache sonomono ha free list ni iru 
 */
_RkRelease()
{
    register struct ncache	*new;

/* get a cache slot at the front of the free list */
    for ( new = Ncfree.nc_anext; new != &Ncfree; new = new->nc_anext ) {
	if ( !new->nc_word || (new->nc_flags&NC_NHEAP) )
	    continue;
	PrintCache("RELEASE", new);
    /* kousinn sareteireba jisho he kakikomu */
	(void)flushCache(new->nc_dic, new);
    /* heap wo kaihou suru */
	_RkFreeHeap(new->nc_word);
    /* hash list kara hazusu */
	hremove(new);
    /* shokisetteiwo okonau */
	new->nc_dic = (struct DM *)0;
	new->nc_utime  = 0;
	new->nc_flags  = 0;
	new->nc_word = (unsigned char *)0;
	new->nc_address = 0;
	new->nc_count = 0;
	return 1;
    };
    return 0;
}


/* _RkEnrefCache
 *	cache no reference counter wo kousin suru 
 */
int
_RkEnrefCache(cache)
struct ncache *cache;
{
    cache->nc_count++;
    return cache->nc_count;
}

/* _RkDerefCache
 *	cache wo free list he tunageru
 *	ERROR no atta cache ha, free list no sentou he tunagerareru
 * NOTE:
 *	newCache niyori wariaterarerumade sono naiyou ha yuukou dearu.
 */
int
_RkDerefCache(cache)
struct ncache *cache;
{
    struct DM	*dm = cache->nc_dic;

    if ( cache->nc_count == 0) {
	_Rkpanic("wrong cache count %s %d#%d",
	    dm ? dm->dm_dicname : "-", cache->nc_address, cache->nc_count);
    };

    cache->nc_count--;
    if ( cache->nc_count == 0 ) {
	PrintCache("NOREF", cache);
/* avail list no saigo he idou suru */
	aremove(cache);
/* ERROR na cache ha sugu ni sairiyou sareru you, sentou ni modesu */
	if ( cache->nc_flags & NC_ERROR ) {
	    cache->nc_anext = Ncfree.nc_anext;
	    cache->nc_aprev = &Ncfree;
	    Ncfree.nc_anext->nc_aprev = cache;
	    Ncfree.nc_anext = cache;
	}
	else {				/* saigo he */
	    cache->nc_anext = &Ncfree;
	    cache->nc_aprev = Ncfree.nc_aprev;
	    Ncfree.nc_aprev->nc_anext = cache;
	    Ncfree.nc_aprev = cache;
	};
    };
    return cache->nc_count;
}

/* _RkPurgeCache
 *	cache wo kaihou suru 
 */
void	
_RkPurgeCache(cache)
struct ncache	*cache;
{
    PrintCache("KILL", cache);
/* hash list kara torinozoku */
    hremove(cache);
/* free list no sentou he tunageru */
    aremove(cache);
    cache->nc_anext = Ncfree.nc_anext;
    cache->nc_aprev = &Ncfree;
    Ncfree.nc_anext->nc_aprev = cache;
    Ncfree.nc_anext = cache;
}

/* _RkKillCache
 *	jisho no cache wo kaihousuru
 *	naiyou ga kousin sareteireba sync suru
 * NOTE:
 *	jisho table kara jisho ga torinozokareru toki nomi call sareru
 */
void	
_RkKillCache(dm)
struct DM	*dm;
{
    register struct RkParam	*sx = &SX;
    int		i;

/* jisho kara yomidasareta cache nituite ... */
    for ( i = 0; i < sx->maxcache; i++ ) {
	struct ncache	*cache = &sx->cache[i];

	if ( dm == cache->nc_dic ) {
	/* naiyou ga henkou sareta node kakimodosu */
	    (void)flushCache(dm, cache);
	/* hash list kara tori, free list no sentou he tunageru */
	    _RkPurgeCache(cache);
	};
    };
}
/* _RkFindCache
 *	hash list ni ireba, sore wo riyou suru.
 */
struct ncache	*
_RkFindCache(dm, addr)
struct DM	*dm;
long		addr;	/* jisho naide word wo shimesu uniq na atai */
{
    register struct ncache	*head, *cache;

/* hash list ni aruka siraberu. */
    head = &Nchash[hash(addr)];
    for ( cache = head->nc_hnext; cache != head; cache = cache->nc_hnext ) 
	if ( cache->nc_dic == dm && cache->nc_address == addr ) 
	    return cache;
    return (struct ncache *)0;
}

void
_RkRehashCache(cache, addr)
struct ncache	*cache;
long		addr;
{
    register struct ncache	*head;

/* hash list ni aruka siraberu. */
    head = &Nchash[hash(addr)];
    if ( head != &Nchash[hash(cache->nc_address)] ) {
    /* hash list no sentou he idou suru */
	hremove(cache);
	cache->nc_hnext = head->nc_hnext;
	cache->nc_hprev = head;
	head->nc_hnext->nc_hprev = cache;
	head->nc_hnext = cache;
    };
    cache->nc_address = addr;
}

/* _RkReadCache
 *	jisho kara tango record wo yomu.
 *	hash list ni ireba, sore wo riyou suru.
 */
struct ncache	*
_RkReadCache(dm, addr)
struct DM	*dm;
long		addr;	/* jisho naide word wo shimesu uniq na atai */
{
    register struct ncache	*head, *cache;

/* hash list ni aruka siraberu. */
    head = &Nchash[hash(addr)];
    for ( cache = head->nc_hnext; cache != head; cache = cache->nc_hnext ) {
	if ( cache->nc_dic == dm && cache->nc_address == addr ) {
	/* free list kara hazusu */
	    aremove(cache);
	/* hash list no sentou he idou suru */
	    hremove(cache);

	    cache->nc_hnext = head->nc_hnext;
	    cache->nc_hprev = head;
	    head->nc_hnext->nc_hprev = cache;
	    head->nc_hnext = cache;
	    (void)_RkEnrefCache(cache);
	    PrintCache("FOUND",  cache);
	    return(cache);
	};
    };
/* atarasiku cache wo sakusei */
    if ( cache = newCache(dm, addr) ) {
	if ( DST_READ(dm, cache) ) {	/* fail to read */
	    RkDebug("C#FAIL_READ: %s %d", dm->dm_dicname, cache->nc_address, 0);
	/* free list no sentou he modosu */
	    cache->nc_anext = Ncfree.nc_anext;
	    cache->nc_aprev = &Ncfree;
	    Ncfree.nc_anext->nc_aprev = cache;
	    Ncfree.nc_anext = cache;
	    return (struct ncache *)0;
	}
	else {
	/* hash list no sentou he tunageru */
	    cache->nc_hnext = head->nc_hnext;
	    cache->nc_hprev = head;
	    head->nc_hnext->nc_hprev = cache;
	    head->nc_hnext = cache;
	    (void)_RkEnrefCache(cache);
	    PrintCache("READ",  cache);
	    return(cache);
	};
    }
    else {
	RkDebug("C#FAIL_CACHE: %s %d", dm->dm_dicname, addr, 0);
	return (struct ncache *)0;
    };
}
