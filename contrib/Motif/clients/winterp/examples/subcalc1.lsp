; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         subcalc1.lsp
; RCS:          $Header: subcalc1.lsp,v 1.2 91/10/05 18:57:41 mayer Exp $
; Description:  Demo of spawning an interactive subprocess and interacting
;		with the subrpocess through XT_ADD_INPUT. Subprocess can be
;		off doing a long calculation while WINTERP GUI remains active.
;	        Subprocesses and XT_ADD_INPUT only available in WINTERP 1.14...
; Author:       Niels Mayer, HPLabs
; Created:      Sat Oct  5 18:56:33 1991
; Modified:     Sat Oct  5 18:57:34 1991 (Niels Mayer) mayer@hplnpm
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
      subproc-pty input-cb command-editor-w quit-button-w list-w top-w rc-w
      )

  ;;; Widgets

  (setq top-w
	(send TOP_LEVEL_SHELL_WIDGET_CLASS :new "Subcalc" "subcalc"
	      ))
  (setq rc-w
	(send XM_ROW_COLUMN_WIDGET_CLASS :new :managed top-w
	      :XMN_ORIENTATION		:vertical
	      :XMN_PACKING		:pack_tight
	      :XMN_ENTRY_ALIGNMENT	:alignment_center
	      ))
  (setq quit-button-w
	(send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed rc-w
	      :XMN_LABEL_STRING		"Quit"
	      ))
  (setq command-editor-w 
	(send XM_TEXT_FIELD_WIDGET_CLASS :new :managed rc-w
	      ))
  (setq list-w
	(send XM_LIST_WIDGET_CLASS :new :managed :scrolled rc-w
	      :XMN_VISIBLE_ITEM_COUNT	20
	      ))

  (send top-w :realize)

  ;;; Callbacks

  (send quit-button-w :set_callback	;XtAppAddCallback()
	:XMN_ACTIVATE_CALLBACK		;invoke when button pushed...
	'()				;no local vars.
	'(				;callback code...
	  (xt_remove_input input-cb)	;must remove this before closing
	  (close subproc-pty)		;close the file --> quits subprocess
	  (exp_wait)			;wait(2) on the subprocess
	  ))

  (send command-editor-w :set_callback	;XtAppAddCallback()
	:XMN_ACTIVATE_CALLBACK		;invoke when <return> ... hit.
	'(callback_widget)		;bound to the current value of command-editor-w
	'(				;callback code...
	  (format subproc-pty "~A\n"	;send text in editor to the subprocess
		  (send callback_widget :get_string)) ;XmTextFieldGetString()
	  ))

  ;;; Subprocess

  (setq subproc-pty (exp_spawn "bc" "bc")) ;create subprocess, the bc(1) calculator

  (setq input-cb			
	(xt_add_input			;XtAppAddInput()
	 subproc-pty			
	 :read_line			;for each line output, bind to FDINPUTCB_READ_LINE 
	 '(				;input callback code...
	   (send list-w :add_item FDINPUTCB_READ_LINE 0) ;XmListAddItem()
	   (send list-w :set_bottom_pos 0) ;XmListSetBottomPos()
	   )
	 ))
  )



