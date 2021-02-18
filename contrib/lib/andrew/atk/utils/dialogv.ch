/* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#include <view.ih>

struct dialogv_HitRock {
    procedure func;
    long rock;
};

class dialogv : view {
classprocedures:
    InitializeClass() returns boolean;
    InitializeObject(struct sbutton *self) returns boolean;
    FinalizeObject(struct sbutton *self);
    Create(char **list, char *font, int style) returns struct dialogv *;
    
overrides:
    SetDataObject(struct dataobject *dataobject);
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);

    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    DesiredSize(long width, long height, enum view_DSpass pass, long *dWidth, long *dheight) returns enum view_DSattributes;
    LinkTree(struct view *parent);
    PostMenus(struct menulist *ml);
    ReceiveInputFocus();
    
macromethods:
    GetTextView() ((self)->text)
    GetButtonsView() ((self)->buttons)
    GetTextData() ((struct text *)view_GetDataObject((struct view *)(self)->text))
    GetButtonsData() ((struct sbutton *)view_GetDataObject((struct view *)(self)->buttons))
    GetExtraMenus() ((self)->extramenus)
    GetExtraKeys()  ((self)->extrakeys)
    
methods:
   PostChoice(struct im *im, struct view *client, boolean *cflag, int deflt, boolean blocking, long pos) returns int;
   PostInput(struct im *im, struct view *client, procedure choicefunc, long choicerock, boolean blocking, long pos) returns int;
   SetLayout(int rows, int cols);
   InstallRider(struct view *rider);
   InstallSidekick(struct view *sidekick);
   SetExtraMenus(struct menulist *ml);
   SetExtraKeyState(struct keystate *ks);
   ActivateButton(int ind);
   DeActivateButton(int ind);
   Vanish();
   UnVanish();
   CancelQuestion();
   
data:
    struct textview *text;
    struct sbuttonv *buttons;
    struct view *rider, *sidekick;
    long rwidth, rheight, roffset;
    long twidth, theight;
    long swidth, sheight;
    long bwidth, bheight;
    boolean destroyall;
    struct dialogv_HitRock hr;
    struct menulist *extramenus;
    struct keystate *extrakeys;
    struct view *client;
    boolean didsizeonce;
    int lastchoice;
    boolean *cont;
};

