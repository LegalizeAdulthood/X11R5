/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *scache_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/cmenu/RCS/scache.c,v 1.4 1991/09/12 17:09:36 bobg Exp $";

/* This file (scache.c) and scache.h will implement a string cacheing mechanism so that even if menus are leaked they won't represent much of a drain... */

#include <stdio.h>
#include "scache.h"

static struct scache_node scache[256];

#define HASH(str,len) (((*(str)<<2)+((str)[(len)>>1])+((str)[(len)-(len)?1:0]<<2)+((str)[len>>2]<<2)+(len))&0xff)
#define BLOCKSIZE 5
#define GROWTHFACTOR 2

void scache_Init()
{
    bzero(scache,sizeof(scache));
}

char *scache_Hold(str)
char *str;
{
    unsigned long len;
    unsigned long hash;
    struct scache_node *s;
    struct scache_el *e=NULL;

    if(!str) str="";
    
    len=strlen(str);
    hash=HASH(str,len);
    s=scache+hash;

    if(s->els) {
	int i;
	struct scache_el **p=s->scache_el;
	for(i=0;i<s->els;i++, p++) {
	    if(!strcmp((*p)->str, str)) {
		(*p)->refcount++;
		return (*p)->str;
	    }
	    if((*p)->refcount<=0) e=(*p);
	}
    }
    if(!e) {
	if(s->els>=s->maxels) {
	    s->maxels=s->maxels*GROWTHFACTOR+BLOCKSIZE;
	    if(s->scache_el) s->scache_el=(struct scache_el **)realloc(s->scache_el, s->maxels*sizeof(struct scache_el *));
	    else s->scache_el=(struct scache_el **)malloc(s->maxels*sizeof(struct scache_el *));
	    if(!s->scache_el) {
		s->els=0;
		s->maxels=0;
		return NULL;
	    }
	}
	e=(struct scache_el *)malloc(len+sizeof(struct scache_el));
	if(!e) return NULL;
	s->els++;
    } else e=(struct scache_el *)realloc(e, len+sizeof(struct scache_el));
    s->scache_el[s->els-1]=e;
    strcpy(e->str,str);
    e->refcount=1;
    return e->str;
}

void scache_Free(str)
char *str;
{
    scache_REFCOUNT(str)--;
}

void scache_Dump()
{
    int i;
    printf("doing scache dump\n");
    for(i=0;i<256;i++) {
	printf("------\n");
	if(scache[i].els) {
	    int j;
	    for(j=0;j<scache[i].els;j++) printf("%s\n",scache[i].scache_el[j]->str);
	}
    }
}
