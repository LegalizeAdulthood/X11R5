/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/annot/RCS/iconview.ch,v 1.3 1991/09/12 20:15:34 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/annot/RCS/iconview.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	char *sketch_ch_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/annot/RCS/iconview.ch,v 1.3 1991/09/12 20:15:34 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class iconview : view
{
  overrides:
    GetOrigin(long width, long height, long *originX, long *originY);
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    ReceiveInputFocus();
    Update();
    FullUpdate(enum view_UpdateType type, long left, long top,
		long width, long right);
    DesiredSize(long width, long height, enum view_DSpass pass,	
		 long *dWidth, long *dheight) 
      returns enum view_DSattributes;
    SetDataObject(struct dataobject * do);
    ObservedChanged(struct thisobject * data, long value);
    UnlinkTree();
 methods:
    DecidedSize(w,h);
    RecommendSize(w,h);
    Open();
    Close();
    SetIconFont(char *iconfont, int iconstyle, int iconpts);
    SetIconChar(int iconchar);
    SetTitleFont(char *titlefont, int titlestyle, int titlepts);
    SetChild(char *viewclass);
    GetChild() returns struct view *;
classprocedures:
    InitializeObject(struct thisobject *self) returns boolean;
    InitializeClass() returns boolean;
    FinalizeObject(struct view *self);
    CloseRelated(struct view *v);
    OpenRelated(struct view *v);
  data:
    struct fontdesc * titlefont;
    char iconchar;
    struct fontdesc * iconfont;
    struct view * child;
    struct view * bottomview;
    int	isopen;
    long cx, cy, cw, ch;   /* childs extents in my logical space */
    long dw, dh;    /* desired hight and width */
    short neednewsize;
    struct iconview *next;
};
