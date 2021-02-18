; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         trans.lsp
; RCS:          $Header: trans.lsp,v 1.4 91/10/05 19:04:49 mayer Exp $
; Description:  Tests of Xt trnaslation, accelerator facilies. Also test
;               winterp's "Lisp()" action, which allows you to call the
;               lisp evaluator from a translation/accelerator table.
; Author:       Niels Mayer, HPLabs
; Created:      Fri Nov 24 20:16:35 1989
; Modified:     Sat Oct  5 19:04:18 1991 (Niels Mayer) mayer@hplnpm
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

;;
;; Test use of 'Lisp' action to call lisp evaluator through translation
;; or accelerator table entry:
;;

(setq trans
      (XT_PARSE_TRANSLATION_TABLE 
       "Ctrl<Key>L: Lisp(quack action_widget action_xevent) \
Ctrl<Key>K: Lisp(print 'ctrl-key-k-hit)"
       ))

(setq accel
      (XT_PARSE_ACCELERATOR_TABLE
       "Ctrl<Key>L: Lisp(quack action_widget action_xevent) \
Ctrl<Key>K: Lisp(print 'ctrl-key-k-hit)"
       ))

(defun quack (w e)
  (format T "widget=~A; event=~A\n" w e)
)

(setq pb_w
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "pb_w" rc_w
	    ))
(send pb_w :augment_translations trans)
(send pb_w :override_translations trans)
(send pb_w :augment_translations "Ctrl<Key>A: Lisp(print 'ctrl-key-a-hit)")
(send pb_w :uninstall_translations)	;this completely disables widget!


;;
;; Accelerators
;;
(send rc_w :set_values
      :xmn_accelerators "Ctrl<Key>A: Lisp(print 'ctrl-key-a-hit)"
      )

(progn					;install accelerator on  rc_w
					;onto each pushbuttonwe create
  (setq pb_w
	(send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed "pb_w" rc_w
	      ))
  (send pb_w :install_all_accelerators rc_w)
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(setq te_w (send XM_TEXT_WIDGET_CLASS :new :managed :scrolled rc_w
		 :XMN_EDIT_MODE :MULTI_LINE_EDIT
		 :XMN_HEIGHT 400
		 :XMN_WIDTH  300
		 ))
(send te_w :override_translations
      "Ctrl<Key>A: beginning-of-line()\
       <Key>Return: newline-and-indent()")
