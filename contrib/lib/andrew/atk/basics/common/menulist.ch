/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/menulist.ch,v 2.11 1991/09/12 19:22:32 bobg Exp $ */
/* $ACIS:menulist.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/menulist.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidmenulist_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/menulist.ch,v 2.11 1991/09/12 19:22:32 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* menulist.H
 * Class header file for menulist package.
 *
 */

#define menulist_GetMLVersion(alist)	((alist)->version)
#define menulist_RewindML(alist)		((alist)->curMenu = (alist)->menus)
#define menulist_RewindBeforeMC(alist)	((alist)->curChainBefore = (alist)->menuChainBefore)
#define menulist_RewindAfterMC(alist)	((alist)->curChainAfter = (alist)->menuChainAfter)

struct itemlist {
    struct itemlist *next;		/* next guy in the list */
    char *string;			/* what to put in the menu */
    long enableMask;			/* used to enable this item */ 
    long functionData;			/* sent when we call the procedure */
    struct proctable_Entry *proc;	/* what to call if selected */
};

struct headerlist {
    struct headerlist *next;
    long assocKey;				/* Association key. */
    struct menulist *menulist;		/* The menulist this link points to. */
};

class menulist { /* Maybe should be subclass of observe. */
    methods:
        DuplicateML(struct view *view) returns struct menulist *;
        SetView(struct view *view);
        AddToML(char *menuString, struct proctable_Entry *menuProc, long functionData, long mask);
        DeleteFromML(char *menustring);
        ClearML();
        NextME(char **outString, long *outData, struct proctable_Entry **outProc) returns boolean;
        NextBeforeMC() returns struct menulist *;
        NextAfterMC() returns struct menulist *;
        ChainBeforeML(struct menulist *chainee, long key);
        ChainAfterML(struct menulist *chainee, long key);
        UnchainML(long key);
        GetChainedML(long key) returns struct menulist *;
        ClearChain();
	SetMask(long mask) returns boolean;  /* TRUE if changed */

    macromethods:
        GetMask() (self->selectMask)
      /*item is of type "struct itemlist *" */
        ItemIsEnabled(item) \
            ((self->selectMask&(item)->enableMask)==(item)->enableMask)

    classprocedures:
    InitializeObject(struct menulist *self) returns boolean;
        FinalizeObject(struct menulist *self);
        Create(struct view *view) returns struct menulist *;
/* Version control functions for use by internal modules only. */
        NextMLVersion() returns int;
        IncrementMLVersion();

    data:
        long version;				/* version of the menu */
	long menuVersion;			/* version of self->menus */
        struct basicobject *object;		/* The guy who owns these menus. */
        struct itemlist *menus;			/* ptr to list of words and procs to call */
        struct itemlist *curMenu;		/* the current one */
        struct headerlist *menuChainBefore;	/* Pointer to linked list of before chained menulists */
        struct headerlist *menuChainAfter;	/* Pointer to linked list of after chained menulists */
        struct headerlist *curChainBefore;	/* the current before one */
        struct headerlist *curChainAfter;	/* the current after one */
        int *refcount;                          /* Pointer to a reference count for the menus field of this structure. */
	long selectMask, oldMask;		/* enables various items in list */
/* Window manager dependent fields. */
        long regionID;				/* region id for installed menu list; only needed for wm, wasted space for x. */
	long installVersion;			/* version displayed in wm */
	struct im *curIM;			/* the one we displayed in last */
};
