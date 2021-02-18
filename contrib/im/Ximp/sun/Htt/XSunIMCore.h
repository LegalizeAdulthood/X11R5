#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunIMCore.h 1.6 91/08/24";
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

/*
 *  Base class "imlogic_session" declaration
 */

#ifndef xim_iml_session_DEFIND
#ifndef xim_iml_inheritance_DEFIND
typedef struct _iml_session {
#endif xim_iml_inheritance_DEFIND

    struct _iml_session *next ;
    char *classname ;    /* right now,jle/wnn/cio. future, it could be sj3 */
    char *enginename ;   /* Because our scheme has multiple le hierarchy */
    char *locale ;       /* locale for this IC */
    int language_engine ;/* ENGINE ID: to support multiple language engine */
    int session_id ;	/* to keep self identification. */
    int public_status ; /* This status can be set/got by IML_SET/GET_STATUS */
    int initial_public_status ;/* will be copied from public status per call */
    int private_status ;
    iml_inst        *remainder ; /* awaiting event put back returning */
    iml_inst        *remainder2 ;/* awaiting recall by RESULTS_REQUIRED */
    iml_inst       **rrv ;
    iml_inst_slot_t *short_term_slot; /* for new/delete */
    iml_inst_slot_t *long_term_slot;  /* for new2/delete2 */
    int active_regions ;
    iml_region_stack_t  current_region ;

    XKeyEvent *event;
    KeySym     keysym;
    char      *XLookupBuf ;
    int        XLookupBuflen ;
    Status     status;
/*
 * Region Specific data
 *
 * Preedit region
 */
    int PreEditTextBufferSize ;
    int PreEditAttrBufferSize ;
    XIMPreeditDrawCallbackStruct PreEditTextInfo ; /* allocate once */
    int PreEditBufferSizeInCallback ;
/*
 * Status region
 */

    IMStatusStringRemain StatusStringRemainBuffer ;    

/*
 * Lookup Choice region
 */

    WhoIsMaster 	WhoIsMaster ;
    LayoutInfo		XIMPreference ;
    LayoutInfo		CBPreference ;
    XIMLookupDrawCallbackStruct *ld ; /* As a terminal */
/*
 * Aux region
 */

/*
 * member functions
 */
    int  (*iml_open)();
    void (*iml_close)();
    void (*iml_reset)();
    void (*iml_get_status)();
    void (*iml_get_key_map)();
    void (*iml_set_status)();
    void (*iml_set_key_map)();
    int  (*iml_send_event)();
    void (*iml_set_keyboard_state)();
    void (*iml_conversion_on)();
    void (*iml_send_results)();
    void (*iml_send_commit)();
    
#ifndef xim_iml_inheritance_DEFIND
} iml_session_t;

#endif xim_iml_inheritance_DEFIND
#endif xim_iml_session_DEFIND 


