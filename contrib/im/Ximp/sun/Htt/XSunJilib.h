/* $Id: jilib.h,v 4.0 89/10/27 20:15:32 ishisone Rel $ */

/*
 *	jilib.h -- jilib $BMQ%X4@00:19:24!%$%k(J (Wnn Version4 $BBP1~HG(J)
 *		version 4.0
 *		ishisone@sra.co.jp
 */

/*
 * Copyright (c) 1989  Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 */

#ifndef _jilib_h
#define _jilib_h

#include	"jslib.h"

/* errormessage $B%3!<%k/home/sundae4/donation/si/SCCS_DIRECTORIES/usr.lib/libxim/SCCS/s.XSunJilib.hC%/$G;H$o$l$k!"%(%i!<$N<oN`$r<($9Dj?t(J */
#define TYPE_WARNING	0
#define TYPE_ERROR	1

/* confirm $B%3!<%k/home/sundae4/donation/si/SCCS_DIRECTORIES/usr.lib/libxim/SCCS/s.XSunJilib.hC%/$G;H$o$l$k!"00:19:24!%$%k$N<oN`$r<($9Dj?t(J */
#define TYPE_DIC	1
#define TYPE_HINDO	2

#ifdef __STDC__
extern WNN_JSERVER_ID *jiOpenServer(char *servername, int timeout);
extern int jiCloseServer(WNN_JSERVER_ID *server);
extern WNN_ENV *jiCreateEnv(WNN_JSERVER_ID *server, char *envname,
			    int override, char *wnnrcfile,
			    void (*errmsgfunc)(), int (*confirmfunc)(),
			    caddr_t client_data);
extern int jiDeleteEnv(WNN_ENV *env);
#else
extern WNN_JSERVER_ID *jiOpenServer();
extern int jiCloseServer();
extern WNN_ENV *jiCreateEnv();
extern int jiDeleteEnv();
#endif

#endif
