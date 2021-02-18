/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*********************\
* 		      *
*  WOOL_OBJECT  Atom  *
*  BODY		      *
* 		      *
\*********************/

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_coll.h"
#include "wl_number.h"
#include "wl_list.h"
#include "wl_string.h"
#include "wl_pointer.h"
#include "wl_active.h"
#include "wl_name.h"
#include "INTERN.h"
#include "wl_atom.h"

/*
 * Constructor:
 * Constructed via the hash table management routines.
 */

/*
 * Warning: You should NEVER call WLAtom_make, call wool_atom instead,
 * or your atom won't be in the hash table!
 */

WOOL_Atom 
WLAtom_make(p_name, c_val)	/* makes an atom */
char           *p_name;		/* is COPIED to atom's p_name */
WOOL_OBJECT     c_val;		/* is just pointed to */
{
    WOOL_Quark       object = (WOOL_Quark)
    Malloc(sizeof(struct _WOOL_Quark) + strlen(p_name));

    object -> type = WLAtom;
    zrt_put(&(object -> type));
    strcpy(object -> p_name, p_name);
    object -> c_val = c_val;
    if (c_val)
	increase_reference(c_val);
    return (WOOL_Atom) &(object -> type);
}

WOOL_OBJECT
WLAtom_equal(a1, a2)
WOOL_Atom a1, a2;
{
    if (a2 != a1)
	return NIL;
    else if (a1 == (WOOL_Atom) NIL)
	return TRU;
    else
	return (WOOL_OBJECT) a1;
}

/************************\
* 			 *
*  hash table routines 	 *
* 			 *
\************************/

/*
 * Hash function definition:
 * HASH_FUNCTION: hash function, hash = hashcode, hp = pointer on char,
 *				 hash2 = temporary for hashcode.
 * INITIAL_TABLE_SIZE in slots
 * HASH_TABLE_GROWS how hash table grows.
 */

/* Mock lisp function */
/*
#define HASH_FUNCTION 	  hash = (hash << 5) - hash + *hp++;
#define INITIAL_HASH_SIZE 2017
#define HASH_TABLE_GROWS  HashTableSize = HashTableSize * 2;
*/

/* aho-sethi-ullman's HPJ (sizes should be primes)*/

#define HASH_FUNCTION	hash <<= 4; hash += *hp++; \
    if(hash2 = hash & 0xf0000000) hash ^= (hash2 >> 24) ^ hash2;
#define INITIAL_HASH_SIZE 4095	/* should be 2^n - 1 */
#define HASH_TABLE_GROWS  HashTableSize = HashTableSize << 1 + 1;

/* GNU emacs function */
/*
#define HASH_FUNCTION 	  hash = (hash << 3) + (hash >> 28) + *hp++;
#define INITIAL_HASH_SIZE 2017
#define HASH_TABLE_GROWS  HashTableSize = HashTableSize * 2;
*/

/* end of hash functions */

/*
 * The hash table is used to store atoms via their P_NAME:
 *
 * P_NAME --hash--> ATOM |--p_name--> "foo"
 *			 |--c_val--> value of the atom (result of eval)
 *
 * if c_val is UNDEFINED, symbol was undefined. If c_val is NULL, 
 * symbol value is NIL. Parsing replaces p_names with ATOMS.
 */

int             HashTableSize;
static          HashTableLimit;
static          HashTableUsed;
static WOOL_Atom *HashTable;	/* table of WLAtom objects */

/*
 * HashSlot gives the slot (pointer to WOOL_Atom) of a name
 * (slot points to NULL if it is not defined)
 */

WOOL_Atom      *
HashSlot(s)
char  *s;
{
    unsigned int hash, hash2;
    WOOL_Atom *p;
    char  *hp = s;
    char  *ns;

    hash = 0;
    while (*hp) {		/* computes hash function */
	HASH_FUNCTION
    }
    p = HashTable + hash % HashTableSize;
    while (*p) {
	ns = (*p) -> p_name;
	if (ns[0] == s[0] && strcmp(ns, s) == 0)
	    break;
	p--;
	if (p < HashTable)
	    p = HashTable + HashTableSize - 1;
    }
    return p;
}

HashTableGrows()
{
    WOOL_Atom *t, *p;
    int       i;
    int             OldHashTableSize = HashTableSize;

    t = HashTable;
    HASH_TABLE_GROWS
	HashTableLimit = HashTableSize / 3;
    HashTable = (WOOL_Atom *) Malloc(HashTableSize * sizeof(*HashTable));
    for (p = HashTable + HashTableSize; p > HashTable;)
	*--p = NULL;
    for (i = 0; i < OldHashTableSize; i++)
	if (t[i]) {
	    WOOL_Atom      *ps = HashSlot(t[i] -> p_name);

	    *ps = t[i];
	}
    Free(t);
}

/*
 * wool_atom(name)
 * return an WOOL_Atom, which is the one at the slot, if present,
 * or is created if name didn't exist, with c_val UNDEFINED. (NULL)
 * This function is called by the parser for each NAME encountered.
 * so that the parsed expression points directly to atoms.
 * The reference count of the atom is set to 1.
 */

WOOL_Atom 
wool_atom(tag)
char           *tag;
{
    WOOL_Atom *slot;

    if (HashTableUsed >= HashTableLimit)
	HashTableGrows();
    if (!*(slot = HashSlot(tag))) {	/* undefined, make a new one */
	HashTableUsed++;
	increase_reference((*slot = WLAtom_make(tag, UNDEFINED)));
    }
    return *slot;
}

/* WLAtom_unbind
 * Removes an atom from the hash table
 */

WOOL_OBJECT
WLAtom_unbind(obj)
WOOL_Atom obj;
{
    decrease_reference(obj -> c_val);
    obj -> c_val = UNDEFINED;
    if (obj -> reference_count == 1)
	decrease_reference(obj);
    return NIL;
}

/* must be called before allocating any atom
 */

HashTable_init()
{
    WOOL_Atom *p;

    HashTableSize = INITIAL_HASH_SIZE;
    HashTableLimit = HashTableSize / 3;
    HashTable = (WOOL_Atom *) Malloc(HashTableSize * sizeof(*HashTable));
    for (p = HashTable + HashTableSize; p > HashTable;)
	*--p = NULL;
}

#ifdef STATS

/*
 * hashstats:
 * statistics about the hash table
 */

WOOL_OBJECT 
hashstats()
{
    int             out_of_place;

    wool_puts("Statistics about hash table:\n");
    wool_printf("  %d slots used ", HashTableUsed);
    wool_printf("out of %d allocated\n", HashTableSize);
    out_of_place = outplacedslots();
    wool_printf("  and %d slots out of place ", out_of_place);
    wool_printf("(%d %%)\n", (out_of_place * 100) / HashTableUsed);
    return NIL;
}

int 
outplacedslots()
{
    WOOL_Atom *slot;
    int    n = 0;

    for (slot = HashTable; slot < HashTable + HashTableSize; slot++) {
	if (*slot) {
	    unsigned int hash, hash2;
	    char  *hp = (*slot) -> p_name;
	    char  *ns;

	    hash = 0;
	    while (*hp) {	/* computes hash function */
		HASH_FUNCTION
	    }
	    ns = (*(HashTable + hash % HashTableSize)) -> p_name;
	    if (!(ns[0] == (*slot) -> p_name[0] &&
		  strcmp(ns, (*slot) -> p_name) == 0)) {
		n++;
	    }
	}
    }
    return n;
}

/*
 *  prints the whole hash table
 */

WOOL_OBJECT
oblist()
{
    WOOL_Atom *slot;
    int    num = 0;

    for (slot = HashTable; slot < HashTable + HashTableSize; slot++) {
	if (*slot) {
	    wool_printf("%s ", (*slot) -> p_name);
	    if ((*slot) -> type == WLAtom) {
		if (((*slot) -> p_name[0] > ' ') && ((*slot) -> c_val)) {
		    wool_printf("(%s): ", (((*slot) -> c_val) -> type)[0]);
		    wool_print((*slot) -> c_val);
		}
	    } else {
		wool_print(*slot);
	    }
	    num++;
	    wool_newline();
	}
    }
    return (WOOL_OBJECT) WLNumber_make(num);
}

#endif /* STATS */

/*
 * prints the names of the atoms pointing to this object (or nothing)
 */

print_atom_pointing_to(object)
WOOL_OBJECT	object;
{
    WOOL_Atom *slot;

    for (slot = HashTable; slot < HashTable + HashTableSize; slot++)
	if (*slot)
	    if ((*slot) -> type == WLAtom)
		if ((*slot) -> c_val)
		    if ((*slot) -> c_val == object)
			wool_printf("%s ", (*slot) -> p_name);
}

#ifdef MLEAK

/*
 * gives the atoms with prefix prefix successivly (or NULL on end);
 * re-initialise with a '\0' prefix
 */

WOOL_Atom
find_next_prefixed_atom(prefix)
char	prefix;
{
    WOOL_Atom *slot;
    static WOOL_Atom *slot0;

    if (prefix == '\0') {
	slot0 = HashTable;
    } else {
	for (slot = slot0; slot < HashTable + HashTableSize; slot++)
	    if ((*slot)
		&& ((*slot) -> p_name[0] == prefix)
		&& ((*slot) -> c_val)) {
		slot0 = slot + 1;
		return (*slot);
	    }
    }
    return NULL;
}

#endif /* MLEAK */
    
/*
 * XLAtom_eval:
 * evaluating an atom is giving a pointer to its c_val field.
 * an atom returns its value, or calls wool_error if undefined
 * (increase ref. of value)
 */

WOOL_OBJECT 
WLAtom_eval(obj)
WOOL_Atom obj;
{
    if (obj -> c_val != UNDEFINED) {
	return obj -> c_val;
    } else
	return wool_error(UNDEFINED_VARIABLE, obj -> p_name);
}

WOOL_OBJECT 
WLAtom_silent_eval(obj)
WOOL_Atom obj;
{
    return obj -> c_val;
}

/*
 * WLAtom_print:
 * printing an atom is printing the string in the p_name field.
 */

WOOL_OBJECT 
WLAtom_print(obj)
WOOL_Atom       obj;
{
    wool_puts(obj -> p_name);	/* perhaps () for NIL? */
    return (WOOL_OBJECT) obj;
}

/*
 * WLAtom_free;
 * Frees the Quark of this Atom
 */

WOOL_OBJECT 
WLAtom_free(obj)
WOOL_Atom       obj;
{
    WOOL_Atom      *slot = HashSlot(obj -> p_name);
    WOOL_Atom      *next_slot = slot - 1;
    WOOL_Atom       atom;

    *slot = NULL;
    while (atom = *(next_slot = (next_slot < HashTable ?
			      HashTable + HashTableSize - 1 : next_slot))) {
	*next_slot = NULL;
	*(HashSlot(atom -> p_name)) = atom;
	next_slot--;
    }
    Free((((char *) obj)
	  - (sizeof(struct _WOOL_Quark) - sizeof(struct _WOOL_Atom))));
    return NULL;
}

/*
 * WLAtom_execute:
 * executes the object in the C_val
 */

WOOL_OBJECT
WLAtom_execute(obj, list)
WOOL_Atom obj;
WOOL_List list;
{
    if (obj -> c_val && (obj -> c_val -> type != WLAtom)) {
	return WOOL_send(WOOL_execute, obj -> c_val, (obj -> c_val, list));
    } else if (obj -> c_val && (obj -> c_val == NIL)) {
	return NIL;
    } else {
	return (wool_error(UNDEFINED_FUNCTION, obj));
    }
}

#ifdef USER_DEBUG
wool_put_spaces(n)
int n;
{
    int             i;

    wool_printf("%d ", n);
    for (i = 0; i < n; i++)
	wool_puts(" ");
}
#endif /* USER_DEBUG */
/*
 * WLAtom_set
 * the normal setq routine
 */

WOOL_OBJECT
WLAtom_set(atom, value)
WOOL_Atom atom;
WOOL_OBJECT value;
{
    WOOL_OBJECT new =  WOOL_send(WOOL_eval, value, (value));

    decrease_reference(atom -> c_val);
    increase_reference(atom -> c_val = new);
    return new;
}

WOOL_OBJECT
WLAtom_setq(atom, value)
WOOL_Atom atom;
WOOL_OBJECT value;
{
    decrease_reference(atom -> c_val);
    increase_reference(atom -> c_val = value);
    return value;
}

/*
 * C_value of an atom:
 *  NIL => 0
 *  t   => 1
 *  oth => adress of atom itself
 */

int
WLAtom_get_C_value(obj)
WOOL_Atom obj;
{
    if (obj == (WOOL_Atom) NIL)
	return NULL;
    else if (obj == (WOOL_Atom) TRU)
	return  1;
    else
	return (int) obj;
}

void
must_be_atom(atom, n)
WOOL_Atom	atom;
int		n;
{
    if ((atom -> type != WLAtom)	/* verify type of arg1 */
	&&(atom -> type != WLPointer)
	&& (atom -> type != WLActive)
	&& (atom -> type != WLName))
	bad_argument(atom, n, "symbol");
}

int
is_an_atom(atom)
WOOL_Atom	atom;
{
    if ((atom -> type != WLAtom)	/* verify type of arg1 */
	&&(atom -> type != WLPointer)
	&& (atom -> type != WLActive)
	&& (atom -> type != WLName))
	return 0;
    else
	return 1;
}
