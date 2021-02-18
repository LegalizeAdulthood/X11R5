/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/sym.c,v 1.8 1991/09/12 16:27:25 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/sym.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/sym.c,v 1.8 1991/09/12 16:27:25 bobg Exp $";
#endif

/* sym.c		

	Code for the sym object
*/
/*
 *    $Log: sym.c,v $
 * Revision 1.8  1991/09/12  16:27:25  bobg
 * Update copyright notice and rcsid
 *
 * Revision 1.7  1989/08/25  14:30:14  gb17
 * bug with sym_Destroy fixed
 *
 * Revision 1.6  89/08/23  16:11:12  wjh
 * andrewos
 * 
 * Revision 1.5  89/06/01  10:09:17  wjh
 * revision for new securtiy system
 * 
 * Revision 1.1  88/10/21  10:42:26  wjh
 * Initial revision
 *  
 * Revision 1.0  88/06/23  12:15:01  gb17
 * Copied from /usr/andrew/lib/nullinset
 */

/*****************************************************************************\
\*****************************************************************************/



#include <class.h>
#include <sym.eh>

#include <andrewos.h>

/*****************************************************************************\
 *
 * CLASS CONSTANTS
 *
\*****************************************************************************/

#define	initialScopes	    4		/* initial size of enclosing scope table   */

#define	SCOPE_NULL	    -1		/* indicates a NULL scope		   */
#define	SCOPE_FREE	    -2		/* indicates a free slot in a table          */


/*****************************************************************************\
 *
 * CLASS DATA
 *
\*****************************************************************************/

static long maxScopes, nextFreeScope, *enclosingScope;
/* this table gives the enclosing scope for each scope.  maxScopes is the size of the
 * table, and nextFreeScope is the index of the lowest numbered free slot in the
 * table.
 */

static long noOfEntries;
static struct sym **table;
/* the hash table.  tableSize gives the size of the table, noOfEntries
 * gives the number of entries in the table, and table is the table itself.
 */

static long primes[] = {5, 11, 17, 37, 67, 131,
                        257, 521, 1031,2053, 4099,8209, 16411,  32771,
                        65537, 131101, 262147, 524309,1048583, 2097169,
                        4194319, 8388617, 16777259, NULL };
static long *tableSize;
/* this is the sequence of prime numbers used for determining the tableSize.
 */

/*****************************************************************************\
 *
 * PRIVATE FUNCTIONS
 *
\*****************************************************************************/

static long
hash(name)
unsigned char *name;
{
    register unsigned long val;
    register unsigned char *pos;

    for (val = 0, pos=name; *pos; ++pos)
	val = ((val<<5) | (val>>27)) ^ *pos;

    return val % *tableSize;
}

static struct sym**
lookup(name, scope, found)
char *name;
long scope;
boolean *found;
{
    register struct sym **s, **start = table+hash(name);

    while (scope != SCOPE_NULL) {

	/* check all the other valid scopes for the symbol, working outward */

	for(s = start; *s != NULL; s = &((*s)->next)) {

	    /* look for name in the current scope, and return if found */

	    if (strcmp(name, (*s)->name) == 0 && scope == (*s)->scope) {
		*found = TRUE;
		return (s);
	    }
	}

	scope = enclosingScope[scope];

    };

    *found = FALSE;
    return s;
}

static struct sym**
lookupInScope(name, scope, found)
char *name;
long scope;
boolean *found;
{
    register struct sym **s;

    if (name == NULL) {
	*found = FALSE;
	return NULL;
    }
    for(s = table+hash(name); *s != NULL; s = &((*s)->next)) {

	/* look for name in the current scope, and return if found */

	if (strcmp(name, (*s)->name) == 0 && scope == (*s)->scope) {
	    *found = TRUE;
	    return (s);
	}
    }

    *found = FALSE;
    return s;
}

static void
insert(self)
struct sym *self;
{
    boolean found;
    struct sym** loc = lookupInScope(self->name, self->scope, &found);

    ++noOfEntries;
    self->next = *loc;
    *loc = self;
}

static void
resizeTable()
{
    struct sym **old = table;
    long oldSize = *tableSize;
    struct sym *pos, *next;
    long i;

    ++tableSize;
    table = (struct sym **) malloc(sizeof(struct sym*) * *tableSize);
    noOfEntries = 0;

    for (i = 0; i < *tableSize; ++i)
	table[i] = NULL;

    for (i = 0; i < oldSize; ++i)
	for (pos = old[i]; pos != NULL;) {
	    next = pos->next;
	    insert(pos);
	    pos = next;
	}

    free(old);
}

static void
resizeArray(a, from, to)
long **a, from, to;
{
    long *new = (long*) malloc(sizeof(long) * to);
    long i;

    for (i = 0; i < from; ++i)
	new[i] = (*a)[i];

    for (i = from; i < to; ++i)
	new[i] = SCOPE_FREE;
    free(*a);
    *a = new;
}

static void
removeScopeFromScopes(scope)
sym_ScopeType scope;
{
    register long i;
 
    for (i = 0; i < maxScopes; ++i)
	if (enclosingScope[i] == scope)
	    removeScopeFromScopes(i);

    enclosingScope[scope] = SCOPE_FREE;
    if (scope < nextFreeScope)
	nextFreeScope = scope;
}

/*****************************************************************************\
 *
 * CLASS METHODS
 *
\*****************************************************************************/

sym_ScopeType
sym__NewScope(ClassID, scope)
struct classhdr *ClassID;
sym_ScopeType scope;
{
    long new = nextFreeScope;

    if (nextFreeScope == maxScopes-1) {
	resizeArray(&enclosingScope, maxScopes, 2*maxScopes);
	maxScopes *= 2;
    }

    enclosingScope[new] = scope;

    do
	++nextFreeScope;
    while (enclosingScope[nextFreeScope] != SCOPE_FREE);

    return new;
}

void
sym__DestroyScope(ClassID, scope)
struct classhdr *ClassID;
sym_ScopeType scope;
{
    register long i, s;
    register struct sym **pos;

    for (i = 0; i < *tableSize; ++i)
	for (pos = &table[i]; *pos != NULL;) {
	    for(s = (*pos)->scope; s != scope && s != SCOPE_NULL;)
		s = enclosingScope[s];

	    if (s == scope) {
		struct sym *trash = *pos;

		*pos = trash->next;
		free(trash->name);
		trash->name = NULL;
		sym_Destroy(trash);
	    } else
		pos = &((*pos)->next);
	}

    removeScopeFromScopes(scope);
}

sym_ScopeType
sym__ParentScope(ClassID, scope)
struct classhdr *ClassID;
sym_ScopeType scope;
{
	return enclosingScope[scope];
}


struct sym*
sym__Define(ClassID, name, proto, scope)
struct classhdr *ClassID;
char *name;
struct sym *proto;
sym_ScopeType scope;
{
    boolean found;
    struct sym *newSym, **loc;

    if (noOfEntries > *tableSize)
	resizeTable();

    loc = lookupInScope(name, scope, &found);

    if (found)
	return NULL;
    else {

	newSym = proto == NULL ? sym_New() : sym_NewFromObject(proto);
	++noOfEntries;

	newSym->name = (unsigned char *)malloc(strlen(name)+1);
	strcpy(newSym->name, name);
	newSym->scope = scope;
	newSym->next = *loc;
	*loc = newSym;
	return newSym;
    }
}


boolean
sym__Undefine(ClassID, name, scope)
struct classhdr *ClassID;
char *name;
sym_ScopeType scope;
{
    boolean found;
    struct sym **loc = lookupInScope(name, scope, &found);

    if (found) {
	struct sym *trash = *loc;

	*loc = trash->next;
	free(trash->name);
	trash->name = NULL;
	sym_Destroy(trash);
	return TRUE;
    }
    else
	return FALSE;
}


struct sym*
sym__Find(ClassID, name, scope)
struct classhdr *ClassID;
char *name;
sym_ScopeType scope;
{
    boolean found;
    struct sym **loc = lookup(name, scope, &found);

    if (found)
	return *loc;
    else
	return NULL;
}

struct sym*
sym__Locate(ClassID, name, proto, scope, new)
struct classhdr *ClassID;
char *name;
struct sym *proto;
sym_ScopeType scope;
boolean *new;
{
    boolean found;
    struct sym *newSym, **loc;

    if (noOfEntries > *tableSize)
	resizeTable();

    loc = lookup(name, scope, &found);

    if (found) {
	*new = FALSE;
	return *loc;
    } else {
	newSym = proto == NULL ? sym_New() : sym_NewFromObject(proto);
	++noOfEntries;

	newSym->name = (unsigned char *)malloc(strlen(name)+1);
	strcpy(newSym->name, name);
	newSym->scope = scope;
	newSym->next = *loc;
	*loc = newSym;
	*new = TRUE;
	return newSym;
    }
}

long
sym__FindAll(ClassID, name, scope, proc, rock)
struct classhdr *ClassID;
char *name;
sym_ScopeType scope;
long (*proc)();
long *rock;
{
    register long i, s;
    register struct sym **pos;

    for (i = 0; i < *tableSize; ++i)
	for (pos = &table[i]; *pos != NULL;) {
	    for(s = (*pos)->scope; s != scope && s != SCOPE_NULL;)
		s = enclosingScope[s];

	    if (s == scope && strcmp(name, (*pos)->name) == 0) {
		long val = proc(*pos, rock);

		if (val != NULL)
		    return val;
	    }
	    pos = &((*pos)->next);
	}
}


boolean
sym__InitializeClass(ClassID)
struct classhdr *ClassID;
{
    long i;

    maxScopes = initialScopes;
    enclosingScope = (long *) malloc(sizeof(long) * maxScopes);
    for (i = 1; i < maxScopes; ++i)
	enclosingScope[i] = SCOPE_FREE;
    enclosingScope[sym_GLOBAL] = SCOPE_NULL;

    nextFreeScope = 1;

    tableSize = primes;
    table = (struct sym **) malloc(sizeof(struct sym*) * *tableSize);
    for(i = 0; i < *tableSize; ++i)
	table[i] = NULL;
    noOfEntries = 0;

    return TRUE;
}


boolean
sym__InitializeObject(ClassID, self)
struct classhdr *ClassID;
struct sym *self;
{
    self->name = NULL;
    return TRUE;
}

void
sym__FinalizeObject(ClassID, self)
struct classhdr *ClassID;
struct sym *self;
{
    return;
}





void
sym__printtable(ClassID)
struct classhdr *ClassID;
{
        long i;
	struct sym *pos;

	printf("enclosingScope:");
	for (i = 0; i < maxScopes; ++i)
	    printf(" %d", enclosingScope[i]);
	printf(" <%d> \n", nextFreeScope);

	printf("table <%d>:\n", noOfEntries);
	for (i = 0; i < *tableSize; ++i) {
	    printf("%d:\n",i);
	    for(pos = table[i]; pos != NULL; pos = pos->next)
		printdata(pos);
	}

	printf("\n\n");

	fflush(stdout);
}


printdata(self)
struct sym *self;
{
    if (self == NULL)
	printf("NULL\n");
    else
	printf("%x:%s,%d\n", self, self->name, self->scope);
}

/*****************************************************************************\
 *
 * INSTANCE METHODS
 *
\*****************************************************************************/

