/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/thmbstrv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/thmbstrv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/thmbstrv.c,v 1.1 1991/09/30 18:16:13 susan Exp $ ";
#endif /* lint */

#include <class.h>
#include <value.ih>
#include <thmbstrv.eh>

char *thumbstrV__GetValueString(self)
struct thumbstrV *self;
{
    struct value *w = thumbstrV_Value(self);
    long len,val;
    char **arr;
    if(((len = value_GetArraySize(w)) == 0) || 
	((arr = value_GetStringArray(w)) == NULL) || 
	  ((val = thumbstrV_GetTmpVal(self))< 0) ||
	  val >= len){
	return super_GetValueString(self);
    }
    return(arr[val]);
}
struct thumbstrV * thumbstrV__DoHit( self,type,x,y,hits )
struct thumbstrV * self;
enum view_MouseAction type;
long x,y,hits;
{
    int mv = value_GetArraySize(thumbstrV_Value(self));
    struct thumbV *sf = (struct thumbV *) self;
    if(mv > 0 && mv != sf->maxval + 1 ){
	int diff ;
	sf->maxval = mv -1 ;
	diff = sf->maxval - sf->minval;
	if(diff < 20) sf->granular = 6;
	else if(diff < 50) sf->granular = 4;
	else if(diff < 100) sf->granular = 2;
	else sf->granular = 0;
    }
    return super_DoHit( self,type,x,y,hits );
}
