/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/compress/RCS/compresv.ch,v 1.2 1991/09/12 20:14:39 bobg Exp $
*/

struct envlist {
    struct environment *envr;
    struct text *txt;
    struct compress *cp;
    struct envlist *nextenv,*prevenv;
    };/*RSKadd*/

/*RSKmod butchered from 'fnotev.ch'*/

class compressv [compresv]: view {
overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
    Update();
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
/*    ObservedChanged (struct observable *changed, long value);*//*RSKmod remove*/
    DesiredSize(long width, long height, enum view_DSpass pass, long *dWidth, long *dheight) returns enum view_DSattributes;
    LinkTree(struct view *parent);
    GetOrigin(long width, long height, long *originX, long *originY);
/*methods:
    pushchild();
    popchild();*//*RSKmod remove*/
classprocedures:
    InitializeClass() returns boolean;
    InitializeObject(struct compressv *self) returns boolean;/*RSKadd*/
    FinalizeObject(struct compressv *self);/*RSKadd*/
    DecompressAll();/*RSKadd*/
    MakeBox(struct compress *cprs);/*RSKadd*/
    HideBoxes();/*RSKadd*/
    UnhideBoxes();/*RSKadd*/
data:
/*    struct impair *imp;*//*RSKremove*/
    struct fontdesc *fd;
    struct cursor *cursor;
    struct text *parenttext;
    struct textview *parentview;
    struct compressv *nextcompress,*prevcompress;
};

