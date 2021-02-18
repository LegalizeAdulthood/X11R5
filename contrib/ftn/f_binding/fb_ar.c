#ifndef lint
static char     sccsid[] = "@(#)fb_ar.c 1.1 91/09/07 FJ";
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

/* Archive functions for the PHIGS FOTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"


static int
valid_ar_fname( fname )
    char	*fname;
{
    int         status = 0;

    if ( fname && strlen(fname) <= MAXNAMLEN)
        status = 1;

    return status;
}


/* OPEN ARCHIVE FILE */
void
poparf_( afid, arcfil)
Pint	*afid;		/* archive identifier	*/
Pint	*arcfil;	/* archive file name	*/
{
    Phg_args			cp_args;
    register Phg_args_ar_open	*args = &cp_args.data.ar_open;
    Phg_ret			ret;
    char			*fname;
    static char			unit_no[5];
    char			*archive_file;	/* archive file name	*/

    GETENV_UNIT_NO( *arcfil, unit_no, archive_file);
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_open_ar_file)) {
        if ( phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
            ERR_REPORT( phg_cur_cph->erh, ERR402);

        } else if ( !valid_ar_fname( archive_file)) {
            /* need a better error code for this, use 190 for now */
            ERR_REPORT( phg_cur_cph->erh, ERR400);

        } else if ( !phg_psl_ar_free_slot( phg_cur_cph->psl)) {
            ERR_REPORT( phg_cur_cph->erh, ERR401);

        } else if ( !(fname = malloc((unsigned)strlen(archive_file) + 1))) {
            ERR_REPORT( phg_cur_cph->erh, ERR900);

        } else {
            args->arid = *afid;
            args->fname = archive_file;
            args->name_length = strlen(archive_file) + 1;
            ret.err = 0;
            CP_FUNC( phg_cur_cph, CP_FUNC_OP_AR_OPEN, &cp_args, &ret);
            if ( !ret.err) {
                strcpy( fname, archive_file);
                (void)phg_psl_add_ar( phg_cur_cph->psl, *afid, fname);
                PSL_AR_STATE( phg_cur_cph->psl) = PAROP;
            } else {
                free( fname);
            }
            ERR_FLUSH( phg_cur_cph->erh);
        }
    }
}


/* CLOSE ARCHIVE FILE */
void
pclarf_( afid)
Pint	*afid;		/* archive identifier	*/
{
    Phg_args			cp_args;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR7, Pfn_close_ar_file)) {
        if ( PSL_AR_STATE( phg_cur_cph->psl) != PAROP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR7);

	} else if ( !phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR404);
	
	} else {
	    cp_args.data.idata = *afid;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_AR_CLOSE, &cp_args, NULL);
	    /* psl_rem_ar() sets the ar state to ARCL if no more ar's open */
	    phg_psl_rem_ar( phg_cur_cph->psl, *afid);
	}
    }
}


/* ARCHIVE STRUCTURES */
void
parst_( afid, n, lstrid)
Pint	*afid;		/* archive identifier	*/
Pint	*n;		/* number of structures identifiers  */
Pint	*lstrid;	/* list of structure identifiers	*/
{
    Phg_args			cp_args;
    register Phg_args_ar_info	*args = &cp_args.data.ar_info;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR7, Pfn_ar_structs)) {
        if ( PSL_AR_STATE( phg_cur_cph->psl) != PAROP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR7);

	} else if ( !phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR404);
	
	} else if ( *n < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);
	
	} else {
	    args->arid = *afid;
	    args->data.num_ints = *n;
	    args->data.ints = lstrid;
	    args->op = PHG_ARGS_AR_STRUCTS;
	    args->resflag = PSL_ARCHIVE_CONFLICT( phg_cur_cph->psl);
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_AR_ARCHIVE, &cp_args, NULL);
	}
    }
}


/* ARCHIVE STRUCTURE NETWORKS */
void
parsn_( afid, n, lstrid)
Pint	*afid;		/* archive identifier	*/
Pint	*n;		/* number of structure identifiers	*/
Pint	*lstrid;	/* list of structure identifiers	*/
{
    Phg_args			cp_args;
    register Phg_args_ar_info	*args = &cp_args.data.ar_info;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR7, Pfn_ar_struct_nets)) {
        if ( PSL_AR_STATE( phg_cur_cph->psl) != PAROP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR7);

	} else if ( !phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR404);
	
	} else if ( *n < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);
	
	} else {
	    args->arid = *afid;
	    args->data.num_ints = *n;
	    args->data.ints = lstrid;
	    args->op = PHG_ARGS_AR_NETWORKS;
	    args->resflag = PSL_ARCHIVE_CONFLICT( phg_cur_cph->psl);
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_AR_ARCHIVE, &cp_args, NULL);
	}
    }
}


/* ARCHIVE ALL STRUCTURES */
void
parast_( afid)
Pint	*afid;		/* archive identifier	*/
{
    Phg_args			cp_args;
    register Phg_args_ar_info	*args = &cp_args.data.ar_info;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR7, Pfn_ar_all_structs)) {
        if ( PSL_AR_STATE( phg_cur_cph->psl) != PAROP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR7);

	} else if ( !phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR404);
	
	} else {
	    args->arid = *afid;
	    args->op = PHG_ARGS_AR_ALL;
	    args->resflag = PSL_ARCHIVE_CONFLICT( phg_cur_cph->psl);
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_AR_ARCHIVE, &cp_args, NULL);
	}
    }
}


/* SET CONFLICT RESOLUTION */
void
pscnrs_( arccr, retcr)
Pint	*arccr;		/* archival conflict resolution  */
Pint	*retcr;		/* retrieval conflict resolution */
{
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_set_conf_res)) {
        if (( *arccr < PCPRE || *arccr > PCREPL) ||
            ( *retcr < PCPRE || *retcr > PCREPL)) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else {
	    PSL_ARCHIVE_CONFLICT( phg_cur_cph->psl) = (Pconf_res)(*arccr);
	    PSL_RETRIEVE_CONFLICT( phg_cur_cph->psl) = (Pconf_res)(*retcr);
        }
    }
}


/* RETRIEVE STRUCTURE IDENTIFIERS */
void
prsid_( afid, ilsize, n, lstrid)
Pint	*afid;		/* archive identifier	*/
Pint	*ilsize;	/* size of the list	*/
Pint	*n;		/* OUT number of structure identifiers	*/
Pint	*lstrid;	/* OUT list of structure identifiers	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR7, Pfn_ret_struct_ids)) {
        if ( PSL_AR_STATE( phg_cur_cph->psl) != PAROP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR7);

	} else if ( !phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR404);

	} else {
	    cp_args.data.idata = *afid;
	    ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_AR_GET_NAMES, &cp_args, &ret);
	    if ( !ret.err) {
		*n = ret.data.int_list.num_ints;
		if ( *n > 0) {
		    if ( *ilsize < *n) {
			ERR_REPORT( phg_cur_cph->erh, ERR2001);

		    } else {
			bcopy( (char*)ret.data.int_list.ints,
				(char*)lstrid, *n * sizeof(Pint));
		    }
		}
	    }
	    ERR_FLUSH( phg_cur_cph->erh);
	}
    }
}


/* RETRIEVE STRUCTURES */ 
void
prest_( afid, n, lstrid)
Pint	*afid;		/* archive identifier	*/
Pint	*n;		/* number of structure identifiers	*/
Pint	*lstrid;	/* list of structure identifiers	*/
{
    Phg_args			cp_args;
    register Phg_args_ar_info	*args = &cp_args.data.ar_info;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR7, Pfn_ret_structs)) {
        if ( PSL_AR_STATE( phg_cur_cph->psl) != PAROP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR7);

	} else if ( !phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR404);
	
	} else if ( *n < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);
	
	} else {
	    args->arid = *afid;
	    args->data.num_ints = *n;
	    args->data.ints = lstrid;
	    args->op = PHG_ARGS_AR_STRUCTS;
	    args->resflag = PSL_RETRIEVE_CONFLICT( phg_cur_cph->psl);
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_AR_RETRIEVE, &cp_args, NULL);
	}
    }
}


/* RETRIEVE STRUCTURE NETWORKS */
void
presn_( afid, n, lstrid)
Pint	*afid;		/* archive identifier	*/
Pint	*n;		/* number of structure identifiers	*/
Pint	*lstrid;	/* list of structure identifiers	*/
{
    Phg_args			cp_args;
    register Phg_args_ar_info	*args = &cp_args.data.ar_info;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR7, Pfn_ret_struct_nets)) {
        if ( PSL_AR_STATE( phg_cur_cph->psl) != PAROP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR7);

	} else if ( !phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR404);
	
	} else if ( *n < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);
	
	} else {
	    args->arid = *afid;
	    args->data.num_ints = *n;
	    args->data.ints = lstrid;
	    args->op = PHG_ARGS_AR_NETWORKS;
	    args->resflag = PSL_RETRIEVE_CONFLICT( phg_cur_cph->psl);
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_AR_RETRIEVE, &cp_args, NULL);
	}
    }
}


/* RETRIEVE ALL STRUCTURES */
void
prast_( afid)
Pint	*afid;		/* archive identifier	*/
{
    Phg_args			cp_args;
    register Phg_args_ar_info	*args = &cp_args.data.ar_info;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR7, Pfn_ret_all_structs)) {
        if ( PSL_AR_STATE( phg_cur_cph->psl) != PAROP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR7);

	} else if ( !phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR404);
	
	} else {
	    args->arid = *afid;
	    args->op = PHG_ARGS_AR_ALL;
	    args->resflag = PSL_RETRIEVE_CONFLICT( phg_cur_cph->psl);
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_AR_RETRIEVE, &cp_args, NULL);
	}
    }
}


/* DELETE STRUCTURES FROM ARCHIVE */
void
pdstar_( afid, n, lstrid)
Pint	*afid;		/* archive identifier	*/
Pint	*n;		/* number of structure identifiers	*/
Pint	*lstrid;	/* list of structure identifiers	*/
{
    Phg_args			cp_args;
    register Phg_args_ar_info	*args = &cp_args.data.ar_info;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR7, Pfn_del_structs_ar)) {
        if ( PSL_AR_STATE( phg_cur_cph->psl) != PAROP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR7);

	} else if ( !phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR404);
	
	} else if ( *n < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);
	
	} else {
	    args->arid = *afid;
	    args->data.num_ints = *n;
	    args->data.ints = lstrid;
	    args->op = PHG_ARGS_AR_STRUCTS;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_AR_DELETE, &cp_args, NULL);
	}
    }
}


/* DELETE STRUCTURE NETWORKS FROM ARCHIVE */
void
pdsnar_( afid, n, lstrid)
Pint	*afid;		/* archive identifier	*/
Pint	*n;		/* number of structure identifiers	*/
Pint	*lstrid;	/* list of structure identifiers	*/
{
    Phg_args			cp_args;
    register Phg_args_ar_info	*args = &cp_args.data.ar_info;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR7, Pfn_del_struct_nets_ar)) {
        if ( PSL_AR_STATE( phg_cur_cph->psl) != PAROP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR7);

	} else if ( !phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR404);
	
	} else if ( *n < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);
	
	} else {
	    args->arid = *afid;
	    args->data.num_ints = *n;
	    args->data.ints = lstrid;
	    args->op = PHG_ARGS_AR_NETWORKS;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_AR_DELETE, &cp_args, NULL);
	}
    }
}


/* DELETE ALL STRUCTURES FROM ARCHIVE */
void
pdasar_( afid)
Pint	*afid;		/* archive identifier	*/
{
    Phg_args			cp_args;
    register Phg_args_ar_info	*args = &cp_args.data.ar_info;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR7, Pfn_del_all_structs_ar)) {
        if ( PSL_AR_STATE( phg_cur_cph->psl) != PAROP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR7);

	} else if ( !phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR404);
	
	} else {
	    args->arid = *afid;
	    args->op = PHG_ARGS_AR_ALL;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_AR_DELETE, &cp_args, NULL);
	}
    }
}


/* INQUIRE ARCHIVE STATE VALUE */
void
pqars_( arsta)
Pint	*arsta;		/* OUT archive state	*/
{
    if ( CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *arsta = PSL_AR_STATE( phg_cur_cph->psl);
    } else {
	*arsta = PARCL;
    }
}


/* INQUIRE ARCHIVE FILES */
void
pqarf_( n, errind, number, afid, arcfil)
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*number;	/* OUT number of archive files open	*/
Pint	*afid;		/* OUT Nth open archive file identifier	*/
Pint	*arcfil;	/* OUT Nth open archive file name	*/
{
    register	Phg_state_list	*psl;
    register	int 		i,j;
    register	int 		n_count = 0;
    char			*name;
    static char 	unit_no[5];

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else {
        psl = phg_cur_cph->psl;
        *errind = 0;
        *number = 0;
        if ( PSL_AR_STATE( phg_cur_cph->psl) == PAROP) {
            for ( i = 0; i < MAX_NO_OPEN_ARFILES; i++) {
                if ( psl->ar_files[i].used) {
                    (*number)++;
                }
            }
            if ( *number > 0) {
                if ( *n < 0 || *n > *number) {
                    *errind = ERR2002;
                } else if ( *n > 0) {
                    for ( i = 0; i < MAX_NO_OPEN_ARFILES; i++) {
                        if ( psl->ar_files[i].used) {
                            n_count++;
                            if ( n_count == *n) {
                                *afid = psl->ar_files[i].arid;
                                for ( j = 0; j <= 99; j++) {
                                    unit_no[0] = '\n';
                                    strcpy( unit_no, "FU00");
                                    if ( j < 10) {
                                        sprintf( &unit_no[3], "%d", j);
                                    } else { 
                                        sprintf( &unit_no[2], "%d", j);
                                    }
                                    name = (char *)getenv( unit_no);
                                    if ( !strcmp( psl->ar_files[i].fname, name))
                                        break;
                                }
                                if ( j <= 99)
                                    *arcfil = j;
                            }
                        }
                    }
                }
            }
        }
    }
}


/* INQUIRE CONFLICT RESOLUTION */
void
pqcnrs_( errind, arccr, retcr)
Pint	*errind;	/* OUT error indicator	*/
Pint	*arccr;		/* OUT archvival resolution	*/
Pint	*retcr;		/* OUT retrieval resolution	*/
{
    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else {
	*errind = 0;
	*arccr = (Pint)PSL_ARCHIVE_CONFLICT( phg_cur_cph->psl);
	*retcr = (Pint)PSL_RETRIEVE_CONFLICT( phg_cur_cph->psl);
    }
}


/* INQUIRE ALL CONFLICTING STRUCTURES */
void
pqcst_( afid, n, errind, ol, ostrid)
Pint	*afid;		/* archive identifier	*/
Pint	*n;		/* element of the structure identifier	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of structure identifier	*/
Pint	*ostrid;	/* OUT Nth structure identifier	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR7;

    } else if ( PSL_AR_STATE( phg_cur_cph->psl) != PAROP) {
	*errind = ERR7;

    } else if ( !phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
	*errind = ERR404;

    } else {
	cp_args.data.q_conflicting.op = PHG_ARGS_CONF_ALL;
	cp_args.data.q_conflicting.arid = *afid;
	ret.err = 0;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_CONFLICTING, &cp_args, &ret);
	if ( ret.err) {
	    *errind = ret.err;
	} else {
	    *errind = 0;
	    *ol = ret.data.int_list.num_ints;
	    if ( *ol > 0) {
		if ( *n < 0 || *n > *ol) {
		    *errind = ERR2002;
                } else {
		    *ostrid = ret.data.int_list.ints[*n - 1];
                }
            }
	}
    }
}


/* INQUIRE CONFLICTING STRUCTURES IN NETWORK */
void
pqcstn_( afid, strid, snsrc, n, errind, ol, ostrid)
Pint	*afid;		/* archive identifier	*/
Pint	*strid;		/* structure identifier	*/
Pint	*snsrc;		/* structure network source	*/
Pint	*n;		/* element of the structure identifier	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of structure identifier 	*/
Pint	*ostrid;	/* OUT Nth structure identifier	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR7;

    } else if ( PSL_AR_STATE( phg_cur_cph->psl) != PAROP) {
	*errind = ERR7;

    } else if ( !phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
	*errind = ERR404;

    } else if ( *snsrc < PCSS || *snsrc > PARCHV) {
	*errind = ERR2000;

    } else {
	cp_args.data.q_conflicting.op = PHG_ARGS_CONF_NET;
	cp_args.data.q_conflicting.arid = *afid;
	cp_args.data.q_conflicting.struct_id = *strid;
	cp_args.data.q_conflicting.src = (Pstruct_net_source)*snsrc;
	ret.err = 0;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_CONFLICTING, &cp_args, &ret);
	if ( ret.err) {
	    *errind = ret.err;
	} else {
	    *errind = 0;
	    *ol = ret.data.int_list.num_ints;
	    if ( *ol > 0) {
		if ( *n < 0 || *n > *ol) {
		    *errind = ERR2002;
                } else {
		    *ostrid = ret.data.int_list.ints[*n - 1];
                }
            }
	}
    }
}


/* RETRIVE PATHS TO ANCESTORS */
void
prepan_( afid, strid, pthord, pthdep, ipthsz, n, ol, apthsz, paths)
Pint	*afid;		/* archive identifier	*/
Pint	*strid;		/* structure identifier	*/
Pint	*pthord;	/* path order	*/
Pint	*pthdep;	/* path depth	*/
Pint	*ipthsz; 	/* size of path buffer	*/
Pint	*n;		/* element of list of pahts	*/
Pint	*ol;		/* OUT number of paths available	*/
Pint	*apthsz;	/* OUT size of the Nth structure path	*/
Pint	*paths;		/* OUT Nth structure path	*/
{
    Phg_args			cp_args;
    Phg_ret			ret;
    register Pelem_ref		*retp;
    register int		i;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR7, Pfn_ret_paths_ancest)) {
        if ( PSL_AR_STATE( phg_cur_cph->psl) != PAROP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR7);

	} else if ( *pthdep < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR207);

	} else if ( !phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR404);

	} else if ( *pthord < PPOTOP || *pthord > PPOBOT) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2000);

	} else {
	    cp_args.data.q_ar_hierarchy.arid = *afid;
	    cp_args.data.q_ar_hierarchy.hier.dir = PHG_ARGS_HIER_ANCESTORS;
	    cp_args.data.q_ar_hierarchy.hier.struct_id = *strid;
	    cp_args.data.q_ar_hierarchy.hier.order = (Ppath_order)*pthord;
	    cp_args.data.q_ar_hierarchy.hier.depth = *pthdep;
	    ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_AR_GET_HIERARCHY, &cp_args, &ret);
	    if ( !ret.err) {
                retp = ret.data.hierarchy.paths;
                *ol = ret.data.hierarchy.counts.num_ints;
                if ( *ol > 0) {
                    if ( *n < 0 || *n > *ol) {
	                ERR_REPORT( phg_cur_cph->erh, ERR2002);
                    } else { 
                        for ( i = 0; i < (*n - 1); i++) {
                            retp += ret.data.hierarchy.counts.ints[i];
                        }
                        *apthsz = ret.data.hierarchy.counts.ints[i];
                        if ( *ipthsz < *apthsz) {
	                    ERR_REPORT( phg_cur_cph->erh, ERR2001);

                        } else {
                            for ( i = 0; i < *apthsz; i++) {
                                paths[i * 2] = retp[i].struct_id;
                                paths[i * 2 + 1] = retp[i].elem_pos;
                            }
                        }
                    }
                } 
            }
	    ERR_FLUSH( phg_cur_cph->erh);
	}
    }
}


/* RETRIVE PATHS TO DESCENDANTS */
void
prepde_( afid, strid, pthord, pthdep, ipthsz, n, ol, apthsz, paths)
Pint	*afid;		/* archive identifier	*/
Pint	*strid;		/* structure identifier	*/
Pint	*pthord;	/* path order	*/
Pint	*pthdep;	/* path depth	*/
Pint	*ipthsz; 	/* size of path buffer	*/
Pint	*n;		/* element of list of pahts	*/
Pint	*ol;		/* OUT number of paths available	*/
Pint	*apthsz;	/* OUT size of the Nth structure path	*/
Pint	*paths;		/* OUT Nth structure path	*/
{
    Phg_args			cp_args;
    Phg_ret			ret;
    register Pelem_ref		*retp;
    register int		i;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR7, Pfn_ret_paths_descs)) {
        if ( PSL_AR_STATE( phg_cur_cph->psl) != PAROP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR7);

	} else if ( *pthdep < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR207);

	} else if ( !phg_psl_inq_ar_open( phg_cur_cph->psl, *afid)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR404);

	} else if ( *pthord < PPOTOP || *pthord > PPOBOT) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2000);

	} else {
	    cp_args.data.q_ar_hierarchy.arid = *afid;
	    cp_args.data.q_ar_hierarchy.hier.dir = PHG_ARGS_HIER_DESCENDANTS;
	    cp_args.data.q_ar_hierarchy.hier.struct_id = *strid;
	    cp_args.data.q_ar_hierarchy.hier.order = (Ppath_order)*pthord;
	    cp_args.data.q_ar_hierarchy.hier.depth = *pthdep;
	    ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_AR_GET_HIERARCHY, &cp_args, &ret);
	    if ( !ret.err) {
                retp = ret.data.hierarchy.paths;
                *ol = ret.data.hierarchy.counts.num_ints;
                if ( *ol > 0) {
                    if ( *n < 0 || *n > *ol) {
                        ERR_REPORT( phg_cur_cph->erh, ERR2002);
                    } else {
                        for ( i = 0; i < (*n - 1); i++) {
                            retp += ret.data.hierarchy.counts.ints[i];
                        }
                        *apthsz = ret.data.hierarchy.counts.ints[i];
                        if ( *ipthsz < *apthsz) {
                            ERR_REPORT( phg_cur_cph->erh, ERR2001);

                        } else {
                            for ( i = 0; i < *apthsz; i++) {
                                paths[i * 2] = retp[i].struct_id;
                                paths[i * 2 + 1] = retp[i].elem_pos;
                            }
                        }
                    }
                }
            }
	    ERR_FLUSH( phg_cur_cph->erh);
	}
    }
}
