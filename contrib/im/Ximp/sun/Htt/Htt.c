/*
 *        %Z%%M% %I% %E%  
 * 
 * Derived from  @(#)Main.c	2.1 91/08/13 11:28:33 FUJITSU LIMITED. */
/*
****************************************************************************

              Copyright 1991, by FUJITSU LIMITED
              Copyright 1991, by Sun Microsystems, Inc.	      

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of FUJITSU LIMITED and Sun
Microsystems, Inc. not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.  FUJITSU LIMITED and Sun Microsystems, Inc. makes no
representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.

FUJITSU LIMITED AND SUN MICROSYSTEMS, INC. DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL FUJITSU LIMITED AND SUN
MICROSYSTEMS, INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Auther: Takashi Fujiwara	FUJITSU LIMITED
				fujiwara@a80.tech.yk.fujitsu.co.jp
	Kiyoaki oya		FUJITSU LIMITED
	Riki	Kawaguchi	FUJITSU LIMITED
	Tomoki	Kuriki		FUJITSU LIMITED
	Hideki  Hiura           (hhiura@Sun.COM) Sun Microsystems, Inc.

****************************************************************************
*/

#ifndef lint
static char     derived_from_sccs_id[] = "@(#)Main.c	2.1 91/08/13 11:28:33 FUJITSU LIMITED.";
static char     sccsid[] = "%Z%%M% %I% %E% Sun Microsystems Inc.";
#endif

#include <stdio.h>
#ifdef	XSF_X11R5
#include <locale.h>
#endif				/* XSF_X11R5 */
#include "Htt.h"
#include <X11/Xutil.h>

#ifdef FUJITSU_SERVER_EXTENSION
#include "fjextensions/cmod.h"
#endif				/* FUJITSU_SERVER_EXTENSION */

#ifdef USE_SIGNAL
#include <signal.h>
#endif				/* USE_SIGNAL */

#ifdef	XSF_X11R5
extern void		DisplayStatus_wcs() ;
#endif	/* XSF_X11R5 */

#ifdef DEBUG
int             dbg_flag = 0;
int             xsync = 0;
#endif				/* DEBUG */
int             verbose_flag = 0;

extern Atom     IMA_IMPROTOCOL;

unsigned char   vendor_name[] = "Sun Microsystems Inc.";
unsigned char   server_name[] = "Htt";
unsigned char   server_version[] = "Version0.9";

unsigned char   server_full_name[64];

Display        *display;
int             screen;
Window          root;

#ifdef	USE_X11R5_LIB
#define	DEFAULT_BASE_FONT  "-*-*-*-R-Normal--*-130-75-75-*-*"
char	*default_font_name = DEFAULT_BASE_FONT ;
#else
#define KANA14  "-*-fixed-medium-r-normal--14-*-*-*-*-*-jisx0201.1976-*"
#define K14     "-*-fixed-medium-r-normal--14-*-*-*-*-*-jisx0208.1983-*"
char	*default_ascii_kana_font_name = KANA14, *default_kanji_font_name = K14;
#endif	/* USE_X11R5_LIB */

#ifdef	XSF_X11R5
char		*default_foreground_color = "black",
		*default_background_color = "white",
		*default_border_color = "black";
#else
char		*default_foreground_color = "white",
		*default_background_color = "black",
		*default_border_color = "white";
#endif	/* XSF_X11R5 */

#ifdef	XSF_X11R5
char		*default_locale_name = "japanese" ;
#endif	/* XSF_X11R5 */

int		 default_border_width = 4;
char		*default_background_pixmap = "";
static char	*default2_background_pixmap = ".";

#ifdef	USE_X11R5_LIB
char		*font_name = NULL ;
#else
char		*ascii_kana_font_name = NULL, *kanji_font_name = NULL;
#endif	/* USE_X11R5_LIB */

char		*foreground_color = NULL,
		*background_color = NULL,
		*border_color = NULL;
char		*display_name = NULL;
#ifdef not_suport
char		*f_keybind = NULL;
#endif	/* not_suport */
char		*geometry = NULL;
char		*background_pixmap = NULL;
int		 bd_width = -1;
int		 max_client = -1;
int		 kanji_buff_size = -1;

#ifdef	XSF_X11R5
char		*locale_name = NULL ;
#endif	/* XSF_X11R5 */

char		*command_name;
char		*resurce_name;
FXimp_Client	*root_client;
FXimp_Client	*focus_in_client;
FXimp_Client	*current_client;
FXimpFont	*current_preedit_font;

short		 no_beep = -1;
short		 default_beep = 1;
short		 keybord_type = -1;
short		 def_keybord = FKEY_NONE;
short		 control_modifiers_grap = -1;
short		 title_on = 1;
unsigned char	*title;

#ifdef	XJP
short           xjp_flag = 0;
#endif	/* XJP */

/* SX/G status ------- */
char		 control_modifiers_flag = 0;
int		 control_modifiers_error_flag = 0;
#ifdef FUJITSU_SERVER_EXTENSION
int		 control_modifiers_event, control_modifiers_error;
unsigned long	 control_modifiers_state = 0;
unsigned long	 old_state = 0;
short		 def_key_grap = 1;
#else	/* FUJITSU_SERVER_EXTENSION */
short		 def_key_grap = 0;
#endif	/* FUJITSU_SERVER_EXTENSION */

#define gray_width 16
#define gray_height 16
static unsigned char gray_bits[] = {
    0xff, 0xff, 0x55, 0x55, 0xff, 0xff, 0x55, 0x55, 0xff, 0xff, 0x55, 0x55,
    0xff, 0xff, 0x55, 0x55, 0xff, 0xff, 0x55, 0x55, 0xff, 0xff, 0x55, 0x55,
0xff, 0xff, 0x55, 0x55, 0xff, 0xff, 0x55, 0x55};

void            ExitProcess();

int 
X_ErrorHandler(display, event)
    Display        *display;
    XErrorEvent    *event;
{
#ifdef FUJITSU_SERVER_EXTENSION
    if (control_modifiers_error_flag) {
	if (event->error_code == BadRequest) {
	    control_modifiers_error_flag = 0;
	    if (control_modifiers_grap) {
		unsigned long   mask;
		mask = ExposureMask | KeyPressMask | KeyReleaseMask
		    | FocusChangeMask;
		mask |= EnterWindowMask | LeaveWindowMask;
		XSelectInput(display, root_client->window, mask);
	    }
	    control_modifiers_grap = 0;
	    return (0);
	}
    }
#endif				/* FUJITSU_SERVER_EXTENSION */
#ifdef FUJITSU_OAK
    FXDefaultError(display, event);
#endif				/* FUJITSU_OAK */
    return (0);
}

int 
X_IOErrorHandler(display)
    Display        *display;
{
    WarningMessage(9);
    ExitProcess();
}


main(argc, argv)
    int             argc;
    char           *argv[];
{
    int             i, rc;
    sprintf((char *) server_full_name, "%s %s (%s)",
	    server_name, server_version, XIMP_PROTOCOL_VERSION);
    title = server_full_name;

    command_name = argv[0];
    resurce_name = argv[0];
    for (i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "-display"))
	    display_name = argv[++i];

#ifdef	USE_X11R5_LIB
	else if (!strcmp(argv[i], "-fn"))
	    font_name = argv[++i];
#else	/* USE_X11R5_LIB */
	else if (!strcmp(argv[i], "-fn"))
	    ascii_kana_font_name = argv[++i];
	else if (!strcmp(argv[i], "-fk"))
	    kanji_font_name = argv[++i];
#endif	/* USE_X11R5_LIB */

	else if (!strcmp(argv[i], "-fg"))
	    foreground_color = argv[++i];
	else if (!strcmp(argv[i], "-bg"))
	    background_color = argv[++i];
	else if (!strcmp(argv[i], "-bd"))
	    border_color = argv[++i];
	else if (!strcmp(argv[i], "-bp")) {
	    if (i + 1 < argc && argv[i + 1][0] != '-')
		background_pixmap = argv[++i];
	    else
		background_pixmap = default2_background_pixmap;
	} else if (!strcmp(argv[i], "-bw"))
	    bd_width = atoi(argv[++i]);

	else if (!strcmp(argv[i], "-none"))
	    keybord_type = FKEY_NONE;
	else if (!strcmp(argv[i], "-jis"))
	    keybord_type = FKEY_G_JIS;
	else if (!strcmp(argv[i], "-oya"))
	    keybord_type = FKEY_G_OYA;
	else if (!strcmp(argv[i], "-oyayubi"))
	    keybord_type = FKEY_G_OYA;

	else if (!strcmp(argv[i], "-nobeep"))
	    no_beep = 1;
	else if (!strcmp(argv[i], "-geometry"))
	    geometry = argv[++i];
	else if (!strcmp(argv[i], "-title")) {
	    title_on = 1;
	    if (*argv[i + 1] != '-')
		title = (unsigned char *) argv[++i];
	} else if (!strcmp(argv[i], "-notitle"))
	    title_on = 0;
	else if (!strcmp(argv[i], "-name"))
	    resurce_name = argv[++i];

#ifdef FUJITSU_SERVER_EXTENSION
	else if (!strcmp(argv[i], "-nograp"))
	    control_modifiers_grap = 0;
#endif				/* FUJITSU_SERVER_EXTENSION */
#ifdef not_suport
	else if (!strcmp(argv[i], "-kb"))
	    f_keybind = argv[++i];
#endif				/* not_suport */
#ifdef DEBUG
	else if (!strcmp(argv[i], "-dbg"))
	    dbg_flag = 1;
	else if (!strcmp(argv[i], "-sync"))
	    xsync = 1;
#endif				/* DEBUG */
#ifdef	XJP
	else if (!strcmp(argv[i], "-xjp"))
	    xjp_flag = 1;
#endif	/* XJP */
	else if (!strcmp(argv[i], "-v"))
	    verbose_flag = 1;
	else if (!strcmp(argv[i], "-verbose"))
	    verbose_flag = 1;

	else if (!strcmp(argv[i], "-name"))
	    resurce_name = argv[i];
#ifdef	XSF_X11R5
	else if (!strcmp(argv[i], "-japan"))
			locale_name = "japanese" ;
#endif	/* XSF_X11R5 */
#ifndef XSF_X11R5
	else if (rc = Dic_Argv(argc - i, &argv[i])) {
	    if (--rc > 0)
		i += rc;
	}
#endif				/* XSF_X11R5 */
	else {
	    ErrorMessage(25, argv[i]);
	    Usage();
	    exit(1);
	}
    }

    XSetErrorHandler(X_ErrorHandler);
    XSetIOErrorHandler(X_IOErrorHandler);

    display = XOpenDisplay(display_name);
    if (!display) {
	ErrorMessage(1, display_name);
	return (-1);
    }
    screen = DefaultScreen(display);
    root = RootWindow(display, screen);

#ifdef DEBUG
    if (xsync)
	XSynchronize(display, 1);
#endif				/* DEBUG */

    rc = GetDefaults();
    if (rc)
	return (4);

#ifdef	XSF_X11R5
	/* Locale set from Resource-DB */
	setlocale( LC_CTYPE, locale_name ) ;
#endif	/* XSF_X11R5 */

    rc = InitialClientTable(max_client);
    if (rc)
	return (4);

#ifndef XSF_X11R5
    rc = InitialKanjiBuff(kanji_buff_size);
    if (rc)
	return (4);
#endif				/* XSF_X11R5 */

#ifndef XSF_X11R5
    rc = Dic_Open();
    if (rc)
	return (4);
#endif				/* XSF_X11R5 */

#ifdef not_suport
    rc = ParseKeyBind(f_keybind);
    if (rc)
	return (4);
#endif	/* not_suport */

    rc = CreateRoot();
    if (rc)
	return (4);

    CreateXimpAtom();
#ifdef XJP
    CreateXjpAtom();
#endif				/* XJP */

    SetSignals();

#ifdef	XSF_X11R5
    xsfmgr_init();
#endif				/* XSF_X11R5 */

    MainLoop();
}

Usage()
{
    fprintf(stderr, "usage: %s [-display host:dpy] [-geometry WxH+X+Y]\n", command_name);
    fprintf(stderr, "\t[{-none|-jis|-oyayubi}] [-nobeep]\n");
    fprintf(stderr, "\t[-fn <ascii kana font_name>] [-fk <kanji font_name>]\n");
    fprintf(stderr, "\t[-fg <color>] [-bg <color>] [-bd <color>] [-bp <pixmap>]\n");
    fprintf(stderr, "\t[-bw <pixels>] [-v[erbose]] [{-title [<string>]|-notitle}] [-name <string>]\n");
#ifdef	XJP
    fprintf(stderr, "\t[-xjp]\n");
#endif	/* XJP */
#ifndef XSF_X11R5
    Dic_Help(stderr);
#endif				/* XSF_X11R5 */
}

void
ExitProcess()
{
#ifdef FUJITSU_SERVER_EXTENSION
    if (control_modifiers_flag && control_modifiers_grap == 1) {
	ControlModifiersUngrap(display);
    }
#endif				/* FUJITSU_SERVER_EXTENSION */

#ifndef XSF_X11R5
    Dic_Close();
#endif				/* XSF_X11R5 */
    ExitClientTable();
    XDestroyWindow(display, root_client->window);
    XCloseDisplay(display);
    exit(0);
}

void
SignalExitFunc()
{
    WarningMessage(8);
    ExitProcess();
}

SetSignals()
{
#ifdef USE_SIGNAL
    signal(SIGHUP, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, (void (*) ()) SignalExitFunc);
    signal(SIGCLD, SIG_IGN);
#endif				/* USE_SIGNAL */
}

int
GetDefaults()
{
    char           *GetStringDefault();

    if (!geometry)
	geometry = GetStringDefault("geometry", "Geometry", NULL);
#ifdef not_suport
    if (!f_keybind)
	f_keybind = GetStringDefault("keyBind", "KeyBind", NULL);
#endif				/* not_suport */

#ifdef	USE_X11R5_LIB
    if (!font_name)
	font_name = GetStringDefault("font", "Font", default_font_name);
#else	/* USE_X11R5_LIB */
    if (!ascii_kana_font_name)
	ascii_kana_font_name = GetStringDefault("font", "Font", default_ascii_kana_font_name);
    if (!kanji_font_name)
	kanji_font_name = GetStringDefault("kanjiFont", "KanjiFont", default_kanji_font_name);
#endif	/* USE_X11R5_LIB */

    if (!foreground_color)
	foreground_color = GetStringDefault("foreground", "Foreground", default_foreground_color);
    if (!background_color)
	background_color = GetStringDefault("background", "Background", default_background_color);
    if (!border_color)
	border_color = GetStringDefault("borderColor", "BorderColor", default_border_color);
    if (!background_pixmap)
	background_pixmap = GetStringDefault("backgroundPixmap", "BackgroundPixmap", default_background_pixmap);
    if (bd_width < 0)
	bd_width = GetIntDefault("borderWidth", "BorderWidth", default_border_width);

    if (max_client < 0)
	max_client = GetIntDefault("maxClient", "MaxClient", FXIMP_MAX_CLIENT_NO);
    if (kanji_buff_size < 0)
	kanji_buff_size = GetIntDefault("workSize", "WorkSize", 4096);

#ifdef FUJITSU_SERVER_EXTENSION
    if (control_modifiers_grap < 0)
	control_modifiers_grap = GetYesNoDefault("keyGrap", "KeyGrap", def_key_grap);
#endif				/* FUJITSU_SERVER_EXTENSION */

    if (keybord_type < 0)
	keybord_type = GetKeybordDefault("keyBord", "KeyBord", def_keybord);
    if (no_beep < 0)
	no_beep = GetYesNoDefault("Beep", "Beep", default_beep) ? 0 : 1;

#ifdef	XSF_X11R5
	if ( !locale_name )
			locale_name = GetStringDefault( "inputLang", "InputLang",
												default_locale_name ) ;
#endif	/* XSF_X11R5 */

    return (0);
}

char           *
GetStringDefault(inst_name, class_name, Default)
    char           *inst_name, *class_name;
    char           *Default;
{
    char           *tmp = XGetDefault(display, resurce_name, inst_name);
    if (!tmp)
	tmp = XGetDefault(display, resurce_name, class_name);
    if (!tmp)
	tmp = Default;
    return (tmp);
}

int
GetIntDefault(inst_name, class_name, Default)
    char           *inst_name, *class_name;
    int             Default;
{
    int             flag = Default;
    char           *tmp = XGetDefault(display, resurce_name, inst_name);
    if (!tmp)
	tmp = XGetDefault(display, resurce_name, class_name);
    if (tmp)
	flag = atoi(tmp);;
    return (flag);
}

int
GetYesNoDefault(inst_name, class_name, Default)
    char           *inst_name, *class_name;
    int             Default;
{
    int             flag = -1;
    char           *tmp = XGetDefault(display, resurce_name, inst_name);
    if (!tmp)
	tmp = XGetDefault(display, resurce_name, class_name);
    if (tmp) {
	if (!strcmp(tmp, "on"))
	    flag = 1;
	else if (!strcmp(tmp, "off"))
	    flag = 0;
	else if (!strcmp(tmp, "On"))
	    flag = 1;
	else if (!strcmp(tmp, "Off"))
	    flag = 0;
	else if (!strcmp(tmp, "True"))
	    flag = 1;
	else if (!strcmp(tmp, "False"))
	    flag = 0;
	else if (!strcmp(tmp, "true"))
	    flag = 1;
	else if (!strcmp(tmp, "false"))
	    flag = 0;
    }
    if (flag == -1)
	flag = Default;
    return (flag);
}

int
GetKeybordDefault(inst_name, class_name, Default)
    char           *inst_name, *class_name;
    int             Default;
{
    int             flag = -1;
    char           *tmp = XGetDefault(display, resurce_name, inst_name);
    if (!tmp)
	tmp = XGetDefault(display, resurce_name, class_name);
    if (tmp) {
	if (!strcmp(tmp, "none"))
	    flag = FKEY_NONE;
	else if (!strcmp(tmp, "jis"))
	    flag = FKEY_G_JIS;
	else if (!strncmp(tmp, "oyaubi", 3))
	    flag = FKEY_G_OYA;
    }
    if (flag == -1)
	flag = Default;
    return (flag);
}

CreateRoot()
{
    int				 x_cood, y_cood;
    int				 default_width,
				 default_height,
				 default_border_width,
				 default_depth;
    int				 default_border_width2 = 0;
    int				 x, y, height, width ;
    int				 max_font_width, max_font_height;
    unsigned long		 foreground_pixel, background_pixel,
				 border_pixel;
    unsigned long		 dummy;
    Colormap			 default_colormap;
    XColor			 screen_def_return, exact_def_return;
    XSizeHints			 hints;
    XWMHints			 wm_hints;
    XSetWindowAttributes	 attributes;
    int				 geometry_mask, gravity;
    int				 rc, len;
    Ximp_StatusPropRec		*current_status;
    Ximp_PreeditPropRec		*current_preedit;
    FXimpFont			*current_status_font;
    Pixmap			 pixmap;
    char			*base_font_name;

    char			*malloc();

    default_colormap = DefaultColormap(display, screen);
    if (XAllocNamedColor(display, default_colormap, foreground_color,
			 &screen_def_return, &exact_def_return))
	foreground_pixel = screen_def_return.pixel;
    else {
	ErrorMessage(10, foreground_color);
	foreground_pixel = WhitePixel(display, screen);
    }

    if (XAllocNamedColor(display, default_colormap, background_color,
			 &screen_def_return, &exact_def_return))
	background_pixel = screen_def_return.pixel;
    else {
	ErrorMessage(10, background_color);
	background_pixel = BlackPixel(display, screen);
    }

    if (XAllocNamedColor(display, default_colormap, border_color,
			 &screen_def_return, &exact_def_return))
	border_pixel = screen_def_return.pixel;
    else {
	ErrorMessage(10, border_color);
	border_pixel = WhitePixel(display, screen);
    }

    if (foreground_pixel == background_pixel) {
	ErrorMessage(11);
	foreground_pixel = WhitePixel(display, screen);
	background_pixel = BlackPixel(display, screen);
    }
    if (keybord_type != FKEY_NONE) {
#ifdef FUJITSU_SERVER_EXTENSION
	if (!ControlModifiersQueryExtension(display,
		      &control_modifiers_event, &control_modifiers_error)) {
	    WarningMessage(80);
#endif				/* FUJITSU_SERVER_EXTENSION */
	    keybord_type == FKEY_NONE;
	    control_modifiers_flag = 0;
#ifdef FUJITSU_SERVER_EXTENSION
	} else {
	    control_modifiers_flag = 1;
	}
#endif				/* FUJITSU_SERVER_EXTENSION */
    }
    root_client = current_client = NewClient(1);

    current_preedit_font = current_client->preedit_font;
    Fj_InitialMode();
#ifndef XSF_X11R5
    InitialTranslationTable();
#endif				/* XSF_X11R5 */

#ifdef	USE_X11R5_LIB
    len = strlen( font_name ) + 2 ;
    base_font_name = malloc(len);
    if (!base_font_name)
	return (-1);
    strcpy( base_font_name, font_name ) ;
#else	/* USE_X11R5_LIB */
    len = strlen(ascii_kana_font_name) + strlen(kanji_font_name) + 2;
    base_font_name = malloc(len);
    if (!base_font_name)
	return (-1);
    sprintf(base_font_name, "%s,%s", ascii_kana_font_name, kanji_font_name);
#endif	/* USE_X11R5_LIB */
    current_client->status_fontname = base_font_name;
    current_client->preedit_fontname = base_font_name;
    rc = FXLoadQueryBaseFont(display, current_preedit_font, base_font_name);
    if (rc)
	return (-1);

    current_client->status_font = current_client->preedit_font;
    current_status_font = current_client->preedit_font;

    default_border_width = bd_width;
    default_width = DisplayWidth(display, screen) - (default_border_width * 2);
    default_height = current_preedit_font->max_font_height / 2;
    if (default_height < 8)
	default_height = 8;
    default_height += current_preedit_font->max_font_height;
    x_cood = 0;
    y_cood = DisplayHeight(display, screen)
	- default_height - (default_border_width * 2);
    gravity = SouthWestGravity;

    if (geometry) {
	geometry_mask = XParseGeometry(geometry, &hints.x, &hints.y,
	 (unsigned int *) &(hints.width), (unsigned int *) &(hints.height));
	if (geometry_mask & WidthValue)
	    default_width = hints.width;
	if (geometry_mask & HeightValue)
	    default_height = hints.height;

	if ((geometry_mask & XValue) && (geometry_mask & XNegative))
	    hints.x += DisplayWidth(display, screen) - default_width;
	if ((geometry_mask & YValue) && (geometry_mask & YNegative))
	    hints.y += DisplayHeight(display, screen) - default_height;

	if (geometry_mask & XValue)
	    x_cood = hints.x;
	if (geometry_mask & YValue)
	    y_cood = hints.y;
	switch (geometry_mask & (XNegative | YNegative)) {
	case 0:
	    gravity = NorthWestGravity;
	    break;
	case XNegative:
	    gravity = NorthEastGravity;
	    break;
	case YNegative:
	    gravity = SouthWestGravity;
	    break;
	default:
	    gravity = SouthEastGravity;
	    break;
	}
    }
    current_client->window = XCreateSimpleWindow(display, root, x_cood, y_cood,
			default_width, default_height, default_border_width,
					    border_pixel, background_pixel);
    current_client->focus_window = current_client->window;

    XStoreName(display, current_client->window, (char *) title);
    if (!title_on) {
	bzero(&attributes, sizeof(XSetWindowAttributes));
	attributes.override_redirect = True;
	attributes.win_gravity = gravity;
	XChangeWindowAttributes(display, current_client->window,
			    CWOverrideRedirect | CWWinGravity, &attributes);
    }
    default_depth = DefaultDepth(display, screen);
    if (*background_pixmap) {
	if (!strcmp(background_pixmap, ".")) {
	    pixmap = XCreatePixmapFromBitmapData(display,
		     current_client->window, (char *) gray_bits, gray_width,
			    gray_height, background_pixel, foreground_pixel,
						 default_depth);
	}
#ifndef XSF_X11R5
	else {
	    pixmap = XmuLocatePixmapFile(screen, background_pixmap,
			  background_pixel, foreground_pixel, default_depth,
					 NULL, 0, NULL, NULL, NULL, NULL);
	}
#endif	/* XSF_X11R5 */
	if (pixmap)
	    XSetWindowBackgroundPixmap(display, current_client->window, pixmap);
    } else {
	pixmap = 0;
    }

    bzero(&wm_hints, sizeof(XWMHints));
#ifdef	XSF_X11R5
    wm_hints.initial_state = IconicState;
#else
    wm_hints.initial_state = NormalState;
#endif				/* XSF_X11R5 */
    wm_hints.flags = InputHint | StateHint;
    wm_hints.input = True;
    XSetWMHints(display, current_client->window, &wm_hints);

    bzero(&hints, sizeof(XSizeHints));
    hints.flags = USSize | PMinSize | PBaseSize | USPosition | PWinGravity;
    hints.x = x_cood;
    hints.y = y_cood;
    hints.width = default_width;
    hints.height = default_height;
    hints.min_width = default_width;
    hints.min_height = default_height;
    hints.base_width = default_width;
    hints.base_height = default_height;
    hints.win_gravity = gravity;
    XSetWMNormalHints(display, current_client->window, &hints);
    /*
     * |<------------------default_width
     * ------------------------------------>| |<--- 10 * w ->|
     * +-------------------------------------------------------+
     * |+--------------------------------------+		| ||  Preedit
     * Window			|  Status Win.	|
     * |+--------------------------------------+		|
     * +-------------------------------------------------------+
     */
#ifdef	USE_X11R5_LIB
    max_font_width = current_status_font->max_font_width;
#else
    max_font_width = current_status_font->kanji_font_width;
#endif	/* USE_X11R5_LIB */
    max_font_height = current_status_font->max_font_height;

    current_client->style = (XIMPreeditNothing | XIMStatusNothing);

    width = default_width - ( max_font_width * 10 ) ;
    x = 0;
    y = 0;
    current_status = current_client->status;
    current_status->Area.width = width;
    current_status->Area.height = default_height;
    current_status->Area.x = x;
    current_status->Area.y = y;
    current_status->Foreground = foreground_pixel;
    current_status->Background = background_pixel;
    current_status->Bg_Pixmap = pixmap;

    current_client->status_window
	= XCreateSimpleWindow(display, current_client->window, x, y, width,
     default_height, default_border_width2, border_pixel, background_pixel);
    XStoreName(display, current_client->status_window, "XIMStatusNothing");
    ChangeWindowAttributes(display, current_client->status_window,
			   current_status);
    width -= ( max_font_width * GetMaxStatusMessageLength(current_client) )
	+ default_border_width * 2;
    x = max_font_width / 4;
    if (x & 0x1)
	x++;
    if (x < 4)
	x = 4;
    width -= x;
    current_preedit = current_client->preedit;
    current_preedit->Area.width = width;
    current_preedit->Area.height = default_height;
    current_preedit->Area.x = x;
    current_preedit->Area.y = y;
    current_preedit->Foreground = foreground_pixel;
    current_preedit->Background = background_pixel;
    current_preedit->Bg_Pixmap = pixmap;

    current_client->preedit_window
	= XCreateSimpleWindow(display, current_client->status_window,
			 x, y, width, default_height, default_border_width2,
			      border_pixel, background_pixel);
    XStoreName(display, current_client->preedit_window, "XIMPreeditNothing");
    ChangeWindowAttributes(display, current_client->preedit_window,
			   current_preedit);
    FXCreateGC(display, current_client->preedit_window,
	       current_preedit_font, foreground_pixel, background_pixel);
#ifdef DEBUG
    dbg_printf("root window=%x focus_window=%x status_window=%x preedit_window=%x\n",
	       current_client->window, current_client->focus_window,
	     current_client->status_window, current_client->preedit_window);
#endif	/* DEBUG */
    return (0);
}

MainLoop()
{
    XEvent          event;
    unsigned long   mask;
    unsigned long   dummy;

#ifdef	XSF_X11R5
    mask = FocusChangeMask;
    mask |= EnterWindowMask | LeaveWindowMask;
#else
    mask = KeyPressMask | FocusChangeMask;
    mask |= EnterWindowMask | LeaveWindowMask;
#endif				/* XSF_X11R5 */
    if (control_modifiers_flag == 0 || control_modifiers_grap == 0)
	mask |= KeyReleaseMask;
    XSelectInput(display, root_client->window, mask);
    XSelectInput(display, root_client->status_window, ExposureMask);
    XSelectInput(display, root_client->preedit_window, ExposureMask);
    XMapWindow(display, root_client->window);
    XMapWindow(display, root_client->status_window);
    root_client->status_visible = 1;

#ifdef FUJITSU_SERVER_EXTENSION
    if (control_modifiers_flag) {
	ControlModifiersGetVersion(display, &dummy, &dummy,
				   &control_modifiers_state, &dummy);
	ControlModifiersSetMask(display, root_client->window,
				control_modifiers_state);
    }
#endif				/* FUJITSU_SERVER_EXTENSION */

    if (control_modifiers_grap == 1)
	control_modifiers_error_flag = 1;

    for (;;) {
	XNextEvent(display, &event);
#ifdef FUJITSU_SERVER_EXTENSION
	if (control_modifiers_flag && event.type == control_modifiers_event) {
	    if ((focus_in_client == (FXimp_Client *) 0)
		|| (focus_in_client->henkan_mode == 0)) {
		DisplayStatus(root_client);
	    }
	    continue;		/* next_item event */
	}
#endif				/* FUJITSU_SERVER_EXTENSION */

	switch (event.type) {
	case EnterNotify:
	case LeaveNotify:
	    if ((event.xcrossing.focus)
		&& (event.xcrossing.detail != NotifyInferior)
		&& (SetCurrentClient(event.xcrossing.window) == 0)) {
		if (event.type == EnterNotify) {
#ifdef DEBUG
		    dbg_printf( "Event : EnterNotify w=0x%x\n",
					event.xcrossing.window ) ;
#endif				/* DEBUG */
		    SetFocusClient();

		} else {
#ifdef DEBUG
		    dbg_printf( "Event : LeaveNotify w=0x%x\n",
					event.xcrossing.window ) ;
#endif				/* DEBUG */
		    UnsetFocusClient();
#ifdef XSF_X11R5
		    xsfIMLunsetfocus(current_client);
#endif				/* XSF_X11R5 */
		}
	    }
	    break;
	case Expose:
	    {
		FXimp_Client   *ximp_client;
#ifdef DEBUG2
		dbg_printf("Expose Event => window=0x%x\n", event.xany.window);
#endif
		if (focus_in_client &&
		    (focus_in_client->status_window == event.xany.window
		   || focus_in_client->preedit_window == event.xany.window))
		    ximp_client = focus_in_client;
		else
		    ximp_client =
			FindClientByPreeditOrStatusWindow(event.xany.window);

		if (ximp_client) {
		    current_client = ximp_client;
		    if (event.xany.window == current_client->status_window)
#ifdef	XSF_X11R5
			DisplayStatus_wcs(current_client);
#else	/* Not XSF_X11R5 */
			DisplayStatus(current_client);
#endif	/* XSF_X11R5 */
		    if (event.xany.window == current_client->preedit_window)
			Redisplay();
		}
	    }
	    break;
	case KeyPress:
	case KeyRelease:
#ifdef DEBUG
		    dbg_printf( "Event : KeyPressNotify w=0x%x\n",
						event.xkey.window ) ;
#endif				/* DEBUG */
	    if ( SetCurrentClient( event.xkey.window ) == 0 ) {
#ifdef XSF_X11R5
		xsfKeyPress( current_client, &event ) ;
#else
		CurrentKeyPress(&event)
#endif				/* XSF_X11R5 */
	    }
	    break;
	case ClientMessage:
	    if (event.xclient.message_type == IMA_IMPROTOCOL) {
		XimpClientMessage(&event);
	    }
#ifdef XJP
	    else {
		XjpClientMessage(&event);
	    }
#endif				/* XJP */
	    break;
	case DestroyNotify:
	    if (SetCurrentClient(event.xkey.window) == 0) {
		DestroyCurrentClient(&event);
	    }
	    break;
	}
    }
}

#ifdef FUJITSU_SERVER_EXTENSION
int
GetKeyMapMode()
{
    int             shift_mode;
    extern unsigned long control_modifiers_state;
    ControlModifiersGetState(display, &control_modifiers_state);
    if (control_modifiers_state & CModKanaLockMask)
	shift_mode = 1;
    else
	shift_mode = (control_modifiers_state & LockMask) ? 2 : 3;
    return (shift_mode);
}
#endif				/* FUJITSU_SERVER_EXTENSION */

int
ChangeShiftMode(sw)
    int             sw;		/* 0: reset Map, 1: set Map 2: change Map */
{
#ifdef FUJITSU_SERVER_EXTENSION
    unsigned long   mask, change;
    mask = CModKanaLockMask | LockMask;

    if (sw > 0) {		/* set Map  1 or 2		 */
	if (sw == 1) {
	    if (control_modifiers_grap == 1) {
		ControlModifiersGrap(display);
	    }
	    ControlModifiersGetState(display, &(current_client->old_state));
	}
	change = 0;
	if (current_client->romaji_mode == 0) {
	    change = current_client->siftmode & FS_ASCII ?
		0 : CModKanaLockMask;
	}
	if (current_client->siftmode == FS_UPPER_ASCII)
	    change |= LockMask;
    } else {
	if (control_modifiers_grap == 1) {
	    ControlModifiersUngrap(display);
	}
	change = current_client->old_state;
    }
    ControlModifiersChangeState(display, mask, change);
#endif				/* FUJITSU_SERVER_EXTENSION */
}

#ifdef DEBUG
dbg_printf(form, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16)
    char           *form;
    unsigned long   a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12,
                    a13, a14, a15, a16;
{
    if (dbg_flag) {
	fprintf(stderr, form,
		a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16);
	fflush(stderr);
    }
}
#endif	/* DEBUG */
