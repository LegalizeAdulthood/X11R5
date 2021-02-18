/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/annot/RCS/dpstextv.ch,v 1.2 1991/09/12 20:15:27 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/annot/RCS/dpstextv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	char *sketch_ch_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/annot/RCS/dpstextv.ch,v 1.2 1991/09/12 20:15:27 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

class dpstextview[dpstextv] : view
{
  classprocedures:
    InitializeObject(struct thisobject *self) returns boolean;
    InitializeClass() returns boolean;
  overrides:
    Update();
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
  methods:
    SetDesired(w, h);
  macromethods:
    SetScaling(x, y, w, h) ((self)->scale_width = (w), (self)->scale_height = (h), (self)->offset_y = (y), (self)->offset_x = (x));
  data:
    int drawn_at_least_once;
    long cached_width, cached_height;
    long desired_width, desired_height;
    double scale_width, scale_height;
    long offset_x, offset_y;
};
