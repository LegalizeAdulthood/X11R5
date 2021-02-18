/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/dict.ch,v 2.6 1991/09/12 19:48:43 bobg Exp $ */
/* $ACIS:dict.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/dict.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsiddictionary_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/dict.ch,v 2.6 1991/09/12 19:48:43 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

package dictionary[dict] {
classprocedures :
    Insert(struct view *view,char *id, char *object) ;
    LookUp(struct view *view,char *id) returns char *;
    CountViews(char *id) returns long;
    ListViews(char *id,struct view **viewarray,long len) returns long;
    CountRefs(struct view *view) returns long;
    ListRefs(struct view *view,char **refarray,long len) returns long;
    Delete(struct view *view,char *id);
    InitializeClass() returns boolean;
};


