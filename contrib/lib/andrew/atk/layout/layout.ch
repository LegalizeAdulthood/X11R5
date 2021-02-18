/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header $ */
/* $ACIS$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/layout/RCS/layout.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidlayout_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/layout/RCS/layout.ch,v 1.3 1991/09/12 19:42:10 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* layout.ch - layout data object definition and interface */

#define layout_VERSION 1
#define SafeFromNull(s) (((s) != NULL) ? (s) : "(null)")

#define getDataObject(self) ((self) -> header.dataobject)


/* globals for entire package */

boolean	debug;				/* nonzero if debugging */

/* component data structure */

/*
Components are stored in a simple linked list.  Order is significant,
with earlier objects in the list drawn in front of later ones.

The forallcomponents(self, c) macro provides a loop down the list.
The cXxxxx macros provide geometry information.
*/

struct component {
    struct component *nextcomponent;	/* forward link in component list */
    struct dataobject *data;		/* embedded data object or NULL if none */
    long left, top, width, height;	/* rectangle containing imbedded object */
    boolean varies:1;			/* TRUE if can be changed in execution mode */
};

#define forallcomponents(self, c) for (c = (self)->firstcomponent; c != NULL; c = c->nextcomponent)

#define cLeft(c)  ((c)->left)
#define cTop(c)  ((c)->top)
#define cRight(c) ((c)->left + (c)->width)
#define cBottom(c) ((c)->top + (c)->height)
#define cWidth(c) ((c)->width)
#define cHeight(c) ((c)->height)

#define cData(c) ((c)->data)
#define cVaries(c) ((c)->varies)


/* actual interface definition */

class layout: dataobject[dataobj] {

overrides:
  ViewName() returns char *;
  Read (FILE * f, long id) returns long;
  Write (FILE * f, long writeid, int level) returns long;
  GetModified() returns long;

methods:
  ToggleDebug();
  RemoveComponent(struct component *c);
  FillInComponent(char *name, struct component *c);
  CreateComponent() returns struct component *;
  SetComponentSize(struct component *c, long x, long y, long w, long h);
  Promote(struct component *c);
  Demote(struct component *c);
  MakeVariable(struct component *c);
  MakeFixed(struct component *c);

macromethods:
  GetFirstComponent() (self->firstcomponent)

classprocedures:
  InitializeClass() returns boolean;
  InitializeObject(struct layout *self) returns boolean;
  FinalizeObject(struct layout *self);

data:
  struct component *firstcomponent;	    /* first in component list */
  int inGetModified;			    /* recursion protection switch */
};

/* end of layout.ch */
