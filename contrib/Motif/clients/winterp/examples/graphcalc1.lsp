; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         graphcalc-options.lsp
; RCS:          $Header: graphcalc1.lsp,v 1.2 91/10/05 16:02:50 mayer Exp $
; Description:  Add an "options" button and popup panel for graphcalc.lsp
; Author:       Niels Mayer, HPLabs
; Created:      Tue Jul 10 10:35:58 1990
; Modified:     Sat Oct  5 16:02:39 1991 (Niels Mayer) mayer@hplnpm
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


(setq graph_opts_pbw
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	    "Options" graph_controlpanel_w
	    ))

(send graph_opts_pbw :set_callback :xmn_activate_callback '()
      '(
	;; create the dialog if it hasn't been created yet
	(if (null graph_options_dialog_w)
	  (setq graph_options_dialog_w (create-graph-options-dialog top_w)))

	;; make it visible
	(send graph_options_dialog_w :popup :grab_none)
;	(send graph_options_dialog_w :manage)
	))

(progn 
  (setq graph_options_dialog_w nil)
  (defun create-graph-options-dialog (parent_w)
    (let*
	((dialog_w
	  (send TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS :new "graphcalc-options" parent_w
		))
	 (rc_w
	  (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed "options_v_rc" dialog_w
		:XMN_ORIENTATION		:vertical
		:XMN_PACKING		:pack_tight
		:XMN_ADJUST_LAST 		nil
		:XMN_ENTRY_ALIGNMENT	:alignment_center
		))
;	 (msg_box_w
;	  (send XM_MESSAGE_BOX_WIDGET_CLASS :new :managed "msg_box_w" dialog_w
;		))
	 )

(setq options_controlpanel_0_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed "options_controlpanel_0" rc_w
	    :XMN_ORIENTATION            :HORIZONTAL
	    :XMN_PACKING                :PACK_TIGHT
	    :XMN_ADJUST_LAST            nil
	    :XMN_ENTRY_ALIGNMENT	:alignment_center
	    ))

(setq options_controlpanel_1_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed "options_controlpanel_1" rc_w
	    :XMN_ORIENTATION            :HORIZONTAL
	    :XMN_PACKING                :PACK_TIGHT
	    :XMN_ADJUST_LAST            nil
	    :XMN_ENTRY_ALIGNMENT	:alignment_center
	    ))

(setq childspacing_pbw
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	    "Set Horizontal Spacing" options_controlpanel_0_w
	    ))
(setq  childspacing_slider_w
       (send XM_SCALE_WIDGET_CLASS :new :managed "disp-slider" options_controlpanel_0_w
	     :XMN_ORIENTATION		:horizontal
	     :XMN_PROCESSING_DIRECTION :MAX_ON_RIGHT 
	     :XMN_SENSITIVE		t
	     :XMN_SHOW_VALUE		t
	     :XMN_MINIMUM		0
	     :XMN_MAXIMUM		+100
	     :XMN_VALUE		20
	     :xmn_width 100
	     :xmn_height 100
	     ))
(send childspacing_pbw :set_callback :xmn_activate_callback '()
      '(
	(send graph_w :set_values :xmn_child_spacing  (send childspacing_slider_w :get_value))
	))

(setq sibspacing_pbw
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	    "Set Vertical Spacing" options_controlpanel_0_w
	    ))
(setq sibspacing_slider_w
       (send XM_SCALE_WIDGET_CLASS :new :managed "disp-slider" options_controlpanel_0_w
	     :XMN_ORIENTATION		:horizontal
	     :XMN_PROCESSING_DIRECTION :MAX_ON_RIGHT 
	     :XMN_SENSITIVE		t
	     :XMN_SHOW_VALUE		t
	     :XMN_MINIMUM		0
	     :XMN_MAXIMUM		+100
	     :XMN_VALUE		20
	     :xmn_width 100
	     :xmn_height 100
	     ))
(send sibspacing_pbw :set_callback :xmn_activate_callback '()
      '(
	(send graph_w :set_values :xmn_sibling_spacing (send sibspacing_slider_w :get_value))
	))

(setq close_pbw
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	    "Close" options_controlpanel_1_w
	    ))

       (send close_pbw :set_callback :xmn_activate_callback '()
	     `(
	       (send ,dialog_w :popdown)
	       ))

      dialog_w				;return
      ))
  )






