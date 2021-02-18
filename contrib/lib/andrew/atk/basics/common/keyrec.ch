/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/keyrec.ch,v 2.7 1991/09/12 19:22:23 bobg Exp $ */
/* $ACIS:keyrec.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/keyrec.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidkeyrec_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/keyrec.ch,v 2.7 1991/09/12 19:22:23 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#define keyrec_VERSION 1

#include <view.ih>

enum keyrec_EventType {
    keyrec_Keystroke,
    keyrec_Menu,
    keyrec_MouseHit
};

union keyrec_KeyValue {
    enum view_MouseAction action;
    long val;
    char *unknown;
};

struct keyitem {
    struct keyitem *next;
    enum keyrec_EventType type;
    struct view *view;
    union keyrec_KeyValue parm[3];
};

class keyrec {
methods:
    Clear();
    StartRecording() returns boolean;	/* Returns FALSE if already recording */
    StopRecording() returns boolean;	/* Returns FALSE if not recording */
    Recording() returns boolean;	/* Returns TRUE if currently recording */
    Copy() returns struct thisobject *;
    RecordEvent(enum keyrec_EventType type, struct view *view, union keyrec_KeyValue value1, union keyrec_KeyValue value2, union keyrec_KeyValue value3);
    StartPlaying() returns boolean;
    NextKey(enum keyrec_EventType *type, struct view **view, union keyrec_KeyValue *value1, union keyrec_KeyValue *value2, union keyrec_KeyValue *value3) returns boolean;
    StopPlaying();
classprocedures:
    InitializeObject(struct keyrec *self) returns boolean;
    FinalizeObject(struct keyrec *self);
data:
    boolean recording;
    struct keyitem *head;
    struct keyitem *tail;
    boolean playing;
    struct keyitem *current;
};
