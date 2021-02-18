/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

struct scache_el {
    long refcount;
    char str[1];
};

struct scache_node {
    unsigned long els, maxels;
    struct scache_el **scache_el;
};

#define scache_REFCOUNT(string) (*(long *)((string)-(long)((struct scache_el *)0)->str))
/* this hash function is pretty poor as far as magic functions go, but it's not too bad... */


#if defined(__STDC__) && !defined(ibm032)
char *scache_Hold(char *str);
void scache_Free(char *str);
#else
char *scache_Hold();
void scache_Free();
#endif
