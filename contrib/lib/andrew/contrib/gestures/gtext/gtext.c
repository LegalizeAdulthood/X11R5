#include <class.h>
#include <gtext.eh>

boolean
gtext__InitializeClass( classID )
    struct classheader *classID;
{
  return TRUE;
}

boolean
gtext__InitializeObject( classID, self )
    struct classheader *classID;
    struct gtext *self;
{
  return TRUE;
}

void
gtext__FinalizeObject( classID, self )
    struct classheader *classID;
    struct gtext *self;
{
}

char *
gtext__ViewName( self )
    struct gtext *self;
{
  return((char*)"gtextv");
}
