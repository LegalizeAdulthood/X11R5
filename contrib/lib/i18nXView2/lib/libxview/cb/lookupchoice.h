/*      "@(#)lookupchoice.h 70.2 91/07/03"                 */

/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents 
 *      pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *      file for terms of the license.
 */

#ifndef		lookup_choice_DEFINED
#define		lookup_choice_DEFINED

#include	<X11/Xresource.h>
#include	<xview/font.h>

#define		LUC_KEY			777

#define		LUC_MAX_LABEL_LEN	4
#define		LUC_LOOKUPSTRING_BUFLEN 4

#define LUC_UDATA_INFO(udata)   ((LucInfo *)(*(Xv_opaque *)udata))


typedef	enum {
	LUC_ALPHABETIC,
	LUC_NUMERIC,
	LUC_ALPHABETIC_LOWER,
	LUC_ALPHABETIC_UPPER,
}  LucKeyKind;

typedef enum {
	LUC_KEY_NUM,
	LUC_KEY_UPPER,
	LUC_KEY_LOWER,
	LUC_KEY_RETURN,
	LUC_KEY_SPACE,
	LUC_KEY_CONTROL,
	LUC_KEY_ARROW,
	LUC_KEY_ILLEGAL,
}   LucKeyType;

typedef enum {
	LUC_INITIAL,
	LUC_START_CALLED,
	LUC_DRAW_CALLED,
	LUC_DONE_CALLED,
}  LucState;

typedef	struct _luc_info {
	XIC		ic;		/* IC 			*/ 
	Display		*dpy;
	Frame		frame;		/* LUC main frame	*/
	Panel		panel;		/* LUC panel		*/
	Panel_item	panel_item;     /* Choice item handle   */
	Frame		p_frame;	/* pre-edit frame       */
	Xv_object	p_obj;
	WhoOwnsLabel    WhoOwnsLabel;   /* Owner of labels      */
	WhoIsMaster	WhoIsMaster;	
	LucKeyKind	key_kind;	/* key label type	*/
	DrawUpDirection	dir;		/* vertical or horizontal */
	XIMLookupStartCallbackStruct
			*start;
	XIMLookupDrawCallbackStruct
			*draw;
	Xv_Font		font;		/* font in LUC		*/
	unsigned int	win_width;	/* frame(canvas) width  */
	unsigned int	win_height;	/* frame(canvas) height */
        int		sc_x;		/* screen width		*/
	int		sc_y;		/* screen hight;	*/
	int		off_x;
	int		off_y;
	int		win_x;		/* frame position 	*/
	int		win_y;
	int		rows;		/* number of lines	*/
	int		columns;	/* number of columns	*/
	int		x0;		/* left(right) margin   */
	int		y0;		/* up(down) margin	*/
	int		dx;		/* width of a cell      */
					/* in PIXEL		*/
	int		dy;		/* height of a cell     */
					/* in PIXEL		*/
	int		first;		/* first can to be appear*/
	int		num;		/* number of candidates */
					/* to be appear		*/
	int		current;	/* current choice	*/
	int		max_can;	/* maximum # of candidates */
	int		char_ratio;
	Bool		keygrab;
	LucState	state;		/* draw() called or not */
}  LucInfo;


#endif		~lookup_choice_DEFINED


