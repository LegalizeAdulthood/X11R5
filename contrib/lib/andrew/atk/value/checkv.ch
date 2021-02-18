#include <fontdesc.ih>
#include <rect.h>

class checkv[checkv] : valueview[valuev]
{
 overrides:
  DrawFromScratch(long x,y,width,height);
  DrawHighlight();
  DrawDehighlight();
  DrawNewValue();
  DoHit(enum view_MouseAction type, long x,y,hits)
    returns struct checkv *;
  LookupParameters();
macromethods:
  GetTmpVal() (self->tmpval)
classprocedures:
  InitializeClass() returns boolean;
data:
  char *label;
  char *fontname;
  struct fontdesc *normalfont;
  struct graphic *BlackPattern;
  short fontsize;
  long  tmpval;
  short checktype;
  long x,y,width,height;
  char *foreground,*background;
};
