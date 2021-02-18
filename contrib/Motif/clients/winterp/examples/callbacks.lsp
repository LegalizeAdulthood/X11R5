; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         callbacks.lsp
; RCS:          $Header: callbacks.lsp,v 1.4 91/10/05 14:44:10 mayer Exp $
; Description:  Demonstration of using callbacks and timeouts.
; Author:       Niels Mayer, HPLabs
; Created:      Sat Nov 25 01:01:08 1989
; Modified:     Sat Oct  5 14:43:48 1991 (Niels Mayer) mayer@hplnpm
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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;TEST CALLBACKS AND TIMEOUTS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun get-date ()
  (let*
      ((pipe (popen "date" "r"))
       (str (read-line pipe))
       )
    (pclose pipe)
    str))

(defun make-rc-shell ()
  (setq top_w
	(send TOP_LEVEL_SHELL_WIDGET_CLASS :new 
	      :XMN_GEOMETRY "500x500+0+0"
	      :XMN_TITLE (get-date)
	      :XMN_ICON_NAME "win-test"
	      ))
  (setq rc_w
	(send XM_ROW_COLUMN_WIDGET_CLASS :new :managed top_w
	      :XMN_ADJUST_LAST nil
	      ))
  (send top_w :realize)
  )

(make-rc-shell)

(setq start_but_w 
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed 
	    "start" rc_w 
;;;	    :XMN_FOREGROUND "green"
;;;	    :XMN_BACKGROUND "black"
	    ))

(send start_but_w :has_callbacks :XMN_ARM_CALLBACK)
(send start_but_w :has_callbacks :XMN_DISARM_CALLBACK)
(send start_but_w :has_callbacks :XMN_ACTIVATE_CALLBACK)

(send start_but_w :set_callback :XMN_ACTIVATE_CALLBACK '()
      `((setq to 
	      (xt_add_timeout
	       1000 
	       '((send ,XM_PUSH_BUTTON_WIDGET_CLASS :new :managed 
		       (get-date) ,rc_w
;;;		       :XMN_BACKGROUND "magenta"
		       )
		 (setq to (xt_add_timeout 1000 TIMEOUT_OBJ))
		 )
	       ))
	))


(send start_but_w :has_callbacks :XMN_ARM_CALLBACK)
(send start_but_w :has_callbacks :XMN_DISARM_CALLBACK)
(send start_but_w :has_callbacks :XMN_ACTIVATE_CALLBACK)

(setq stop_but_w 
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed 
	    "stop" rc_w 
;;;	    :XMN_FOREGROUND "red"
;;;	    :XMN_BACKGROUND "black"
	    ))

(send stop_but_w :has_callbacks :XMN_ARM_CALLBACK)
(send stop_but_w :has_callbacks :XMN_DISARM_CALLBACK)
(send stop_but_w :has_callbacks :XMN_ACTIVATE_CALLBACK)

(send stop_but_w :set_callback :XMN_ACTIVATE_CALLBACK '()
      '(
	(xt_remove_timeout to)
	(format t "quack\n")
	))

(send stop_but_w :has_callbacks :XMN_ARM_CALLBACK) ;==>CALLBACK_HAS_NONE
(send stop_but_w :has_callbacks :XMN_DISARM_CALLBACK) ;==>CALLBACK_HAS_NONE
(send stop_but_w :has_callbacks :XMN_ACTIVATE_CALLBACK)	;==>CALLBACK_HAS_SOME
(send stop_but_w :has_callbacks :XMN_APPLY_CALLBACK) ;==>CALLBACK_NO_LIST
