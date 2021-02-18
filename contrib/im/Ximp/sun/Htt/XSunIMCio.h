#ifndef lint
#ifdef sccs
static char     sccsid[] = "%Z%%M% %I% %E%";
#endif
#endif

/******************************************************************

              Copyright 1990, 1991, by Sun Microsystems, Inc.

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sun Microsystems, Inc.
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.
Sun Microsystems, Inc. makes no representations about the suitability of
this software for any purpose.  It is provided "as is" without
express or implied warranty.

Sun Microsystems Inc. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL Sun Microsystems, Inc. BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

  Author: Masayuki Seino (seino@Japan.Sun.COM)
                                             Sun Microsystems, Inc.
******************************************************************/


#ifndef xim_imlogic_cio_DEFIND
#define xim_imlogic_cio_DEFIND


typedef struct _iml_session {
/* ---- Inherit Base class [imlogic_session] ----- */

#define xim_iml_inheritance_DEFIND
#include "XSunIMCore.h"
#undef  xim_iml_inheritance_DEFIND

/* ----- Derived class [cio_session] declaration ---- */

/*
 * Region Specific data
 *
 * Preedit region
 */

/*
 * Status region
 */
    int NStatusInFile ;
    int NStatus ;
    IMStatusString StatusString[MAXSTATUS] ;
    IMCurrentStatusString CurrentStatusString[MAXSTATUS] ;

/*
 * Lookup Choice region
 */

} iml_session_t;

#define xim_imlogic_session_DEFIND


#define JLE103_STATUS_STRING_FILE "/usr/lib/mle/japanese/se/sunview/mode.dat"

/*
 * cio session private member functions
 */

static int cio_open();
static void cio_close();
static void cio_reset();
static void cio_get_key();
static void cio_get_key_map();
static void cio_set_key();
static void cio_set_key_map();
static int cio_send_event();
static void cio_set_keyboard_state();
static void cio_conversion_on();
static void cio_send_results();
static void cio_conversion_on();
static void cio_status_string();
static void cio_iml_set_choice();

/*extern struct ci_to_env *ci_put();*/
extern int checkonoff();
extern Ci_keycode *setkey();
extern void cio_make_status_string();

#endif  xim_imlogic_cio_DEFINED
#define	ADD	1
#define	REMOVE (-1)


#undef iskanji
#undef iskana
#define		iskanji(x)	((unsigned char)x >= 0xa0)
#define		iskana(x)	((unsigned char)x = 0x8e)


FILE	*fenv;
Ci_clntinfo     clntinfo;
Ci_csinfo	*csinfo;
int	select_id,interm_id;
Cs_select	selreg;

/*
 *      misc region information.
 */
int     misc_id ;                       /* misc region id no            */
Cs_misc         miscreg;                /* misc region save area        */
Cs_select       m_sel1reg;              /* select region save area      */
Cs_select       m_sel2reg;              /* select region save area      */
char    *m_sel1buf ;             /* misc region select field1    */
                                        /*      item buffer             */
int     m_sel1cnt ;                     /* misc region sellect_field1   */
char    *m_sel2buf ;             /* misc region select field2    */
                                        /*      item buffer             */
int     m_sel2cnt ;                     /* misc region sellect_field2   */
                                        /*      item count              */
/*
 *      citest control flags.
 */
int     cnvon;                          /* 1 : convert on
                                           0 : convert off              */
int     selon ;                      /* 1 : select region on
                                           0 : select region off        */
int     miscon ;                     /* 1 : misc region on
                                           0 : misc region off  */
int     active_f;
int     m_sel1on ;                   /* 1 : select region on
                                           0 : select region off        */
int     m_sel2on ;                   /* 1 : select region on
                                           0 : select region off        */
Ci_csinfo       csinfo_save ;           /* CS information               */


