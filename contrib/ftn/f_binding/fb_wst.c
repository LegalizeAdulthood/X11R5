#ifndef lint
static char     sccsid[] = "@(#)fb_wst.c 1.1 91/09/07 FJ";
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

/* Workstation type functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"
#include <X11/Xfuncproto.h>
#if NeedVarargsPrototypes
# include <stdarg.h>
# define Va_start(a,b) va_start(a,b)
#else
# include <varargs.h>
# define Va_start(a,b) va_start(a)
#endif



/* WORKSTATION TYPE SET */
void
fwtset_( wtype, wcf, wkcat, bmf, bfmode, dif, dyid, dnf, dyname, dwf, dwid,
hef, hdexpo, hdf, hddsty, dcmf, dcmodl, wf, width, hf, height, xf, x, yf, y,
wlf, label, wilf, ilabel, bwf, bwidth, len1, len2, len3)
Pint	*wtype;		/* workstation type	*/
Pint	*wcf;		/* workstation category flag	*/
Pint	*wkcat;		/* workstation category	*/
Pint	*bmf;		/* buffer bode flag	*/
Pint	*bfmode;	/* buffer bode	*/
Pint	*dif;		/* display identifier flag	*/
Pint	*dyid;		/* display identifier	*/
Pint	*dnf;		/* display name flag	*/
char	*dyname;	/* display name	*/
Pint	*dwf;		/* conection identifier flag	*/
Pint	*dwid;		/* conection identifier	*/
Pint	*hef;		/* handle expose mode flag	*/
Pint	*hdexpo;	/* handle expose mode	*/
Pint	*hdf;		/* handle expose destroy mode flag	*/
Pint	*hddsty;	/* handle expose destroy mode	*/
Pint	*dcmf;		/* resize DC model mode flag	*/
Pint	*dcmodl;	/* resize DC model mode	*/
Pint	*wf;		/* window width flag	*/
Pint	*width;		/* window width	*/
Pint	*hf;		/* window height flag	*/
Pint	*height;	/* window height	*/
Pint	*xf;		/* window position x flag	*/
Pint	*x;			/* window position x	*/
Pint	*yf;		/* window position y flag	*/
Pint	*y;			/* window position y	*/
Pint	*wlf;		/* window label flag	*/
char	*label;		/* window label	*/
Pint	*wilf;		/* window icon label flag	*/
char	*ilabel;	/* window icon label	*/
Pint	*bwf;		/* window border width flag	*/
Pint	*bwidth;	/* window border width	*/
Pint	*len1;		/* display name string length	*/
Pint	*len2;		/* label string length	*/
Pint	*len3;		/* icon label string length	*/
{
    register Wst	*wst;
    char		*avlist[32];
    F_dy_id		disp;
    F_dw_id		conn;
    Pint		err = 0;
    int			size = 0;
    int		    i = 0;
    char		*str, *w_str;

    bzero( (char *)avlist, (sizeof(char *) * 32) );
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_phigs_ws_type_set)) {
        if (!( wst = phg_fb_wst_exists( *wtype))) {
            ERR_REPORT( phg_cur_cph->erh, ERR52);

        } else if ( wst->bound_status != WST_UNBOUND) {
            /* Only operate on UNBOUND types */
            ERR_REPORT( phg_cur_cph->erh, ERRN100);

        } else {
            if ( *dnf == PON)
                size += (*len1 + 1) * sizeof(char);
            if ( *wlf == PON)
                size += (*len2 + 1) * sizeof(char);
            if ( *wilf == PON)
                size += (*len3 + 1) * sizeof(char);
            str = NULL;
            if ( size > 0 && !(str = (char *)malloc( size))) {
                err = ERR900;

            } else if ( size > 0) {
                w_str = str;
            } else {
                w_str = NULL;
            }

            if ( !err ) {
                if ( *wcf < POFF || *wcf > PON) {
                    err = ERR2000;
                } else if ( *wcf == PON) {
                    if ( *wkcat < POUTPT || *wkcat > PMI) {
                        err = ERR2000;
                    } else {
                        avlist[i++] = PHIGS_WS_CATEGORY;
                        avlist[i++] = (char *)(*wkcat);
                    }
                }
            }

            if ( !err ) {
                if ( *bmf < POFF || *bmf > PON) {
                    err = ERR2000;
                } else if ( *bmf == PON) {
                    if ( *bfmode < PSINGL || *bfmode > PDOUBL) {
                        err = ERR2000;
                    } else {
                        avlist[i++] = PHIGS_X_BUF_MODE;
                        avlist[i++] = (char *)(*bfmode);
                    }
                }
            }

            if ( !err ) {
                if ( *dif < POFF || *dif > PON) {
                    err = ERR2000;
                } else if ( *dif == PON) {
                    if (!( disp = phg_fb_search_display( *dyid))) {
                        err = ERRN200;
                    } else {
                        avlist[i++] = PHIGS_X_DISPLAY;
                        avlist[i++] = (char *)disp->disp_p;
                    }
                }
            }

            if ( !err ) {
                if ( *dnf < POFF || *dnf > PON) {
                    err = ERR2000;
                } else if ( *dnf == PON) {
                    avlist[i++] = PHIGS_X_DISPLAY_NAME;
                    bcopy((char *)dyname, (char *)w_str,
                        (*len1 * sizeof(char)));
                    w_str[*len1] = '\0';
                    avlist[i++] = (char *)w_str;
                    w_str = w_str + (*len1 + 1);
                }
            }

            if ( !err ) {
                if ( *dwf < POFF || *dwf > PON) {
                    err = ERR2000;
                } else if ( *dwf == PON) {
                    if (!( conn = phg_fb_search_connection( *dwid))) {
                        err = ERRN201;
                    } else {
                        avlist[i++] = PHIGS_X_DISPLAY_WINDOW;
                        avlist[i++] = (char *)conn->disp_p;
                        avlist[i++] = (char *)conn->wind_id;
                    }
                }
            }

            if ( !err ) {
                if ( *hef < POFF || *hef > PON) {
                    err = ERR2000;
                } else if ( *hef == PON) {
                    avlist[i++] = PHIGS_X_HANDLE_EXPOSE;
                    avlist[i++] = *hdexpo ? (char *)1: (char *)0;
                }
            }

            if ( !err ) {
                if ( *hdf < POFF || *hdf > PON) {
                    err = ERR2000;
                } else if ( *hdf == PON) {
                    avlist[i++] = PHIGS_X_HANDLE_DESTROY;
                    avlist[i++] = *hddsty ? (char *)1: (char *)0;
                }
            }

            if ( !err ) {
                if ( *dcmf < POFF || *dcmf > PON) {
                    err = ERR2000;
                } else if ( *dcmf == PON) {
                    avlist[i++] = PHIGS_DC_MODEL;
                    avlist[i++] = *dcmodl ? (char *)1: (char *)0;
                }
            }

            if ( !err ) {
                if ( *wf < POFF || *wf > PON) {
                    err = ERR2000;
                } else if ( *wf == PON) {
                    avlist[i++] = PHIGS_TOOL_WIDTH;
                    avlist[i++] = (char *)(*width);
                }
            }

            if ( !err ) {
                if ( *hf < POFF || *hf > PON) {
                    err = ERR2000;
                } else if ( *hf == PON) {
                    avlist[i++] = PHIGS_TOOL_HEIGHT;
                    avlist[i++] = (char *)(*height);
                }
            }

            if ( !err ) {
                if ( *xf < POFF || *xf > PON) {
                    err = ERR2000;
                } else if ( *xf == PON) {
                    avlist[i++] = PHIGS_TOOL_X;
                    avlist[i++] = (char *)(*x);
                }
            }

            if ( !err ) {
                if ( *yf < POFF || *yf > PON) {
                    err = ERR2000;
                } else if ( *yf == PON) {
                    avlist[i++] = PHIGS_TOOL_Y;
                    avlist[i++] = (char *)(*y);
                }
            }

            if ( !err ) {
                if ( *wlf < POFF || *wlf > PON) {
                    err = ERR2000;
                } else if ( *wlf == PON) {
                    avlist[i++] = PHIGS_TOOL_LABEL;
                    bcopy((char *)label, (char *)w_str,
                        (*len2 * sizeof(char)));
                    w_str[*len2] = '\0';
                    avlist[i++] = (char *)w_str;
                    w_str = w_str + (*len2 + 1);
                }
            }

            if ( !err ) {
                if ( *wilf < POFF || *wilf > PON) {
                    err = ERR2000;
                } else if ( *wilf == PON) {
                    avlist[i++] = PHIGS_TOOL_ICON_LABEL;
                    bcopy((char *)ilabel, (char *)w_str,
                        (*len3 * sizeof(char)));
                    w_str[*len3] = '\0';
                    avlist[i++] = (char *)w_str;
                    w_str = w_str + (*len3 + 1);
                }
            }

            if ( !err ) {
                if ( *bwf < POFF || *bwf > PON) {
                    err = ERR2000;
                } else if ( *bwf == PON) {
                    avlist[i++] = PHIGS_TOOL_BORDER_WIDTH;
                    avlist[i++] = (char *)(*bwidth);
                }
            }

            if ( err ) {
	            ERR_REPORT( phg_cur_cph->erh, err);
            } else {
                phg_wst_set_attrs( wst, avlist);
            }
            if ( size > 0 && str != NULL)
                free ( str);
        }
    }
}


/* WORKSTATION TYPE GET */
void
fwtget_( wtype, pbnf, wcf, bmf, dnf, hef, hdf, dcmf, wf, hf, xf, yf, wlf, wilf,
bwf, pbname, wkcat, bfmode, ldyna, dyname, hdexpo, hddsty, dcmodl, width,
height, x, y, llab, label, lilab, ilabel, bwidth, len1, len2, len3)
Pint	*wtype;		/* workstation type	*/
Pint	*pbnf;		/* base workstation type name flag	*/
Pint	*wcf;		/* workstation category flag	*/
Pint	*bmf;		/* buffer bode flag	*/
Pint	*dnf;		/* display name flag	*/
Pint	*hef;		/* handle expose mode flag	*/
Pint	*hdf;		/* handle expose destroy mode flag	*/
Pint	*dcmf;		/* resize DC model mode flag	*/
Pint	*wf;		/* window width flag	*/
Pint	*hf;		/* window height flag	*/
Pint	*xf;		/* window position x flag	*/
Pint	*yf;		/* window position y flag	*/
Pint	*wlf;		/* window label flag	*/
Pint	*wilf;		/* window icon label flag	*/
Pint	*bwf;		/* window border width flag	*/
Pint	*pbname;	/* OUT base workstation type name	*/
Pint	*wkcat;		/* OUT workstation category	*/
Pint	*bfmode;	/* OUT buffer bode	*/
Pint	*ldyna;		/* OUT length of display name	*/
char	*dyname;	/* OUT display name	*/
Pint	*hdexpo;	/* OUT handle expose mode	*/
Pint	*hddsty;	/* OUT handle expose destroy mode	*/
Pint	*dcmodl;	/* OUT resize DC model mode	*/
Pint	*width;		/* OUT window width	*/
Pint	*height;	/* OUT window height	*/
Pint	*x;			/* OUT window position x	*/
Pint	*y;			/* OUT window position y	*/
Pint	*llab;		/* OUT length of window label	*/
char	*label;		/* OUT window label	*/
Pint	*lilab;		/* OUT length of window icon label	*/
char	*ilabel;	/* OUT window icon label	*/
Pint	*bwidth;	/* OUT window border width	*/
Pint	*len1;		/* display name string length	*/
Pint	*len2;		/* label string length	*/
Pint	*len3;		/* icon label string length	*/
{
    Wst			*wst;
    char		*arg;	/* not used	*/
    Pint		err = 0;
    char		*str;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_phigs_ws_type_get)) {
        if (!( wst = phg_fb_wst_exists( *wtype))) {
            ERR_REPORT( phg_cur_cph->erh, ERR52);
        
        } else {
            if ( !err ) {
                if ( *pbnf < POFF || *pbnf > PON) {
                    err = ERR2000;
                } else if ( *pbnf == PON) {
                    *pbname = (Pint)phg_wst_get_attr( wst, PHG_BASE_NAME, arg);
                }
            }

            if ( !err ) {
                if ( *wcf < POFF || *wcf > PON) {
                    err = ERR2000;
                } else if ( *wcf == PON) {
                    *wkcat = (Pint)phg_wst_get_attr( wst, PHG_WS_CATEGORY, arg);
                }
            }

            if ( !err ) {
                if ( *bmf < POFF || *bmf > PON) {
                    err = ERR2000;
                } else if ( *bmf == PON) {
                    *bfmode = (Pint)phg_wst_get_attr( wst, PHG_X_BUF_MODE, arg);
                }
            }

            if ( !err ) {
                if ( *dnf < POFF || *dnf > PON) {
                    err = ERR2000;
                } else if ( *dnf == PON) {
                    str = (char *)phg_wst_get_attr( wst, PHG_X_DISPLAY_NAME,
                        arg);
                    *ldyna = MIN( *len1, strlen(str));
                    bcopy((char *)str, (char *)dyname, (*ldyna * sizeof(char)));
                }
            }

            if ( !err ) {
                if ( *hef < POFF || *hef > PON) {
                    err = ERR2000;
                } else if ( *hef == PON) {
                    *hdexpo = (Pint)phg_wst_get_attr( wst, PHG_X_HANDLE_EXPOSE,
				    	arg);
                }
            }

            if ( !err ) {
                if ( *hdf < POFF || *hdf > PON) {
                    err = ERR2000;
                } else if ( *hdf == PON) {
                    *hddsty = (Pint)phg_wst_get_attr( wst, PHG_X_HANDLE_DESTROY,
				    	arg);
                }
            }

            if ( !err ) {
                if ( *dcmf < POFF || *dcmf > PON) {
                    err = ERR2000;
                } else if ( *dcmf == PON) {
                    *dcmodl = (Pint)phg_wst_get_attr( wst, PHG_DC_MODEL, arg);
                }
            }

            if ( !err ) {
                if ( *wf < POFF || *wf > PON) {
                    err = ERR2000;
                } else if ( *wf == PON) {
                    *width = (Pint)phg_wst_get_attr( wst, PHG_TOOL_WIDTH, arg);
                }
            }

            if ( !err ) {
                if ( *hf < POFF || *hf > PON) {
                    err = ERR2000;
                } else if ( *hf == PON) {
                    *height = (Pint)phg_wst_get_attr( wst, PHG_TOOL_HEIGHT,
                        arg);
                }
            }

            if ( !err ) {
                if ( *xf < POFF || *xf > PON) {
                    err = ERR2000;
                } else if ( *xf == PON) {
                    *x = (Pint)phg_wst_get_attr( wst, PHG_TOOL_X, arg);
                }
            }

            if ( !err ) {
                if ( *yf < POFF || *yf > PON) {
                    err = ERR2000;
                } else if ( *yf == PON) {
                    *y = (Pint)phg_wst_get_attr( wst, PHG_TOOL_Y, arg);
                }
            }

            if ( !err ) {
                if ( *wlf < POFF || *wlf > PON) {
                    err = ERR2000;
                } else if ( *wlf == PON) {
                    str = (char *)phg_wst_get_attr( wst, PHG_TOOL_LABEL, arg);
                    *llab = MIN( *len2, strlen(str));
                    bcopy((char *)str, (char *)label, (*llab * sizeof(char)));
                }
            }

            if ( !err ) {
                if ( *wilf < POFF || *wilf > PON) {
                    err = ERR2000;
                } else if ( *wilf == PON) {
                    str = (char *)phg_wst_get_attr( wst, PHG_TOOL_ICON_LABEL,
                        arg);
                    *lilab = MIN( *len3, strlen(str));
                    bcopy((char *)str, (char *)ilabel, (*lilab * sizeof(char)));
                }
            }

            if ( !err ) {
                if ( *bwf < POFF || *bwf > PON) {
                    err = ERR2000;
                } else if ( *bwf == PON) {
                    *bwidth = (Pint)phg_wst_get_attr( wst,
                        PHG_TOOL_BORDER_WIDTH, arg);
                }
            }
            if ( err ) {
                ERR_REPORT( phg_cur_cph->erh, err);
            }
        }
    }
}


/* WORKSTATION TYPE CREATE */
void
fwtcre_( btype, wcf, wkcat, bmf, bfmode, dif, dyid, dnf, dyname, dwf, dwid,
hef, hdexpo, hdf, hddsty, dcmf, dcmodl, wf, width, hf, height, xf, x, yf, y,
wlf, label, wilf, ilabel, bwf, bwidth, nwtype, len1, len2, len3)
Pint	*btype;		/* base workstation type	*/
Pint	*wcf;		/* workstation category flag	*/
Pint	*wkcat;		/* workstation category	*/
Pint	*bmf;		/* buffer bode flag	*/
Pint	*bfmode;	/* buffer bode	*/
Pint	*dif;		/* display identifier flag	*/
Pint	*dyid;		/* display identifier	*/
Pint	*dnf;		/* display name flag	*/
char	*dyname;	/* display name	*/
Pint	*dwf;		/* conection identifier flag	*/
Pint	*dwid;		/* conection identifier	*/
Pint	*hef;		/* handle expose mode flag	*/
Pint	*hdexpo;	/* handle expose mode	*/
Pint	*hdf;		/* handle expose destroy mode flag	*/
Pint	*hddsty;	/* handle expose destroy mode	*/
Pint	*dcmf;		/* resize DC model mode flag	*/
Pint	*dcmodl;	/* resize DC model mode	*/
Pint	*wf;		/* window width flag	*/
Pint	*width;		/* window width	*/
Pint	*hf;		/* window height flag	*/
Pint	*height;	/* window height	*/
Pint	*xf;		/* window position x flag	*/
Pint	*x;			/* window position x	*/
Pint	*yf;		/* window position y flag	*/
Pint	*y;			/* window position y	*/
Pint	*wlf;		/* window label flag	*/
char	*label;		/* window label	*/
Pint	*wilf;		/* window icon label flag	*/
char	*ilabel;	/* window icon label	*/
Pint	*bwf;		/* window border width flag	*/
Pint	*bwidth;	/* window border width	*/
Pint	*nwtype;	/* OUT new workstation type	*/
Pint	*len1;		/* display name string length	*/
Pint	*len2;		/* label string length	*/
Pint	*len3;		/* icon label string length	*/
{
    Wst			*base = (Wst *)NULL;
    Wst			*wst = (Wst *)NULL;
    char		*avlist[32];
    F_dy_id		disp;
    F_dw_id		conn;
    Pint		err = 0;
    int			size = 0;
    int	    	i = 0;
    char		*str, *w_str;

    bzero( (char *)avlist, (sizeof(char *) * 32) );
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_phigs_ws_type_create)) {
        if (!(base = phg_fb_wst_exists( *btype))) {
            ERR_REPORT( phg_cur_cph->erh, ERR52);

        } else {
            if ( *dnf == PON)
                size += (*len1 + 1) * sizeof(char);
            if ( *wlf == PON)
                size += (*len2 + 1) * sizeof(char);
            if ( *wilf == PON)
                size += (*len3 + 1) * sizeof(char);
            str = NULL;
            if ( size > 0 && !(str = (char *)malloc( size))) {
                err = ERR900;

            } else if ( size > 0) {
                w_str = str;
            } else {
                w_str = NULL;
            }

            if ( !err ) {
                if ( *wcf < POFF || *wcf > PON) {
                    err = ERR2000;
                } else if ( *wcf == PON) {
                    if ( *wkcat < POUTPT || *wkcat > PMI) {
                        err = ERR2000;
                    } else {
                        avlist[i++] = PHIGS_WS_CATEGORY;
                        avlist[i++] = (char *)(*wkcat);
                    }
                }
            }

            if ( !err ) {
                if ( *bmf < POFF || *bmf > PON) {
                    err = ERR2000;
                } else if ( *bmf == PON) {
                    if ( *bfmode < PSINGL || *bfmode > PDOUBL) {
                        err = ERR2000;
                    } else {
                        avlist[i++] = PHIGS_X_BUF_MODE;
                        avlist[i++] = (char *)(*bfmode);
                    }
                }
            }

            if ( !err ) {
                if ( *dif < POFF || *dif > PON) {
                    err = ERR2000;
                } else if ( *dif == PON) {
                    if (!( disp = phg_fb_search_display( *dyid))) {
                        err = ERRN200;
                    } else {
                        avlist[i++] = PHIGS_X_DISPLAY;
                        avlist[i++] = (char *)disp->disp_p;
                    }
                }
            }

            if ( !err ) {
                if ( *dnf < POFF || *dnf > PON) {
                    err = ERR2000;
                } else if ( *dnf == PON) {
                    avlist[i++] = PHIGS_X_DISPLAY_NAME;
                    bcopy((char *)dyname, (char *)w_str,
                        (*len1 * sizeof(char)));
                    w_str[*len1] = '\0';
                    avlist[i++] = (char *)w_str;
                    w_str = w_str + (*len1) + 1;
                }
            }

            if ( !err ) {
                if ( *dwf < POFF || *dwf > PON) {
                    err = ERR2000;
                } else if ( *dwf == PON) {
                    if (!( conn = phg_fb_search_connection( *dwid))) {
                        err = ERRN201;
                    } else {
                        avlist[i++] = PHIGS_X_DISPLAY_WINDOW;
                        avlist[i++] = (char *)conn->disp_p;
                        avlist[i++] = (char *)conn->wind_id;
                    }
                }
            }

            if ( !err ) {
                if ( *hef < POFF || *hef > PON) {
                    err = ERR2000;
                } else if ( *hef == PON) {
                    avlist[i++] = PHIGS_X_HANDLE_EXPOSE;
                    avlist[i++] = *hdexpo ? (char *)1: (char *)0;
                }
            }

            if ( !err ) {
                if ( *hdf < POFF || *hdf > PON) {
                    err = ERR2000;
                } else if ( *hdf == PON) {
                    avlist[i++] = PHIGS_X_HANDLE_DESTROY;
                    avlist[i++] = *hddsty ? (char *)1: (char *)0;
                }
            }

            if ( !err ) {
                if ( *dcmf < POFF || *dcmf > PON) {
                    err = ERR2000;
                } else if ( *dcmf == PON) {
                    avlist[i++] = PHIGS_DC_MODEL;
                    avlist[i++] = *dcmodl ? (char *)1: (char *)0;
                }
            }

            if ( !err ) {
                if ( *wf < POFF || *wf > PON) {
                    err = ERR2000;
                } else if ( *wf == PON) {
                    avlist[i++] = PHIGS_TOOL_WIDTH;
                    avlist[i++] = (char *)(*width);
                }
            }

            if ( !err ) {
                if ( *hf < POFF || *hf > PON) {
                    err = ERR2000;
                } else if ( *hf == PON) {
                    avlist[i++] = PHIGS_TOOL_HEIGHT;
                    avlist[i++] = (char *)(*height);
                }
            }

            if ( !err ) {
                if ( *xf < POFF || *xf > PON) {
                    err = ERR2000;
                } else if ( *xf == PON) {
                    avlist[i++] = PHIGS_TOOL_X;
                    avlist[i++] = (char *)(*x);
                }
            }

            if ( !err ) {
                if ( *yf < POFF || *yf > PON) {
                    err = ERR2000;
                } else if ( *yf == PON) {
                    avlist[i++] = PHIGS_TOOL_Y;
                    avlist[i++] = (char *)(*y);
                }
            }

            if ( !err ) {
                if ( *wlf < POFF || *wlf > PON) {
                    err = ERR2000;
                } else if ( *wlf == PON) {
                    avlist[i++] = PHIGS_TOOL_LABEL;
                    bcopy((char *)label, (char *)w_str,
                        (*len2 * sizeof(char)));
                    w_str[*len2] = '\0';
                    avlist[i++] = (char *)w_str;
                    w_str = w_str + (*len2) + 1;
                }
            }

            if ( !err ) {
                if ( *wilf < POFF || *wilf > PON) {
                    err = ERR2000;
                } else if ( *wilf == PON) {
                    avlist[i++] = PHIGS_TOOL_ICON_LABEL;
                    bcopy((char *)ilabel, (char *)w_str,
                        (*len3 * sizeof(char)));
                    w_str[*len3] = '\0';
                    avlist[i++] = (char *)w_str;
                    w_str = w_str + (*len3) + 1;
                }
            }

            if ( !err ) {
                if ( *bwf < POFF || *bwf > PON) {
                    err = ERR2000;
                } else if ( *bwf == PON) {
                    avlist[i++] = PHIGS_TOOL_BORDER_WIDTH;
                    avlist[i++] = (char *)(*bwidth);
                }
            }

            if ( err ) {
	            ERR_REPORT( phg_cur_cph->erh, err);
            } else {
                if ( wst = phg_wst_create( phg_cur_cph->erh, base, avlist) ) {
                    if ( !phg_cp_add_wst( phg_cur_cph, wst) ) {
                        ERR_REPORT( phg_cur_cph->erh, ERR900);
                        phg_wst_destroy( wst);
                    } else if ( !phg_fb_add_ws_type( wst, nwtype)) {
                        ERR_REPORT( phg_cur_cph->erh, ERR900);
                        phg_wst_destroy( wst);
                    }
                }
            }
            if ( size > 0 && str != NULL)
                free ( str);
        }
    }
}


/* WORKSTATION TYPE DESTROY */
void
pwtdes_( wtype)
Pint	*wtype;		/* workstation type	*/
{
    Wst		*wst;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_phigs_ws_type_destroy)){
        if (!(wst = phg_fb_wst_exists( *wtype))) {
            ERR_REPORT( phg_cur_cph->erh, ERR52);

        } else if ( wst->bound_status != WST_UNBOUND) {
            /* Only operate on UNBOUND types */
            ERR_REPORT( phg_cur_cph->erh, ERRN100);

        } else {
	        phg_cp_rmv_wst( phg_cur_cph, wst);
            phg_wst_destroy( wst);
            phg_fb_delete_ws_type( *wtype);
        }
    }
}


/* INQUIRE LIST element OF AVAILABLE WORKSTATION TYPES */
void
pqewk_( n, errind, number, wktyp)
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*number;	/* OUT number of workstation types	*/
Pint	*wktyp;		/* OUT Nth element of list of workstation type	*/
{
    register Cp_wst_list_entry		*node;
    int			i;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else {
        *errind = 0;
        *number = 0;
        for ( node = phg_cur_cph->wst_list; node; node = node->next)
            ( *number)++;

        if ( *number > 0) {
            if ( *n < 0 || *n > *number) {
                *errind = ERR2002;
            } else {
                node = phg_cur_cph->wst_list;
                for( i = 1; i < *n; i++) {
                    node = node->next;
                }
                *wktyp = phg_fb_inq_ws_type( node->wst);
            }
        }
    }
}


/* INQUIRE NUMBER OF DISPLAY PRIORITIES SUPPORTED */
void
pqdp_( wtype, errind, nspsup)
Pint	*wtype;		/* workstation type	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nspsup;	/* OUT number of display priorities supported	*/
{ 
    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;
    
    } else if ( wst->bound_status != WST_BOUND) {
	*errind = ERR51;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else {
	*errind = 0;
	*nspsup = dt->out_dt.num_display_priorities;
    }
}


/* INQUIRE WORKSTATION CATEGORY */
void
pqwkca_( wtype, errind, wkcat)
Pint	*wtype;		/* workstation type	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*wkcat;		/* OUT workstation category	*/
{
    /* Error 51 is not generated by this function. */

    Wst			*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else {
	*errind = 0;
	*wkcat = (Pint)wst->desc_tbl.phigs_dt.ws_category;
    }
}

static void
get_ws_disp_size( wst, error_ind, size )
    Wst         *wst;
    Pint        *error_ind;
    Pdisp_space_size3   *size;
{
    Phg_args                    cp_args;
    Phg_ret                     ret;

    if ( wst->desc_tbl.xwin_dt.dc_model == PHIGS_DC_LIMITS_FIXED ) {
        Wst_phigs_dt            *dt = &wst->desc_tbl.phigs_dt;

        size->dc_units = dt->dev_coord_units;
        size->size_raster.size_x = dt->dev_addrs_units[0];
        size->size_raster.size_y = dt->dev_addrs_units[1];
        size->size_raster.size_z = dt->dev_addrs_units[2];
        size->size_dc.size_x = dt->dev_coords[0];
        size->size_dc.size_y = dt->dev_coords[1];
        size->size_dc.size_z = dt->dev_coords[2];
    } else {
        /* Assumes the ws is open, which it is if the wst is BOUND. */
        cp_args.data.idata = wst->wsid;
        ret.err = 0;
        CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_WIN_INFO, &cp_args, &ret);
        if ( ret.err) {
                *error_ind = ret.err;

        } else {
            *error_ind = 0;
            *size = ret.data.win_info.display_size;
        }
    }
}


static void
fb_inquire_display_size( wtype, error_ind, size)
Pint			wtype;		/* workstation type	*/
Pint			*error_ind;	/* OUT error indicator	*/
Pdisp_space_size3	*size;		/* OUT display size	*/
{
    /* Error 51 and 62 are not generated by this function */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*error_ind = ERR2;

    } else if (!(wst = phg_fb_wst_exists( wtype))) {
	*error_ind = ERR52;

    } else if ( (dt = &wst->desc_tbl.phigs_dt)->ws_category == PMI) {
	*error_ind = ERR57;

    } else {
	*error_ind = 0;
	switch ( wst->base_type) {
	case WST_BASE_TYPE_X_DRAWABLE:
	    if ( wst->bound_status != WST_BOUND) {
	        *error_ind = ERR51;
	    } else {
	        get_ws_disp_size( wst, error_ind, size);
	    }
	    break;
	case WST_BASE_TYPE_X_TOOL:
	    if ( wst->bound_status != WST_BOUND) {
	        size->dc_units = dt->dev_coord_units;
	        size->size_raster.size_x = dt->dev_addrs_units[0];
	        size->size_raster.size_y = dt->dev_addrs_units[1];
	        size->size_raster.size_z = dt->dev_addrs_units[2];
	        size->size_dc.size_x = dt->dev_coords[0];
	        size->size_dc.size_y = dt->dev_coords[1];
	        size->size_dc.size_z = dt->dev_coords[2];
	    } else {
	        get_ws_disp_size( wst, error_ind, size);
	    }
	    break;
	}
    }
}


/* INQUIRE DISPLAY SPACE SIZE 3 */
void
pqdsp3_( wtype, errind, dcunit, dx, dy, dz, rx, ry, rz)
Pint	*wtype;		/* workstation type	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*dcunit;	/* OUT device coodinate unit	*/
Pfloat	*dx;		/* OUT MAX display volume size X (DC)	*/
Pfloat	*dy;		/* OUT MAX display volume size Y (DC)	*/
Pfloat	*dz;		/* OUT MAX display volume size Z (DC)	*/
Pint	*rx;		/* OUT MAX display volume size X (raster unit)	*/
Pint	*ry;		/* OUT MAX display volume size Y (raster unit)	*/
Pint	*rz;		/* OUT MAX display volume size Z (raster unit)	*/
{
    Pdisp_space_size3	size;		/* work display size	*/

    fb_inquire_display_size( *wtype, errind, &size);
    if ( !*errind) {
        *dcunit = size.dc_units;
        *dx = size.size_dc.size_x;
        *dy = size.size_dc.size_y;
        *dz = size.size_dc.size_z;
        *rx = size.size_raster.size_x;
        *ry = size.size_raster.size_y;
        *rz = size.size_raster.size_z;
    }
}

/* INQUIRE DISPLAY SPACE SIZE */
void
pqdsp_( wtype, errind, dcunit, dx, dy, rx, ry)
Pint	*wtype;		/* workstation type	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*dcunit;	/* OUT device coodinate unit	*/
Pfloat	*dx;		/* OUT MAX display volume size X (DC) */
Pfloat	*dy;		/* OUT MAX display volume size Y (DC) */
Pint	*rx;		/* OUT MAX display volume size X (raster unit) */
Pint	*ry;		/* OUT MAX display volume size Y (raster unit) */
{
    Pdisp_space_size3		size;		/* work display size	*/

    fb_inquire_display_size( *wtype, errind, &size);
    if ( !*errind) {
	*dcunit = size.dc_units;
	*dx = size.size_dc.size_x;
	*dy = size.size_dc.size_y;
	*rx = size.size_raster.size_x;
	*ry = size.size_raster.size_y;
    }
}


/* INQUIRE HLHSR (IDENTIFIER) FACILITIES */
void
pqhrif_( wtype, ni, errind, nhrid, hrid)
Pint	*wtype;		/* workstation type	*/
Pint	*ni;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nhrid;		/* OUT number of HLHSR identifiers	*/
Pint	*hrid;		/* OUT NIth element of list of HLHSR identifier	*/
{
    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( wst->bound_status != WST_BOUND) {
	*errind = ERR51;

    } else if ( (dt = &wst->desc_tbl.phigs_dt)->ws_category == PMI) {
	*errind = ERR57;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else {
	*errind = 0;
	*nhrid = dt->num_hlhsr_ids;
	if ( *nhrid > 0) {
	    if ( *ni < 0 || *ni > *nhrid) {
		*errind = ERR2002;
            } else if ( *ni > 0) {
		*hrid = dt->hlhsr_ids[*ni - 1];
    	    }
        }
    }
}


/* INQUIRE HLHSR (MODE) FACILITIES */
void
pqhrmf_( wtype, nm, errind, nhrmd, hrmd)
Pint	*wtype;		/* workstation type	*/
Pint	*nm;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nhrmd;		/* OUT number of HLHSR modes	*/
Pint	*hrmd;		/* OUT NMth element of list of HLHR mode	*/
{
    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( wst->bound_status != WST_BOUND) {
	*errind = ERR51;

    } else if ( (dt = &wst->desc_tbl.phigs_dt)->ws_category == PMI) {
	*errind = ERR57;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else {
	*errind = 0;
	*nhrmd = dt->num_hlhsr_modes;
	if ( *nhrmd > 0) {
	    if ( *nm < 0 || *nm > *nhrmd) {
		*errind = ERR2002;
            } else if ( *nm > 0) {
		*hrmd = dt->hlhsr_modes[*nm - 1];
            }
	}
    }
}


/* INQUIRE VIEW FACILITIES */
void
pqvwf_( wtype, errind, npvwi)
Pint	*wtype;		/* workstation type	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*npvwi;		/* OUT number of predefined view indices	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( (dt = &wst->desc_tbl.phigs_dt)->ws_category == PMI) {
	*errind = ERR57;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else {
	*errind = 0;
	*npvwi = dt->num_predefined_views;
    }
}


/* INQUIRE PREDEFINED VIEW REPRESENTATION */
void
pqpvwr_( wtype, pvwi, errind, vwormt, vwmpmt, vwcplm, xyclpi, bclipi, fclipi)
Pint		*wtype;		/* workstation type	*/
Pint		*pvwi;		/* predefined view index	*/
Pint		*errind;	/* OUT error indicator	*/
Pmatrix3  	vwormt;		/* OUT view orientation matrix	*/
Pmatrix3  	vwmpmt;		/* OUT view mapping matrix	*/
Plimit3 	*vwcplm;	/* OUT view clipping limits	*/
Pint		*xyclpi;	/* OUT x-y clipping indicator	*/
Pint		*bclipi;	/* OUT back clipping indicator	*/
Pint		*fclipi;	/* OUT front clipping indicator	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( (dt = &wst->desc_tbl.phigs_dt)->ws_category == PMI) {
	*errind = ERR57;

    } else if ( *pvwi < 0) {
	*errind = ERR114;

    } else if ( *pvwi >= dt->num_predefined_views) {
	*errind = ERR101;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else {
	*errind = 0;
        MATRIX_DATA_TRANSLATE( 3, dt->default_views[*pvwi].ori_matrix, vwormt);
        MATRIX_DATA_TRANSLATE( 3, dt->default_views[*pvwi].map_matrix, vwmpmt);
	*vwcplm = dt->default_views[*pvwi].clip_limit;
	*xyclpi = (Pint)dt->default_views[*pvwi].xy_clip;
	*bclipi = (Pint)dt->default_views[*pvwi].back_clip;
	*fclipi = (Pint)dt->default_views[*pvwi].front_clip;
    }
}


/* INQUIRE WORKSTATION CLASSIFICATION */
void
pqwkcl_( wtype, errind, vrtype)
Pint	*wtype;		/* workstation type	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*vrtype;	/* OUT workstation classification	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else {
	*errind = 0;
	*vrtype = (Pint)dt->out_dt.ws_class;
    }
}


/* INQUIRE DYNAMICS OF WORKSTATION ATTRIBUTES */
void
pqdswa_( wtype, errind, plbun, pmbun, txbun, inbun, edbun, parep, colrep,
vwrep, wktr, hlfltr, infltr, hlhsr)
Pint	  *wtype;	/* workstation type	*/
Pint	  *errind;	/* OUT error indicator	*/
Pint	  *plbun;	/* OUT polyline representation changeable	*/
Pint	  *pmbun;	/* OUT polymarker representation changeable	*/
Pint	  *txbun;	/* OUT text representation changeable	*/
Pint	  *inbun;	/* OUT interior representation changeable	*/
Pint	  *edbun;	/* OUT edge representation changeable	*/
Pint	  *parep;	/* OUT pattern representation changeable	*/
Pint	  *colrep;	/* OUT colour representation changeable	*/
Pint	  *vwrep;	/* OUT view representation changeable	*/
Pint	  *wktr;	/* OUT workstation transformation changeable	*/
Pint	  *hlfltr;	/* OUT highlighting filter changeable	*/
Pint	  *infltr;	/* OUT invisibility filter changeable	*/
Pint	  *hlhsr;	/* OUT HLHSR mode changeable	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else {
	*errind = 0;
	*plbun = (Pint)dt->out_dt.polyline_bundle_rep;
	*pmbun = (Pint)dt->out_dt.polymarker_bundle_rep;
	*txbun = (Pint)dt->out_dt.text_bundle_rep;
	*inbun = (Pint)dt->out_dt.interior_bundle_rep;
	*edbun = (Pint)dt->out_dt.edge_bundle_rep;
	*parep = (Pint)dt->out_dt.pattern_rep;
	*colrep = (Pint)dt->out_dt.colour_rep;
	*vwrep = (Pint)dt->out_dt.view_rep;
	*wktr = (Pint)dt->out_dt.ws_xform;
	*hlfltr = (Pint)dt->out_dt.highlight_filter;
	*infltr = (Pint)dt->out_dt.invis_filter;
	*hlhsr = (Pint)dt->out_dt.hlhsr_mode;
    }
}


/* INQUIRE DEFAULT DISPLAY UPDATE STATE */
void
pqddus_( wtype, errind, defmod, modmod)
Pint	*wtype;		/* workstation type	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*defmod;	/* OUT deferral mode	*/
Pint	*modmod;	/* OUT modification mode	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else {
	*errind = 0;
	*defmod = (Pint)dt->out_dt.deferral_mode;
	*modmod = (Pint)dt->out_dt.modification_mode;
    }
}


/* INQUIRE POLYLINE FACILITIES */
void
pqplf_( wtype, n, errind, nlt, lt, nlw, nomlw, rlwmin, rlwmax, nppli)
Pint	*wtype;		/* workstation type	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nlt;		/* OUT number of linetypes	*/
Pint	*lt;		/* OUT Nth element of list of linetype	*/
Pint	*nlw;		/* OUT number of linewidths	*/
Pfloat	*nomlw;		/* OUT nominal linewidth	*/
Pfloat	*rlwmin;	/* OUT range of linewidths MIN	*/
Pfloat	*rlwmax;	/* OUT range of linewidths MAX	*/
Pint	*nppli;		/* OUT number of polyline indices	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else {
	*errind = 0;
	*nlt = dt->out_dt.num_linetypes;
	if ( *nlt > 0) {
	    if ( *n < 0 || *n > *nlt) {
                *errind = ERR2002;
                return;
            } else if( *n > 0) {
	        *lt = dt->out_dt.linetypes[*n - 1];
	    }
        }
	*nlw = dt->out_dt.num_linewidths;
	*nomlw = dt->out_dt.nominal_linewidth;
	*rlwmin = dt->out_dt.min_linewidth;
	*rlwmax = dt->out_dt.max_linewidth;
	*nppli = dt->out_dt.num_predefined_polyline_indices;
    }
}


/* INQUIRE POLYLINE FACILITIES PLUS */
void
pqplfp_( wtype, nrlt, nrsm, errind, nlt, lt, nlw, nomlw, rlwmin, rlwmax, nsm,
sm, nppli)
Pint	*wtype;		/* workstation type	*/
Pint	*nrlt;		/* list elemsen requested of linetypes	*/
Pint	*nrsm;		/* list element requested of shading method	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nlt;		/* OUT number of linetype	*/
Pint	*lt;		/* OUT NRLTth element of list of linetype	*/
Pfloat	*nlw;		/* OUT number of linewidth	*/
Pfloat	*nomlw;		/* OUT normal linewidth	*/
Pfloat	*rlwmin;	/* OUT range of linewidth MIN	*/
Pfloat	*rlwmax;	/* OUT range of linewidth MAX	*/
Pint	*nsm;		/* OUT number of shading method	*/
Pint	*sm;		/* OUT NRSMth element of list of shading method	*/
Pint	*nppli;		/* OUT number of predefined polyline indicies	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst	  			*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN) ) {
	*errind = ERR62;

    } else {
	*errind = 0;
	*nlt = dt->out_dt.num_linetypes;
	if ( *nlt > 0) {
	    if ( *nrlt < 0 || *nrlt > *nlt) {
                *errind = ERR2002;
                return;
            } else if ( *nrlt < 0) {
	        *lt = dt->out_dt.linetypes[*nrlt - 1];
            }
	}
	*nsm = dt->out_dt.num_polyline_shading_methods;
	if ( *nsm > 0) {
	    if ( *nrsm < 0 || *nrsm > *nsm) {
                *errind = ERR2002;
                return;
            } else if ( *nrsm < 0) {
	        *sm = dt->out_dt.polyline_shading_methods[*nrsm - 1];
            }
	}
	*nlw = dt->out_dt.num_linewidths;
	*nomlw = dt->out_dt.nominal_linewidth;
	*rlwmin = dt->out_dt.min_linewidth;
	*rlwmax = dt->out_dt.max_linewidth;
	*nppli = dt->out_dt.num_predefined_polyline_indices;
    }
}


/* INQUIRE PREDEFINED POLYLINE REPRESENTATION */
void
pqpplr_( wtype, pli, errind, ltype, lwidth, coli)
Pint	*wtype;		/* workstation type	*/
Pint	*pli;		/* predefined polyline index	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ltype;		/* OUT linetype	*/
Pfloat	*lwidth;	/* OUT linewidth scale factor	*/
Pint	*coli;		/* OUT polyline colour index	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( *pli < 1) {
	*errind = ERR100;

    } else if ( *pli > dt->out_dt.num_predefined_polyline_indices) {
	*errind = ERR102;

    } else {
        Pline_bundle_plus *lb =
	    &dt->out_dt.default_polyline_bundle_table[*pli - 1];

	*errind = 0;
	*ltype = lb->type;
	*lwidth = lb->width;
        if ( lb->colr.type != PINDCT) {
            *errind = ERR134;
        } else {
	    *coli = lb->colr.val.ind;
        }
    }
}


/* INQUIRE PREDEFINED POLYLINE REPRESENTATION PLUS */
void
pqpplp_( wtype, pli, ccsbsz, errind, ltype, lwidth, ctype, coli, ol, colr,
plsm, acri, aval)
Pint	*wtype;		/* workstation type	*/
Pint	*pli;		/* predefined polyline index	*/
Pint	*ccsbsz;	/* colour component buffer size	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ltype;		/* OUT linetype	*/
Pfloat	*lwidth;	/* OUT linewidth scale factor	*/
Pint	*ctype;		/* OUT colour type	*/
Pint	*coli;		/* OUT colour index	*/
Pint	*ol;		/* OUT number of colour components	*/
Pfloat	*colr;		/* OUT colour specification	*/
Pint	*plsm;		/* OUT polyline shading method	*/
Pint	*acri;		/* OUT curve approximation criteria type	*/
Pint	*aval;		/* OUT curve approximation criteria values	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( *pli < 1) {
	*errind = ERR100;

    } else if ( *pli > dt->out_dt.num_predefined_polyline_indices) {
	*errind = ERR102;

    } else {
        Pline_bundle_plus *lb =
	    &dt->out_dt.default_polyline_bundle_table[*pli - 1];

	*errind = 0;
	*ltype = lb->type;
	*lwidth = lb->width;
	*plsm = lb->shad_meth;
	*acri = lb->approx_type;
	*aval = lb->approx_val;
        GCOLR_DATA_RE_TRANSLATE_PLUS( lb->colr, *ccsbsz, *errind, *ctype, *coli,
            *ol, colr);
    }
}


/* INQUIRE POLYMARKER FACILITIES */
void
pqpmf_( wtype, n, errind, nmt, mt, nms, nomms, rmsmin, rmsmax, nppmi)
Pint	*wtype;		/* workstation type	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nmt;		/* OUT number of marker types	*/
Pint	*mt;		/* OUT Nth element of list of marker type	*/
Pint	*nms;		/* OUT number of marker sizes	*/
Pfloat	*nomms;		/* OUT nominal marker size	*/
Pfloat	*rmsmin;	/* OUT range marker sizes NIN	*/
Pfloat	*rmsmax;	/* OUT range marker sizes MAX	*/
Pint	*nppmi;		/* OUT number of marker indices	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else {
	*errind = 0;
	*nmt = dt->out_dt.num_marker_types;
	if ( *nmt > 0) {
	    if ( *n < 0 || *n > *nmt) {
	        *errind = ERR2002;
                return;
            } else if ( *n > 0) {
	        *mt = dt->out_dt.marker_types[*n - 1]; 
	    }
        }
	*nms = dt->out_dt.num_marker_sizes;
	*nomms = dt->out_dt.nominal_marker_size;
	*rmsmin = dt->out_dt.min_marker_size;
	*rmsmax = dt->out_dt.max_marker_size;
	*nppmi = dt->out_dt.num_predefined_polymarker_indices;
    }
}


/* INQUIRE PREDEDINED POLYMARKER REPRESENTATION */
void
pqppmr_( wtype, pmi, errind, mtype, mszsf, coli)
Pint	*wtype;		/* workstation type	*/
Pint	*pmi;		/* predefined polymarker index	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mtype;		/* OUT marker type	*/
Pfloat	*mszsf;		/* OUT marker size scale factor	*/
Pint	*coli;		/* OUT polymarker colour index	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( *pmi < 1) {
	*errind = ERR100;

    } else if ( *pmi > dt->out_dt.num_predefined_polymarker_indices) {
	*errind = ERR102;

    } else {
	Pmarker_bundle_plus *mb =
             &dt->out_dt.default_polymarker_bundle_table[*pmi - 1];

	*errind = 0;
	*mtype = mb->type;
	*mszsf = mb->size;
	if ( mb->colr.type != PINDCT) {
            *errind = ERR134;
        } else {
	    *coli = mb->colr.val.ind;
        }
    }
}


/* INQUIRE PREDEFINED POLYMARKER REPRESENTATION PLUS */
void
pqppmp_( wtype, pmi, ccsbsz, errind, mtype, mszsf, ctype, coli, ol, colr)
Pint	*wtype;		/* workstation type	*/
Pint	*pmi;		/* predefined polymarker index	*/
Pint	*ccsbsz;	/* colour component buffer size	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mtype;		/* OUT markertype	*/
Pfloat	*mszsf;		/* OUT marker size scale factor	*/
Pint	*ctype;		/* OUT colour type	*/
Pint	*coli;		/* OUT colour index	*/
Pint	*ol;		/* OUT number of colour components	*/
Pfloat	*colr;		/* OUT colour specification	*/
{
    /* Error 51 and 62 not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( *pmi < 1) {
	*errind = ERR100;

    } else if ( *pmi > dt->out_dt.num_predefined_polymarker_indices) {
	*errind = ERR102;

    } else {
	Pmarker_bundle_plus *mb =
             &dt->out_dt.default_polymarker_bundle_table[*pmi - 1];

	*errind = 0;
	*mtype = mb->type;
	*mszsf = mb->size;
        GCOLR_DATA_RE_TRANSLATE_PLUS( mb->colr, *ccsbsz, *errind, *ctype, *coli,
            *ol, colr);
    }
}


/* INQUIRE TEXT FACILITIES */
void
pqtxf_( wtype, n, errind, nfpp, font, prec, nchh, minchh, maxchh, nchx,
minchx, maxchx, nptxi)
Pint	*wtype;		/* workstation type	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nfpp;	 	/* OUT number of text font precision paire	*/
Pint	*font;	 	/* OUT Nth element of list of text fonts 	*/
Pint	*prec;		/* OUT Nth element of list of text precisions	*/
Pint	*nchh;	 	/* OUT number of character heights	*/
Pfloat	*minchh;	/* OUT MIN character height	*/
Pfloat	*maxchh;	/* OUT MAX character height	*/
Pint	*nchx;		/* OUT number of character expansion factors	*/
Pfloat	*minchx;	/* OUT MIN character expansion factor	*/
Pfloat	*maxchx;	/* OUT MAX character expansion factor	*/
Pint	*nptxi;		/* OUT number of predefined text indices	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;
    int				charset_index;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else {
	*errind = 0;
	/* Since the character set is not specified by this function, use
	 * ASCII, which is in slot 0 of the text pair table */
	charset_index = phg_cb_charset_index(dt,PCS_ASCII);
	*nfpp = dt->out_dt.num_text_pairs[charset_index];
	if ( *nfpp > 0) {
	    if ( *n < 0 || *n > *nfpp) {
                *errind = ERR2002;
                return;
	    } else if ( *n > 0) {
	        *font = dt->out_dt.text_pairs[charset_index][*n - 1].font; 
	        *prec = (Pint)dt->out_dt.text_pairs[charset_index][*n - 1].prec;
            }
	}
	*nchh = dt->out_dt.num_char_heights;
	*minchh = dt->out_dt.min_char_height;
	*maxchh = dt->out_dt.max_char_height;
	*nchx = dt->out_dt.num_char_expansion_factors;
	*minchx = dt->out_dt.min_char_expansion_factor;
	*maxchx = dt->out_dt.max_char_expansion_factor;
        *nptxi = dt->out_dt.num_predefined_text_indices;
    }
}


/* INQUIRE PREDEFINED TEXT REPRESENTATION */
void
pqptxr_( wtype, ptxi, errind, font, prec, chxp, chsp, coli)
Pint	*wtype;		/* workstation type	*/
Pint	*ptxi;		/* predefined text index	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*font;		/* OUT text font	*/
Pint	*prec;		/* OUT text precision	*/
Pfloat	*chxp;		/* OUT character expansion factor	*/
Pfloat	*chsp;		/* OUT character spacing	*/
Pint	*coli;		/* OUT text colour index	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( *ptxi < 1) {
	*errind = ERR100;

    } else if ( *ptxi > dt->out_dt.num_predefined_text_indices) {
	*errind = ERR102;

    } else {
	Ptext_bundle_plus *tb =
            &dt->out_dt.default_text_bundle_table[*ptxi - 1];

	*errind = 0;
	*font = tb->font;
	*prec = (Pint)tb->prec;
	*chxp = tb->char_expan;
	*chsp = tb->char_space;
	if ( tb->colr.type != PINDCT) {
            *errind = ERR134;
        } else {
	    *coli = tb->colr.val.ind;
        }
    }
}


/* INQUIRE PREDEFINED TEXT REPRESENTATION PLUS */
void
pqptxp_( wtype, ptxi, ccsbsz, errind, font, prec, chxp, chsp, ctype, coli, ol,
colr)
Pint	*wtype;		/* workstation type	*/
Pint	*ptxi;		/* predefined text index	*/
Pint	*ccsbsz;	/* colour component buffer size	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*font;		/* OUT text font	*/
Pint	*prec;		/* OUT text precision	*/
Pfloat	*chxp;		/* OUT character expansion factor	*/
Pfloat	*chsp;		/* OUT character spacing	*/
Pint	*ctype;		/* OUT colour type	*/
Pint	*coli;		/* OUT colour index	*/
Pint	*ol;		/* OUT number of colour components	*/
Pfloat	*colr;		/* OUT colour specification	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( *ptxi < 1) {
	*errind = ERR100;

    } else if ( *ptxi > dt->out_dt.num_predefined_text_indices) {
	*errind = ERR102;

    } else {
	Ptext_bundle_plus *tb =
            &dt->out_dt.default_text_bundle_table[*ptxi - 1];

	*errind = 0;
	*font = tb->font;
	*prec = (Pint)tb->prec;
	*chxp = tb->char_expan;
	*chsp = tb->char_space;
        GCOLR_DATA_RE_TRANSLATE_PLUS( tb->colr, *ccsbsz, *errind, *ctype, *coli,
            *ol, colr);
    }
}


/* INQUIRE ANNOTATION FACILITIES */
void
pqanf_( wtype, n, errind, nas, as, nchh, minchh, maxchh)
Pint	*wtype;		/* workstation type	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nas;		/* OUT number of annotation styles	*/
Pint	*as;		/* OUT Nth element of list of annotation styles	*/
Pint	*nchh;		/* OUT number of annotation text character heights */
Pfloat	*minchh;	/* OUT MIN annotation text character height	*/
Pfloat	*maxchh;	/* OUT MAX annotation text character height	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
        *errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
        || dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
        *errind = ERR59;

    } else if ( dt->ws_category == PMO) {
        *errind = ERR62;

    } else {
        *errind = 0;
        *nas = dt->out_dt.num_annotation_styles;
        if ( *nas > 0) {
            if ( *n < 0 || *n > *nas) {
                *errind = ERR2002;
            } else if ( *n > 0) {
                *as = dt->out_dt.annotation_styles[*n - 1];
            }
        }
        *nchh = dt->out_dt.num_char_heights;
        *minchh = dt->out_dt.min_char_height;
        *maxchh = dt->out_dt.max_char_height;
    }
}


/* INQUIRE INTERIOR FACILITIES */
void
pqif_( wtype, ni, nh, errind, nis, is, nhs, hs, npfai)
Pint	*wtype;		/* workstation type	*/
Pint	*ni;		/* list element of interior styles requested	*/
Pint	*nh;		/* list element of hatch styles requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nis;		/* OUT number of interior styles	*/
Pint	*is;		/* OUT NIth element of list of interior styles	*/
Pint	*nhs;		/* OUT number of hatch styles	*/
Pint	*hs;		/* OUT NHth element of list of hatch styles	*/
Pint	*npfai;		/* OUT number of predefined interior indices	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else {
	*errind = 0;
	*nis = MIN(5, dt->out_dt.num_interior_styles);
	if ( *nis > 0) {
	    if ( *ni < 0 || *ni > *nis) {
                *errind = ERR2002;
                return;
	    } else if ( *ni > 0) {
	        *is = (Pint)dt->out_dt.interior_styles[*ni - 1];
            }
	}
	*nhs = dt->out_dt.num_hatch_styles;
	if ( *nhs > 0 ) {
	    if ( *nh < 0 || *nh > *nhs) {
                *errind = ERR2002;
                return;
            } else if ( *nh > 0) {
	        *hs = dt->out_dt.hatch_styles[*nh - 1];
            }
	}
	*npfai = dt->out_dt.num_predefined_interior_indices;
    }
}


/* INQUIRE INTERIOR FACILITIES PLUS */
void
pqifp_( wtype, nris, nrhs, nrrq, nrsm, errind, nis, is, nhs, hs, nrq, rq,
nsm, sm, npfai)
Pint	*wtype;		/* workstation type	*/
Pint	*nris;		/* list element of interior style requested	*/
Pint	*nrhs;		/* list element of hatch style requested	*/
Pint	*nrrq;		/* list element of reflectance equations requested */
Pint	*nrsm;		/* list element of shading method requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nis;		/* OUT number of interior styles	*/
Pint	*is;		/* OUT NRISth element of list of interior styles */
Pint	*nhs;		/* OUT number of hatch styles	*/
Pint	*hs;		/* OUT NRHSth element of list of hatch styles	*/
Pint	*nrq;		/* OUT number of reflectance equations	*/
Pint	*rq;		/* OUT NRRQth element of list of reflectance equations*/
Pint	*nsm;		/* OUT number of shading method	*/
Pint	*sm;		/* OUT NRSMth element of list of shading method	*/
Pint	*npfai;		/* OUT number of predefined interior indices	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else {
	*errind = 0;
	*nis = dt->out_dt.num_interior_styles;
	if ( *nis > 0) {
	    if ( *nris < 0 || *nris > *nis) {
                *errind = ERR2002;
                return;
            } else if ( *nris > 0) {
                *is = (Pint)dt->out_dt.interior_styles[*nris - 1];
            }
	}
	*nhs = dt->out_dt.num_hatch_styles;
	if ( *nhs > 0 ) {
	    if ( *nrhs < 0 || *nrhs > *nhs) {
                *errind = ERR2002;
                return;
            } else if ( *nrhs > 0) {
	        *hs = dt->out_dt.hatch_styles[*nrhs - 1];
            }
	}
	*nrq = dt->out_dt.num_refeqs;
	if ( *nrq > 0 ) {
	    if ( *nrrq < 0 || *nrrq > *nrq) {
                *errind = ERR2002;
                return;
            } else if ( *nrrq > 0) {
	        *rq = dt->out_dt.refl_eqns[*nrrq - 1];
            }
	}
	*nsm = dt->out_dt.num_interior_shading_methods;
	if ( *nsm > 0 ) {
	    if ( *nrsm < 0 || *nrsm > *nsm) {
                *errind = ERR2002;
                return;
            } else if ( *nrsm > 0) {
	        *sm = dt->out_dt.polyline_shading_methods[*nrsm - 1];
            }
	}
	*npfai = dt->out_dt.num_predefined_interior_indices;
    }
}


/* INQUIRE PREDEFINED INTERIOR REPRESENTATION */
void
pqpir_( wtype, pii, errind, style, stylid, coli)
Pint	*wtype;		/* workstation type	*/
Pint	*pii;		/* predefined interior index	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*style;		/* OUT interior style	*/
Pint	*stylid;	/* OUT interior style index	*/
Pint	*coli;		/* OUT interior colour index	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( *pii < 1) {
	*errind = ERR100;

    } else if ( *pii > dt->out_dt.num_predefined_interior_indices) {
	*errind = ERR102;

    } else {
	Pint_bundle_plus *ib =
	    &dt->out_dt.default_interior_bundle_table[*pii - 1];

	*errind = 0;
	*style = (Pint)ib->style;
	*stylid = ib->style_ind;
	if ( ib->colr.type != PINDCT) {
            *errind = ERR134;
        } else {
	    *coli = ib->colr.val.ind;
        }
    }
}


/* INQUIRE PREDEFINED INTERIOR REPRESENTATION PLUS */
void
pqpirp_( wtype, pii, iccss, biccss, sccss, bsccss, errind, ictyp, icoli, iol,
icolr, bictyp, bicoli, biol, bicolr, ints, bints, styli, bstyli, irfeq, birfeq,
ism, bism, amrc, dirc, sprc, sctyp, scoli, sol, scolr, spex, bamrc, bdirc,
bsprc, bsctyp, bscoli, bsol, bscolr, bspex, acri, aval)
Pint	*wtype;		/* workstation type	*/
Pint	*pii;		/* predefined interior index	*/
Pint    *iccss;		/* interior colour component buffer size        */
Pint    *biccss;	/* back interior colour component buffer size   */
Pint    *sccss;		/* specular colour component buffer size        */
Pint    *bsccss;	/* back specular colour component buffer size   */
Pint	*errind;	/* OUT error indicator	*/
Pint    *ictyp;		/* OUT interior colour type     */
Pint    *icoli;		/* OUT interior colour index    */
Pint    *iol;		/* OUT number of interior colour components     */
Pfloat  *icolr;		/* OUT interior colour specification    */
Pint    *bictyp;	/* OUT back interior colour type        */
Pint    *bicoli;	/* OUT back interior colour index       */
Pint    *biol;		/* OUT number of back interior colour components */
Pfloat  *bicolr;	/* OUT back interior colour specification       */
Pint	*ints;		/* OUT interior style	*/
Pint	*bints;		/* OUT back interior style	*/
Pint	*styli;		/* OUT interior style index */
Pint	*bstyli;	/* OUT back interior style index */
Pint	*irfeq;		/* OUT interior reflectance equation	*/
Pint	*birfeq;	/* OUT back interior reflectance equation	*/
Pint	*ism;		/* OUT interior shading method	*/
Pint	*bism;		/* OUT back interior shading method	*/
Pfloat	*amrc;		/* OUT ambient reflection coefficent	*/
Pfloat	*dirc;		/* OUT diffuse reflection coefficent	*/
Pfloat	*sprc;		/* OUT specular reflection coefficent	*/
Pint    *sctyp;		/* OUT specular colour type     */
Pint    *scoli;		/* OUT specular colour index    */
Pint    *sol;		/* OUT number of specular colour components     */
Pfloat  *scolr;		/* OUT specular colour specification    */
Pfloat	*spex;		/* OUT specular exponent	*/
Pfloat	*bamrc;		/* OUT back ambient reflection coefficent	*/
Pfloat	*bdirc;		/* OUT back diffuse reflection coefficent	*/
Pfloat	*bsprc;		/* OUT back specular reflection coefficent	*/
Pint    *bsctyp;	/* OUT back specular colour type        */
Pint    *bscoli;	/* OUT back specular colour index       */
Pint    *bsol;		/* OUT number of back specular colour components */
Pfloat  *bscolr;	/* OUT specular back colour specification       */
Pfloat	*bspex;		/* OUT back specular exponent	*/
Pint	*acri;		/* OUT surface approximation criteria type	*/
Pfloat	*aval;		/* OUT surface approximation criteria values */
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( *pii < 1) {
	*errind = ERR100;

    } else if ( *pii > dt->out_dt.num_predefined_interior_indices) {
	*errind = ERR102;

    } else {
	Pint_bundle_plus *ib =
	    &dt->out_dt.default_interior_bundle_table[*pii - 1];

	*errind = 0;
        /* pack interior colour data record */ 
        GCOLR_DATA_RE_TRANSLATE_PLUS( ib->colr, *iccss, *errind, *ictyp, *icoli,
            *iol, icolr);
        if ( *errind) {
            return;
        }
        /* pack back interior colour data record */ 
        GCOLR_DATA_RE_TRANSLATE_PLUS( ib->back_colr, *biccss, *errind, *bictyp,
            *bicoli, *biol, bicolr);
        if ( *errind) {
            return;
        }
        *ints = (Pint)ib->style;
        *bints = (Pint)ib->back_style;
        *styli = ib->style_ind;
        *bstyli = ib->back_style_ind;
        *irfeq = ib->refl_eqn;
        *birfeq = ib->back_refl_eqn;
        *ism = ib->shad_meth;
        *bism = ib->back_shad_meth;
        *amrc = ib->refl_props.ambient_coef;
        *dirc = ib->refl_props.diffuse_coef;
        *sprc = ib->refl_props.specular_coef;
        /* pack specular colour data record	*/
        GCOLR_DATA_RE_TRANSLATE_PLUS( ib->refl_props.specular_colr, *sccss,
            *errind, *sctyp, *scoli, *sol, scolr);
        if ( *errind) {
            return;
        }
        *spex = ib->refl_props.specular_exp;

        *bamrc = ib->back_refl_props.ambient_coef;
        *bdirc = ib->back_refl_props.diffuse_coef;
        *bsprc = ib->back_refl_props.specular_coef;
        /* pack back specular colour data record	*/
        GCOLR_DATA_RE_TRANSLATE_PLUS( ib->back_refl_props.specular_colr,
            *bsccss, *errind, *bsctyp, *bscoli, *bsol, bscolr);
        if ( *errind) {
            return;
        }
        *bspex = ib->back_refl_props.specular_exp;
        *acri = ib->approx_type;
        aval[0] = ib->approx_val[0];
        aval[1] = ib->approx_val[1];
    }
}


/* INQUIRE EDGE FACILITIES */
void
pqedf_( wtype, n, errind, nedt, edt, nedw, nomedw, redwmn, redwmx, npedi)
Pint	*wtype;		/* workstation type	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nedt;		/* OUT number of edgetypes	*/
Pint	*edt;		/* OUT Nth element of list of edgetypes	*/
Pint	*nedw;		/* OUT number of edgewidths	*/
Pfloat	*nomedw;	/* OUT nominal edgewidth	*/
Pfloat	*redwmn;	/* OUT range of edgewidths MIN	*/
Pfloat	*redwmx;	/* OUT range of edgewidths MAX	*/
Pint	*npedi;		/* OUT number of predefined edge indices	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else {
	*errind = 0;
	*nedt = dt->out_dt.num_edge_types;
	if ( *nedt > 0) {
	    if ( *n < 0 || *n > *nedt) {
                *errind = ERR2002;
                return;
            } else if ( *n > 0) {
	        *edt = dt->out_dt.edge_types[*n - 1];
            }
        }
	*nedw = dt->out_dt.num_edgewidths;
	*nomedw = dt->out_dt.nominal_edgewidth;
	*redwmn = dt->out_dt.min_edgewidth;
	*redwmx = dt->out_dt.max_edgewidth;
	*npedi = dt->out_dt.num_predefined_edge_indices;
    }
}


/* INQUIRE PREDEFINED EDGE REPRESENTATION */
void
pqpedr_( wtype, pedi, errind, edflag, edtype, ewidth, coli)
Pint	*wtype;		/* workstation type	*/
Pint	*pedi;		/* predefined edge index	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*edflag;	/* OUT edge flag	*/
Pint	*edtype;	/* OUT edgetype	*/
Pfloat	*ewidth;	/* OUT edgewidth scale factor	*/
Pint	*coli;		/* OUT edge colour index	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( *pedi < 1) {
	*errind = ERR100;

    } else if ( *pedi > dt->out_dt.num_predefined_edge_indices) {
	*errind = ERR102;

    } else {
	Pedge_bundle_plus *eb =
            &dt->out_dt.default_edge_bundle_table[*pedi - 1];

	*errind = 0;
	*edflag = (Pint)eb->flag;
	*edtype = eb->type;
	*ewidth = eb->width;
	if ( eb->colr.type != PINDCT) {
            *errind = ERR134;
        } else {
	    *coli = eb->colr.val.ind;
        }
    }
}


/* INQUIRE PREDEFINED EDGE REPRESENTATION PLUS */
void
pqperp_( wtype, pedi, ccsbsz, errind, edflag, edtype, ewidth, ctype, coli, ol,
colr)
Pint	*wtype;		/* workstation type	*/
Pint	*pedi;		/* predefined edge index	*/
Pint	*ccsbsz;	/* colour component buffer size	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*edflag;	/* OUT edge flag	*/
Pint	*edtype;	/* OUT edgetype	*/
Pfloat	*ewidth;	/* OUT edgewidth scale factor	*/
Pint	*ctype;		/* OUT colour type	*/
Pint	*coli;		/* OUT colour index	*/
Pint	*ol;		/* OUT number of colour components	*/
Pfloat	*colr;		/* OUT colour specification	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( *pedi < 1) {
	*errind = ERR100;

    } else if ( *pedi > dt->out_dt.num_predefined_edge_indices) {
	*errind = ERR102;

    } else {
	Pedge_bundle_plus *eb =
            &dt->out_dt.default_edge_bundle_table[*pedi - 1];

	*errind = 0;
	*edflag = (Pint)eb->flag;
	*edtype = eb->type;
	*ewidth = eb->width;
        GCOLR_DATA_RE_TRANSLATE_PLUS( eb->colr, *ccsbsz, *errind, *ctype, *coli,
            *ol, colr);
    }
}


/* INQUIRE PATTERN FACILITIES */
void
pqpaf_( wtype, errind, nppai)
Pint	*wtype;		/* workstation type	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nppai;		/* OUT number of predefined pattern indices	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else {
	*errind = 0;
	*nppai = dt->out_dt.num_pattern_types;
    }
}


/* INQUIRE PREDEFINED PATTERN REPRESENTATION */
void
pqppar_( wtype, ppai, dimx, dimy, errind, dx, dy, colia)
Pint	*wtype;		/* workstation type	*/
Pint	*ppai;		/* predefined pattern index	*/
Pint	*dimx;		/* MAX pattern array dimensions X	*/
Pint	*dimy;		/* MAX pattern array dimensions Y	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*dx;		/* OUT pattern colour index array dimensions X	*/
Pint	*dy;		/* OUT pattern colour index array dimensions Y	*/
Pint	*colia;		/* OUT pattern colour index array	*/
{
    /* Error 51 and 62 not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if (!(wst = phg_fb_wst_exists( *wtype))) {
        *errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
        || dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
        *errind = ERR59;

    } else if ( *ppai < 1) {
        *errind = ERR112;

    } else if ( *ppai > dt->out_dt.num_pattern_types) {
        *errind = ERR102;

    } else {
        Ppat_rep_plus *pb = &dt->out_dt.pattern_types[*ppai - 1];

        *dx = pb->dims.size_x;
        *dy = pb->dims.size_y;
        if ( *dimx < *dx || *dimy < *dy) { 
            *errind = ERR2001;

        } else {
            *errind = 0;
            COLR_IND_DATA_RE_TRANSLATE( pb, *dx, *dy, *dimx, *dimy, colia);
        }
    }
}


/* INQUIRE PREDEFINED PATTERN REPRESENTATION PLUS */
void
pqppap_( wtype, ppai, ccsbsz, dimx, dimy, errind, dx, dy, ctype, colia, ol,
colra)
Pint	*wtype;		/* workstation type	*/
Pint	*ppai;		/* predefined pattern index	*/
Pint	*ccsbsz;	/* colour component buffer size	*/
Pint	*dimx;		/* MAX pattern array dimensions X	*/
Pint	*dimy;		/* MAX pattern array dimensions Y	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*dx;		/* pattern array dimensions X	*/
Pint	*dy;		/* pattern array dimensions Y	*/
Pint	*ctype;		/* OUT colour type	*/
Pint	*colia;		/* OUT colour index	array */
Pint	*ol;		/* OUT number of colour components	*/
Pfloat	*colra;		/* OUT colour specification	array */
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
        *errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
        || dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
        *errind = ERR59;

    } else if ( *ppai < 1) {
        *errind = ERR112;

    } else if ( *ppai > dt->out_dt.num_pattern_types) {
        *errind = ERR102;

    } else {
        Ppat_rep_plus	*pb = &dt->out_dt.pattern_types[*ppai - 1];

        *dx = pb->dims.size_x;
        *dy = pb->dims.size_y;
        *ctype = pb->type;
        if ( *dimx < *dx || *dimy < *dy) {
            *errind = ERR2001;

        } else {
            *errind = 0;
            switch ( *ctype) {
            case PINDCT:
                COLR_IND_DATA_RE_TRANSLATE( pb, *dx, *dy, *dimx, *dimy, colia);
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                *ol = 3;
                if ( *ccsbsz < *ol) {
                    *errind = ERR2001;

                } else {
                    COLR_VAL_DATA_RE_TRANSLATE( pb, *dx, *dy, *dimx, *dimy,
                        colra);
                }
                break;
            }
        }
    }
}


/* INQUIRE COLOUR MODEL FACILITIES */
void
pqcmdf_( wtype, n, errind, ol, cmod, dfcmod)
Pint	*wtype;		/* workstation type	*/
Pint	*n;		/* element of list of colour models	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of colour models	*/
Pint	*cmod;		/* OUT Nth colour models	*/
Pint	*dfcmod;	/* OUT default colour model	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else {
	*errind = 0;
	*ol = dt->out_dt.num_colour_models;
	if ( *ol > 0) {
	    if ( *n < 0 || *n > *ol){
		*errind = ERR2002;
                return;
	    } else if ( *n > 0) {
		*cmod = dt->out_dt.colour_models[*n - 1];
            }
	}
	*dfcmod = dt->out_dt.default_colour_model;
    }
}


/* INQUIRE DIRECT COLOUR MODEL FACILITIES */
void
pqdcmf_( wtype, n, errind, ndcm, dcm)
Pint	*wtype;		/* workstation type	*/
Pint	*n;		/* list element of direct colour model requested */
Pint	*errind;	/* OUT error indicator	*/
Pint	*ndcm;		/* OUT number of direct colour model	*/
Pint	*dcm;		/* OUT Nth element of list of direct colour model */
{
    /* TODO: This function is a duplicate of pinqcolourmodelfacil */
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else {
	*errind = 0;
	*ndcm = dt->out_dt.num_colour_models;
	if ( *ndcm > 0) {
	    if ( *n < 0 || *n > *ndcm) {
		*errind = ERR2002;
	    } else if ( *n > 0) {
	        *dcm = dt->out_dt.colour_models[*n -1];
            }
	}
    }
}


/* INQUIRE COLOUR FACILITIES */
void
pqcf_( wtype, errind, ncoli, cola, npci, cc)
Pint	*wtype;		/* workstation type	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ncoli;		/* OUT number of colour indices	*/
Pint	*cola;		/* OUT colours available	*/
Pint	*npci;		/* OUT number of predefined colour indices	*/
Pfloat	*cc;		/* OUT primary colour-chromaticity coefficients	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst 			*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( wst->bound_status != WST_BOUND) {
	*errind = ERR51;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else {
	*errind = 0;
	*ncoli = dt->out_dt.num_colours;
	*cola = (Pint)dt->out_dt.colour_availability;
	*npci = dt->out_dt.num_predefined_colours;

    /* TODO: Figure out how to convert the PEX info to PHIGS info */
	cc[0] = dt->out_dt.chroma_info.xr;
	cc[1] = dt->out_dt.chroma_info.yr;
	cc[2] = dt->out_dt.chroma_info.Yr;
	cc[3] = dt->out_dt.chroma_info.xg;
	cc[4] = dt->out_dt.chroma_info.yg;
	cc[5] = dt->out_dt.chroma_info.Yg;
	cc[6] = dt->out_dt.chroma_info.xb;
	cc[7] = dt->out_dt.chroma_info.yb;
	cc[8] = dt->out_dt.chroma_info.Yb;
    }
}


/* INQUIRE PREDEFINED COLOUR REPRESENTATION */
void
pqpcr_( wtype, pci, ccsbsz, errind, ol, cspec)
Pint	*wtype; 	/* workstation type	*/
Pint	*pci;		/* predefined colour index	*/
Pint	*ccsbsz;	/* colour component specifications buffer size	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of colour component	*/
Pfloat	*cspec;		/* OUT colour specifications	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt	*dt;
    Wst			*wst;
    Pint                colr_type = PRGB;		/* only RGB suppoted */
    static Pint         colr_table[5] = {0, 3, 3, 3, 3}; /* colour table */
    Pfloat              *pt;
    int                 i;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( *pci < 0) {
	*errind = ERR113;

    } else if ( *pci >= dt->out_dt.num_predefined_colours) {
	*errind = ERR102;
 
    } else if ( *ccsbsz < colr_table[colr_type]) {
        *errind = ERR2001;

    } else {
	*errind = 0;
        *ol = colr_table[colr_type];
	pt = (Pfloat *)(&dt->out_dt.default_colour_table[*pci]);
        for ( i = 0; i < colr_table[colr_type]; i++) {
            cspec[i] = pt[i];
        }
    }
}


/* INQUIRE LIST element OF AVAILABLE GENERALIZED DRAWING PRIMITIVES 3 */
void
pqegd3_( wtype, n, errind, ngdp, gdpl)
Pint	*wtype;		/* workstation type	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ngdp;		/* OUT number of 3D GDPs	*/
Pint	*gdpl;		/* OUT Nth element of list of 3D GDP identifiers */
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else {
	*errind = 0;
	*ngdp = dt->out_dt.num_gdp3;
	if ( *ngdp > 0) {
	    if ( *n < 0 || *n > *ngdp) {
		*errind = ERR2002;
            } else if ( *n > 0) {
		*gdpl = dt->out_dt.gdp3_ids[*n - 1];
            }
	}
    }
}


/* INQUIRE LIST element OF AVAILABLE GENERALIZED DRAWING PRIMITIVES */
void
pqegdp_( wtype, n, errind, ngdp, gdpl)
Pint	*wtype;		/* workstation type	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ngdp;		/* OUT number of GDPs	*/
Pint	*gdpl;		/* OUT Nth element of list of GDP identifiers	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else {
	*errind = 0;
	*ngdp = dt->out_dt.num_gdp;
	if ( *ngdp > 0) {
	    if ( *n < 0 || *n > *ngdp) {
		*errind = ERR2002;
            } else if ( *n > 0) {
		*gdpl = dt->out_dt.gdp_ids[*n - 1];
            }
	}
    }
}

static int
find_gdp_id( id,  n, list)
Pint    id;
Pint    n;
Pint    *list;
{
    register int i;
    for (i = 0 ; i < n ; i++)
        if (list[i] == id) return i;    /* 0 is a valid return value */
    return -1;
}


/* INQUIRE GENERALIZED DRAWING PRIMITIVE 3 */
void
pqgdp3_( wtype, gdp, errind, nbnd, bndl)
Pint	*wtype;		/* workstation type	*/
Pint	*gdp;		/* 3D GDP identifier	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nbnd;		/* OUT number of sets of attributes used	*/
Pint	*bndl;		/* OUT list of sets of attributes used	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt                *dt;
    Wst                         *wst;
    int                         ind;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
        *errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
        || dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
        *errind = ERR59;

    } else if (( ind = find_gdp_id( *gdp, dt->out_dt.num_gdp3,
        dt->out_dt.gdp3_ids)) < 0) {
        *errind = ERR64;

    } else {
        Wst_gdp_attrs	*gdp_attrs = &dt->out_dt.gdp3_attrs[ind];

        *errind = 0;
        *nbnd = MIN( 5, gdp_attrs->number);
        if ( *nbnd  > 0) {
            bcopy( (char *)gdp_attrs->attrs, (char *)bndl,
                *nbnd * sizeof(Pint));
        }
    }
}


/* INQUIRE GENERALIZED DRAWING PRIMITIVE */
void
pqgdp_( wtype, gdp, errind, nbnd, bndl)
Pint	*wtype;		/* workstation type	*/
Pint	*gdp;		/* GDP identifier	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nbnd;		/* OUT number of sets of attributes used	*/
Pint	*bndl;		/* OUT list of sets of attributes used	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt                *dt;
    Wst                         *wst;
    int                         ind;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
        *errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
        || dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
        *errind = ERR59;

    } else if (( ind = find_gdp_id( *gdp, dt->out_dt.num_gdp,
        dt->out_dt.gdp_ids)) < 0) {
        *errind = ERR64;

    } else {
        Wst_gdp_attrs	*gdp_attrs = &dt->out_dt.gdp_attrs[ind];

        *errind = 0;
        *nbnd = MIN( 5, gdp_attrs->number);
        if ( *nbnd  > 0) {
            bcopy( (char *)gdp_attrs->attrs, (char *)bndl,
                *nbnd * sizeof(Pint));
        }
    }
}



/* INQUIRE LIST element OF AVAILABLE GENERALIZED STRUCTURE ELEMENTS */
void
pqegse_( wtype, n, errind, ngse, gsel)
Pint	*wtype;		/* workstation type	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ngse;		/* OUT number of GSEs	*/
Pint	*gsel;		/* OUT Nth element of list of GSE identifers	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!(wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else {
	*errind = 0;
	*ngse = dt->out_dt.num_gse;
	if ( *ngse > 0) {
	    if ( *n < 0 || *n > *ngse) {
		*errind = ERR2002;
            } else if ( *n > 0) {
		*gsel = dt->out_dt.gse_ids[*n - 1];
            }
	}
    }
}


/* INQUIRE WORKSTATION STATE TABLE LENGTHS */
void
pqwksl_( wtype, errind, mplbte, mpmbte, mtxbte, minbte, medbte, mpai, mcoli,
vwtbi)
Pint	*wtype;		/* workstation type	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mplbte;	/* OUT MAX number of polyline bundle table entries */
Pint	*mpmbte;	/* OUT MAX number of polymarker bundle table entries */
Pint	*mtxbte;	/* OUT MAX number of text bundle table entries	*/
Pint	*minbte;	/* OUT MAX number of interior bundle table entries */
Pint	*medbte;	/* OUT MAX number of edge bundle table entries	*/
Pint	*mpai;		/* OUT MAX number of pattern indices	*/
Pint	*mcoli;		/* OUT MAX number of colour indices	*/
Pint	*vwtbi;		/* OUT MAX number of view table indices	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;
    
    } else {
	*errind = 0;
	*mplbte = dt->out_dt.num_polyline_bundle_entries;
	*mpmbte = dt->out_dt.num_polymarker_bundle_entries;
	*mtxbte = dt->out_dt.num_text_bundle_entries;
	*minbte = dt->out_dt.num_interior_bundle_entries;
	*medbte = dt->out_dt.num_edge_bundle_entries;
	*mpai = dt->out_dt.num_pattern_table_entries;
	*mcoli = dt->out_dt.num_colour_indices;
	*vwtbi = dt->num_view_indices;
    }
}


/* INQUIRE WORKSTATION TABLE LENGTHS PLUS */
void
pqwslp_( wtype, errind, mdcte, mlste, mcmte)
Pint	*wtype;		/* workstation type	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mdcte;		/* OUT MAX number of depth cue table entries	*/
Pint	*mlste;		/* OUT MAX number of light source table entries	*/
Pint	*mcmte;		/* OUT MAX number of colourmapping table entries */
{
    /* Error 51 and Error 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else {
	*errind = 0;
	*mdcte = dt->out_dt.num_depth_cue_bundle_entries;
	*mlste = dt->out_dt.num_light_src_bundle_entries;
        /* TODO ... */
	*mcmte = 1;
    }
}


/* INQUIRE DYNAMICS OF STRUCTURES */
void
pqdstr_( wtype, errind, strcon, post, unpost, delete, refmod)
Pint	*wtype;		/* workstation type	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*strcon;	/* OUT structure content modifications	*/
Pint	*post;		/* OUT posting a structure	*/
Pint	*unpost;	/* OUT unposting a structure	*/
Pint	*delete;	/* OUT deleting a structure	*/
Pint	*refmod;	/* OUT reference modification	*/
{
    /* Error 51 and 62 are not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else {
	*errind = 0;
	*strcon = (Pint)dt->out_dt.struct_content_mod;
	*post = (Pint)dt->out_dt.post;
	*unpost = (Pint)dt->out_dt.unpost;
	*delete = (Pint)dt->out_dt.struct_delete;
	*refmod = (Pint)dt->out_dt.ref_mod;
    }
}


/* INQUIRE NUMBER OF AVAILABLE LOGICAL INPUT DEVICES */
void
pqli_( wtype, errind, nlcd, nskd, nvld, nchd, npkd, nstd)
Pint 	*wtype;		/* workstation type	*/
Pint   	*errind;	/* OUT error indicator	*/
Pint   	*nlcd;		/* OUT number of locator devices	*/
Pint   	*nskd;		/* OUT number of stroke devices	*/
Pint   	*nvld;		/* OUT number of valuator devices	*/
Pint   	*nchd;		/* OUT number of choice devices	*/
Pint   	*npkd;		/* OUT number of pick devices	*/
Pint   	*nstd;		/* OUT number of string devices	*/
{
    Wst_phigs_dt		*dt;
    Wst 			*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( wst->bound_status != WST_BOUND) {
	*errind = ERR51;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == PINPUT
	|| dt->ws_category == POUTIN) ) {
	*errind = ERR61;

    } else {
	*errind = 0;
	*nlcd = dt->in_dt.num_devs.loc;
	*nskd = dt->in_dt.num_devs.stroke;
	*nvld = dt->in_dt.num_devs.val;
	*nchd = dt->in_dt.num_devs.choice;
	*npkd = dt->in_dt.num_devs.pick;
	*nstd = dt->in_dt.num_devs.string;
    }
}


/* INQUIRE DEFAULT LOCATOR DEVICE DATA 3 */
void
pqdlc3_( wtype, devno, n, mldr, errind, dpx, dpy, dpz, ol, pet, evol, ldr,
datrec)
Pint	*wtype;		/* workstation type	*/
Pint	*devno;		/* logical input device number	*/
Pint	*n;		/* list element requested	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pfloat	*dpx;		/* OUT default initial locator position X	*/
Pfloat	*dpy;		/* OUT default initial locator position Y	*/
Pfloat	*dpz;		/* OUT default initial locator position Z	*/
Pint	*ol;		/* OUT number of available prompt and echo types */
Pint	*pet;		/* OUT Nth element of list of prompt and echo types */
Plimit3	*evol;		/* OUT default echo volume	*/
Pint	*ldr;		/* OUT number of array elements used in data record */
char	*datrec;	/* OUT data record	*/
{
    /* Error 51 is not generated by this function. */

    		Wst_phigs_dt	*dt;
    register	Wst_defloc	*defloc;
    Wst				*wst;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries	*/

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == PINPUT
	|| dt->ws_category == POUTIN) ) {
	*errind = ERR61;

    } else if ( *devno < 1 || *devno > dt->in_dt.num_devs.loc) {
	*errind = ERR250;

    } else {
	*errind = 0;
	defloc = &(dt->in_dt.locators[*devno - 1]);
	*dpx = defloc->position.x;
	*dpy = defloc->position.y;
	*dpz = defloc->position.z;
	*evol = defloc->e_volume;
	*ol = defloc->num_pets;
	if ( *ol > 0) {
	    if ( *n < 0 || *n > *ol) {
		*errind = ERR2002;
                return;
            } else if ( *n > 0) {
		*pet = defloc->pets[*n - 1];
            }
        } 
        fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr, errind, ldr, datrec);
    }
}


/* INQUIRE DEFAULT LOCATOR DEVICE DATA */
void
pqdlc_( wtype, devno, n, mldr, errind, dpx, dpy, ol, pet, earea, ldr, datrec)
Pint	*wtype;		/* workstation type	*/
Pint	*devno;		/* logical input device number	*/
Pint	*n;		/* list element requested	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pfloat	*dpx;		/* OUT default initial locator position X	*/
Pfloat	*dpy;		/* OUT default initial locator position Y	*/
Pint	*ol;		/* OUT number of available prompt and echo types */
Pint	*pet;		/* OUT Nth element of list of prompt and echo types */
Plimit	*earea;		/* OUT default echo area	*/
Pint	*ldr;		/* OUT number of array elements used in data record */
char	*datrec;	/* OUT data record	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    register Wst_defloc		*defloc;
    Wst	   			*wst;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries	*/

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == PINPUT
	|| dt->ws_category == POUTIN) ) {
	*errind = ERR61;

    } else if ( *devno < 1 || *devno > dt->in_dt.num_devs.loc) {
	*errind = ERR250;

    } else {
	*errind = 0;
	defloc = &(dt->in_dt.locators[*devno - 1]);
	*dpx = defloc->position.x;
	*dpy = defloc->position.y;
	CB_ECHO_VOLUME_TO_AREA( defloc->e_volume, *earea);
	*ol = defloc->num_pets;
	if ( *ol > 0) {
	    if ( *n < 0 || *n > defloc->num_pets) {
		*errind = ERR2002;
                return;
            } else if ( *n > 0) {
		*pet = defloc->pets[*n - 1];
            }
        }
        fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr, errind, ldr, datrec);
    }
}


/* INQUIRE DEFAULT STROKE DEVICE DATA 3 */
void
pqdsk3_( wtype, devno, n, mldr, errind, mbuff, ol, pet, evol, ldr, datrec)
Pint	*wtype;		/* workstation type	*/
Pint	*devno;		/* logical input device number	*/
Pint	*n;		/* list element requested	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mbuff;		/* OUT available input buffer size	*/
Pint	*ol;		/* OUT number of available prompt and echo types */
Pint	*pet;		/* OUT Nth element of list of prompt and echo types */
Plimit3	*evol;		/* OUT default echo volume	*/
Pint	*ldr;		/* OUT number of array elements used in data record */
char	*datrec;	/* OUT data record	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    register Wst_defstroke	*defstroke;
    Wst				*wst;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	ia[2];		/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	ra[4];		/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries	*/

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == PINPUT
	|| dt->ws_category == POUTIN) ) {
	*errind = ERR61;

    } else if ( *devno < 1 || *devno > dt->in_dt.num_devs.stroke) {
	*errind = ERR250;

    } else {
	*errind = 0;
	defstroke = &(dt->in_dt.strokes[*devno - 1]);
	*mbuff = defstroke->max_bufsize;
	*evol = defstroke->e_volume;
	*ol = defstroke->num_pets;
	if ( *ol > 0) {
	    if ( *n < 0 || *n > *ol) {
		*errind = ERR2002;
                return;
            } else if ( *n > 0) {
		*pet = defstroke->pets[*n - 1];
            }
        }
        il = 2;
        rl = 4;
        ia[0] = defstroke->record.buffer_size;
        ia[1] = defstroke->record.init_pos;
        ra[0] = defstroke->record.x_interval;
        ra[1] = defstroke->record.y_interval;
        ra[2] = defstroke->record.z_interval;
        ra[3] = defstroke->record.time_interval;
        fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr, errind, ldr, datrec);
    }
}


/* INQUIRE DEFAULT STROKE DEVICE DATA */
void
pqdsk_( wtype, devno, n, mldr, errind, mbuff, ol, pet, earea, ldr, datrec)
Pint	*wtype;		/* workstation type	*/
Pint	*devno;		/* logical input device number	*/
Pint	*n;		/* list element requested	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mbuff;		/* OUT available input buffer size	*/
Pint	*ol;		/* OUT number of available prompt and echo types */
Pint	*pet;		/* OUT Nth element of list of prompt and echo types */
Plimit	*earea;		/* OUT default echo area	*/
Pint	*ldr;		/* OUT number of array elements used in data record */
char	*datrec;	/* OUT data record	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    register Wst_defstroke	*defstroke;
    Wst				*wst;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	ia[2];		/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	ra[3];		/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries	*/

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == PINPUT
	|| dt->ws_category == POUTIN) ) {
	*errind = ERR61;

    } else if ( *devno < 1 || *devno > dt->in_dt.num_devs.stroke) {
	*errind = ERR250;

    } else {
	*errind = 0;
	defstroke = &(dt->in_dt.strokes[*devno - 1]);
	*mbuff = defstroke->max_bufsize;
	CB_ECHO_VOLUME_TO_AREA( defstroke->e_volume, *earea);
	*ol = defstroke->num_pets;
	if ( *ol > 0) {
	    if ( *n < 0 || *n > *ol) {
		*errind = ERR2002;
                return;
            } else if ( *n > 0) {
		*pet = defstroke->pets[*n - 1];
            }
        }
        il = 2;
        rl = 3;
        ia[0] = defstroke->record.buffer_size;
        ia[1] = defstroke->record.init_pos;
	ra[0] = defstroke->record.x_interval;
	ra[1] = defstroke->record.y_interval;
	ra[2] = defstroke->record.time_interval;
	fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr, errind, ldr, datrec);
    }
}


/* INQUIRE DEFAULT VALUATOR DEVICE DATA 3 */
void
pqdvl3_( wtype, devno, n, mldr, errind, dval, ol, pet, evol, ldr, datrec)
Pint	*wtype;		/* workstation type	*/
Pint	*devno;		/* logical input device number	*/
Pint	*n;		/* list element requested	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pfloat	*dval;		/* OUT default initial value	*/
Pint	*ol;		/* OUT number of available prompt and echo types */
Pint	*pet;		/* OUT Nth element of list of prompt and echo types */
Plimit3	*evol;		/* OUT default echo volume	*/
Pint	*ldr;		/* OUT number of array elements used in data record */
char	*datrec;	/* OUT data record	*/
{
    /* Error 51 is not generated by this function. */

    		Wst_phigs_dt	*dt;
    register	Wst_defval	*defval;
    Wst				*wst;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	ra[2];		/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries	*/

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == PINPUT
	|| dt->ws_category == POUTIN) ) {
	*errind = ERR61;

    } else if ( *devno < 1 || *devno > dt->in_dt.num_devs.val) {
	*errind = ERR250;

    } else {
	*errind = 0;
	defval = &(dt->in_dt.valuators[*devno - 1]);
	*dval = defval->value;
	*evol = defval->e_volume;
	*ol = defval->num_pets;
	if ( *ol > 0) {
	    if ( *n < 0 || *n > *ol) {
		*errind = ERR2002;
                return;
            } else if ( *n > 0) {
		*pet = defval->pets[*n - 1];
            }
        }
	rl = 2;
	ra[0] = defval->record.low;
	ra[1] = defval->record.high;
	fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr, errind, ldr, datrec);
    }
}


/* INQUIRE DEFAULT VALUATOR DEVICE DATA */
void
pqdvl_( wtype, devno, n, mldr, errind, dval, ol, pet, earea, ldr, datrec)
Pint	*wtype;		/* workstation type	*/
Pint	*devno;		/* logical input device number	*/
Pint	*n;		/* list element requested	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pfloat	*dval;		/* OUT default initial value	*/
Pint	*ol;		/* OUT number of available prompt and echo types */
Pint	*pet;		/* OUT Nth element of list of prompt and echo types */
Plimit	*earea;		/* OUT default echo area	*/
Pint	*ldr;		/* OUT number of array elements used in data record */
char	*datrec;	/* OUT data record	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    register Wst_defval		*defval;
    Wst				*wst;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	ra[2];		/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries	*/

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == PINPUT
	|| dt->ws_category == POUTIN) ) {
	*errind = ERR61;

    } else if ( *devno < 1 || *devno > dt->in_dt.num_devs.val) {
	*errind = ERR250;

    } else {
	*errind = 0;
	defval = &(dt->in_dt.valuators[*devno - 1]);
	*dval = defval->value;
	CB_ECHO_VOLUME_TO_AREA( defval->e_volume, *earea);
	*ol = defval->num_pets;
	if ( *ol > 0) {
	    if ( *n < 0 || *n > *ol) {
		*errind = ERR2002;
                return;
            } else if ( *n > 0) {
		*pet = defval->pets[*n - 1];
            }
        }
	rl = 2;
	ra[0] = defval->record.low;
	ra[1] = defval->record.high;
	fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr, errind, ldr, datrec);
    }
}


/* INQUIRE DEFAULT CHOICE DEVICE DATA 3 */
void
pqdch3_( wtype, devno, n, mldr, errind, malt, ol, pet, evol, ldr, datrec)
Pint	*wtype;		/* workstation type	*/
Pint	*devno;		/* logical input device number	*/
Pint	*n;		/* list element requested	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*malt;		/* OUT MAX number of choice alternatives	*/
Pint	*ol;		/* OUT number of available prompt and echo types */
Pint	*pet;		/* OUT Nth element of list of prompt and echo types */
Plimit3	*evol;		/* OUT default echo volume	*/
Pint	*ldr;		/* OUT number of array elements used in data record */
char	*datrec;	/* OUT data record	*/
{
    /* Error 51 is not generated by this function. */

    		Wst_phigs_dt	*dt;
    register	Wst_defchoice	*defchoice;
    Wst				*wst;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries	*/

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == PINPUT
	|| dt->ws_category == POUTIN) ) {
	*errind = ERR61;

    } else if ( *devno < 1 || *devno > dt->in_dt.num_devs.choice) {
	*errind = ERR250;

    } else {
	*errind = 0;
	defchoice = &(dt->in_dt.choices[*devno - 1]);
	*malt = defchoice->choices;
	*evol = defchoice->e_volume;
	*ol = defchoice->num_pets;
	if ( *ol > 0) {
	    if ( *n < 0 || *n > *ol) {
		*errind = ERR2002;
                return;
            } else if ( *n > 0) {
		*pet = defchoice->pets[*n - 1];
            }
        }
	fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr, errind, ldr, datrec);
    }
}


/* INQUIRE DEFAULT CHOICE DEVICE DATA */
void
pqdch_( wtype, devno, n, mldr, errind, malt, ol, pet, earea, ldr, datrec)
Pint	*wtype;		/* workstation type	*/
Pint	*devno;		/* logical input device number	*/
Pint	*n;		/* list element requested	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*malt;		/* OUT MAX number of choice alternatives	*/
Pint	*ol;		/* OUT number of available prompt and echo types */
Pint	*pet;		/* OUT Nth element of list of prompt and echo types */
Plimit	*earea;		/* OUT default echo area	*/
Pint	*ldr;		/* OUT number of array elements used in data record */
char	*datrec;	/* OUT data record	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    register Wst_defchoice	*defchoice;
    Wst				*wst;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries	*/

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == PINPUT
	|| dt->ws_category == POUTIN) ) {
	*errind = ERR61;

    } else if ( *devno < 1 || *devno > dt->in_dt.num_devs.choice) {
	*errind = ERR250;

    } else {
	*errind = 0;
	defchoice = &(dt->in_dt.choices[*devno - 1]);
	*malt = defchoice->choices;
	CB_ECHO_VOLUME_TO_AREA( defchoice->e_volume, *earea);
	*ol = defchoice->num_pets;
	if ( *ol > 0) {
	    if ( *n < 0 || *n > *ol) {
		*errind = ERR2002;
                return;
            } else if ( *n > 0) {
		*pet = defchoice->pets[*n - 1];
            }
        }
	fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr, errind, ldr, datrec);
    }
}


/* INQUIRE DEFAULT PICK DIVICE DATA 3 */
void
pqdpk3_( wtype, devno, n, mldr, errind, ol, pet, evol, ldr, datrec)
Pint	*wtype;		/* workstation type	*/
Pint	*devno;		/* logical input device number	*/
Pint	*n;		/* list element requested	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of available prompt and echo types */
Pint	*pet;		/* OUT Nth element of list of prompt and echo types */
Plimit3	*evol;		/* OUT default echo volume	*/
Pint	*ldr;		/* OUT number of array elements used in data record */
char	*datrec;	/* OUT data record	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    register Wst_defpick	*defpick;
    Wst				*wst;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries	*/

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == PINPUT
	|| dt->ws_category == POUTIN) ) {
	*errind = ERR61;

    } else if ( *devno < 1 || *devno > dt->in_dt.num_devs.pick) {
	*errind = ERR250;

    } else {
	*errind = 0;
	defpick = &(dt->in_dt.picks[*devno - 1]);
	*evol = defpick->e_volume;
	*ol = defpick->num_pets;
	if ( *ol > 0) {
	    if ( *n < 0 || *n > *ol) {
		*errind = ERR2002;
                return;
            } else if ( *n > 0) {
		*pet = defpick->pets[*n - 1];
            }
        }
	fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr, errind, ldr, datrec);
    }
}


/* INQUIRE DEFAULT PICK DIVICE DATA */
void
pqdpk_( wtype, devno, n, mldr, errind, ol, pet, earea, ldr, datrec)
Pint	*wtype;		/* workstation type	*/
Pint	*devno;		/* logical input device number	*/
Pint	*n;		/* list element requested	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of available prompt and echo types */
Pint	*pet;		/* OUT Nth element of list of prompt and echo types */
Plimit	*earea;		/* OUT default echo area	*/
Pint	*ldr;		/* OUT number of array elements used in data record */
char	*datrec;	/* OUT data record	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    register Wst_defpick	*defpick;
    Wst				*wst;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries	*/

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == PINPUT
	|| dt->ws_category == POUTIN) ) {
	*errind = ERR61;

    } else if ( *devno < 1 || *devno > dt->in_dt.num_devs.pick) {
	*errind = ERR250;

    } else {
	*errind = 0;
	defpick = &(dt->in_dt.picks[*devno - 1]);
	CB_ECHO_VOLUME_TO_AREA( defpick->e_volume, *earea);
	*ol = defpick->num_pets;
	if ( *ol > 0) {
	    if ( *n < 0 || *n > *ol) {
		*errind = ERR2002;
                return;
            } else if ( *n > 0) {
		*pet = defpick->pets[*n - 1];
            }
        }
	fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr, errind, ldr, datrec);
    }
}


/* INQUIRE DEFAULT STRING DIVICE DATA 3 */
void
pqdst3_( wtype, devno, n, mldr, errind, mbuff, ol, pet, evol, ldr, datrec)
Pint	*wtype;		/* workstation type	*/
Pint	*devno;		/* logical input device number	*/
Pint	*n;		/* list element requested	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mbuff; 	/* OUT available string buffer size	*/
Pint	*ol;		/* OUT number of available prompt and echo types */
Pint	*pet;		/* OUT Nth element of list of prompt and echo types */
Plimit3	*evol;		/* OUT default echo volume	*/
Pint	*ldr;		/* OUT number of array elements used in data record */
char	*datrec;	/* OUT data record	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    register Wst_defstring	*defstring;
    Wst				*wst;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	ia[2];		/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries	*/

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == PINPUT
	|| dt->ws_category == POUTIN) ) {
	*errind = ERR61;

    } else if ( *devno < 1 || *devno > dt->in_dt.num_devs.string) {
	*errind = ERR250;

    } else {
	*errind = 0;
	defstring = &(dt->in_dt.strings[*devno - 1]);
	*mbuff = defstring->max_bufsize;
	*evol = defstring->e_volume;
	*ol = defstring->num_pets;
	if ( *ol > 0) {
	    if ( *n < 0 || *n > *ol) {
		*errind = ERR2002;
                return;
            } else if ( *n > 0) {
		*pet = defstring->pets[*n - 1];
            }
        }
	il = 2;
	ia[0] = defstring->record.buffer_size;
	ia[1] = defstring->record.init_pos;
	fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr, errind, ldr, datrec);
    }
}


/* INQUIRE DEFAULT STRING DIVICE DATA */
void
pqdst_( wtype, devno, n, mldr, errind, mbuff, ol, pet, earea, ldr, datrec)
Pint	*wtype;		/* workstation type	*/
Pint	*devno;		/* logical input device number	*/
Pint	*n;		/* list element requested	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mbuff;		/* OUT available string buffer size	*/
Pint	*ol;		/* OUT number of available prompt and echo types */
Pint	*pet;		/* OUT Nth element of list of prompt and echo types */
Plimit	*earea;		/* OUT default echo area	*/
Pint	*ldr;		/* OUT number of array elements used in data record */
char	*datrec;	/* OUT data record	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    register Wst_defstring	*defstring;
    Wst				*wst;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	ia[2];		/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries	*/

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == PINPUT
	|| dt->ws_category == POUTIN) ) {
	*errind = ERR61;

    } else if ( *devno < 1 || *devno > dt->in_dt.num_devs.string) {
	*errind = ERR250;

    } else {
	*errind = 0;
	defstring = &dt->in_dt.strings[*devno - 1];
	*mbuff = defstring->max_bufsize;
	CB_ECHO_VOLUME_TO_AREA( defstring->e_volume, *earea);
	*ol = defstring->num_pets;
	if ( *ol > 0) {
	    if ( *n < 0 || *n > *ol) {
		*errind = ERR2002;
                return;
            } else if ( *n > 0) {
		*pet = defstring->pets[*n - 1];
            }
        }
	il = 2;
	ia[0] = defstring->record.buffer_size;
	ia[1] = defstring->record.init_pos;
	fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr, errind, ldr, datrec);
    }
} 


/* INQUIRE CURVE AND SURFACE FACILITIES */
void
pqcsf_( wtype, nrcact, nrsact, nrtact, nrpsct, errind, mnubsc, mtrimc, ncact,
cact, ncast, cast, ntact, tact, npsct, psct)
Pint	*wtype;	   /* workstation type */
Pint	*nrcact;   /* list element requested of curve approx types	*/
Pint	*nrsact;   /* list element requested of surface approx types	*/
Pint	*nrtact;   /* list element requested of trim curve approx types	*/
Pint	*nrpsct;   /* list element requested of param surface characteristics */
Pint	*errind;   /* OUT error indidator	*/
Pint	*mnubsc;   /* OUT MAX nun-uniform b-sprine curve order supported */
Pint	*mtrimc;   /* OUT MAX trim curve order supported	*/
Pint	*ncact;	   /* OUT number of curve approx types	*/
Pint	*cact;     /* OUT NRCACTth element of curve approx types	*/
Pint	*ncast;	   /* OUT number of surface approx types	*/
Pint	*cast;	   /* OUT NRSACTth element of surface approx types	*/
Pint	*ntact;	   /* OUT number element of trim curve approx types	*/
Pint	*tact;	   /* OUT NRTACTth element of trim curve approx types	*/
Pint	*npsct;	   /* OUT number of param surface characteristics	*/
Pint	*psct;	   /* OUT NRPSCTth elemtnt of param surface characteristics */
{
    /* Error 51 and 62 are not generated by this function */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK(phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else {
	*errind = 0;
	*mnubsc = dt->out_dt.max_nurb_order;
	*mtrimc = dt->out_dt.max_trim_curve_order;
	*ncact = dt->out_dt.num_curve_approx_types;
	if ( *ncact > 0) {
	    if ( *nrcact < 0 || *nrcact > *ncact) {
		*errind = ERR2002;
                return;
            } else if ( *nrcact > 0) {
                *cact = dt->out_dt.curve_approx_types[*ncact - 1];
            }
        }
	*ncast = dt->out_dt.num_surface_approx_types;
        if ( *ncast > 0) {
	    if ( *nrsact < 0 || *nrsact > *ncast) {
		*errind = ERR2002;
                return;
            } else if ( *nrsact > 0) {
                *cast = dt->out_dt.surface_approx_types[*ncast - 1];
            }
        }
	*ntact = dt->out_dt.num_trim_curve_approx_types;
        if ( *ntact > 0) {
	    if ( *nrtact < 0 || *nrtact > *ntact) {
		*errind = ERR2002;
                return;
            } else if ( *nrtact > 0) {
                *tact = dt->out_dt.trim_curve_approx_types[*ntact - 1];
            }
        }
	*npsct = dt->out_dt.num_para_surf_characs;
        if ( *npsct > 0) {
	    if ( *nrpsct < 0 || *nrpsct > *npsct) {
		*errind = ERR2002;
            } else if ( *nrpsct > 0) {
                *psct = dt->out_dt.para_surf_characs[*npsct - 1];
            }
        }
    }
}


/* INQUIRE DEPTH CUE FACILITIES */
void
pqdcf_( wtype, errind, npdci)
Pint	*wtype;		/* workstation type	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*npdci;		/* OUT number of predefined depth cue indices	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else {
	*errind = 0;
	*npdci = dt->out_dt.num_predefined_depth_cue_indices;
    }
}


/* INQUIRE PREDEFINED DEPTH CUE REPRESENTATION */
void
pqpdcr_( wtype, pdci, ccsbsz, errind, dcmode, dcmin, dcmax, dcsfmi, dcsfmx,
ctype, coli, ol, colr)
Pint	*wtype;		/* workstation type	*/
Pint	*pdci;		/* predefined depth cue index	*/
Pint    *ccsbsz;    /* colour component buffer size */
Pint	*errind;	/* OUT error indicator	*/
Pint	*dcmode;	/* OUT depth cue mode	*/
Pfloat	*dcmin;		/* OUT depth cue reference plane MIN	*/
Pfloat	*dcmax;		/* OUT depth cue reference plane MAX	*/
Pfloat	*dcsfmi;	/* OUT depth cue scale factor MIN	*/
Pfloat	*dcsfmx;	/* OUT depth cue scale factor MAX	*/
Pint	*ctype;		/* OUT colour type	*/
Pint	*coli;		/* OUT colour index	*/
Pint	*ol;		/* OUT number of colour components	*/
Pfloat	*colr;		/* OUT colour specification	*/
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else if ( dt->ws_category == PMO) {
	*errind = ERR62;

    } else if ( *pdci < 0) {
	*errind = ERR119;

    } else if ( *pdci > dt->out_dt.num_predefined_depth_cue_indices) {
	*errind = ERR102;

    } else {
        Pdcue_bundle *dcb = &dt->out_dt.default_depth_cue_table[*pdci];

        *errind = 0;
        *dcmode = (Pint)dcb->mode;
        *dcmin = dcb->ref_planes[0];
        *dcmax = dcb->ref_planes[1];
        *dcsfmi = dcb->scaling[0];
        *dcsfmx = dcb->scaling[1];
        GCOLR_DATA_RE_TRANSLATE_PLUS( dcb->colr, *ccsbsz, *errind, *ctype,
            *coli, *ol, colr);
    }
}


/* INQUIRE LIGHT SOURCE FACILITIES */
void
pqlsf_( wtype, nrls, errind, nlstyp, lstyp, msnals, nplsi)
Pint	*wtype;		/* workstation type	*/
Pint	*nrls;		/* list element requested of light source types	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nlstyp;	/* OUT number of light source types	*/
Pint	*lstyp;		/* OUT NRLSth element of list of light source types */
Pint	*msnals;	/* OUT MAX number of simultaneously active non-ambient
                               light source table entries	*/
Pint	*nplsi;		/* OUT number of predefined light source indices */
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt		*dt;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
	*errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
	|| dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
	*errind = ERR59;

    } else {
	*errind = 0;
	*nlstyp = dt->out_dt.num_light_src_types;
	if ( *nlstyp > 0) {
	    if ( *nrls < 0 || *nrls > *nlstyp) {
                *errind = ERR2002;
                return;
            } else if ( *nrls > 0) {
	        *lstyp = dt->out_dt.light_src_types[*nrls - 1];
            }
	}
	*msnals = dt->out_dt.max_light_src;
	*nplsi = dt->out_dt.num_predefined_light_src_indices;
    }
}

/* INQUIRE PREDEFINED LIGHT SOURCE REPRESENTATION */
void
pqplsr_( wtype, plsi, mldr, errind, lstyp, ldr, datrec)
Pint    *wtype;     /* workstation type */
Pint    *plsi;      /* predefined ligth source index    */
Pint    *mldr;      /* dimension of data record array   */
Pint    *errind;    /* OUT error indicator  */
Pint    *lstyp;     /* OUT light source type    */
Pint    *ldr;       /* OUT length of data record    */
char    *datrec;    /* OUT data record  */
{
    /* Error 51 is not generated by this function. */

    Wst_phigs_dt        *dt;
    Wst             *wst;

    Pint    il = 0;     /* number of integer entries    */
    Pint    ia[2];      /* array containing integer entries */
    Pint    rl = 0;     /* number of real entries   */
    Pfloat  ra[NUM_COLR_COMP + 10]; /* array containing real entries */
    Pint    sl = 0;     /* number of character string entries   */
    Pint    *lstr = NULL;   /* length of character string entries   */
    char    *str = NULL;    /* character string entries */

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
        *errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
        || dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
        *errind = ERR59;

    } else if ( dt->ws_category == PMO) {
        *errind = ERR62;

    } else if ( *plsi < 1) {
        *errind = ERR129;

    } else if ( *plsi > dt->out_dt.num_predefined_light_src_indices) {
        *errind = ERR102;

    } else {
        Plight_src_bundle *lsb =
            &dt->out_dt.default_light_src_table[*plsi - 1];

        *errind = 0;
        *lstyp = lsb->type;
        switch ( *lstyp) {
        case PLIGHT_AMBIENT:
            GCOLR_DATA_RE_TRANSLATE( lsb->rec.ambient.colr, il, ia, rl, ra);
            break;
        case PLIGHT_DIRECTIONAL:
            ra[0] = lsb->rec.directional.dir.delta_x;
            ra[1] = lsb->rec.directional.dir.delta_y;
            ra[2] = lsb->rec.directional.dir.delta_z;
            rl = 3;
            GCOLR_DATA_RE_TRANSLATE( lsb->rec.directional.colr, il, ia, rl,
                &(ra[3]));
            break;
        case PLIGHT_POSITIONAL:
            ra[0] = lsb->rec.positional.pos.x;
            ra[1] = lsb->rec.positional.pos.y;
            ra[2] = lsb->rec.positional.pos.z;
            ra[3] = lsb->rec.positional.coef[0];
            ra[4] = lsb->rec.positional.coef[1];
            rl = 5;
            GCOLR_DATA_RE_TRANSLATE( lsb->rec.positional.colr, il, ia, rl,
                &(ra[5]));
            break;
        case PLIGHT_SPOT:
            ra[0] = lsb->rec.spot.pos.x;
            ra[1] = lsb->rec.spot.pos.y;
            ra[2] = lsb->rec.spot.pos.z;
            ra[3] = lsb->rec.spot.dir.delta_x;
            ra[4] = lsb->rec.spot.dir.delta_y;
            ra[5] = lsb->rec.spot.dir.delta_z;
            ra[6] = lsb->rec.spot.exp;
            ra[7] = lsb->rec.spot.coef[0];
            ra[8] = lsb->rec.spot.coef[1];
            ra[9] = lsb->rec.spot.angle;
            rl = 10;
            GCOLR_DATA_RE_TRANSLATE( lsb->rec.spot.colr, il, ia, rl, &(ra[10]));
            break;
        }
        fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr, errind, ldr, datrec);
    }
}


/* INQUIRE RENDERING COLOUR MODEL FACILITIES */
void
pqrcmf_( wtype, n, errind, nrcm, rcm)
Pint    *wtype;     /* workstation type */
Pint    *n;         /* list element requested of rendering colour model */
Pint    *errind;    /* OUT error indicator  */
Pint    *nrcm;      /* OUT number of rendering colour model */
Pint    *rcm;       /* OUT Nth element of list of rendering colour model */
{
    Wst_phigs_dt                *dt;
    Wst                 *wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
        *errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
        || dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
        *errind = ERR59;

    } else if ( dt->ws_category == PMO) {
        *errind = ERR62;

    } else {
        *errind = 0;
        *nrcm = dt->out_dt.num_rendering_colour_models;
        if ( *nrcm > 0) {
            if ( *n < 0 || *n > *nrcm) {
                *errind = ERR2002;
            } else if ( *n > 0) {
                *rcm = dt->out_dt.rendering_colour_models[*n - 1];
            }
        }
    }
}


/* INQUIRE TEXT EXTENT */
void
fqtxx_( wktype, font, chxp, chsp, chh, txp, txalh, txalv, str, errind, txexrx,
txexry, copx, copy, length)
Pint	*wktype;	/* workstation type */
Pint	*font;		/* text font    */
Pfloat	*chxp;		/* character expansion factor   */
Pfloat	*chsp;		/* character spacing    */
Pfloat	*chh;		/* character height */
Pint	*txp;		/* text path    */
Pint	*txalh;		/* text horizontal alignment    */
Pint	*txalv;		/* text vertical alignment  */
char	*str;		/* character string */
Pint	*errind;	/* OUT error indicator  */
Pfloat	*txexrx;	/* OUT text extent rectangle X  */
Pfloat	*txexry;	/* OUT text extent rectangle Y  */
Pfloat	*copx;		/* OUT concatenation offset X   */
Pfloat	*copy;		/* OUT concatenation offset Y   */
int	*length;	/* string length    */
{
    int                     phg_cb_valid_text_pair();
    Phg_args                cp_args;
    Phg_ret                 ret;
    register Phg_args_q_text_extent *args = &cp_args.data.q_text_extent;
    register Wst_phigs_dt   *dt;
    int                     charset_index;
    Wst                     *wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wktype))) {
        *errind = ERR52;

    } else if ( wst->bound_status != WST_BOUND) {
       	*errind = ERR51;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
        || dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
        *errind = ERR59;

    /* check for valid text pairs using charset 1 */
    } else if ( (charset_index = phg_cb_charset_index(dt,PCS_ASCII)) == -1) {
    /* this should never happen, so the assure is always wrong. */
        assure(charset_index != -1);

    } else if ( !phg_cb_valid_text_pair( *font, PPREC_STROKE,
        dt->out_dt.num_text_pairs[charset_index],
        dt->out_dt.text_pairs[charset_index])) {
        *errind = ERR106;

    } else if ( (*txp < PRIGHT || *txp > PDOWN) ||
              (*txalh < PAHNOR || *txalh > PARITE) ||
              (*txalv < PAVNOR || *txalv > PABOTT)) {
        *errind = ERR2000;

    } else if ( !str || !str[0]) {  /* don't waste time if no string. */
        *errind = 0;
        txexrx[0] = txexrx[1] = txexry[0] = txexry[1] = *copx = *copy = 0.0;

    } else {
        args->wsid = wst->wsid;
        /* use charset 1 for all charsets since this function does not
         * specify them */
        args->font = *font;
        args->path = (Ptext_path)*txp;
        args->hor = (Phor_text_align)*txalh;
        args->ver = (Pvert_text_align)*txalv;
        args->height = *chh;
        args->char_expan = *chxp;
        args->spacing = *chsp;
        if (( args->str = str))
            args->length = *length + 1;
        else
            args->length = 0;
        ret.err = 0;
        CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_TEXT_EXTENT, &cp_args, &ret);
        if ( ret.err) {
            *errind = ret.err;
        } else {
            *errind = 0;
            txexrx[0] = ret.data.text_extent.rect.p.x;
            txexrx[1] = ret.data.text_extent.rect.q.y;
            txexry[0] = ret.data.text_extent.rect.p.x;
            txexry[1] = ret.data.text_extent.rect.q.y;
            *copx = ret.data.text_extent.offset.x;
            *copy = ret.data.text_extent.offset.y;
        }
    }
}


/* INQUIRE DYNAMICS OF WORKSTATION ATTRIBUTES PLUS */
void
pqdwap_( wtype, errind, lsr, dcr, cmr)
Pint	*wtype;		/* workstation type */
Pint	*errind;	/* OUT error indicator  */
Pint	*lsr;		/* OUT light source refresentation  */
Pint	*dcr;		/* OUT depth cue source refresentation  */
Pint	*cmr;		/* OUT colour mapping refresentation    */
{
    /* Error 51 and 62 are not generated by this function */

    Wst_phigs_dt    *dt;
    Wst         *wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
        *errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
        || dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
        *errind = ERR59;

    } else {
        *errind = 0;
        *lsr = (Pint)dt->out_dt.light_source_rep;
        *dcr = (Pint)dt->out_dt.dcue_rep;
        *cmr = (Pint)dt->out_dt.colour_mapping_rep;
    }
}


/* INQUIRE COLOUR MAPPING FACILITIES */
void
pqcmf_( wtype, nrcm, errind, ncmtyp, cmtyp, npcmi)
Pint    *wtype;		/* workstation type */
Pint    *nrcm;		/* list element requested   */
Pint    *errind;	/* OUT error indicator  */
Pint	*ncmtyp;	/* OUT number of colour mapping method	*/
Pint	*cmtyp;		/* OUT Nth element of list of colour mapping method */
Pint	*npcmi;		/* OUT number of predefined colour mapping indices */
{
    Wst_phigs_dt                *dt;
    Wst                         *wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
        *errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
        || dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
        *errind = ERR59;

    } else if ( dt->ws_category == PMO) {
        *errind = ERR62;

    } else {
        *errind = 0;
        *ncmtyp =  dt->out_dt.num_colr_mapping_methods;
        if ( *ncmtyp > 0) {
            if ( *nrcm < 0 || *nrcm > *ncmtyp) {
                *errind = ERR2002;
                return;
            } else if ( *nrcm > 0) {
                *cmtyp = dt->out_dt.colr_mapping_methods[*nrcm - 1];
            }
        }
        *npcmi = dt->out_dt.num_predefined_colr_mapping_indices;
    }
}


/* INQUIRE PREDEFINED COLOUR MAPPING REPRESENTATION */
void
pqpcmr_( wtype, pcmi, mldr, errind, cmm, ldr, darec)
Pint	*wtype;		/* workstation type	*/
Pint	*pcmi;		/* predefined colour mapping index	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*cmm;		/* OUT colour mapping method	*/
Pint	*ldr;		/* OUT length of data record array	*/
char	*darec;		/* OUT data record	*/
{
    Wst_phigs_dt        *dt;
    Phg_colr_map_rep    *rep;
    Wst                 *wst;
    Pint                *i_ptr = NULL;
    Pfloat              *f_ptr = NULL;
    Pint                w_ia = 0;
    int                 i;

    Pint        il = 0;         /* number of integer entries    */
    Pint        *ia = NULL;     /* array containing integer entries     */
    Pint        rl = 0;         /* number of real entries       */
    Pfloat      *ra = NULL;     /* array containing real entries     */
    Pint        sl = 0;         /* number of character string entries   */
    Pint        *lstr = NULL;   /* length of character string entries   */
    char        *str = NULL;    /* character string entries     */

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
        *errind = ERR52;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
        || dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
        *errind = ERR59;

    } else if ( dt->ws_category == PMO) {
        *errind = ERR62;

    } else if ( *pcmi < 0) {
        *errind = ERR121;

    } else if ( *pcmi > dt->out_dt.num_predefined_colr_mapping_indices) {
        *errind = ERR102;

    } else {
        *errind = 0;
        rep = &dt->out_dt.default_colr_mapping_table[*pcmi - 1];
        *cmm = rep->method;
        switch ( *cmm) {
        case PCOLR_MAP_TRUE:
            /* no data record contents */
            break;

        case PCOLR_MAP_PSEUDO:
            il = 3;
            rl = rep->rec.meth_r2.weights.num_floats +
                 rep->rec.meth_r2.colrs.num_colr_reps * 3;
            PMALLOC( *errind, Pint, il, ia);
            PMALLOC( *errind, Pfloat, rl, ra);
            if ( !*errind) {
                ia[0] = rep->rec.meth_r2.colr_model;
                ia[1] = rep->rec.meth_r2.weights.num_floats;
                ia[2] = rep->rec.meth_r2.colrs.num_colr_reps;
                bcopy ((char *)rep->rec.meth_r2.weights.floats, (char *)ra,
                    rep->rec.meth_r2.weights.num_floats * sizeof(Pfloat));
                /* bad case PINDCT */
                if ( rep->rec.meth_r2.colr_model != PINDCT) {
                    bcopy( (char *)rep->rec.meth_r2.colrs.colr_reps,
                        (char *)&ra[ia[1]], (sizeof(Pfloat) * ia[2]) );
                }
            }
            break;
        case PCOLR_MAP_PSEUDO_N:
            il = 2 + rep->rec.meth_r3.colr_lists.num_lists;
            for ( i = 0; i < rep->rec.meth_r3.colr_lists.num_lists; i++) {
                rl += rep->rec.meth_r3.colr_lists.lists[i].num_floats;
            }
            PMALLOC( *errind, Pint, il, ia);
            PMALLOC( *errind, Pfloat, rl, ra);
            if ( !*errind) {
                ia[0] = rep->rec.meth_r3.colr_model;
                ia[1] = rep->rec.meth_r3.colr_lists.num_lists;
                i_ptr = &(ia[2]);
                f_ptr = ra;
                w_ia = 0;
                for ( i = 0; i < rep->rec.meth_r3.colr_lists.num_lists; i++) {
                    i_ptr[i] = rep->rec.meth_r3.colr_lists.lists[i].num_floats
                             + w_ia;
                    bcopy ((char *)rep->rec.meth_r3.colr_lists.lists[i].floats,
                        (char *)f_ptr,
                        rep->rec.meth_r3.colr_lists.lists[i].num_floats *
                        sizeof(Pfloat));
                    w_ia = i_ptr[i];
                    f_ptr += rep->rec.meth_r3.colr_lists.lists[i].num_floats;
                }
            }
            break;
        }
        if ( !*errind) {
            fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr,
                errind, ldr, darec);
        }
        PFREE( il, ia);
        PFREE( rl, ra);
    }
}


/* INQUIRE COLOUR MAPPING METHOD FACILITIES */
void
pqcmmf_( wtype, cmm, mldr, errind, ldr, darec)
Pint	*wtype;		/* workstation type	*/
Pint	*cmm;		/* colour mapping method	*/
Pint	*mldr;		/* dimensions of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ldr;		/* OUT length og data record array	*/
char	*darec;		/* OUT data record	*/
{
    Wst_phigs_dt    *dt;
    Wst         	*wst;

    Pint        il = 0;         /* number of integer entries    */
    Pint        ia[1];          /* array containing integer entries     */
    Pint        rl = 0;         /* number of real entries       */
    Pfloat      *ra = NULL;     /* array containing real entries     */
    Pint        sl = 0;         /* number of character string entries   */
    Pint        *lstr = NULL;   /* length of character string entries   */
    char        *str = NULL;    /* character string entries     */

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if (!( wst = phg_fb_wst_exists( *wtype))) {
        *errind = ERR52;

    } else if ( wst->bound_status != WST_BOUND ) {
        *errind = ERR51;

    } else if ( !((dt = &wst->desc_tbl.phigs_dt)->ws_category == POUTPT
        || dt->ws_category == POUTIN || dt->ws_category == PMO) ) {
        *errind = ERR59;

    } else if ( dt->ws_category == PMO) {
        *errind = ERR62;

    } else if ( !phg_cb_int_in_list( *cmm, dt->out_dt.num_colr_mapping_methods,
        dt->out_dt.colr_mapping_methods ) ) {
        *errind = ERR126;

    } else {
        switch ( *cmm) {
        case PCOLR_MAP_TRUE:
            il = 1;
            ia[0] = wst->desc_tbl.phigs_dt.out_dt.num_true_colours;
            break;
        case PCOLR_MAP_PSEUDO:
            il = 1;
            ia[0] = wst->desc_tbl.phigs_dt.out_dt.num_colours -
                wst->desc_tbl.phigs_dt.out_dt.num_true_colours;
            break;
        case PCOLR_MAP_PSEUDO_N:
            break;
        }
        if ( !*errind) {
            fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr,
                errind, ldr, darec);
        }
    }
}
