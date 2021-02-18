/*
 * Copyright IBM Corporation 1991
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/* 
 * The purpose of this program is to allow the pen to do mouse emulation.  It 
 * creates window with three tiles on the screen.  The user can toggle 
 * between mouse buttons 1, 2 or 3.  When the extension converts a stroke 
 * event into a mouse event, it uses the mouse button selected through this 
 * client.  The reverse-videoed tile is the currently selected mouse button.
 * - Michael Sacks - 05/31/89
 * - revised to support x11r5-rs6k xpli protocol - Doris Chow  10-04-91
 */
#include <stdio.h>
  /* standard X11 includes */
#define NEED_EVENTS
#include <X11/Xlibint.h>        /* includes Xproto.h and Xlib.h */
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Xos.h>
/* 
 * PLI extension includes:  XPLIlib.h includes PLIproto.h which includes 
 * PLIsstk.h which includes PLIcoord.h
 */
#include <XPLIlib.h>
  /* 
   * defaults that will be used in wsimple.h - note that these must be defined 
   * before wsimple.h is included
   */
#define TITLE_DEFAULT "PLIpm"
#define ICON_NAME_DEFAULT "PLIpm"
#define DEFX_DEFAULT 50
#define DEFY_DEFAULT 50
#define DEFWIDTH_DEFAULT 90
#define DEFHEIGHT_DEFAULT 30
#define BORDER_WIDTH_DEFAULT 4
#define REVERSE_DEFAULT 0
#define BORDER_DEFAULT "black"
#define BACK_COLOR_DEFAULT "white"
#define FORE_COLOR_DEFAULT "black"
#define BODY_FONT_DEFAULT "ncenb18"
#define MIN_X_SIZE 90
#define MAX_X_SIZE 150
#define MIN_Y_SIZE 30
#define MAX_Y_SIZE 50
  
#include "wsimple.h"

/* children of the parent window */
Window LeftButton, MiddleButton, RightButton;

/* width and height of these children - each 1/3 of the width */
int subwidth = MIN_Y_SIZE;
int subheight = MIN_Y_SIZE;


/* for our one and only font */
XFontStruct *myfont;

/* 
 * on alternate expose events, the program calls either PLICopyStrokeToClient
 * or PLIMoveStrokeToClient; to show this graphically I swap the foreground
 * and background colors on alternate expose events; these GC's are used to
 * accomplish the swap.
 */
GC normal_gc, reverse_gc;
GC buttonGCs[3];  /* keep track of the GC's for each mouse button */
unsigned long buttonBackgrounds[3];  /* ditto for background colors  */



/*
 * Report the syntax for program; X_USAGE is a macro defined twice in 
 * wsimple.h.  Obviously, the second declaration is what counts
 */
void usage()
{
  fprintf(stderr,
	  "%s usage:  %s %s ", program_name,program_name,X_USAGE);
  fprintf (stderr, 
	   "[-fn <fontname>]\n");
  exit(1);
}


void main(argc, argv)
     int argc;
     char **argv;
     
{
  
  /* 
   * I use one of the standard X11 cursors for my window; the standard 
   * cursors are listed in X11/cursorfont.h - see also O'Reilly Volume 2 
   * pages 641-642
   */
  Cursor cursor;
  XEvent report;       /* passed to XNextEvent */

  int window_size = 0;
  
  /* returned by PLIRegisterExtension() - declared in XPLIlib.h */
  PLIExtData *pextdata;
  int width, height, display_width, display_height, i;
  char keybuf[20];

  /* macro from wsimple.h to get program name */
  INIT_NAME;

  /* 
   * open the display connection, get the Xdefaults, parse the standard X11 
   * command line args - in wsimple.h.  The variables dpy and screen are 
   * defined in wsimple.h and are initialized by this routine
   */
   Get_X_Options(&argc, argv);

  /* now parse the application specific args */
  for (i = 1; i < argc; i++) {
    if (!strcmp("-help", argv[i])) usage ();
    if (!strcmp("-", argv[i]))
      continue;
    if (!strcmp("-fn", argv[i]) || !strcmp("-font", argv[i])) {
      if (++i >= argc) usage ();
      body_font_name = argv[i];
      continue;
    }
  }
  

  /* get the font - wsimple.c */
  myfont = Open_Font(body_font_name);

  /* 
   * the following are some of the standard X11 display macros - see 
   * Scheifler and Gettys pages 13-22
   */
  display_width = DisplayWidth(dpy,screen);
  display_height = DisplayHeight(dpy,screen);
  
  /* 
   * Resolve the X options:  geometry, icon_name, font, bitmapfile,
   * but not colors - wsimple.c
   */
  Resolve_X_Options();

  /* 
   * this routine creates the window, sets the size hints for the window 
   * manager, resolves the colors, sets the window border, and sets the icon 
   * bitmap - wsimple.c
   */
  Create_Default_Window();

  /* make a cursor for the window */
  cursor = XCreateFontCursor (dpy, XC_left_ptr);
  if (cursor != None) {
    XDefineCursor (dpy, wind, cursor);
  }
  

  /* 
   * create two GC's for text and line drawing - we care about foreground 
   * color, background color, font, and line drawing attributes
   */
  normal_gc = Get_Default_GC();
  reverse_gc = Get_Default_GC();
  /* flip the foreground and background colors in reverse_gc */
  XSetForeground(dpy,reverse_gc,background);
  XSetBackground(dpy,reverse_gc,foreground);
    /* register the extension */
  pextdata = PLIRegisterExtension(dpy);

  /* 
   * set the input mask for the parent window for standard stuff; note that 
   * although we use the stroke extension, we want the pen events as mouse 
   * events - this gets rid of the pen grab problem.
   */
  XSelectInput(dpy, wind, KeyPressMask | ButtonPressMask | StructureNotifyMask );
  
  
  LeftButton = XCreateSimpleWindow(dpy,wind,0,0,30,30,0,0,foreground);
  MiddleButton = XCreateSimpleWindow(dpy,wind,30,0,30,30,0,0,background);
  RightButton = XCreateSimpleWindow(dpy,wind,60,0,30,30,0,0,background);

  XSelectInput(dpy, LeftButton, ExposureMask | ButtonPressMask  );
  XSelectInput(dpy, MiddleButton, ExposureMask | ButtonPressMask  );
  XSelectInput(dpy, RightButton, ExposureMask | ButtonPressMask  );
  
  
  /* start with the active mouse button set to the left mouse button */
  buttonGCs[0] = reverse_gc;
  buttonGCs[1] = normal_gc;
  buttonGCs[2] = normal_gc;

  PLISetPointerButton(dpy,Button1);
  
    /* 
   * Display the window  - caution:  never start drawing until you get an 
   * expose event 
   */
  XMapSubwindows(dpy,wind);
  XMapWindow(dpy,wind);

  /* 
   * The main loop of this application simply figures out which window the 
   * pointer event comes from and makes that the active pointer button.  If 
   * it gets a 'q' as a key press event, it quits.
   */
  /* standard X11 forever main loop */
  while (1)
    {
      int havestroke;
      int count;
      

      XNextEvent(dpy, &report);
      switch (report.type)
        {
	  
        case Expose:
          /* get rid of all other Expose events on the queue */
          while (XCheckTypedEvent(dpy, Expose, &report));
	  XClearWindow(dpy,wind);
	  XClearWindow(dpy,LeftButton);
	  XClearWindow(dpy,MiddleButton);
	  XClearWindow(dpy,RightButton);
	  /* put text into the window using the current graphics context */
	  draw_text(LeftButton, buttonGCs[0], body_font, subwidth, subheight,"1");
	  draw_text(MiddleButton, buttonGCs[1], body_font, subwidth, subheight,"2");
	  draw_text(RightButton, buttonGCs[2], body_font, subwidth, subheight,"3");
	  break;

        case ConfigureNotify:
	  /* 
	   * window has been resized, change width and height to send to 
	   * draw_text in next Expose
	   */
	  if ( report.xconfigure.window == wind )
	    {
	    
	    width = report.xconfigure.width;
	    subwidth = width / 3;
	    subheight = report.xconfigure.height;
	    XMoveResizeWindow(dpy,LeftButton,0,0,subwidth,subheight);
	    XMoveResizeWindow(dpy,MiddleButton, subwidth,0,
			      subwidth,subheight);
	    /* make the third window cover the rest of the parent window */
	    XMoveResizeWindow(dpy,RightButton,( 2 * subwidth ) ,0,
			      ( subwidth + ( width % 3 )),subheight);
	  }
          break;
      
        case ButtonPress:
	  /* 
	   * based on whether we have a left or right button press, we set 
	   * the window background color and the active_GC.  We also decide 
	   * whether to copy or move data from the server on our next stroke 
	   * event.  We then redraw  the window background and text.
	   */
	  if ( report.xbutton.window == LeftButton )
	    {
	      
	      XSetWindowBackground(dpy,LeftButton,foreground);
	      XSetWindowBackground(dpy,MiddleButton,background);
	      XSetWindowBackground(dpy,RightButton,background);
	      buttonGCs[0] = reverse_gc;
	      buttonGCs[1] = normal_gc;
	      buttonGCs[2] = normal_gc;
	      XClearWindow(dpy,LeftButton);
	      XClearWindow(dpy,MiddleButton);
	      XClearWindow(dpy,RightButton);
              draw_text(LeftButton, buttonGCs[0], body_font, subwidth, subheight,"1");
              draw_text(MiddleButton, buttonGCs[1], body_font, subwidth, subheight,"2");
              draw_text(RightButton, buttonGCs[2], body_font, subwidth, subheight,"3");
	      PLISetPointerButton(dpy,Button1);
	    }
	  
	  else if ( report.xbutton.window == MiddleButton )
	    {
	      XSetWindowBackground(dpy,LeftButton,background);
	      XSetWindowBackground(dpy,MiddleButton,foreground);
	      XSetWindowBackground(dpy,RightButton,background);
	      buttonGCs[0] = normal_gc;
	      buttonGCs[1] = reverse_gc;
	      buttonGCs[2] = normal_gc;
	      XClearWindow(dpy,LeftButton);
	      XClearWindow(dpy,MiddleButton);
	      XClearWindow(dpy,RightButton);
              draw_text(LeftButton, buttonGCs[0], body_font, subwidth, subheight,"1");
              draw_text(MiddleButton, buttonGCs[1], body_font, subwidth, subheight,"2");
              draw_text(RightButton, buttonGCs[2], body_font, subwidth, subheight,"3");
	      PLISetPointerButton(dpy,Button2);
	    }
      
	  else if ( report.xbutton.window == RightButton )

	    {
	      XSetWindowBackground(dpy,LeftButton,background);
	      XSetWindowBackground(dpy,MiddleButton,background);
	      XSetWindowBackground(dpy,RightButton,foreground);
	      buttonGCs[0] = normal_gc;
	      buttonGCs[1] = normal_gc;
	      buttonGCs[2] = reverse_gc;
	      XClearWindow(dpy,LeftButton);
	      XClearWindow(dpy,MiddleButton);
	      XClearWindow(dpy,RightButton);
              draw_text(LeftButton, buttonGCs[0], body_font, subwidth, subheight,"1");
              draw_text(MiddleButton, buttonGCs[1], body_font, subwidth, subheight,"2");
              draw_text(RightButton, buttonGCs[2], body_font, subwidth, subheight,"3");
	      PLISetPointerButton(dpy,Button3);
	    }
	  break;
	  
	case KeyPress:
	  count = XLookupString(&report,keybuf,10,0,0);
	  keybuf[count] = 0;
	  /* exit only if user presses q */
	  if (!(strcmp(keybuf,"q")))
	    {
	      XUnloadFont (dpy, body_font->fid);
	      XFreeGC (dpy, normal_gc);
	      XFreeGC (dpy, reverse_gc);
	      XCloseDisplay(dpy);
	      exit(0);
	    }
	  break;
	}
    }
}



draw_text(win, gc, font_info, win_width, win_height, string)
     Window win;
     GC gc;
     XFontStruct *font_info;
     unsigned int win_width, win_height;
     char *string;
     
{
  int len;
  int width;
  int font_height;
  int y_offset;
  
  
  /* need length for both XTextWidth and XDrawString */
  len = strlen(string);
  
  /* get string widths for centering */
  width = XTextWidth(font_info, string, len);
  font_height = font_info->max_bounds.ascent + font_info->max_bounds.descent;

  /* 
   * note:  in general you would want to subtract the max_bounds.descent, but 
   * in this case we no there are no descenders, since we know what the 
   * string is
   */
  y_offset = win_height  - ( win_height - font_height ) / 2;
    
  /* set text to center of window */
  
  /* output text, centered on each line */
  XDrawString (dpy,win,gc,(win_width - width)/2,
	       (int)(y_offset),string,len);

}

