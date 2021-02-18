/******************************************************************************
 *
 * gtextv - Gesture Text View
 * Medical Informatics 
 * Washington University, St. Louis
 * July 29, 1991
 *
 * Scott Hassan
 * Steve Cousins
 * Mark Frisse
 *
 *****************************************************************************/

#define NOPOINT -1L

#define MODULE "gtextv"
/*#define DEBUGFLAG */

/*****************************************************************************
 * 
 * gtextv.c -- The Gesture Text View Module
 *
 *****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>

#include <class.h>
#include <text.ih>
#include <textv.ih>
#include <dataobj.ih>
#include <mark.ih>
#include <environ.ih>

/*****************************************************************************/

#include <gtextv.eh>

#include <math.h>
#include <gestures/bitvector.h>
#include <gestures/matrix.h>
#include <gestures/sc.h>
#include <gestures/fv.h>
#include <gestures/zdebug.h>

/*****************************************************************************/

long clock()
{
    struct timeval this;
    gettimeofday(&this, NULL);
    return this.tv_usec;
}

/* strdup: return a freshly malloced copy of a string so that it
  can be safely freed later. */

static char *strdup(x)
char *x;
{
    char *p=(char *)malloc(strlen(x) + 1);
    if(!p) return x;
    strcpy(p, x);
    return p;
}

sClassifier fullclassifier;
sClassifier doneclassifier;
static FV fv;
char _zdebug_flag[128];

char *ClassifyFv();
char *ClassifyVector();

#define GESTURE_FILE "/lib/gestures/gest.cl"

/******************************************************************************
 * InitializeClass(class)
 * 
 * Setup the Gesture Recognizer and Read in the Gesture file.
 *
 *****************************************************************************/

boolean gtextv__InitializeClass(class) 
     struct classheader *class;
{
  FILE *fp;
  char *temp;
  char *filename;

  doneclassifier = NULL;

  temp = (char *) environ_GetProfile("GestureFile");

  if(temp == NULL) 
    filename = (char *) strdup(environ_AndrewDir(GESTURE_FILE));
  else
    filename = (char *) strdup(temp);

  fp = fopen(filename,"r");

  if(fp == NULL) {
    printf("gtextv: Cannot find file: %s\n", filename);
    return FALSE;
  }

  fullclassifier = sRead(fp);
  fclose(fp);

  fv = FvAlloc(0);

/*  free(filename);*/

  return TRUE;
}     

/******************************************************************************
 * InitializeObject(class, self)
 * 
 * Set the object's initial data to null values.
 *
 *****************************************************************************/

boolean gtextv__InitializeObject(class, self)
     struct classheader *class;
     struct gtextv *self;
{
  self->parstart = NOPOINT;
  self->parend = NOPOINT;

  self->limit = 500;
  self->xp = (int *)malloc(self->limit * sizeof(int));
  self->yp = (int *)malloc(self->limit * sizeof(int));
  self->index = 0;

  return TRUE;
}
/******************************************************************************
 * FinalizeObject(class, self)
 * 
 * Free up some of the used memory.
 *
 *****************************************************************************/

void gtextv__FinalizeObject(class, self)
     struct classheader *class;
     struct gtextv *self;
{
  free(self->xp);
  free(self->yp);
}

/******************************************************************************
 * Hit(self, action, x, y, numclicks)
 * 
 * Track the mouse cursor and process the path.
 *
 *****************************************************************************/

struct view *gtextv__Hit(self, action, x, y, numclicks)
     struct gtextv *self;
     enum view_MouseAction action;
     long x,y, numclicks;
{
  int i;
  struct view *temp;
  int transfer;
  long ts_timer;
  char *class;
  struct rectangle VisualRect;
  struct view *vptr;
  long newPos;
  int my, mx;

  newPos = gtextv_Locate(self,x,y, &vptr);

  temp = (struct view *)self;
  ts_timer = clock() / 1000;

  gtextv_GetVisualBounds(self, &VisualRect);
  
  gtextv_SetTransferMode(self, graphic_XOR);

  if(action == view_LeftMovement) { /* Track the mouse movements */
    self->xp[self->index] = x;
    self->yp[self->index] = y;
    gtextv_MoveTo(self, self->xp[self->index - 1], self->yp[self->index - 1]);
    gtextv_DrawLineTo(self, x, y);
    self->index++;
    FvAddPoint(fv, x, VisualRect.height - y, ts_timer, 0);

  } else if(action == view_LeftDown) { /* signals the start of an action */
    self->xp[self->index] = x;
    self->yp[self->index] = y;
    gtextv_MoveTo(self, x, y);
    gtextv_DrawLineTo(self, x, y);
    self->index++;
    FvInit(fv); /* initialize the gesture handler */
    FvAddPoint(fv, x, VisualRect.height - y, ts_timer, 0);

  } else if(action == view_LeftUp) {  /* signals the end of an action */
    i=0;
    gtextv_MoveTo(self, self->xp[i], self->yp[i]);
    for(i=0; i<self->index; i++) {
      gtextv_DrawLineTo(self, self->xp[i], self->yp[i]);
    }
    FvAddPoint(fv, x, VisualRect.height - y, ts_timer, 0);
    class = ClassifyFv(); /* Classify the sampled points into a gesture */
/*    printf("clasifyfv: %s\n", class);*/
/************************************************************ PARSTART Gesture handler *****/
    if(!strcmp(class, "parstart")) {
      if(CalcMiddle(self->xp, self->yp, self->index, &mx, &my)==TRUE) {
	self->parstart = gtextv_Locate(self,mx,my, &vptr);
	if(self->parend != NOPOINT) {
	  if(self->parstart < self->parend) {
	    gtextv_SetDotPosition(self, self->parstart);
	    gtextv_SetDotLength(self, self->parend - self->parstart);
	    textview_CopyRegionCmd(self);
	  } else {
	    gtextv_SetDotPosition(self, self->parstart);
	    gtextv_SetDotLength(self, 0);
	  }
	}
      }
/************************************************************ PAREND Gesture handler *****/
    } else if(!strcmp(class, "parend")) {
      if(CalcMiddle(self->xp, self->yp, self->index, &mx, &my)==TRUE) {
	self->parend = gtextv_Locate(self,mx,my, &vptr);
	if(self->parstart != NOPOINT) {
	  if(self->parstart > self->parend) {
	    self->parstart = NOPOINT;
	    self->parend = NOPOINT;
	  } else {
	    gtextv_SetDotPosition(self, self->parstart);
	    gtextv_SetDotLength(self, self->parend - self->parstart);
	    textview_CopyRegionCmd(self);
	  }
	}
      }
/************************************************************ .DOT Gesture handler *****/
    } else if(!strcmp(class, ".dot")) {
      self->parstart = NOPOINT;
      self->parend = NOPOINT;
      
      gtextv_SetDotLength(self, 0);
      gtextv_SetDotPosition(self, newPos);
/************************************************************ TOP Gesture handler *****/
    } else if(!strcmp(class, "top")) {
      textview_BeginningOfTextCmd(self);
/************************************************************ BOTTOM Gesture handler *****/
    } else if(!strcmp(class, "bottom")) {
      textview_EndOfTextCmd(self);
/************************************************************ DOWN Gesture handler *****/
    } else if(!strcmp(class, "down")) {
      textview_NextScreenCmd(self);
/************************************************************ UP Gesture handler *****/
    } else if(!strcmp(class, "up")) {
      textview_PrevScreenCmd(self);
/************************************************************ PASTE Gesture handler *****/
    } else if(!strcmp(class, "paste")) {
      newPos = gtextv_Locate(self,self->xp[0],self->yp[0], &vptr);
      gtextv_SetDotPosition(self, newPos);
      gtextv_SetDotLength(self, 0);
      textview_YankCmd(self);
      self->parstart = newPos;
      self->parend = newPos + gtextv_GetDotLength(self);
/************************************************************ SELECT Gesture handler *****/
    } else if(!strcmp(class, "select")) {
      self->parstart = gtextv_Locate(self, self->xp[0], self->yp[0], &vptr);
      self->parend = gtextv_Locate(self, self->xp[self->index - 1], 
				     self->yp[self->index - 1], &vptr);
      gtextv_SetDotPosition(self, self->parstart);
      gtextv_SetDotLength(self, self->parend - self->parstart);
      textview_CopyRegionCmd(self);
/************************************************************ DELETE Gesture handler *****/
    } else if(!strcmp(class, "delete")) {
      newPos = gtextv_Locate(self,self->xp[0],self->yp[0], &vptr);
      if(gtextv_GetDotLength(self) >= 0) {
	if(self->parstart != NOPOINT && self->parend != NOPOINT) {
	  if(newPos >= self->parstart && newPos <= self->parend) {
	    textview_ZapRegionCmd(self);
	    self->parstart = NOPOINT;
	    self->parend = NOPOINT;
	  }
	}
      }
    }
    self->index = 0;
  }

  gtextv_SetTransferMode(self, graphic_COPY);
  return((struct view *)temp);
}

/**********************************************************************/

CalcMiddle(xp, yp, length, middlex, middley)
     int *xp;
     int *yp;
     int length;
     int *middlex;
     int *middley;
{
  int i;


  if(length>0) {
    *middlex = *middley = 0;

    for(i=0; i<length; i++) {
      *middlex += xp[i];
      *middley += yp[i];
    }
    *middlex = *middlex / length;
    *middley = *middley / length;
    return TRUE;
  } else {
    *middlex = NOPOINT;
    *middley = NOPOINT;
    return FALSE;
  }
}

/**********************************************************************/

char *ClassifyFv()
{
  Vector v;

  v = FvCalc(fv);
  return ClassifyVector(v);
}

/**********************************************************************/

char *ClassifyVector(y)
     Vector y;
{
  register sClassDope cd;
  
  cd = sClassify(fullclassifier, y);
  if(cd == NULL) {
    return NULL;
  } else {
    return(cd->name);
  }
}

