/* $Id: ConvProto.h,v 1.4 1991/09/27 10:23:20 ishisone Rel $ */

/* $BJQ49%5!<%P$N%W%m%U%!%$%k$,F~$k%W%m%Q%F%#L>(J */
#define CONVERSION_PROFILE	"_CONVERSION_PROFILE"

/* $BJQ49%5!<%P$N%W%m%U%!%$%k$N%W%m%Q%F%#$HJQ49B0@-$N%W%m%Q%F%#$N%?%$%W(J */
#define CONVERSION_ATTRIBUTE_TYPE	"_CONVERSION_ATTRIBUTE_TYPE"

/* $B%W%m%H%3%k%P!<%8%g%sL>(J */
#define PROTOCOL_VERSION	"PROTOCOL-2.0"

#define CONV_ATTR(code,len)	((unsigned long)((code)<<16)+(len))

#define UPPER16U(data)		(((data)>>16)&0xffff)
#define UPPER16S(data)		((short)(((data)>>16)&0xffff))
#define LOWER16U(data)		((data)&0xffff)
#define LOWER16S(data)		((short)((data)&0xffff))

#define CODE_OF_ATTR(head)	UPPER16U(head)
#define LENGTH_OF_ATTR(head)	LOWER16U(head)

/*
 * Conversion Profile Codes
 */

#define CONVPROF_PROTOCOL_VERSION	1
#define CONVPROF_SUPPORTED_STYLES	2
#define CONVPROF_SUPPORTED_EXTENSIONS	3
#define CONVPROF_EXTENSION_DATA		4

/*
 * Standard Conversion Attribute Codes (0-255)
 */

/* 0-127: can be specified at any time (startup and during conversion) */
#define CONVATTR_NONE			0
#define CONVATTR_INDIRECT		1
#define CONVATTR_FOCUS_WINDOW		2
#define CONVATTR_SPOT_LOCATION		3
#define CONVATTR_CLIENT_AREA		4
#define CONVATTR_STATUS_AREA		5
#define CONVATTR_COLORMAP		6
#define CONVATTR_COLOR			7
#define CONVATTR_BACKGROUND_PIXMAP	8
#define CONVATTR_LINE_SPACING		9
#define CONVATTR_FONT_ATOMS		10
#define CONVATTR_CURSOR			11

/* 128-255: can be specified only at startup time */
#define CONVATTR_INPUT_STYLE		128
#define CONVATTR_EVENT_CAPTURE_METHOD	129
#define CONVATTR_USE_EXTENSION		255

/* argument for CONVATTR_INPUT_STYLE and CONVPROP_SUPPORTED_STYLES */
#define CONVARG_ROOTWINDOW		1L
#define CONVARG_OFFTHESPOT		2L
#define CONVARG_OVERTHESPOT		4L

/* argument for CONVATTR_EVENT_CAPTURE_METHOD */
#define CONVARG_NONE			0L
#define CONVARG_CREATE_INPUTONLY	1L
#define CONVARG_SELECT_FOCUS_WINDOW	2L

/*
 * $B%W%m%U%!%$%k%G!<%?(J / $BJQ49B0@-%G!<%?$NI=8=J}K!(J
 *
 * $BJQ49%5!<%P$NFC@-$rI=$9%W%m%U%!%$%k%G!<%?$H!"JQ49$K4X$9$kB0@-$r;XDj$9(J
 * $B$kJQ49B0@-%G!<%?$O6&DL$N%U%)!<%^%C%H$rMQ$$$k!#(J
 *
 * $B8D!9$N%G!<%?$O(J 32bit$BCM$NG[Ns$GI=8=$5$l$k!#:G=i$N(J 1$B%o!<%I$O%X%C%@$G!"(J
 * $B$=$l$K(J 0$B%o!<%I0J>e$N%G!<%?$,B3$/!#%X%C%@$N>e0L(J 16bit $B$O$=$N%W%m%U%!%$(J
 * $B%k(J / $BJQ49B0@-$N%3!<%I$rI=$7!"2<0L(J 16 bit $B$OB3$/%G!<%?$N%o!<%I?t(J 
 * (32bit $BC10L(J) $B$rI=$9!#(J
 *
 *	+----------------+----------------+
 *	|  Code (16bit)  | Length (16bit) |
 *	+----------------+----------------+
 *	|              Data0              |
 *	+---------------------------------+
 *	|              .....              |
 *	+---------------------------------+
 *	|              DataN              |
 *	+---------------------------------+
 *
 * $B<B:]$N%W%m%U%!%$%k%G!<%?$dJQ49B0@-%G!<%?$O$3$N%G!<%?$,$$$/$D$+O"B3$7(J
 * $B$?$b$N$G$"$k!#(J
 */

/*
 * $B%W%m%U%!%$%k%G!<%?(J
 *
 * $B%W%m%U%!%$%k%G!<%?MQ$N%3!<%I$O<!$N(J 4$B<oN`$,Dj5A$5$l$F$$$k!#JQ49B0@-(J
 * $B%G!<%?$H0[$J$j!"%W%i%$%Y!<%HMQ$N%3!<%INN0h$J$I$OMQ0U$5$l$F$$$J$$!#(J
 *
 * Protocol Version
 *	code: 1
 *	data-length: 1
 *	data[0]:
 *		CARD32: protocol version atom ("PROTOCOL-2.0")
 *
 *	$B%G!<%?$OJQ49%5!<%P$N%W%m%H%3%k%P!<%8%g%s$rI=$9%"%H%`$G$"$k!#$3$3(J
 *	$B$GDj5A$5$l$F$$$k%W%m%H%3%k$N%P!<%8%g%s$O(J "PROTOCOL-2.0" $B$G$"$k!#(J
 *
 * Supported Styles
 *	code: 2
 *	data-length: 1
 *	data[0]:
 *		CARD32: input styles
 *
 *	$B%G!<%?$OJQ49%5!<%P$,%5%]!<%H$9$kF~NO%9%?%$%k$rI=$9!#%5%]!<%H$9$k(J
 *	$BF~NO%9%?%$%k$NCM$N(J bitwise-or $B$G$"$k!#(J
 *
 * Supported Extensions
 *	code: 3
 *	data-length: N
 *	data[0]:
 *		CARD32: extension atom 1 (Atom)
 *	...
 *	data[N-1]:
 *		CARD32: extension atom N (Atom)
 *
 *	$B%G!<%?$OJQ49%5!<%P$,%5%]!<%H$9$k3HD%$rI=$9%"%H%`$N%j%9%H$G$"$k!#(J
 *
 * Extension Data
 *	code: 4
 *	data-length: N
 *	data[0]:
 *		CARD32: extension atom (Atom)
 *	data[1] - data[N-1]:
 *		extension specific data
 *
 *	$B%G!<%?$O3HD%FH<+$KDj5A$7$?%W%m%U%!%$%k%G!<%?$G$"$k!#I8=`%W%m%H%3(J
 *	$B%k$H$7$F$O%G!<%?$N@hF,$K3HD%%"%H%`(J ($B$3$l$O(JSupported Extensions 
 *	$B$K;XDj$5$l$?$b$N$G$J$1$l$P$J$i$J$$(J)$B$r$D$1$k$3$H$r5,Dj$9$k$@$1$G!"(J
 *	$B$=$N8e$N%G!<%?$K4X$7$F$O0l@Z5,Dj$7$J$$!#(J
 *
 * $B%/%i%$%"%s%HB&$NL5MQ$N:.Mp$rKI$0$?$a!"(JProtocol Version $B$H(J Supported
 * Styles$B$N9`L\$OI,$:$J$1$l$P$J$i$J$$!#$^$?!"(JExtension Data $B0J30$O%W%m%U%!(J
 * $B%$%k%G!<%?$NCf$KF1$8%3!<%I$N%G!<%?$,J#?t$"$C$F$O$J$i$J$$!#(J
 */

/*
 * $BJQ49B0@-%G!<%?(J
 *
 * $BB0@-%3!<%I$N$&$A!"(J0 $B$+$i(J 255 $B$^$G$OI8=`%W%m%H%3%k$,;HMQ$9$k$b$N$G!"8=(J
 * $B:_B0@-$,3d$j?6$i$l$F$$$J$$$+$i$H$$$C$F>!<j$K;HMQ$7$F$O$J$i$J$$!#$=$N(J
 * $B$h$&$JL\E*$N$?$aB0@-%3!<%I(J 256 $B$+$i(J 65535 $B$,%W%i%$%Y!<%H%3!<%I3HD%NN(J
 * $B0h$H$7$FMQ0U$5$l$F$$$k!#$?$@$7$3$NNN0h$N;HMQ$KEv$?$C$F$O$"$i$+$8$a$=(J
 * $B$N3HD%%3!<%I$r;HMQ$9$k$3$H$r(J Use Extension ($B2<5-;2>H(J) $B$rMQ$$$F$"$i$+(J
 * $B$8$a@k8@$9$kI,MW$,$"$k!#(J
 *
 * $BB0@-%G!<%?$N;XDjJ}K!$K$O!"JQ493+;O;~$K;XDj$9$kJ}K!$H!"JQ49Cf$K;XDj$9(J
 * $B$kJ}K!$N(J 2$BDL$j$,$"$k$,!"B0@-%3!<%I$K$h$C$F$OJQ493+;O;~$K$7$+;XDj$G$-(J
 * $B$J$$$b$N$,$"$k!#$=$3$G!"(J0-255 $B$NI8=`%3!<%I$N$&$A!"(J0 $B$+$i(J 127 $B$^$G$OJQ(J
 * $B493+;O;~$G$bJQ49Cf$G$b;XDj$G$-$k$b$N!"(J128 $B$+$i(J 255 $B$^$G$OJQ493+;O;~$K(J
 * $B$7$+;XDj$G$-$J$$$b$N!"$KJ,$1$F$"$k!#3HD%%3!<%I$K$D$$$F$OFC$K$3$N$h$&(J
 * $B$J6hJL$ODj$a$J$$!#(J
 *
 * $B$3$N%W%m%H%3%k$GDj5A$5$l$kB0@-%3!<%I$O<!$NDL$j$G$"$k!#(J
 *
 * -- $BJQ493+;O;~$K$b!"JQ49ESCf$K$b;XDj$G$-$k$b$N(J --
 *
 * No Operation:
 *	code: 0
 *	data-length: N (could be 0)
 *	data: anything
 *
 *	$B2?$b$7$J$$!#%W%m%Q%F%#$N$"$kItJ,$r%9%-%C%W$5$;$k$N$KJXMx!#(J
 *	
 * Indirect Attribute:
 *	code: 1
 *	data-length: 1
 *	data[0]:
 *		CARD32: property name (Atom)
 *
 *	$B;XDj$5$l$?%W%m%Q%F%#$K=>$C$FB0@-$r@_Dj$9$k!#(JCONVERSION_ATTRIBUTE
 * 	$B%$%Y%s%H$GJ#?t$NB0@-%G!<%?$r@_Dj$7$?$$;~$d!"%$%Y%s%H$KB0@-%G!<%?$,(J
 *	$BF~$j@Z$i$J$$;~$K;HMQ$9$k!#(J
 *
 * Focus Window:
 *	code: 2
 *	data-length: 1
 *	data[0]:
 *		CARD32: focus window (Window)
 *
 *	$B%U%)!<%+%9%&%#%s%I%&$r;XDj$9$k!#(J
 *
 * Spot Location:
 *	data-length: 1
 *	data[0]:
 *		INT16(upper 16bit): X
 *		INT16(lower 16bit): Y
 *
 *	$B%9%]%C%H%m%1!<%7%g%s$r;XDj$9$k!#%Y!<%9%i%$%s$N3+;OE@$G;XDj$9$k!#(J
 *
 * Client Area:
 *	data-length: 2
 *	data[0]:
 *		INT16(upper 16bit): X
 *		INT16(lower 16bit): Y
 *	data[1]:
 *		CARD16(upper 16bit): Width
 *		CARD16(lower 16bit): Height
 *
 *	$BJQ49%F%-%9%HI=<(NN0h$r;XDj$9$k!#(J
 *
 * Status Area:
 *	data-length: 2
 *	data[0]:
 *		INT16(upper 16bit): X
 *		INT16(lower 16bit): Y
 *	data[1]:
 *		CARD16(upper 16bit): Width
 *		CARD16(lower 16bit): Height
 *
 *	$B%9%F!<%?%9I=<(NN0h$r;XDj$9$k!#(J
 *
 * Colormap:
 *	data-length: 1
 *	data[0]:
 *		CARD32: colormap (XID)
 *
 *	$B%+%i!<%^%C%W(J ID $B$r;XDj$9$k!#(J
 *
 * Color:
 *	data-length: 2
 *	data[0]:
 *		CARD32: foreground pixel
 *	data[1]:
 *		CARD32: background pixel
 *
 *	$B%U%)%"%0%i%&%s%I$H%P%C%/%0%i%&%s%I$N%T%/%;%kCM$r;XDj$9$k!#(J
 *
 * Background Pixmap:
 *	data-length: 1
 *	data[0]:
 *		CARD32: background pixmap (Pixmap)
 *
 *	$B%P%C%/%0%i%&%s%I$N(J Pixmap ID $B$r;XDj$9$k!#(J
 *
 * Line Spacing:
 *	data-length: 1
 *	data[0]:
 *		CARD32: line spacing
 *
 *	$B9T4V$r;XDj$9$k!#%Y!<%9%i%$%s4V$N5wN%$G;XDj$9$k!#(J
 *
 * Font Atoms:
 *	data-length: N (>0)
 *	data[0]:
 *		CARD32: font atom 1 (Atom)
 *	...
 *	data[N-1]:
 *		CARD32: font atom N (Atom)
 *
 *	$B;HMQ$9$k%U%)%s%H$N(J "FONT" $B%"%H%`$N%j%9%H$r;XDj$9$k!#(J
 *
 * Cursor:
 *	data-length: 1
 *	data[0]:
 *		CARD32: cursor (Cursor)
 *
 *	$B%+!<%=%k(J ID $B$r;XDj$9$k!#(J
 *
 * -- $BJQ493+;O;~$N$_;XDj$G$-$k$b$N(J --
 *
 * Input Style:
 *	data-length: 1
 *	data[0]:
 *		CARD32: input style
 *
 *	$BF~NOJ}K!$r;XDj$9$k!#(J
 *	$B%G%U%)%k%H$O(J Root Window Style $B$G$"$k!#(J
 *
 * Event Capture Method:
 *	data-length: 1
 *	data[0]:
 *		CARD32: event capture method
 *
 *	$B%/%i%$%"%s%H%&%#%s%I%&$+$i$N%$%Y%s%H$N<hF@J}K!$r;XDj$9$k!#%G%U%)(J
 *	$B%k%H$O%/%i%$%"%s%H%&%#%s%I%&$NA0$K(J InputOnly $B%&%#%s%I%&$r:n$C$F(J
 *	$B$=$N%-!<%$%Y%s%H$r%;%l%/%H$9$k$H$$$&$b$N$G$"$k!#B>$NJ}K!$H$7$F$O!"(J
 *	$B%U%)!<%+%9%&%#%s%I%&(J ($B%U%)!<%+%9%&%#%s%I%&$,;XDj$5$l$F$$$J$1$l$P(J
 *	$B%/%i%$%"%s%H%&%#%s%I%&(J) $B$N%-!<%$%Y%s%H$rD>@\%;%l%/%H$9$k(J ($B$3$N>l(J
 *	$B9g!"JQ49Cf$O%/%i%$%"%s%H$O%-!<%$%Y%s%H$rL5;k$7$J$/$F$O$J$i$J$$(J) 
 *	$B$b$N$H!"2?$b$7$J$$!"$D$^$jJQ49Cf$N%/%i%$%"%s%H$O%-!<%$%Y%s%H$r%U(J
 *	$B%m%s%H%(%s%I$K(J SendEvent $B$7$J$/$F$O$J$i$J$$!"$H$$$&J}K!$,$"$k!#(J
 *
 * Use Extension:
 *	data-length: N
 *	data[0]:
 *		CARD32: extension atom 1 (Atom)
 *	...
 *	data[N-1]:
 *		CARD32: extension atom N (Atom)
 *
 *	$B$3$NB0@-@_Dj$G;HMQ$5$l$k3HD%$r;XDj$9$k!#$3$3$G;XDj$9$k3HD%$O%5!<(J
 *	$B%P$,%5%]!<%H$7$F$$$k$b$N!"$D$^$j%W%m%U%!%$%k%G!<%?Cf$N(JSupported
 *	Extensions $B$K=q$+$l$?3HD%$G$J$1$l$P$J$i$J$$!#(J
 */

