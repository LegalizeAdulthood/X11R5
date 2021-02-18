#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunIMPriv.h 1.3 91/07/15";
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

#ifndef xim_imlogic_private_DEFIND
#define xim_imlogic_private_DEFIND

#define FALES 0
#define TRUE 1
#define MAXSTATUS 50
#define STATUSNAMELEN 80
#define STATUSVALUELEN 80
#define STATUSSTRINGLEN 90
#define BUFSIZE 256
#define SLOTMINSIZE 64
#define Private static


typedef struct {
    wchar_t	name[STATUSNAMELEN];
    wchar_t	value[STATUSVALUELEN];
    wchar_t	string[STATUSSTRINGLEN];
    int		order ;
} IMStatusString ;

typedef struct {
    wchar_t	*name;
    wchar_t	*value;
    wchar_t	*string;
    int		order ;
} IMCurrentStatusString ;

typedef struct {
    wchar_t *buf ;
    int	    size ;
} IMStatusStringRemain ;

typedef struct _current_region {
    struct _current_region * next ;
    struct _current_region * prev ;    
    int region ;
}iml_region_stack_t ;

#define IML_SESSION_RECYCLE (-2)

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

/*
 * several buffer size definitions
 */
#define DEFAULTPreEditTextBufferSize 512
#define DEFAULTPreEditAttrBufferSize 512
#define DEFAULTStatusStringRemainBufferSize 512
#define DEFAULTXLookupStringBufferSize 512
#define LOCALENAMELEN 64


/*
 * for the private_status
 */
#define CB_PARTIAL_UPDATE	0x00000002
#define LE_PARTIAL_UPDATE	0x00000004
#define IMPLICIT_CONVERSION_ON	0x00000008
/*
 * for the active_regions
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

/*
 * IMLogic Error Code
 */
#define IMLERR_CANNOT_OPEN (-1)
#define IMLERR_UNKNOWN_OPCODE (-2)


#endif  xim_imlogic_private_DEFINED
