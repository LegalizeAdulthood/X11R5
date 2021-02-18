/******************************************************************************
 *
 * testapp.ch - Test Application
 * Medical Informatics 
 * Washington University, St. Louis
 * July 29, 1991
 *
 * Scott Hassan
 * Steve Cousins
 * Mark Frisse
 *
 *****************************************************************************/

#define TESTLOADFILE "test.ez"

/*****************************************************************************
 * 
 * testapp.c -- The application module
 *
 *****************************************************************************/

#include <im.ih>
#include <frame.ih>
#include <event.ih>
#include <app.ih>
#include <text.ih>
#include <textv.ih>
#include <attribs.h>
#include <scroll.ih>
#include <filetype.ih>

#include <gtextv.ih>

/*****************************************************************************/

#include <stdio.h>

#include <testapp.eh>


/*****************************************************************************/

boolean testapp__InitializeObject(classID, self)
     struct classheader *classID;
     struct testapp *self;
{
  return TRUE;

}

void testapp__FinalizeObject(classID, self) 
     struct classheader *classID;
     struct testapp *self;
{
}

/*****************************************************************************/

boolean testapp__Start(self)
struct testapp *self;
{
  struct attributes attrs, *attr;
  long objectID;
  FILE *fp;

  self->im = im_Create(NULL); /* Create an interaction manager. */
  if(!self->im) {
      printf("%s: failed to start an interaction manager.\n",testapp_GetName(self));
      return(FALSE);
    }

  self->gtextv = gtextv_New(); /* Create an IRS view.            */
  if(!self->gtextv) { 
      printf("%s: failed to create the IRS view object.\n",testapp_GetName(self));
      return(FALSE);
    }

  self->text = text_New();
  gtextv_SetDataObject(self->gtextv, self->text);

  attrs.next = (struct attributes *)NULL;
  attrs.value.integer = 0;
  text_SetAttributes(self->text, &attrs);

  self->scroll = scroll_Create(self->gtextv, scroll_LEFT);

  attr = NULL;

  fp = fopen(TESTLOADFILE,"r");
  filetype_Lookup(fp, TESTLOADFILE, &objectID, attr);

  if(attr != NULL)
    text_SetAttributes(self->text, attr);
    
  text_Read(self->text, fp, objectID);
  fclose(fp);

  text_SetAttributes(self->text, &attrs);
/*  text_SetReadOnly(self->text, TRUE);*/

  self->frame = frame_New();
  frame_SetView(self->frame, self->scroll);
  im_SetView(self->im, self->frame);

  gtextv_WantInputFocus(self->gtextv, self->gtextv);

  im_ForceUpdate();

  return TRUE;

}
