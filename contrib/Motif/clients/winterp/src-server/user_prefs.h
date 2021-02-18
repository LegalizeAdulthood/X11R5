/* -*-C-*-
********************************************************************************
*
* File:         user_prefs.h
* RCS:          $Header: user_prefs.h,v 1.4 91/03/14 03:12:54 mayer Exp $
* Description:  type definition and global datastructure for user preferences
* Author:       Niels Mayer, HPLabs
* Created:      Thu Aug 11 00:06:04 1988
* Modified:     Thu Oct  3 20:00:57 1991 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r5 contrib tape release
*
* WINTERP Copyright 1989, 1990, 1991 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
*
********************************************************************************
*/

#include "../src-server/config.h"

typedef struct {
  String lisp_init_file;
  String lisp_transcript_file;
  String lisp_lib_dir;
  Boolean enable_init_msgs;

#ifdef WINTERP_WANT_INET_SERVER
  int    service_port;
  String service_name;
  Boolean enable_AF_INET_server;
#endif				/* WINTERP_WANT_INET_SERVER */

#ifdef WINTERP_WANT_UNIX_SERVER
  Boolean enable_AF_UNIX_server;
  String  unix_socket_filepath;
#endif				/* WINTERP_WANT_UNIX_SERVER */

  Boolean enable_XtError_break;
  Boolean enable_XtWarning_break;
  Boolean enable_XError_break;
} USER_PREFS_DATA, *USER_PREFS_DATA_PTR;

extern USER_PREFS_DATA user_prefs;
