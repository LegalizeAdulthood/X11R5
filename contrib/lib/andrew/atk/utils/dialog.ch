/* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#include <sbutton.ih>
#define dialog_DS_VERSION 5

class dialog : dataobject[dataobj] {
classprocedures:
    InitializeClass() returns boolean;
    InitializeObject(struct sbutton *self) returns boolean;
    FinalizeObject(struct sbutton *self);
    
overrides:
    ViewName() returns char *;
    Read (FILE *fp, long id) returns long;
    Write (FILE *fp, long id, int level) returns long;

macromethods:
    GetStyle() (sbutton_GetStyle(self->prefs))
    GetText() ((self)->text)
    GetButtons() ((self)->buttons)
    GetHitFunc() ((self)->buttons->hitfunc)
    GetPrefs() ((self)->prefs)

methods:
    SetButtons(struct sbutton *buttons);
    GetForeground() returns char *;
    GetBackground() returns char *;
    SetText(struct text *text);
    
data:
    struct text *text;
    struct sbutton *buttons;
    struct sbutton_prefs *prefs;
};

