/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#define BUTT_SYS 0
#define BUTT_YEAR 1
#define BUTT_MON 2
#define BUTT_DAY 3
#define BUTT_WKDAY 4
#define BUTT_WKDAYSELECT 5
#define BUTT_HR 6
#define BUTT_MIN 7
#define BUTT_ADD 8
#define BUTT_DEL 9
#define NUMBUTTS 10 /* one plus previous */

class enodeview[enodev]:lpair {
    classprocedures:
      InitializeObject(struct enodeview *self) returns boolean;
    overrides:
      ObservedChanged (struct thisobject *changed, long value);
    methods:
      SetChimpview(struct chimpview *);
    data:
      struct butter *butts[NUMBUTTS];
      struct chimpview *mychimpview;
};
