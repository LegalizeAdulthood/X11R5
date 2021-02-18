; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         subprocess.lsp
; RCS:          $Header: subprocess.lsp,v 1.3 91/10/05 19:02:39 mayer Exp $
; Description:  Demo of spawning an interactive subprocess and interacting
;		with the subrpocess through XT_ADD_INPUT. Subprocess can be
;		off doing a long calculation while WINTERP GUI remains active.
;	        Subprocesses and XT_ADD_INPUT only available in WINTERP 1.14...
; Author:       Niels Mayer, HPLabs
; Created:      Sat Oct  5 18:58:46 1991
; Modified:     Sat Oct  5 19:02:32 1991 (Niels Mayer) mayer@hplnpm
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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



(setq f1 (exp_spawn "bc" "bc"))
(let ((line NIL))
  (setq input-cb1
	(xt_add_input f1
		      :read
		      '(
			(let ((c (read-char FDINPUTCB_FILE)))
			  (cond
			   ((char= c #\newline) 
			    (terpri)
			    (mapcar 'princ (reverse line))
			    (terpri)
			    (setq line nil)
			    )
			   (t
			    (setq line (cons c line))
			    ))
			  ))))
  )

; (load "rc-shell")
; (let ((line NIL))
;   (setq input-cb1
; 	(xt_add_input f1
; 		      :read
; 		      '(
; 			(let ((c (read-char FDINPUTCB_FILE)))
; 			  (cond
; 			   ((char= c #\newline) 
; 			    (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
; 				  rc_w
; 				  :xmn_label_string
; 				  (format nil "~A" (reverse line)))
; 			    (setq line nil)
; 			    )
; 			   (t
; 			    (setq line (cons c line))
; 			    ))
; 			  ))))
;   )


(format f1 "scale=10\n")
(format f1 "731 / 223\n")

;(xt_remove_input input-cb1)
;(xt_add_input input-cb1)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(setq f2 (exp_popen "bc"))
(let ((line NIL))
  (setq input-cb2
	(xt_add_input f2
		      :read
		      '(
			(let ((c (read-char FDINPUTCB_FILE)))
			  (cond
			   ((char= c #\newline) 
			    (terpri)
			    (mapcar 'princ (reverse line))
			    (terpri)
			    (setq line nil)
			    )
			   (t
			    (setq line (cons c line))
			    ))
			  ))))
  )

(format f2 "scale=2\n")
(format f2 "731 / 223\n")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;
;;; must call xt_remove_input before closing file, otherwise you get
;;; jillions of "error: X Toolkit Warning: Select failed; error code 9"
;;;

(input_active_p input-cb1)
(xt_remove_input input-cb1)
(input_active_p input-cb1)
(close f1)
(exp_wait)

;;;
;;; must call xt_remove_input before closing file, otherwise you get
;;; jillions of "error: X Toolkit Warning: Select failed; error code 9"
;;;
(xt_remove_input input-cb2)
(input_active_p input-cb2)
(close f2)
(exp_wait)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(setq bc_proc_io (exp_spawn "bc" "bc"))

(load "rc-shell")			;'rc_w' is now a XmRowColumn layout manager.

(let ((line NIL))			;'line' is part of callback's func closure.
  (setq input-cb1
	(xt_add_input	   ;add an input callback whenever data readable
	 '(		   ;FDINPUTCB_FILE is locally bound inside inputcb
	   (let ((c (read-char FDINPUTCB_FILE))) ;read a single char
	     (cond
	      ((char= c #\newline)	;if at end of line, make a label
	       (send XM_LABEL_WIDGET_CLASS :new :managed rc_w
		     :XMN_LABEL_STRING (format nil "~A" (reverse line)))
	       (setq line nil)		;and clear line
	       )
	      (t			;else prepend char to line
	       (setq line (cons c line)) 
	       )))
	   )
	 ))
  )

(format bc_proc_io "scale=10\n")

(format bc_proc_io "731 / 223\n")
