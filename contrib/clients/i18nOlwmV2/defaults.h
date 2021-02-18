/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

/* @(#) defaults.h 50.2 90/12/12 Crucible */

/* default constants */
#define DEFAULTCOLOR    "#40A0C0"
#define CMAP            DefaultColormap(dpy,DefaultScreen(dpy))
#define DEFAULTFONT	"9x15"
#ifdef OW_I18N
#define DEFAULTFONT_LOCALE	"C"
#endif

void	SetAllDBValues();
