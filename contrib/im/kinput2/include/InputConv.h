/* $Id: InputConv.h,v 1.14 1991/09/26 00:35:08 ishisone Rel $ */
/*
 * Copyright (c) 1990  Software Research Associates, Inc.
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

#ifndef _InputConv_h
#define _InputConv_h

#include "ICtypes.h"
#include <X11/Object.h>

/*
  InputConv new resources:

  name			class		type		default		access
  ----------------------------------------------------------------------------
  selectionControl	Callback	Callback	NULL		C
  textChangeNotify	Callback	Callback	NULL		C
  modeChangeNotify	Callback	Callback	NULL		C
  endNotify		Callback	Callback	NULL		C
  fixNotify		Callback	Callback	NULL		C
  auxControl		Callback	Callback	NULL		C
  displayObjectClass	Class		Pointer		NULL		CG
*/

/* new resources */
#define XtNselectionControl "selectionControl"
#define XtNtextChangeNotify "textChangeNotify"
#define XtNmodeChangeNotify "modeChangeNotify"
#define XtNfixNotify "fixNotify"
#define XtNendNotify "endNotify"
#define XtNauxControl "auxControl"
#define XtNdisplayObjectClass "displayObjectClass"
#define XtCClass "Class"

typedef struct _InputConvClassRec *InputConvObjectClass;
typedef struct _InputConvRec *InputConvObject;

extern WidgetClass inputConvObjectClass;


/*
 * XtNselectionControl $B%3!<%k%P%C%/$N(J call_data $B9=B$BN(J
 */

typedef struct {

    int command;	/* $B%3%^%s%I(J
			 *	Start: $B9`L\A*Br3+;O%j%/%(%9%H(J
			 *	End: $B9`L\A*Br=*N;%j%/%(%9%H(J
			 *	Set: $B%+%l%s%H9`L\@_Dj%j%/%(%9%H(J
			 *	Move: $B%+%l%s%H9`L\0\F0%j%/%(%9%H(J
			 */
#define ICSelectionStart	1
#define ICSelectionEnd		2
#define ICSelectionSet		3
#define ICSelectionMove		4

    union {
	int selection_kind;	/* ICSelectionStart $B$N;~(J */
#define ICSelectionCandidates	1
#define ICSelectionSymbols	2

	int current_item;	/* ICSelectionSet/ICSelectionEnd $B$N;~(J */
				/* ICSelectionEnd $B$N>l9g$O%3!<%k%P%C%/(J
				 * $B4X?tB&$G8=:_$N%+%l%s%H8uJdHV9f$rBeF~$9$k(J
				 * ($B%+%l%s%H$,$J$1$l$P(J -1 $B$rBeF~$9$k(J)
				 */

	int dir;		/* ICSelectionMove $B$N;~(J */
    } u;

} ICSelectionControlArg;


/*
 * ICCompareSegment $B$GJV$5$l$kCM(J
 */

#define ICSame		0	/* $BA4$/F1$8(J */
#define ICAttrChanged	1	/* attr $B$N$_0[$J$k(J */
#define ICStringChanged	2	/* data $B$,0[$J$k(J */
				/* attribute $B$H(J data $B$H$b$K0[$J$l$P(J
				 * (ICAttrChanged | ICStringChanged) $B$,JV$k(J
				 */


/*
 * XtNauxControl $B%3!<%k%P%C%/$N(J call_data $B9=B$BN(J
 */

typedef struct {

    int command;	/* $B%3%^%s%I(J
			 *	Start: $B9`L\A*Br3+;O%j%/%(%9%H(J
			 *	End: $B9`L\A*Br=*N;%j%/%(%9%H(J
			 *	Change: $B%+%l%s%H9`L\@_Dj%j%/%(%9%H(J
			 */
#define ICAuxStart		1
#define ICAuxEnd		2
#define ICAuxChange		3

} ICAuxControlArg;

/*
 * $B%Q%V%j%C%/%$%s%?!<%U%'%$%9%U%!%s%/%7%g%s(J
 */

/*
 * Boolean ICSupportMultipleObjects(WidgetClass objectclass)
 *	$B;XDj$5$l$?%*%V%8%'%/%H%/%i%9$+$iJ#?t$N%$%s%9%?%s%9$,@8@.$G$-$k$+(J
 *	$B$I$&$+$rJV$9(J
 *	True $B$J$i2DG=$@$7!"(JFalse $B$J$i$?$@0l$D$N%$%s%9%?%s%9$7$+@8@.$G$-$J$$(J
 *	$B0z?t$,(J Widget $B$G$O$J$/(J WidgetClass $B$G$"$k$3$H$KCm0U(J
 */
extern Boolean ICSupportMultipleObjects(
#if NeedFunctionPrototypes
	WidgetClass	/* objectclass */
#endif
);

/*
 * int ICInputEvent(Widget ojbect, XEvent *event)
 *	$B%*%V%8%'%/%H$KF~NO%$%Y%s%H$rM?$($k(J
 *	$B%(%i!<$,5/$3$C$?>l9g$O(J -1 $B$=$l0J30$O(J 0 $B$,JV$k(J
 */
extern int ICInputEvent(
#if NeedFunctionPrototypes
	Widget		/* object */,
	XEvent *	/* event */
#endif
);

/*
 * ICString *ICGetMode(Widget object)
 *	$B8=:_$NF~NO%b!<%I$r(J ICString $B$N7A<0$GJV$9(J
 *	$B%G!<%?$O(J InputConvObject $B$N$b$N$J$N$G>!<j$KCM$rJQ99$7$?$j(J
 *	free() $B$7$F$O$J$i$J$$(J
 */
extern ICString *ICGetMode(
#if NeedFunctionPrototypes
	Widget		/* object */
#endif
);

/*
 * int ICCursorPos(Widget object, Cardinal *segidx, Cardinal *offset)
 *	$B8=:_$N%$%s%5!<%H%+!<%=%k0LCV$rJV$9(J
 *	$B8=:_%+!<%=%k$,$J$$;~$K$O(J 0 $B$"$k;~$K$O(J 1 $B$,JV$k(J
 *	1 $B$,JV$5$l$?;~$K$O!"(Jsegidx $B$K%+!<%=%k$N$"$k%;%0%a%s%HHV9f!"(J
 *	offset $B$K%;%0%a%s%HFb$N0LCV(J($B@hF,$+$i2?J8;zL\$+(J) $B$,JV$5$l$k(J
 */
extern int ICCursorPos(
#if NeedFunctionPrototypes
	Widget		/* object */,
	Cardinal *	/* segidx */,
	Cardinal *	/* offset */
#endif
);

/*
 * int ICNumSegments(Widget object)
 *	$B%;%0%a%s%H?t$rJV$9(J
 */
extern int ICNumSegments(
#if NeedFunctionPrototypes
	Widget		/* object */
#endif
);

/*
 * ICString *ICGetSegment(Widget object, Cardinal n)
 *	n $BHVL\$N%;%0%a%s%H$rJV$9(J ($B@hF,$N%;%0%a%s%H$O(J n=0)
 *	$B;XDj$5$l$?%;%0%a%s%H$,B8:_$7$J$1$l$P(J NULL $B$rJV$9(J
 *	$B%G!<%?$O(J InputConvObject $B$N$b$N$J$N$G>!<j$KCM$rJQ99$7$?$j(J
 *	free() $B$7$F$O$J$i$J$$(J
 */
extern ICString *ICGetSegment(
#if NeedFunctionPrototypes
	Widget		/* object */,
	Cardinal	/* n */
#endif
);

/*
 * int ICCompareSegment(Widget object, ICString *seg1, ICString *seg2,
 *			Cardinal *nchar)
 *	$BFs$D$N%;%0%a%s%H$rHf3S$9$k(J
 *	$BHf3S7k2L$O4X?t$NCM$H$7$FJV$5$l$k(J
 *	nchar $B$K%;%0%a%s%H$N@hF,$+$i$N0lCW$9$kJ8;z?t$,JV$5$l$k(J
 */
extern int ICCompareSegment(
#if NeedFunctionPrototypes
	Widget		/* object */,
	ICString *	/* seg1 */,
	ICString *	/* seg2 */,
	Cardinal *	/* nchar */
#endif
);

/*
 * ICString *ICGetItemList(Widget object, Cardinal *num_items)
 *	$BA*Br9`L\$N%j%9%H$rJV$9(J
 *	$B9`L\A*BrCf$G$J$1$l$P(J NULL $B$rJV$9(J
 *	num_items $B$K9`L\?t$,JV$5$l$k(J
 *	$B%G!<%?$O(J InputConvObject $B$N$b$N$J$N$G>!<j$KCM$rJQ99$7$?$j(J
 *	free() $B$7$F$O$J$i$J$$(J
 *	$B$3$N4X?t$NJV$9%j%9%H$OA*BrCf(J (ICSelectionStart $B$N%3!<%k%P%C%/$+$i(J
 *	ICSelectionEnd $B$N%3!<%k%P%C%/$^$?$O(J ICSelectItem() $B$,%3!<%k$5$l$k$^$G(J)
 *	$BM-8z$G$"$k(J
 */
extern ICString *ICGetItemList(
#if NeedFunctionPrototypes
	Widget		/* object */,
	Cardinal *	/* num_items */
#endif
);

/*
 * int ICSelectItem(Widget object, int n)
 *	n $BHVL\$N9`L\$,A*Br$5$l$?$3$H$r%*%V%8%'%/%H$KCN$i$;$k(J
 *	n < 0 $B$J$i$I$N9`L\$bA*Br$5$l$J$+$C$?$3$H$r<($9(J
 *	$B9`L\A*BrCf$G$J$+$C$?$j;XDj$7$?9`L\$,B8:_$7$J$1$l$P(J -1 $B$rJV$9(J
 *	$B$=$l0J30$O(J 0 $B$rJV$9(J
 *	XtNselectionControl $B%3!<%k%P%C%/Cf$G$3$N4X?t$r8F$VI,MW$O$J$$(J
 *	(ICSelectionControlArg.u.current_item $B$K%+%l%s%H$N9`L\HV9f$r(J
 *	$BBeF~$9$k$@$1$G$h$$(J)
 */
extern int ICSelectItem(
#if NeedFunctionPrototypes
	Widget		/* object */,
	int		/* n */
#endif
);

/*
 * int ICGetConvertedString(Widget object, Atom *encoding, int *format,
 *			    int *length, XtPointer *string)
 *	$BJQ49$5$l$?%F%-%9%H$r(J string $B$KJV$9(J
 *	encoding $B$K$O!"%F%-%9%H$N%(%s%3!<%G%#%s%0$r;XDj$7$F$*$/(J
 *	$B$?$@$7$3$l$OC1$J$k%j%/%(%9%H$G$"$C$F!"JQ49%*%V%8%'%/%H$O(J
 *	$BJL$N%(%s%3!<%G%#%s%0$GJV$7$F$b$h$$(J
 *	encoding $B$K$O<B:]$N%(%s%3!<%G%#%s%0$,JV$5$l$k(J
 *	$BJQ49%*%V%8%'%/%H$O>/$J$/$H$b(J COMPOUND_TEXT $B%(%s%3!<%G%#%s%0$O(J
 *	$B%5%]!<%H$7$J$/$F$O$J$i$J$$(J
 *	format $B$K$O(J 8/16/32 $B$N$$$:$l$+!"(Jlength $B$O(J string $B$N%(%l%a%s%H?t$,(J
 *	$B$=$l$>$lJV$5$l$k(J
 *	$B%F%-%9%H$NNN0h$O(J malloc $B$5$l$F$$$k$N$G$3$N4X?t$r8F$s$@B&$G(J
 *	free $B$7$J$1$l$P$J$i$J$$(J
 *	$BJQ49%F%-%9%H$,$J$$;~$d%(%i!<$N>l9g$K$O(J -1$B!"$=$&$G$J$1$l$P(J 0 $B$,(J
 *	$B4X?t$NCM$H$7$FJV$5$l$k(J
 *
 *	$B$3$N4X?t$O(J XtNfixNotify $B%3!<%k%P%C%/$NCf$G;H$o$l$k$3$H$rA[Dj$7$F$$$k(J
 */
extern int ICGetConvertedString(
#if NeedFunctionPrototypes
	Widget		/* object */,
	Atom *		/* encoding */,
	int *		/* format */,
	int *		/* length */,
	XtPointer *	/* string */
#endif
);

/*
 * int ICClearConversion(Widget object)
 *	$B6/@)E*$K(J ($BJQ49ESCf$G$"$C$F$b(J) $BJQ49%F%-%9%H$r%/%j%"$9$k(J
 *	$B2?$i$+$NM}M3$G%/%j%"$G$-$J$+$C$?;~$K$O(J -1$B!"$=$&$G$J$1$l$P(J 0 $B$,(J
 *	$BJV$5$l$k(J
 */
extern int ICClearConversion(
#if NeedFunctionPrototypes
	Widget		/* object */
#endif
);

/*
 * ICString *ICGetAuxSegments(Widget object, Cardinal *n,
 *                            Cardinal *ns, Cardinal *nc)
 *	AUX$BNN0h$N%;%0%a%s%H$rJV$9!#(J
 *	$B;XDj$5$l$?%;%0%a%s%H$,B8:_$7$J$1$l$P(J NULL $B$rJV$9(J
 *	$B%G!<%?$O(J InputConvObject $B$N$b$N$J$N$G>!<j$KCM$rJQ99$7$?$j(J
 *	free() $B$7$F$O$J$i$J$$(J
 *      $BF@$i$l$?%;%0%a%s%H?t!"%+%l%s%H%;%0%a%s%H!"%+%l%s%H%;%0%a%s%H(J
 *      $BFb$N%+!<%=%k%]%8%7%g%s$,$=$l$>$l(J n, ns, nc $B$K$FJV$k!#(J
 */
extern ICString *ICGetAuxSegments(
#if NeedFunctionPrototypes
	Widget		/* object */,
	Cardinal *	/* n */,
	Cardinal *	/* ns */,
	Cardinal *	/* nc */
#endif
);

#endif /* _InputConv_h */
