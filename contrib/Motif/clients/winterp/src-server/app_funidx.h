/* -*-C-*-
********************************************************************************
*
* File:         app_funidx.h
* RCS:          $Header: app_funidx.h,v 1.1 91/03/14 03:12:46 mayer Exp $
* Description:  This file is #included by w_funtab.h. It allows applications
*		built upon winterp to add new  functions to the xlisp function
*		table set up in w_funtab.c. Any entries in this file must have
*		corresponding function table entries in app_fundecl.h.
* Author:       Niels Mayer, HPLabs
* Created:      Fri Dec  1 16:39:10 1989
* Modified:     Thu Oct  3 19:58:07 1991 (Niels Mayer) mayer@hplnpm
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

/*
	FORMAT FOR ENTRIES IN THIS FILE: 
	this is just a sequence of identifiers, all terminated by commas ','.
	The name of the identifiers is composed of the C method or function 
	name (added in app_fundecl.h) prepended by 'FTAB_'.

 FTAB_The_First_Application_Primitive,
 FTAB_The_Second_Application_Primitive,

 Note: see also app_fundecl.h, app_funextn.h and w_funtab.h.
*/
