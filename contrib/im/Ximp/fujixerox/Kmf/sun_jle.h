/*

Copyright 1991 by Fuji Xerox Co., Ltd., Tokyo, Japan.

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of M.I.T. not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission.  Fuji Xerox Co., Ltd. makes no representations
about the suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.

FUJI XEROX CO.,LTD. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJI XEROX CO.,LTD. BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

*/

#include <mle/cm.h>
#include <mle/env.h>
#include <mle/cmkeys.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CMBUFSIZE (1024)

typedef struct {
    int ncandidate;
    char **candidates;
    int disp_first;
    int disp_num;
} SelectRec, *Select;

typedef struct {
    char *key;
    u_char *label;
} JLEmodeRec, *JLEmode;

typedef struct _JLEPrivate {
    int cm_id;
    Cm_session cm_session;
    int cmon;
    int cmon_key;
    int mode;
    int region;
    Select select;
    JLEmode ce_name;
    int ce_name_active;
    int ce_name_count;
    JLEmode conv;
    int conv_active;
    int conv_count;
    JLEmode cm_state;
    int cm_state_active;
    int cm_state_count;
    Bool preedit_active;
    Bool status_active;
} JLEPrivateRec, *JLEPrivate;
