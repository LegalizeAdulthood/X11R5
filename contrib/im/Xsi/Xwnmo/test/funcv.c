/*
 * $Id: funcv.c,v 1.1 1991/09/11 06:48:32 ohm Exp $
 */
/*
 * X11R5 Input Method Test Program
 * funcv.c v 1.0   Fri Mar  8 14:51:03 JST 1991
 */

/*
 * Copyright 1991 by OMRON Corporation
 *
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include "exvalue.h"

VALUABLE	*mkstruct(valname, mode, setval, getval) /* MaKe STRUCT valuable */
char	*valname;
FLAG	mode;
#ifdef SYSV
caddr_t	*setval;
caddr_t *getval;
#else
void	*setval;
void	*getval;
#endif
{
    VALUABLE	*p;

    p = (VALUABLE *)malloc(sizeof(VALUABLE));
    p->vname = valname;
    p->mode = mode;
    p->sval = setval;
    p->gval = getval;
    return (p);
}
    
FLAG	 	verval(va_st)	/* VERify VALuable */
VALUABLE	*va_st;
{
    switch(va_st->mode) {
      case STR:
	fprintf(icfp, "value %-35s: set -> %-10s, get -> %-10s",
		va_st->vname, va_st->sval, va_st->gval);
	if (!strcmp(va_st->sval, va_st->gval)) {
	    fprintf(icfp, "\n");
	    return(OK);
	}
	fprintf(icfp, "\t...Failed.\n");
	return(NG);
	break;
      case HEX:
	fprintf(icfp, "value %-35s: set -> 0x%-8X, get -> 0x%-8X",
		va_st->vname, va_st->sval, va_st->gval);
	break;
      case DEC:
	fprintf(icfp, "value %-35s: set -> %-10d, get -> %-10d",
		va_st->vname, va_st->sval, va_st->gval);
	break;
    }
    if ((strcmp(va_st->vname, PNW)) && (strcmp(va_st->vname, PNH)) &&
	(strcmp(va_st->vname, SNW)) && (strcmp(va_st->vname, SNH))) {
	if (va_st->sval != va_st->gval) {
	    fprintf(icfp, "\t...Failed.\n");
	    return(NG);
	}
    }
    fprintf(icfp, "\n");
    return(OK);
}

	
	
	


