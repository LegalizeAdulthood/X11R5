#ifndef lint
static char     sccsid[] = "@(#)fb_mf.c 1.1 91/09/07 FJ";
#endif

/***********************************************************
Copyright 1989, 1990, 1991, by Fujitsu Limited, Sun Microsystems, Inc.
and the X Consortium.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Fujitsu, Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity
pertaining to distribution of the software without specific, written
prior permission.  

FUJITSU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJITSU BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
******************************************************************/

/* Metafile functions for the PHIGS FORTRAN binding	*/

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"


/* WRITE ITEM TO METAFILE */
void
pwitm_( wkid, type, ildr, ldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*type;		/* item type	*/
Pint	*ildr;		/* item data record length	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record	*/
{
    if (CB_ENTRY_CHECK( phg_cur_cph, ERR3, Pfn_write_item))
	/* NOT IMPLEMENTED */
	ERR_REPORT( phg_cur_cph->erh, ERRN152);
}


/* GET ITEM TYPE FROM METAFILE */
void
pgtitm_( wkid, type, idrl)
Pint	*wkid;		/* workstation identifier	*/
Pint	*type;		/* OUT item type	*/
Pint	*idrl;		/* OUT item data record length	*/
{
    if (CB_ENTRY_CHECK( phg_cur_cph, ERR3, Pfn_get_item_type))
	/* NOT IMPLEMENTED */
	ERR_REPORT( phg_cur_cph->erh, ERRN152);
}


/* READ ITEM FROM METAFILE */
void
prditm_( wkid, midrl, mldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*midrl;		/* MAX item data record length	*/
Pint	*mldr;		/* dimension of item data record	*/
char	*datrec;	/* OUT data record	*/
{
    if (CB_ENTRY_CHECK( phg_cur_cph, ERR3, Pfn_read_item))
	/* NOT IMPLEMENTED */
	ERR_REPORT( phg_cur_cph->erh, ERRN152);
}

/* INTERPRET ITEM */
void
piitm_( type, idrl, ldr, datrec)
Pint	*type;		/* item type	*/
Pint	*idrl;		/* item data record length	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record	*/
{
    if (CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_interpret_item))
	/* NOT IMPLEMENTED */
	ERR_REPORT( phg_cur_cph->erh, ERRN152);
}
