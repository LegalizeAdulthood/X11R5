/* $Header: /usr3/emu/include/RCS/config.h,v 1.4 90/10/12 14:08:46 jkh Exp $ */

/*
 * This file is part of the PCS emu system.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * This file contains all user-configurable options for emu.
 *
 * Author: Jordan K. Hubbard
 * Date: March 20th, 1990.
 * Description: Here lie all user settable options and compile-time
 *		flags for emu. You may also want to look at os.h
 *		for OS-specific options.
 *
 * Revision History:
 *
 * $Log:	config.h,v $
 * Revision 1.4  90/10/12  14:08:46  jkh
 * Name changes; threw out NO_TRANSLATIONS.
 * 
 * Revision 1.3  90/07/26  02:42:44  jkh
 * Added new copyright.
 * 
 * Revision 1.2  90/07/09  03:06:01  jkh
 * MAX_TRANSLATIONS added.
 * 
 * Revision 1.1  90/05/02  18:10:08  jkh
 * Initial revision
 * 
 *
 */

/* maximum line length */
#define MAX_CHARS_IN_LINE	1000

/* maximal number of lines */
#define MAX_LINES		1000
