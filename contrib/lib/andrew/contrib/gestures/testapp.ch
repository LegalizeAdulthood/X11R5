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

class testapp : application[app] {
	overrides:
	  Start() returns boolean;
	methods:
	classprocedures:
	  InitializeObject(struct testapp *self) returns boolean;
	  FinalizeObject(struct testapp *self);
	data:
	  struct im *im;
	  struct view *view;
	  struct gtextv *gtextv;
	  struct text *text;
	  struct frame *frame;
	  struct scroll *scroll;
};
