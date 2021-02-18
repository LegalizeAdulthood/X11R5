/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/cltextv.ch,v 1.3 1991/09/12 19:51:47 bobg Exp $ */
/* $ACIS: $ */
/* $Source:  */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_cltextview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/cltextv.ch,v 1.3 1991/09/12 19:51:47 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#define cltextview_PREPROCESS 0
#define cltextview_POSTPROCESS 1

struct cltextview_observer
{
  struct basicobject * observer;
  procedure callBack;
  long rock;
};

class cltextview[cltextv] : textview[textv]  { 
    overrides:
      GetClickPosition(long position, long numberOfClicks, enum view_MouseAction action, long startLeft, long startRight, long *leftPos, long *rightPos);
      FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);

    methods:
      AddClickObserver( struct basicobject * observer,
			procedure callBack, long rock );
      RemoveClickObserver( struct basicobject * observer );
      RemoveClick( struct basicobject * observer,
		 procedure callBack );
    classprocedures:
      FinalizeObject(struct cltextview *self);
      InitializeObject(struct cltextview *self) returns boolean;
    data:
      short maxObservers;			/* number of entries in observers table */
      struct cltextview_observer *observers;	/* table of observers */
      struct cursor *cursor;
};

