/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/nbutterv.c,v 1.1 1991/09/27 20:36:16 rr2b Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/nbutterv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/nbutterv.c,v 1.1 1991/09/27 20:36:16 rr2b Exp $ ";
#endif /* lint */
#include <class.h>

#include <sbutton.ih>
#include "nbutterv.eh"

boolean nbutterv__Touch(self, ind, action)
struct nbutterv *self;
int ind;
enum view_MouseAction action;
{ 
    struct sbutton *b=nbutterv_ButtonData(self);

    sbutton_GetHitFuncRock(b)=action;
    
    return super_Touch(self, ind, action);

}
