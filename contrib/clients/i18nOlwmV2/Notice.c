/*
 *      (c) Copyright 1989, 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) Notice.c 50.6 91/08/26 Crucible";

#ifdef OW_I18N
#include "i18n.h"
#endif
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <olgx/olgx.h>

#ifdef OW_I18N
#include "ollocale.h"
#endif
#include "olwm.h"
#include "notice.h"
#include "globals.h"

extern Graphics_info	*olgx_gistext;
extern Graphics_info	*olgx_gisbutton;

extern int		PointInRect();

#define	NOTICE_EVENT_MASK	( ButtonPressMask | ButtonReleaseMask \
				  | PointerMotionMask | KeyPressMask )
#define	NOTICE_ATTR_MASK	( CWBorderPixel | CWBackPixel | CWSaveUnder )

/* difference between inside beveled box and outside beveled box */
#define BORDER_WIDTH		5	
#define	MIN_BOX_SIDE		( 15 + BORDER_WIDTH )	/* betw side & text */
#define	MIN_BOX_TOP		( 15 + BORDER_WIDTH )	/* betw top & text */
#define	MIN_BOX_BOTTOM		( 15 + BORDER_WIDTH )	/* betw bot & text */
#define MIN_BUTTON_SPACE	15		/* space between buttons */
#define MIN_BUTTON_VSPACE	15		/* space above buttons */
#define MIN_STRING_VSPACE	5		/* space above/below strings */

typedef struct {
	int		x;
	int		y;
	unsigned int	width;		/* space taken up by text */
	unsigned int	fullWidth;	/* width including endcaps */
} noticeButtonDetails;

typedef struct {
	Display			*dpy;
	Window			window;
	unsigned int		buttonHeight;
	unsigned int		fontHeight;
	unsigned int		boxHeight;
	unsigned int		boxWidth;
	int			x;
	int			y;
	int			totalButtonWidth;
	noticeButtonDetails	*buttonInfo;
#ifdef OW_I18N
	int			numString;
	wchar_t			**stringTexts;
#endif
} noticeBoxDetails;

static void calculateBoxDimensions();
static void drawNoticeBox();
static int  noticeEventLoop();

/*
 * NoticeBox -- pop up a box which forces the user to answer
 * 		  a question using the buttons
 *
 * Arguments:
 *	dpy	    - pointer to current display
 *	screen	    - index to current screen
 *	noticeBox   - pointer to NoticeBox structure:
 *		numButtons	(number of buttons)
 *		defaultButton	(index into buttonText for mouse warp)
 *		buttonText	(array of strings for button text)
 *		numStrings	(number of descriptive strings)
 *		stringText	(array of strings for description)
 *		boxX		(box origin (-1 =use default/centered))
 *		boxY		(box origin (-1 =use default/centered))
 *
 *	Default placement of the box is centered in the display
 *	Returns -1 on failure (0 for 0th button, 1 for 1st button, etc.)
 */
int
UseNoticeBox( dpy, screen, noticeBox )
Display		*dpy;
int		screen;
NoticeBox	*noticeBox;
{
	XSetWindowAttributes	attributes;
	int			buttonSelected;
	noticeBoxDetails	boxDetails;
	int			pointerX, pointerY;
	int			dummyInt;
	Window			dummyWin;
	int			ii;
	int			defaultButton;

	/* set up window attributes structure */
	attributes.border_pixel = GRV.Bg2Color;
	attributes.background_pixel = GRV.Bg1Color;
	attributes.save_under = True;

	/* set up initial elements of boxDetails structure */
	boxDetails.dpy = dpy;
	boxDetails.buttonInfo = 
		(noticeButtonDetails *)MemAlloc( noticeBox->numButtons 
			* (unsigned int)sizeof(noticeButtonDetails) );

	/* figure out size to make window and where to put it */
	calculateBoxDimensions( screen, noticeBox, &boxDetails ); 

	/* create window */
	boxDetails.window = XCreateWindow( dpy, 
					   RootWindow(dpy, 
						      DefaultScreen(dpy)),
				           boxDetails.x, boxDetails.y, 
					   boxDetails.boxWidth, 
					   boxDetails.boxHeight, 0, 
					   DefaultDepth(dpy, DefaultScreen(dpy)), 
					   InputOutput, CopyFromParent, 
				           NOTICE_ATTR_MASK,
				           &attributes );
	
	/* grab server and map window so box can be drawn here */
	XGrabServer( dpy );
	XMapRaised( dpy, boxDetails.window );

	/* only beep if beeps are selected */
	if (GRV.Beep != BeepNever)
		XBell(dpy,100);

	drawNoticeBox( noticeBox, &boxDetails );

	/* save current mouse position */
	XQueryPointer( dpy, RootWindow(dpy, DefaultScreen(dpy)),
		       &dummyWin, &dummyWin,
		       &pointerX, &pointerY,
		       &dummyInt, &dummyInt, &dummyInt );

	/* warp pointer to default button */
	defaultButton = noticeBox->defaultButton;
	if (GRV.PopupJumpCursor)
	    XWarpPointer( dpy, None, boxDetails.window, 0, 0, 0, 0, 
		      boxDetails.buttonInfo[defaultButton].x
		        + boxDetails.buttonInfo[defaultButton].fullWidth/2, 
		      boxDetails.buttonInfo[defaultButton].y
		        + boxDetails.buttonHeight/2 );

	XSelectInput( dpy, boxDetails.window, NOTICE_EVENT_MASK );
	buttonSelected = noticeEventLoop( noticeBox, &boxDetails );

	/* put the pointer back where we got it */
	/* REMIND we need to test to see if the pointer is still within the 
	* default button */
	if (GRV.PopupJumpCursor)
	    XWarpPointer( dpy, None, RootWindow(dpy, DefaultScreen(dpy)),
		      0, 0, 0, 0, pointerX, pointerY );

	/* free up the server as soon as we're done with it */
	XUngrabServer( dpy );

	/* destroy window */
	XDestroyWindow( dpy, boxDetails.window );
	MemFree( boxDetails.buttonInfo );
	return( buttonSelected );
}

/*
 * noticeEventLoop -- watch for events while in notice box
 *
 */
static int
noticeEventLoop( noticeBox, details )
NoticeBox		*noticeBox;
noticeBoxDetails	*details;
{
	XEvent	event;
	Bool	done = False;
	int	buttonSelected = -1;
	int	buttonDown = -1;
	Bool	buttonDrawnDown = False;
	int	ii;
	int	buttonState;		/* OLGX_NORMAL or OLGX_DEFAULT */
	int	keyboardGrabbed = False;

	/* take control until we get an answer */
	/* REMIND:  we're assuming the pointer can be grabbed, 
	 * since we were successfully called from some menu 
	 * REMIND: we may want to change the pointer during the grab
	 */
	XGrabPointer( details->dpy, details->window, False, 
		      NOTICE_EVENT_MASK,
		      GrabModeAsync, GrabModeAsync,
		      None, None, CurrentTime );

	/* REMIND: need to check return, in case someone else has
	 * grabbed the keyboard (what to do then? just XGrabKey(),
	 * or ignore keypress events?)
	 */
	keyboardGrabbed = ( GrabSuccess == XGrabKeyboard( 
					details->dpy, details->window,
					False, GrabModeAsync, 
					GrabModeAsync, CurrentTime ) );

	while ( !done )
	{
	    XNextEvent( details->dpy, &event );

	    /*
	     * Discard synthetic events
	     *
	     * This event loop is only interested in user events, so we
	     * don't even bother checking to see if this event is a mouse
	     * or keyboard event.
	     */
	    if (event.xany.send_event)
		continue;

	    switch (event.type)
	    {
		case ButtonPress:
		    /* first check to see if we're even in the notice box */
		    if ( !PointInRect( event.xbutton.x, 
				       event.xbutton.y,
				       0, 0,
				       details->boxWidth,
				       details->boxHeight ) )
			break;

		    /* if on one of the notice buttons, depress it */
		    for ( ii = 0 ; ii < noticeBox->numButtons ; ii++ )
		    {
			if ( PointInRect( event.xbutton.x, 
					  event.xbutton.y,
					  details->buttonInfo[ii].x, 
					  details->buttonInfo[ii].y,
					  details->buttonInfo[ii].fullWidth,
					  details->buttonHeight ) )
			{
			    buttonDown = ii;
			    buttonDrawnDown = True;
			    olgx_draw_button( olgx_gisbutton, 
					details->window, 
					details->buttonInfo[ii].x, 
					details->buttonInfo[ii].y,
					details->buttonInfo[ii].fullWidth, 0,
					noticeBox->buttonText[ii], 
					OLGX_INVOKED );
			    break;
			}
		    }
		    break;

		case ButtonRelease:
		    /* only a depressed button can be selected */
		    if ( PointInRect( event.xbutton.x, 
				event.xbutton.y,
				details->buttonInfo[buttonDown].x, 
				details->buttonInfo[buttonDown].y,
				details->buttonInfo[buttonDown].fullWidth,
				details->buttonHeight ) )
		    {
			buttonSelected = buttonDown;
			done = True;
		    }

		    if (!done)
		    {
			buttonDown = -1;
			buttonDrawnDown = False;
		    }

		    break;
		
		case MotionNotify:
		    /* check state of buttons */
		    if ( buttonDown < 0 )
			break;

		    if ( PointInRect( event.xbutton.x, 
				event.xbutton.y,
				details->buttonInfo[buttonDown].x, 
				details->buttonInfo[buttonDown].y,
			        details->buttonInfo[buttonDown].fullWidth,
			        details->buttonHeight ) )
		    {
			/* Moved within button down at start of motion.
			 * If the button was is drawn undepressed,
			 * make it go down.
			 */
			if ( !buttonDrawnDown )
			{
			    olgx_draw_button( olgx_gisbutton, 
				details->window, 
				details->buttonInfo[buttonDown].x, 
				details->buttonInfo[buttonDown].y,
				details->buttonInfo[buttonDown].fullWidth, 0,
				noticeBox->buttonText[buttonDown], 
				OLGX_INVOKED );

			    buttonDrawnDown = True;
			}
		    }
		    else
		    {
			/* Moved out of button that was down at start.
			 * If the button was previously drawn down,
			 * draw it undepressed.
			 */
			if ( buttonDrawnDown )
			{
			    /* check if needs "default" indicator */
			    if ( noticeBox->defaultButton == buttonDown )
			    	buttonState = OLGX_DEFAULT;
			    else
			    	buttonState = OLGX_NORMAL;

			    olgx_draw_button( olgx_gisbutton, 
				    details->window, 
				    details->buttonInfo[buttonDown].x,
				    details->buttonInfo[buttonDown].y,
				    details->buttonInfo[buttonDown].fullWidth,
				    0, noticeBox->buttonText[buttonDown], 
				    buttonState | OLGX_ERASE );

			    buttonDrawnDown = False;
			}
		    }

		    break;

		case KeyPress:
		    if (keyboardGrabbed &&
			event.xkey.keycode == GRV.ConfirmKey.keycode )
		    {
			buttonSelected = noticeBox->defaultButton;
			done = True;
		    }

		    break;

		default:
		    break;
	    }
	}

	/* release control */
	XUngrabPointer( details->dpy, CurrentTime );
	if ( keyboardGrabbed )
		XUngrabKeyboard( details->dpy, CurrentTime );
	XFlush( details->dpy );

	return( buttonSelected);
}

/*
 * calculateBoxDimensions -- determine size of box needed, etc. based on
 * 		information passed in in noticeBox structure.  Return
 *		values in boxDetails structure.
 */
static void
calculateBoxDimensions( screen, noticeBox, boxDetails )
int			screen;
NoticeBox		*noticeBox;
noticeBoxDetails	*boxDetails;
{
	unsigned int	displayWidth, displayHeight;
	int		longestStringLength = 0;
	int		totalButtonWidth = 0;
	int		ii;
#ifdef OW_I18N
	register wchar_t	*p1, *p2, **pp;
#endif

#ifdef OW_I18N
	boxDetails->fontHeight = GRV.TextFontSetInfo.fs_extents->max_logical_extent.height;
#else
	boxDetails->fontHeight = GRV.TextFontInfo->ascent + 
					GRV.TextFontInfo->descent;
#endif

	/* calculate the width of the text which appears inside the button */
	for ( ii = 0 ; ii < noticeBox->numButtons ; ii++ )
	{
		boxDetails->buttonInfo[ii].width = 
#ifdef OW_I18N
			XwcTextEscapement(GRV.ButtonFontSetInfo.fs, 
				    noticeBox->buttonText[ii], 
				    wslen(noticeBox->buttonText[ii]) );
#else
			XTextWidth( GRV.ButtonFontInfo, 
				    noticeBox->buttonText[ii], 
				    strlen(noticeBox->buttonText[ii]) );
#endif
		boxDetails->buttonInfo[ii].fullWidth = 
			boxDetails->buttonInfo[ii].width 
				+ 2 * ButtonEndcap_Width(olgx_gisbutton);
	}

	/* add together widths of all the buttons */
	for ( ii = 0 ; ii < noticeBox->numButtons ; ii++ )
		totalButtonWidth += boxDetails->buttonInfo[ii].fullWidth;
	boxDetails->totalButtonWidth = totalButtonWidth;

#ifdef OW_I18N
	/*
	 * Split out the '\n' contains string to separate strings
	 */
	for ( ii = 0, p1 = noticeBox->stringText; *p1 != '\0'; p1++ )
	{
		if (*p1 == '\n')
			ii++;
	}
	boxDetails->numString = ii + 1;
	boxDetails->stringTexts = (wchar_t **)malloc(sizeof(wchar_t *)
						* boxDetails->numString);
	for ( pp = boxDetails->stringTexts, p1 = noticeBox->stringText;
	      *p1 != '\0';
	      pp++)
	{
		if ((p2 = wschr(p1, '\n')) != NULL)
		{
			ii = p2 - p1;
			*pp = (wchar_t *)malloc(sizeof(wchar_t) * (ii + 1));
			wsncpy(*pp, p1, ii);
			p1 = p2 + 1;
		}
		else
		{
			*pp = wsdup(p1);
			p1 += wslen(p1);
		}
	}
#endif OW_I18N
	/* figure out which descriptive string is longest */
#ifdef OW_I18N
	for ( ii = 0 ; ii < boxDetails->numString ; ii++ )
		longestStringLength = 
			MAX( longestStringLength,
			     XwcTextEscapement(GRV.TextFontSetInfo.fs, 
				         boxDetails->stringTexts[ii], 
				         wslen(boxDetails->stringTexts[ii]) ) );
#else
	for ( ii = 0 ; ii < noticeBox->numStrings ; ii++ )
		longestStringLength = 
			MAX( longestStringLength,
			     XTextWidth( GRV.TextFontInfo, 
				         noticeBox->stringText[ii], 
				         strlen(noticeBox->stringText[ii]) ) );
#endif

	/* 
	 * REMIND: this calculation assumes all the buttons are
	 * on the same line
	 */
	boxDetails->boxWidth = 2 * MIN_BOX_SIDE 
		   + MAX( longestStringLength,
			  totalButtonWidth + 
			      /* amount of space between all the buttons */
			      MIN_BUTTON_SPACE * (noticeBox->numButtons - 1) );

	boxDetails->buttonHeight = Button_Height(olgx_gisbutton);

	boxDetails->boxHeight = MIN_BOX_TOP + MIN_BOX_BOTTOM
#ifdef OW_I18N
		    + ( boxDetails->numString * boxDetails->fontHeight )
		    /* amount of space vertically between all the strings */
		    + ( (boxDetails->numString - 1) * MIN_STRING_VSPACE )
		    + MIN_BUTTON_VSPACE + boxDetails->buttonHeight;
#else
		    + ( noticeBox->numStrings * boxDetails->fontHeight )
		    /* amount of space vertically between all the strings */
		    + ( (noticeBox->numStrings - 1) * MIN_STRING_VSPACE )
		    + MIN_BUTTON_VSPACE + boxDetails->buttonHeight;
#endif

	displayWidth = DisplayWidth( boxDetails->dpy, screen );
	displayHeight = DisplayHeight( boxDetails->dpy, screen );

	/* if not set, create default "origin" for box: centered */
	boxDetails->x = ( noticeBox->boxX == -1 ) 
			    ? (int)( displayWidth - boxDetails->boxWidth )/2 
			    : noticeBox->boxX ;
	boxDetails->y = ( noticeBox->boxY == -1 ) 
			    ? (int)( displayHeight - boxDetails->boxHeight )/2 
			    : noticeBox->boxY ;

}

/*
 * drawNoticeBox -- draw box outline, strings, and buttons using information
 *		in noticeBox and boxDetails structures which are passed in.
 *		Location of buttons (x, y) are set in boxDetails structure
 *		for use later (mouse warping, determining pointer position).
 */
static void
drawNoticeBox( noticeBox, boxDetails )
NoticeBox		*noticeBox;
noticeBoxDetails	*boxDetails;
{
	int	ii;
	int	buttonX, buttonY;
	int	buttonState;		/* OLGX_NORMAL or OLGX_DEFAULT */

	/* draw beveled box */
	olgx_draw_box( olgx_gistext, boxDetails->window, 0, 0, 
		       boxDetails->boxWidth, boxDetails->boxHeight, 
		       OLGX_NORMAL, True );

	olgx_draw_box( olgx_gistext, boxDetails->window, 
		       BORDER_WIDTH, BORDER_WIDTH, 
		       boxDetails->boxWidth - 2 * BORDER_WIDTH, 
		       boxDetails->boxHeight - 2 * BORDER_WIDTH, 
		       OLGX_NORMAL, True );

	/* draw descriptive text 
	 * REMIND: all strings are along the left edge (MIN_BOX_SIDE) 
	 */
#ifdef OW_I18N_DEBUG
fprintf(stderr, "NoticeDraw: String [%ws]\n", noticeBox->stringText[ii]);
#endif
#ifdef OW_I18N
	for ( ii = 0 ; ii < boxDetails->numString ; ii++ )
		olgx_draw_text( olgx_gistext, boxDetails->window, 
			boxDetails->stringTexts[ii], MIN_BOX_SIDE,
			/* need to move each line further down the screen */
			MIN_BOX_TOP - GRV.TextFontSetInfo.fs_extents->max_logical_extent.y
								* ( ii + 1 )
#else
	for ( ii = 0 ; ii < noticeBox->numStrings ; ii++ )
		olgx_draw_text( olgx_gistext, boxDetails->window, 
			noticeBox->stringText[ii], MIN_BOX_SIDE,
			/* need to move each line further down the screen */
			MIN_BOX_TOP + GRV.TextFontInfo->ascent * ( ii + 1 )
#endif
				+ ( MIN_STRING_VSPACE * ii ),
			0, False, OLGX_NORMAL );

	/* put buttons in - notice that it's a single row */
	/* row of buttons should be centered within available space,
	 * assuming MIN_BUTTON_SPACE between each of the them
	 */
	buttonX = ( boxDetails->boxWidth - (boxDetails->totalButtonWidth 
		        + MIN_BUTTON_SPACE * (noticeBox->numButtons - 1)) )/2;
	/* this calculates from the bottom of the box */
	buttonY = ( boxDetails->boxHeight - 
			( MIN_BOX_BOTTOM + boxDetails->buttonHeight ) );
	for ( ii = 0 ; ii < noticeBox->numButtons ; ii++ )
	{
		if ( noticeBox->defaultButton == ii )
			buttonState = OLGX_DEFAULT;
		else
			buttonState = OLGX_NORMAL;

		/* save button's x, y values for use later */
		boxDetails->buttonInfo[ii].x = buttonX;
		boxDetails->buttonInfo[ii].y = buttonY;

		olgx_draw_button( olgx_gisbutton, boxDetails->window, 
				  buttonX, buttonY,
				  boxDetails->buttonInfo[ii].fullWidth, 0,
				  noticeBox->buttonText[ii], 
				  buttonState );

		/* set up buttonX for next button */ 
		buttonX = buttonX + boxDetails->buttonInfo[ii].fullWidth
				+ MIN_BUTTON_SPACE;
	}
}
