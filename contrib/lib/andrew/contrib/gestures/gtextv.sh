#! /bin/sh
# This is a shell archive.  Remove anything before this line, then unpack
# it by saving it into a file and typing "sh file".  To overwrite existing
# files, type "sh file -c".  You can also feed this as standard input via
# unshar, or by typing "sh <file", e.g..  If this archive is complete, you
# will see the following message at the end:
#		"End of shell archive."
# Contents:  Imakefile README README.ez gtextv.c gtextv.ch mygest.cl
#   test.ez testapp.c testapp.ch
# Wrapped by hassan@informatics on Tue Jul 30 09:49:43 1991
PATH=/bin:/usr/bin:/usr/ucb ; export PATH
if test -f 'Imakefile' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'Imakefile'\"
else
echo shar: Extracting \"'Imakefile'\" \(680 characters\)
sed "s/^X//" >'Imakefile' <<'END_OF_FILE'
X#******************************************************************************
X#*
X#* gtextv - Gesture Text View
X#* Medical Informatics 
X#* Washington University, St. Louis
X#* July 29, 1991
X#*
X#* Scott Hassan
X#* Steve Cousins
X#* Mark Frisse
X#*
X#******************************************************************************
X
XGESTURE = /mig/hassan/gesture
XGESTURE_RECOG_LIB = $(GESTURE)/recognition/librecog.a
X
XLOCALINCLUDES = -I$(GESTURE)/recognition/
X
XDOBJS = testapp.do gtextv.do
XIHFILES = testapp.ih gtextv.ih
X
XNormalObjectRule()
XNormalATKRule()
X
XDynamicMultiObject(gtextv.do, gtextv.o, $(GESTURE_RECOG_LIB), -lm)
XDynamicMultiObject(testapp.do, testapp.o, ,)
X
XDependTarget()
X
X
END_OF_FILE
if test 680 -ne `wc -c <'Imakefile'`; then
    echo shar: \"'Imakefile'\" unpacked with wrong size!
fi
# end of 'Imakefile'
fi
if test -f 'README' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'README'\"
else
echo shar: Extracting \"'README'\" \(3268 characters\)
sed "s/^X//" >'README' <<'END_OF_FILE'
X
X
X                                 gtextv
X                      Gesture Based TextView Object
X
X
X                             Scott W. Hassan
X                           Medical Informatics
X                           St. Louis, Missouri
X                      hassan@informatics.wustl.edu
X
X
XAfter seeing the Gesture Package developed by Dean Rubine*, I decided
Xthat Andrew needed an interface for gestures.  I put together this
Xsimple text object that uses a few hand-made gestures to cut, copy,
Xpaste, scroll, and set the dot.
X
X*      Dean Rubine
X        CMU Information Technology Center
X        4910 Forbes Avenue
X        Pittsburgh, PA 15213-3890
X        Dean.Rubine@cs.cmu.edu
X
X
X1.0 Compiling:
X
X1. ftp the gestures package from emsworth.andrew.cmu.edu (128.2.30.62). 
Xfile: /gestures/gestures.tar.Z
X
X2. uncompress, untar, and make it.
X
X3. In the gtextv's Imakefile, change the GESTURE variable to reflect the
Xinstallation directory.
X
X4. genmake and make it.
X
X5. run the test program.
X
X2.0 The Gestures:
X
XFlick Up:
X	-- Scrolls one screen view full of text up.
X  [An Andrew ToolKit view (an animated drawing) was included here, but
X                        could not be displayed.]
XFlick Down:
X	-- Scrolls one screen view full of text down.
X  [An Andrew ToolKit view (an animated drawing) was included here, but
X                        could not be displayed.]
X
XFlick Top:
X	-- Scrolls to the top of the document.
X  [An Andrew ToolKit view (an animated drawing) was included here, but
X                        could not be displayed.]
X
XFlick Bottom:
X	-- Scrolls to the bottom of the document.
X  [An Andrew ToolKit view (an animated drawing) was included here, but
X                        could not be displayed.]
XRegion Start:
X	-- Position of the start of a Region.
X  [An Andrew ToolKit view (an animated drawing) was included here, but
X                        could not be displayed.]
XRegion End:
X	-- Position of the end of a Region and then highlight.
X
X  [An Andrew ToolKit view (an animated drawing) was included here, but
X                        could not be displayed.]
XSelect Region:
X	-- Selects a region of text.  (simply, drag the mouse over text.  The
Xstarting and ending points determine the region. )
X
X  [An Andrew ToolKit view (an animated drawing) was included here, but
X                        could not be displayed.]
X
X
X
XCut Region:
X	-- Delete the selected Region from the text and place it in the cut
Xbuffer.  Using the start of the gesture as the hot-spot.  Here are two
Xcommon gestures for delete.
X
X  [An Andrew ToolKit view (an animated drawing) was included here, but
X could not be displayed.]	[An Andrew ToolKit view (an animated drawing)
X             was included here, but could not be displayed.]
X
X
X
XPaste Region:
X	-- Yanks a Region from the cut buffer to the start of the gesture.
X	Note: Try to make it rounder than the figure.  Basically, an upper-case P.
X  [An Andrew ToolKit view (an animated drawing) was included here, but
X                        could not be displayed.]
X3.0 Comments:
X
X	I think this is an interesting type of interface, fast and efficient. 
XIf you have any additions to gtextv, please send them to me.  I created
Xthis template textview object so that others could build on it, taking a
Xlot of load off me.  Well, have fun.
END_OF_FILE
if test 3268 -ne `wc -c <'README'`; then
    echo shar: \"'README'\" unpacked with wrong size!
fi
# end of 'README'
fi
if test -f 'README.ez' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'README.ez'\"
else
echo shar: Extracting \"'README.ez'\" \(5162 characters\)
sed "s/^X//" >'README.ez' <<'END_OF_FILE'
X\begindata{text,876632}
X\textdsversion{12}
X\template{default}
X
X
X
X\majorheading{\italic{\majorheading{gtextv}}}
X
X\majorheading{Gesture Based TextView Object}
X
X
X
X\center{Scott W. Hassan
X
XMedical Informatics
X
XSt. Louis, Missouri
X
X\typewriter{\smaller{hassan@informatics.wustl.edu}}}
X
X
X
XAfter seeing the Gesture Package developed by Dean Rubine*, I decided that 
XAndrew needed an interface for gestures.  I put together this simple text 
Xobject that uses a few hand-made gestures to cut, copy, paste, scroll, and set 
Xthe dot.
X
X\smaller{\smaller{
X*      Dean Rubine
X
X        CMU Information Technology Center
X
X        4910 Forbes Avenue
X
X        Pittsburgh, PA 15213-3890
X
X        Dean.Rubine@cs.cmu.edu
X
X}}
X
X\section{1.0 Compiling:}
X
X
X1. ftp the gestures package from \typewriter{emsworth.andrew.cmu.edu} 
X(128.2.30.62). 
X
Xfile: \typewriter{/gestures/gestures.tar.Z}
X
X
X2. \typewriter{uncompress}, un\typewriter{tar}, and \typewriter{make} it.
X
X
X3. In the gtextv's \typewriter{Imakefile}, change the \typewriter{GESTURE} 
Xvariable to reflect the installation directory.
X
X
X4. \typewriter{genmake} and \typewriter{make} it.
X
X
X5. run the \typewriter{test} program.
X
X
X\section{2.0 The Gestures:}
X
X
X\subsection{Flick Up:
X
X}	-- Scrolls one screen view full of text up.
X
X\center{\
X\begindata{fad,913512}
X$C 10
X$T 30
X$L andy12
X$P 0,0,20000,256
X$F
X$V 38,68 37,55
X$F
X$V 39,69 39,42
X$F
X$V 38,69 38,12
X$$
X\enddata{fad,913512}
X\view{fadview,913512,0,74,80}
X}
X\subsection{Flick Down:
X
X}	-- Scrolls one screen view full of text down.
X
X\center{\
X\begindata{fad,917632}
X$C 10
X$T 30
X$L andy12
X$P 0,0,20000,256
X$F
X$V 36,7 36,20
X$F
X$V 34,11 34,33
X$F
X$V 34,14 34,68
X$$
X\enddata{fad,917632}
X\view{fadview,917632,1,74,80}
X
X}
X\subsection{Flick Top:
X
X}	-- Scrolls to the top of the document.
X
X\center{\
X\begindata{fad,920608}
X$C 10
X$T 30
X$L andy12
X$P 0,0,20000,256
X$F
X$V 37,70 36,56
X$F
X$V 37,69 37,44
X$F
X$V 34,68 34,14
X$F
X$V 35,67 35,9
X$V 35,9 41,15
X$F
X$V 35,65 35,10
X$V 35,10 50,28
X$$
X\enddata{fad,920608}
X\view{fadview,920608,2,74,80}
X
X}
X\subsection{Flick Bottom:}
X
X	-- Scrolls to the bottom of the document.
X
X\center{\
X\begindata{fad,923904}
X$C 10
X$T 30
X$L andy12
X$P 0,0,20000,256
X$F
X$V 35,8 35,21
X$F
X$V 38,10 39,40
X$V 39,40 39,40
X$F
X$V 34,14 34,68
X$F
X$V 35,9 35,67
X$V 35,67 41,58
X$F
X$V 35,10 35,69
X$V 35,69 49,47
X$$
X\enddata{fad,923904}
X\view{fadview,923904,3,74,80}
X}
X\subsection{Region Start:}
X
X	-- Position of the start of a Region.
X
X\center{\
X\begindata{fad,927232}
X$C 10
X$T 30
X$L andy12
X$P 0,0,20000,256
X$F
X$V 50,17 38,17
X$F
X$V 52,19 21,19
X$V 21,19 21,28
X$F
X$V 51,20 22,20
X$V 22,20 22,63
X$V 22,63 30,64
X$F
X$V 49,18 20,18
X$V 20,18 24,62
X$V 24,62 51,62
X$$
X\enddata{fad,927232}
X\view{fadview,927232,4,74,80}
X}
X\subsection{Region End:}
X
X	-- Position of the end of a Region and then highlight.
X
X
X\center{\
X\begindata{fad,930608}
X$C 10
X$T 30
X$L andy12
X$P 0,0,20000,256
X$F
X$V 21,19 30,19
X$F
X$V 20,20 47,20
X$V 47,20 47,30
X$F
X$V 22,21 51,20
X$V 51,20 51,63
X$V 51,63 41,63
X$F
X$V 24,16 53,16
X$V 53,16 54,64
X$V 54,64 28,62
X$$
X\enddata{fad,930608}
X\view{fadview,930608,5,74,80}
X}
X\subsection{Select Region:}
X
X	-- Selects a region of text.  (simply, drag the mouse over text.  The 
Xstarting and ending points determine the region. )
X
X
X\center{\
X\begindata{fad,933968}
X$C 10
X$T 30
X$L andy12
X$P 0,0,20000,256
X$F
X$V 4,17 19,31
X$F
X$V 9,15 60,63
X$$
X\enddata{fad,933968}
X\view{fadview,933968,6,74,80}
X}
X
X
X
X\subsection{Cut Region:}
X
X	-- Delete the selected Region from the text and place it in the cut buffer. 
X Using the start of the gesture as the hot-spot.  Here are two common gestures 
Xfor delete.
X
X
X\center{\
X\begindata{fad,936840}
X$C 10
X$T 30
X$L andy12
X$P 0,0,20000,256
X$F
X$V 15,18 22,28
X$F
X$V 20,20 47,60
X$V 47,60 47,46
X$F
X$V 17,19 51,63
X$V 51,63 49,15
X$V 49,15 38,29
X$F
X$V 24,16 54,64
X$V 54,64 53,16
X$V 53,16 21,63
X$$
X\enddata{fad,936840}
X\view{fadview,936840,7,74,80}}	
X\center{\
X\begindata{fad,940200}
X$C 10
X$T 30
X$L andy12
X$P 0,0,20000,256
X$F
X$V 4,55 15,51
X$F
X$V 6,56 45,36
X$V 45,36 47,30
X$F
X$V 8,56 45,40
X$V 45,40 49,25
X$F
X$V 11,63 45,40
X$V 45,40 46,27
X$V 46,27 35,19
X$V 35,19 27,32
X$F
X$V 11,63 45,42
X$V 45,42 47,30
X$V 47,30 41,20
X$V 36,18 29,25
X$V 29,25 32,38
X$V 32,38 45,42
X$V 45,42 52,39
X$F
X$V 13,60 45,40
X$V 45,40 45,29
X$V 45,29 32,23
X$V 32,23 29,32
X$V 29,32 39,38
X$V 39,38 50,42
X$V 50,42 69,29
X$$
X\enddata{fad,940200}
X\view{fadview,940200,8,74,80}
X
X}
X
X
X\subsection{Paste Region:}
X
X	-- Yanks a Region from the cut buffer to the start of the gesture.
X
X	Note: Try to make it rounder than the figure.  Basically, an upper-case P.
X
X\center{\
X\begindata{fad,944440}
X$C 10
X$T 30
X$L andy12
X$P 0,0,20000,256
X$F
X$V 18,65 20,47
X$F
X$V 21,63 26,17
X$V 26,17 33,18
X$F
X$V 20,63 27,14
X$V 27,14 55,16
X$V 55,16 51,20
X$F
X$V 18,64 29,11
X$V 29,11 60,12
X$V 60,12 51,38
X$V 51,38 43,39
X$F
X$V 20,63 28,12
X$V 28,12 60,12
X$V 60,12 49,39
X$V 49,39 22,39
X$$
X\enddata{fad,944440}
X\view{fadview,944440,9,74,80}
X}
X\section{3.0 Comments:
X
X}
X	I think this is an interesting type of interface, fast and efficient.  If you 
Xhave any additions to gtextv, please send them to me.  I created this template 
Xtextview object so that others could build on it, taking a lot of load off me. 
X Well, have fun.
X
X\enddata{text,876632}
END_OF_FILE
if test 5162 -ne `wc -c <'README.ez'`; then
    echo shar: \"'README.ez'\" unpacked with wrong size!
fi
# end of 'README.ez'
fi
if test -f 'gtextv.c' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'gtextv.c'\"
else
echo shar: Extracting \"'gtextv.c'\" \(9374 characters\)
sed "s/^X//" >'gtextv.c' <<'END_OF_FILE'
X/******************************************************************************
X *
X * gtextv - Gesture Text View
X * Medical Informatics 
X * Washington University, St. Louis
X * July 29, 1991
X *
X * Scott Hassan
X * Steve Cousins
X * Mark Frisse
X *
X *****************************************************************************/
X
X#define NOPOINT -1L
X
X#define MODULE "gtextv"
X/*#define DEBUGFLAG */
X
X/*****************************************************************************
X * 
X * gtextv.c -- The Gesture Text View Module
X *
X *****************************************************************************/
X
X#include <stdio.h>
X#include <sys/types.h>
X#include <sys/time.h>
X
X#include <class.h>
X#include <text.ih>
X#include <textv.ih>
X#include <dataobj.ih>
X#include <mark.ih>
X#include <environ.ih>
X
X/*****************************************************************************/
X
X#include <gtextv.eh>
X
X#include <math.h>
X#include "util.h"
X#include "bool.h"
X#include "bitvector.h"
X#include "matrix.h"
X#include "sc.h"
X#include "fv.h"
X#include "zdebug.h"
X
X/*****************************************************************************/
X
Xlong clock();
XsClassifier fullclassifier;
XsClassifier doneclassifier;
Xstatic FV fv;
Xchar _zdebug_flag[128];
X
Xchar *ClassifyFv();
Xchar *ClassifyVector();
X
X#define GESTURE_FILE "mygest.cl"
X
X/******************************************************************************
X * InitializeClass(class)
X * 
X * Setup the Gesture Recognizer and Read in the Gesture file.
X *
X *****************************************************************************/
X
Xboolean gtextv__InitializeClass(class) 
X     struct classheader *class;
X{
X  FILE *fp;
X  char *temp;
X  char *filename;
X
X  doneclassifier = NULL;
X
X  temp = (char *) environ_GetProfile("GestureFile");
X
X  if(temp == NULL) 
X    filename = (char *) strdup(GESTURE_FILE);
X  else
X    filename = (char *) strdup(temp);
X
X  fp = fopen(filename,"r");
X
X  if(fp == NULL) {
X    printf("gtextv: Cannot find file: %s\n", filename);
X    return FALSE;
X  }
X
X  fullclassifier = sRead(fp);
X  fclose(fp);
X
X  fv = FvAlloc(0);
X
X  free(filename);
X
X  return TRUE;
X}     
X
X/******************************************************************************
X * InitializeObject(class, self)
X * 
X * Set the object's initial data to null values.
X *
X *****************************************************************************/
X
Xboolean gtextv__InitializeObject(class, self)
X     struct classheader *class;
X     struct gtextv *self;
X{
X  self->parstart = NOPOINT;
X  self->parend = NOPOINT;
X
X  self->limit = 500;
X  self->xp = (int *)malloc(self->limit * sizeof(int));
X  self->yp = (int *)malloc(self->limit * sizeof(int));
X  self->index = 0;
X
X  return TRUE;
X}
X/******************************************************************************
X * FinalizeObject(class, self)
X * 
X * Free up some of the used memory.
X *
X *****************************************************************************/
X
Xvoid gtextv__FinalizeObject(class, self)
X     struct classheader *class;
X     struct gtextv *self;
X{
X  free(self->xp);
X  free(self->yp);
X}
X
X/******************************************************************************
X * Hit(self, action, x, y, numclicks)
X * 
X * Track the mouse cursor and process the path.
X *
X *****************************************************************************/
X
Xstruct view *gtextv__Hit(self, action, x, y, numclicks)
X     struct gtextv *self;
X     enum view_MouseAction action;
X     long x,y, numclicks;
X{
X  int i;
X  struct view *temp;
X  int transfer;
X  long ts_timer;
X  char *class;
X  struct rectangle VisualRect;
X  struct view *vptr;
X  long newPos;
X  int my, mx;
X
X  newPos = gtextv_Locate(self,x,y, &vptr);
X
X  temp = (struct view *)self;
X  ts_timer = clock() / 1000;
X
X  gtextv_GetVisualBounds(self, &VisualRect);
X  
X  gtextv_SetTransferMode(self, graphic_XOR);
X
X  if(action == view_LeftMovement) { /* Track the mouse movements */
X    self->xp[self->index] = x;
X    self->yp[self->index] = y;
X    gtextv_MoveTo(self, self->xp[self->index - 1], self->yp[self->index - 1]);
X    gtextv_DrawLineTo(self, x, y);
X    self->index++;
X    FvAddPoint(fv, x, VisualRect.height - y, ts_timer, 0);
X
X  } else if(action == view_LeftDown) { /* signals the start of an action */
X    self->xp[self->index] = x;
X    self->yp[self->index] = y;
X    gtextv_MoveTo(self, x, y);
X    gtextv_DrawLineTo(self, x, y);
X    self->index++;
X    FvInit(fv); /* initialize the gesture handler */
X    FvAddPoint(fv, x, VisualRect.height - y, ts_timer, 0);
X
X  } else if(action == view_LeftUp) {  /* signals the end of an action */
X    i=0;
X    gtextv_MoveTo(self, self->xp[i], self->yp[i]);
X    for(i=0; i<self->index; i++) {
X      gtextv_DrawLineTo(self, self->xp[i], self->yp[i]);
X    }
X    FvAddPoint(fv, x, VisualRect.height - y, ts_timer, 0);
X    class = ClassifyFv(); /* Classify the sampled points into a gesture */
X/*    printf("clasifyfv: %s\n", class);*/
X/************************************************************ PARSTART Gesture handler *****/
X    if(!strcmp(class, "parstart")) {
X      if(CalcMiddle(self->xp, self->yp, self->index, &mx, &my)==TRUE) {
X	self->parstart = gtextv_Locate(self,mx,my, &vptr);
X	if(self->parend != NOPOINT) {
X	  if(self->parstart < self->parend) {
X	    gtextv_SetDotPosition(self, self->parstart);
X	    gtextv_SetDotLength(self, self->parend - self->parstart);
X	    textview_CopyRegionCmd(self);
X	  } else {
X	    gtextv_SetDotPosition(self, self->parstart);
X	    gtextv_SetDotLength(self, 0);
X	  }
X	}
X      }
X/************************************************************ PAREND Gesture handler *****/
X    } else if(!strcmp(class, "parend")) {
X      if(CalcMiddle(self->xp, self->yp, self->index, &mx, &my)==TRUE) {
X	self->parend = gtextv_Locate(self,mx,my, &vptr);
X	if(self->parstart != NOPOINT) {
X	  if(self->parstart > self->parend) {
X	    self->parstart = NOPOINT;
X	    self->parend = NOPOINT;
X	  } else {
X	    gtextv_SetDotPosition(self, self->parstart);
X	    gtextv_SetDotLength(self, self->parend - self->parstart);
X	    textview_CopyRegionCmd(self);
X	  }
X	}
X      }
X/************************************************************ .DOT Gesture handler *****/
X    } else if(!strcmp(class, ".dot")) {
X      self->parstart = NOPOINT;
X      self->parend = NOPOINT;
X      
X      gtextv_SetDotLength(self, 0);
X      gtextv_SetDotPosition(self, newPos);
X/************************************************************ TOP Gesture handler *****/
X    } else if(!strcmp(class, "top")) {
X      textview_BeginningOfTextCmd(self);
X/************************************************************ BOTTOM Gesture handler *****/
X    } else if(!strcmp(class, "bottom")) {
X      textview_EndOfTextCmd(self);
X/************************************************************ DOWN Gesture handler *****/
X    } else if(!strcmp(class, "down")) {
X      textview_NextScreenCmd(self);
X/************************************************************ UP Gesture handler *****/
X    } else if(!strcmp(class, "up")) {
X      textview_PrevScreenCmd(self);
X/************************************************************ PASTE Gesture handler *****/
X    } else if(!strcmp(class, "paste")) {
X      newPos = gtextv_Locate(self,self->xp[0],self->yp[0], &vptr);
X      gtextv_SetDotPosition(self, newPos);
X      gtextv_SetDotLength(self, 0);
X      textview_YankCmd(self);
X      self->parstart = newPos;
X      self->parend = newPos + gtextv_GetDotLength(self);
X/************************************************************ SELECT Gesture handler *****/
X    } else if(!strcmp(class, "select")) {
X      self->parstart = gtextv_Locate(self, self->xp[0], self->yp[0], &vptr);
X      self->parend = gtextv_Locate(self, self->xp[self->index - 1], 
X				     self->yp[self->index - 1], &vptr);
X      gtextv_SetDotPosition(self, self->parstart);
X      gtextv_SetDotLength(self, self->parend - self->parstart);
X      textview_CopyRegionCmd(self);
X/************************************************************ DELETE Gesture handler *****/
X    } else if(!strcmp(class, "delete")) {
X      newPos = gtextv_Locate(self,self->xp[0],self->yp[0], &vptr);
X      if(gtextv_GetDotLength(self) >= 0) {
X	if(self->parstart != NOPOINT && self->parend != NOPOINT) {
X	  if(newPos >= self->parstart && newPos <= self->parend) {
X	    textview_ZapRegionCmd(self);
X	    self->parstart = NOPOINT;
X	    self->parend = NOPOINT;
X	  }
X	}
X      }
X    }
X    self->index = 0;
X  }
X
X  gtextv_SetTransferMode(self, graphic_COPY);
X  return((struct view *)temp);
X}
X
X/**********************************************************************/
X
XCalcMiddle(xp, yp, length, middlex, middley)
X     int *xp;
X     int *yp;
X     int length;
X     int *middlex;
X     int *middley;
X{
X  int i;
X
X
X  if(length>0) {
X    *middlex = *middley = 0;
X
X    for(i=0; i<length; i++) {
X      *middlex += xp[i];
X      *middley += yp[i];
X    }
X    *middlex = *middlex / length;
X    *middley = *middley / length;
X    return TRUE;
X  } else {
X    *middlex = NOPOINT;
X    *middley = NOPOINT;
X    return FALSE;
X  }
X}
X
X/**********************************************************************/
X
Xchar *ClassifyFv()
X{
X  Vector v;
X
X  v = FvCalc(fv);
X  return ClassifyVector(v);
X}
X
X/**********************************************************************/
X
Xchar *ClassifyVector(y)
X     Vector y;
X{
X  register sClassDope cd;
X  
X  cd = sClassify(fullclassifier, y);
X  if(cd == NULL) {
X    return NULL;
X  } else {
X    return(cd->name);
X  }
X}
X
END_OF_FILE
if test 9374 -ne `wc -c <'gtextv.c'`; then
    echo shar: \"'gtextv.c'\" unpacked with wrong size!
fi
# end of 'gtextv.c'
fi
if test -f 'gtextv.ch' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'gtextv.ch'\"
else
echo shar: Extracting \"'gtextv.ch'\" \(1162 characters\)
sed "s/^X//" >'gtextv.ch' <<'END_OF_FILE'
X/******************************************************************************
X *
X * gtextv - Gesture Text View
X * Medical Informatics 
X * Washington University, St. Louis
X * July 29, 1991
X *
X * Scott Hassan
X * Steve Cousins
X * Mark Frisse
X *
X *****************************************************************************/
X
X/*****************************************************************************
X * 
X * gtextv.c -- The Gesture Text View Module
X *
X *****************************************************************************/
X
Xclass gtextv : textview [textv] {
X
X overrides:
X  Hit(enum view_MouseAction action, long x, long y, long numberOfClicks)
X    returns struct view *;
X methods:
X classprocedures:
X  InitializeClass() returns boolean;
X  InitializeObject(struct gtextv *self) returns boolean;
X  FinalizeObject(struct gtextv *self);
X
X data:
X  int *xp, *yp;          /* Mouse Buffering Arrays */
X  int index;             /* The current length of the buffers */
X  int limit;             /* The current limit of the buffers */
X  long parstart, parend; /* The starting and ending positions for */
X                         /* the selected region. */
X}; 
X
X
END_OF_FILE
if test 1162 -ne `wc -c <'gtextv.ch'`; then
    echo shar: \"'gtextv.ch'\" unpacked with wrong size!
fi
# end of 'gtextv.ch'
fi
if test -f 'mygest.cl' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'mygest.cl'\"
else
echo shar: Extracting \"'mygest.cl'\" \(3868 characters\)
sed "s/^X//" >'mygest.cl' <<'END_OF_FILE'
X11 classes
Xparstart
Xparend
Xdelete
Xup
Xdown
Xtop
Xbottom
X.dot
Xpaste
Xbold
Xselect
X V 12    -0.981138 -0.121676 48.9693 1.07576 42.3684 0.018445 -0.99203 77.6531 -2.9226 4.40199 3.64243 11.825
X V 12    -105.094 128.061 0.182993 22.6255 0.250737 34.6526 -116.839 -0.321137 -4.12996 15.7049 -3.2729 2.98536
X V 12    0.994058 -0.00848377 41.1507 1.00992 35.2037 0.192817 -0.973498 64.4463 3.06158 4.35587 3.89646 12.2063
X V 12    194.943 119.222 -0.603392 130.233 0.697904 -22.5942 -108.942 -0.0410281 3.56011 14.7454 -2.58108 2.44471
X V 12    0.713589 -0.255553 86.401 0.680255 78.9294 0.420288 -0.289828 151.87 -5.91561 6.96613 9.88572 12.2033
X V 12    126.998 23.7531 -1.08767 78.1549 0.963084 -9.91401 -28.2516 0.208332 -4.0517 12.0945 0.689303 2.12267
X V 12    -0.0130905 0.997763 59.6236 1.51667 59.5973 -0.0190699 0.998846 59.7733 -0.0439211 0.306944 0.061788 2.80667
X V 12    74.2965 60.2332 0.255024 147.042 -0.523587 -7.48313 23.8527 0.0711995 1.37098 -5.13019 1.34977 3.18141
X V 12    -0.0389925 -0.996814 72.759 1.51396 72.7433 -0.00217843 -0.997924 72.9077 -0.189811 0.217213 0.0363177 2.5
X V 12    71.5117 -7.72232 -0.126852 87.8858 0.411258 11.0047 -8.36779 -0.117828 1.52731 2.63579 -0.656708 0.529948
X V 12    0.0845478 0.991049 79.1087 1.35506 60.4788 0.275404 0.953685 101.873 2.02088 3.6115 7.02645 6.72
X V 12    72.7014 61.4323 1.07501 145.349 -1.23295 4.98398 20.4778 -0.0579532 2.66613 0.735913 1.53818 2.9496
X V 12    -0.067531 -0.995859 85.3253 1.36837 65.0013 -0.255712 -0.963777 107.79 2.24995 3.4439 7.18427 7.07333
X V 12    72.1952 -4.03788 0.990401 81.5927 -0.509314 2.79739 -3.93041 -0.312045 2.19516 7.29466 -0.209265 0.419649
X V 12    0 0 0 0 0 0 0 0 0 0 0 0
X V 12    0 0 0 0 0 0 0 0 0 0 0 0
X V 12    0.168885 0.954362 64.7779 0.881044 36.331 0.396249 0.795785 108.979 4.31118 5.02115 3.21005 7.65333
X V 12    57.9432 66.5588 1.74459 116.681 -1.83632 12.0405 -3.0025 -0.191058 4.24935 12.9362 -3.16795 1.26517
X V 12    -0.130542 -0.988891 58.6281 1.15576 52.8127 -0.142249 -0.964811 126.734 5.90353 7.8276 11.9575 7.71333
X V 12    59.3141 -4.82943 -1.49093 69.1114 1.38378 23.1491 -20.6208 0.159987 5.97042 19.5765 -0.983929 -1.18935
X V 12    0.831715 -0.42174 124.764 0.430832 122.086 0.866434 -0.396734 128.363 0.434767 1.43219 2.49303 3.63333
X V 12    117.591 -22.2888 -0.327509 56.1211 0.631526 6.37988 -12.0346 -0.0147602 2.58341 1.43567 0.805491 0.78397
X V 11    -163.824 -259.002 -148.179 -150.871 -79.9854 -164.067 -81.5792 -0 -143.376 -133.501 -92.032
X M 12 12
X 154.237 -5.1651 -0.248939 51.5103 0.128987 -37.6612 4.12167 0.115299 1.07082 -0.146438 0.385108 -0.412705
X -5.1651 136.737 0.12035 14.3755 -0.0291214 -17.0376 -102.616 -0.0664486 -1.39088 3.40255 -0.68347 0.560556
X -0.248939 0.12035 0.122677 0.00830623 -0.0951407 -0.314648 0.0856071 -0.0258563 -0.0458333 0.164988 -0.0803785 -0.0423575
X 51.5103 14.3755 0.00830623 84.3251 -0.205434 5.12473 13.3444 0.0438463 1.27891 0.0724866 -0.0839429 0.504051
X 0.128987 -0.0291214 -0.0951407 -0.205434 0.082495 0.0861485 -0.424607 0.0155308 0.0322825 -0.0969249 0.0634908 0.0354724
X -37.6612 -17.0376 -0.314648 5.12473 0.0861485 50.4614 7.71106 0.111789 1.64885 -0.472846 -0.0696273 0.597543
X 4.12167 -102.616 0.0856071 13.3444 -0.424607 7.71106 117.522 0.153587 1.22156 -7.47375 1.75882 0.739181
X 0.115299 -0.0664486 -0.0258563 0.0438463 0.0155308 0.111789 0.153587 0.00904709 0.00947425 -0.0792907 0.023596 0.00705533
X 1.07082 -1.39088 -0.0458333 1.27891 0.0322825 1.64885 1.22156 0.00947425 0.921562 -0.0687988 -0.0262179 -0.0177828
X -0.146438 3.40255 0.164988 0.0724866 -0.0969249 -0.472846 -7.47375 -0.0792907 -0.0687988 5.4733 -1.30444 -0.749848
X 0.385108 -0.68347 -0.0803785 -0.0839429 0.0634908 -0.0696273 1.75882 0.023596 -0.0262179 -1.30444 0.661735 0.129144
X -0.412705 0.560556 -0.0423575 0.504051 0.0354724 0.597543 0.739181 0.00705533 -0.0177828 -0.749848 0.129144 0.476159
END_OF_FILE
if test 3868 -ne `wc -c <'mygest.cl'`; then
    echo shar: \"'mygest.cl'\" unpacked with wrong size!
fi
# end of 'mygest.cl'
fi
if test -f 'test.ez' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'test.ez'\"
else
echo shar: Extracting \"'test.ez'\" \(10865 characters\)
sed "s/^X//" >'test.ez' <<'END_OF_FILE'
X\begindata{text,1031800}
X\textdsversion{12}
X\template{med}
X
X
X
X\chapter{\bigger{\bigger{SEARCH HELP WINDOW
X
X}}
X}\author{\smaller{WALT (Washington University's Approach to Lots of 
XText)}}\chapter{
X
X}\author{Scott W. Hassan
X
XJune 1991
X
X
X
X}\section{\bold{1.0 Searching and Walt:}}
X
X
X\paragraph{\italic{Walt}'s most powerful feature and main objective is its 
Xhigh speed searching.  The high speed is accompished through the use of word 
Xindexes that point to the documents that contain the search query word.  When 
Xa group of search hits (a \italic{search hit} is a specific instance of the 
Xword or group of words in the text) are found, depending on the search 
Xalgorithm used, they are ordered in some way and displayed allowing the user 
Xto choose from the possible search hits.
X
X}\section{
X\bold{2.0 The Search Input Dialog:
X
X
X}}\paragraph{\section{\section{Before doing a search when using \italic{Walt}, 
Xthe screen display should look like Figure 2.0.  This is the \italic{search 
Xinput dialog }}}and allows the user to enter a search query and gives options 
Xto control the search.  To enter a search query, just start typing a word on 
Xthe keyboard (remember, keep the mouse in the \italic{Walt} window.)   The 
Xword should appear below in the Search Input Dialog.  You can use most editing 
Xcommands to correctly enter the search query.  Use spaces to denote different 
Xwords.\section{\section{
X
X}}}
X\center{\
X\begindata{raster,1089544}
X2 0 68266 68266 0 0 290 96
Xbits 1089544 290 96
Xzj0108q |
Xzw |
X03e0i38zq |
X0660i18zq |
X0c20i18zq |
X0e21c78dc79bzq |
X07826cc6cc9d98	zp |
X03c620c6181998	zp |
Xg!e7c6181980zp |
X08660cc6181980	zp |
X08660cc6181980	zp |
X0ce32cc60c9998	zp |
X0fc1c76f073dd8	zp |
Xzw |
Xm3fZJe0j |
Xzw |
Xzw |
Xzw |
Xzw |
Xzw |
Xzw |
Xzw |
Xzw |
Xzw |
Xg03c0r0fs3cm |
Xg0c303cg30h18	l30c3f0qc3g81	c73f80h |
Xg100866g10h08	l402118p01g!80	c208c0h |
Xg100842g10h08	l4f2108l08i01	3c8140a20840h |
Xg2004815cf1cb	9c78l9f9109cb	b6e1c6c39b1ei	027e4140b20820
X h |
Xg200481359226	a2c8l9f911a26	9b3323644d88i	027e4220920820
X h |
Xg2004812113e4	3e88l9f91f3e4	11106227c888i	027e42209a0820
X h |
Xg200481211204	2088l9f910204	!11a2240888i02	7e43e08a0820h |
Xg100842211204	2088l4f210204	111222240888i	013c84108a0840
X h |
Xg10086621b324	32d8l40210324	111322264888i	01g84108608c0
X h |
Xg0c303c70c9ce	1c64l30c381ce	3bb9d7739dc6j	c30e39c63f80h |
Xg03c0r0fs3cm |
Xzw |
Xzw |
Xzw |
Xg03c0r0fs3cm |
Xg0c31dc701006	h18k30c020cc0f	c0gc0lc303c7e0	j |
Xg100888201002	h08k40!204404	60g40k01g8662	30j |
Xg13c8cc60g!02	g08k4020504404	20g40k01g8422	10j |
Xg27e4444731f2	c79c78k801050	4404671c4c70j	02g481230j |
Xg27e444489243	6222c8k801088	4407cdb64890j	02g4813e0j |
Xg27e466cf92!22	3e88k80108844	0468a270c0j02	g4812c0j |
Xg27e42a8813!22	2088k8010f844	0428a25060j02	g481240j |
Xg13c83b8811c2	222088k402104	440428a24830j	01g842220j |
Xg1008110c9102	2232d8k402104	44046db64490j	01g866230j |
Xg0c30110739e7	719c64k30c38e	ee0fc71ce6e0k	c303c718j |
Xg03c0h0210n0f	s3cm |
Xk0330zp |
Xk01e0zp |
Xzw |
Xzw |
Xzw |
Xzw |
Xzw |
Xzw |
Xzw |
Xzw |
Xzw |
Xzw |
Xzw |
Xzw |
Xzg0fNf8l |
Xzg30n06l |
Xzg43Ne1l |
Xzg8cn1880k |
Xz0110n0440k |
Xz0120n0240k |
Xz0240h10g0180	h0120k |
Xz0240h10h80h01	20k |
Xz0280h28h80ia0	k |
Xz0280h286cd89d	c0ha0k |
Xz0280h44366c88	80ha0k |
Xz0280h44224488	80ha0k |
Xz0280h7c224485	ia0k |
Xz0280h82224487	ia0k |
Xz0280h82366c82	ia0k |
Xz0280g01c72c59	c2ia0k |
Xz0280i204004i	a0k |
Xz0240i204014h	0120k |
Xz0240i70e018h	0120k |
Xz0120n0240k |
Xz0110n0440k |
Xzg8cn1880k |
Xzg43Ne1l |
Xzg30n06l |
Xzg0fNf8l |
Xzw |
Xzw |
Xzw |
Xzw |
Xzw |
Xzw |
X\enddata{raster, 1089544}
X\view{rasterview,1089544,0,0,0}
X\italic{Figure 2.0
X
X
X}
X}\paragraph{After typing the search query (should look like Figure 2.1), 
Xeither click on the \bold{Apply} button or hit the \bold{Return} key.  This 
Xwill activate the search and the program will be busy for a while.  Just sit 
Xback and wait.  
X
X}\center{
X\begindata{raster,1150440}
X2 0 68266 68266 0 0 297 106
Xbits 1150440 297 106
Xzx |
Xzx |
Xzx |
Xzx |
Xgf8i0ej01cczl |
X0198i06kcczl |
X0308i06kc0zl |
X038871e371e6c0	01e78dc6dc783c	zj |
X01e09b31b32766	032cc6cd!cc64	zj |
Xgf18831860666	06gc618cc0cc0	zj |
Xg39f9f1860660	0607c618cc7cc0	zj |
X02198331860660	060cc618!ccc0	zj |
X02198331860660	060cc618!ccc0	zj |
X0338cb31832666	032cc60d!cc64	zj |
X03f071dbc1cf76	01c76f075e7638	zj |
Xzx |
Xm0fZJf8k |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xhf0r03c0r0fn |
Xg030c0fg0ch06	l0c30fcq30c020	71cfe0i |
Xg0402198004h02	l100846q40!20	308230i |
Xg0402108004h02	l13c842l02j4f	2050288210i |
Xg080120573c72	e71el27e44272	edb871b0e6c780	i9f90502c8208
X i |
Xg0801204d6489	a8b2l27e44689	a6ccc8d91362j	9f9088248208i |
Xg0801204844f9	0fa2l27e47cf9	04441889f222j	9f9088268208i |
Xg080120484481	0822l27e44081	044468890222j	9f90f8228208i |
Xg040210884481	0822l13c84081	044488890222j	4f2104228210i |
Xg040219886cc9	0cb6l100840c9	0444c8899222j	402104218230i |
Xg030c0f1c3273	8719l0c30e073	8eee75dce77180	i30c38e718fe0
X i |
Xhf0r03c0r0fn |
Xzx |
Xzx |
Xzx |
Xhf0r03c0r0fn |
Xg030c771c0401	80g06k0c300833	03f0g30l30c0f1	f8k |
Xg0402220804g80	g02k10!081101	18g10l4021988c	k |
Xg04f23318h!80	02k1008141101	08g10l40210884	k |
Xg09f9!11cc7cb1	e71ek20041411	0119c7131ck80	12048ck |
Xg09f911122490	d888b2k200422	1101f36d9224k	801204f8k |
Xg09f919b3e4!88	8fa2k20042211	011a289c30k80	1204b0k |
Xg09f90aa204c8	!8822k20043e11	010a289418k80	120490k |
Xg04f20ee20470	!8822k10084111	010a28920ck40	210888k |
Xg040204432440	888cb6k100841	11011b6d9124k	4021988ck |
Xg030c0441ce79	dc6719k0c30e3	bb83f1c739b8k	30c0f1c6k |
Xhf0i84n03c0r0f	n |
Xlcczq |
Xl78zq |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzg03Nfem |
Xzg0cn0180l |
Xzg10Nf840l |
Xzg23n0620l |
Xzg44n0110l |
Xzg48o90l |
Xzg90h04h60i48	l |
Xzg90h04h20i48	l |
Xzga0h0ah20i28	l |
Xzga0h0a1b3627	70h28l |
Xzga0h110d9b22	20h28l |
Xzga0h11089122	20h28l |
Xzga0h1f089121	40h28l |
Xzga0h20889121	c0h28l |
Xzga0h208d9b20	80h28l |
Xzga0h71cb1670	80h28l |
Xzga0i081001i28	l |
Xzg90i081005i48	l |
Xzg90i1c3806i48	l |
Xzg48o90l |
Xzg44n0110l |
Xzg23n0620l |
Xzg10Nf840l |
Xzg0cn0180l |
Xzg03Nfem |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
X\enddata{raster, 1150440}
X\view{rasterview,1150440,1,0,0}
X\italic{Figure 2.1
X
X
X}}\section{\bold{3.0  The Search Result Dialog:
X
X}}\center{
X}\paragraph{Figure 3.0 displays the result of the search query.  A search was 
Xmade for the word \bold{\typewriter{cardiac }}and there were 248 hits of 
Xcardiac throughout the text database.  The histogram shows a sorted list of 
Xsearch hits.  The X axis is an individual section where the search hit occurs 
Xand the Y axis is the logarithmic weight of the search hit.   The first dotted 
Xline is a weight of 10, the second is a weight of 100, and the top of the 
Xhistogram has a weight of 1000.  Rarely should you ever get a weight over 100. 
X On the left of the histogram, there are the 'best' search hits decreasing to 
Xthe right of the diagram.
X
X}
X\paragraph{Each of the bars of the histogram is active.  Click on a bar and 
X\italic{Walt} will jump to that section and display it.  The white bars mean 
Xthat the search hit is in another book, the gray bars mean that the search hit 
Xis in the current book but in another chapter, and the black bars mean that 
Xthe search hit is in the current chapter.
X
X
X}\paragraph{The buttons at the bottom of the Search Result Dialog are 
X\bold{Previous}, \bold{Next}, \bold{New} \bold{Search}, and help.  The 
X\bold{Next} Button is used to move to the next search hit and the 
X\bold{Previous} Button is used to move to the previous search hit.  The 
X\bold{Next} Button is associated with the \bold{Return} key so that you can 
Xjust hit the \bold{Return} key on the keyboard.  \bold{New Search} will bring 
Xthe Search Input Dialog back and allow the user to enter into a new search 
Xquery.  Typing any other key will do the same at this point.
X
X}\center{\
X\begindata{raster,1130688}
X2 0 68266 68266 0 0 302 114
Xbits 1130688 302 114
Xzx |
Xzx |
Xzx |
Xgf8i0ej01cczl |
X0198i06kcczl |
X0308i06kc0zl |
X038871e371e6c0	01e78dc6dc783c	zj |
X01e09b31b32766	032cc6cd!cc64	zj |
Xgf18831860666	06gc618cc0cc0	zj |
Xg39f9f1860660	0607c618cc7cc0	zj |
X02198331860660	060cc618!ccc0	zj |
X02198331860660	060cc618!ccc0	zj |
X0338cb31832666	032cc60d!cc64	zj |
X03f071dbc1cf76	01c76f075e7638	zj |
Xzx |
Xm0fZJf8k |
Xy0280071808h78	11e0o |
Xy02800c!08hcc	3330o |
Xy0fc0!08g80g84	3210o |
Xy05039f0b19e7	20045330o |
Xy0506c80d8889	200891e0o |
Xy05044808888c	g109330o |
Xy1f8448088886	g211210o |
Xh08v0a04480888	83g41fa10o |
Xh08v0a06c80888	8920c41330o |
Xh08v0a039e1ddc	6e20fc11e0o |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh0d/55,5550k |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh08zu |
Xh0ffezt |
Xh0fd7G80zr |
Xh0feaba80zr |
Xh0fd755Hzq |
Xh0a/aa,aaa0k |
Xh0fd755d575Lc0	zj |
Xh0feabaaeabaa	ebGaeabaac0zj |
Xh0fd755d5755d	57Gd5755d40zj |
Xh0feabaaeabaa	ebGaeabaaYf8k |
Xh0fd755d5755d	57Gd5755d5755	d5755d5755d575	5d57Gd5755d57
X 55d57558k |
Xh0feabaaeabaa	ebGaeabaaeaba	aeabaaeabaaeab	aaebGaeabaaea
X baaeaba8k |
Xh0fd755d5755d	57Gd5755d5755	d5755d5755d575	5d57Gd5755d57
X 55d57558k |
Xh0feabaaeabaa	ebGaeabaaeaba	aeabaaeabaaeab	aaebGaeabaaea
X baaeaba8k |
Xh0fd755d5755d	57Gd5755d5755	d5755d5755d575	5d57Gd5755d57
X 55d57558k |
Xh0feabaaeabaa	ebGaeabaaeaba	aeabaaeabaaeab	aaebGaeabaaea
X baaeaba8k |
Xh0fd755d5755d	57Gd5755d5755	d5755d5755d575	5d57Gd5755d57
X 55d57558k |
Xh0feabaaeabaa	ebGaeabaaeaba	aeabaaeabaaeab	aaebGaeabaaea
X baaeaba8k |
Xh0fd755d5755d	57Gd5755d5755	d5755d5755d575	5d57Gd5755d57
X 55d57558k |
Xh0feabaaeabaa	ebGaeabaaeaba	aeabaaeabaaeab	aaebGaeabaaea
X baaeaba8k |
Xh0fd755d5755d	57Gd5755d5755	d5755d5755d575	5d57Gd5755d57
X 55d57558k |
Xh0fZOf8k |
Xzx |
Xzx |
Xh06zu |
Xh0fzu |
Xh1f80zt |
Xh3fc0zt |
Xh7fe0zt |
XsO80u |
Xr03o60u |
XhNf8g043fMfe10	g0fN80g03e0g |
Xg03n06g08c0m01	88g30n60g0c18	g |
Xg04n01g11o44g	40n10g1004g |
Xg08o8012o24g80	n08g2002g |
Xg10g7eh02j4024	h038ek12011c70	h0e80h1804g43	81g |
Xg10g23h02j4024	h0184k12010c20	h1980h0804g44	c1g |
Xg20g21m2028h01	44g01i0a020a20	h1080h0802g84	4080 |
Xg20g215ce77639	98e0g2028h0164	73bbc0h0a020b	239ddc181c7173
X 8b02g80c080 |
Xg20g233512226c	8920g2028h0124	8911i0a020924	48880e22c8d64d
X 82g81!80 |
Xg20g3e21f22244	8980g2028h0134	f8e1i0a0209a7	c888033e188408
X 82g81g80 |
Xg20g2021014244	88c0g2028h0114	8041i0a0208a4	0550ga0688408
X 82g81g80 |
Xg20g202101c244	8860g2028h0114	80e1i0a0208a4	077010a0888408
X 82g80g80 |
Xg20g202190826c	d920g2028h010c	c911i0a020866	422019b2c88648
X 82g81g80 |
Xg20g!70e08738	6dc0g2028h038c	73b8c0h0a021c	638220171c75c3
X 9dc2g81g80 |
Xg20o2028o0a02	o02g80g80 |
Xg10o4024o1201	o04g4001g |
Xg10o4024o1201	o04g4001g |
Xg08o8012o24g80	n08g2002g |
Xg04n01g11o44g	40n10g1004g |
Xg03n06g08c0m01	88g30n60g0c18	g |
XhNf8g043fMfe10	g0fN80g03e0g |
Xr03o60u |
XsO80u |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
Xzx |
X\enddata{raster, 1130688}
X\view{rasterview,1130688,2,0,0}
X\italic{Figure 3.0}
X
X}\paragraph{\author{\section{
X}}}\bold{ALSO SEE}
X
X	\typewriter{WALT
X
X	tf-idf weighting scheme
X
X}\enddata{text,1031800}
END_OF_FILE
if test 10865 -ne `wc -c <'test.ez'`; then
    echo shar: \"'test.ez'\" unpacked with wrong size!
fi
# end of 'test.ez'
fi
if test -f 'testapp.c' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'testapp.c'\"
else
echo shar: Extracting \"'testapp.c'\" \(2576 characters\)
sed "s/^X//" >'testapp.c' <<'END_OF_FILE'
X/******************************************************************************
X *
X * testapp.ch - Test Application
X * Medical Informatics 
X * Washington University, St. Louis
X * July 29, 1991
X *
X * Scott Hassan
X * Steve Cousins
X * Mark Frisse
X *
X *****************************************************************************/
X
X#define TESTLOADFILE "test.ez"
X
X/*****************************************************************************
X * 
X * testapp.c -- The application module
X *
X *****************************************************************************/
X
X#include <im.ih>
X#include <frame.ih>
X#include <event.ih>
X#include <app.ih>
X#include <text.ih>
X#include <textv.ih>
X#include <attribs.h>
X#include <scroll.ih>
X#include <filetype.ih>
X
X#include <gtextv.ih>
X
X/*****************************************************************************/
X
X#include <stdio.h>
X
X#include <testapp.eh>
X
X
X/*****************************************************************************/
X
Xboolean testapp__InitializeObject(classID, self)
X     struct classheader *classID;
X     struct testapp *self;
X{
X  return TRUE;
X
X}
X
Xvoid testapp__FinalizeObject(classID, self) 
X     struct classheader *classID;
X     struct testapp *self;
X{
X}
X
X/*****************************************************************************/
X
Xboolean testapp__Start(self)
Xstruct testapp *self;
X{
X  struct attributes attrs, *attr;
X  long objectID;
X  FILE *fp;
X
X  self->im = im_Create(NULL); /* Create an interaction manager. */
X  if(!self->im) {
X      printf("%s: failed to start an interaction manager.\n",testapp_GetName(self));
X      return(FALSE);
X    }
X
X  self->gtextv = gtextv_New(); /* Create an IRS view.            */
X  if(!self->gtextv) { 
X      printf("%s: failed to create the IRS view object.\n",testapp_GetName(self));
X      return(FALSE);
X    }
X
X  self->text = text_New();
X  gtextv_SetDataObject(self->gtextv, self->text);
X
X  attrs.next = (struct attributes *)NULL;
X  attrs.value.integer = 0;
X  text_SetAttributes(self->text, &attrs);
X
X  self->scroll = scroll_Create(self->gtextv, scroll_LEFT);
X
X  attr = NULL;
X
X  fp = fopen(TESTLOADFILE,"r");
X  filetype_Lookup(fp, TESTLOADFILE, &objectID, attr);
X
X  if(attr != NULL)
X    text_SetAttributes(self->text, attr);
X    
X  text_Read(self->text, fp, objectID);
X  fclose(fp);
X
X  text_SetAttributes(self->text, &attrs);
X/*  text_SetReadOnly(self->text, TRUE);*/
X
X  self->frame = frame_New();
X  frame_SetView(self->frame, self->scroll);
X  im_SetView(self->im, self->frame);
X
X  gtextv_WantInputFocus(self->gtextv, self->gtextv);
X
X  im_ForceUpdate();
X
X  return TRUE;
X
X}
END_OF_FILE
if test 2576 -ne `wc -c <'testapp.c'`; then
    echo shar: \"'testapp.c'\" unpacked with wrong size!
fi
# end of 'testapp.c'
fi
if test -f 'testapp.ch' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'testapp.ch'\"
else
echo shar: Extracting \"'testapp.ch'\" \(679 characters\)
sed "s/^X//" >'testapp.ch' <<'END_OF_FILE'
X/******************************************************************************
X *
X * testapp.ch - Test Application
X * Medical Informatics 
X * Washington University, St. Louis
X * July 29, 1991
X *
X * Scott Hassan
X * Steve Cousins
X * Mark Frisse
X *
X *****************************************************************************/
X
Xclass testapp : application[app] {
X	overrides:
X	  Start() returns boolean;
X	methods:
X	classprocedures:
X	  InitializeObject(struct testapp *self) returns boolean;
X	  FinalizeObject(struct testapp *self);
X	data:
X	  struct im *im;
X	  struct view *view;
X	  struct gtextv *gtextv;
X	  struct text *text;
X	  struct frame *frame;
X	  struct scroll *scroll;
X};
END_OF_FILE
if test 679 -ne `wc -c <'testapp.ch'`; then
    echo shar: \"'testapp.ch'\" unpacked with wrong size!
fi
# end of 'testapp.ch'
fi
echo shar: End of shell archive.
exit 0
-- 

