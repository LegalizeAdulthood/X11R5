/******************************************************************

              Copyright 1991, by FUJITSU LIMITED

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of FUJITSU LIMITED
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.
FUJITSU LIMITED makes no representations about the suitability of
this software for any purpose.  It is provided "as is" without
express or implied warranty.

FUJITSU LIMITED DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJITSU LIMITED BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

  Author: Takashi Fujiwara     FUJITSU LIMITED 
                               fujiwara@a80.tech.yk.fujitsu.co.jp

******************************************************************/

#include <stdio.h>
#include <X11/Xlocale.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/Xresource.h>

#define W_WIDTH   422
#define W_HEIGHT  162

#define MAXTYPE 20
#define DEFAULT_LOCALE		"ja_JP.EUC"
#define DEFAULT_FONT_NAME	"-*-*-*-R-Normal--*-130-75-75-*-*"

static void     CreateStatus();
static Window	JxCreateTextWindow();
static int      JxDestroyTextWindow();
static int      JxRealizeText();
static int      JxRedisplayText();
static int      _JxNextLine();
static int      JxWriteText();

main(argc, argv)
int	argc;
char	*argv[];
	{
	Display			*display;
	int			screen;
	Window			root, window, client;
	XRectangle		rect, StatusArea;
	XEvent			event;
	unsigned long		mask;
	unsigned long		fpixel, bpixel;
	int			font_height;
	XPoint			point, spot;
	int			i, n;
	char			*base_font_name = NULL;
	int			len = 128;
	char			string[128];
	KeySym			keysym;
	int			count;
	XIMStyles		*im_styles;
	XIMStyle		xim_mode = XIMPreeditPosition | XIMStatusArea;
	XIM			im;
	XIC			ic;
	XFontSet		font_set;
	char			*locale = NULL;
	XVaNestedList		preedit_attr, status_attr;
	char			*missing_list;
	int			missing_count;
	char			*def_string;
	char			*display_name = NULL;
	int			backend_mode = 0;
	char			*res_file = NULL;
	XrmDatabase		rdb = NULL;
        XWMHints                hints ;

	for(i=1; i<argc; i++) {
		if(!strcmp(argv[i], "-Root")) {
			xim_mode = XIMPreeditNothing | XIMStatusNothing;
			}
		else if(!strcmp(argv[i], "-Pos")) {
			xim_mode = XIMPreeditPosition | XIMStatusNothing;
			}
		else if(!strcmp(argv[i], "-fs"))
			base_font_name = argv[++i];
		else if(!strcmp(argv[i], "-display"))
			display_name = argv[++i];
		else if(!strcmp(argv[i], "-back"))
			backend_mode = 1;
		else if(!strcmp(argv[i], "-xrm_file"))
			res_file = argv[++i];
		}
	rect.x      = rect.y      = 0;
	rect.width  = W_WIDTH;
	rect.height = W_HEIGHT;
	display = XOpenDisplay(display_name);
	screen  = DefaultScreen(display);
	root    = RootWindow(display, screen);
	fpixel  = BlackPixel(display, screen);
	bpixel  = WhitePixel(display, screen);
	window = XCreateSimpleWindow(display, root, 0, 0,
				W_WIDTH, W_HEIGHT, 2, bpixel, fpixel);

        hints.flags = InputHint ;
        hints.input = True ;    
        XSetWMHints( display, window, &hints ) ;

	if(locale == NULL)
		locale = DEFAULT_LOCALE;
	if(setlocale(LC_CTYPE, locale) == NULL) {
		fprintf(stderr, "Error : setlocale() !\n");
 		exit(0);
		}

	if (base_font_name == NULL)
		base_font_name = DEFAULT_FONT_NAME;
	font_set = XCreateFontSet(display, base_font_name, &missing_list,
			       &missing_count, &def_string);

	client = JxCreateTextWindow(display, window, 0, 0,
				W_WIDTH-2, W_HEIGHT-2, 1, bpixel, fpixel,
				font_set, &font_height);
	if(res_file) {
		printf("Set Database : file name = %s\n", res_file);
		rdb = XrmGetFileDatabase(res_file);
		}

	if((im = XOpenIM(display, rdb, NULL, NULL)) == NULL) {
		printf("Error : XOpenIM() !\n");
		exit(0);
		}

	XStoreName(display, window, "XIM demo");
	XStoreName(display, client, "XIM client");

	mask = StructureNotifyMask | FocusChangeMask | ExposureMask;
	XSelectInput(display, window, mask);
	mask = ExposureMask | KeyPressMask | FocusChangeMask;
	XSelectInput(display, client, mask);

	spot.x = 0;
	spot.y = font_height;

	if(xim_mode & XIMPreeditPosition) {
		if(res_file) {
			preedit_attr = XVaCreateNestedList( NULL,
						XNArea, &rect,
						XNSpotLocation, &spot,
						XNFontSet, font_set,
						NULL );
			}
		else {
			preedit_attr = XVaCreateNestedList( NULL,
						XNArea, &rect,
						XNSpotLocation, &spot,
						XNForeground, bpixel,
						XNBackground, fpixel,
						XNFontSet, font_set,
						NULL );
			}
		}

	if(xim_mode & XIMStatusArea) {
		if(res_file) {
			status_attr = XVaCreateNestedList( NULL,
						XNArea, &StatusArea,
						XNFontSet, font_set,
						NULL );
			}
		else {
			status_attr = XVaCreateNestedList( NULL,
						XNArea, &StatusArea,
						XNForeground, bpixel,
						XNBackground, fpixel,
						XNFontSet, font_set,
						NULL );
			}
		}

	XGetIMValues(im, XNQueryInputStyle, &im_styles, NULL);
	n = 1;
	if(im_styles != (XIMStyles *)NULL) {
		for(i=0; i<im_styles->count_styles; i++) {
			if(xim_mode == im_styles->supported_styles[i]) {
				n = 0;
				break;
				}
			}
		}
	if(n) {
		printf("warning : Unsupport InputStyle. or No IMserver.\n");
		xim_mode = XIMPreeditNothing | XIMStatusNothing;
		}

	XMapWindow(display, window);
	XMapWindow(display, client);
	if(xim_mode & XIMStatusArea) {
		CreateStatus(display, window, client, font_height, 
			       fpixel, bpixel, &StatusArea);
		}

	if(xim_mode == (XIMPreeditPosition | XIMStatusArea)) {
		if(backend_mode) {
			ic = XCreateIC(im, 
				XNInputStyle, xim_mode,
				XNClientWindow, window,
				XNFocusWindow, client,
				XNPreeditAttributes, preedit_attr,
				XNStatusAttributes, status_attr,
				"XNExtXimp_Backfront", 1,
				NULL );
			}
		else {
			ic = XCreateIC(im, 
				XNInputStyle, xim_mode,
				XNClientWindow, window,
				XNFocusWindow, client,
				XNPreeditAttributes, preedit_attr,
				XNStatusAttributes, status_attr,
				"XNExtXimp_Backfront", 1,
				NULL );
			}
		}
	else if(xim_mode == (XIMPreeditPosition | XIMStatusNothing)) {
		if(backend_mode) {
			ic = XCreateIC(im, 
				XNInputStyle, xim_mode,
				XNClientWindow, window,
				XNFocusWindow, client,
				XNPreeditAttributes, preedit_attr,
				"XNExtXimp_Backfront", 1,
				NULL );
			}
		else {
			ic = XCreateIC(im, 
				XNInputStyle, xim_mode,
				XNClientWindow, window,
				XNFocusWindow, client,
				XNPreeditAttributes, preedit_attr,
				NULL );
			}
		}
	else {
		if(backend_mode) {
			ic = XCreateIC(im, 
				XNInputStyle, xim_mode,
		    		XNClientWindow, window,
		  		XNFocusWindow, client,
				"XNExtXimp_Backfront", 1,
		    		NULL );
			}
		else {
			ic = XCreateIC(im, 
				XNInputStyle, xim_mode,
		    		XNClientWindow, window,
		  		XNFocusWindow, client,
		    		NULL );
			}
		}
	if(ic == NULL) {
		printf("Error : XCreateIC() !\n");
		XCloseIM(im);
		exit(0);
		}

	if(res_file) {
		static char Resouce_name[] = "*";
		char *ret;

		ret = XSetICValues(ic, XNResourceName, Resouce_name, NULL);
		if(ret != NULL)
			printf("Error : XSetICValues() name = %s\n", ret);
		}

	JxRealizeText(display, client, &point);
	if(xim_mode & XIMPreeditPosition) {
		preedit_attr = XVaCreateNestedList(NULL,
					XNSpotLocation, &point,
					NULL );
		XSetICValues(ic, XNPreeditAttributes, preedit_attr, NULL);
		}

	for(;;) {
		XNextEvent(display, &event);
		if(XFilterEvent(&event, NULL) == True)
			continue;
		switch(event.type ) {
		case FocusIn :
			if(event.xany.window == window)
				XSetInputFocus(display, client, RevertToParent, CurrentTime);
			else if(event.xany.window == client) {
				XSetICFocus(ic);
				}
			break;
		case FocusOut :
			if(event.xany.window == client) {
				XUnsetICFocus(ic);
				}
			break;
		case Expose :
			if(event.xany.window == client)
				JxRedisplayText(display, client, font_set);
			break;
		case KeyPress :
			count = XmbLookupString(ic, &event, string, len, &keysym, NULL);
			if( count == 1 && string[0] == (0x1F&'c')) {	/* exit */
				goto exit;
				}
			if( count > 0 ) {
				JxWriteText(display, client, font_set, 
						count, string, &point);
				if(xim_mode & XIMPreeditPosition) {
					preedit_attr = XVaCreateNestedList(NULL,
						XNSpotLocation, &point,
						NULL );
                           		XSetICValues(ic,
						XNPreeditAttributes, preedit_attr,
						NULL );
					}
				}
			break;
		case MappingNotify :
			XRefreshKeyboardMapping( &event );
			break;
		case DestroyNotify :
			printf("Error : DestroyEvent !\n");
			break;
			}
		}
exit:
	XDestroyIC(ic);
	XCloseIM(im);
	JxDestroyTextWindow(display, client);
	XDestroyWindow(display, window);
	XCloseDisplay(display);
	return(0);
	}

static void
CreateStatus(display, window, client, font_height, fpixel, bpixel, StatusArea)
     Display	*display;
     Window   window, client;
     XRectangle     *StatusArea;
     {
     int            screen;
     Window root;
     int x, y;
     unsigned width, height, bw, depth;
     XSizeHints     sizehints;
     static int	status_hh;
     int	status_bw = 2;
     int	status_bw2 = status_bw * 2;
     int	status_height = font_height + font_height/2;

     screen  = DefaultScreen(display);
     root    = RootWindow(display, screen);
     XGetGeometry (display, window, &root, &x, &y, &width, &height, &bw, &depth);
     XUnmapWindow(display, window);
     status_hh = status_height + status_bw2;
     height += status_hh;
     sizehints.flags = USSize | PMinSize; 
     sizehints.width     = width;
     sizehints.min_width = width;
     sizehints.height     = height;
     sizehints.min_height = height;
     XSetNormalHints(display, window, &sizehints);
     XResizeWindow (display, window, width, height);
     XMapWindow(display, window);
     StatusArea->x = 0;
     StatusArea->y = height - status_hh;
     StatusArea->width  = width - status_bw2;
     StatusArea->height = status_height;
     }

GC      Jxgc_on, Jxgc_off;
int     Jxcx, Jxcy;
int     Jxcx_offset=2, Jxcy_offset=2;
int     Jxsfont_w, Jxwfont_w, Jxfont_height;
XRectangle	*Jxfont_rect;
int	Jxw_width, Jxw_height;

#define Jxmax_line	10
int	Jxsize[Jxmax_line];
char 	Jxbuff[Jxmax_line][128];
int	Jxline_no;
int	Jxline_height;

static Window
JxCreateTextWindow(display, window, x, y,
			w_width, w_height, w_bw, bpixel, fpixel,
			font_set, font_heightp)
     Display	*display;
     int	x, y, w_width, w_height, w_bw;
     unsigned long	fpixel, bpixel;
     XFontSet	font_set;
     int	*font_heightp;
     {
     Window	client;
     unsigned long	mask;
     XGCValues      gcvalues;
     XFontSetExtents *fset_extents;

     Jxw_width  = w_width;
     Jxw_height = w_height;

     client = XCreateSimpleWindow(display, window, x, y,
				w_width, w_height, w_bw, bpixel, fpixel);

     mask   = GCBackground | GCForeground;
     gcvalues.foreground = bpixel;
     gcvalues.background = fpixel;
     Jxgc_on  = XCreateGC(display, window, mask, &gcvalues);
     gcvalues.foreground = fpixel;
     gcvalues.background = bpixel;
     Jxgc_off = XCreateGC(display, window, mask, &gcvalues);

     Jxsfont_w = XmbTextEscapement(font_set, "$", 1);
     fset_extents = XExtentsOfFontSet(font_set);
     Jxfont_rect = &(fset_extents->max_logical_extent);
     *font_heightp = Jxfont_height = Jxfont_rect->height;
     Jxline_height = Jxfont_height + 2;
     Jxwfont_w = Jxfont_rect->width;
     return( client );
     }

static int
JxDestroyTextWindow(display, client)
     Display *display;
     Window	client;
     {
     XFreeGC(display, Jxgc_on);
     XFreeGC(display, Jxgc_off);
     XDestroyWindow(display, client);
     }

static int
JxRealizeText(display, client, point)
     Display *display;
     Window	client;
     XPoint    *point;
     {
     int	i;

     Jxline_no = 0;
     for(i=0; i < Jxmax_line; i++) {
	 Jxsize[i] = 0;
	 Jxbuff[i][0] = 0;
	 }
     Jxcx = Jxcx_offset;
     Jxcy = Jxcy_offset; 
     XFillRectangle(display, client, Jxgc_on, Jxcx, Jxcy, Jxsfont_w, Jxfont_height);
     point->x = Jxcx;
     point->y = Jxcy - Jxfont_rect->y;
     }

static int
JxRedisplayText(display, client, font_set)
     Display *display;
     Window	client;
     XFontSet	font_set;
     {
     int   i, fy;
     for(i=0; i < Jxmax_line; i++) {
         fy   = i * Jxline_height + Jxcy_offset - Jxfont_rect->y;
	 if( Jxsize[i] > 0 ){
             XmbDrawString(display, client, font_set, Jxgc_on, 
			   Jxcx_offset, fy, Jxbuff[i], Jxsize[i]);
		}
         }
     XFillRectangle(display, client, Jxgc_on,
			     Jxcx, Jxcy, Jxsfont_w, Jxfont_height);
     }

static int
_JxNextLine()
    {
     Jxcx = Jxcx_offset;
     Jxcy += Jxline_height;
     Jxline_no++;
     if(Jxline_no >= Jxmax_line) {
         Jxline_no = 0;
         Jxcy = Jxcy_offset;
         }
    Jxsize[Jxline_no] = 0;
    Jxbuff[Jxline_no][0] = 0;
    }

#define	LF	0x0a
#define	CR	0x0d
#define	TAB	0x09

static int
JxWriteText(display, client, font_set, len, string, point)
     Display *display;
     Window	client;
     XFontSet	font_set;
     int	len;
     char	*string;
     XPoint  *point;
     {
     int   fy;
     XFillRectangle(display, client, Jxgc_off, Jxcx, Jxcy, Jxsfont_w, Jxfont_height);
     if(len == 1 && 
	 (string[0] == LF || string[0] == TAB || string[0] == CR)) {
	 _JxNextLine();
         XFillRectangle(display, client, Jxgc_off, 0, Jxcy, Jxw_width, Jxfont_height);
	 }
     else {
	 if(Jxcx >= (Jxw_width - Jxwfont_w)
	 || (Jxsize[Jxline_no] + len) >=  256) {
	     _JxNextLine();
             XFillRectangle(display, client, Jxgc_off, 0, Jxcy, Jxw_width, Jxfont_height);
	 }
	 strncpy(&Jxbuff[Jxline_no][Jxsize[Jxline_no]], string, len);
	 Jxsize[Jxline_no] += len;
         fy   = -Jxfont_rect->y + Jxcy;
         XmbDrawString(display, client, font_set, Jxgc_on, 
			      Jxcx, fy, string, len);
	 Jxcx += XmbTextEscapement(font_set, string, len);
	 if(Jxcx >= Jxw_width) {
	     _JxNextLine();
             XFillRectangle(display, client, Jxgc_off, 0, Jxcy, Jxw_width, Jxfont_height);
	     }
	 }
     XFillRectangle(display, client, Jxgc_on, Jxcx, Jxcy, Jxsfont_w, Jxfont_height);
     point->x = Jxcx;
     point->y = Jxcy - Jxfont_rect->y;
     }
