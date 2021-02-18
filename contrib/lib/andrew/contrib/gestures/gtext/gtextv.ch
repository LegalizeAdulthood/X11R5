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

/*****************************************************************************
 * 
 * gtextv.c -- The Gesture Text View Module
 *
 *****************************************************************************/

class gtextv : textview [textv] {

 overrides:
  Hit(enum view_MouseAction action, long x, long y, long numberOfClicks)
    returns struct view *;
 methods:
 classprocedures:
  InitializeClass() returns boolean;
  InitializeObject(struct gtextv *self) returns boolean;
  FinalizeObject(struct gtextv *self);

 data:
  int *xp, *yp;          /* Mouse Buffering Arrays */
  int index;             /* The current length of the buffers */
  int limit;             /* The current limit of the buffers */
  long parstart, parend; /* The starting and ending positions for */
                         /* the selected region. */
}; 


