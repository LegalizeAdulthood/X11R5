/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header $ */
/* $ACIS$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/layout/RCS/box.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidbox_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/layout/RCS/box.ch,v 1.2 1991/09/12 19:41:54 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* box.ch - box data object definition and interface */

#include <rect.h>

#define box_VERSION 1
#define SafeFromNull(s) (((s) != NULL) ? (s) : "(null)")

#define getDataObject(self) ((self) -> header.dataobject)


/* globals for entire package */

boolean	debug;				/* nonzero if debugging */

/* actual interface definition */

class box: dataobject[dataobj] {

overrides:
  ViewName() returns char *;
  Read (FILE * f, long id) returns long;
  Write (FILE * f, long writeid, int level) returns long;
  GetModified() returns long;

methods:
  ToggleDebug();
  FillInContents(char *name);

macromethods:

classprocedures:
  InitializeClass() returns boolean;
  InitializeObject(struct box *self) returns boolean;
  FinalizeObject(struct box *self);

data:
  int inGetModified;			    /* recursion protection switch */
  struct dataobject *contents;		    /* contained object */
};

/* end of box.ch */
