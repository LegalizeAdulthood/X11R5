/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Noriyuki Hosoe, Sony Corporation
 */

#define	SJ3SERV_VERSION_NO	1

#define	SJ3_CONNECT	1
#define	SJ3_DISCONNECT	2

#define	SJ3_OPENDICT	11
#define	SJ3_CLOSEDICT	12

#define	SJ3_OPENSTDY	21
#define	SJ3_CLOSESTDY	22
#define	SJ3_STDYSIZE	23

#define	SJ3_LOCK	31
#define	SJ3_UNLOCK	32

#define	SJ3_PH2KNJ	41

#define	SJ3_CL2KNJ	51
#define	SJ3_NEXTCL	52
#define	SJ3_PREVCL	53
#define	SJ3_CL2KNJ_ALL	54
#define	SJ3_CL2KNJ_CNT	55

#define	SJ3_STUDY	61
#define	SJ3_CLSTUDY	62

#define	SJ3_ADDDICT	71
#define	SJ3_DELDICT	72

#define	SJ3_MAKEDICT	81
#define	SJ3_MAKESTDY	82
#define	SJ3_MAKEDIR	83
#define	SJ3_ACCESS	84

#define	SJ3_GETDICT	91
#define	SJ3_NEXTDICT	92
#define	SJ3_PREVDICT	93

#define	SJ3_WHO		100
#define	SJ3_QUIT	101
#define	SJ3_KILL	102
#define	SJ3_VERSION	103
#define	SJ3_DICTPASS	104
#define	SJ3_DICTCMNT	105
#define	SJ3_STDYPASS	106
#define	SJ3_STDYCMNT	107

