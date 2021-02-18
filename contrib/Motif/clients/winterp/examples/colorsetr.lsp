; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         colorsetr.lsp
; RCS:          $Header: colorsetr.lsp,v 1.3 91/10/05 15:12:38 mayer Exp $
; Description:  A useful interface building tool -- use this to create and set
;		colors on widgets.
;		NOTE: THIS WON'T WORK W/ MOTIF 1.0 DUE TO USE OF 1.1 FUNCTIONS
; Author:       Niels Mayer, HPLabs
; Created:      Mon Oct 29 02:44:55 1990
; Modified:     Sat Oct  5 15:12:25 1991 (Niels Mayer) mayer@hplnpm
; Language:     Lisp
; Package:      N/A
; Status:       X11r5 contrib tape release
;
; WINTERP Copyright 1989, 1990, 1991 Hewlett-Packard Company (by Niels Mayer).
; XLISP version 2.1, Copyright (c) 1989, by David Betz.
;
; Permission to use, copy, modify, distribute, and sell this software and its
; documentation for any purpose is hereby granted without fee, provided that
; the above copyright notice appear in all copies and that both that
; copyright notice and this permission notice appear in supporting
; documentation, and that the name of Hewlett-Packard and Niels Mayer not be
; used in advertising or publicity pertaining to distribution of the software
; without specific, written prior permission.  Hewlett-Packard and Niels Mayer
; makes no representations about the suitability of this software for any
; purpose.  It is provided "as is" without express or implied warranty.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(let (
      toplevel_w
      rc_w apply_pb_w color_label_w
      r_la_w r_scale_w
      g_la_w g_scale_w
      b_la_w b_scale_w
      (background_color (aref (X_ALLOC_N_COLOR_CELLS_NO_PLANES 1) 0))
      color_array
      )

  (setq toplevel_w
	(send TOP_LEVEL_SHELL_WIDGET_CLASS :new "colorsetr"
	      :XMN_TITLE "Color Setter"
	      :XMN_ICON_NAME "ColorSetr"
	      ))
  (setq rc_w
	(send XM_FORM_WIDGET_CLASS :new :managed
	      "form" toplevel_w
	      ))
  (setq apply_pb_w
	(send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	      "button_apply" rc_w
	      :XMN_LABEL_STRING		"Set Color On Selected Widget"
	      :XMN_TOP_ATTACHMENT	:attach_position
	      :XMN_BOTTOM_ATTACHMENT	:attach_position
	      :XMN_TOP_POSITION		0
	      :XMN_BOTTOM_POSITION	13
	      :XMN_LEFT_ATTACHMENT	:attach_form
	      :XMN_RIGHT_ATTACHMENT	:attach_form
	      ))
;;;(setq take_pb_w
;;;      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
;;;	    "button_take" rc_w
;;;	    :xmn_label_string "Take Color From Selected Widget"
;;;	    ))
  (setq color_label_w
	(send XM_LABEL_WIDGET_CLASS :new :managed "label_color" rc_w
	      :XMN_LABEL_STRING		"Color"
	      :XMN_TOP_ATTACHMENT	:attach_position
	      :XMN_BOTTOM_ATTACHMENT	:attach_position
	      :XMN_TOP_POSITION		13
	      :XMN_BOTTOM_POSITION	25
	      :XMN_LEFT_ATTACHMENT	:attach_form
	      :XMN_RIGHT_ATTACHMENT	:attach_form
	      ))
  (setq r_la_w
	(send XM_LABEL_GADGET_CLASS :new :managed "label_red" rc_w
	      :XMN_LABEL_STRING		"R:"
	      :XMN_TOP_ATTACHMENT	:attach_position
	      :XMN_BOTTOM_ATTACHMENT	:attach_position
	      :XMN_TOP_POSITION		25
	      :XMN_BOTTOM_POSITION	50
	      :XMN_LEFT_ATTACHMENT	:attach_form
	      ))
  (setq r_scale_w
	(send XM_SCALE_WIDGET_CLASS :new :managed "scale_red" rc_w
	      :XMN_SHOW_VALUE		t
	      :XMN_ORIENTATION		:horizontal
	      :XMN_MAXIMUM		255
	      :XMN_MINIMUM		0
	      :XMN_TOP_ATTACHMENT	:attach_position
	      :XMN_BOTTOM_ATTACHMENT	:attach_position
	      :XMN_TOP_POSITION		25
	      :XMN_BOTTOM_POSITION	50
	      :XMN_LEFT_ATTACHMENT	:attach_widget
	      :XMN_LEFT_WIDGET		r_la_w
	      :XMN_RIGHT_ATTACHMENT	:attach_form
	      ))
  (setq g_la_w
	(send XM_LABEL_GADGET_CLASS :new :managed "label_green" rc_w
	      :XMN_LABEL_STRING		"G:"
	      :XMN_TOP_ATTACHMENT	:attach_position
	      :XMN_BOTTOM_ATTACHMENT	:attach_position
	      :XMN_TOP_POSITION		50
	      :XMN_BOTTOM_POSITION	75
	      :XMN_LEFT_ATTACHMENT	:attach_form
	      ))
  (setq g_scale_w
	(send XM_SCALE_WIDGET_CLASS :new :managed "scale_green" rc_w
	      :XMN_SHOW_VALUE		t
	      :XMN_ORIENTATION		:horizontal
	      :XMN_MAXIMUM		255
	      :XMN_MINIMUM		0
	      :XMN_TOP_ATTACHMENT	:attach_position
	      :XMN_BOTTOM_ATTACHMENT	:attach_position
	      :XMN_TOP_POSITION		50
	      :XMN_BOTTOM_POSITION	75
	      :XMN_LEFT_ATTACHMENT	:attach_widget
	      :XMN_LEFT_WIDGET		g_la_w
	      :XMN_RIGHT_ATTACHMENT	:attach_form
	      ))
  (setq b_la_w
	(send XM_LABEL_GADGET_CLASS :new :managed "label_blue" rc_w
	      :XMN_LABEL_STRING		"B:"
	      :XMN_TOP_ATTACHMENT	:attach_position
	      :XMN_BOTTOM_ATTACHMENT	:attach_position
	      :XMN_TOP_POSITION		75
	      :XMN_BOTTOM_POSITION	100
	      :XMN_LEFT_ATTACHMENT	:attach_form
	      ))
  (setq b_scale_w
	(send XM_SCALE_WIDGET_CLASS :new :managed "scale_blue" rc_w
	      :XMN_SHOW_VALUE		t
	      :XMN_ORIENTATION		:horizontal
	      :XMN_MAXIMUM		255
	      :XMN_MINIMUM		0
	      :XMN_TOP_ATTACHMENT	:attach_position
	      :XMN_BOTTOM_ATTACHMENT	:attach_position
	      :XMN_TOP_POSITION		75
	      :XMN_BOTTOM_POSITION	100
	      :XMN_LEFT_ATTACHMENT	:attach_widget
	      :XMN_LEFT_WIDGET		b_la_w
	      :XMN_RIGHT_ATTACHMENT	:attach_form
	      ))

  (send toplevel_w :realize)

;;;(defvar bg_pixel nil)
;;;(defvar fg_pixel nil)
;;;(defvar ts_pixel nil)
;;;(defvar bs_pixel nil)
;;;(defvar sel_pixel nil)
;;;(send take_pb_w :set_callback :XMN_ACTIVATE_CALLBACK '()
;;;      '(
;;;	(send (get_moused_widget) :get_values
;;;	      :xmn_top_shadow_color 'ts_pixel
;;;	      :xmn_bottom_shadow_color 'bs_pixel
;;;	      :xmn_foreground 'fg_pixel
;;;	      :xmn_background 'bg_pixel)
;;;
;;;	(send color_label_w :set_values
;;;	      :xmn_top_shadow_color ts_pixel
;;;	      :xmn_bottom_shadow_color bs_pixel
;;;	      :xmn_foreground fg_pixel
;;;	      :xmn_background bg_pixel)
;;;	))

  ;; share same callback code between R, G, and B :XMN_DRAG_CALLBACK...
  (setq apply-sliders-to-color-label
	`(
	  (let ((saved-integer-format *INTEGER-FORMAT*)
		)
	    (setq *INTEGER-FORMAT* "%02lx")
    
	    ;; bug -- should unwind-protect this incase we run out of
	    ;; colors -- force *INTEGER-FORMAT* back to original value.
	    ;; I'm blowing this off because xlisp2.1c/WINTERP 1.14 has 
	    ;; 'special' (dynamically bound) variables.
	    (send ,color_label_w :set_values
		  :XMN_BACKGROUND
		  (x_store_color ,background_color
				 (format nil "#~A~A~A" ;RGB in hexadecimal
					 (send ,r_scale_w :get_value) ;R
					 (send ,g_scale_w :get_value) ;G
					 (send ,b_scale_w :get_value)))	;B
		  )

	    (setq *INTEGER-FORMAT* saved-integer-format)
	    )))

  ;; set up drag callbacks so that we can se result of color immediately
  ;; we also need to set up value changed callbacks below...
  (send r_scale_w :set_callback :XMN_DRAG_CALLBACK '()
	apply-sliders-to-color-label
	)
  (send g_scale_w :set_callback :XMN_DRAG_CALLBACK '()
	apply-sliders-to-color-label
	)
  (send b_scale_w :set_callback :XMN_DRAG_CALLBACK '()
	apply-sliders-to-color-label
	)

  ;; value changed callbacks are needed because colors won't change if you only
  ;; have drag callbacks and you move the slider by means other than dragging.
  (send r_scale_w :set_callback :XMN_VALUE_CHANGED_CALLBACK '()
	apply-sliders-to-color-label
	)
  (send g_scale_w :set_callback :XMN_VALUE_CHANGED_CALLBACK '()
	apply-sliders-to-color-label
	)
  (send b_scale_w :set_callback :XMN_VALUE_CHANGED_CALLBACK '()
	apply-sliders-to-color-label
	)

  (send apply_pb_w :set_callback :XMN_ACTIVATE_CALLBACK '()
	'(
	  (let ((saved-integer-format *INTEGER-FORMAT*)
		)
	    (setq *INTEGER-FORMAT* "%02lx") ;hack: print in hex by setting string used by sprintf in format
	    (setq color_array
		  (XM_GET_COLORS
		   (x_store_color background_color
				  (format nil "#~A~A~A"	;RGB in hexadecimal
					  (send r_scale_w :get_value) ;R
					  (send g_scale_w :get_value) ;G
					  (send b_scale_w :get_value)))	;B
		   ))
	    ;; bug -- should unwind-protect this incase we run out of
	    ;; colors -- force *INTEGER-FORMAT* back to original value.
	    ;; I'm blowing this off because xlisp2.1c/WINTERP 1.14 has 
	    ;; 'special' (dynamically bound) variables.
	    (send (get_moused_widget) :set_values
		  ;; :XMN_BACKGROUND (aref color_array 0)
		  :XMN_BACKGROUND		background_color
		  :XMN_FOREGROUND		(aref color_array 1)
		  :XMN_TOP_SHADOW_COLOR		(aref color_array 2)
		  :XMN_BOTTOM_SHADOW_COLOR	(aref color_array 3)
		  :XMN_TROUGH_COLOR		(aref color_array 4)
		  )

	    (setq *INTEGER-FORMAT* saved-integer-format)
	    )))

  (apply 'eval apply-sliders-to-color-label )
  )
