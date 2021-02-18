/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
class link: pushbutton[pshbttn] {
    classprocedures:
      InitializeClass() returns boolean;
      InitializeObject(struct link *self) returns boolean;
      FinalizeObject(struct link *self);
    overrides:
      Read (FILE *file, long id) returns long;
      Write (FILE *file, long writeid, int level) returns long;
    methods:
      SetLink(char *link);
      SetPos(long pos);
      SetLen(long len);
    macromethods:
      GetLink() (self->link)
      GetPos() (self->pos)
      GetLen() (self->len)
      LabelSetP() (self->label_set)
    data:
      char *link;
      long pos;
      long len;
      int label_set;
#ifdef PL8
      int new_ds;
#endif
};

