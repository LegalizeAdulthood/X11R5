/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/tindex.ch,v 1.2 1991/09/12 20:03:12 bobg Exp $ */
/* $ACIS:tindex.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/tindex.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtindex_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/tindex.ch,v 1.2 1991/09/12 20:03:12 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

package tindex {
    classprocedures:
      IndexTerm(struct text *d,char *term,char **error)returns int;
      WriteIndexList(struct text *d,FILE *f);
      ReadIndexList(struct text *d,FILE *f) returns char *;
      InitializeClass() returns boolean;
      PrintIndex(struct view *self);
      PreviewIndex(struct view *self);
};

