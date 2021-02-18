#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunIMPub.h 1.4 91/08/13";
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

#ifndef xim_imlogic_public_DEFIND
#define xim_imlogic_public_DEFIND


/*
 * Iml Public function declaration
 */

extern char * memalign();
extern char * calloc();

/*
 *  iml_subr.c
 */

Public iml_session_t * 	_iml_get_session();
Public int 		_iml_construct_session();
Public IMM_Region 	_iml_get_current_region();
Public int 		_iml_put_current_region();
Public int 		_iml_pop_current_region();
Public void 		_iml_post_proc();
Public void 		_iml_error();
Public int 		_iml_calc_visible_pos();
Public iml_inst *	_iml_link_inst_tail();
Public void             _iml_fake_nop_inst();
Public Status           _iml_make_commit_inst();
Public iml_inst *	_iml_make_nop_inst();
Public iml_inst *	_iml_make_imm_set_status_inst();
Public iml_inst *	_iml_make_preedit_start_inst();
Public iml_inst *	_iml_make_preedit_erase_inst();
Public iml_inst *	_iml_make_preedit_done_inst();
Public iml_inst *	_iml_make_status_start_inst();
Public iml_inst *	_iml_make_status_done_inst();
Public iml_inst *	_iml_make_lookup_start_inst();
Public iml_inst *	_iml_make_lookup_process_inst();
Public iml_inst *	_iml_make_lookup_done_inst();

#ifndef IMLTRACE
#define _iml_slot_trace(a,b,c,d,e,f) /**/
#define _iml_inst_trace(a,b) /**/
#define _iml_trace(a,b,c) /**/
#define _imm_trace(a,b,c) /**/
#define _iml_xdmem(a,b) /**/
#else
Public void _iml_slot_trace() ;
Public void _iml_inst_trace() ;
Public void _iml_trace() ;
Public void _imm_trace() ;
Public void _iml_xdmem() ;
#endif IMLTRACE

/*
 * iml_mman.c Slot allocataor/deallocater
 */

Public void * _iml_new();
Public void * _iml_delete();
Public void * _iml_new2();
Public void * _iml_delete2();

/*
 * iml_le_sw.c
 */
Public int _iml_configure();

/*
 * XSunKeyBind.c
 */
Public int XSunLookupString();
#endif  xim_imlogic_public_DEFINED
