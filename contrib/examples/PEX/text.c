/* $XConsortium: text.c,v 5.2 91/04/02 17:05:17 hersh Exp $ */

/***********************************************************
Copyright (c) 1991 by Sun Microsystems, Inc. and the X Consortium.

                    All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
** Example program to test text rendering 
*/
#include <stdio.h>
#include <errno.h>

#ifdef USE_X_DRAWABLE
#include "X11/Xlib.h"
#include "X11/Xatom.h"
#endif /*USE_X_DRAWABLE*/

#ifdef PEX_SI_PHIGS
#include "phigs/phigs.h"
#else /*PEX_SI_PHIGS*/
#include <phigs/phigs.h>
#endif /*PEX_SI_PHIGS*/

/* Workstation ids */
#define WS1 	1

#ifdef USE_X_DRAWABLE
extern void init_window();
#endif /*USE_X_DRAWABLE*/

static Ppoint_list3 markers;
static char phigsstring[] = "PEX-SI RENDERS TEXT";
static char phigsstring1[] = "Testing Character Spacing & Up Vector";
static char annostring1[] = "Testing 3D Annotation Text; Style=2";
static char annostring2[] = "Testing 3D Annotation Text; Style=1";
static char annostring3[] = "Testing 2D Annotation Text; Style=2";
static char annostring4[] = "Testing 2D Annotation Text; Style=1";

static Ptext_bundle rep = {1,PPREC_STRING,3.0,1.5,5};
static Ppoint txpt = {0.05,0.85};

static Pvec pup = {1.0,1.0};
static Pvec pupd = {0.0,1.0};

static Ppoint3 txpta = {0.5,0.5,0.0};
static Ppoint3 txptc = {0.3,0.75,0.0};

static Pvec3 txdir[2] = {1.0, 0.0, 0.0,
    		 0.0, 1.0, 0.0};
static Ppoint txptb = {0.05,0.95};
static Ppoint txptd = {0.1,0.6};

static Ppoint3 txpt1 = {0.5,0.5,0.0};
static Ppoint3 offset1 = {-0.5,-0.5,0.0};

static Ppoint txpt3 = {0.5,0.5};
static Ppoint offset3 = {-0.5,0.4};

static Ppoint3 txpt2 = {0.2,0.2,0.0};
static Ppoint3 offset2 = {-0.2,0.4,0.0};

static Ppoint txpt4 = {0.1,0.5};
static Ppoint offset4 = {-0.0,0.2};


static void
init_phigs()
{
#ifdef USE_X_DRAWABLE
    extern Display *appl_display;
    extern Window appl_window;
    Pconnid_x_drawable conn;
#endif /*USE_X_DRAWABLE*/

#ifdef USE_X_DRAWABLE
    conn.display = appl_display;
    conn.drawable_id = appl_window;
#endif /*USE_X_DRAWABLE*/
/*
 *   ... open a phigs workstation 
 */
    popen_phigs( NULL, 0);
    {
     Psys_st	sys_state;

     pinq_sys_st( &sys_state );
       	 if (sys_state != PSYS_ST_PHOP) {
       	   exit(1);
     }
    }
#ifdef USE_X_DRAWABLE
    popen_ws(WS1, (Pconnid)(&conn), phigs_ws_type_x_drawable);
#else /*!USE_X_DRAWABLE*/
    popen_ws(WS1, (Pconnid)NULL, phigs_ws_type_x_tool);
#endif /*USE_X_DRAWABLE*/
    {
   	 Pws_st  ws_state;
 
       	 pinq_ws_st(&ws_state);
       	 if (ws_state != PWS_ST_WSOP) {
       exit(3);
     }
    }
}

static void
init()
{
#ifdef USE_X_DRAWABLE
    init_window();
#endif /* USE_X_DRAWABLE */
    init_phigs();
}

main()
{
    init();

    markers.num_points = 1;

    popen_struct(1);

    /* 2D text */

        pset_text_path (PPATH_RIGHT);  
        pset_char_ht (-0.015);
        pset_text_colr_ind (1);
        pset_char_expan (-2.0);
        ptext(&txptb, "PEX-SI's 2D/3D Text EXPANDS !");
    
        pset_text_path (PPATH_RIGHT);  
        pset_char_expan (-1.0);
        pset_char_space (0.8);
        pset_char_ht (-0.015);
        pset_char_up_vec (&pup);
        ptext(&txptd, phigsstring1);
    
        /* 2D text with text bundle */
    
        pset_text_path (PPATH_RIGHT);  
        pset_char_ht (-0.015);
        pset_char_up_vec (&pupd);
        pset_text_rep (1,1,&rep);
        pset_indiv_asf(PASPECT_CHAR_EXPAN, PASF_BUNDLED);
        pset_indiv_asf(PASPECT_CHAR_SPACE, PASF_BUNDLED);
        pset_indiv_asf(PASPECT_TEXT_COLR_IND, PASF_BUNDLED);
        ptext(&txpt, "Text Bundle Test");
    
        /* 3D text */
    
        pset_marker_size(5.0);
        markers.points = &txpta;
        ppolymarker(&markers);
    
        pset_indiv_asf(PASPECT_CHAR_EXPAN, PASF_INDIV);
        pset_indiv_asf(PASPECT_CHAR_SPACE, PASF_INDIV);
        pset_indiv_asf(PASPECT_TEXT_COLR_IND, PASF_INDIV);
        pset_char_space (0.0);
        pset_text_path (PPATH_RIGHT);
        pset_char_ht (-0.01);
        pset_char_expan (-2.0);
        pset_text_colr_ind (2);
        ptext3(&txpta,txdir,phigsstring);
    
        pset_text_path (PPATH_LEFT);  
        pset_char_ht (-0.015);
        pset_char_expan (-1.0);
        pset_text_colr_ind (3);
        ptext3(&txpta,txdir,"PEX-SI TEXT PATH LEFT"); 
    
        pset_text_path (PPATH_UP);  
        pset_char_ht (0.02);
        pset_text_colr_ind (6);
        ptext3(&txpta,txdir,"PEX-SI PATH UP"); 
    
        pset_text_path (PPATH_DOWN);  
        pset_char_ht (0.025);
        pset_text_colr_ind (7);
        ptext3(&txpta,txdir,"SI PATH DOWN"); 
    
        pset_text_path (PPATH_RIGHT);  
        pset_char_ht (0.03);
        pset_text_colr_ind (4);
        ptext3(&txptc,txdir,phigsstring);
    
        /* Annotation text 3D */
    
        pset_linewidth(2.0);
        pset_line_colr_ind(5);
        pset_linetype (4);
        pset_anno_path (PPATH_RIGHT);  
        pset_anno_char_ht (-0.015);
        pset_anno_style (2);
        pset_text_colr_ind (5);
        panno_text_rel3(&txpt1,&offset1,annostring1);
    
        pset_anno_path (PPATH_RIGHT);  
        pset_anno_char_ht (-0.015);
        pset_char_expan (-2.0);
        pset_anno_style (1);
        pset_text_colr_ind (3);
        panno_text_rel3(&txpt2,&offset2,annostring2);
    
        /* Annotation text 2D */
    
        pset_linewidth(1.0);
        pset_line_colr_ind(6);
        pset_linetype (3);
        pset_anno_path (PPATH_RIGHT);  
        pset_anno_char_ht (-0.020);
        pset_anno_style (2);
        pset_text_colr_ind (6);
        panno_text_rel(&txpt3,&offset3,annostring3);
    
        pset_anno_path (PPATH_RIGHT);  
        pset_anno_char_ht (-0.020);
        pset_char_expan (-1.0);
        pset_anno_style (1);
        pset_text_colr_ind (7);
        panno_text_rel(&txpt4,&offset4,annostring4);

    pclose_struct();

    ppost_struct(WS1,1,(Pfloat)0.0);

    printf("return...");
    getchar();   /* Exit Signal; a keyboard input */

    pclose_ws(WS1);
    pclose_phigs();
}




