/* ************************************************** *
 *						      *
 *  Request, Reply, Event, Error Printing	      *
 *						      *
 *	James Peterson, 1988			      *
 *	(c) Copyright MCC, 1988 		      *
 *						      *
 * ************************************************** */

#include "scope.h"
#include "x11.h"


/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

/*
  In printing the contents of the fields of the X11 packets, some
  fields are of obvious value, and others are of lesser value.  To
  control the amount of output, we generate our output according
  to the level of Verbose-ness that was selected by the user.

  Verbose = 0 ==  Headers only, time and request/reply/... names.

  Verbose = 1 ==  Very useful content fields.

  Verbose = 2 ==  Almost everything.

  Verbose = 3 ==  Every single bit and byte.

*/

/*
  To aid in making the choice between level 1 and level 2, we
  define the following define, which does not print relatively
  unimportant fields.
*/

#define printfield(a,b,c,d,e) if (Verbose > 1) PrintField(a,b,c,d,e)


/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

PrintSetUpMessage(buf)
     unsigned char *buf;
{
  short   n;
  short   d;

  enterprocedure("PrintSetUpMessage");
  if (Verbose < 1)
    return;
  SetIndentLevel(PRINTCLIENT);
  PrintField(buf, 0, 1, BYTEMODE, "byte-order");
  PrintField(buf, 2, 2, CARD16, "major-version");
  PrintField(buf, 4, 2, CARD16, "minor-version");
  printfield(buf, 6, 2, DVALUE2(n), "length of name");
  n = IShort(&buf[6]);
  printfield(buf, 8, 2, DVALUE2(d), "length of data");
  d = IShort(&buf[8]);
  PrintString8(&buf[12], (long)n, "authorization-protocol-name");
  PrintString8(&buf[pad((long)(12+n))], (long)d, "authorization-protocol-data");
}

PrintSetUpReply(buf)
     unsigned char *buf;
{
  enterprocedure("PrintSetUpReply");
  SetIndentLevel(PRINTSERVER);
  if (IByte(&buf[0]))
    PrintSuccessfulSetUpReply(buf);
  else
    PrintFailedSetUpReply(buf);
}

PrintFailedSetUpReply(buf)
     unsigned char *buf;
{
  short   n;

  PrintField(buf, 0, 1, 0, "SetUp Failed");
  if (Verbose < 1)
    return;
  printfield(buf, 1, 1, DVALUE1(n), "length of reason in bytes");
  n = IByte(&buf[1]);
  PrintField(buf, 2, 2, CARD16, "major-version");
  PrintField(buf, 4, 2, CARD16, "minor-version");
  printfield(buf, 6, 2, DVALUE2((n + p) / 4), "length of data");
  PrintString8(&buf[8], (long)n, "reason");
}

PrintSuccessfulSetUpReply(buf)
     unsigned char *buf;
{
  short   v;
  short   n;
  short   m;

  if (Verbose < 1)
    return;
  PrintField(buf, 2, 2, CARD16, "protocol-major-version");
  PrintField(buf, 4, 2, CARD16, "protocol-minor-version");
  printfield(buf, 6, 2, DVALUE2(8 + 2*n + (v + p + m) / 4), "length of data");
  PrintField(buf, 8, 4, CARD32, "release-number");
  PrintField(buf, 12, 4, CARD32, "resource-id-base");
  PrintField(buf, 16, 4, CARD32, "resource-id-mask");
  PrintField(buf, 20, 4, CARD32, "motion-buffer-size");
  printfield(buf, 24, 2, DVALUE2(v), "length of vendor");
  v = IShort(&buf[24]);
  printfield(buf, 26, 2, CARD16, "maximum-request-length");
  printfield(buf, 28, 1, CARD8, "number of roots");
  m = IByte(&buf[28]);
  printfield(buf, 29, 1, DVALUE1(n), "number of pixmap-formats");
  n = IByte(&buf[29]);
  PrintField(buf, 30, 1, BYTEORDER, "image-byte-order");
  PrintField(buf, 31, 1, BYTEORDER, "bitmap-format-bit-order");
  PrintField(buf, 32, 1, CARD8, "bitmap-format-scanline-unit");
  PrintField(buf, 33, 1, CARD8, "bitmap-format-scanline-pad");
  PrintField(buf, 34, 1, KEYCODE, "min-keycode");
  PrintField(buf, 35, 1, KEYCODE, "max-keycode");
  PrintString8(&buf[40], (long)v, "vendor");
  (void)PrintList(&buf[pad((long)(40+v))], (long)n, FORMAT, "pixmap-formats");
  (void)PrintList(&buf[pad((long)(40+v) + 8 * n)], (long)m, SCREEN, "roots");
}


/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

static char *REQUESTHEADER = "............REQUEST";
static char *EVENTHEADER = "..............EVENT";
static char *ERRORHEADER = "..............ERROR";
static char *REPLYHEADER = "..............REPLY";


/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

/* Error Printing procedures */

RequestError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* Request */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

ValueError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* Value */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, INT32, "bad value");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

WindowError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* Window */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, CARD32, "bad resource id");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

PixmapError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* Pixmap */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, CARD32, "bad resource id");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

AtomError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* Atom */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, CARD32, "bad atom id");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

CursorError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* Cursor */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, CARD32, "bad resource id");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

FontError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* Font */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, CARD32, "bad resource id");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

MatchError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* Match */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

DrawableError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* Drawable */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, CARD32, "bad resource id");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

AccessError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* Access */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

AllocError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* Alloc */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

ColormapError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* Colormap */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, CARD32, "bad resource id");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

GContextError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* GContext */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, CARD32, "bad resource id");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

IDChoiceError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* IDChoice */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, CARD32, "bad resource id");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

NameError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* Name */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

LengthError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* Length */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

ImplementationError(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, ERROR, ERRORHEADER) /* Implementation */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 8, 2, CARD16, "minor opcode");
  PrintField(buf, 10, 1, CARD8, "major opcode");
}

/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

/* Event Printing procedures */

KeyPressEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* KeyPress */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, KEYCODE, "detail");
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, TIMESTAMP, "time");
  PrintField(buf, 8, 4, WINDOW, "root");
  PrintField(buf, 12, 4, WINDOW, "event");
  PrintField(buf, 16, 4, WINDOW, "child");
  PrintField(buf, 20, 2, INT16, "root-x");
  PrintField(buf, 22, 2, INT16, "root-y");
  PrintField(buf, 24, 2, INT16, "event-x");
  PrintField(buf, 26, 2, INT16, "event-y");
  PrintField(buf, 28, 2, SETofKEYBUTMASK, "state");
  PrintField(buf, 30, 1, BOOL, "same-screen");
}

KeyReleaseEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* KeyRelease */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, KEYCODE, "detail");
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, TIMESTAMP, "time");
  PrintField(buf, 8, 4, WINDOW, "root");
  PrintField(buf, 12, 4, WINDOW, "event");
  PrintField(buf, 16, 4, WINDOW, "child");
  PrintField(buf, 20, 2, INT16, "root-x");
  PrintField(buf, 22, 2, INT16, "root-y");
  PrintField(buf, 24, 2, INT16, "event-x");
  PrintField(buf, 26, 2, INT16, "event-y");
  PrintField(buf, 28, 2, SETofKEYBUTMASK, "state");
  PrintField(buf, 30, 1, BOOL, "same-screen");
}

ButtonPressEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* ButtonPress */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, BUTTON, "detail");
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, TIMESTAMP, "time");
  PrintField(buf, 8, 4, WINDOW, "root");
  PrintField(buf, 12, 4, WINDOW, "event");
  PrintField(buf, 16, 4, WINDOW, "child");
  PrintField(buf, 20, 2, INT16, "root-x");
  PrintField(buf, 22, 2, INT16, "root-y");
  PrintField(buf, 24, 2, INT16, "event-x");
  PrintField(buf, 26, 2, INT16, "event-y");
  PrintField(buf, 28, 2, SETofKEYBUTMASK, "state");
  PrintField(buf, 30, 1, BOOL, "same-screen");
}

ButtonReleaseEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* ButtonRelease */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, BUTTON, "detail");
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, TIMESTAMP, "time");
  PrintField(buf, 8, 4, WINDOW, "root");
  PrintField(buf, 12, 4, WINDOW, "event");
  PrintField(buf, 16, 4, WINDOW, "child");
  PrintField(buf, 20, 2, INT16, "root-x");
  PrintField(buf, 22, 2, INT16, "root-y");
  PrintField(buf, 24, 2, INT16, "event-x");
  PrintField(buf, 26, 2, INT16, "event-y");
  PrintField(buf, 28, 2, SETofKEYBUTMASK, "state");
  PrintField(buf, 30, 1, BOOL, "same-screen");
}

MotionNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* MotionNotify */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, MOTIONDETAIL, "detail");
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, TIMESTAMP, "time");
  PrintField(buf, 8, 4, WINDOW, "root");
  PrintField(buf, 12, 4, WINDOW, "event");
  PrintField(buf, 16, 4, WINDOW, "child");
  PrintField(buf, 20, 2, INT16, "root-x");
  PrintField(buf, 22, 2, INT16, "root-y");
  PrintField(buf, 24, 2, INT16, "event-x");
  PrintField(buf, 26, 2, INT16, "event-y");
  PrintField(buf, 28, 2, SETofKEYBUTMASK, "state");
  PrintField(buf, 30, 1, BOOL, "same-screen");
}

EnterNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* EnterNotify */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, ENTERDETAIL, "detail");
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, TIMESTAMP, "time");
  PrintField(buf, 8, 4, WINDOW, "root");
  PrintField(buf, 12, 4, WINDOW, "event");
  PrintField(buf, 16, 4, WINDOW, "child");
  PrintField(buf, 20, 2, INT16, "root-x");
  PrintField(buf, 22, 2, INT16, "root-y");
  PrintField(buf, 24, 2, INT16, "event-x");
  PrintField(buf, 26, 2, INT16, "event-y");
  PrintField(buf, 28, 2, SETofKEYBUTMASK, "state");
  PrintField(buf, 30, 1, BUTTONMODE, "mode");
  PrintField(buf, 31, 1, SCREENFOCUS, "same-screen, focus");
}

LeaveNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* LeaveNotify */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, ENTERDETAIL, "detail");
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, TIMESTAMP, "time");
  PrintField(buf, 8, 4, WINDOW, "root");
  PrintField(buf, 12, 4, WINDOW, "event");
  PrintField(buf, 16, 4, WINDOW, "child");
  PrintField(buf, 20, 2, INT16, "root-x");
  PrintField(buf, 22, 2, INT16, "root-y");
  PrintField(buf, 24, 2, INT16, "event-x");
  PrintField(buf, 26, 2, INT16, "event-y");
  PrintField(buf, 28, 2, SETofKEYBUTMASK, "state");
  PrintField(buf, 30, 1, BUTTONMODE, "mode");
  PrintField(buf, 31, 1, SCREENFOCUS, "same-screen, focus");
}

FocusInEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* FocusIn */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, ENTERDETAIL, "detail");
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "event");
  PrintField(buf, 8, 1, BUTTONMODE, "mode");
}

FocusOutEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* FocusOut */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, ENTERDETAIL, "detail");
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "event");
  PrintField(buf, 8, 1, BUTTONMODE, "mode");
}

KeymapNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* KeymapNotify */ ;
  if (Verbose < 1)
    return;
  PrintBytes(&buf[1], (long)31,"keys");
}

ExposeEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* Expose */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "window");
  PrintField(buf, 8, 2, CARD16, "x");
  PrintField(buf, 10, 2, CARD16, "y");
  PrintField(buf, 12, 2, CARD16, "width");
  PrintField(buf, 14, 2, CARD16, "height");
  PrintField(buf, 16, 2, CARD16, "count");
}

GraphicsExposureEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* GraphicsExposure */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 2, CARD16, "x");
  PrintField(buf, 10, 2, CARD16, "y");
  PrintField(buf, 12, 2, CARD16, "width");
  PrintField(buf, 14, 2, CARD16, "height");
  PrintField(buf, 16, 2, CARD16, "minor-opcode");
  PrintField(buf, 18, 2, CARD16, "count");
  PrintField(buf, 20, 1, CARD8, "major-opcode");
}

NoExposureEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* NoExposure */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 2, CARD16, "minor-opcode");
  PrintField(buf, 10, 1, CARD8, "major-opcode");
}

VisibilityNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* VisibilityNotify */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "window");
  PrintField(buf, 8, 1, VISIBLE, "state");
}

CreateNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* CreateNotify */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "parent");
  PrintField(buf, 8, 4, WINDOW, "window");
  PrintField(buf, 12, 2, INT16, "x");
  PrintField(buf, 14, 2, INT16, "y");
  PrintField(buf, 16, 2, CARD16, "width");
  PrintField(buf, 18, 2, CARD16, "height");
  PrintField(buf, 20, 2, CARD16, "border-width");
  PrintField(buf, 22, 1, BOOL, "override-redirect");
}

DestroyNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* DestroyNotify */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "event");
  PrintField(buf, 8, 4, WINDOW, "window");
}

UnmapNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* UnmapNotify */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "event");
  PrintField(buf, 8, 4, WINDOW, "window");
  PrintField(buf, 12, 1, BOOL, "from-configure");
}

MapNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* MapNotify */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "event");
  PrintField(buf, 8, 4, WINDOW, "window");
  PrintField(buf, 12, 1, BOOL, "override-redirect");
}

MapRequestEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* MapRequest */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "parent");
  PrintField(buf, 8, 4, WINDOW, "window");
}

ReparentNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* ReparentNotify */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "event");
  PrintField(buf, 8, 4, WINDOW, "window");
  PrintField(buf, 12, 4, WINDOW, "parent");
  PrintField(buf, 16, 2, INT16, "x");
  PrintField(buf, 18, 2, INT16, "y");
  PrintField(buf, 20, 1, BOOL, "override-redirect");
}

ConfigureNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* ConfigureNotify */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "event");
  PrintField(buf, 8, 4, WINDOW, "window");
  PrintField(buf, 12, 4, WINDOW, "above-sibling");
  PrintField(buf, 16, 2, INT16, "x");
  PrintField(buf, 18, 2, INT16, "y");
  PrintField(buf, 20, 2, CARD16, "width");
  PrintField(buf, 22, 2, CARD16, "height");
  PrintField(buf, 24, 2, CARD16, "border-width");
  PrintField(buf, 26, 1, BOOL, "override-redirect");
}

ConfigureRequestEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* ConfigureRequest */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, STACKMODE, "stack-mode");
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "parent");
  PrintField(buf, 8, 4, WINDOW, "window");
  PrintField(buf, 12, 4, WINDOW, "sibling");
  PrintField(buf, 16, 2, INT16, "x");
  PrintField(buf, 18, 2, INT16, "y");
  PrintField(buf, 20, 2, CARD16, "width");
  PrintField(buf, 22, 2, CARD16, "height");
  PrintField(buf, 24, 2, CARD16, "border-width");
  PrintField(buf, 26, 2, CONFIGURE_BITMASK, "value-mask");
}

GravityNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* GravityNotify */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "event");
  PrintField(buf, 8, 4, WINDOW, "window");
  PrintField(buf, 12, 2, INT16, "x");
  PrintField(buf, 14, 2, INT16, "y");
}

ResizeRequestEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* ResizeRequest */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "window");
  PrintField(buf, 8, 2, CARD16, "width");
  PrintField(buf, 10, 2, CARD16, "height");
}

CirculateNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* CirculateNotify */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "event");
  PrintField(buf, 8, 4, WINDOW, "window");
  PrintField(buf, 12, 4, WINDOW, "parent");
  PrintField(buf, 16, 1, CIRSTAT, "place");
}

CirculateRequestEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* CirculateRequest */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "parent");
  PrintField(buf, 8, 4, WINDOW, "window");
  PrintField(buf, 16, 1, CIRSTAT, "place");
}

PropertyNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* PropertyNotify */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "window");
  PrintField(buf, 8, 4, ATOM, "atom");
  PrintField(buf, 12, 4, TIMESTAMP, "time");
  PrintField(buf, 16, 1, PROPCHANGE, "state");
}

SelectionClearEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* SelectionClear */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, TIMESTAMP, "time");
  PrintField(buf, 8, 4, WINDOW, "owner");
  PrintField(buf, 12, 4, ATOM, "selection");
}

SelectionRequestEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* SelectionRequest */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, TIMESTAMP, "time");
  PrintField(buf, 8, 4, WINDOW, "owner");
  PrintField(buf, 12, 4, WINDOW, "requestor");
  PrintField(buf, 16, 4, ATOM, "selection");
  PrintField(buf, 20, 4, ATOM, "target");
  PrintField(buf, 24, 4, ATOM, "property");
}

SelectionNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* SelectionNotify */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, TIMESTAMP, "time");
  PrintField(buf, 8, 4, WINDOW, "requestor");
  PrintField(buf, 12, 4, ATOM, "selection");
  PrintField(buf, 16, 4, ATOM, "target");
  PrintField(buf, 20, 4, ATOM, "property");
}

ColormapNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* ColormapNotify */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "window");
  PrintField(buf, 8, 4, COLORMAP, "colormap");
  PrintField(buf, 12, 1, BOOL, "new");
  PrintField(buf, 13, 1, CMAPCHANGE, "state");
}

ClientMessageEvent(buf)
     unsigned char *buf;
{
  short format;
  long type;
  
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* ClientMessage */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, CARD8, "format");
  format = IByte(&buf[1]);
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 4, WINDOW, "window");
  PrintField(buf, 8, 4, ATOM, "type");
  type = ILong(&buf[8]);
  if (type == 31 /* string */)
    PrintString8(&buf[12], 20L, "data");
  else if (format == 16)
    (void)PrintList(&buf[12], 10L, INT16, "data");
  else if (format == 32)
    (void)PrintList(&buf[12], 5L, INT32, "data");
  else
    PrintBytes(&buf[12], 20L, "data");
}

MappingNotifyEvent(buf)
     unsigned char *buf;
{
  PrintField(buf, 0, 1, EVENT, EVENTHEADER) /* MappingNotify */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  PrintField(buf, 4, 1, MAPOBJECT, "request");
  PrintField(buf, 5, 1, KEYCODE, "first-keycode");
  PrintField(buf, 6, 1, CARD8, "count");
}

/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

/* Request and Reply Printing procedures */

CreateWindow(buf)
     unsigned char *buf;
{
  /* Request CreateWindow is opcode 1 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* CreateWindow */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, CARD8, "depth");
  printfield(buf, 2, 2, DVALUE2(8 + n), "request length");
  PrintField(buf, 4, 4, WINDOW, "wid");
  PrintField(buf, 8, 4, WINDOW, "parent");
  PrintField(buf, 12, 2, INT16, "x");
  PrintField(buf, 14, 2, INT16, "y");
  PrintField(buf, 16, 2, CARD16, "width");
  PrintField(buf, 18, 2, CARD16, "height");
  PrintField(buf, 20, 2, CARD16, "border-width");
  PrintField(buf, 22, 2, WINDOWCLASS, "class");
  PrintField(buf, 24, 4, VISUALIDC, "visual");
  PrintField(buf, 28, 4, WINDOW_BITMASK, "value-mask");
  PrintValues(&buf[28], 4, WINDOW_BITMASK, &buf[32], "value-list");
}

ChangeWindowAttributes(buf)
     unsigned char *buf;
{
  /* Request ChangeWindowAttributes is opcode 2 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ChangeWindowAttributes */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(3 + n), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
  PrintField(buf, 8, 4, WINDOW_BITMASK, "value-mask");
  PrintValues(&buf[8], 4, WINDOW_BITMASK, &buf[12], "value-list");
}

GetWindowAttributes(buf)
     unsigned char *buf;
{
  /* Request GetWindowAttributes is opcode 3 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GetWindowAttributes */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
}

GetWindowAttributesReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GetWindowAttributes */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, BACKSTORE, "backing-store");
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(3), "reply length");
  PrintField(buf, 8, 4, VISUALID, "visual");
  PrintField(buf, 12, 2, WINDOWCLASS, "class");
  PrintField(buf, 14, 1, BITGRAVITY, "bit-gravity");
  PrintField(buf, 15, 1, WINGRAVITY, "win-gravity");
  PrintField(buf, 16, 4, CARD32, "backing-planes");
  PrintField(buf, 20, 4, CARD32, "backing-pixel");
  PrintField(buf, 24, 1, BOOL, "save-under");
  PrintField(buf, 25, 1, BOOL, "map-is-installed");
  PrintField(buf, 26, 1, MAPSTATE, "map-state");
  PrintField(buf, 27, 1, BOOL, "override-redirect");
  PrintField(buf, 28, 4, COLORMAP, "colormap");
  PrintField(buf, 32, 4, SETofEVENT, "all-event-masks");
  PrintField(buf, 36, 4, SETofEVENT, "your-event-mask");
  PrintField(buf, 40, 2, SETofDEVICEEVENT, "do-not-propagate-mask");
}

DestroyWindow(buf)
     unsigned char *buf;
{
  /* Request DestroyWindow is opcode 4 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* DestroyWindow */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
}

DestroySubwindows(buf)
     unsigned char *buf;
{
  /* Request DestroySubwindows is opcode 5 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* DestroySubwindows */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
}

ChangeSaveSet(buf)
     unsigned char *buf;
{
  /* Request ChangeSaveSet is opcode 6 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ChangeSaveSet */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, INS_DEL, "mode");
  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
}

ReparentWindow(buf)
     unsigned char *buf;
{
  /* Request ReparentWindow is opcode 7 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ReparentWindow */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(4), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
  PrintField(buf, 8, 4, WINDOW, "parent");
  PrintField(buf, 12, 2, INT16, "x");
  PrintField(buf, 14, 2, INT16, "y");
}

MapWindow(buf)
     unsigned char *buf;
{
  /* Request MapWindow is opcode 8 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* MapWindow */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
}

MapSubwindows(buf)
     unsigned char *buf;
{
  /* Request MapSubwindows is opcode 9 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* MapSubwindows */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
}

UnmapWindow(buf)
     unsigned char *buf;
{
  /* Request UnmapWindow is opcode 10 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* UnmapWindow */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
}

UnmapSubwindows(buf)
     unsigned char *buf;
{
  /* Request UnmapSubwindows is opcode 11 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* UnmapSubwindows */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
}

ConfigureWindow(buf)
     unsigned char *buf;
{
  /* Request ConfigureWindow is opcode 12 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ConfigureWindow */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(3 + n), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
  PrintField(buf, 8, 2, CONFIGURE_BITMASK, "value-mask");
  PrintValues(&buf[8], 2, CONFIGURE_BITMASK, &buf[12], "value-list");
}

CirculateWindow(buf)
     unsigned char *buf;
{
  /* Request CirculateWindow is opcode 13 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* CirculateWindow */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, CIRMODE, "direction");
  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
}

GetGeometry(buf)
     unsigned char *buf;
{
  /* Request GetGeometry is opcode 14 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GetGeometry */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
}

GetGeometryReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GetGeometry */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, CARD8, "depth");
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
  PrintField(buf, 8, 4, WINDOW, "root");
  PrintField(buf, 12, 2, INT16, "x");
  PrintField(buf, 14, 2, INT16, "y");
  PrintField(buf, 16, 2, CARD16, "width");
  PrintField(buf, 18, 2, CARD16, "height");
  PrintField(buf, 20, 2, CARD16, "border-width");
}

QueryTree(buf)
     unsigned char *buf;
{
  /* Request QueryTree is opcode 15 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* QueryTree */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
}

QueryTreeReply(buf)
     unsigned char *buf;
{
  short   n;
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* QueryTree */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4(n), "reply length");
  PrintField(buf, 8, 4, WINDOW, "root");
  PrintField(buf, 12, 4, WINDOW, "parent");
  printfield(buf, 16, 2, DVALUE2(n), "number of children");
  n = IShort(&buf[16]);
  (void)PrintList(&buf[32], (long)n, WINDOW, "children");
}

InternAtom(buf)
     unsigned char *buf;
{
  short   n;
  /* Request InternAtom is opcode 16 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* InternAtom */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, BOOL, "only-if-exists");
  printfield(buf, 2, 2, DVALUE2(2 + (n + p) / 4), "request length");
  printfield(buf, 4, 2, DVALUE2(n), "length of name");
  n = IShort(&buf[4]);
  PrintString8(&buf[8], (long)n, "name");
}

InternAtomReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* InternAtom */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
  PrintField(buf, 8, 4, ATOM, "atom");
}

GetAtomName(buf)
     unsigned char *buf;
{
  /* Request GetAtomName is opcode 17 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GetAtomName */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, ATOM, "atom");
}

GetAtomNameReply(buf)
     unsigned char *buf;
{
  short   n;
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GetAtomName */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4((n + p) / 4), "reply length");
  printfield(buf, 8, 2, DVALUE2(n), "length of name");
  n = IShort(&buf[8]);
  PrintString8(&buf[32], (long)n, "name");
}

ChangeProperty(buf)
     unsigned char *buf;
{
  long    n;
  short   format;
  long    type;

  /* Request ChangeProperty is opcode 18 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ChangeProperty */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, CHANGEMODE, "mode");
  printfield(buf, 2, 2, DVALUE2(6 + (n + p) / 4), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
  PrintField(buf, 8, 4, ATOM, "property");
  PrintField(buf, 12, 4, ATOM, "type");
  type = ILong(&buf[12]);
  PrintField(buf, 16, 1, CARD8, "format");
  format = IByte(&buf[16]);
  printfield(buf, 20, 4, CARD32, "length of data");
  n = ILong(&buf[20]);
  if (type == 31 /* string */)
    PrintString8(&buf[24], n * format/8, "data");
  else if (format == 16)
    (void)PrintList(&buf[24], n, INT16, "data");
  else if (format == 32)
    (void)PrintList(&buf[24], n, INT32, "data");
  else
    PrintBytes(&buf[24], n * format/8, "data");
}

DeleteProperty(buf)
     unsigned char *buf;
{
  /* Request DeleteProperty is opcode 19 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* DeleteProperty */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(3), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
  PrintField(buf, 8, 4, ATOM, "property");
}

GetProperty(buf)
     unsigned char *buf;
{
  /* Request GetProperty is opcode 20 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GetProperty */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, BOOL, "delete");
  printfield(buf, 2, 2, CONST2(6), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
  PrintField(buf, 8, 4, ATOM, "property");
  PrintField(buf, 12, 4, ATOMT, "type");
  PrintField(buf, 16, 4, CARD32, "long-offset");
  printfield(buf, 20, 4, CARD32, "long-length");
}

GetPropertyReply(buf)
     unsigned char *buf;
{
  long    n;
  short   format;
  long    type;

  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GetProperty */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, CARD8, "format");
  format = IByte(&buf[1]);
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4((n + p) / 4), "reply length");
  PrintField(buf, 8, 4, ATOM, "type");
  type = ILong(&buf[8]);
  PrintField(buf, 12, 4, CARD32, "bytes-after");
  printfield(buf, 16, 4, CARD32, "length of value");
  n = ILong(&buf[16]);
  if (type == 31 /* string */)
    PrintString8(&buf[32], n * format/8, "value");
  else if (format == 16)
    (void)PrintList(&buf[32], n, INT16, "value");
  else if (format == 32)
    (void)PrintList(&buf[32], n, INT32, "value");
  else
    PrintBytes(&buf[32], n * format/8, "value");
}

ListProperties(buf)
     unsigned char *buf;
{
  /* Request ListProperties is opcode 21 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ListProperties */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
}

ListPropertiesReply(buf)
     unsigned char *buf;
{
  short   n;
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* ListProperties */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4(n), "reply length");
  printfield(buf, 8, 2, DVALUE2(n), "number of atoms");
  n = IShort(&buf[8]);
  (void)PrintList(&buf[32], (long)n, ATOM, "atoms");
}

SetSelectionOwner(buf)
     unsigned char *buf;
{
  /* Request SetSelectionOwner is opcode 22 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* SetSelectionOwner */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(4), "request length");
  PrintField(buf, 4, 4, WINDOW, "owner");
  PrintField(buf, 8, 4, ATOM, "selection");
  PrintField(buf, 12, 4, TIMESTAMP, "time");
}

GetSelectionOwner(buf)
     unsigned char *buf;
{
  /* Request GetSelectionOwner is opcode 23 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GetSelectionOwner */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, ATOM, "selection");
}

GetSelectionOwnerReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GetSelectionOwner */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
  PrintField(buf, 8, 4, WINDOW, "owner");
}

ConvertSelection(buf)
     unsigned char *buf;
{
  /* Request ConvertSelection is opcode 24 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ConvertSelection */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(6), "request length");
  PrintField(buf, 4, 4, WINDOW, "requestor");
  PrintField(buf, 8, 4, ATOM, "selection");
  PrintField(buf, 12, 4, ATOM, "target");
  PrintField(buf, 16, 4, ATOM, "property");
  PrintField(buf, 20, 4, TIMESTAMP, "time");
}

SendEvent(buf)
     unsigned char *buf;
{
  /* Request SendEvent is opcode 25 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* SendEvent */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, BOOL, "propagate");
  printfield(buf, 2, 2, CONST2(11), "request length");
  PrintField(buf, 4, 4, WINDOWD, "destination");
  PrintField(buf, 8, 4, SETofEVENT, "event-mask");
  PrintField(buf, 12, 32, EVENTFORM, "event");
}

GrabPointer(buf)
     unsigned char *buf;
{
  /* Request GrabPointer is opcode 26 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GrabPointer */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, BOOL, "owner-events");
  printfield(buf, 2, 2, CONST2(6), "request length");
  PrintField(buf, 4, 4, WINDOW, "grab-window");
  PrintField(buf, 8, 2, SETofPOINTEREVENT, "event-mask");
  PrintField(buf, 10, 1, PK_MODE, "pointer-mode");
  PrintField(buf, 11, 1, PK_MODE, "keyboard-mode");
  PrintField(buf, 12, 4, WINDOW, "confine-to");
  PrintField(buf, 16, 4, CURSOR, "cursor");
  PrintField(buf, 20, 4, TIMESTAMP, "time");
}

GrabPointerReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GrabPointer */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, GRABSTAT, "status");
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
}

UngrabPointer(buf)
     unsigned char *buf;
{
  /* Request UngrabPointer is opcode 27 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* UngrabPointer */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, TIMESTAMP, "time");
}

GrabButton(buf)
     unsigned char *buf;
{
  /* Request GrabButton is opcode 28 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GrabButton */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, BOOL, "owner-events");
  printfield(buf, 2, 2, CONST2(6), "request length");
  PrintField(buf, 4, 4, WINDOW, "grab-window");
  PrintField(buf, 8, 2, SETofPOINTEREVENT, "event-mask");
  PrintField(buf, 10, 1, PK_MODE, "pointer-mode");
  PrintField(buf, 11, 1, PK_MODE, "keyboard-mode");
  PrintField(buf, 12, 4, WINDOW, "confine-to");
  PrintField(buf, 16, 4, CURSOR, "cursor");
  PrintField(buf, 20, 1, BUTTONA, "button");
  PrintField(buf, 22, 2, SETofKEYMASK, "modifiers");
}

UngrabButton(buf)
     unsigned char *buf;
{
  /* Request UngrabButton is opcode 29 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* UngrabButton */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, BUTTONA, "button");
  printfield(buf, 2, 2, CONST2(3), "request length");
  PrintField(buf, 4, 4, WINDOW, "grab-window");
  PrintField(buf, 8, 2, SETofKEYMASK, "modifiers");
}

ChangeActivePointerGrab(buf)
     unsigned char *buf;
{
  /* Request ChangeActivePointerGrab is opcode 30 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ChangeActivePointerGrab */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(4), "request length");
  PrintField(buf, 4, 4, CURSOR, "cursor");
  PrintField(buf, 8, 4, TIMESTAMP, "time");
  PrintField(buf, 12, 2, SETofPOINTEREVENT, "event-mask");
}

GrabKeyboard(buf)
     unsigned char *buf;
{
  /* Request GrabKeyboard is opcode 31 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GrabKeyboard */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, BOOL, "owner-events");
  printfield(buf, 2, 2, CONST2(4), "request length");
  PrintField(buf, 4, 4, WINDOW, "grab-window");
  PrintField(buf, 8, 4, TIMESTAMP, "time");
  PrintField(buf, 12, 1, PK_MODE, "pointer-mode");
  PrintField(buf, 13, 1, PK_MODE, "keyboard-mode");
}

GrabKeyboardReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GrabKeyboard */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, GRABSTAT, "status");
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
}

UngrabKeyboard(buf)
     unsigned char *buf;
{
  /* Request UngrabKeyboard is opcode 32 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* UngrabKeyboard */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, TIMESTAMP, "time");
}

GrabKey(buf)
     unsigned char *buf;
{
  /* Request GrabKey is opcode 33 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GrabKey */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, BOOL, "owner-events");
  printfield(buf, 2, 2, CONST2(4), "request length");
  PrintField(buf, 4, 4, WINDOW, "grab-window");
  PrintField(buf, 8, 2, SETofKEYMASK, "modifiers");
  PrintField(buf, 10, 1, KEYCODEA, "key");
  PrintField(buf, 11, 1, PK_MODE, "pointer-mode");
  PrintField(buf, 12, 1, PK_MODE, "keyboard-mode");
}

UngrabKey(buf)
     unsigned char *buf;
{
  /* Request UngrabKey is opcode 34 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* UngrabKey */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, KEYCODEA, "key");
  printfield(buf, 2, 2, CONST2(3), "request length");
  PrintField(buf, 4, 4, WINDOW, "grab-window");
  PrintField(buf, 8, 2, SETofKEYMASK, "modifiers");
}

AllowEvents(buf)
     unsigned char *buf;
{
  /* Request AllowEvents is opcode 35 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* AllowEvents */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, EVENTMODE, "mode");
  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, TIMESTAMP, "time");
}

GrabServer(buf)
     unsigned char *buf;
{
  /* Request GrabServer is opcode 36 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GrabServer */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(1), "request length");
}

UngrabServer(buf)
     unsigned char *buf;
{
  /* Request UngrabServer is opcode 37 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* UngrabServer */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(1), "request length");
}

QueryPointer(buf)
     unsigned char *buf;
{
  /* Request QueryPointer is opcode 38 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* QueryPointer */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
}

QueryPointerReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* QueryPointer */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, BOOL, "same-screen");
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
  PrintField(buf, 8, 4, WINDOW, "root");
  PrintField(buf, 12, 4, WINDOW, "child");
  PrintField(buf, 16, 2, INT16, "root-x");
  PrintField(buf, 18, 2, INT16, "root-y");
  PrintField(buf, 20, 2, INT16, "win-x");
  PrintField(buf, 22, 2, INT16, "win-y");
  PrintField(buf, 24, 2, SETofKEYBUTMASK, "mask");
}

GetMotionEvents(buf)
     unsigned char *buf;
{
  /* Request GetMotionEvents is opcode 39 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GetMotionEvents */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(4), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
  PrintField(buf, 8, 4, TIMESTAMP, "start");
  PrintField(buf, 12, 4, TIMESTAMP, "stop");
}

GetMotionEventsReply(buf)
     unsigned char *buf;
{
  long   n;
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GetMotionEvents */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4(2*n), "reply length");
  printfield(buf, 8, 4, DVALUE4(n), "number of events");
  n = ILong(&buf[8]);
  (void)PrintList(&buf[32], n, TIMECOORD, "events");
}

TranslateCoordinates(buf)
     unsigned char *buf;
{
  /* Request TranslateCoordinates is opcode 40 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* TranslateCoordinates */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(4), "request length");
  PrintField(buf, 4, 4, WINDOW, "src-window");
  PrintField(buf, 8, 4, WINDOW, "dst-window");
  PrintField(buf, 12, 2, INT16, "src-x");
  PrintField(buf, 14, 2, INT16, "src-y");
}

TranslateCoordinatesReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* TranslateCoordinates */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, BOOL, "same-screen");
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
  PrintField(buf, 8, 4, WINDOW, "child");
  PrintField(buf, 12, 2, INT16, "dst-x");
  PrintField(buf, 14, 2, INT16, "dst-y");
}

WarpPointer(buf)
     unsigned char *buf;
{
  /* Request WarpPointer is opcode 41 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* WarpPointer */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(6), "request length");
  PrintField(buf, 4, 4, WINDOW, "src-window");
  PrintField(buf, 8, 4, WINDOW, "dst-window");
  PrintField(buf, 12, 2, INT16, "src-x");
  PrintField(buf, 14, 2, INT16, "src-y");
  PrintField(buf, 16, 2, CARD16, "src-width");
  PrintField(buf, 18, 2, CARD16, "src-height");
  PrintField(buf, 20, 2, INT16, "dst-x");
  PrintField(buf, 22, 2, INT16, "dst-y");
}

SetInputFocus(buf)
     unsigned char *buf;
{
  /* Request SetInputFocus is opcode 42 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* SetInputFocus */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, FOCUSAGENT, "revert-to");
  printfield(buf, 2, 2, CONST2(3), "request length");
  PrintField(buf, 4, 4, WINDOWNR, "focus");
  PrintField(buf, 8, 4, TIMESTAMP, "time");
}

GetInputFocus(buf)
     unsigned char *buf;
{
  /* Request GetInputFocus is opcode 43 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GetInputFocus */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(1), "request length");
}

GetInputFocusReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GetInputFocus */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, FOCUSAGENT, "revert-to");
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
  PrintField(buf, 8, 4, WINDOWNR, "focus");
}

QueryKeymap(buf)
     unsigned char *buf;
{
  /* Request QueryKeymap is opcode 44 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* QueryKeymap */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(1), "request length");
}

QueryKeymapReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* QueryKeymap */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(2), "reply length");
  PrintBytes(&buf[8], 32L, "keys");
}

OpenFont(buf)
     unsigned char *buf;
{
  short   n;

  /* Request OpenFont is opcode 45 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* OpenFont */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(3 + (n + p) / 4), "request length");
  PrintField(buf, 4, 4, FONT, "font-id");
  printfield(buf, 8, 2, DVALUE2(n), "length of name");
  n = IShort(&buf[8]);
  PrintString8(&buf[12], (long)n, "name");
}

CloseFont(buf)
     unsigned char *buf;
{
  /* Request CloseFont is opcode 46 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* CloseFont */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, FONT, "font");
}

QueryFont(buf)
     unsigned char *buf;
{
  /* Request QueryFont is opcode 47 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* QueryFont */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, FONTABLE, "font");
}

QueryFontReply(buf)
     unsigned char *buf;
{
  short   n;
  long    m;
  long   k;

  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* QueryFont */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4(7 + 2*n + 3*m), "reply length");
  PrintField(buf, 8, 12, CHARINFO, "min-bounds");
  PrintField(buf, 24, 12, CHARINFO, "max-bounds");
  PrintField(buf, 40, 2, CARD16, "min-char-or-byte2");
  PrintField(buf, 42, 2, CARD16, "max-char-or-byte2");
  PrintField(buf, 44, 2, CARD16, "default-char");
  printfield(buf, 46, 2, DVALUE2(n), "number of FONTPROPs");
  n = IShort(&buf[46]);
  PrintField(buf, 48, 1, DIRECT, "draw-direction");
  PrintField(buf, 49, 1, CARD8, "min-byte1");
  PrintField(buf, 50, 1, CARD8, "max-byte1");
  PrintField(buf, 51, 1, BOOL, "all-chars-exist");
  PrintField(buf, 52, 2, INT16, "font-ascent");
  PrintField(buf, 54, 2, INT16, "font-descent");
  printfield(buf, 56, 4, DVALUE4(m), "number of CHARINFOs");
  m = ILong(&buf[56]);
  k = PrintList(&buf[60], (long)n, FONTPROP, "properties");
  (void)PrintList(&buf[60 + k], (long)m, CHARINFO, "char-infos");
}

QueryTextExtents(buf)
     unsigned char *buf;
{
  short   n;

  /* Request QueryTextExtents is opcode 48 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* QueryTextExtents */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 1, 1, BOOL, "odd length?");
  printfield(buf, 2, 2, DVALUE2(2 + (2*n + p) / 4), "request length");
  n = (IShort(&buf[2]) - 2) * 4 / 2;
  if (IBool(&buf[1]))
    n -= 1;
  PrintField(buf, 4, 4, FONTABLE, "font");
  PrintString16(&buf[8], (long)n, "string");
}

QueryTextExtentsReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* QueryTextExtents */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, DIRECT, "draw-direction");
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
  PrintField(buf, 8, 2, INT16, "font-ascent");
  PrintField(buf, 10, 2, INT16, "font-descent");
  PrintField(buf, 12, 2, INT16, "overall-ascent");
  PrintField(buf, 14, 2, INT16, "overall-descent");
  PrintField(buf, 16, 4, INT32, "overall-width");
  PrintField(buf, 20, 4, INT32, "overall-left");
  PrintField(buf, 24, 4, INT32, "overall-right");
}

ListFonts(buf)
     unsigned char *buf;
{
  short   n;

  /* Request ListFonts is opcode 49 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ListFonts */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(2 + (n + p) / 4), "request length");
  PrintField(buf, 4, 2, CARD16, "max-names");
  printfield(buf, 6, 2, DVALUE2(n), "length of pattern");
  n = IShort(&buf[6]);
  PrintString8(&buf[8], (long)n, "pattern");
}

ListFontsReply(buf)
     unsigned char *buf;
{
  short   n;

  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* ListFonts */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4((n + p) / 4), "reply length");
  printfield(buf, 8, 2, CARD16, "number of names");
  n = IShort(&buf[8]);
  PrintListSTR(&buf[32], (long)n, "names");
}

ListFontsWithInfo(buf)
     unsigned char *buf;
{
  short   n;

  /* Request ListFontsWithInfo is opcode 50 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ListFontsWithInfo */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(2 + (n + p) / 4), "request length");
  PrintField(buf, 4, 2, CARD16, "max-names");
  printfield(buf, 6, 2, DVALUE2(n), "length of pattern");
  n = IShort(&buf[6]);
  PrintString8(&buf[8], (long)n, "pattern");
}

ListFontsWithInfoReply(buf)
     unsigned char *buf;
{
  short which;

  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* ListFontsWithInfo */ ;
  if (Verbose < 1) return;
  which = IByte(&buf[1]);
  if (which != 0)
    {
      ListFontsWithInfoReply1(buf);
      KeepLastReplyExpected();
    }
  else
    ListFontsWithInfoReply2(buf);
}

ListFontsWithInfoReply1(buf)
     unsigned char *buf;
{
  short   n;
  short   m;
  printfield(buf, 1, 1, DVALUE1(n), "length of name in bytes");
  n = IByte(&buf[1]);
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4(7 + 2*m + (n + p) / 4), "reply length");
  PrintField(buf, 8, 12, CHARINFO, "min-bounds");
  PrintField(buf, 24, 12, CHARINFO, "max-bounds");
  PrintField(buf, 40, 2, CARD16, "min-char-or-byte2");
  PrintField(buf, 42, 2, CARD16, "max-char-or-byte2");
  PrintField(buf, 44, 2, CARD16, "default-char");
  printfield(buf, 46, 2, DVALUE2(m), "number of FONTPROPs");
  m = IShort(&buf[46]);
  PrintField(buf, 48, 1, DIRECT, "draw-direction");
  PrintField(buf, 49, 1, CARD8, "min-byte1");
  PrintField(buf, 50, 1, CARD8, "max-byte1");
  PrintField(buf, 51, 1, BOOL, "all-chars-exist");
  PrintField(buf, 52, 2, INT16, "font-ascent");
  PrintField(buf, 54, 2, INT16, "font-descent");
  PrintField(buf, 56, 4, CARD32, "replies-hint");
  (void)PrintList(&buf[60], (long)m, FONTPROP, "properties");
  PrintString8(&buf[60 + 8 * m], (long)n, "name");
}

ListFontsWithInfoReply2(buf)
     unsigned char *buf;
{
  PrintField(buf, 1, 1, CONST1(0), "last-reply indicator");
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(7), "reply length");
}

SetFontPath(buf)
     unsigned char *buf;
{
  short   n;
  /* Request SetFontPath is opcode 51 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* SetFontPath */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(2 + (n + p) / 4), "request length");
  printfield(buf, 4, 2, CARD16, "number of paths");
  n = IShort(&buf[4]);
  PrintListSTR(&buf[8], (long)n, "paths");
}

GetFontPath(buf)
     unsigned char *buf;
{
  /* Request GetFontPath is opcode 52 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GetFontPath */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 2, 2, CONST2(1), "request list");
}

GetFontPathReply(buf)
     unsigned char *buf;
{
  short   n;
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GetFontPath */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4((n + p) / 4), "reply length");
  printfield(buf, 8, 2, CARD16, "number of paths");
  n = IShort(&buf[8]);
  PrintListSTR(&buf[32], (long)n, "paths");
}

CreatePixmap(buf)
     unsigned char *buf;
{
  /* Request CreatePixmap is opcode 53 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* CreatePixmap */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, CARD8, "depth");
  printfield(buf, 2, 2, CONST2(4), "request length");
  PrintField(buf, 4, 4, PIXMAP, "pixmap-id");
  PrintField(buf, 8, 4, DRAWABLE, "drawable");
  PrintField(buf, 12, 2, CARD16, "width");
  PrintField(buf, 14, 2, CARD16, "height");
}

FreePixmap(buf)
     unsigned char *buf;
{
  /* Request FreePixmap is opcode 54 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* FreePixmap */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, PIXMAP, "pixmap");
}

CreateGC(buf)
     unsigned char *buf;
{
  /* Request CreateGC is opcode 55 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* CreateGC */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(4 + n), "request length");
  PrintField(buf, 4, 4, GCONTEXT, "graphic-context-id");
  PrintField(buf, 8, 4, DRAWABLE, "drawable");
  PrintField(buf, 12, 4, GC_BITMASK, "value-mask");
  PrintValues(&buf[12], 4, GC_BITMASK, &buf[16], "value-list");
}

ChangeGC(buf)
     unsigned char *buf;
{
  /* Request ChangeGC is opcode 56 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ChangeGC */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(3 + n), "request length");
  PrintField(buf, 4, 4, GCONTEXT, "gc");
  PrintField(buf, 8, 4, GC_BITMASK, "value-mask");
  PrintValues(&buf[8], 4, GC_BITMASK, &buf[12], "value-list");
}

CopyGC(buf)
     unsigned char *buf;
{
  /* Request CopyGC is opcode 57 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* CopyGC */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(4), "request length");
  PrintField(buf, 4, 4, GCONTEXT, "src-gc");
  PrintField(buf, 8, 4, GCONTEXT, "dst-gc");
  PrintField(buf, 12, 4, GC_BITMASK, "value-mask");
}

SetDashes(buf)
     unsigned char *buf;
{
  short   n;
  /* Request SetDashes is opcode 58 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* SetDashes */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(3 + (n + p) / 4), "request length");
  PrintField(buf, 4, 4, GCONTEXT, "gc");
  PrintField(buf, 8, 2, CARD16, "dash-offset");
  printfield(buf, 10, 2, DVALUE2(n), "length of dashes");
  n = IShort(&buf[10]);
  PrintBytes(&buf[12], (long)n, "dashes");
}

SetClipRectangles(buf)
     unsigned char *buf;
{
  short   n;

  /* Request SetClipRectangles is opcode 59 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* SetClipRectangles */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, RECTORDER, "ordering");
  printfield(buf, 2, 2, DVALUE2(3 + 2*n), "request length");
  n = (IShort(&buf[2]) - 3) / 2;
  PrintField(buf, 4, 4, GCONTEXT, "gc");
  PrintField(buf, 8, 2, INT16, "clip-x-origin");
  PrintField(buf, 10, 2, INT16, "clip-y-origin");
  (void)PrintList(&buf[12], (long)n, RECTANGLE, "rectangles");
}

FreeGC(buf)
     unsigned char *buf;
{
  /* Request FreeGC is opcode 60 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* FreeGC */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, GCONTEXT, "gc");
}

ClearArea(buf)
     unsigned char *buf;
{
  /* Request ClearArea is opcode 61 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ClearArea */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, BOOL, "exposures");
  printfield(buf, 2, 2, CONST2(4), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
  PrintField(buf, 8, 2, INT16, "x");
  PrintField(buf, 10, 2, INT16, "y");
  PrintField(buf, 12, 2, CARD16, "width");
  PrintField(buf, 14, 2, CARD16, "height");
}

CopyArea(buf)
     unsigned char *buf;
{
  /* Request CopyArea is opcode 62 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* CopyArea */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(7), "request length");
  PrintField(buf, 4, 4, DRAWABLE, "src-drawable");
  PrintField(buf, 8, 4, DRAWABLE, "dst-drawable");
  PrintField(buf, 12, 4, GCONTEXT, "gc");
  PrintField(buf, 16, 2, INT16, "src-x");
  PrintField(buf, 18, 2, INT16, "src-y");
  PrintField(buf, 20, 2, INT16, "dst-x");
  PrintField(buf, 22, 2, INT16, "dst-y");
  PrintField(buf, 24, 2, CARD16, "width");
  PrintField(buf, 26, 2, CARD16, "height");
}

CopyPlane(buf)
     unsigned char *buf;
{
  /* Request CopyPlane is opcode 63 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* CopyPlane */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(8), "request length");
  PrintField(buf, 4, 4, DRAWABLE, "src-drawable");
  PrintField(buf, 8, 4, DRAWABLE, "dst-drawable");
  PrintField(buf, 12, 4, GCONTEXT, "gc");
  PrintField(buf, 16, 2, INT16, "src-x");
  PrintField(buf, 18, 2, INT16, "src-y");
  PrintField(buf, 20, 2, INT16, "dst-x");
  PrintField(buf, 22, 2, INT16, "dst-y");
  PrintField(buf, 24, 2, CARD16, "width");
  PrintField(buf, 26, 2, CARD16, "height");
  PrintField(buf, 28, 4, CARD32, "bit-plane");
}

PolyPoint(buf)
     unsigned char *buf;
{
  short   n;
  /* Request PolyPoint is opcode 64 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* PolyPoint */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, COORMODE, "coordinate-mode");
  printfield(buf, 2, 2, DVALUE2(3 + n), "request length");
  n = (IShort(&buf[2]) - 3);
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 4, GCONTEXT, "gc");
  (void)PrintList(&buf[12], (long)n, POINT, "points");
}

PolyLine(buf)
     unsigned char *buf;
{
  short   n;
  /* Request PolyLine is opcode 65 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* PolyLine */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, COORMODE, "coordinate-mode");
  printfield(buf, 2, 2, DVALUE2(3 + n), "request length");
  n = (IShort(&buf[2]) - 3);
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 4, GCONTEXT, "gc");
  (void)PrintList(&buf[12], (long)n, POINT, "points");
}

PolySegment(buf)
     unsigned char *buf;
{
  short   n;
  /* Request PolySegment is opcode 66 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* PolySegment */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(3 + 2*n), "request length");
  n = (IShort(&buf[2]) - 3) / 2;
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 4, GCONTEXT, "gc");
  (void)PrintList(&buf[12], (long)n, SEGMENT, "segments");
}

PolyRectangle(buf)
     unsigned char *buf;
{
  short   n;
  /* Request PolyRectangle is opcode 67 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* PolyRectangle */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(3 + 2*n), "request length");
  n = (IShort(&buf[2]) - 3) / 2;
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 4, GCONTEXT, "gc");
  (void)PrintList(&buf[12], (long)n, RECTANGLE, "rectangles");
}

PolyArc(buf)
     unsigned char *buf;
{
  short   n;
  /* Request PolyArc is opcode 68 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* PolyArc */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(3 + 3*n), "request length");
  n = (IShort(&buf[2]) - 3) / 3;
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 4, GCONTEXT, "gc");
  (void)PrintList(&buf[12], (long)n, ARC, "arcs");
}

FillPoly(buf)
     unsigned char *buf;
{
  short   n;
  /* Request FillPoly is opcode 69 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* FillPoly */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(4 + n), "request length");
  n = (IShort(&buf[2]) - 4);
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 4, GCONTEXT, "gc");
  PrintField(buf, 12, 1, POLYSHAPE, "shape");
  PrintField(buf, 13, 1, COORMODE, "coordinate-mode");
  (void)PrintList(&buf[16], (long)n, POINT, "points");
}

PolyFillRectangle(buf)
     unsigned char *buf;
{
  short   n;
  /* Request PolyFillRectangle is opcode 70 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* PolyFillRectangle */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(3 + 2*n), "request length");
  n = (IShort(&buf[2]) - 3) / 2;
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 4, GCONTEXT, "gc");
  (void)PrintList(&buf[12], (long)n, RECTANGLE, "rectangles");
}

PolyFillArc(buf)
     unsigned char *buf;
{
  short   n;
  /* Request PolyFillArc is opcode 71 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* PolyFillArc */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(3 + 3*n), "request length");
  n = (IShort(&buf[2]) - 3) / 3;
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 4, GCONTEXT, "gc");
  (void)PrintList(&buf[12], (long)n, ARC, "arcs");
}

PutImage(buf)
     unsigned char *buf;
{
  short   n;
  /* Request PutImage is opcode 72 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* PutImage */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, IMAGEMODE, "format");
  printfield(buf, 2, 2, DVALUE2(6 + (n + p) / 4), "request length");

  /* the size of the Image is overestimated by the following computation of n,
     because we ignore that padding of the request to a multiple of 4 bytes.
     The image may not be a multiple of 4 bytes.  The actual size of the image
     is determined as follows: for format = Bitmap or format = XYPixmap, the
     size is (left-pad + width) [ pad to multiple of bitmap-scanline-pad from
     SetUpReply ] divide by 8 to get bytes times height times depth for format
     = ZPixmap, take the depth and use it to find the bits-per-pixel and
     scanline-pad given in one of the SetUpReply DEPTH records. width *
     bits-per-pixel pad to multiple of scanline-pad divide by 8 to get bytes
     times height times depth For simplicity, we ignore all this and just use
     the request length to (over)estimate the size of the image */

  n = (IShort(&buf[2]) - 6) * 4;
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 4, GCONTEXT, "gc");
  PrintField(buf, 12, 2, CARD16, "width");
  PrintField(buf, 14, 2, CARD16, "height");
  PrintField(buf, 16, 2, INT16, "dst-x");
  PrintField(buf, 18, 2, INT16, "dst-y");
  PrintField(buf, 20, 1, CARD8, "left-pad");
  PrintField(buf, 21, 1, CARD8, "depth");
  PrintBytes(&buf[24], (long)n, "data");
}

GetImage(buf)
     unsigned char *buf;
{
  /* Request GetImage is opcode 73 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GetImage */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, IMAGEMODE, "format");
  printfield(buf, 2, 2, CONST2(5), "request length");
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 2, INT16, "x");
  PrintField(buf, 10, 2, INT16, "y");
  PrintField(buf, 12, 2, CARD16, "width");
  PrintField(buf, 14, 2, CARD16, "height");
  PrintField(buf, 16, 4, CARD32, "plane-mask");
}

GetImageReply(buf)
     unsigned char *buf;
{
  long    n;

  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GetImage */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, CARD8, "depth");
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4((n + p) / 4), "reply length");

  /* to properly compute the actual size of the image, we have to remember the
     width and height values from the request.  Again, we (over)estimate its
     length from the length of the reply */
  n = ILong(&buf[4]) * 4;
  PrintField(buf, 8, 4, VISUALID, "visual");
  PrintBytes(&buf[32], n, "data");
}

PolyText8(buf)
     unsigned char *buf;
{
  short   n;

  /* Request PolyText8 is opcode 74 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* PolyText8 */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(4 + (n + p) / 4), "request length");
  n = (IShort(&buf[2]) - 4) * 4;
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 4, GCONTEXT, "gc");
  PrintField(buf, 12, 2, INT16, "x");
  PrintField(buf, 14, 2, INT16, "y");
  PrintTextList8(&buf[16], n, "items");
}

PolyText16(buf)
     unsigned char *buf;
{
  short   n;

  /* Request PolyText16 is opcode 75 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* PolyText16 */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(4 + (n + p) / 4), "request length");
  n = (IShort(&buf[2]) - 4) * 4;
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 4, GCONTEXT, "gc");
  PrintField(buf, 12, 2, INT16, "x");
  PrintField(buf, 14, 2, INT16, "y");
  PrintTextList16(&buf[16], n, "items");
}

ImageText8(buf)
     unsigned char *buf;
{
  short   n;
  /* Request ImageText8 is opcode 76 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ImageText8 */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 1, 1, DVALUE1(n), "length of string");
  n = IByte(&buf[1]);
  printfield(buf, 2, 2, DVALUE2(4 + (n + p) / 4), "request length");
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 4, GCONTEXT, "gc");
  PrintField(buf, 12, 2, INT16, "x");
  PrintField(buf, 14, 2, INT16, "y");
  PrintString8(&buf[16], (long)n, "string");
}

ImageText16(buf)
     unsigned char *buf;
{
  short   n;
  /* Request ImageText16 is opcode 77 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ImageText16 */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 1, 1, DVALUE1(n), "length of string");
  n = IByte(&buf[1]);
  printfield(buf, 2, 2, DVALUE2(4 + (2*n + p) / 4), "request length");
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 4, GCONTEXT, "gc");
  PrintField(buf, 12, 2, INT16, "x");
  PrintField(buf, 14, 2, INT16, "y");
  PrintString16(&buf[16], (long)n, "string");
}

CreateColormap(buf)
     unsigned char *buf;
{
  /* Request CreateColormap is opcode 78 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* CreateColormap */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, ALLORNONE, "alloc");
  printfield(buf, 2, 2, CONST2(4), "request length");
  PrintField(buf, 4, 4, COLORMAP, "color-map-id");
  PrintField(buf, 8, 4, WINDOW, "window");
  PrintField(buf, 12, 4, VISUALID, "visual");
}

FreeColormap(buf)
     unsigned char *buf;
{
  /* Request FreeColormap is opcode 79 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* FreeColormap */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, COLORMAP, "cmap");
}

CopyColormapAndFree(buf)
     unsigned char *buf;
{
  /* Request CopyColormapAndFree is opcode 80 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* CopyColormapAndFree */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(3), "request length");
  PrintField(buf, 4, 4, COLORMAP, "color-map-id");
  PrintField(buf, 8, 4, COLORMAP, "src-cmap");
}

InstallColormap(buf)
     unsigned char *buf;
{
  /* Request InstallColormap is opcode 81 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* InstallColormap */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, COLORMAP, "cmap");
}

UninstallColormap(buf)
     unsigned char *buf;
{
  /* Request UninstallColormap is opcode 82 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* UninstallColormap */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, COLORMAP, "cmap");
}

ListInstalledColormaps(buf)
     unsigned char *buf;
{
  /* Request ListInstalledColormaps is opcode 83 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ListInstalledColormaps */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
}

ListInstalledColormapsReply(buf)
     unsigned char *buf;
{
  short   n;
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* ListInstalledColormaps */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4(n), "reply length");
  printfield(buf, 8, 2, DVALUE2(n), "number of cmaps");
  n = IShort(&buf[8]);
  (void)PrintList(&buf[32], (long)n, COLORMAP, "cmaps");
}

AllocColor(buf)
     unsigned char *buf;
{
  /* Request AllocColor is opcode 84 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* AllocColor */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(4), "request length");
  PrintField(buf, 4, 4, COLORMAP, "cmap");
  PrintField(buf, 8, 2, CARD16, "red");
  PrintField(buf, 10, 2, CARD16, "green");
  PrintField(buf, 12, 2, CARD16, "blue");
}

AllocColorReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* AllocColor */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
  PrintField(buf, 8, 2, CARD16, "red");
  PrintField(buf, 10, 2, CARD16, "green");
  PrintField(buf, 12, 2, CARD16, "blue");
  PrintField(buf, 16, 4, CARD32, "pixel");
}

AllocNamedColor(buf)
     unsigned char *buf;
{
  short   n;
  /* Request AllocNamedColor is opcode 85 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* AllocNamedColor */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(3 + (n + p) / 4), "request length");
  PrintField(buf, 4, 4, COLORMAP, "cmap");
  printfield(buf, 8, 2, DVALUE2(n), "length of name");
  n = IShort(&buf[8]);
  PrintString8(&buf[12], (long)n, "name");
}

AllocNamedColorReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* AllocNamedColor */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
  PrintField(buf, 8, 4, CARD32, "pixel");
  PrintField(buf, 12, 2, CARD16, "exact-red");
  PrintField(buf, 14, 2, CARD16, "exact-green");
  PrintField(buf, 16, 2, CARD16, "exact-blue");
  PrintField(buf, 18, 2, CARD16, "visual-red");
  PrintField(buf, 20, 2, CARD16, "visual-green");
  PrintField(buf, 22, 2, CARD16, "visual-blue");
}

AllocColorCells(buf)
     unsigned char *buf;
{
  /* Request AllocColorCells is opcode 86 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* AllocColorCells */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, BOOL, "contiguous");
  printfield(buf, 2, 2, CONST2(3), "request length");
  PrintField(buf, 4, 4, COLORMAP, "cmap");
  PrintField(buf, 8, 2, CARD16, "colors");
  PrintField(buf, 10, 2, CARD16, "planes");
}

AllocColorCellsReply(buf)
     unsigned char *buf;
{
  short   n;
  short   m;
  short   k;
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* AllocColorCells */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4(n + m), "reply length");
  printfield(buf, 8, 2, DVALUE2(n), "number of pixels");
  n = IShort(&buf[8]);
  printfield(buf, 10, 2, DVALUE2(m), "number of masks");
  m = IShort(&buf[10]);
  k = PrintList(&buf[32], (long)n, CARD32, "pixels");
  (void)PrintList(&buf[32 + k], (long)m, CARD32, "masks");
}

AllocColorPlanes(buf)
     unsigned char *buf;
{
  /* Request AllocColorPlanes is opcode 87 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* AllocColorPlanes */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, BOOL, "contiguous");
  printfield(buf, 2, 2, CONST2(4), "request length");
  PrintField(buf, 4, 4, COLORMAP, "cmap");
  PrintField(buf, 8, 2, CARD16, "colors");
  PrintField(buf, 10, 2, CARD16, "reds");
  PrintField(buf, 12, 2, CARD16, "greens");
  PrintField(buf, 14, 2, CARD16, "blues");
}

AllocColorPlanesReply(buf)
     unsigned char *buf;
{
  short   n;
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* AllocColorPlanes */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4(n), "reply length");
  printfield(buf, 8, 2, DVALUE2(n), "number of pixels");
  n = IShort(&buf[8]);
  PrintField(buf, 12, 4, CARD32, "red-mask");
  PrintField(buf, 16, 4, CARD32, "green-mask");
  PrintField(buf, 20, 4, CARD32, "blue-mask");
  (void)PrintList(&buf[32], (long)n, CARD32, "pixels");
}

FreeColors(buf)
     unsigned char *buf;
{
  short   n;

  /* Request FreeColors is opcode 88 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* FreeColors */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(3 + n), "request length");
  n = IShort(&buf[2]) - 3;
  PrintField(buf, 4, 4, COLORMAP, "cmap");
  PrintField(buf, 8, 4, CARD32, "plane-mask");
  (void)PrintList(&buf[12], (long)n, CARD32, "pixels");
}

StoreColors(buf)
     unsigned char *buf;
{
  short   n;
  /* Request StoreColors is opcode 89 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* StoreColors */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(2 + 3*n), "request length");
  n = (IShort(&buf[2]) - 2) / 3;
  PrintField(buf, 4, 4, COLORMAP, "cmap");
  (void)PrintList(&buf[8], (long)n, COLORITEM, "items");
}

StoreNamedColor(buf)
     unsigned char *buf;
{
  short   n;
  /* Request StoreNamedColor is opcode 90 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* StoreNamedColor */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, COLORMASK, "which colors?");
  printfield(buf, 2, 2, DVALUE2(4 + (n + p) / 4), "request length");
  PrintField(buf, 4, 4, COLORMAP, "cmap");
  PrintField(buf, 8, 4, CARD32, "pixel");
  printfield(buf, 12, 2, DVALUE2(n), "length of name");
  n = IShort(&buf[12]);
  PrintString8(&buf[16], (long)n, "name");
}

QueryColors(buf)
     unsigned char *buf;
{
  short   n;
  /* Request QueryColors is opcode 91 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* QueryColors */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(2 + n), "request length");
  n = IShort(&buf[2]) - 2;
  PrintField(buf, 4, 4, COLORMAP, "cmap");
  (void)PrintList(&buf[8], (long)n, CARD32, "pixels");
}

QueryColorsReply(buf)
     unsigned char *buf;
{
  short   n;
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* QueryColors */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4(2*n), "reply length");
  printfield(buf, 8, 2, DVALUE2(n), "number of colors");
  n = IShort(&buf[8]);
  (void)PrintList(&buf[32], (long)n, RGB, "colors");
}

LookupColor(buf)
     unsigned char *buf;
{
  short   n;
  /* Request LookupColor is opcode 92 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* LookupColor */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(3 + (n + p) / 4), "request length");
  PrintField(buf, 4, 4, COLORMAP, "cmap");
  printfield(buf, 8, 2, DVALUE2(n), "length of name");
  n = IShort(&buf[8]);
  PrintString8(&buf[12], (long)n, "name");
}

LookupColorReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* LookupColor */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
  PrintField(buf, 8, 2, CARD16, "exact-red");
  PrintField(buf, 10, 2, CARD16, "exact-green");
  PrintField(buf, 12, 2, CARD16, "exact-blue");
  PrintField(buf, 14, 2, CARD16, "visual-red");
  PrintField(buf, 16, 2, CARD16, "visual-green");
  PrintField(buf, 18, 2, CARD16, "visual-blue");
}

CreateCursor(buf)
     unsigned char *buf;
{
  /* Request CreateCursor is opcode 93 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* CreateCursor */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(8), "request length");
  PrintField(buf, 4, 4, CURSOR, "cursor-id");
  PrintField(buf, 8, 4, PIXMAP, "source");
  PrintField(buf, 12, 4, PIXMAP, "mask");
  PrintField(buf, 16, 2, CARD16, "fore-red");
  PrintField(buf, 18, 2, CARD16, "fore-green");
  PrintField(buf, 20, 2, CARD16, "fore-blue");
  PrintField(buf, 22, 2, CARD16, "back-red");
  PrintField(buf, 24, 2, CARD16, "back-green");
  PrintField(buf, 26, 2, CARD16, "back-blue");
  PrintField(buf, 28, 2, CARD16, "x");
  PrintField(buf, 30, 2, CARD16, "y");
}

CreateGlyphCursor(buf)
     unsigned char *buf;
{
  /* Request CreateGlyphCursor is opcode 94 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* CreateGlyphCursor */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(8), "request length");
  PrintField(buf, 4, 4, CURSOR, "cursor-id");
  PrintField(buf, 8, 4, FONT, "source-font");
  PrintField(buf, 12, 4, FONT, "mask-font");
  PrintField(buf, 16, 2, CARD16, "source-char");
  PrintField(buf, 18, 2, CARD16, "mask-char");
  PrintField(buf, 20, 2, CARD16, "fore-red");
  PrintField(buf, 22, 2, CARD16, "fore-green");
  PrintField(buf, 24, 2, CARD16, "fore-blue");
  PrintField(buf, 26, 2, CARD16, "back-red");
  PrintField(buf, 28, 2, CARD16, "back-green");
  PrintField(buf, 30, 2, CARD16, "back-blue");
}

FreeCursor(buf)
     unsigned char *buf;
{
  /* Request FreeCursor is opcode 95 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* FreeCursor */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, CURSOR, "cursor");
}

RecolorCursor(buf)
     unsigned char *buf;
{
  /* Request RecolorCursor is opcode 96 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* RecolorCursor */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(5), "request length");
  PrintField(buf, 4, 4, CURSOR, "cursor");
  PrintField(buf, 8, 2, CARD16, "fore-red");
  PrintField(buf, 10, 2, CARD16, "fore-green");
  PrintField(buf, 12, 2, CARD16, "fore-blue");
  PrintField(buf, 14, 2, CARD16, "back-red");
  PrintField(buf, 16, 2, CARD16, "back-green");
  PrintField(buf, 18, 2, CARD16, "back-blue");
}

QueryBestSize(buf)
     unsigned char *buf;
{
  /* Request QueryBestSize is opcode 97 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* QueryBestSize */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, OBJECTCLASS, "class");
  printfield(buf, 2, 2, CONST2(3), "request length");
  PrintField(buf, 4, 4, DRAWABLE, "drawable");
  PrintField(buf, 8, 2, CARD16, "width");
  PrintField(buf, 10, 2, CARD16, "height");
}

QueryBestSizeReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* QueryBestSize */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
  PrintField(buf, 8, 2, CARD16, "width");
  PrintField(buf, 10, 2, CARD16, "height");
}

QueryExtension(buf)
     unsigned char *buf;
{
  short   n;
  /* Request QueryExtension is opcode 98 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* QueryExtension */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(2 + (n + p) / 4), "request length");
  printfield(buf, 4, 2, DVALUE2(n), "length of name");
  n = IShort(&buf[4]);
  PrintString8(&buf[8], (long)n, "name");

}

QueryExtensionReply(buf)
     unsigned char *buf;
{
  extern unsigned char LookForPEXFlag;
  extern unsigned char PEXCode;
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* QueryExtension */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
  PrintField(buf, 8, 1, BOOL, "present");
  PrintField(buf, 9, 1, CARD8, "major-opcode");
  PrintField(buf, 10, 1, CARD8, "first-event");
  PrintField(buf, 11, 1, CARD8, "first-error");

  /* PEX content */
  if (LookForPEXFlag) PEXCode = (unsigned char)(buf[9]);
  LookForPEXFlag=0;

}

ListExtensions(buf)
     unsigned char *buf;
{
  /* Request ListExtensions is opcode 99 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ListExtensions */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(1), "request length");
}

ListExtensionsReply(buf)
     unsigned char *buf;
{
  short   n;

  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* ListExtensions */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 1, 1, CARD8, "number names");
  n = IByte(&buf[1]);
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4((n + p) / 4), "reply length");
  PrintListSTR(&buf[32], (long)n, "names");
}

ChangeKeyboardMapping(buf)
     unsigned char *buf;
{
  short   n;
  short   m;
  /* Request ChangeKeyboardMapping is opcode 100 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ChangeKeyboardMapping */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, DVALUE1(n), "keycode-count");
  n = IByte(&buf[1]);
  printfield(buf, 2, 2, DVALUE2(2 + nm), "request length");
  PrintField(buf, 4, 1, KEYCODE, "first-keycode");
  PrintField(buf, 5, 1, DVALUE1(m), "keysyms-per-keycode");
  m = IByte(&buf[5]);
  (void)PrintList(&buf[8], (long)(n * m), KEYSYM, "keysyms");
}

GetKeyboardMapping(buf)
     unsigned char *buf;
{
  /* Request GetKeyboardMapping is opcode 101 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GetKeyboardMapping */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 1, KEYCODE, "first-keycode");
  PrintField(buf, 5, 1, CARD8, "count");
}

GetKeyboardMappingReply(buf)
     unsigned char *buf;
{
  long    n;
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GetKeyboardMapping */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, DVALUE1(n), "keysyms-per-keycode");
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4(n*m), "reply length");
  n = ILong(&buf[4]);
  (void)PrintList(&buf[32], n, KEYSYM, "keysyms");
}

ChangeKeyboardControl(buf)
     unsigned char *buf;
{
  /* Request ChangeKeyboardControl is opcode 102 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ChangeKeyboardControl */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(2 + n), "request length");
  PrintField(buf, 4, 4, KEYBOARD_BITMASK, "value-mask");
  PrintValues(&buf[4], 4, KEYBOARD_BITMASK, &buf[8], "value-list");
}

GetKeyboardControl(buf)
     unsigned char *buf;
{
  /* Request GetKeyboardControl is opcode 103 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GetKeyboardControl */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(1), "request length");
}

GetKeyboardControlReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GetKeyboardControl */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, OFF_ON, "global-auto-repeat");
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(5), "reply length");
  PrintField(buf, 8, 4, CARD32, "led-mask");
  PrintField(buf, 12, 1, CARD8, "key-click-percent");
  PrintField(buf, 13, 1, CARD8, "bell-percent");
  PrintField(buf, 14, 2, CARD16, "bell-pitch");
  PrintField(buf, 16, 2, CARD16, "bell-duration");
  PrintBytes(&buf[20], 32L, "auto-repeats");
}

Bell(buf)
     unsigned char *buf;
{
  /* Request Bell is opcode 104 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* Bell */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, INT8, "percent");
  printfield(buf, 2, 2, CONST2(1), "request length");
}

ChangePointerControl(buf)
     unsigned char *buf;
{
  /* Request ChangePointerControl is opcode 105 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ChangePointerControl */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(3), "request length");
  PrintField(buf, 4, 2, INT16, "acceleration-numerator");
  PrintField(buf, 6, 2, INT16, "acceleration-denominator");
  PrintField(buf, 8, 2, INT16, "threshold");
  PrintField(buf, 10, 1, BOOL, "do-acceleration");
  PrintField(buf, 11, 1, BOOL, "do-threshold");
}

GetPointerControl(buf)
     unsigned char *buf;
{
  /* Request GetPointerControl is opcode 106 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GetPointerControl */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(1), "request length");
}

GetPointerControlReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GetPointerControl */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
  PrintField(buf, 8, 2, CARD16, "acceleration-numerator");
  PrintField(buf, 10, 2, CARD16, "acceleration-denominator");
  PrintField(buf, 12, 2, CARD16, "threshold");
}

SetScreenSaver(buf)
     unsigned char *buf;
{
  /* Request SetScreenSaver is opcode 107 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* SetScreenSaver */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(3), "request length");
  PrintField(buf, 4, 2, INT16, "timeout");
  PrintField(buf, 6, 2, INT16, "interval");
  PrintField(buf, 8, 1, NO_YES, "prefer-blanking");
  PrintField(buf, 9, 1, NO_YES, "allow-exposures");
}

GetScreenSaver(buf)
     unsigned char *buf;
{
  /* Request GetScreenSaver is opcode 108 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GetScreenSaver */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(1), "request length");
}

GetScreenSaverReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GetScreenSaver */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
  PrintField(buf, 8, 2, CARD16, "timeout");
  PrintField(buf, 10, 2, CARD16, "interval");
  PrintField(buf, 12, 1, NO_YES, "prefer-blanking");
  PrintField(buf, 13, 1, NO_YES, "allow-exposures");
}

ChangeHosts(buf)
     unsigned char *buf;
{
  short   n;
  /* Request ChangeHosts is opcode 109 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ChangeHosts */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, INS_DEL, "mode");
  printfield(buf, 2, 2, DVALUE2(2 + (n + p) / 4), "request length");
  PrintField(buf, 4, 1, HOSTFAMILY, "family");
  printfield(buf, 6, 2, CARD16, "length of address");
  n = IShort(&buf[6]);
  PrintBytes(&buf[8], (long)n, "address");
}

ListHosts(buf)
     unsigned char *buf;
{
  /* Request ListHosts is opcode 110 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ListHosts */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(1), "request length");
}

ListHostsReply(buf)
     unsigned char *buf;
{
  short   n;
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* ListHosts */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, DIS_EN, "mode");
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4(n / 4), "reply length");
  printfield(buf, 8, 2, CARD16, "number of hosts");
  n = IShort(&buf[8]);
  (void)PrintList(&buf[32], (long)n, HOST, "hosts");
}

SetAccessControl(buf)
     unsigned char *buf;
{
  /* Request SetAccessControl is opcode 111 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* SetAccessControl */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, DIS_EN, "mode");
  printfield(buf, 2, 2, CONST2(1), "request length");
}

SetCloseDownMode(buf)
     unsigned char *buf;
{
  /* Request SetCloseDownMode is opcode 112 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* SetCloseDownMode */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, CLOSEMODE, "mode");
  printfield(buf, 2, 2, CONST2(1), "request length");
}

KillClient(buf)
     unsigned char *buf;
{
  /* Request KillClient is opcode 113 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* KillClient */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(2), "request length");
  PrintField(buf, 4, 4, RESOURCEID, "resource");
}

RotateProperties(buf)
     unsigned char *buf;
{
  short   n;
  /* Request RotateProperties is opcode 114 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* RotateProperties */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, DVALUE2(3 + n), "request length");
  PrintField(buf, 4, 4, WINDOW, "window");
  printfield(buf, 8, 2, DVALUE2(n), "number of properties");
  n = IShort(&buf[8]);
  PrintField(buf, 10, 2, INT16, "delta");
  (void)PrintList(&buf[12], (long)n, ATOM, "properties");
}

ForceScreenSaver(buf)
     unsigned char *buf;
{
  /* Request ForceScreenSaver is opcode 115 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* ForceScreenSaver */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, SAVEMODE, "mode");
  printfield(buf, 2, 2, CONST2(1), "request length");
}

SetPointerMapping(buf)
     unsigned char *buf;
{
  short   n;
  /* Request SetPointerMapping is opcode 116 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* SetPointerMapping */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 1, 1, DVALUE1(n), "length of map");
  n = IByte(&buf[1]);
  printfield(buf, 2, 2, DVALUE2(1 + (n + p) / 4), "request length");
  PrintBytes(&buf[4], (long)n,"map");
}

SetPointerMappingReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* SetPointerMapping */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, RSTATUS, "status");
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
}

GetPointerMapping(buf)
     unsigned char *buf;
{
  /* Request GetPointerMapping is opcode 117 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GetPointerMapping */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(1), "request length");
}

GetPointerMappingReply(buf)
     unsigned char *buf;
{
  short   n;
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GetPointerMapping */ ;
  if (Verbose < 1)
    return;
  printfield(buf, 1, 1, DVALUE1(n), "length of map");
  n = IByte(&buf[1]);
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4((n + p) / 4), "reply length");
  PrintBytes(&buf[32], (long)n,"map");
}

SetModifierMapping(buf)
     unsigned char *buf;
{
  short   n;
  /* Request SetModifierMapping is opcode 118 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* SetModifierMapping */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  PrintField(buf, 1, 1, DVALUE1(n), "keycodes-per-modifier");
  n = IByte(&buf[1]);
  printfield(buf, 2, 2, DVALUE2(1 + 2*n), "request length");
  PrintBytes(&buf[4 + 0 * n], (long)n,"Shift keycodes");
  PrintBytes(&buf[4 + 1 * n], (long)n,"Lock keycodes");
  PrintBytes(&buf[4 + 2 * n], (long)n,"Control keycodes");
  PrintBytes(&buf[4 + 3 * n], (long)n,"Mod1 keycodes");
  PrintBytes(&buf[4 + 4 * n], (long)n,"Mod2 keycodes");
  PrintBytes(&buf[4 + 5 * n], (long)n,"Mod3 keycodes");
  PrintBytes(&buf[4 + 6 * n], (long)n,"Mod4 keycodes");
  PrintBytes(&buf[4 + 7 * n], (long)n,"Mod5 keycodes");
}

SetModifierMappingReply(buf)
     unsigned char *buf;
{
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* SetModifierMapping */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, RSTATUS, "status");
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, CONST4(0), "reply length");
}

GetModifierMapping(buf)
     unsigned char *buf;
{
  /* Request GetModifierMapping is opcode 119 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* GetModifierMapping */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(1), "request length");
}

GetModifierMappingReply(buf)
     unsigned char *buf;
{
  short   n;
  PrintField(RBf, 0, 1, REPLY, REPLYHEADER) /* GetModifierMapping */ ;
  if (Verbose < 1)
    return;
  PrintField(buf, 1, 1, DVALUE1(n), "keycodes-per-modifier");
  n = IByte(&buf[1]);
  printfield(buf, 2, 2, CARD16, "sequence number");
  printfield(buf, 4, 4, DVALUE4(2*n), "reply length");
  (void)PrintList(&buf[32], (long)n, KEYCODE, "keycodes");
}

NoOperation(buf)
     unsigned char *buf;
{
  /* Request NoOperation is opcode 127 */
  PrintField(buf, 0, 1, REQUEST, REQUESTHEADER) /* NoOperation */ ;
  if (Verbose < 1)
    return;
  if (Verbose > 1)
    PrintField(SBf, 0, 4, CARD32, "sequence number");

  printfield(buf, 2, 2, CONST2(1), "request length");
}
