/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/roffchrs.c,v 2.6 1991/09/12 16:30:31 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/roffchrs.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/roffchrs.c,v 2.6 1991/09/12 16:30:31 bobg Exp $ ";
#endif /* lint */

/* special chars for rofftext.c
 *
 */

#include <class.h>
#include <rofftext.ih>
#include <hash.ih>

#define Mangle(self,name,def) hash_Store(self->SpecialChars,name,StrDup(def))

InitChars(self)
struct rofftext *self;
{
    Mangle(self,"em","-");
    Mangle(self,"hy","-");
    Mangle(self,"bu","o");
    Mangle(self,"14","1/4");
    Mangle(self,"12","1/2");
    Mangle(self,"34","3/4");
    Mangle(self,"fi","fi");
    Mangle(self,"fl","fl");
    Mangle(self,"ff","ff");
    Mangle(self,"Fi","ffi");
    Mangle(self,"Fl","ffl");
    Mangle(self,"**","*");
    Mangle(self,"pl","+");
    Mangle(self,"mi","-");
    Mangle(self,"eq","=");
    Mangle(self,"aa","'");
    Mangle(self,"ga","`");
    Mangle(self,"ul","_");
    Mangle(self,"sl","/");
    Mangle(self,"br","|");
    Mangle(self,"or","|");
    Mangle(self,"ci","O");
    Mangle(self,"fm","'");

    /* greek letters */
    Mangle(self,"*a","A");
    Mangle(self,"*b","B");
    Mangle(self,"*g","/");
    Mangle(self,"*d","D");
    Mangle(self,"*e","S");
    Mangle(self,"*z","Q");
    Mangle(self,"*y","N");
    Mangle(self,"*h","O");
    Mangle(self,"*i","i");

    /* math */
    Mangle(self,">=",">=");
    Mangle(self,"<=","<=");
    Mangle(self,"!=","!=");
    Mangle(self,"==","==");
    Mangle(self,"ap","~");
    Mangle(self,"->","->");
    Mangle(self,"<-","<-");
    Mangle(self,"ua","^");
    Mangle(self,"da","v");
    Mangle(self,"mu","x");
    Mangle(self,"di","/");
    Mangle(self,"+-","+-");
    Mangle(self,"no","!");

    /* more to come */
}
