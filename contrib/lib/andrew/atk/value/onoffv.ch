/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/onoffv.ch,v 2.8 1991/09/30 18:26:45 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/onoffv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_onoffV_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/onoffv.ch,v 2.8 1991/09/30 18:26:45 susan Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#include <fontdesc.ih>
#include <rect.h>

class onoffV[onoffv] : buttonV[buttonv]
{
 overrides:
  LookupParameters();
classprocedures:
  InitializeClass() returns boolean;
  InitializeObject(struct thisobject *self) returns boolean;
data:
};

/* parameters to spst
   label		string		Defaults to NULL
   bodyfont		string		Defaults to "andytype"
   bodyfont-size	long		Defaults to 10
   max-value		long		defaults to 100
  min-value		long		dafaults to 0
  increment		float		defaults to 1.0
*/
