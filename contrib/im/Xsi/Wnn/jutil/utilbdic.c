/*
 * $Id: utilbdic.c,v 1.2 1991/09/16 21:32:28 ohm Exp $
 */
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/*	Version 4.0
 */
/*
  Binary (Updatable, Stable) dictionary common routine.
*/

/* extern functions are
  output_header(ofpter)
  output_hindo(ofpter)
  output_hinsi(ofpter)
*/

#include <stdio.h>
#include "commonhd.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"

int wnnerror;

struct JT jt;
struct wnn_file_head file_head;

extern char *passwd;

output_hinsi(ofpter)
register FILE *ofpter;
{
    register int i;

    for(i = 0 ; i < jt.maxserial ; i++){
	put_short(ofpter,jeary[i]->hinsi);
    }
/*
  if(i & 0x1){
    put_short(ofpter,0);
  }
*/
}

input_hinsi(ifpter)
register FILE *ifpter;
{
    register int i;
    unsigned short s;

    for(i = 0 ; i < jt.maxserial ; i++){
	if(get_short(&s, ifpter) == -1)return(-1);
	jeary[i]->hinsi = s;
    }
/*
  if(i & 0x1){
    get_short(ifpter);
  }
*/
  return(0);
}

#ifdef nodef
revdic_jt(jtp, match, hostart, tary)
struct JT *jtp;
int match;
char *hostart;
struct uind1 *tary;
{
/* Must reverse hinsi?????? */

    if(jtp->syurui == WNN_UD_DICT){
	rev_ud_hontai(hostart,jtp->maxhontai, match);
	rev_ud_table(tary,jtp->maxtable, match);
    }else{
	rev_sd_hontai(hostart, match);
    }
    return(0);
}
#endif
