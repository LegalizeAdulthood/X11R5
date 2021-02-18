/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/hash.c,v 2.5 1991/09/12 16:30:17 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/hash.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/hash.c,v 2.5 1991/09/12 16:30:17 bobg Exp $ ";
#endif /* lint */

/* A hash table */

#include <class.h>
#include <hash.eh>
#include <glist.ih>

struct egg {
    char *key,*value;
};



int DefaultHash(key)
char *key;
{
    char c;
    int index=0;

    while ((c = *key++) != '\0') {
        index += c;
    }
    index &= (hash_BUCKETS-1);
    return index;
}


boolean hash__InitializeClass(classID)
struct classheader *classID;
{
    return TRUE;
}

boolean hash__InitializeObject(classID,self)
struct classheader *classID;
struct hash *self;
{
    int i;
    for(i=0;i<hash_BUCKETS;i++)
        self->buckets[i] = NULL;
    self->hash = DefaultHash;
    return TRUE;
}

void hash__FinalizeObject(classID,self)
struct classheader *classID;
struct hash *self;
{
    int i;
    for (i=0;i<hash_BUCKETS;i++)
        if (self->buckets[i] != NULL)
            glist_Destroy(self->buckets[i]);
}

void hash__Store(self,key,value)
struct hash *self;
char *key;
char *value;
{
    int bucket = (*self->hash)(key);
    struct egg *egg = (struct egg *)malloc(sizeof(struct egg));

    egg->key = (char *)malloc(strlen(key)+1);
    strcpy(egg->key,key);
    egg->value = value;

    if (self->buckets[bucket] == NULL)
        self->buckets[bucket] = glist_Create(NULL);

    glist_Insert(self->buckets[bucket],egg);
}

int FindEgg(egg,key)
struct egg *egg;
char *key;
{

    if (strcmp(egg->key,key)==0)
        return TRUE;
    else
        return FALSE;
}



char *hash__Lookup(self,key)
struct hash *self;
char *key;
{
    int bucket = (*self->hash)(key);
    struct egg *egg;

    if (self->buckets[bucket] == NULL) {
        return NULL;
    }
    else {
        egg = (struct egg *) glist_Find(self->buckets[bucket],FindEgg,key);
        if (egg != NULL) {
            return egg->value;
        }
        else {
            return NULL;
        }
    }
}

char * hash__Delete(self,key)
struct hash *self;
char *key;
{
    int bucket = (*self->hash)(key);
    struct egg *egg;
    if (self->buckets[bucket] == NULL)
        return NULL;
    egg = (struct egg *)glist_Find(self->buckets[bucket],FindEgg,key);
    if (egg != NULL) {
        glist_Delete(self->buckets[bucket],egg,FALSE);
        free(egg->key);
        free(egg);
        return egg->value; /* OK to reference after just freed */
    }
    else
        return NULL;
    
}

char *hash__Rename(self,key,new)
struct hash *self;
char *key,*new;
{
    int bucket = (*self->hash)(key);
    struct egg *egg;
    if (self->buckets[bucket] == NULL)
        return NULL;
    egg = (struct egg *)glist_Find(self->buckets[bucket],FindEgg,key);
    if (egg == NULL)
        return NULL;
    else {
        glist_Delete(self->buckets[bucket],egg,FALSE);
        egg->key = (char *)realloc(egg->key,strlen(new)+1);
        strcpy(egg->key,new);
        bucket = (*self->hash)(new);
        if (self->buckets[bucket] == NULL)
            self->buckets[bucket] = glist_Create(NULL);
        glist_Insert(self->buckets[bucket],egg);
        return egg->value;
    }
}

void hash__Clear(self, valFree)
struct hash *self;
procedure valFree;
{
    int i;
    struct egg *egg;
    struct glist *gl;

    for (i=0;i<hash_BUCKETS;i++) {
	if ((gl = self->buckets[i]) != NULL)  {
	    while ((egg = (struct egg *) glist_Pop(gl)) != NULL)  {
		if (egg->key != NULL)  {
		    free(egg->key);
		}
		if (egg->value != NULL && valFree != NULL)  {
		    (*valFree)(egg->value);
		}
		free(egg);
	    }
	    glist_Destroy(self->buckets[i]);
	    self->buckets[i] = NULL;
	}
    }
}


PrintAll(egg,nothing)
struct egg *egg;
int nothing;
{
    printf("Egg (%s) contains (%s)\n",egg->key,egg->value);
    return FALSE;
}



void hash__Debug(self)
struct hash *self;
{
    int i;
    for (i=0;i<hash_BUCKETS;i++) {
        if (self->buckets[i] == NULL)
            printf("Bucket %d is NULL\n",i);
        else {
            printf("Bucket %d ------------------>\n",i);
            glist_Find(self->buckets[i],PrintAll,NULL);
        }
    }
}
