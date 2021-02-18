/*
 * $Id: wnn_string.h,v 1.4 1991/09/18 06:27:34 proj Exp $
 */
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1991
 * Copyright OMRON Corporation. 1991
 * Copyright ASTEC, Inc. 1991
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

#ifndef _WNN_STRING_
#define _WNN_STRING_

extern int sStrcpy();
extern int Sstrcpy();
extern void Sreverse();
extern char * Stos();
extern char *sStrncpy();
extern w_char *Strcat();
extern w_char *Strncat();
extern int Strcmp();
extern int Substr();
extern int Strncmp();
extern w_char *Strncpy();
#ifndef JS
extern int Strlen();
extern w_char *Strcpy();
#endif
extern w_char *Index();
extern w_char *Rindex();
extern w_char *Strpbrk();
extern int Strspn();
extern int Strcspn();
extern w_char *Strtok();
extern int Strwidth();
extern int Strnwidth();
#if	defined(CONVERT_by_STROKE) || defined(CONVERT_with_SiSheng)
extern int pan_Sstrcpy();
#endif
#endif
