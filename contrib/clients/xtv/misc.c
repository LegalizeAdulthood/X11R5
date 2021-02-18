/*
 * Copyright (c) 1991 Old Dominion University.
 * Copyright (c) 1991 University of North Carolina at Chapel Hill.
 * All rights reserved.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  Old Dominion University and the University of North
 * Carolina at Chapel Hill make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

/*
	misc.c
	Miscellaneous Routines

*/

#include <stdio.h>
#include <varargs.h>
#include <X11/Xmd.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Cardinals.h>

/*
 * Begin Murray Addition
 */
#include <X11/Xaw/Scrollbar.h>
#include "ptrlists.h"
/*
 * End Murray Addition
 */

#include "misc.h"

/* #define DEBUGON FALSE */

#define DEBUGUpdateMessage	01&DEBUGON
#define DEBUGSwapInt	0&DEBUGON
#define DEBUGSwapLong	0&DEBUGON
#define DEBUGpad	0&DEBUGON

extern char	INITIAL_MSG1[];
/*
 *	ErrorMessage()
 *
 *	Print the printf-style string to stderr
 *
 *	RETURN VALUE:	void
 */
void ErrorMessage( va_alist )
va_dcl
{
	va_list	ArgPointer;
	char	*Format;

	va_start( ArgPointer );
	Format = va_arg( ArgPointer, char * );
	vfprintf( stderr, Format, ArgPointer )
	    va_end( ArgPointer );
}

/*
 *	FatalError()
 *
 *	Print the printf-style string to stderr and bomb
 *
 *	RETURN VALUE:	void...  VERY VOID.
 */
void FatalError( va_alist )
va_dcl
{
	va_list	ArgPointer;
	char	*Format;

	va_start( ArgPointer );
	Format = va_arg( ArgPointer, char * );
	vfprintf( stderr, Format, ArgPointer )
	    va_end( ArgPointer );
	exit(1);
}



/* ------------------------------------------------------------------------------------------- */

Widget		WMessageLabel;
Widget		WMessageTextBox;
Widget          WMessageScrollbar;
static char	*MessageBuffer = INITIAL_MSG1;
Arg		args[10];
int		n;

static char OldMessageBuffer[1024] = " ";
static char DisplayMessageBuffer[2048] = " ";
static char *MessageSeparator = "\n__________________________________________\n";


/*
 * Begin Murray Addition
 */

/*
 * For the definition of PTRLIST, see lists.h and ptrlists.h
 * with the corresponding .c files. This is a very flexible
 * form of linked list.
 *
 * Note that the message update function has been split into
 * two pieces.
 *    displayMessage() - which displays the current message in the
 *                       message box.
 *    UpdateMessage()  - this adds a new message to the end of the
 *                       message buffer. If necessary the second
 *                       message is thrown away.
 *
 * There are two scroll messages to go with the WMessageBar widget.
 *    msgScrolled()    - this scrolls the message buffer either one
 *                       message up or down.
 *    msgJumped()      - this tracks the scroll bar updating when
 *                       necessary the message in the message box.
 *
 * There are two utility functions for the message buffer.
 *    dumpMessageBuffer()
 *                     - this dumps the contents of the message
 *                       buffer to stderr.
 *    freeMessageBuffer()
 *                     - this frees the memory used by the message
 *                       buffer.
 *
 *
 *
 * msgBuffer is a PTRLIST * of char *
 * oldPos is used as a cache of the old position of the scrollbar
 *   in terms of list item number.
 * MAXNUMMSG is a macro defining the size of the message buffer
 *   in terms of number of messages.
 * msgLock is a flag which is available, but unused. If msgLock
 *   is ever non zero, then the message box will lock on a message
 *   and always return to it if it can. When it no longer can return,
 *   the message is locked to the most recently displayed message.
 *   The scroll actions reset the locked message to whatever message
 *   they end up on.
 */
static PTRLIST *msgBuffer = 0L;
static int      oldPos = -1;
#define MAXNUMMSG 20
static int msgLock = 0;

/*
 * dumpMessageBuffer()
 *
 * Dump the message buffer to stderr.
 */

void dumpMessageBuffer()
{
  fprintf(stderr, "=====          Message Buffer Start        =\n");
  fprintf(stderr, "===== Message Buffer contains %d items.\n", items(msgBuffer));
  for (start(msgBuffer); !offright(msgBuffer); forth(msgBuffer))
    {
      fprintf(stderr, "=====            Message Divider            =\n");
      fprintf(stderr, "%s\n", (char *) getItem(msgBuffer));
    }      
  fprintf(stderr, "=====           Message Buffer End          =\n");
}

/*
 * freeMessageBuffer()
 *
 * This can only be called at termination.
 * It frees the entire message buffer and renders
 * the list unusable.
 */

void freeMessageBuffer()
{
  for (start(msgBuffer); !offright(msgBuffer); forth(msgBuffer))
    deleteItem(msgBuffer);
  free(msgBuffer);
}

/*
 * displayMessage()
 *
 * Display the current element in the msgBuffer.
 * If there are no elements, then ignore the display.
 *
 * NOTE: There should NEVER be a situation where the
 * message buffer is empty or not pointing at a valid
 * message. This is due to the initial message posting
 * upon startup.
 */
static void displayMessage()
{
  n=0;
  /*
   * Check to see if we don't have a current message.
   */
  if (!items(msgBuffer) || offright(msgBuffer) || offleft(msgBuffer))
    XtSetArg(args[n], XtNlabel, "-- No Messages --");
  else
    XtSetArg(args[n], XtNlabel, (char *) getItem(msgBuffer));
  n++;
  XtSetValues( WMessageTextBox, args, n );
  XFlush(XtDisplay(WMessageTextBox));
}

/*
 * msgScrolled
 *
 * This is an XtCallbackProc for use with the message box.
 *
 * NOTE: If the person happens to push the mouse button at
 * the very top of the scroll bar, then it comes back with no
 * sign. Therefore, in this case only do we ignore it.
 */
static XtCallbackProc
msgScrolled(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  long item_pos;

  if (!((int) call_data))
    /*
     * I can't scroll if I don't have a sign on the
     * button.
     */
    return;
  if ((int) call_data < 0)
    {
      /*
       * Scroll the message buffer back one message.
       */
      back(msgBuffer);
      /*
       * oldPos is used in the thumb scrolling to cut
       * down on the flickering.
       */
      oldPos--;
      if (offleft(msgBuffer))
	/*
	 * Make sure that we are always on a valid message.
	 */
	{
	  forth(msgBuffer);
	  oldPos++;
	}
    }
  else
    {
      /*
       * Scroll the message buffer forth one message.
       */
      forth(msgBuffer);
      /*
       * oldPos is used in the thumb scrolling to cut
       * down on the flickering.
       */
      oldPos++;
      if (offright(msgBuffer))
	/*
	 * Make sure that we are always on a valid message.
	 */
	{
	  back(msgBuffer);
	  oldPos--;
	}
    }
  /*
   * Find our current position in the list. To do this, save where we
   * are, then count back until we hit the left edge. This results in
   * a value between one and items-1. Then set the new thumb position.
   * The thumb length doesn't change.
   *
   * Then restore the message buffer to the correct message, and display
   * it.
   */
  saveCursor(msgBuffer);
  for (item_pos= -1; !offleft(msgBuffer); back(msgBuffer), item_pos++)
    ;
  XawScrollbarSetThumb(w, ((float) item_pos)/((float) items(msgBuffer)), -1.0);
  restoreCursor(msgBuffer);
  displayMessage();
}

/*
 * msgJumped
 *
 * This is an XtCallbackProc for use with the message box.
 */
static XtCallbackProc
msgJumped(w, closure, call_data)
Widget w;
XtPointer closure, call_data;
{
  float top = *((float *) call_data);
  int new_pos;
  int itemNum;

  /*
   * Make an adjustment in case the user goes past the bottom of the scroll bar.
   * This is due to the way the new scroll bar position is determined.
   *
   * Then check to see if we are on the same message as before. If we are, then
   * just reset the thumb and leave. This cuts out the unnecessary chatter of the
   * image in the message box.
   */
  if (top >= 1.0)
    top = 0.99;
  new_pos = (int) (top*((float) items(msgBuffer)));
  if (new_pos == oldPos)
    {
      XawScrollbarSetThumb(w, ((float) new_pos)/((float) items(msgBuffer)), -1.0);
      return;
    }
  /*
   * Find our current position in the list. Then set the new thumb position.
   * Our new position is passed into us. So just set the thumb.
   * The thumb length doesn't change.
   *
   * Set the message buffer to the correct message, and display
   * it.
   */
  for (itemNum = 0, start(msgBuffer); itemNum < new_pos; itemNum++, forth(msgBuffer))
    ;

  XawScrollbarSetThumb(w, ((float) new_pos)/((float) items(msgBuffer)), -1.0);
  displayMessage();
  oldPos = new_pos;
}

/*
 *	UpdateMessage()
 *
 *	Take a printf-style argument list and bring the message
 *	box up-to-date.
 *
 *	RETURN VALUE:	void
 */
void UpdateMessage( va_alist )
va_dcl
{
  va_list	ArgPointer;
  char	*Format;
  char *newMsg;

  va_start( ArgPointer );
  Format = va_arg( ArgPointer, char * );
  vsprintf( MessageBuffer, Format, ArgPointer );
  va_end( ArgPointer );

/*
 * Instead of displaying the new message directly
 * the display has been moved to DisplayMessage(),
 * which displays the current message in the message
 * buffer.
 *
 * What we need to do is add the new message onto
 * the end of the message buffer and, if necessary,
 * save the current element in the message buffer,
 * so that we can redisplay the message after
 * showing the new one.
 */

/*
 * First create a new string for the current message
 * in the list. Then copy the new message into this
 * space and set the last element to a null. If we
 * need to save the current position in the message
 * buffer, then do so. Then move to the end of the
 * buffer, add the new message and display it.
 * Then restore the cursor and old message.
 */
  newMsg = (char *) malloc(strlen(MessageBuffer)+1);
  (void) strcpy(newMsg, MessageBuffer);

  if (msgLock && items(msgBuffer))
    saveCursor(msgBuffer);

  /*
   * If we are at the message buffer limit, then throw away
   * the second message. The second is chosen, since the
   * first message to the chair contains joining information.
   */
  if (items(msgBuffer) == MAXNUMMSG)
    {
      start(msgBuffer);
      forth(msgBuffer);
      deleteItem(msgBuffer);
    }

  /*
   * Add the new message to the end of the buffer.
   */
  finish(msgBuffer);
  addItem(msgBuffer, (void *) newMsg);

  /*
   * Only ring the bell, if we are adding
   * a new message. This cuts out ALOT of
   * bells on scrolling.
   */
  XBell(XtDisplay(WMessageTextBox), 0);

  displayMessage();

  /*
   * Message locking is currently disabled.
   *
   * If it is ever enabled, then this keeps the
   * same message in the message window. Note
   * that new messages are displayed when they
   * come in, but are immediately replaced by
   * the old message. Should a timer go here?
   */

  if (msgLock && haveCursor(msgBuffer))
    {
      restoreCursor(msgBuffer);
      displayMessage();
    }
  /*
   * Find our current position in the list. To do this, save where we
   * are, then count back until we hit the left edge. This results in
   * a value between one and items-1. Then set the new thumb position.
   * The thumb length is adjusted to reflect the presence of the new
   * message.
   *
   * Then restore the message buffer to the correct message, and display
   * it.
   */
  saveCursor(msgBuffer);
  for (oldPos= -1; !offleft(msgBuffer); back(msgBuffer), oldPos++)
    ;
  XawScrollbarSetThumb(WMessageScrollbar, ((float) oldPos)/((float) items(msgBuffer)), 1.0/((float) items(msgBuffer)));
  restoreCursor(msgBuffer);
}

void
lockMessages()
{
  msgLock = 1;
}

void
unlockMessages()
{
  msgLock = 0;
}

/*
 * End Murray Addition
 */

static BYTE	LocByteOrder;


/*
 *	InitializeMisc()
 *
 *	Initialize the message box and anything else handled by this module
 *
 *	RETURN VALUE:	void
 */
void InitializeMisc( AppCon, Parent )
XtAppContext AppCon;
Widget Parent;
{
  char * newMsg;

  float top = 0.5;
  XtArgVal *l_top;

	n=0;
	XtSetArg( args[n],	XtNshowGrip,	FALSE	);
	n++;
	XtSetArg( args[n], 	XtNlabel, 	"Message from System" );
	n++;
	XtSetArg( args[n],	XtNborderWidth, 0 );
	n++;
	WMessageLabel = XtCreateManagedWidget( "MessageLabel", labelWidgetClass, Parent, args, n);

  /*
   * Begin Murray Addition
   */
  n=0;
  XtSetArg( args[n],      XtNshowGrip, FALSE );

  n++;
  if (sizeof(float) > sizeof(XtArgVal))
    {
      XtSetArg(args[n], XtNtopOfThumb, &top);
    }
  else
    {
      l_top = (XtArgVal *) &top;
      XtSetArg(args[n], XtNtopOfThumb, *l_top);
    }

  n++;
  
  WMessageScrollbar = XtCreateManagedWidget( "MessageBar", scrollbarWidgetClass, Parent, args, n);

  XtAddCallback(WMessageScrollbar, XtNjumpProc, msgJumped, NULL);
  XtAddCallback(WMessageScrollbar, XtNscrollProc, msgScrolled, NULL);

  XawScrollbarSetThumb(WMessageScrollbar, 0.0, 1.0);

  n=0;
  XtSetArg( args[n],	XtNshowGrip,	FALSE	);
  n++;
  XtSetArg( args[n], 	XtNlabel, 	MessageBuffer );
  n++;
  
  WMessageTextBox = XtCreateManagedWidget( "MessageBox", labelWidgetClass, Parent, args, n);
  
  /*
   * Initialize the msgBuffer.
   * Due to the way that the initial message comes up, we need to capture
   * it here.
   */
  
  if (!msgBuffer)
    {
      msgBuffer = ptrNew();
      newMsg = (char *) malloc(strlen(MessageBuffer)+1);
      (void) strcpy(newMsg, MessageBuffer);
      addItem(msgBuffer, (void *) newMsg);
    }
  
/*
 * End Murray Addition
 */


	/* Initialize SwapInt */
	{
		static unsigned short	TestWord = 0x1234;
		BYTE			*bp;

		bp = (BYTE *)(&TestWord);
		LocByteOrder = ( bp[0] == 0x12 ? MSB_First : LSB_First );
#		if DEBUGSwapInt || DEBUGSwapLong
		fprintf( stderr, "InitializeMisc(): Local system has %cSB [%02x-%02x] first\n",
		    ( LocByteOrder == MSB_First ? 'M' : 'L' ), bp[0], bp[1] );
#		endif
	}

}


#if NEEDSWAP	
/*
 *	SwapInt()
 *
 *	Swap the bytes in a 16-bit integer if needed
 *
 *	RETURN VALUE:	swapped integer
 */
CARD16 SwapInt( Value, SwapMode )
CARD16 Value;
BYTE SwapMode;	/* 0x42=MSB first, 0x6c=LSB first */
{
	CARD16			Result;

	if ( SwapMode == LocByteOrder ) {
#	if DEBUGSwapInt
		fprintf( stderr, "SwapInt( %04x, %cSB ) = %04x\n", Value,
		    ( SwapMode == MSB_First ? 'M' : 'L' ), Value );
#	endif

		return Value;
	}

	memcpy(((char *)&Result),((char *)&Value)+1,1);
	memcpy(((char *)&Result)+1,((char *)&Value),1);

#	if DEBUGSwapInt
	fprintf( stderr, "SwapInt( %04x, %cSB ) = %04x\n", Value,
	    ( SwapMode == MSB_First ? 'M' : 'L' ), *((CARD16 *)(Result)) );
#	endif
	return(Result);
}

/*
 *	SwapLong()
 *
 *	Swap the bytes in a 32-bit unsigned integer if needed
 *
 *	RETURN VALUE:	swapped Long
 */
CARD32 SwapLong( Value, SwapMode )
CARD32 Value;
BYTE SwapMode;	/* 0x42=MSB first, 0x6c=LSB first */
{
	CARD32			Result;

	if ( SwapMode == LocByteOrder ) {
		return Value;
	}

	memcpy(((char *)&Result),((char *)&Value)+3,1);
	memcpy(((char *)&Result)+1,((char *)&Value)+2,1);
	memcpy(((char *)&Result)+2,((char *)&Value)+1,1);
	memcpy(((char *)&Result)+3,((char *)&Value),1);

#	if DEBUGSwapLong
	fprintf( stderr, "SwapLong( %08x, %cSB ) = %08x\n", Value,
	    ( SwapMode == MSB_First ? 'M' : 'L' ), *((CARD32 *)(Result)) );
#	endif
	return(Result);
}
#endif /* swap functions and macros */

/*
 *	LocalByteOrder()
 *
 *	Return a byte containing the local byte order
 *
 *	RETURN VALUE:	MSB_First or LSB_First
 */
BYTE LocalByteOrder()
{
	return LocByteOrder;
}
pad(n)
int n;
{
   int i;
#if DEBUGpad
fprintf( stderr, "pad(), n %d\n", n);
#endif
   i = (n % 4);
   return(n+ ((i)? 4-i:0));
}

void LGetArgs(CommandArgs,s)
char *CommandArgs[];
char *s;
{
	int i;
	i=0;
	while (*s) {
		/* skip white spaces */
		while ((*s == ' ') || (*s == '\t')) *s++ = NULL;
		CommandArgs[i++] = s;
		/* skip over this word to next white space */
		while((*s != NULL) && (*s != ' ') && (*s != '\t')) s++;
	}
	CommandArgs[i]=NULL;
}
