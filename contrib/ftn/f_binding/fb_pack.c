#ifndef lint
static char     sccsid[] = "@(#)fb_pack.c 1.1 91/09/07 FJ";
#endif

/***********************************************************
Copyright 1990, 1991, by Fujitsu Limited.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Fujitsu not be used
in advertising or publicity pertaining to distribution of the software
without specific, written prior permission.  

FUJITSU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJITSU BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
******************************************************************/

/* Data record utility functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"

static int		int_len = sizeof(Pint);
static int		float_len = sizeof(Pfloat);
static int		char_len = sizeof(char);
static int		header = sizeof(Pint) * 3;
#define RECORD_LENGTH( _il, _rl, _sl, _total_str) \
    (header + (int_len * (_il)) + (float_len * (_rl)) + (int_len * (_sl)) + \
    (char_len * (_total_str)))



/* PACK DATA RECORD */
void
fprec_( il, ia, rl, ra, sl, lstr, str, mldr, errind, ldr, datrec, length)
Pint	*il;		/* number of integer entries	*/
Pint	*ia;		/* array containing of integer entries	*/
Pint	*rl;		/* number of real entries	*/
Pfloat	*ra;		/* array containing of real entries	*/
Pint	*sl;		/* number of character string entries	*/
Pint	*lstr;		/* lengths of each character string entries	*/
char	*str;		/* character string entries	*/
Pint	*mldr;		/* dimension of data record	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ldr;		/* OUT number of array elements 	*/
char	*datrec;	/* OUT data record	*/
int	*length;	/* string length	*/
{
    Pint	total_str = 0;
    Pint	total_rec = 0;
    Pint	*recptr = (Pint *)datrec;
    register int	i;
    
    if ( *il < 0 || *rl < 0 || *sl < 0) {
        *errind = ERR2004;

    } else {
        for ( i = 0; i < *sl; i++) {
            if ( lstr[i] < 0 || lstr[i] > *length) {
                *errind = ERR2004;
                return;
            }
            total_str += lstr[i];    
        }

        total_rec = RECORD_LENGTH( *il, *rl, *sl, total_str);
        if ( total_rec == header) {
            total_rec = 0;
        }
        if ( (total_rec + int_len) > ((*mldr) * 80)) {
            *errind = ERR2001;

        } else {
            *errind  = 0;
            *ldr = 1 + (total_rec + int_len - 1) / 80;
            *recptr = total_rec;
            recptr++;
            if ( total_rec > 0) {
     		/* pack integer data */
                *recptr = *il;
                recptr++;
                if ( *il > 0) {
                    bcopy((char *)ia, (char *)recptr, (*il * int_len));
                    recptr += *il;
                }
        	/* pack float data */
                *recptr = *rl;
                recptr++;
                if ( *rl > 0) {
                    bcopy((char *)ra, (char *)recptr, (*rl * float_len));
                    recptr += *rl;
                }
		/* pack string data */
                *recptr = *sl;
                recptr++;
                for ( i = 0; i < *sl; i++) {
                    *recptr = lstr[i];
                    recptr++;
                    if ( lstr[i] > 0) {
                        bcopy((char *)str, (char *)recptr,(lstr[i] * char_len));
                        recptr = (Pint *)((char*)recptr + lstr[i]);
                    }
                    str += *length;
                } 
            }
        }
    }
}


#define CHECK_REC_DATA( _rec, _num) \
    (((_num) = (_rec)) < 0)

/* UNPACK DATA RECORD */
void
furec_( ldr, datrec, iil, irl, isl, errind, il, ia, rl, ra, sl, lstr, str,
length)
Pint	*ldr;		/* number of array element 	*/
char	*datrec;	/* data record	*/
Pint	*iil;		/* dimension of integer array	*/
Pint	*irl;		/* dimension of real array	*/
Pint	*isl;		/* dimension of character array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*il;		/* OUT number of integer entries	*/
Pint	*ia;		/* OUT array containing integer entries	*/
Pint	*rl;		/* OUT number of real entries	*/
Pfloat	*ra;		/* OUT array containing real entries	*/
Pint	*sl;		/* OUT number of character string entries	*/
Pint	*lstr;		/* OUT length of each character string entries	*/
char	*str;		/* OUT character string entries	*/
int	*length;	/* OUT character string entries	*/
{
    Pint	total_str = 0;
    Pint	total_rec = 0;
    Pint	*recptr = (Pint *)datrec;
    register int	i;

    *errind = 0;
    if ( *ldr < 0) {
        *errind = ERR2004;

    } else if ( *ldr == 0) {
        *errind = ERR2003;

    } else if (( *recptr + int_len) > (*ldr * 80)) {
        *errind = ERR2003;

    } else if ( CHECK_REC_DATA( *recptr, total_rec)) {
        *errind = ERR2003;

    } else {
        if ( total_rec == 0) {
            *il = *rl = *sl = 0;

        } else {
            if ( CHECK_REC_DATA( *(recptr += 1), *il)) {
                *errind = ERR2003;

            } else if ( CHECK_REC_DATA( *(recptr += *il + 1), *rl)) {
                *errind = ERR2003;

            } else if ( CHECK_REC_DATA( *(recptr += *rl + 1), *sl)) {
                *errind = ERR2003;

            } else if ( *iil < *il || *irl < *rl || *isl < *sl) {
                *errind = ERR2001;

            } else {
                recptr++;
                for ( i = 0; i < *sl; i++) {
                    if ( *recptr < 0 || *recptr > *length) {
                        *errind = ERR2003;
                        return;
                    }
                    total_str += *recptr;
                    recptr = (Pint *)((char *)recptr + int_len + *recptr);
                } 
                if ( total_rec != RECORD_LENGTH( *il, *rl, *sl, total_str) ) {
                    *errind  = ERR2003;

                }
            }
        }
    }

    if ( !*errind) {
	/* unpack integer data */
        recptr = (Pint *)datrec;
        recptr += 2;
        if ( *il > 0) {
            bcopy((char *)recptr, (char *)ia, (*il * int_len));
            recptr += *il;
        }
	/* unpack float data */
        recptr++;
        if ( *rl > 0) {
            bcopy((char *)recptr, (char *)ra, (*rl * float_len));
            recptr += *rl;
        }
	/* unpack string data */
        recptr++;
        for ( i = 0; i < *sl; i++) {
            lstr[i] = *recptr;
            recptr++;
            if ( lstr[i] > 0) {
                bcopy((char *)recptr, (char *)str, (lstr[i] * char_len));
                recptr = (Pint *)((char*)recptr + lstr[i]);
            }
            str += *length;
        }
    }
}


void
fb_pack_rec( il, ia, rl, ra, sl, lstr, str, mldr, errind, ldr, datrec)
Pint	il;		/* number of integer entries	*/
Pint	*ia;		/* array containing of integer entries	*/
Pint	rl;		/* number of real entries	*/
Pfloat	*ra;		/* array containing of real entries	*/
Pint	sl;		/* number of character string entries	*/
Pint	*lstr;		/* lengths of each character string entries	*/
char	*str;		/* character string entries	*/
Pint	mldr;		/* dimension of data record	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ldr;		/* OUT number of array elements 	*/
char	*datrec;	/* OUT data record	*/
{
    Pint	total_str = 0;
    Pint	total_rec = 0;
    Pint	*recptr = (Pint *)datrec;
    register int	i;

    for ( i = 0; i < sl; i++) {
        total_str += lstr[i];
    }

    total_rec = RECORD_LENGTH( il, rl, sl, total_str);
    if ( total_rec == header) {
        total_rec = 0;
    }
    if ( (total_rec + int_len) > (mldr * 80)) {
        *errind  = ERR2001;

    } else {
        *errind  = 0;
        *ldr  = 1 + (total_rec + int_len - 1) / 80;
        *recptr = total_rec;
        recptr++;
        if ( total_rec > 0) {
            /* pack integral data */
            *recptr = il;
            recptr++;
            if ( il > 0) {
                bcopy((char *)ia, (char *)recptr, (il * int_len));
                recptr += il;
            }
            /* pack float data */
            *recptr = rl;
            recptr++;
            if ( rl > 0) {
                bcopy((char *)ra, (char *)recptr, (rl * float_len));
                recptr += rl;
            }
	    /* pack string data */
            *recptr = sl;
            recptr++;
            for ( i = 0; i < sl; i++) {
                *recptr  = lstr[i];
                recptr++;
                if ( lstr[i] > 0) {
                    bcopy((char *)str, (char *)recptr, (lstr[i] * char_len));
                    recptr = (Pint *)((char*)recptr + lstr[i]);
                }
                str += lstr[i];
            } 
        }
    }
}

        
int
fb_unpack_datrec( ldr, datrec, errind, rec)
Pint		ldr;		/* number of array element 	*/
char		*datrec;	/* FORTRAN data record	*/
Pint		*errind;	/* OUT error indicator	*/
Pdata_rec	*rec;		/* OUT data record	*/
{
    Pint		total_rec = 0;
    Pint		total_str = 0;
    Pint		size = 0;
    Pint		*recptr = (Pint *)datrec;
    char		*w_str;
    register int	i;

    *errind = 0;
    if ( ldr < 0) {
        *errind = ERR2004;

    } else if ( ldr == 0) {
        *errind = ERR2003;

    } else if (( *recptr + int_len) > (ldr * 80) ) {
        *errind = ERR2003;

    } else if ( CHECK_REC_DATA( *recptr, total_rec)) {
        *errind = ERR2003;

    } else {
        if ( total_rec == 0) {
            rec->data.il = rec->data.rl = rec->data.sl = 0;

        } else {
            if ( CHECK_REC_DATA( *(recptr += 1), rec->data.il)) {
                *errind = ERR2003;

            } else if ( CHECK_REC_DATA( *(recptr += rec->data.il + 1),
                rec->data.rl)) {
                *errind = ERR2003;

            } else if ( CHECK_REC_DATA( *(recptr += rec->data.rl + 1),
                rec->data.sl)) {
                *errind = ERR2003;

            } else {
                recptr++;
                for ( i = 0; i < rec->data.sl; i++) {
                    if ( *recptr < 0) {
                        *errind = ERR2003;
                        return;
                    }
                    total_str += *recptr;
                    recptr = (Pint *)((char *)recptr + int_len + *recptr);
                } 
                if ( total_rec != RECORD_LENGTH( rec->data.il, rec->data.rl,
                     rec->data.sl, total_str) ) {
                    *errind  = ERR2003;

                }
            }
        }
    }

    if ( !*errind) {
        rec->buf = NULL;
        rec->size = (rec->data.il * int_len) + (rec->data.rl * float_len) + 
                    (rec->data.sl * int_len) + (total_str * char_len); 
        if ( rec->size > 0 && !( rec->buf = malloc(rec->size))) {
            *errind = ERR900;

        } else if ( rec->size > 0) {
            rec->data.ia = (Pint *)rec->buf;
            rec->data.ra = (Pfloat *)(rec->data.ia + rec->data.il);
            rec->data.lstr = (Pint *)(rec->data.ra + rec->data.rl);
            rec->data.str = (char *)(rec->data.lstr + rec->data.sl);
	    /* unpack integer data */
            recptr = (Pint *)datrec;
            recptr += 2;
            if ( rec->data.il > 0) {
                bcopy ( (char *)recptr, (char *)rec->data.ia,
                    (rec->data.il * int_len));
                recptr += rec->data.il;
            }
	    /* unpack float data */
            recptr++;
            if ( rec->data.rl > 0) {
                bcopy ( (char *)recptr, (char *)rec->data.ra,
                    (rec->data.rl * int_len));
                recptr += rec->data.rl;
            }
	    /* unpack string data */
            w_str = rec->data.str;
            recptr++;
            for ( i = 0; i < rec->data.sl; i++) {
                rec->data.lstr[i] = *recptr;
                recptr++;
                if ( rec->data.lstr[i] > 0) {
                    bcopy((char *)recptr, (char *)w_str,
                        (rec->data.lstr[i] * int_len));
                    recptr = (Pint *)((char *)recptr + rec->data.lstr[i]);
                }
                w_str += rec->data.lstr[i];
            }
        }
    }
    if ( !*errind)
        return 0;
    else
        return 1;
}

void
fb_del_datrec( rec)
Pdata_rec	*rec;
{
    if ( rec->size > 0 && rec->buf)
        free( rec->buf);
}
