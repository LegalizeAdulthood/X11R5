/*
 * Copyright IBM Corporation 1987,1988,1989
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

/* THIS VERSION FOR PHOTRON STYLE DIGITIZER ATTACHED VIA /DEV/TTY */
/* In most of our implementations, there is a device driver for the
   notepad device. This device driver handles the tablet port,
   performs inking, converts tablet coordinates to scaled display
   coordinates, and maintains the alignment and inking status
   information. In the interest of portability, we provide this
   module, and NodevUtil.c which permit the attachment of the
   digitizer via /dev/tty<n>. The disadvantage of this implementation
   is that inking is no longer done in realtime, and the X server
   will experience additional overhead due to system calls to
   read from /dev/tty. The performance should be good enough
   to allow experimentation with the extension. */
#include <sys/time.h>
#include <fcntl.h>
#include <sys/mode.h>
#include "plicoord.h"
#define NEED_EVENTS
#define NEED_REPLIES
#include "xpliDevStr.h"
#include "scrnintstr.h"
#include "PLIproto.h"

#define PLIBUFSIZE 512
#define PLIDEVNAME "/dev/tty0"

static int xpliNumFormats = 1;
static PixmapFormatRec xpliBSPixmapFormatRec[1] =
{{3 /* pli has 3 bits/pixel */, 8 /* ppc wants 8 bit/pixel in pixmaps */, 
    32 /* scanline occupies integral number of words */ }};

static ScreenPtr lclScreenP;
static int plidev;
static t_xpliDevStrP xpliDevStrP; 
static struct timestruc_t todcur, todprev;
static int usetimer;
static int badframe=0;

/* This function is called everytime the dispatch loop is run for
   input devices. It returns when there is no input available */

static int
BSTtySelect(devP, startP, endP)
     DeviceIntPtr devP;
     unsigned char *startP, endP; /* not used in this implementation */
{
  t_xpliDevStrP xpliDevStrP;
  int i;   long secs; short rc, x, y ;
  static unsigned char rchar, fchar[5], pendown, penout, flags;
  static int plicnt = 0, framecnt = 0;
  static char plibuf[PLIBUFSIZE], *plicurP;

  if (usetimer) {
    gettimer(TIMEOFDAY, &todcur);
    if ((todcur.tv_sec - todprev.tv_sec) == 0)
      if ((todcur.tv_nsec - todprev.tv_nsec) < (NS_PER_SEC / 10)) return 0; }
  for (;;) {
    if (plicnt == 0) {
      if ((plicnt = read(plidev, &plibuf[0], PLIBUFSIZE)) <= 0) {
	return 0; } 
      plicurP = &plibuf[0]; }
    fchar[framecnt] = rchar = *plicurP++; plicnt--;
    if ((rchar & 0x40) && framecnt) {
      badframe++; framecnt = 0; }
    switch (framecnt) {
    case 0:
      if (rchar & 0x04) flags = 0x01; else flags = 0;
      if (rchar & 0x01) flags |= 0x10;
      framecnt = 1;
      break;
    case 1:
      x = rchar & 0x3F;
      framecnt = 2;
      break;
    case 2:
      x |= (rchar & 0x3F) << 6;
      framecnt = 3;
      break;
    case 3:
      y = rchar & 0x3F;
      framecnt = 4;
      break;
    case 4:
      y |= (rchar & 0x3F) << 6;
      framecnt = 0;
      (xpliDevStrP->processXY)(xpliDevStrP, flags, x, y);
      break;
    } } 
  return 0;
} 

/* this routine is called by the DIX for DEVICE_INIT, DEVICE_ON,
   DEVICE_OFF, and DEVICE_CLOSE */

static int
BSTtyDevProc(devP, cmd)
     DevicePtr devP; int cmd;
{ struct timestruc_t todmax, todres; /*reqd by call but not used */
  struct termios ttyld;
  switch (cmd) {
  case DEVICE_INIT:
    break;
  case DEVICE_ON:
    if (!(plidev = open(PLIDEVNAME, O_RDWR, 0))) return BadImplementation;
    ttyld.c_iflag = 0;
    ttyld.c_oflag = 0;
    ttyld.c_cflag = CLOCAL | CREAD | CS8 | B9600 ;
    ttyld.c_clflag = 0;
    ttyld.c_cc[VMIN] = '\0';
    ttyld.c_cc[VTIME] = '\0';
    tcsetattr(plidev, TCSANOW, &ttyld);
    /* NOTE we want Posix line mode, no canonical
       mode processing, TIME == 0, MIN == 0 (i.e. return
       immediately even if no characters are read). This is
       not optimal, since the digitizer sends frames in
       bursts and we don't want to make a system call for
       each character. On the other hand, we don't want to have
       the X server block in the read. Hence, it is a good idea to throttle
       the polling of the tty. This implementation of a throttle
       is not very useful unless the time functions are significantly
       faster than the kernel call to read the device. */
    restimer(TIMEOFDAY, &todres, &todmax);
    if (todres.tv_nsec != 0 && todres.tv_nsec < (NS_PER_SEC / 10)) {
      usetimer = 1;
      gettimer(TIMEOFDAY, &todprev); }
    else usetimer = 0;
    AddEnabledDevice(plidev); /* set flag for select */
    AddInputCheck(0, 0, devP, BSTtySelect); /* always check for input */
    break;
  case DEVICE_OFF:
    close(plidev);
    plidev = -1;
    break;
  case DEVICE_CLOSE:
    break;
  }
  return Success;
}

t_xpliDevStrP
InitBSTtyDev(argc, argv)
  int argc; char *argv[];
{ 
  register int i, j;
  xpliDevStrP = (t_xpliDevStrP) xalloc(sizeof(t_xpliDevStr));
  xpliDevStrP->devP = AddInputDevice(BSTtyDevProc, TRUE);
  xpliDevStrP->devP->devicePrivate = (pointer) xpliDevStrP;
  xpliDevStrP->config.xResol = (xpliDevStrP->screenP->width * 10 *
    (1<<COORDSCALE)) / xpliDevStrP->screenP->mmWidth;
  xpliDevStrP->config.yResol = (xpliDevStrP->screenP->height * 10 *
    (1<<COORDSCALE)) / xpliDevStrP->screenP->mmHeight;
  xpliDevStrP->config.samplesSec = 72;
  xpliDevStrP->config.digType = DIG_TYPE_PROX;
  /* setup the function pointers */
  if (InitVGA(xpliDevStrP, argc, argv)) {
    xfree(xpliDevStrP);
    return NULL; } 
  if (InitVGAInk(xpliDevStrP))  {
    xfree(xpliDevStrP);
    return NULL; } 
  if (InitNodev(xpliDevStrP))  {
    xfree(xpliDevStrP);
    return NULL; } 
  return xpliDevStrP;
}
