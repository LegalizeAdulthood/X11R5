; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         identifier.lsp
; RCS:          $Header: identifier.lsp,v 1.2 91/10/05 17:44:10 mayer Exp $
; Description:  A useful UI debugging tool. Loading this file creates a panel that
;		allows you to click on a widget to identify it, click on a widget
;		to destroy it, or change the foreground and background colors of
;		the widget you click on. For Motif 1.1, the "Identify Selected Widget"
;		button becomes especially useful because it will print out the
;		fully qualified resource name -- this allows setting up your
;		X-resources on a per widget basis and allows you to better understand
;		which widgets are affected by a particular setting in your ~/.Xdefaults...
; Author:       Niels Mayer, HPLabs
; Created:      Mon Oct 29 02:44:55 1990
; Modified:     Sat Oct  5 17:40:03 1991 (Niels Mayer) mayer@hplnpm
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

(let* (toplevel_w
       rc_w identify_pb_w destroy_pb_w
       fg_rc_w fg_la_w fg_ed_w
       bg_rc_w bg_la_w bg_ed_w
       )

  (setq toplevel_w
	(send TOP_LEVEL_SHELL_WIDGET_CLASS :new "identshl"
	      :XMN_TITLE	"Widget Operations"
	      :XMN_ICON_NAME	"WidgetOps"
	      ))
  (setq rc_w
	(send XM_ROW_COLUMN_WIDGET_CLASS :new :managed
	      "rc" toplevel_w
	      :XMN_PACKING		:no_packing
	      :XMN_NUM_COLUMNS		1
	      :XMN_ENTRY_ALIGNMENT	:alignment_center
	      :XMN_ORIENTATION		:vertical
	      ))
  (setq identify_pb_w
	(send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	      "identify" rc_w
	      :XMN_LABEL_STRING "Identify Selected Widget"
	      ))
  (setq destroy_pb_w
	(send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	      "destroy" rc_w
	      :XMN_LABEL_STRING "Destroy Selected Widget"
	      ))
  (setq fg_rc_w
	(send XM_ROW_COLUMN_WIDGET_CLASS :new :managed
	      "rc_fgcolor" rc_w
	      :XMN_PACKING		:pack_tight
	      :XMN_ENTRY_ALIGNMENT	:alignment_center
	      :XMN_ORIENTATION		:horizontal
	      ))
  (setq fg_la_w
	(send XM_LABEL_WIDGET_CLASS :new :managed
	      "label_fgcolor" fg_rc_w
	      :XMN_LABEL_STRING "Set Foreground Color\nof Selected Widget:"
	      ))
  (setq fg_ed_w
	(send XM_TEXT_WIDGET_CLASS :new :managed
	      "edit_fgcolor" fg_rc_w
	      :XMN_EDIT_MODE       :single_line_edit
	      ))
  (setq bg_rc_w
	(send XM_ROW_COLUMN_WIDGET_CLASS :new :managed
	      "rc_bgcolor" rc_w
	      :XMN_PACKING		:pack_tight
	      :XMN_ENTRY_ALIGNMENT	:alignment_center
	      :XMN_ORIENTATION		:horizontal
	      ))
  (setq bg_la_w
	(send XM_LABEL_WIDGET_CLASS :new :managed
	      "label_bgcolor" bg_rc_w
	      :XMN_LABEL_STRING "Set Background Color\nof Selected Widget:"
	      ))
  (setq bg_ed_w
	(send XM_TEXT_WIDGET_CLASS :new :managed
	      "edit_bgcolor" bg_rc_w
	      :XMN_EDIT_MODE       :SINGLE_LINE_EDIT
	      ))

  (send toplevel_w :realize)


  (if (and (eq *MOTIF_VERSION* 1) (eq *MOTIF_REVISION* 0))
      ;; Motif 1.0 version -- method :NAME not def'd in X11r3
      ;; so we can't do all the fancy stuff as done below
      (send identify_pb_w :set_callback :XMN_ACTIVATE_CALLBACK '()
	    '(
	      (let ((w (get_moused_widget))
		    height
		    width)

		(send w :get_values
		      :XMN_HEIGHT 'height
		      :XMN_WIDTH  'width)

		(format t "\nwidget=~A\n\tparent=~A\n\theight=~A\n\twidth=~A\n"
			w (send w :parent) height width)
		)))
    ;; Motif 1.1/X11r4 version -- attempts to print fully qualified
    ;; resource name. Note that more work needs to be done on resource
    ;; name printing... current logic was hacked, not designed.
    (send identify_pb_w :set_callback :XMN_ACTIVATE_CALLBACK '()
	  '(
	    (let ((w (get_moused_widget))
		  height
		  width)

	      (send w :get_values
		    :XMN_HEIGHT 'height
		    :XMN_WIDTH  'width)

	      (format t "\nwidget=~A\n\tparent=~A\n\theight=~A\n\twidth=~A\n"
		      w (send w :parent) height width)

	      (let* ((name (send w :name))
		     (resname (if (string= name "") "*" name))
		     (wildcard-p nil))
  
		(loop

		 (if (null (setq w (send w :parent)))
		     (return (format t "\tX-resource = ~A\n" resname)))

		 (setq name (send w :name))
		 (cond ((string= name "")
			(cond ((not wildcard-p)
			       (setq resname (strcat "*" resname))
			       (setq wildcard-p t)))
			)
		       (t
			(cond (wildcard-p
			       (setq resname (strcat name resname))
			       (setq wildcard-p nil)
			       )
			      (T
			       (setq resname (strcat name "." resname))
			       )
			      ))
		       )
		 ))
	      )))
    )

  (send destroy_pb_w :set_callback :XMN_ACTIVATE_CALLBACK '()
	'(
	  (send (get_moused_widget) :destroy)
	  ))

  (send fg_ed_w :set_callback :XMN_ACTIVATE_CALLBACK
	'(CALLBACK_WIDGET)
	'(
	  (send (get_moused_widget) :set_values
		:XMN_FOREGROUND (send CALLBACK_WIDGET :get_string)
		)
	  ))

  (send bg_ed_w :set_callback :XMN_ACTIVATE_CALLBACK
	'(CALLBACK_WIDGET)
	'(
	  (send (get_moused_widget) :set_values
		:XMN_BACKGROUND (send CALLBACK_WIDGET :get_string)
		)
	  ))

  )
