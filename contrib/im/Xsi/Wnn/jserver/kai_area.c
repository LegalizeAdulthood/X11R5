/*
 * $Id: kai_area.c,v 1.2 1991/09/16 21:31:49 ohm Exp $
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
/*********************
 * kaiseki work area
**********************/

#include "commonhd.h"
#include "ddefine.h"

w_char	*bun;	/* D */

w_char giji_eisuu[20];	/* 擬似「英数」の定義 */

int	maxchg;
int	initjmt;	/* I think initjmt is the length of jmt_ */

int	*maxj;	/* maxj is counts to entries in jmt_ */

/* jmt_ptr is used in jmt0.c only.
   but it must be changed by clients.
   jishobiki does not use it. this is sent to jishobiki by arguments
   */

/* j_e_p is used to hold the current point to which jmtw_ is used */
struct	jdata	*j_e_p;
struct	jdata	**jmt_;
struct	jdata	*jmtw_;
struct	jdata	**jmt_end;
struct	jdata	*jmtw_end;
struct	jdata	**jmt_ptr;
struct	jdata	***jmtp;

struct	FT	*ft;

#ifdef	nodef
/********************************
 *	疑似自立語の品詞	*
 ********************************/
int	sentou_no;	/* 「先頭」文節先頭に成れるもの */
int	suuji_no;	/* 「数字」数詞相当 */
int	katakanago_no;	/* 「カナ」外来語など 名詞相当 */
int	eisuu_no;	/* 「英数」*/
int	kigou_no;	/* 「記号」 */
int	toji_kakko_no;	/* 「閉括弧」 */
int	fuzokugo_no;	/* 付属語だけ */
int	kai_kakko_no;	/* 「開括弧」*/
int	giji_no;	/* 疑似 */
#endif	/* nodef */
