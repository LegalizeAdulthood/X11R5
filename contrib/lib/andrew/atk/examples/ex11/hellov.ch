/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/examples/ex11/RCS/hellov.ch,v 2.7 1991/09/12 19:34:31 bobg Exp $ */
/* Mouse drag, key bindings, menus, scroll bar, messages, prompting for input */

class helloworldview[hellov]: view {
overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long height);
    Update();
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    ReceiveInputFocus();
    LoseInputFocus();
    GetApplicationLayer() returns struct view *;
    DeleteApplicationLayer(struct view *v);
    GetInterface(int type) returns char *;
classprocedures:
    InitializeClass() returns boolean;
data:
    struct keystate *keystate;
    struct menulist *menulist;
    boolean haveInputFocus;
    boolean HaveDownTransition;
    long hitX,hitY;
    long x,y;
    boolean blackOnWhite;
    long frameX, frameY;
    long newFrameX, newFrameY;
    int vrWidth,vrHeight;
};
