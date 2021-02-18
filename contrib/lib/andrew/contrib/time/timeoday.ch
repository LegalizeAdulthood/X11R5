/* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#include <fontdesc.ih>
#include <event.ih>

class timeoday: dataobject[dataobj] {
    classprocedures:
      InitializeClass() returns boolean;
      InitializeObject(struct timeoday *self) returns boolean;
      FinalizeObject(struct timeoday *self);
    overrides:
      Read (FILE *fp, long id) returns long;
      Write (FILE *fp, long id, int level) returns long;
    methods:
      SetFormat(char *format);
      SetFont(struct fontdesc *f);
      UpdateTime();
      FormatTime();
      WriteDataPart(FILE *fp);
      ReadDataPart(FILE *fp) returns long;
      InitializeDefaults() returns boolean;
    macromethods:
      GetTod() (self->tod)
      GetFont() (self->myfontdesc)
      GetTime() (self->now)
      GetFormat() (self->format)
      GetEvent() (self->ev)
      SetTime(thetime) (self->now = (thetime))
    data:
      char *tod, *format;
      long now;
      long epoch;
      struct event *ev;
      struct fontdesc *myfontdesc;
};

