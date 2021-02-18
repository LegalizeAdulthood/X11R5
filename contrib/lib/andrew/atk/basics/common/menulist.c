/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/menulist.c,v 2.9 1991/09/12 15:59:17 bobg Exp $ */
/* $ACIS:menulist.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/menulist.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/menulist.c,v 2.9 1991/09/12 15:59:17 bobg Exp $";
#endif /* lint */

/* menulist.c
 * Provides an abstraction for cooperative menu usage among views.
 */

#include <class.h>
#include <menulist.eh>
#include <proctbl.ih>

#define class_StaticEntriesOnly
#include <view.ih>
#undef class_StaticEntriesOnly


static long nextMLVersion = 0;

boolean menulist__InitializeObject(classID, self)
    struct classheader *classID;
    struct menulist *self;
{
    self->regionID = -1;
    self->curIM = NULL;
    self->version = 0;
    self->installVersion = -1;
    self->object = NULL;
    self->menus = NULL;
    self->refcount = (int *) malloc(sizeof(int));
    *self->refcount = 1;
    self->curMenu = NULL;
    self->menuChainBefore = NULL;
    self->menuChainAfter = NULL;
    self->curChainBefore = NULL;
    self->curChainAfter = NULL;
    self->selectMask=0;
    return TRUE;
}

void menulist__FinalizeObject(classID, self)
    struct classheader *classID;
    struct menulist *self;
{

    if (*self->refcount == 1) {
	menulist_ClearML(self);
	free(self->refcount);
    }
    else if (*self->refcount > 1) {
	*self->refcount -= 1;
    }
    menulist_ClearChain(self);
}

void menulist__SetView(self, view)
    struct menulist *self;
    struct view *view;
{

    self->object = (struct basicobject *) view;
}

struct menulist *menulist__Create(classID, view)
    struct classheader *classID;
    struct view *view;
{

    struct menulist *thisMenu;

    thisMenu = menulist_New();
    menulist_SetView(thisMenu, view);
    return thisMenu;
}

struct menulist *menulist__DuplicateML(self, view)
    struct menulist *self;
    struct view *view;
{

    struct menulist *newMenus;

    newMenus = menulist_Create(view);
    free(newMenus->refcount);
    *self->refcount += 1;
    newMenus->refcount = self->refcount;
    newMenus->menus = self->menus;
    return newMenus;
}

/* Copy a menu list's menu items. Used to implement copy-on-write for item lists.
 * This routine should only be called when *menulist->refcount > 1
 */
static void copyItems(menulist)
struct menulist *menulist;
{
    struct itemlist *traverse = menulist->menus;

    menulist->menus = NULL;
    for (; traverse != NULL; traverse = traverse->next) {
        struct itemlist *thisItem;

        thisItem = (struct itemlist *) malloc(sizeof(struct itemlist));
        thisItem->string = (char *) malloc(strlen(traverse->string) + 1);
        strcpy(thisItem->string, traverse->string);
        thisItem->proc = traverse->proc;
        thisItem->functionData = traverse->functionData;
	thisItem->enableMask = traverse->enableMask;
        thisItem->next = menulist->menus;

        menulist->menus = thisItem;
    }
    *menulist->refcount -= 1;
    menulist->refcount = (int *) malloc(sizeof(int));
    *menulist->refcount = 1;
}

void menulist__AddToML(self, string, menuProc, functionData, mask)
    struct menulist *self;
    char *string;
    struct proctable_Entry *menuProc;
    long functionData; /* Actually any 32 bit crufty... */
    long mask;
{

    struct itemlist *thisItem;
    boolean link = FALSE;

    if (string == NULL)
        return;

    if (*self->refcount > 1)
        copyItems(self);

    for (thisItem = self->menus; thisItem != NULL && (strcmp(thisItem->string, string) != 0); thisItem = thisItem->next);

    if (thisItem == NULL) {
        thisItem = (struct itemlist *) malloc(sizeof(struct itemlist));
        thisItem->string = (char *) malloc(strlen(string) + 1);
        strcpy(thisItem->string, string);
        link = TRUE;
    }
    thisItem->proc = menuProc;
    thisItem->functionData = functionData;
    thisItem->enableMask=mask;
    if (link) { /* Only link it in after the data is valid. */
        thisItem->next = self->menus;
        self->menus = thisItem;
    }
    self->version = self->menuVersion = nextMLVersion;
}

void menulist__DeleteFromML(self, string)
    struct menulist *self;
    char *string;
{

    struct itemlist *traverse, **previous = &(self->menus);

    if (string == NULL)
        return;

    if (*self->refcount > 1)
        copyItems(self);

    for (traverse = self->menus; traverse != NULL && (strcmp(traverse->string, string) != 0); traverse = traverse->next) {
        previous = &(traverse->next);
    }

    if (traverse != NULL) {
        *previous = traverse->next;
        free(traverse->string);
        free(traverse);
	self->version = self->menuVersion = nextMLVersion;
    }
}

boolean menulist__SetMask(self,mask)
struct menulist *self;
long mask;
{
    if(mask!=self->selectMask){
	self->selectMask=mask;
	return TRUE;
    }else
	return FALSE;
}

void menulist__ClearML(self)
    struct menulist *self;
{

    struct itemlist *traverse, *next;

    if (*self->refcount == 1)  {
        for (traverse = self->menus; traverse != NULL; traverse = next) {
            free(traverse->string);
            next = traverse->next;
            free(traverse);
        }
    }
    else {
        if (*self->refcount < 1)
            fprintf(stderr, "menulist: internal error, refcount < 1 in ClearML\n");
        else {
            *self->refcount -= 1;
            self->refcount = (int *) malloc(sizeof(int));
            *self->refcount = 1;
        }
    }
    self->menus = NULL;
    self->version = self->menuVersion = nextMLVersion;
}

boolean menulist__NextME(self, outString, outData, outProc)
    struct menulist *self;
    char **outString;
    long *outData;
    struct proctable_Entry **outProc;
{

    if (self->curMenu != NULL) {
        *outString = self->curMenu->string;
        *outData = self->curMenu->functionData;
        *outProc = self->curMenu->proc;
        self->curMenu = self->curMenu->next;
        return TRUE;
    }
    else
        return FALSE;
}

struct menulist *menulist__NextBeforeMC(self)
    struct menulist *self;
{

    register struct menulist *value;

    if (self->curChainBefore != NULL) {
        value = self->curChainBefore->menulist;
        self->curChainBefore = self->curChainBefore->next;
        return value;
    }
    return NULL;
}

struct menulist *menulist__NextAfterMC(self)
    struct menulist *self;
{

    register struct menulist *value;

    if (self->curChainAfter != NULL) {
        value = self->curChainAfter->menulist;
        self->curChainAfter = self->curChainAfter->next;
        return value;
    }
    return NULL;
}

void menulist__ChainBeforeML(self, chainee, key)
    struct menulist *self;
    struct menulist *chainee;
    long key;
{

    struct headerlist *tempHeader, *next, **previous;

    if (chainee == NULL) /* Need to handle posting of NULL since it is used to clear menus. */
        return;

    previous = &(self->menuChainBefore);
    for (tempHeader = self->menuChainBefore; tempHeader != NULL; tempHeader = next) {
        if (tempHeader->menulist == chainee) {
            if (tempHeader->assocKey != key)
                tempHeader->assocKey = key;
            return;
        }
        else if (tempHeader->assocKey == key) {
            *previous = next = tempHeader->next;
            free(tempHeader);
            continue;
        }
        next = tempHeader->next;
    }
    
    previous = &(self->menuChainAfter);
    for (tempHeader = self->menuChainAfter; tempHeader != NULL; tempHeader = next) {
        if (tempHeader->menulist == chainee || tempHeader->assocKey == key) {
            *previous = next = tempHeader->next;
            free(tempHeader);
            continue;
        }
        next = tempHeader->next;
    }

    tempHeader = (struct headerlist *) malloc(sizeof(struct headerlist));
    tempHeader->menulist = chainee;
    tempHeader->next = self->menuChainBefore;
    tempHeader->assocKey = key;
    self->menuChainBefore = tempHeader;
    self->version = nextMLVersion;
}

void menulist__ChainAfterML(self, chainee, key)
    struct menulist *self;
    struct menulist *chainee;
    long key;
{

    struct headerlist *tempHeader, *next, **previous;

    if (chainee == NULL) /* Need to handle posting of NULL since it is used to clear menus. */
        return;

    previous = &(self->menuChainBefore);
    for (tempHeader = self->menuChainBefore; tempHeader != NULL; tempHeader = next) {
        if (tempHeader->menulist == chainee || tempHeader->assocKey == key) {
            *previous = next = tempHeader->next;
            free(tempHeader);
            continue;
        }
        next = tempHeader->next;
    }
   
    previous = &(self->menuChainAfter);
    for (tempHeader = self->menuChainAfter; tempHeader != NULL; tempHeader = next) {
        if (tempHeader->menulist == chainee) {
            if (tempHeader->assocKey != key)
                tempHeader->assocKey = key;
            return;
        }
        else if (tempHeader->assocKey == key) {
            *previous = next = tempHeader->next;
            free(tempHeader);
            continue;
        }
        next = tempHeader->next;
    }

    tempHeader = (struct headerlist *) malloc(sizeof(struct headerlist));
    tempHeader->menulist = chainee;
    tempHeader->next = self->menuChainAfter;
    tempHeader->assocKey = key;
    self->menuChainAfter = tempHeader;
    self->version = nextMLVersion;
}

void menulist__UnchainML(self, key)
    struct menulist *self;
    long key;
{

    struct headerlist *traverse, **previous;

    previous = &(self->menuChainBefore);
    for (traverse = self->menuChainBefore; traverse != NULL && (traverse->assocKey != key); traverse = traverse->next)
        previous = &(traverse->next);

    if (traverse == NULL) {
        previous = &(self->menuChainAfter);
        for (traverse = self->menuChainAfter; traverse != NULL && (traverse->assocKey != key); traverse = traverse->next)
            previous = &(traverse->next);
    }

    if (traverse != NULL) {
        *previous = traverse->next;
        free(traverse);
        self->version = nextMLVersion;
    }
}

struct menulist *menulist__GetChainedML(self, key)
    struct menulist *self;
    long key;
{

    struct headerlist *traverse;

    for (traverse = self->menuChainBefore; traverse != NULL; traverse = traverse->next)
        if (traverse->assocKey != key)
            return traverse->menulist;
    for (traverse = self->menuChainAfter; traverse != NULL; traverse = traverse->next)
        if (traverse->assocKey != key)
            return traverse->menulist;
    return NULL;
}

void menulist__ClearChain(self)
    struct menulist *self;
{

    boolean didSomething = FALSE;
    struct headerlist *traverse, *next;

    for (traverse = self->menuChainBefore; traverse != NULL; traverse = next) {
        next = traverse->next;
        free(traverse);
        didSomething = TRUE;
    }
    self->menuChainBefore = NULL;
    for (traverse = self->menuChainAfter; traverse != NULL; traverse = next) {
        next = traverse->next;
        free(traverse);
        didSomething = TRUE;
    }
    self->menuChainAfter = NULL;

    if (didSomething)
        self->version = nextMLVersion;
}

int menulist__NextMLVersion(classID)
    struct classheader *classID;
{
    return nextMLVersion;
}

void menulist__IncrementMLVersion(classID)
    struct classheader *classID;
{
    ++nextMLVersion;
}
