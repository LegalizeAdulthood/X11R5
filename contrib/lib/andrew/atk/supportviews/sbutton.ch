 /* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#include <rect.h>

#define DS_VERSION 10 /* datastream version */

/*
  Appearances for pushbutton:

  Style  Look
    0    plain, in line text
    1    a boxed rectangle
    2    a motif-ish 3-Dish button
    3    a single rectangle
    4	 a more motif-ish button
*/
#define sbutton_PLAIN 0
#define sbutton_BOXEDRECT 1
#define sbutton_THREEDEE 2
#define sbutton_PLAINBOX 3
#define sbutton_MOTIF 4

#define sbutton_SIZECHANGED 1
#define sbutton_TRIGGERCHANGED 2
#define sbutton_LABELCHANGED 4
#define sbutton_FONTCHANGED 8
#define sbutton_STYLECHANGED 16
#define sbutton_ACTIVATIONCHANGED 32
#define sbutton_ALLCHANGED 64

#define sbutton_CHANGEBASE 128  /* reserve 128 values (no relation to sbutton_ALLCHANGED is implied...) for specifying other change details, numbers>128 indicate that button #-128 has had a change in activation status */

#define sbutton_BACKGROUND 0
#define sbutton_TOPSHADOW 1
#define sbutton_TOP 2
#define sbutton_BOTTOMSHADOW 3
#define sbutton_FOREGROUND 4
#define sbutton_LABELFG 5
#define sbutton_LABELBG 6
#define sbutton_COLORS 7

struct sbutton_prefs {
    int writeid;
    int refcount;
    int style;
    struct fontdesc *font;
    char *colors[sbutton_COLORS];
    char *name;
};

struct sbutton_info {
    struct sbutton_prefs *prefs;
    char *label;
    long rock;
    struct atom *trigger;
    boolean lit;
};


struct sbutton_list {
    char *label;
    long rock;
    char *trigger;
    boolean lit;
};

enum sbutton_sizepolicy {
    sbutton_NoneSet,
    sbutton_SizeFixed,
    sbutton_GrowRows,
    sbutton_GrowColumns,
    sbutton_ResizeFunction
};

class sbutton: dataobject[dataobj] {
  classprocedures:
    InitializeClass() returns boolean;
    InitializeObject(struct sbutton *self) returns boolean;
    FinalizeObject(struct sbutton *self);
    CreateSButton(struct sbutton_prefs *prefs) returns struct sbutton *;
    CreateFilledSButton(struct sbutton_prefs *prefs, struct sbutton_list *blist) returns struct sbutton *;
    GetNewPrefs(char *name) returns struct sbutton_prefs *;
    InitPrefs(struct sbutton_prefs *prefs, char *name);
    FreePrefs(struct sbutton_prefs *prefs);
    WritePrefs(FILE *fp, struct sbutton_prefs *prefs);
    DuplicatePrefs(struct sbutton_prefs *prefs, char *newname) returns struct sbutton_prefs *;
    
  overrides:
    ViewName() returns char *;
    Read (FILE *fp, long id) returns long;
    Write (FILE *fp, long id, int level) returns long;
    NotifyObservers(long value);
    
  methods:

    ActivateButton(int ind);
    DeActivateButton(int ind);

    Actuate(int index);
    SetRock(int index, long rock);
    SetLayout(int rows, int cols, enum sbutton_sizepolicy);
    SetLabel(int index,char *txt);
    SetLit(int index, boolean litp);
    SetPrefs(int index, struct sbutton_prefs *);
    Enumerate(boolean (*func)(),long rock) returns int;
    SetTrigger(int index, char *name);
    Delete(int index);
    Swap(int i1, int i2);
    SetChangeFlag(long on);
    GetChangeType() returns long;
    
  macros:
    GetName(prefs) ((prefs)->name)
    GetStyle(prefs) ((prefs)->style)
    GetFont(prefs) ((prefs)->font)
    GetColor(prefs, color) ((prefs)->colors[color])
    GetLabelFG(prefs) ((prefs)->colors[sbutton_LABELFG])
    GetLabelBG(prefs) ((prefs)->colors[sbutton_LABELBG])
    GetBackground(prefs) ((prefs)->colors[sbutton_BACKGROUND])
    GetForeground(prefs) ((prefs)->colors[sbutton_FOREGROUND])
    GetTop(prefs) ((prefs)->colors[sbutton_TOP])
    GetTopShadow(prefs) ((prefs)->colors[sbutton_TOPSHADOW])
    GetBottomShadow(prefs) ((prefs)->colors[sbutton_BOTTOMSHADOW])
    GetRefCount(prefs) ((prefs)->refcount)
    
  macromethods:
    GetRock(index) (((index)<(self)->count && (index)>=0)?(self)->buttons[index].rock:0)
    GetLit(index) (((index)<(self)->count && (index)>=0)?(self)->buttons[index].lit:FALSE)
    GetLabel(index) (((index)<(self)->count && (index)>=0)?(self)->buttons[index].label:NULL)
    GetPrefs(index) (((index)<(self)->count && (index)>=0)?(self)->buttons[index].prefs:NULL)
    GetTrigger(index) (((index)<(self)->count && (index)>=0)?(self)->buttons[index].trigger:NULL)

    GetSingleActivationFlag((self)->singleactivation);
    GetButtons() ((self)->buttons)
    GetRows() ((self)->rows)
    GetCols() ((self)->cols)
    GetSizePolicy() ((self)->sizepolicy)
    GetResizeRock() ((self)->rezizerock)
    GetResizeFunc() ((self)->resizefunc)
    GetHitFunc() ((self)->hitfunc)
    GetHitFuncRock() ((self)->hitfuncrock)
    GetCount() ((self)->count)
    GetDefaultPrefs() ((self)->prefs)
    GetMattePrefs() ((self)->matteprefs)
      
  data:
    /* Resizefunc is called as resizefunc(self, resizerock) when the dimensions of the button box are insufficient for it's content. NOTE: this has nothing to do with it's physical dimensions on screen, only with how many rows and columns the buttons are broken up into. */
    procedure resizefunc;
    long resizerock;
    /* Hitfunc is called as hitfunc(self, hitfuncrock, buttonindex, buttonrock) called when a mouse button is released over an activated button. Button is the position of the button's information in the buttons array. */
    procedure hitfunc;
    long hitfuncrock;
    
    int maxcount;
    int count;
    enum sbutton_sizepolicy sizepolicy;
    int rows,cols;
    struct sbutton_info *buttons; /* pointer to an array of count sbutton_info structures or null if no buttons have been created */
    int triggerchange; /* the index of the last button whose trigger changed */
    long change; /* a bitwise or of the change #defines */
    int activated; /* which button is currently active */
    boolean singleactivation; /* true if only one button may be active at a time */
    struct sbutton_prefs *prefs; /* the default set of preferences to use */
    struct sbutton_prefs *matteprefs; /* the prefs to use for the matte */
};

