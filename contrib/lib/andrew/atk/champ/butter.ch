/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
class butter: dataobject[dataobj] {
    classprocedures:
      InitializeObject(struct butter *self) returns boolean;
    methods:
      SetText(char *txt);
      SetHitFunction(procedure HitFunction);
      SetButtonFont(struct fontdesc *f);
      SetRocks(char *r1, int r2);
    macromethods:
      GetText() (self->text)
      GetHitFunction() (self->HitFunction)
      GetButtonFont() (self->myfontdesc)
      GetRockPtr() (self->rockptr)
      GetRockInt() (self->rockint)
    data:
      char *text, *rockptr, *myfontname;
      int (*HitFunction)( /* rockptr, rockint, struct butter *, enum view_MouseAction */ );
      long rockint, myfonttype, myfontsize;
      struct fontdesc *myfontdesc;
      struct cursor *mycursor;
};

