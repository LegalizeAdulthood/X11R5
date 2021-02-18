; -*-Lisp-*-
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; File:         SHELL.lsp
; RCS:          $Header: SHELL.lsp,v 1.2 91/10/05 04:01:39 mayer Exp $
; Description:  
; Author:       Niels Mayer, HPLabs
; Created:      Sun Feb 10 20:34:10 1991
; Modified:     Sat Oct  5 03:59:29 1991 (Niels Mayer) mayer@hplnpm
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

(if (and (eq *MOTIF_VERSION* 1) (>= *MOTIF_REVISION* 1))
    (send *TOPLEVEL_WIDGET* :get_argv)	;get_argv has problems in Motif 1.0 when no args supplied at WINTERP startup time.
  )

(send *TOPLEVEL_WIDGET* :set_argv #("foo" "bar" "baz" "jimmy" "hat" "in" "the" "mix"))

(send *TOPLEVEL_WIDGET* :get_argv)

(setq top_w
      (send TOP_LEVEL_SHELL_WIDGET_CLASS :new "top"
	    :XMN_GEOMETRY "=200x50+0+0"
	    ))
(send top_w :realize)
(send top_w :IS_MOTIF_WM_RUNNING)

(setq app_w 
      (send APPLICATION_SHELL_WIDGET_CLASS :new "app"
	    :XMN_GEOMETRY "=200x50+100+100"
	    ))
(send app_w :set_argv #("foo" "bar" "baz" "jimmy" "hat" "in" "the" "mix"))
(send app_w :realize)
(send app_w :IS_MOTIF_WM_RUNNING)
(send app_w :get_argv)

(if (and (eq *MOTIF_VERSION* 1) (>= *MOTIF_REVISION* 1))
    (progn
      (setq ov_w
	    (send OVERRIDE_SHELL_WIDGET_CLASS :new "override"
		  :XMN_GEOMETRY "=200x50+200+200"
		  ))
      (send ov_w :realize)
      (send ov_w :IS_MOTIF_WM_RUNNING)
      )
  )


(setq tx_w
      (send TRANSIENT_SHELL_WIDGET_CLASS :new "transient"
	    :XMN_GEOMETRY "=200x50+300+300"
	    ))
(send tx_w :realize)
(send tx_w :IS_MOTIF_WM_RUNNING)

(xt_add_timeout 10000
		'(
		  (send top_w :unrealize)
		  ))

(xt_add_timeout 12000
		'(
		  (send app_w :unrealize)
		  ))

(if (and (eq *MOTIF_VERSION* 1) (>= *MOTIF_REVISION* 1))
    (xt_add_timeout 14000
		    '(
		      (send ov_w :unrealize)
		      ))
  )

(xt_add_timeout 16000
		'(
		  (send tx_w :unrealize)
		  ))

(xt_add_timeout 18000 
		'(
		  (send xmdia_popup_w :unrealize)
		  ))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(progn
  (setq top_popup_w 
	(send TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS :new "top_popup" top_w
	      :XMN_GEOMETRY "=200x50+50+50"
	      ))
  (send top_popup_w :IS_MOTIF_WM_RUNNING)

  (setq app_popup_w
	(send APPLICATION_POPUP_SHELL_WIDGET_CLASS :new "app_popup" top_w
	      :XMN_GEOMETRY "=200x50+150+150"
	      ))
  (send app_popup_w :set_argv #("foo" "bar" "baz" "jimmy" "hat" "in" "the" "mix"))
  (send app_popup_w :get_argv)
  (send app_popup_w :IS_MOTIF_WM_RUNNING)

  (if (and (eq *MOTIF_VERSION* 1) (>= *MOTIF_REVISION* 1))
      (progn
	(setq ov_popup_w
	      (send OVERRIDE_POPUP_SHELL_WIDGET_CLASS :new "override_popup" top_w
		    :XMN_GEOMETRY "=200x50+250+250"
		    ))
	(send ov_popup_w :IS_MOTIF_WM_RUNNING)
	))

  (setq tx_popup_w
	(send TRANSIENT_POPUP_SHELL_WIDGET_CLASS :new "transient_popup" top_w
	      :XMN_GEOMETRY "=200x50+350+350"
	      ))
  (send tx_popup_w :IS_MOTIF_WM_RUNNING)

  (setq xmdia_popup_w
	(send XM_DIALOG_POPUP_SHELL_WIDGET_CLASS :new "dialog_popup" top_w
	      :XMN_GEOMETRY "=200x50+400+400"
	      ))
  (send xmdia_popup_w :IS_MOTIF_WM_RUNNING)

  ;; (setq xmnu_popup_w
  ;;(send XM_MENU_POPUP_SHELL_WIDGET_CLASS :new "menu_popup" top_w
  ;; 	    :XMN_GEOMETRY "=200x50+450+450"
  ;; 	    ))
  ;; 
  ;; (send xmnu_popup_w  :popup :grab_exclusive)
  ;; (send xmnu_popup_w :manage)


  (xt_add_timeout 1000 
		  '(
		    (send top_popup_w :popup :grab_none)
		    ))

  (xt_add_timeout 2000 
		  '(
		    (send app_popup_w :popup :grab_none)
		    ))

  (if (and (eq *MOTIF_VERSION* 1) (>= *MOTIF_REVISION* 1))
      (xt_add_timeout 3000 
		      '(
			(send ov_popup_w :popup :grab_none)
			))
    )

  (xt_add_timeout 4000 
		  '(
		    (send tx_popup_w :popup :grab_none)
		    ))

  (xt_add_timeout 5000 
		  '(
		    (send xmdia_popup_w :manage)
		    ))


  (xt_add_timeout 6000 
		  '(
		    (send top_popup_w :popdown)
		    ))

  (xt_add_timeout 7000 
		  '(
		    (send app_popup_w :popdown)
		    ))

  (if (and (eq *MOTIF_VERSION* 1) (>= *MOTIF_REVISION* 1))
      (xt_add_timeout 8000 
		      '(
			(send ov_popup_w :popdown)
			))
    )

  (xt_add_timeout 9000 
		  '(
		    (send tx_popup_w :popdown)
		    ))

  (xt_add_timeout 10000 
		  '(
		    (send xmdia_popup_w :unmanage)
		    ))

  )
