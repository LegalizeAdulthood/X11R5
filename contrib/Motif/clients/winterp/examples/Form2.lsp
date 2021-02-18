; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         form-widg2.lsp
; RCS:          $Header: Form2.lsp,v 1.2 91/10/05 02:58:10 mayer Exp $
; Description:  Form widget layout -- similar to formtest.c on p 91-92 of
;		Doug Young's Motif book.	
; Author:       Niels Mayer, HPLabs
; Created:      Fri Jul 27 00:59:56 1990
; Modified:     Sat Oct  5 02:57:51 1991 (Niels Mayer) mayer@hplnpm
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

(defun motif-1.1-p () 
  (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 1)))

(setq toplevel_w
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "form2shl"
	    :XMN_GEOMETRY	(if (motif-1.1-p)
				    "+1+1"
				    "300x300+1+1") ;work around Motif 1.0 form-sizing bug.
	    :XMN_TITLE		"Winterp:RC-SHELL"
	    :XMN_ICON_NAME	"W:RC-SHELL"
	    ))

(setq form_w
      (send XM_FORM_WIDGET_CLASS :new :managed
	    "form" toplevel_w
	    ))

;;
;; place 5 equal-sized buttons attached to the right side of the form widget
;; we specify the :xmn_{top,bottom}_attachment resources as :attach_position,
;; which means that we must specify the :xmn_top_position, :xmn_bottom_position
;; as the vertical percentage taken up by each button. (for 5 buttons, that means
;; each button takes up approx 20% (make that 18 + 2 for spacing)...
;; if more buttons are specified, the percentages must be changed to the
;; appropriate value.
;;

(setq pb0_w
      (send XM_TEXT_WIDGET_CLASS :new :managed
	    "pb0" form_w
	    :XMN_STRING		"Mothra\nIn\nPupa\nStage"
	    :XMN_EDIT_MODE		:multi_line_edit
	    :XMN_TOP_ATTACHMENT		:attach_form
	    :XMN_BOTTOM_ATTACHMENT	:attach_form
	    :XMN_LEFT_ATTACHMENT	:attach_form
	    ))

(setq pb1_w
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	    "pb1" form_w
	    :XMN_LABEL_STRING		"Jack In"
	    :XMN_TOP_ATTACHMENT		:attach_position
	    :XMN_BOTTOM_ATTACHMENT	:attach_position
	    :XMN_TOP_POSITION		0
	    :XMN_BOTTOM_POSITION	20
	    :XMN_RIGHT_ATTACHMENT	:attach_form
	    :XMN_LEFT_ATTACHMENT	:attach_widget
	    :XMN_LEFT_WIDGET		pb0_w
	    ))

(setq pb2_w
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	    "pb2" form_w
	    :XMN_LABEL_STRING		"Jack Out"
	    :XMN_TOP_ATTACHMENT		:attach_position
	    :XMN_BOTTOM_ATTACHMENT	:attach_position
	    :XMN_TOP_POSITION		20
	    :XMN_BOTTOM_POSITION	40
	    :XMN_RIGHT_ATTACHMENT	:attach_form
	    :XMN_LEFT_ATTACHMENT	:attach_widget
	    :XMN_LEFT_WIDGET		pb0_w
	    ))

(setq pb3_w
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	    "pb3" form_w
	    :XMN_LABEL_STRING		"Do Watoosi"
	    :XMN_TOP_ATTACHMENT		:attach_position
	    :XMN_BOTTOM_ATTACHMENT	:attach_position
	    :XMN_TOP_POSITION		40
	    :XMN_BOTTOM_POSITION	60
	    :XMN_RIGHT_ATTACHMENT	:attach_form
	    :XMN_LEFT_ATTACHMENT	:attach_widget
	    :XMN_LEFT_WIDGET		pb0_w
	    ))

(setq pb4_w
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	    "pb4" form_w
	    :XMN_LABEL_STRING		"Begin Hyperreality"
	    :XMN_TOP_ATTACHMENT	:attach_position
	    :XMN_BOTTOM_ATTACHMENT	:attach_position
	    :XMN_TOP_POSITION		60
	    :XMN_BOTTOM_POSITION	80
	    :XMN_RIGHT_ATTACHMENT	:attach_form
	    :XMN_LEFT_ATTACHMENT	:attach_widget
	    :XMN_LEFT_WIDGET		pb0_w
	    ))

(setq pb5_w
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	    "pb5" form_w
	    :XMN_LABEL_STRING		"Winterpmute"
	    :XMN_TOP_ATTACHMENT		:attach_position
	    :XMN_BOTTOM_ATTACHMENT	:attach_position
	    :XMN_TOP_POSITION		80
	    :XMN_BOTTOM_POSITION	100
	    :XMN_RIGHT_ATTACHMENT	:attach_form
	    :XMN_LEFT_ATTACHMENT	:attach_widget
	    :XMN_LEFT_WIDGET		pb0_w
	    ))

(send toplevel_w :realize)
