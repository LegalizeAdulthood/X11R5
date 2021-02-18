; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         helloworld.lsp
; RCS:          $Header: helloworld.lsp,v 1.2 91/10/05 17:32:58 mayer Exp $
; Description:  
; Author:       Niels Mayer, HPLabs
; Created:      Wed Jul 11 17:07:30 1990
; Modified:     Sat Oct  5 17:32:47 1991 (Niels Mayer) mayer@hplnpm
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

(let* (					;let* declares local variables
       (top_w				;top_w is local to "let*" form
	(send TOP_LEVEL_SHELL_WIDGET_CLASS :new "heloshl"
	      :XMN_TITLE     "Hello World"
	      :XMN_ICON_NAME "Hello World"
	      ))
       (but_w				;but_w is local to "let*" form
	(send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	      "helobut" top_w
	      :XMN_LABEL_STRING "hello world"
;;;           :XMN_FONT_LIST    "8x13"
	      ))
       )

  (send but_w :add_callback :XMN_ACTIVATE_CALLBACK '()
	'(
	  (format t "hello world\n")	;print a message to standard output
	  ))
  (send top_w :realize)
  )

