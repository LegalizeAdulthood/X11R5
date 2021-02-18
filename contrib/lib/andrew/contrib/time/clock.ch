/* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* #include <time.h> */
#include <fontdesc.ih>
#include <event.ih>

struct clock_time {
  double hours, minutes, seconds;
};

enum border_shapes {circle = 0, square = 1};

struct clock_options {
  long timestamp;
  int hours_width, minutes_width, seconds_width, border_width;
  int hours_length, minutes_length, seconds_length; /* -100 -> 100 */
  enum border_shapes border_shape;
  int major_ticks, minor_ticks;
  int tick_length;
  int num_labels;
  char **labels;
  char *fontfamily;
  int fontface;
};

class clock: dataobject[dataobj] {
    classprocedures:
      InitializeClass() returns boolean;
      InitializeObject(struct clock *self) returns boolean;
      FinalizeObject(struct clock *self);
    overrides:
      Read (FILE *fp, long id) returns long;
      Write (FILE *fp, long id, int level) returns long;
    methods:
      SetOptions(struct clock_options *options);
    macromethods:
      ReadClock() (&(self->clockface))
      GetOptions() (&(self->options));
    data:
      struct clock_time clockface;
      struct clock_options options;
      long now;
      long epoch;
      struct event *ev;
};

