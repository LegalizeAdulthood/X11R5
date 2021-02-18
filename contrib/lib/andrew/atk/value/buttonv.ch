/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/buttonv.ch,v 2.9 1991/09/30 18:26:45 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/buttonv.ch,v $ */
#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_mbuttonv_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/buttonv.ch,v 2.9 1991/09/30 18:26:45 susan Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#include <fontdesc.ih>
#include <sbutton.ih>

#define buttonV_NONE 0
#define buttonV_SURROUND 1
#define buttonV_ADJACENT 2
#define buttonV_EITHER 3
class buttonV[buttonv] : valueview[valuev]
{
 overrides:
Hit(enum view_MouseAction type, long x, long y, long numberOfClicks)
    returns struct view *;
  DrawFromScratch(long x,y,width,height);
  DrawHighlight();
  DrawDehighlight();
  DrawNewValue();
  DoHit(enum view_MouseAction type, long x,y,hits)
    returns struct buttonV *;
  LookupParameters();
methods:
  CacheSettings();
  DrawButtonText(char *text,long len,struct rectangle *rect,struct rectangle *rect2,boolean pushd);
  HandleStyleString(char *s);
classprocedures:
  InitializeClass() returns boolean;
  InitializeObject() returns boolean;
  FinalizeObject(); 
macromethods:
  SetButtonType(TYPE) (self->buttontype = TYPE)
  GetButtonType() (self->buttontype)
  SetFixedCount(VAL) (self->fixedcount = (VAL))
  GetFixedCount() (self->fixedcount)
  SetFourway(VAL) (self->fourway = (VAL))
  GetFourway() (self->fourway)
  SetFixedColumns(VAL) (self->fixedcolumns = self->columns = (VAL))
  GetFixedColumns() (self->fixedcolumns)
 data:
  char * label;
  char *l[4];
  char * fontname;
  short fontsize;
  short buttontype;
  short count,fixedcount;
  long max,rtl,rhw,x,y;
  struct fontdesc * activefont;
  boolean pushed,valueset,movedoff,vertical,topdown,fourway;
  struct list *list;
  struct buttonV_rl *current;
  long bcnt,offset,columns,bsize,fixedcolumns; 
  struct sbutton_info si; /* TO BE REMOVED */
  struct sbutton_prefs *prefs;
  int mono;
};

/* parameters to spst
   label		string		Defaults to NULL
   bodyfont		string		Defaults to "andytype"
   bodyfont-size	long		Defaults to 10
*/
