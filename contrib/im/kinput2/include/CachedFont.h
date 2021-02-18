/* $Id: CachedFont.h,v 1.2 1991/09/23 04:09:05 ishisone Rel $ */
/*
 * Copyright (c) 1991  Software Research Associates, Inc.
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

#ifndef _CachedFont_h
#define _CachedFont_h

/*
 * XFontStruct *CachedLoadQueryFontByName(Display *dpy, String name)
 *	XLoadQueryFont() $B$H$[$\F1$8$@$,!";XDj$5$l$?%U%)%s%H$,(J
 *	$B$9$G$K%*!<%W%s$5$l$F$$$l$P$=$l$rJV$9!#(J 
 */
extern XFontStruct *CachedLoadQueryFontByName(
#if NeedFunctionPrototypes
	Display *	/* dpy */,
	String		/* name */
#endif
);

/*
 * XFontStruct *CachedLoadQueryFontByProp(Display *dpy, Atom atom)
 *	CachedLoadQueryFontByName() $B$H$[$\F1$8$@$,!"%U%)%s%HL>$G$O$J$/!"(J
 *	$B$=$l$r%"%H%`$K$7$?$b$N(J ($B$D$^$j%U%)%s%H$N(J "FONT" $B%W%m%Q%F%#$NCM(J)
 *	$B$G;XDj$9$k!#(J
 *	$B$3$l$O$3$N%"%H%`$NCM(J ($B$*$h$S$3$N%"%H%`$KBP1~$9$kJ8;zNs$G$"$k@5<0$J(J
 *	$B%U%)%s%HL>(J) $B$,M#0l%U%)%s%H$N<BBN$KBP$7$F(J unique $B$JCM$@$H9M$($i$l$k(J
 *	$B$+$i$G$"$k!#(J
 */
extern XFontStruct *CachedLoadQueryFontByProp(
#if NeedFunctionPrototypes
	Display *	/* dpy */,
	Atom		/* atom */
#endif
);

/*
 * XFontStruct *CachedLoadFontByFontStruct(Display *dpy, XFontStruct *font)
 *	CachedFreeFont() $B$G0lEY;HMQ$7$J$/$J$C$?%U%)%s%H$r:F$S%m!<%I$9$k$N$K(J
 *	$BMQ$$$k!#$9$G$K%m!<%I$5$l$F$$$l$P$=$N%U%)%s%H$N%j%U%!%l%s%9%+%&%s%H$r(J
 *	$BA}$d$9$@$1$G$"$k!#(J
 *	$B$b$7;XDj$5$l$?(J font $B$,(J CachedLoadQueryFontByName() $B$^$?$O(J
 *	CachedLoadQueryFontByProp() $B$r;HMQ$7$F:n$i$l$?$b$N$G$O$J$$;~$K$O(J
 *	NULL $B$,JV$k!#$=$&$G$J$$;~$K$O(J font $B$NCM$,$=$N$^$^JV$5$l$k!#(J
 */
extern XFontStruct *CachedLoadFontByFontStruct(
#if NeedFunctionPrototypes
	Display *	/* dpy */,
	XFontStruct *	/* font */
#endif
);

/*
 * void CachedFreeFont(Display *dpy, XFontStruct *font)
 *	CachedLoadQueryFontByName() $B$^$?$O(J CachedLoadQueryFontByProp() $B$G(J
 *	$BF@$i$l$?%U%)%s%H$r;HMQ$7$J$/$J$C$?;~$K8F$V!#(J
 *	XFreeFont() $B$H$[$\F1$8$@$,!"%U%)%s%H>pJs$r%-%c%C%7%e$7$F$$$k(J
 *	$B4X78>e!";XDj$5$l$?%U%)%s%H$X$N;2>H$,$"$k8B$j%U%)%s%H$O(J
 *	$B%/%m!<%:$5$l$J$$!#(J
 *	$B$^$?!"%U%)%s%H>pJs(J (XFontStruct $BFb$N>pJs$N$&$A!"(Jfid $B$r=|$/$b$N(J)
 *	$B$O$?$H$(%U%)%s%H$X$N;2>H$,$J$/$J$C$F$bJ];}$5$l$k!#%U%)%s%H9=B$BN(J
 *	$B$b$=$N$^$^J];}$5$l$k$N$G!"$$$C$?$s(J CachedFreeFont() $B$7$?%U%)%s%H$r(J
 *	CachedLoadFontByFontStruct() $B$G:F$S%m!<%I$9$k$3$H$b$G$-$k!#(J
 */
extern void CachedFreeFont(
#if NeedFunctionPrototypes
	Display *	/* dpy */,
	XFontStruct *	/* font */
#endif
);

#endif
