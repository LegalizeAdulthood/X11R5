/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 *
 * "Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty."
 *
 */

#include "fig.h"
#include "resources.h"
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "u_create.h"
#include "u_fonts.h"
#include "u_list.h"
#include "u_search.h"
#include "w_canvas.h"
#include "w_drawprim.h"
#include "w_mousefun.h"
#include "w_setup.h"

extern PIX_FONT lookfont();

#define	CTRL_H	8
#define	NL	10
#define	CR	13
#define	CTRL_U	21
#define	CTRL_X	24
#define	SP	32
#define	DEL	127

#define			BUF_SIZE	400

char            prefix[BUF_SIZE],	/* part of string left of mouse click */
                suffix[BUF_SIZE];	/* part to right of click */
int             leng_prefix, leng_suffix;
static int      char_ht, char_wid;
static int      base_x, base_y;

static PR_SIZE  tsize;
static PR_SIZE  ssize;

static int      work_psflag, work_font, work_fontsize, work_textjust;
static          finish_n_start();
static          init_text_input(), cancel_text_input();
static          wrap_up();
static          char_handler();
static F_text  *new_text();

static int cpy_n_char();
static int prefix_length();
static int initialize_char_handler();
static int terminate_char_handler();
static int erase_char_string();
static int draw_char_string();
static int turn_on_blinking_cursor();
static int turn_off_blinking_cursor();
static int move_blinking_cursor();

text_drawing_selected()
{
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_middlebut_proc = null_proc;
    canvas_leftbut_proc = init_text_input;
    canvas_rightbut_proc = null_proc;
    set_mousefun("posn cursor", "", "");
    set_cursor(&pencil_cursor);
}

static
finish_n_start(x, y)
{
    wrap_up();
    init_text_input(x, y);
}

finish_text_input()
{
    wrap_up();
    text_drawing_selected();
    draw_mousefun_canvas();
}

static
cancel_text_input()
{
    erase_char_string();
    terminate_char_handler();
    if (cur_t != NULL) {
	draw_text(cur_t, PAINT);
	toggle_textmarker(cur_t);
    }
    text_drawing_selected();
    draw_mousefun_canvas();
    reset_action_on();
}

static
new_text_line()
{
    wrap_up();
    cur_y += (int) ((float) char_ht * cur_textstep / 10.0);
    cur_x = base_x;
    init_text_input(cur_x, cur_y);
}

static
wrap_up()
{
    PR_SIZE         size;

    reset_action_on();
    erase_char_string();
    terminate_char_handler();

    if (cur_t == NULL) {	/* a brand new text */
	if (leng_prefix == 0)
	    return;
	cur_t = new_text();
	add_text(cur_t);
    } else {			/* existing text modified */
	strcat(prefix, suffix);
	leng_prefix += leng_suffix;
	if (leng_prefix == 0) {
	    delete_text(cur_t);
	    return;
	}
	if (!strcmp(cur_t->cstring, prefix)) {
	    /* we didn't change anything */
	    draw_text(cur_t, PAINT);
	    toggle_textmarker(cur_t);
	    return;
	}
	new_t = copy_text(cur_t);
	change_text(cur_t, new_t);
	if (strlen(new_t->cstring) >= leng_prefix) {
	    strcpy(new_t->cstring, prefix);
	} else {		/* free old and allocate new */
	    cfree(new_t->cstring);
	    if ((new_t->cstring = new_string(leng_prefix + 1)) != NULL)
		strcpy(new_t->cstring, prefix);
	}
	size = pf_textwidth(new_t->font, psfont_text(new_t), new_t->size,
			    leng_prefix, prefix);
	new_t->height = size.y;
	new_t->length = size.x;	/* in pixels */
	cur_t = new_t;
    }
    draw_text(cur_t, PAINT);
    toggle_textmarker(cur_t);
}

static
init_text_input(x, y)
    int             x, y;
{
    int             basx;

    cur_x = x;
    cur_y = y;

    set_action_on();
    set_mousefun("reposn cursor", "finish text", "cancel");
    draw_mousefun_canvas();
    canvas_kbd_proc = char_handler;
    canvas_middlebut_proc = finish_text_input;
    canvas_leftbut_proc = finish_n_start;
    canvas_rightbut_proc = cancel_text_input;

    /*
     * set working font info to current settings. This allows user to change
     * font settings while we are in the middle of accepting text without
     * affecting this text i.e. we don't allow the text to change midway
     * through
     */

    work_fontsize = cur_fontsize;
    work_font = using_ps ? cur_ps_font : cur_latex_font;
    work_psflag = using_ps;
    work_textjust = cur_textjust;

    /* load the X font and get its id for this font, size */
    canvas_font = lookfont(x_fontnum(work_psflag, work_font), work_fontsize);
    char_ht = char_height(canvas_font);
    char_wid = char_width(canvas_font);

    if ((cur_t = text_search(cur_x, cur_y)) == NULL) {	/* new text input */
	leng_prefix = leng_suffix = 0;
	*suffix = 0;
	prefix[leng_prefix] = '\0';
	base_x = cur_x;
	base_y = cur_y;
    } else {			/* clicked on existing text */
	if (hidden_text(cur_t)) {
	    put_msg("Can't edit hidden text");
	    reset_action_on();
	    text_drawing_selected();
	    return;
	}
	toggle_textmarker(cur_t);
	draw_text(cur_t, ERASE);
	switch (cur_t->type) {
	case T_LEFT_JUSTIFIED:
	    basx = cur_t->base_x;
	    break;
	case T_CENTER_JUSTIFIED:
	    basx = cur_t->base_x - cur_t->length / 2;
	    break;
	case T_RIGHT_JUSTIFIED:
	    basx = cur_t->base_x - cur_t->length;
	    break;
	}
	leng_suffix = strlen(cur_t->cstring);
	/* leng_prefix is # of char in the text before the cursor */
	leng_prefix = prefix_length(cur_t->cstring, cur_x - basx);
	leng_suffix -= leng_prefix;
	cpy_n_char(prefix, cur_t->cstring, leng_prefix);
	strcpy(suffix, &cur_t->cstring[leng_prefix]);
	tsize = pf_textwidth(cur_t->font, psfont_text(cur_t), cur_t->size,
			     leng_prefix, prefix);
	ssize = pf_textwidth(cur_t->font, psfont_text(cur_t), cur_t->size,
			     leng_suffix, suffix);
	cur_x = base_x = basx;
	cur_y = base_y = cur_t->base_y;
	cur_x += tsize.x;
	work_font = cur_t->font;
	work_fontsize = cur_t->size;
	work_textjust = cur_t->type;
    }
    initialize_char_handler(canvas_win, finish_text_input,
			    base_x, base_y);
    draw_char_string();
}

static
F_text         *
new_text()
{
    F_text         *text;
    PR_SIZE         size;

    if ((text = create_text()) == NULL)
	return (NULL);

    if ((text->cstring = new_string(leng_prefix + 1)) == NULL) {
	free((char *) text);
	return (NULL);
    }
    text->type = work_textjust;
    text->font = work_font;	/* put in current font number */
    text->size = work_fontsize;	/* added 9/25/89 B.V.Smith */
    text->angle = cur_angle;
    text->flags = cur_textflags;
    text->color = cur_color;
    text->depth = 0;
    text->pen = 0;
    size = pf_textwidth(text->font, psfont_text(text), text->size,
			leng_prefix, prefix);
    text->length = size.x;	/* in pixels */
    text->height = size.y;	/* in pixels */
    text->base_x = base_x;
    text->base_y = base_y;
    strcpy(text->cstring, prefix);
    text->next = NULL;
    return (text);
}

static int
cpy_n_char(dst, src, n)
    char           *dst, *src;
    int             n;
{
    /* src must be longer than n chars */

    while (n--)
	*dst++ = *src++;
    *dst = '\0';
}

static int
prefix_length(string, where_p)
    char           *string;
    int             where_p;
{
    /* c stands for character unit and p for pixel unit */
    int             l, len_c, len_p;
    int             char_wid, where_c;
    PR_SIZE         size;

    if (canvas_font == NULL)
	fprintf(stderr, "xfig: Error, in prefix_length, canvas_font = NULL\n");
    len_c = strlen(string);
    size = pf_textwidth(cur_t->font, psfont_text(cur_t), cur_t->size,
			len_c, string);
    len_p = size.x;
    if (where_p >= len_p)
	return (len_c);		/* entire string is the prefix */

    char_wid = char_width(canvas_font);
    where_c = where_p / char_wid;	/* estimated char position */
    size = pf_textwidth(cur_t->font, psfont_text(cur_t), cur_t->size,
			where_c, string);
    l = size.x;			/* actual length (pixels) of string of
				 * where_c chars */
    if (l < where_p) {
	do {			/* add the width of next char to l */
	    l += (char_wid = char_advance(canvas_font, string[where_c++]));
	} while (l < where_p);
	if (l - (char_wid >> 1) >= where_p)
	    where_c--;
    } else if (l > where_p) {
	do {			/* subtract the width of last char from l */
	    l -= (char_wid = char_advance(canvas_font, string[--where_c]));
	} while (l > where_p);
	if (l + (char_wid >> 1) >= where_p)
	    where_c++;
    }
    return (where_c);
}

/*******************************************************************

	char handling routines

*******************************************************************/

#define			BLINK_INTERVAL	700	/* milliseconds blink rate */

static PIXWIN   pw;
static int      ch_height;
static int      cbase_x, cbase_y;
static float    rbase_x, rcur_x;
static int      obase_x;

static          (*cr_proc) ();

static
draw_cursor(x, y)
    int             x, y;
{
    pw_vector(pw, x, y, x, y - ch_height, INV_PAINT, 1, RUBBER_LINE, 0.0);
}

static int
initialize_char_handler(p, cr, bx, by)
    PIXWIN          p;
    int             (*cr) ();
    int             bx, by;
{
    pw = p;
    cr_proc = cr;
    rbase_x = obase_x = cbase_x = bx;	/* keep real base so dont have
					 * roundoff */
    rcur_x = cur_x;
    cbase_y = by;

    ch_height = char_height(canvas_font);
    turn_on_blinking_cursor(draw_cursor, draw_cursor,
			    cur_x, cur_y, (long) BLINK_INTERVAL);
}

static int
terminate_char_handler()
{
    turn_off_blinking_cursor();
    cr_proc = NULL;
}

/*
 * we use INV_PAINT below instead of ERASE and PAINT to avoid interactions
 * with the cursor.  It means that we need to do a ERASE before we start the
 * cursor and a PAINT after it is turned off.
 */

static int
erase_char_string()
{
    pw_text(pw, cbase_x, cbase_y, INV_PAINT,
	    work_font, work_psflag, work_fontsize, prefix);
    if (leng_suffix)
	pw_text(pw, cur_x, cbase_y, INV_PAINT,
		work_font, work_psflag, work_fontsize, suffix);
}

static int
draw_char_string()
{
    pw_text(pw, cbase_x, cbase_y, INV_PAINT,
	    work_font, work_psflag, work_fontsize, prefix);
    if (leng_suffix)
	pw_text(pw, cur_x, cbase_y, INV_PAINT,
		work_font, work_psflag, work_fontsize, suffix);
    move_blinking_cursor(cur_x, cur_y);
}

static
char_handler(c)
    unsigned char   c;
{
    int             cwidth;

    if (cr_proc == NULL)
	return;

    if (c == CR) {
	erase_char_string();
	/*
	 * comment out the cr_proc() and un-comment the new_text-line() to
	 * have new text automatically start after old
	 */
	/* cr_proc(); */
	new_text_line();
    } else if (c == DEL || c == CTRL_H) {
	if (leng_prefix > 0) {
	    /*
	     * To increase efficiency you could remove the erase_char_string
	     * from the next line and add the following in the switch below:
	     * LEFT: 			erase_suffix CENTER:	erase_prefix
	     * erase_suffix RIGHT:	erase_prefix A similar thing would
	     * need to be done for draw_char_string and for the other text
	     * functions below.
	     */
	    erase_char_string();
	    cwidth = char_advance(canvas_font, prefix[leng_prefix - 1]);
	    /* correct text/cursor posn for justification and zoom factor */
	    switch (work_textjust) {
	    case T_LEFT_JUSTIFIED:
		/* move the suffix to the left */
		rcur_x -= (float) (cwidth);
		break;
	    case T_CENTER_JUSTIFIED:
		/* advance right by cwidth/2 */
		rbase_x += (float) (cwidth / 2.0);
		/* move suffix left by cwidth/2 */
		rcur_x -= (float) (cwidth / 2.0);
		break;
	    case T_RIGHT_JUSTIFIED:
		/* move prefix to right */
		rbase_x += (float) (cwidth);
		break;
	    }
	    prefix[--leng_prefix] = '\0';
	    cbase_x = rbase_x;	/* fix */
	    cur_x = rcur_x;
	    draw_char_string();
	}
    } else if (c == CTRL_U || c == CTRL_X) {
	if (leng_prefix > 0) {
	    erase_char_string();
	    switch (work_textjust) {
	    case T_CENTER_JUSTIFIED:
		while (leng_prefix--)	/* subtract char width/2 per char */
		    rcur_x -= char_advance(canvas_font, prefix[leng_prefix]) /
			2.0;
		cur_x = cbase_x = rbase_x = rcur_x;
		break;
	    case T_RIGHT_JUSTIFIED:
		cbase_x = rbase_x = cur_x = rcur_x;
		break;
	    case T_LEFT_JUSTIFIED:
		cur_x = rcur_x = cbase_x = rbase_x;
		break;
	    }
	    leng_prefix = 0;
	    *prefix = '\0';
	    draw_char_string();
	}
    } else if (c < SP) {
	put_msg("Invalid character ignored");
    } else if (leng_prefix + leng_suffix == BUF_SIZE) {
	put_msg("Text buffer is full, character is ignored");
    } else {			/* normal text character */
	erase_char_string();
	cwidth = char_advance(canvas_font, c);
	/* correct text/cursor posn for justification and zoom factor */
	switch (work_textjust) {
	case T_LEFT_JUSTIFIED:
	    /* move the suffix to the right */
	    rcur_x += (float) (cwidth);
	    break;
	case T_CENTER_JUSTIFIED:
	    /* advance left by cwidth/2 */
	    rbase_x -= (float) (cwidth / 2.0);
	    /* move suffix right by cwidth/2 */
	    rcur_x += (float) (cwidth / 2.0);
	    break;
	case T_RIGHT_JUSTIFIED:
	    /* move prefix to left */
	    rbase_x -= (float) (cwidth);
	    break;
	}
	prefix[leng_prefix++] = c;
	prefix[leng_prefix] = '\0';
	cbase_x = rbase_x;	/* fix */
	cur_x = rcur_x;
	draw_char_string();
    }
}

/*******************************************************************

	blinking cursor handling routines

*******************************************************************/

static int      cursor_on, cursor_is_moving;
static int      cursor_x, cursor_y;
static XtTimerCallbackProc blink();
static int      (*erase) ();
static int      (*draw) ();
static unsigned long blink_timer;

static XtIntervalId blinkid;
static int      stop_blinking = False;
static int      cur_is_blinking = False;

static int
turn_on_blinking_cursor(draw_cursor, erase_cursor, x, y, msec)
    int             (*draw_cursor) ();
    int             (*erase_cursor) ();
    int             x, y;
    unsigned long   msec;
{
    draw = draw_cursor;
    erase = erase_cursor;
    cursor_is_moving = 0;
    cursor_x = x;
    cursor_y = y;
    blink_timer = msec;
    draw(x, y);
    cursor_on = 1;
    if (!cur_is_blinking) {	/* if we are already blinking, don't request
				 * another */
	blinkid = XtAppAddTimeOut(tool_app, blink_timer, blink, 0);
	cur_is_blinking = True;
    }
    stop_blinking = False;
}

static int
turn_off_blinking_cursor()
{
    if (cursor_on)
	erase(cursor_x, cursor_y);
    stop_blinking = True;
}

static XtTimerCallbackProc
blink(client_data, id)
#if XtSpecificationRelease >= 4
    XtPointer       client_data;

#else
    caddr_t         client_data;

#endif
    XtIntervalId   *id;
{
    if (!stop_blinking) {
	if (cursor_is_moving)
	    return (0);
	if (cursor_on) {
	    erase(cursor_x, cursor_y);
	    cursor_on = 0;
	} else {
	    draw(cursor_x, cursor_y);
	    cursor_on = 1;
	}
	blinkid = XtAppAddTimeOut(tool_app, blink_timer, blink, 0);
    } else {
	stop_blinking = False;	/* signal that we've stopped */
	cur_is_blinking = False;
    }
    return (0);
}

static int
move_blinking_cursor(x, y)
    int             x, y;
{
    cursor_is_moving = 1;
    if (cursor_on)
	erase(cursor_x, cursor_y);
    cursor_x = x;
    cursor_y = y;
    draw(cursor_x, cursor_y);
    cursor_on = 1;
    cursor_is_moving = 0;
}
