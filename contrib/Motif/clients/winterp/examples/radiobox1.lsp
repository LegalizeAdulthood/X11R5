; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         radiobox1.lsp
; RCS:          $Header: radiobox1.lsp,v 1.2 91/10/05 18:46:49 mayer Exp $
; Description:  The straighforward way to define a radio box.
;               See radiobox2.lsp for a better way using a WINTERP-subclassed
;               toggle-button. Just load this file to see the example.
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:22:37 1989
; Modified:     Sat Oct  5 18:46:33 1991 (Niels Mayer) mayer@hplnpm
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

(setq toplevel_w
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new 
	    :XMN_GEOMETRY "500x500+1+1"
	    :XMN_TITLE "Radio-Box-Test #1"
	    :XMN_ICON_NAME "Radio-Box-Test #1"
	    ))

(setq rowcol_w
      (send XM_ROW_COLUMN_WIDGET_CLASS :new :managed :radio_box "rc" toplevel_w
	    ))

(send toplevel_w :realize)

(do* 
 (;; local vars
  (i 0 (1+ i))
  but_w
  (buttons nil)
  )
 (;; test and return
  (eql i 20)
  )
 ;; body
 (setq but_w
       (send XM_TOGGLE_BUTTON_GADGET_CLASS :new :managed rowcol_w
	     :XMN_LABEL_STRING (format nil "Button ~A" i)
	     ))
 (send but_w :set_callback :xmn_value_changed_callback
       '(callback_widget callback_set)
       '((if callback_set
	     (print_set_button callback_widget buttons)
	   )))
 (setq buttons (cons (cons but_w i) buttons))
 )


(defun print_set_button (widget buttons)
  (format T "Selected button ~A\n" (cdr (assoc widget buttons)))
  )
