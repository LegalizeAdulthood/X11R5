/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  		       COPYRIGHT 1991 HUBERT RECHSTEINER		      %
%			                                                      %
%                                                                             %
%  Permission to use, copy, modify, distribute, and sell this software and    %
%  its documentation for any purpose is hereby granted without fee, provided  %   %  that the above copyright notice appear in all copies and that both that    %    %  copyright notice and this permission notice appear in  supporting          %
%  documentation, and that the name of the author not be used in advertising  % 
%  or publicity pertaining to distribution of the software without specific,  % 
%  written prior permission. The author makes no representations about the    %
%  suitability of this software for any purpose.  It is provided "as is"      %
%  without express or implied warranty.                                       %
%                                                                             %
%  THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,          %
%  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN        %
%  NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR           %
%  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS        %
%  OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE      %
%  OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE         %
%  USE OR PERFORMANCE OF THIS SOFTWARE.                                       %
%                                                                             %
%                                                                             %
%  Software Design       : Hubert Rechsteiner January 1991                    %
%									      %
%  Further developments  : Doris Hirt 					      %
%                          Nicolas Repond 				      %
%                          Hubert Rechsteiner				      %
%                          October 1991					      %
%									      %
%									      %
%  Please send any improvements, bug fixes, useful modifications, and         %
%  comments to:  selfmoving@uliis.unil.ch                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/


/*SccsId "@(#)SelfMP.h	3.1 	 10/4/91"*/


#ifndef	HR_SELFMOVINGP_H
#define HR_SELFMOVINGP_H
#include "SelfM.h"


#define NB_HANDLES  8

        typedef struct _header *Header;

	typedef struct _HrSelfMovingClassPart {
		Header			selfmoving_list;
		Header			brother_list;
	} HrSelfMovingClassPart;


 	typedef struct _HrSelfMovingClassRec {
		CoreClassPart		core_class;
                CompositeClassPart	composite_class;
		HrSelfMovingClassPart	selfMoving_class;
	} HrSelfMovingClassRec;


	extern	HrSelfMovingClassRec	HrselfMovingClassRec;


	typedef	struct _HrSelfMovingPart {
		Position	x_child;
		Position	y_child;
                Boolean		selected;
		Boolean		allow_move;
		Cardinal	widget_id;
		Boolean		active;
                Boolean		grid_on;
                int		grid;
                int		horizontal_line_offset;
		int		vertical_line_offset;
                Boolean		show_horizontal_line;
                Boolean		show_vertical_line;
		int		selection_border;
		Boolean		select_in_reverse_video;
                HrHandleType	north_west_handle;
                HrHandleType	north_handle;
                HrHandleType	north_east_handle;
                HrHandleType	east_handle;
                HrHandleType	south_east_handle;
                HrHandleType	south_handle;
                HrHandleType	south_west_handle;
                HrHandleType	west_handle;
                Dimension	minimum_width;
		Dimension	minimum_height;
		Dimension	maximum_width;
		Dimension	maximum_height;
		HrAlignType	alignment;
		Boolean		install_accelerators;
		XtTranslations  handle_translations; 
                GC		gc;
		XtCallbackList	select_list;
		XtCallbackList	unselect_list;
		XtCallbackList	move_list;
		XtCallbackList	during_move_list;
		XtCallbackList	resize_list;
		XtCallbackList	during_resize_list;
                XtCallbackList	double_click_list;
		XtCallbackList	acc_move_list;
		int		width,height;
		int		first; 
		int		prov_x,prov_y;
		int		motion;
		Boolean		clicked;
		Position	x_field,y_field,delta_x,delta_y;
		Display		*dpy;
		Window		invisible_window;
                Boolean		toggle;
		Boolean		handle_creation;
                Widget		sizing_handles[NB_HANDLES];
                HrHandleType    sizing_handles_state[NB_HANDLES];
                Dimension	sizing_handles_width;
		Dimension	sizing_handles_border_width;
		Cardinal	nb_children;
		Boolean		first_expose;
                Dimension       parent_width,parent_height;
                Dimension	saved_border_width;
		Widget		child;
		Boolean		first_sizing;
		int		delta_x_sizing;
		int		delta_y_sizing;
		int		which_handle;
		int		old_x_rect_sizing;
		int		old_y_rect_sizing;
		int		old_width_rect_sizing;
		int		old_height_rect_sizing;
		Boolean		do_resize;
		Boolean		double_click;
		Time		old_time;
                int		x_acc;
		int		y_acc;
                int		prov_x_acc;
		int		prov_y_acc;
                int		xr_acc;
		int		yr_acc;
		int		width_acc;
		int		height_acc;
		Boolean		motion_acc;
		Boolean		first_acc;
		Boolean		please_do_motion;
	} HrSelfMovingPart;

	typedef struct _HrSelfMovingRec {
		CorePart	    core;
		CompositePart	    composite;
		HrSelfMovingPart    selfMoving;
	} HrSelfMovingRec;

#define XtRHrHandleType		"HrHandleType"
#define XtRHrAlignType		"HrAlignType"
#endif SELFMOVINGP_H
