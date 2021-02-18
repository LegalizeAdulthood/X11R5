/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/mbuttonv.ch,v 1.1 1991/09/30 18:16:13 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/mbuttonv.ch,v $ */
#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_mbuttonv_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/mbuttonv.ch,v 1.1 1991/09/30 18:16:13 susan Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#include <fontdesc.ih>
#define mbuttonv_NONE 0
#define mbuttonv_SURROUND 1
#define mbuttonv_ADJACENT 2
#define mbuttonv_EITHER 3
class mbuttonv[mbuttonv] : valueview[valuev]
{
 overrides:
Hit(enum view_MouseAction type, long x, long y, long numberOfClicks)
    returns struct view *;
  DrawFromScratch(long x,y,width,height);
  DrawHighlight();
  DrawDehighlight();
  DrawNewValue();
  DoHit(enum view_MouseAction type, long x,y,hits)
    returns struct mbuttonv *;
  LookupParameters();
 classprocedures:
  InitializeClass() returns boolean;
macromethods:
  SetButtonType(TYPE) (self->buttontype = TYPE)
  GetButtonType() (self->buttontype)
 data:
  char * label;
  char * fontname;
  short fontsize;
  short buttontype;
  short count;
  long max,rtl,rhw,x,y;
  struct fontdesc * activefont;
    double foreground_color[3], background_color[3];
boolean pushed,mono,valueset,movedoff,vertical;
    struct FontSummary *FontSummary;
	struct list *list;
	struct mbuttonv_rl *current;
};

/* parameters to spst
   label		string		Defaults to NULL
   bodyfont		string		Defaults to "andytype"
   bodyfont-size	long		Defaults to 10
*/
