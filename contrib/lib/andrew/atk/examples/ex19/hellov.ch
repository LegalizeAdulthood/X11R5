/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/examples/ex19/RCS/hellov.ch,v 2.7 1991/09/12 19:36:42 bobg Exp $ */
/* Mouse drag, key bindings, menus, scroll bar, data object, messages, prompting for input, GetApplicationLayer */

#include "rect.h"

class helloworldview[hellov]: view {
overrides:
    SetDataObject(struct helloworld *hw);
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
    Update();
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    ReceiveInputFocus();
    LoseInputFocus();
    GetInterface(int type) returns char *;
    GetApplicationLayer() returns struct view *;
    DeleteApplicationLayer(struct view *);
    LinkTree(struct view *parent);
    ObservedChanged(struct helloworld *obj, long val);
classprocedures:
    InitializeClass() returns boolean;
data:
    struct keystate *keystate;
    struct menulist *menulist;
    boolean HaveDownTransition;
    boolean haveInputFocus;
    long hitX,hitY;
    int x,y;
    boolean blackOnWhite;
    boolean redrawSubView;
    long frameX, frameY;
    long newFrameX, newFrameY;
    int vrWidth,vrHeight;
    struct view *view;
    struct view *applayer;
    struct cursor *idleCursor, *placeCursor, *dragCursor;
};
