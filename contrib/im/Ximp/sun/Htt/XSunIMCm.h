#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunIMCm.h 1.3 91/07/15";
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

  Author: Hideki Hiura (hhiura@Sun.COM)
	  				     Sun Microsystems, Inc.
******************************************************************/

#ifndef xim_imlogic_jle103_DEFIND
#define xim_imlogic_jle103_DEFIND


typedef struct _iml_session {
/* ---- Inherit Base class [imlogic_session] ----- */

#define xim_iml_inheritance_DEFIND
#include "XSunIMCore.h"
#undef  xim_iml_inheritance_DEFIND

/* ----- Derived class [jle103_session] declaration ---- */

wchar_t conversion_on_key ;

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

#define DEFAULTPreEditTextBufferSize 512
#define DEFAULTPreEditAttrBufferSize 512
#define DEFAULTStatusStringRemainBufferSize 512
/*
 * for the active_ regions
 */
#define PREEDIT_IS_ACTIVE	0x00000001
#define STATUS_IS_ACTIVE	0x00000002
#define LOOKUP_IS_ACTIVE	0x00000004
#define AUX_IS_ACTIVE		0x00000008

/*
 * for the remainder flag due to handle pseudo_packet in eval_it()
 */

#define PRE_EDIT_DONE_REQUIRED	0x00000001
#define STATUS_DONE_REQUIRED	0x00000002
#define STATUS_DRAW_REQUIRED	0x00000004
#define IMM_COMMIT_REQUIRED	0x00000008

#define JLE103_STATUS_STRING_FILE "/usr/lib/mle/japanese/se/sunview/mode.dat"

#define MAX_ENV_OPCODE 153

/*
 * jle103 session private member functions
 */

Private int jle103_open();
Private void jle103_close();
Private void jle103_reset();
Private void jle103_get_key();
Private void jle103_get_key_map();
Private void jle103_set_key_map();
Private int  jle103_send_event();
Private void jle103_set_keyboard_state();
Private void jle103_conversion_on();
Private void jle103_send_results();
Private void jle103_send_commit();
Private void jle103_make_conversion_on();
Private void jle103_make_status_string();
Private void jle103_set_choice();
Private void jle103_attr2feedback();
Private int jle103_create_session();
Private void jle103_opsw_init();
Private int construct_cm_initial_data();
Private iml_inst **eval_it();
Private void  init_status_string();

Private void eval_cm_to_env_packet();
Private void destruct_session();

Private struct cm_to_env *get_cte();
Private env_commit();
Private iml_inst *make_preedit_erase_inst();
Private iml_inst *make_preedit_done_inst();


extern struct cm_to_env *cm_put();

#endif  xim_imlogic_jle103_DEFINED
