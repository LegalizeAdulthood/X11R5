/*
 *	convlib2.c -- X11 $B%D!<%k%-%C%HJQ49F~NOMQ%i%$%V%i%j(J
 *
 *		ishisone@sra.co.jp
 */

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

/*
 * --$B%$%s%?!<%U%'%$%9(J--
 *
 * $BMQ0U$5$l$F$$$k%U%!%s%/%7%g%s$O<!$N(J4$B$D(J ($B$=$N$&$A(J1$B$D$O(J backward
 * compatibility $B$N$?$a(J) $B$@$1$G$"$k!#;DG0$J$,$i(J X11R5 $B$G:N$jF~$l$i$l$kI8(J
 * $B=`F~NO%$%s%?!<%U%'%$%9(J XIM $B$K$O=>$C$F$$$J$$!#$7$+$7(J Xt $B%D!<%k%-%C%H$r(J
 * $BMxMQ$9$k%W%m%0%i%`$J$i(J XIM $B$h$j$b$O$k$+$K4JC1$KAH$_9~$`$3$H$,$G$-$k$@(J
 * $B$m$&!#(J
 *
 * void _beginConversionWithAttributes(
 *	Widget w,		$BJQ49F~NO$r$7$?$$(J widget
 *	Atom catom,		$B%;%l%/%7%g%s%"%H%`(J eg "_JAPANESE_CONVERSION"
 *	Atom tatom,		$BJQ49%F%-%9%H%?%$%W(J eg "COMPOUND_TEXT"
 *	void (*inputproc)(),	$BJQ49%F%-%9%HF~NO%3!<%k%P%C%/4X?t(J
 *	void (*startendproc)(),	$BJQ493+;O(J / $B=*N;(J / $B%"%\!<%H(J $B%3!<%k%P%C%/4X?t(J
 *	XtPointer client_data,	$B%3!<%k%P%C%/4X?t$KEO$5$l$k%G!<%?(J
 *	ArgList attrs,		$BJQ49B0@-%j%9%H(J
 *	Cardinal nattrs		$BB0@-%j%9%H$N9`L\?t(J
 *	)
 *
 *	$BJQ49$r3+;O$9$k!#$b$C$H@53N$K$$$&$H!"0z?t(J catom $B$K;XDj$5$l$?%;%l(J
 *	$B%/%7%g%s%"%H%`$N%*!<%J!<(J ($B$3$l$,JQ49%U%m%s%H%(%s%I!"Nc$($P(J
 *	kinput2 $B$G$"$k(J) $B$rC5$7!"JQ49$N%j%/%(%9%H$rAw$k!#F1;~$K0z?t(J attrs
 *	$B$G;XDj$5$l$kJQ49B0@-(J ($BNc$($P%+!<%=%k$N0LCV(J) $B$r%U%m%s%H%(%s%I$KDL(J
 *	$BCN$9$k!#JQ49B0@-$K$D$$$F$O8e$GJL$K@bL@$9$k!#(J
 *
 *	startendproc $B$OJQ49$N>uBV$NJQ2=$r%"%W%j%1!<%7%g%s$KCN$i$;$k$?$a(J
 *	$B$N%3!<%k%P%C%/$G$"$k!#$3$N%U%!%s%/%7%g%s$O<!$N$h$&$J7A<0$G8F$S=P(J
 *	$B$5$l$k!#(J
 *
 *	(*startendproc)(Widget w, Atom catom, int state, XtPointer client_data)
 *
 *	w, catom, client_data $B$O(J _beginConversionWithAttributes() $B$G;XDj(J
 *	$B$7$?$b$N$HF1$8$b$N$,EO$5$l$k!#(Jstate $B$K$O(J3$B<oN`$"$j!"$=$l$>$l(J
 *		 0: $B%U%m%s%H%(%s%I$,JQ49%j%/%(%9%H$r<u$1IU$1$?(J
 *		-1: $B%U%m%s%H%(%s%I$,JQ49%j%/%(%9%H$r5qH]$7$?(J
 *		 1: $BJQ49$,=*N;$7$?(J
 *	$B$H$$$&$3$H$rI=$9!#(J
 *
 *	$B:F$S(J _beginConversionWithAttributes() $B$N0z?t$N@bL@$KLa$C$F(Jtatom 
 *	$B$O%U%m%s%H%(%s%I$+$iAw$i$l$F$/$kJ8;zNs$N%(%s%3!<%G%#%s%0$N;XDj$G(J
 *	$B$"$k!#(J
 *	kinput2 $B%W%m%H%3%k$G$O!"%U%m%s%H%(%s%I$KBP$7$F(J COMPOUND_TEXT$B%(%s(J
 *	$B%3!<%G%#%s%0$r%5%]!<%H$9$k$3$H$7$+5a$a$F$$$J$$$N$G!"B>$N%(%s%3!<(J
 *	$B%G%#%s%0$r%5%]!<%H$9$k$+$I$&$+$O%U%m%s%H%(%s%I$N%$%s%W%j%a%s%F!<(J
 *	$B%7%g%s$K0MB8$9$k!#=>$C$F(J COMPOUNT_TEXT $B0J30$N%(%s%3!<%G%#%s%0$r(J
 *	$B;XDj$9$k$H!"<B:]$KAw$i$l$F$/$kJ8;zNs$N%(%s%3!<%G%#%s%0$,;XDj$7$?(J
 *	$B$b$N$H0[$J$k$3$H$b$"$jF@$k$N$GCm0U$9$k$3$H!#(J
 *
 *	$B3NDjJ8;zNs$,JQ49%U%m%s%H%(%s%I$+$iAw$i$l$F$/$k$H(J inputproc $B$K;X(J
 *	$BDj$5$l$?%3!<%k%P%C%/%U%!%s%/%7%g%s$,<!$N$h$&$J7A<0$G8F$P$l$k!#(J
 *
 *	(*inputproc)(Widget w, Atom catom,
 *			Atom proptype, int propformat,
 *			unsigned long propsize, unsigned char *propvalue,
 *			XtPointer client_data)
 *
 *	w $B$H(J catom$B!"$=$l$K(J client_data $B$O(J 
 *	_beginConversionWithAttributes() $B$G;XDj$7$?$b$N$G$"$k!#(J
 *
 *	proptype $B$OJ8;zNs$N%(%s%3!<%G%#%s%0!"(Jpropformat $B$O(J1$BJ8;z$N%S%C%H(J
 *	$BD9!"(Jpropsize $B$,D9$5!"$=$7$F(J propvalue $B$K<B:]$NJ8;zNs%G!<%?$,F~$C(J
 *	$B$F$$$k!#<B$OJ8;zNs$O%U%m%s%H%(%s%I$+$i(J X $B$N%&%#%s%I%&%W%m%Q%F%#(J
 *	$B$H$7$FEO$5$l$F$*$j!"$3$l$i$N%Q%i%a!<%?$O$=$N%W%m%Q%F%#$N%Q%i%a!<(J
 *	$B%?$=$N$b$N$G$"$k!#=>$C$F(J XGetWindowProperty() $B$N@bL@$r;2>H$9$k$H(J
 *	$B3F%Q%i%a!<%?$N0UL#$,$O$C$-$j$o$+$k$@$m$&!#(J
 *
 * void _changeConversionAttributes(
 *	Widget	w,
 *	Atom catom,		$B%;%l%/%7%g%s%"%H%`(J eg "_JAPANESE_CONVERSION"
 *	ArgList attrs,		$BJQ49B0@-%j%9%H(J
 *	Cardinal nattrs		$BB0@-%j%9%H$N9`L\?t(J
 *	)
 *
 *	$BJQ49Cf$KJQ49B0@-$rJQ2=$5$;$k!#Nc$($P%+!<%=%k0LCV$,JQ$o$C$?$H$-$K(J
 *	$B$3$N%U%!%s%/%7%g%s$G$=$l$r%U%m%s%H%(%s%I$KCN$i$;$k$3$H$,$G$-$k!#(J
 *
 * void _endConversion(
 *	Widget w,
 *	Atom catom,		$B%;%l%/%7%g%s%"%H%`(J eg "_JAPANESE_CONVERSION"
 *	Boolean throwaway	$B$3$N8eMh$?JQ497k2L$r<u$1$H$k$+$I$&$+(J
 *	)
 *
 *	$BJQ49$r=*N;$5$;$k!#DL>o!"JQ49$N=*N;$OJQ49%U%m%s%H%(%s%I$,9T$J$&$N(J
 *	$B$GFC$K%"%W%j%1!<%7%g%s$+$i$3$N%U%!%s%/%7%g%s$r;H$C$F6/@)E*$K=*N;(J
 *	$B$5$;$kI,MW$O$J$$!#(J
 *	$B0z?t(J throwaway $B$,(J True $B$@$H$3$N%U%!%s%/%7%g%s$r<B9T$7$?8e$K%U%m(J
 *	$B%s%H%(%s%I$+$iAw$i$l$?J8;zNs$rL5;k$9$k!#(J
 *
 * void _beginConversion(	-- provided for backward compatibility
 *	Widgete w,		$BJQ49F~NO$r$7$?$$(J widget
 *	Atom catom,		$B%;%l%/%7%g%s%"%H%`(J eg "_JAPANESE_CONVERSION"
 *	Atom tatom,		$BJQ49%F%-%9%H%?%$%W(J eg "COMPOUND_TEXT"
 *	void (*inputproc)(),	$BJQ49%F%-%9%HF~NO%3!<%k%P%C%/4X?t(J
 *	XtPointer client_data	$B%3!<%k%P%C%/4X?t$KEO$5$l$k%G!<%?(J
 *	)
 *
 *	$B$3$l$O(J backward compatibility $B$N$?$a$KMQ0U$5$l$?%U%!%s%/%7%g%s$G(J
 *	$B$"$k!#$3$N%U%!%s%/%7%g%s$G$OJQ49B0@-$N;XDj$,0l@Z$G$-$J$$$7!"JQ49(J
 *	$B>uBV$NJQ2=$bCN$k$3$H$,$G$-$J$$$N$G!"$G$-$k$@$1(J
 *	_beginConversionWithAttributes() $B$rMQ$$$k$N$,K>$^$7$$!#(J
 *
 *
 * --$B%;%l%/%7%g%s%"%H%`(J--
 *
 * _beginConversionWithAttributes() $B$J$I$K;XDj$9$k%;%l%/%7%g%s%"%H%`$O(J
 * $BF~NO$7$?$$8@8l$K$h$C$F0[$J$j!"<!$N$h$&$JL>A0$K$J$C$F$$$k!#(J
 *	"_<$B8@8lL>(J>_CONVERSION"
 * $BNc$($PF|K\8l$N>l9g$O(J "_JAPANESE_CONVERSION" $B$G$"$k!#(J
 *
 *
 * --$BJQ49B0@-%j%9%H(J--
 *
 * $BJQ49B0@-$O(J XtSetValues() $B$J$I$G;HMQ$5$l$k%j%9%H$HF1$87A(J (ArgList) $B$G$"$k!#(J
 * $BB0@-$H$7$F;XDj$G$-$k$N$O<!$N9`L\$G$"$k!#4pK\E*$K$3$l$i$NB0@-$O(J XIM $B$N(J
 * $B;EMM$G$NDj5A$r$=$N$^$^:NMQ$7$F$$$k$N$G!"5?LdE@$,$"$l$P(J XIM $B$N%I%-%e%a%s%H$r(J
 * $B;2>H$7$F$[$7$$!#(J
 *
 * "inputStyle" : $BCM(J String
 *	$BF~NO%9%?%$%k$r;XDj$9$k!#CM$O(J
 *		"root" (root window style):	$BJL%&%#%s%I%&$K$h$kJQ49(J
 *		"off" (off-the-spot style):	$B;XDj$7$?JQ49NN0hFb$G$NJQ49(J
 *		"over" (over-the-spot style):	$B$=$N>lJQ49(J
 *	$B$N$I$l$+$NJ8;zNs$r;XDj$9$k!#(J
 *
 * "focusWindow" : $BCM(J Window
 *	$BJQ49$r9T$J$&%&%#%s%I%&$r;XDj$9$k!#$3$l$,;XDj$5$l$?$J$+$C$?>l9g$K(J
 *	$B$O(J _beginConversionWithAttributes() $B$G;XDj$7$?(J Widget $B$N%&%#%s%I(J
 *	$B%&$,;H$o$l$k$N$GDL>o$O;XDj$7$J$/$F$h$$!#(J
 *
 * "spotX", "spotY" : $BCM(J Position
 *	$B%9%]%C%H%m%1!<%7%g%s$r;XDj$9$k!#$3$l$OF~NO%9%?%$%k$,(J 
 *	over-the-spot $B$N;~$N$_M-8z$G$"$k!#J8;z$r=q$-;O$a$k0LCV$r;XDj$9$k(J
 *	$B$,!"(JspotY $B$O%Y!<%9%i%$%s$N0LCV$G$"$k$3$H$KCm0U!#(J
 *	spotX$B!"(JspotY $B$N$&$AJRJ}$@$1;XDj$7$F$bL58z!#(J
 *
 * "foreground", "background" : $BCM(J Pixel
 *	$BA07J?'!"GX7J?'$r;XDj$9$k!#$3$l$bJRJ}$@$1;XDj$7$F$bL58z!#(J
 *
 * "eventCaptureMethod" : $BCM(J String
 *	$B%U%m%s%H%(%s%I$,$I$N$h$&$KF~NO%$%Y%s%H$r$H$k$+$r;XDj$9$k!#(J
 *		"none"		$B2?$b$7$J$$(J
 *		"inputOnly"	InputOnly $B%&%#%s%I%&$K$h$k(J
 *		"focusSelect"	$B%U%)!<%+%9%&%#%s%I%&$N%$%Y%s%H$rD>@\(J
 *				$B%;%l%/%H$9$k(J
 *	$B$G$"$k!#(J
 *
 *	"$B2?$b$7$J$$(J" $B$r;XDj$7$?>l9g!"%"%W%j%1!<%7%g%s$OJQ49Cf$K%"%W%j%1!<(J
 *	$B%7%g%s$KMh$?%$%Y%s%H$r%U%m%s%H%(%s%I$KEO$7$F(J (XSendEvent() $B$r;H(J
 *	$B$&(J) $B$d$i$J$1$l$P$J$i$J$$!#$3$N%$%Y%s%H$N%U%)%o!<%I:n6H$O$3$N%i%$(J
 *	$B%V%i%j$G$O%5%]!<%H$7$F$$$J$$!#=>$C$F(J "$B2?$b$7$J$$(J" $B$r;XDj$9$k$3$H(J
 *	$B$O$"$^$j$*4+$a$7$J$$!#(J
 *
 *	"$B%U%)!<%+%9%&%#%s%I%&$N%$%Y%s%H$rD>@\%;%l%/%H$9$k(J" $B>l9g!"%"%W%j(J
 *	$B%1!<%7%g%s$OJQ49Cf$O(J sendevent $B%U%i%0$NN)$C$F$$$J$$%-!<%$%Y%s%H(J
 *	$B$rL5;k$7$J$/$F$O$J$i$J$$!#(Jsendevent $B%U%i%0$NN)$C$?%-!<%$%Y%s%H$O(J
 *	$B%U%m%s%H%(%s%I$+$iLa$5$l$?%$%Y%s%H$G$"$k2DG=@-$,$"$j!"$3$l$OL5;k(J
 *	$B$7$J$/$F$bNI$$$,!"EvA3$N$3$H$J$,$i%;%-%e%j%F%#$K$O5$$r$D$1$J$/$F(J
 *	$B$O$J$i$J$$!#(J
 *
 *	"InputOnly $B%&%#%s%I%&$K$h$k(J" $B$r;XDj$9$k$H%U%m%s%H%(%s%I$O%/%i%$(J
 *	$B%"%s%H$N%&%#%s%I%&(J ($B$3$l$O%U%)!<%+%9%&%#%s%I%&$G$O$J$/!"(J
 *	_beginConversionWithAttributes() $B$G;XDj$7$?(J Widget $B$N%&%#%s%I%&(J
 *	$B$G$"$k(J) $B$NA0$KF)L@$J%&%#%s%I%&$r:n$j!"$=$3$KMh$?%$%Y%s%H$r2#<h$j(J
 *	$B$9$k!#$3$N>l9g%$%Y%s%H$O%U%m%s%H%(%s%I$,>!<j$K$H$C$F$7$^$&$N$G%"(J
 *	$B%W%j%1!<%7%g%s$O%$%Y%s%H$K4X$7$F2?$b9M$($J$/$F$h$$!#=>$C$FJ}K!$H(J
 *	$B$7$F$O$3$l$,0lHV4JC1$G$"$k!#$7$+$7$J$,$iNc$($P(J click-to-type $B7A(J
 *	$B<0$N%&%#%s%I%&%^%M!<%8%c$r;H$C$?$j$7$F%-!<F~NO$N%U%)!<%+%9$,@_Dj(J
 *	$B$5$l$F$$$k>l9g$K$O$&$^$/$$$+$J$$!#(J
 *
 * "lineSpacing" : $BCM(J int
 *	$B9T4V3V$r;XDj$9$k!#%Y!<%9%i%$%s4V$N5wN%$r;XDj$9$k!#(J
 *
 * "clientArea" : $BCM(J XRectangle $B$X$N%]%$%s%?(J
 *	$BJQ49%F%-%9%H$NI=<($K;HMQ$9$kNN0h$r;XDj$9$k!#(J
 *
 * "statusArea" : $BCM(J XRectangle $B$X$N%]%$%s%?(J
 *	$B%b!<%II=<($K;HMQ$9$kNN0h$r;XDj$9$k!#(J
 *
 * "cursor" : $BCM(J Cursor
 *	$B;HMQ$9$k%+!<%=%k(J ($B%^%&%9%+!<%=%k$M(J) $B$r;XDj$9$k!#(J
 *
 * "fonts" : $BCM(J NULL $B%?!<%_%M!<%H$5$l$?(J XFontStruct * $B$NG[Ns(J
 *	$B;HMQ$9$k%U%)%s%H$r;XDj$9$k!#=gHV$O$I$&$G$b$h$$!#%U%m%s%H%(%s%IB&(J
 *	$B$GH=CG$9$k!#$?$@$7(J XLFD $B$K=>$o$J$$%U%)%s%H$r;XDj$5$l$k$H!"%U%m%s(J
 *	$B%H%(%s%I$G$=$N%-%c%i%/%?%;%C%H$,$o$+$i$:!"$=$N%U%)%s%H$,;H$o$l$J(J
 *	$B$$$3$H$,$"$k!#(J
 *
 * $B>e5-$NB0@-$N$&$A!"(JinputStyle $B$H(J eventCaptureMethod $B$OJQ49ESCf$G(J ($B$D$^$j(J
 * _changeConversionAttributes() $B$G(J) $BJQ99$9$k$3$H$O$G$-$J$$!#(J
 */

#ifndef lint
static char	*rcsid = "$Id: convlib2.c,v 1.8 1991/10/01 07:43:13 ishisone Rel $";
#endif

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "ConvProto.h"

typedef struct {
    Display	*display;
    Atom	profileAtom;	/* "_CONVERSION_PROFILE" */
    Atom	typeAtom;	/* "_CONVERSION_ATTRIBUTE_TYPE" */
    Atom	versionAtom;	/* "PROTOCOL-2.0" */
    Atom	reqAtom;	/* "CONVERSION_REQUEST" */
    Atom	notifyAtom;	/* "CONVERSION_NOTIFY" */
    Atom	endAtom;	/* "CONVERSION_END" */
    Atom	endReqAtom;	/* "CONVERSION_END_REQUEST" */
    Atom	attrAtom;	/* "CONVERSION_ATTRIBUTE" */
    Atom	attrNotifyAtom;	/* "CONVERSION_ATTRIBUTE_NOTIFY" */
} ConversionAtoms;

typedef struct {
    Atom	convatom;
    Window	convowner;
    Atom	property;
    void	(*inputproc)();
    void	(*startendproc)();
    XtPointer	closure;
} ConversionContext;

static XContext	convertPrivContext;

#if __STDC__
/* function prototype */
static void callStart(Widget, ConversionContext *);
static void callFail(Widget, ConversionContext *);
static void callEnd(Widget, ConversionContext *);
static long getInputStyle(String);
static long getCaptureMethod(String);
static ConversionAtoms *getAtoms(Widget);
static ConversionContext *getConversionContext(Widget);
static void recvConvAck(Widget, XtPointer, XEvent *, Boolean *);
static void getConv(Widget, XtPointer, XEvent *, Boolean *);
static Boolean setConvAttrProp(Widget, ArgList, Cardinal, Atom);
static int makeAttrData(Widget, ArgList, Cardinal, unsigned long **);
static Boolean checkProtocols(Display *, Window, ConversionAtoms *);
#else
static void callStart();
static void callFail();
static void callEnd();
static long getInputStyle();
static long getCaptureMethod();
static ConversionAtoms *getAtoms();
static ConversionContext *getConversionContext();
static void recvConvAck();
static void getConv();
static Boolean setConvAttrProp();
static int makeAttrData();
static Boolean checkProtocols();
#endif

static void
callStart(w, context)
Widget w;
ConversionContext *context;
{
    if (context->startendproc != NULL) {
	(*context->startendproc)(w, context->convatom,
				 0, context->closure);
    }
}

static void
callFail(w, context)
Widget w;
ConversionContext *context;
{
    if (context->startendproc != NULL) {
	(*context->startendproc)(w, context->convatom,
				 -1, context->closure);
    }
}

static void
callEnd(w, context)
Widget w;
ConversionContext *context;
{
    if (context->startendproc != NULL) {
	(*context->startendproc)(w, context->convatom,
				 1, context->closure);
    }
}

static long
getInputStyle(s)
String s;
{
    String p;
    char buf[64];

    (void)strcpy(buf, s);
    for (p = buf; *p != '\0'; p++) {
	if ('A' <= *p && *p <= 'Z') *p += 'a' - 'A';
    }
    if (!strcmp(buf, "over")) return CONVARG_OVERTHESPOT;
    if (!strcmp(buf, "off")) return CONVARG_OFFTHESPOT;
    return CONVARG_ROOTWINDOW;
}

static long
getCaptureMethod(s)
String s;
{
    String p;
    char buf[64];

    (void)strcpy(buf, s);
    for (p = buf; *p != '\0'; p++) {
	if ('A' <= *p && *p <= 'Z') *p += 'a' - 'A';
    }
    if (!strcmp(buf, "none")) return CONVARG_NONE;
    if (!strcmp(buf, "focusselect")) return CONVARG_SELECT_FOCUS_WINDOW;
    return CONVARG_CREATE_INPUTONLY;
}

static ConversionAtoms *
getAtoms(w)
Widget	w;
{
    int i;
    Display *disp = XtDisplay(w);
    ConversionAtoms *cap;
    static ConversionAtoms *convatomp;
    static Cardinal ndisp = 0;
#define nalloc	2

    /*
     * $B%"%H%`$O%G%#%9%W%l%$$4$H$K0c$&$N$G!"(J
     * $B%G%#%9%W%l%$$4$H$K:n$i$J$/$F$O$J$i$J$$(J
     */

    /* $B$9$G$K%"%H%`$,:n$i$l$F$$$k$+$I$&$+D4$Y$k(J */
    cap = convatomp;
    for (i = 0; i < ndisp; i++, cap++) {
	if (cap->display == disp) return cap;
    }

    /*
     * $B$^$@:n$i$l$F$$$J$$$N$G?7$7$/:n$k(J
     */
    if (ndisp == 0) {
	/* $B:G=i$J$N$G(J Context $B$bF1;~$K:n$k(J */
	convertPrivContext = XUniqueContext();
	convatomp = (ConversionAtoms *)
	  XtMalloc(sizeof(ConversionAtoms) * nalloc);
	cap = convatomp;
    } else if (ndisp % nalloc == 0) {
	/* $B%5%$%:$rA}$d$9(J */
	convatomp = (ConversionAtoms *)
	  XtRealloc((char *)convatomp,
		    sizeof(ConversionAtoms) * (ndisp + nalloc));
	cap = convatomp + ndisp;
    } else {
	cap = convatomp + ndisp;
    }

    /* $B%G%#%9%W%l%$$NEPO?(J */
    cap->display = disp;

    /* Atom $B$N:n@.(J */
    cap->profileAtom = XInternAtom(disp, CONVERSION_PROFILE, False);
    cap->typeAtom = XInternAtom(disp, CONVERSION_ATTRIBUTE_TYPE, False);
    cap->versionAtom = XInternAtom(disp, PROTOCOL_VERSION, False);
    cap->reqAtom = XInternAtom(disp, "CONVERSION_REQUEST", False);
    cap->notifyAtom = XInternAtom(disp, "CONVERSION_NOTIFY", False);
    cap->endAtom = XInternAtom(disp, "CONVERSION_END", False);
    cap->endReqAtom = XInternAtom(disp, "CONVERSION_END_REQUEST", False);
    cap->attrAtom = XInternAtom(disp, "CONVERSION_ATTRIBUTE", False);
    cap->attrNotifyAtom = XInternAtom(disp, "CONVERSION_ATTRIBUTE_NOTIFY", False);

    ndisp++;

    return cap;
}

static ConversionContext *
getConversionContext(w)
Widget	w;
{
    ConversionContext *context;

    if (XFindContext(XtDisplay(w), XtWindow(w),
		     convertPrivContext, (caddr_t *)&context)) {
	/* error -- $BB?J,%3%s%F%-%9%H$,8+$D$+$i$J$+$C$?$?$a(J */
	return NULL;
    } else {
	return context;
    }
}

/* ARGSUSED */
static void
recvConvAck(w, closure, ev, junk)
Widget	w;
XtPointer	closure;
XEvent	*ev;
Boolean	*junk;	/* NOTUSED */
{
    XClientMessageEvent *cev = &(ev->xclient);
    ConversionAtoms *cap;
    ConversionContext *context;

    if (ev->type != ClientMessage) return;

    cap = getAtoms(w);
    context = getConversionContext(w);

    /* $B@5$7$$%$%Y%s%H$+$I$&$+%A%'%C%/$9$k(J */
    if (cev->window != XtWindow(w) ||
	cev->message_type != cap->notifyAtom ||
	cev->data.l[0] != context->convatom) {
	return;
    }

    /*
     * $B$3$N%O%s%I%i$O$b$&MQ:Q$_$J$N$G30$9(J
     */
    XtRemoveEventHandler(w, NoEventMask, True, recvConvAck, closure);

    if (cev->data.l[2] == None) {
	XtWarning("selection request failed");
	XDeleteContext(XtDisplay(w), XtWindow(w), convertPrivContext);
	callFail(w, context);
	XtFree((char *)context);
	return;
    }

    callStart(w, context);

    /*
     * PropertyNotify $B$H(J CONVERSION_END $BMQ$N%$%Y%s%H%O%s%I%i$r(J
     * $BEPO?$9$k(J
     */
    XtAddEventHandler(w, PropertyChangeMask, True, getConv, closure);

    /* $B%W%m%Q%F%#L>$r%9%H%"$9$k(J */
    context->property = cev->data.l[2];
}

/* ARGSUSED */
static void
getConv(w, closure, ev, junk)
Widget	w;
XtPointer	closure;
XEvent	*ev;
Boolean	*junk;	/* NOTUSED */
{
    ConversionAtoms *cap;
    ConversionContext *context;

    /* PropertyNotify $B$H(J ClientMessage $B0J30$OL5;k$9$k(J */
    if (ev->type != PropertyNotify && ev->type != ClientMessage) return;

    cap = getAtoms(w);
    context = getConversionContext(w);

    if (ev->type == ClientMessage) {
	XClientMessageEvent *cev = &(ev->xclient);

	/*
	 * $BK\Ev$KF~NO=*N;$N%$%Y%s%H$+$I$&$+%A%'%C%/$9$k(J
	 */
	if (cev->message_type == cap->endAtom &&
	    cev->format == 32 &&
	    cev->data.l[0] == context->convatom) {
	    /* $B%&%#%s%I%&$N%3%s%F%-%9%H$r:o=|$9$k(J */
	    XDeleteContext(XtDisplay(w), XtWindow(w),
			   convertPrivContext);
	    /* $B%$%Y%s%H%O%s%I%i$r30$9(J */
	    XtRemoveEventHandler(w, PropertyChangeMask, True,
				 getConv, closure);
	    callEnd(w, context);
	    XtFree((char *)context);
	}
    } else {			/* PropertyNotify */
	XPropertyEvent *pev = &(ev->xproperty);
	Atom proptype;
	int propformat;
	unsigned long propsize, rest;
	unsigned char *propvalue;

	if (context->property == None) return;

	/* $B@5$7$$%$%Y%s%H$+$I$&$+$N%A%'%C%/(J */
	if (pev->window != XtWindow(w) ||
	    pev->atom != context->property ||
	    pev->state != PropertyNewValue) {
	    return;
	}

	/* $B$b$7%3!<%k%P%C%/4X?t(J context->inputproc $B$,(J
	 * NULL $B$J$i$P%W%m%Q%F%#$r:o=|$9$k$@$1(J
	 */
	if (context->inputproc == NULL) {
	    XDeleteProperty(XtDisplay(w), XtWindow(w), context->property);
	    return;
	}

	/* $B%W%m%Q%F%#$+$iJQ49J8;zNs$r<h$j=P$9(J */
	XGetWindowProperty(XtDisplay(w), XtWindow(w),
			   context->property,
			   0L, 100000L, True, AnyPropertyType,
			   &proptype, &propformat, &propsize, &rest,
			   &propvalue);

	/* $B%W%m%Q%F%#$N%?%$%W!&%U%)!<%^%C%H$N%A%'%C%/(J */
	if (proptype == None) {
	    /* $B%W%m%Q%F%#$,B8:_$7$J$+$C$?(J
	     * $B$3$l$OO"B3$7$F2?2s$b%W%m%Q%F%#$K%G!<%?$,(J
	     * $BF~$l$i$l$?;~!"0l2s$N(J GetWindowProperty $B$G(J
	     * $BJ#?t$N%G!<%?$r$H$C$F$7$^$C$?$"$H$K5/$-$k(J
	     * $B=>$C$F$3$l$O%(%i!<$G$O$J$$(J
	     */
	    return;
	}

	/* $B%3!<%k%P%C%/$r8F$V(J */
	(*context->inputproc)(w, context->convatom,
			      proptype, propformat,
			      propsize, propvalue,
			      context->closure);
    }
}

static Boolean
setConvAttrProp(w, attrs, nattrs, prop)
Widget w;
ArgList attrs;
Cardinal nattrs;
Atom prop;
{
    unsigned long *data;
    int len;

    if ((len = makeAttrData(w, attrs, nattrs, &data)) > 0) {
	XChangeProperty(XtDisplay(w), XtWindow(w),
			prop, prop, 32,
			PropModeReplace, (unsigned char *)data, len);
	XtFree((char *)data);
	return True;
    }
    return False;
}

static int
makeAttrData(w, args, nargs, datap)
Widget w;
ArgList args;
Cardinal nargs;
unsigned long **datap;
{
    unsigned long *data;
    Cardinal len;
    Boolean spotx_specified = False, spoty_specified = False;
    Boolean fore_specified = False, back_specified = False;
    Pixel savedfg, savedbg;
    Position savedx, savedy;

#define ALLOC(n) data = (unsigned long *)XtRealloc((char *)data, 4*(len+(n)))

    data = NULL;
    len = 0;
    while (nargs-- > 0) {
	if (!strcmp(args->name, "spotX")) {
	    savedx = (Position)args->value;
	    spotx_specified = True;
	} else if (!strcmp(args->name, "spotY")) {
	    savedy = (Position)args->value;
	    spoty_specified = True;
	} else if (!strcmp(args->name, "foreground")) {
	    savedfg = (Pixel)args->value;
	    fore_specified = True;
	} else if (!strcmp(args->name, "background")) {
	    savedbg = (Pixel)args->value;
	    back_specified = True;
	} else if (!strcmp(args->name, "focusWindow")) {
	    Window win = (Window)args->value;
	    ALLOC(2);
	    data[len] = CONV_ATTR(CONVATTR_FOCUS_WINDOW, 1);
	    data[len + 1] = (unsigned long)win;
	    len += 2;
	} else if (!strcmp(args->name, "inputStyle")) {
	    long style = getInputStyle((String)args->value);
	    ALLOC(2);
	    data[len] = CONV_ATTR(CONVATTR_INPUT_STYLE, 1);
	    data[len + 1] = style;
	    len += 2;
	} else if (!strcmp(args->name, "eventCaptureMethod")) {
	    long method = getCaptureMethod((String)args->value);
	    ALLOC(2);
	    data[len] = CONV_ATTR(CONVATTR_EVENT_CAPTURE_METHOD, 1);
	    data[len + 1] = method;
	    len += 2;
	} else if (!strcmp(args->name, "lineSpacing")) {
	    int spacing = (int)args->value;
	    ALLOC(2);
	    data[len] = CONV_ATTR(CONVATTR_LINE_SPACING, 1);
	    data[len + 1] = spacing;
	    len += 2;
	} else if (!strcmp(args->name, "clientArea")) {
	    XRectangle *rectp = (XRectangle *)args->value;
	    ALLOC(3);
	    data[len] = CONV_ATTR(CONVATTR_CLIENT_AREA, 2);
	    data[len + 1] = (rectp->x << 16) | (rectp->y & 0xffff);
	    data[len + 2] = (rectp->width << 16) | (rectp->height & 0xffff);
	    len += 3;
	} else if (!strcmp(args->name, "statusArea")) {
	    XRectangle *rectp = (XRectangle *)args->value;
	    ALLOC(3);
	    data[len] = CONV_ATTR(CONVATTR_STATUS_AREA, 2);
	    data[len + 1] = (rectp->x << 16) | (rectp->y & 0xffff);
	    data[len + 2] = (rectp->width << 16) | (rectp->height & 0xffff);
	    len += 3;
	} else if (!strcmp(args->name, "cursor")) {
	    Cursor cursor = (Cursor)args->value;
	    ALLOC(2);
	    data[len] = CONV_ATTR(CONVATTR_CURSOR, 1);
	    data[len + 1] = cursor;
	    len += 2;
	} else if (!strcmp(args->name, "fonts")) {
	    XFontStruct **fontp = (XFontStruct **)args->value;
	    int nfonts, nrealfonts;
	    int i;

	    for (nfonts = 0; fontp[nfonts] != NULL; nfonts++)
	        ;
	    ALLOC(nfonts + 1);
	    nrealfonts = 0;
	    for (i = 0; i < nfonts; i++) {
		unsigned long atom;
		if (XGetFontProperty(fontp[i], XA_FONT, &atom)) {
		    data[len + ++nrealfonts] = atom;
		}
	    }
	    data[len] = CONV_ATTR(CONVATTR_FONT_ATOMS, nrealfonts);
	    len += nrealfonts + 1;
	} else {
	    String params[1];
	    Cardinal num_params;

	    params[0] = args->name;
	    XtAppWarningMsg(XtWidgetToApplicationContext(w),
			    "conversionError", "invalidResource",
			    "ConversionLibraryError",
			    "_beginConversionWithAttributes: unknown resource %s",
			    params, &num_params);
	}
	args++;
    }
    if (spotx_specified && spoty_specified) {
	ALLOC(2);
	data[len] = CONV_ATTR(CONVATTR_SPOT_LOCATION, 1);
	data[len + 1] = (savedx << 16) | (savedy & 0xffff);
	len += 2;
    }
    if (fore_specified && back_specified) {
	ALLOC(3);
	data[len] = CONV_ATTR(CONVATTR_COLOR, 2);
	data[len + 1] = savedfg;
	data[len + 2] = savedbg;
	len += 3;
    }
    *datap = data;
    return len;
#undef ALLOC
}

static Boolean
checkProtocols(dpy, window, cap)
Display *dpy;
Window window;
ConversionAtoms *cap;
{
    Atom type;
    int format;
    unsigned long nitems;
    unsigned long bytesafter;
    unsigned long *data, *saveddata;
    int err;
    Boolean ret;

    data = NULL;
    err = XGetWindowProperty(dpy, window, cap->profileAtom,
			     0L, 100L, False,
			     cap->typeAtom,
			     &type, &format, &nitems,
			     &bytesafter, (unsigned char **)&data);
    if (err) return False;
    if (format != 32 || type != cap->typeAtom) {
	if (data != NULL) XtFree((char *)data);
	return False;
    }

    ret = False;
    saveddata = data;
    while (nitems > 0) {
	int code = CODE_OF_ATTR(*data);
	int len = LENGTH_OF_ATTR(*data);

	data++;
	nitems--;
	if (nitems < len) break;

	switch (code) {
	case CONVPROF_PROTOCOL_VERSION:
	    if (*data == cap->versionAtom) ret = True;
	    break;
	case CONVPROF_SUPPORTED_STYLES:
	    break;	/* XXX for now */
	default:
	    break;
	}
	data += len;
	nitems -= len;
    }
    XtFree((char *)saveddata);

    return ret;
}


/*
 *	public functions
 */

void
_beginConversionWithAttributes(w, catom, tatom, inputproc, startendproc, client_data, attrs, nattrs)
Widget w;
Atom catom;		/* Selection Atom e.g. JAPANESE_CONVERSION */
Atom tatom;		/* Property Type Atom e.g. COMPOUND_TEXT */
void (*inputproc)();	/* conversion text callback function */
void (*startendproc)();	/* conversion start/end callback function */
XtPointer client_data;	/* client_data passed to callback function */
ArgList attrs;		/* attribute data */
Cardinal nattrs;	/* number of attr args */
{
    Window owner;
    XEvent event;
    ConversionAtoms *cap;
    ConversionContext *context;
    Boolean anyattr;

    cap = getAtoms(w);

    /* $BJQ49%5!<%P$rC5$9(J */
    if ((owner = XGetSelectionOwner(XtDisplay(w), catom)) == None) {
	/* $B$J$$(J
	 * $B$b$7$bJQ49Cf$@$C$?$iJQ49$rCf;_$9$k(J
	 */
	XtWarning("Conversion Server not found");
	if ((context = getConversionContext(w)) != NULL) {
	    /* $B%$%Y%s%H%O%s%I%i$r30$9(J */
	    XtRemoveEventHandler(w, NoEventMask, True, recvConvAck,
				 (XtPointer)NULL);
	    XtRemoveEventHandler(w, PropertyChangeMask, True, getConv,
				 (XtPointer)NULL);
	    /* $B%&%#%s%I%&$N%3%s%F%-%9%H$r:o=|$9$k(J */
	    XDeleteContext(XtDisplay(w), XtWindow(w), convertPrivContext);
	    callEnd(w, context);
	    XtFree((char *)context);
	}
	return;
    }

    /*
     * $B:#$9$G$KJQ49Cf$+$I$&$+D4$Y$k(J
     * $BJQ49Cf$J$i2?$b$;$:$K%j%?!<%s$9$k!D$o$1$K$O$$$+$J$$(J
     * $B$J$<$+$H$$$&$H!"JQ49%5!<%P$,2?$i$+$N;v>p$GESCf$G;`$s$@>l9g(J
     * CONVERSION_END $B$,%/%i%$%"%s%H$KMh$J$$$3$H$,$"$k$+$i$G$"$k(J
     * $B$=$3$G!"JQ49Cf$N>l9g$G$b(J SelectionOwner $B$rC5$7$F!"$=$l$,(J
     * $B:G=i$K(J _beginConversion() $B$,8F$P$l$?;~$H(J WindowID $B$,F1$8$+(J
     * $B$I$&$+3NG'$9$k(J
     * $BK\Ev$O(J SelectionOwner $B$K$J$C$?;~4V$b%A%'%C%/$7$?$$$N$@$,(J
     * ICCCM $B$K=R$Y$i$l$F$$$k$h$&$K!"(JGetSelectionOwner $B$G$O(J
     * $B$=$l$,$o$+$i$J$$$N$G$"$-$i$a$k(J
     */
    if ((context = getConversionContext(w)) != NULL) {
	Window curOwner;
	curOwner = (catom == context->convatom) ? owner :
	  XGetSelectionOwner(XtDisplay(w), context->convatom);
	if (curOwner == context->convowner) {
	    /* $B2?$b$;$:$K%j%?!<%s(J */
	    return;
	}
	/* SelectionOwner $B$,JQ$o$C$F$$$k(J
	 * $B$3$l$OESCf$GJQ49%5!<%P$,%/%i%C%7%e$7$?$K0c$$$J$$(J
	 * $B$H$$$&$3$H$G(J CONVERSION_END $B$,Mh$?;~$HF1$8$h$&$J(J
	 * $B=hM}$r$9$k(J
	 */
	/* $B%$%Y%s%H%O%s%I%i$r30$9(J
	 * CONVERSION_NOTIFY $B$N%$%Y%s%H$,Mh$k$^$G$O(J
	 * recvConvAck() $B$,%O%s%I%i$G!"$=$N8e$O(J
	 * getConv() $B$,%O%s%I%i$G$"$k(J
	 */
	XtRemoveEventHandler(w, NoEventMask, True, recvConvAck,
			     (XtPointer)NULL);
	XtRemoveEventHandler(w, PropertyChangeMask, True, getConv,
			     (XtPointer)NULL);
	/* $B%&%#%s%I%&$N%3%s%F%-%9%H$r:o=|$9$k(J */
	XDeleteContext(XtDisplay(w), XtWindow(w), convertPrivContext);
	callEnd(w, context);
	XtFree((char *)context);
    }

    /*
     * $B%5!<%P$+$i$N(J CONVERSION_NOTIFY $BMQ$N%$%Y%s%H%O%s%I%i$r(J
     * $BEPO?$9$k(J
     */
    XtAddEventHandler(w, NoEventMask, True, recvConvAck, (XtPointer)NULL);

    /*
     * $B%3%s%F%-%9%H$r$D$/$C$FI,MW$J>pJs$rEPO?$9$k(J
     */
    context = XtNew(ConversionContext);
    context->convatom = catom;
    context->convowner = owner;
    context->property = None;	/* $B$3$l$O(J CONVERSION_NOTIFY $B$,Mh$?;~$K(J
				 * $B@5$7$/@_Dj$5$l$k(J */
    context->inputproc = inputproc;
    context->startendproc = startendproc;
    context->closure = client_data;
    XSaveContext(XtDisplay(w), XtWindow(w),
		 convertPrivContext, (caddr_t)context);

    /*
     * $BJQ49B0@-%j%9%H$,;XDj$5$l$F$$$l$P%W%m%Q%F%#$K$=$l$rEPO?$9$k(J
     */
    if (nattrs != 0 && attrs != NULL &&
	checkProtocols(XtDisplay(w), owner, cap)) {
	anyattr = setConvAttrProp(w, attrs, nattrs, cap->attrAtom);
    }

    /*
     * ClientMessage $B%$%Y%s%H$r;H$C$FF|K\8lF~NO$r%j%/%(%9%H$9$k(J
     */
    event.xclient.type = ClientMessage;
    event.xclient.window = owner;
    event.xclient.message_type = cap->reqAtom;
    event.xclient.format = 32;
    event.xclient.data.l[0] = catom;
    event.xclient.data.l[1] = XtWindow(w);
    event.xclient.data.l[2] = tatom;
    /* $B7k2L$r%9%H%"$9$k%W%m%Q%F%#L>$O!"B?8@8l$rF1;~$K;HMQ$9$k$3$H$r(J
     * $B9M$($F!"(Jselection atom $B$r;HMQ$9$k$3$H$K$9$k(J
     */
    event.xclient.data.l[3] = catom;
    event.xclient.data.l[4] = anyattr ? cap->attrAtom : None;
    XSendEvent(XtDisplay(w), owner, False, NoEventMask, &event);
}

/* this is provided for backward compatibility */
void
_beginConversion(w, catom, tatom, inputproc, client_data)
Widget	w;
Atom	catom;			/* Selection Atom e.g. JAPANESE_CONVERSION */
Atom	tatom;			/* Property Type Atom e.g. COMPOUND_TEXT */
void	(*inputproc)();		/* conversion text callback function */
XtPointer client_data;		/* client_data passed to callback function */
{
    _beginConversionWithAttributes(w, catom, tatom, inputproc,
				   (void (*)())NULL, client_data,
				   (ArgList)NULL, 0);
}

void
_changeConversionAttributes(w, catom, attrs, nattrs)
Widget	w;
Atom	catom;			/* Selection Atom e.g. JAPANESE_CONVERSION */
ArgList	attrs;			/* attribute data */
Cardinal nattrs;		/* number of attr args */
{
    XEvent event;
    ConversionAtoms *cap;
    ConversionContext *context;
    unsigned long *data;
    int len;

    if (attrs == NULL || nattrs == 0) return;

    cap = getAtoms(w);
    context = getConversionContext(w);

    if (context == NULL || (catom != None && catom != context->convatom)) {
	return;
    }

    if (XGetSelectionOwner(XtDisplay(w), context->convatom) !=
	context->convowner) {
	/* $BJQ49%5!<%P$,0[$J$k!"$"$k$$$O$J$$(J */
	XtRemoveEventHandler(w, NoEventMask, True, recvConvAck,
			     (XtPointer)NULL);
	XtRemoveEventHandler(w, PropertyChangeMask, True, getConv,
			     (XtPointer)NULL);
	/* $B%&%#%s%I%&$N%3%s%F%-%9%H$r:o=|$9$k(J */
	XDeleteContext(XtDisplay(w), XtWindow(w), convertPrivContext);
	callEnd(w, context);
	XtFree((char *)context);
	return;
    }

    data = NULL;
    if ((len = makeAttrData(w, attrs, nattrs, &data)) == 0) return;

    event.xclient.type = ClientMessage;
    event.xclient.window = context->convowner;
    event.xclient.message_type = cap->attrNotifyAtom;
    event.xclient.format = 32;
    event.xclient.data.l[0] = context->convatom;
    event.xclient.data.l[1] = XtWindow(w);
    if (len <= 3 && len == LENGTH_OF_ATTR(data[0]) + 1) {
	int i;
	/* $B%$%Y%s%H$NCf$K<}$^$k(J */
	for (i = 0; i < len; i++) {
	    event.xclient.data.l[2 + i] = data[i];
	}
    } else {
	XChangeProperty(XtDisplay(w), XtWindow(w),
			cap->attrAtom, cap->attrAtom, 32,
			PropModeReplace, (unsigned char *)data, len);
	event.xclient.data.l[2] = CONV_ATTR(CONVATTR_INDIRECT, 1);
	event.xclient.data.l[3] = cap->attrAtom;
    }

    XSendEvent(XtDisplay(w), context->convowner, False, NoEventMask, &event);

    if (data != NULL) XtFree((char *)data);
}

void
_endConversion(w, catom, throwaway)
Widget	w;
Atom	catom;		/* Selection Atom */
Boolean	throwaway;
{
    XEvent event;
    ConversionAtoms *cap;
    ConversionContext *context;

    cap = getAtoms(w);
    context = getConversionContext(w);

    if (context == NULL || (catom != None && catom != context->convatom)) {
	return;
    }

    if (XGetSelectionOwner(XtDisplay(w), context->convatom) !=
	context->convowner) {
	/* $BJQ49%5!<%P$,0[$J$k!"$"$k$$$O$J$$(J */
	XtRemoveEventHandler(w, NoEventMask, True, recvConvAck,
			     (XtPointer)NULL);
	XtRemoveEventHandler(w, PropertyChangeMask, True, getConv,
			     (XtPointer)NULL);
	/* $B%&%#%s%I%&$N%3%s%F%-%9%H$r:o=|$9$k(J */
	XDeleteContext(XtDisplay(w), XtWindow(w), convertPrivContext);
	/* $B%3!<%k%P%C%/$r8F$V(J */
	callEnd(w, context);
	XtFree((char *)context);
	return;
    }

    if (throwaway) context->inputproc = NULL;

    event.xclient.type = ClientMessage;
    event.xclient.window = context->convowner;
    event.xclient.message_type = cap->endReqAtom;
    event.xclient.format = 32;
    event.xclient.data.l[0] = context->convatom;
    event.xclient.data.l[1] = XtWindow(w);

    XSendEvent(XtDisplay(w), context->convowner, False, NoEventMask, &event);
}
