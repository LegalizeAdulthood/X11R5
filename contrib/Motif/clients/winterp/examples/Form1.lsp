; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         form-widg1.lsp
; RCS:          $Header: Form1.lsp,v 1.3 91/10/05 02:39:00 mayer Exp $
; Description:  Form widget layout -- same as formtest.c on p 88 of Doug Young's
;		Motif book.					
; Author:       Niels Mayer, HPLabs
; Created:      Fri Jul 27 00:22:55 1990
; Modified:     Sat Oct  5 02:38:43 1991 (Niels Mayer) mayer@hplnpm
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
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "form1shl"
	    :XMN_GEOMETRY	"200x200+1+1"
	    :XMN_TITLE		"Winterp:FORM TEST"
	    :XMN_ICON_NAME	"W:FORM TEST"
	    ))

(setq form_w
      (send XM_FORM_WIDGET_CLASS :new :managed
	    "form" toplevel_w
	    ))

(setq pb1_w
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	    "pb1" form_w
	    :XMN_TOP_ATTACHMENT		:attach_form
	    :XMN_LEFT_ATTACHMENT	:attach_form
	    :XMN_RIGHT_ATTACHMENT	:attach_form
	    ))

(setq pb2_w
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	    "pb2" form_w
	    :XMN_TOP_ATTACHMENT		:attach_widget
	    :XMN_TOP_WIDGET		pb1_w
	    :XMN_LEFT_ATTACHMENT	:attach_form
	    :XMN_RIGHT_ATTACHMENT	:attach_form	    
	    ))

(setq pb3_w
      (send XM_PUSH_BUTTON_WIDGET_CLASS :new :managed
	    "pb3" form_w
	    :XMN_TOP_ATTACHMENT		:attach_widget
	    :XMN_TOP_WIDGET		pb2_w
	    :XMN_LEFT_ATTACHMENT	:attach_form
	    :XMN_RIGHT_ATTACHMENT	:attach_form
	    :XMN_BOTTOM_ATTACHMENT	:attach_form
	    ))

(send toplevel_w :realize)
