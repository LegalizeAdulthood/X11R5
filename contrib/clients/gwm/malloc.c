/* Copyright 1989-91 GROUPE BULL -- See licence conditions in file COPYRIGHT */
/*****************************************************************************\
*                                                                             *
* WOOL malloc:                                                                *
* A fast malloc, with optional:                                               *
* -DSTATS	stats                                                         *
* -DMLEAK	memory lossage tracing via hash coding                        *
* -DDEBUG	debugging information                                         *
* 	range checking (on_line and with a dbx call)                          *
* -DMALLOCDEBUG more info                                                     *
*                                                                             *
\*****************************************************************************/

#define WlO WOOL_OBJECT
#define WlError wool_error
#define WlAtom WOOL_Atom
#define WlPuts wool_puts
#define WlPrintf wool_printf
#define WlMallocInit malloc_init
#define WlMallocZoneBegin malloc_zone_begin
#define WlMallocZoneEnd malloc_zone_end
#define WlMstats mstats


#include "EXTERN.h"
#ifdef STATS
#include <stdio.h>
#else
#define	NULL 0
#endif
#include <sys/types.h>
#if defined(i386) && !defined(sequent) && !defined(SCO)
typedef unsigned char	u_char;
typedef unsigned int	u_int;
#endif /* i386 */
#include "wool.h"

#ifdef STANDARD_MALLOC
# ifndef DO_NOT_REDEFINE_MALLOC
# define DO_NOT_REDEFINE_MALLOC
# endif
# ifndef DO_NOT_USE_SBRK
# define DO_NOT_USE_SBRK
# endif
#endif

#ifndef DO_NOT_REDEFINE_MALLOC
#define REAL_malloc(nbytes)	malloc(nbytes)
#define REAL_free(ptr)		free(ptr)
#define REAL_realloc(ptr, nbytes) realloc(ptr, nbytes)
#define REAL_calloc(n, s)	calloc(n, s)
#else /* DO_NOT_REDEFINE_MALLOC */
#define REAL_malloc(nbytes)	wool_malloc(nbytes)
#define REAL_free(ptr)		wool_free(ptr)
#define REAL_realloc(ptr, nbytes) wool_realloc(ptr, nbytes)
#define REAL_calloc(n, s)	wool_calloc(n, s)
#endif /* DO_NOT_REDEFINE_MALLOC */

#ifdef MLEAK
#include "wl_atom.h"
WlAtom      *WlHashSlot();
int MLEAK_on = 0;
int MLEAK_count = 0;
int MLEAK_num = 0;
MLEAK_break(){}		/* MLEAK_num reached! */
extern WlAtom WlAtomFindNextPrefixedAtom();
extern int WlHashTableSize;
#endif /* MLEAK */



/*
 * This storage allocator trades space for speed in separate arenas for each
 * 2^n sizes (minus the header of 4 bytes (12 with debug on)
 * 
 * The overhead on a block is at least 4 bytes.  When free, this space
 * contains a pointer to the next free block, and the bottom two bits must
 * be zero.  When in use, the first byte is set to MAGIC, and the second
 * byte is the size index.  The remaining bytes are for alignment.
 * Whith DEBUG enabled, if the size of the block fits
 * in two bytes, then the top two bytes hold the size of the requested block
 * plus the range checking words, and the header word MINUS ONE.
 */

union overhead {
    union overhead *ov_next;	/* when free */
    struct {
	u_char          ovu_magic;	/* magic number */
	u_char          ovu_index;	/* bucket # */
#ifdef DEBUG
	u_short         ovu_rmagic;	/* range magic number */
	u_int           ovu_size;	/* actual block size */
#endif
    }               ovu;
#ifdef DOUBLE_ALIGN
    double          dummy;	/* Put union on double word boundary */
#endif /* DOUBLE_ALIGN */
};

#define	ov_magic	ovu.ovu_magic
#define	ov_index	ovu.ovu_index
#define	ov_rmagic	ovu.ovu_rmagic
#define	ov_size		ovu.ovu_size

#define	MAGIC		0xef	/* = 239 magic # on accounting info */
#define RMAGIC		0x5555	/* = 21845 magic # on range info */
#define WlFREED_MAGIC 0x31416	/* block has been freed */

#ifdef DEBUG
#define	RSLOP		sizeof (u_short)
#else
#define	RSLOP		0
#endif

/*
 * nextf[i] is the pointer to the next free block of size 2^(i+3).  The
 * smallest allocatable block is 8 bytes.  The overhead information
 * precedes the data area returned to the user.
 */

#define	NBUCKETS 30
static union overhead *nextf[NBUCKETS];
extern char    *WlSbrk();

static int      pagesz;		/* page size */
static int      pagebucket;	/* page size bucket */
static void	morecore();

#ifdef STATS
/*
 * nmalloc[i] is the difference between the number of mallocs and frees
 * for a given block size.
 */
static u_int    nmalloc[NBUCKETS];

#endif

char *WlMallocZoneBegin, *WlMallocZoneEnd;

/* WlMallocCheck is a routine that you can call regularily to verify validity 
 * of the whole malloced blocks
 * you MUST compile with -DMALLOCDEBUG, but not necessarily with -DDEBUG
 * then malloc, free & realloc will call it each time
 */

#ifdef MALLOCDEBUG
#define MDbucketMax 10000
char *MDbuckets[MDbucketMax];
int MDbucketsSize[MDbucketMax];
int MDbucket = 0;
int MDnocheck = 0;

int
WlMallocCheck()
{
    int i, j, sz, nblks, bucket;
    union overhead *op, *pop;

    if (MDnocheck)
	return 0;

    for (i = 0; i < MDbucket; i++) {
	sz = MDbucketsSize[i];
	bucket = 0;
	while (sz > (1 << (bucket + 3)))
	    bucket++;
	if (sz < pagesz) {
	    nblks = pagesz / sz;
	} else {
	    nblks = 1;
	}
	for (j = 0; j < nblks; j++) {
	    op = (union overhead *) (MDbuckets[i] + j*sz);
	    if (((int)(op->ov_magic)) == MAGIC) { /* in use */
		ASSERT(op->ov_index == bucket);
#ifdef DEBUG
		ASSERT(op -> ov_rmagic == RMAGIC);
		ASSERT(*(u_short *)((caddr_t)(op + 1)+op->ov_size)==RMAGIC);
#endif /*DEBUG*/
	    } else {			/* free, follow chain */
		pop = op;
		while (op) {
		    op = op->ov_next;
		}
	    }
	}
    }
    return 0;
}

#endif /*MALLOCDEBUG*/
	
#ifndef DO_NOT_USE_SBRK
#define WlSbrk(n) sbrk(n)
#else /*DO_NOT_USE_SBRK*/

/* for OSes with buggy malloc, we cannot use sbrk directly, so we just waste
 * memory...
 */

/* TODO: be smart, allocate a big arena, and do the splitting ourselves */

char *
WlSbrk(amt)
    int amt;
{
    return (char *) malloc(amt);
}

#endif /*DO_NOT_USE_SBRK*/


/*
 * Init malloc lists: to be called AT THE TOP of your program!!!
 * setup page size and align break pointer so all data will be page aligned.
 */

WlMallocInit()
{
    union overhead *op;
    int    bucket;
    unsigned amt, n;

    pagesz = n = getpagesize();
    op = (union overhead *) WlSbrk(0);
    WlMallocZoneBegin = (char *) op;
    n = n - sizeof(*op) - ((int) op & (n - 1));
    if ((int) n < 0)
	n += pagesz;
    if (n) {
	if ((char *) WlSbrk(n) == (char *) -1)
	    WlError("no more memory%s", "");
    }
    bucket = 0;
    amt = 8;
    while (pagesz > amt) {
	amt <<= 1;
	bucket++;
    }
    pagebucket = bucket;
}

/*
 * Convert amount of memory requested into closest block size stored in
 * hash buckets which satisfies request. Account for space used per block
 * for accounting.
 */

char           *
REAL_malloc(nbytes)
unsigned        nbytes;
{
    union overhead *op;
    int    bucket;
    unsigned amt, n;
#ifdef DEBUG
    char *result;
#endif

#ifdef MALLOCDEBUG
    WlMallocCheck();
#endif
#ifdef DEBUG
    if ((nbytes > 100000) 
#  ifdef DEBUG2
	|| (nbytes == 0)
#  endif
	) {
	fprintf(stderr, "MALLOC: trying to allocate %d bytes\n", nbytes);
	stop_if_in_dbx();
    }
#endif /* DEBUG */

    if (nbytes <= (n = pagesz - sizeof(*op) - RSLOP)) {
#ifndef DEBUG
	amt = 8;		/* size of first bucket */
	bucket = 0;
#else
	amt = 16;		/* size of first bucket */
	bucket = 1;
#endif
	n = -(sizeof(*op) + RSLOP);
    } else {
	amt = pagesz;
	bucket = pagebucket;
    }
    while (nbytes > amt + n) {  /* find bucket */
	amt <<= 1;
	bucket++;
    }

    /* if no hash bucket, allocates more memory */
    if ((op = nextf[bucket]) == NULL) {
	morecore(bucket);
	if ((op = nextf[bucket]) == NULL)	/* TO_DO: compact space? */
	    WlError("no more memory%s", "");
    }
    /* remove from linked list */
    nextf[bucket] = op -> ov_next;
    op -> ov_index = bucket;
#ifdef STATS
    nmalloc[bucket]++;
#endif

#ifdef DEBUG
    /* Record allocated size of block and bound space with magic numbers. */
    op -> ov_size = (nbytes + RSLOP - 1) & ~(RSLOP - 1);
    op -> ov_magic = MAGIC;
    op -> ov_rmagic = RMAGIC;
    *(u_short *) ((caddr_t) (op + 1) + op -> ov_size) = RMAGIC;
    ASSERT(((char *) (op + 1)));
#else
# ifdef MALLOCDEBUG
    op -> ov_magic = MAGIC;
# endif
#endif /*DEBUG*/
#ifndef MLEAK
#ifdef DEBUG
    result = ((char *) (op + 1));
    return result;
#else
    return ((char *) (op + 1));
#endif /*DEBUG*/
#else
    if (MLEAK_on) {
	char            name[20];
	WlAtom            atom;
	char           *ptr = (char *) (op + 1);

	sprintf(name + 1, "0x%x", ptr);
	name[0] = '\r';
	MLEAK_on = 0;
	atom = WlIntern(name);
	MLEAK_on = 1;
	atom -> c_val = (WlO) ++MLEAK_count;
	if (MLEAK_num == MLEAK_count)
	    MLEAK_break();
	return ptr;
    } else {
	return ((char *) (op + 1));
    }
#endif /* MLEAK */
}

/*
 * Allocate more memory to the indicated bucket.
 */
static void
morecore(bucket)
int             bucket;
{
    union overhead *op;
    int    sz;		/* size of desired block */
    int    amt;	/* amount to allocate */
    int    nblks;	/* how many blocks we get */

    sz = 1 << (bucket + 3);
    if (sz < pagesz) {
	amt = pagesz;
	nblks = amt / sz;
    } else {
	amt = sz + pagesz;
	nblks = 1;
    }
    op = (union overhead *) WlSbrk(amt);
    WlMallocZoneEnd = ((char *) op) + amt;
#ifdef MALLOCDEBUG
    ASSERT(MDbucket < MDbucketMax);
    MDbuckets[MDbucket] = (char *) op;
    MDbucketsSize[MDbucket] = sz;
    MDbucket++;
#endif
    /* no more room! */
    if ((int) op == -1)
	return;

    /* Add new memory allocated to that on free list for this hash bucket. */
    nextf[bucket] = op;
    while (--nblks > 0) {
	op -> ov_next = (union overhead *) ((caddr_t) op + sz);
	op = (union overhead *) ((caddr_t) op + sz);
    }
    op -> ov_next = NULL;
}

REAL_free(cp)
char           *cp;
{
    int    size;
    union overhead *op;
#ifdef DEBUG
    char *last_ptr;
#endif /* DEBUG */

#ifdef MALLOCDEBUG
    WlMallocCheck();
#endif

#ifdef MLEAK
    if (WlHashTableSize) {
	char            name[20];
	WlAtom      *patom;

	sprintf(name + 1, "0x%x", cp);
	name[0] = '\r';
	patom = WlHashSlot(name);
	if (*patom)
	    (*patom) -> c_val = 0;
    }
#endif /* MLEAK */

    ASSERT(cp != NULL);
    if(!cp) return;
    op = (union overhead *) ((caddr_t) cp - sizeof(union overhead));
    ASSERT(op -> ov_magic == MAGIC);	/* make sure it was in use */
    ASSERT(op -> ov_rmagic == RMAGIC);
    ASSERT(*(u_short *) ((caddr_t) (op + 1) + op -> ov_size) == RMAGIC);
    size = op -> ov_index;
    ASSERT(size < NBUCKETS);
#ifdef DEBUG
    last_ptr = cp + op -> ov_size;
#endif /* DEBUG */
    op -> ov_next = nextf[size];
    nextf[size] = op;
#ifdef DEBUG
    /* we erase all the data zone, to detect earlier re-using a freed block */
#ifdef SIMPLE_LHS
    WlIntFill(cp, last_ptr, WlFREED_MAGIC);
#else /* SIMPLE_LHS */
    while (cp <  last_ptr)
	*(((int *) cp)++) = WlFREED_MAGIC;
#endif /* SIMPLE_LHS */
#endif /* DEBUG */
#ifdef STATS
    nmalloc[size]--;
#endif
}

#ifdef SIMPLE_LHS
WlIntFill(p, last, value)
int *p, *last, value;
{
    while (p < last)
	*p++ = value;
}
#endif /* SIMPLE_LHS */

/*
 * Simple realloc without storage compaction
 * can be handled NULL pointer, but not 0 size...
 */

char           *
REAL_realloc(cp, nbytes)
char           *cp;
unsigned        nbytes;
{
    u_int  onb, i;
    union overhead *op;
    char           *res;

#ifdef MALLOCDEBUG
    WlMallocCheck();
    ASSERT(nbytes);
#endif
    if (!cp)
	return REAL_malloc(nbytes);
    op = (union overhead *) ((caddr_t) cp - sizeof(union overhead));
    i = op -> ov_index;
    onb = 1 << (i + 3);
    if (onb < pagesz)
	onb -= sizeof(*op) + RSLOP;
    else
	onb += pagesz - sizeof(*op) - RSLOP;
    /* avoid the copy if same size block */
    if (i) {
	i = 1 << (i + 2);
	if (i < pagesz)
	    i -= sizeof(*op) + RSLOP;
	else
	    i += pagesz - sizeof(*op) - RSLOP;
    }
    if (nbytes <= onb && nbytes > i) {
#ifdef DEBUG
	op -> ov_size = (nbytes + RSLOP - 1) & ~(RSLOP - 1);
	*(u_short *) ((caddr_t) (op + 1) + op -> ov_size) = RMAGIC;
#endif
	return (cp);
    } else {
	if ((res = REAL_malloc(nbytes)) == NULL)
	    return (NULL);
	ASSERT(cp != res);
	bcopy(cp, res, (nbytes < onb) ? nbytes : onb);
	REAL_free(cp);
	return (res);
    }
}

char *
REAL_calloc(nelem, elsize)
unsigned nelem, elsize;
{
    char           *result = REAL_malloc(nelem * elsize);

    if (result)
	bzero(result, nelem * elsize);
    return result;
}

/*****************************************************************************\
* 				misc utilities                                *
\*****************************************************************************/
/* returns room in current chunk
 * we set up things in debug mode so that all the chunk can be allocated
 * without trashing end markers
 */

int
WlMallocedSize(cp)
    char           *cp;
{
    u_int  onb, i;
    union overhead *op;

    if (!cp)
	return 0;
    op = (union overhead *) ((caddr_t) cp - sizeof(union overhead));
    i = op -> ov_index;
    onb = 1 << (i + 3);
    if (onb < pagesz)
	onb -= sizeof(*op) + RSLOP;
    else
	onb += pagesz - sizeof(*op) - RSLOP;
#ifdef DEBUG
    /* set Recorded allocated size to max */
    op -> ov_size = (onb + RSLOP - 1) & ~(RSLOP - 1);
    op -> ov_magic = MAGIC;
    op -> ov_rmagic = RMAGIC;
    *(u_short *) ((caddr_t) (op + 1) + op -> ov_size) = RMAGIC;
#endif
    return onb;
}


#ifdef STATS
/*
 * WlMstats - print out statistics about malloc
 * 
 * Prints two lines of numbers, one showing the length of the free list
 * for each size category, the second showing the number of mallocs -
 * frees for each size category.
 */
WlO
WlMstats()
{
    int    i, j;
    union overhead *p;
    int             totfree = 0, totused = 0;

    int             Nbuckets = (NBUCKETS > 14 ? 14 : NBUCKETS);

    WlPuts("Memory allocation statistics \n");
    WlPuts("size");
    for (i = 0; i < Nbuckets; i++) {
	if (i < 7) {
	    WlPrintf("%5ld", (long) (1 << (i + 3)));
	} else {
	    WlPrintf("%4ldk", (long) (1 << (i - 7)));
	}
    }
    WlPuts("\nfree");
    for (i = 0; i < Nbuckets; i++) {
	for (j = 0, p = nextf[i]; p; p = p -> ov_next, j++);
	WlPrintf("%5ld", (long) j);
	totfree += j * (1 << (i + 3));
    }
    WlPuts("\nused");
    for (i = 0; i < Nbuckets; i++) {
	WlPrintf("%5ld", (long) nmalloc[i]);
	totused += nmalloc[i] * (1 << (i + 3));
    }
    WlPrintf("\n\tTotal in use: %d,", (long) totused);
    WlPrintf("total free: %d\n", (long) totfree);
    return NIL;
}
#endif /* STATS */

/************\
* 	     *
* DBX tools  *
* 	     *
\************/

#ifdef DEBUG

/*
 * verify if pointer is still valid
 */

int
WlVerifyMalloc(cp)
char           *cp;
{
    int    size;
    union overhead *op;

    if (cp == NULL)
	return 0;
    op = (union overhead *) ((caddr_t) cp - sizeof(union overhead));
    ASSERT(op -> ov_magic == MAGIC);	/* make sure it was in use */
    if (op -> ov_magic != MAGIC)
	return 0;			/* sanity */
    ASSERT(op -> ov_rmagic == RMAGIC);
    ASSERT(*(u_short *) ((caddr_t) (op + 1) + op -> ov_size) == RMAGIC);
    size = op -> ov_index;
    ASSERT(size < NBUCKETS);
    return 1;
}

#endif /* DEBUG */

#ifdef MLEAK

/************************************************************************\
* 									 *
* malloc-leak tracing: (colas: uses the hash table!)			 *
* 									 *
* Under dbx:								 *
* 									 *
* Turn tracing on by setting MLEAK_on to 1				 *
* When malloced blocks are done, reset it to 0				 *
* then MLEAK_print(file,n) prints the nth first (if any) traced blocks	 *
* remain allocated then. (file=0 means stdout)				 *
* 									 *
* then by re-executing the program and setting MLEAK_num to the desired	 *
* number and setting a beakpoint in MLEAK_break, bdx will halt when the	 *
* desired block will be allocated					 *
* 									 *
* Exemple of test file:							 *
* 									 *
* 	(load ".gwmrc.gwm")						 *
* 	(? "set MLEAK_on = 1\n")					 *
* 	(break)								 *
* 	(load ".gwmrc.gwm")						 *
* 	(? "set MLEAK_on = 0\n")					 *
* 	(break)								 *
* 	(load ".gwmrc.gwm")						 *
* 	(load ".gwmrc.gwm")						 *
* 	(? "call MLEAK_print(\"FOO\", 10000)\n")			 *
* 	(? "stop in MLEAK_break\n")					 *
* 	(? "continue\n")						 *
* 	(break)								 *
* 									 *
\************************************************************************/

MLEAK_print(filename, n)
char *filename;
int	n;
{
    int             i = 0, old_MLEAK_on = MLEAK_on;
    WlAtom       atom;
    FILE           *ptr;

    MLEAK_on = 0;
    if ((!filename) || filename[0] == '\0') {
	filename = NULL;
	ptr = NULL;
    } else {
	ptr = fopen(filename, "w+");
    }
    WlAtomFindNextPrefixedAtom('\0');
    while ((atom = WlAtomFindNextPrefixedAtom('\r')) && i++ < n) {
	fprintf((ptr ? ptr : stdout),
		"%d th malloc at %s is still there\n",
		atom -> c_val, atom -> p_name + 1);
    }
    if (filename)
	fclose(ptr);
    MLEAK_on = old_MLEAK_on;
}

#endif /* MLEAK */
