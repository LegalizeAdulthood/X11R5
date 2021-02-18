; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         interact.lsp
; RCS:          $Header: interact.lsp,v 1.4 91/10/05 18:19:53 mayer Exp $
; Description:  Play around with interactive features of winterp. 
;               GET_MOUSED_WIDGET allows you to send a message to any widget
;               that you can see. Thus you can interactively change your
;               interfaces' appearance or functionality without having to
;               remember the name  of the desired widget. Note that this'll
;               even work on big composite widgets that create other widgets
;               internally....
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:36:28 1989
; Modified:     Sat Oct  5 18:18:12 1991 (Niels Mayer) mayer@hplnpm
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

(defun invert-moused-widget ()
  (let
      ((widget (get_moused_widget))
       foreground
       background
       )
    (send widget :get_values
	  :XMN_FOREGROUND 'foreground
	  :XMN_BACKGROUND 'background
	  )
    (send widget :set_values
	  :XMN_FOREGROUND background
	  :XMN_BACKGROUND foreground
	  )
    ))

(invert-moused-widget)

(send (get_moused_widget) :get_values
      :XMN_X nil
      :XMN_Y nil
      :XMN_HEIGHT nil
      :XMN_WIDTH nil
      )

(send (get_moused_widget) :set_values 
      :xmn_font_list "6x10")

(send  (GET_MOUSED_WIDGET) :destroy)

(get_moused_widget)

(send (GET_MOUSED_WIDGET) :set_values :xmn_foreground "red")
(send (GET_MOUSED_WIDGET) :set_values :xmn_foreground "white")
(send (GET_MOUSED_WIDGET) :set_values :xmn_foreground "blue")
(send (GET_MOUSED_WIDGET) :set_values :xmn_foreground "green")
(send (GET_MOUSED_WIDGET) :set_values :xmn_background "lightgrey")
(send (GET_MOUSED_WIDGET) :set_values :xmn_background "grey")
(send (GET_MOUSED_WIDGET) :set_values :xmn_background "dimgrey")

(send (get_moused_widget) :set_values
      :xmn_label_string (symbol-name (gensym))
      )

(load "rc-shell")			;need this for 'rc_w' below
(send (get_moused_widget) :set_callback :xmn_activate_callback
      '()
      '((send xm_push_button_gadget_class :new :managed "quackquack" rc_w)))

(send (get_moused_widget) :set_callback :xmn_activate_callback
      '(callback_reason callback_xevent)
      '(
	(format T "reason = ~A; event = ~A\n" callback_reason callback_xevent)
	)
      )


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Evaluate this and click on a widget to find out information about widget.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(let (
      (widget (get_moused_widget))
      height
      width
      )
  (send widget :get_values
      :xmn_height 'height
      :xmn_width 'width
      )
  (format T "\tparent=~A\n\twidget=~A\n\theight=~A\n\twidth=~A\n"
          (send widget :parent)
          widget
          height
          width)
  )
