; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         hostlookup.lsp
; RCS:          $Header: hostlookup.lsp,v 1.2 91/10/05 17:34:57 mayer Exp $
; Description:  A lamo application that uses X_REFRESH_DISPLAY to 
;		popup and display contents of a "working dialog"
;		before a time-consuming subprocess begins to execute.
;		
;		Note that X_REFRESH_DISPLAY is only defined on HPUX WINTERPs.
;		You may be able to get this working for your machine as well...
;		see src-server/w_utils.c:Wut_Prim_X_REFRESH_DISPLAY() and
;		src-server/utils.c for details....
; Author:       Niels Mayer, HPLabs
; Created:      Fri Feb  8 19:59:47 1991
; Modified:     Sat Oct  5 17:33:50 1991 (Niels Mayer) mayer@hplnpm
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


(let (top-w edit-w dialog-w)

  (setq top-w
	(send TOP_LEVEL_SHELL_WIDGET_CLASS :new "top-w"
	      :XMN_TITLE "Hostname lookup"
	      :XMN_ICON_NAME "Hostname lookup"
	      ))
  (setq edit-w
	(send XM_TEXT_WIDGET_CLASS :new :managed
	      "edit-w" top-w
	      :XMN_EDIT_MODE       :SINGLE_LINE_EDIT
	      :XMN_COLUMNS         80
	      ))

  (send edit-w :set_callback :XMN_ACTIVATE_CALLBACK '()
	'(
	  (let ((hostname (send edit-w :get_string)))
	    (send dialog-w :set_values 
		  :XMN_MESSAGE_STRING (strcat "Looking up hostname " hostname)
		  )
	    (send dialog-w :manage)

	    (x_refresh_display 300)

	    ;; If x_refresh_display isn't defined, you can substite the following ultra-kludge
	    ;; (send dialog-w :update_display)
	    ;; (system "sleep 1")
	    ;; (send dialog-w :update_display)

	    (let ((pipe (popen (strcat "grep -i '" hostname "' /etc/hosts") "r")))
	      (send edit-w :set_string (read-line pipe))
	      (pclose pipe)
	      )
	    )
	  (send dialog-w :unmanage)
	  ))
   
  (send top-w :realize)

  (setq dialog-w
	(send XM_MESSAGE_BOX_WIDGET_CLASS :new :unmanaged :working_dialog
	      "working_dialog" top-w
	      ))

  )
