/* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#include <header.ih>
class headrtv: view {
classprocedures:
    InitializeObject(struct thisobject *self) returns boolean;
    InitializeClass() returns boolean;
    
overrides:
    PostKeyState(struct keystate *);
    Print(FILE *file,char *processor, char *finalFormat, boolean topLevel);
    WantInputFocus(struct view *requestor);
    LinkTree(struct view *parent);
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    SetDataObject(struct header *object);
    Update();
    ReceiveInputFocus();
    LoseInputFocus();
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
    DesiredSize(long width, long height, enum view_DSpass pass, long *dWidth, long *dheight) returns enum view_DSattributes;
methods:
data:
    struct fontdesc *font,*bfont,*bifont,*ifont;
    struct keystate *keystate;
    boolean open;
    long top;
    long closebox, wherebox;
    int my_focus;
    struct lpair *sections;
    struct textview *textvs[header_TEXTS];
};
