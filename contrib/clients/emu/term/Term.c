#ifndef lint
static char *rcsid = "$Header: /usr3/Src/emu/term/RCS/Term.c,v 1.17 91/09/30 18:07:14 me Exp Locker: jkh $";
#endif

/*
 * This file is part of the PCS emu program.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * This is the standard wrapper code for the term widget.
 *
 * Author: Jordan K. Hubbard
 * Date: April 23th, 1990.
 * Description: The usual entry points and data structures required by the
 *		X Toolkit.
 *
 * Revision History:
 *
 * $Log:	Term.c,v $
 * Revision 1.17  91/09/30  18:07:14  me
 * got rid of some compiler warnings in R5
 * 
 * Revision 1.16  91/08/29  11:30:17  jkh
 * dunno
 * 
 * Revision 1.15  91/07/11  17:30:45  tom
 * To quiet a sniveling compiler
 * 
 * Revision 1.14  91/07/05  14:38:30  tom
 * Whatever jkh did.
 * 
 * Revision 1.13  90/11/20  17:45:10  jkh
 * Alpha.
 * 
 * Revision 1.12  90/11/18  13:44:35  jkh
 * Check-in before changes to process mechanism.
 * 
 * Revision 1.11  90/11/13  14:55:13  jkh
 * Double fd version.
 * 
 * Revision 1.1  90/05/03  05:50:09  jkh
 * Initial revision
 * 
 *
 */

#include "TermP.h"
#include "TermCanvas.h"

Local XtGeometryResult GeometryManager();
Local void Initialize(), Realize(), Destroy(), Resize();
Local Boolean SetValues();
Local XtGeometryResult DefLayout();

#define offset(field) XtOffset(TermWidget, term.field)

Local XtResource resources[] = {
     { XpNcommand,		XpCCommand,		XtRString,
	    sizeof(String),	offset(command),	XtRImmediate,
	    XpNdefaultCommand	},
     { XpNcommandArgs,		XpCCommandArgs,		XtRPointer,
	    sizeof(String *),	offset(command_args),	XtRImmediate,
	    NULL		},
     { XpNtermType,		XpCTermType,		XtRString,
	    sizeof(String),	offset(term_type),	XtRImmediate,
	    XpNdefaultTermType	},
     { XtNwidthInc,		XtCWidthInc,		XtRDimension,
	    sizeof(Dimension),	offset(w_inc),		XtRImmediate,
	    (caddr_t)0 },
     { XtNheightInc,		XtCHeightInc,		XtRDimension,
	    sizeof(Dimension),	offset(h_inc),		XtRImmediate,
	    (caddr_t)0 },
     { XpNcanvas,		XpCCanvas,		XtRWidget,
	    sizeof(Widget),	offset(canvas),		XtRImmediate,
	    NULL		},
     { XpNiopRequestProc,	XpCIopRequestProc,	XtRCallback,
	    sizeof(VoidFuncPtr),offset(iop_req),	XtRImmediate,
	    NULL		},
     { XpNlayoutProc,		XpCLayoutProc,		XtRFunction,
	    sizeof(GFuncPtr),	offset(layout_kids),	XtRImmediate,
	    (caddr_t)DefLayout	},
     { XpNinParser,		XpCInParser,		XtRCallback,
	    sizeof(XtCallbackList),offset(in_parse),	XtRImmediate,
	    NULL		},
     { XpNoutParser,		XpCOutParser,		XtRCallback,
	    sizeof(XtCallbackList),offset(out_parse),	XtRImmediate,
	    NULL		},
     { XpNprocessDeath,		XpCProcessDeath,	XtRCallback,
	    sizeof(XtCallbackList),offset(proc_death),	XtRImmediate,
	    NULL		},
     { XpNreadSize,		XpCReadSize,		XtRInt,
	    sizeof(int),	offset(read_size),	XtRImmediate,
	    (caddr_t)XpNdefaultReadSize },
     { XpNutmpInhibit,		XpCUtmpInhibit,		XtRBoolean,
	    sizeof(Boolean),	offset(utmp_inhibit),	XtRImmediate,
	    (caddr_t)FALSE	},
     { XpNloginShell,		XpCLoginShell,		XtRBoolean,
	    sizeof(Boolean),	offset(login_shell),	XtRImmediate,
	    (caddr_t)FALSE	},
     { XpNrops,			XpCRops,		XtRString,
	    sizeof(String),	offset(rops),		XtRImmediate,
	    NULL		},
};

Export TermClassRec termClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass)&compositeClassRec,
    /* class_name		*/	"Term",
    /* widget_size		*/	sizeof(TermRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	FALSE,
    /* compress_exposure	*/	FALSE,
    /* compress_enterleave	*/	FALSE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	NULL,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	NULL,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL,
  },
  { /* composite fields         */
    /* geometry_manager         */      GeometryManager,
    /* change_managed           */      NULL,
    /* insert_child             */      XtInheritInsertChild,
    /* delete_child             */      XtInheritDeleteChild,
    /* extension                */      NULL,
  },
  { /* Term fields */
    /* signal bump count	*/	0,
    /* process list		*/	NULL,
  },
};

Export WidgetClass termWidgetClass = (WidgetClass)&termClassRec;

Local void
Initialize(req, new)
Widget req, new;
{
     TermWidget w = (TermWidget)new;

     /* Allocate space for things that, uh,  need space allocated for them. */
     w->term.cb = Fresh(ComBlock);
     w->term.save = Fresh(ComBlock);
     cb_opcode(w->term.save) = OP_NOP;

     /* Clear some things first so we don't get confused later */
     w->term.zero_state = NULL;
     w->term.rparse_list = NULL;
     w->term.pid = 0;
     w->term.xi = w->term.xo = NULL;
     w->term.w_inc = w->term.h_inc = 0;
     w->term.chars_to_parse = 0;
     w->term.char_parse_mark = 0;

     /* Make sure we start out being able to resize */ 
     UNLOCK_RESIZE(w);

     /* Now make sure at least one parser is installed */
     if (!parser_install(w) && !w->term.in_parse)
	  fatal("No input parser found for term type '%s'",
		w->term.term_type);

     /* Get the initial tty settings from the invoking terminal */
     w->term.init_tty = tty_get_values(NULL);

     /* Set up and initialize tty */
     if (!tty_open(w))
	  fatal("Couldn't open and initialize pty");

     if (w->term.rops) {
	  w->term.rops = backslash_convert(w->term.rops);
	  rparse_add(w, w->term.rops);
     }
     /* Call whatever tty init sequence has declared for setting up the tty */
     XpTermDoRop(w, ROP_INIT_TTY);
}

Local void
Destroy(w)
TermWidget w;
{
     /* Remove the fd's from the input handler and close them */
     tty_close(w);

     /* If there's an active PID, nuke it */
     process_signal(w, SIGKILL);

     /* Now clean up the parser, if necessary. */
#ifndef NO_GENERIC_PARSER
     if (w->term.zero_state)
	  destroy_state(w->term.zero_state);
     if (w->term.current_states)
	  destroy_current_states(w);
#endif
     if (w->term.rparse_list)
	  destroy_rparse_list(w->term.rparse_list);

     /* Nuke common storage */
     XtFree((char *)w->term.cb);
     XtFree((char *)w->term.save);
}

Local Boolean
SetValues(old, req, new)
TermWidget old, req, new;
{
     Arg args[4];
     int i, redisplay = FALSE;

     if (strcomp(new->term.term_type, old->term.term_type))
	  XpTermDoEmulation(new, new->term.term_type);

     i = 0;
     if (new->term.w_inc != old->term.w_inc) {
	  XtSetArg(args[i], XtNwidthInc, new->term.w_inc);	i++;
     }
     if (new->term.h_inc != old->term.h_inc) {
	  XtSetArg(args[i], XtNheightInc, new->term.h_inc);	i++;
     }
     if (i) {
	  XtSetValues(XtParent(new), args, i);
	  redisplay = TRUE;
     }
     return redisplay;
}

Local void
Realize(w, value_mask, attributes)
TermWidget w;
Mask *value_mask;
XSetWindowAttributes *attributes;
{
     Arg args[4];
     int i;
     
     if (!w->composite.num_children && !CANVAS_W(w))
	  fatal("No canvas given for term widget");
     /* If no canvas explicitly specified, default it to first child */
     else if (!CANVAS_W(w))
	  CANVAS_W(w) = w->composite.children[0];

     /* Be safe */
     if (w->core.height == 0)
	  w->core.height = 1;
     if (w->core.width == 0)
	  w->core.width = 1;

     if (!w->term.read_size || w->term.read_size > BUF_SZ)
	  fatal("read size of %d is zero or > max internal buffer size of %d",
		w->term.read_size, BUF_SZ);

     /* Create window with which to manage child */
     XtCreateWindow((Widget)w, (unsigned int)InputOutput,
		    (Visual *)CopyFromParent, *value_mask, attributes);
     XtSetKeyboardFocus((Widget)w, CANVAS_W(w));

     /* Send initial size and position request to layout proc */
     LOCK_RESIZE(w);
     (void) (*(w->term.layout_kids))(w, w, w->core.width, w->core.height);
     UNLOCK_RESIZE(w);

     /*
      * Set up a handshake for launching the process when the canvas is
      * finally sized and mapped.
      */
     i = 0;
     XtSetArg(args[i], XpNnotifyFirstMap, process_create);	i++;
     XtSetValues(CANVAS_W(w), args, i);
}

Local void
Resize(w)
TermWidget w;
{
     if (XtIsRealized((Widget)w) && RESIZE_UNLOCKED(w)) {
	  LOCK_RESIZE(w);
	  (void) (*(w->term.layout_kids))(w, NULL, w->core.width,
					  w->core.height);
	  UNLOCK_RESIZE(w);
     }
}

Local XtGeometryResult
GeometryManager(w, request, reply)
TermWidget w;
XtWidgetGeometry *request;
XtWidgetGeometry *reply; /* RETURN */
{
     TermWidget dad; /* That's me */
     XtGeometryResult res;
     Dimension width, height;

     dad = (TermWidget)XtParent(w);
     if (!XtIsRealized((Widget)dad))
	  return XtGeometryNo;
     /* Position request always refused */
     if (request->request_mode & CWX || request->request_mode & CWY)
          res = XtGeometryNo;
     else {
	  width = (request->request_mode & CWWidth) ? request->width :
	       w->core.width;
	  height = (request->request_mode & CWHeight) ? request->height :
	       w->core.height;
	  if (RESIZE_UNLOCKED(dad)) {
	       LOCK_RESIZE(dad);
	       res = (*(dad->term.layout_kids))(dad, w, width, height);
	       UNLOCK_RESIZE(dad);
	  }
     }
     return res;
}

/* Our default layout manager. Assumes very simplistic environment */
Local XtGeometryResult
DefLayout(self, child, width, height)
TermWidget self;
Widget child;
Dimension width, height;
{
     Arg args[5];
     int i;
     XtGeometryResult result = XtGeometryYes;

     if (self->composite.num_children > 1) {
	  warn("Term widget needs a layout procedure for multiple children!");
	  result = XtGeometryNo;
     }
     else if (!XtIsRealized((Widget)self))
	  result = XtGeometryNo;
     else {
	  if (child) {
	       /* We need to resize ourselves */
	       i = 0;
	       XtSetArg(args[i], XtNwidth, width);	i++;
	       XtSetArg(args[i], XtNheight, height);	i++;
	       XtSetValues((Widget)self, args, i);
	  }
          XtResizeWidget(child, width, height, 0);
     }
     return result;
}

/*
 * Perhaps at some point I'll make both XpTermDoRop() and
 * XpTermDispatchRequest() varargs routines that also let you specify
 * a list of register/value pairs. The appropriate registers would then
 * be set before making the request. That would be very handy.
 */
Export Inline void
XpTermDoRop(w, rop)
TermWidget w;
int rop;
{
     cb_opcode(w->term.cb) = rop;
     rparse(w, w->term.cb);
}

Export Inline void
XpTermDispatchRequest(w, req)
TermWidget w;
int req;
{
     cb_opcode(w->term.cb) = req;
     dispatch(w, w->term.cb);
}

Export Inline void
XpTermInsertText(w, str, len)
TermWidget w;
String str;
int len;
{
     register ComBlockPtr tmp = w->term.cb;

     strncpy(cb_buffer(tmp), str, len);
     cb_nbytes(tmp) = len;
     cb_opcode(tmp) = OP_INSERT;
     dispatch(w, tmp);
}

Export Inline void
XpTermSetRegister(w, reg, type, val)
register TermWidget w;
register unsigned char reg;
int type;
caddr_t val;
{
     if (reg < 0 || reg > CB_NREGS)
	  warn("XpTermSetRegister: Register %d is out of range.", reg);
     else {
	  cb_reg_type(w->term.cb, reg) = type;
	  cb_reg_data(w->term.cb, reg) = (Generic)val;
     }
}

Export Inline caddr_t
XpTermGetRegister(w, reg, type)
register TermWidget w;
register unsigned char reg;
int *type;
{
     caddr_t rval = NULL;

     if (reg < 0 || reg > CB_NREGS)
	  warn("XpTermGetRegister: Register %d is out of range.", reg);
     else {
	  if (type)
	       *type = cb_reg_type(w->term.cb, reg);
	  rval = (caddr_t)cb_reg_data(w->term.cb, reg);
     }
     return rval;
}

/*
 * Install a new emulation type. Tight now, this assumes that you
 * no longer want the old one, but this may change.
 */
Export Boolean
XpTermDoEmulation (w, emulation)
TermWidget w;
String emulation;
{
     String old = w->term.term_type;

     parser_remove(w, old);
     if (!parser_install(w, emulation)) {
	  warn("Couldn't find a parser for term type '%s'", emulation);
	  if (!parser_install(w, old))
	       fatal("Couldn't reinstall old %s parser!", old);
	  else
	       return FALSE;
     }
     else
	  return TRUE;
}

/* Return a pointer to an term widget's common block as a generic pointer */
Export Inline caddr_t
XpTermComBlock(w)
TermWidget w;
{
     /*
      * We can't return this pointer as its actual type since clients of
      * this routine won't know anything about the ComBlock type; it's
      * opaque to applications. The purpose of this routine is only to
      * return a "handle" which will get passed along to another widget
      * (like the canvas) that knows how to cast it back into something
      * appropriate.
      */
     return (caddr_t)w->term.cb;
}

/*
 * Routines to associate widgets with pids follow. Since the toolkit has no
 * way of dealing with signals, and I can't use Xlib's context mechanism
 * without a globally open display (and I'm a widget, so that's a no-go),
 * I have to keep track of such things myself.
 */
Export void
XpNrememberWidget(w)
TermWidget w;
{
     PTable *tmp = Fresh(PTable);

     if (!termClassRec.term_class.procs)
	  termClassRec.term_class.procs = tmp;
     else {
	  tmp->next = termClassRec.term_class.procs;
	  termClassRec.term_class.procs = tmp;
     }
     tmp->w = w;
}

Export TermWidget
XpNfindWidgetByPid(pid)
int pid;
{
     PTable *tmp = termClassRec.term_class.procs;

     while (tmp) {
	  if (tmp->w->term.pid == pid)
	       return tmp->w;
	  else
	       tmp = tmp->next;
     }
     return NULL;
}

Export void
XpNremoveRememberedWidget(w)
TermWidget w;
{
     PTable *tmp = termClassRec.term_class.procs;

     if (tmp->w == w) {
	  termClassRec.term_class.procs = tmp->next;
	  XtFree((char *)tmp);
     }
     else while (tmp->next) {
	  if (tmp->next->w == w) {
	       PTable *save = tmp->next;

	       tmp->next = tmp->next->next;
	       XtFree((char *)save);
	       break;
	  }
	  else
	       tmp = tmp->next;
     }
}
