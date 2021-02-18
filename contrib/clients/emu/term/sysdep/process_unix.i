/* -*-Mode: C; -*- */

#ifndef _PROCESS_UNIX_INCLUDE
#define _PROCESS_UNIX_INCLUDE

/* $Header: /usr3/Src/emu/term/sysdep/RCS/process_unix.i,v 1.1 91/09/30 21:35:14 jkh Exp $ */

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
 * Various process manipulation routines that will work on any flavor of
 * unix.
 *
 * Author: Jordan K. Hubbard
 * Date: August 22nd, 1991
 * Description: Here are some routines that appear to be applicable to any
 *		generic and reasonably sane unix.
 *
 *
 * $Log:	process_unix.i,v $
Revision 1.1  91/09/30  21:35:14  jkh
Initial revision

 *
 */

/* Mourn for the recently deceased */
Local int
unix_process_wait(fc)
int *fc;
{
     int pid;

     if ((pid = wait(fc)) == -1)
          perror("wait");
     return pid;
}

#endif	/* _PROCESS_UNIX_INCLUDE */
