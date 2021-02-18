; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         subcalc.lsp
; RCS:          $Header: subcalc.lsp,v 1.2 91/10/05 18:56:08 mayer Exp $
; Description:  Demo of spawning an interactive subprocess and interacting
;		with the subrpocess through XT_ADD_INPUT. Subprocess can be
;		off doing a long calculation while WINTERP GUI remains active.
;	        Subprocesses and XT_ADD_INPUT only available in WINTERP 1.14...
; Author:       Niels Mayer, HPLabs
; Created:      Sat Oct  5 18:51:56 1991
; Modified:     Sat Oct  5 18:55:57 1991 (Niels Mayer) mayer@hplnpm
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

(let (					;declare local variables
      subproc_pty input-cb command_editor_w quit_button_w list_w top_w rc_w
      )

  ;;; Widgets

  (setq top_w
	(send TOP_LEVEL_SHELL_WIDGET_CLASS :new "Calc" "calc"
	      ))
  (setq rc_w
	(send XM_ROW_COLUMN_WIDGET_CLASS :new :managed "rc" top_w
	      :XMN_ORIENTATION		:vertical
	      :XMN_PACKING		:pack_tight
	      :XMN_ENTRY_ALIGNMENT	:alignment_center
	      ))
  (setq quit_button_w
	(send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed rc_w
	      :XMN_LABEL_STRING		"Quit"
	      ))
  (setq command_editor_w 
	(send XM_TEXT_FIELD_WIDGET_CLASS :new :managed rc_w
	      ))
  (setq list_w
	(send XM_LIST_WIDGET_CLASS :new :managed :scrolled rc_w
	      :XMN_VISIBLE_ITEM_COUNT	20
	      ))

  (send top_w :realize)

  ;;; Callbacks

  (send quit_button_w :set_callback	;XtAppAddCallback()
	:XMN_ACTIVATE_CALLBACK		;invoke when button pushed...
	'()				;no local vars.
	'(				;callback code
	  (xt_remove_input input-cb)	;must remove this before closing
	  (close subproc_pty)		;close the file --> quits subprocess
	  (exp_wait)			;wait on the subprocess
	  ))

  (send command_editor_w :set_callback	;XtAppAddCallback()
	:XMN_ACTIVATE_CALLBACK		;invoke when <return> ... hit.
	'(callback_widget)		;bound to the current value of command_editor_w
	'(				;code to execute
	  (format subproc_pty "~A\n"	;send text in editor to the subprocess
		  (send callback_widget :get_string))
	  )
	)

  ;;; Subprocess

  (setq subproc_pty (exp_spawn "bc" "bc")) ;create subprocess, the bc(1) calculator

  (let ((line NIL))			;for every line output from bc(1), append result-
    (setq input-cb			;-to list widget...
	  (xt_add_input subproc_pty
			:read
			'(
			  (let ((c (read-char FDINPUTCB_FILE)))
			    (cond
			     ((char= c #\newline) 
			      (send list_w :add_item (format nil "~A" (reverse (cdr line))) 0)
			      (send list_w :set_bottom_pos 0)
			      (setq line nil)
			      )
			     (t
			      (setq line (cons c line))
			      ))
			    ))))
    )

  )
