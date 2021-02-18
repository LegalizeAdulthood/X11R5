/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *pfpack_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/snap2/pfio/RCS/pfpack.c,v 1.8 1991/09/12 17:20:57 bobg Exp $";

/*
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/snap2/pfio/RCS/pfpack.c,v 1.8 1991/09/12 17:20:57 bobg Exp $
 */

#include <pfio.h>
#include <varargs.h>
#include <pfioseg.h>

/*
 * pack arguments for network transfer
 */
void PFpack(va_alist)
va_dcl
{
    va_list ap;
    register atype;
    register along;
    char *astr;
    PFM_pt o;
    va_start(ap);
    o=va_arg(ap,PFM_pt);
    atype=va_arg(ap,int);  
    along=va_arg(ap,int);
    if(atype!=LS_alocop)
	(*(o->parerr))(o,"PFIO:invalid opcode %d",atype);
    PF_aloc(o,along);
    o->buf+=MIN_PAK;		/*skip the length we will come back to it*/
    
    /* pack up the argument list*/
    while(TRUE) {
        if(PFM_remaining(*o)<4)
	  (*(o->parerr))(o,"PFIO:output reserve too small");
	atype=va_arg(ap,int);
   	switch(atype) {
	    case LS_dropoff:
		goto end_of_arg_list;
	    case LS_int32:
		along=va_arg(ap,long);
		*(o->buf++) = along >> 24;
		*(o->buf++) = along >> 16;
		*(o->buf++) = along >> 8;
		*(o->buf++) = along ;
		break;
	    case LS_int16:
		along=va_arg(ap,long);
   		*(o->buf++) = along >> 8;
		*(o->buf++) = along ;
		break;
	    case LS_int8:
		along=va_arg(ap,long);
		*(o->buf++) = along ;
		break;
	    case LS_cstr:
		along=va_arg(ap,long);	/*string length*/
		if(PFM_remaining(*o)<(along+2))
		  (*(o->parerr))(o,"PFIO:output reserve too small");
   		*(o->buf++) = along >> 8;
		*(o->buf++) = along ;
		astr=va_arg(ap,char *);	/*string to send*/
		bcopy(astr,o->buf,((int)along)); /*put on the string*/
		o->buf+=along;	/*and advace output*/
		*(o->buf++)=0;	/*null terminate it*/
		break;
	    case LS_rstr:		/*rest of packet as string*/
		along=va_arg(ap,long);	/*string length*/
		astr=va_arg(ap,char *);	/*string to send*/
		if(PFM_remaining(*o)<(along+2))
		  (*(o->parerr))(o,"PFIO:output reserve too small");
		bcopy(astr,o->buf,((int)along)); /*put on the string*/
		o->buf+=along;	/*and advace output*/
		*(o->buf++)=0;	/*null terminate it*/
		goto end_of_arg_list;
	    default:
		(*(o->parerr))(o,"invalid send opcode %d",atype);
	}
    }

/*
 * all the arguments have been packed
 * backpatch the length
 */
end_of_arg_list:
    PF_dropoff(o);	/*queue this block up for writing*/
}
