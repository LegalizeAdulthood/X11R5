/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

class helloworldview[hellov] : view {
 overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long
width, long height);
    Update();
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks)
returns struct view *;
    ReceiveInputFocus();
    LoseInputFocus();
 classprocedures:
    InitializeClass()returns boolean;
 data:
    struct keystate *keystate;
    long x,y;
    long distX,distY;
    long newX,newY;
    boolean HaveDownTransition;
    boolean haveInputFocus;
    boolean blackOnWhite;
    boolean newBlackOnWhite;
};

