#ifndef lint
static char     sccsid[] = "@(#)fb_ctrl.c 1.1 91/09/07 FJ";
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

/* Control functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "phg_dt.h"
#include "cp.h"
#include "fb_priv.h"

#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#else
extern char *getenv();
#endif

/* The global CP handle for the binding.
Cp_handle	phg_cur_cph = NULL;
*/
static int              xinfo_used;
static Pxphigs_info     xphigs_info;
static char             *work_pointer = NULL;
static int              error_num = 0;


#define MERGE_DATABASE( _flag, _db, _rmdb) \
    if ( (_flag) != 0) { \
        XrmMergeDatabases( (_db), &(_rmdb)); \
    } else { \
        (_rmdb) = (_db); \
        (_flag) = 1; \
    }

/* OPEN XPHIGS */
void
fopxph_( errfil, bufa, dif, dyid, rmbf, rmbfna, rmdf, nrmd, airmd, rmstr,
aif, apname, apclas, nomon, clicss, len1, len2, len3, len4)
Pint	*errfil;	/* error file	*/
Pint	*bufa;		/* amount of memory unit	*/
Pint	*dif;		/* display identifier flag	*/
Pint	*dyid;		/* display identifier	*/
Pint	*rmbf;		/* resource database flag	*/
char	*rmbfna;	/* resource database name	*/
Pint	*rmdf;		/* resource data flag	*/
Pint	*nrmd;		/* number of resource data string	*/
Pint	*airmd;		/* lengths of each resource data string entries	*/
char	*rmstr;		/* resource data string	*/
Pint	*aif;		/* application infomation flag	*/
char	*apname;	/* application name	*/
char	*apclas;	/* application class	*/
Pint	*nomon;		/* no PHIGS moniter flag	*/
Pint	*clicss;	/* clients side CSS flag	*/
Pint	*len1;		/* resource database name string length	*/
Pint	*len2;		/* resource data string length	*/
Pint	*len3;		/* aprication name string length	*/
Pint	*len4;		/* prication class string length	*/
{
    F_dy_id		disp;
    char		*w_str = NULL;
    int			size = 0;
    int			db_flag = 0;
    int			len;
    int			i;
    XrmDatabase db;

    xinfo_used = 1;
    bzero( (char *)&xphigs_info, (int)sizeof(Pxphigs_info) );
    XrmInitialize();

    work_pointer = NULL;
    error_num = 0;

    /* error check */
    if ( *dif < POFF || *dif > PON ||
         *rmbf < POFF || *rmbf > PON ||
         *rmdf < POFF || *rmdf > PON ||
         *aif < POFF || *aif > PON ||
         *nomon  < PMON || *nomon  > PNOMON ||
         *clicss < PSERVR || *clicss > PCLIET ) {
         error_num = ERR2000;

    } else {
        /* Control stuff. */
        if ( *dif == PON) {
            if ( !(disp = phg_fb_search_display( *dyid))) {
                error_num = ERRN200;
            } else {
                xphigs_info.display = disp->disp_p;
            }
        }

        if ( !error_num) {
            if ( *nomon == PNOMON) {
                xphigs_info.flags.no_monitor = *nomon;
            }

            if ( *clicss == PCLIET) {
                xphigs_info.flags.force_client_SS = *clicss;
            }

	        /* count malloc size */
            if ( *rmbf == PON) {
                size += (*len1 + 1) * sizeof(char);
            }
            if ( *rmdf == PON) {
                for ( i = 0; i < *nrmd; i++) {
                    size += (*len2 + 1) * sizeof(char);
                }
            }
            if ( *aif == PON) {
                size += (*len3 + 1) * sizeof(char);
                size += (*len4 + 1) * sizeof(char);
            }
            /* malloc work area */
            if ( size > 0 && !( work_pointer = (char *)malloc(size))) {
                error_num = ERR900;

            } else if ( size > 0) {
                w_str = work_pointer;
                /* RMDB stuff. */
                if ( *aif == PON) {
                    bcopy((char *)apname, (char *)w_str,
                        (*len3 * sizeof(char)));
                    w_str[*len3] = '\0';
                    xphigs_info.appl_id.name = w_str;
                    w_str += *len3 + 1;
                    bcopy((char *)apclas, (char *)w_str,
                        (*len4 * sizeof(char)));
                    w_str[*len4] = '\0';
                    xphigs_info.appl_id.class_name = w_str;
                    w_str += *len4 + 1;
                }

                if ( *rmbf == PON) {
                    bcopy((char *)rmbfna, (char *)w_str,
                        (*len1 * sizeof(char)));
                    w_str[*len1] = '\0';
                    db = XrmGetFileDatabase(w_str);
                    MERGE_DATABASE( db_flag, db, xphigs_info.rmdb);
                    w_str += *len1 + 1;
                }

                if ( *rmdf == PON) {
                    for ( i = 0; i < *nrmd; i++) {
                        if ( airmd[i] < 0 || airmd[i] > *len2) {
                            error_num = ERR2004;
                        } else {
                            bcopy((char *)rmstr, (char *)w_str,
                                (*len2 * sizeof(char)));
                            w_str[airmd[i]] = '\0';
                            db = XrmGetStringDatabase(w_str);
                            MERGE_DATABASE( db_flag, db, xphigs_info.rmdb);
                            rmstr += *len2;
                            w_str += *len2 + 1;
                        }
                    }
                }
            }
        }
    }
    phg_fb_open_phigs( errfil, bufa, Pfn_popenpex);
}

/* Need to get W_OK for SYSV */
#ifdef SYSV
#include <unistd.h>
#endif

static int
check_errfile( error_file)
char        *error_file;
{
    int         status = 0;
    FILE        *erf;

    /* see if error file can be opened for writing */
    if ( error_file) {
        if ( access( error_file, W_OK) == 0) {
            status = 1;

        /* file might not exist, see if it can be created */
        } else if ( errno == ENOENT && (erf = fopen( error_file, "a")) ) {
            status = 1;
            (void) fclose( erf);
            (void) unlink( error_file);
        }
    } else {    /* NULL means use stderr */
        status = 1;
    }
    return status;
}

static void
check_env_controls()
{
    char                *str;

    /* If the application already set something we'll override it. */
    if ( str = getenv( "PEX_SI_API_NO_PM" ) ) {
        xinfo_used = 1;
        xphigs_info.flags.no_monitor = atoi( str ) ? 1 : 0;
    }
    if ( str = getenv( "PEX_SI_API_CLIENT_SS" ) ) {
        xinfo_used = 1;
        xphigs_info.flags.force_client_SS = atoi( str ) ? 1 : 0;
    }
}



/* OPEN PHIGS */
void
popph_( errfil, bufa)
Pint	*errfil;	/* error file's unit number */
Pint	*bufa;		/* NOT USED */
{ 
    phg_fb_open_phigs( errfil, bufa, Pfn_open_phigs);
}


void
phg_fb_open_phigs( errfil, bufa, func_id)
Pint	*errfil;	/* error file's unit number */
Pint	*bufa;		/* NOT USED */
Pint	func_id;	/* function code Pfn_open_phigs or Pfn_open_xphigs */
{ 
    char		*error_file;	/* error file name */
    static char	unit_no[5];	/* error file's enviroment variable */

    GETENV_UNIT_NO( *errfil, unit_no, error_file);
    if ( CB_PHIGS_OPEN( phg_cur_cph)) {
        ERR_SET_CUR_FUNC( phg_cur_cph->erh, func_id);
        ERR_REPORT( phg_cur_cph->erh, ERR1);

    } else if ( *errfil < 0 || *errfil > 99) {
        ERR_HANDLE( ERR450, func_id, (char *)NULL);

    } else if ( !check_errfile( error_file)) {
        /* have to bypass the error reporting code: */
        ERR_HANDLE( ERR450, func_id, error_file);

    } else if ( error_num != 0) {
        /* by open xphigs */
        ERR_HANDLE( error_num, func_id, error_file);
        error_num = 0;

    } else if ( !phg_fb_set_ws_type_table()) {
        ERR_HANDLE( ERR900, func_id, error_file);

    } else {
        check_env_controls();
        if ( phg_cur_cph = phg_cpxc_open( error_file,
                xinfo_used ? &xphigs_info : (Pxphigs_info *)NULL ) ) {
            phg_cur_cph->psl->phg_sys_state = PSYS_ST_PHOP;
        }
    }

    /* Clear any controls so they don't carry over to next OPEN. */
    if ( xinfo_used ) {
        xinfo_used = 0;
        bzero( (char *)&xphigs_info, (int)sizeof(Pxphigs_info) );
        if ( work_pointer != NULL) { 
            free( work_pointer);
            work_pointer = NULL;
        }
    }
}


/* CLOSE PHIGS */
void
pclph_()
{
    Phg_args	cp_args;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR4, Pfn_close_phigs)) {
        if ( phg_cur_cph->psl->phg_ws_state != PWSCL ||
             phg_cur_cph->psl->phg_struct_state != PSTCL ||
             phg_cur_cph->psl->phg_ar_state != PARCL) {
            ERR_REPORT( phg_cur_cph->erh, ERR4);
        
        } else {
            CP_FUNC( phg_cur_cph, CP_FUNC_OP_CLOSE_PHIGS, &cp_args, NULL);
            phg_fb_delete_all_ws_type();
            phg_fb_delete_all_display_id();
            phg_fb_delete_all_conn_id();
            phg_cur_cph = NULL;
        }
    }
}


/* INQUIRE SYSTEM STATE VALUE */
void
pqsys_( syssta)
Pint	*syssta;	/* OUT system state value	*/
{
    if ( CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *syssta = PPHOP;
    } else {
        *syssta = PPHCL;
    }
}


/* INQUIRE PHIGS FACILITIES */
void
pqphf_( ncs, errind, simopw, simopa, numesn, olcs, cs, nfln, ifln)
Pint	*ncs;		/* character set requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*simopw;	/* OUT MAX number of simultaneously open ws	*/
Pint	*simopa;	/* OUT MAX number of simultaneously open archives */
Pint	*numesn;	/* OUT MAX number of available names for name sets */
Pint	*olcs;		/* OUT number of available character sets	*/
Pint	*cs;		/* OUT NCSth of available character sets	*/
Pint	*nfln;		/* OUT MAX length of normal filter list for ISS */
Pint	*ifln;		/* OUT MAX length of inverted filter list for ISS */
{
    if ( !CB_PHIGS_OPEN( phg_cur_cph)) {
        *errind = ERR2;
 
    } else {
	*errind = 0;
	*simopw = phg_cur_cph->pdt.max_num_open_workstations;
	*simopa = phg_cur_cph->pdt.max_num_open_archives;
	*numesn = phg_cur_cph->pdt.max_num_names_for_nameset;
	*nfln = phg_cur_cph->pdt.max_length_normal_iss_filter;
	*ifln = phg_cur_cph->pdt.max_length_inverted_iss_filter;
	*olcs = phg_cur_cph->pdt.char_sets.num_ints;
	if ( *olcs > 0) {
	    if ( *ncs < 0 || *ncs > *olcs) {
		*errind = ERR2002;
            } else if ( *ncs > 0) {
                *cs = phg_cur_cph->pdt.char_sets.ints[*ncs - 1];
            }
	}
    }
}


/* INQUIRE GENERALIZED STRUCTURE ELEMENT FACILITIES */
void
pqgsef_( n, errind, ol, gseid, wsdind)
Pint	*n;	  	/* element of the list of available GSEs	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;	  	/* OUT number of GSEs	*/
Pint	*gseid;	  	/* OUT GSE identifier of Nth element of GSEs	*/
Pint    *wsdind;	/* OUT ws dependency indicator of Nth element of GSEs */
{
    if ( !CB_PHIGS_OPEN( phg_cur_cph)) {
	*errind = ERR2;
 
    } else {
	*errind = 0;
	*ol = phg_cur_cph->pdt.gses.num_id_facs;
	if ( *ol > 0) {
	    if ( *n < 0 || *n > *ol) {
		*errind = ERR2002;
            } else if ( *n > 0) {
                *gseid = phg_cur_cph->pdt.gses.id_facs[*n - 1].id;
                *wsdind = (Pint)phg_cur_cph->pdt.gses.id_facs[*n - 1].ind;
	    }
	}
    }
}


/* INQUIRE MODELLING CLIPPING FACILITIES */
void
pqmclf_( n, errind, ndpmcv, ol, mclpop)
Pint    *n;		/* list element requested	*/
Pint    *errind;	/* OUT error indicator	*/
Pint    *ndpmcv;	/* OUT number of distinct planes	*/
Pint    *ol;		/* OUT number of modelling clipping operators	*/
Pint    *mclpop;	/* OUT Nth element of modelling clipping operators */
{
    if ( !CB_PHIGS_OPEN( phg_cur_cph)) {
        *errind = ERR2;

    } else {
	*errind = 0;
	*ndpmcv = phg_cur_cph->pdt.max_num_model_clip_volumes;
	*ol = phg_cur_cph->pdt.model_clip_ops.num_ints;
	if ( *ol > 0) {
	    if ( *n < 0 || *n > *ol) {
		*errind = ERR2002;
            } else if ( *n > 0) {
                *mclpop = phg_cur_cph->pdt.model_clip_ops.ints[*n - 1];
	    }
        }
    }
}


/* EMERGENCY CLOSE PHIGS */
void
peclph_()
{
    Phg_args    cp_args;

    if ( CB_PHIGS_OPEN( phg_cur_cph)) {
        CP_FUNC( phg_cur_cph, CP_FUNC_OP_EMERG_CLOSE, &cp_args, NULL);
        CP_FUNC( phg_cur_cph, CP_FUNC_OP_CLOSE_PHIGS, &cp_args, NULL);
        phg_fb_delete_all_ws_type();
        phg_fb_delete_all_display_id();
        phg_fb_delete_all_conn_id();
        phg_cur_cph = NULL;
    }
}
