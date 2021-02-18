#ifndef lint
static char *rcsid = "$Header: /usr3/emu/canvas/RCS/TermCanvas.c,v 1.19 91/08/13 17:10:19 jkh Exp Locker: me $";
#endif
/*
 * This file is part of the PCS emu system.
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
 * Routines implementing the TermCanvas Widget used in emu
 *
 * Author: Michael Elbel
 * Date: March 20th, 1990.
 * Description: Public and private routines for the TermCanvas Widget
 *
 * Revision History:
 *
 * $Log:	TermCanvas.c,v $
 * Revision 1.19  91/08/13  17:10:19  jkh
 * Zero'ing a few more fields.
 * 
 * Revision 1.18  91/08/12  20:55:41  jkh
 * Whatever weenie did.
 * 
 * Revision 1.17  91/02/13  17:05:16  me
 * Corrected loading of 'XtDefaultFont'
 * 
 * Revision 1.16  90/10/25  11:52:19  me
 * *** empty log message ***
 * 
 * Revision 1.15  90/10/25  11:27:55  me
 * Corrected the default translations
 * 
 * Revision 1.14  90/10/25  11:27:12  me
 * Jordan's changes
 * 
 * Revision 1.13  90/10/12  12:40:21  me
 * checkmark
 * 
 * Revision 1.12  90/08/31  19:51:21  me
 * Checkpoint vor dem Urlaub
 * 
 * Revision 1.11  90/08/23  16:20:47  me
 * because
 * 
 * Revision 1.10  90/08/09  10:15:25  me
 * changed the names of the selection action to look like the ones of xterm,
 * added a call to w->term.notify_first_map when we get the first expose
 * event
 * 
 * Revision 1.9  90/07/26  02:33:53  jkh
 * Added new copyright.
 * 
 * Revision 1.8  90/07/18  16:32:23  me
 * checkpoint
 * 
 * Revision 1.7  90/06/13  11:42:05  me
 * *** empty log message ***
 * 
 * Revision 1.6  90/06/08  16:50:26  me
 * Checkpoint
 * 
 * Revision 1.5  90/06/07  15:33:27  me
 * went back to passing Com Blocks, Widgetized the contact Routine's name
 * 
 * Revision 1.4  90/05/11  14:19:32  me
 * Resizing now handled
 * 
 * Revision 1.3  90/05/08  17:32:11  me
 * Tab initialization added.
 * 
 * Revision 1.2  90/05/08  14:34:06  me
 * Jordan's infamous changes
 * 
 * Revision 1.1  90/04/12  14:26:11  me
 * Initial revision
 * 
 *
 */

#include "canvas.h"
/* #include <X11/Quarks.h> */

#define offset(field) XtOffset(TermCanvasWidget, term.field)

/*
 * I'm using "black" and "white instead of XtDefaultForeground and
 * XtDefaultBackground because I have no way to detect where the color
 * comes from in the widget and the toolkit automatically switches
 * XtdefaultForeground and XtDefaultBackground if *reverseVideo is on
 */

Local XtResource resources[] = {
     { XtNforeground,	XtCForeground,	XtRPixel,	sizeof(Pixel),
	    offset(text_color),		XtRString,	"black"		},
     { XtNbackground,	XtCBackground,	XtRPixel,	sizeof(Pixel),
	    offset(background_color),	XtRString,	"white"		},
     { XpNcursorFg,	XpCCursorFg,	XtRPixel,	sizeof(Pixel),
	    offset(cursor_fg),		XtRString,	"black"		},
     { XpNcursorBg,	XpCCursorBg,	XtRPixel,	sizeof(Pixel),
	    offset(cursor_bg),		XtRString,	"white"		},
     { XtNreverseVideo,	XtCReverseVideo,XtRBoolean,	sizeof(Boolean),
	    offset(reverse_mode),	XtRString,	"False"		},
     { XtNfont,		XtCFont,	XtRString,	sizeof(String),
	    offset(normal_font_n),	XtRString,	XtDefaultFont	},
     { XpNboldFont,	XpCBoldFont,	XtRString,	sizeof(String),
	    offset(bold_font_n),	XtRString,	XtDefaultFont	},
     { "defaultFont",	"DefaultFont",	XtRString,	sizeof(String),
	    offset(default_font_n),	XtRString,	"fixed"		},
     { XpNunderlineWidth,XpCUnderlineWidth,XtRDimension,sizeof(Dimension),
	    offset(ul_width),		XtRString,	"1"		},
     { XpNlines,	XpCLines,	XtRInt,		sizeof(int),
	    offset(lines),		XtRString,	"24"		},
     { XpNcolumns,	XpCColumns,	XtRDimension,	sizeof(Dimension),
	    offset(columns),		XtRString,	"80"		},
     { XpNcellWidth,	XpCCellWidth,	XtRDimension,	sizeof(Dimension),
	    offset(cell_width),		XtRString,	"0"		},
     { XpNcellHeight,	XpCCellHeight,	XtRDimension,	sizeof(Dimension),
	    offset(cell_height),	XtRString,	"0"		},
     { XpNcursorHeight,	XpCCursorHeight,XtRDimension,	sizeof(Dimension),
	    offset(cursor_height),	XtRString,	"0"		},
     { XpNcursorWidth,	XpCCursorWidth,	XtRDimension,	sizeof(Dimension),
	    offset(cursor_width),	XtRString,	"0"		},
     { XpNcursorBlinking, XpCCursorBlinking,XtRBoolean,	sizeof(Boolean),
	    offset(cursor_blinking),	XtRString,	"True"		},
     { XpNblinkInterval,XpCBlinkInterval,XtRInt,	sizeof(int),
	    offset(blink_interval), 	XtRString,	"500"		},
     { XpNblinkWOFocus,	XpCBlinkWOFocus,XtRBoolean,	sizeof(Boolean),
	    offset(blink_wo_focus), 	XtRString,	"False"		},
     { XpNtextBlinkInterval,XpCTextBlinkInterval,XtRInt,sizeof(int),
	    offset(tblink_interval),	XtRString, 	"500"		},
     { XpNwrapAround,	XpCWrapAround,	XtRBoolean,	sizeof(Boolean),
	    offset(wrap_around),	XtRString,	"True"		},
     { XpNinsertMode,	XpCInsertMode,	XtRBoolean,	sizeof(Boolean),
	    offset(insert_mode),	XtRString,	"False"		},
     { XpNbellVolume,	XpCBellVolume,	XtRDimension,	sizeof(Dimension),
	    offset(bell_volume), 	XtRString,	"100"		},
     { XpNdefTabWidth,	XpCDefTabWidth,	XtRDimension,	sizeof(Dimension),
	    offset(def_tab_width), 	XtRString,	"8"		},
     { XpNtermType,	XpCTermType,	XtRString,	sizeof(String),
	    offset(term_type),	XtRImmediate,		NULL		},
     { XpNsetSize,	XpCSetSize,	XtRFunction,	sizeof(VoidFuncPtr),
	    offset(set_size),		XtRImmediate,	NULL		},
     { XpNoutput,	XpCOutput,	XtRFunction,	sizeof(VoidFuncPtr),
	    offset(output),		XtRImmediate,	NULL		},
     { XpNnotifyFirstMap,XpCNotifyFirstMap,XtRFunction,	sizeof(VoidFuncPtr),
	    offset(notify_first_map),	XtRImmediate,	NULL		},
     { XpNcomBlock,	XpCComBlock,	XtRPointer,	sizeof(ComBlockPtr),
	    offset(com_block),		XtRImmediate,	NULL		},
     { XpNselectionInverse,XpCSelectionInverse,XtRBoolean,sizeof(Boolean),
	    offset(selection_inv), 	XtRString,	"False"		},
     { XpNpointerShape,	XpCPointerShape,XtRCursor,	sizeof(Cursor),
	    offset(pointer),		XtRString,	"xterm"		},
     { XpNpointerColor, XpCPointerColor,XtRString,	sizeof(String),
	    offset(pointer_color),	XtRString,	"black"		},
     { XpNsaveLines,	XpCSaveLines,	XtRInt,		sizeof(int),
	    offset(save_lines),		XtRString,	"-1"		},
     { XpNadjustScrollBar,XpCAdjustScrollBar,XtRFunction,sizeof(VoidFuncPtr),
	    offset(adjust_scroll_bar),	XtRImmediate,	NULL		},
     { XpNmultiClickTime,XpCMultiClickTime,XtRInt,	sizeof(int),
	    offset(multi_click_time),	XtRString,	"300"		},
};
#undef offset

Local void focus_in_handler(), focus_out_handler(),
     key_input_handler(), call_parser(), call_canvas();


Local XtActionsRec actions[] =
{
     { "focus-in",		focus_in_handler	},
     { "focus-out",		focus_out_handler	},
     { "key-input",		key_input_handler	},
     { "select-start",		start_selection		},
     { "select-extend",		extend_selection	},
     { "select-end",		end_selection		},
     { "insert-selection",	paste_selection		},
     { "call-parser",		call_parser		},
     { "call-canvas",		call_canvas		},
     { NULL,			NULL			}
};

Local char translations[] = "\
<FocusIn>:		focus-in()\n\
<FocusOut>:		focus-out()\n\
<Btn1Down>:		select-start()\n\
<Btn1Motion>:		select-extend()\n\
<Btn1Up>:		select-end(PRIMARY, CUT_BUFFER0)\n\
<Btn3Down>:		select-extend()\n\
<Btn3Motion>:		select-extend()\n\
<Btn3Up>:		select-end(PRIMARY, CUT_BUFFER0)\n\
<Btn2Up>:		insert-selection(PRIMARY, CUT_BUFFER0)\n\
Shift <Key>Prior:	call-canvas(46, \"\", a, 200, b, 1)\n\
Shift <Key>Next:	call-canvas(46, \"\", a, -200, b, 1)\n\
Shift <Key>Up:		call-canvas(46, \"\", a, 1, b, 1)\n\
Shift <Key>Down:	call-canvas(46, \"\", a, -1, b, 1)\n\
<Key>:			key-input()\n\
";

/* 46 is OP_SCROLL_SCREEN_RELATIVE */

Local void Initialize();
Local void Realize();
Local void Canvas_Expose();
Local void Resize();
Local void Destroy();

Export TermCanvasClassRec termCanvasClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &coreClassRec,
    /* class_name		*/	"TermCanvas",
    /* widget_size		*/	sizeof(TermCanvasRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	XtExposeCompressMaximal |
	                                XtExposeGraphicsExposeMerged,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	Canvas_Expose,
    /* set_values		*/	NULL,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* TermCanvas fields */
    /* empty			*/	0
  }
};

Export WidgetClass termCanvasWidgetClass = (WidgetClass)&termCanvasClassRec;

/*
 * Initialize Routine, gets called upon Creation of the Widget
 */
Local void
Initialize(request, new)
Widget request;
Widget new;
{
     TermCanvasWidget w = (TermCanvasWidget)new;
     XFontStruct *fn;
     XFontStruct *fb;

     /* Initialize the Char Array */
     w->term.char_array = create_char_array(w->term.lines, w->term.columns);
     w->term.array_cur.lin = w->term.array_cur.col = 0;

     /* Initialize the Save Array */
     /* Default size ?? */
     if (w->term.save_lines == -1)
	  w->term.save_lines = 64 > w->term.lines ? 64 : w->term.lines;
     w->term.save_array = (CharArray)XtMalloc(w->term.save_lines *
					      sizeof(Line));
     bzero(w->term.save_array, w->term.save_lines * sizeof(Line));
     
     w->term.save_size = 0;

     /* Initialize the scroll position and length */
     w->term.scroll_pos = 0;
     
     /* Initialize the Tab List */
     w->term.tabs = create_tab_list(w->term.columns);
     clear_tab_list(w->term.tabs, w->term.columns);
     init_fixed_tabs(w->term.tabs, w->term.columns, w->term.def_tab_width);

     /* Initialize the flut and saved flut */
     set_flut(w->term.flut, 0, 0, 256);
     set_flut(w->term.save_flut, 0, 0, 256);

     /*
      * Initialize the Fonts.
      *
      * I could have used converters, but since the Toolkit
      * Documentation doesn't specify what happens with the
      * allocated FontStructs, I can't rely on the current
      * behaviour.
      * So i'm partly reproducing functionality from 'CvtStringToFontStruct'
      * in lib/Xt/Converters.c.
      */
     
     /* Check for XtDefaultFont */
     if (strcmp(w->term.normal_font_n, XtDefaultFont) == 0) {
	  w->term.normal_font_n = w->term.default_font_n;
     }
     
     /* Now load them */
     if ((fn = XLoadQueryFont(XtDisplay(w), w->term.normal_font_n)) == NULL)
	  fatal("Couldn't even load the normal Font '%s'",
		w->term.normal_font_n);
     if ((fb = XLoadQueryFont(XtDisplay(w), w->term.bold_font_n)) == NULL) {
	  warn("Couldn't load the bold Font, using the normal one");
	  fb = fn;
     }
     
     w->term.normal_font = fn;
     w->term.bold_font = fb;
     
     if (fn->max_bounds.width != fn->min_bounds.width)
	  warn("Normal font is not of fixed width!");
     
     w->term.cell_width = fn->max_bounds.width;
     w->term.cell_height = fn->ascent + fn->descent;
     w->term.font_descent = fn->descent;
     
     if (fb->max_bounds.width != fb->min_bounds.width)
	  warn("Bold font is not of fixed width!");

     if (fn->max_bounds.width != fb->max_bounds.width)
	  warn("Normal and bold fonts are of different width (%d vs. %d)",
	       fn->max_bounds.width, fb->max_bounds.width);
     if ((fn->ascent + fn->descent) != (fb->ascent + fb->descent))
	  warn("Normal and bold fonts are of different height (%d vs. %d)",
	       fn->ascent + fn->descent, fb->ascent + fb->descent);
     if (fn->descent != fb->descent)
	  warn("Normal and bold fonts have different descent (%d vs. %d)",
	       fn->descent, fb->descent);
     
     /* Initialize the Cursor Variables */
     w->term.screen_cur.x = 0;
     w->term.screen_cur.y = w->term.cell_height;
     
     /* set up the scrolling region */
     w->term.scroll_top = 0;
     if (w->term.lines > 0) {
	  w->term.scroll_bottom = w->term.lines - 1;
     }
     else {
	  w->term.scroll_bottom = 0;
     }

     /* cursor positioning abolute */
     w->term.rel_cursor_pos = False;

     /* we don't own any selections yet */
     w->term.n_sel_atoms = w->term.n_sel_size = 0;
     w->term.sel_atoms = NULL;
     
     w->term.select_on = False;
     w->term.select_on = False;
     w->term.sel_start_x = w->term.sel_start_y = 0;
     w->term.sel_end_x = w->term.sel_end_y = 0;

     /* we haven't been mapped yet */
     w->term.mapped_yet = False;

     /* Check if we have no size yet */
     if (w->core.width == 0)
	  w->core.width = w->term.columns * w->term.cell_width;
     if (w->core.height == 0)
	  w->core.height = w->term.lines * w->term.cell_height;

     /* Initialize the text blink TimeoutId */
     w->term.blink_id = 0;

     /* Initialize the selection text */
     w->term.selection_text = NULL;
     
     /* Initialize the motion head */
     w->term.motionHead = NULL;

     /* Initialize the current character under the cursor */
     w->term.curs_char.value = 0;
}

/*
 * Realize Routine, gets called when the widget is realized
 */
Local void
Realize(widget, value_mask, attributes)
Widget		 widget;
Mask		 *value_mask;
XSetWindowAttributes *attributes;
{
     void blink_cursor();
     
     register TermCanvasWidget w = (TermCanvasWidget)widget;
     XGCValues val;

     Arg args[5];
     int n;
     int i;
     
     Pixel fg, bg, cfg, cbg;
     
     /* Set up the mouse pointer */
     attributes->cursor = w->term.pointer;
     *value_mask |= CWCursor;
     
     /*
      * This didn't work, do it directly.
      *
      * w->term.color_cells = DisplayCells(XtDisplay(w), XtScreen(w));
      */
     w->term.color_cells = XtScreen(w)->root_visual->map_entries;
     
     /* Initialize the colors */
     w->term.window_fg.pixel = w->term.text_color;
     XQueryColor(XtDisplay(w), w->core.colormap, &(w->term.window_fg));
     w->term.window_bg.pixel = w->term.background_color;
     XQueryColor(XtDisplay(w), w->core.colormap, &(w->term.window_bg));
     
     /*
      * If we're on a black and white screen, it makes no sense to
      * set the pointer color, just ignore it.
      */
     if (w->term.color_cells > 2)  {
	  if (XParseColor(XtDisplay(w), w->core.colormap,
			  w->term.pointer_color, &(w->term.pointer_fg)) != 1) {
	       warn("Can't find pointer color \"%s\", using text color",
		    w->term.pointer_color);
	       w->term.pointer_fg = w->term.window_fg;
	  }
	  
	  /*
	   * If the pointer Foreground is the same as the window
	   * foreground we have to be careful when switching to
	   * reverse video (Or get an invisible pointer) on color
	   * displays.
	   */
	  w->term.pt_fg_eq_wd_fg =
	       (w->term.window_fg.red == w->term.pointer_fg.red) &&
	       (w->term.window_fg.green == w->term.pointer_fg.green) &&
	       (w->term.window_fg.blue == w->term.pointer_fg.blue);
     }
     else {
	  w->term.pointer_fg = w->term.window_fg;
     }

 
     if (w->term.reverse_mode) {
	  fg = w->term.background_color;
	  bg = w->term.text_color;
	  w->term.background_color = bg;
	  w->term.text_color = fg;
	  
	  /* Set the mouse cursor */
	  if ((w->term.color_cells > 2) && !w->term.pt_fg_eq_wd_fg) {
	       XRecolorCursor(XtDisplay(w), w->term.pointer,
			      &(w->term.pointer_fg), &(w->term.window_fg));
	  }
	  else {
	       XRecolorCursor(XtDisplay(w), w->term.pointer,
			      &(w->term.window_bg), &(w->term.window_fg));
	  }
     }
     else {
	  fg = w->term.text_color;
	  bg = w->term.background_color;
	  
	  /* Change the cursor */
	  XRecolorCursor(XtDisplay(w), w->term.pointer,
			 &(w->term.pointer_fg), &(w->term.window_bg));
     }
     
     /*
      * Set the background pixel, so XtCreateWindow uses the right
      * color.
      */
     n = 0;
     XtSetArg(args[n], XtNbackground, bg);
     n++;
     XtSetValues(widget, args, n);

     /*
      * The text cursor
      *
      * If we're on a black and white display default to foreground
      * and background colors respectively.
      */
     if (w->term.color_cells > 2) {
	  if (w->term.cursor_fg == bg) {
	       cfg = w->term.cursor_bg;
	       cbg = w->term.cursor_fg;
	  }
	  else {
	       cfg = w->term.cursor_fg;
	       cbg = w->term.cursor_bg;
	  }
     }
     else {
	  w->term.cursor_fg = cfg = bg;
	  w->term.cursor_bg = cbg = fg;
     }
     
     /*
      * Initialize the cit,
      */ 
     for (i = 1; i < 16; i++) {
	  w->term.cit[i].fg.set = False;
	  w->term.cit[i].bg.set = False;
     }
     
     /*
      * Set up entry 0 of the to be the foreground and background
      * so emu will work without somebody explicitly setting any
      * colors
      */
     w->term.cit[0].fg.pix = fg;
     w->term.cit[0].bg.pix = bg;
     w->term.cit[0].fg.set = True;
     w->term.cit[0].bg.set = True;
     
     XtCreateWindow((Widget)w, (unsigned int)InputOutput,
		    (Visual *)CopyFromParent, *value_mask, attributes);

     /* create the text GCs */
     /* first normal Text */
     val.foreground = fg;
     val.background = bg;
     
     val.line_width = w->term.ul_width;
     val.function = GXcopy;
     val.font = w->term.normal_font->fid;
     
     w->term.norm_gc = XCreateGC(XtDisplay(w), XtWindow(w),
				 GCFunction | GCForeground | GCBackground
				 | GCFont | GCLineWidth,
				 &val);
     
     /* then special text, let's initialize it to bold */
     val.font = w->term.bold_font->fid;
     
     w->term.spec_gc = XCreateGC(XtDisplay(w), XtWindow(w),
				 GCFunction | GCForeground | GCBackground
				 | GCFont | GCLineWidth,
		&val);
     
     /* set the old_attribs accordingly */
     w->term.old_attribs = ATT_BOLD;
     
     /* clear the current and cursor attributes */
     w->term.act_attribs = w->term.curs_char.attributes = 0;
     w->term.gc = w->term.norm_gc;

     /* clear current and saved color */
     w->term.act_color = 0;
     w->term.old_color = 0;
     
     /* create the clearing GC */
     val.foreground = bg;
	  
     w->term.clear_gc = XCreateGC(XtDisplay(w), XtWindow(w),
				  GCFunction|GCForeground|GCBackground,
				  &val);
	  
     /* create Cursor GCs */
     val.font = w->term.normal_font->fid;
     
     val.foreground = cfg;
     val.background = cbg;
     w->term.cursor_graph_gc = XCreateGC(XtDisplay(w), XtWindow(w),
				 GCFunction|GCForeground|GCBackground|GCFont,
					 &val);
     val.foreground = cbg;
     val.background = cfg;
     w->term.cursor_text_gc = XCreateGC(XtDisplay(w), XtWindow(w),
				GCFunction|GCForeground|GCBackground|GCFont,
					&val);
     
     validate_cursor_size(w);
     
     val.foreground = fg;
     val.background = bg;
     w->term.cursor_rem_gc = XCreateGC(XtDisplay(w), XtWindow(w),
				GCFunction|GCForeground|GCBackground|GCFont,
				       &val);

     /* have the cursor be drawn with the first expose event */
     w->term.cursor_visible = True;
     w->term.cursor_on = False;

     /* start the blinking cursor */
     if (w->term.cursor_blinking)
	  w->term.timeout_id = XtAddTimeOut(w->term.blink_interval,
					    blink_cursor, w);
     
     /* blinking text should be visible */
     w->term.blink_text_on = True;

     /* if the ComBlockPtr isn't set yet allocate our own */
     if (w->term.com_block == NULL) {
	  warn("TermCanvas: No ComBlockPtr supplied, creating my own");
	  w->term.com_block = (ComBlockPtr)(XtMalloc(sizeof(ComBlock)));
	  w->term.com_block_alloc = True;
     }
     else 
     {
	  w->term.com_block_alloc = False;
     }
     
#if 0
     /* Make an initial Resize Request */
     XtMakeResizeRequest(w, (Dimension)(w->term.cell_width * w->term.columns),
			 (Dimension)(w->term.cell_height * w->term.lines),
			 NULL, NULL);
#endif
}

/*
 * Resize Handler, will get called by the Intrinsics
 */
Local void
Resize(widget)
Widget widget;
{
     TermCanvasWidget w = (TermCanvasWidget)widget;
     Dimension lines, columns;

     lines = w->core.height / w->term.cell_height;
     columns = w->core.width / w->term.cell_width;
     
#ifdef DEBUG
     debug("canvas resize: Resizing to %d rows %d columns (%dx%d)",
	   lines, columns, w->core.width, w->core.height);
#endif
     
     if (w->term.set_size != NULL)
	  w->term.set_size(XtParent(w), lines, columns, w->core.width,
			   w->core.height);
     
     if ((lines == w->term.lines) && (columns == w->term.columns))
	  return;
     
     /*
      * Unmark the selection before resizing the char array to avoid
      * trouble with the coordinates.
      */
     if (w->term.select_on)
	  unmark_selection(w);
     
     w->term.char_array = resize_char_array(w, w->term.char_array,
					    w->term.lines, w->term.columns,
					    lines, columns);

     resize_save_area(w, w->term.columns, columns);
     
     w->term.tabs = resize_tab_list(w->term.tabs, w->term.columns, columns);

     /*
      * If the cursor would be beyond the end of the screen put it to the
      * border
      */
     if ((lines > 0) && (w->term.array_cur.lin >= lines)) {
	  w->term.array_cur.lin = lines - 1;
	  w->term.screen_cur.y = lines * w->term.cell_height;
     }
     if ((columns > 0) && (w->term.array_cur.col >= columns)) {
	  w->term.array_cur.col = columns - 1;
	  w->term.screen_cur.x = (columns - 1) * w->term.cell_width;
     }
     
     /*
      * Clear the screen and redraw everything, but only if we are
      * already realized.
      */
     /*
     if (XtIsRealized(w)) {
	  clear_whole_screen(w);
	  redraw_rect(w, w->term.char_array, 0, 0, lines - 1, columns - 1);
	  flip_cursor_on(w);
     }
     */

     /*
      * If the scrolling region was at the bottom of the screen
      * put it to the new bottom.
      */
     if ((w->term.scroll_bottom == w->term.lines - 1) ||
	 (w->term.lines == 0)) {
	  w->term.scroll_bottom = lines - 1;
     }
     if (w->term.scroll_top >= lines) {
	  w->term.scroll_top = lines - 1;
     }

     /* Set the new lines and columns */
     w->term.lines = lines;
     w->term.columns = columns;
}

/*
 * Destroy Procedure, Frees up allocated stuff and stops the blinking
 * cursor.
 */
Local void
Destroy(w)
register TermCanvasWidget w;
{
     if (w->term.timeout_id != 0) {
	  XtRemoveTimeOut(w->term.timeout_id);
	  w->term.timeout_id = 0;
     }
     
     /* Free the current array */
     free_char_array(w->term.char_array, w->term.lines);

     /* Free the save_array */
     free_char_array(w->term.save_array, w->term.save_lines);

     /* Free the tab list */
     XtFree(w->term.tabs);

     /* Destroy the ComBlock, if we have allocated it ourselves */
     if (w->term.com_block_alloc)
	  XtFree((caddr_t)w->term.com_block);

     /* Free the GCs */
     XFreeGC(XtDisplay(w), w->term.norm_gc);
     XFreeGC(XtDisplay(w), w->term.spec_gc);
     XFreeGC(XtDisplay(w), w->term.cursor_graph_gc);
     XFreeGC(XtDisplay(w), w->term.cursor_text_gc);
     XFreeGC(XtDisplay(w), w->term.cursor_rem_gc);
     XFreeGC(XtDisplay(w), w->term.clear_gc);

     /* Free the MotionList stuff */
     canvasFreeMotions(w);
}

/*
 * Expose Handler. Will get called by the Intrinsics
 */
Local void
Canvas_Expose(widget, event, region)
Widget widget;
XEvent *event;
Region region;
{
     TermCanvasWidget w = (TermCanvasWidget)widget;
     Position start_lin, start_col, end_lin, end_col;
     register int tmp;

     /* calculate the character Rectangle to redraw */
     tmp = event->xexpose.y;
     start_lin = tmp / w->term.cell_height;
     tmp += event->xexpose.height;
     end_lin = (tmp - 1) / w->term.cell_height;
     if (end_lin >= w->term.lines)
	  end_lin = w->term.lines - 1;
	  
     tmp = event->xexpose.x;
     start_col = tmp / w->term.cell_width;
     tmp += event->xexpose.width;
     end_col = (tmp - 1) / w->term.cell_width + 1;
     if (end_col >= w->term.columns)
	  end_col = w->term.columns - 1;

     /* do the redrawing */
     redraw_rect(w, w->term.char_array, start_lin, start_col,
		 end_lin, end_col);

     /* if the cursor was inside the rectangle and is visible, draw it */
     if (w->term.cursor_visible
	 && ((w->term.array_cur.col >= start_col)
	     && (w->term.array_cur.col <= end_col))
	 && ((w->term.array_cur.lin >= start_lin)
	     && (w->term.array_cur.lin <= end_col)))
	  flip_cursor_on(w);
     
     /* if necessary call the notify_first_map routine */
     if (!w->term.mapped_yet) {
	  if (w->term.notify_first_map != NULL)
	       w->term.notify_first_map(XtParent(w));
	  w->term.mapped_yet = True;
     }
}

/* Routine to blink the cursor - gets called via XtAddTimeOut */
Export void
blink_cursor(w, id)
TermCanvasWidget w;
XtIntervalId id;
{
     if (w->term.cursor_visible) {
	  flip_cursor_off(w);
	  w->term.cursor_visible = False;
     }
     else {
	  w->term.cursor_visible = True;
	  flip_cursor_on(w);
     }
     
     if (w->term.cursor_blinking)
	  w->term.timeout_id = XtAddTimeOut(w->term.blink_interval,
					    blink_cursor, w);
}

/* Generic Action Routines start here */

/* Routine that gets called when the Widget Loses the Input Focus */
Local void
focus_out_handler(widget, event, params, num_params)
Widget widget;
XEvent *event;
String *params;
Cardinal *num_params;
{
     TermCanvasWidget w = (TermCanvasWidget)widget;

     if (w->term.focused) {
	  if (XtIsRealized((Widget)w)) {
	       if (w->term.cursor_visible)
		    flip_cursor_off(w);
	       w->term.outline_cursor = True;
	       
	       if (!w->term.blink_wo_focus && w->term.cursor_blinking) {
		    if (w->term.timeout_id != 0) {
			 XtRemoveTimeOut(w->term.timeout_id);
			 w->term.timeout_id = 0;
		    }
		    
		    w->term.cursor_visible = True;
		    flip_cursor_on(w);	/* turn it on anyways */
	       } else {
		    if (w->term.cursor_visible)
			 flip_cursor_on(w);
	       }
	  } else {
	       w->term.outline_cursor = True;
	  }
	  w->term.focused = False;
     }
}
			 
/* Routine that gets called when the Widget gets the Input Focus */
Local void
focus_in_handler(widget, event, params, num_params)
Widget widget;
XEvent *event;
String *params;
Cardinal *num_params;
{
     TermCanvasWidget w = (TermCanvasWidget)widget;

     if (!w->term.focused) {
	  if (XtIsRealized(widget)) {
	       flip_cursor_off(w);
	       w->term.outline_cursor = False;
	       
	       if (!w->term.blink_wo_focus && w->term.cursor_blinking)
		    if (w->term.timeout_id == 0)
			 w->term.timeout_id =
			      XtAddTimeOut(w->term.blink_interval, 
					   blink_cursor, w);
	       
	       flip_cursor_on(w);
	  } else
	       w->term.outline_cursor = False;
	  w->term.focused = True;
     }
}

			 
/*
 * Generic key input handling Routine.
 * Called without parameters it will translate the event into a Latin-1
 * String and call the reverse parser.
 * If mod1 is pressed during the event, the resulting string's high bits
 * get set to indicate a pressed Meta Key (we assume that Meta is bound
 * to mod1.
 * 
 * If a parameter is supplied this string is used instead.
 */
Local void
key_input_handler(widget, event, params, num_params)
Widget widget;
XEvent *event;
String *params;
Cardinal *num_params;
{
     register TermCanvasWidget w = (TermCanvasWidget)widget;
     register ComBlockPtr block = w->term.com_block;
     
     if (*num_params == 0) { /* look up the event */
	  unsigned char buffer[100];
	  int i, j;
	  KeySym keysym;

	  if ((i = XLookupString (&event->xkey, (char *)buffer, 100,
				  &keysym, NULL))
	      != 0) {
	       if ((event->xkey.state & Mod1Mask) != 0) {
		    /* set the high bits */
		    for (j = 0; j < i; j++) {
			 buffer[j] |= (unsigned char)0x80;
		    }
	       }
	       cb_opcode(block) = OP_INSERT;
	       cb_nbytes(block) = i;
	       strcpy(cb_buffer(block), buffer);
	       if (w->term.output != NULL) {
		    w->term.output(XtParent(w), block);
	       }
	  }
     }
     else {
	  cb_opcode(block) = OP_INSERT;
	  cb_nbytes(block) = strlen(params[0]);
	  strcpy(cb_buffer(block), (unsigned char *)params[0]);
	  if (w->term.output != NULL) {
	       w->term.output(XtParent(w), block);
	  }
     }
}

/*
 * Action Routine to call the reverse parser directly.
 *
 * The calling format is:
 *
 * call-parser(<opcode>, <buffer>, [<Register>, <int_val>], ...)
 * 	<opcode> is the opcode to call,
 * 	<buffer> the value for the comblock_buffer
 * 	<Register> <int_val> Pairs specify which Registers to set
 * 		with <Register> being the uppercase Letter for the Register,
 * 		<int_val> being the Value to put int that Reg.
 *
 * 	e.g. a translation of "call-parser (5, foobar, A, 12, B, 20)
 * 	will call the reverse parser with the opcode 5, string "foobar"
 * 	in the buffer and Registers A and B of the comblock set to 12 and 20.
 *
 * Note: No Error checking on the Register Names and values is done.
 */
Local void
call_parser(widget, event, params, num_params)
Widget widget;
XEvent *event;
String *params;
Cardinal *num_params;
{
     register TermCanvasWidget w = (TermCanvasWidget)widget;
     register ComBlockPtr block = w->term.com_block;
     
     Import int atoi();
     register int i,reg;
     register int np = *num_params;

     if (np < 2) {
	  warn("action 'call-parser' called with too few arguments (%d)", np);
	  return;
     }

     cb_opcode(block) = atoi(params[0]);
     strcpy(cb_buffer(block), (unsigned char *)params[1]);
     cb_nbytes(block) = strlen(params[1]);
     
     params += 2;
     np -=2;

     for (i = 0; i < (np - 1); i += 2) {
	  reg = *params[i] & 0x7f;
	  
	  cb_reg_data(block, reg) = (Generic)atoi(params[i+1]);
	  cb_reg_type(block, reg) = CB_INT_TYPE;
     }
     if (w->term.output != NULL) {
	  w->term.output(XtParent(w), block);
     }
}
     
/*
 * Action Routine to call the canvas directly.
 *
 * The calling format is:
 *
 * call-canvas(<opcode>, <buffer>, [<Register>, <int_val>], ...)
 *
 * For the exact parameter description see cal-parser
 */

Local void
call_canvas(widget, event, params, num_params)
Widget widget;
XEvent *event;
String *params;
Cardinal *num_params;
{
     ComBlock block;
     
     register int i,reg;
     register int np = *num_params;
     Import int atoi();

     if (np < 2) {
	  warn("action 'call-canvas' called with too few arguments (%d)", np);
	  return;
     }
     cb_opcode(&block) = atoi(params[0]);
     strcpy(cb_buffer(&block), (unsigned char *)params[1]);
     cb_nbytes(&block) = strlen(params[1]);
     
     params += 2;
     np -=2;

     for (i = 0; i < (np - 1); i += 2) {
	  reg = *params[i] & 0x7f;
	  
	  cb_reg_data(&block, reg) = (Generic)atoi(params[i+1]);
	  cb_reg_type(&block, reg) = CB_INT_TYPE;
     }
     /* Always put the cursor back */
     XpTermCanvasDispatch(widget, &block, True);
}

     
/*
 * Generic canvas dispatcher.
 * Requests of OP_NOP are just ignored.
 * The flag is used to determine whether the cursor should be put back
 */
Export int
XpTermCanvasDispatch(widget, blockptr, flag)
register Widget widget;
register ComBlockPtr blockptr;
register Boolean flag;
{
     register int opc = cb_opcode(blockptr);
     register int retval;
     
     if ((opc >= 0) && (opc < OP_NUM))
	  retval = ((*jumptable[opc])(widget, blockptr));
     else
	  if (opc != OP_NOP) {
	       warn("XpTermCanvasDispatch: Illegal opcode %d", opc);
	       retval = 99;
	  }

     if (flag)
	  flip_cursor_on(widget);
     return(retval);
}
