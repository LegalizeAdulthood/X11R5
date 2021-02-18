/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/frame/RCS/framev.ch,v 2.8 1991/09/12 19:40:07 bobg Exp $ */
/* $ACIS:framev.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/frame/RCS/framev.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidframeview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/frame/RCS/framev.ch,v 2.8 1991/09/12 19:40:07 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#define frameview_VERSION 1

class frameview[framev] : textview[textv] {
    overrides:
        Update();
        WantInputFocus(struct view *requestor);
        LoseInputFocus();
        PostMenus(struct menulist *menulist);
        PostKeyState(struct keystate *keystate);

    methods:
        SetMessageLine(struct framemessage *messageLine);
        GetMessageLine() returns struct framemessage *;
	Return(long key); 
	Help(long key); 
	Complete(long key); 

    classprocedures:
        FinalizeObject(struct frameview *self);
        Create(struct framemessage *messageLine) returns struct frameview *;
        InitializeClass() returns boolean;
	InitializeObject(struct frameview *self) returns boolean;    

    data:
        struct framemessage *messageLine;
        struct keystate *keystate;
        struct menulist *menulist;
        struct mark *transientMark;
	struct event *event; /* Timer evet to erase transient messages. */
	boolean amLosingInputFocus;
};

