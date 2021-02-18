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
 * This program is a descendent of hwwin.c, but no longer enqueues the
 * strokes itself.  Instead, it simply fields whatever stroke events are sent
 * its way, requests the stroke data, and draws them out on the screen.
 * Based on what mouse button has been pressed, it changes its foreground and 
 * background colors, and either copies the strokes from the server or moves 
 * the data.  If the stroke doesn't start within the window, it redirects the
 * stroke.
 */

/* 
 * There are two modules in this program: the one you are reading now,
 * PLIcatch.c, which is the application itself.  The second module,
 * wsimple.c, is lifted from the MIT tape.  Many of the MIT clients use this
 * support package.  It is a suite of convenience routines that fulfills the
 * basic set of operations required of a no-frills X11 client .
 * You can understand what most of these routines do by observing how they
 * are used in hwwin.c.  However there are some routines in wsimple.c that
 * have not been used in the demo program.
 *                                          Michael Sacks - 05/31/89
 ******   modified for x11r5/rs6k protocol  Doris Chow    - 10/03/91
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
   * this is the icon used when the window manager iconifies the program; I 
   * made it using the bitmap program in x11r3/bin
   */
#include "pli.icon"

  /* 
   * defaults that will be used in wsimple.h - note that these must be defined 
   * before wsimple.h is included
   */
#define TITLE_DEFAULT "proof"
#define ICON_NAME_DEFAULT "proof"
#define DEFX_DEFAULT 50
#define DEFY_DEFAULT 50
#define DEFWIDTH_DEFAULT 600
#define DEFHEIGHT_DEFAULT 200
#define BORDER_WIDTH_DEFAULT 4
#define REVERSE_DEFAULT 0
#define BORDER_DEFAULT "black"
#define BACK_COLOR_DEFAULT "white"
#define FORE_COLOR_DEFAULT "black"
#define BODY_FONT_DEFAULT "ncenb18"
#define MIN_X_SIZE 200
#define MAX_X_SIZE 600
#define MIN_Y_SIZE 100
#define MAX_Y_SIZE 400
  /* for window sizing */
#define SMALL 1
#define OK 0

/* 
 * We cache strokeid's and stroke data locally.  This define says how big to 
 * make our local storage.  This is a candidate for being a command line 
 * option. 
 */
#define LOCALSTROKESMAX 128
  
#include "wsimple.h"

  /* 
   * these variables contain offsets that are added to the xy data as it is
   * read in from the data file.  They provide a means of shifting the origin
   * of the strokes relative to the root window.  These offsets can be set
   * from the command line with -xorg and -yorg parameters; but one should
   * provide meaningful defaults; defaults of 0 are acceptable; negative
   * values are also acceptable.  Because I am currently running on the
   * megapel I provided defaults of 100.  No error checking is done, so be
   * sure not to shift the data off of the screen altogether.  The offsets
   * are added to the stroke data just once: when they are read in from the
   * input file.  See GetStrokesFromFile() in PLIfileio.c
   */
int x_offset = 100;
int y_offset = 100;

/* for our one and only font */
XFontStruct *myfont;

/* 
 * on alternate expose events, the program calls either PLICopyStrokeToClient
 * or PLIMoveStrokeToClient; to show this graphically I swap the foreground
 * and background colors on alternate expose events; these GC's are used to
 * accomplish the swap.
 */
GC normal_gc, reverse_gc, active_gc;

/*
 * Report the syntax for program; X_USAGE is a macro defined twice in 
 * wsimple.h.  Obviously, the second declaration is what counts
 */
void usage()
{
  fprintf(stderr,
	  "%s usage:  %s %s ", program_name,program_name,X_USAGE);
  fprintf (stderr, 
	   "[-fn <fontname>]\n              [-xorg {x stroke offset}] [-yorg {y stroke offset}] \n");
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
  XStrokeEvent *strokeeventP;
  /* holds the stroke data after it has been read in and massaged */
  PLIStroke **strokeArray;
  int numstrokes;        /* 
  int numstrokes;         * how many strokes in stroke array */

  /* 
   * This array holds the strokeid's of the strokes that have been copied but
   * not yet deleted from the server.  This strategy works for the demo
   * program because it is always the case that we do a copy or move
   * immediately upon receiving the stroke event.  However, if you did not
   * want to do this, you would have to maintain an array of stroke events
   * because you need to know how much storage to pass to the copy or move
   * routine, and it's the stroke event that contains this information. If 
   * you already have the stroke data and all you want to do is delete or
   * redirect strokes at some future time, then all you need to save is
   * the strokeids, as I have done here.
   */
  CARD16 strokeIDs[LOCALSTROKESMAX];

  /* 
   * This array stores the stroke data that we have copied or moved from the 
   * server.  We use it to repaint the window on an Expose event.
   *
   * A convenient way of getting storage for stroke data is to use the
   * macro MALLOCPLISTROKE ( from PLIsstk.h )with a pointer to the stroke 
   * event. 
   */
  PLIStroke *localstrokes[LOCALSTROKESMAX];
  int strokeindex;        /* index into the above arrays */
  int window_size = 0;
  
  /* returned by PLIRegisterExtension() - declared in XPLIlib.h */
  PLIExtData *pextdata;
  PLIStroke *strokeP;

  /* 
   * scale_factor is the power of 2 that is passed to the tc2dc and dc2tc 
   * macros defined in plicoord.h.  This value is part of the data returned 
   * by the PLIRegisterExtension() routine.  Once again, as a reminder, 
   * stroke data sent to the server by PLIEnqueueStrokes() is scaled, 
   * root-window relative.  The coordinates received in a stroke event are 
   * unscaled, event-window relative.  The data returned by 
   * PLIMoveStrokeToClient or PLICopyStrokeToClient are scaled event-window
   * relative.  You will need the macros tc2dc and dc2tc to convert stroke
   * data.
   */
  int i, scale_factor;
  char keybuf[10];    /* for finding out what key was pressed */
  int line_width, line_style, cap_style, join_style;
  int width, height, display_width, display_height;
  int docopy = 0;
      
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
    if (!strcmp("-xorg", argv[i]))
      {
	
	if (++i >= argc) usage();
	x_offset = atoi(argv[i]);
	continue;
      }
    if (!strcmp("-yorg", argv[i]))
      {
	
	if (++i >= argc) usage();
	y_offset = atoi(argv[i]);
	continue;
      }
    if (!strcmp("-fn", argv[i]) || !strcmp("-font", argv[i])) {
      if (++i >= argc) usage ();
      body_font_name = argv[i];
      continue;
    }
    if (argv[i][0] == '-') usage();
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
   * Create pixmap of depth 1 (bitmap) for icon; this data is in the 
   * include file pli.icon 
   */
  icon_pixmap = XCreateBitmapFromData(dpy, RootWindow(dpy,screen), 
				      icon_bitmap_bits,
                                      icon_bitmap_width, 
				      icon_bitmap_height);
  
  /* 
   * this routine creates the window, sets the size hints for the window 
   * manager, resolves the colors, sets the window border, and sets the icon 
   * bitmap - wsimple.c
   */
  Create_Default_Window();

  /* make a cursor for the window */
  cursor = XCreateFontCursor (dpy, XC_bogosity);
  if (cursor != None) {
    XDefineCursor (dpy, wind, cursor);
  }

  /* 
   * create two GC's for text and line drawing - we care about foreground 
   * color, background color, font, and line drawing attributes
   */
  normal_gc = Get_Default_GC();
  reverse_gc = Get_Default_GC();
  /* set the line attributes for drawing the lines - once for each GC */
  line_width = 0;
  line_style = LineSolid;
  cap_style = CapRound;
  join_style = JoinRound;
  XSetLineAttributes (dpy, normal_gc, line_width,line_style, cap_style,
                      join_style);
  XSetLineAttributes (dpy, reverse_gc, line_width,line_style, cap_style,
                      join_style);
  /* flip the foreground and background colors in reverse_gc */
  XSetForeground(dpy,reverse_gc,background);
  XSetBackground(dpy,reverse_gc,foreground);
  /* start off normal */
  active_gc = normal_gc;
    /* register the extension */
  pextdata = PLIRegisterExtension(dpy);
  /* get the scale factor */
  scale_factor = pextdata->scalefac;
  

  /* set the input mask for the standard stuff plus stroke events */
  XSelectInput(dpy, wind, ExposureMask | KeyPressMask |
               ButtonPressMask | StructureNotifyMask | 
	       pextdata->eventMask);

  strokeindex = 0;       /* no pending stroke events */
  
  /* 
   * Display the window  - caution:  never start drawing until you get an 
   * expose event 
   */
  XMapWindow(dpy,wind);

  /* 
   * this is the main loop of the application.  Like all X applications it is
   * event-driven.  In the case of an Expose event, repaint the window
   * background using the current background color, and then draw the text
   * using the current graphics context.  In the case of a ConfigureNotify
   * event, set the new current width and height, and make sure that the
   * window has not been resized too small.  Under uwm, the user is not
   * permitted to resize you smaller than the size hints, so this code is
   * unnecessary.  But you can't assume window-manager policy.  In the case
   * of a KeyPress, check to see if the user has pressed 'q'; if so, exit
   * gracefully.  In the case of a ButtonPress, if it's the left mouse
   * button, set the window background color and active_GC to normal, if it's
   * the right mouse button, set them to reverse ( flip foreground and
   * background ).  If it's the middle mouse button, delete our local cache
   * of the strokes we have copied from the server.  Then clear the window
   * using the current and possibly new background color and draw the text
   * using the current and possibly new GC.  Also set the flag that says
   * whether to copy or move data from the server. In the case of a stroke
   * event, if it's a STARTOFSTROKE or ENDOFSTROKE, and the stroke is not in
   * the window, then reject the stroke.  In the case of an ENDOFSTROKE,
   * either copy the stroke data over or move it, depending on which mouse
   * button was pressed.  Either way, once you have the stroke data, convert
   * it to unscaled data, draw it out using XDrawLines(), and cache it away
   * in the localstrokes array.  We also check to see if our localstrokes
   * array is getting too full.  If it is, we delete the strokes both locally
   * and from the server.  A word about redirecting strokes: because we are
   * doing mouse-emulation with the pen, it is important for a PLI-aware
   * client to redirect as soon as it possibly can.  If you can tell that you
   * don't want the stroke by looking at proximity points then redirect right
   * then and there.  Certainly you should be able to redirect most of the
   * time based on start of stroke.  Do it as quickly as you can.  Remember,
   * you are not the only client on the screen!
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
          if (window_size == SMALL)
            /* if window was resized too small to use */
            TooSmall(wind, active_gc, body_font);
          else
            {
	      /* 
	       *paint the window background with the current background color
	       */
	      XClearWindow(dpy,wind);
	      /* redraw the lines */
	      for ( i=0;i<strokeindex;i++)
			  XDrawLines(dpy,wind,active_gc,localstrokes[i]->xys,
				     localstrokes[i]->numXYs,CoordModeOrigin);
	    }
          break;
        case ConfigureNotify:
	  /* 
	   * window has been resized, change width and height to send to 
	   * draw_text in next Expose
	   */
	  width = report.xconfigure.width;
          height = report.xconfigure.height;
          if ((width < size_hints.min_width) ||
              (height < size_hints.min_height))
            window_size = SMALL;
          else
            window_size = OK;
          break;
        case ButtonPress:
	  /* 
	   * based on whether we have a left or right button press, we set 
	   * the window background color and the active_GC.  We also decide 
	   * whether to copy or move data from the server on our next stroke 
	   * event.  We then redraw  the window background and text.
	   */
	  switch (report.xbutton.button) 	    
	    {
	    case Button1:
	      XSetWindowBackground(dpy,wind,background);
	      active_gc = normal_gc;
	      /* delete all the strokes we have copied but not moved  */
	      for ( i=0;i<strokeindex;i++)
		free(localstrokes[i]);
	      strokeindex = 0;
	      break;
	    }
	  /* paint the new background color */
	  XClearWindow(dpy,wind);
	  /* put text into the window using the new graphics context */
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
	      /* 
	       * get rid of any strokes that have been copied but not deleted 
	       */
	      for ( i=0;i<strokeindex;i++)
		{
		  free(localstrokes[i]);

		  if ( docopy )
		    if (!(PLIDeleteStrokes(dpy,
					   strokeIDs[i])))
		      printf("PLIDeleteStrokes failed    \n");
		}

	      XCloseDisplay(dpy);
	      exit(1);
	    }
	  break;

        default:
	  /* 
	   * any other kind of event except for a stroke event falls on 
	   * the floor
	   */

	  if (report.type == pextdata->eventCode)
            { /* stroke event */
	      strokeeventP = (XStrokeEvent *) &report;
	      switch(strokeeventP->detail & DET_TYPE_MASK)
		{ /* stroke event type switch */
		case DET_MOTION:  
		  /* proximity stroke - pen is hovering tablet */
		  break;
		  
		case DET_START:
		  /* beginning of stroke */
		  if ( (strokeeventP->detail & DET_INOUT_MASK) != DET_STRT_IN)
		    /* 
		     *  The stroke is outside the window, so reject it; 
		     *  a RejectStroke request will delete pending 
		     *  strokeevents from the client's event queue 
		     *  and delete previous strokes from server.
		     */
		    if (!(PLIRejectStroke(dpy,strokeeventP->strokeID)))
		      printf("PLIRejectStroke failed    \n");
		  break;
		  
		case DET_STROKE:
		  /* partial stroke */
		  break;
		  
		case DET_END:
		  /* end of stroke */
		  /* This indicates complete stroke to process */
		  if ( (strokeeventP->detail & DET_INOUT_MASK) != DET_ALL_IN)
		    {
		      if (!(PLIRejectStroke(dpy,strokeeventP->strokeID)))
			printf("PLIRejectStroke failed    \n");
		    }
		  
		  else
		    { 
		      /* This very useful macro is defined in PLIsstk.h */
		      strokeP = MALLOCPLISTROKE(strokeeventP);
		      havestroke = False;
		      /* we copy or move on alternate expose events */
		      if ( docopy )
			{ /* docopy */
			  if (PLICopyStrokeToClient(dpy,wind,strokeeventP->strokeID,strokeP, strokeeventP->numXYs) )
			    {
			      havestroke = True;
			      /* 
			       * stroke is within the window:  get the stroke 
			       * xys based on which mouse button was pressed and 
			       * released; if we have no more room to store strokes 
			       * locally, then delete them locally and delete them 
			       * from the server
			       */
			      if (strokeindex == LOCALSTROKESMAX)
				{
				  /* 
				   * no more space for strokes - delete the 
				   * strokes locally and then delete them 
				   * from the server
				   */
				  for ( i=0;i<LOCALSTROKESMAX;i++)
				    {
				    free(localstrokes[i]);
				    if (!(PLIDeleteStrokes(dpy,
					  strokeIDs[i])))	
				      printf("PLIDeleteStrokes failed\n");
				  }
				  strokeindex = 0;
				}
			    }
			  else
			    printf("PLICopyStrokeToClient failed\n");
			} /* docopy */
		      else
			{ /* domove */
			  if (PLIMoveStrokeToClient(dpy,wind,strokeeventP->strokeID,strokeP,strokeeventP->numXYs))
			    havestroke = True;
			  else		
			    printf("PLIMoveStrokeToClient failed    \n");
			} /* domove */
		      if ( havestroke )
			{
			  
			  /* 
			   * at this point, I have my stroke data; I simply 
			   * convert it to to unscaled xy's and draw it out 
			   * using XDrawlines; but you could just as easily 
			   * ship it off to a recognizer or segmenter; 
			   */
			  for ( i = 0; i < strokeP->numXYs;i++)
			    {
			      /* 
			       * the coordinate conversion macros tc2dc and dc2dc 
			       * are defined in plicoord.h.  They are convenient 
			       * and accurate for converting between the two 
			       * coordinate systems.  Use them!
			       */
			      strokeP->xys[i].x = tc2dc(strokeP->xys[i].x,scale_factor,0);
			      strokeP->xys[i].y = tc2dc(strokeP->xys[i].y,scale_factor,0);
			    }
			      
			  /* 
			   * Save the stroke and strokeid locally in case you 
			   * want to redraw on expose or to delete later.  
			   * This code is not quite correct:  if we have 
			   * moved the data from the server, then there is no 
			   * point in storing the strokeID, since the stroke 
			   * has already been deleted from the server.  
			   * However, there is no harm in asking to delete a 
			   * stroke that has already been deleted, as long as 
			   * the stroke is not older than the server's 
			   * current oldest stroke.
			   */
			  strokeIDs[strokeindex] = strokeeventP->strokeID;
			  localstrokes[strokeindex++] = strokeP;
			  
			  /* draw the stroke */
			  
			  XDrawLines(dpy,wind,active_gc,strokeP->xys,strokeP->numXYs,CoordModeOrigin);
			}
		    }
		  /* stroke is within the window */
		  break;
		  
		case DET_WITHDRAWN:
		  /*
		   * The PLI protocol allows for a stroke to be
		   * broadcasted to all windows which are touched
		   * by the stroke or grabbed the stroke focus.
                   * Given this equal distribution, one of the
		   * potential clients may lay absolute claim to
		   * the stroke and force it to be deleted and withdraw
		   * from the other clients.  This message indicates
		   * that a current stroke that this client may have
                   * been expecting/pending is no longer available
		   * from the server.
		   */
		  printf("\nStroke %d withdrawn for window \n",
			 strokeeventP->strokeID);
		  break;

		default:
		  printf("\nUnknown stroke report type %x\n",
			 strokeeventP->type & 0x0f);
		  break;
		  
		} /* stroke event type switch */
	      
            } /* stroke event */
	  
          break;
	  
	  
        }  /* end switch */
    }     /* end while */
}


TooSmall(win, gc, font_info)
     Window win;
     GC gc;
     XFontStruct *font_info;
{
  char *string1 = "Too Small";
  int y_offset, x_offset;
  
  y_offset = font_info->max_bounds.ascent +2;
  x_offset = 2;
  
  /* output text, centered on each line */
  XDrawString(dpy, win, gc, x_offset, y_offset, string1,
              strlen(string1));
}

